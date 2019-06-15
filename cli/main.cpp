#include <string>
#include <iostream>
#include <sstream> 

#include "replxx.hxx"
#include "stdext/exception.h"

#include "parser.h"

using Replxx = replxx::Replxx;



int main(int argc, char** argv)
{
  Replxx rx;
  while(true)
  {
    auto line = std::string(rx.input("cult/graph>"));
    try
    {
      auto sexprs = parser::parseSexprs(line);
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