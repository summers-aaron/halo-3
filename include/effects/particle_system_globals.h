#include "game/player_mapping_globals.h"
#ifndef EFFECTS_PARTICLE_SYSTEM_GLOBALS_H
#define EFFECTS_PARTICLE_SYSTEM_GLOBALS_H
#include "game/player_control_globals.h"

#include "gpu_particle/shared_gpu_storage.h"
#include "rasterizer/rasterizer_vertex_definitions.h"

#include "effects/particle_array_globals.h"
#include "gpu_particle/spawn_shader_pair.h"
#include "gpu_particle/global_vertex_shader.h"
#include "gpu_particle/particle_emitter_gpu.h"

#include "memory/data_types.h"
#include "effects/effect_array_globals.h"
#include "game/game_globals_storage.h"
#include "game/player_mapping_types.h"
#include "cutscene/cinematic_globals_types.h"
#include "effects/particle_effect_data_array_types.h"

struct s_particle_system_limits_owner;
#include "camera/observer_globals_types.h"
class c_particle_system;
#include "cache/restricted_memory_regions.h"
void __tls_set_g_impact_array_data_allocator(void *object);
extern "C" void particle_emitter_gpu_set_storage_address(void *address);

extern "C" {
extern s_particle_spawn_shader_pair g_particle_spawn_shaders;
extern s_gpu_global_vertex_shader<c_particle_emitter_gpu::s_render_vertex_shader_constant_list> g_particle_render_vertex_shader;
extern s_gpu_global_vertex_shader<c_particle_emitter_gpu::s_render_vertex_shader_constant_list> g_particle_render_model_vertex_shader;
#include "effects/effect_data.h"
extern __declspec(thread) void *g_particle_gpu_storage_address;
extern t_restricted_allocation_manager<1, static_cast<void (*)(void *)>(0), static_cast<void (*)(void *)>(0),
    &__tls_set_g_impact_array_data_allocator> g_particle_gpu_data_allocator;
extern t_restricted_allocation_manager<2, static_cast<void (*)(void *)>(0), static_cast<void (*)(void *)>(0),
    &particle_emitter_gpu_set_storage_address> g_particle_gpu_storage_allocator;
long particle_system_pulse_emitters(c_particle_system *system, float delta);
void particle_block_refresh_vertex_buffers(void *storage);
}

#endif
