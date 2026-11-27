#ifndef INCLUDE_PHYSICS_RUNTIME_NODE_TO_PHYSICS_MODEL_RIGID_BODY_INDEX_MAPPING_H
#define INCLUDE_PHYSICS_RUNTIME_NODE_TO_PHYSICS_MODEL_RIGID_BODY_INDEX_MAPPING_H
#include "physics/runtime_node_rigid_body_indices_types.h"
class c_runtime_node_to_physics_model_rigid_body_index_mapping
{
public:
    c_runtime_node_to_physics_model_rigid_body_index_mapping(long object_index, bool include_all);
    long get_rigid_body_count(long node_index) const;
    long get_rigid_body_index(long node_index, long rigid_body_index) const;
    int m_node_count;
    union
    {
        s_runtime_node_rigid_body_indices entries[257];
        struct
        {
            s_runtime_node_rigid_body_indices m_node_to_rigid_body_mapping[256];
            s_runtime_node_rigid_body_indices m_unmapped_rigid_bodies;
        };
    };
};
typedef char runtime_node_mapping_size[(sizeof(c_runtime_node_to_physics_model_rigid_body_index_mapping) == 0x4448) ? 1 : -1];
#endif
