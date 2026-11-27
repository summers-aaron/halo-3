#ifndef GPU_PARTICLE_GPU_STORAGE_H
#define GPU_PARTICLE_GPU_STORAGE_H

#include <vectorintrinsics.h>
#include "cseries/cseries.h"
#include "gpu_particle/contrail_target_entry.h"
#include "math/real_math_types.h"

// A vertex element stored as t_type: the memexport stream constant fields of its buffer (component swap, format,
// numeric type, endian swap), the vertex format and D3D declaration type it is declared with and the pack type it is
// written with. The method bodies are in gpu_particle/gpu_storage_inline.h.
template<typename t_type, long t_surface_swap, long t_color_format, long t_component_format, long t_endian,
    long t_vertex_format, long t_d3d_type, long t_pack_type>
struct s_gpu_storage : s_gpu_storage_interface
{
    s_gpu_storage(long semantic, long usage_index) :
        s_gpu_storage_interface(semantic, usage_index)
    {
    }

    virtual void layout(unsigned long vertex_stride, unsigned long element_offset);
    virtual void refresh();
    virtual unsigned long size();
    virtual long kind();
    virtual unsigned long declaration_type();

    void set_value(unsigned long index, const t_type &value);
};

// The vertex elements the GPU particle and contrail layouts store, named by their D3D declaration type.
typedef s_gpu_storage<real_vector4d, 0, 38, 7, 2, 3, 0x1a23a6, NONE> s_gpu_storage_float4;
typedef s_gpu_storage<unsigned __int64, 0, 32, 7, 1, 16, 0x1a2360, VPACK_FLOAT16_4> s_gpu_storage_half4;
typedef s_gpu_storage<unsigned __int64, 0, 26, 0, 1, 10, 0x1a205a, VPACK_NORMSHORT4> s_gpu_storage_ushort4n;
typedef s_gpu_storage<unsigned long, 0, 25, 2, 1, 7, 0x2c2259, VPACK_NORMSHORT2> s_gpu_storage_ushort2;
typedef s_gpu_storage<unsigned long, 0, 25, 0, 1, 9, 0x2c2059, VPACK_NORMSHORT2> s_gpu_storage_ushort2n;
typedef s_gpu_storage<unsigned long, 0, 31, 7, 1, 15, 0x2c235f, VPACK_FLOAT16_2> s_gpu_storage_half2;
typedef s_gpu_storage<unsigned long, 1, 6, 0, 2, 6, 0x182886, VPACK_D3DCOLOR> s_gpu_storage_color;
typedef s_gpu_storage<unsigned long, 0, 7, 1, 2, 17, 0x2a2187, VPACK_NORMPACKED32> s_gpu_storage_dec3n;

#endif // GPU_PARTICLE_GPU_STORAGE_H
