#ifndef GPU_PARTICLE_PARTICLE_UPDATE_SHADER_H
#define GPU_PARTICLE_PARTICLE_UPDATE_SHADER_H

#include <stddef.h>
#include "gpu_particle/gpu_empty_constant_list_types.h"
#include "gpu_particle/particle_emitter_gpu.h"
#include "gpu_particle/shader_pair.h"

typedef s_gpu_shader_pair<c_particle_emitter_gpu::s_update_vertex_shader_constant_list,
    s_gpu_empty_constant_list> s_update_shader_pair;

typedef char particle_update_shader_size[(sizeof(void *) != 4 || ((sizeof(s_update_shader_pair) == 0x88))) ? 1 : -1];
typedef char particle_update_pixel_reference_offset[
    (sizeof(void *) != 4 || ((offsetof(s_update_shader_pair, pixel_shader)
    + offsetof(s_gpu_pixel_shader<s_gpu_empty_constant_list>, shader_reference) == 0x68))) ? 1 : -1];

extern "C" { extern s_update_shader_pair g_particle_update_shaders; }

#endif // GPU_PARTICLE_PARTICLE_UPDATE_SHADER_H
