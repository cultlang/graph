/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/. */

#pragma once

#include "graph/filters/filters.hpp"

#include "engine.hpp"
#include "query.hpp"

#include "pipe_empty.hpp"
#include "pipe_vertex.hpp"
#include "pipe_edge.hpp"
#include "pipe_filter.hpp"
#include "pipe_aggregation.hpp"
#include "pipe_markers.hpp"
#include "pipe_subquery.hpp"

namespace graph
{
    template<typename TGraph, typename TQueryFinal>
    class GraphQueryLibraryCore
        : protected GraphQueryLibraryBase<TGraph, TQueryFinal>
    {
    public:
        template<typename ...TArgs>
        TQueryFinal v(TArgs... args)
        {
            return this->addPipe(std::make_unique<GraphQueryPipeVertex<TGraph>>(std::forward<TArgs>(args)...));
        }

        template<typename TFuncEdges, typename TFuncEdgeNodes>
        TQueryFinal e(TFuncEdges func_edges, TFuncEdgeNodes func_edgeNodes)
        {
            typedef typename filter::specialize_filter<TGraph, std::decay_t<TFuncEdges>> FuncEdgesSpecializer;
            typedef typename filter::specialize_filter<TGraph, std::decay_t<TFuncEdgeNodes>> FuncEdgeNodesSpecializer;
            return this->addPipe(
                std::make_unique<GraphQueryPipeEdges<
                    TGraph,
                    GraphQueryPipeEdgesEnum::All,
                    typename FuncEdgesSpecializer::type,
                    typename FuncEdgeNodesSpecializer::type
                >>(
                    FuncEdgesSpecializer::specialize(std::forward<TFuncEdges>(func_edges)),
                    FuncEdgeNodesSpecializer::specialize(std::forward<TFuncEdgeNodes>(func_edgeNodes))
                ));
        }
        template<typename TFuncEdges>
        TQueryFinal e(TFuncEdges func_edges)
        {
            return e(func_edges, filter::constant<true>());
        }

        template<typename TFuncEdges, typename TFuncEdgeNodes>
        TQueryFinal in(TFuncEdges func_edges, TFuncEdgeNodes func_edgeNodes)
        {
            typedef typename filter::specialize_filter<TGraph, std::decay_t<TFuncEdges>> FuncEdgesSpecializer;
            typedef typename filter::specialize_filter<TGraph, std::decay_t<TFuncEdgeNodes>> FuncEdgeNodesSpecializer;
            return this->addPipe(
                std::make_unique<GraphQueryPipeEdges<
                    TGraph,
                    GraphQueryPipeEdgesEnum::Incoming,
                    typename FuncEdgesSpecializer::type,
                    typename FuncEdgeNodesSpecializer::type
                >>(
                    FuncEdgesSpecializer::specialize(std::forward<TFuncEdges>(func_edges)),
                    FuncEdgeNodesSpecializer::specialize(std::forward<TFuncEdgeNodes>(func_edgeNodes))
                ));
        }
        template<typename TFuncEdges>
        TQueryFinal in(TFuncEdges func_edges)
        {
            return in(func_edges, filter::constant<true>());
        }

        template<typename TFuncEdges, typename TFuncEdgeNodes>
        TQueryFinal out(TFuncEdges func_edges, TFuncEdgeNodes func_edgeNodes)
        {
            typedef typename filter::specialize_filter<TGraph, std::decay_t<TFuncEdges>> FuncEdgesSpecializer;
            typedef typename filter::specialize_filter<TGraph, std::decay_t<TFuncEdgeNodes>> FuncEdgeNodesSpecializer;
            return this->addPipe(
                std::make_unique<GraphQueryPipeEdges<
                    TGraph,
                    GraphQueryPipeEdgesEnum::Outgoing,
                    typename FuncEdgesSpecializer::type,
                    typename FuncEdgeNodesSpecializer::type
                >>(
                    FuncEdgesSpecializer::specialize(std::forward<TFuncEdges>(func_edges)),
                    FuncEdgeNodesSpecializer::specialize(std::forward<TFuncEdgeNodes>(func_edgeNodes))
                ));
        }
        template<typename TFuncEdges>
        TQueryFinal out(TFuncEdges func_edges)
        {
            return out(func_edges, filter::constant<true>());
        }

        template<typename TFuncNodes>
        TQueryFinal filter(TFuncNodes func_nodes)
        {
            typedef typename filter::specialize_filter<TGraph, std::decay_t<TFuncNodes>> FuncNodesSpecializer;
            return this->addPipe(
                std::make_unique<GraphQueryPipeFilter<
                    TGraph,
                    typename FuncNodesSpecializer::type
                >>(
                    FuncNodesSpecializer::specialize(std::forward<TFuncNodes>(func_nodes))
                ));
        }
        
        TQueryFinal unique()
        {
            return this->addPipe(std::make_unique<GraphQueryPipeUnique<TGraph>>());
        }
        
        TQueryFinal take(size_t limit)
        {
            return this->addPipe(std::make_unique<GraphQueryPipeLimit<TGraph>>(limit));
        }

        TQueryFinal as(std::string const& marker)
        {
            return this->addPipe(
                std::make_unique<GraphQueryPipeMark<TGraph>>(
                    this->engine()->requireMarker(marker)
                ));
        }

        TQueryFinal merge(std::vector<std::string> const& markers)
        {
            std::vector<size_t> marker_values;
            marker_values.resize(markers.size());
            auto& engine = this->engine();
            std::transform(markers.begin(), markers.end(), marker_values.begin(),
                [&](auto s) { return engine->requireMarker(s); });

            return this->addPipe(std::make_unique<GraphQueryPipeMerge<TGraph>>(marker_values));
        }

        TQueryFinal except(std::string const& marker)
        {
            return this->addPipe(std::make_unique<GraphQueryPipeExcept<TGraph>>(this->engine()->requireMarker(marker)));
        }

        TQueryFinal back(std::string const& marker)
        {
            return this->addPipe(std::make_unique<GraphQueryPipeBack<TGraph>>(this->engine()->requireMarker(marker)));
        }

        template<typename TFuncSubQuery>
        TQueryFinal optional(TFuncSubQuery sub_query)
        {
            return this->addPipe(std::make_unique<GraphQueryPipeOptional<TGraph>>(this->extractPipeline(sub_query(this->newQueryPipeline()))));
        }

        template<typename TFuncSubQuery, typename TFuncRepeatFilter>
        TQueryFinal repeat_breadth(TFuncSubQuery sub_query, TFuncRepeatFilter repeat)
        {
            return this->addPipe(
                std::make_unique<GraphQueryPipeRepeatBreadthFirst<
                    TGraph,
                    TFuncRepeatFilter
                >>(
                    this->extractPipeline(sub_query(this->newQueryPipeline())),
                    repeat
                ));
        }

        template<typename TFuncSubQuery, typename TFuncRepeatFilter, typename TFuncEmitFilter>
        TQueryFinal repeat_breadth(TFuncSubQuery sub_query, TFuncRepeatFilter repeat, TFuncEmitFilter emit)
        {
            return this->addPipe(
                std::make_unique<GraphQueryPipeRepeatBreadthFirst<
                    TGraph,
                    TFuncRepeatFilter,
                    TFuncEmitFilter
                >>(
                    this->extractPipeline(sub_query(this->newQueryPipeline())),
                    repeat,
                    emit
                ));
        }

        template<typename TFuncSubQuery, typename TFuncRepeatFilter>
        TQueryFinal repeat_depth(TFuncSubQuery sub_query, TFuncRepeatFilter repeat)
        {
            return this->addPipe(
                std::make_unique<GraphQueryPipeRepeatDepthFirst<
                    TGraph,
                    TFuncRepeatFilter
                >>(
                    this->extractPipeline(sub_query(this->newQueryPipeline())),
                    repeat
                ));
        }

        template<typename TFuncSubQuery, typename TFuncRepeatFilter, typename TFuncEmitFilter>
        TQueryFinal repeat_depth(TFuncSubQuery sub_query, TFuncRepeatFilter repeat, TFuncEmitFilter emit)
        {
            return this->addPipe(
                std::make_unique<GraphQueryPipeRepeatDepthFirst<
                    TGraph,
                    TFuncRepeatFilter,
                    TFuncEmitFilter
                >>(
                    this->extractPipeline(sub_query(this->newQueryPipeline())),
                    repeat,
                    emit
                ));
        }
    };
}

