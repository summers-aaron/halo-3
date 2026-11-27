#ifndef AI_ACTOR_DATUM_H
#define AI_ACTOR_DATUM_H

#include <stddef.h>

#include "math/real_math_types.h"
#include "objects/object_reference_types.h"
#include "ai/sector_ref.h"
#include "ai/ai_point3d.h"
#include "ai/ai_direction.h"
#include "ai/ai_actions.h"
#include "ai/actor_moving.h"
#include "ai/recent_obstacle.h"
#include "ai/interaction_target.h"
#include "ai/joint_behavior_types.h"
#include "ai/hint_ref.h"
#include "ai/firing_position_ref.h"
#include "ai/path_types.h"

// Six-byte remembered firing-position entry; actor_flush_position_indices visits four entries.
struct actor_firing_position_entry {
    union { short ticks; struct { bool temporary; char unused; }; };
    firing_position_ref reference;
};

// match: 0x502798: retain actors.cpp's existing aggregate-return unit-index boundary.
struct s_actors_unit_index { long value; };

struct actor_persistent_movement {
    union {
        real_plane3d plane;
        struct { real_vector3d direction; float offset; };
    };
    union { float tolerance; float speed; };
    short ticks;
};

struct actor_dialogue_entry {
    long index;
    long time;
};

struct s_stimulus {
    long type;
    short time;
    short priority;
};

// Sampled body pose of an actor's unit (actor_input_sample_position fills it; actor_datum + 0x34c).
struct actor_position_data {
    real_point3d head;                            // unit_get_head_position
    real_point3d body;                            // object_get_origin
    real_vector3d forward;
    s_location location;
    real_vector3d velocity;
};

// The danger the actor is currently reacting to (actor_datum + 0x4c4), filled by the assess_*_danger functions.
struct actor_danger_zone {
    short danger_type;
    short state;
    bool evade;
    bool active;
    char pad6;
    bool forced;
    long object_index;
    long parent_index;
    long pref_index;                              // prop ref index
    float radius;
    real_point3d position;
    real_vector3d velocity;
    real_point3d projected_position;
    float distance;
    float midpoint_distance_squared;
    float extent;
    real_point3d midpoint;
    short ticks;
};

// One 28-byte step of an actor's path (actor_path + 0x40).
struct actor_path_point {
    short frame_index;                            // (-1 when absolute)
    short flags;
    c_sector_ref sector;
    c_hint_ref hint;
    c_ai_point3d point;
};

// The path the actor is following (actor_datum + 0x72c).
struct actor_path {
    bool valid;
    bool final_point;                             // (path_result: reaches the requested destination)
    bool failed;
    c_ai_point3d start_point;
    c_ai_point3d destination;
    real_vector3d destination_vector;             // (c_path_destination::vector)
    c_sector_ref destination_sector;
    float destination_distance;                   // (c_path_destination distance)
    long obstacle_object_index;
    union { short result_value; c_flags_no_init<long, short, 8> obstacle_flags; };
    signed char point_count;
    signed char point_index;
    actor_path_point points[4];
};

// One candidate turn of a ground vehicle (generate_turns / rate_turn / test_turn), and the turn a directional ground
// vehicle is following (actor_datum + 0x85c).
struct turn_info {
    bool left;                                    // (debug_warthog_turn_info prints LEFT/RIGHT)
    bool reverse;                                 // (debug_warthog_turn_info prints (REV))
    float angle;
    float rating;
    real_vector2d direction;
    float facing_angle;
    short index;
    float throttle;
    float previous_throttle;
    float previous_steering;
    float steering_scale;
    float steering;
    bool valid;
};

// Steering history of a nondirectional ground vehicle or a giant (actor_datum + 0x85c).
struct ground_steering_state {
    float previous_steering;
    float previous_throttle;
    float steering;
    float throttle;
    short direct;                                 // 1 when driving straight along the path, 0 when moving freely
};

struct actor_morph_data {
    long last_morph_time;
    short morph_delay_ticks;
    short distance_damage_ticks;
    unsigned short distance_damage_reset_ticks;
    short current_form;
    signed char morph_locked;
    bool immediate_morph;
    bool force_morph;
    short pending_form;
    short pending_form_priority;
    short turtle_timer;
};

struct actor_situation {
    unsigned char prop_count;
    short best_prop_class;
    long best_prop_index;
    bool close_enemy;
};

typedef char actor_morph_data_size[(sizeof(actor_morph_data) == 0x18) ? 1 : -1];
typedef char actor_situation_size[(sizeof(actor_situation) == 0xc) ? 1 : -1];
typedef char actor_situation_close_enemy_offset[(offsetof(actor_situation, close_enemy) == 8) ? 1 : -1];

