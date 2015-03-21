#ifndef _task_map_hpp_
#define _task_map_hpp_

#include "abstract_task_map.hpp"
#include "task_box.hpp"

#include <map>

#include <iterator>

namespace pools
{
    class task_map : public abstract_task_map
	{
        // cursor is a sort of a pointer to the opened box
        typedef std::map<int, simulator::task_box>::iterator cursor;

        std::map<int, simulator::task_box> level_box_map;

        const cursor last = level_box_map.end();
        // case: no opened boxes
        cursor position = last;

        // stores saved cursors
        cursor saved_cursors[2];

    public:

        void open_box(int level) override;
        // ! Preconditions: box is opened !
        void open_next_box() override;
        // ! Preconditions: pool is not empty !
        void open_box() override;

        // ! Postconditions: no opened boxes !
        void remove_box() override;

        // ! Preconditions: i is in { 0, 1 } !
        void save_box(int i) override;
        void load_box(int i) override;
    };
}

#endif // _task_map_hpp_
