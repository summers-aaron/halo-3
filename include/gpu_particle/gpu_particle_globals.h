#ifndef GPU_PARTICLE_GPU_PARTICLE_GLOBALS_H
#define GPU_PARTICLE_GPU_PARTICLE_GLOBALS_H
#include "physics/physics_constants_tls.h"

#include "gpu_particle/shared_gpu_storage.h"
#include "gpu_particle/light_volume_allocators.h"
#include "gpu_particle/contrail_allocators.h"
#include "rasterizer/rasterizer_vertex_definitions.h"

#include "gpu_particle/gpu_debug.h"

#include "effects/particle_array_globals.h"
#include "ai/actor_dynamic_firing_position.h"

#include "objects/widgets/widgets.h"
#include "effects/effect_array_globals.h"
#include "gpu_particle/global_vertex_shader.h"
#include "gpu_particle/contrail_gpu_types.h"
#include "effects/light_volume_gpu_types.h"

class common_gpu_vertex_buffer;
struct s_particle_block_contrail_shader_pair;
struct s_observer_result;
struct s_gpu_variant_data;
struct s_effects_particle_system_list;

extern "C" {
extern common_gpu_vertex_buffer g_contrail_gpu_vertex_buffer;
extern s_gpu_global_vertex_shader<c_light_volume_gpu::s_render_vertex_shader_constant_list> g_effect_shader_cache;
extern common_gpu_vertex_buffer g_particle_gpu_vertex_buffer;
extern unsigned char g_particle_gpu_block_until_idle;
extern s_particle_block_contrail_shader_pair g_contrail_frame_shader_pair;
extern unsigned long k_contrail_default_dirty_mask;
extern unsigned char g_contrail_element_alloc_success[];
extern unsigned char g_contrail_row_alloc_success[];
extern unsigned char g_contrail_link_alloc_success[];
extern unsigned char g_particle_element_alloc_success[];
extern bool k_particle_block_7slot_required[];
extern int k_particle_block_7slot_keys[];
extern const s_observer_result *g_particle_render_camera;
extern unsigned char g_effect_camera_active;
extern s_gpu_variant_data g_particle_model_gpu_variant;
extern s_gpu_variant_data k_particle_default_gpu_variant;
extern unsigned char g_particle_queue_alloc_success[];
extern unsigned long g_particle_default_used_states;
extern const s_observer_result k_default_particle_block_view;
extern s_effects_particle_system_list g_particle_system_lists[];
}

#include "math/matrix_math_types.h"
struct s_data_array;
#include "game/game_time_types.h"
#include "gpu_particle/spawn_shader_pair.h"
struct s_contrail_layout;
struct s_weighted_atmosphere_parameters;
struct s_geometry_sample;
struct particle_block_declaration;
struct particle_block_declaration_element;
struct particle_block_queue_system;

extern "C" {
extern s_particle_spawn_shader_pair g_particle_spawn_shaders;
extern s_contrail_spawn_shader_pair g_contrail_spawn_shaders;
extern s_contrail_layout g_contrail_export_layout;
#include "effects/effect_lightprobe_memory.h"
}
extern unsigned char g_contrail_gpu_block_until_idle;



extern "C" {
extern s_gpu_global_vertex_shader<c_contrail_gpu::s_render_vertex_shader_constant_list> g_contrail_render_vertex_shader;
extern __declspec(thread) void *g_contrail_gpu_buffer_address;
}
extern particle_block_queue_system g_particle_block_queue_system;

#endif // GPU_PARTICLE_GPU_PARTICLE_GLOBALS_H
