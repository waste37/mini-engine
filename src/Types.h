#pragma once

#include <cstdint>
#include <cstddef>

// integers
typedef int32_t i32;
typedef int64_t i64;

typedef float f32;
typedef double f64;
// unsigned
typedef uint32_t u32;
typedef uint64_t u64;
// raw memory unit
typedef char byte;
// sizes and pointer types
typedef ptrdiff_t isize; // default size
typedef uintptr_t uptr;
typedef intptr_t  iptr;
typedef size_t usize; // for compatibility with external apis

// a bool, the intention is to use as a bitset
typedef int32_t b32;
// UTF-8 and UTF-16 respectively
typedef uint8_t u8;
typedef char16_t c16;

// make macros work with signed sisze.
//#define sizeof(x)       sizeof(x)
// #define countof(a)      (sizeof(a) / sizeof(*(a)))
#define lengthof(s)     (countof(s) - 1)

#define s8(s) (s8){(u8 *)s, lengthof(s)}
struct s8 {
    u8 *Data;
    isize Length;
};

#define s16(s) (s16){(u##s, lengthof(u##s)}
struct s16 {
    c16 *Data;
    isize Length;
};

