#include "communicator.hpp"

int simulator::communicator::latency;
long simulator::communicator::bandwidth;

void simulator::communicator::apply_settings(const JSONNode& node)
{
    latency = node["latency"].as_int();
    bandwidth = node["bandwidth"].as_int();
}

// constructor
simulator::communicator::communicator(int size) :
    n(size),
    send_bytes(size, 0),
    receive_bytes(size, 0),
    notify()
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

void simulator::communicator::send(int sender, const serializer& message, long long timestamp, int receiver)
{
    auto p = receive_box.find(receiver);

    bool message_ready = false;

    if (p != receive_box.end()) {
        const receive_request& request = p->second;
        message_ready = (request.sender == any || request.sender == sender);
    }

    long delay = latency + message.actual_size()/bandwidth;

    if (message_ready) {
        receive_request& request = p->second;

        deliver(request.storage, message);

        send_bytes[sender] += message.actual_size();
        receive_bytes[receiver] += message.actual_size();

        notify[receiver](receive_response{ sender, timestamp + delay });

        receive_box.erase(p);

    } else {
        send_box[receiver].insert(
            std::multimap<int, send_request>::value_type{ sender, send_request{ message, timestamp + delay } }
        );
    }
}

simulator::receive_response simulator::communicator::recv(int receiver, Buffer& storage, int sender)
{
    auto q = send_box.find(receiver);

    std::multimap<int, send_request>::const_iterator p;

    bool message_ready = false;

    if (q != send_box.end()) {
        const auto& message_box = q->second;  // messages for 'receiver' process

        if (sender == any) {
            p = message_box.begin();
            message_ready = true;
        } else {
            p = message_box.find(sender);
            message_ready = (p != message_box.end());
        }
    }

    if (message_ready) {
        auto& message_box = q->second;

        sender = p->first;
        const send_request& request = p->second;

        deliver(storage, request.content);

        send_bytes[sender] += request.content.actual_size();
        receive_bytes[receiver] += request.content.actual_size();

        message_box.erase(p);
        if (message_box.empty())
            send_box.erase(q);

        return receive_response{ sender, request.timestamp };
    } else {
        receive_box.insert(
            std::map<int, receive_request>::value_type{ receiver, receive_request{ storage, sender } }
        );
		return receive_response{ any, timer::sleep_time };
    }
}
