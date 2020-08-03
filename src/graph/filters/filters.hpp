/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/. */

#pragma once

namespace ugly {
namespace filter
{
    template<typename TGraph, typename TType>
    struct specialize_filter
    {
        using type = TType;

        static inline type specialize(type && t)
        {
            return std::move(t);
        }
    };
}}

#include "constant.hpp"
#include "value.hpp"

