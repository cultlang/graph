/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/. */

#pragma once

#include <vector>
#include <deque>
#include <string>

#include "../util.hpp"

namespace ugly {
namespace model
{
    struct Empty { };

    template<typename TCoreData, typename TPropKeyData>
    struct DataCoreConfigBuilder
    {
        using Node = TCoreData;
        using Edge = TCoreData;
        using Path = TCoreData;

        using Label = TCoreData;
        using Prop = TCoreData;

        using PropKey = TPropKeyData;
    };

    template<typename TPropData, typename TPropKeyData, typename TOtherData=Empty>
    struct DataPropDataConfigBuilder
    {
        using Node = TOtherData;
        using Edge = TOtherData;
        using Path = TOtherData;

        using Label = TOtherData;
        using Prop = TPropData;

        using PropKey = TPropKeyData;
    };

    template<typename TStore>
    struct StorageConfigBuilder
    {
        using Store = TStore;
    };

    template<typename TDataConfig, typename TStorageConfig>
    struct ConfigBuilder
    {
        using Data = TDataConfig;
        using Storage = TStorageConfig;
    };
}}
