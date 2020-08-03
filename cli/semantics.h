/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/. */

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
        using StrGraphConfig = ugly::model::ConfigBuilder<
            ugly::model::DataCoreConfigBuilder<std::string, std::string>,
            ugly::model::StorageConfigBuilder<ugly::storage::SimpleStorage>
        >;
        using StrGraph = ugly::model::PathPropertyGraph< StrGraphConfig >;

        using res_type = std::vector<StrGraph::Node*>;
        std::map<std::string, std::shared_ptr<StrGraph>> _graphs;
        std::map<std::string, std::function<void(std::shared_ptr<ParseTree>)>> _globals;

        std::map<std::string, std::function<void(std::shared_ptr<StrGraph>, std::shared_ptr<ParseTree>)>> _graphFunctions;
        
    public:
        Semantics();
        void evaluate(std::shared_ptr<ParseTree> p);

    private:
    // Globals
        void newGraph(std::shared_ptr<ParseTree>);
        void deleteGraph(std::shared_ptr<ParseTree>);

    // Graph Mutation
        void addNode(std::shared_ptr<StrGraph>, std::shared_ptr<ParseTree>);
        void addEdge(std::shared_ptr<StrGraph>, std::shared_ptr<ParseTree>);

    // Query
        void query(std::shared_ptr<StrGraph>, std::shared_ptr<ParseTree>);

    };
}
