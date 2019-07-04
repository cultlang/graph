/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/. */

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
    class GraphQueryPipeMark
        : public GraphQueryEngine<TGraph>::Pipe
    {
    private:
        using Query = GraphQueryEngine<TGraph>;

    // config
    protected:
        size_t _mark;

    // state
    protected:

    public:
        inline GraphQueryPipeMark(size_t mark)
            : _mark(mark)
        { }

        inline GraphQueryPipeMark(GraphQueryPipeMark const&) = default;

        inline ~GraphQueryPipeMark() = default;

    protected:
        inline virtual void cleanup() override { };

        inline virtual typename Query::PipeResult pipeFunc(
            TGraph const* graph,
            std::shared_ptr<typename Query::Gremlin> const& gremlin
        ) override
        {
            if (!gremlin)
                return Query::PipeResultEnum::Pull;
            
            gremlin->marks[_mark] = gremlin->node();

            return gremlin;
        }
    };

    
    template<typename TGraph>
    class GraphQueryPipeMerge
        : public GraphQueryEngine<TGraph>::Pipe
    {
    private:
        using Query = GraphQueryEngine<TGraph>;

    // config
    protected:
        std::vector<size_t> _markers;

    // state
    protected:
        std::shared_ptr<typename Query::Gremlin> _gremlin;
        typename decltype(_markers)::const_iterator _markers_it;

    public:
        inline GraphQueryPipeMerge(std::vector<size_t> markers)
            : _markers(markers)
            , _markers_it(_markers.end())
        { }

        inline GraphQueryPipeMerge(GraphQueryPipeMerge const& that)
            : GraphQueryPipeMerge(that._markers)
        { }

        inline ~GraphQueryPipeMerge() = default;

    protected:
        inline virtual void cleanup() override { };

        virtual typename Query::PipeState* init() const override
        {
            auto res = new GraphQueryPipeMerge(*this);

            return res;
        };

        inline virtual typename Query::PipeResult pipeFunc(
            TGraph const* graph,
            std::shared_ptr<typename Query::Gremlin> const& gremlin
        ) override
        {
            auto empty = _markers_it == _markers.end();
            if (!gremlin && empty)
                return Query::PipeResultEnum::Pull;
            
            if (empty)
            {
                _gremlin = gremlin;
                _markers_it = _markers.begin();
            }

            typename TGraph::Node const* n;
            while ((n = _gremlin->getMarker(*_markers_it)) == nullptr)
                ++_markers_it;

            if (n != nullptr)
            {
                ++_markers_it;
                return Query::makeGremlin(n, _gremlin);
            }
            else
            {
                return Query::PipeResultEnum::Pull;
            }
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
        size_t _marker;

    // state
    protected:

    public:
        inline GraphQueryPipeExcept(size_t marker)
            : _marker(marker)
        { }

        inline GraphQueryPipeExcept(GraphQueryPipeExcept const&) = default;

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
            
            if (gremlin->getMarker(_marker) == gremlin->node())
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
        size_t _marker;

    // state
    protected:

    public:
        inline GraphQueryPipeBack(size_t marker)
            : _marker(marker)
        { }

        inline GraphQueryPipeBack(GraphQueryPipeBack const&) = default;

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
            
            return GraphQueryEngine<TGraph>::gotoVertex(gremlin, gremlin->getMarker(_marker));
        }
    };

}