#include <WorldRegistry.h>

bool WorldRegistry::Create()
{
    RegisterComponent<Entity>();
    m_Types.Reserve(128);
    m_Entities.Reserve(128);
    m_AvailableIDs.Reserve(128);
    m_Entities.Push({ 0, 0, 0, 0 });
    return true;
}

void WorldRegistry::Destroy()
{
    for (isize i = 0; i < m_Types.Size(); ++i) {
        for (isize j = 0; j < m_Types[i].Chunks.Size(); ++j) {
            delete[](u8*)m_Types[i].Chunks[j].Data;
        }
    }
}

bool WorldRegistry::DeleteEntity(Entity e)
{
    // doesn't exist: never generated such index
    if (e.Index >= m_Entities.Size()) {
        return false;
    }
    // doesn't exist: already deleted
    EntityInfo* info = &m_Entities[e.Index];
    if (e.Version != info->Version) {
        return false;
    }

    m_AvailableIDs.Push(e.Index);
    info->Version++;

    ChunkList* type = &m_Types[info->TypeIndex];
    Chunk* chunk = &type->Chunks[info->ChunkIndex];
    // swap last entity with the one that is being deleted
    if (chunk->Count > 1) {
        isize last = chunk->Count - 1;
        isize cur = info->IndexInChunk;

        Entity* last_entity = (Entity*)chunk->Data + last;
        m_Entities[last_entity->Index].IndexInChunk = cur;

        isize offset = 0;
        for (isize i = 0; i < type->ComponentsIDs.Size(); ++i) {
            u32 id = type->ComponentsIDs[i];
            isize size = m_RegisteredComponents.Sizes[id];
            u8* component_cur = (u8*)(chunk->Data) + offset + size * cur;
            u8* component_last = (u8*)(chunk->Data) + offset + size * last;
            for (isize byte = 0; byte < size; ++byte) {
                component_cur[byte] = component_last[byte];
            }
            offset += size * chunk->EntityCapacity;
        }
    }

    chunk->Count--;
    if (type->Chunks.Size() > 1 && type->Chunks[type->Chunks.Size() - 1].Count == 0) {
        Chunk c = type->Chunks.Pop();
        delete[](u8*)c.Data;
    }
    return true;
}

Entity WorldRegistry::NextEntity()
{
    Entity e;
    // get entity id and version;
    if (m_AvailableIDs.Empty()) {
        e.Index = m_Entities.Size();
        e.Version = 0;
        m_Entities.Resize(e.Index + 1);
        m_Entities[e.Index].Version = e.Version;
    } else {
        e.Index = m_AvailableIDs.Pop();
        e.Version = ++m_Entities[e.Index].Version;
    }
    return e;
}

byte* WorldRegistry::GetComponentDataInternal(Entity e, u32 component_id)
{
    if (e.Index >= m_Entities.Size()) {
        return nullptr;
    }

    EntityInfo* info = &m_Entities[e.Index];
    if (e.Version != info->Version) {
        return nullptr;
    }

    ChunkList* type = &m_Types[info->TypeIndex];
    Chunk* chunk = &type->Chunks[info->ChunkIndex];

    isize offset = 0;
    for (isize i = 0; i < type->ComponentsIDs.Size(); ++i) {
        u32 id = type->ComponentsIDs[i];
        isize size = m_RegisteredComponents.Sizes[id];
        if (id == component_id) {
            offset += size * info->IndexInChunk;
            break;
        }
        offset += size * chunk->EntityCapacity;
    }
    return chunk->Data + offset;
}

u32 WorldRegistry::RegisterTypeInternal(const Vector<u32>& component_ids)
{
    for (isize i = 0; i < m_Types.Size(); ++i) {
        if (m_Types[i].ComponentsIDs == component_ids) {
            return i;
        }
    }

    u32 result_index = m_Types.Size();
    m_Types.Resize(m_Types.Size() + 1);
    ChunkList* result = &m_Types[m_Types.Size() - 1];

    result->ComponentsIDs.Resize(component_ids.Size());
    for (isize i = 0; i < component_ids.Size(); ++i) {
        result->ComponentsIDs[i] = component_ids[i];
    }

    result->Chunks.Resize(1);
    result->Chunks[0].Data = new byte[Chunk::CHUNK_SIZE];

    isize type_size = 0; // the 
    for (isize i = 0; i < result->ComponentsIDs.Size(); ++i) {
        u32 id = result->ComponentsIDs[i];
        type_size += m_RegisteredComponents.Sizes[id];
    }

    result->Chunks[0].EntityCapacity = Chunk::CHUNK_SIZE / type_size;
    result->Chunks[0].Count = 0;
    return result_index;
}

void WorldRegistry::ChunkListPushEntity(Entity e)
{
    ChunkList* type = &m_Types[m_Entities[e.Index].TypeIndex];
    Chunk* chunk = nullptr;
    if (type->Chunks[type->Chunks.Size() - 1].Count == type->Chunks[type->Chunks.Size() - 1].EntityCapacity) {
        type->Chunks.Resize(type->Chunks.Size() + 1);
        chunk = &type->Chunks[type->Chunks.Size() - 1];
        chunk->Count = 0;
        chunk->Data = new byte[Chunk::CHUNK_SIZE];
        chunk->EntityCapacity = type->Chunks[type->Chunks.Size() - 2].EntityCapacity;
    } else {
        chunk = &type->Chunks[type->Chunks.Size() - 1];
    }

    Entity* loc = (Entity*)chunk->Data + chunk->Count;
    *loc = e;

    m_Entities[e.Index].ChunkIndex = type->Chunks.Size() - 1;
    m_Entities[e.Index].IndexInChunk = chunk->Count;
    chunk->Count++;
}

