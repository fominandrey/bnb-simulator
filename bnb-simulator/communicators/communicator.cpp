#include "communicator.hpp"

// constructor
simulator::communicator::communicator(int size) :
    n(size), send_bytes(size, 0), receive_bytes(size, 0), notify()
{
    notify.reserve(size);
}

// obtains the notifier for the pseudo-process and returns its id
int simulator::communicator::link(const signal& ding)
{
    notify.push_back(ding);
    return notify.size() - 1;
}

void simulator::communicator::deliver(Buffer& storage, const Buffer& message) const
{
	int n = message.size();

	if (storage.resize(n))
        BNB_ERROR_REPORT("Too large buffer requested");

    memcpy(storage.data(), message.data(), n);
}

void simulator::communicator::send(int sender, const BinarySerializer& message, long long timestamp, int receiver)
{
    auto p = receive_box.find(receiver);

    if (p != receive_box.end() && ((p->second).sender == any || (p->second).sender == sender)) {

        deliver((p->second).storage, message);
        send_bytes[sender] += message.size();
        receive_bytes[receiver] += message.size();

        notify[receiver](receive_response{ sender, timestamp });

        receive_box.erase(p);

    } else {
        send_box[receiver].insert(
            std::multimap<int, send_request>::value_type{ sender, send_request{ message, timestamp } }
        );
    }
}

simulator::receive_response simulator::communicator::recv(int receiver, Buffer& storage, int sender)
{
    std::multimap<int, send_request>::iterator p;

    auto q = send_box.find(receiver);

    bool message_ready = false;

    if (q != send_box.end()) {
        if (sender == any) {
            p = (q->second).begin();
            message_ready = true;
        } else {
            p = (q->second).find(sender);
            message_ready = (p != (q->second).end());
        }
    }

    if (message_ready) {
        sender = p->first;
        long long time = (p->second).timestamp;

        deliver(storage, (p->second).content);
        send_bytes[sender] += (p->second).content.size();
        receive_bytes[receiver] += (p->second).content.size();

        (q->second).erase(p);
        if ((q->second).empty())
            send_box.erase(q);

        return receive_response{ sender, time };
    } else {
        receive_box.insert(
            std::map<int, receive_request>::value_type{ receiver, receive_request{ storage, sender } }
        );
		return receive_response{ any, timer::sleep_time };
    }
}
