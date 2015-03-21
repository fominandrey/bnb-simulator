#ifndef _pseudo_process_hpp_
#define _pseudo_process_hpp

#include <communicators/communicator.hpp>

#include <kernel/dmmemory/bnbdmsolver.hpp>

#include <iostream>
#include <fstream>

namespace simulator
{
    // pseudo-process
    class process
	{
        timer time{};

        communicator& mComm;

        BNBScheduler& mSched;
        BNBResolver& mSolver;
        Tracer& mTracer;

        std::string mStatFileName = "statistics";

		// pseudo-process id
        int self;
        // pseudo-process pool size
		int size;

        enum class process_state
		{
            active,
            sleep,
            awake
        } state = process_state::active;

        // response for the last receive request
        receive_response response{};

        BinarySerializer binser{};

        DmCounters cnt{};

        SolverInfo info{};

        BNBScheduler::Action action{};
        BNBScheduler::Event event{};

        bool quit = false;

		// process that gathers data prior to exit
		static const int accumulator = 0;

		int source = 0;

        // to be initialized only once, these objects are stored in the class instance
        std::string s{};
        std::ostringstream os{};

        void trace(BNBScheduler::Action& action, BNBScheduler::Event& event, const SolverInfo& info, bool last = false);

        void writeStats(const std::string& s) const;

		// Actions:

		void send_command();
		void send_sub();
		void send_records();
		void send_sub_and_records();

		void receive();
		void complete_receive();

		void solve();

        void work();

		bool finish();
		void complete_gather();

    public:
        // constructor
        process(communicator& comm, BNBScheduler& sched, BNBResolver& solver, Tracer& tracer);

        void setStatsFileName(const std::string& fname) { mStatFileName = fname; }

        void awaken(const receive_response& r);

		bool proceed();
    };
}

#endif // _pseudo_process_hpp_
