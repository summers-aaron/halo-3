#ifndef GPU_PARTICLE_COMMON_GPU_H
#define GPU_PARTICLE_COMMON_GPU_H

// Functions defined in tu/graphicslibxbox/common_gpu.cpp that other units call.

#ifndef RASTERIZER_XENON_RASTERIZER_BUFFER_RESOURCE_TYPES_H
#include <xtl.h>
#endif
#include <stddef.h>
#include "rasterizer/rasterizer_vertex_buffer.h"

class common_gpu_vertex_buffer : public c_rasterizer_vertex_buffer {
public:
    D3DVertexBuffer xenon_buffer;
    void *address;
    int format, element_count, element_size;

#if !defined(_XBOX) || defined(__clang__)
    constexpr common_gpu_vertex_buffer() : c_rasterizer_vertex_buffer(), xenon_buffer(), address(0), format(0), element_count(0), element_size(0) {}
    constexpr common_gpu_vertex_buffer(int format, int element_count, int element_size) :
        c_rasterizer_vertex_buffer(), xenon_buffer(), address(0), format(format), element_count(element_count), element_size(element_size) {}
#endif
};

typedef char common_gpu_buffer_size[(sizeof(void *) != 4 || ((sizeof(common_gpu_vertex_buffer) == 0x38))) ? 1 : -1];
typedef char common_gpu_buffer_address_offset[(sizeof(void *) != 4 || ((offsetof(common_gpu_vertex_buffer, address) == 0x28))) ? 1 : -1];

extern "C" void common_gpu_vertex_buffer_set_address(common_gpu_vertex_buffer *object, void *address);

#include "gpu_particle/gpu_constant_table_types.h"

#endif // GPU_PARTICLE_COMMON_GPU_H
