#pragma once

#include "Types.h"
#include "Vector.h"


struct Entity {
    u32 Index;
    u32 Version;
};

constexpr Entity NULL_ENTITY = {0, 0};

extern u32 g_NextComponentID;
template <typename D> struct IComponent {
    static u32 ID() {
        static u32 id = g_NextComponentID++;
        return id;
    }
};

struct ComponentInfo {
    static constexpr u64 NULL_COMPONENT = 0;
    static constexpr usize INVALID_INDEX = 0;
    Vector<usize> Sizes;
    Vector<const char*> Names;
    // u64   TypeMasks[MAX_COMPONENTS];
    // static inline usize ComponentIndex(u64 typemask) {
    //     if (typemask == 0) return INVALID_INDEX;
    //     usize index = 0;
    //     while ((typemask & 0x1) == 0) {
    //         typemask >>= 1;
    //         index++;
    //     }
    //     return index;
    // }
};

struct Chunk {
    static constexpr usize CHUNK_SIZE = (16 * 1024);
    usize EntityCapacity;
    usize Count;
    void *Data;
};

struct ChunkList {
    // a component could be a unique id or a bit in this mask
    // to check wether a component is contained is faster in
    // the latter case, however to retrieve the component location
    // inside the chunk is fastest in the former, and it also
    // allow us to avoid the problem of having at most 64 component?
    // however having 64 components is not a problem, and
    // we could use the offsets
    Vector<u32> ComponentsIDs{};
    // these grow one at the time
    Vector<Chunk> Chunks{};
};

struct EntityInfo {
    struct Chunk *Chunk;
    ChunkList *TypeInfo;
    u32 IndexInChunk;
    u32 Version;
};

class WorldRegistry {
public:

    WorldRegistry() = default;
    bool Create();
    void Destroy();

    // now returns the index of a component
    void RegisterComponent(const char *name, u32 id, usize size);
    Entity CreateEntity(const Vector<u32> &component_ids);
    bool DeleteEntity(Entity e);
    void *GetComponentData(Entity e, u32 component_id);

    void DebugRegisteredComponents() const;
    void DebugRegisteredTypes() const;
    void DebugRegisteredEntities() const;
private:
    ChunkList *RegisterType(const Vector<u32> &component_ids);

    // void PushEntity(Entity e, ChunkList *type);
    // used for computing offsets inside chunks for retrieving the data
    ComponentInfo m_RegisteredComponents;
    Vector<EntityInfo> m_Entities{};
    Vector<i32> m_AvailableIDs{};
    Vector<ChunkList> m_Types{};
};

