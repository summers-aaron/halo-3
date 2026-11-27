#ifndef GPU_PARTICLE_CONTRAIL_SHADER_INITIALIZERS_H
#define GPU_PARTICLE_CONTRAIL_SHADER_INITIALIZERS_H

#include "gpu_particle/contrail_gpu_types.h"
#include "gpu_particle/vertex_shader.h"

template<> void s_gpu_vertex_shader<c_contrail_gpu::s_spawn_vertex_shader_constant_list>::initialize();
template<> void s_gpu_shader<c_contrail_gpu::s_update_vertex_shader_constant_list>::initialize(c_rasterizer_constant_table_ref table_ref);

#endif // GPU_PARTICLE_CONTRAIL_SHADER_INITIALIZERS_H
