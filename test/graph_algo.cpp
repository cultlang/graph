/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/. */

#include "catch2/catch.hpp"

#include "shared.h"

#include "graph/graph.hpp"

#include <string>

using namespace ugly;
using namespace Catch::Matchers;


TEST_CASE( "ugly::findNode", "[ugly::findNode]" )
{
    test_help::StrGraph g;

    g.addNode("test");

    CHECK(findNode(g, "test") != nullptr);
    CHECK(findNode(g, "not-test") == nullptr);
}
