// blamlibxbox/static_camera.obj: from the records 0x297d68 0x297d88 0x297da8 0x297e48
// @flags /TP /O2 /Oc

#include <ppcintrinsics.h>
#include <math.h>
#include <stdlib.h>
#include "camera/static_camera.h"
#include "camera/authored_camera.h"
#include "math/real_math.h"

// @address 0x297d68 ?set_position@c_static_camera@@UAAXPBTreal_point3d@@@Z
void c_static_camera::set_position(const real_point3d *position)
{
    m_position = *position;
}

// @address 0x297d68 ?set_position@c_flying_camera@@UAAXPBTreal_point3d@@@Z
void c_flying_camera::set_position(const real_point3d *position)
{
    m_position = *position;
}

// @address 0x297d88 ?set_forward@c_static_camera@@UAAXPBTreal_vector3d@@@Z
void c_static_camera::set_forward(const real_vector3d *forward)
{
    m_forward = *forward;
}

// @address 0x297da8 ?update@c_static_camera@@UAAXJMPAUs_observer_command@@@Z
void c_static_camera::update(long user_index, float elapsed_time, s_observer_command *command)
{
    command->position = m_position;
    command->focus_distance = 0.0f;
    command->forward = m_forward;
    command->up = m_up;
    command->field_of_view = m_field_of_view;
    command->flags = 1;
}

// @address 0x297e48 ?get_target@c_authored_camera@@UBAJXZ
long c_authored_camera::get_target() const
{
    if (m_sub_camera_active)
        return m_camera.get_target();
    return m_target;
}
