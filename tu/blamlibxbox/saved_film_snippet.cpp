// blamlibxbox/saved_film_snippet.obj: from the records 0x1e1c98 0x1e20f0 0x1e2b20 0x280130 0x280198 0x3bc810
// @flags /TP /O2 /Oc
// @address 0x8c42a0 ?g_camera_globals@@3Us_camera_globals@@A
// @address 0x3bc810 ?print@?$c_static_string@$0DA@@@QAAPBDPBDZZ
// @address 0x94e3b0 g_saved_film_timer_globals
// @address 0x808560 g_simulation_globals
// @address 0x11df70c g_director_globals
// @address 0x11df8cc ?g_observer_globals@@3PAUs_player_globals@@A
// @address 0x94e3ac g_saved_film_playback_tick
#include "saved_games/saved_film_work_types.h"
#include "memory/tls_globals.h"
#include "camera/camera_globals.h"
#include "game/game_globals_storage.h"
#include <stdio.h>
#include "math/real_math.h"
#include <stdarg.h>
#include "cache/cache_file_tag_resource_runtime.h"
#include "camera/observer.h"
#include "saved_games/saved_film_snippet.h"
#include "camera/director.h"
#include "saved_games/saved_film.h"
#include "simulation/simulation_world.h"
#include "camera/observer_director.h"
#include "simulation/simulation_globals.h"
#include "game/player_types.h"
#include "simulation/simulation_world_types.h"
#include "camera/camera_mode_types.h"
#include "game/player_mapping_types.h"
#include "saved_games/saved_film_globals.h"
#include "saved_games/saved_film_timing.h"
#include "saved_games/saved_film_recording_types.h"
#include "saved_games/saved_film_encoding.h"
#include "camera/camera_types.h"
#include "camera/observer_globals_types.h"
#include "saved_games/saved_film_snippet_internals.h"
#include "camera/saved_film_director_internals.h"
#include "saved_games/saved_film_tick.h"
#include "game/game_state_accessors.h"
#include "game/player_absolute_index.h"

// @address 0x1e1c98 saved_film_snippet_handle_message
extern "C" void saved_film_snippet_handle_message(long message_a, long message_b, const void *message_c, long message_d)
{
    if (g_saved_film_timer_globals.timer_limit != -1 && g_saved_film_recording_globals.state != 0)
        saved_film_encode_game_state(message_b, message_c, message_d);
    if (game_in_progress())
        simulation_world_update_views_with_message(g_simulation_globals.world, 0, message_a, message_b);
}

// @address 0x280130 saved_film_snippet_find_last_recorded_marker
extern "C" long saved_film_snippet_find_last_recorded_marker(s_saved_film_snippet_marker *markers)
{
    long last_recorded_index = -1;
    for (long marker_index = 0; marker_index < k_saved_film_snippet_marker_count; marker_index++)
    {
        if (markers[marker_index].recorded)
            last_recorded_index = marker_index;
    }
    return last_recorded_index;
}

// @address 0x280198 saved_film_snippet_find_best_marker_for_tick
extern "C" long saved_film_snippet_find_best_marker_for_tick(s_saved_film_snippet_marker *markers, long tick, bool seekable_only)
{
    long best_index = -1;
    long best_tick = 0;
    for (long marker_index = 0; marker_index < k_saved_film_snippet_marker_count; marker_index++)
    {
        s_saved_film_snippet_marker *marker = &markers[marker_index];
        if (marker->valid && marker->tick <= tick &&
            (best_index == -1 || tick - marker->tick < tick - best_tick) &&
            (!seekable_only || marker->seekable))
        {
            best_index = marker_index;
            best_tick = marker->tick;
        }
    }
    return best_index;
}

template<long k_maximum_count>
const char *c_static_string<k_maximum_count>::print(const char *format, ...)
{
    va_list args;
    va_start(args, format);
    _vsnprintf(m_string, k_maximum_count - 1, format, args);
    m_string[k_maximum_count - 1] = 0;
    return m_string;
}
template const char *c_static_string<48>::print(const char *format, ...);

static __forceinline bool saved_film_snippet_user_is_active(long user_index)
{
    bool result = false;
    if (user_index != -1)
        result = tls_player_mapping_globals()->player_indices[user_index] != -1;
    return result;
}

static __forceinline c_saved_film_director *saved_film_snippet_get_director(long user_index)
{
    s_controllers *globals = g_director_globals;
    s_director_storage *storage = globals->directors + user_index;
    return static_cast<c_saved_film_director *>(storage->get_director());
}

