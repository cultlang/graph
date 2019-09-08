/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/. */

#pragma once

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

        template<typename TFuncEdges>
        TQueryFinal e(TFuncEdges func_edges)
        {
            auto edgeNodeFunc = [](auto n, auto e, auto ne){ return true; };
            return this->addPipe(std::make_unique<GraphQueryPipeEdges<TGraph, GraphQueryPipeEdgesEnum::All, TFuncEdges, decltype(edgeNodeFunc)>>(std::forward<TFuncEdges>(func_edges), edgeNodeFunc));
        }
        template<typename TFuncEdges, typename TFuncEdgeNodes>
        TQueryFinal e(TFuncEdges func_edges, TFuncEdgeNodes func_edgeNodes)
        {
            return this->addPipe(std::make_unique<GraphQueryPipeEdges<TGraph, GraphQueryPipeEdgesEnum::All, TFuncEdges, TFuncEdgeNodes>>(std::forward<TFuncEdges>(func_edges), std::forward<TFuncEdgeNodes>(func_edgeNodes)));
        }

        template<typename TFuncEdges>
        TQueryFinal in(TFuncEdges func_edges)
        {
            auto inEdgeNodeFunc = [](auto n, auto e, auto ne){ return true; };
            return this->addPipe(std::make_unique<GraphQueryPipeEdges<TGraph, GraphQueryPipeEdgesEnum::Incoming, TFuncEdges, decltype(inEdgeNodeFunc)>>(std::forward<TFuncEdges>(func_edges), inEdgeNodeFunc));
        }
        template<typename TFuncEdges, typename TFuncEdgeNodes>
        TQueryFinal in(TFuncEdges func_edges, TFuncEdgeNodes func_edgeNodes)
        {
            return this->addPipe(std::make_unique<GraphQueryPipeEdges<TGraph, GraphQueryPipeEdgesEnum::Incoming, TFuncEdges, TFuncEdgeNodes>>(std::forward<TFuncEdges>(func_edges), std::forward<TFuncEdgeNodes>(func_edgeNodes)));
        }

        template<typename TFuncEdges>
        TQueryFinal out(TFuncEdges func_edges)
        {
            auto outEdgeNodeFunc = [](auto n, auto e, auto ne){ return true; };
            return this->addPipe(std::make_unique<GraphQueryPipeEdges<TGraph, GraphQueryPipeEdgesEnum::Outgoing, TFuncEdges, decltype(outEdgeNodeFunc)>>(std::forward<TFuncEdges>(func_edges), outEdgeNodeFunc));
        }
        template<typename TFuncEdges, typename TFuncEdgeNodes>
        TQueryFinal out(TFuncEdges func_edges, TFuncEdgeNodes func_edgeNodes)
        {
            return this->addPipe(std::make_unique<GraphQueryPipeEdges<TGraph, GraphQueryPipeEdgesEnum::Outgoing, TFuncEdges, TFuncEdgeNodes>>(std::forward<TFuncEdges>(func_edges), std::forward<TFuncEdgeNodes>(func_edgeNodes)));
        }

        template<typename TFuncNodes>
        TQueryFinal filter(TFuncNodes func_nodes)
        {
            return this->addPipe(std::make_unique<GraphQueryPipeFilter<TGraph, TFuncNodes>>(std::forward<TFuncNodes>(func_nodes)));
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
            return this->addPipe(std::make_unique<GraphQueryPipeMark<TGraph>>(this->engine()->requireMarker(marker)));
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

        template<typename TFuncSubQuery>
        TQueryFinal repeat_times(TFuncSubQuery sub_query, size_t times)
        {
            auto pipe_desc = this->extractPipeline(sub_query(this->newQueryPipeline()));
            for (int i = 0; i < times; ++i)
            {
                this->appendPipes(pipe_desc);
            }

            return std::move(*(TQueryFinal*)this);
        }

        template<typename TFuncSubQuery, typename TFuncUntil>
        TQueryFinal repeat_until(TFuncSubQuery sub_query, TFuncUntil until)
        {
            return this->addPipe(std::make_unique<GraphQueryPipeRepeatUntil<TGraph, TFuncUntil>>(this->extractPipeline(sub_query(this->newQueryPipeline())), until));
        }
    };
}

