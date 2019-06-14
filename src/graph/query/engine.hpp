#pragma once

#include <memory>
#include <variant>
#include <vector>
#include <set>
#include <tuple>

#include "../util.hpp"

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
            typename TGraph::Node const* node;

            std::map<size_t, typename TGraph::Node const*> labels;

        public:
            typename TGraph::Node const* getLabel(size_t label) const
            {
                auto it = labels.find(label);
                if ( it == labels.end() )
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

        class Pipe
        {
        protected:
            friend class GraphQueryEngine<TGraph>;

        public:
            virtual ~Pipe() = default;

        protected:
            inline virtual Pipe* init() const { return const_cast<Pipe*>(this); };
            inline virtual void cleanup() { delete this; };

            inline virtual std::vector<std::shared_ptr<class PipeLine>> getSubPipelines() { return { }; };

            virtual PipeResult pipeFunc(TGraph const* graph, std::shared_ptr<Gremlin> const& gremlin) = 0;
        };

        class PipeLine final
        {
            bool _isActive;
            std::vector<std::unique_ptr<Pipe>> _pipes;
            std::vector<Pipe*> _state;

        public:
            PipeLine()
                : _isActive(false)
                , _pipes()
                , _state()
            {
            }

            ~PipeLine()
            {
                reset();
            }

        public:
            inline void addPipe(std::unique_ptr<Pipe> && pipe)
            {
                if (_isActive)
                    throw graph_error("Pipeline is active.");
                _pipes.push_back(std::move(pipe));
            }

            inline void ensureActive()
            {
                if (!_isActive)
                {
                    _state.reserve(_pipes.size());
                    for (auto const& pipe : _pipes)
                    {
                        _state.push_back(pipe->init());
                    }
                    _isActive = true;
                }
            }

            inline size_t countPipes() const
            {
                return _pipes.size();
            }

            inline void reset()
            {
                for (auto pipe : _state)
                {
                    pipe->cleanup();
                }
                _state.clear();
                _isActive = false;
            }

            inline std::vector<std::shared_ptr<Gremlin>> run(TGraph* graph)
            {
                ensureActive();

                // The results of the query
                std::vector<std::shared_ptr<Gremlin>> results;

                // We operate "one to the right" because we need sentinals on both sides.
                const size_t max = _state.size();
                size_t done = 0;
                size_t pc = max;

                PipeResult maybe_gremlin = nullGremlin();

                while (done < max)
                {
                    auto gremlin = std::holds_alternative<std::shared_ptr<Gremlin>>(maybe_gremlin)
                        ? std::get<std::shared_ptr<Gremlin>>(maybe_gremlin)
                        : nullGremlin();

                    maybe_gremlin = _state[pc - 1]->pipeFunc(graph, gremlin);

                    auto maybe_gremlin_is_enum = std::holds_alternative<PipeResultEnum>(maybe_gremlin);
                    if (maybe_gremlin_is_enum)
                    {
                        auto result_enum = std::get<PipeResultEnum>(maybe_gremlin);
                        maybe_gremlin = nullGremlin();
                        
                        // Pipe needs more input
                        if (result_enum == PipeResultEnum::Pull)
                        {
                            // Try the previous pipe
                            if (pc - 1 > done)
                            {
                                pc --;
                                continue;
                            }
                            // , the previous pipe is done then so is this one
                            else
                                done = pc;
                        }
                        // Pipe is finished
                        else if (result_enum == PipeResultEnum::Done)
                        {
                            done = pc;
                        }
                    }

                    // Move to next pipe
                    pc ++; 

                    if (pc > max)
                    {
                        // check for a gremlin that popped out
                        if (!maybe_gremlin_is_enum)
                        {
                            auto gremlin = std::get<std::shared_ptr<Gremlin>>(maybe_gremlin);
                            if (gremlin)
                            {
                                results.push_back(gremlin);
                            }
                        }

                        // step back
                        maybe_gremlin = nullGremlin();
                        pc --;
                    }
                }

                return results;
            }
        };

    public:
        static inline std::shared_ptr<Gremlin> makeGremlin(typename TGraph::Node const* node)
        {
            std::shared_ptr<Gremlin> ret = std::make_shared<Gremlin>();
            ret->node = node;

            return ret;
        }
        static inline std::shared_ptr<Gremlin> makeGremlin(typename TGraph::Node const* node, std::shared_ptr<Gremlin> const& original)
        {
            if (!original)
                return makeGremlin(node);

            std::shared_ptr<Gremlin> ret = std::make_shared<Gremlin>(*original);
            ret->node = node;

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
        std::shared_ptr<PipeLine> _pipeline;

        size_t _labelCounter;
        std::map<std::string, size_t> _labels;

    // state
    private:
        TGraph* _graph;

    public:
        GraphQueryEngine()
            : _pipeline()
            , _labelCounter(0)
            , _labels()
            , _graph(nullptr)
        { }
        GraphQueryEngine(TGraph* g)
            : _pipeline()
            , _labelCounter(0)
            , _labels()
            , _graph(g)
        { }

    public:
        inline void reset()
        {
            if (_pipeline)
                _pipeline->reset();
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

        inline void setPipeline(std::shared_ptr<PipeLine> pipeline)
        {
            reset();
            _pipeline = pipeline;
            reset();
        }

        inline std::shared_ptr<PipeLine> getPipeline() const
        {
            return _pipeline;
        }

        inline size_t requireLabel(std::string const& label)
        {
            auto lb = _labels.lower_bound(label);

            if(lb != _labels.end() && !(_labels.key_comp()(label, lb->first)))
            {
                return lb->second;
            }
            else
            {
                auto id = ++_labelCounter;
                _labels.insert(lb, typename decltype(_labels)::value_type(label, id));
                return id;
            }
        }

        inline std::vector<typename TGraph::Node const*> run()
        {
            auto results = _pipeline->run(_graph);

            // TODO build a better way to map projected reuslts
            // (perhaps by mutating query template and templating this function?)

            std::vector<typename TGraph::Node const*> ret;
            ret.reserve(results.size());
            for (auto grem : results)
            {
                ret.push_back(grem->node);
            }

            return ret;
        }
    };
}
