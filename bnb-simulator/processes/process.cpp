#include "process.hpp"

void simulator::process::trace(BNBScheduler::Action& action, BNBScheduler::Event& event, const SolverInfo& info, bool last)
{
    mTracer.traceAction(time.get(), self, action, event, info);
    if (last)
        mTracer.flush(self);
}

void simulator::process::writeStats(const std::string& s) const
{
    if (mStatFileName.empty())
        std::cout << s << std::endl;
    else
        std::ofstream(mStatFileName) << s << std::endl;
}

// constructor
simulator::process::process(communicator& comm, BNBScheduler& sched, BNBResolver& solver, Tracer& tracer) :
    mComm(comm), mSched(sched), mSolver(solver), mTracer(tracer),
    self(mComm.link(std::bind(&process::awaken, this, std::placeholders::_1))),
	size(mComm.size())
{
	mSched.setRank(self);
    mSched.setSize(size);
	event.mCode = BNBScheduler::Events::START;
}

void simulator::process::awaken(const receive_response& r)
{
    response = r;
    time.set(response.time);
    state = process_state::awake;
}

void simulator::process::send_command()
{
    int code = BNBDmSolver::MessageTypes::COMMAND;
    int dest = action.mArgs[0];
    int command = action.mArgs[1];

    binser << code;
    binser << command;

    time.spend(timer::time_cost::send_cost());

    mComm.send(self, binser, time.get(), dest);
    binser.reset();

    event.mCode = BNBScheduler::Events::SENT;

    cnt.sent_commands++;
    cnt.sent_parcels++;
}

void simulator::process::send_sub()
{
    int code = BNBDmSolver::MessageTypes::SUBPROBLEMS;
	int dest = action.mArgs[0];
    int num = action.mArgs[1];

	if (info.mNSub > 0) {
        binser << code;
        mSolver.getSubs(num, binser);

        time.spend(timer::time_cost::send_cost());

        mComm.send(self, binser, time.get(), dest);
        binser.reset();

        event.mCode = BNBScheduler::Events::SENT;

    	cnt.sent_parcels++;
		cnt.sent_subs += num;
    } else {
		event.mCode = BNBScheduler::Events::NOTHING_TO_SEND;
    }
}

void simulator::process::send_records()
{
    int code = BNBDmSolver::MessageTypes::RECORDS;
    int dest = action.mArgs[0];

    binser << code;
    mSolver.getRecords(binser);

    time.spend(timer::time_cost::send_cost());

    mComm.send(self, binser, time.get(), dest);
    binser.reset();

	event.mCode = BNBScheduler::Events::SENT;

	cnt.sent_parcels++;
	cnt.sent_records++;
}

void simulator::process::send_sub_and_records()
{
    int code = BNBDmSolver::MessageTypes::SUB_AND_RECORDS;
	int dest = action.mArgs[0];
    int num = action.mArgs[1];

	if (info.mNSub > 0) {
        binser << code;
        mSolver.getRecords(binser);
        mSolver.getSubs(num, binser);

        time.spend(timer::time_cost::send_cost());

        mComm.send(self, binser, time.get(), dest);
        binser.reset();

        event.mCode = BNBScheduler::Events::SENT;

        cnt.sent_parcels++;
        cnt.sent_records++;
        cnt.sent_subs += num;
    } else {
        event.mCode = BNBScheduler::Events::NOTHING_TO_SEND;
    }
}

void simulator::process::solve()
{
	long long n = action.mArgs[0];

    mSolver.solve(n);

	if (n < 0) {
		event.mCode = BNBScheduler::Events::ERROR;
    } else {
        time.spend(timer::time_cost::solve_cost() * n);

        event.mCode = BNBScheduler::Events::DONE;
        event.mArgs[0] = n;

        cnt.steps_done += n;
    }
}

void simulator::process::receive()
{
	response = mComm.recv(self, binser);
	long long t = response.time;

	if (t == timer::sleep_time) {
        state = process_state::sleep;
    } else {
		time.set(t);
		complete_receive();
    }
}

