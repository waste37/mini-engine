#include <Platform.h>

#ifdef _WIN32
#include <PlatformWIN32.h>
#elif __linux__
#include <PlatformLinux.h>
#else 
#error "Unsupported platform"
#endif
