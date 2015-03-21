#include "abstract_task_pool.hpp"

void pools::abstract_task_pool::set_sending_strategy()
{
    switch (strategy) {
    // depth-first search
    case pool_strategy::DFS :
        strategy = pool_strategy::WFS;
        break;
    // width-first search and best-first search
    case pool_strategy::WFS :
    case pool_strategy::BFS :
        strategy = pool_strategy::DFS;
        break;
    // this case can't occur
    default:
        BNB_ERROR_REPORT("No such strategy");
    }
}
