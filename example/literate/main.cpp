/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/. */

/* This document is a literate programming-esque introduction to this graph library. This library
 * is an in memory native graph library designed to be extensible and ergonomic.
 */

/* First we include the header for our library some helper stuff we don't want to bother seeing.
 * The library header includes the constituent headers from the two main components of the system
 * which are model and query, as well as the support files.
 */
#include "graph/graph.hpp"
#include "main.h"

/* Now we will discuss what a graph object is. We use a templated graph system which allows for
 * data model templates to be mixed togeather into a final graph object which sill be responsible
 * for all storage and access of the graph and it's elements.
 * 
 * Below we describe a basic string graph. That is a graph where every element of the graph is uses
 * a standard C++ string as it's value.
 * 
 * This is the core of any graph and is relatively flexible. Instead of a value a pointer to an 
 * abstract base type could be used, or one of the mixins that provides better support for diverse
 * types.
 */
using StrGraphConfig = graph::model::ConfigBuilder<
    graph::model::DataCoreConfigBuilder<std::string, std::string>,
    graph::model::StorageConfigBuilder<graph::storage::SimpleStorage>
>;
using StrGraph = graph::model::PathPropertyGraph< StrGraphConfig >;

/* TODO For a more complex example we will create a simple tagged record and use it as part of a typed
 * graph. The design of our data model allows for an arbitrary number of orthoganol mixins, though
 * performance may suffer due to memory layout if care is not taken, and mixins must cooperate if
 * they have complicated interactions.
 */

int main(int argc, char** argv)
{
    Stopwatch sw;

    /* Opening and loading a file. There are a lot of strange formats.
    */

    std::ifstream file("data/test.nt");
    if (!file.is_open())
    {
        std::cout << "Failed to open file." << std::endl;
        return -1;
    }

    StrGraph g;
    size_t entry_count = 0;
    try
    {
        graph::read_nt(file, 
            [&] (auto subject, auto predicate, auto object, auto label)
            {
                auto s = graph::requireNode(g, subject);
                auto o = graph::requireNode(g, object);
                auto p = g.addEdge(predicate, { s, o });
                if (label != nullptr)
                {
                    auto l = graph::requireLabel(g, *label);
                    //g.attachLabel(, l);
                }
                entry_count++;
            });
    }
    catch (std::exception const& ex)
    {
        std::cout << "Error while loading: " << ex.what() << std::endl;
        return 1;
    }
    
    std::cout << fmt::format("Loaded {0} nodes and {1} edges in {2} entries.", g.nodeCount(), g.edgeCount(), entry_count) << std::endl;

    /* An example query
    */
   
    try
    {
        auto thor = graph::findNode(g, "thor");

        // thor's parents and grandparents
        auto r_thorsParentsAndGrandparents = graph::query(&g)
            .v(thor)
            .out( [](auto n, auto e) { return e->data == "parents"; } )
            .as("parent")
            .out( [](auto n, auto e) { return e->data == "parents"; } )
            .as("grand-parent")
            .merge({ "parent", "grand-parent" })
            .unique()
            .run();

        std::cout << fmt::format("Thor's parents and grand-parents: {0}.", r_thorsParentsAndGrandparents[0]->data) << std::endl;

        // thor is related to someone licked into being
        auto r_thorsWeirdCousin = graph::query(&g)
            .v(graph::findNode(g, "thor"))
            .repeat_breadth(
                [](auto _) { return _.out( [](auto e) { return e->data == "parents"; } ); },
                [&](auto n) { return true; },
                [&](auto n, auto r)
                {
                    // TODO simplify this
                    bool found = false;
                    g.forEdgesOnNode(n, [&](auto e)
                    {
                        if (e->data != "creator") return true;
                        g.forPropsOnEdge(e, [&](auto p)
                        {
                            found = p->data == "licked-into-being";
                            return !found;
                        });
                        return !found;
                    });
                    g.forPropsOnNode(n, [&](auto p) { return !(found = p->data == "licked-into-being"); });
                    return found;
                })
            .run();

        std::cout << fmt::format("Thor's weird cousin: {0}.", r_thorsWeirdCousin[0]->data) << std::endl;
    }
    catch (std::exception const& ex)
    {
        std::cout << "Error while loading: " << ex.what() << std::endl;
        return 1;
    }
}
