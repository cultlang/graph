[Unnamed Graph Library] is an in-memory graph database for C++ with a focus on extensibility and runtime performance, it has a tinker pop style syntax (in both C++ and [WIP] from strings), and includes an optimizable query system (with a [WIP] batteries included query library).

[Read the manual.](MANUAL.md) Examples and getting started documentation is still a work in progress, for now the [`/test/graph_query.cpp` file](/test/graph_query.cpp) is the best starting point.

### Developing

We use bazel, [follow instructions for installing bazel on your platform](https://docs.bazel.build/versions/master/install.html).

To run tests run `bazel test ...`
