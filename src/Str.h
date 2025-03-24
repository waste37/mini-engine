#pragma once
#include <Types.h>
#ifdef _WIN32
#include <wchar.h>
#endif

struct Str {
#ifdef _WIN32
    const wchar_t *Data;
#else
    const char *Data;
#endif
    usize Length;
};

#ifdef _WIN32

#define WIDE_STRING(x) L##x
#define S(x) Str{ .Data = WIDE_STRING(x), .Length = wcslen(WIDE_STRING(x)) };

#else

#define S(x) Str{ .Data = x, .Length = sizeof(x) - 1 };

#endif
