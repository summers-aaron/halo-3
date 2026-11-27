#ifndef AI_WEAPON_DATUM_TYPES_H
#define AI_WEAPON_DATUM_TYPES_H

#include "items/item_types.h"
#include <stddef.h>
#include "animations/animation_id.h"

struct s_weapon_magazine
{
    short state;
    short ticks;
    short ticks_total;
    short rounds_inventory;
    short predicted_rounds_inventory;
    short rounds_loaded;
    short predicted_rounds_loaded;
    short rounds_recharge_remainder;
    short fire_id;
    short chamber_id;
    short time;
    short pending_reload;
};

struct s_weapon_barrel {
    char active;
    char state_1;
    short ticks;
    unsigned short flags;
    unsigned short value;
    unsigned short firing_effect_mask;
    short current_firing_effect;
    short current_firing_effect_shots;
    short projectile_counter;
    float rate_fraction;
    float ejection_port;
    float illumination_fraction;
    float firing_error_fraction;
    float error;
    float fire_time;

    float recovery_fraction;
    short extra_shots;
    unsigned char shot_count;
    unsigned char recovery_delay_ticks;
    long firing_effect_index;
};
struct s_weapon_trigger {
    char state;
    unsigned char tracking_ticks;
    unsigned char mispredict_ticks;
    short timer;
    unsigned short flags;
    long charging_effect_index;
};
struct s_weapon_first_person_emulation
{
    long animation_graph_index;
    c_animation_id animation_id;
    float authored_time;
    long animation_name;
    long secondary;
};
struct weapon_datum
{
    union { item_datum item; long definition_index; };
    union { unsigned short flags; unsigned char flags_bytes[2]; c_flags_no_init<long, unsigned short, 16> weapon_flags; };
    unsigned short control_flags;
    unsigned char analog_trigger;
    unsigned char previous_analog_trigger;
    unsigned char trigger_reference;
    signed char trigger_direction;
    signed char trigger_ticks;
    unsigned char analog_value;
    union { signed char current_state_ticks; signed char tracked_model_target_index; };
    union { bool predicted; unsigned char prediction_ticks; };
    union { unsigned short flags_180; short weapon_state; };
    short ready_ticks;
    short time_184;
    short multiplayer_weapon_identifier;
    short simulation_weapon_identifier;
    unsigned char turn_on_timer;
    unsigned char ready_for_use_timer;
    float heat;
    float age;
    float predicted_age;
    union { float f198; float jitter; };
    float current_power;
    union { float f1a0; float integrated_light_power; };
    long tracked_object_index;
    long pad1a8; // unreferenced bytes between tracked_object_index and queued_projectile_count; no readers in tree
    long pad1ac;
    short queued_projectile_count;
    s_weapon_barrel barrels[2];
    s_weapon_trigger triggers[2];
    s_weapon_magazine magazines[2];
    long overheated_effect_index;
    long game_time_last_fired;
    long target_time;
    s_weapon_first_person_emulation first_person_emulation;
    long first_person_sound_index;
    long first_person_sound_animation_name;
};

typedef char weapon_multiplayer_identifier_offset[(offsetof(weapon_datum, multiplayer_weapon_identifier) == 0x186) ? 1 : -1];

typedef char weapon_magazine_size[(sizeof(s_weapon_magazine) == 0x18) ? 1 : -1];
typedef char weapon_integrated_light_power_offset[(offsetof(weapon_datum, integrated_light_power) == 0x1a0) ? 1 : -1];
typedef char weapon_age_offset[(offsetof(weapon_datum, age) == 0x190) ? 1 : -1];
typedef char weapon_jitter_offset[(offsetof(weapon_datum, jitter) == 0x198) ? 1 : -1];
typedef char weapon_magazine_ticks_total_offset[(offsetof(s_weapon_magazine, ticks_total) == 4) ? 1 : -1];
typedef char weapon_tracked_model_target_offset[(offsetof(weapon_datum, tracked_model_target_index) == 0x17e) ? 1 : -1];
typedef char weapon_predicted_offset[(offsetof(weapon_datum, predicted) == 0x17f) ? 1 : -1];
typedef char weapon_magazines_offset[(offsetof(weapon_datum, magazines) == 0x234) ? 1 : -1];

typedef char weapon_barrel_size[(sizeof(s_weapon_barrel) == 0x34) ? 1 : -1];
typedef char weapon_trigger_size[(sizeof(s_weapon_trigger) == 0xc) ? 1 : -1];
typedef char weapon_trigger_timer_offset[(offsetof(s_weapon_trigger, timer) == 4) ? 1 : -1];
typedef char weapon_queued_projectile_count_offset[(offsetof(weapon_datum, queued_projectile_count) == 0x1b0) ? 1 : -1];
typedef char weapon_barrels_offset[(offsetof(weapon_datum, barrels) == 0x1b4) ? 1 : -1];
typedef char weapon_triggers_offset[(offsetof(weapon_datum, triggers) == 0x21c) ? 1 : -1];
typedef char weapon_first_person_offset[(offsetof(weapon_datum, first_person_emulation) == 0x270) ? 1 : -1];
typedef char weapon_datum_size[(sizeof(weapon_datum) == 0x28c) ? 1 : -1];

typedef char weapon_state_offset[(offsetof(weapon_datum, weapon_state) == 0x180) ? 1 : -1];
typedef char weapon_flags_bytes_offset[(offsetof(weapon_datum, flags_bytes) == 0x174) ? 1 : -1];

#endif // AI_WEAPON_DATUM_TYPES_H
