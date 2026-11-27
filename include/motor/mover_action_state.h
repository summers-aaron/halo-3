#ifndef MOTOR_MOVER_ACTION_STATE_H
#define MOTOR_MOVER_ACTION_STATE_H

#include "ai/interaction_target.h"
#include "math/real_math_types.h"
#include "units/action_request.h"
#include <stddef.h>

// The mover's action block (mover_datum::action): 108 bytes, cleared by mover_new.
struct mover_state
{
    char header_bytes[12];
    long object;
    long action;
    union {
        char action_flags_bytes[4];
        struct { signed char melee_type; unsigned char weapon_slot; } smash;
    };
    union { long task; long smash_target_index; };
    union {
        char task_storage[32];
        short smash_ticks;
        struct {
            char task_prefix_storage[16];
            bool has_vector;
            char vector_alignment[3];
            real_point3d vector;
        } surge;
    };
    long target;
    union {
        char target_storage[28];
        bool prediction_active;
        struct {
            char target_prefix_storage[8];
            short timer;
            short squad_index;
            short spawn_index;
            short location_index;
        } emit_ai;
        struct { char target_prefix_storage[18]; unsigned char deployed; } turret;
        struct { char target_prefix_storage[16]; short stow_weapon; } berserk;
        struct {
            char target_prefix_storage[20];
            long definition_index;
            unsigned char a;
            bool b;
            bool change_team;
        } morph;
    };
    union {
        c_interaction_target interaction;
        s_action_hoist_reference hoist_target;
    };
    long spawner_index;
    long last_action_time;
};

typedef char mover_prediction_active_offset[(offsetof(mover_state, prediction_active) == 64) ? 1 : -1];

// Layout checks for the action-specific views of the shared unions.
typedef char mover_action_size[(sizeof(mover_state) == 0x6c) ? 1 : -1];
typedef char mover_smash_target_offset[(offsetof(mover_state, smash_target_index) == 0x18) ? 1 : -1];
typedef char mover_smash_ticks_offset[(offsetof(mover_state, smash_ticks) == 0x1c) ? 1 : -1];
typedef char mover_surge_flag_offset[(offsetof(mover_state, surge.has_vector) == 0x2c) ? 1 : -1];
typedef char mover_surge_vector_offset[(offsetof(mover_state, surge.vector) == 0x30) ? 1 : -1];
typedef char mover_emit_timer_offset[(offsetof(mover_state, emit_ai.timer) == 0x48) ? 1 : -1];
typedef char mover_emit_flags_offset[(offsetof(mover_state, turret.deployed) == 0x52) ? 1 : -1];
typedef char mover_berserk_offset[(offsetof(mover_state, berserk.stow_weapon) == 0x50) ? 1 : -1];
typedef char mover_morph_offset[(offsetof(mover_state, morph.definition_index) == 0x54) ? 1 : -1];
typedef char mover_hoist_offset[(offsetof(mover_state, hoist_target) == 0x5c) ? 1 : -1];

#endif // MOTOR_MOVER_ACTION_STATE_H
