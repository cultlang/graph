#pragma once

#include "query.hpp"

#include "pipe_empty.hpp"
#include "pipe_vertex.hpp"
#include "pipe_edge.hpp"
#include "pipe_filter.hpp"
#include "pipe_aggregation.hpp"
#include "pipe_labels.hpp"
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

        TQueryFinal as(std::string const& label)
        {
            return this->addPipe(std::make_unique<GraphQueryPipeLabel<TGraph>>(this->engine()->requireLabel(label)));
        }

        TQueryFinal except(std::string const& label)
        {
            return this->addPipe(std::make_unique<GraphQueryPipeExcept<TGraph>>(this->engine()->requireLabel(label)));
        }

        TQueryFinal back(std::string const& label)
        {
            return this->addPipe(std::make_unique<GraphQueryPipeBack<TGraph>>(this->engine()->requireLabel(label)));
        }

        template<typename TFuncSubQuery>
        TQueryFinal optional(TFuncSubQuery sub_query)
        {
            return this->addPipe(std::make_unique<GraphQueryPipeOptional<TGraph>>(extractPipeline(sub_query(newQueryPipeline()))));
        }
    };
}

