#ifndef GPU_PARTICLE_CONTRAIL_EXPORT_TYPES_H
#define GPU_PARTICLE_CONTRAIL_EXPORT_TYPES_H

#include <stddef.h>

// Prefix-compatible with c_rasterizer_vertex_buffer: accepted code passes this global to
// c_rasterizer::set_vertex_stream as one (particle_block.cpp), so the first word is the
// declaration/stride/count header (March c_rasterizer_vertex_buffer).
struct s_contrail_export {
    unsigned char m_declaration;
    unsigned char m_vertex_stride;
    unsigned short m_vertex_count;
    D3DResource *resource;
};

typedef char contrail_export_size[(sizeof(s_contrail_export) == 0x08) ? 1 : -1];
typedef char contrail_export_resource_offset[(offsetof(s_contrail_export, resource) == 0x04) ? 1 : -1];

#endif // GPU_PARTICLE_CONTRAIL_EXPORT_TYPES_H