// @address 0x1e20f0 saved_film_snippet_set_cameras_from_states
extern "C" void saved_film_snippet_set_cameras_from_states(s_saved_film_snippet_camera *states)
{
    int lock = ::tag_resources_lock_game();
    for (long user_index = 0; user_index < 4; ++user_index)
    {
        s_saved_film_snippet_player_state *saved_player = &states[user_index].player;
        if (saved_film_snippet_user_is_active(user_index) &&
            saved_film_snippet_get_director(user_index)->get_type() == 1 && saved_player->valid)
        {
            c_saved_film_director *director = saved_film_snippet_get_director(user_index);
            s_saved_film_snippet_camera *state = &states[user_index];
            observer_clear((s_observer *)(&g_observer_globals->blocks[user_index]));
            director->set_camera_mode(state->mode, 0.0f, 1);
            long camera_index = state->camera_index;
            director->watched_player_index = -1;
            director->m_camera_mode = camera_index;
            director->get_camera()->set_target(-1);
            c_camera *camera = director->get_camera();
            long absolute_index = saved_player->absolute_index;
            long player_index;
            player_datum *player = 0;
            if (absolute_index != -1)
                player = (player_datum *)datum_try_and_get_absolute(tls_player_data(), absolute_index);
            if (player)
                player_index = player_index_from_absolute_player_index(absolute_index);
            if (player)
            {
                if (!(player->flags & 2))
                {
                    director_set_watched_player(director, player_index);
                    if (!observer_director_is_free_camera(director))
                    {
                        long unit_index = player->unit_index;
                        if (unit_index == -1)
                            unit_index = player->dead_unit_index;
                        camera->set_target(unit_index);
                    }
                }
            }
            camera->set_position(&state->position);
            camera->set_forward(&state->forward);
        }
    }
    ::tag_resources_unlock_game((*(long *)(&lock)));
}

// @address 0x1e2b20 ?saved_film_snippet_finished_seek@@YAXPAX@Z
void saved_film_snippet_finished_seek(void *finished_flag)
{
    long tick = g_saved_film_playback_tick;
    if (g_saved_film_timer_globals.timer_limit != -1 && g_saved_film_timer_globals.timer_limit <= tick)
    {
        long marker_index = saved_film_snippet_find_best_marker_for_tick(g_saved_film_work_globals.markers, g_saved_film_timer_globals.timer_limit, false);
        if (marker_index != -1)
            saved_film_load_game_state(marker_index);

        if (saved_film_is_recording())
        {
            if (g_saved_film_recording_globals.state == 5)
            {
                g_saved_film_recording_globals.state = 6;
                saved_film_update_playback_speed(g_saved_film_playback_tick);
            }
            else
            {
                if (g_saved_film_recording_globals.state == 15 &&
                    g_saved_film_playback_tick == g_saved_film_recording_globals.first_tick)
                {
                    saved_film_snippet_set_cameras_from_states(g_saved_film_recording_globals.camera_states);
                    saved_film_reset_recording_state();
                    *(unsigned char *)finished_flag = 1;
                }
                saved_film_update_playback_speed(g_saved_film_playback_tick);
            }
        }
        else
        {
            saved_film_update_playback_speed(g_saved_film_playback_tick);
        }
    }
    else
    {
        if (g_saved_film_timer_globals.timer_pending != 0)
        {
            long pending_index = saved_film_find_seek_entry(g_saved_film_timer_globals.timer_pending);
            if (pending_index != -1)
                saved_film_load_game_state(pending_index);
        }
        else if (g_saved_film_timer_globals.timer_other != -1)
        {
            long other_index = g_saved_film_timer_globals.timer_other;
            if (other_index == 0)
            {
                g_saved_film_timer_globals.authored_camera_users.clear();
                other_index = g_saved_film_timer_globals.timer_other;
            }

            bool active = false;
            if (g_saved_film_work_globals.markers[other_index].valid)
                active = saved_film_load_game_state(other_index);
            if (!active && !g_saved_film_timer_globals.request_pending)
            {
                g_saved_film_timer_globals.request_pending = true;
                g_saved_film_timer_globals.requested_state = 13;
            }
        }
    }

    for (long user_index = 0; user_index < 4; ++user_index)
    {
        if (saved_film_snippet_user_is_active(user_index) &&
            g_director_globals->directors[user_index].get_director()->get_type() == 1)
        {
            s_controllers *globals = g_director_globals;
            static_cast<c_saved_film_director *>(globals->directors[user_index].get_director())->m_adjusting_playback_speed = false;
            float value = -g_camera_globals.saved_film_revert_transition_time;
            globals->transition = value;
            globals->transition_remaining = value;
        }
    }
    g_saved_film_timer_globals.timer_pending = 0;
    g_saved_film_timer_globals.timer_other = -1;
}
