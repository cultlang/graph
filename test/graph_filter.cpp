/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/. */

#include "catch2/catch.hpp"

#include "shared.h"

#include "graph/graph.hpp"

#include <string>
#include <iostream>

using namespace graph;
namespace f = graph::filter;
using namespace Catch::Matchers;

TEST_CASE( "graph filter basics", "[graph::filters]" )
{

}


TEST_CASE( "graph filters usable in graph::query() builders", "[graph::filters]" )
{
    test_help::str_graph g;

    test_help::fillStrGraphWithNorse(g);

    SECTION( ".in() can use filters." )
    {
        auto q = query(&g)
            .v(findNode(g, "thor"))
            .in(f::byValue("parents"));

        CHECK(q->getPipeline()->countPipes() == 2);
        
        auto r = q.run();

        CHECK(r.size() == 3); // Thor has 3 children
    }
}

