[Unnamed Graph Library] is an in-memory graph database for C++ with a focus on extensibility and runtime performance. It's primary purpose is to host the reflection system of the cult programming language. It has a tinker pop style syntax (in both C++ and from text [WIP]), and includes an optimizable query system (with a batteries included query library [WIP]).

[Read the manual.](MANUAL.md) Examples and getting started documentation is still a work in progress, for now the [`/test/graph_query.cpp` file](/test/graph_query.cpp) is the best starting point.

### Developing

We use bazel, [follow instructions for installing bazel on your platform](https://docs.bazel.build/versions/master/install.html).

To run tests run `bazel test ... --test_output=errors --enable_runfiles` (if on windows: from an admin powershell)
