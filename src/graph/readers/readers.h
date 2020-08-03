/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/. */

#pragma once

#include <iostream>
#include <functional>

namespace ugly
{
    typedef std::function< void (std::string&, std::string&, std::string&, std::string*) > ReadNTripleFunc;

    bool read_nt(std::istream& in, ReadNTripleFunc func);
}
