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
