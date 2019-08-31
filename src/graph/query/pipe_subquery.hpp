/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/. */

#pragma once

#include <memory>
#include <type_traits>

#include "engine.hpp"
#include "pipe_vertex.hpp"

namespace graph
{

	/******************************************************************************
	** GraphQuerySubQueryBase
	******************************************************************************/

    template<typename TGraph>
    class GraphQuerySubQueryBase
        : public GraphQueryEngine<TGraph>::Pipe
    {
    private:
        using Query = GraphQueryEngine<TGraph>;
        using PipeLineDescription = typename Query::PipeLineDescription;
        using PipeLineState = typename Query::PipeLineState;

    // config
    protected:
        std::shared_ptr<PipeLineDescription> _pipeline;

    // state
    protected:
        PipeLineState _state;
        std::shared_ptr<typename Query::Gremlin> _gremlin;

    private:
        inline static std::shared_ptr<PipeLineDescription> _fixupPipeline(std::shared_ptr<PipeLineDescription> pipeline)
        {
            pipeline->insertPipe(0, std::make_unique<GraphQueryPipeVertex<TGraph>>());
            return pipeline;
        }

    public:
        inline GraphQuerySubQueryBase(std::shared_ptr<PipeLineDescription> pipeline)
            : _pipeline(_fixupPipeline(pipeline))
            , _state(_pipeline)
        { }

        inline GraphQuerySubQueryBase(GraphQuerySubQueryBase const&) = default;
        inline GraphQuerySubQueryBase(GraphQuerySubQueryBase &&) = default;

        inline ~GraphQuerySubQueryBase() = default;

    protected:

    protected:
        inline virtual void cleanup() override { };

        virtual typename Query::PipeState* init() const override
        {
            auto res = new GraphQuerySubQueryBase(*this);
            res->_state.init();

            return res;
        };

        inline virtual typename Query::PipeResult pipeFunc(
            TGraph const* graph,
            std::shared_ptr<typename Query::Gremlin> const& gremlin
        ) override
        {
            auto vertex_state = (GraphQueryPipeVertex<TGraph>*)_state.get(0);
            auto empty = vertex_state->getNodes().size() == 0;

            if (!gremlin && empty)
                return Query::PipeResultEnum::Pull;

            // we have a gremlin to begin the query with
            if (empty)
            {
                _gremlin = gremlin;

                // TODO use gremlin source to inject gremlins
                vertex_state->setNode(gremlin->node());
            }

            auto subquery_result = _state.next(graph);
            typename Query::PipeResult result;

            if (_state.done()) // subquery_result == null
            {
                if (gremlin)
                // There was no result on the pipeline
                // return the gremlin
                {
                    result = gremlin;
                }
                else
                {
                    result = Query::PipeResultEnum::Pull;
                }

                _gremlin = nullptr;
                vertex_state->clear();
            }
            else
            {
                result = GraphQueryEngine<TGraph>::gotoVertex(_gremlin, subquery_result->node());
            }
            
            return result;
        }
    };

	/******************************************************************************
	** GraphQueryPipeOptional
	******************************************************************************/

    template<typename TGraph>
    class GraphQueryPipeOptional
        : public GraphQuerySubQueryBase<TGraph>
    {
    private:
        using Query = GraphQueryEngine<TGraph>;
        using PipeLineDescription = typename Query::PipeLineDescription;
        using PipeLineState = typename Query::PipeLineState;

        using Base = GraphQuerySubQueryBase<TGraph>;

    // config
    protected:

    // state
    protected:

    private:

    public:
        inline GraphQueryPipeOptional(std::shared_ptr<PipeLineDescription> pipeline)
            : Base(_pipeline)
        { }

        inline GraphQueryPipeOptional(GraphQueryPipeOptional const&) = default;
        inline GraphQueryPipeOptional(GraphQueryPipeOptional &&) = default;

        inline ~GraphQueryPipeOptional() = default;

    protected:

        inline virtual typename Query::PipeResult pipeFunc(
            TGraph const* graph,
            std::shared_ptr<typename Query::Gremlin> const& gremlin
        ) override
        {
            auto vertex_state = (GraphQueryPipeVertex<TGraph>*)_state.get(0);
            auto empty = vertex_state->getNodes().size() == 0;

            if (!gremlin && empty)
                return Query::PipeResultEnum::Pull;

            // we have a gremlin to begin the query with
            if (empty)
            {
                _gremlin = gremlin;

                // TODO use gremlin source to inject gremlins
                vertex_state->setNode(gremlin->node());
            }

            auto subquery_result = _state.next(graph);
            typename Query::PipeResult result;

            if (_state.done()) // subquery_result == null
            {
                if (gremlin)
                // There was no result on the pipeline
                // return the gremlin
                {
                    result = gremlin;
                }
                else
                {
                    result = Query::PipeResultEnum::Pull;
                }

                _gremlin = nullptr;
                vertex_state->clear();
            }
            else
            {
                result = GraphQueryEngine<TGraph>::gotoVertex(_gremlin, subquery_result->node());
            }
            
            return result;
        }
    };

	/******************************************************************************
	** GraphQueryPipeRepeatUntil
	******************************************************************************/

}
