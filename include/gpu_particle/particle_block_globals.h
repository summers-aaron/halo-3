#ifndef GPU_PARTICLE_PARTICLE_BLOCK_GLOBALS_H
#define GPU_PARTICLE_PARTICLE_BLOCK_GLOBALS_H
#include "physics/physics_constants_tls.h"

#include "effects/particle_array_globals.h"

#include "rasterizer/rasterizer_shader_gpr_globals.h"

#include "gpu_particle/contrail_vertex_declarations.h"
#include "gpu_particle/particle_block_queue_system.h"

struct s_contrail_layout;
struct real_matrix4x3;
extern "C" {
extern s_contrail_layout g_light_volume_gpu_layout;
extern const real_matrix4x3 k_real_math_identity_matrix_storage;
}
#endif // GPU_PARTICLE_PARTICLE_BLOCK_GLOBALS_H
