#ifndef GPU_PARTICLE_LIGHT_VOLUME_UPDATE_SHADER_TYPES_H
#define GPU_PARTICLE_LIGHT_VOLUME_UPDATE_SHADER_TYPES_H

#include <stddef.h>
#include "effects/light_volume_gpu_types.h"
#include "gpu_particle/gpu_empty_constant_list_types.h"
#include "gpu_particle/shader_pair.h"

typedef s_gpu_shader_pair<c_light_volume_gpu::s_update_vertex_shader_constant_list,
    s_gpu_empty_constant_list> s_light_volume_update_shader_pair;
typedef char light_volume_update_vertex_reference_offset[
    (sizeof(void *) != 4 || offsetof(s_light_volume_update_shader_pair, vertex_shader) +
        offsetof(s_gpu_vertex_shader<c_light_volume_gpu::s_update_vertex_shader_constant_list>, shader_reference) == 4) ? 1 : -1];
typedef char light_volume_update_pixel_offset[
    (sizeof(void *) != 4 || offsetof(s_light_volume_update_shader_pair, pixel_shader) == 0x70) ? 1 : -1];
extern "C" { extern s_light_volume_update_shader_pair g_light_volume_gpu_update_shader; }

#endif // GPU_PARTICLE_LIGHT_VOLUME_UPDATE_SHADER_TYPES_H
