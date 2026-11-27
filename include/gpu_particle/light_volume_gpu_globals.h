#ifndef GPU_PARTICLE_LIGHT_VOLUME_GPU_GLOBALS_H
#define GPU_PARTICLE_LIGHT_VOLUME_GPU_GLOBALS_H

#include "gpu_particle/shared_gpu_storage.h"
#include "gpu_particle/light_volume_allocators.h"
#include "gpu_particle/contrail_vertex_declarations.h"
#include "gpu_particle/light_volume_render_shader.h"
#include "cache/restricted_address.h"
#include "effects/effect_runtime_storage.h"
#include "rasterizer/rasterizer_vertex_definitions.h"

#include "gpu_particle/gpu_debug.h"

struct s_data_array;
struct s_instance_array;

class common_gpu_vertex_buffer;
#include "rasterizer/rasterizer_vertex_declaration_types.h"

struct s_light_volume_gpu_target_table;

extern "C" {
extern const bool k_light_volume_render_required[];
extern const int k_light_volume_render_keys[];
#if defined(_XBOX) && !defined(__clang__)
extern const volatile float k_light_volume_gpu_zero;
#endif
extern common_gpu_vertex_buffer g_light_volume_gpu_buffer;
extern s_light_volume_gpu_target_table g_light_volume_gpu_target_table[];
extern unsigned char g_light_volume_gpu_allocation_succeeded[];
extern unsigned char g_light_volume_gpu_row_allocation_succeeded[];
extern __declspec(thread) void *g_light_volume_gpu_buffer_address;
extern unsigned char g_light_volume_gpu_block_until_idle;
}

#include "gpu_particle/light_volume_update_shader_types.h"
template<class ConstantList> class s_gpu_global_vertex_shader;
struct s_gpu_empty_constant_list;
extern "C" {
extern unsigned long k_light_volume_states_initial_flags;
extern s_gpu_global_vertex_shader<c_light_volume_gpu::s_render_vertex_shader_constant_list> g_effect_shader_cache;
}
#endif
