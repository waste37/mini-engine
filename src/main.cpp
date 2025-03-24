#include <WorldRegistry.h>
// #include <LinearAlgebra.h>
#include <Geometry.h>
#include <iostream>

#include <filesystem>
#include <fstream>

#include <chrono>

class Timer {
public:
    Timer()
    {
        std::cout << "start stopwatch\n";
        beg = std::chrono::steady_clock::now();
    }
    ~Timer()
    {
        const auto dt = elapsed();
        std::cout << "elapsed: " << dt.count() / 1000.0 << "ms" << std::endl;
    }

    std::chrono::nanoseconds elapsed()
    {
        end = std::chrono::steady_clock::now();
        return std::chrono::nanoseconds(end - beg);
    }

    void reset() { beg = std::chrono::steady_clock::now(); }

private:
    std::chrono::steady_clock::time_point beg, end;
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

void BasicBenchmark()
{
    WorldRegistry world{};
    world.Create();

    for (i32 i = 0; i < 10000; ++i) {
        Entity e = world.CreateEntity<Position, Velocity>();
        Position* pos = world.GetComponentData<Position>(e);
        pos->x = 10.0f + i;
        pos->y = 11.0f + i;

        Velocity* v = world.GetComponentData<Velocity>(e);
        v->dx = -10.0f + i;
        v->dy = -11.0f + i;
    }

    for (i32 i = 0; i < 10000; ++i) {
        Entity e = world.CreateEntity<Position, Health>();
        Position* pos = world.GetComponentData<Position>(e);
        pos->x = 20.0f + i;
        pos->y = 21.0f + i;
        Health* hp = world.GetComponentData<Health>(e);
        hp->health = 0;
    }

    for (i32 i = 0; i < 1000; ++i) {
        Entity e = world.CreateEntity<Velocity, Health>();
        Velocity* pos = world.GetComponentData<Velocity>(e);
        pos->dx = 30.0f + i;
        pos->dy = 31.0f + i;
        Health* hp = world.GetComponentData<Health>(e);
        hp->health = 0;
    }

    {
        auto t = Timer();
        for (WorldRegistry::ViewIterator it = world.View<Position>(); !it.AtEnd(); it.Next()) {
            Position* p = it.Get<Position>();
            p->x = 0.0f;
            p->y = 0.0f;
        }

        for (WorldRegistry::ViewIterator it = world.View<Health>(); !it.AtEnd(); it.Next()) {
            Health* p = it.Get<Health>();
            p->health++;
        }

        for (WorldRegistry::ViewIterator it = world.View<Position, Health>(); !it.AtEnd(); it.Next()) {
            Health* hp = it.Get<Health>();
            Position* p = it.Get<Position>();
            hp->health++;
            p->x = 0;
        }

        for (WorldRegistry::ViewIterator it = world.View<Velocity>(); !it.AtEnd(); it.Next()) {
            Velocity* p = it.Get<Velocity>();
            p->dx = 0.0;
            p->dy = 0.0;
        }
    }

    world.DebugRegisteredComponents();
    world.DebugRegisteredTypes();
    world.Destroy();
}

struct Mesh {
    Vector<Vec3> vertices;
    Vector<Vec3> normals;
    Vector<i32> indices;
};

usize FileSize(const char* filename)
{
    std::error_code ec;
    return std::filesystem::file_size(filename, ec);
}

usize ReadFile(char* buf, usize size, const char* filename)
{
    std::ifstream in(filename, std::ios::binary);
    in.read(buf, size);
    usize read_bytes = in ? size : in.gcount();
    buf[read_bytes] = '\0';
    in.close();
    return read_bytes;
}

i32 main()
{
    usize filesize = FileSize("assets/cube.obj");
    std::cout << filesize << std::endl;
    char* buf = new char[filesize + 1];
    if (filesize != ReadFile(buf, filesize, "assets/cube.obj")) {
        std::cout << "problems reading\n";
    }
    std::cout << buf << "#" << std::endl;
    delete[] buf;
}

