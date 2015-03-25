#include "timer.hpp"

void simulator::timer::apply_settings(const JSONNode& node)
{
    time_cost::apply_settings(node["time cost"]);
}

int simulator::timer::time_cost::solve;
int simulator::timer::time_cost::overhead;

void simulator::timer::time_cost::apply_settings(const JSONNode& node)
{
    solve = node["solve"].as_int();
    overhead = node["overhead"].as_int();
}
