/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/. */

#pragma once

#include <memory>

#include "engine.hpp"

namespace graph
{

	/******************************************************************************
	** GraphQueryPipeEmpty
	******************************************************************************/

    template<typename TGraph>
    class GraphQueryPipeEmpty
        : public GraphQueryEngine<TGraph>::Pipe
    {
    private:
        using Query = GraphQueryEngine<TGraph>;

    public:
        inline GraphQueryPipeEmpty() = default;
        inline GraphQueryPipeEmpty(GraphQueryPipeEmpty const&) = default;
        inline GraphQueryPipeEmpty(GraphQueryPipeEmpty &&) = default;

        inline ~GraphQueryPipeEmpty() = default;

    protected:
        inline virtual void cleanup() override { };

        inline virtual typename Query::PipeResult pipeFunc(
            TGraph const* graph,
            std::shared_ptr<typename Query::Gremlin> const& gremlin
        ) override
        {
            return gremlin
                ? typename Query::PipeResult(gremlin)
                : typename Query::PipeResult(Query::PipeResultEnum::Pull);
        }
    };
    
}