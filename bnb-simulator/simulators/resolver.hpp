#ifndef _resolver_hpp_
#define _resolver_hpp_

#define NDEBUG

#include <libjson/libjson.h>

#include <kernel/dmmemory/sstrat.hpp>
#include <kernel/dmmemory/bnbresolver.hpp>

#include <pools/simple_pool.hpp>

#include <random>

namespace simulator
{
    class resolver : public BNBResolver
	{
        pools::simple_pool task_pool;

        bool solvable = true;

        bool record_updated = false;

        static int max_level;

        // tools to generate pseudo-random integers
        std::mt19937 generator{ std::random_device{}() };
        std::uniform_int_distribution<> distribution{ 0, max_level - 1 };

        bool split(const task& t) { return !(distribution(generator) < t.level); }

    public:

        static void apply_settings(const JSONNode& node);

        void load_initial_task() { task_pool.add(task{0}); }

        void getInfo(SolverInfo& info) override;

        void resetInfo() override;

        void getSubs(int& num, BinarySerializer& ser) override;

        void getRecords(BinarySerializer& ser) override;

        int putSubs(BinarySerializer& ser) override;

        void putRecords(BinarySerializer& ser) override;

        void setSearchStrategy(int strategy) override;

        void setHeuristic(int heuristic) override;

        void setBoundingMethod(int method) override;

        void solve(long long& steps) override;
    };
}

#endif // _resolver_hpp_
