#ifndef _task_box_hpp_
#define _task_box_hpp_

#include "abstract_task_box.hpp"

namespace simulator
{
    class task_box : public pools::abstract_task_box
	{
        task single;

    public:
        // constructor
        explicit task_box(int level) : single{level}
        {}

        void add(const task&) override;
        void add(task&&) override;
        // ! Preconditions: not empty !
        const task& get() const override;
        // ! Preconditions: not empty !
        void remove() override;
    };
}

#endif // _task_box_hpp_
