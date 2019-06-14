#pragma once

#include "graph/graph.hpp"

#include <string>

#ifdef __clang__
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wunused-variable"
#endif

namespace test_help
{
    using str_graph = graph::Graph< graph::GraphCore< graph::basic_core_config<std::string> > >;

    void fillStrGraphWithNorse(str_graph & g);
}
