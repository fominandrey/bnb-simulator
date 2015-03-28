#include "resolver.hpp"

int simulator::resolver::max_level;

void simulator::resolver::apply_settings(const JSONNode& node)
{
    max_level = node["maximal task level"].as_int();
}

void simulator::resolver::getInfo(SolverInfo& info)
{
    info.mNSub = task_pool.size();
    info.mRecUpdated = record_updated;
}

void simulator::resolver::resetInfo()
{
    record_updated = false;
}

// no actions except recording to the log
void simulator::resolver::getRecords(BinarySerializer& ser)
{}

// no actions except recording to the log
void simulator::resolver::putRecords(BinarySerializer& ser)
{}

void simulator::resolver::setSearchStrategy(int strategy)
{
    solvable = (strategy != SearchStrategies::NOS);

    if (solvable) {
        switch (strategy) {
        // best-first search
        case SearchStrategies::BFS :
            task_pool.set_strategy(pools::pool_strategy::BFS);
            break;
        // depth-first search
        case SearchStrategies::DFS :
            task_pool.set_strategy(pools::pool_strategy::DFS);
            break;
        // width-first search
        case SearchStrategies::WFS :
            task_pool.set_strategy(pools::pool_strategy::WFS);
            break;
        // this case can't occur
        default:
            BNB_ERROR_REPORT("No such strategy");
        }
    }
}

// no actions except recording to the log
void simulator::resolver::setHeuristic(int heuristic)
{}

// no actions except recording to the log
void simulator::resolver::setBoundingMethod(int method)
{}

void simulator::resolver::getSubs(int& num, BinarySerializer& ser)
{
    int n = (task_pool.size() < num) ? task_pool.size() : num;
    // n tasks to send out
    ser << n;

    if (n > 0) {
        // save search strategy prior to set sending strategy
        task_pool.save_strategy();
        task_pool.set_sending_strategy();

        // k tasks have been put to the serializer
        int k = 0;

        while (k < n && !task_pool.empty()) {
            ser << task_pool.get();
            task_pool.remove();
            ++k;
        }
        // restore search strategy
        task_pool.restore_strategy();
    }

    num = n;
}

int simulator::resolver::putSubs(BinarySerializer& ser)
{
    int n;
    // n tasks to be put into the pool
    ser >> n;

    task t;

    for (int i = 0; i < n; ++i) {
        ser >> t;
        task_pool.add(t);
    }

    return n;
}

void simulator::resolver::solve(long long& steps)
{
    // k steps have been done
    long long k = 0;

    if (solvable) {

         while (k < steps && !task_pool.empty()) {
            task t = task_pool.get();
            task_pool.remove();
            // split into 2 subsets
            if (split(t)) {
                task_pool.add(task{ t.level + 1 });
                task_pool.add(task{ t.level + 1 });
            }
            ++k;
        }
    }

    steps = k;
}
