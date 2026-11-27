#ifndef UNITS_ACTION_REQUEST_H
#define UNITS_ACTION_REQUEST_H

#include <stddef.h>

#include "math/real_math_types.h"

struct s_action_hoist_reference {
    long a;
    long b;
};

struct action_request {
    union {
        long action;
        long type;
    };
    union {
        char payload[0x30];
        char data[0x30];
        struct { bool use_forward; real_vector3d forward; } deploy;
        struct {
            union { long unit_index; long target_index; };
            short seat_index;
            bool flag_a;
            bool flag_b;
        };
        struct {
            union {
                long weapon;
                struct { bool immediate; bool simulated; bool force; };
            };
            union {
                long mode;
                struct {
                    real_point3d position;
                    real_vector3d direction;
                } grenade;
            };
        };
        struct { short squad_index, spawn_index, location_index; } emit_ai;
        struct { short direction, flags; long target; long player_target; } melee;
        struct { short melee_type; bool has_vector; real_vector2d vector; } melee_ai;
        struct { short type, weapon_slot; unsigned char flag, mode; long target; } melee_player;
        struct {
            union { short first; short ticks; };
            union { short second; short type; };
        } resurrect;
        struct { long value; short index; char mode; bool flag; } update;
        struct { unsigned char a, b; } bytes;
        struct { long parent_index; short seat_index; bool flag_a; bool flag_b; } seat;
        struct { long vehicle; short seat; bool immediate; bool force; } enter_vehicle;
        struct {
            long animation;
            long animation_index;
            bool flag_a;
            bool flag_b;
            signed char animation_mode;
            bool flag_c;
        } initial_animation;
        struct { unsigned char flag; unsigned char value; } evict;
        struct { bool immediate; bool pause; } open;
        struct { bool immediate; bool exit; } exit_vehicle;
        // Replicated grenade initiation and equipment throw requests (actions 56/57).
        struct {
            bool initiate, release, forced;

            real_point3d position;
            real_vector3d direction;
        } grenade_initiate;
        struct {
            long definition_index;
            bool initiate, release, forced;

            real_point3d position;
            real_vector3d direction;
        } throw_item;
        struct { long object_index; short duration; } infection;
        struct {
            union { long character; long definition_index; };
            union { long time; long widget_index; };
            union { bool force; unsigned char a; };
            union { bool immediate; bool b; };
            union { char form; signed char mode; };
            union { bool unknown; bool change_team; };
        } morph;
        // actor_impulse_submit (0x516ac0), action 42: an unnormalized flags byte at +0x18.
        struct { unsigned char mode, has_position; long type; real_vector3d position; unsigned char flags; } impulse;
        struct { char mode; bool has_vector; real_vector3d vector; bool flag; } motor;
        struct { unsigned short value; real_point3d position; real_vector3d velocity; } motion;
        struct { real_point3d position; real_vector3d velocity; unsigned short value; } vectors;
        struct { real_point3d point; real_vector2d vector; bool flag; } attach;
        struct { signed char mode; bool has_vector; long animation; real_point3d vector; bool extra; } animation;
        struct { signed char has_vector; real_point3d vector; float value; } surge;
        struct { real_point3d position; real_vector3d forward; bool has_position; long posture; bool a, b; } posture;
        struct {
            long graph;
            long animation;
            bool custom;
            bool loop;

            long object_index;
            bool interpolate;
            bool use_position;

            real_point3d position;
            real_vector3d forward;
            bool extra;
        } play;
        struct {
            long animation;
            long variant;
            bool interpolate;
            bool b0d;

            long target;
            bool no_command;
            bool has_facing;

            real_point3d position;
            real_vector3d facing;
            bool b30;
        } custom_animation;
        struct {
            real_point3d point;
            real_vector3d direction;
            long object_index;
            bool hint;
        } hint;
        // land, transient posture, hoist and phase requests; a jump request keeps its flag at +0x10
        struct {
            real_point3d position;
            union {
                real_vector3d alignment;
                unsigned char jump_flag;
            };
        };
        struct { bool has_position; union { real_point2d position_yz; real_vector2d direction; }; } jump;
        struct { real_vector3d velocity; real_vector3d normal; } leap_recovery;
        struct { real_point3d destination; real_vector3d velocity; } leap_anticipation;
        struct { signed char type, region; real_vector3d direction; bool has_direction; } ping;
        struct { real_point3d position; real_vector3d alignment; s_action_hoist_reference reference; } hoist;
        struct { real_point3d destination; float duration; } phase;
        struct { real_point3d point; float value; } surge_finished;
        struct { short a, b; unsigned char c, d; long e; } command;
    };
};

