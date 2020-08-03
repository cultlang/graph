/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/. */

#pragma once

#include "filters.hpp"

namespace ugly {
namespace filter
{
    template<bool TConstant>
    struct ConstantFilterGeneric
    {
        inline static const bool value = TConstant;

    public:
        inline ConstantFilterGeneric() = default;

    public:
        template<typename... TArgs>
        inline bool operator()(TArgs... args)
        {
            return value;
        }
    };

    template<bool TConstant>
    typename ConstantFilterGeneric<TConstant> constant()
    {
        return ConstantFilterGeneric<TConstant>();
    }
}}
