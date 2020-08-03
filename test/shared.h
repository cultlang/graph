/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/. */

#pragma once

#include "graph/graph.hpp"

#include <string>

#ifdef __clang__
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wunused-variable"
#endif

namespace test_help
{
    using StrGraphConfig = ugly::model::ConfigBuilder<
        ugly::model::DataCoreConfigBuilder<std::string, std::string>,
        ugly::model::StorageConfigBuilder<ugly::storage::SimpleStorage>
    >;
    using StrGraph = ugly::model::PathPropertyGraph< StrGraphConfig >;

    void fillStrGraphWithNorse(StrGraph & g);
}