typedef char action_request_grenade_initiate_offset[(offsetof(action_request, grenade_initiate.initiate) == 4) ? 1 : -1];
typedef char action_request_throw_initiate_offset[(offsetof(action_request, throw_item.initiate) == 8) ? 1 : -1];
typedef char action_request_target_offset[(offsetof(action_request, target_index) == 4) ? 1 : -1];

typedef char action_request_impulse_type_offset[(offsetof(action_request, impulse.type) == 8) ? 1 : -1];
typedef char action_request_impulse_position_offset[(offsetof(action_request, impulse.position) == 0xc) ? 1 : -1];
typedef char action_request_impulse_flags_offset[(offsetof(action_request, impulse.flags) == 0x18) ? 1 : -1];

typedef char action_request_morph_definition_offset[(offsetof(action_request, morph.definition_index) == 4) ? 1 : -1];
typedef char action_request_morph_widget_offset[(offsetof(action_request, morph.widget_index) == 8) ? 1 : -1];
typedef char action_request_morph_flags_offset[(offsetof(action_request, morph.a) == 0xc) ? 1 : -1];
typedef char action_request_morph_mode_offset[(offsetof(action_request, morph.mode) == 0xe) ? 1 : -1];
typedef char action_request_morph_team_offset[(offsetof(action_request, morph.change_team) == 0xf) ? 1 : -1];

typedef char action_request_emit_ai_offset[(offsetof(action_request, emit_ai.location_index) == 8) ? 1 : -1];

typedef char action_request_size[(sizeof(action_request) == 0x34) ? 1 : -1];
typedef char action_request_vehicle_offset[(offsetof(action_request, enter_vehicle.vehicle) == 4) ? 1 : -1];
typedef char action_request_animation_mode_offset[(offsetof(action_request, initial_animation.animation_mode) == 0xe) ? 1 : -1];

typedef char action_request_deploy_forward_layout[(offsetof(action_request, deploy.forward) == 0x8) ? 1 : -1];
typedef char action_request_melee_ai_vector_layout[(offsetof(action_request, melee_ai.vector) == 0x8) ? 1 : -1];
typedef char action_request_melee_player_target_layout[(offsetof(action_request, melee_player.target) == 0xc) ? 1 : -1];
typedef char action_request_grenade_initiate_position_layout[(offsetof(action_request, grenade_initiate.position) == 0x8) ? 1 : -1];
typedef char action_request_throw_item_position_layout[(offsetof(action_request, throw_item.position) == 0xc) ? 1 : -1];
typedef char action_request_motor_vector_layout[(offsetof(action_request, motor.vector) == 0x8) ? 1 : -1];
typedef char action_request_motion_position_layout[(offsetof(action_request, motion.position) == 0x8) ? 1 : -1];
typedef char action_request_animation_vector_layout[(offsetof(action_request, animation.vector) == 0xc) ? 1 : -1];
typedef char action_request_surge_vector_layout[(offsetof(action_request, surge.vector) == 0x8) ? 1 : -1];
typedef char action_request_posture_posture_layout[(offsetof(action_request, posture.posture) == 0x20) ? 1 : -1];
typedef char action_request_play_object_index_layout[(offsetof(action_request, play.object_index) == 0x10) ? 1 : -1];
typedef char action_request_play_position_layout[(offsetof(action_request, play.position) == 0x18) ? 1 : -1];
typedef char action_request_custom_animation_target_layout[(offsetof(action_request, custom_animation.target) == 0x10) ? 1 : -1];
typedef char action_request_custom_animation_position_layout[(offsetof(action_request, custom_animation.position) == 0x18) ? 1 : -1];
typedef char action_request_jump_position_yz_layout[(offsetof(action_request, jump.position_yz) == 0x8) ? 1 : -1];
typedef char action_request_ping_direction_layout[(offsetof(action_request, ping.direction) == 0x8) ? 1 : -1];
typedef char action_request_command_e_layout[(offsetof(action_request, command.e) == 0xc) ? 1 : -1];
typedef char action_request_alignment_layout[(offsetof(action_request, alignment) == 0x10) ? 1 : -1];
typedef char action_request_jump_flag_layout[(offsetof(action_request, jump_flag) == 0x10) ? 1 : -1];

#endif // UNITS_ACTION_REQUEST_H
