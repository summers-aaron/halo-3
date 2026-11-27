#ifndef INTERFACE_CHUD_CHUD_POINT_TRACKER_TYPES_H
#define INTERFACE_CHUD_CHUD_POINT_TRACKER_TYPES_H

#include "math/real_math_types.h"
#include <stddef.h>

class c_chud_point_tracker
{
public:
    __forceinline void prepare_for_draw(float maximum_distance);
    void update(const real_point3d *position, bool valid)
    {
        m_valid = valid;
        m_world_position = *position;
    }
    void get_position_virtual(real_point2d *position) const { *position = m_position; }
    float get_offscreen_amount() const { return m_offscreen_amount; }

public:
    bool m_valid;
    real_point3d m_world_position;
    real_point2d m_screen_position;
    real_point2d m_position;
    float m_offscreen_amount;
    bool m_behind;
    float m_turn_direction;
};

typedef char chud_point_tracker_size[(sizeof(c_chud_point_tracker) == 0x2c) ? 1 : -1];
typedef char chud_point_tracker_position_offset[(offsetof(c_chud_point_tracker, m_position) == 0x18) ? 1 : -1];

#endif