// Actor state stored in the actor_data pool.
struct actor_datum {
    short identifier;                             // datum salt
    struct {                                      // March actor->meta
        short type;
        bool update_valid;
        bool swarm;
        signed char actively_controlled;
        bool active;
        bool squadless;
        bool force_active;
        union { bool disposable; unsigned char spawn_flags; };
        bool deleted;
        bool perception_timeslice;
        float weight;
        union { long unit_index; s_actors_unit_index unit_index_value; };
        long swarm_index;
        long next_squad_actor_index;
        short team;
        bool seeking_squad;
        long original_squad_index;
        short squad_seek_ticks;
        short squad_seek_attempts;
        short outside_bsp_ticks;                      // (actor_general_update)
        short equipment_condition_flags;              // (actor_new drop conditions)
        long squad_index;
        long starting_location_data;
        short cell_index;
        short starting_location_index;
        signed char uncovering;
        bool tracking_timeslice;
        short pathfinding_ticks_a;
        short pathfinding_ticks_b;
        long creation_time;
        long activation_ticks;
        long deactivation_ticks;
        long attach_time;                             // squadless_attach_actor stores game time
        long erase_time;
        short highest_status_refresh_ticks;
        short highest_state_refresh_ticks;
        union { long character_index; long character_definition_index; };
        long equipment_index;                         // (actor_new selects equipment)
        long first_prop_ref_index;
        float track_threshold;
        long clump_index;
        long next_clump_actor_index;
    };
    char behavior_state_data[0x200];
    short state_offsets[8];
    short layer_count;
    invitation_data invitations[4];
    long child_test_times[14];                    // behavior cooldown timestamps
    short behavior_mode;
    union { short combat_status; short weapon_state_ticks; };
    long combat_status_time;
    s_stimulus stimuli[5];
    unsigned long stimulus_flags;
    bool search_allowed;
    bool search_flag;
    unsigned char search_active;
    bool blind;
    unsigned char deaf;
    union { bool use_object; bool combat_flag339; };
    bool deployed_turret;
    bool combat_active;                           // actor_combat_status_update latches combat status >= 7
    bool weapon_lowered;
    union { bool weapon_up; bool lower_weapon_blocked_a; };                               // ai_scripting_set_weapon_up (March stb 818)
    bool avoidance_disabled;
    bool searching;
    union { bool suppress_combat; bool uses_clump; };
    bool broken;
    bool berserking;
    union {
        signed char cover_flags;
        // covered is the low two bits (actor_set_covered: cover_flags |= 1, &= ~2; retail 0x825308e0 rlwimi r6,r7,0,30,31);
        // the console allocates bit-fields from the most significant bit, x86 from the least.
        struct {
#if defined(_XBOX) && !defined(__clang__)
            signed char rest : 6;
            signed char covered : 2;
#else
            signed char covered : 2;
            signed char rest : 6;
#endif
        };
    };
    bool pending_berserking;
    bool weapon_stowed;
    signed char danger_ticks;
    short berserk_ticks;
    short cover_index;
    actor_position_data position;
    union { bool airborne; bool combat_flag380; };
    union { bool underwater; bool combat_flag381; };
    bool variable_up;
    bool driver;
    union { bool passenger; bool combat_flag384; };
    union { bool controlling_other_unit; bool gun_uses_parent; };
    bool vehicle_style_disabled;
    bool friendly_player_in_vehicle;
    union { long root_vehicle_index; long vehicle_index; };
    short vehicle_driver_type;
    unsigned short vehicle_state;
    union { long controlled_unit_index; long gun_unit_index; };
    bool movement_3d;
    bool sector_valid;
    c_ai_point3d pathfinding_point;
    c_sector_ref pathfinding_sector;
    union { real_vector3d facing_vector; real_vector3d combat_direction; };
    real_vector3d aiming_vector;
    real_vector3d looking_vector;
    real_vector3d left_vector;
    real_vector3d up_vector;
    float inherent_scariness;
    float body_vitality;
    float shield_vitality;
    float body_damage;
    float shield_damage;
    bool player_weight_valid;                     // ai_dialogue caches dialogue_player_weighting here; actors clears it every update
    float player_weight;                          // ai_dialogue: the cached weight
    c_recent_obstacle recent_obstacles[3];
    c_interaction_target interaction_target;
    long peer_death_time;
    short ticks_since_combat;
    long last_damage_dialogue_time;
    short dialogue_type;
    actor_dialogue_entry dialogue_entries[4];
    long dialogue_object_index;                    // actor_handle_deleted_object_directions clears this pair
    float dialogue_object_distance;
    long postcombat_time;
    c_sector_ref dialogue_sector;
    c_ai_point3d dialogue_point;
    bool pathfinding_location_invalid;            // actor_find_pathfinding_location: no valid sector (else it copies it above)
    short pending_vocalization;                   // actors name; ai_dialogue stores the speech item's vocalization
    short vocalization_delay_ticks;               // actors name; ai_dialogue stores a random delay in ticks
    signed char vocalization_pitch_range;         // unit_dialogue_sound_permutation.pitch_range (actors passes &0x47a to the vocalize call)
    signed char vocalization_permutation;         // unit_dialogue_sound_permutation.permutation
    bool had_shield;                              // (actor_input_update observes shield loss)
    short throttle_ticks;
    actor_morph_data morph;
    actor_situation situation;
    union { short target_flags; unsigned short targeting_flags; };
    short prefer_target_team;                     // ai_scripting_prefer_target_team
    long scripted_target_index;                   // an ai_index set by script (hs_function 0x2c5da0); perception matches prop owners against it
    union { long target_prop_index; long combat_target_prop_index; };
    union { long target_marker_name; long targeting_marker; };
    union { unsigned long handle; long target_actor_index; };
    long target_object_index;
    long target_age;                              // ticks: perception counts it up, reset on a new target
    union {
        unsigned short encounter_index;           // perception compares the unsigned identifier
        short target_ticks;                        // No target uses the signed -1 sentinel.
    };
    // perception: salience of an unowned prop when encounter_index is set; actors clears it
    union { bool unowned_target_salient; unsigned char unowned_target_salient_raw; };
    actor_danger_zone danger_zone;
    short surprise_level;
    long surprise_prop_index;
    bool stunned;
    float stun_scale;
    real_vector3d stun_direction;
    long cover_friend_actor_index;
    unsigned short cover_friend_behavior;
    short cover_friend_timer;
    long danger_zone_tracking_start_time;         // (March 0x534)
    long danger_zone_tracking_last_time;          // (March 0x538)
    short activity_index;                         // March assert actor->activity.activity_index
    short activity_variant;
    short activity_parameter;
    bool activity_flag;
    long attached_vehicle_index;
    long destination_unit_index;
    float reserved_distance;                      // (ai_vehicles reservation score)
    short vehicle_entry_flags;                    // seat reservation/entry bits (actors, ai_vehicles, behavior)
    union { unsigned short destination_seat_index; short vehicle_seat_index; };
    short vehicle_desire;                         // Vehicle entry preference.
    short attach_timer;
    short reserved_timer;
    short exit_timer;
    short reserved_ticks;
    long exited_vehicle_index;
    short exited_seat_index;
    short detach_timer;
    short seat_reentry_timer;                     // ai_vehicles name; actor_stimulus sets 15 s on vehicle eviction
    short seat_attach_timer;                      // (actor_handle_seat_attach 10-second delay)
    long turtle_time;
    unsigned char player_visible;
    unsigned char player_visible_last;
    short charge_timer;
    short player_visibility_ticks;
    unsigned char boarding_dialogue_active;       // ai_player_update_boarding_dialogue
    bool player_seen;                             // actor_perception_player_state_refresh latches a close player
    short boarding_dialogue_ticks;
    short boarding_dialogue_player_index;
    float nearest_object_distance;
    union { unsigned short pickup_player; short pickup_player_signed; };
    short pickup_wait_ticks;
    short pickup_state;                           // (ai_vehicles pickup state machine)
    float activity_scariness;
    float danger;
    bool pathfinding_type;
    long charge_deactivate_time;
    struct {                                      // March actor->firing_positions
        bool firing_position_locked;
        bool flying;
        bool goal_area_valid;
        short goal_status;
        long dynamic_firing_set_index;
        long support_object_index;
        short support_index;
        short last_discarded_firing_position;
        actor_firing_position_entry firing_positions[4];
        union { int firing_position; firing_position_ref firing_position_reference; };
        c_ai_point3d current_firing_position_point;
        bool discarded_position_valid;
    };
    short look_mode;
    c_ai_direction look_direction;
    union { short aim_mode; short combat_look_mode; };
    c_ai_direction aim_direction;
    bool lower_weapon_forced;
    struct {                                      // control state from flee_control on (4-byte aligned)
        bool flee_control;
        bool flee_movement;                           // behavior_flee_control; actor_get_movement_type selects 145
        bool force_movement_71;                       // actor_get_movement_type's higher-priority movement override
        bool path_aim;
        bool blind_panic_control_active;
        bool speed_override_valid;                   // Enables the movement speed override.
        long control_type;
        union { real_point3d control_point; real_vector3d control_vector; };
        float speed_override;                       // Requested movement speed.
        bool leap_requested;
        bool leap_active;
        bool jump_boosted;
        bool leap_target_valid;
        union { real_point2d leap_target; real_vector2d leap_direction; };
        float leap_speed;
        float leap_height;
        long leap_prop_index;
        float urgency_override;
        bool ignore_obstacles;                        // cs_ignore_obstacles (ai_atoms copies the command script's flag)
        bool destroying_obstacle;
        bool zigzag;
        bool boost_allowed;
        bool throttle_close;
        bool pose_allowed;
        bool wandering;                               // set only by behavior_wander_control
        bool combat_jump_allowed;
        bool crouch;
        bool moving_crouch;
        bool suppress_player_visibility;
        bool throttle_override_valid;                 // scripted vehicle throttle (ai_vehicles, actor_moving, vehicle movement)
        float throttle_override;
        bool lower_weapon_blocked_b;
        bool combat_flag665;
        bool suppress_burst_aiming;
        bool ready;
        union { long move_target_type; long fire_target_type; };
        union { c_ai_point3d move_target_point; c_ai_point3d fire_target_point; long fire_target_prop_index; long move_target_prop_index; };
        bool allow_unseen_target_fire;
        bool bypass;
        union { bool direction_dirty; bool lower_weapon_alternate; };
        bool lower_weapon_blocked_c;
        unsigned char secondary_control_mode;
        unsigned char secondary_control_flags;
        long secondary_control_target;
    };
    c_ai_action action;
    long flags;
    c_destination_orders destination_orders;      // March actor->control.path.destination_orders
    union { short path_state; unsigned short combat_value724; };                             // Signed path state.
    bool path_refresh_disabled;
    short path_refresh_failures;
    actor_path path;
    union {
        path_error_info path_error;
        struct {
            short path_result_time;
            short path_result_count;
            path_step_source failed_thresholds[3];
        };
    };
    real_point3d movement_point;
    short movement_point_ticks;
    short isolation_level;                        // Isolation level used by the guardian surge.
    real_vector3d throttle;
    real_vector3d steering_vector;
    union {
        actor_persistent_movement persistent_movement;
        struct {
            real_plane3d persistent_movement_plane;
            float persistent_movement_distance;
            short persistent_movement_ticks;
            short move_freeze_ticks;              // ai_dialogue raises it to a tick count (behavior.cpp agrees)
        };
    };
    union { short shooting_suppression_ticks; short combat_ticks838; };
    real_vector2d jump_aim;
    float jump_horizontal_velocity;
    float jump_vertical_velocity;
    long movement_index;
    long movement_obstacle_index;
    short movement_obstacle_ticks;
    long movement_object_index;
    union {                                       // 52 bytes of movement-controller state (actors clears all of it)
        struct {
            bool turning_in_place;
            bool holding_position;
            short movement_mode;
            union {                               // ground speed override / flying steering history
                float vehicle_speed;
                float previous_yaw;
            };
            float yaw;
            float previous_pitch;
            float pitch;
            float urgency;
            real_vector3d vehicle_throttle;
            real_vector3d previous_throttle;
        };
        turn_info vehicle_turn;                   // directional ground vehicles (March debug_warthog_turn_info)
        ground_steering_state ground_steering;            // nondirectional ground vehicles and the giant scarab
    };
    short attention_level;
    short attention_ticks;
    c_ai_direction attention_direction;
    long attention_object_index;
    long attention_prop_index;
    long attention_actor_index;
    c_ai_direction secondary_aim_direction;
    c_ai_direction previous_aim_direction;
    long aim_object_index;
    real_vector3d forward8e0;
    real_vector3d forward8ec;
    real_vector3d forward8f8;
    union { short aim_flags; short zigzag_timer; };
    short emotion_type;                           // ai_dialogue sets it with the emotion of a dialogue event
    union { short weapon_flags; short weapon_flag908; };
    union { short target_state; short combat_aim_mode; };                           // Signed combat aiming mode.
    union { short aim_ticks; short delay; };
    union { short aim_timer; short combat_ticks90e; };
    union { short aim_field910; short burst_cooldown_ticks; };
    union { short firing_ticks; short clear_line_of_fire_ticks; };
    short special_fire_ticks;
    union { short burst_count; short special_fire_situation; };
    union { float burst_length; float accuracy; };
    union { float aim_error; float firing_time; };
    union { long aim_target_index; long firing_start_time; };
    union { short firing_count; short impact_count; };
    union { short current_fire_target_type; unsigned short firing_type; };               // March actor->control.current_fire_target_type
    union {
        long current_fire_target_prop_index;
        long target_prop_ref_index;
        c_ai_point3d target_manual_point;
    };
    union { long attack_time; long fire_target_ticks; };
    short target_visibility;
    union { short attack_flags; short friendly_blocked_ticks; };
    union { long attack_object_index; long friendly_blocked_time; };
    c_ai_point3d target_point;
    float target_distance;
    union { real_vector3d attack_vector; real_vector3d target_aim_vector; };
    union { float attack_distance; float target_projectile_speed; };
    c_ai_point3d burst_target;
    real_point3d burst_target_position;
    real_vector3d burst_offset;
    real_vector3d burst_return;
    real_point3d burst_aim_point;
    float aim_field9a8;
    real_vector3d combat_aim_vector;
    float burst_aim_velocity;
    union { float damage_scale; float burst_aim_delay; };                         // Scales actor damage.
    union { short grenade_timer; short grenade_throw_ticks; };                          // (ai_atoms name; read as a signed short)
    union { long aim_field9c4; long grenade_throw_time; };
    union { c_ai_point3d target; c_ai_point3d grenade_target_point; };
    union { long target_unit; long grenade_target_type; };
    long grenade_target_index;
    real_vector3d grenade_velocity;
    float grenade_time;
    c_ai_point3d equipment_target;
    union { long movement_target_index; long last_combat_jump_time; };
    c_ai_point3d surge_destination;
    bool movement_target_valid;
    real_point3d movement_target;
    long output_animation_state;
    long output_weapon_index;
    real_vector2d output_animation_alignment;
    unsigned short output_aiming_speed;
    unsigned int control_flags;
    real_vector3d output_throttle;
    union { float output_primary_trigger; float control_trigger_primary; };
    union { float output_secondary_trigger; float control_trigger_secondary; };
    real_vector3d output_facing;
    real_vector3d output_aiming;
    real_vector3d output_looking;
    real_point3d output_gaze;
    long command_index;
    long command_script_index;
    short command_script_layer;
};
typedef char actor_datum_off_speed_override_valid[(offsetof(actor_datum, speed_override_valid) == 0x621) ? 1 : -1];
typedef char actor_datum_off_speed_override[(offsetof(actor_datum, speed_override) == 0x634) ? 1 : -1];
typedef char actor_datum_off_zigzag[(offsetof(actor_datum, zigzag) == 0x656) ? 1 : -1];
typedef char actor_datum_off_damage_scale[(offsetof(actor_datum, damage_scale) == 0x9bc) ? 1 : -1];
typedef char actor_datum_off_zigzag_timer[(offsetof(actor_datum, zigzag_timer) == 0x904) ? 1 : -1];
typedef char actor_datum_off_last_combat_jump_time[(offsetof(actor_datum, last_combat_jump_time) == 0xa00) ? 1 : -1];
typedef char actor_datum_suppress_burst_aiming_offset[(offsetof(actor_datum, suppress_burst_aiming) == 0x666) ? 1 : -1];
typedef actor_datum s_actor_datum;

