#ifndef _pool_strategies_hpp_
#define _pool_strategies_hpp_

namespace pools
{
    enum class pool_strategy
	{
        // best-first search
        BFS,
        // depth-first search
        DFS,
        // width-first search
        WFS
    };
}

#endif // _pool_strategies_hpp_
