#include "resolver.hpp"

int simulator::resolver::max_level;

//
void simulator::resolver::apply_settings(const JSONNode& node)
{
    max_level = node["maximal task level"].as_int();
}

// constructor
simulator::resolver::resolver()
{
    // add initial task to the pool
    task_pool.open_box(0);
    task_pool.add(task{0});
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

// no actions except recording to the log file
void simulator::resolver::getRecords(BinarySerializer& ser)
{}

// no actions except recording to the log file
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

// no actions except recording to the log file
void simulator::resolver::setHeuristic(int heuristic)
{}

// no actions except recording to the log file
void simulator::resolver::setBoundingMethod(int method)
{}

void simulator::resolver::getSubs(int& num, BinarySerializer& ser)
{
    int n = (task_pool.size() < num) ? task_pool.size() : num;
    // n tasks to be sent out
    ser << n;

    if (!task_pool.empty()) {
        // save search strategy prior to set sending strategy
        task_pool.save_strategy();

        task_pool.set_sending_strategy();

        task t;
        // q tasks have been sent out
        int q = 0;

        do {
            task_pool.open_box();

            do {
                t = task_pool.get();
                task_pool.remove();

                ser << t;

                ++q;

                if (!(q < n)) {
                    if (task_pool.box_empty())
                        task_pool.remove_box();
                    // break both cycles
                    goto done;
                }
            } while (!task_pool.box_empty());

            task_pool.remove_box();

        } while (!task_pool.empty());

    done:
        // load saved strategy
        task_pool.load_strategy();
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
        task_pool.open_box(t.level);
        task_pool.add(t);
    }

    return n;
}

void simulator::resolver::solve(long long& steps)
{
    // q steps have been done
    long long q = 0;

    if (solvable && !task_pool.empty()) {

        task t;

        do {
            task_pool.open_box();
            task_pool.save_box(0);
            // open and save the next box
            task_pool.open_next_box();
            task_pool.save_box(1);
            // load the previous box
            task_pool.load_box(0);

            do {
                t = task_pool.get();
                task_pool.remove();

                // split t into 2 subtasks
                if (split(t)) {
                    t = task{ t.level + 1 };
                    task_pool.load_box(1);
                    task_pool.add(t);
                    task_pool.add(t);
                    task_pool.load_box(0);
                }

                ++q;

                if (!(q < steps)) {
                    if (task_pool.box_empty())
                        task_pool.remove_box();
                    // break both cycles
                    goto done;
                }

            } while (!task_pool.box_empty());

            task_pool.remove_box();

        } while (!task_pool.empty());
    }

done:
    steps = q;
}
