/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/. */

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

#include "config/config.h"

#include "storage/storage.h"

#include "model/model.h"

#include "algo.hpp"

#include "filters/filters.hpp"

#include "readers/readers.h"

#include "query/engine.hpp"
#include "query/query.hpp"
#include "query/query_library_core.hpp"

namespace ugly
{
    template<typename TGraph, template <typename, typename> typename TGraphQueryLibrary = GraphQueryLibraryCore>
    GraphQuery<TGraph, TGraphQueryLibrary> make_query(TGraph* g)
    {
        return GraphQuery<TGraph, TGraphQueryLibrary>(g);
    }
}

// New syntax
namespace ugly
{
    // TODO forward this from somewhere
    template<typename TGraphConfig>
    class Graph
    {
        public:
            struct Node {};
            struct Edge {};
            struct Prop {};
            struct Label {};
    };

    // TODO forward this from somewhere
    template<typename TGraph, typename TQueryLayers>
    class Query
    {

    };
}
