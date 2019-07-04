/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/. */

#pragma once

#include <stdexcept>

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
        inline bool invoke_return_bool_or_true(Func func, Args&& ...args)
        {
            if constexpr (std::is_void<typename std::invoke_result<Func, Args...>::type>::value)
            {
                func(std::forward<Args>(args)...);
                return true;
            }
            else
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