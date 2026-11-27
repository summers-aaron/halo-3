#ifndef GPU_PARTICLE_SHADER_PAIR_H
#define GPU_PARTICLE_SHADER_PAIR_H

#include "gpu_particle/vertex_shader.h"

template<class VertexConstants, class PixelConstants>
struct s_gpu_shader_pair
{
    s_gpu_vertex_shader<VertexConstants> vertex_shader;
    s_gpu_pixel_shader<PixelConstants> pixel_shader;

    __forceinline void initialize()
    {
        vertex_shader.initialize();
        pixel_shader.initialize();
    }

    __forceinline void dispose()
    {
        vertex_shader.shader_reference = 0;
        pixel_shader.shader_reference = 0;
    }
};

#endif // GPU_PARTICLE_SHADER_PAIR_H
