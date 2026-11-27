#ifndef CREATURES_CREATURE_DATUM_H
#define CREATURES_CREATURE_DATUM_H

#include <stddef.h>

#include "objects/object_datum.h"
#include "cseries/cseries_wrapped.h"
#include "physics/character_physics_component.h"

// March creature_control_verify: control_flags, turning_speed, throttle, forward and up.
struct s_creature_control_data {
    long action;
    long animation;
    union { unsigned short control_flags; unsigned short flags; };
    signed char turning_speed;
    char pad0b;
    union { real_vector3d throttle; real_vector3d movement; };
    union { real_vector3d forward; real_vector3d facing; };
    real_vector3d up;
};

enum e_creature_flags {
    _creature_flag_actively_controlled = 0,  // creature_set_actively_controlled
    _creature_flag_1 = 1,
    _creature_flag_2 = 2,
    _creature_flag_3 = 3,
    _creature_flag_airborne = 5,             // creature ground motor program: not on the ground
    _creature_flag_7 = 7
};

struct _creature_datum {
    s_object_block_reference motor_tasks;                   // (March mover_new: c_motor_task array)
    s_object_block_reference motor_program;                 // (creature_new: c_motor_program_state)
    s_object_block_reference action;                        // (March mover_new)
    union {
        unsigned short flags;
        c_flags_no_init<e_creature_flags, unsigned short, 10> m_flags;
    };
    short team;
    long link_type;                                         // (0 swarm, 1 flock)
    s_object_block_reference creature_reference;
    s_creature_control_data control;
    real_vector3d turning_velocity;                         // creature_update_turning: angular velocity of the facing
    float turning_amount;
    c_character_physics_component physics;
    char damage_timers_padding[0x238 - 0x234]; // filler between physics and the damage timer cluster; no creature reader uses these bytes
    signed char attached_damage_delay_ticks;
    signed char impact_delay_ticks;
    short death_delay_ticks;
    short form_lifetime;                                    // (swarm_spawner_set_creature_lifetime)
};

// Creatures have the object prefix, but no _unit_datum.
struct creature_datum {
    object_datum object;
    _creature_datum creature;
};

typedef char creature_datum_control_size[(sizeof(s_creature_control_data) == 0x30) ? 1 : -1];
typedef char creature_datum_size[(sizeof(creature_datum) == 0x240) ? 1 : -1];
typedef char creature_datum_offset_creature[(offsetof(creature_datum, creature) == 0x160) ? 1 : -1];
typedef char creature_datum_offset_flags[(offsetof(creature_datum, creature.flags) == 0x16c) ? 1 : -1];
typedef char creature_datum_offset_control[(offsetof(creature_datum, creature.control) == 0x178) ? 1 : -1];
typedef char creature_datum_offset_physics[(offsetof(creature_datum, creature.physics) == 0x1b8) ? 1 : -1];
typedef char creature_datum_offset_lifetime[(offsetof(creature_datum, creature.form_lifetime) == 0x23c) ? 1 : -1];
typedef char creature_datum_offset_attached_damage_delay[(offsetof(creature_datum, creature.attached_damage_delay_ticks) == 0x238) ? 1 : -1];
typedef char creature_datum_offset_death_delay[(offsetof(creature_datum, creature.death_delay_ticks) == 0x23a) ? 1 : -1];

typedef char creature_datum_creature_impact_delay_ticks_offset[(offsetof(creature_datum, creature.impact_delay_ticks) == 0x239) ? 1 : -1];

#endif // CREATURES_CREATURE_DATUM_H
