#include "simple_pool.hpp"

const task& pools::simple_pool::get() const
{
    switch (strategy) {
    // depth-first search
    case pool_strategy::DFS :
        return *(--container.cend());
        break;
    // width-first search and best-first search
    case pool_strategy::WFS :
    case pool_strategy::BFS :
        return *container.cbegin();
        break;
    // this case can't occur
    default:
        BNB_ERROR_REPORT("No such strategy");
    }
}

void pools::simple_pool::remove()
{
    switch (strategy) {
    // depth-first search
    case pool_strategy::DFS :
        container.erase(--container.cend());
        break;
    // width-first search and best-first search
    case pool_strategy::WFS :
    case pool_strategy::BFS :
        container.erase(container.cbegin());
        break;
    // this case can't occur
    default:
        BNB_ERROR_REPORT("No such strategy");
    }
}
