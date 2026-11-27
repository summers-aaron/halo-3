#ifndef GPU_PARTICLE_LIGHT_VOLUME_RENDER_SHADER_H
#define GPU_PARTICLE_LIGHT_VOLUME_RENDER_SHADER_H

#include "effects/light_volume_gpu_types.h"
#include "gpu_particle/global_vertex_shader.h"

extern "C" {
extern s_gpu_global_vertex_shader<c_light_volume_gpu::s_render_vertex_shader_constant_list> g_light_volume_gpu_render_shader;
}

#endif // GPU_PARTICLE_LIGHT_VOLUME_RENDER_SHADER_H
