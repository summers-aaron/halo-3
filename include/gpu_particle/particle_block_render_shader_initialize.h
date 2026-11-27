#ifndef GPU_PARTICLE_PARTICLE_BLOCK_RENDER_SHADER_INITIALIZE_H
#define GPU_PARTICLE_PARTICLE_BLOCK_RENDER_SHADER_INITIALIZE_H

template<class ConstantList>
__forceinline void s_gpu_global_vertex_shader<ConstantList>::initialize(const s_tag_reference *reference)
{
    shader_reference = reference;
    particle_block_bind_render_slot_set(&constants, initialize_constant_table());
}
#endif