typedef char actor_datum_off_control_vector[(offsetof(actor_datum, control_vector) == 0x628) ? 1 : -1];

typedef char actor_firing_position_entry_size[(sizeof(actor_firing_position_entry) == 6) ? 1 : -1];
typedef char actor_datum_off_firing_positions[(offsetof(actor_datum, firing_positions) + offsetof(actor_firing_position_entry, reference) == 0x5ba) ? 1 : -1];
typedef char actor_datum_off_control[((int)&((actor_datum *)0)->look_mode == 0x5e8) ? 1 : -1];
typedef char actor_datum_control_span[(offsetof(actor_datum, action) - offsetof(actor_datum, look_mode) == 0xa0) ? 1 : -1];
typedef char actor_datum_off_dialogue_object[((int)&((actor_datum *)0)->dialogue_object_index == 0x454) ? 1 : -1];
typedef char actor_datum_off_dialogue_distance[((int)&((actor_datum *)0)->dialogue_object_distance == 0x458) ? 1 : -1];
typedef char actor_datum_off_aim_910[((int)&((actor_datum *)0)->aim_field910 == 0x910) ? 1 : -1];
typedef char actor_datum_off_aim_9a8[((int)&((actor_datum *)0)->aim_field9a8 == 0x9a8) ? 1 : -1];
typedef char actor_datum_off_aim_9c4[((int)&((actor_datum *)0)->aim_field9c4 == 0x9c4) ? 1 : -1];
typedef char actor_datum_off_9dc[((int)&((actor_datum *)0)->grenade_target_index == 0x9dc) ? 1 : -1];
typedef char actor_datum_off_output_speed[((int)&((actor_datum *)0)->output_aiming_speed == 0xa34) ? 1 : -1];
typedef char actor_datum_off_output_gaze[((int)&((actor_datum *)0)->output_gaze == 0xa74) ? 1 : -1];

