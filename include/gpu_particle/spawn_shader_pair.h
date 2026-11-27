#ifndef GPU_PARTICLE_SPAWN_SHADER_PAIR_H
#define GPU_PARTICLE_SPAWN_SHADER_PAIR_H

#include "gpu_particle/contrail_gpu_types.h"
#include "gpu_particle/gpu_empty_constant_list_types.h"
#include "gpu_particle/particle_emitter_gpu.h"
#include "gpu_particle/shader_pair.h"

typedef s_gpu_shader_pair<c_particle_emitter_gpu::s_spawn_vertex_shader_constant_list,
    s_gpu_empty_constant_list> s_particle_spawn_shader_pair;
typedef s_gpu_shader_pair<c_contrail_gpu::s_spawn_vertex_shader_constant_list,
    s_gpu_empty_constant_list> s_contrail_spawn_shader_pair;

#endif
