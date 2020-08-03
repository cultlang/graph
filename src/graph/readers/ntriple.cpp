/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/. */
#pragma once

#include <iostream>

#include "tao/pegtl.hpp"

#include "readers.h"

namespace nquads
{
    namespace p = tao::pegtl;

    struct hash_comment : p::until< p::eolf > {};

    struct iriref : p::seq< p::one< '<' >, p::star<p::not_one< '>' >>, p::one< '>' > > {};
    struct literal : p::seq< p::one< '"' >, p::star<p::not_one< '"' >>, p::one< '"' > > {};

    struct subject : p::sor<iriref> {};
    struct predicate : p::sor<iriref> {};
    struct object : p::sor<iriref, literal> {};
    struct label : p::sor<iriref> {};

    struct triple : p::seq< subject, p::space, predicate, p::space, object, p::space > {};
    struct quad : p::seq< subject, p::space, predicate, p::space, object, p::space, label, p::space > {};
    struct statement : p::seq< p::sor< quad, triple >, p::one< '.' >, p::star< p::space > > {};

    struct document : p::until< p::eof, p::must< statement > > {};


    template <typename Rule>
    struct action
    {};

    template <>
    struct action<subject>
    {
        template< typename Input >
        static void apply( const Input& in, ugly::ReadNTripleFunc& fn, std::vector<std::string>& vec )
        {
            vec[0] = in.string();
        }
    };
    template <>
    struct action<predicate>
    {
        template< typename Input >
        static void apply( const Input& in, ugly::ReadNTripleFunc& fn, std::vector<std::string>& vec )
        {
            vec[1] = in.string();
        }
    };
    template <>
    struct action<object>
    {
        template< typename Input >
        static void apply( const Input& in, ugly::ReadNTripleFunc& fn, std::vector<std::string>& vec )
        {
            vec[2] = in.string();
        }
    };

    template <>
    struct action<statement>
    {
        template< typename Input >
        static void apply( const Input& in, ugly::ReadNTripleFunc& fn, std::vector<std::string>& vec )
        {
            fn(vec[0], vec[1], vec[2], nullptr);
        }
    };
}

namespace ugly
{
    bool read_nt(std::istream& in, ReadNTripleFunc func)
    {
        tao::pegtl::istream_input parsein( in, 1024 * 1024 * 10, "" );

        std::vector<std::string> entryVec(4);
        return tao::pegtl::parse< nquads::document, nquads::action >( parsein, func, entryVec );
    }
}
