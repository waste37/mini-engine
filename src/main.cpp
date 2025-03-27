#include <WorldRegistry.h>
// #include <LinearAlgebra.h>
#include <Geometry.h>
#include <iostream>

#include <filesystem>
#include <fstream>

#include <chrono>

class Timer {
public:
    Timer()
    {
        std::cout << "start stopwatch\n";
        beg = std::chrono::steady_clock::now();
    }
    ~Timer()
    {
        const auto dt = elapsed();
        std::cout << "elapsed: " << dt.count() / 1000.0 << "ms" << std::endl;
    }

    std::chrono::nanoseconds elapsed()
    {
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

void BasicBenchmark()
{
    WorldRegistry world{};
    world.Create();

    for (i32 i = 0; i < 10000; ++i) {
        Entity e = world.CreateEntity<Position, Velocity>();
        Position* pos = world.GetComponentData<Position>(e);
        pos->x = 10.0f + i;
        pos->y = 11.0f + i;

        Velocity* v = world.GetComponentData<Velocity>(e);
        v->dx = -10.0f + i;
        v->dy = -11.0f + i;
    }

    for (i32 i = 0; i < 10000; ++i) {
        Entity e = world.CreateEntity<Position, Health>();
        Position* pos = world.GetComponentData<Position>(e);
        pos->x = 20.0f + i;
        pos->y = 21.0f + i;
        Health* hp = world.GetComponentData<Health>(e);
        hp->health = 0;
    }

    for (i32 i = 0; i < 1000; ++i) {
        Entity e = world.CreateEntity<Velocity, Health>();
        Velocity* pos = world.GetComponentData<Velocity>(e);
        pos->dx = 30.0f + i;
        pos->dy = 31.0f + i;
        Health* hp = world.GetComponentData<Health>(e);
        hp->health = 0;
    }

    {
        auto t = Timer();
        for (WorldRegistry::ViewIterator it = world.View<Position>(); !it.AtEnd(); it.Next()) {
            Position* p = it.Get<Position>();
            p->x = 0.0f;
            p->y = 0.0f;
        }

        for (WorldRegistry::ViewIterator it = world.View<Health>(); !it.AtEnd(); it.Next()) {
            Health* p = it.Get<Health>();
            p->health++;
        }

        for (WorldRegistry::ViewIterator it = world.View<Position, Health>(); !it.AtEnd(); it.Next()) {
            Health* hp = it.Get<Health>();
            Position* p = it.Get<Position>();
            hp->health++;
            p->x = 0;
        }

        for (WorldRegistry::ViewIterator it = world.View<Velocity>(); !it.AtEnd(); it.Next()) {
            Velocity* p = it.Get<Velocity>();
            p->dx = 0.0;
            p->dy = 0.0;
        }
    }

    world.DebugRegisteredComponents();
    world.DebugRegisteredTypes();
    world.Destroy();
}

struct Mesh {
    Vector<Vec3> vertices;
    Vector<Vec3> normals;
    Vector<i32> indices;
};

usize FileSize(const char* filename)
{
    std::error_code ec;
    return std::filesystem::file_size(filename, ec);
}

usize ReadFile(char* buf, usize size, const char* filename)
{
    std::ifstream in(filename, std::ios::binary);
    in.read(buf, size);
    usize read_bytes = in ? size : in.gcount();
    buf[read_bytes] = '\0';
    in.close();
    return read_bytes;
}


#include <assimp/Importer.hpp>      // C++ importer interface
#include <assimp/scene.h>           // Output data structure
#include <assimp/postprocess.h>     // Post processing flags

bool ImportFile(const char *filename) {
    // Create an instance of the Importer class..
    Assimp::Importer importer;
    const aiScene* scene = importer.ReadFile(filename, aiProcess_CalcTangentSpace | aiProcess_Triangulate | aiProcess_JoinIdenticalVertices | aiProcess_SortByPType); 
    if (nullptr == scene) {
        return false;
    }

    for (isize i = 0; i < scene->mNumMeshes; ++i) {
        const auto &mesh = scene->mMeshes[i];
        std::cout << "vectices: \n";
        for (isize j = 0; j < mesh->mNumVertices; ++j) {
            const auto &v = mesh->mVertices[j];
            std::cout << v.x << " " << v.y << " " << v.z << std::endl;
        }

        std::cout << "textures: \n";
        for (isize j = 0; j < AI_MAX_NUMBER_OF_TEXTURECOORDS; ++j) {
            const auto uvs = mesh->mTextureCoords[j];
            if (uvs) {
                std::cout << "uvs:\n";
                for (isize k = 0; k < mesh->mNumVertices; k++) {
                    const auto uv = uvs[k];
                    if (mesh->mNumUVComponents[j] == 2) {
                        std::cout << uv.x << " " << uv.y << std::endl;
                    }
                }
            }
        }
        std::cout << "normals: \n";
        for (isize j = 0; j < mesh->mNumVertices; ++j) {
            const auto &v = mesh->mNormals[j];
            std::cout << v.x << " " << v.y << " " << v.z << std::endl;
        }

    }

    return true;
}
 
i32 main()
{
    return !ImportFile("assets/Mech_F_432/Material/mech_f_432.obj");
}

