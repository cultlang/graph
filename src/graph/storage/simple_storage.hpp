/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/. */

#pragma once

#include <vector>
#include <deque>
#include <string>

#include "graph/util.hpp"

namespace ugly {
namespace storage
{
    class SimpleStorage
    {
    public:
        struct PerNode
        {
            std::vector<void*> _props;

            std::vector<void*> _edges;
        };

        struct PerEdge
        {
            std::vector<void*> _props;

            std::vector<void*> _nodes;
        };

        struct PerPath
        {

        };

        struct PerLabel
        {

        };

        struct PerProp
        {
            void* _parent;

        };

    protected:
        struct _PrimaryStore
        {
            void* _store;
            size_t _storeElemSize;

            inline _PrimaryStore()
                : _store(nullptr)
            {

            }

            template<typename T>
            inline void init()
            {
                assert(_store == nullptr);

                _store = new std::deque<T>();
                _storeElemSize = sizeof(T);
            }

            template<typename T>
            inline std::deque<T>* get()
            {
                assert(sizeof(T) == _storeElemSize);
                return (std::deque<T>*)_store;
            }

            template<typename T>
            inline std::deque<T> const* get() const
            {
                assert(sizeof(T) == _storeElemSize);
                return (std::deque<T> const*)_store;
            }
        };

    private:
        _PrimaryStore _nodes;
        _PrimaryStore _edges;
        _PrimaryStore _paths;
        _PrimaryStore _labels;
        _PrimaryStore _props;

    // initPrimary*Store
    public:
        template<typename Node>
        inline void initPrimaryNodeStore()
        {
            _nodes.init<Node>();
        }
        template<typename Edge>
        inline void initPrimaryEdgeStore()
        {
            _edges.init<Edge>();
        }
        template<typename Path>
        inline void initPrimaryPathStore()
        {
            _paths.init<Path>();
        }
        template<typename Label>
        inline void initPrimaryLabelStore()
        {
            _labels.init<Label>();
        }
        template<typename Prop>
        inline void initPrimaryPropStore()
        {
            _props.init<Prop>();
        }

    // countPrimary*Store
    public:
        template<typename Node>
        inline size_t countPrimaryNodeStore() const
        {
            return _nodes.get<Node>()->size();
        }
        template<typename Edge>
        inline size_t countPrimaryEdgeStore() const
        {
            return _edges.get<Edge>()->size();
        }
        template<typename Path>
        inline size_t countPrimaryPathStore() const
        {
            return _paths.get<Path>()->size();
        }
        template<typename Label>
        inline size_t countPrimaryLabelStore() const
        {
            return _labels.get<Label>()->size();
        }
        template<typename Prop>
        inline size_t countPrimaryPropStore() const
        {
            return _props.get<Prop>()->size();
        }


    // make*
    public:
        template<typename Node, typename Data>
        inline Node* makeNode(Data const& data)
        {
            Node& ref = _nodes.get<Node>()->emplace_back(data, PerNode());

            return &ref;
        }
        template<typename Edge, typename Data>
        inline Edge* makeEdge(Data const& data)
        {
            Edge& ref = _edges.get<Edge>()->emplace_back(data, PerEdge());

            return &ref;
        }
        template<typename Path, typename Data>
        inline Path* makePath(Data const& data)
        {
            Path& ref = _paths.get<Path>()->emplace_back(data, PerPath());

            return &ref;
        }
        template<typename Label, typename Data>
        inline Label* makeLabel(Data const& data)
        {
            Label& ref = _labels.get<Label>()->emplace_back(data, PerLabel());

            return &ref;
        }
        template<typename Prop, typename Data>
        inline Prop* makeProp(Data const& data)
        {
            Prop& ref = _props.get<Prop>()->emplace_back(data, PerProp());

            return &ref;
        }

    // all*{Begin/End}
    public:
        template<typename Node>
        inline typename std::deque<Node>::const_iterator allNodesBegin() const
        {
            return _nodes.get<Node>()->begin();
        }
        template<typename Node>
        inline typename std::deque<Node>::const_iterator allNodesEnd() const
        {
            return _nodes.get<Node>()->end();
        }

