#ifndef PHYSICS_RUNTIME_NODE_TO_HAVOK_COMPONENT_RIGID_BODY_INDEX_MAPPING_H
#define PHYSICS_RUNTIME_NODE_TO_HAVOK_COMPONENT_RIGID_BODY_INDEX_MAPPING_H

#include <stddef.h>

// havok_utilities.cpp owns the constructor (0x25a160). The member names and
// offsets agree with its definition and walker_physics.cpp's inline accessor.
class c_runtime_node_to_havok_component_rigid_body_index_mapping
{
public:
    c_runtime_node_to_havok_component_rigid_body_index_mapping(long object_index);
    __forceinline long get_node_count() const { return m_node_count; }
    __forceinline long get_rigid_body_index(long node_index) const
    {
        return node_index == -1 ? m_unmapped_rigid_body : m_node_to_rigid_body_mapping[node_index];
    }

    long m_node_count;
    signed char m_node_to_rigid_body_mapping[256];
    signed char m_unmapped_rigid_body;
};

typedef char runtime_node_to_havok_mapping_size[(sizeof(c_runtime_node_to_havok_component_rigid_body_index_mapping) == 0x108) ? 1 : -1];
typedef char runtime_node_to_havok_mapping_nodes_offset[(offsetof(c_runtime_node_to_havok_component_rigid_body_index_mapping,
    m_node_to_rigid_body_mapping) == 4) ? 1 : -1];
typedef char runtime_node_to_havok_mapping_unmapped_offset[(offsetof(c_runtime_node_to_havok_component_rigid_body_index_mapping,
    m_unmapped_rigid_body) == 0x104) ? 1 : -1];

#endif
