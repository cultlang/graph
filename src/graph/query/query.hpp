#pragma once

#include "engine.hpp"

namespace graph
{
    template<typename TGraph, typename TQueryFinal>
    class GraphQueryLibraryBase
    {
    protected:
        virtual std::shared_ptr<GraphQueryEngine<TGraph>> & engine() = 0;
        
        virtual TQueryFinal addPipe(std::unique_ptr<typename GraphQueryEngine<TGraph>::PipeDescription> && Pipe) = 0;

        virtual TQueryFinal newQueryPipeline() = 0;
        virtual std::shared_ptr<typename GraphQueryEngine<TGraph>::PipeLineDescription> extractPipeline(TQueryFinal const& other) = 0;
    };

    template<typename TGraph, template <typename, typename> typename TGraphQueryLibrary> 
    class GraphQuery final
        : public TGraphQueryLibrary<TGraph, GraphQuery<TGraph, TGraphQueryLibrary>>
    {
    private:
        std::shared_ptr<GraphQueryEngine<TGraph>> _engine;
        std::shared_ptr<typename GraphQueryEngine<TGraph>::PipeLineDescription> _pipeline;

        // subquery
        GraphQuery(std::shared_ptr<GraphQueryEngine<TGraph>> & engine)
        {
            _engine = engine;
            _pipeline = std::make_shared<typename GraphQueryEngine<TGraph>::PipeLineDescription>();
        }

    protected:
        inline virtual std::shared_ptr<GraphQueryEngine<TGraph>> & engine() override
        {
            return _engine;
        }
        inline virtual GraphQuery addPipe(std::unique_ptr<typename GraphQueryEngine<TGraph>::PipeDescription> && Pipe) override
        {
            _pipeline->addPipe(std::move(Pipe));

            return std::move(*this);
        }
        
        inline virtual GraphQuery newQueryPipeline() override
        {
            return GraphQuery(_engine);
        }
        inline virtual std::shared_ptr<typename GraphQueryEngine<TGraph>::PipeLineDescription> extractPipeline(GraphQuery const& query)
        {
            return query._pipeline;
        }


    public:
        GraphQuery(TGraph* g)
        {
            _engine = std::make_shared<GraphQueryEngine<TGraph>>(g);
            _pipeline = std::make_shared<typename GraphQueryEngine<TGraph>::PipeLineDescription>();
            _engine->setPipeline(_pipeline);
        }

        ~GraphQuery() = default;

        GraphQuery(GraphQuery&& that) = default;
        GraphQuery& operator=(GraphQuery&& other) = default;

        GraphQuery(GraphQuery const& that) = delete;
        GraphQuery& operator=(const GraphQuery& other) = delete;

        inline GraphQueryEngine<TGraph>* operator->()
        {
            return _engine.operator->();
        }

        inline std::vector<typename TGraph::Node const*> run()
        {
            // TODO template magic to pass query template outs
            return _engine->run();
        }

        inline bool done()
        {
            return _engine->done();
        }
        
        inline typename TGraph::Node const* next()
        {
            // TODO template magic to pass query template outs
            return _engine->next();
        }
    };
}
