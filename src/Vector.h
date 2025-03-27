#pragma once
#include <Types.h>
#include <ostream>

#include <cstdio>

template <typename T>
class Vector {
    static constexpr isize INITIAL_CAPACITY = 8;
public:
    Vector() : m_Data(new T[INITIAL_CAPACITY]), m_Size(0), m_Capacity(INITIAL_CAPACITY) {}
    Vector(isize size)
    {
        m_Size = size;
        InitCapacityToPowerOfTwo();
        m_Data = new T[m_Capacity];
    }

    Vector(isize size, T value)
    {
        m_Size = size;
        InitCapacityToPowerOfTwo();
        m_Data = new T[m_Capacity];
        for (isize i = 0; i < m_Size; ++i) {
            m_Data[i] = value;
        }
    }

    Vector(const std::initializer_list<T>& xs)
    {
        m_Size = xs.size();
        InitCapacityToPowerOfTwo();
        m_Data = new T[m_Capacity];
        isize i = 0;
        for (const T x : xs) {
            m_Data[i++] = x;
        }
    }

    Vector(const Vector<T>& xs)
    {
        m_Size = xs.m_Size;
        m_Capacity = xs.m_Capacity;
        m_Data = new T[m_Capacity];

        for (isize i = 0; i < m_Size; ++i) {
            m_Data[i] = xs.m_Data[i];
        }
    }

    ~Vector()
    {
        delete[] m_Data;
    }

    void Push(T x)
    {
        if (m_Size == m_Capacity) {
            Grow();
        }
        m_Data[m_Size++] = x;
    }

    T Pop()
    {
        T x = m_Data[--m_Size];
        if (m_Capacity > INITIAL_CAPACITY && m_Size <= m_Capacity / 2) {
            Shrink();
        }

        return x;
    }

    void Reserve(isize size)
    {
        m_Capacity = NearestPowerOfTwo(size);
        T* new_data = new T[m_Capacity];
        for (isize i = 0; i < m_Size; ++i) {
            new_data[i] = std::move(m_Data[i]);
        }
        delete[] m_Data;
        m_Data = new_data;
    }

    void Resize(isize size)
    {
        m_Size = size;
        while (m_Size >= m_Capacity) {
            Grow();
        }
        // while (m_Capacity > INITIAL_CAPACITY && m_Size < m_Capacity / 2) {
        //     Shrink();
        // }
    }

    void Resize(isize size, T x)
    {
        isize old_size = m_Size;
        Resize(size);
        while (old_size < m_Size) {
            m_Data[old_size++] = x;
        }
    }

    inline bool Empty() const
    {
        return m_Size == 0;
    }

    inline void Clear()
    {
        Resize(0);
    }

    inline isize Size() const
    {
        return m_Size;
    }

    inline const T& operator[](isize i) const
    {
        return m_Data[i];
    }

    inline T& operator[](isize i)
    {
        return m_Data[i];
    }

    inline bool operator==(const Vector<T>& other) const
    {
        if (m_Size != other.m_Size) return false;
        for (isize i = 0; i < m_Size; ++i) {
            if (m_Data[i] != other.m_Data[i]) {
                return false;
            }
        }
        return true;
    }

    T* m_Data;
    isize m_Size;
    isize m_Capacity;
private:
    inline isize NearestPowerOfTwo(isize n)
    {
        isize result = 1;
        isize i = 0;
        while (result < n) {
            result = (1 << i++);
        }
        return result;
    }

    inline void InitCapacityToPowerOfTwo()
    {
        m_Capacity = INITIAL_CAPACITY;
        while (m_Capacity < m_Size) {
            m_Capacity = (1ull << m_Capacity);
        }
    }

    inline void Grow()
    {
        m_Capacity *= 2;
        T* new_data = new T[m_Capacity];
        for (isize i = 0; i < m_Size; ++i) {
            new_data[i] = std::move(m_Data[i]);
        }
        delete[] m_Data;
        m_Data = new_data;
    }

    inline void Shrink()
    {
        m_Capacity /= 2;
        T* new_data = new T[m_Capacity];
        for (isize i = 0; i < m_Size; ++i) {
            new_data[i] = std::move(m_Data[i]);
        }
        delete[] m_Data;
        m_Data = new_data;
    }
};


template <class T>
std::ostream& operator<<(std::ostream& out, const Vector<T>& v)
{
    out << "{";
    for (isize i = 0; i < v.m_Size; ++i) {
        out << v.m_Data[i];
        if (i < v.m_Size - 1) {
            out << ", ";
        }
    }
    out << "}";

    return out;
}
