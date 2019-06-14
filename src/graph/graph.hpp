#pragma once

#include <map>
#include <vector>
#include <deque>

/*
    This header only graph library has some very specific design constraints:

    * Highly configurable, it should be usable in a lot of circumstances.
        * C friendly potential.
    * Supports node labels, properties, and sorted n-ary edges.
    * Supports parent graphs, which it can override locally.
    * This necessitates component style storage.
    * Supports type based helper lookups.
*/

#include "util.hpp"

#include "model/core.hpp"
#include "model/typed.hpp"
#include "model/todo.hpp"
#include "model/final.hpp"

#include "algo.hpp"

#include "query/engine.hpp"
#include "query/query.hpp"
#include "query/query_library_core.hpp"

namespace graph
{
    template<typename TGraph, template <typename, typename> typename TGraphQueryLibrary = GraphQueryLibraryCore>
    GraphQuery<TGraph, TGraphQueryLibrary> query(TGraph* g)
    {
        return GraphQuery<TGraph, TGraphQueryLibrary>(g);
    }
}
