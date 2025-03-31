#include <Platform.h>

// for stuff i don't know/ is not possible to do in a platform independent way
#ifdef _WIN32
#include <PlatformWIN32.cpp>
#elif __linux__
#include <PlatformLinux.cpp>
#else 
#error "Unsupported platform"
#endif

// the stuff that can be made cross platform, but is still platform related is here.

#include <filesystem>
#include <fstream>
#include <iostream>

s8 ReadFile(const char *path) {
    isize file_size = std::filesystem::file_size(path);
    std::cout << path << " (size=" << file_size << ")" << std::endl;
    s8 result{};

    FILE *f = fopen(path, "r");
    result.Length = file_size + 1;
    result.Data = new u8[result.Length];
    fread(result.Data, file_size, 1, f);
    result.Data[file_size] = 0;
    fclose(f);
    return result;
}

