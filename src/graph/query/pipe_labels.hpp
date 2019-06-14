#pragma once

#include <memory>

#include "engine.hpp"

/*
 * This file contains the following pipe implementations
 * - GraphQueryPipeLabel (AKA `as`)
 * - GraphQueryPipeExcept (AKA `except`)
 * - GraphQueryPipeBack (AKA `back`)
 */

namespace graph
{

    template<typename TGraph>
    class GraphQueryPipeLabel
        : public GraphQueryEngine<TGraph>::Pipe
    {
    private:
        using Query = GraphQueryEngine<TGraph>;

    // config
    protected:
        size_t _label;

    // state
    protected:

    public:
        inline GraphQueryPipeLabel(size_t label)
            : _label(label)
        { }

        inline GraphQueryPipeLabel(GraphQueryPipeLabel const&) = default;
        inline GraphQueryPipeLabel(GraphQueryPipeLabel &&) = default;

        inline ~GraphQueryPipeLabel() = default;

    protected:
        inline virtual void cleanup() override { };

        inline virtual typename Query::PipeResult pipeFunc(
            TGraph const* graph,
            std::shared_ptr<typename Query::Gremlin> const& gremlin
        ) override
        {
            if (!gremlin)
                return Query::PipeResultEnum::Pull;
            
            gremlin->labels[_label] = gremlin->node;

            return gremlin;
        }
    };


    template<typename TGraph>
    class GraphQueryPipeExcept
        : public GraphQueryEngine<TGraph>::Pipe
    {
    private:
        using Query = GraphQueryEngine<TGraph>;

    // config
    protected:
        size_t _label;

    // state
    protected:

    public:
        inline GraphQueryPipeExcept(size_t label)
            : _label(label)
        { }

        inline GraphQueryPipeExcept(GraphQueryPipeExcept const&) = default;
        inline GraphQueryPipeExcept(GraphQueryPipeExcept &&) = default;

        inline ~GraphQueryPipeExcept() = default;

    protected:
        inline virtual void cleanup() override { };

        inline virtual typename Query::PipeResult pipeFunc(
            TGraph const* graph,
            std::shared_ptr<typename Query::Gremlin> const& gremlin
        ) override
        {
            if (!gremlin)
                return Query::PipeResultEnum::Pull;
            
            if (gremlin->getLabel(_label) == gremlin->node)
                return Query::PipeResultEnum::Pull;

            return gremlin;
        }
    };


    template<typename TGraph>
    class GraphQueryPipeBack
        : public GraphQueryEngine<TGraph>::Pipe
    {
    private:
        using Query = GraphQueryEngine<TGraph>;

    // config
    protected:
        size_t _label;

    // state
    protected:

    public:
        inline GraphQueryPipeBack(size_t label)
            : _label(label)
        { }

        inline GraphQueryPipeBack(GraphQueryPipeBack const&) = default;
        inline GraphQueryPipeBack(GraphQueryPipeBack &&) = default;

        inline ~GraphQueryPipeBack() = default;

    protected:
        inline virtual void cleanup() override { };

        inline virtual typename Query::PipeResult pipeFunc(
            TGraph const* graph,
            std::shared_ptr<typename Query::Gremlin> const& gremlin
        ) override
        {
            if (!gremlin)
                return Query::PipeResultEnum::Pull;
            
            return GraphQueryEngine<TGraph>::gotoVertex(gremlin, gremlin->getLabel(_label));
        }
    };

}