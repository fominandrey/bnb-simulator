#ifndef _abstract_task_pool_hpp_
#define _abstract_task_pool_hpp_

#include <util/common/bnberrcheck.hpp>

#include "pool_strategies.hpp"
#include "task.hpp"

#include <utility> // std::move

namespace pools
{
    class abstract_task_pool
	{
        // to save search strategy and restore it later
        pool_strategy save;

    protected:

        pool_strategy strategy = pool_strategy::WFS;

    public:

        virtual bool empty() const = 0;
        virtual long long size() const = 0;

        void set_strategy(pool_strategy s) { strategy = s; }
        void set_sending_strategy();

        void save_strategy() { save = strategy; }
        void restore_strategy() { strategy = save; }

        virtual void add(const task& t) = 0;
        virtual void add(task&& t) = 0;

        virtual const task& get() const = 0;

        virtual void remove() = 0;

        virtual ~abstract_task_pool() = default;
    };
}

#endif // _abstract_task_pool_hpp_
