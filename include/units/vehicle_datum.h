#ifndef UNITS_VEHICLE_DATUM_H
#define UNITS_VEHICLE_DATUM_H

#include <stddef.h>

#include "units/unit_types.h"
#include "units/vehicle_type_component.h"
#include "ai/sector_ref.h"
#include "animations/animation_id.h"

// March vehicle_reset: c_flags_no_init<e_vehicle_flags, unsigned short, 10>.
enum e_vehicle_flags {
    _vehicle_hover_bit = 2,
    _vehicle_flag_3 = 3,
    _vehicle_alien_scout_trick = 3,
    _guardian_vehicle_boost = 4,
    _vehicle_alien_scout_boost = 4
};

struct _vehicle_datum {
    __forceinline c_vehicle_type_alien_fighter *get_alien_fighter()
    {
        return type_component.get_type_alien_fighter();
    }
    union {
        unsigned short flags;
        c_flags_no_init<e_vehicle_flags, unsigned short, 10> m_flags;
    };
    unsigned char upside_down_ticks;                        // grounded ticks spent upside down (vehicle_motor_program)
    unsigned char airborne_ticks;
    unsigned char flip_direction;
    unsigned char flip_time;
    unsigned char braking_ticks;
    unsigned short grounded; // friction-point mask
    float function_value;
    union {
        char type_state[0x84];
        c_vehicle_type_component type_component;
        struct { char type_component_state[0x64]; unsigned char suspension_samples[32]; };
    };                                  // (vehicle-type component storage)
    union { unsigned char suspension_weights[32]; unsigned char smoothed_suspension[32]; };
    long friction_update_time;                              // game time of the last friction-point query (havok_vehicle_physics)
    long squad_index;                                       // (squads.cpp)
    long next_vehicle_index;
    long starting_location_name;
    long counter;                                           // (vehicle_reset)
    unsigned long locked_seats[2];                          // (ai_vehicle_reserve)
    long reservations[128];                                 // 64 allied + 64 enemy actor indices
    long pathfinding_time;                                  // (vehicle_find_pathfinding_location)
    short pathfinding_structure_index;
    long pathfinding_surface_index;
    c_sector_ref pathfinding_sector;
    long pathfinding_instanced_geometry_index;
    long pathfinding_object_index;
    unsigned long pathfinding_bsp_reference;
    real_point3d pathfinding_point;
    long animation_graph_index;
    c_animation_id impulse_ids[4];
    c_animation_id screen_ids[3];
    union { float angular_velocity; float parent_height; };
    union { float velocity; float impulse; };
    real_vector3d forward;
    real_point3d last_position;
    s_object_block_reference suspension;                    // (vehicle_new allocates this block)
};

struct vehicle_datum {
    object_datum object;
    _unit_datum unit;
    _vehicle_datum vehicle;
};

typedef char vehicle_datum_size[(sizeof(vehicle_datum) == 0x784) ? 1 : -1];
typedef char vehicle_datum_type_state_offset[(offsetof(vehicle_datum, vehicle.type_state) == 0x458) ? 1 : -1];
typedef char vehicle_datum_function_value_offset[(offsetof(vehicle_datum, vehicle.function_value) == 0x454) ? 1 : -1];
typedef char vehicle_datum_offset_unit[(offsetof(vehicle_datum, unit) == 0x160) ? 1 : -1];
typedef char vehicle_datum_offset_vehicle[(offsetof(vehicle_datum, vehicle) == 0x448) ? 1 : -1];
typedef char vehicle_datum_offset_airborne_ticks[(offsetof(vehicle_datum, vehicle.airborne_ticks) == 0x44b) ? 1 : -1];
typedef char vehicle_datum_offset_braking_ticks[(offsetof(vehicle_datum, vehicle.braking_ticks) == 0x44e) ? 1 : -1];
typedef char vehicle_datum_offset_grounded[(offsetof(vehicle_datum, vehicle.grounded) == 0x450) ? 1 : -1];
typedef char vehicle_datum_offset_squad[(offsetof(vehicle_datum, vehicle.squad_index) == 0x500) ? 1 : -1];
typedef char vehicle_datum_offset_suspension[(offsetof(vehicle_datum, vehicle.suspension) == 0x780) ? 1 : -1];
typedef char vehicle_datum_offset_locked_seats[(offsetof(vehicle_datum, vehicle.locked_seats) == 0x510) ? 1 : -1];
typedef char vehicle_datum_offset_reservations[(offsetof(vehicle_datum, vehicle.reservations) == 0x518) ? 1 : -1];

typedef char vehicle_flip_direction_offset[(offsetof(vehicle_datum, vehicle.flip_direction) == 0x44c) ? 1 : -1];
typedef char vehicle_flip_time_offset[(offsetof(vehicle_datum, vehicle.flip_time) == 0x44d) ? 1 : -1];
typedef char vehicle_upside_down_ticks_offset[(offsetof(vehicle_datum, vehicle.upside_down_ticks) == 0x44a) ? 1 : -1];
typedef char vehicle_friction_update_time_offset[(offsetof(vehicle_datum, vehicle.friction_update_time) == 0x4fc) ? 1 : -1];
typedef char vehicle_pathfinding_surface_index_offset[(offsetof(vehicle_datum, vehicle.pathfinding_surface_index) == 0x720) ? 1 : -1];
typedef char vehicle_pathfinding_instanced_geometry_index_offset[
    (offsetof(vehicle_datum, vehicle.pathfinding_instanced_geometry_index) == 0x728) ? 1 : -1];
typedef char vehicle_pathfinding_bsp_reference_offset[(offsetof(vehicle_datum, vehicle.pathfinding_bsp_reference) == 0x730) ? 1 : -1];

typedef char vehicle_suspension_offset[(offsetof(vehicle_datum, vehicle.suspension_samples) == 0x4bc) ? 1 : -1];
typedef char vehicle_smoothed_suspension_offset[(offsetof(vehicle_datum, vehicle.smoothed_suspension) == 0x4dc) ? 1 : -1];
typedef char vehicle_parent_height_offset[(offsetof(vehicle_datum, vehicle.parent_height) == 0x760) ? 1 : -1];
typedef char vehicle_impulse_offset[(offsetof(vehicle_datum, vehicle.impulse) == 0x764) ? 1 : -1];

#endif // UNITS_VEHICLE_DATUM_H
