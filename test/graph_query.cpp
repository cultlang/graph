/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/. */

#include "catch2/catch.hpp"

#include "shared.h"

#include "graph/graph.hpp"

#include <string>
#include <iostream>

using namespace graph;
using namespace Catch::Matchers;


TEST_CASE( "graph::query() basics", "[graph::GraphQuery]" )
{
    test_help::str_graph g;

    test_help::fillStrGraphWithNorse(g);

    SECTION( "graph::query() returns working query object" )
    {
        auto q = query(&g);

        CHECK(q->getGraph() == &g);
        CHECK(q->getPipeline()->countPipes() == 0);

        auto r = q.run();

        CHECK(r.size() == 0);
    }

    SECTION( "->addPipe() can add GraphQueryPipeEmpty manually" )
    {
        auto q = query(&g);

        q->getPipeline()->addPipe(std::make_unique<GraphQueryPipeEmpty<decltype(g)>>());

        CHECK(q->getGraph() == &g);
        CHECK(q->getPipeline()->countPipes() == 1);

        auto r = q.run();

        CHECK(r.size() == 0);
    }

    SECTION( ".v() can add GraphQueryStepVertex (AKA `v`) with syntax (single node)" )
    {
        auto q = query(&g)
            .v(findNode(g, "thor"));

        CHECK(q->getGraph() == &g);
        CHECK(q->getPipeline()->countPipes() == 1);
        
        auto r = q.run();

        CHECK(r.size() == 1);
    }

    SECTION( "run can be called multiple times" )
    {
        auto q = query(&g)
            .v(findNode(g, "thor"));

        CHECK(q->getGraph() == &g);
        CHECK(q->getPipeline()->countPipes() == 1);
        
        auto r0 = q.run();

        CHECK(r0.size() == 1);

        auto r1 = q.run();

        CHECK(r1.size() == 1);
    }

    SECTION( ".v() can add GraphQueryStepVertex (AKA `v`) with syntax (multiple nodes)" )
    {
        auto q = query(&g)
            .v(std::vector { findNode(g, "thor"), findNode(g, "odin"), findNode(g, "jord") });

        CHECK(q->getGraph() == &g);
        CHECK(q->getPipeline()->countPipes() == 1);
        
        auto r = q.run();

        CHECK(r.size() == 3);
    }

    SECTION( ".v() can add GraphQueryStepVertex (AKA `v`) with syntax (repeat nodes)" )
    {
        auto thor = findNode(g, "thor");
        auto q = query(&g)
            .v(std::vector { thor, thor, thor, thor, thor });

        CHECK(q->getGraph() == &g);
        CHECK(q->getPipeline()->countPipes() == 1);
        
        auto r = q.run();

        CHECK(r.size() == 5);
    }

    SECTION( "query can be used as a generator" )
    {
        auto q = query(&g)
            .v(std::vector { findNode(g, "thor"), findNode(g, "odin") });

        CHECK(q->getGraph() == &g);
        CHECK(q->getPipeline()->countPipes() == 1);
        
        CHECK(q.done() == false);

        auto r0 = q.next();
        CHECK(q.done() == false);
        REQUIRE(r0 != nullptr);
        CHECK(r0->data == "thor");

        SECTION( "and run does not interrupt it" )
        {
            auto r = q.run();

            CHECK(r.size() == 2);
        }

        auto r1 = q.next();
        CHECK(q.done() == false);
        REQUIRE(r1 != nullptr);
        CHECK(r1->data == "odin");

        auto r2 = q.next();
        CHECK(q.done() == true);
        CHECK(r2 == nullptr);

        SECTION( "and calling next past done does not break it" )
        {
            auto r3 = q.next();
            CHECK(q.done() == true);
            CHECK(r3 == nullptr);
        }
    }

    SECTION( "during a generator run, reset must be called to modify" )
    {
        auto q = query(&g);

        q->getPipeline()->addPipe(std::make_unique<GraphQueryPipeEmpty<decltype(g)>>());

        CHECK(q->getGraph() == &g);
        CHECK(q->getPipeline()->countPipes() == 1);

        auto r = q.next();

        SECTION( "not doing so causes a recoverable exception" )
        {
            REQUIRE_THROWS_AS( q->getPipeline()->addPipe(std::make_unique<GraphQueryPipeEmpty<decltype(g)>>()), graph_error );
        }

        q->reset();
        q->getPipeline()->addPipe(std::make_unique<GraphQueryPipeEmpty<decltype(g)>>());

        CHECK(q->getGraph() == &g);
        CHECK(q->getPipeline()->countPipes() == 2);

        r = q.next();
    }
}


