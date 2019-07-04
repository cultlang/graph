/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/. */

#pragma once

#include <memory>
#include <set>

#include "engine.hpp"

namespace graph
{

    template<typename TGraph>
    class GraphQueryPipeUnique
        : public GraphQueryEngine<TGraph>::Pipe
    {
    private:
        using Query = GraphQueryEngine<TGraph>;

    // config
    protected:

    // state
    protected:
        std::set<typename TGraph::Node const*> _seen;

    public:
        inline GraphQueryPipeUnique()
            : _seen()
        { }

        inline GraphQueryPipeUnique(GraphQueryPipeUnique const&) = default;
        inline GraphQueryPipeUnique(GraphQueryPipeUnique &&) = default;

        inline ~GraphQueryPipeUnique() = default;

    protected:
        virtual typename Query::Pipe* init() const override
        {
            auto res = new GraphQueryPipeUnique();

            return res;
        };

        inline virtual typename Query::PipeResult pipeFunc(
            TGraph const* graph,
            std::shared_ptr<typename Query::Gremlin> const& gremlin
        ) override
        {
            if (!gremlin)
                return Query::PipeResultEnum::Pull;
            if (_seen.count(gremlin->node))
                return Query::PipeResultEnum::Pull;
            _seen.insert(gremlin->node);
            return gremlin;
        }
    };
    
}