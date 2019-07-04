/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/. */

#pragma once

#include <vector>
#include <deque>
#include <string>
#include "../util.hpp"

namespace graph
{
    struct void_t
    {
    };

    template<typename TCoreData, typename TBase = void_t>
    struct basic_core_config : public TBase
    {
        using CoreData = TCoreData;

        using LabelData = void_t;
        using NodeData = void_t;
        using EdgeData = void_t;
        using PropData = void_t;

        // support plf colony?
        template<typename T>
        using Storage = std::deque<T>;
    };

    // TODO, another graph type that is more effeciently organized
    //   maybe by not supporting parent overrides
    template <typename TConfig>
    class GraphCore
    {
    // This type types:
    public:
        // TODO move value kind out of this
        // TODO a way for this to be used by wrapping templates
        enum class MetaFlags : uint32_t
        {
            None = 0,

            Mask_Kind = (4 - 1) << 1,
            Value_Kind_Label = ((uint8_t)GraphKind::Label - 1) << 1,
            Value_Kind_Node = ((uint8_t)GraphKind::Node - 1) << 1,
            Value_Kind_Edge = ((uint8_t)GraphKind::Edge - 1) << 1,
            Value_Kind_Prop = ((uint8_t)GraphKind::Prop - 1) << 1,

            Flag_InverseEdge = 1 << 3,
            //Flag_Override = 1 << 4,
        };

    // Graph types:
    public:
        using Config = TConfig;
        
        using CoreData = typename Config::CoreData;

        using LabelData = typename Config::LabelData;
        using NodeData = typename Config::NodeData;
        using EdgeData = typename Config::EdgeData;
        using PropData = typename Config::PropData;

        template<typename T>
        using Storage = typename Config::template Storage<T>;

        struct Core
        {
        public:
            CoreData data;
            MetaFlags flags;
        };

        struct Label;
        struct Node;
        struct Edge;
        struct Prop;

        // A graph "label" metadata
        struct Label
        {
        public:
            LabelData label_data;

            std::vector<Node*> nodes;
        };

        // A graph "node" metadata
        struct Node
        {
        public:
            NodeData node_data;

            std::vector<Label*> labels;
            std::vector<Edge*> edges;
            std::vector<Prop*> props;
        };

        // A graph "edge" metadata
        struct Edge
        {
        public:
            EdgeData edge_data;

            std::vector<Node*> nodes;
            std::vector<Prop*> props;
        };

        // A graph "prop" metadata
        struct Prop
        {
        public:
            PropData prop_data;

            Core* owner;
        };

    // Actual type
    public:
        template<typename TFinal>
        class Actual
        {
        public:
            using MetaFlags = typename TFinal::MetaFlags;
            using CoreData = typename TFinal::CoreData;

            using Core = typename TFinal::Core;
            using Label = typename TFinal::Label;
            using Node = typename TFinal::Node;
            using Edge = typename TFinal::Edge;
            using Prop = typename TFinal::Prop;

        protected:
            using LabelStorage = typename TFinal::template Storage<Label>;
            using NodeStorage = typename TFinal::template Storage<Node>;
            using EdgeStorage = typename TFinal::template Storage<Edge>;
            using PropStorage = typename TFinal::template Storage<Prop>;

            LabelStorage _labels;
            NodeStorage _nodes;
            EdgeStorage _edges;
            PropStorage _props;

        // Add functions
        public:
            inline Label* addLabel(CoreData const& data)
            {
                Label& ref = _labels.emplace_back();
                ref.data = data;
                ref.flags = MetaFlags::Value_Kind_Label;

                return &ref;
            }

            inline Node* addNode(CoreData const& data)
            {
                Node& ref = _nodes.emplace_back();
                ref.data = data;
                ref.flags = MetaFlags::Value_Kind_Node;

                return &ref;
            }
            
            // By default edges point from 0-index to all others
            inline Edge* addEdge(CoreData const& data, std::vector<Node const*> const& nodes, bool invert = false)
            {
                auto nodes_size = nodes.size();
                if (nodes_size < 2)
                    throw graph_error("Edges must connect at least two nodes.");

                Edge& ref = _edges.emplace_back();
                ref.data = data;
                ref.flags = MetaFlags::Value_Kind_Edge;
                if (invert) ref.flags = (MetaFlags)((uint64_t)ref.flags | (uint64_t)MetaFlags::Flag_InverseEdge);

                ref.nodes.reserve(nodes_size);
                for (auto node : nodes)
                {
                    auto node_actual = const_cast<Node*>(node);
                    ref.nodes.push_back(node_actual);
                    node_actual->edges.push_back(&ref);
                }

                return &ref;
            }
            
            inline Prop* addProp(CoreData const& data, Node* on_node)
            {
                Prop& ref = _props.emplace_back();
                ref.data = data;
                ref.flags = MetaFlags::Value_Kind_Prop;

                on_node->props.push_back(&ref);
                ref.owner = on_node;

                return &ref;
            }

            inline Prop* addProp(CoreData const& data, Edge* on_edge)
            {
                Prop& ref = _props.emplace_back();
                ref.data = data;
                ref.flags = MetaFlags::Value_Kind_Prop;

                on_edge->props.push_back(&ref);
                ref.owner = on_edge;

                return &ref;
            }

        // Update functions
        public:
            inline void attachLabel(Node* node, Label* label)
            {
                label->nodes.push_back(node);
                node->labels.push_back(label);
            }

