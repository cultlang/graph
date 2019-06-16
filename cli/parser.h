#pragma once
#include <memory>

#include "grammar.h"
#include "semantics.h"

namespace parser {


  std::shared_ptr<semantics::ParseTree> parseSexprs(std::string);
}