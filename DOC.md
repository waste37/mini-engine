# Documentation

This documentation is intended to be used when reading the code, rather than for an end user.

## Entity Component System

All entity data is stored inside a WorldRegistry.
The operations supported are:

- `RegisterComponent(component_size)`:
- `CreateEntity(type)`
- `DeleteEntity(id)`
- `GetComponentData(id, component_mask)`


when an entity is created, we:

1. find the entity id, which can be either:
    - a progressive index
    - an index of a previously deleted entity

2. register it into EntityInfo, if it was already existing update, if not, grow the array and set it.
2. register the chunklist for the entity type if it doesn't exist, or find a pointer to it if it exsits
3. push the entity inside the chunklist
2. store the entity info for the