typedef char actor_datum_off_combat_active[((int)&((actor_datum *)0)->combat_active == 0x33b) ? 1 : -1];
typedef char actor_datum_off_player_seen[((int)&((actor_datum *)0)->player_seen == 0x583) ? 1 : -1];

typedef char actor_datum_off_visible[((int)&((actor_datum *)0)->player_visible == 0x57c) ? 1 : -1];
typedef char actor_datum_off_visibility_ticks[((int)&((actor_datum *)0)->player_visibility_ticks == 0x580) ? 1 : -1];
typedef char actor_datum_off_nearest_object[((int)&((actor_datum *)0)->nearest_object_distance == 0x588) ? 1 : -1];
typedef char actor_datum_off_boosted[((int)&((actor_datum *)0)->jump_boosted == 0x63a) ? 1 : -1];
typedef char actor_datum_off_pose_allowed[((int)&((actor_datum *)0)->pose_allowed == 0x659) ? 1 : -1];
typedef char actor_datum_off_path_disabled[((int)&((actor_datum *)0)->path_refresh_disabled == 0x726) ? 1 : -1];
typedef char actor_datum_off_path_failures[((int)&((actor_datum *)0)->path_refresh_failures == 0x728) ? 1 : -1];
typedef char actor_datum_off_persistent[((int)&((actor_datum *)0)->persistent_movement == 0x820) ? 1 : -1];
typedef char actor_datum_off_obstacle[((int)&((actor_datum *)0)->movement_obstacle_index == 0x850) ? 1 : -1];
typedef char actor_datum_off_obstacle_ticks[((int)&((actor_datum *)0)->movement_obstacle_ticks == 0x854) ? 1 : -1];
typedef char actor_path_point_off_hint[((int)&((actor_path_point *)0)->hint == 8) ? 1 : -1];
typedef char actor_datum_off_jump_aim[((int)&((actor_datum *)0)->jump_aim == 0x83c) ? 1 : -1];
typedef char actor_datum_off_jump_horizontal[((int)&((actor_datum *)0)->jump_horizontal_velocity == 0x844) ? 1 : -1];
typedef char actor_datum_off_jump_vertical[((int)&((actor_datum *)0)->jump_vertical_velocity == 0x848) ? 1 : -1];

