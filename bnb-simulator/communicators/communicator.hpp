#ifndef _communicator_hpp_
#define _communicator_hpp_

#include <util/common/bnberrcheck.hpp>

#include "timer.hpp"
#include "request.hpp"

#include <map>
#include <queue>
#include <vector>
#include <algorithm>
#include <functional>

namespace simulator
{
	class communicator
	{
		// process pool size
		int n;

        static int latency;
        static long bandwidth;

        std::vector<long long> send_bytes;
        std::vector<long long> receive_bytes;

        typedef std::function<void (const receive_response&)> signal;

        std::vector<signal> notify;

        std::map<int, std::map<int, std::priority_queue<send_request>>> send_box{};
        std::map<int, receive_request> receive_box{};

        typedef std::map<int, std::priority_queue<send_request>>::value_type requests_by_sender;

        static bool prioritize_senders(const requests_by_sender& a, const requests_by_sender& b)
        {
            return (a.second.top() < b.second.top());
        }

        void deliver(Buffer& storage, const Buffer& content) const;

    public:

        static void apply_settings(const JSONNode& node);

        // constructor
        explicit communicator(int size);

        // obtains the notifier for the process and returns its id
        int link(const signal& ding);

        int size() const { return n; }

        long long getSentBytes(int i) const { return send_bytes[i]; }
        long long getRecvBytes(int i) const { return receive_bytes[i]; }

        static const int any = -1;

        void send(int sender, const serializer& message, long long timestamp, int receiver);

        receive_response recv(int receiver, Buffer& storage, int sender = any);
	};
}

#endif // _communicator_hpp_
