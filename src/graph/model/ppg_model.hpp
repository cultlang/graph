/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/. */

#pragma once

#include <vector>
#include <deque>
#include <string>

#include "graph/util.hpp"
#include "graph/storage/storage.h"

namespace ugly {
namespace model
{
    template <typename TConfig>
    class PathPropertyGraph
    {
    public:
        using NodeData = typename TConfig::Data::Node; 
        using EdgeData = typename TConfig::Data::Edge; 
        using PathData = typename TConfig::Data::Path; 
        using LabelData = typename TConfig::Data::Label; 
        using PropData = typename TConfig::Data::Prop; 

    public:
        struct Node
        {
            using Store = typename TConfig::Storage::Store::PerNode;

            NodeData data;
            Store store;

            Node(NodeData const& data, Store && store)
                : data(data), store(store)
            { }
        };
        struct Edge
        {
            using Store = typename TConfig::Storage::Store::PerEdge;

            EdgeData data;
            Store store;
            bool inverted;

            Edge(EdgeData const& data, Store && store)
                : data(data), store(store), inverted(false)
            { }
        };
        struct Path
        {
            using Store = typename TConfig::Storage::Store::PerPath;

            PathData data;
            Store store;

            Path(PathData const& data, Store && store)
                : data(data), store(store)
            { }
        };

        struct Label
        {
            using Store = typename TConfig::Storage::Store::PerLabel;

            LabelData data;
            Store store;

            Label(LabelData const& data, Store && store)
                : data(data), store(store)
            { }
        };
        struct Prop
        {
            using Store = typename TConfig::Storage::Store::PerProp;

            PropData data;
            Store store;

            Prop(PropData const& data, Store && store)
                : data(data), store(store)
            { }
        };

    private:

        typename TConfig::Storage::Store _storage;

    public:

        inline PathPropertyGraph()
        {
            _storage.template initPrimaryNodeStore<Node>();
            _storage.template initPrimaryEdgeStore<Edge>();
            _storage.template initPrimaryPathStore<Path>();
            _storage.template initPrimaryLabelStore<Label>();
            _storage.template initPrimaryPropStore<Prop>();

            // TODO: run storage "config" lambda here
        }
    // Count functions
    public:
        inline size_t nodeCount() const { return _storage.template countPrimaryNodeStore<Node>(); }
        inline size_t edgeCount() const { return _storage.template countPrimaryEdgeStore<Edge>(); }
        inline size_t pathCount() const { return _storage.template countPrimaryPathStore<Path>(); }
        inline size_t labelCount() const { return _storage.template countPrimaryLabelStore<Label>(); }
        inline size_t propCount() const { return _storage.template countPrimaryPropStore<Prop>(); }

    // Introspection functions
    public:
        static inline bool isEdgeInverted(Edge const* edge)
        {
            return edge->inverted;
        }

    // Add functions
    public:
        inline Label* addLabel(LabelData const& data)
        {
            Label* ref = _storage.template makeLabel<Label>(data);

            return ref;
        }

        inline Node* addNode(NodeData const& data)
        {
            Node* ref = _storage.template makeNode<Node>(data);

            return ref;
        }

        inline Node* addNode(NodeData const& data, std::vector<Label const*> const& labels)
        {
            Node* ref = _storage.template makeNode<Node>(data);

            for (auto label : labels)
            {
                _storage.attachNodeLabel(ref, ref->store, label, label->store);
            }

            return ref;
        }
        
        // By default edges point from 0-index to all others
        inline Edge* addEdge(EdgeData const& data, std::vector<Node const*> const& nodes, bool invert = false)
        {
            auto nodes_size = nodes.size();
            if (nodes_size < 2)
                throw graph_error("Edges must connect at least two nodes.");

            Edge* ref = _storage.template makeEdge<Edge>(data);
            ref->inverted = invert;

            _storage.template setEdgeListOfNodes<Edge, Node>(
                ref, ref->store,
                nodes.begin(), nodes.end(), [](Node const* n){ return &n->store; }
            );

            return ref;
        }
        
        inline Prop* addProp(PropData const& data, Node* on_node)
        {
            Prop* ref = _storage.template makeProp<Prop>(data);

            _storage.attachNodeProp(
                on_node, on_node->store,
                ref, ref->store
            );

            return ref;
        }

        inline Prop* addProp(PropData const& data, Edge* on_edge)
        {
            Prop* ref = _storage.template makeProp<Prop>(data);

            _storage.attachEdgeProp(
                on_edge, on_edge->store,
                ref, ref->store
            );

            return ref;
        }

    // Iterate all functions
    public:
        template<typename Func>
        inline void forAllNodes(Func func) const
        {
            for (auto node_it = _storage.template allNodesBegin<Node>(); node_it != _storage.template allNodesEnd<Node>(); ++node_it)
            {
                if (!_detail::invoke_return_bool_or_true(func, (Node const*)&*node_it))
                    break;
            }
        }

