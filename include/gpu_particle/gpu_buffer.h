#ifndef GPU_PARTICLE_GPU_BUFFER_H
#define GPU_PARTICLE_GPU_BUFFER_H

#include <stddef.h>
#include "gpu_particle/common_gpu.h"
#include "rasterizer/rasterizer_vertex_buffer.h"

// s_gpu_buffer: the common vertex buffer plus write-lock state.
// March emits the same lock operations, with its earlier, smaller buffer layout.
struct s_gpu_buffer : common_gpu_vertex_buffer
{
    bool locked;

#if !defined(_XBOX) || defined(__clang__)
    constexpr s_gpu_buffer() : common_gpu_vertex_buffer(), locked(false) {}
    constexpr s_gpu_buffer(int format, int element_count, int element_size) :
        common_gpu_vertex_buffer(format, element_count, element_size), locked(false) {}
#endif
    __forceinline void write_lock()
    {
        if (!locked)
        {
            locked = true;
            c_rasterizer_vertex_buffer_factory_interface::write_lock(this);
        }
    }
    __forceinline void unlock()
    {
        if (locked)
        {
            locked = false;
            c_rasterizer_vertex_buffer_factory_interface::unlock(this);
        }
    }
};
typedef char gpu_buffer_size[(sizeof(void *) != 4 || ((sizeof(s_gpu_buffer) == 0x3c))) ? 1 : -1];
typedef char gpu_buffer_lock_offset[(sizeof(void *) != 4 || ((offsetof(s_gpu_buffer, locked) == 0x38))) ? 1 : -1];

#endif
