#ifndef GPU_PARTICLE_PASS_STATE_TYPES_H
#define GPU_PARTICLE_PASS_STATE_TYPES_H

#include "gpu_particle/gpu_constant_table_types.h"

// The pass state IS the shader constants: the sole caller passes &constants and the callee uses it as a
// constant-table base (descs + set_constants), the same alias this slot-set memory carries everywhere else.
struct s_pass_state : s_gpu_constant_table_base
{
};

#endif // GPU_PARTICLE_PASS_STATE_TYPES_H
