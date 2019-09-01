/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/. */

#pragma once

#include <memory>
#include <vector>
#include <type_traits>

#include "engine.hpp"

namespace graph
{

    enum class GraphQueryPipeEdgesEnum
    {
        All,
        Incoming,
        Outgoing
    };

	/******************************************************************************
	** GraphQueryPipeEdges
	******************************************************************************/

    template<typename TGraph, GraphQueryPipeEdgesEnum EMode, typename TFuncEdges, typename TFuncEdgeNodes>
    class GraphQueryPipeEdges
        : public GraphQueryEngine<TGraph>::Pipe
    {
    private:
        using Query = GraphQueryEngine<TGraph>;

    // config
    protected:
        TFuncEdges _func_edges;
        TFuncEdgeNodes _func_edgeNodes;

    // state
    protected:
        std::shared_ptr<typename Query::Gremlin> _gremlin;
        std::vector<typename TGraph::Edge const*> _edges;
        typename decltype(_edges)::const_iterator _edges_it;
        std::vector<typename TGraph::Node const*> _nodes;
        typename decltype(_nodes)::const_iterator _nodes_it;

    public:
        inline GraphQueryPipeEdges(TFuncEdges const& func_edges, TFuncEdgeNodes const& func_edgeNodes)
            : _func_edges(func_edges)
            , _func_edgeNodes(func_edgeNodes)
            , _gremlin()
            , _edges()
            , _edges_it(_edges.end())
            , _nodes()
            , _nodes_it(_nodes.end())
        { }

        inline GraphQueryPipeEdges(GraphQueryPipeEdges const& that)
            : GraphQueryPipeEdges(_func_edges, _func_edgeNodes)
        { }

        inline ~GraphQueryPipeEdges() = default;

    protected:
        virtual typename Query::PipeState* init() const override
        {
            auto res = new GraphQueryPipeEdges(*this);

            return res;
        };

        static inline bool _modeFilterEdges(typename TGraph::Node const* n, typename TGraph::Edge const* e)
        {
            if constexpr (EMode == GraphQueryPipeEdgesEnum::All)
                return true;
            else if constexpr (EMode == GraphQueryPipeEdgesEnum::Incoming)
                return edgeIsIncoming<TGraph>(n, e);
            else if constexpr (EMode == GraphQueryPipeEdgesEnum::Outgoing)
                return edgeIsOutgoing<TGraph>(n, e);
            else
                static_assert(stdext::always_false, "Bad EMode");
            
        }

        static inline bool _modeFilterEdgeNodes(typename TGraph::Node const* n, typename TGraph::Edge const* e, typename TGraph::Node const* en)
        {
            if constexpr (EMode == GraphQueryPipeEdgesEnum::All)
                return n != en;
            else if constexpr (EMode == GraphQueryPipeEdgesEnum::Incoming)
                return !edgeIsIncoming<TGraph>(en, e);
            else if constexpr (EMode == GraphQueryPipeEdgesEnum::Outgoing)
                return !edgeIsOutgoing<TGraph>(en, e);
            else
                static_assert(stdext::always_false, "Bad EMode");
            
        }

        inline bool _call_func_edges(typename TGraph::Node const* n, typename TGraph::Edge const* e)
        {
            if (!_modeFilterEdges(n, e))
                return false;

            if constexpr (std::is_invocable_v<TFuncEdges, typename TGraph::Edge*>)
                return _func_edges(e);
            else if constexpr (std::is_invocable_v<TFuncEdges, typename TGraph::Node*, typename TGraph::Edge*>)
                return _func_edges(n, e);
            else
                static_assert(stdext::always_false, "TFuncNodes bad signature.");
        }

        inline bool _call_func_edgeNodes(typename TGraph::Node const* n, typename TGraph::Edge const* e, typename TGraph::Node const* en)
        {
            if (!_modeFilterEdgeNodes(n, e, en))
                return false;

            if constexpr (std::is_invocable_v<TFuncEdgeNodes, typename TGraph::Node*>)
                return _func_edgeNodes(en);
            else if constexpr (std::is_invocable_v<TFuncEdgeNodes,  typename TGraph::Edge*, typename TGraph::Node*>)
                return _func_edgeNodes(e, en);
            else if constexpr (std::is_invocable_v<TFuncEdgeNodes, typename TGraph::Node*, typename TGraph::Edge*, typename TGraph::Node*>)
                return _func_edgeNodes(n, e, en);
            else
                static_assert(stdext::always_false, "TFuncEdgeNodes bad signature.");
        }

        inline virtual typename Query::PipeResult pipeFunc(
            TGraph const* graph,
            std::shared_ptr<typename Query::Gremlin> const& gremlin
        ) override
        {
            auto empty = _edges_it == _edges.end() && _nodes_it == _nodes.end();
            if (!gremlin && empty)
                return Query::PipeResultEnum::Pull;

            // get a collection of edges on this node
            if (empty) 
            {
                _gremlin = gremlin;

                auto n = _gremlin->node();
                _edges = collectEdges(*graph, n,
                    [&](auto e) { return _call_func_edges(n, e); });
                _edges_it = _edges.begin();
                _nodes_it = _nodes.end();
            }

            // pop to a new edge
            while (_nodes_it == _nodes.end()) 
            {
                if (_edges_it == _edges.end())
                    return Query::PipeResultEnum::Pull;

                // TODO: simplyfy this, we know which nodes on the edge are the ones we want to
                //   traverse, we may want to revert our iterator paradigm too.
                auto n = _gremlin->node();
                auto e = (typename TGraph::Edge const*) *(_edges_it ++);
                _nodes = collectNodes(*graph, e, 
                    [&](auto en) { return _call_func_edgeNodes(n, e, en); });
                _nodes_it = _nodes.begin();
            }

            return GraphQueryEngine<TGraph>::gotoVertex(_gremlin, (typename TGraph::Node const*) *(_nodes_it ++));
        }
    };
    
}