/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/. */

#pragma once

#include <memory>
#include <type_traits>
#include <queue>
#include <stack>

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

    private:
        inline static std::shared_ptr<PipeLineDescription> _fixupPipeline(std::shared_ptr<PipeLineDescription> pipeline)
        {
            pipeline->insertPipe(0, std::make_unique<GraphQueryPipeVertex<TGraph>>());
            return pipeline;
        }

    public:
        inline GraphQuerySubQueryBase(std::shared_ptr<PipeLineDescription> pipeline)
            : _pipeline(_fixupPipeline(pipeline))
        { }

        inline GraphQuerySubQueryBase(GraphQuerySubQueryBase const&) = default;
        inline GraphQuerySubQueryBase(GraphQuerySubQueryBase &&) = default;

        inline ~GraphQuerySubQueryBase() = default;

    protected:

    protected:
        inline virtual void cleanup() override { };
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
        PipeLineState _state;
        std::shared_ptr<typename Query::Gremlin> _gremlin;

    private:

    public:
        inline GraphQueryPipeOptional(std::shared_ptr<PipeLineDescription> pipeline)
            : Base(pipeline)
            , _state(_pipeline)
            , _gremlin()
        { }

        inline GraphQueryPipeOptional(GraphQueryPipeOptional const&) = default;
        inline GraphQueryPipeOptional(GraphQueryPipeOptional &&) = default;

        inline ~GraphQueryPipeOptional() = default;

    protected:

        inline virtual typename Query::PipeState* init() const override
        {
            auto res = new GraphQueryPipeOptional(*this);
            res->_state.init();

            return res;
        };

        inline GraphQueryPipeVertex<TGraph>* _getCurrentStateVertexPipe()
        {
            return (GraphQueryPipeVertex<TGraph>*)_state.get(0);
        }

        inline void _doSetupState()
        {
            if (_state.done())
                _state.reset();
            // TODO use gremlin source to inject gremlins
            _getCurrentStateVertexPipe()->setNode(_gremlin->node());
        }

        inline virtual typename Query::PipeResult pipeFunc(
            TGraph const* graph,
            std::shared_ptr<typename Query::Gremlin> const& gremlin
        ) override
        {
            auto empty = _getCurrentStateVertexPipe()->getNodes().size() == 0;

            if (!gremlin && empty)
                return Query::PipeResultEnum::Pull;

            // we have a gremlin to begin the query with
            if (empty)
            {
                _gremlin = gremlin;
                _doSetupState();
            }

            auto subquery_result = _state.next(graph);
            typename Query::PipeResult result;

            if (_state.done()) // subquery_result == null
            {
                if (gremlin)
                // There was no result on the pipeline, return the gremlin if possible
                // This uses the fact that a failed query will happen immedietly to implement optional behaviour
                {
                    result = gremlin;
                }
                else
                {
                    result = Query::PipeResultEnum::Pull;
                }

                _gremlin = nullptr;
                _getCurrentStateVertexPipe()->clear();
            }
            else
            {
                result = GraphQueryEngine<TGraph>::gotoVertex(_gremlin, subquery_result->node());
            }
            
            return result;
        }
    };

	/******************************************************************************
	** GraphQueryPipeRepeatBreadthFirst
	******************************************************************************/

    template<typename TGraph, typename TFuncRepeatFilter, typename TFuncEmitFilter = void*>
    class GraphQueryPipeRepeatBreadthFirst
        : public GraphQuerySubQueryBase<TGraph>
    {
    private:
        using Query = GraphQueryEngine<TGraph>;
        using PipeLineDescription = typename Query::PipeLineDescription;
        using PipeLineState = typename Query::PipeLineState;

        using Base = GraphQuerySubQueryBase<TGraph>;

    // config
    protected:
        TFuncRepeatFilter _func_repeat;
        TFuncEmitFilter _func_emit;

    // state
    protected:
        PipeLineState _state;
        std::queue<std::shared_ptr<typename Query::Gremlin>> _gremlins;
        

    private:

    public:
        inline GraphQueryPipeRepeatBreadthFirst(std::shared_ptr<PipeLineDescription> pipeline, TFuncRepeatFilter const& func_repeat)
            : Base(pipeline)
            , _func_repeat(func_repeat)
            , _func_emit(nullptr)
            , _state(_pipeline)
            , _gremlins()
        { }

        inline GraphQueryPipeRepeatBreadthFirst(std::shared_ptr<PipeLineDescription> pipeline, TFuncRepeatFilter const& func_repeat, TFuncEmitFilter const& func_emit)
            : Base(pipeline)
            , _func_repeat(func_repeat)
            , _func_emit(func_emit)
            , _state(_pipeline)
            , _gremlins()
        { }

        inline GraphQueryPipeRepeatBreadthFirst(GraphQueryPipeRepeatBreadthFirst const&) = default;
        inline GraphQueryPipeRepeatBreadthFirst(GraphQueryPipeRepeatBreadthFirst &&) = default;

        inline ~GraphQueryPipeRepeatBreadthFirst() = default;

    protected:

        inline virtual typename Query::PipeState* init() const override
        {
            auto res = new GraphQueryPipeRepeatBreadthFirst(*this);
            res->_state.init();

            return res;
        };

        inline bool _call_func_repeat(typename TGraph::Node const* n, std::shared_ptr<typename Query::Gremlin> const& r)
        {
            if constexpr (std::is_invocable_v<TFuncRepeatFilter>)
                return _func_repeat();
            else if constexpr (std::is_invocable_v<TFuncRepeatFilter, decltype(n)>)
                return _func_repeat(n);
            else if constexpr (std::is_invocable_v<TFuncRepeatFilter, decltype(n), decltype(r)>)
                return _func_repeat(n, r);
            else
                static_assert(stdext::always_false<>, "TFuncRepeatFilter bad signature.");
        }

        inline bool _call_func_emit(typename TGraph::Node const* n, std::shared_ptr<typename Query::Gremlin> const& r)
        {
            if constexpr (std::is_invocable_v<TFuncEmitFilter, decltype(n)>)
                return _func_emit(n);
            else if constexpr (std::is_invocable_v<TFuncEmitFilter, decltype(n), decltype(r)>)
                return _func_emit(n, r);
            else
                static_assert(stdext::always_false<>, "TFuncEmitFilter bad signature.");
        }

        inline GraphQueryPipeVertex<TGraph>* _getCurrentStateVertexPipe()
        {
            return (GraphQueryPipeVertex<TGraph>*)_state.get(0);
        }

        inline void _doSetupState()
        {
            if (_state.done())
                _state.reset();
            // TODO use gremlin source to inject gremlins
            _getCurrentStateVertexPipe()->setNode(_gremlins.front()->node());
        }

        inline virtual typename Query::PipeResult pipeFunc(
            TGraph const* graph,
            std::shared_ptr<typename Query::Gremlin> const& gremlin
        ) override
        {
            auto empty = _getCurrentStateVertexPipe()->getNodes().size() == 0 && _gremlins.empty();

            if (!gremlin && empty)
                return Query::PipeResultEnum::Pull;

            // we have a gremlin to begin the query with
            if (empty)
            {
                _gremlins.push(gremlin);
                _doSetupState();
            }

            bool emit = false;
            std::shared_ptr<typename Query::Gremlin> subquery_result;
            do
            {
                subquery_result = _state.next(graph);
                
                if (_state.done()) // subquery_result == null
                {
                    _gremlins.pop();
                    _getCurrentStateVertexPipe()->clear();

                    // reset
                    if (_gremlins.empty())
                    {
                        // this will run the empty branch above eventually
                        return Query::PipeResultEnum::Pull;
                    }
                    else
                    {
                        _doSetupState();
                        continue;
                    }
                }

                bool repeat = emit = _call_func_repeat(subquery_result->node(), subquery_result);
                if constexpr (!std::is_same_v<TFuncEmitFilter, void*>)
                {
                    emit = _call_func_emit(subquery_result->node(), subquery_result);
                }

                if (repeat)
                {
                    _gremlins.push(subquery_result);
                }
            } while (!emit);

            return GraphQueryEngine<TGraph>::gotoVertex(_gremlins.front(), subquery_result->node());
        }
    };

	/******************************************************************************
	** GraphQueryPipeRepeatDepthFirst
	******************************************************************************/

    template<typename TGraph, typename TFuncRepeatFilter, typename TFuncEmitFilter = void*>
    class GraphQueryPipeRepeatDepthFirst
        : public GraphQuerySubQueryBase<TGraph>
    {
    private:
        using Query = GraphQueryEngine<TGraph>;
        using PipeLineDescription = typename Query::PipeLineDescription;
        using PipeLineState = typename Query::PipeLineState;

        using Base = GraphQuerySubQueryBase<TGraph>;

    // config
    protected:
        TFuncRepeatFilter _func_repeat;
        TFuncEmitFilter _func_emit;

    // state
    protected:
        struct _DepthStackState
        {
            PipeLineState* state;
            std::shared_ptr<typename Query::Gremlin> gremlin;

            inline GraphQueryPipeVertex<TGraph>* _getStateVertexPipe()
            {
                return (GraphQueryPipeVertex<TGraph>*)state->get(0);
            }

            inline _DepthStackState(GraphQueryPipeRepeatDepthFirst* this_, std::shared_ptr<typename Query::Gremlin> gremlin)
                : state(new PipeLineState(this_->_pipeline))
                , gremlin(gremlin)
            {
                state->init();
                _getStateVertexPipe()->setNode(gremlin->node());
            }

            inline ~_DepthStackState()
            {
                delete state;
            }
        };

        std::stack<_DepthStackState> _state;
        

    private:

    public:
        inline GraphQueryPipeRepeatDepthFirst(std::shared_ptr<PipeLineDescription> pipeline, TFuncRepeatFilter const& func_repeat)
            : Base(pipeline)
            , _func_repeat(func_repeat)
            , _func_emit(nullptr)
            , _state()
        { }

        inline GraphQueryPipeRepeatDepthFirst(std::shared_ptr<PipeLineDescription> pipeline, TFuncRepeatFilter const& func_repeat, TFuncEmitFilter const& func_emit)
            : Base(pipeline)
            , _func_repeat(func_repeat)
            , _func_emit(func_emit)
            , _state()
        { }

        inline GraphQueryPipeRepeatDepthFirst(GraphQueryPipeRepeatDepthFirst const&) = default;
        inline GraphQueryPipeRepeatDepthFirst(GraphQueryPipeRepeatDepthFirst &&) = default;

        inline ~GraphQueryPipeRepeatDepthFirst() = default;

    protected:

        inline virtual typename Query::PipeState* init() const override
        {
            auto res = new GraphQueryPipeRepeatDepthFirst(*this);

            return res;
        };

        inline bool _call_func_repeat(typename TGraph::Node const* n, std::shared_ptr<typename Query::Gremlin> const& r)
        {
            if constexpr (std::is_invocable_v<TFuncRepeatFilter>)
                return _func_repeat();
            else if constexpr (std::is_invocable_v<TFuncRepeatFilter, decltype(n)>)
                return _func_repeat(n);
            else if constexpr (std::is_invocable_v<TFuncRepeatFilter, decltype(n), decltype(r)>)
                return _func_repeat(n, r);
            else
                static_assert(stdext::always_false<>, "TFuncRepeatFilter bad signature.");
        }

        inline bool _call_func_emit(typename TGraph::Node const* n, std::shared_ptr<typename Query::Gremlin> const& r)
        {
            if constexpr (std::is_invocable_v<TFuncEmitFilter, decltype(n)>)
                return _func_emit(n);
            else if constexpr (std::is_invocable_v<TFuncEmitFilter, decltype(n), decltype(r)>)
                return _func_emit(n, r);
            else
                static_assert(stdext::always_false<>, "TFuncEmitFilter bad signature.");
        }

        inline void _doPushState(std::shared_ptr<typename Query::Gremlin> const& gremlin)
        {
            _state.emplace(this, gremlin);
        }

        inline virtual typename Query::PipeResult pipeFunc(
            TGraph const* graph,
            std::shared_ptr<typename Query::Gremlin> const& gremlin
        ) override
        {
            auto empty = _state.empty();

            if (!gremlin && empty)
                return Query::PipeResultEnum::Pull;

            // we have a gremlin to begin the query with
            if (empty)
            {
                _doPushState(gremlin);
            }

            bool emit = false;
            std::shared_ptr<typename Query::Gremlin> subquery_result;
            do
            {
                subquery_result = _state.top().state->next(graph);
                
                if (_state.top().state->done()) // subquery_result == null
                {
                    _state.pop();

                    // reset
                    if (_state.empty())
                    {
                        // this will run the empty branch above eventually
                        return Query::PipeResultEnum::Pull;
                    }
                    else
                    {
                        continue;
                    }
                }

                bool repeat = emit = _call_func_repeat(subquery_result->node(), subquery_result);
                if constexpr (!std::is_same_v<TFuncEmitFilter, void*>)
                {
                    emit = _call_func_emit(subquery_result->node(), subquery_result);
                }

                if (emit)
                {
                    subquery_result = GraphQueryEngine<TGraph>::gotoVertex(_state.top().gremlin, subquery_result->node());;
                }

                if (repeat)
                {
                    _doPushState(subquery_result);
                }
            } while (!emit);

            return subquery_result;
        }
    };
}