typedef char actor_datum_size_stim[(sizeof(s_stimulus) == 8) ? 1 : -1];
typedef char actor_datum_size_position[(sizeof(actor_position_data) == 0x34) ? 1 : -1];
typedef char actor_datum_size_danger_zone[(sizeof(actor_danger_zone) == 0x58) ? 1 : -1];
typedef char actor_datum_size_path_point[(sizeof(actor_path_point) == 0x1c) ? 1 : -1];
typedef char actor_datum_size_path[(sizeof(actor_path) == 0xb0) ? 1 : -1];
typedef char actor_datum_size_actor[(sizeof(actor_datum) == 0xa8c) ? 1 : -1];
typedef char actor_datum_off_vehicle_turn_index[(offsetof(actor_datum, vehicle_turn) + offsetof(turn_info, index) == 0x874) ? 1 : -1];
typedef char actor_datum_off_path_error[(offsetof(actor_datum, path_error) == 0x7dc) ? 1 : -1];
typedef char actor_datum_off_failed_thresholds[(offsetof(actor_datum, failed_thresholds) == 0x7e0) ? 1 : -1];
typedef char actor_datum_off_output_alignment[(offsetof(actor_datum, output_animation_alignment) == 0xa2c) ? 1 : -1];
typedef char actor_datum_off_script_layer[(offsetof(actor_datum, command_script_layer) == 0xa88) ? 1 : -1];
typedef char actor_datum_off_boarding_dialogue_active[((int)&((actor_datum *)0)->boarding_dialogue_active == 0x582) ? 1 : -1];
typedef char actor_datum_off_boarding_dialogue_ticks[((int)&((actor_datum *)0)->boarding_dialogue_ticks == 0x584) ? 1 : -1];
typedef char actor_datum_off_boarding_dialogue_player_index[((int)&((actor_datum *)0)->boarding_dialogue_player_index == 0x586) ? 1 : -1];
typedef char actor_datum_off_recent_obstacles[((int)&((actor_datum *)0)->recent_obstacles == 0x404) ? 1 : -1];
typedef char actor_datum_off_invitations[((int)&((actor_datum *)0)->invitations == 0x288) ? 1 : -1];
typedef char actor_datum_off_interaction_target[((int)&((actor_datum *)0)->interaction_target == 0x41c) ? 1 : -1];
typedef char actor_datum_off_dialogue_entries[((int)&((actor_datum *)0)->dialogue_entries == 0x434) ? 1 : -1];
typedef char actor_datum_off_flee_movement[((int)&((actor_datum *)0)->flee_movement == 0x61d) ? 1 : -1];
typedef char actor_datum_off_flying_throttle[((int)&((actor_datum *)0)->vehicle_throttle == 0x874) ? 1 : -1];
typedef char actor_datum_off_ignore_obstacles[((int)&((actor_datum *)0)->ignore_obstacles == 0x654) ? 1 : -1];
typedef char actor_datum_off_throttle_override_valid[((int)&((actor_datum *)0)->throttle_override_valid == 0x65f) ? 1 : -1];
typedef char actor_datum_off_throttle_override[((int)&((actor_datum *)0)->throttle_override == 0x660) ? 1 : -1];
typedef char actor_datum_off_vehicle_turn[((int)&((actor_datum *)0)->vehicle_turn == 0x85c) ? 1 : -1];
typedef char actor_datum_off_ground_steering[((int)&((actor_datum *)0)->ground_steering == 0x85c) ? 1 : -1];
typedef char actor_datum_off_attention_level[((int)&((actor_datum *)0)->attention_level == 0x890) ? 1 : -1];
typedef char turn_info_size[(sizeof(turn_info) == 0x34) ? 1 : -1];
typedef char turn_info_off_index[((int)&((turn_info *)0)->index == 0x18) ? 1 : -1];
typedef char turn_info_off_valid[((int)&((turn_info *)0)->valid == 0x30) ? 1 : -1];
typedef char actor_datum_off_previous_aim[((int)&((actor_datum *)0)->previous_aim_direction == 0x8c8) ? 1 : -1];
typedef char actor_datum_off_output_animation[((int)&((actor_datum *)0)->output_animation_state == 0xa24) ? 1 : -1];
typedef char actor_path_off_final_point[((int)&((actor_path *)0)->final_point == 0x01) ? 1 : -1];
typedef char actor_path_off_start_point[((int)&((actor_path *)0)->start_point == 0x04) ? 1 : -1];
typedef char actor_path_off_destination_vector[((int)&((actor_path *)0)->destination_vector == 0x24) ? 1 : -1];
typedef char actor_path_off_destination_sector[((int)&((actor_path *)0)->destination_sector == 0x30) ? 1 : -1];
typedef char actor_path_off_destination_distance[((int)&((actor_path *)0)->destination_distance == 0x34) ? 1 : -1];
typedef char actor_datum_off_outside_bsp[((int)&((actor_datum *)0)->outside_bsp_ticks == 0x2c) ? 1 : -1];
typedef char actor_datum_off_equipment_conditions[((int)&((actor_datum *)0)->equipment_condition_flags == 0x2e) ? 1 : -1];
typedef char actor_datum_off_equipment[((int)&((actor_datum *)0)->equipment_index == 0x60) ? 1 : -1];
typedef char actor_datum_off_child_test_times[((int)&((actor_datum *)0)->child_test_times == 0x2c8) ? 1 : -1];
typedef char actor_datum_off_dialogue_sector[((int)&((actor_datum *)0)->dialogue_sector == 0x460) ? 1 : -1];
typedef char actor_datum_off_dialogue_point[((int)&((actor_datum *)0)->dialogue_point == 0x464) ? 1 : -1];
typedef char actor_datum_off_had_shield[((int)&((actor_datum *)0)->had_shield == 0x47c) ? 1 : -1];
typedef char actor_datum_off_reserved_distance[((int)&((actor_datum *)0)->reserved_distance == 0x558) ? 1 : -1];
typedef char actor_datum_off_attach_timer[((int)&((actor_datum *)0)->attach_timer == 0x562) ? 1 : -1];
typedef char actor_datum_off_exited_vehicle[((int)&((actor_datum *)0)->exited_vehicle_index == 0x56c) ? 1 : -1];
typedef char actor_datum_off_detach_timer[((int)&((actor_datum *)0)->detach_timer == 0x572) ? 1 : -1];
typedef char actor_datum_off_seat_attach_timer[((int)&((actor_datum *)0)->seat_attach_timer == 0x576) ? 1 : -1];
typedef char actor_datum_off_pickup_state[((int)&((actor_datum *)0)->pickup_state == 0x590) ? 1 : -1];
typedef char actor_datum_off_output_throttle[((int)&((actor_datum *)0)->output_throttle == 0xa3c) ? 1 : -1];
typedef char actor_datum_off_output_primary_trigger[((int)&((actor_datum *)0)->output_primary_trigger == 0xa48) ? 1 : -1];
typedef char actor_datum_off_output_secondary_trigger[((int)&((actor_datum *)0)->output_secondary_trigger == 0xa4c) ? 1 : -1];
typedef char actor_datum_off_output_facing[((int)&((actor_datum *)0)->output_facing == 0xa50) ? 1 : -1];
typedef char actor_datum_off_output_aiming[((int)&((actor_datum *)0)->output_aiming == 0xa5c) ? 1 : -1];
typedef char actor_datum_off_output_looking[((int)&((actor_datum *)0)->output_looking == 0xa68) ? 1 : -1];
typedef char actor_datum_offset_unit_index[(((int)&((actor_datum *)0)->unit_index) == 0x14) ? 1 : -1];
typedef char actor_datum_offset_squad_index[(((int)&((actor_datum *)0)->squad_index) == 0x30) ? 1 : -1];
typedef char actor_datum_offset_character_index[(((int)&((actor_datum *)0)->character_index) == 0x5c) ? 1 : -1];
typedef char actor_datum_offset_behavior_state_data[(((int)&((actor_datum *)0)->behavior_state_data) == 0x74) ? 1 : -1];
typedef char actor_datum_offset_behavior_mode[(((int)&((actor_datum *)0)->behavior_mode) == 0x300) ? 1 : -1];
typedef char actor_datum_offset_stimuli[(((int)&((actor_datum *)0)->stimuli) == 0x308) ? 1 : -1];
typedef char actor_datum_offset_position[(((int)&((actor_datum *)0)->position) == 0x34c) ? 1 : -1];
typedef char actor_datum_offset_root_vehicle_index[(((int)&((actor_datum *)0)->root_vehicle_index) == 0x388) ? 1 : -1];
typedef char actor_datum_offset_pathfinding_point[(((int)&((actor_datum *)0)->pathfinding_point) == 0x398) ? 1 : -1];
typedef char actor_datum_offset_facing_vector[(((int)&((actor_datum *)0)->facing_vector) == 0x3ac) ? 1 : -1];
typedef char actor_datum_offset_target_prop_index[(((int)&((actor_datum *)0)->target_prop_index) == 0x4ac) ? 1 : -1];
typedef char actor_datum_offset_danger_zone[(((int)&((actor_datum *)0)->danger_zone) == 0x4c4) ? 1 : -1];
typedef char actor_datum_offset_attached_vehicle_index[(((int)&((actor_datum *)0)->attached_vehicle_index) == 0x550) ? 1 : -1];
typedef char actor_datum_offset_dynamic_firing_set_index[(((int)&((actor_datum *)0)->dynamic_firing_set_index) == 0x5ac) ? 1 : -1];
typedef char actor_datum_offset_look_direction[(((int)&((actor_datum *)0)->look_direction) == 0x5ec) ? 1 : -1];
typedef char actor_datum_offset_action[(((int)&((actor_datum *)0)->action) == 0x688) ? 1 : -1];
typedef char actor_datum_offset_path[(((int)&((actor_datum *)0)->path) == 0x72c) ? 1 : -1];
typedef char actor_datum_offset_isolation_level[(((int)&((actor_datum *)0)->isolation_level) == 0x806) ? 1 : -1];
typedef char actor_datum_offset_shooting_suppression_ticks[(((int)&((actor_datum *)0)->shooting_suppression_ticks) == 0x838) ? 1 : -1];
typedef char actor_datum_offset_attention_direction[(((int)&((actor_datum *)0)->attention_direction) == 0x894) ? 1 : -1];
typedef char actor_datum_offset_control_flags[(((int)&((actor_datum *)0)->control_flags) == 0xa38) ? 1 : -1];
typedef char actor_datum_offset_command_script_index[(((int)&((actor_datum *)0)->command_script_index) == 0xa84) ? 1 : -1];
typedef char actor_datum_offset_scripted_target_index[(((int)&((actor_datum *)0)->scripted_target_index) == 0x4a8) ? 1 : -1];
typedef char actor_datum_offset_target_age[(((int)&((actor_datum *)0)->target_age) == 0x4bc) ? 1 : -1];
typedef char actor_datum_offset_activity_index[(((int)&((actor_datum *)0)->activity_index) == 0x548) ? 1 : -1];
typedef char actor_datum_offset_destination_orders[(((int)&((actor_datum *)0)->destination_orders) == 0x6bc) ? 1 : -1];
typedef char actor_datum_offset_path_state[(((int)&((actor_datum *)0)->path_state) == 0x724) ? 1 : -1];
typedef char actor_datum_offset_current_fire_target_type[(((int)&((actor_datum *)0)->current_fire_target_type) == 0x926) ? 1 : -1];
typedef char actor_datum_offset_player_weight[(((int)&((actor_datum *)0)->player_weight) == 0x400) ? 1 : -1];
typedef char actor_datum_offset_pending_vocalization[(((int)&((actor_datum *)0)->pending_vocalization) == 0x476) ? 1 : -1];
typedef char actor_datum_offset_vocalization_permutation[(((int)&((actor_datum *)0)->vocalization_permutation) == 0x47b) ? 1 : -1];
typedef char actor_datum_offset_close_enemy[(((int)&((actor_datum *)0)->situation.close_enemy) == 0x4a0) ? 1 : -1];
typedef char actor_datum_offset_damage_dialogue[(((int)&((actor_datum *)0)->last_damage_dialogue_time) == 0x42c) ? 1 : -1];
typedef char actor_datum_offset_blind[(((int)&((actor_datum *)0)->blind) == 0x337) ? 1 : -1];
typedef char actor_datum_offset_deaf[(((int)&((actor_datum *)0)->deaf) == 0x338) ? 1 : -1];
typedef char actor_datum_offset_last_morph[(((int)&((actor_datum *)0)->morph.last_morph_time) == 0x480) ? 1 : -1];
typedef char actor_datum_offset_morph_delay[(((int)&((actor_datum *)0)->morph.morph_delay_ticks) == 0x484) ? 1 : -1];
typedef char actor_datum_offset_current_form[(((int)&((actor_datum *)0)->morph.current_form) == 0x48a) ? 1 : -1];
typedef char actor_datum_offset_morph_locked[(((int)&((actor_datum *)0)->morph.morph_locked) == 0x48c) ? 1 : -1];
typedef char actor_datum_offset_immediate_morph[(((int)&((actor_datum *)0)->morph.immediate_morph) == 0x48d) ? 1 : -1];
typedef char actor_datum_offset_pending_form_priority[(((int)&((actor_datum *)0)->morph.pending_form_priority) == 0x492) ? 1 : -1];
typedef char actor_datum_offset_encounter_index[(((int)&((actor_datum *)0)->encounter_index) == 0x4c0) ? 1 : -1];
typedef char actor_datum_offset_unowned_target_salient[(((int)&((actor_datum *)0)->unowned_target_salient) == 0x4c2) ? 1 : -1];
typedef char actor_datum_offset_seat_reentry_timer[(((int)&((actor_datum *)0)->seat_reentry_timer) == 0x574) ? 1 : -1];
typedef char actor_datum_offset_move_freeze_ticks[(((int)&((actor_datum *)0)->move_freeze_ticks) == 0x836) ? 1 : -1];
typedef char actor_datum_offset_emotion_type[(((int)&((actor_datum *)0)->emotion_type) == 0x906) ? 1 : -1];

