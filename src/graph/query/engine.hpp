/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/. */

#pragma once

#include <memory>
#include <variant>
#include <vector>
#include <set>
#include <tuple>
#include <atomic>

#include "graph/util.hpp"

/*
 * This file contains the query engine, which has the following parts:
 * - Gremlin
 * - Pipe - Base class
 * - PipeLine - Container
 *   - Including interpreter
 */

namespace graph
{
    template<typename TGraph>
    class GraphQueryEngine final
    {
    public:
        using Graph = TGraph;

        class Gremlin
        {
        public:
            typename TGraph::Core const* graphObject;

            std::map<size_t, typename TGraph::Node const*> marks;

        public:
            typename TGraph::Node const* node() const
            {
                assert(graphObject == nullptr || TGraph::isNode(graphObject));
                return (typename TGraph::Node const*)graphObject;
            }


        public:
            typename TGraph::Node const* getMarker(size_t mark) const
            {
                auto it = marks.find(mark);
                if ( it == marks.end() )
                    return nullptr;
                else
                    return it->second;
            }
        };

        enum class PipeResultEnum
        {
            Done,
            Pull
        };

        using PipeResult = std::variant<PipeResultEnum, std::shared_ptr<Gremlin>>;

        class PipeDescription
        {
        protected:
            friend class GraphQueryEngine<TGraph>;

        public:
            virtual ~PipeDescription() = default;

        protected:
            virtual class PipeState* init() const = 0;
            virtual std::vector<std::shared_ptr<class PipeLine>> getSubPipelines() const = 0;
        };

        class PipeState
        {
        protected:
            friend class GraphQueryEngine<TGraph>;

        public:
            virtual ~PipeState() = default;

        protected:
            virtual void cleanup() = 0;
            virtual PipeResult pipeFunc(TGraph const* graph, std::shared_ptr<Gremlin> const& gremlin) = 0;
        };

        class Pipe
            : public PipeDescription
            , public PipeState
        {
        protected:
            inline virtual class PipeState* init() const { return const_cast<Pipe*>(this); };
            inline virtual std::vector<std::shared_ptr<class PipeLine>> getSubPipelines() const { return { }; };

            inline virtual void cleanup() { delete this; };
        };

        class PipeLineDescription
        {
            std::vector<std::shared_ptr<PipeDescription>> _pipes;
            mutable std::atomic<int32_t> _inUse;

            friend class PipeLineState;

        public:
            PipeLineDescription()
                : _pipes()
                , _inUse(0)
            {
            }

            ~PipeLineDescription()
            {
            }

        public:
            inline void insertPipe(size_t i, std::unique_ptr<PipeDescription> && pipe)
            {
                int32_t expected = 0;
                if (i > _pipes.size())
                    throw graph_error("Index out of range for insert.");

                if (!_inUse.compare_exchange_strong(expected, -1))
                    throw graph_error("PipeLineDescription is in use, clone or wait for queries to finish.");

                _pipes.insert(_pipes.begin() + i, std::move(pipe));

                _inUse = 0;
            }

            inline void addPipe(std::unique_ptr<PipeDescription> && pipe)
            {
                int32_t expected = 0;
                if (!_inUse.compare_exchange_strong(expected, -1))
                    throw graph_error("PipeLineDescription is in use, clone or wait for queries to finish.");

                _pipes.push_back(std::move(pipe));

                _inUse = 0;
            }

            inline void appendPipes(std::shared_ptr<PipeLineDescription> description)
            {
                int32_t expected_this = 0;
                int32_t expected_that = 0;
                if (0 == description->_pipes.size())
                    throw graph_error("Empty description.");

                if (!_inUse.compare_exchange_strong(expected_this, -1))
                    throw graph_error("PipeLineDescription is in use, clone or wait for queries to finish.");

                if (!description->_inUse.compare_exchange_strong(expected_that, -1))
                {
                    _inUse = 0;
                    throw graph_error("PipeLineDescription (that) is in use, clone or wait for queries to finish.");
                }

                for (auto it : description->_pipes)
                {
                    _pipes.push_back(it);
                }

                description->_inUse = 0;
                _inUse = 0;
            }

