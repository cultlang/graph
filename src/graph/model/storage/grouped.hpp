/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/. */

#pragma once

#include <map>
#include "../../util.hpp"
#include "simple.hpp"

namespace graph
{
    template <typename T, typename TGroup>
    class GroupedStorage
    {
        std::map<TGroup, SimpleStorage<T>> _data;

    public:
        GroupedStorage() = default;
        ~GroupedStorage() = default;

        template <typename... TArgs>
        inline T& emplace_back(TGroup const& group, TArgs&&... args)
        {
            return _data.try_emplace(group).first.emplace_back(std::forward<TArgs>(args)...);
        }

        inline size_t size() const
        {
            return _data.size();
        }
    };
}
