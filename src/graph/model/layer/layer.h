/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/. */

#pragma once

namespace ugly::model::layer
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
}
