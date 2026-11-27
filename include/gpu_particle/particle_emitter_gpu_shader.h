#ifndef GPU_PARTICLE_PARTICLE_EMITTER_GPU_SHADER_H
#define GPU_PARTICLE_PARTICLE_EMITTER_GPU_SHADER_H

#include "gpu_particle/particle_emitter_gpu.h"
#include "gpu_particle/vertex_shader.h"

template<> void s_gpu_vertex_shader<c_particle_emitter_gpu::s_spawn_vertex_shader_constant_list>::initialize();

#endif // GPU_PARTICLE_PARTICLE_EMITTER_GPU_SHADER_H
