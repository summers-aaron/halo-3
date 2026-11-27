#ifndef GAME_GAME_ENGINE_RUNTIME_EXTERNS_H
#define GAME_GAME_ENGINE_RUNTIME_EXTERNS_H
#include "game/player_control_globals.h"

#include "networking/network_globals.h"
#include "game/player_mapping_globals.h"
#include "game/game_engine_globals.h"
#include "camera/saved_film_director_storage.h"

#include "cseries/cseries_unicode.h"
#include "cache/restricted_memory_regions.h"
#include "game/game_engine.h"

typedef t_restricted_allocation_manager<3, static_cast<void (*)(void *)>(0), static_cast<void (*)(void *)>(0),
    &__tls_set_g_local_game_engine_globals_allocator> c_local_game_engine_globals_allocation_manager;
typedef t_restricted_allocation_manager<3, static_cast<void (*)(void *)>(0), static_cast<void (*)(void *)>(0),
    &__tls_set_g_game_engine_globals_allocator> c_game_engine_globals_allocation_manager;

template<>
void c_local_game_engine_globals_allocation_manager::deallocate(void *memory);

class c_game_variant;
class c_game_results;
class c_map_variant;
template<long size> class c_static_wchar_string;
struct c_static_wchar_string_1024;
typedef c_map_variant s_game_engine_map_info;
struct s_player_waypoint_timers;
typedef c_map_variant s_game_engine_map_variant_quotas;

extern "C" bool string_list_get_normal_string(long string_list_index, long string_index, c_static_wchar_string<256> *string);
extern "C" void text_group_get_string(long tag_index, long string_id, c_static_wchar_string_1024 *string);
extern "C" void game_results_grant_top_player_achievement(c_game_results *game_results);
extern "C" void game_engine_set_map_info_name(s_game_engine_map_info *info, const unsigned short *source);
extern "C" c_game_variant *game_engine_get_current_variant(void);
extern "C" void game_engine_update_waypoint_timers(s_player_waypoint_timers *waypoints);
extern "C" unsigned char game_is_scoreboard_blocked(int *out_reason);
extern "C" void simulation_action_object_latch_update(long object_index, long x);
extern "C" void game_engine_apply_map_variant_quotas(s_game_engine_map_info *info, const s_game_engine_map_variant_quotas *map_variant);

struct s_game_engine_globals;
struct s_user_globals;
struct s_life_cycle_globals;
#include "interface/hud_waypoint_globals.h"
struct s_local_game_engine_globals;
class c_window_manager;

// Imported storage used by game_engine.cpp; address tags remain in owning units.
extern "C" s_life_cycle_globals g_network_life_cycle_globals;
extern "C" long g_game_engine_display_user_flags;
extern "C" c_window_manager g_window_manager;
extern "C" __declspec(thread) s_local_game_engine_globals *g_local_game_engine_globals;

#endif // GAME_GAME_ENGINE_RUNTIME_EXTERNS_H
