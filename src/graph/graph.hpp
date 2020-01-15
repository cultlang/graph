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

#include "model/storage/simple.hpp"

#include "model/core.hpp"
#include "model/typed.hpp"
#include "model/todo.hpp"
#include "model/final.hpp"

#include "algo.hpp"

#include "filters/filters.hpp"

#include "readers/readers.h"

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
