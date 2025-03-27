#pragma once

#include <Types.h>
#include <Vector.h>
#include <tuple>

class UniqueIDGenerator {
    static u32 Identifier() {
        static u32 value = 0;
        return value++;
    }
public:
    template <typename T>
    static u32 Type() {
        static const u32 value = Identifier();
        return value;
    }
};

struct ComponentInfo {
    static constexpr u64 NULL_COMPONENT = 0;
    static constexpr isize INVALID_INDEX = 0;
    Vector<isize> Sizes{};
    Vector<const char*> Names{};
};

struct Entity {
    constexpr Entity(u32 Index, u32 Version) : Index(Index), Version(Version) {}
    Entity() : Index(0), Version(0) {}
    u32 Index;
    u32 Version;
};

constexpr Entity NULL_ENTITY = { 0, 0 };

struct Chunk {
    static constexpr isize CHUNK_SIZE = (16 * 1024);
    isize EntityCapacity;
    isize Count;
    byte* Data;
};

struct ChunkList {
    Vector<u32> ComponentsIDs{};
    Vector<Chunk> Chunks{};
};

struct EntityInfo {
    u32 ChunkIndex;
    u32 TypeIndex;
    u32 IndexInChunk;
    u32 Version;
};

class WorldRegistry {
public:
    WorldRegistry() = default;
    bool Create();
    void Destroy();

    template <typename ...ComponentTypes>
    Entity CreateEntity()
    {
        Entity e = NextEntity();
        (RegisterComponent<ComponentTypes>(), ...);
        u32 type_index = RegisterType<ComponentTypes...>();
        m_Entities[e.Index].TypeIndex = type_index;
        ChunkListPushEntity(e);
        return e;
    }

    bool DeleteEntity(Entity e);
    template <typename T>
    T* GetComponentData(Entity e)
    {
        u32 component_id = UniqueIDGenerator::Type<T>();
        return (T*)GetComponentDataInternal(e, component_id);
    }
    void DebugRegisteredComponents() const;
    void DebugRegisteredTypes() const;
    void DebugRegisteredEntities() const;

    class ViewIterator {
    public:
        ViewIterator();
        ViewIterator(const Vector<const ChunkList*>& types, const Vector<u32>& ids, const Vector<isize>* sizes);
        void Next(); // go to next thing and get current
        template <typename T>
        T* Get()
        {  // get current entity data
            return (T*)GetInternal(UniqueIDGenerator::Type<T>());
        }

        inline bool AtEnd()
        {
            if (m_AtEnd) return true;
            return m_AtEnd = !(m_CurrentType < m_Types.Size() &&
                m_CurrentChunk < m_Types[m_CurrentType]->Chunks.Size() &&
                m_CurrentIndex < m_Types[m_CurrentType]->Chunks[m_CurrentChunk].Count);
        }
        //  void Delete(); // delete current thing
    private:
        byte* GetInternal(u32 id);
        void RebuildSlice();
        void AdvanceSlice();
        Vector<const ChunkList*> m_Types; // i have a vector of pointer to chunklists
        const Vector<isize>* m_Sizes; // i have a vector of pointer to chunklists
        isize m_CurrentType = 0;
        isize m_CurrentChunk = 0;
        isize m_CurrentIndex = 0;
        bool m_AtEnd = false;

        Vector<u32> m_IDs{};      // i have an id per component
        Vector<byte*> m_Slice{};  // m_IDs[i] == typeof(m_Slice[i])
    };

    template <typename ...ComponentTypes>
    ViewIterator View() const
    {
        Vector<u32> component_ids{};
        (component_ids.Push(UniqueIDGenerator::Type<ComponentTypes>()), ...);
        return ViewInternal(component_ids);
    }
private:
    template <typename T>
    void RegisterComponent()
    {
        const char* name = typeid(T).name();
        u32 id = UniqueIDGenerator::Type<T>();
        isize size = sizeof(T);
        if (!m_RegisteredComponents.Sizes.Size() || id >= m_RegisteredComponents.Sizes.Size() - 1) {
            m_RegisteredComponents.Sizes.Resize(id + 1);
            m_RegisteredComponents.Names.Resize(id + 1);
        }
        m_RegisteredComponents.Sizes[id] = size;
        m_RegisteredComponents.Names[id] = name;
    }

    template <typename ...ComponentTypes>
    u32 RegisterType()
    {
        Vector<u32> component_ids = { 0 };
        (component_ids.Push(UniqueIDGenerator::Type<ComponentTypes>()), ...);
        return RegisterTypeInternal(component_ids);
    }

    u32 RegisterTypeInternal(const Vector<u32>& component_ids);
    ViewIterator ViewInternal(const Vector<u32>& component_ids) const;
    byte* GetComponentDataInternal(Entity e, u32 component_id);
    Entity NextEntity();
    void ChunkListPushEntity(Entity e);

    ComponentInfo m_RegisteredComponents{};
    Vector<EntityInfo> m_Entities{};
    Vector<u32> m_AvailableIDs{};
    Vector<ChunkList> m_Types{};
};

