#ifndef GPU_PARTICLE_LIGHT_VOLUME_GPU_SHADER_INITIALIZE_H
#define GPU_PARTICLE_LIGHT_VOLUME_GPU_SHADER_INITIALIZE_H
#include "gpu_particle/global_vertex_shader.h"
#include "effects/light_volume_gpu_types.h"

template<>
s_entry_table *s_gpu_global_vertex_shader<c_light_volume_gpu::s_render_vertex_shader_constant_list>::initialize_constant_table();
template<>
void s_gpu_global_shader<c_light_volume_gpu::s_render_vertex_shader_constant_list>::initialize(c_rasterizer_constant_table_ref table_ref);

template<>
__forceinline void s_gpu_global_vertex_shader<c_light_volume_gpu::s_render_vertex_shader_constant_list>::initialize(const s_tag_reference *reference)
{
    shader_reference = reference;
    c_rasterizer_constant_table_ref table_ref;
    table_ref.table = initialize_constant_table();
    constants.initialize(table_ref);
}

// The shader argument is the shader cache.
extern "C" s_entry_table *contrail_gpu_get_constant_table(const void *shader);
#endif
