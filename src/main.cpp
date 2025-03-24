#include <WorldRegistry.h>
// #include <LinearAlgebra.h>
#include <iostream>
#include <cassert>

#include <chrono>

class Timer {
public:
    Timer() { 
        std::cout << "start stopwatch\n";
        beg = std::chrono::steady_clock::now();
    }
    ~Timer() {
        const auto dt = elapsed();
        std::cout << "elapsed: " << dt.count() / 1000.0 << "ms" << std::endl;
    }

    std::chrono::nanoseconds elapsed() {
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

void BasicBenchmark() {
    WorldRegistry world{};
    world.Create();

    for (i32 i = 0; i < 10000; ++i) {
        Entity e = world.CreateEntity<Position, Velocity>();
        Position *pos = world.GetComponentData<Position>(e);
        pos->x = 10.0f + i;
        pos->y = 11.0f + i;

        Velocity *v = world.GetComponentData<Velocity>(e);
        v->dx = -10.0f + i;
        v->dy = -11.0f + i;
    }

    for (i32 i = 0; i < 10000; ++i) {
        Entity e = world.CreateEntity<Position, Health>();
        Position *pos = world.GetComponentData<Position>(e);
        pos->x = 20.0f + i;
        pos->y = 21.0f + i;
        Health *hp= world.GetComponentData<Health>(e);
        hp->health = 0;
    }

    for (i32 i = 0; i < 1000; ++i) {
        Entity e = world.CreateEntity<Velocity, Health>();
        Velocity *pos = world.GetComponentData<Velocity>(e);
        pos->dx = 30.0f + i;
        pos->dy = 31.0f + i;
        Health *hp = world.GetComponentData<Health>(e);
        hp->health = 0;
    }

    {
        auto t = Timer();
        for (WorldRegistry::ViewIterator it = world.View<Position>(); !it.AtEnd(); it.Next()) {
            Position *p = it.Get<Position>();
            p->x = 0.0f;
            p->y = 0.0f;
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


struct Vec3 : public IComponent<Vec3> {
    Vec3(f32 v) {
        m_Data[0] = v;
        m_Data[1] = v;
        m_Data[2] = v;
    }

    Vec3(f32 x, f32 y, f32 z) {
        m_Data[0] = x;
        m_Data[1] = y;
        m_Data[2] = z;
    }

    Vec3(const Vec3 &v) {
        m_Data[0] = v.m_Data[0];
        m_Data[1] = v.m_Data[1];
        m_Data[2] = v.m_Data[2];
    }

    inline Vec3 &operator=(const Vec3 &v) {
        m_Data[0] = v.m_Data[0];
        m_Data[1] = v.m_Data[1];
        m_Data[2] = v.m_Data[2];

        return *this;
    }

    inline f32 &X() { return m_Data[0]; };
    inline f32 X() const { return m_Data[0]; };
    inline f32 &Y() { return m_Data[1]; };
    inline f32 Y() const { return m_Data[1]; };
    inline f32 &Z() { return m_Data[2]; };
    inline f32 Z() const { return m_Data[2]; };
    inline f32 &operator[](usize i) {
        assert(i < 3);
        return m_Data[i];
    } 
    inline f32 operator[](usize i) const {
        assert(i < 3);
        return m_Data[i];
    } 
protected: 
    f32 m_Data[3];
};

inline Vec3 operator+(const Vec3 &u, const Vec3 &v) {
    return { u[0] + v[0], u[1] + v[1], u[2] + v[2] };
}

inline Vec3 operator-(const Vec3 &u, const Vec3 &v) {
    return { u[0] - v[0], u[1] - v[1], u[2] - v[2] };
}

inline Vec3 operator*(f32 s, const Vec3 &v) {
    return { s * v[0], s * v[1], s * v[2] };
}

inline Vec3 operator*(const Vec3 &v, f32 s) {
    return s * v;
}

inline Vec3 operator/(const Vec3 &v, f32 s) {
    return (1.0f / s) * v;
}


inline std::ostream &operator<<(std::ostream &os, const Vec3 &v) {
    return os << "(" << v[0] << ", " << v[1] << ", " << v[2] << ")";
}

using Point3 = Vec3;

struct Color : public Vec3 {
    f32 X() = delete;
    f32 Y() = delete;
    f32 Z() = delete;

    f32 R() {
        return m_Data[0];
    }
    f32 G() {
        return m_Data[1];
    }
    f32 B() {
        return m_Data[2];
    }
};

int main() {
    // loading stuff??
    Vec3 u = {1, 2, 3};

    Color c = {0};
    std::cout << c << "+" << u << "=" << u + c << std::endl;
}

