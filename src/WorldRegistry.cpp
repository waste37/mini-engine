#include <WorldRegistry.h>

bool WorldRegistry::Create() {
    m_RegisteredComponents.ComponentCount = 0;
    std::memset(m_RegisteredComponents.Sizes, 0, ComponentMetadata::MAX_COMPONENTS * sizeof(*m_RegisteredComponents.Sizes));
    std::memset(m_RegisteredComponents.TypeMasks, 0, ComponentMetadata::MAX_COMPONENTS * sizeof(*m_RegisteredComponents.TypeMasks));
    m_Entities.Resize(1);
    return true;
}

void WorldRegistry::Destroy() {
    for (usize i = 0; i < m_Types.Size(); ++i) {
        for (usize j = 0; j < m_Types[i].Chunks.Size(); ++j) {
            delete[] (u8*)m_Types[i].Chunks[i].Data;
        }
    }
}

u64 WorldRegistry::RegisterComponent(usize size) {
    if (m_RegisteredComponents.ComponentCount >= ComponentMetadata::MAX_COMPONENTS) {
        return ComponentMetadata::NULL_COMPONENT;
    }
    u64 typemask = (1 << m_RegisteredComponents.ComponentCount);
    m_RegisteredComponents.Sizes[m_RegisteredComponents.ComponentCount] = size;
    m_RegisteredComponents.TypeMasks[m_RegisteredComponents.ComponentCount] = typemask;
    m_RegisteredComponents.ComponentCount++;
    return typemask;
}

Entity WorldRegistry::CreateEntity(uint64_t typemask) {
    // entity ids are strictly > 0 so that we can represent undefined entities using 0
    // we don't care about overflow as probably memory ends before that!
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

    ChunkList *type = RegisterType(typemask);
    m_Entities[e.Index].TypeInfo = type;

    Chunk *chunk = nullptr;
    if (type->Chunks[type->Chunks.Size()-1].Count == type->Chunks[type->Chunks.Size()-1].EntityCapacity) {
        printf("need to create a new chunk\n");
    } else {
        chunk = &type->Chunks[type->Chunks.Size()-1];
    }
    Entity *loc = static_cast<Entity*>(chunk->Data) + chunk->Count;
    *loc = e;
    m_Entities[e.Index].Chunk = chunk;
    m_Entities[e.Index].IndexInChunk = chunk->Count;
    chunk->Count++;

    // we ran out of memory or something..
    if (type == nullptr) return NULL_ENTITY;
    return e;
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
    if (info->Chunk->Count > 0) {
        info->Chunk->Count--;
    }

    ChunkList *type_info = m_Entities[e.Index].TypeInfo;
    if (type_info->Chunks.Size() > 1 && type_info->Chunks[type_info->Chunks.Size()-1].Count == 0) {
        Chunk c = type_info->Chunks.Pop();
        delete [] (u8*)c.Data;
    }

    return true;
}

void *WorldRegistry::GetComponentData(Entity e, u64 mask) {
    if (e.Index >= m_Entities.Size()) {
        return nullptr;
    }

    EntityInfo *info = &m_Entities[e.Index];
    if (e.Version != info->Version) {
        return nullptr;
    }
    usize offset = sizeof(Entity) * info->Chunk->EntityCapacity;

    u64 typemask = info->TypeInfo->TypeMask;
    printf("typemask: 0x%x\n", typemask);
    for (usize i = 0; i < m_RegisteredComponents.ComponentCount; ++i) {
        if (m_RegisteredComponents.TypeMasks[i] & mask) {
            offset += m_RegisteredComponents.Sizes[i] * info->IndexInChunk;
            // we have computed our offset..
            break;
        }
        if ((m_RegisteredComponents.TypeMasks[i] & typemask) == 0) {
            continue;
        }

        offset += m_RegisteredComponents.Sizes[i] * info->Chunk->EntityCapacity;
    printf("offset is %zu\n", offset);
    }

    printf("offset is %zu\n", offset);
    return static_cast<u8*>(info->Chunk->Data) + offset;
    // find the offset of the requested component inside the entity chunk
    // return it
}

ChunkList *WorldRegistry::RegisterType(u64 typemask) {
    if (typemask == 0) {
        return nullptr;
    }
    //printf("asked to register %lx\n", typemask);
    for (usize i = 0; i < m_Types.Size(); ++i) {
        if ((m_Types[i].TypeMask & typemask) == typemask) {
            //printf("type already registered\n");
            return &m_Types[i];
        }
    }

    //printf("creating type\n");
    //let's create it
    m_Types.Resize(m_Types.Size()+1);
    ChunkList *result = &m_Types[m_Types.Size() - 1];
    result->TypeMask = typemask;
    result->Chunks.Resize(1);
    result->Chunks[0].Data = new u8[Chunk::CHUNK_SIZE];

    usize type_size = sizeof(Entity); // the 
    for (usize i = 0; i < m_RegisteredComponents.ComponentCount; ++i) {
        if (m_RegisteredComponents.TypeMasks[i] & typemask) {
            type_size += m_RegisteredComponents.Sizes[i];
        }
    }
    result->Chunks[0].EntityCapacity = Chunk::CHUNK_SIZE / type_size;
    result->Chunks[0].Count = 0;
    return result;
}

void WorldRegistry::DebugRegisteredComponents() const {
    printf("Currently registered components: \n");
    for (usize i = 0; i < m_RegisteredComponents.ComponentCount; ++i) {
        printf("component %zu: mask = 0x%lx, size=%zu\n", 
                i, m_RegisteredComponents.TypeMasks[i], m_RegisteredComponents.Sizes[i]);
    }
}

void WorldRegistry::DebugRegisteredTypes() const {
    printf("Currently registered types: \n");
    for (usize i = 0; i < m_Types.Size(); ++i) {
        printf("type %zu: mask = 0x%lx\n", i, m_Types[i].TypeMask);
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
