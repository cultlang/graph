#include "shared.h"

using namespace graph;

void test_help::fillStrGraphWithNorse(test_help::str_graph & g)
{
    // from http://www.veritablehokum.com/comic/the-norse-god-family-tree/

    /* Data Model:
     * - [Edges]
     *   - parents: where possible is organized (child, mother, father)
     */ 

    // Helper temporaries
    typename test_help::str_graph::Edge* e;


    auto audumbla = g.addNode("audumbla");
    g.addProp("animal", audumbla);
    g.addProp("cow", audumbla);



    auto tyr = g.addNode("tyr");

    auto mimir = g.addNode("mimir");

    auto bestla = g.addNode("bestla");

    auto burr = g.addNode("burr");

    auto buri = g.addNode("buri");
    e = g.addEdge("creator", { buri, audumbla });
    g.addProp("licked-into-being", e);

    e = g.addEdge("married", { burr, bestla });



    // now it stops beign weird
    auto fjorgynn = g.addNode("fjorgynn");
    auto fjorgynn_wife = g.addNode("fjorgynn_wife");

    auto vili = g.addNode("vili");
    e = g.addEdge("parents", { vili, bestla, burr });

    auto ve = g.addNode("ve");
    e = g.addEdge("parents", { ve, bestla, burr });

    auto hoenir = g.addNode("hoenir");
    e = g.addEdge("parents", { hoenir, bestla, burr });



    auto odin = g.addNode("odin");
    e = g.addEdge("parents", { odin, bestla, burr });

    auto jord = g.addNode("jord");

    auto frigg = g.addNode("frigg");
    e = g.addEdge("parents", { frigg, fjorgynn_wife, fjorgynn });

    e = g.addEdge("married", { odin, frigg });



    auto nanna = g.addNode("nanna");

    auto baldr = g.addNode("baldr");

    auto hodr = g.addNode("hodr");
    e = g.addEdge("parents", { hodr, frigg, odin });

    auto bragi = g.addNode("bragi");
    e = g.addEdge("parents", { bragi, frigg, odin });

    auto idunn = g.addNode("idunn");

    e = g.addEdge("married", { baldr, nanna });



    auto forseti = g.addNode("forseti");
    e = g.addEdge("parents", { forseti, nanna, baldr });

    auto sif = g.addNode("sif");

    auto thor = g.addNode("thor");
    e = g.addEdge("parents", { thor, jord, odin });

    auto jarnsaxa = g.addNode("jarnsaxa");

    e = g.addEdge("married", { thor, sif });




    auto skadi = g.addNode("skadi");

    auto njord = g.addNode("njord");

    auto ullr = g.addNode("ullr");
    e = g.addEdge("parents", { ullr, sif, odin });

    auto modi = g.addNode("modi");
    e = g.addEdge("parents", { modi, sif, thor });

    auto thrud = g.addNode("thrud");
    e = g.addEdge("parents", { thrud, sif, thor });

    auto magni = g.addNode("magni");
    e = g.addEdge("parents", { magni, jarnsaxa, thor });



    auto gerdr = g.addNode("gerdr");

    auto freyr = g.addNode("freyr");
    e = g.addEdge("parents", { freyr, skadi, njord });

    auto freya = g.addNode("freya");
    e = g.addEdge("parents", { freya, skadi, njord });

    auto odr = g.addNode("odr");

    e = g.addEdge("married", { freyr, gerdr });
    e = g.addEdge("married", { odr, freya });
}



#include "catch2/catch.hpp"

using namespace Catch::Matchers;

TEST_CASE( "test_help works correctly", "[test_help]" )
{
    test_help::str_graph g;

    test_help::fillStrGraphWithNorse(g);
}