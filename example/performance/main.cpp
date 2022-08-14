/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/. */

#include "main.h"

/* Now we will discuss what a graph object is. We use a templated graph system which allows for
 * data model templates to be mixed togeather into a final graph object which sill be responsible
 * for all storage and access of the graph and it's elements.
 * 
 * Below we describe a basic string graph. That is a graph where every element of the graph is uses
 * a standard C++ string as it's value.
 * 
 * This is the core of any graph and is relatively flexible. Instead of a value a pointer to an 
 * abstract base type could be used, or one of the mixins that provides better support for diverse
 * types.
 */
using StrGraphConfig = ugly::model::ConfigBuilder<
    ugly::model::DataCoreConfigBuilder<std::string, std::string>,
    ugly::model::StorageConfigBuilder<ugly::storage::SimpleStorage>
>;
using StrGraph = ugly::model::PathPropertyGraph< StrGraphConfig >;

/* TODO For a more complex example we will create a simple tagged record and use it as part of a typed
 * graph. The design of our data model allows for an arbitrary number of orthoganol mixins, though
 * performance may suffer due to memory layout if care is not taken, and mixins must cooperate if
 * they have complicated interactions.
 */

int main(int argc, char** argv)
{
    Stopwatch sw;

    sw.reset();

    std::cout << sw.elapsed_str() << std::endl;

    return 0;
}