void WorldRegistry::DebugRegisteredComponents() const
{
    printf("Currently registered components: \n");
    for (isize i = 0; i < m_RegisteredComponents.Names.Size(); ++i) {
        printf("component %zu: name = %s, size=%zu\n",
            i, m_RegisteredComponents.Names[i], m_RegisteredComponents.Sizes[i]);
    }
}

void WorldRegistry::DebugRegisteredTypes() const
{
    printf("Currently registered types: \n");
    for (isize i = 0; i < m_Types.Size(); ++i) {
        printf("Type %zu\nComponents:\n", i);
        for (isize j = 0; j < m_Types[i].ComponentsIDs.Size(); ++j) {
            isize id = m_Types[i].ComponentsIDs[j];
            printf("\tname: %s, size: %zu\n", m_RegisteredComponents.Names[id], m_RegisteredComponents.Sizes[id]);
        }
        printf("Chunks:\n");
        for (isize j = 0; j < m_Types[i].Chunks.Size(); ++j) {
            printf("\tchunk %zu: count: %zu max: %zu\n", j,
                m_Types[i].Chunks[j].Count, m_Types[i].Chunks[j].EntityCapacity);
        }
    }

}
void WorldRegistry::DebugRegisteredEntities() const
{
    printf("Currently registered entities: \n");
    for (isize i = 1; i < m_Entities.Size(); ++i) {
        printf("id: %zu, version: %u, type: %u, chunk: %u, position: %u\n",
            i,
            m_Entities[i].Version,
            m_Entities[i].TypeIndex,
            m_Entities[i].ChunkIndex,
            m_Entities[i].IndexInChunk
        );
    }
}

WorldRegistry::ViewIterator WorldRegistry::ViewInternal(const Vector<u32>& component_ids) const
{
    Vector<const ChunkList*> good;
    for (isize i = 0; i < m_Types.Size(); ++i) {
        bool good_type = true;
        for (isize j = 0; j < component_ids.Size(); ++j) {
            bool contained = false;
            for (isize t = 0; t < m_Types[i].ComponentsIDs.Size(); ++t) {
                if (m_Types[i].ComponentsIDs[t] == component_ids[j]) {
                    contained = true;
                    break;
                }
            }
            if (contained == false) {
                good_type = false;
                break;
            }
        }
        if (good_type && m_Types[i].Chunks[0].Count) {
            good.Push(&m_Types[i]);
        }
    }

    if (good.Size() == 0) {
        return ViewIterator();
    }
    return ViewIterator(good, component_ids, &m_RegisteredComponents.Sizes);
}

WorldRegistry::ViewIterator::ViewIterator() : m_AtEnd(false) {}

WorldRegistry::ViewIterator::ViewIterator(const Vector<const ChunkList*>& types, const Vector<u32>& ids, const Vector<isize>* sizes)
    : m_Types(types), m_Sizes(sizes), m_IDs(ids)
{
    // build the current slice
    m_Slice.Resize(m_IDs.Size(), nullptr);
    RebuildSlice();
}


void WorldRegistry::ViewIterator::Next()
{
    m_CurrentIndex++;
    bool need_rebuild = false;
    if (m_CurrentIndex == m_Types[m_CurrentType]->Chunks[m_CurrentChunk].Count) {
        m_CurrentChunk++;
        m_CurrentIndex = 0;
        need_rebuild = true;
    }

    if (m_CurrentChunk == m_Types[m_CurrentType]->Chunks.Size()) {
        m_CurrentType++;
        m_CurrentChunk = 0;
        m_CurrentIndex = 0;
        need_rebuild = true;
    }

    if (!AtEnd()) {
        if (need_rebuild) {
            RebuildSlice();
        } else {
            AdvanceSlice();
        }
    }
}

void WorldRegistry::ViewIterator::RebuildSlice()
{
    isize offset = 0;
    for (isize i = 0; i < m_Types[m_CurrentType]->ComponentsIDs.Size(); ++i) {
        u32 id = m_Types[m_CurrentType]->ComponentsIDs[i];
        isize size = (*m_Sizes)[id];
        const Chunk* chunk = &m_Types[m_CurrentType]->Chunks[m_CurrentChunk];
        for (isize i = 0; i < m_IDs.Size(); ++i) {
            if (id == m_IDs[i]) {
                m_Slice[i] = chunk->Data + offset + size * m_CurrentIndex;
                break;
            }
        }
        offset += size * chunk->EntityCapacity;
    }
}

void WorldRegistry::ViewIterator::AdvanceSlice()
{
    for (isize i = 0; i < m_Slice.Size(); ++i) {
        u32 id = m_IDs[i];
        m_Slice[i] = m_Slice[i] + (*m_Sizes)[id];
    }
}

byte* WorldRegistry::ViewIterator::GetInternal(u32 component_id)
{
    for (isize i = 0; i < m_IDs.Size(); ++i) {
        if (component_id == m_IDs[i]) {
            return m_Slice[i];
        }
    }
    return nullptr;
}

u32 g_NextComponentID = 0;
