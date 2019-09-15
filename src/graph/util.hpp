/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/. */

#pragma once

#include <stdexcept>
#include "stdext/exception.h"
#include "stdext/introspection.hpp"

namespace graph
{
    class graph_error
        : public std::runtime_error
    {
        using std::runtime_error::runtime_error;
    };

    namespace _detail
    {
        template<typename Func,
            typename... Args>
        inline std::enable_if_t<std::is_void<std::invoke_result_t<Func, Args...>>::value,
            bool> invoke_return_bool_or_true(Func func, Args&& ...args)
        {
            func(std::forward<Args>(args)...);
            return true;
        }

        template<typename Func,
            typename... Args>
        inline std::enable_if_t<!std::is_void<std::invoke_result_t<Func, Args...>>::value,
            bool> invoke_return_bool_or_true(Func func, Args&& ...args)
        {
            return func(std::forward<Args>(args)...);
        }
                
    }

    enum class GraphKind : uint8_t
    {
        Unknown = 0,
        Label = 1,
        Node = 2,
        Edge = 3,
        Prop = 4,
    };
}