typedef char actor_datum_off_last_discarded[(offsetof(actor_datum, last_discarded_firing_position) == 0x5b6) ? 1 : -1];
typedef char actor_datum_off_current_firing_point[(offsetof(actor_datum, current_firing_position_point) == 0x5d4) ? 1 : -1];

// Combat control layout checks.

typedef char actor_combat_off_lower_weapon_forced[(offsetof(actor_datum, lower_weapon_forced) == 0x618) ? 1 : -1];

typedef char actor_combat_off_ready[(offsetof(actor_datum, ready) == 0x667) ? 1 : -1];

typedef char actor_combat_off_special_fire_ticks[(offsetof(actor_datum, special_fire_ticks) == 0x914) ? 1 : -1];

typedef char actor_combat_off_target_manual_point[(offsetof(actor_datum, target_manual_point) == 0x928) ? 1 : -1];

typedef char actor_combat_off_burst_target[(offsetof(actor_datum, burst_target) == 0x968) ? 1 : -1];

typedef char actor_combat_off_burst_aim_velocity[(offsetof(actor_datum, burst_aim_velocity) == 0x9b8) ? 1 : -1];

typedef char actor_combat_off_grenade_velocity[(offsetof(actor_datum, grenade_velocity) == 0x9e0) ? 1 : -1];

