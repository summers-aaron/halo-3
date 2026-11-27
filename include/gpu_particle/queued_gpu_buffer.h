#ifndef GPU_PARTICLE_QUEUED_GPU_BUFFER_H
#define GPU_PARTICLE_QUEUED_GPU_BUFFER_H

#include <stddef.h>
#include "gpu_particle/gpu_buffer.h"
#include "gpu_particle/gpu_storage.h"
#include "gpu_particle/target_table_types.h"

// The queued element's address, a layout of one element.
struct particle_block_queue_address_group : s_gpu_target_table<1>
{
    __forceinline particle_block_queue_address_group() :
        address(3, 1)
    {
        slots[0] = &address;
    }

    s_gpu_storage_ushort2 address;
};

// A layout queued for the GPU: its elements and the queued element address.
template<class t_layout>
struct s_queued_gpu_layout : t_layout
{
    __declspec(align(16)) particle_block_queue_address_group address_group;
};

// One queued frame of a layout: the layout and the two vertex buffers it is written to.
template<class t_layout, long k_gpu_max>
struct s_queued_gpu_buffer : s_queued_gpu_layout<t_layout>
{
    s_queued_gpu_buffer();

    __declspec(align(16)) s_gpu_buffer first;
    s_gpu_buffer second;
    long count;

    __forceinline void write_lock()
    {
        first.write_lock();
        second.write_lock();
    }
    __forceinline void unlock()
    {
        first.unlock();
        second.unlock();
    }
};

// Two queued frames: the one the GPU reads and the one being filled.
template<class t_layout, long k_gpu_max>
struct s_queued_gpu_buffer_system
{
    s_queued_gpu_buffer_system() :
        current(&frames[0]),
        next(&frames[1])
    {
    }

    __forceinline void swap()
    {
        long index = next - frames;
        current = &frames[index];
        next = &frames[(index + 1) % 2];
        next->count = 0;
    }

    __forceinline void reset_active()
    {
        current->count = 0;
        next->count = 0;
    }

    s_queued_gpu_buffer<t_layout, k_gpu_max> frames[2];
    s_queued_gpu_buffer<t_layout, k_gpu_max> *current, *next;
};

typedef char queued_gpu_address_offset[(offsetof(particle_block_queue_address_group, address) == 0x30) ? 1 : -1];

#endif // GPU_PARTICLE_QUEUED_GPU_BUFFER_H
