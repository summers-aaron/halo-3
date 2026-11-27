#ifndef GPU_PARTICLE_CONTRAIL_QUEUE_SYSTEM_H
#define GPU_PARTICLE_CONTRAIL_QUEUE_SYSTEM_H

#include "gpu_particle/contrail_gpu_types.h"
#include "gpu_particle/queued_gpu_buffer.h"

template<>
s_queued_gpu_buffer<c_contrail_gpu::s_layout, 128>::s_queued_gpu_buffer();

typedef char contrail_layout_size[(sizeof(void *) != 4 || ((sizeof(c_contrail_gpu::s_layout) == 0x270))) ? 1 : -1];
typedef char contrail_layout_position_offset[(sizeof(void *) != 4 || ((offsetof(c_contrail_gpu::s_layout, position) == 0x130))) ? 1 : -1];
typedef char contrail_queue_address_group_offset[
    (sizeof(void *) != 4 || ((offsetof(s_queued_gpu_layout<c_contrail_gpu::s_layout>, address_group) == 0x270))) ? 1 : -1];
typedef char contrail_queue_layout_size[(sizeof(void *) != 4 || ((sizeof(s_queued_gpu_layout<c_contrail_gpu::s_layout>) == 0x2d0))) ? 1 : -1];
typedef char contrail_queue_frame_size[(sizeof(void *) != 4 || ((sizeof(s_queued_gpu_buffer<c_contrail_gpu::s_layout, 128>) == 0x350))) ? 1 : -1];
typedef char contrail_queue_system_size[
    (sizeof(void *) != 4 || ((sizeof(s_queued_gpu_buffer_system<c_contrail_gpu::s_layout, 128>) == 0x6b0))) ? 1 : -1];

#endif
