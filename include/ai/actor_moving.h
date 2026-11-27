#ifndef AI_ACTOR_MOVING_H
#define AI_ACTOR_MOVING_H

#include <stddef.h>

#include "ai/pathfinding_mode_types.h"

#include "math/real_math_types.h"
#include "cseries/cseries_wrapped.h"
#include "ai/sector_ref.h"
#include "ai/firing_position_ref.h"
#include "ai/ai_point3d.h"
#include "ai/ai_actions.h"
#include "ai/reserved_hints.h"

struct firing_position_ref;
class c_path_destination;

enum e_destination_flags
{
    _destination_flag_0 = 0,
    _destination_flag_1 = 1,
    _destination_flag_2 = 2,
    _destination_flag_3 = 3,                      // action pending (posture, corner, land set it; equals compares action)
    _destination_flag_4 = 4,
    _destination_flag_5 = 5,
    _destination_flag_6 = 6,
    k_destination_flags_count = 7
};

// 0x68-byte movement orders (actor_datum + 0x6bc; March asserts `actor->control.path.destination_orders`).
// Every method kept out
// of line (all but test and set) lives in actor_moving.cpp. March's constructor builds `flags` (c_flags()).
// type: 0 none, 1 point, 2 air point, 3 firing position, 4 prop.
class c_destination_orders
{
public:
    void clear();
    bool equals(long actor_index, c_destination_orders *other);
    bool ground_destination();
    bool air_destination();
    bool to_raw(long actor_index, c_path_destination *destination, e_pathfinding_mode *mode);
    // match: 0x535b88, 0x5458f8: callers keep the point/sector words live across the call; removal reloads both words from memory after the call.
    __declspec(noalias) bool to_point(c_ai_point3d *point, c_sector_ref *sector);
    __forceinline bool to_point(real_point3d *point, c_sector_ref *sector)
    {
        c_ai_point3d ai_point;
        ai_point.set(point);
        return to_point(&ai_point, sector);
    }
    bool to_air_point(c_ai_point3d *point);
    __forceinline bool to_air_point(real_point3d *point)
    {
        c_ai_point3d ai_point;
        ai_point.set(point);
        return to_air_point(&ai_point);
    }
    bool to_firing_position(firing_position_ref reference);
    bool to_prop(long prop_ref_index);
    bool corner(long actor_index, const c_ai_point3d *point, real_vector3d *normal, short corner_type, bool corner_flag);
    bool posture(long actor_index, const c_ai_point3d *point, real_vector3d *facing, long posture_name);
    bool perch(long actor_index, real_vector3d *normal);
    bool land(long actor_index);
    __forceinline bool test(e_destination_flags flag) { return flags.test(flag); }
    __forceinline void set(e_destination_flags flag) { set(flag, true); }
    __forceinline void set(e_destination_flags flag, bool value) { flags.set(flag, value); }

    short type;
    c_flags<e_destination_flags, short, k_destination_flags_count> flags;
    union                                         // 0x04 per type (March clear() memsets each member)
    {
        struct
        {
            c_ai_point3d point;
            c_sector_ref sector;
        } point_destination;                      // types 1, 2
        long firing_position_index;               // type 3: firing_position_ref (two shorts)
        firing_position_ref firing_position;
        long prop_ref_index;                      // type 4
    };
    long target_index;                            // object (to_prop: prop state's, else the prop ref's)
    float f1c;
    float f20;
    float f24;
    float f28;
    real_vector3d m_destination_facing;           // March assert text names it
    c_ai_action action;
};

typedef char destination_orders_size[(sizeof(c_destination_orders) == 0x68) ? 1 : -1];
typedef char destination_orders_offset_target_index[((offsetof(c_destination_orders, target_index)) == 0x18) ? 1 : -1];
typedef char destination_orders_offset_facing[((offsetof(c_destination_orders, m_destination_facing)) == 0x2c) ? 1 : -1];
typedef char destination_orders_offset_action[((offsetof(c_destination_orders, action)) == 0x38) ? 1 : -1];


#include "ai/actor_moving_functions.h"

float rays_nearest_distance(real_point2d *, real_vector2d *, real_point2d *, real_vector2d *, bool *, float *, float *);

enum e_deceleration_obstacle_results;
float deceleration_obstacle_attenuation(long actor_index, long object_index, float radius, float deceleration_distance,
    float deceleration_velocity, float stop_distance, const real_vector2d *direction, real_point3d *obstacle_position_out,
    float *obstacle_radius_out, e_deceleration_obstacle_results *results);

#endif // AI_ACTOR_MOVING_H
