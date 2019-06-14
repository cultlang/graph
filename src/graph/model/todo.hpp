#pragma once

#include <vector>

#include "../util.hpp"

namespace graph
{
    // Provides a way to reference parent graphs, but keep changes local to this one
    template<typename TGraphBase>
    class GraphDifferentiated
        : public TGraphBase
    {
    // Actual type:
    public:
        template<typename TFinal>
        class Actual
            : public TGraphBase::template Actual<TFinal>
        {
            using Base = typename TGraphBase::template Actual<TFinal>;

        protected:

            std::vector<TFinal*> _parents;
        };
    };

    // Provides a way to name edges, nodes, labels, and props
    // Along with effecient access patterns
    template<typename TGraphBase>
    class GraphNamed
        : public TGraphBase
    {
    // Actual type:
    public:
        template<typename TFinal>
        class Actual
            : public TGraphBase::template Actual<TFinal>
        {
            using Base = typename TGraphBase::template Actual<TFinal>;

        protected:

            std::vector<TFinal*> _parents;
        };
    };
}