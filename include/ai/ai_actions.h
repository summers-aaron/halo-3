#ifndef AI_AI_ACTIONS_H
#define AI_AI_ACTIONS_H

#include <stddef.h>

#include "math/real_math_types.h"
#include "ai/sector_ref.h"
#include "ai/ai_point3d.h"
#include "ai/ai_direction.h"

extern const real_point3d *global_origin3d;      // math/real_math.h

enum e_ai_action_result { _ai_action_result_0, _ai_action_result_1, _ai_action_result_2 };

// 0x30-byte pending unit action (actor_datum + 0x688, c_destination_orders + 0x38). Methods as in the March 2007
// debug build (asserts name ai\ai_actions.cpp); execute and get_entry_point are out of line in ai_actions.cpp.
// type selects the action: 1 posture, 2 corner, 3 perch, 4 land, 5 animation, 9 ticks countdown.
class c_ai_action
{
public:
    __forceinline void clear()
    {
        type = 0;
        point_valid = false;
        b5 = false;
        ticks = 0;
        posture_name = -1;
        impulse_index = 0;
        point.set(global_origin3d, -1, -1);
        direction.clear();
    }
    __forceinline bool valid() { return type != 0; }
    __forceinline bool setup_corner(const c_ai_point3d *corner_point, const c_ai_direction *corner_direction, short corner_type, bool corner_flag)
    {
        type = 2;
        point_valid = corner_point != 0;
        if (corner_point)
            point = *corner_point;
        if (corner_direction)
            direction = *corner_direction;
        else
            direction.clear();
        this->corner_type = corner_type;
        this->corner_flag = corner_flag;
        return true;
    }
    __forceinline bool setup_posture(const c_ai_point3d *posture_point, const c_ai_direction *posture_direction, long posture_name)
    {
        type = 1;
        point_valid = posture_point != 0;
        if (posture_point)
            point = *posture_point;
        if (posture_direction)
            direction = *posture_direction;
        else
            direction.clear();
        this->posture_name = posture_name;
        return true;
    }
    __forceinline bool setup_perch(const c_ai_point3d *perch_point, const real_vector3d *normal)
    {
        bool result = false;
        if (perch_point && normal)
        {
            type = 3;
            point_valid = true;
            point = *perch_point;
            direction.to_vector(normal);
            result = true;
        }
        return result;
    }
    __forceinline bool setup_land(const c_ai_point3d *land_point)
    {
        bool result = false;
        if (land_point)
        {
            type = 4;
            point_valid = true;
            point = *land_point;
            direction.clear();
            result = true;
        }
        return result;
    }
    __forceinline bool setup_animation(const c_ai_direction *animation_direction, long impulse_index)
    {
        type = 5;
        point_valid = false;
        if (animation_direction)
            direction = *animation_direction;
        else
            direction.clear();
        this->impulse_index = impulse_index;
        return true;
    }
    bool get_entry_point(long actor_index, const c_ai_point3d *point, const c_sector_ref *sector,
        c_ai_point3d *out_point, real_vector3d *out_vector, c_sector_ref *out_sector);
    e_ai_action_result execute(long actor_index);

    long type;
    bool point_valid;                             // setup_*: point given (posture, corner), always (perch, land), never (animation)
    bool b5;                                      // only ever cleared
    short ticks;                                  // type 9 counts it down (March: two cleared bytes)
    union                                         // 0x08 per type
    {
        long posture_name;                        // type 1: posture_info_get_by_name
        long impulse_index;                       // type 5: actor_impulse_submit
        struct                                    // type 2: runtime_cornering_get_entry
        {
            short corner_type;
            bool corner_flag;
            char pad_b;
        };
    };
    c_ai_point3d point;
    c_ai_direction direction;
};

typedef char ai_action_size[(sizeof(c_ai_action) == 0x30) ? 1 : -1];
typedef char ai_action_direction_offset[((offsetof(c_ai_action, direction)) == 0x1c) ? 1 : -1];


// Functions defined in tu/ailibxbox/ai_actions.cpp that other units call.

struct object_marker;

void actor_stow_weapon(long actor_index, bool stow, bool force);
bool actor_exit_posture(long actor_index);
bool ai_resurrect_get_marker(long object_index, object_marker *marker, bool *marker_up);
long actor_resurrect(long unit_index, short team, short resurrect_type, short squad_index);
bool process_pending_ground_actions(long actor_index);
bool actor_evade(long actor_index);
bool ai_handle_resurrection(long unit_index);

struct object_placement_data;

extern "C" void object_get_action_points(long object_index, object_placement_data *out);

#endif // AI_AI_ACTIONS_H
