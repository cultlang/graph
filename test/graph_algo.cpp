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
