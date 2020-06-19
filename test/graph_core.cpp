/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/. */

#include "catch2/catch.hpp"

#include "graph/graph.hpp"

#include "shared.h"

using namespace graph;
using namespace Catch::Matchers;

TEST_CASE( "::graph::model::PathPropertyGraph basic initialization, adding, and counts", "[graph::model::PathPropertyGraph]" )
{
    test_help::StrGraph g;

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



TEST_CASE( "::graph::model::PathPropertyGraph inspection", "[graph::model::PathPropertyGraph]" )
{
    test_help::StrGraph g;

    auto l0 = g.addLabel("label");

    auto n0 = g.addNode("node-0");
    auto n1 = g.addNode("node-1");
    auto n2 = g.addNode("node-2");
    auto n3 = g.addNode("node-3");

    auto e0 = g.addEdge("edge", { n0, n1 });

    auto p0 = g.addProp("prop-0", n0);
    auto p1 = g.addProp("prop-1", n0);
    auto p2 = g.addProp("prop-2", e0);

    REQUIRE(g.labelCount() == 1);
    REQUIRE(g.nodeCount() == 4);
    REQUIRE(g.edgeCount() == 1);
    REQUIRE(g.propCount() == 3);

    SECTION( "inspect labels on nodes" )
    {

    }

    SECTION( "inspect edges on nodes (find it / void)" )
    {
        bool found = false;
        g.forEdgesOnNode(n0, [&](auto e){ if (e0->data == e->data) found = true; });
        CHECK(found == true);
    }
    SECTION( "inspect edges on nodes (find it / break)" )
    {
        bool found = false;
        g.forEdgesOnNode(n1, [&](auto e){ if (e0->data == e->data) found = true; return !found; });
        CHECK(found == true);
    }
    SECTION( "inspect edges on nodes (don't find / void)" )
    {
        bool found = false;
        g.forEdgesOnNode(n2, [&](auto e){ if (e0->data == e->data) found = true; });
        CHECK(found == false);
    }
    SECTION( "inspect edges on nodes (don't find / break)" )
    {
        bool found = false;
        g.forEdgesOnNode(n3, [&](auto e){ if (e0->data == e->data) found = true; return !found; });
        CHECK(found == false);
    }


    SECTION( "inspect nodes in edges (find it / void)" )
    {
        bool found = false;
        g.forNodesInEdge(e0, [&](auto n){ if (n0->data == n->data) found = true; });
        CHECK(found == true);
    }
    SECTION( "inspect nodes in edges (find it / break)" )
    {
        bool found = false;
        g.forNodesInEdge(e0, [&](auto n){ if (n1->data == n->data) found = true; return !found; });
        CHECK(found == true);
    }
    SECTION( "inspect nodes in edges (don't find / void)" )
    {
        bool found = false;
        g.forNodesInEdge(e0, [&](auto n){ if (n2->data == n->data) found = true; });
        CHECK(found == false);
    }
    SECTION( "inspect nodes in edges (don't find / break)" )
    {
        bool found = false;
        g.forNodesInEdge(e0, [&](auto n){ if (n3->data == n->data) found = true; return !found; });
        CHECK(found == false);
    }


    SECTION( "inspect props on nodes (find it / void)" )
    {
        bool found = false;
        g.forPropsOnNode(n0, [&](auto p){ if (p0->data == p->data) found = true; });
        CHECK(found == true);
    }
    SECTION( "inspect props on nodes (find it / break)" )
    {
        bool found = false;
        g.forPropsOnNode(n0, [&](auto p){ if (p1->data == p->data) found = true; return !found; });
        CHECK(found == true);
    }
    SECTION( "inspect props on nodes (don't find / void)" )
    {
        bool found = false;
        g.forPropsOnNode(n2, [&](auto p){ if (p0->data == p->data) found = true; });
        CHECK(found == false);
    }
    SECTION( "inspect props on nodes (don't find / break)" )
    {
        bool found = false;
        g.forPropsOnNode(n3, [&](auto p){ if (p1->data == p->data) found = true; return !found; });
        CHECK(found == false);
    }


    SECTION( "inspect props on edges (find it / void)" )
    {
        bool found = false;
        g.forPropsOnEdge(e0, [&](auto p){ if (p2->data == p->data) found = true; });
        CHECK(found == true);
    }
    SECTION( "inspect props on edges (find it / break)" )
    {
        bool found = false;
        g.forPropsOnEdge(e0, [&](auto p){ if (p2->data == p->data) found = true; return !found; });
        CHECK(found == true);
    }
    SECTION( "inspect props on edges (don't find / void)" )
    {
        bool found = false;
        g.forPropsOnEdge(e0, [&](auto p){ if (p0->data == p->data) found = true; });
        CHECK(found == false);
    }
    SECTION( "inspect props on edges (don't find / break)" )
    {
        bool found = false;
        g.forPropsOnEdge(e0, [&](auto p){ if (p1->data == p->data) found = true; return !found; });
        CHECK(found == false);
    }


    REQUIRE(g.labelCount() == 1);
    REQUIRE(g.nodeCount() == 4);
    REQUIRE(g.edgeCount() == 1);
    REQUIRE(g.propCount() == 3);
}



TEST_CASE( "::graph::model::PathPropertyGraph updates", "[graph::model::PathPropertyGraph]" )
{
    test_help::StrGraph g;

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