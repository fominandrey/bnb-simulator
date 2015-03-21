#include "task_hash_map.hpp"

void pools::task_hash_map::save_box(int i)
{
    saved_cursors[i] = position;
}

void pools::task_hash_map::load_box(int i)
{
    position = saved_cursors[i];
    box = (position != last) ? &(position.in_map->second) : 0;
}

void pools::task_hash_map::open_box(int level)
{
    position.in_map = level_box_map.insert( { level, simulator::task_box{level} } ).first;

    auto hint = std::lower_bound(level_list.begin(), level_list.end(), level);

    position.in_list = (hint == level_list.end() || *hint > level) ? level_list.insert(hint, level) : hint;

    box = &(position.in_map->second);
}

// ! Preconditions: box is opened !
void pools::task_hash_map::open_next_box()
{
    int level = position.in_map->first + 1;

    position.in_map = level_box_map.insert( { level, simulator::task_box{level} } ).first;

    auto hint = std::next(position.in_list);

    if (hint == level_list.end() || *hint > level)
        position.in_list = level_list.insert(hint, level);

    box = &(position.in_map->second);
}

// ! Preconditions: pool is not empty !
void pools::task_hash_map::open_box()
{
    switch (strategy) {
    // width-first search and best-first search
    case pool_strategy::WFS :
    case pool_strategy::BFS :
        position.in_list = level_list.begin();
        break;
    // depth-first search
    case pool_strategy::DFS :
        position.in_list = std::prev(level_list.end());
        break;
    // this case can't occur
    default:
        BNB_ERROR_REPORT("No such strategy");
    }

    int level = *position.in_list;
    position.in_map = level_box_map.find(level);

    box = &(position.in_map->second);
}

// ! Postconditions: no opened boxes !
void pools::task_hash_map::remove_box()
{
    level_box_map.erase(position.in_map);
    level_list.erase(position.in_list);
    position = last;
    box = 0;
}
