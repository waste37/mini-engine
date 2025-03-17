#include <WorldRegistry.h>

bool WorldRegistry::Create() {
    RegisterComponent<Entity>();
    m_Entities.Push({nullptr, nullptr, 0, 0});
    return true;
}

void WorldRegistry::Destroy() {
    for (usize i = 0; i < m_Types.Size(); ++i) {
        for (usize j = 0; j < m_Types[i].Chunks.Size(); ++j) {
            delete[] (u8*)m_Types[i].Chunks[j].Data;
        }
    }
}

bool WorldRegistry::DeleteEntity(Entity e) {
    // doesn't exist: never generated such index
    if (e.Index >= m_Entities.Size()) {
        return false;
    }
    // doesn't exist: already deleted
    EntityInfo *info = &m_Entities[e.Index];
    if (e.Version != info->Version) {
        return false;
    }

    m_AvailableIDs.Push(e.Index);
    info->Version++;

    // swap last entity with the one that is being deleted
    if (info->Chunk->Count > 1) {
        usize last = info->Chunk->Count - 1;
        usize cur = info->IndexInChunk;

        Entity *last_entity = (Entity *)info->Chunk->Data + last;
        m_Entities[last_entity->Index].IndexInChunk = cur;

        usize offset = 0;
        for (usize i = 0; i < info->TypeInfo->ComponentsIDs.Size(); ++i) {
            u32 id = info->TypeInfo->ComponentsIDs[i];
            usize size = m_RegisteredComponents.Sizes[id];
            u8 *component_cur = (u8*)(info->Chunk->Data) + offset + size * cur;
            u8 *component_last = (u8*)(info->Chunk->Data) + offset + size * last;
            for (usize byte = 0; byte < size; ++byte) {
                component_cur[byte] = component_last[byte];
            }

            offset += size * info->Chunk->EntityCapacity;
        }
    }

    info->Chunk->Count--;

    ChunkList *type_info = m_Entities[e.Index].TypeInfo;
    if (type_info->Chunks.Size() > 1 && type_info->Chunks[type_info->Chunks.Size()-1].Count == 0) {
        Chunk c = type_info->Chunks.Pop();
        delete [] (u8*)c.Data;
    }
    return true;
}

Entity WorldRegistry::NextEntity() {
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

ChunkList *WorldRegistry::RegisterTypeInternal(const Vector<u32> &component_ids) {
    for (usize i = 0; i < m_Types.Size(); ++i) {
        if (m_Types[i].ComponentsIDs == component_ids) {
            return &m_Types[i];
        }
    }

    m_Types.Resize(m_Types.Size()+1);
    ChunkList *result = &m_Types[m_Types.Size() - 1];

    result->ComponentsIDs.Resize(component_ids.Size());
    for (usize i = 0; i < component_ids.Size(); ++i) {
        result->ComponentsIDs[i] = component_ids[i];
    }

    result->Chunks.Resize(1);
    result->Chunks[0].Data = new u8[Chunk::CHUNK_SIZE];

    usize type_size = 0; // the 
    for (usize i = 0; i < result->ComponentsIDs.Size(); ++i) {
        u32 id = result->ComponentsIDs[i];
        type_size += m_RegisteredComponents.Sizes[id];
    }

    result->Chunks[0].EntityCapacity = Chunk::CHUNK_SIZE / type_size;
    result->Chunks[0].Count = 0;
    return result;
}

void WorldRegistry::ChunkListPushEntity(Entity e) {
    ChunkList *type = m_Entities[e.Index].TypeInfo;
    Chunk *chunk = nullptr;
    if (type->Chunks[type->Chunks.Size()-1].Count == type->Chunks[type->Chunks.Size()-1].EntityCapacity) {
        type->Chunks.Resize(type->Chunks.Size()+1);
        chunk = &type->Chunks[type->Chunks.Size()-1];  
        chunk->Count = 0;
        chunk->Data = new u8[Chunk::CHUNK_SIZE];
        chunk->EntityCapacity = type->Chunks[type->Chunks.Size()-2].EntityCapacity;
    } else {
        chunk = &type->Chunks[type->Chunks.Size()-1];
    }

    Entity *loc = (Entity*)chunk->Data + chunk->Count;
    *loc = e;
    m_Entities[e.Index].Chunk = chunk;
    m_Entities[e.Index].IndexInChunk = chunk->Count;
    chunk->Count++;
}

void WorldRegistry::DebugRegisteredComponents() const {
    printf("Currently registered components: \n");
    for (usize i = 0; i < m_RegisteredComponents.Names.Size(); ++i) {
        printf("component %zu: name = %s, size=%zu\n", 
                i, m_RegisteredComponents.Names[i], m_RegisteredComponents.Sizes[i]);
    }
}

void WorldRegistry::DebugRegisteredTypes() const {
    printf("Currently registered types: \n");
    for (usize i = 0; i < m_Types.Size(); ++i) {
        printf("Type %zu\nComponents:\n", i);
        for (usize j = 0; j < m_Types[i].ComponentsIDs.Size(); ++j) {
            usize id = m_Types[i].ComponentsIDs[j];
            printf("\tname: %s, size: %zu\n", m_RegisteredComponents.Names[id], m_RegisteredComponents.Sizes[id]);
        }
        printf("Chunks:\n");
        for (usize j = 0; j < m_Types[i].Chunks.Size(); ++j) {
            printf("\tchunk %zu: count: %zu max: %zu\n", j,
            m_Types[i].Chunks[j].Count, m_Types[i].Chunks[j].EntityCapacity);
        }
    }

}
void WorldRegistry::DebugRegisteredEntities() const {
    printf("Currently registered entities: \n");
    for (usize i = 1; i < m_Entities.Size(); ++i) {
        printf("id: %zu, version: %u, type: %p, chunk: %p, position: %u\n", 
            i, 
            m_Entities[i].Version,
            m_Entities[i].TypeInfo,
            m_Entities[i].Chunk,
            m_Entities[i].IndexInChunk
        );
    }
}

u32 g_NextComponentID = 0;
