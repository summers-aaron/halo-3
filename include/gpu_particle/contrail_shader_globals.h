#ifndef GPU_PARTICLE_CONTRAIL_SHADER_GLOBALS_H
#define GPU_PARTICLE_CONTRAIL_SHADER_GLOBALS_H

#include "gpu_particle/contrail_gpu_types.h"
#include "gpu_particle/contrail_update_shader_pair.h"

extern "C" s_gpu_update_shader_pair<c_contrail_gpu::s_update_vertex_shader_constant_list> g_contrail_update_shaders;

#endif // GPU_PARTICLE_CONTRAIL_SHADER_GLOBALS_H
