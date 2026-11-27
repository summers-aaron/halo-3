#ifndef GPU_PARTICLE_GPU_STORAGE_INLINE_H
#define GPU_PARTICLE_GPU_STORAGE_INLINE_H

#include "gpu_particle/gpu_buffer.h"
#include "gpu_particle/gpu_storage.h"

void water_renderer_set_fetch_constant(int *fetch_constant_words, void *fetch_vertex_buffer, unsigned int fetch_max_index,
    unsigned int fetch_component_swap, unsigned int fetch_numeric_type, unsigned int fetch_format, unsigned int fetch_endian_swap);

#define GPU_STORAGE_TEMPLATE template<typename t_type, long t_surface_swap, long t_color_format, long t_component_format, \
    long t_endian, long t_vertex_format, long t_d3d_type, long t_pack_type>
#define GPU_STORAGE s_gpu_storage<t_type, t_surface_swap, t_color_format, t_component_format, t_endian, t_vertex_format, \
    t_d3d_type, t_pack_type>

GPU_STORAGE_TEMPLATE
inline void GPU_STORAGE::layout(unsigned long vertex_stride, unsigned long element_offset)
{
    stride = vertex_stride / sizeof(t_type);
    offset = element_offset / sizeof(t_type);
}

GPU_STORAGE_TEMPLATE
inline void GPU_STORAGE::refresh()
{
    s_gpu_buffer *buffer = static_cast<s_gpu_buffer *>(owner);
    water_renderer_set_fetch_constant(&bounds.left, owner, buffer->element_size * buffer->element_count * stride,
        t_surface_swap, t_component_format, t_color_format, t_endian);
}

GPU_STORAGE_TEMPLATE
inline unsigned long GPU_STORAGE::size()
{
    return sizeof(t_type);
}

GPU_STORAGE_TEMPLATE
inline long GPU_STORAGE::kind()
{
    return t_vertex_format;
}

GPU_STORAGE_TEMPLATE
inline unsigned long GPU_STORAGE::declaration_type()
{
    return t_d3d_type;
}

GPU_STORAGE_TEMPLATE
__forceinline void GPU_STORAGE::set_value(unsigned long index, const t_type &value)
{
    t_type *destination = static_cast<t_type *>(static_cast<s_gpu_buffer *>(owner)->address);
    destination[stride * index + offset] = value;
}

#undef GPU_STORAGE
#undef GPU_STORAGE_TEMPLATE

#endif // GPU_PARTICLE_GPU_STORAGE_INLINE_H
