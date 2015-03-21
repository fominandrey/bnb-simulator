#ifndef _abstract_task_pool_hpp_
#define _abstract_task_pool_hpp_

#include <util/common/bnberrcheck.hpp>

#include "pool_strategies.hpp"
#include "task.hpp"

#include <utility>

namespace pools
{
    class abstract_task_pool
	{
        // size of the pool
        long long n = 0;
        // stores saved search strategy
        pool_strategy save;

    protected:

        pool_strategy strategy = pool_strategy::WFS;

        void increase_size() { ++n; }
        void decrease_size() { --n; }

    public:

        bool empty() const { return !(n > 0); }
        long long size() const { return n; }

        void set_strategy(pool_strategy s) { strategy = s; }
        void set_sending_strategy();

        void save_strategy() { save = strategy; }
        void load_strategy() { strategy = save; }

        virtual void add(const task& t) = 0;
        virtual void add(task&& t) = 0;

        virtual const task& get() const = 0;

        virtual void remove() = 0;

        virtual ~abstract_task_pool() = default;
    };
}

#endif // _abstract_task_pool_hpp_
