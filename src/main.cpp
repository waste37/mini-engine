#include <WorldRegistry.h>
#include <iostream>

// Base class template using CRTP

// Specializations inheriting from UniqueIDProvider

struct Position {
    f32 x, y;
};

struct Velocity {
    f32 dx, dy;
};

struct Health {
    i32 health;
    i32 armor;
};

void bench() {
    WorldRegistry *world = new WorldRegistry;
    world->Create();
    u64 positionmask = world->RegisterComponent(sizeof(Position));
    u64 velocitymask = world->RegisterComponent(sizeof(Velocity));
    // Entity e1 = world->CreateEntity(positionmask | velocitymask);
    // Entity e2 = world->CreateEntity(positionmask | velocitymask);

    Position *pos;
    for (usize i = 0; i < 500; ++i) {
        Entity e = world->CreateEntity(positionmask | velocitymask);
        pos = (Position*)world->GetComponentData(e, positionmask);
        pos->x = i + 10.0;
        pos->y = i + 11.0;
    }
    for (usize i = 0; i < 800; ++i) {
        Entity e = world->CreateEntity(positionmask);
        std::cout << e.Index << std::endl;
        pos = (Position*)world->GetComponentData(e, positionmask);
        pos->x = i + 10.0;
        pos->y = i + 11.0;
    }
    //world->Destroy();
}

int main() {
    bench();

    // pos = (Position*)world.GetComponentData(e2, positionmask);
    // pos->x = 20.0;
    // pos->y = 21.0;


    // pos = (Position*)world.GetComponentData(e1, positionmask);
    // printf("e1 pos: %f, %f\n", pos->x, pos->y);

    // pos = (Position*)world.GetComponentData(e2, positionmask);
    // printf("e2 pos: %f, %f\n", pos->x, pos->y);

    // world.DeleteEntity(e1);
    // pos = (Position*)world.GetComponentData(e2, positionmask);
    // printf("e2 pos: %f, %f\n", pos->x, pos->y);
    // pos = (Position*)world.GetComponentData(e1, positionmask);
    // printf("e1 doesn't exist: %p\n", pos);
    // Entity e3 = world.CreateEntity(positionmask);
    // printf("e3 has id: %u\n", e3.Index);

    // world.DebugRegisteredComponents();
    // world.DebugRegisteredTypes();
    // world.DebugRegisteredEntities();

    // std::cout << Position::ID() << std::endl;
    // std::cout << Velocity::ID() << std::endl;
    // std::cout << Health::ID() << std::endl;
    // std::cout << Position::ID() << std::endl;
}
