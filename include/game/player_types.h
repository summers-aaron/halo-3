#ifndef GAME_PLAYER_TYPES_H
#define GAME_PLAYER_TYPES_H

#include <stddef.h>

#include "objects/object_reference_types.h"
#include "math/real_math_types.h"
#include "game/player_traits.h"
#include <string.h>

#include "game/player_configuration.h"

struct game_player_options {
    bool valid;
    bool left_game;
    short user_index;
    long controller_index;
    s_machine_identifier machine_identifier;
    s_player_identifier player_identifier;
    s_player_configuration configuration;
};

// Eight recent shots are retained for each player.
struct s_player_shot_info {
    union { unsigned short identifier; short signed_identifier; };
    unsigned char flags;
    long time;
};

struct s_player_input_pair { long a; long b; };

struct player_datum {
    short identifier;
    unsigned long flags;
    s_player_identifier player_identifier;
    long left_game_time;
    union {
        s_machine_identifier machine;
        s_machine_identifier machine_identifier;
    };
    short machine_index;
    short user_index;
    long controller_index;
    s_cluster_reference cluster_reference;
    long unit_index;
    long dead_unit_index;
    long next_unit_index;
    unsigned long suppressed_buttons;
    unsigned short previous_triggers;
    unsigned char teleport_counter;
    unsigned char illegal_position_ticks;
    signed char melee_recovery_ticks;
    union { signed char melee_remaining_ticks; signed char primary_fire_ticks; };
    union { signed char grenade_recovery_ticks; signed char secondary_fire_ticks; };
    real_point3d position;
    s_player_configuration configuration;
    s_player_configuration desired_configuration;
    long coop_respawn_delay_ticks;
    bool coop_respawn_pending;
    bool coop_respawn_initial;
    long respawn_timer;
    long respawn_penalty_ticks;
    long death_timer;
    long respawn_seconds;
    union { long airborne_ticks; long ticks_without_unit; };
    long grenade_recharge_ticks;
    long last_shot_at_object_index;
    long last_shot_at_time;
    short sprint_ticks;
    short sprint_recovery_ticks;
    short vehicle_ejection_ticks;
    long tracking_object_index;
    short tracking_model_index;
    unsigned char tracking_warning_ticks;
    unsigned char locking_warning_ticks;
    long tracking_ticks;
    short grace_ticks;
    unsigned char b2ae;
    signed char campaign_player_slot;
    union { long equipment_touch_time; long invisibility_time; };
    union { float equipment_touch_scale; float invisibility_duration; };
    long target_player_index;
    short target_absolute_player_index;
    signed char vehicle_entry_ticks;
    signed char unitless_countdown_ticks;
    signed char ticks_with_unit;
    bool previous_crouch_state;
    bool crouch_state;
    short target_confidence;
    union {
        short lives;
        unsigned short count;
    };
    long respawn_player_index;
    long last_death_round_ticks;
    c_player_traits traits;
    union {
        struct {
            long powerup_times[3];
            long respawn_target_player_index;
        };
        long powerup_and_respawn_times[4];
    };
    union { unsigned char input_active; bool look_override; };
    union {
        struct { s_player_input_pair input_target; s_player_input_pair input_extra; };
        struct { float editor_distance; float editor_rotation_z; float editor_rotation_x; float editor_rotation_y; };
    };
    union { unsigned short hill_traits_flag; short ticks_in_hill; };
    short ticks_outside_hill;
    unsigned short hill_counter;
    unsigned short : 16;
    unsigned char achievement_kill_counters[2];
    unsigned char achievement_damage_info[5];
    long last_vehicle_exit_index;
    long last_vehicle_exit_time;
    s_player_shot_info shots[8];
    unsigned short respawn_counter;
};

