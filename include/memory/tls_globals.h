#include "scenario/scenario_kill_trigger_volumes_state_types.h"
#ifndef INCLUDE_MEMORY_TLS_GLOBALS_H
#define INCLUDE_MEMORY_TLS_GLOBALS_H
#include "cutscene/cinematic_globals_types.h"
#include "ai/joint_data.h"
#include "ai/squad_data.h"

extern "C" {
extern __declspec(thread) struct s_render_texture_camera_globals *g_render_texture_camera_globals;
}

#include "memory/data.h"
#include "memory/tls_data_arrays.h"

#include "game/game_globals_storage.h"
struct s_game_engine_globals;
#include "ai/actor_globals.h"
struct s_cinematic_globals_definition;
struct s_user_globals;
struct s_controllers;
struct s_render_texture_camera_globals;

#include "game/game_time_types.h"

// Canonical thread-local storage slots; named slots use owner-unit address tags.
// Do not infer TLS slot ownership from setter names; identical setters share one implementation.
#include "game/game_engine_globals.h"
#include "game/player_mapping_globals.h"
extern "C" {
extern __declspec(thread) s_controllers *g_director_globals;          // director globals
extern __declspec(thread) unsigned long *g_deterministic_random_seed_ptr;
}

static __forceinline s_data_array *tls_actor_data() { return actor_data; }
static __forceinline s_data_array *tls_player_data() { return player_data; }
static __forceinline s_game_engine_globals *tls_game_engine_globals() { return game_engine_globals; }
static __forceinline s_data_array *tls_prop_ref_data() { return prop_ref_data; }
static __forceinline ai_globals_type *tls_actor_globals() { return ai_globals; }
static __forceinline game_globals_storage *tls_game_globals() { return game_globals; }
static __forceinline s_data_array *tls_squad_data() { return squad_data; }
static __forceinline s_data_array *tls_ai_prop_data() { return prop_data; }
static __forceinline s_data_array *tls_clump_data() { return clump_data; }
static __forceinline s_render_texture_camera_globals *tls_render_texture_camera_globals() { return g_render_texture_camera_globals; }
static __forceinline s_data_array *tls_object_header_data() { return object_header_data; }
static __forceinline s_data_array *tls_joint_state_data() { return g_joint_data; }
static __forceinline s_cinematic_globals_definition *tls_cinematic_globals() { return cinematic_globals; }
static __forceinline game_time_globals_definition *tls_game_time_globals() { return game_time_globals; }
static __forceinline s_user_globals *tls_player_mapping_globals() { return player_mapping_globals; }
static __forceinline s_data_array *tls_impact_data() { return g_impact_data; }
static __forceinline s_data_array *tls_swarm_data() { return g_swarm_data; }
static __forceinline s_data_array *tls_prop_data() { return tracking_data; }
static __forceinline s_data_array *tls_simulation_gamestate_entity_data() { return g_simulation_gamestate_entity_data; }
static __forceinline s_data_array *&tls_simulation_gamestate_entity_data_slot() { return g_simulation_gamestate_entity_data; }
static __forceinline s_data_array *tls_impact_array_data() { return g_impact_array_data; }
static __forceinline unsigned long *tls_random_seed() { return g_deterministic_random_seed_ptr; }

// Direct lvalue spellings for readers whose accepted scheduling depends on TLS expressions.
#define TLS_PROP_DATA tracking_data
#define TLS_GAME_TIME_GLOBALS game_time_globals
#define TLS_IMPACT_DATA g_impact_data
#define TLS_IMPACT_ARRAY_DATA g_impact_array_data

#define TLS_RANDOM_SEED g_deterministic_random_seed_ptr

#endif // INCLUDE_MEMORY_TLS_GLOBALS_H
