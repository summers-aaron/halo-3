#ifndef GAME_GAME_RESULTS_H
#define GAME_GAME_RESULTS_H

#include "game/game_types.h"
#include <stddef.h>

#include "game/game_results_types.h"
#include "game/game_results_damage_breakdown_types.h"
#include "game/game_results_damage_statistic_types.h"
#include "game/game_results_damage_type_description_types.h"
#include "game/game_results_player_vs_player_statistic_types.h"

// Functions defined in tu/blamlibxbox/game_results.cpp.

struct s_game_results_event;
struct s_game_results_incremental;
struct s_game_results_incremental_update;
enum e_game_results_event_carry_type;

void game_results_statistic_increment(long player_absolute_index, e_game_team team_index, e_game_results_statistic statistic, long amount);
void game_results_statistic_set(long player_absolute_index, e_game_team team_index, e_game_results_statistic statistic, long value);

void game_results_notify_player_indices_changed();
void game_results_notify_active_teams_changed();
void game_results_set_final_player_score_and_standing(long player_absolute_index, long score, long standing);
void game_results_set_final_team_score_and_standing(e_game_team team_index, long score, long standing);
void game_results_insert_event(const s_game_results_event *event);
void game_results_insert_carry_event(long player_index, e_game_results_event_carry_type carry_type, long carry_value);
void game_results_insert_kill_event(long killing_player_index, long dead_player_index, unsigned char damage_reporting_info);
long game_results_get_maximum_rounds_completed(void);
void game_results_set_incremental(struct s_game_results_incremental const * incremental);
void game_results_initialize_for_new_map();
void game_results_apply_incremental_update(s_game_results_incremental *results, const s_game_results_incremental_update *update);
bool game_results_calculate_incremental_update(const s_game_results_incremental *previous_state, s_game_results_incremental_update *incremental_update);
void game_results_update();

class c_bitstream;

extern "C" int game_results_sort_players_by_standing(const void *a, const void *b);
extern "C" int game_results_sort_players_by_statistics(const void *a, const void *b);
extern "C" void game_results_medal_increment(long player_index);
extern "C" bool game_results_decode_packet(c_bitstream *packet, s_game_results_incremental_update *results);

struct s_game_results_player_data;

union real_point3d;

extern "C" unsigned char game_results_get_player_position(long player_index, real_point3d *position);
extern "C" int game_results_compare_team_score_share(const s_game_results_player_data *a, const s_game_results_player_data *b);
extern "C" void game_results_update_network_statistics(void);

class c_game_results;
extern "C" const c_game_results *g_game_results_sort_results;

void game_results_player_vs_player_statistic_increment(long player_absolute_index, long opponent_absolute_index,
    e_game_results_player_vs_player_statistic statistic, long amount);
long game_results_count_damage_statistic(long player_absolute_index, e_game_results_damage_statistic statistic, e_game_results_damage_breakdown_type category);

// Data defined in tu/blamlibxbox/game_results.cpp that other units use.
struct g_game_results_globals
{
    bool active;
    bool finished;
    bool updating;
    long event_index;
    long last_carry_event_time;
};

typedef char game_results_globals_size_must_be_12[(sizeof(struct g_game_results_globals) == 12) ? 1 : -1];
typedef char game_results_globals_updating_offset[(offsetof(struct g_game_results_globals, updating) == 2) ? 1 : -1];
typedef char game_results_globals_event_index_offset_must_be_4[(offsetof(struct g_game_results_globals, event_index) == 4) ? 1 : -1];
typedef char game_results_globals_last_carry_event_time_offset[(offsetof(struct g_game_results_globals, last_carry_event_time) == 8) ? 1 : -1];
extern struct g_game_results_globals g_game_results_globals;

extern const e_game_results_damage_type_description g_game_results_damage_type_descriptions[];
extern c_game_results g_current_game_results;

extern "C" void game_results_damage_statistic_increment(long player_index, long statistic, long damage_type, long amount);

#endif // GAME_GAME_RESULTS_H
