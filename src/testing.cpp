#include <cassert>
#include <cstdio>
//#include <GLFW/glfw3.h>
//#include <GL/gl.h>

#include <cstdint>
#include <cstddef>
#include <cstdlib>
#include <iostream>
#include <sstream>
#include <string>

#include "Types.h"
#include "Vector.h"
#include "WorldRegistry.h"

// a world has entities and components
// the user implements systems
namespace Testing {
   // sample enum for foreground colors
   enum class FG : unsigned char {
       Def     = 39,
       Black   = 30,
       Red     = 31,
       Green   = 32,
       Yellow  = 33,
       Blue    = 34,
       Magenta = 35,
       Cyan    = 36,
       Gray    = 37
   };

//    // initialize them at start of program - mandatory
//    // so that even if user redirects, we've a copy
//    std::streambuf const *coutbuf = std::cout.rdbuf();
//    std::streambuf const *cerrbuf = std::cerr.rdbuf();
//    std::streambuf const *clogbuf = std::clog.rdbuf();
//
//    // Here is the implementation of isTerminal
//    // which checks if program is writing to Terminal or not
//    bool IsTerminal(const std::streambuf *osbuf) {
//        FILE *currentStream = nullptr;
//        if(osbuf == coutbuf) {
//            currentStream = stdout;
//        }
//        else if(osbuf == cerrbuf || osbuf == clogbuf) {
//            currentStream = stderr;
//        }
//        else {
//            return false;
//        }
//        return isatty(fileno(currentStream));
//    }
//
   // will check if TERM supports color and isTerminal()
   inline std::ostream &operator<<(std::ostream &os, FG v) {
       std::streambuf const *osbuf = os.rdbuf();
       return os;
   }

   bool VectorConstructor() {
       {
           Vector<int> v{};
           std::stringstream s;
           s << v;
           if (s.str() != "{}") return false;
       }
       {
           Vector<int> v(10);
           if (v.Size() != 10) return false;
       }
       {
           Vector<int> v(10, 6);
           std::stringstream s;
           s << v;
           if (s.str() != "{6, 6, 6, 6, 6, 6, 6, 6, 6, 6}") return false;
       }
       {
           Vector<int> v = {1,2,3};
           std::stringstream s;
           s << v;
           if (s.str() != "{1, 2, 3}") return false;
       }
       return true;
   }

    bool VectorPush() {
        {
            Vector<int> v{};
            v.Push(1);
            v.Push(2);
            v.Push(3);
            std::stringstream s;
            s << v;
            if (s.str() != "{1, 2, 3}") return false;
        }
        return true;
    }

    bool VectorPop() {
        {
            Vector<int> v = {1, 2, 3};
            if (v.Pop() != 3) return false;
            if (v.Pop() != 2) return false;
            if (v.Pop() != 1) return false;
            std::stringstream s;
            s << v;
            if (s.str() != "{}") return false;
        }
        return true;
    }

    bool VectorSize() {
        {
            Vector<int> v{};
            if (v.Size() != 0) return false;
        }
        {
            Vector<int> v(1000);
            if (v.Size() != 1000) return false;
        }
        {
            Vector<int> v(10, 6);
            if (v.Size() != 10) return false;
        }
        {
            Vector<int> v = {1, 2, 3};
            if (v.Size() != 3) return false;
        }
        return true;
    }

    bool VectorResize() {
        {
            Vector<int> v{};
            v.Resize(10);
            if (v.Size() != 10) return false;
        }

        {
            Vector<int> v{};
            v.Resize(10, 6);
            if (v.Size() != 10) return false;
            std::stringstream s;
            s << v;
            if (s.str() != "{6, 6, 6, 6, 6, 6, 6, 6, 6, 6}") return false;
        }

        return true;
    }

    bool VectorReserve() {
        Vector<int> v{};
        v.Reserve(10);
        if (v.Size() != 0) return false;
        v.Resize(10);
        return true;
    }

struct Position {
    f32 x, y;
};

struct Velocity {
    f32 dx, dy;
};

    bool WorldEntityCRUD() {
        // WorldRegistry world{};
        // world.Create();
        // u64 positionmask = world.RegisterComponent(sizeof(Position));
        // u64 velocitymask = world.RegisterComponent(sizeof(Velocity));

        // Entity e1 = world.CreateEntity(positionmask | velocitymask);
        // Entity e2 = world.CreateEntity(positionmask | velocitymask);

        // Position *pos = (Position*)world.GetComponentData(e1, positionmask);
        // pos->x = 10.0;
        // pos->y = 11.0;

        // pos = (Position*)world.GetComponentData(e2, positionmask);
        // pos->x = 20.0;
        // pos->y = 21.0;


        // pos = (Position*)world.GetComponentData(e1, positionmask);
        // if (pos->x != 10.0 || pos->y != 11.0) return false;

        // pos = (Position*)world.GetComponentData(e2, positionmask);
        // if (pos->x != 20.0 || pos->y != 21.0) return false;
        // world.Destroy();
        return true;
    }


    void RunTest(const char *test_name, bool(*test)()) {
        std::cout << FG::Green << "running test: " << FG::Def << test_name << "... " << std::flush;
        i32 result = test();
        if (!result) {
            std::cout << FG::Red << "failed\n";
            std::exit(EXIT_FAILURE);
        }
        std::cout << FG::Green << "done\n" << FG::Def;
    }

    void Run() {
        std::cout << "\nVector tests:\n";
        RunTest("VectorConstructor", VectorConstructor);
        RunTest("VectorPush", VectorPush);
        RunTest("VectorPop", VectorPop);
        RunTest("VectorSize", VectorSize);
        RunTest("VectorResize", VectorResize);
        RunTest("VectorReserve", VectorReserve);
        RunTest("WorldEntityCRUD", WorldEntityCRUD);
        std::cout << "\nWorldRegistry tests:\n";
        std::cout << "All tests passed\n\n";
    }
}


i32 main() {
    Testing::Run();
}
