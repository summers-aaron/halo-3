#ifndef GPU_PARTICLE_PARTICLE_BLOCK_CONTRAIL_SHADER_PAIR_H
#define GPU_PARTICLE_PARTICLE_BLOCK_CONTRAIL_SHADER_PAIR_H

#include "gpu_particle/vertex_shader.h"
#include "gpu_particle/particle_block_contrail_vertex_shader_constant_list_types.h"
#include "gpu_particle/gpu_empty_constant_list_types.h"

struct s_particle_block_contrail_shader_pair
{
    s_gpu_vertex_shader<s_particle_block_contrail_vertex_shader_constant_list> vertex_shader;
    s_gpu_pixel_shader<s_gpu_empty_constant_list> pixel_shader;

    __forceinline void initialize()
    {
        vertex_shader.initialize();
        pixel_shader.initialize();
    }
};

#endif // GPU_PARTICLE_PARTICLE_BLOCK_CONTRAIL_SHADER_PAIR_H
