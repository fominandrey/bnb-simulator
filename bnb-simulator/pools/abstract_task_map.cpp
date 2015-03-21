#include "abstract_task_map.hpp"

// ! Preconditions: box is opened !
void pools::abstract_task_map::add(const task& t)
{
    box->add(t);
    increase_size();
}

// ! Preconditions: box is opened !
void pools::abstract_task_map::add(task&& t)
{
    box->add(std::move(t));
    increase_size();
}

// ! Preconditions: box is opened !
const task& pools::abstract_task_map::get() const
{
    return box->get();
}

// ! Preconditions: box is opened !
void pools::abstract_task_map::remove()
{
    box->remove();
    decrease_size();
}
