#include <WorldRegistry.h>
#include <iostream>

#include <ctime>

class Timer {
public:
    Timer() { 
        std::cout << "start stopwatch\n";
        clock_gettime(CLOCK_REALTIME, &beg_); 
    }
    ~Timer() {
        double dt = elapsed();
        std::cout << "elapsed: " << dt * 1000 << "ms" << std::endl;
    }

    double elapsed() {
        clock_gettime(CLOCK_REALTIME, &end_);
        return end_.tv_sec - beg_.tv_sec +
            (end_.tv_nsec - beg_.tv_nsec) / 1000000000.;
    }

    void reset() { clock_gettime(CLOCK_REALTIME, &beg_); }

private:
    timespec beg_, end_;
};


struct Position : public IComponent<Position> {
    f32 x, y;
};

struct Velocity : public IComponent<Velocity> {
    f32 dx, dy;
};

struct Health : public IComponent<Health> {
    i32 health;
    i32 armor;
};

void BasicBenchmark() {
    WorldRegistry world{};
    world.Create();

    for (usize i = 0; i < 10000; ++i) {
        Entity e = world.CreateEntity<Position, Velocity>();
        Position *pos = world.GetComponentData<Position>(e);
        pos->x = 10.0 + i;
        pos->y = 11.0 + i;

        Velocity *v = world.GetComponentData<Velocity>(e);
        v->dx = -10.0 + i;
        v->dy = -11.0 + i;
    }

    for (usize i = 0; i < 10000; ++i) {
        Entity e = world.CreateEntity<Position, Health>();
        Position *pos = world.GetComponentData<Position>(e);
        pos->x = 20.0 + i;
        pos->y = 21.0 + i;
        Health *hp= world.GetComponentData<Health>(e);
        hp->health = 0;
    }

    for (usize i = 0; i < 1000; ++i) {
        Entity e = world.CreateEntity<Velocity, Health>();
        Velocity *pos = world.GetComponentData<Velocity>(e);
        pos->dx = 30.0 + i;
        pos->dy = 31.0 + i;
        Health *hp = world.GetComponentData<Health>(e);
        hp->health = 0;
    }

    {
        auto t = Timer();
        for (WorldRegistry::ViewIterator it = world.View<Position>(); !it.AtEnd(); it.Next()) {
            Position *p = it.Get<Position>();
            p->x = 0.0;
            p->y = 0.0;
        }

        for (WorldRegistry::ViewIterator it = world.View<Health>(); !it.AtEnd(); it.Next()) {
            Health *p = it.Get<Health>();
            p->health++;
        }

        for (WorldRegistry::ViewIterator it = world.View<Position, Health>(); !it.AtEnd(); it.Next()) {
            Health *hp = it.Get<Health>();
            Position *p = it.Get<Position>();
            hp->health++;
            p->x = 0;
        }

        for (WorldRegistry::ViewIterator it = world.View<Velocity>(); !it.AtEnd(); it.Next()) {
            Velocity *p = it.Get<Velocity>();
            p->dx = 0.0;
            p->dy = 0.0;
        }
    }

    world.DebugRegisteredComponents();
    world.DebugRegisteredTypes();
    world.Destroy();
}



int main() {
    // loading stuff??
}

