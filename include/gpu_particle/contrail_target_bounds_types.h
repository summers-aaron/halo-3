#ifndef GPU_PARTICLE_CONTRAIL_TARGET_BOUNDS_TYPES_H
#define GPU_PARTICLE_CONTRAIL_TARGET_BOUNDS_TYPES_H

#include "gpu_particle/particle_block_bounds_types.h"
#include "math/real_math_types.h"
#include "memory/data_types.h"

struct __declspec(align(16)) s_contrail_target_bounds {
    s_particle_block_bounds bounds;
    real_point2d size;
};

#endif // GPU_PARTICLE_CONTRAIL_TARGET_BOUNDS_TYPES_H
