/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/. */

#include "catch2/catch.hpp"

#include "shared.h"

#include "graph/graph.hpp"

#include <string>

using namespace graph;
using namespace Catch::Matchers;


TEST_CASE( "graph::findNode", "[graph::findNode]" )
{
    test_help::str_graph g;

    g.addNode("test");

    CHECK(findNode(g, "test") != nullptr);
    CHECK(findNode(g, "not-test") == nullptr);
}
