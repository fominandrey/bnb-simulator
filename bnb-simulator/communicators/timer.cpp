#include "timer.hpp"

//
void simulator::timer::apply_settings(const JSONNode& node)
{
    time_cost::apply_settings(node["time cost"]);
}

int simulator::timer::time_cost::solve;

//
void simulator::timer::time_cost::apply_settings(const JSONNode& node)
{
    solve = node["solve"].as_int();
}
