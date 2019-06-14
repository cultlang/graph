#pragma once

#include <map>
#include <vector>
#include <deque>

namespace graph
{
    template<typename TGraphBase>
    class GraphFinalizer final
        : public TGraphBase
    {
    public:
        /*
         * All types listed here are expected to be forwarded by all 'Actual' types
         */
        
        using MetaFlags = typename TGraphBase::MetaFlags;
        using CoreData = typename TGraphBase::CoreData;

        struct Label
            : public TGraphBase::Core
            , public TGraphBase::Label
        { };

        struct Node
            : public TGraphBase::Core
            , public TGraphBase::Node
        { };

        struct Edge
            : public TGraphBase::Core
            , public TGraphBase::Edge
        { };

        struct Prop
            : public TGraphBase::Core
            , public TGraphBase::Prop
        { };
    };

    // This type realizes the graph
    template<typename TGraphBase>
    class Graph
        : public TGraphBase::template Actual<GraphFinalizer<TGraphBase>>
    {

    };
}
