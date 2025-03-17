#pragma once
#include "Types.h"
#include <ostream>
#include <cstring>

template <typename T>
class Vector {
    static constexpr usize INITIAL_CAPACITY = (1 << 3);
public:
    Vector() : m_Data(new T[INITIAL_CAPACITY]), m_Size(0), m_Capacity(INITIAL_CAPACITY) { }

    Vector(usize size) {
        m_Size = size;
        InitCapacityToPowerOfTwo();
        m_Data = new T[m_Capacity];
    }

    Vector(usize size, T value) {
        m_Size = size;
        InitCapacityToPowerOfTwo();
        m_Data = new T[m_Capacity];
        for (usize i = 0; i < m_Size; ++i) {
            m_Data[i] = value;
        }
    }

    Vector(const std::initializer_list<T> &xs) {
        m_Size = xs.size();
        InitCapacityToPowerOfTwo();
        m_Data = new T[m_Capacity];
        usize i = 0;
        for (const T x : xs) {
            m_Data[i++] = x;
        }
    }

    ~Vector() {
        delete[] m_Data;
    }

    void Push(T x) {
        if (m_Size == m_Capacity) {
            Grow();
        }
        m_Data[m_Size++] = x;
    }

    T Pop() {
        T x = m_Data[--m_Size];
        if (m_Capacity > INITIAL_CAPACITY && m_Size <= m_Capacity / 2) {
            Shrink();
        }

        return x;
    }

    void Resize(usize size) {
        m_Size = size;
        while (m_Size >= m_Capacity) {
            Grow();
        }
        while (m_Capacity > INITIAL_CAPACITY && m_Size <= m_Capacity / 2) {
            Shrink();
        }
    }

    void Resize(usize size, T x) {
        usize old_size = m_Size;
        Resize(size);
        while (old_size < m_Size) {
            m_Data[old_size++] = x;
        }
    }

    inline bool Empty() const {
        return m_Size == 0;
    }

    inline void Clear() {
        Resize(0);
    }

    inline usize Size() const {
        return m_Size;
    }

    inline const T &operator[](usize i) const {
        return m_Data[i];
    }

    inline T &operator[](usize i) {
        return m_Data[i];
    }

    inline bool operator==(const Vector<T> &other) const {
        if (m_Size != other.m_Size) return false;
        for (usize i = 0; i < m_Size; ++i) {
            if (m_Data[i] != other.m_Data[i]) {
                return false;
            }
        }
        return true;
    }

    T *m_Data; 
    usize m_Size;
    usize m_Capacity;
private:
    inline void InitCapacityToPowerOfTwo() {
        m_Capacity = 2;
        while (m_Capacity < m_Size) {
            m_Capacity = (1 << m_Capacity);
        }
    }

    inline void Grow() {
        m_Capacity *= 2;
        T *new_data = new T[m_Capacity];
        for (usize i = 0; i < m_Size; ++i) {
            new_data[i] = std::move(m_Data[i]);
        }
        delete [] m_Data;
        m_Data = new_data;
    }

    inline void Shrink() {
        m_Capacity /= 2;
        T *new_data = new T[m_Capacity];
        for (usize i = 0; i < m_Size; ++i) {
            new_data[i] = std::move(m_Data[i]);
        }
        delete [] m_Data;
        m_Data = new_data;
    }
};


template <class T>
std::ostream &operator<<(std::ostream &out, const Vector<T> &v) {
    out << "{";
    for (usize i = 0; i < v.m_Size; ++i) {
        out << v.m_Data[i];
        if (i < v.m_Size - 1) {
            out << ", ";
        }
    }
    out << "}";

    return out;
}
