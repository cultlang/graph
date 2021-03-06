/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/. */

#pragma once

#include <map>
#include <vector>

#include "util.hpp"

/*
 * This file contains the various minor algorithms and is laid out as follows:
 * 
 * - edgeIsIncoming
 * - findNode
 * - collectEdges
 * - collectNodes
 * - copyGraph (TODO)
 */

namespace ugly
{
    template<typename TGraph>
    bool edgeIsIncoming(TGraph const& g, typename TGraph::Node const* n, typename TGraph::Edge const* e)
    {
        return (g.indexOfNodeInEdgeThrowing(n, e) != 0) != TGraph::isEdgeInverted(e);
    }
    template<typename TGraph>
    bool edgeIsOutgoing(TGraph const& g,  typename TGraph::Node const* n, typename TGraph::Edge const* e)
    {
        return (g.indexOfNodeInEdgeThrowing(n, e) == 0) != TGraph::isEdgeInverted(e);
    }

    template<typename TGraph>
    typename TGraph::Node const* findNode(TGraph const& g, typename TGraph::NodeData const& v)
    {
        typename TGraph::Node const* res = nullptr;
        g.forAllNodes([&](auto n)
        {
            if (n->data == v)
                res = n;
            return n->data != v;
        });
        return res;
    }

    template<typename TGraph>
    typename TGraph::Label const* findLabel(TGraph const& g, typename TGraph::LabelData const& v)
    {
        typename TGraph::Label const* res = nullptr;
        g.forAllLabels([&](auto n)
        {
            if (n->data == v)
                res = n;
            return n->data != v;
        });
        return res;
    }

    template<typename TGraph>
    typename TGraph::Node const* requireNode(TGraph& g, typename TGraph::NodeData const& v)
    {
        typename TGraph::Node const* res = findNode(g, v);
        if (res == nullptr)
            res = g.addNode(v);
        return res;
    }

    template<typename TGraph>
    typename TGraph::Label const* requireLabel(TGraph& g, typename TGraph::LabelData const& v)
    {
        typename TGraph::Label const* res = findLabel(g, v);
        if (res == nullptr)
            res = g.addLabel(v);
        return res;
    }

    template<typename TGraph, typename Func>
    typename std::vector<typename TGraph::Edge const*> collectEdges(TGraph const& g, typename TGraph::Node const* n, Func const& func)
    {
        typename std::vector<typename TGraph::Edge const*> res;
        g.forEdgesOnNode(n, [&](auto e)
        {
            if (func(e))
                res.push_back(e);
        });
        return res;
    }

    template<typename TGraph, typename Func>
    typename std::vector<typename TGraph::Node const*> collectNodes(TGraph const& g, typename TGraph::Edge const* e, Func const& func)
    {
        typename std::vector<typename TGraph::Node const*> res;
        g.forNodesInEdge(e, [&](auto n)
        {
            if (func(n))
                res.push_back(n);
        });
        return res;
    }

    template<typename TGraph>
    void copyGraph(TGraph& dst, TGraph const& src)
    {
        // TODO: need a copy data-section function to copy types and other data
        //std::map<TGraph::Label*, TGraph::Label*> _labelMap;

        src.forAllLabels([&](auto l)
        {
            dst.addLabel(l->data);
        });
    }
}
