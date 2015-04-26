#ifndef _pseudo_process_hpp_
#define _pseudo_process_hpp

#include <communicators/communicator.hpp>
#include <simulators/resolver.hpp>

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

        BNBScheduler& mSched; // bnb-solver class

        resolver& mSolver;

        Tracer& mTracer; // bnb-solver class

        std::string mStatFileName = "stats";

		// process id
        int self;
        // process pool size
		int size;

        enum class process_state
		{
            active,
            sleep,
            awake
        } state = process_state::active;

        // response for the last receive request
        receive_response response{};

        serializer binser{};

        BNBScheduler::Action action{};
        BNBScheduler::Event event{};

        SolverInfo info{};

        DmCounters cnt{};

		// master process id
		static const int master = 0;

        void trace(BNBScheduler::Action& action, BNBScheduler::Event& event, const SolverInfo& info);

        void writeStats(const std::string& s) const;

		// Actions:

		void solve();

		void send_command();
		void send_records();
		void send_sub();
		void send_sub_and_records();

		void receive();
		void complete_receive();

		void receive_command();
		void receive_records();
		void receive_sub();
		void receive_sub_and_records();

		void dump_stats();

        bool work();

    public:
        // constructor
        process(communicator& comm, BNBScheduler& sched, resolver& solver, Tracer& tracer);

        void setStatsFileName(const std::string& fname) { mStatFileName = fname; }

        void awaken(const receive_response& r);

        long long activate();
        // returns true to quit
		bool proceed();
    };
}

#endif // _pseudo_process_hpp_
