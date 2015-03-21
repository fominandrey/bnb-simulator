#include "task_box.hpp"

void simulator::task_box::add(const task&)
{
    increase_size();
}

void simulator::task_box::add(task&&)
{
    increase_size();
}

// ! Preconditions: not empty !
const task& simulator::task_box::get() const
{
    return single;
}

// ! Preconditions: not empty !
void simulator::task_box::remove()
{
    decrease_size();
}
