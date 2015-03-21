#include "apply_settings.hpp"

#include <simulators/resolver.hpp>
#include <processes/process.hpp>

#include <kernel/dmmemory/simpsched.hpp>
#include <kernel/dmmemory/schedtracer.hpp>

#include <list>

int main()
{
    apply_settings();

    int n, steps;

    std::cin >> n >> steps;

    simulator::communicator comm(n);

    std::list<SimpSched> scheduler_pool;
    std::list<simulator::resolver> resolver_pool;
    std::list<SchedTracer> tracer_pool;

    std::list<simulator::process> process_pool;

    for (int i = 0; i < n; ++i) {
        scheduler_pool.emplace_back(steps);
        resolver_pool.emplace_back();
        tracer_pool.emplace_back(1);

        process_pool.emplace_back(comm, scheduler_pool.back(), resolver_pool.back(), tracer_pool.back());
    }

    while (!process_pool.empty()) {
        auto process_cursor = process_pool.begin();

        while (process_cursor != process_pool.end()) {
            if (process_cursor->proceed())
                ++process_cursor;
            else
                process_cursor = process_pool.erase(process_cursor);
        }
    }
}
