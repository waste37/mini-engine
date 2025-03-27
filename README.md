# Mini Engine

Mini Engine is a personal project where I experiment with a simple yet flexible **Entity-Component System (ECS)** and a **deferred shading pipeline**.

## Objectives

1. Maintain a **small and contained** codebase.
2. Implement features in the **simplest and most specific way** possible.
3. Ensure a **simple data layout** that allows for **fast iteration**.

## Roadmap

### Core Features
- [x] **Entity-Component System (ECS)**
    - [x] CRUD operations for entities and components
    - [x] Component-based iterators/views
- [ ] **World Definitions**
    - [ ] Load world configurations from a JSON file
    - [ ] Resource manager: being able to load texture, model and material into game.
- [ ] **Deferred Shading Renderer**
    - [ ] Load models from JSON specifications
    - [ ] Load textures from JSON specifications
    - [ ] Implement a deferred shading pipeline
    - [ ] Add camera controls (mouse + keyboard)
- [ ] **Optimization**
    - [ ] Basic frustum culling

### Optional Enhancements
- [ ] Implement shadow mapping
- [ ] Add a basic **Physically Based Rendering (PBR)** material system

---

This project is a work in progress, and feedback are always welcome!


### Worlds

A world is defined in json, the data structure is the following:

```json

{
    "scene": {
        "entities": [
            {
                ""
            }
        ],
    }
    {
        "name": "player",
        "rigidbody": {
            "mass": 0,
            "position": [10.0, 0.0, 2.0],
            "velocity": [0, 0, 0];
        },
        "model": "path/to/model.obj",
        "material": "path/to/material.mtl",
        "shader": "path/to/shader.glsl",
    },
    {
        "rigidbody": {
            "mass": 0,
            "position": [10.0, 0.0, 2.0],
            "velocity": [0, 0, 0];
        },
        "model": "path/to/model.obj",
        "material": "path/to/material.mtl",
        "shader": "path/to/shader.glsl",
    },
    {
        "rigidbody": {
            "mass": 0,
            "position": [10.0, 0.0, 2.0],
            "velocity": [0, 0, 0];
        },
        "model": "path/to/model.obj",
        "material": "path/to/material.mtl",
        "shader": "path/to/shader.glsl",
    }
}

```

