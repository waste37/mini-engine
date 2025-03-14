#include <cassert>
#include <cstdio>
#include <cstdint>
#include <GLFW/glfw3.h>
#include <GL/gl.h>

void stuff() {
    glfwInit();
    //glfwWindowHint(GLFW_)
    GLFWwindow *window = glfwCreateWindow(800, 800, "ecs-testing", nullptr, nullptr);

    glfwMakeContextCurrent(window);
    glViewport(0, 0, 800, 800);

    while (!glfwWindowShouldClose(window)) {
        glClearColor(1.0, 1.0, 1.0, 1.0);
        glClear(GL_COLOR_BUFFER_BIT);
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwDestroyWindow(window);
    glfwTerminate();
}

// an ecs with archetypes


// a chunk is a block of N kb, that contains
// entities of the same type, with components stored sequentially.

// the mask should allow me to retrieve which components are in the chunk?

// what is not clear is how does one iterate a view of this db..
// do i need to iterate

typedef uint64_t EntityID;
typedef int64_t EntityType;
#define NULL_ENTITY_TYPE -1
#define MAX_COMPONENTS 64

// The number of possible chunklists is 2 ^ 64 - 1.
// All the 64 bit combinations.
// A chunklist contains a list of chunks, each of 16 * 1024 bytes
// And information to step inside the chunks and retrieve components data.
struct ChunkListsMetadata {
    int64_t m_ChunkListsCount;
    uint64_t *m_TypeMasks;
};

#define CHUNK_SIZE (16 * 1024)
// each chunk's data is structured as follows:
// EntityID entities[];
// Component1 ones[];
// Component2 twos[];
// ...
// ComponentN ns[];
// one can use the ComponentsMetadata.sizes to find the offsets inside
// the data array;

struct Chunk {
    int64_t available_entities;
    void *m_Data;
};

struct ChunkList {
    uint64_t m_TypeMask;
    int64_t m_ComponentCount;
    int64_t m_ChunksCount;
    Chunk *m_Chunks;
};



ChunkList ChunkLists[3];

struct ComponentsMetadata {
    int32_t  sizes[MAX_COMPONENTS];
    uint64_t signatures[MAX_COMPONENTS];
    int64_t  component_count;
};

ComponentsMetadata RegisteredComponents = {0};

void DebugComponentsMetadata() {
    for (int32_t i = 0; i < RegisteredComponents.component_count; ++i) {
        printf("component %d: {size: %d, mask: %x}\n", i, RegisteredComponents.sizes[i], RegisteredComponents.signatures[i]);
    }
}

uint64_t RegisterComponent(uint32_t size) {
    assert(RegisteredComponents.component_count < MAX_COMPONENTS);
    uint64_t signature = 1 << RegisteredComponents.component_count;
    RegisteredComponents.sizes[RegisteredComponents.component_count] = size;
    RegisteredComponents.signatures[RegisteredComponents.component_count] = signature;
    RegisteredComponents.component_count++;
    return signature;
}

EntityID current_id = 0;
EntityID CreateEntity(uint64_t type_mask) {
    // i check if a chunk for this type already exists,
    // if not, i create it.
    EntityID id = current_id++;

    // EntityType chunklist_id = FindChunkListByType(type_mask);
    // if (chunklist_id != NULL_ENTITY_TYPE) {
    //     ChunkListAddEntity(chunklist_id, id);
    // } else {
    //     CreateChunkList(type_mask, id);
    // }
    return id;
}

struct Position {
    float x, y;
};

int main() {
    uint64_t position_mask = RegisterComponent(sizeof(Position));
    EntityID e = CreateEntity(position_mask);
    printf("created entity %d\n", e);
    DebugComponentsMetadata();
    DebugChunkLists();
}
