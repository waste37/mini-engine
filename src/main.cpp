#include <WorldRegistry.h>
#include <iostream>

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

int main() {
    WorldRegistry *world = new WorldRegistry;
    world->Create();
    Entity e1 = world->CreateEntity<Position, Velocity>();
    Position *pos = world->GetComponentData<Position>(e1);
    pos->x = 10.0;
    pos->y = 11.0;

    Entity e2 = world->CreateEntity<Position, Velocity>();
    pos = world->GetComponentData<Position>(e2);
    pos->x = 20.0;
    pos->y = 21.0;

    Entity e3 = world->CreateEntity<Position, Velocity>();
    pos = world->GetComponentData<Position>(e3);
    pos->x = 30.0;
    pos->y = 31.0;

    world->DeleteEntity(e1);

    pos = world->GetComponentData<Position>(e3);
    printf("e3 has pos = %f %f", pos->x, pos->y);

    e1 = world->CreateEntity<Position, Velocity>();
    printf("e1 now has id: %u", e1.Index);

    world->DebugRegisteredComponents();
    world->DebugRegisteredTypes();
    world->DebugRegisteredEntities();

    world->Destroy();
}
