#ifndef _task_hpp_
#define _task_hpp_

struct task
{
    // level in the bnb-tree
    int level;

    bool operator <(const task& other) const { return (level < other.level); }
};

#endif // _task_hpp_
