/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/. */

#pragma once

#include "graph/model/layer/layer.h"

namespace ugly::config
{
    struct EmptyType
    {

    };

    template<typename TValue>
    struct SimpleValueType
    {
        TValue value;

        bool operator ==(SimpleValueType<TValue> const& other) const
        {
            return this->value == other.value;
        }
    };

    template<typename TPointer>
    struct VirtualPointerType
    {
        TPointer* value;
        
        // TODO: requires check pointer base type for comparison operator
        bool operator ==(VirtualPointerType<TPointer> const& other) const
        {
            return this->value == other.value;
        }
    };



    struct GraphConfigurationDefault
    {
        using DefaultType = ugly::config::EmptyType;

        // optional overrides
        //using NodeType = ugly::config::EmptyType;
        //using EdgeType = ugly::config::EmptyType;
        //using PropType = ugly::config::EmptyType;
        //using LabelType = ugly::config::EmptyType;

        using Layers = ugly::model::layer::VoidLayer<>;
    };


    template<typename TGraphConfig>
    struct GraphConfigurationResolver
    {
        // TODO helper resolutions
        // TGraphConfig::NodeType || TGraphConfig::DefaultType
    };
}