        template<typename Label>
        inline typename std::deque<Label>::const_iterator allLabelsBegin() const
        {
            return _labels.get<Label>()->begin();
        }
        template<typename Label>
        inline typename std::deque<Label>::const_iterator allLabelsEnd() const
        {
            return _labels.get<Label>()->end();
        }

    // relations getting
    public:
        template<typename Node, typename Edge>
        inline std::vector<Node const*> getEdgeListOfNodes(Edge const* ref, PerEdge const& per) const
        {
            std::vector<Node const*> list;
            list.reserve(per._nodes.size());

            std::transform(per._nodes.begin(), per._nodes.end(), std::back_inserter(list),
                [](void* p) { return (Node const*)p; });

            return list;
        }

        template<typename Edge, typename Node>
        inline std::vector<Edge const*> getNodeListOfEdges(Node const* ref, PerNode const& per) const
        {
            std::vector<Edge const*> list;
            list.reserve(per._edges.size());

            std::transform(per._edges.begin(), per._edges.end(), std::back_inserter(list),
                [](void* p) { return (Edge const*)p; });

            return list;
        }

        template<typename Prop, typename Node>
        inline std::vector<Prop const*> getNodeListOfProps(Node const* ref, PerNode const& per) const
        {
            std::vector<Prop const*> list;
            list.reserve(per._props.size());

            std::transform(per._props.begin(), per._props.end(), std::back_inserter(list),
                [](void* p) { return (Prop const*)p; });

            return list;
        }

        template<typename Prop, typename Edge>
        inline std::vector<Prop const*> getEdgeListOfProps(Edge const* ref, PerEdge const& per) const
        {
            std::vector<Prop const*> list;
            list.reserve(per._props.size());

            std::transform(per._props.begin(), per._props.end(), std::back_inserter(list),
                [](void* p) { return (Prop const*)p; });

            return list;
        }

    // relations setting
    public:
        template<typename Edge, typename NodeIt, typename NodeGetter>
        inline void setEdgeListOfNodes(Edge const* edge, PerEdge const& edge_per, NodeIt begin, NodeIt end, NodeGetter getter)
        {
            // TODO lock

            auto& edge_per_mut = const_cast<PerEdge&>(edge_per);
            // TODO remove from old nodes
            edge_per_mut._nodes.clear();
            for (auto it = begin; it != end; ++it)
            {
                auto const* node = *it;
                auto& node_per_mut = const_cast<PerNode&>(*getter(node));

                node_per_mut._edges.push_back((void*)edge);
                edge_per_mut._nodes.push_back((void*)node);
            }

        }

        template<typename Node, typename Prop>
        inline void attachNodeProp(Node const* node, PerNode const& node_per, Prop const* prop, PerProp const& prop_per)
        {
            // TODO lock
            auto& node_per_mut = const_cast<PerNode&>(node_per);
            auto& prop_per_mut = const_cast<PerProp&>(prop_per);

            if (prop_per_mut._parent != nullptr)
            {
                // TODO remove from old parent
            }

            node_per_mut._props.push_back((void*)prop);
            prop_per_mut._parent = (void*)node;
        }

        template<typename Edge, typename Prop>
        inline void attachEdgeProp(Edge const* edge, PerEdge const& edge_per, Prop const* prop, PerProp const& prop_per)
        {
            // TODO lock
            auto& edge_per_mut = const_cast<PerEdge&>(edge_per);
            auto& prop_per_mut = const_cast<PerProp&>(prop_per);

            if (prop_per_mut._parent != nullptr)
            {
                // TODO remove from old parent
            }

            edge_per_mut._props.push_back((void*)prop);
            prop_per_mut._parent = (void*)edge;
        }

        template<typename Node, typename Label>
        inline void attachNodeLabel(Node const* ref, PerNode const& per, Label const* label_ref, PerLabel const& label_per)
        {
            
        }
    };
}}
