#ifndef GEOMETRY_RENDER_GEOMETRY_MESH_TYPES_H
#define GEOMETRY_RENDER_GEOMETRY_MESH_TYPES_H
#include "tag_files/tag_types.h"
#include <stddef.h>

// Element of s_mesh::instance_buckets: one mesh supernode with its block of
// instance indices. Same 16-byte shape as the predicted-resources view.
struct s_mesh_instance_bucket
{
    unsigned short mesh_index;
    char alignment[2];
    c_typed_tag_block<unsigned short> instances;
};

typedef char mesh_instance_bucket_size[(sizeof(s_mesh_instance_bucket) == 16) ? 1 : -1];
typedef char mesh_instance_bucket_instances_offset[(offsetof(s_mesh_instance_bucket, instances) == 4) ? 1 : -1];

// Xbox 360 serialized mesh: rigid node and vertex format at 0x2d and 0x2e.
struct s_mesh
{
    s_tag_block parts;
    s_tag_block subparts;
    short vertex_buffer_indices[8];
    short index_buffer_indices[2];
    unsigned char flags;
    unsigned char node_index;
    unsigned char vertex_type;
    unsigned char prt_type;
    unsigned char index_buffer_type;
    unsigned char alignment[3];
    s_tag_block instance_buckets;
    s_tag_block water_indices;
};
struct s_mesh_node_map
{
    long node_count;
    TAG_POINTER(const unsigned char) nodes;
    TAG_POINTER(void) definition;
};
typedef char mesh_size[(sizeof(s_mesh) == 0x4c) ? 1 : -1];
typedef char mesh_node_offset[(offsetof(s_mesh, node_index) == 0x2d) ? 1 : -1];
typedef char mesh_vertex_type_offset[(offsetof(s_mesh, vertex_type) == 0x2e) ? 1 : -1];
#endif
