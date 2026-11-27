#ifndef GPU_PARTICLE_CONTRAIL_VERTEX_DECLARATIONS_H
#define GPU_PARTICLE_CONTRAIL_VERTEX_DECLARATIONS_H

#include <stddef.h>

#include "rasterizer/rasterizer_vertex_declaration_types.h"

struct s_gpu_buffer;

extern "C" {
extern s_gpu_buffer g_contrail_frame_vertex_buffer;
}

#endif // GPU_PARTICLE_CONTRAIL_VERTEX_DECLARATIONS_H
