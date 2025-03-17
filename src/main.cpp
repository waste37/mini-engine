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

struct Health : public IComponent<Health>{
    i32 health;
    i32 armor;
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

    world.DeleteEntity(e1);
    pos = (Position*)world.GetComponentData(e2, positionmask);
    printf("e2 pos: %f, %f\n", pos->x, pos->y);
    pos = (Position*)world.GetComponentData(e1, positionmask);
    printf("e1 doesn't exist: %p\n", pos);
    Entity e3 = world.CreateEntity(positionmask);
    printf("e3 has id: %u\n", e3.Index);

    world.DebugRegisteredComponents();
    world.DebugRegisteredTypes();
    world.DebugRegisteredEntities();
    world.Destroy();

    std::cout << Position::ID() << std::endl;
    std::cout << Velocity::ID() << std::endl;
    std::cout << Health::ID() << std::endl;
    std::cout << Position::ID() << std::endl;
}
