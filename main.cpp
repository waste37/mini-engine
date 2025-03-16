#include <WorldRegistry.h>
#include <iostream>
struct Position {
    f32 x, y;
};

struct Velocity {
    f32 dx, dy;
};

int main() {
    WorldRegistry world{};
    world.Create();
    u64 positionmask = world.RegisterComponent(sizeof(Position));
    u64 velocitymask = world.RegisterComponent(sizeof(Velocity));
    Entity e1 = world.CreateEntity(positionmask | velocitymask);
    Entity e2 = world.CreateEntity(positionmask | velocitymask);

    Position *pos = (Position*)world.GetComponentData(e1, positionmask);
    pos->x = 10.0;
    pos->y = 11.0;

    pos = (Position*)world.GetComponentData(e2, positionmask);
    pos->x = 20.0;
    pos->y = 21.0;


    pos = (Position*)world.GetComponentData(e1, positionmask);
    printf("e1 pos: %f, %f\n", pos->x, pos->y);

    pos = (Position*)world.GetComponentData(e2, positionmask);
    printf("e2 pos: %f, %f\n", pos->x, pos->y);

    world.DebugRegisteredComponents();
    world.DebugRegisteredTypes();
    world.DebugRegisteredEntities();
    world.Destroy();
}
