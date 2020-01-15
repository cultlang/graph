The graph library included in this repository is a header only implementation of an in memory graph database. It has some interesting properties (and will soon be split into it's own library).

1. The data model of the graph is composable and extensible. It uses a core template interface of functions and data types. By composing template types more complex data models can be built up.
2. A library of functors templated off of the graph description allows for creating custom filters and projections for use in iterating over the graph.
3. The query model of the graph is lazy and composable. It uses an interpreted query structure to allow arbitrary extension and composition of queries. Additionally a tinker-pop-esque syntax is then placed on top of this to provide a native C++ graph query language.
4. A library of query functions provide extensive power to the query system.
5. Potential additions which aren't currently planned nor implemented.

## 1. Graph Data Model

### Theory

The graph model chosen is robust, it works off of some general axioms, with a couple of assumptions about edge cases.

1. All core graph kinds can have associated data (aka `Core`).
  * In the case of a typed graph this would include types.
2. Nodes (a core graph kind) are the primary unit in a graph, and everything revolves around them.
3. Edges (a core graph kind) join togeather an arbitrary number of nodes.
4. Edges are directional, pointing **from** (outwards) a primary node **to** (inwards) all the other nodes. The primary node is always at the 0 index. Because edges can involve an arbitrary number of nodes a single node can *point at* many other nodes.
5. Edges can be inverted, because nodes can involve an arbitrary number of nodes an inverted edge allows a single node to be *pointed at* by many other nodes.
6. Labels (a core graph kind) can include an arbitrary number of nodes (and a node can have an arbitrary number of labels), this allows the graph to be grouped into categories. One view of labels is that they are a pathological case of an edge that involves many nodes.
7. Properties (a core graph kind; often shortened to Props) can be placed on a node or an edge as an additional piece of information about that object. Properties may only have a single owner. One view of properties is that they are a pathological pattern of an edge that maps to a node that has no other relationships.
8. Edges must involve 2 or more nodes (as properties and labels prevent the pathological need for other cases).

Some notable things fall out of this that are worth discussing:
* Nodes are the focus, we can confirm this by seeing that every core kind involves nodes (and no other kind involves every kind).
* Edges can't be placed in labels. The authors didn't find this use case to be especially compelling (see patterns for alterntives; can also be corrected with a mixin), it also steps on the toes of adding types to the system.
  * Props can't be placed in labels for similar reasons.
  * It is also a step towards homoginizing the model to an interesting extreme that eliminates edges.
* Labels can't have props. The authors didn't find this use case compelling (see patterns for alterntives; can also be corrected with a mixin).
  * It is also a step towards homoginizing the model to an extreme that eliminates labels (in favor of simply being edges again).
* Edges can't join Edges. The authors didn't find this compelling after the arbitrarily sized edges were added (see patterns for alterntives; could theorhetically be fixed wiht a mixin).
  * It is also a step towards homoginizing the model to an extreme that eliminates nodes.

### Designing Data Models

Designing a data model for a graph is the hardest part of using a graph library. We have attempted to provide a variety of tools for whatever data model one might be facing, as we describe below.

Designing graph models is complicated in large part because of the variety of options at one's disposal. We describe some thoughts here to solve these conundrums when using our library.

#### Concern: Source of Truth vs. Fast to Query

One common problem that is important to decide early is whether the data model is a source of truth or if it's a fast to query store of information. It could also be somewhere in between but that can be complicated if not well documented.

Consider our example Norse Gods dataset. Thor (a node) was born of an affair, he has a `parents` relationship and one of his parents has a `marriage` relationship that does not involve his other parent (to cover our bases we could add properties to each that include his birth date and the marriage's start and end date). But the key question of this concern becomes: how do we know that Thor was born of an affair?

There are two primary options in front of us (besides redesigning the data model to avoid the problem). One option is to query all of the relevant information every time (check if the parents married to each other, check the relevants dates, and so on). The other options is to compute the value and store it in the graph in easy to access locations (store a `was-affair` marker property on the `parents` edge). Either way we will need a method to compute the answer.

* Source of Truth: Rather than allow a stale value to be left in the graph, as the source of truth we would prefer that the database has the minimal set of information required to know something. This means computing the value every time we want to look it up, which while memory effecient is expensive. While more expensive to query, this has benefits for graphs that are updated often, notably taking less time to perform the write and less information to worry about updating.
* Fast to Query: By storing the value in easy to access locations we have to traverse much less of the graph, and hence have our answer faster. This potentially means storing a lot of extra information, which while computationally efficient, is memory expensive. In general this is better with static graphs, especially if we can compute them from a serialized source of truth.


#### Pattern: Marker Property

In an untyped graph a common pattern is to use a "marker" property which marks a specific node or edge as being special in some way. A simple call to the algorithmic helper `hasProp` will find it.

In typed graphs it is generally better to use types to represent this (which then have faster lookups). In keyed graphs it is easier to use a keyed property (with even an empty value) to represent this (which then have faster lookups).

#### Anti-Pattern: Label Properties

Wanting to put properties on labels is usually an anti-pattern. Here are some motivations and their solutions:

* There are a large number of nodes which share some sort of property, they are currently in a label and we want this property to be findable by all of them. This is usually an indication that there is some hidden structure involved - a common ancestor for example - that should instead be turned into it's own node (which can have properties) a number of smaller edges.
* There is an external piece of data that is related to the label that should be accessible through the graph. This solution is simple, then that object (with all of it's properties) should be the *value* of that label. Typed graphs help with this problem.
* The lablel really needs a representation in the graph. In this case one can create a special node that represents that label. Using a special type (which gets sorted quickly in typed graphs) or a marker property.

#### Anti-Pattern: Labeled Properties

Wanting to put properties in labels is usually an anti-pattern.

The primary motivation for this is invariably seperating different kinds of properties on the node. There are two solutions to this depending on how one would prefer to seperate them (including using both):

* The properties need to be typed, and hence a typed graph should be used.
* The properties need to be keyed, and hence a keyed property graph should be used.

#### Anti-Pattern: Labeled Edges

Wanting to put edges in labels is usually an anti-pattern. 

The primary motivation for this is invariably wanting to distinguish between different groups of different edges (often during traversal). Generally speaking, this distinguishment is based on "extra" information about the edge, but it is still often information (e.g. data). For example to express the fact Thor was born of an affair we would want to somehow express that on his `parents` edge relationship. There are a couple of recommended solutions:

* Expand the edge data model directly. For example the data stored in the edge could be `parents+affair`, this solution benefits from typed data, allowing the type to express what possible variants could be found.
* Add properties to the edges. This is best for an aspect-esque system where the property's existence gives you more information than it's value. For example adding a `was-affair` property to the edge. In general this implies using keyed or typed properties. This has goes back to the concern of "Source of Truth vs. Fast to Query".

#### Anti-Pattern: Edge Edges

Wanting to put edges in edges is usually an anti-pattern.

In general this implies a prety basic solution of creating a new node that represents the edge and splitting the existing edge into two different kinds of edges (plus the third the other edges was meant to represent).


### Graph Data Type Architecture

The graph data model is created by composing layers as shown here:

`Graph<GraphTyped<GraphCore<my_graph_config>>>`

This is then expanded (by graph) into the following inheritence structure:

* `Graph<GraphTyped<GraphCore<my_graph_config>>>` (`graph_final.hpp`)
  * `TFinal` = `GraphFinalizer<GraphTyped<GraphCore<my_graph_config>>>` (`graph_final.hpp`)
    * `GraphTyped<GraphCore<my_graph_config>>` (`graph_typed.hpp`)
      * `GraphCore<my_graph_config>` (`graph_core.hpp`)
  * `GraphTyped<GraphCore<my_graph_config>>::Actual<TFinal>` (`graph_typed.hpp`)
    * `GraphCore<my_graph_config>::Actual<TFinal>` (`graph_core.hpp`)

From this we can see that the graph type is constructed twice. Once with the named class wrapping the config, this is responsible for deciding what the final types and helper functions will be. Indeed `GraphFinalizer` contains the primary type interface of the resulting graph by joining together the graph types (completing the dual path hierarchy):

* Forwards `MetaFlags`
* Forwards `Data`
* Forwards `Config`
* Forwards `Core`
* Joins `Label` with `Core` and forwards
* Joins `Node` with `Core` and forwards
* Joins `Edge` with `Core` and forwards
* Joins `Prop` with `Core` and forwards

These types are then used by the `Actual` implementations to write the functions that operate across them, allocate storage, and so on.

In general the graph data type wraps all graph access to allow effecient access - though currently implementation details are exposed - regardless of what the internal data structure may become.

#### Add Functions

In general the core add functions are:

* `Label* addLabel(LabelData)`
* `Node* addNode(NodeData)`
* `Edge* addEdge(EdgeData, std::vector<Node*>)`
* `Prop* addProp(PropData, Node*)`
* `Prop* addProp(PropData, Edge*)`

`GraphTyped` adds required type id (type as determined by the config)  as a second parameter to all add functions.

`GraphKeyedProps` adds a required key argument to the `addProp` functions.

#### Update Functions

In general the update functions are:

* `void attachLabel(Node*, Label*)`
* `void attachEdge(Node*, Edge*)`
* `void attachEdge(Node*, Edge*, size_t index)`

#### Delete Functions

In general the delete functions are:

* `void deleteLabel(Label*)`
* `void deleteNode(Node*, bool destroy_edges=true)`
  If `destroy_edges` is true it destroy any participating edges, otherwise it will simply remove itself from the edges (edges with less than two partipating nodes are destroyed).
* `void deleteEdge(Edge*)`
* `void deleteProp(Prop*)`

#### Stat Functions

In general the stat functions are:

* `size_t labelCount()`
* `size_t nodeCount()`
* `size_t edgeCount()`
* `size_t propCount()`

#### Inspection Functions

In general the inspection functions are:

* `bool isLabel(Core*)`
* `bool isNode(Core*)`
* `bool isEdge(Core*)`
* `bool isProp(Core*)`
* `bool isEdgeInverted(Edge*)`

#### Getter Functions

In general the getter functions are:

* `Core getOwner(Prop*)`
* `void getLabelNodes(Label*)`
* `void getNodeLabels(Node*)`
* `void getNodeEdges(Node*)`
* `void getNodeProps(Node*)`
* `void getEdgeNodes(Edge*)`
* `void getEdgeProps(Edge*)`

These copy the whole array and are faster if you always want the whole array.

#### Iteration Functions

In general the iteration functions (mirroring most of the getter functions) are:

* `void forAllLabels(Func)`
* `void forAllNodes(Func)`
* `void forAllEdges(Func)`
* `void forAllNodesInLabel(Func)`
* `void forAllLabelsOnNode(Func)`
* `void forAllEdgesOnNode(Func)`
* `void forAllPropsOnNode(Func)`
* `void forAllNodesInEdge(Func)`
* `void forAllPropsOnEdge(Func)`

These allow for early termination with an optional boolean return value. In general these should be prefered for most filter/selection operations.

## 2. Filters and other Functors

## 3. Query Engine

The query engine is based off of the [Dagoba](http://www.aosabook.org/en/500L/dagoba-an-in-memory-graph-database.html) database.

### Graph Query Engine Architecture

The primary type is the `GraphQueryEngine` which is templated on the graph data type it is working against. It has some child types (which are hence also templated off of the graph data type), notably `Gremlin`, `Pipe`, and `PipeResult` (a variant between a gremlin or a feedback to the pipeline interpreter). The query engine stores a list of pipes, the current state of those pipes, and a `run()` function.

The `run()` function is the interpreter of the pipes. First it makes stateful versions of the pipes by calling a virtual function on `Pipe`. Then it traverses the pipe definitions until the rightmost one is done. As it traverses the pipes it calls their `pipeFunc` which returns whather that pipe is done, if it needs to "pull" more from the left (pipes that pull from a done pipe also count as done), or a gremlin (a form of result wrapper that generally stores a node) which is then passed to the next pipe. Finally it projects the resulting gremlins into a result set of nodes that is returned (TODO: add templated property projection).

We then provide a number of common `Pipe` implementations, which are split into four primary sections (besides their boilerplate memory management). They can have custom templated types which can be passed into their constructor (e.g. forwarded from syntax). They can have configuration data (e.g. what is passed into their constructor). They can have state data (which is stepup and reset by making new copys of the configured versions). And they have a pipe function which does the main logic. These can be constructed and added to a `GraphQueryEngine` by hand with `addPipe` but it is often a tedious process.

### Graph Query Syntax

We use some template magic to allow users to use a tinker pop style graph query syntax.

#### Architecture

The primary type involved in the template magic is `GraphQuery` object is created by `query()`. This object is templated off of the draph data type (e.g. `Graph<>`) and a "graph query library". The primary purpose of `GraphQuery` is to manage the memory of the `GraphQueryEngine` as the query is built up and allow the user to either store the query object or to immedietly run it.

The graph query library parameter is what allows us to have an extensible method chaining syntax. By providing a base `GraphQueryLibraryBase` (which like all graph query libraries is templated off of the graph data type and the return type of the query builder (aka `GraphQuery`)) we allow `GraphQuery` to override the primary syntax helper function: add a `Pipe` object and return the correct chaing object.

Mangement of the lifetime of lambdas passed into the library are up to the user (e.g. to prevent them from attempting to access stack values that fell out of scope). Generally they have a straight forward lifetime as a user would expect (e.g. they live for as long as the resulting query does). The design of filters other functors are meant to alliviate this concern.

## 4. Query Library

## 5. Other Ideas (aka TODOs)

### Solve Data Model Composition

* Set up the graph add functions to take `Core` instead of just `Data` to allow passing in types and other stuf as a single argument.
* Create a kind specific `*Data` to allow customizing each kind's data individually.
  * Perhaps use a configured union to keep the same organization.
* Use template `...` arguments to allow stacking them.

### Evented

It should be possible to add signals to nodes (and potentially edges, properties, and labels) which allows them to notify subscribers. In general though this seems like overkill, with targeted events in typed objects being the better user space solution. Might still be useful as a building block.

### Iterators

It should be possible to create iterators (to use in place of the the `get*` and `forAll*` functions). In theory this would allow more memory effecient (and hence performance effecient) queries.

One way of invalidating iterators is by versioning relevant data structures.

### Source of Truth Notation

It should be possible to create a graph layer that allows us to mark which pieces of information are sources of truth and which are cached. Which would be tangentially useful for serializtion.

A next step would then be ensuring that all computed values are updated whenever the sources of truth are. This would be a complicated mix-in but theorheticall very much worth the effort. Could use an evented graph.