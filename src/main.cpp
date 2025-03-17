#include <WorldRegistry.h>
#include <iostream>

// Base class template using CRTP

// Specializations inheriting from UniqueIDProvider

struct Position : public IComponent<Position> {
    f32 x, y;
};

struct Velocity : public IComponent<Velocity> {
    f32 dx, dy;
};

struct Health {
    i32 health;
    i32 armor;
};

void bench() {
    WorldRegistry *world = new WorldRegistry;
    world->Create();
    world->RegisterComponent("Position", Position::ID(), sizeof(Position));
    world->RegisterComponent("Velocity", Velocity::ID(), sizeof(Velocity));
    // Entity e1 = world->CreateEntity(positionmask | velocitymask);
    // Entity e2 = world->CreateEntity(positionmask | velocitymask);

    Position *pos;
    Velocity *v;
    for (usize i = 0; i < 1000; ++i) {
        Entity e = world->CreateEntity({1, 2});
        pos = (Position*)world->GetComponentData(e, 1);
        pos->x = i + 10.0;
        pos->y = i + 11.0;
        v = (Velocity*)world->GetComponentData(e, 2);
        v->dx = i + 10.0;
        v->dy = i + 11.0;
    }

    for (usize i = 0; i < 1000; ++i) {
        Entity e = world->CreateEntity({1});
        pos = (Position*)world->GetComponentData(e, 1);
        pos->x = i + 20.0;
        pos->y = i + 21.0;
    }
    // world->DebugRegisteredComponents();
    // world->DebugRegisteredTypes();

    world->Destroy();
}

int main() {
    bench();

    // WorldRegistry *world = new WorldRegistry;
    // world->Create();
    // world->RegisterComponent("Position", Position::ID(), sizeof(Position));
    // world->RegisterComponent("Velocity", Velocity::ID(), sizeof(Velocity));
    // Entity e = world->CreateEntity({1, 2});
    // Position *pos;
    // pos = (Position*)world->GetComponentData(e, 1);
    // pos->x = 20.0;
    // pos->y = 21.0;

    // pos = (Position*)world->GetComponentData(e, 1);
    // printf("e pos: %f, %f\n", pos->x, pos->y);
    // world->DebugRegisteredComponents();
    // world->DebugRegisteredTypes();
    // world->DebugRegisteredEntities();
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
