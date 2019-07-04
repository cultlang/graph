/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/. */


#include "parser.h"


#include "stdext/exception.h"

std::shared_ptr<semantics::ParseTree> parser::parseSexprs(std::string s)
{
  auto in = pegtl::string_input(s, "repl");
  auto semantics = std::make_shared<semantics::ParseTree>();
  pegtl::parse< grammar::grammar, grammar::action >( in, semantics );
  return semantics;
}