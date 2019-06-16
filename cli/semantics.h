#pragma once

#include <string>
#include <map>
#include <vector>
#include <memory>

#include "graph/graph.hpp"
#include <functional>

namespace semantics {
    struct ParseTree {
    public:
        std::string variable;
        std::vector<std::vector<std::string>> sexprs;
    };


    class Semantics {
        using str_graph = graph::Graph< graph::GraphCore< graph::basic_core_config<std::string> > >;
        using res_type = std::vector<str_graph::Node*>;
        std::map<std::string, std::shared_ptr<str_graph>> _graphs;
        std::map<std::string, std::function<void(std::shared_ptr<ParseTree>)>> _globals;

        std::map<std::string, std::function<void(std::shared_ptr<str_graph>, std::shared_ptr<ParseTree>)>> _graphFunctions;
        
    public:
        Semantics();
        void evaluate(std::shared_ptr<ParseTree> p);

    private:
    // Globals
        void newGraph(std::shared_ptr<ParseTree>);
        void deleteGraph(std::shared_ptr<ParseTree>);

    // Graph Mutation
        void addNode(std::shared_ptr<str_graph>, std::shared_ptr<ParseTree>);
        void addEdge(std::shared_ptr<str_graph>, std::shared_ptr<ParseTree>);

    // Query
        void query(std::shared_ptr<str_graph>, std::shared_ptr<ParseTree>);

    };
}