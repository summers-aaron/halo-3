// blamlibxbox/debug_director.obj: from the records 0x2936b8 0x293980 0x2939f0 0x293a00 0x293a08
// @compiler xdk6130
// @flags /TP /O2 /J /Oc
// @address 0x293980 ?should_draw_hud@c_debug_director@@UBA_NXZ
// @address 0x744618 k_debug_director_camera_modes
// @address 0x11df8cc ?g_observer_globals@@3PAUs_player_globals@@A
#include "math/matrix_math_types.h"
#include "memory/tls_globals.h"
#include "memory/data_types.h"
#include "memory/datum_index.h"
#include "physics/collision_types.h"
#include "objects/object_reference_types.h"
#include "objects/object_datum.h"
#include "shell/shell.h"
#include "interface/user_interface_controller.h"
#include "camera/camera.h"
#include "input/input_abstraction.h"
#include "input/input_abstraction_types.h"
#include "camera/debug_director.h"
#include "camera/observer_globals_types.h"
#include "camera/camera_mode_types.h"
#include "physics/collision_bsp_types.h"
#include "physics/collision_test_vector.h"
#include "camera/debug_director_internals.h"
#include "camera/camera_types.h"

#if !defined(_XBOX) || defined(__clang__)
e_camera_mode k_debug_director_camera_modes[2] = {_camera_mode_flying, _camera_mode_following};
#endif

static __forceinline void debug_director_flags_set(unsigned long *flags, long bit, bool enabled)
{
    unsigned long mask = 1UL << bit;
    if (enabled)
        *flags |= mask;
    else
        *flags &= ~mask;
}

bool c_debug_director::should_draw_hud() const
{
    return get_camera()->get_type() != _camera_mode_flying && get_camera()->get_type() != _camera_mode_static;
}

// @address 0x2939f0 ?should_draw_hud_saved_film@c_debug_director@@UBA_NXZ
bool c_debug_director::should_draw_hud_saved_film() const
{
    return should_draw_hud();
}

// @address 0x293a00 ?inhibits_facing@c_debug_director@@UBA_NXZ
bool c_debug_director::inhibits_facing() const
{
    return m_inhibits_facing;
}

// @address 0x293a00 ?inhibits_input@c_debug_director@@UBA_NXZ
bool c_debug_director::inhibits_input() const
{
    return m_inhibits_facing;
}

// @address 0x293a08 ?changed_camera@c_debug_director@@AAAXXZ
void c_debug_director::changed_camera()
{
    set_camera_mode(k_debug_director_camera_modes[m_camera_index], 0.0f, 0);
    if (get_camera()->get_type() == _camera_mode_flying)
    {
        m_inhibits_facing = true;
        debug_director_flags_set(&m_camera.m_flags, 2, false);
        return;
    }
    else
    {
        m_inhibits_facing = false;
    }
}

// @address 0x2936b8 ?update@c_debug_director@@UAAXM@Z
void c_debug_director::update(float dt)
{
    s_collision_test_flags test_flags;
    real_vector3d direction;
    long controller_index = user_interface_controller_get_rumble_enabled(m_user_index);
    if (controller_index != -1 && camera_responding_gate((e_controller_index)controller_index) == 0)
    {
        s_game_input_state *input_state = &input_abstraction_globals.input_states[controller_index];
        c_abstract_button *buttons = input_state->buttons;
        c_abstract_button *mode_button = &buttons[12];
        if (buttons[0].m_alternate_button_index != -1)
            buttons[0].m_alternate_button_index = -1;
        bool long_press = mode_button->down_msec() >= 1000;
        if (long_press)
        {
            m_camera_index = (m_camera_index - 1) & 1;
            changed_camera();
            mode_button->m_flags |= 1;
        }
        if (k_debug_director_camera_modes[m_camera_index] == _camera_mode_flying)
        {
            c_abstract_button *facing_button = &buttons[9];
            if (facing_button->down_frames() == 1)
            {
                m_inhibits_facing = !m_inhibits_facing;
                facing_button->m_flags |= 1;
                if (m_inhibits_facing)
                {
                    set_camera_mode(_camera_mode_flying, 0.0f, 0);
                    debug_director_flags_set(&m_camera.m_flags, 2, false);
                }
                else
                    set_camera_mode(_camera_mode_static, 0.0f, 0);
            }
        }
        if (buttons[13].down_frames() == 1)
        {
            if (get_camera()->get_type() == _camera_mode_flying)
            {
                collision_result collision;
                test_flags.collision_flags = k_collision_test_camera.collision_flags;
                test_flags.object_flags = k_collision_test_camera.object_flags | 2;
                direction.i = g_observer_globals->blocks[m_user_index].camera_result.forward.i * 100.0f;
                direction.j = g_observer_globals->blocks[m_user_index].camera_result.forward.j * 100.0f;
                direction.k = g_observer_globals->blocks[m_user_index].camera_result.forward.k * 100.0f;
                if (collision_test_vector(test_flags, false,
                    &g_observer_globals->blocks[m_user_index].camera_result.position, &direction, -1, -1, -1, &collision)
                    && collision.object_index != -1
                    && ((s_object_header *)tls_object_header_data()->data)
                        [datum_index_to_absolute_index(collision.object_index)].object_type == 0)
                {
                    set_camera_mode(_camera_mode_orbiting, 0.5f, 0);
                    get_camera()->set_target(collision.object_index);
                }
            }
            else if (get_camera()->get_type() == _camera_mode_orbiting)
            {
                set_camera_mode(_camera_mode_flying, 0.0f, 0);
                debug_director_flags_set(&m_camera.m_flags, 2, false);
            }
        }
    }
    c_director::update(dt);
}
