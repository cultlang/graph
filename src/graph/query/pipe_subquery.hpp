#pragma once

#include <memory>
#include <type_traits>

#include "engine.hpp"
#include "pipe_vertex.hpp"

namespace graph
{

    template<typename TGraph>
    class GraphQueryPipeOptional
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

    private:
        inline static std::shared_ptr<PipeLineDescription> _fixupPipeline(std::shared_ptr<PipeLineDescription> pipeline)
        {
            pipeline->addPipe(std::make_unique<GraphQueryPipeVertex<TGraph>>());
            return pipeline;
        }

    public:
        inline GraphQueryPipeOptional(std::shared_ptr<PipeLineDescription> pipeline)
            : _pipeline(pipeline)
            , _state(_pipeline)
        { }

        inline GraphQueryPipeOptional(GraphQueryPipeOptional const&) = default;
        inline GraphQueryPipeOptional(GraphQueryPipeOptional &&) = default;

        inline ~GraphQueryPipeOptional() = default;

    protected:
        inline virtual void cleanup() override { };

        inline virtual typename Query::PipeResult pipeFunc(
            TGraph const* graph,
            std::shared_ptr<typename Query::Gremlin> const& gremlin
        ) override
        {
            if (!gremlin)
                return Query::PipeResultEnum::Pull;

            return gremlin;
        }
    };

}
