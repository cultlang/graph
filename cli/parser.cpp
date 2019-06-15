#include "parser.h"
#include "stdext/exception.h"

std::vector<std::vector<std::string>> parser::parseSexprs(std::string s)
{
  auto line = pegtl::string_input(s, "repl");
  auto root = tao::pegtl::parse_tree::parse< grammar::grammar, grammar::selector>( line );
  std::vector<std::vector<std::string>> sexprs;
  if(root.get())
  {
    for(auto& call: root->children)
    {
      for(auto& f : call->children)
      {
        std::vector<std::string> s;
        s.push_back(std::string(f->string()));
        sexprs.push_back(std::move(s));
      }
      
    }

    return sexprs;
  }
  else
  {
    throw stdext::exception("Parse Error");
  }
  
}