            inline size_t countPipes() const
            {
                return _pipes.size();
            }
        };

        class PipeLineState
        {
            std::vector<PipeState*> _state;
            std::shared_ptr<PipeLineDescription const> _description;

        public:
            PipeLineState(std::shared_ptr<PipeLineDescription const> description)
                : _state()
                , _description(description)
                , _i_maybeGremlin(nullGremlin())
            {
            }

            // Our copy operation resets the state
            // perhaps in the future pipe-states could support copy operators.
            PipeLineState(PipeLineState const& that)
                : PipeLineState(that._description)
            {
            }

            ~PipeLineState()
            {
                cleanup();
            }

        private:
            inline void aquireDescription()
            {
                int v = _description->_inUse.load();
                while (!_description->_inUse.compare_exchange_weak(v, v + 1)) { }
            }
            inline void releaseDescription()
            {
                int v = _description->_inUse.load();
                while (!_description->_inUse.compare_exchange_weak(v, v - 1)) { }
            }

        public:
            inline void cleanup()
            {
                if (_state.size() == 0)
                    return;

                for (auto pipe : _state)
                {
                    pipe->cleanup();
                }
                _state.clear();

                releaseDescription();
            }

            inline void init()
            {
                if (_state.size() != 0)
                    throw graph_error("PipeLineState is currently in use!");
                
                aquireDescription();

                if (_description->_pipes.size() == 0)
                {
                    _i_begin();
                    releaseDescription();
                    return;
                }

                _state.reserve(_description->_pipes.size());
                for (auto const& pipe : _description->_pipes)
                {
                    _state.push_back(pipe->init());
                }

                _i_begin();
            }

            inline PipeState* get(size_t index)
            {
                return _state.at(index);
            }
            inline PipeState const* get(size_t index) const
            {
                return _state.at(index);
            }

        // Interpreter
        private:
            size_t _i_max, _i_done, _i_pc;
            PipeResult _i_maybeGremlin;

            inline void _i_begin()
            {
                // We operate "one to the right" because we need sentinals on both sides.
                _i_max = _state.size();
                _i_done = 0;
                _i_pc = _i_max;

                _i_maybeGremlin = nullGremlin();
            }

            inline bool _i_shouldContinue() const
            {
                return _i_done < _i_max;
            }

            inline std::shared_ptr<Gremlin> _i_step(TGraph const* graph)
            {
                std::shared_ptr<Gremlin> result_gremlin = nullGremlin();

                auto gremlin = std::holds_alternative<std::shared_ptr<Gremlin>>(_i_maybeGremlin)
                    ? std::get<std::shared_ptr<Gremlin>>(_i_maybeGremlin)
                    : nullGremlin();

                _i_maybeGremlin = _state[_i_pc - 1]->pipeFunc(graph, gremlin);

                auto maybe_gremlin_is_enum = std::holds_alternative<PipeResultEnum>(_i_maybeGremlin);
                if (maybe_gremlin_is_enum)
                {
                    auto result_enum = std::get<PipeResultEnum>(_i_maybeGremlin);
                    _i_maybeGremlin = nullGremlin();
                    
                    // Pipe needs more input
                    if (result_enum == PipeResultEnum::Pull)
                    {
                        // Try the previous pipe
                        if (_i_pc - 1 > _i_done)
                        {
                            _i_pc --;
                            return result_gremlin;
                        }
                        // , the previous pipe is done then so is this one
                        else
                            _i_done = _i_pc;
                    }
                    // Pipe is finished
                    else if (result_enum == PipeResultEnum::Done)
                    {
                        _i_done = _i_pc;
                    }
                }

                // Move to next pipe
                _i_pc ++; 

                if (_i_pc > _i_max)
                {
                    // check for a gremlin that popped out
                    if (!maybe_gremlin_is_enum)
                    {
                        result_gremlin = std::get<std::shared_ptr<Gremlin>>(_i_maybeGremlin);
                    }

                    // step back
                    _i_maybeGremlin = nullGremlin();
                    _i_pc --;
                }

                return result_gremlin;
            }

            inline std::shared_ptr<Gremlin> _i_next(TGraph const* graph)
            {
                while (_i_shouldContinue())
                {
                    if (auto result = _i_step(graph))
                        return result;
                }

                return nullGremlin();
            }

