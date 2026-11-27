#ifndef GPU_PARTICLE_PARTICLE_BLOCK_QUEUE_TYPES_H
#define GPU_PARTICLE_PARTICLE_BLOCK_QUEUE_TYPES_H

#include "gpu_particle/gpu_buffer.h"
#include "gpu_particle/gpu_storage_inline.h"
#include "gpu_particle/particle_emitter_gpu.h"
#include "gpu_particle/queued_gpu_buffer.h"
#include "math/real_math_types.h"

struct particle_block_queue_frame : s_queued_gpu_layout<c_particle_emitter_gpu::s_layout> {
    particle_block_queue_frame();

    __declspec(align(16)) s_gpu_buffer first;
    s_gpu_buffer second;
    long count;

    __forceinline void write_lock() { first.write_lock(); second.write_lock(); }
    __forceinline void unlock() { first.unlock(); second.unlock(); }
};

struct particle_block_queue_system {
    particle_block_queue_frame frames[2];
    particle_block_queue_frame *current, *next;

    particle_block_queue_system() : current(&frames[0]), next(&frames[1]) {}
};

typedef char particle_layout_size[(sizeof(void *) != 4 || ((sizeof(c_particle_emitter_gpu::s_layout) == 0x350))) ? 1 : -1];
typedef char particle_queue_frame_size[(sizeof(void *) != 4 || ((sizeof(particle_block_queue_frame) == 0x430))) ? 1 : -1];
typedef char particle_queue_first_offset[(sizeof(void *) != 4 || ((offsetof(particle_block_queue_frame, first) == 0x3b0))) ? 1 : -1];
typedef char particle_queue_address_group_offset[(sizeof(void *) != 4 || ((offsetof(particle_block_queue_frame, address_group) == 0x350))) ? 1 : -1];
typedef char particle_queue_system_size[(sizeof(void *) != 4 || ((sizeof(particle_block_queue_system) == 0x870))) ? 1 : -1];
extern particle_block_queue_system g_particle_block_queue_system;

#endif
