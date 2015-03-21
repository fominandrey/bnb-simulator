#ifndef _task_hash_map_hpp_
#define _task_hash_map_hpp_

#include "abstract_task_map.hpp"
#include "task_box.hpp"

#include <list>
#include <unordered_map>

#include <iterator>
#include <algorithm>

namespace pools
{
    class task_hash_map : public abstract_task_map
	{
		std::unordered_map<int, simulator::task_box> level_box_map;
        std::list<int> level_list;

        // cursor is a sort of a pointer to the opened box
        struct cursor
		{
            std::unordered_map<int, simulator::task_box>::iterator in_map;
            std::list<int>::iterator in_list;

            bool operator == (const cursor& other)
			{ return (in_map == other.in_map) && (in_list == other.in_list); }

			bool operator != (const cursor& other)
			{ return (in_map != other.in_map) || (in_list != other.in_list); }
        };

        const cursor last = { level_box_map.end(), level_list.end() };
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

#endif // _task_hash_map_hpp_
