/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/. */

#pragma once

#include "graph/graph.hpp"

#include <string>

#ifdef __clang__
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wunused-variable"
#endif

namespace test_help
{
    using StrGraphConfig = ugly::model::ConfigBuilder<
        ugly::model::DataCoreConfigBuilder<std::string, std::string>,
        ugly::model::StorageConfigBuilder<ugly::storage::SimpleStorage>
    >;
    using StrGraph = ugly::model::PathPropertyGraph< StrGraphConfig >;

    void fillStrGraphWithNorse(StrGraph & g);
}

// basic str graph
namespace ugly_test
{
    struct StrGraphConfiguration
        : public ugly::config::GraphConfigurationDefault
    {
        // simple definition
        using DefaultType = ugly::config::SimpleValueType<std::string>;
    };

    using StrGraph = ugly::Graph<StrGraphConfiguration>;

    using StrNode = typename StrGraph::Node;
    using StrEdge = typename StrGraph::Edge;
    using StrProp = typename StrGraph::Prop;
    using StrLabel = typename StrGraph::Label;

    using StrQuery = ugly::Query<StrGraph, ugly::query::layer::Default<>>;
}

// object oriented graph
namespace ugly_test
{
    class IShared
    {
        public:
            virtual inline ~IShared() = default;

            virtual std::string typeName() = 0;
    };

    class INode
        : public IShared
    {
        public:
            virtual inline ~INode() = default;

            virtual std::string valueDisplay() = 0;
    };

    class IEdge
        : public IShared
    {
        public:
            virtual inline ~IEdge() = default;

            virtual std::string edgeDisplay(INode* src, INode* dst) = 0;
    };

    struct ObjGraphConfiguration
        : public ugly::config::GraphConfigurationDefault
    {
        using NodeType = ugly::config::VirtualPointerType<INode>;
        using EdgeType = ugly::config::VirtualPointerType<IEdge>;
        using PropType = ugly::config::VirtualPointerType<IShared>;
        using LabelType = ugly::config::VirtualPointerType<IShared>;

        using Layers = ugly::model::layer::VoidLayer<>;
    };

    using ObjGraph = ugly::Graph<ObjGraphConfiguration>;

    using ObjNode = typename ObjGraph::Node;
    using ObjEdge = typename ObjGraph::Edge;

    using ObjQuery = ugly::Query<ObjGraph, ugly::query::layer::Default<>>;
}
