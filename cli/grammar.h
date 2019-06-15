#pragma once 
#include <tao/pegtl.hpp>
#include <tao/pegtl/contrib/parse_tree.hpp>

namespace pegtl = tao::pegtl;
using namespace pegtl;
namespace grammar {
  struct whitespace : pegtl::star<space> {};
  struct call_open : pegtl::one< '(' > {};
	struct call_close : pegtl::one < ')' > {};
  struct singlestring : if_must< one< '"' >, until< one< '"' > > > {};

  struct literal: sor<singlestring> {};

  

  struct params: list<literal, one<','>, blank> {};

  struct call
      : pegtl::seq<identifier, call_open, params, call_close> //params, 
  {};

   struct grammar
      : pegtl::must<pegtl::list<call, pegtl::one<'.'>, blank>, pegtl::eolf >
   {};


   template< typename Rule >
   struct action
   {};

  
  template< typename Rule > struct selector : std::false_type {};
  //template<> struct selector< grammar > : std::true_type {};
  template<> struct selector< identifier > : std::true_type {};
  template<> struct selector< call > : std::true_type {};
  template<> struct selector< params > : std::true_type {};
  template<> struct selector< literal > : std::true_type {};

}