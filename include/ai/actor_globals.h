#ifndef AI_ACTOR_GLOBALS_H
#define AI_ACTOR_GLOBALS_H

#include "objects/object_reference_types.h"
#include "math/real_math_types.h"
#include "structures/cluster_bit_vectors.h"
#include <stddef.h>

struct s_ai_mission_critical_unit {
    long unit_index;
    union {
        long source_index;
        long object_index;
    };
    short flags;
    short bring_forward_ticks;
    union {
        float bring_forward_value;
        float time;
    };
};
typedef s_ai_mission_critical_unit s_mission_critical;

// Pending sound entry: ai_sound_new_at_position reads 28-byte entries at globals + 0x40.
struct s_ai_sound_slot {
    short type;
    short volume;
    short count;
    short pad6;
    real_point3d position;
    long time;
    long pad24;
};
typedef char ai_sound_slot_size[(sizeof(s_ai_sound_slot) == 28) ? 1 : -1];

// AI update state stored in thread-local storage.
struct ai_globals_type {
    union {
        bool active;
        bool ai_active;
    };
    union {
        bool enabled;
        bool gate;
    };
    bool initialized;
    bool firing_positions_evaluated;
    bool pad_04;
    bool throttled;
    short maximum_ticks_a;
    short worst_ticks_a;
    bool timeslice_given_a;
    char pad_0b;
    short maximum_ticks_b;
    short worst_ticks_b;
    bool timeslice_given_b;
    char pad_11;
    union {
        short count;
        short next_decision_index;
    };
    union {
        long first_squadless_actor;
        int first_actor_index;
    };
    long age;
    float major_upgrade_bias;
    union {
        bool dialogue_enabled;
        bool dialogue;
        bool enabled_at_20;
    };
    char pad_21;
    union {
        short dialogue_suspend_level;
        short dialogue_disabled;
    };
    long actor_indices[2];
    long squad_indices[2];
    long objective_indices[2];
    unsigned short sound_head;
    unsigned short sound_tail;
    s_ai_sound_slot sound_slots[32];
    union {
        bool perception_enabled;
        bool drop_grenades;
        bool grenades_enabled;
    };
    union {
        long current_objective;
        long scripted_unknown_time;
    };
    short deleted_object_count;
    short pad3ca;
    long deleted_object_indices[8];
    long current_squad;
    short last_actor_index;
    short active_actor_count;
    union { long current_actor; long last_player_sound_time; };
    long last_cleanup_time; // Prop cleanup throttle.
    s_ai_mission_critical_unit mission_critical[8];
    union { short current_mission_critical; s_cluster_reference pas_cluster_reference; };
    short pad47e;
    s_game_cluster_bit_vectors pas;
};

typedef char actor_globals_size_mc[(sizeof(s_ai_mission_critical_unit) == 0x10) ? 1 : -1];
typedef char actor_globals_size_globals[(sizeof(ai_globals_type) == 0x680) ? 1 : -1];

typedef char actor_globals_cleanup_offset[((unsigned long)&((ai_globals_type *)0)->last_cleanup_time == 0x3f8) ? 1 : -1];

typedef char actor_globals_pas_offset[(offsetof(ai_globals_type, pas) == 0x480) ? 1 : -1];
typedef char actor_globals_sound_time_offset[(offsetof(ai_globals_type, last_player_sound_time) == 0x3f4) ? 1 : -1];

typedef char actor_globals_sound_slots_offset[(offsetof(ai_globals_type, sound_slots) == 0x40) ? 1 : -1];
typedef char actor_globals_deletion_count_offset[(offsetof(ai_globals_type, deleted_object_count) == 0x3c8) ? 1 : -1];
typedef char actor_globals_deletion_indices_offset[(offsetof(ai_globals_type, deleted_object_indices) == 0x3cc) ? 1 : -1];

extern __declspec(thread) ai_globals_type *ai_globals;

#endif // AI_ACTOR_GLOBALS_H
