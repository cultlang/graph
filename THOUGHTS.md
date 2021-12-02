Redo the storage, try 37, thoughts:

- Nodes are stored ala an Archetype Ecs Table
- Properties are Components in the Archetype
- Edges are archetypes ala nodes.
  - three kinds, one->many, many->one, all to each other.
- Labels are a sort list / linked list within the archetype for fast partial iteration.
- entity ids optional? (use tag bits? label bits?)

