/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/. */

#pragma once

namespace ugly::query::layer
{
    template<typename TSubLayer = void>
    class VoidLayer;

    template<typename TSubLayer>
    class VoidLayer
    {
        using SubLayer = TSubLayer;

        template<typename TBaseActual>
        class Actual
            : public TSubLayer::template Actual<TBaseActual>
        {

        };
    };

    template<>
    class VoidLayer<void>
    {
        using SubLayer = void;

        template<typename TBaseActual>
        class Actual
            : public TBaseActual
        {

        };
    };

    // TODO: replace with default list of libraries
    template<typename TSubLayer = VoidLayer<void>>
    using Default = VoidLayer<TSubLayer>;
}
