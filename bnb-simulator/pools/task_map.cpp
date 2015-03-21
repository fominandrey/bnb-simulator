#include "task_map.hpp"

void pools::task_map::save_box(int i)
{
    saved_cursors[i] = position;
}

void pools::task_map::load_box(int i)
{
    position = saved_cursors[i];
    box = (position != last) ? &(position->second) : 0;
}

void pools::task_map::open_box(int level)
{
    auto hint = (position != last) ? std::next(position) : last;

    position = level_box_map.insert(hint, { level, simulator::task_box{level} });

    box = &(position->second);
}

// ! Preconditions: box is opened !
void pools::task_map::open_next_box()
{
    int level = position->first + 1;

    position = level_box_map.insert(std::next(position), { level, simulator::task_box{level} } );

    box = &(position->second);
}

// ! Preconditions: pool is not empty !
void pools::task_map::open_box()
{
    switch (strategy) {
    // width-first search and best-first search
    case pool_strategy::WFS :
    case pool_strategy::BFS :
        position = level_box_map.begin();
        break;
    // depth-first search
    case pool_strategy::DFS :
        position = std::prev(level_box_map.end());
        break;
    // this case can't occur
    default:
        BNB_ERROR_REPORT("No such strategy");
    }

    box = &(position->second);
}

// ! Postconditions: no opened boxes !
void pools::task_map::remove_box()
{
    level_box_map.erase(position);
    position = last;
    box = 0;
}
