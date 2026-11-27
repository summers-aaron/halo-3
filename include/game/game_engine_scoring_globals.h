#ifndef GAME_GAME_ENGINE_SCORING_GLOBALS_H
#define GAME_GAME_ENGINE_SCORING_GLOBALS_H

#include <stddef.h>
#include "game/game_engine_area_set_types.h"
#include "game/game_engine_candy_monitor_types.h"
#include "memory/datum_index.h"
#include "math/real_math_types.h"
#include "memory/static_arrays.h"
#include "game/map_variant.h"
#include "game/game_engine_sandbox_globals.h"
#include "game/game_types.h"
#include "game/statborg_entry_types.h"
#include "items/multiplayer_weapon_tracker.h"
#include "game/game_engine_event_data.h"
#include "simulation/game_interface/simulation_game_action.h"
#include "simulation/simulation_connection_statistics_types.h"
#include "simulation/simulation_player_netdebug_data_types.h"


struct s_game_engine_player_waypoint
{
    bool active;
    real_point3d position;
    unsigned short ticks;
    unsigned short initial_ticks;
    unsigned char state;
    unsigned char timer;
    unsigned char queued_state;
};

typedef char game_engine_waypoint_size[(sizeof(s_game_engine_player_waypoint) == 0x18) ? 1 : -1];


typedef c_map_variant s_game_engine_map_info;

struct s_game_engine_fade_entry
{
    byte active;
    real_point3d position;
    word countdown_active;
    word countdown;
    signed char state;
    union { unsigned char timer; signed char action_ticks; };
    unsigned char queued_state;
};


struct s_player_statborg_statistics
{
    short values[26];
};

struct s_statborg_player {
 short pending;
 short total;
 short stats[9];
 short round_kills;
 short round_deaths;
 long round_time;
 long round_score;
 short round_assists;
 short round_betrayals;
 short round_suicides;
 short round_sprees;
 short last_kill;
 signed char round_locked;
 long round_damage;
};
struct s_statborg_team {
 short pending;
 short total;
 short stats[9];
 signed char round_locked;
};
struct s_statborg_kill_stat {
 short values[11];
 short killing_spree;
 short multikill_count;
 long last_kill_time;
 long team_kills;
 short weapon_streak[4];
 short last_team;
 long last_damage;
};

class c_game_statborg
{
public:
 void build_statborg_update(unsigned long *mask,long size,void *out);
 bool apply_statborg_update(unsigned long mask,long size,const void *data);
 void record_kill(long object_index,long killer_index,e_game_team team,unsigned char damage_info,float window,bool flag);
private:
 void record_player_death(long player_index, long killer_player_index, bool suicide, bool betrayal, unsigned char damage_reporting_info);
 void record_player_kill(long player_index, long victim_index, long time_window, unsigned char damage_reporting_info);

public:
    void stats_reset_for_round_switch();
    void stats_promote_to_authority();
    __forceinline void reset_player_stat(long player, long stat)
    {
        long absolute = datum_index_to_absolute_index(player);
        m_player_stats[absolute][stat] = 0;
        simulation_action_game_statborg_update(1 << absolute);
    }
    void adjust_player_stat(long player_index, e_statborg_entry statistic, short adjustment, long game_results_statistic, bool adjust_team);
    short get_player_stat(long player_index, long statistic) const { return m_player_stats[datum_index_to_absolute_index(player_index)][statistic]; }
    short get_team_stat(long team_index, long statistic) const { return m_team_stats[team_index][statistic]; }

    union
    {
        short m_player_stats[16][26];
        s_player_statborg_statistics player_stats[16];
        s_statborg_player players[16];
        s_statborg_kill_stat stats[16];
    };
 __forceinline void set_player_stat(long index, e_statborg_entry statistic, short value)
 {
     unsigned long absolute_index = datum_index_to_absolute_index(index);
     m_player_stats[absolute_index][statistic] = value;
 }
 __forceinline void set_team_stat(long team,e_statborg_entry statistic,short value) {m_team_stats[team][statistic]=value;}

