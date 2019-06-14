#pragma once

#include <memory>
#include <vector>

#include "engine.hpp"

namespace graph
{

    template<typename TGraph>
    class GraphQueryPipeVertex
        : public GraphQueryEngine<TGraph>::Pipe
    {
    private:
        using Query = GraphQueryEngine<TGraph>;

    // config
    protected:
        std::vector<typename TGraph::Node const*> _nodes;

    // state
    protected:
        typename decltype(_nodes)::iterator _it;

    public:
        inline GraphQueryPipeVertex(std::vector<typename TGraph::Node const*> nodes)
            : _nodes(nodes)
            , _it(_nodes.end())
        { }
        inline GraphQueryPipeVertex(typename TGraph::Node const* node)
            : _nodes({node})
            , _it(_nodes.end())
        { }

        inline GraphQueryPipeVertex(GraphQueryPipeVertex const&) = default;
        inline GraphQueryPipeVertex(GraphQueryPipeVertex &&) = default;

        inline ~GraphQueryPipeVertex() = default;

    protected:
        virtual typename Query::Pipe* init() const override
        {
            auto res = new GraphQueryPipeVertex(_nodes);
            res->_it = res->_nodes.begin();

            return res;
        };

        inline virtual typename Query::PipeResult pipeFunc(
            TGraph const* graph,
            std::shared_ptr<typename Query::Gremlin> const& gremlin
        ) override
        {
            if (_it == _nodes.end())
                return Query::PipeResultEnum::Done;
            else
                return GraphQueryEngine<TGraph>::makeGremlin(*(_it ++), gremlin);
        }
    };

}