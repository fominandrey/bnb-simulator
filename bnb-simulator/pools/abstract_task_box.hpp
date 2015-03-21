#ifndef _abstract_task_box_hpp_
#define _abstract_task_box_hpp_

#include "task.hpp"

namespace pools
{
    class abstract_task_box
	{
        // box size
        long long n = 0;

    protected:
        //
        void increase_size() { ++n; }
        void decrease_size() { --n; }

    public:

        bool empty() const { return !(n > 0); }

        long long size() const { return n; }

        virtual void add(const task&) = 0;
        virtual void add(task&&) = 0;

        virtual const task& get() const = 0;

        virtual void remove() = 0;

        virtual ~abstract_task_box() = default;
    };
}

#endif // _abstract_task_box_hpp_
