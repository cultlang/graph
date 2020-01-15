/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/. */

#pragma once

#include <deque>
#include "../../util.hpp"

namespace graph
{
    template <typename T>
    class SimpleStorage
    {
        std::deque<T> _data;

    public:
        SimpleStorage() = default;
        ~SimpleStorage() = default;

        template <typename... TArgs>
        inline T& emplace_back(TArgs&&... args)
        {
            return _data.emplace_back(std::forward<TArgs>(args)...);
        }

        inline size_t size() const
        {
            return _data.size();
        }

    public:
        class const_iterator
            : public std::deque<T>::const_iterator
        {
            public:
                const_iterator() = default;
                const_iterator(typename std::deque<T>::const_iterator const& that)
                    : std::deque<T>::const_iterator(that)
                {}
                const_iterator(typename std::deque<T>::const_iterator && that)
                    : std::deque<T>::const_iterator(that)
                {}
                ~const_iterator() = default;
        };

        inline const_iterator begin() const
        {
            return _data.begin();
        }

        inline const_iterator end() const
        {
            return _data.end();
        }
    };
}
