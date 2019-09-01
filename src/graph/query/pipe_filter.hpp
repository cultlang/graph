/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/. */

#pragma once

#include <memory>
#include <type_traits>

#include "engine.hpp"

namespace graph
{

	/******************************************************************************
	** GraphQueryPipeFilter
	******************************************************************************/

    template<typename TGraph, typename TFuncNodes>
    class GraphQueryPipeFilter
        : public GraphQueryEngine<TGraph>::Pipe
    {
    private:
        using Query = GraphQueryEngine<TGraph>;

    // config
    protected:
        TFuncNodes _func_nodes;

    // state
    protected:

    public:
        inline GraphQueryPipeFilter(TFuncNodes const& func_nodes)
            : _func_nodes(func_nodes)
        { }

        inline GraphQueryPipeFilter(GraphQueryPipeFilter const&) = default;
        inline GraphQueryPipeFilter(GraphQueryPipeFilter &&) = default;

        inline ~GraphQueryPipeFilter() = default;

    protected:
        inline virtual void cleanup() override { };

        inline virtual typename Query::PipeResult pipeFunc(
            TGraph const* graph,
            std::shared_ptr<typename Query::Gremlin> const& gremlin
        ) override
        {
            if (!gremlin)
                return Query::PipeResultEnum::Pull;

            bool filter_accepted;
            if constexpr (std::is_invocable_v<TFuncNodes, decltype(gremlin->node())>)
                filter_accepted = _func_nodes(gremlin->node());
            else if constexpr (std::is_invocable_v<TFuncNodes, decltype(gremlin->node()), decltype(gremlin)>)
                filter_accepted = _func_nodes(gremlin->node(), gremlin);
            else
                static_assert(stdext::always_false, "TFuncNodes bad signature.");

            if (filter_accepted)
                return gremlin;
            else
                return Query::PipeResultEnum::Pull;
        }
    };

}