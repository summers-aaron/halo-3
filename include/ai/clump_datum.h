#ifndef AI_CLUMP_DATUM_H
#define AI_CLUMP_DATUM_H

#include <stddef.h>

#include "math/real_math_types.h"
#include "ai/joint_behavior_types.h"
#include "ai/task_group_types.h"

// Per-clump AI state.
struct clump_datum {
    short identifier;
    short spare_after_identifier;
    real_point3d position;
    short actor_count;
    short team;
    float player_distance;
    float radius;
    union {
        long first_prop_index;
        long first_prop;
    };
    union {
        long first_actor_index;
        long first_actor;
        long first;
    };
    bool active;
    bool giant;
    short age;
    long last_active_time;
    float activation_value;
    float minimum_prop_importance;
    float maximum_prop_salience;
    short prop_acquisition_delay_ticks;
    union { short s58; unsigned short death_flags; }; // clump_new and clump_update_behavior_state reset this counter.
    union {
        s_task_group group;
        struct {
            union { short combat_state; short count; };
            short state_flags;
            long state_time;
        };
    };
    float maximum_perception_distance;
    long last_time;
    long last_visible_enemy_time;
    unsigned char disposable_actors_pending;
    union { bool dialogue_ready; unsigned char dialogue_flags; };
    union { short death_count; unsigned short update_count; };
    long dialogue_state;
    char spare_after_dialogue_state[0x64 - 0x58];
    union {
        long last_scared_time;                    // behavior_scared_by_leader_deactivate writes game time
        long refs[1];                             // One-entry reference list.
    };
    invitation_data invitations[10];
};
typedef clump_datum s_clump_datum;

typedef char clump_datum_size[(sizeof(clump_datum) == 0x108) ? 1 : -1];
typedef char clump_datum_invitations_offset[(offsetof(clump_datum, invitations) == 0x68) ? 1 : -1];
typedef char clump_datum_scared_time_offset[(offsetof(clump_datum, last_scared_time) == 0x64) ? 1 : -1];
typedef char clump_datum_player_distance_offset[(offsetof(clump_datum, player_distance) == 0x14) ? 1 : -1];
typedef char clump_datum_active_offset[(offsetof(clump_datum, active) == 0x24) ? 1 : -1];
typedef char clump_datum_giant_offset[(offsetof(clump_datum, giant) == 0x25) ? 1 : -1];
typedef char clump_datum_last_active_offset[(offsetof(clump_datum, last_active_time) == 0x28) ? 1 : -1];
typedef char clump_datum_activation_offset[(offsetof(clump_datum, activation_value) == 0x2c) ? 1 : -1];
typedef char clump_datum_min_importance_offset[(offsetof(clump_datum, minimum_prop_importance) == 0x30) ? 1 : -1];
typedef char clump_datum_max_salience_offset[(offsetof(clump_datum, maximum_prop_salience) == 0x34) ? 1 : -1];
typedef char clump_datum_acquisition_delay_offset[(offsetof(clump_datum, prop_acquisition_delay_ticks) == 0x38) ? 1 : -1];
typedef char clump_datum_combat_state_offset[(offsetof(clump_datum, combat_state) == 0x3c) ? 1 : -1];
typedef char clump_datum_perception_distance_offset[(offsetof(clump_datum, maximum_perception_distance) == 0x44) ? 1 : -1];
typedef char clump_datum_visible_enemy_offset[(offsetof(clump_datum, last_visible_enemy_time) == 0x4c) ? 1 : -1];
typedef char clump_datum_disposable_offset[(offsetof(clump_datum, disposable_actors_pending) == 0x50) ? 1 : -1];

typedef char clump_group_offset[(offsetof(clump_datum, group) == 0x3c) ? 1 : -1];
typedef char clump_state_counter_offset[(offsetof(clump_datum, s58) == 0x3a) ? 1 : -1];
typedef char clump_last_time_offset[(offsetof(clump_datum, last_time) == 0x48) ? 1 : -1];

typedef char clump_death_flags_offset[(offsetof(clump_datum, death_flags) == 0x3a) ? 1 : -1];

typedef char clump_datum_refs_offset[(offsetof(clump_datum, refs) == 0x64) ? 1 : -1];

#endif // AI_CLUMP_DATUM_H
