#include <string>
#include <iostream>
#include <sstream> 

#include "replxx.hxx"
#include "stdext/exception.h"

#include "parser.h"
#include "semantics.h"

using Replxx = replxx::Replxx;



int main(int argc, char** argv)
{
  Replxx rx;
  semantics::Semantics semantics;
  while(true)
  {
    
    auto line = std::string(rx.input("cult/graph>"));
    try
    {
      auto sexprs = parser::parseSexprs(line);
      semantics.evaluate(sexprs);
    }
    catch(const stdext::exception& e)
    {
      rx.print(e.what());
      rx.print("\n");
      //std::cerr << e.what() << '\n';
    }
    
  }
  return 0;
}