TEST_CASE( "graph::query() syntax traversal queries", "[graph::GraphQuery]" )
{
    test_help::str_graph g;

    test_help::fillStrGraphWithNorse(g);

    SECTION( ".e() can query for specific edges." )
    {
        auto q = query(&g)
            .v(findNode(g, "thor"))
            // same as "out" (mostly)
            .e( [](auto n, auto e) { return edgeIsOutgoing<decltype(g)>(n, e) && e->data == "parents"; } );

        CHECK(q->getPipeline()->countPipes() == 2);
        
        auto r = q.run();

        CHECK(r.size() == 2); // Thor has 2 parents
    }

    SECTION( ".e() can follow edges in a specific way." )
    {
        auto q_mom = query(&g)
            .v(findNode(g, "thor"))
            // same as "out" (mostly)
            .e( [](auto n, auto e) { return edgeIsOutgoing<decltype(g)>(n, e) && e->data == "parents"; },
                // mother is in slot 1 (size guard not needed: 2 size edges are guarnteed)
                [](auto n, auto e, auto ne) { return e->nodes[1] == ne; });

        auto q_dad = query(&g)
            .v(findNode(g, "thor"))
            // same as "out" (mostly)
            .e( [](auto n, auto e) { return edgeIsOutgoing<decltype(g)>(n, e) && e->data == "parents"; },
                // father is in slot 2 (size guard requried)
                [](auto n, auto e, auto ne) { return e->nodes.size() > 2 && e->nodes[2] == ne; });

        CHECK(q_mom->getPipeline()->countPipes() == 2);
        CHECK(q_dad->getPipeline()->countPipes() == 2);
        
        auto r_mom = q_mom.run();
        auto r_dad = q_dad.run();

        REQUIRE(r_mom.size() == 1);
        REQUIRE(r_mom[0]->data == "jord"); // Thor's mom

        REQUIRE(r_dad.size() == 1);
        REQUIRE(r_dad[0]->data == "odin"); // Thor's dad
    }

    SECTION( ".in() can get nodes of incoming edges (1 arg)." )
    {
        auto q = query(&g)
            .v(findNode(g, "thor"))
            .in( [](auto e) { return e->data == "parents"; } );

        CHECK(q->getPipeline()->countPipes() == 2);
        
        auto r = q.run();

        CHECK(r.size() == 3); // Thor has 3 children
    }

    SECTION( ".out() can get nodes of outgoing edges (1 arg)." )
    {
        auto q = query(&g)
            .v(findNode(g, "thor"))
            .out( [](auto e) { return e->data == "parents"; } );

        CHECK(q->getPipeline()->countPipes() == 2);
        
        auto r = q.run();

        CHECK(r.size() == 2); // Thor has 2 parents
    }

    SECTION( ".filter() can filter nodes (1 arg, contrived)." )
    {
        auto q = query(&g)
            .v(std::vector { findNode(g, "thor"), findNode(g, "odin"), findNode(g, "odr") })
            .filter( [](auto n) { return n->data[0] != 'o'; } );

        CHECK(q->getPipeline()->countPipes() == 2);
        
        auto r = q.run();

        REQUIRE(r.size() == 1);
        CHECK(r[0]->data == "thor");
    }

    SECTION( ".filter() can filter gremlins (2 arg, contrived)." )
    {
        auto q = query(&g)
            .v(std::vector { findNode(g, "thor"), findNode(g, "odin"), findNode(g, "odr") })
            .filter( [](auto n, auto r) { return r->node()->data[0] != 'o'; } );

        CHECK(q->getPipeline()->countPipes() == 2);
        
        auto r = q.run();

        REQUIRE(r.size() == 1);
        CHECK(r[0]->data == "thor");
    }

    SECTION( ".unique() ensures unique results (contrived)" )
    {
        auto thor = findNode(g, "thor");
        auto q = query(&g)
            .v(std::vector { thor, thor, thor, thor, thor })
            .unique();

        CHECK(q->getPipeline()->countPipes() == 2);

        auto r = q.run();

        REQUIRE(r.size() == 1); // Thor is unique
    }

    SECTION( ".unique() ensures unique results (large query)" )
    {
        auto q = query(&g)
            .v(findNode(g, "thor"))
            .out( [](auto n, auto e) { return e->data == "parents"; } )
            .in( [](auto n, auto e) { return e->data == "parents"; } );

        CHECK(q->getPipeline()->countPipes() == 3);

        auto r0 = q.run();

        q->reset();
        q = q.unique();

        CHECK(q->getPipeline()->countPipes() == 4);

        auto r1 = q.run();

        REQUIRE(r0.size() > r1.size());
        REQUIRE(r1.size() == 4); // Thor has 4 siblings (including himself)
    }

    SECTION( ".take() ensures limited results (contrived)" )
    {
        auto thor = findNode(g, "thor");
        auto q = query(&g)
            .v(std::vector { thor, thor, thor, thor, thor })
            .take(3);

        CHECK(q->getPipeline()->countPipes() == 2);

        auto r = q.run();

        REQUIRE(r.size() == 3); // We only wanted 3 thors

        q = q
            .out( [](auto n, auto e) { return e->data == "parents"; } );

        CHECK(q->getPipeline()->countPipes() == 3);

        r = q.run();

        REQUIRE(r.size() == 6); // We only wanted 3 thors * 2 parents
    }
}