            inline void attachEdge(Node* node, Edge* edge)
            {
                edge->nodes.push_back(node);
                node->edges.push_back(edge);
            }

            inline void attachEdge(Node* node, Edge* edge, size_t index)
            {
                if (index > edge->nodes.size())
                    throw graph_error("Argument `index` out of range.");

                edge->nodes.insert(edge->nodes.begin() + index, node);
                node->edges.push_back(edge);
            }

        // Printer functions
        public:
            inline std::string print(Core*) const
            {

            }

        // Stat functions
        public:
            inline size_t labelCount() const
            {
                return _labels.size();
            }
            inline size_t nodeCount() const
            {
                return _nodes.size();
            }
            inline size_t edgeCount() const
            {
                return _edges.size();
            }
            inline size_t propCount() const
            {
                return _props.size();
            }

        // Introspection functions
        public:
            
            static inline bool isLabel(Core const* core)
            {
                return ((uint32_t)core->flags & (uint32_t)MetaFlags::Mask_Kind) == (uint32_t)MetaFlags::Value_Kind_Label;
            }
            static inline bool isNode(Core const* core)
            {
                return ((uint32_t)core->flags & (uint32_t)MetaFlags::Mask_Kind) == (uint32_t)MetaFlags::Value_Kind_Node;
            }
            static inline bool isEdge(Core const* core)
            {
                return ((uint32_t)core->flags & (uint32_t)MetaFlags::Mask_Kind) == (uint32_t)MetaFlags::Value_Kind_Edge;
            }
            static inline bool isProp(Core const* core)
            {
                return ((uint32_t)core->flags & (uint32_t)MetaFlags::Mask_Kind) == (uint32_t)MetaFlags::Value_Kind_Prop;
            }

            static inline bool isEdgeInverted(Edge const* edge)
            {
                return ((uint32_t)edge->flags & (uint32_t)MetaFlags::Flag_InverseEdge) != 0;
            }

        // Getter functions
        public:
            inline std::vector<Node*> getLabelNodes(Label const* label) const
            {
                return label->nodes;
            }

            inline std::vector<Label*> getNodeLabels(Node const* node) const
            {
                return node->labels;
            }
            inline std::vector<Edge*> getNodeEdges(Node const* node) const
            {
                return node->edges;
            }
            inline std::vector<Prop*> getNodeProps(Node const* node) const
            {
                return node->props;
            }

            inline std::vector<Node*> getEdgeNodes(Edge const* edge) const
            {
                return edge->nodes;
            }
            inline std::vector<Prop*> getEdgeProps(Edge const* edge) const
            {
                return edge->props;
            }

        // Iteration functions
        public:
            template<typename Func>
            inline void forAllLabels(Func func) const
            {
                for (auto label_it = _labels.begin(); label_it != _labels.end(); ++label_it)
                {
                    if (!_detail::invoke_return_bool_or_true(func, (Label const*)&*label_it))
                        break;
                }
            }

            template<typename Func>
            inline void forAllNodes(Func func) const
            {
                for (auto node_it = _nodes.begin(); node_it != _nodes.end(); ++node_it)
                {
                    if (!_detail::invoke_return_bool_or_true(func, (Node const*)&*node_it))
                        break;
                }
            }

            template<typename Func>
            inline void forAllEdges(Func func) const
            {
                for (auto edge_it = _edges.begin(); edge_it != _edges.end(); ++edge_it)
                {
                    if (!_detail::invoke_return_bool_or_true(func, (Edge const*)*edge_it))
                        break;
                }
            }

            template<typename Func>
            inline void forAllNodesInLabel(Label const* label, Func func) const
            {
                for (auto node_it = label->nodes.begin(); node_it != label->nodes.end(); ++node_it)
                {
                    if (!_detail::invoke_return_bool_or_true(func, (Node const*)*node_it))
                        break;
                }
            }

            template<typename Func>
            inline void forAllLabelsOnNode(Node const* node, Func func) const
            {
                for (auto label_it = node->labels.begin(); label_it != node->labels.end(); ++label_it)
                {
                    if (!_detail::invoke_return_bool_or_true(func, (Label const*)*label_it))
                        break;
                }
            }

            template<typename Func>
            inline void forAllEdgesOnNode(Node const* node, Func func) const
            {
                for (auto edge_it = node->edges.begin(); edge_it != node->edges.end(); ++edge_it)
                {
                    if (!_detail::invoke_return_bool_or_true(func, (Edge const*)*edge_it))
                        break;
                }
            }

            template<typename Func>
            inline void forAllPropsOnNode(Node const* node, Func func) const
            {
                for (auto prop_it = node->props.begin(); prop_it != node->props.end(); ++prop_it)
                {
                    if (!_detail::invoke_return_bool_or_true(func, (Prop const*)*prop_it))
                        break;
                }
            }

            template<typename Func>
            inline void forAllNodesInEdge(Edge const* edge, Func func) const
            {
                for (auto node_it = edge->nodes.begin(); node_it != edge->nodes.end(); ++node_it)
                {
                    if (!_detail::invoke_return_bool_or_true(func, (Node const*)*node_it))
                        break;
                }
            }
            
            template<typename Func>
            inline void forAllPropsOnEdge(Edge const* edge, Func func) const
            {
                for (auto prop_it = edge->props.begin(); prop_it != edge->props.end(); ++prop_it)
                {
                    if (!_detail::invoke_return_bool_or_true(func, (Prop const*)*prop_it))
                        break;
                }
            }
        };
    };
}