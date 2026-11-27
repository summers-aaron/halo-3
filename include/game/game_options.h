#ifndef GAME_GAME_OPTIONS_H
#define GAME_GAME_OPTIONS_H

#include <stddef.h>

#include "game/game_types.h"
#include "game/player_types.h"
#include "game/game_variant.h"
#include "game/map_variant.h"

// The armaments each campaign player carries into the next level.
struct s_persistent_campaign_weapon {
    short rounds;
    short loaded_rounds;
    short inventory_rounds;
    short age;
};

struct s_persistent_campaign_player {
    bool valid;
    s_persistent_campaign_weapon primary_weapon;
    s_persistent_campaign_weapon backpack_weapon;
    s_persistent_campaign_weapon secondary_weapon;
    unsigned char grenade_counts[4];
};

struct s_campaign_armaments {
    s_persistent_campaign_player players[4];
};

// The hopper a matchmade game was started from (life_cycle_handler_in_match builds it from the session's hopper
// parameters; the March signature begin_load_map(c_network_session *, const s_game_matchmaking_options *) names it).
struct s_game_matchmaking_options {
    unsigned short hopper_identifier;                       // (copied only for matchmade games)
    unsigned char xlast_index;                              // (in_game rejects >= 32: 1 << k_xlast_index_bits)
    bool is_ranked;                                         // (game_results: hopper_ranked)
    bool is_team_game;                                      // (game_results: hopper_team_based)
    wchar_t hopper_name[32];
    long trueskill_draw_probability;
    float trueskill_beta_performance_variation;
    float trueskill_tau_dynamics_factor;
    long team_count;                                        // (life_cycle_handler_in_game: rejects < 0)
    long player_count;                                      // (life_cycle_handler_in_game: rejects < 0)
};

// The machines taking part (encoded as one 864-bit block "machines" by game_options_encode/decode, copied whole by
// life_cycle_in_game_initialize_options_from_session; name from the Halo Online build).
struct game_machine_options {
    unsigned long valid_machine_mask;
    s_machine_identifier machines[16];
    bool local_machine_exists;
    s_machine_identifier local_machine;
};

struct game_options {
    long game_mode;
    union {
        signed char game_simulation;
        signed char simulation_type;
        signed char simulation;
    };
    signed char game_network_type;
    short game_tick_rate;
    union {
        unsigned char random_data[8];
        unsigned __int64 game_instance;
    };
    unsigned long random_seed;                              // (generate_random_seed returns unsigned long; in_game compares it unsigned)
    long language;
    long determinism_version;
    e_campaign_id campaign_id;
    e_map_id map_id;
    char scenario_path[260];
    short initial_zone_set_index;
    bool load_level_only;
    signed char dump_machine_index;
    bool dump_object_log;
    bool dump_random_seeds;
    union {
        bool playtest_mode;
        bool survival_enabled;
    };
    short game_playback;
    bool record_saved_film;
    long playback_start_in_ticks;
    long playback_length_in_ticks;
    union {
        short campaign_difficulty;
        short campaign_difficulty_level;
    };
    short campaign_insertion_point;
    short campaign_metagame_scoring;
    bool campaign_metagame_enabled;
    bool survival_mode_enabled;
    union {
        long primary_skulls;
        unsigned long active_skulls;
        unsigned long campaign_skulls;
    };
    long secondary_skulls;
    s_campaign_armaments campaign_armaments;
    bool matchmade_game;
    char alignment_1c5[7];
    s_game_matchmaking_options matchmaking_options;
    c_game_variant game_variant;
    c_map_variant map_variant;
    union {
        game_machine_options machine_options;
        struct {                                            // the flat names units not yet switched use
            long machine_valid_mask;
            s_machine_identifier machines[16];
            bool local_machine_exists;
            s_machine_identifier local_machine;
        };
    };
    game_player_options players[16];
};

typedef game_options s_game_options;

typedef char game_options_off_matchmaking[
    (offsetof(game_options, matchmaking_options) == 0x1cc) ? 1 : -1];
typedef char game_options_off_playtest_mode[
    (offsetof(game_options, playtest_mode) == 0x12e) ? 1 : -1];

typedef char game_options_size_armaments[(sizeof(s_campaign_armaments) == 120) ? 1 : -1];
typedef char game_options_size_matchmaking[(sizeof(s_game_matchmaking_options) == 92) ? 1 : -1];
typedef char game_machine_options_size[(sizeof(game_machine_options) == 0x6c) ? 1 : -1];
typedef char game_options_size_check[(sizeof(game_options) == 0xf810) ? 1 : -1];
typedef char game_options_off_game_variant[(offsetof(game_options, game_variant) == 0x228) ? 1 : -1];
typedef char game_options_off_map_variant[(offsetof(game_options, map_variant) == 0x490) ? 1 : -1];
typedef char game_options_off_machines[(offsetof(game_options, machine_valid_mask) == 0xe520) ? 1 : -1];
typedef char game_options_off_players[(offsetof(game_options, players) == 0xe590) ? 1 : -1];

// Functions defined in tu/blamlibxbox/game_options.cpp that other units call.

struct s_game_engine_base_variant_metadata_view;
struct s_saved_game_item_metadata_packed;

extern "C" int game_options_find_player(const void *identifier, const void *address);
extern "C" bool game_variant_decode(c_game_variant *variant, c_bitstream *packet);
extern "C" void game_variant_set_from(c_game_variant *variant, const c_game_variant *source);
extern "C" void game_engine_variant_set_metadata(void *variant, const s_saved_game_item_metadata_packed *metadata);

#endif // GAME_GAME_OPTIONS_H
