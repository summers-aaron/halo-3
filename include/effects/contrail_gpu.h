#ifndef INCLUDE_EFFECTS_CONTRAIL_GPU_H
#define INCLUDE_EFFECTS_CONTRAIL_GPU_H

#include "gpu_particle/contrail_gpu_types.h"

extern "C" void contrail_gpu_refresh_vertex_buffers(s_queued_gpu_buffer_system<c_contrail_gpu::s_layout, 128> *buffer_system);

#endif // INCLUDE_EFFECTS_CONTRAIL_GPU_H
