// blamlibxbox/authored_camera.obj: from the records 0x297e30 0x297e78 0x2980f0
// @compiler xdk6130
// @flags /TP /O2 /Oc
#include "math/matrix_math_types.h"
#include "camera/camera_globals.h"
#include "math/real_math.h"
#include "cseries/cseries.h"
#include "memory/tls_globals.h"
#include "game/players.h"
#include "game/player_types.h"
#include "camera/observer.h"
#include "camera/authored_camera.h"
// @address 0x761454 ?g_observer_default_field_of_view@@3MA
#include "game/director_perspective_types.h"






#include "camera/camera_mode_types.h"
#include "camera/observer_command.h"
#include "camera/camera_types.h"
#include "saved_games/saved_film_timer_globals.h"
// @address 0x86ce0 ?get_target@c_camera@@UBAJXZ
// @address 0x94e3b0 g_saved_film_timer_globals

// @address 0x8c42a0 ?g_camera_globals@@3Us_camera_globals@@A
#if !defined(_XBOX) || defined(__clang__)
s_camera_globals g_camera_globals;
#endif

// @address 0x297e30 ?get_perspective@c_authored_camera@@UBA?AW4e_director_perspective@@XZ
e_director_perspective c_authored_camera::get_perspective() const
{
    return m_sub_camera_active ? _director_perspective_first_person : _director_perspective_authored;
}


static __forceinline float authored_camera_suggested_field_of_view()
{
    return g_observer_default_field_of_view;
}

static __forceinline player_datum *authored_camera_player_try_and_get_absolute(long absolute_index)
{
    player_datum *result = 0;
    if (absolute_index != -1)
    {
        s_data_array *array = tls_player_data();
        if (absolute_index >= 0 && absolute_index < array->first_unallocated)
        {
            player_datum *player = (player_datum *)(array->data + absolute_index * array->size);
            if (player->identifier != 0)
                result = player;
        }
    }
    return result;
}

static __forceinline long authored_camera_player_index_from_absolute(long absolute_index)
{
    s_data_array *array = tls_player_data();
    const player_datum *player = (const player_datum *)(array->data + array->size * absolute_index);
    return ((unsigned long)player->identifier << 16) | absolute_index;
}

// @address 0x297e78 ?update@c_authored_camera@@UAAXJMPAUs_observer_command@@@Z
void c_authored_camera::update(long user_index, float elapsed_time, s_observer_command *command)
{
    bool sub_camera_active = false;
    bool sub_camera_following = false;
    if (TEST_BIT(g_saved_film_timer_globals.replay_flags, 0))
    {
        s_film_replay_metadata metadata = g_saved_film_timer_globals.replay_metadata;
        long unit_index = -1;
        if (g_saved_film_timer_globals.replay_metadata.type != 0)
        {
            long absolute_player_index = g_saved_film_timer_globals.replay_metadata.player_index;
            player_datum *player = authored_camera_player_try_and_get_absolute(absolute_player_index);
            if (player && !TEST_BIT(player->flags, 1))
            {
                players_set_local_player(user_index, authored_camera_player_index_from_absolute(absolute_player_index));
                unit_index = player->unit_index;
            }
        }
        if (metadata.type == 1)
        {
            m_camera.set_target(unit_index);
            m_camera.update(user_index, elapsed_time, command);
            sub_camera_active = true;
        }
        else if (metadata.type == 2)
        {
            m_target = unit_index;
            m_position = metadata.position;
            m_forward = metadata.forward;
            command->position = m_position;
            command->forward = m_forward;
            generate_up_vector3d(&command->forward, &command->up);
            bool following = metadata.following;
            float field_of_view;
            if (following)
                field_of_view = g_camera_globals.flying_camera_field_of_view * 0.017453292f;
            else
                field_of_view = authored_camera_suggested_field_of_view();
            command->field_of_view = field_of_view;
            if (m_sub_camera_following != following)
            {
                command->parameter_timers[4] = g_camera_globals.flying_camera_parameter_time;
                command->parameter_flags[4] = 1;
            }
            command->timer = (unit_index == -1) ? 0.033333335f : 0.0f;
            sub_camera_following = following;
            command->flags = 0x801;
        }
    }
    m_sub_camera_active = sub_camera_active;
    m_sub_camera_following = sub_camera_following;
}



// @address 0x2980f0 authored_camera_clear_target
void authored_camera_clear_target(c_authored_camera *camera, long object_index)
{
    if (camera->m_camera.get_target() == object_index)
    {
        camera->m_camera.set_target(-1);
    }
    if (camera->m_target == object_index)
    {
        camera->m_target = -1;
    }
}
