#include <cassert>

struct Vec3 {
    Vec3() {
        m_Data[0] = 0.0f;
        m_Data[1] = 0.0f;
        m_Data[2] = 0.0f;
    }

    Vec3(f32 v)
    {
        m_Data[0] = v;
        m_Data[1] = v;
        m_Data[2] = v;
    }

    Vec3(f32 x, f32 y, f32 z)
    {
        m_Data[0] = x;
        m_Data[1] = y;
        m_Data[2] = z;
    }

    Vec3(const Vec3& v)
    {
        m_Data[0] = v.m_Data[0];
        m_Data[1] = v.m_Data[1];
        m_Data[2] = v.m_Data[2];
    }

    inline Vec3& operator=(const Vec3& v)
    {
        m_Data[0] = v.m_Data[0];
        m_Data[1] = v.m_Data[1];
        m_Data[2] = v.m_Data[2];

        return *this;
    }

    inline f32& X() { return m_Data[0]; };
    inline f32 X() const { return m_Data[0]; };
    inline f32& Y() { return m_Data[1]; };
    inline f32 Y() const { return m_Data[1]; };
    inline f32& Z() { return m_Data[2]; };
    inline f32 Z() const { return m_Data[2]; };
    inline f32& operator[](usize i)
    {
        assert(i < 3);
        return m_Data[i];
    }
    inline f32 operator[](usize i) const
    {
        assert(i < 3);
        return m_Data[i];
    }
protected:
    f32 m_Data[3];
};

inline Vec3 operator+(const Vec3& u, const Vec3& v)
{
    return { u[0] + v[0], u[1] + v[1], u[2] + v[2] };
}

inline Vec3 operator-(const Vec3& u, const Vec3& v)
{
    return { u[0] - v[0], u[1] - v[1], u[2] - v[2] };
}

inline Vec3 operator*(f32 s, const Vec3& v)
{
    return { s * v[0], s * v[1], s * v[2] };
}

inline Vec3 operator*(const Vec3& v, f32 s)
{
    return s * v;
}

inline Vec3 operator/(const Vec3& v, f32 s)
{
    return (1.0f / s) * v;
}


inline std::ostream& operator<<(std::ostream& os, const Vec3& v)
{
    return os << "(" << v[0] << ", " << v[1] << ", " << v[2] << ")";
}

using Point3 = Vec3;

struct Color : public Vec3 {
    f32 X() = delete;
    f32 Y() = delete;
    f32 Z() = delete;

    inline f32 R() const { return m_Data[0]; }
    inline f32 G() const { return m_Data[1]; }
    inline f32 B() const { return m_Data[2]; }
    inline f32& R() { return m_Data[0]; }
    inline f32& G() { return m_Data[1]; }
    inline f32& B() { return m_Data[2]; }
};

struct Vec2 {
    Vec2() 
    {
        m_Data[0] = 0.0f;
        m_Data[1] = 0.0f;
    }

    Vec2(f32 v)
    {
        m_Data[0] = v;
        m_Data[1] = v;
    }

    Vec2(f32 x, f32 y)
    {
        m_Data[0] = x;
        m_Data[1] = y;
    }

    Vec2(const Vec2& v)
    {
        m_Data[0] = v.m_Data[0];
        m_Data[1] = v.m_Data[1];
    }

    inline Vec2& operator=(const Vec2& v)
    {
        m_Data[0] = v.m_Data[0];
        m_Data[1] = v.m_Data[1];
        return *this;
    }

    inline f32& X() { return m_Data[0]; };
    inline f32 X() const { return m_Data[0]; };
    inline f32& Y() { return m_Data[1]; };
    inline f32 Y() const { return m_Data[1]; };
    inline f32& operator[](usize i)
    {
        assert(i < 2);
        return m_Data[i];
    }
    inline f32 operator[](usize i) const
    {
        assert(i < 2);
        return m_Data[i];
    }
private:
    f32 m_Data[2];
};

inline Vec2 operator+(const Vec2& u, const Vec2& v)
{
    return { u[0] + v[0], u[1] + v[1] };
}

inline Vec2 operator-(const Vec2& u, const Vec2& v)
{
    return { u[0] - v[0], u[1] - v[1] };
}

inline Vec2 operator*(f32 s, const Vec2& v)
{
    return { s * v[0], s * v[1] };
}

inline Vec2 operator*(const Vec2& v, f32 s)
{
    return s * v;
}

inline Vec2 operator/(const Vec2& v, f32 s)
{
    return (1.0f / s) * v;
}

inline std::ostream& operator<<(std::ostream& os, const Vec2& v)
{
    return os << "(" << v[0] << ", " << v[1] << ")";
}

