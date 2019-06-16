#include "parser.h"


#include "stdext/exception.h"

std::shared_ptr<semantics::ParseTree> parser::parseSexprs(std::string s)
{
  auto in = pegtl::string_input(s, "repl");
  auto semantics = std::make_shared<semantics::ParseTree>();
  pegtl::parse< grammar::grammar, grammar::action >( in, semantics );
  return semantics;
}