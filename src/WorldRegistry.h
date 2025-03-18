#pragma once

#include "Types.h"
#include "Vector.h"
#include <tuple>

extern u32 g_NextComponentID;
template <typename Component> 
struct IComponent {
    static inline u32 ID() {
        static u32 id = g_NextComponentID++;
        return id;
    }
};

struct ComponentInfo {
    static constexpr u64 NULL_COMPONENT = 0;
    static constexpr usize INVALID_INDEX = 0;
    Vector<usize> Sizes{};
    Vector<const char*> Names{};
};

struct Entity : public IComponent<Entity> {
    u32 Index;
    u32 Version;
};

constexpr Entity NULL_ENTITY = {.Index = 0, .Version = 0};

struct Chunk {
    static constexpr usize CHUNK_SIZE = (16 * 1024);
    usize EntityCapacity;
    usize Count;
    void *Data;
};

struct ChunkList {
    Vector<u32> ComponentsIDs{};
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

    template <typename ...ComponentTypes>
    Entity CreateEntity() {
        Entity e = NextEntity();
        (RegisterComponent<ComponentTypes>(), ...);
        ChunkList *type = RegisterType<ComponentTypes...>();
        m_Entities[e.Index].TypeInfo = type;
        ChunkListPushEntity(e);
        return e;
    }

    bool DeleteEntity(Entity e);
    template <typename T>
    T *GetComponentData(Entity e) {
        u32 component_id = T::ID();
        return (T*)GetComponentDataInternal(e, component_id);
    }
    void DebugRegisteredComponents() const;
    void DebugRegisteredTypes() const;
    void DebugRegisteredEntities() const;

    class ViewIterator {
    public:
        ViewIterator();
        ViewIterator(const Vector<const ChunkList*> &types, const Vector<u32> &ids, const Vector<usize> *sizes);
        void Next(); // go to next thing and get current
        template <typename T>
        T *Get() {  // get current entity data
            return (T*)GetInternal(T::ID());
        } 

        inline bool AtEnd() {
            if (m_AtEnd) return true;
            return m_AtEnd = !(m_CurrentType < m_Types.Size() &&
                   m_CurrentChunk < m_Types[m_CurrentType]->Chunks.Size() &&
                   m_CurrentIndex < m_Types[m_CurrentType]->Chunks[m_CurrentChunk].Count);
        }
      //  void Delete(); // delete current thing
    private:
        void *GetInternal(u32 id);
        void RebuildSlice();
        void AdvanceSlice();
        Vector<const ChunkList*> m_Types; // i have a vector of pointer to chunklists
        const Vector<usize> *m_Sizes; // i have a vector of pointer to chunklists
        usize m_CurrentType = 0;
        usize m_CurrentChunk = 0;
        usize m_CurrentIndex = 0;
        bool m_AtEnd = false;

        Vector<u32> m_IDs{};      // i have an id per component
        Vector<void*> m_Slice{};  // m_IDs[i] == typeof(m_Slice[i])
    };

    template <typename ...ComponentTypes>
    ViewIterator View() const {
        Vector<u32> component_ids = {0};
        (component_ids.Push(ComponentTypes::ID()), ...);
        return ViewInternal(component_ids);
    }
private:
    template <typename T>
    void RegisterComponent() {
        const char *name = typeid(T).name();
        u32 id = T::ID();
        usize size = sizeof(T);
        if (!m_RegisteredComponents.Sizes.Size() || id >= m_RegisteredComponents.Sizes.Size() - 1) {
            m_RegisteredComponents.Sizes.Resize(id + 1);
            m_RegisteredComponents.Names.Resize(id + 1);
        }
        m_RegisteredComponents.Sizes[id] = size;
        m_RegisteredComponents.Names[id] = name;
    }

    template <typename ...ComponentTypes>
    ChunkList *RegisterType() {
        Vector<u32> component_ids = {0};
        (component_ids.Push(ComponentTypes::ID()), ...);
        return RegisterTypeInternal(component_ids);
    }

    ChunkList *RegisterTypeInternal(const Vector<u32> &component_ids);
    ViewIterator ViewInternal(const Vector<u32> &component_ids) const;
    void *GetComponentDataInternal(Entity e, u32 component_id);
    Entity NextEntity();
    void ChunkListPushEntity(Entity e);

    ComponentInfo m_RegisteredComponents{};
    Vector<EntityInfo> m_Entities{};
    Vector<i32> m_AvailableIDs{};
    Vector<ChunkList> m_Types{};
};

