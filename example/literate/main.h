/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/. */

#include <iostream>
#include <fstream>
#include <chrono>
#include <filesystem>

#include "tao/pegtl.hpp"

template <typename T, typename TRep, typename TPeriod>
T duration_cast(const std::chrono::duration<TRep, TPeriod>& duration)
{
    return duration.count() * static_cast<T>(TPeriod::num) / static_cast<T>(TRep::den);
}

template <typename TClock = std::chrono::steady_clock>
class Stopwatch
{
    typename TClock::time_point _last;

public:
    Stopwatch()
        : _last(TClock::now())
    {}

    void reset()
    {
        _last = TClock::now();
    }

    typename TClock::duration elapsed() const
    {
        return TClock::now() - _last;
    }

    std::string elapsed_str() const
    {
        return std::to_ duration_cast
    }
};