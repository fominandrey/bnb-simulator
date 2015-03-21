#ifndef _abstract_task_map_hpp_
#define _abstract_task_map_hpp_

#include "abstract_task_pool.hpp"
#include "abstract_task_box.hpp"

namespace pools
{
    class abstract_task_map : public abstract_task_pool
	{

    protected:
        abstract_task_box* box = 0;

    public:

        bool box_opened() const { return (box != 0); }
        // ! Preconditions: box is opened !
        bool box_empty() const { return box->empty(); }
        // ! Preconditions: box is opened !
        long long box_size() const { return box->size(); }

        virtual void open_box(int level) = 0;
        virtual void open_next_box() = 0;
        virtual void open_box() = 0;

        virtual void remove_box() = 0;

        // ! Precondition: i is in { 0, 1 } !
        virtual void save_box(int i) = 0;
        virtual void load_box(int i) = 0;

        // ! Preconditions: box is opened !
        void add(const task& t) override;
        void add(task&& t) override;
        // ! Preconditions: box is opened !
        const task& get() const override;
        // ! Preconditions: box is opened !
        void remove() override;
    };
}

#endif // _abstract_task_map_hpp_