// Member boundaries around implicit alignment gaps.
typedef char actor_datum_flee_control_layout[
    (offsetof(actor_datum, flee_control) == 0x61c) ? 1 : -1];
typedef char actor_datum_layer_count_layout[
    (offsetof(actor_datum, layer_count) == 0x284) ? 1 : -1];
typedef char actor_datum_berserk_ticks_layout[
    (offsetof(actor_datum, berserk_ticks) == 0x348) ? 1 : -1];
typedef char actor_datum_ticks_since_combat_layout[
    (offsetof(actor_datum, ticks_since_combat) == 0x428) ? 1 : -1];
typedef char actor_datum_dialogue_entries_layout[
    (offsetof(actor_datum, dialogue_entries) == 0x434) ? 1 : -1];
typedef char actor_datum_throttle_ticks_layout[
    (offsetof(actor_datum, throttle_ticks) == 0x47e) ? 1 : -1];
typedef char actor_datum_prop_count_layout[
    (offsetof(actor_datum, situation.prop_count) == 0x498) ? 1 : -1];
typedef char actor_datum_best_prop_class_layout[
    (offsetof(actor_datum, situation.best_prop_class) == 0x49a) ? 1 : -1];
typedef char actor_datum_surprise_prop_index_layout[
    (offsetof(actor_datum, surprise_prop_index) == 0x520) ? 1 : -1];
