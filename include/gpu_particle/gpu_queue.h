#ifndef GPU_PARTICLE_GPU_QUEUE_H
#define GPU_PARTICLE_GPU_QUEUE_H

#include <xtl.h>

__vector4 gpu_queue_pack_color_vec(const __vector4 &value);
__vector4 gpu_queue_pack_rotation_vec(const __vector4 &value);

#endif // GPU_PARTICLE_GPU_QUEUE_H