typedef char player_types_input_pair_size[(sizeof(s_player_input_pair) == 8) ? 1 : -1];
typedef char player_types_off_coop_respawn[(offsetof(player_datum, coop_respawn_delay_ticks) == 0x270) ? 1 : -1];
typedef char player_types_off_tracking[(offsetof(player_datum, tracking_object_index) == 0x2a0) ? 1 : -1];
typedef char player_types_editor_distance_offset[(offsetof(player_datum, editor_distance) == 0x300) ? 1 : -1];
typedef char player_types_editor_rotation_x_offset[(offsetof(player_datum, editor_rotation_x) == 0x308) ? 1 : -1];
typedef char player_types_editor_rotation_y_offset[(offsetof(player_datum, editor_rotation_y) == 0x30c) ? 1 : -1];
typedef char player_types_off_input[(offsetof(player_datum, input_target) == 0x300) ? 1 : -1];
typedef char player_types_off_input_end[(offsetof(player_datum, achievement_kill_counters) == 0x318) ? 1 : -1];
typedef char player_types_off_achievement_kill_counters[(offsetof(player_datum, achievement_kill_counters) == 0x318) ? 1 : -1];
typedef char player_types_size_id[(sizeof(s_player_identifier) == 8) ? 1 : -1];
typedef char player_types_size_machine[(sizeof(s_machine_identifier) == 6) ? 1 : -1];
typedef char player_types_size_emblem[(sizeof(s_emblem_info) == 8) ? 1 : -1];
typedef char player_types_size_appearance[(sizeof(s_player_appearance) == 30) ? 1 : -1];
typedef char player_types_size_client[(sizeof(s_player_configuration_from_client) == 0xc8) ? 1 : -1];
typedef char player_types_size_host[(sizeof(s_player_configuration_from_host) == 0x48) ? 1 : -1];
typedef char player_types_host_player_name_offset[(offsetof(s_player_configuration_from_host, player_name) == 0) ? 1 : -1];
typedef char player_types_size_config[(sizeof(s_player_configuration) == 0x110) ? 1 : -1];
typedef char player_types_size_options[(sizeof(game_player_options) == 0x128) ? 1 : -1];
typedef char player_types_respawn_target_offset[(offsetof(player_datum, respawn_target_player_index) == 0x2f8) ? 1 : -1];
typedef char player_types_respawn_counter_offset[(offsetof(player_datum, respawn_counter) == 0x368) ? 1 : -1];
typedef char player_types_size_datum[(sizeof(player_datum) == 0x370) ? 1 : -1];
typedef char player_types_size_shot[(sizeof(s_player_shot_info) == 8) ? 1 : -1];
typedef char player_types_off_respawn_penalty[(offsetof(player_datum, respawn_penalty_ticks) == 0x27c) ? 1 : -1];
typedef char player_types_off_campaign_slot[(offsetof(player_datum, campaign_player_slot) == 0x2af) ? 1 : -1];
typedef char player_types_off_equipment_touch_time[(offsetof(player_datum, equipment_touch_time) == 0x2b0) ? 1 : -1];
typedef char player_types_off_equipment_touch_scale[(offsetof(player_datum, equipment_touch_scale) == 0x2b4) ? 1 : -1];
typedef char player_types_off_airborne_ticks[(offsetof(player_datum, airborne_ticks) == 0x288) ? 1 : -1];
typedef char player_types_off_last_death_round_ticks[(offsetof(player_datum, last_death_round_ticks) == 0x2cc) ? 1 : -1];
typedef char player_types_off_achievement_damage_info[(offsetof(player_datum, achievement_damage_info) == 0x31a) ? 1 : -1];
typedef char player_types_off_vehicle_exit[(offsetof(player_datum, last_vehicle_exit_index) == 0x320) ? 1 : -1];
typedef char player_types_off_vehicle_exit_time[(offsetof(player_datum, last_vehicle_exit_time) == 0x324) ? 1 : -1];
typedef char player_types_off_shots[(offsetof(player_datum, shots) == 0x328) ? 1 : -1];
typedef char player_types_off_unit[(offsetof(player_datum, unit_index) == 0x28) ? 1 : -1];
typedef char player_types_off_grenade_recovery[(offsetof(player_datum, grenade_recovery_ticks) == 0x3e) ? 1 : -1];
typedef char player_types_off_position[(offsetof(player_datum, position) == 0x40) ? 1 : -1];
typedef char player_types_off_team[(offsetof(player_datum, configuration.host.team_index) == 0x138) ? 1 : -1];
typedef char player_types_off_desired[(offsetof(player_datum, desired_configuration) == 0x160) ? 1 : -1];
typedef char player_types_off_respawn[(offsetof(player_datum, respawn_timer) == 0x278) ? 1 : -1];
typedef char player_types_off_lives[(offsetof(player_datum, lives) == 0x2c6) ? 1 : -1];
typedef char player_types_off_traits[(offsetof(player_datum, traits) == 0x2d0) ? 1 : -1];
typedef char player_types_off_powerup[(offsetof(player_datum, powerup_times) == 0x2ec) ? 1 : -1];
typedef char player_types_off_client_global_statistics[(offsetof(s_player_configuration_from_client, global_statistics) == 0x70) ? 1 : -1];
typedef char player_types_off_client_bungienet_user[(offsetof(s_player_configuration_from_client, bungienet_user) == 0x54) ? 1 : -1];
typedef char player_types_off_client_experience_growth_banned[(offsetof(s_player_configuration_from_client, experience_growth_banned) == 0x6c) ? 1 : -1];
typedef char player_types_off_client_displayed_statistics[(offsetof(s_player_configuration_from_client, displayed_statistics) == 0x80) ? 1 : -1];
typedef char player_types_off_client_hopper_statistics[(offsetof(s_player_configuration_from_client, hopper_statistics) == 0xac) ? 1 : -1];
typedef char player_types_off_host_calculated_statistics[(offsetof(s_player_configuration_from_host, calculated_statistics) == 0x28) ? 1 : -1];

typedef char player_types_off_melee_remaining[(offsetof(player_datum, melee_remaining_ticks) == 0x3d) ? 1 : -1];
typedef char player_types_off_tracking_warning[(offsetof(player_datum, tracking_warning_ticks) == 0x2a6) ? 1 : -1];
typedef char player_types_off_locking_warning[(offsetof(player_datum, locking_warning_ticks) == 0x2a7) ? 1 : -1];

typedef char player_types_previous_crouch_offset[(offsetof(player_datum, previous_crouch_state) == 0x2c1) ? 1 : -1];
typedef char player_types_crouch_offset[(offsetof(player_datum, crouch_state) == 0x2c2) ? 1 : -1];

typedef char player_king_ticks_offset[(offsetof(player_datum, ticks_in_hill) == 0x310) ? 1 : -1];
typedef char player_king_outside_ticks_offset[(offsetof(player_datum, ticks_outside_hill) == 0x312) ? 1 : -1];
typedef char player_king_counter_offset[(offsetof(player_datum, hill_counter) == 0x314) ? 1 : -1];

#endif // GAME_PLAYER_TYPES_H
