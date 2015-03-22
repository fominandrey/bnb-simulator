#include "timer.hpp"

void simulator::timer::apply_settings(const JSONNode& node)
{
    time_cost::apply_settings(node["time cost"]);
}

int simulator::timer::time_cost::solve;
int simulator::timer::time_cost::send;
int simulator::timer::time_cost::receive;

void simulator::timer::time_cost::apply_settings(const JSONNode& node)
{
    solve = node["solve"].as_int();
    send = node["send"].as_int();
    receive = node["receive"].as_int();
}
