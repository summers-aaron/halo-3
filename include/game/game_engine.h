#ifndef GAME_GAME_ENGINE_H
#define GAME_GAME_ENGINE_H

#include "game/game_types.h"

// Functions defined in tu/blamlibxbox/game_engine.cpp.

class c_game_statborg;
struct game_engine_interface_state;
union real_point3d;
struct s_multiplayer_weapon_tracker;
enum e_game_engine_end_condition;

void game_engine_update_player_sitting_out(void);
bool game_engine_running(void);
c_game_statborg *game_engine_get_statborg(void);
void game_engine_update_after_game(void);
bool game_engine_player_has_shield(long player_index);
void game_engine_build_allowable_team_designators(void);
long game_engine_get_player_score(long team_index, bool combined);
long game_engine_get_player_kills(long player_index);
long game_engine_get_score_to_win_round(void);
bool game_engine_player_is_playing(long player_index);
short get_ever_active_team_or_player_count(void);
long game_engine_get_player_place(long player_index);
bool game_engine_are_teams_tied(void);
bool game_engine_are_players_tied(void);
bool game_engine_in_round(void);
long game_engine_get_round(void);
long game_engine_choose_winner_index(void);
bool game_engine_adjust_player_score(long player_index, long adjustment);
void game_engine_end_round_with_winner(long winner, bool end_game, e_game_engine_end_condition end_condition);
bool game_engine_player_is_out_of_lives(long player_index);
void game_engine_multiplayer_weapon_reset(long weapon_index, const real_point3d *position);
s_multiplayer_weapon_tracker *game_engine_multiplayer_weapon_get_internal(long identifier);
const s_multiplayer_weapon_tracker *game_engine_multiplayer_weapon_get(short id);
void game_engine_multiplayer_weapons_delete_all(void);
void game_engine_garbage_collect();
long game_engine_compare_players(long player_index, long other_player_index, bool compare_statistics);
long game_engine_compare_teams(long first_team, long second_team, bool compare_statistics);
bool game_engine_weapon_is_bomb(long object_index);
bool game_engine_game_object_has_fallen_too_far(long object_index);
bool game_engine_player_has_infinite_ammo(long object_index);
long game_engine_get_player_state_index(long player_index);
void game_engine_update_console_scoreboard(void);
void game_engine_client_respond_to_new_state(long state);
void game_engine_game_starting(void);
bool game_engine_get_hud_interface_state(long user_index, game_engine_interface_state *state);
void game_engine_update(void);

struct c_static_wchar_string_1024;
struct s_game_engine_fade_entry;

extern "C" long game_engine_get_scores_player_score(long player_index, bool combined);
extern "C" void game_engine_player_set_respawn_timer(long player_index, long ticks);
extern "C" void game_engine_get_multiplayer_string(long string_index, c_static_wchar_string_1024 *string);
extern "C" void game_engine_game_ending(void);
extern "C" void game_engine_boot_player(long player_index);
extern "C" s_game_engine_fade_entry *game_engine_fade_set_state(s_game_engine_fade_entry *entry, signed char state);
extern "C" void game_engine_update_scoreboard(float param);
extern "C" void game_engine_spawn_variant_objects(void *map_variant_data);
extern "C" long game_engine_find_team_place(long team);

class c_game_variant;

extern "C" void process_game_engine_globals_messages(void);
extern "C" void game_engine_update_user_directors(long value);
extern "C" long game_engine_get_winning_team(void);
extern "C" long game_engine_get_winning_player(void);
extern "C" void game_engine_get_player_winning_status(long player_index, long *winning_status);
extern "C" bool game_engine_player_grants_infinite_ammo(long player_index);
void game_engine_reset_stats_for_round_switch(void);
extern "C" bool game_engine_has_grenades_enabled(void);
extern "C" bool game_engine_has_symmetric_rules(void);
extern "C" bool game_engine_has_asymmetric_rules(void);
void __tls_set_g_game_engine_globals_allocator(void *address);
extern "C" void game_statborg_start_stats(c_game_statborg *statborg);
extern "C" long game_engine_get_player_rating(long index);
extern "C" void game_engine_update_waypoint(s_game_engine_fade_entry *waypoint);
void data_mine_insert_multiplayer_game_options(const char *reason, const c_game_variant *variant);
void data_mine_insert_multiplayer_statistics(void);
void game_engine_update_after_game_update_state(void);
extern "C" void game_engine_server_respond_to_new_state(void);
void game_engine_update_score_and_standing(void);
void game_engine_update_player_states(void);
void update_player_waypoint_data(long player_index);
void game_engine_garbage_collect_for_round_switch(void);
void game_engine_update_global_fade_timers(float delta);
void game_engine_initialize_for_new_round(void);
long game_engine_get_team_rounds_won(e_game_team team);
void game_engine_adjust_team_score_for_composition(e_game_team team);
extern "C" long game_engine_get_winning_status(e_game_team team);


#include "cseries/cseries_types.h"

// Shared declarations used by game_engine.cpp.
template<long, void (*)(void *), void (*)(void *), void (*)(void *)>
class t_restricted_allocation_manager;
struct s_list_globals;
struct s_game_engine_score_cache;
void __tls_set_g_local_game_engine_globals_allocator(void *address);
extern "C" s_game_engine_score_cache g_game_engine_score_cache[2];
extern "C" s_list_globals g_game_engine_globals_message_queue;
extern t_restricted_allocation_manager<3,static_cast<void (*)(void *)>(0),static_cast<void (*)(void *)>(0),
    &__tls_set_g_local_game_engine_globals_allocator> g_local_game_engine_globals_allocator;
extern t_restricted_allocation_manager<3,static_cast<void (*)(void *)>(0),static_cast<void (*)(void *)>(0),
    &__tls_set_g_game_engine_globals_allocator> g_game_engine_globals_allocator;
void game_engine_end_game(e_game_engine_end_condition game_engine_end_condition);

extern "C" bool game_engine_player_is_awaiting_respawn(unsigned long player_index);


extern char k_boot_betrayer_screen_message_vtable[];

void game_engine_initialize(void);
void game_engine_dispose(void);
void game_engine_initialize_for_new_map(void);
void game_engine_dispose_from_old_map(void);


#endif // GAME_GAME_ENGINE_H