typedef char actor_datum_stun_scale_layout[
    (offsetof(actor_datum, stun_scale) == 0x528) ? 1 : -1];
typedef char actor_datum_exited_vehicle_index_layout[
    (offsetof(actor_datum, exited_vehicle_index) == 0x56c) ? 1 : -1];
typedef char actor_datum_activity_scariness_layout[
    (offsetof(actor_datum, activity_scariness) == 0x594) ? 1 : -1];
typedef char actor_datum_charge_deactivate_time_layout[
    (offsetof(actor_datum, charge_deactivate_time) == 0x5a0) ? 1 : -1];
typedef char actor_datum_goal_status_layout[
    (offsetof(actor_datum, goal_status) == 0x5a8) ? 1 : -1];
typedef char actor_datum_control_type_layout[
    (offsetof(actor_datum, control_type) == 0x624) ? 1 : -1];
typedef char actor_datum_secondary_control_target_layout[
    (offsetof(actor_datum, secondary_control_target) == 0x684) ? 1 : -1];
typedef char actor_datum_grenade_throw_time_layout[
    (offsetof(actor_datum, grenade_throw_time) == 0x9c4) ? 1 : -1];
typedef char actor_datum_movement_target_layout[
    (offsetof(actor_datum, movement_target) == 0xa18) ? 1 : -1];
typedef char actor_path_start_point_layout[
    (offsetof(actor_path, start_point) == 4) ? 1 : -1];
typedef char actor_danger_zone_ticks_layout[
    (offsetof(actor_danger_zone, ticks) == 0x54) ? 1 : -1];

#endif // AI_ACTOR_DATUM_H