TEST_CASE( "graph::query() syntax label queries", "[graph::GraphQuery]" )
{
    test_help::str_graph g;

    test_help::fillStrGraphWithNorse(g);

    SECTION( ".as() can label nodes (contrived)" )
    {
        auto q = query(&g)
            .v(findNode(g, "thor"))
            .as("me");

        CHECK(q->getPipeline()->countPipes() == 2);

        auto r = q.run();

         // Thor is returned
        REQUIRE(r.size() == 1);
        CHECK(r[0]->data == "thor");
    }

    SECTION( ".marge() can merge based on labels (large query)" )
    {
        auto q = query(&g)
            .v(findNode(g, "thor"))
            .out( [](auto n, auto e) { return e->data == "parents"; } )
            .as("parent")
            .out( [](auto n, auto e) { return e->data == "parents"; } )
            .as("grand-parent")
            .merge({ "parent", "grand-parent" })
            .unique();

        CHECK(q->getPipeline()->countPipes() == 7);

        auto r = q.run();

        REQUIRE(r.size() == 6); // Thor has 2 + 4 parents-esque
    }

    SECTION( ".except() can filter based on labels (large query)" )
    {
        auto q = query(&g)
            .v(findNode(g, "thor"))
            .as("me")
            .out( [](auto n, auto e) { return e->data == "parents"; } )
            .in( [](auto n, auto e) { return e->data == "parents"; } )
            .unique()
            .except("me");

        CHECK(q->getPipeline()->countPipes() == 6);

        auto r = q.run();

        REQUIRE(r.size() == 3); // Thor has 3 siblings
    }

    SECTION( ".back() can allow backtracking (large query)" )
    {
        auto q = query(&g)
            .v(findNode(g, "fjorgynn"))
            .in( [](auto n, auto e) { return e->data == "parents"; } )
                .as("me")
                    .in( [](auto n, auto e) { return e->data == "parents"; } )
                    .out( [](auto n, auto e) { return e->data == "parents"; } )
                    .out( [](auto n, auto e) { return e->data == "parents"; } )
                    .filter( [](auto n) { return n->data == "bestla"; } )
            .back("me").unique()
            ;

        auto r = q.run();

        REQUIRE(r.size() == 1);
        CHECK(r[0]->data == "frigg"); // frigg is the daughter of fjorgynn who had children with one of bestla's sons
    }
}