        public:

            inline void reset()
            {
                cleanup();
                init();
            }

            inline bool done() const
            {
                return !_i_shouldContinue();
            }

            inline std::shared_ptr<Gremlin> next(TGraph const* graph)
            {
                return _i_next(graph);
            }

            inline std::vector<std::shared_ptr<Gremlin>> run(TGraph* graph)
            {
                reset();

                std::vector<std::shared_ptr<Gremlin>> results;

                while(auto result = _i_next(graph))
                {
                    results.push_back(result);
                }

                return results;
            }
        };

    public:
        static inline std::shared_ptr<Gremlin> makeGremlin(typename TGraph::Node const* node)
        {
            std::shared_ptr<Gremlin> ret = std::make_shared<Gremlin>();
            ret->graphObject = node;

            return ret;
        }
        static inline std::shared_ptr<Gremlin> makeGremlin(typename TGraph::Node const* node, std::shared_ptr<Gremlin> const& original)
        {
            if (!original)
                return makeGremlin(node);

            std::shared_ptr<Gremlin> ret = std::make_shared<Gremlin>(*original);
            ret->graphObject = node;

            return ret;
        }
        static inline std::shared_ptr<Gremlin> gotoVertex(std::shared_ptr<Gremlin> const& current, typename TGraph::Node const* node)
        {
            return makeGremlin(node, current);
        }

        static inline std::shared_ptr<Gremlin> nullGremlin()
        {
            return std::shared_ptr<Gremlin>();
        }

    // configure
    private:
        std::shared_ptr<PipeLineDescription> _pipelineDescription;
        PipeLineState* _pipelineState;

        size_t _markerCounter;
        std::map<std::string, size_t> _markers;

    // state
    private:
        TGraph* _graph;

    public:
        GraphQueryEngine()
            : _pipelineDescription()
            , _pipelineState(nullptr)
            , _markerCounter(0)
            , _markers()
            , _graph(nullptr)
        { }
        GraphQueryEngine(TGraph* g)
            : _pipelineDescription()
            , _pipelineState(nullptr)
            , _markerCounter(0)
            , _markers()
            , _graph(g)
        { }

        ~GraphQueryEngine()
        {
            reset();
        }

    public:
        inline void reset()
        {
            if (_pipelineState != nullptr)
            {
                delete _pipelineState;
                _pipelineState = nullptr;
            }
        }

        inline void setGraph(TGraph* g)
        {
            reset();
            _graph = g;
        }

        inline TGraph* getGraph() const
        {
            return _graph;
        }

        inline size_t requireMarker(std::string const& label)
        {
            auto lb = _markers.lower_bound(label);

            if(lb != _markers.end() && !(_markers.key_comp()(label, lb->first)))
            {
                return lb->second;
            }
            else
            {
                auto id = ++_markerCounter;
                _markers.insert(lb, typename decltype(_markers)::value_type(label, id));
                return id;
            }
        }

        inline void setPipeline(std::shared_ptr<PipeLineDescription> pipeline)
        {
            reset();
            _pipelineDescription = pipeline;
        }

        inline std::shared_ptr<PipeLineDescription> getPipeline() const
        {
            return _pipelineDescription;
        }

        inline bool done() const
        {
            if (_pipelineState == nullptr)
                return false;
            return _pipelineState->done();
        }

        inline typename TGraph::Node const* next()
        {
            if (_pipelineState == nullptr)
            {
                _pipelineState = new PipeLineState(_pipelineDescription);
                _pipelineState->init();
            }

            auto res = _pipelineState->next(_graph);

            return res ? res->node() : nullptr;
        }

        inline std::vector<typename TGraph::Node const*> run()
        {
            PipeLineState state(_pipelineDescription);
            state.init();

            auto results = state.run(_graph);

            // TODO build a better way to map projected reuslts
            // (perhaps by mutating query template and templating this function?)

            std::vector<typename TGraph::Node const*> ret;
            ret.reserve(results.size());
            for (auto grem : results)
            {
                ret.push_back(grem->node());
            }

            return ret;
        }
    };
}
