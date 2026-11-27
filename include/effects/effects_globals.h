#ifndef EFFECTS_EFFECTS_GLOBALS_H
#define EFFECTS_EFFECTS_GLOBALS_H
#include "cutscene/cinematic_globals_types.h"
#include "effects/effect_runtime_storage.h"

#include "effects/contrail_profile_data.h"
#include "game/players_global_data.h"
#include "game/player_mapping_globals.h"
#include "game/player_data.h"

#include "effects/particle_array_globals.h"

#include "effects/effect_event_data.h"
#include "effects/effect_array_globals.h"
#include "effects/effect_location_data.h"
#include "physics/breakable_surface_event_data.h"
#include "effects/effect_counts.h"
#include "effects/effect_lightprobe_slot_data.h"
#include "effects/effect_lightprobe_memory.h"

#include "cache/restricted_memory_regions.h"
#include "effects/effects.h"
#include "effects/contrails.h"
#include "game/player_control.h"
#include "physics/impacts.h"

struct s_effect_render_location;
struct s_effect_camera_info;
struct s_data_array;
struct s_particle_system_limits_owner;

// Address tags stay in effects.cpp; these declarations emit no storage.
extern "C" {
extern unsigned int g_effect_render_message_count;
extern s_effect_render_location g_effect_render_locations[128];
extern unsigned long g_effect_render_location_count;
extern c_render_batch *g_effect_damage_batch;
extern long g_effect_last_particle_system_index;
extern long g_effect_last_contrail_system_index;
extern long g_effect_last_light_volume_system_index;
extern long g_effect_last_contrail_profile_index;
extern s_effect_camera_info g_effect_camera_world_info;
extern const char k_effect_creation_random_name[];
extern float k_effect_non_looping_death_delay;
extern bool g_effect_cull_nonviolent_corpses;
extern int g_effect_location_warning_count;
extern s_effect_camera_info g_effect_camera_info;
extern long g_effect_limit_count_index;
extern bool g_effect_limit_reached[3];
extern bool g_event_limit_reached[3];
extern bool g_effect_location_limit_reached[3];
extern bool g_effect_scenery_limit_reached;
extern bool g_effect_lightprobe_limit_reached[3];
extern bool g_effect_contrail_profile_limit_reached[3];
}

typedef t_restricted_allocation_manager<3, static_cast<void (*)(void *)>(0), static_cast<void (*)(void *)>(0),
    __tls_set_g_effect_data_allocator> t_effect_data_allocator;
typedef t_restricted_allocation_manager<2, static_cast<void (*)(void *)>(0), static_cast<void (*)(void *)>(0),
    &__tls_set_g_havok_game_state_allocator> t_effect_lightprobe_allocator;
extern t_effect_lightprobe_allocator g_effect_lightprobe_allocator;

extern t_effect_data_allocator g_effect_data_allocator;
typedef t_restricted_allocation_manager<3, static_cast<void (*)(void *)>(0), static_cast<void (*)(void *)>(0),
    __tls_set_g_event_data_allocator> t_event_data_allocator;
extern t_event_data_allocator g_event_data_allocator;
typedef t_restricted_allocation_manager<3, static_cast<void (*)(void *)>(0), static_cast<void (*)(void *)>(0),
    &__tls_set_g_impact_data_allocator> t_effect_location_data_allocator;
extern t_effect_location_data_allocator g_effect_location_data_allocator;
typedef t_restricted_allocation_manager<3, static_cast<void (*)(void *)>(0), static_cast<void (*)(void *)>(0),
    __tls_set_g_contrail_location_allocator> t_effect_lightprobe_slot_data_allocator;
extern t_effect_lightprobe_slot_data_allocator g_effect_lightprobe_slot_data_allocator;
typedef t_restricted_allocation_manager<1, static_cast<void (*)(void *)>(0), static_cast<void (*)(void *)>(0),
    &__tls_set_g_player_control_globals_deterministic_allocator> t_effect_counts_allocator;
extern t_effect_counts_allocator g_effect_counts_allocator;
typedef t_restricted_allocation_manager<3, adopt_old_effect_render_message_queue, static_cast<void (*)(void *)>(0),
    __tls_set_g_effect_message_queue_allocator> t_effect_message_queue_allocator;
extern t_effect_message_queue_allocator g_effect_message_queue_allocator;

extern "C" {
#include "effects/effect_message_queue_tls.h"
#include "effects/effect_data.h"

extern __declspec(thread) s_data_array *g_light_volume_location_data;
#include "effects/effect_lightprobe_memory.h"
extern __declspec(thread) s_data_array *g_static_lighting_effect_attachments;
}

struct data_array;
struct s_cinematic_globals_definition;
struct s_effect_render_globals;
#include "game/game_globals_storage.h"
struct s_damage_owner;
struct s_geometry_sample;
#include "game/game_time_types.h"
struct s_controllers;
struct s_visibility_globals;
struct s_user_globals;
union real_vector3d;
struct fpw_first_person;

extern "C" {
#include "ai/command_script_tls.h"
extern __declspec(thread) s_effect_render_globals *g_effect_render_globals;
extern const float k_real_constant_pool[];
extern const float k_random_word_normalization;
extern __declspec(thread) fpw_first_person *g_first_person_weapon_data;
extern const float k_real_zero_storage;
extern __declspec(thread) s_controllers *g_director_globals;
extern s_visibility_globals g_visibility_globals;
}


#endif // EFFECTS_EFFECTS_GLOBALS_H