TEST_CASE( "graph::query() sub-queries", "[graph::GraphQuery]" )
{
    test_help::str_graph g;

    test_help::fillStrGraphWithNorse(g);

    SECTION( ".optional() can replace a node with an existing subquery (contrived - empty path)" )
    {
        auto q = query(&g)
            .v(findNode(g, "thor"))
            .optional([](auto _) { return _.out( [](auto n, auto e) { return e->data == "creator"; } ); });

        CHECK(q->getPipeline()->countPipes() == 2);

        auto r = q.run();

        REQUIRE(r.size() == 1);
        CHECK(r[0]->data == "thor"); // thor has no creator (he has parents)
    }

    SECTION( ".optional() can replace a node with an existing subquery (contrived - with existing path)" )
    {
        auto q = query(&g)
            .v(findNode(g, "thor"))
            .optional([](auto _) { return _.out( [](auto n, auto e) { return e->data == "parents"; } ); });

        CHECK(q->getPipeline()->countPipes() == 2);

        auto r = q.run();

        REQUIRE(r.size() == 2);  // thor has 2 parents
    }

    SECTION( ".repeat_breadth() will repeat a subquery until a condition is met, breadth first (contrived - success)" )
    {
        int visit_count = 0;
        auto q = query(&g)
            .v(findNode(g, "thor"))
            .repeat_breadth(
                [](auto _) { return _.out( [](auto e) { return e->data == "parents"; } ); },
                [&]() { visit_count++; return true; },
                [&](auto n, auto r)
                {
                    // TODO simplify this
                    bool found = false;
                    g.forAllEdgesOnNode(n, [&](auto e)
                    {
                        if (e->data != "creator") return true;
                        g.forAllPropsOnEdge(e, [&](auto p)
                        {
                            found = p->data == "licked-into-being";
                            return !found;
                        });
                        return !found;
                    });
                    g.forAllPropsOnNode(n, [&](auto p) { return !(found = p->data == "licked-into-being"); });
                    return found;
                });

        CHECK(q->getPipeline()->countPipes() == 2); // copies the pipe expression

        auto r = q.run();

        CHECK(visit_count == 7); // breadth first visted more people

        REQUIRE(r.size() == 1);  // thor is related to someone licked into being
        CHECK(r[0]->data == "buri"); // that wierdo is buri
    }

    SECTION( ".repeat_breadth() will repeat a subquery until a condition is met, breadth first (contrived - failure)" )
    {
        int visit_count = 0;
        auto q = query(&g)
            .v(findNode(g, "frigg"))
            .repeat_breadth(
                [](auto _) { return _.out( [](auto e) { return e->data == "parents"; } ); },
                [&]() {  visit_count++; return true; },
                [&](auto n, auto r)
                {
                    // TODO simplify this
                    bool found = false;
                    g.forAllEdgesOnNode(n, [&](auto e)
                    {
                        if (e->data != "creator") return true;
                        g.forAllPropsOnEdge(e, [&](auto p)
                        {
                            found = p->data == "licked-into-being";
                            return !found;
                        });
                        return !found;
                    });
                    g.forAllPropsOnNode(n, [&](auto p) { return !(found = p->data == "licked-into-being"); });
                    return found;
                });

        CHECK(q->getPipeline()->countPipes() == 2); // copies the pipe expression

        auto r = q.run();

        CHECK(visit_count == 2); // breadth first visted everyone
        REQUIRE(r.size() == 0);  // frigg is not related to someone licked into being
    }

    SECTION( ".repeat_depth() will repeat a subquery until a condition is met, depth first (contrived - success)" )
    {
        int visit_count = 0;
        auto q = query(&g)
            .v(findNode(g, "thor"))
            .repeat_breadth(
                [](auto _) { return _.out( [](auto e) { return e->data == "parents"; } ); },
                [&]() { visit_count++; return true; },
                [&](auto n, auto r)
                {
                    // TODO simplify this
                    bool found = false;
                    g.forAllEdgesOnNode(n, [&](auto e)
                    {
                        if (e->data != "creator") return true;
                        g.forAllPropsOnEdge(e, [&](auto p)
                        {
                            found = p->data == "licked-into-being";
                            return !found;
                        });
                        return !found;
                    });
                    g.forAllPropsOnNode(n, [&](auto p) { return !(found = p->data == "licked-into-being"); });
                    return found;
                });

        CHECK(q->getPipeline()->countPipes() == 2); // copies the pipe expression

        auto r = q.run();

        CHECK(visit_count == 7); // depth first visted less people

        REQUIRE(r.size() == 1);  // thor is related to someone licked into being
        CHECK(r[0]->data == "buri"); // that wierdo is buri
    }

    SECTION( ".repeat_depth() will repeat a subquery until a condition is met, depth first (contrived - failure)" )
    {
        int visit_count = 0;
        auto q = query(&g)
            .v(findNode(g, "frigg"))
            .repeat_depth(
                [](auto _) { return _.out( [](auto e) { return e->data == "parents"; } ); },
                [&]() {  visit_count++; return true; },
                [&](auto n, auto r)
                {
                    // TODO simplify this
                    bool found = false;
                    g.forAllEdgesOnNode(n, [&](auto e)
                    {
                        if (e->data != "creator") return true;
                        g.forAllPropsOnEdge(e, [&](auto p)
                        {
                            found = p->data == "licked-into-being";
                            return !found;
                        });
                        return !found;
                    });
                    g.forAllPropsOnNode(n, [&](auto p) { return !(found = p->data == "licked-into-being"); });
                    return found;
                });

        CHECK(q->getPipeline()->countPipes() == 2); // copies the pipe expression

        auto r = q.run();

        CHECK(visit_count == 2); // depth first visted everyone
        REQUIRE(r.size() == 0);  // frigg is not related to someone licked into being
    }
}