        template<typename Func>
        inline void forAllLabels(Func func) const
        {
            for (auto label_it = _storage.template allLabelsBegin<Label>(); label_it != _storage.template allLabelsEnd<Label>(); ++label_it)
            {
                if (!_detail::invoke_return_bool_or_true(func, (Label const*)&*label_it))
                    break;
            }
        }

    // Iterate on functions
    public:
        template<typename Func>
        inline void forLabelsOnNode(Node const* node, Func func) const
        {
            auto list = _storage.template getNodeListOfLabels<Label>(node, node->store);

            for (auto edge_it = list.begin(); edge_it != list.end(); ++edge_it)
            {
                if (!_detail::invoke_return_bool_or_true(func, (Edge const*)*edge_it))
                    break;
            }
        }

        template<typename Func>
        inline void forEdgesOnNode(Node const* node, Func func) const
        {
            auto list = _storage.template getNodeListOfEdges<Edge>(node, node->store);

            for (auto edge_it = list.begin(); edge_it != list.end(); ++edge_it)
            {
                if (!_detail::invoke_return_bool_or_true(func, (Edge const*)*edge_it))
                    break;
            }
        }

        template<typename Func>
        inline void forNodesInEdge(Edge const* edge, Func func) const
        {
            auto list = _storage.template getEdgeListOfNodes<Node>(edge, edge->store);

            for (auto node_it = list.begin(); node_it != list.end(); ++node_it)
            {
                if (!_detail::invoke_return_bool_or_true(func, (Node const*)*node_it))
                    break;
            }
        }

        template<typename Func>
        inline void forPropsOnNode(Node const* node, Func func) const
        {
            auto list = _storage.template getNodeListOfProps<Prop>(node, node->store);

            for (auto prop_it = list.begin(); prop_it != list.end(); ++prop_it)
            {
                if (!_detail::invoke_return_bool_or_true(func, (Prop const*)*prop_it))
                    break;
            }
        }

        template<typename Func>
        inline void forPropsOnEdge(Edge const* edge, Func func) const
        {
            auto list = _storage.template getEdgeListOfProps<Prop>(edge, edge->store);

            for (auto prop_it = list.begin(); prop_it != list.end(); ++prop_it)
            {
                if (!_detail::invoke_return_bool_or_true(func, (Prop const*)*prop_it))
                    break;
            }
        }

    // Attach functions
    public:
        inline void attachLabel(Node* node, Label* label)
        {
            _storage.attachNodeLabel(
                node, node->store,
                label, label->store
            );
        }

        inline void attachEdge(Node* node, Edge* edge)
        {
            auto list = _storage.template getEdgeListOfNodes<Node>(edge, edge->store);

            list.insert(list.end(), node);

            _storage.setEdgeListOfNodes<Edge, Node>(
                edge, edge->store,
                list.begin(), list.end(), [](Node const* n){ return &n->store; }
            );
        }

        inline void attachEdge(Node* node, Edge* edge, size_t index)
        {
            auto list = _storage.template getEdgeListOfNodes<Node>(edge, edge->store);

            if (index > list.size())
                throw graph_error("Argument `index` out of range.");

            list.insert(list.begin() + index, node);
            
            _storage.setEdgeListOfNodes<Edge, Node>(
                edge, edge->store,
                list.begin(), list.end(), [](Node const* n){ return &n->store; }
            );
        }

    // Relationship introspection functions
    public:
        inline int indexOfNodeInEdge(Node const* node, Edge const* edge) const
        {
            auto list = _storage.template getEdgeListOfNodes<Node>(edge, edge->store);
            auto list_begin = list.begin();
            auto list_end = list.end();

            auto it = std::find(list_begin, list_end, node);

            if (it == list_end)
                return -1;
            
            return it - list_begin;
        }

        inline int indexOfNodeInEdgeThrowing(Node const* node, Edge const* edge) const
        {
            auto res = indexOfNodeInEdge(node, edge);
            if (res == -1)
                throw graph_error("Node not in edge relation.");
            
            return res;
        }

    // Detach functions
    public:
        inline void detachLabel(Node* node, Label* label)
        {

        }

        inline void detachEdge(Node* node, Edge* edge)
        {
            auto list = _storage.template getEdgeListOfNodes<Node>(edge, edge->store);

            list.insert(list.end(), node);

            _storage.setEdgeListOfNodes<Edge, Node>(
                edge, edge->store,
                list.begin(), list.end(), [](Node const* n){ return &n->store; }
            );
        }

        inline void detachEdge(Node* node, Edge* edge, size_t index)
        {
            auto list = _storage.template getEdgeListOfNodes<Node>(edge, edge->store);

            if (index > list.size())
                throw graph_error("Argument `index` out of range.");

            list.insert(list.begin() + index, node);
            
            _storage.setEdgeListOfNodes<Edge, Node>(
                edge, edge->store,
                list.begin(), list.end(), [](Node const* n){ return &n->store; }
            );
        }

    // Delete functions
    public:

    };
}}
