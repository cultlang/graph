/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/. */

#pragma once

#include "filters.hpp"

namespace ugly {
namespace filter
{
    template<typename TValue>
    struct ValueFilterGeneric
    {
    protected:
        typename TValue _value;

        template<typename TGraph>
        friend struct ValueFilterSpecialized;
    
    public:
        inline ValueFilterGeneric(TValue && value)
            : _value(std::forward<TValue>(value))
        {

        }
    };

    template<typename TGraph>
    struct ValueFilterSpecialized
    {
    protected:
        typename TGraph::EdgeData _value;
    
    public:
        inline ValueFilterSpecialized(typename TGraph::EdgeData const& value)
            : _value(value)
        {

        }

        template<typename TValue>
        inline ValueFilterSpecialized(ValueFilterGeneric<TValue> && value)
            : _value(std::move(value._value))
        {

        }

    public:
        inline bool operator()(typename TGraph::Edge const* c)
        {
            return c->data == _value;
        }
    };

    template<typename TType>
    typename ValueFilterGeneric<TType> byValue(TType && v)
    {
        return ValueFilterGeneric<TType>(std::forward<TType>(v));
    }

    template<typename TGraph, typename TValue>
    struct specialize_filter<TGraph, ValueFilterGeneric<TValue>>
    {
        using type = ValueFilterSpecialized<TGraph>;

        static inline ValueFilterSpecialized<TGraph> specialize(ValueFilterGeneric<TValue> && t)
        {
            return ValueFilterSpecialized<TGraph>(std::move(t));
        }
    };
}}
