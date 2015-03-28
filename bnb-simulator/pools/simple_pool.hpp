#ifndef _simple_pool_hpp_
#define _simple_pool_hpp_

#include "abstract_task_pool.hpp"

#include <set>

namespace pools
{
    class simple_pool : public abstract_task_pool
    {
        std::multiset<task> container;

    public:

        bool empty() const override { return container.empty(); }
        long long size() const override { return container.size(); }

        void add(const task& t) override { container.insert(t); }
        void add(task&& t) override { container.insert(std::move(t)); }

        const task& get() const override;

        void remove() override;

        void trace() const;
    };
}

#endif // _simple_pool_hpp_
