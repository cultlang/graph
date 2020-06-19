/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/. */

#include "semantics.h"

#include "stdext/bindmem.hpp"
#include "stdext/exception.h"

using namespace semantics;

namespace _hack {
    std::string stripQuotes(std::string s)
    {
        return s.substr(1, s.size() - 2);
    }
}

using namespace _hack;

Semantics::Semantics()
{
    _globals["newGraph"] = stdext::bindmem(this, &Semantics::newGraph);
    _globals["deleteGraph"] = stdext::bindmem(this, &Semantics::deleteGraph);

    _graphFunctions["addNode"] = stdext::bindmem(this, &Semantics::addNode);
    _graphFunctions["addEdge"] = stdext::bindmem(this, &Semantics::addEdge);

    _graphFunctions["v"] = stdext::bindmem(this, &Semantics::query);
}

void Semantics::evaluate(std::shared_ptr<ParseTree> p)
{
    if(p->variable == "global")
    {
        auto fn = _globals.find(p->sexprs[0][0]);
        if(fn == _globals.end())
        {
            throw stdext::exception("No Such Global: {0}", p->sexprs[0][0]);
        }
        fn->second(p);
    }
    else
    {
        auto graph = _graphs.find(p->variable);
        if(graph == _graphs.end())
        {
            throw stdext::exception("No Such Graph: {0}", p->variable);
        }
        auto fn = _graphFunctions.find(p->sexprs[0][0]);
        if(fn == _graphFunctions.end())
        {
            throw stdext::exception("No Such Graph Function: {0}", p->sexprs[0][0]);
        }
        fn->second(graph->second, p);
    }
    return;
}

void Semantics::newGraph(std::shared_ptr<ParseTree> s)
{
    auto args = std::vector<std::string>(s->sexprs[0].begin() + 1, s->sexprs[0].end());
    if(args.size() != 1)
        throw stdext::exception("global.newGraph Expected argument count of 1");
    

    if(_graphs.find(args[0]) != _graphs.end())
    {
        throw stdext::exception("Graph: {0} Exists", args[0]);
    }
    _graphs[stripQuotes(args[0])] = std::make_shared<Semantics::StrGraph>();
}

void Semantics::deleteGraph(std::shared_ptr<ParseTree> s)
{
    auto args = std::vector<std::string>(s->sexprs[0].begin() + 1, s->sexprs[0].end());
    if(args.size() != 1)
        throw stdext::exception("global.deleteGraph Expected argument count of 1");
    

    if(_graphs.find(args[0]) == _graphs.end())
    {
        throw stdext::exception("Graph: {0} Does not Exist", args[0]);
    }
    _graphs.erase(args[0]);
}

void Semantics::addNode(std::shared_ptr<StrGraph> g, std::shared_ptr<ParseTree> t)
{
    auto args = std::vector<std::string>(t->sexprs[0].begin() + 1, t->sexprs[0].end());
    // res_type();//res_type{const_cast<StrGraph::Node*>(g->addNode(args[0]))};
}

void Semantics::addEdge(std::shared_ptr<StrGraph> g, std::shared_ptr<ParseTree> t)
{
    auto args = std::vector<std::string>(t->sexprs[0].begin() + 1, t->sexprs[0].end());
    if(args.size() != 3)
    {
        throw stdext::exception("Expected arguments of form (nodeFrom, nodeTo, edge");
    }

    auto from = graph::findNode(*g, args[0]);
    auto to = graph::findNode(*g, args[1]);
    if(!from) throw stdext::exception("From Node Does not exist");
    if(!to) throw stdext::exception("To Node Does not exist");
    
    auto e = g->addEdge(args[2], {from, to});
    //return res_type(); //res_type{const_cast<StrGraph::Node*>(from), const_cast<StrGraph::Node*>(to)};
}

void Semantics::query(std::shared_ptr<StrGraph>, std::shared_ptr<ParseTree>)
{

}