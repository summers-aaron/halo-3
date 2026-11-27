#ifndef UNITS_GIANT_DATUM_H
#define UNITS_GIANT_DATUM_H

#include "units/unit_types.h"
#include "units/motor_enums.h"
#include "ai/sector_ref.h"
#include <ppcintrinsics.h>

enum e_foot_tracking_state;

enum e_giant_buckle_state
{
    k_giant_buckle_state_none = 0,
    k_giant_buckle_state_descending,
    k_giant_buckle_state_buckled,
    k_giant_buckle_state_ascending,
    k_giant_buckle_state_finished,
    k_number_of_giant_buckle_states
};

struct s_giant_scarab_anim
{
    short index;
    unsigned short count;
};

struct s_giant_scarab_motor_flags
{
    unsigned long storage;
    bool test(int bit) const { return (bool)(_rotl(storage, 32 - bit) & 1); }
    void clear(int bit) { storage &= ~(1ul << bit); }
    void set(int bit) { storage |= (1ul << bit); }
};

struct s_giant_foot
{
    long marker;
    real_point3d ground;
    real_point3d future;
    real_point3d ray_origin;
    real_vector3d ray_direction;
    float weight;
    short frame;
    c_flags_no_init<long, unsigned char, 8> flags;
    c_enum<e_foot_tracking_state, unsigned char, 0, 4> state;
};

struct s_buckling_map
{
    short width, height;
    real_point3d points[32][32];
};

struct s_giant_datum
{
    object_datum object;
    _unit_datum unit;
    s_giant_foot feet[4];
    char spare_after_feet[0x55c - 0x538];
    s_buckling_map buckling_map;
    real_matrix4x3 buckling_center;
    real_point3d highest_ground;
    real_point3d lowest_ground;
    real_quaternion rotation;
    union { unsigned long giant_flags; s_giant_scarab_motor_flags motor_flags; };
    unsigned char havok_rebuild_ticks;
    float buckle_position;
    float buckle_velocity;
    float buckle_acceleration;
    union { float buckle_fraction; float playback_rate_scale; };
    float buckle_timer;
    float active_interpolation_time;
    long foot_count;
    char spare_before_init_zero[0x35e4 - 0x35e0];
    long spare_init_zero; // zeroed at construction, never read
    char spare_after_init_zero[0x35ec - 0x35e8];
    long node_index;
    short variant_index;
    c_enum<e_giant_buckle_state, short, 0, k_number_of_giant_buckle_states> buckling_state;
    real_matrix4x3 identity_copy;
    long pathfinding_time;
    short pathfinding_structure_index;
    long pathfinding_surface_index;
    c_sector_ref pathfinding_sector;
    long pathfinding_instanced_geometry_index;
    long pathfinding_object_index;
    unsigned long pathfinding_flags;
    real_point3d pathfinding_point;
};
typedef char giant_feet_offset[(offsetof(s_giant_datum, feet) == 0x448) ? 1 : -1];
typedef char giant_buckling_map_offset[(offsetof(s_giant_datum, buckling_map) == 0x55c) ? 1 : -1];
typedef char giant_identity_copy_offset[(offsetof(s_giant_datum, identity_copy) == 0x35f4) ? 1 : -1];
typedef char giant_datum_size[(sizeof(s_giant_datum) == 0x3650) ? 1 : -1];


typedef char giant_offset_motor_flags[(offsetof(s_giant_datum, motor_flags) == 0x35bc) ? 1 : -1];
typedef char giant_offset_buckle_position[(offsetof(s_giant_datum, buckle_position) == 0x35c4) ? 1 : -1];
typedef char giant_offset_playback_rate_scale[(offsetof(s_giant_datum, playback_rate_scale) == 0x35d0) ? 1 : -1];
typedef char giant_offset_active_interpolation_time[(offsetof(s_giant_datum, active_interpolation_time) == 0x35d8) ? 1 : -1];
typedef char giant_offset_anim[(offsetof(s_giant_datum, variant_index) == 0x35f0) ? 1 : -1];
typedef char giant_offset_buckling_state[(offsetof(s_giant_datum, buckling_state) == 0x35f2) ? 1 : -1];

typedef char giant_offset_havok_rebuild_ticks[(offsetof(s_giant_datum, havok_rebuild_ticks) == 0x35c0) ? 1 : -1];
typedef char giant_offset_pathfinding_time[(offsetof(s_giant_datum, pathfinding_time) == 0x3628) ? 1 : -1];
typedef char giant_offset_pathfinding_structure_index[(offsetof(s_giant_datum, pathfinding_structure_index) == 0x362c) ? 1 : -1];
typedef char giant_offset_pathfinding_surface_index[(offsetof(s_giant_datum, pathfinding_surface_index) == 0x3630) ? 1 : -1];
typedef char giant_offset_pathfinding_instanced_geometry_index[
    (offsetof(s_giant_datum, pathfinding_instanced_geometry_index) == 0x3638) ? 1 : -1];
typedef char giant_offset_pathfinding_object_index[(offsetof(s_giant_datum, pathfinding_object_index) == 0x363c) ? 1 : -1];
typedef char giant_offset_pathfinding_flags[(offsetof(s_giant_datum, pathfinding_flags) == 0x3640) ? 1 : -1];
typedef char giant_offset_pathfinding_point[(offsetof(s_giant_datum, pathfinding_point) == 0x3644) ? 1 : -1];

#endif // UNITS_GIANT_DATUM_H
