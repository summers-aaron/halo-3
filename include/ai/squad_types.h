#ifndef AI_SQUAD_TYPES_H
#define AI_SQUAD_TYPES_H

#include <stddef.h>

#include <string.h>
#include "math/real_math_types.h"
#include "ai/Zone.h"

struct s_style_flags {
    unsigned long bits[7];

    void set_all() { memset(bits, 0xff, sizeof(bits)); }
    void clear() { memset(bits, 0, sizeof(bits)); }
    void set(long index, bool value) {
        if (value)
            bits[index >> 5] |= 1UL << (index & 31);
        else
            bits[index >> 5] &= ~(1UL << (index & 31));
    }
    void intersect(const s_style_flags *other) {
        for (unsigned long i = 0; i < 7; ++i)
            bits[i] &= other->bits[i];
    }
};

struct squad_datum {
    short header;
    short flags;
    float upgrade_bias;
    union {
        float original_vitality;
        float active_count;
        float strength_weight;
    };
    float original_count;
    union {
        short count0;
        short strength;
    };
    union {
        short count1;
        short current_swarm_count;
    };
    union {
        short living_count;
        short count2;
        short current_non_swarm_count;
    };
    union {
        float health;
        float status;
    };
    short prebattle_living_count;
    union {
        unsigned short fighting_count;
        short reserved_count;
    };
    union {
        real_point3d centroid;
        real_point3d mean_position;
    };
    union {
        short count;
        short spawn_count;
    };
    union {
        char state;
        signed char status_level;
    };
    short activation_ticks;
    signed char num_dynamic_areas;                // (calculate_dynamic_areas)
    zone_area_ref dynamic_areas[10];
    long vehicle_index;
    union {
        short objective_index;
        short objective;
    };
    union {
        short task_index;
        short task;
    };
    union {
        short previous_objective_index;
        short objective2_index;
    };
    union {
        short previous_task_index;
        short task2_index;
    };
    long last_task_change_time;
    long first_actor_index;
    long parent_index;
    long first_vehicle_index;
    short next_squad_index;
    signed char team;
    long last_activation_time;                    // (squad_activate)
    short inactive_ticks;                         // (ai_update_activation)
    long follow_object_index;                     // (calculate_dynamic_areas)
    s_style_flags style;
    long style_index;
    short next_index;
    bool temporary;
    union {
        short squad_spawn_count;
        short highest_rank;
    };
};
typedef squad_datum s_squad_datum;

typedef char squad_types_size_style[(sizeof(s_style_flags) == 0x1c) ? 1 : -1];
typedef char squad_types_size_squad[(sizeof(squad_datum) == 0xb0) ? 1 : -1];
typedef char squad_types_off_health[(offsetof(squad_datum, health) == 0x18) ? 1 : -1];
typedef char squad_types_off_prebattle[(offsetof(squad_datum, prebattle_living_count) == 0x1c) ? 1 : -1];
typedef char squad_types_off_fighting[(offsetof(squad_datum, fighting_count) == 0x1e) ? 1 : -1];
typedef char squad_types_off_activation_ticks[(offsetof(squad_datum, activation_ticks) == 0x30) ? 1 : -1];
typedef char squad_types_off_highest_rank[(offsetof(squad_datum, highest_rank) == 0xac) ? 1 : -1];
typedef char squad_types_off_strength_weight[(offsetof(squad_datum, strength_weight) == 8) ? 1 : -1];
typedef char squad_types_off_dynamic_count[(offsetof(squad_datum, num_dynamic_areas) == 0x32) ? 1 : -1];
typedef char squad_types_off_dynamic_areas[(offsetof(squad_datum, dynamic_areas) == 0x34) ? 1 : -1];
typedef char squad_types_off_first_vehicle[(offsetof(squad_datum, first_vehicle_index) == 0x74) ? 1 : -1];
typedef char squad_types_off_last_activation[(offsetof(squad_datum, last_activation_time) == 0x7c) ? 1 : -1];
typedef char squad_types_off_inactive_ticks[(offsetof(squad_datum, inactive_ticks) == 0x80) ? 1 : -1];
typedef char squad_types_off_follow_object[(offsetof(squad_datum, follow_object_index) == 0x84) ? 1 : -1];

#endif // AI_SQUAD_TYPES_H
