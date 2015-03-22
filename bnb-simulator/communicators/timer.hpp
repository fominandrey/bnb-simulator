#ifndef _timer_hpp_
#define _timer_hpp_

#define NDEBUG

#include <libjson/libjson.h>

namespace simulator
{
    class timer
	{
        long long time = 0;

    public:

        class time_cost
		{
            // for one step
            static int solve;
            // for sending
            static int send;
            // for receiving
            static int receive;

        public:

            static void apply_settings(const JSONNode& node);

            static int solve_cost() { return solve; }
            static int send_cost() { return send; }
            static int receive_cost() { return receive; }
        };

        static void apply_settings(const JSONNode& node);

    	static const long long sleep_time = -1;

    	long long get() const { return time; }
    	void spend(long long t) { time += t; }
    	void set(long long t) { if (time < t) time = t; }
    };
}

#endif // _timer_hpp_