    __forceinline void player_indices_swapped(long player_index1, long player_index2);
    __forceinline void player_deleted(long player_index) {}
    __forceinline void player_added(long player_index);
    union
    {
        short m_team_stats[8][12];         /* 24-byte rows */
        struct
        {
            union { short m_team_rows[8][12]; s_statborg_team teams[8]; };       /* teams[8] */
            bool m_stats_started;
            union { unsigned long m_update_mask; unsigned long update_mask; };
            union { bool m_finalized; unsigned char flag; };
        };
    };
    public: void adjust_team_stat(enum e_game_team team, enum e_statborg_entry statistic, short adjustment, long game_results_statistic);
};

struct s_game_engine_queued_event
{
    s_game_engine_queued_event() { flags.clear(); }
    long time;
    c_flags_no_init<long, unsigned char, 8> flags;
    s_game_engine_event_data event;
};

typedef char game_statborg_size[(sizeof(c_game_statborg) == 0x40c) ? 1 : -1];
typedef char game_statborg_finalized_offset[(offsetof(c_game_statborg, m_finalized) == 0x408) ? 1 : -1];

struct s_dead_player_info
{
    long player_index;
    long time;
    real_point3d position;
};

struct s_territories_globals;
struct s_juggernaut_spawn_state;
struct s_vip_globals;
struct s_king_globals;

struct s_oddball_globals
{
    c_static_array<long, 5> ball_object_indices;
    c_static_array<real_point3d, 5> ball_positions;
    c_static_array<short, 5> ball_timers;
    c_static_array<long, 5> carrier_player_indices;
    c_static_array<signed char, 5> score_ticks;
    c_static_array<char, 5> ball_states;
    c_static_array<long, 5> ball_drop_ticks;
};

struct s_game_engine_globals                /* tls game_engine_globals */
{
    __forceinline s_juggernaut_spawn_state &juggernaut();
    __forceinline s_vip_globals &vip();
    __forceinline s_king_globals &king();
    __forceinline s_territories_globals &territories();
    __forceinline c_area_set<c_area, 3> *assault_team_areas();

    long  flags;                            /* bit0: scoring leaders dirty; bit3: team activity valid */
    word allowable_team_designators;
    word initial_team_mask;
    word designator_mask; /* set to -1 when the game has teams (0x205fc8) */
    word valid_team_mask; /* set to -1 when the game has teams (0x205fc8) */
    word active_teams;
    word ever_active_teams;
    union { short team_mapping[9]; short shorts_10[9]; }; /* set to -1 for a new map (0x205fc8) */
    union { char team_lives[8]; char team_lives_22[8]; c_static_array<char, 8> shared_team_lives; }; /* per-team lives for a new map */
    union { long long_2c; long gamestate_index; };                           /* set to -1 for a new map (0x205fc8) */
    union { long long_30; long statborg_gamestate_index; };                           /* set to -1 for a new map (0x205fc8) */
    union { long longs_34[16]; long player_gamestate_indices[16]; };                      /* set to -1 for a new map (0x205fc8) */
    c_map_variant map_info;
    short game_engine_state;                       /* (== 1 in game_engine_in_round) */
    short round_index;
    long round_start_time;
    char leaders[124];                      /* stored score list for game_engine_scoring_update_leaders */
    union
    {
        struct { s_sandbox_globals sandbox; };
        s_oddball_globals oddball;
        // The remaining variant accessors use the shared storage.
        char variant_globals[0xf98c - 0xe18c];
        struct
        {
            char ctf_state_prefix[0xf834 - 0xe18c]; // goal_areas + flag_spawn_areas + head scalars
            unsigned char flag_state[9];
        };
        struct
        {
            c_static_array<c_area_set<c_area, 3>, 9> assault_goal_areas;
            c_static_array<c_area_set<c_area, 3>, 9> assault_bomb_spawn_areas;
            unsigned char assault_bomb_exploded;
            long defensive_team;
            long sudden_death_timer;
            long ending_timer;
            unsigned short assault_helper_flags;
            unsigned short assault_variant_flags;
            long bomb_arming_timers[9];
            short bomb_reset_timers[9];
            unsigned char bomb_weapon_flags[9];
            long player_that_placed_bombs[9];
            short timeout_return_second_counter[9];
        };
    };
    short timer_seconds;
    float user_fade[4];
    unsigned char pending_flags;
    byte fade_padding;
    short end_round_timer;
    union { short short_f9a4; unsigned short next_shot_identifier; };
    s_dead_player_info dead_players[64];
    c_game_statborg statborg;
    union
    {
        s_game_engine_fade_entry player_fades[16];
        s_game_engine_player_waypoint player_waypoints[16];
    };
    long player_update_time;                /* game_engine_update broadcasts player updates every 3 s */
    union
    {
        s_simulation_connection_statistics connection_statistics[16];
        s_simulation_player_netdebug_data player_netdebug_data[16];
    };
    c_multiplayer_candy_monitor_manager candy_monitor_manager;

