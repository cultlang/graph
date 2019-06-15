#include "catch2/catch.hpp"

#include "graph/graph.hpp"

#include "shared.h"

using namespace graph;
using namespace Catch::Matchers;

TEST_CASE( "::graph::GraphCore basic initialization, adding, and counts", "[graph::GraphCore]" )
{
    test_help::str_graph g;

    REQUIRE(g.labelCount() == 0);
    REQUIRE(g.nodeCount() == 0);
    REQUIRE(g.edgeCount() == 0);
    REQUIRE(g.propCount() == 0);

    SECTION( "adding a label, only adds a label" )
    {
        g.addLabel("a-label");

        CHECK(g.labelCount() == 1);
        CHECK(g.nodeCount() == 0);
        CHECK(g.edgeCount() == 0);
        CHECK(g.propCount() == 0);
    }

    SECTION( "adding a node, only adds a node" )
    {
        g.addNode("a-node");

        CHECK(g.labelCount() == 0);
        CHECK(g.nodeCount() == 1);
        CHECK(g.edgeCount() == 0);
        CHECK(g.propCount() == 0);
    }

    SECTION( "adding an edge (only on nodes), only adds an edge" )
    {
        auto a = g.addNode("a-node");
        auto b = g.addNode("b-node");

        CHECK(g.labelCount() == 0);
        CHECK(g.nodeCount() == 2);
        CHECK(g.edgeCount() == 0);
        CHECK(g.propCount() == 0);

        auto e = g.addEdge("a-edge", { a, b });

        CHECK(g.labelCount() == 0);
        CHECK(g.nodeCount() == 2);
        CHECK(g.edgeCount() == 1);
        CHECK(g.propCount() == 0);
    }

    SECTION( "adding an edge with too few nodes excepts" )
    {
        auto a = g.addNode("a-node");

        CHECK(g.labelCount() == 0);
        CHECK(g.nodeCount() == 1);
        CHECK(g.edgeCount() == 0);
        CHECK(g.propCount() == 0);

        SECTION( "0 nodes" )
        {
            REQUIRE_THROWS_AS( g.addEdge("a-edge", { }), graph_error );
        }
        SECTION( "1 node" )
        {
            REQUIRE_THROWS_AS( g.addEdge("a-edge", { a }), graph_error );
        }

        CHECK(g.labelCount() == 0);
        CHECK(g.nodeCount() == 1);
        CHECK(g.edgeCount() == 0);
        CHECK(g.propCount() == 0);
    }

    SECTION( "adding a property (on node), only adds a property" )
    {
        auto n = g.addNode("a-node");

        CHECK(g.labelCount() == 0);
        CHECK(g.nodeCount() == 1);
        CHECK(g.edgeCount() == 0);
        CHECK(g.propCount() == 0);

        auto p = g.addProp("a-prop", n);

        CHECK(g.labelCount() == 0);
        CHECK(g.nodeCount() == 1);
        CHECK(g.edgeCount() == 0);
        CHECK(g.propCount() == 1);
    }

    SECTION( "adding a property (on edge), only adds a property" )
    {
        auto a = g.addNode("a-node");
        auto b = g.addNode("b-node");
        auto e = g.addEdge("a-edge", { a, b });
        
        CHECK(g.labelCount() == 0);
        CHECK(g.nodeCount() == 2);
        CHECK(g.edgeCount() == 1);
        CHECK(g.propCount() == 0);

        auto p = g.addProp("a-prop", e);

        CHECK(g.labelCount() == 0);
        CHECK(g.nodeCount() == 2);
        CHECK(g.edgeCount() == 1);
        CHECK(g.propCount() == 1);
    }
}



TEST_CASE( "::graph::GraphCore updates", "[graph::GraphCore]" )
{
    test_help::str_graph g;

    auto l = g.addLabel("label");

    auto n0 = g.addNode("node-0");
    auto n1 = g.addNode("node-1");
    auto n2 = g.addNode("node-2");
    auto n3 = g.addNode("node-3");

    auto e = g.addEdge("edge", { n0, n1 });

    REQUIRE(g.labelCount() == 1);
    REQUIRE(g.nodeCount() == 4);
    REQUIRE(g.edgeCount() == 1);
    REQUIRE(g.propCount() == 0);

    // TODO
    SECTION( "attaching a label to a node" )
    {
        g.attachLabel(n0, l);
    }

    SECTION( "attaching an edge to a node" )
    {
        g.attachEdge(n2, e);
    }

    SECTION( "attaching an edge to a node, inserted first" )
    {
        g.attachEdge(n3, e, 0);
    }

    SECTION( "attaching an edge to a node, inserted middle" )
    {
        g.attachEdge(n3, e, 1);
    }

    SECTION( "attaching an edge to a node, inserted last" )
    {
        g.attachEdge(n3, e, 2);
    }

    SECTION( "attaching an edge to a node, inserted out of range, error" )
    {
        REQUIRE_THROWS_AS( g.attachEdge(n3, e, 5), graph_error );
    }

    REQUIRE(g.labelCount() == 1);
    REQUIRE(g.nodeCount() == 4);
    REQUIRE(g.edgeCount() == 1);
    REQUIRE(g.propCount() == 0);
}