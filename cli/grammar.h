#pragma once 
#include <tao/pegtl.hpp>
#include <tao/pegtl/contrib/parse_tree.hpp>

#include "semantics.h"

namespace pegtl = tao::pegtl;
using namespace pegtl;
namespace grammar {
  struct whitespace : pegtl::star<space> {};
  struct call_open : pegtl::one< '(' > {};
	struct call_close : pegtl::one < ')' > {};
  struct singlestring : if_must< one< '"' >, until< one< '"' > > > {};

  struct literal: sor<singlestring> {};

  struct variable: pegtl::identifier {};

  struct params: list<literal, one<','>, blank> {};

  struct call
      : pegtl::seq<identifier, call_open, params, call_close> //params, 
  {};

  struct grammar
    : pegtl::must<variable, pegtl::one<'.'>, pegtl::list<call, pegtl::one<'.'>, blank>, pegtl::eolf >
  {};


   template< typename Rule >
   struct action
   {};

  template<>
  struct action< literal >
  {
    template< typename Input >
    inline static void apply( const Input& in, std::shared_ptr<semantics::ParseTree> s )
    {
      auto& sexpr = s->sexprs.back();

      sexpr.push_back(in.string());
    }
  };

  template<>
  struct action< identifier >
  {
    template< typename Input >
    inline static void apply( const Input& in, std::shared_ptr<semantics::ParseTree> s )
    {
      s->sexprs.push_back(std::vector<std::string>());
      auto& sexpr = s->sexprs.back();
      sexpr.push_back(in.string());
    }
  };

  template<>
  struct action< variable>
  {
    template< typename Input >
    inline static void apply( const Input& in, std::shared_ptr<semantics::ParseTree> s )
    {
      s->variable = in.string();
    }
  };
}