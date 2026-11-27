#ifndef GPU_PARTICLE_CONTRAIL_UPDATE_SHADER_PAIR_H
#define GPU_PARTICLE_CONTRAIL_UPDATE_SHADER_PAIR_H

#include "gpu_particle/shader_slot_types.h"
#include "gpu_particle/gpu_empty_constant_list_types.h"

// The update vertex shader's slot set is followed by its vertex type and pixel shader.
template<class ConstantList>
struct s_gpu_update_shader_pair
{
    long shader_index;
    const s_tag_reference *shader_reference;
    s_gpu_shader<ConstantList> constants;
    long vertex_type;
    s_gpu_pixel_shader<s_gpu_empty_constant_list> pixel;

    __forceinline void dispose()
    {
        shader_reference = 0;
        pixel.shader_reference = 0;
    }
};

#endif // GPU_PARTICLE_CONTRAIL_UPDATE_SHADER_PAIR_H
