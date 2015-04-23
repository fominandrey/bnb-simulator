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
simulator::process::process(communicator& comm, BNBScheduler& sched, resolver& solver, Tracer& tracer) :
    mComm(comm),
    mSched(sched),
    mSolver(solver),
    mTracer(tracer),
    self(mComm.link(std::bind(&process::awaken, this, std::placeholders::_1))),
	size(mComm.size())
{
	mSched.setRank(self);
    mSched.setSize(size);
    // add initial problem to the pool
    if (self == master)
        mSolver.load_initial_task();
	event.mCode = BNBScheduler::Events::START;
}

void simulator::process::awaken(const receive_response& r)
{
    response = r;
    time.set(response.time);
    state = process_state::awake;
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

void simulator::process::send_command()
{
    int code = BNBDmSolver::MessageTypes::COMMAND;
    int dest = action.mArgs[0];

    binser << code;

    for (int i = 1; i < MAX_ARGS; ++i)
        binser << action.mArgs[i];

    binser.assign_size(serializer::parcel_size::command_size());

    time.spend(timer::time_cost::overhead_cost());

    mComm.send(self, binser, time.get(), dest);
    binser.reset();

    event.mCode = BNBScheduler::Events::SENT;

    cnt.sent_commands++;
    cnt.sent_parcels++;
}

void simulator::process::send_records()
{
    int code = BNBDmSolver::MessageTypes::RECORDS;
    int dest = action.mArgs[0];

    binser << code;

    mSolver.getRecords(binser);

    binser.assign_size(serializer::parcel_size::record_size());

    time.spend(timer::time_cost::overhead_cost());

    mComm.send(self, binser, time.get(), dest);
    binser.reset();

	event.mCode = BNBScheduler::Events::SENT;

	cnt.sent_parcels++;
	cnt.sent_records++;
}

void simulator::process::send_sub()
{
    int code = BNBDmSolver::MessageTypes::SUBPROBLEMS;
	int dest = action.mArgs[0];
    int n = action.mArgs[1];

	if (info.mNSub > 0) {
        binser << code;

        mSolver.getSubs(n, binser);

        binser.assign_size(serializer::parcel_size::sub_size() * n);

        time.spend(timer::time_cost::store_cost() * n +
            timer::time_cost::overhead_cost());

        mComm.send(self, binser, time.get(), dest);
        binser.reset();

        event.mCode = BNBScheduler::Events::SENT;

    	cnt.sent_parcels++;
		cnt.sent_subs += n;
    } else {
		event.mCode = BNBScheduler::Events::NOTHING_TO_SEND;
    }
}

void simulator::process::send_sub_and_records()
{
    int code = BNBDmSolver::MessageTypes::SUB_AND_RECORDS;
	int dest = action.mArgs[0];
    int n = action.mArgs[1];

	if (info.mNSub > 0) {
        binser << code;

        mSolver.getRecords(binser);
        mSolver.getSubs(n, binser);

        binser.assign_size(serializer::parcel_size::sub_size() * n +
            serializer::parcel_size::record_size());

        time.spend(timer::time_cost::store_cost() * n +
            timer::time_cost::overhead_cost());

        mComm.send(self, binser, time.get(), dest);
        binser.reset();

        event.mCode = BNBScheduler::Events::SENT;

        cnt.sent_parcels++;
        cnt.sent_records++;
        cnt.sent_subs += n;
    } else {
        event.mCode = BNBScheduler::Events::NOTHING_TO_SEND;
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

void simulator::process::receive_command()
{
    event.mCode = BNBScheduler::Events::COMMAND_ARRIVED;

    event.mArgs[0] = response.sender;

    for (int i = 1; i < MAX_ARGS; ++i)
        binser >> event.mArgs[i];

    cnt.recv_commands++;
}

void simulator::process::receive_records()
{
    event.mCode = BNBScheduler::Events::DATA_ARRIVED;

    event.mArgs[0] = response.sender;

    mSolver.putRecords(binser);

    cnt.recv_records++;
}

void simulator::process::receive_sub()
{
    event.mCode = BNBScheduler::Events::DATA_ARRIVED;

    event.mArgs[0] = response.sender;

    int n = mSolver.putSubs(binser);

    time.spend(timer::time_cost::load_cost() * n);

    cnt.recv_subs += n;
}

void simulator::process::receive_sub_and_records()
{
    event.mCode = BNBScheduler::Events::DATA_ARRIVED;

    event.mArgs[0] = response.sender;

    mSolver.putRecords(binser);

    int n = mSolver.putSubs(binser);

    time.spend(timer::time_cost::load_cost() * n);

    cnt.recv_records++;
    cnt.recv_subs += n;
}

void simulator::process::complete_receive()
{
    time.spend(timer::time_cost::overhead_cost());

	int code;

    binser >> code;

    cnt.recv_parcels++;

	switch (code) {
	case BNBDmSolver::MessageTypes::COMMAND :
		receive_command();
		break;
    case BNBDmSolver::MessageTypes::RECORDS :
		receive_records();
		break;
    case BNBDmSolver::MessageTypes::SUBPROBLEMS :
		receive_sub();
		break;
    case BNBDmSolver::MessageTypes::SUB_AND_RECORDS :
		receive_sub_and_records();
		break;
	// this case can't occur
	default:
        std::cout << "message type [" << code << "]\n";
		BNB_ERROR_REPORT("Undefined message type");
    }

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

		    if (self == master) {
                os << "time = " << time.get() << '\n';
                os << master << ": " << cnt.toString() << '\n';
            } else {
                binser << cnt.toString();
                mComm.send(self, binser, time.get(), master);
                return false;
            }
		    // master starts to gather data that other processes have sent
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