void simulator::process::complete_receive()
{
    int r = response.sender;
	int code, command;

    binser >> code;

    cnt.recv_parcels++;

	switch (code) {
	case BNBDmSolver::MessageTypes::COMMAND :
		event.mCode = BNBScheduler::Events::COMMAND_ARRIVED;
        binser >> command;
        event.mArgs[0] = r;
        event.mArgs[1] = command;
        cnt.recv_commands++;
		break;
    case BNBDmSolver::MessageTypes::RECORDS :
        event.mCode = BNBScheduler::Events::DATA_ARRIVED;
        event.mArgs[0] = r;
        mSolver.putRecords(binser);
        cnt.recv_records++;
		break;
    case BNBDmSolver::MessageTypes::SUBPROBLEMS :
        event.mCode = BNBScheduler::Events::DATA_ARRIVED;
        event.mArgs[0] = r;
        cnt.recv_subs += mSolver.putSubs(binser);
		break;
    case BNBDmSolver::MessageTypes::SUB_AND_RECORDS :
        event.mCode = BNBScheduler::Events::DATA_ARRIVED;
        event.mArgs[0] = r;
        mSolver.putRecords(binser);
        cnt.recv_subs += mSolver.putSubs(binser);
		break;
	// this case can't occur
	default:
        std::cout << "message type [" << code << "]\n";
		BNB_ERROR_REPORT("Undefined message type");
    }

    time.spend(timer::time_cost::receive_cost());

    binser.reset();
}

void simulator::process::work()
{
	if (state == process_state::active) {

    	mSolver.getInfo(info);
    	mSched.action(event, info, action);
    	trace(action, event, info);

        switch (action.mCode) {
        case BNBScheduler::Actions::DUMMY_ACTION :
            break;
        case BNBScheduler::Actions::SET_SEARCH_STRATEGY :
            mSolver.setSearchStrategy(action.mArgs[0]);
            event.mCode = BNBScheduler::Events::SEARCH_STRATEGY_SET;
            break;
        case BNBScheduler::Actions::SEND_COMMAND :
			send_command();
			break;
        case BNBScheduler::Actions::SEND_SUB :
			send_sub();
			break;
        case BNBScheduler::Actions::SEND_RECORDS :
			send_records();
            break;
        case BNBScheduler::Actions::SEND_SUB_AND_RECORDS :
			send_sub_and_records();
            break;
        case BNBScheduler::Actions::RECV :
			receive();
            break;
        case BNBScheduler::Actions::SOLVE :
			solve();
            break;
        case BNBScheduler::Actions::EXIT :
            quit = true;
            break;
        // this case can't occur
        default:
            std::cout << "action [" << action.mCode << "]\n";
            BNB_ERROR_REPORT("Undefined action");
        }
    } else {
		state = process_state::active;
		complete_receive();
	}
}

bool simulator::process::finish()
{
	if (state == process_state::active) {

		if (source == 0) {
			mSolver.getInfo(info);
		    trace(action, event, info, true);

		    cnt.recv_bytes = mComm.getRecvBytes(self);
		    cnt.sent_bytes = mComm.getSentBytes(self);

		    if (self == accumulator) {
                os << "time = " << time.get() << '\n';
                os << accumulator << ": " << cnt.toString() << '\n';
            } else {
                binser << cnt.toString();
                mComm.send(self, binser, time.get(), accumulator);
                return false;
            }
		    // accumulator starts to gather data that other pseudo-processes have sent
			++source;
		}

        while (source < size) {
            response = mComm.recv(self, binser, source);
            long long t = response.time;

            if (t == timer::sleep_time) {
                state = process_state::sleep;
                return true;
            } else {
                time.set(t);
                complete_gather();
            }
        }

        writeStats(os.str());
        return false;

	} else {
		state = process_state::active;
		complete_gather();
		return true;
	}
}

void simulator::process::complete_gather()
{
    binser >> s;
    os << source << ": " << s << '\n';
    binser.reset();
    s.clear();
	++source;
}

bool simulator::process::proceed()
{
    if (state != process_state::sleep) {
		if (!quit) {
			work();
        } else {
            return finish();
        }
    }
    return true;
}