    __forceinline c_multiplayer_candy_monitor_manager *get_candy_monitor_manager()
    {
        return &candy_monitor_manager;
    }
    long state_change_timer;
    long desired_game_engine_state;                      /* (== 1 in game_engine_in_round) */
    byte game_finished;
    long garbage_collection_speed;
    unsigned long : 32;
    long game_engine_index;
    union { long multiplayer_weapon_count; long game_object_count; };
    s_multiplayer_weapon_tracker multiplayer_weapons[8];
    char teleporter_slot_table[0xe10];                /* teleporter slot table for 0x205fc8 */
    long predicted_event;
    s_game_engine_queued_event queued_events[64];
    long last_respawn_time;
    long respawn_count;
};

typedef char game_engine_candy_manager_offset[(offsetof(s_game_engine_globals, candy_monitor_manager) == 0x10538) ? 1 : -1];

typedef char game_engine_predicted_event_offset[(offsetof(s_game_engine_globals, predicted_event) == 0x14be4) ? 1 : -1];
typedef char game_engine_gamestate_offset[(offsetof(s_game_engine_globals, gamestate_index) == 0x2c) ? 1 : -1];
typedef char game_engine_desired_state_offset[(offsetof(s_game_engine_globals, desired_game_engine_state) == 0x13d3c) ? 1 : -1];
typedef char game_engine_queued_events_offset[(offsetof(s_game_engine_globals, queued_events) == 0x14be8) ? 1 : -1];
typedef char game_engine_queued_event_size[(sizeof(s_game_engine_queued_event) == 0x30) ? 1 : -1];

typedef char game_engine_netdebug_offset[(offsetof(s_game_engine_globals, player_netdebug_data) == 0x10438) ? 1 : -1];
typedef char game_engine_waypoints_offset[(offsetof(s_game_engine_globals, player_waypoints) == 0x102b4) ? 1 : -1];

typedef char game_engine_objects_offset[(offsetof(s_game_engine_globals, multiplayer_weapons) == 0x13d54) ? 1 : -1];
typedef char game_engine_sandbox_offset[(offsetof(s_game_engine_globals, sandbox) == 0xe18c) ? 1 : -1];
typedef char game_engine_variant_globals_offset[(offsetof(s_game_engine_globals, variant_globals) == 0xe18c) ? 1 : -1];
typedef char game_engine_oddball_offset[(offsetof(s_game_engine_globals, oddball) == 0xe18c) ? 1 : -1];
typedef char game_engine_index_offset[(offsetof(s_game_engine_globals, game_engine_index) == 0x13d4c) ? 1 : -1];

typedef char game_engine_editor_mode_offset[
    ((offsetof(s_game_engine_globals, map_info) + offsetof(c_map_variant, m_game_engine_subtype)) == 0x194) ? 1 : -1];

typedef char game_engine_globals_size[(sizeof(s_game_engine_globals) == 0x157f0) ? 1 : -1];
typedef char game_engine_connection_statistics_offset[(offsetof(s_game_engine_globals, connection_statistics) == 0x10438) ? 1 : -1];


typedef char game_engine_dead_players_offset[(offsetof(s_game_engine_globals, dead_players) == 0xf9a8) ? 1 : -1];
typedef char game_engine_flag_state_offset[(offsetof(s_game_engine_globals, flag_state) == 0xf834) ? 1 : -1];
typedef char game_engine_simulation_indices_offset[
    ((offsetof(s_game_engine_globals, map_info) + offsetof(c_map_variant, m_simulation_indices)) == 0xdfc4) ? 1 : -1];

#endif
