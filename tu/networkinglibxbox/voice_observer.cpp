// networkinglibxbox/voice_observer.obj: from the records 0x41f8e0 0x41fa38 0x41fb28 0x41fdf8 0x41ffc0 0x420078 0x4202b0 0x420500 0x420658 0x420868 0x420988 0x420b88 0x420db8 0x420e78 0x421000 0x4216b8 0x421840 0x4219d8 0x421db0 0x421e88 0x422070 0x4225d8 0x422770 0x4229b0 0x422a88 0x422af8 0x422d00
#include "game/game_engine_waypoint_constants.h"
#include <xtl.h>
#include "memory/data_types.h"
#include "memory/tls_globals.h"
#include <xaudio.h>
#include "cseries/crt_xdk.h"
#include "math/matrix_math_types.h"
#include "math/real_math.h"
#include "game/game_globals_storage.h"
#include "game/game_state_accessors.h"
#include "game/game_time_access.h"
#include "tag_files/tag_types.h"
#include "game/game_time_types.h"
#include "shell/shell.h"
#include "game/game_types.h"
#include "game/player_types.h"
#include "game/game_globals.h"
#include "memory/static_arrays.h"
#include "cseries/cseries_wrapped.h"
#include "game/game.h"
#include "game/game_engine.h"
#include "objects/objects.h"
#include "sound/game_sound.h"
#include "units/units.h"
#include "units/unit_types.h"
#include "game/game_engine_team.h"
#include "networking/online/voice_xenon.h"
#include "networking/voice/voice_xenon_services.h"
#include "game/game_engine_sandbox.h"
#include "simulation/simulation.h"
#include "networking/online/voice_observer.h"
#include "math/matrix_math.h"
#include "input/input.h"
#include "input/input_xenon.h"
#include "game/game_engine_settings_definitions.h"
#include "networking/online/voice_data_types.h"
#include "networking/online/voice_speaker_volume_types.h"
#include "networking/online/voice_destination_types.h"
#include "cache/cache_files.h"
#include "input/input_abstraction.h"
#include "sound/sound_manager.h"
#include "networking/online/voice_interface_types.h"
#include "scenario/scenario.h"
#include "interface/user_interface_controller.h"
#include "camera/director.h"
#include "sound/sound_math.h"
#include "networking/online/voice_player.h"
#include <stdlib.h>
#include <math.h>
#include <ppcintrinsics.h>
#include "networking/online/voice_observer_types.h"
#include "networking/network_time_types.h"
#include "networking/network_time_access.h"
#include "tag_files/tag_groups.h"
#include "networking/network_configuration_types.h"
#include "networking/network_configuration.h"
#include "networking/online/voice_engine_xenon.h"
#include "networking/online/voice_observer_dependencies.h"
#include "sound/sound_manager_types.h"
#include "game/multiplayer_runtime_globals.h"
#include "sound/game_sound_impulse.h"
#include "input/input_abstraction_types.h"
#include "sound/game_sound_types.h"
#include "sound/game_looping_sound_data.h"
#include "interface/window_manager_network_api.h"
#include "networking/online/voice_observer_constants.h"
// @flags /TP /O2 /Oc
// @address 0x41ffc0 voice_observer_is_push_to_talk_active
// @address 0x41ec00 ?add_packet@c_player_voice_queue@@QAAXJPBXJPIBUs_voice_route@@PBUs_voice_packet_header@@@Z
// @address 0x421db0 ?get_proximity_voice_distance@c_voice_observer@@AAAMJJ@Z
// @address 0x421e88 ?get_voice_actual_destination@c_voice_observer@@AAA?AW4e_voice_destination@@JJW42@@Z
// @address 0x3c04b0 ?voice_get_mute@@YA_NJJ@Z
// @address 0x3c0b90 ?voice_get_player_controller_index@@YA?AW4e_controller_index@@J@Z
// @address 0x4229b0 ?player_is_alive_or_wounded@c_voice_observer@@CA_NJ@Z
// @address 0x422a88 ?player_ever_alive@c_voice_observer@@CA_NJ@Z
// @address 0x422d00 ?player_get_unit_body_position@c_voice_observer@@CA_NJPATreal_point3d@@_N@Z
// @address 0x422af8 voice_observer_get_team_chat_mask
// @address 0x809e78 ?g_voice_interface_globals@@3Us_voice_interface_globals@@A
// @address 0x8087a8 ?g_network_configuration@@3Us_network_configuration@@A
// @address 0x10909f0 ?g_voice_observer@@3Vc_voice_observer@@A
// @address 0xac748 ?object_try_and_get_and_verify_type@@YAPAXJK@Z
// @address 0xadf48 ?unit_get_body_position@@YA_NJPATreal_point3d@@@Z
// @address 0x957f0 ?voice_enforcing_team_restrictions@@YA_NXZ
// @address 0x3c0f10 ?voice_lifecycle_out_of_game@@YA_NXZ
// @address 0x3c0748 ?voice_players_are_allies@@YA_NJJ@Z
// @address 0x3c0648 ?voice_players_on_same_team@@YA_NJJ@Z

struct IXAudioVoice;


class c_xenon_voice_engine;

struct s_voice_speaker_volume;
struct s_remote_player_voice_destination;

// @address 0x8c447c g_network_time_globals

struct s_remote_player_voice_destination {
    long local_player_index;
    e_voice_destination destination;
    float distance;
};


// @address 0x80608 GetTickCount


// @address 0x41f8e0 ?initialize@c_voice_observer@@QAAXXZ
void c_voice_observer::initialize()
{
    // match: 0x41f8e0: value range assumption guides branch lowering
    __assume(this == &g_voice_observer);
    m_player_proximity_mask.set_all(0);
    m_player_push_to_talk_mask.set_all(0);
    m_player_in_game_mask.set_all(0);
    m_player_incoming_voice_to_speakers_mask.set_all(0);
    m_player_incoming_voice_to_headset_mask.set_all(0);
    m_player_push_to_talk_start_time.set_all(0);
    m_player_push_to_talk_last_time.set_all(0);
    m_player_death_gurgle_mask.set_all(0);
    m_player_push_to_talk_sound_index.set_all(-1);
    m_player_mute_mask.set_all(0);
    m_player_voice_flags.set_all(0);
    m_incoming_voice_mask = 0;
    m_voice_repeater_mask = 0;
    m_initialized = true;
}

// @address 0x41ffc0 voice_observer_is_push_to_talk_active
extern "C" bool voice_observer_is_push_to_talk_active(void *observer, long player_index)
{
    bool allowed = false;
    if (g_voice_observer.m_initialized) {
        unsigned long last_time = g_voice_observer.m_player_push_to_talk_start_time[player_index];
        allowed = last_time != 0 &&
            (float)(long)(network_time_get() - last_time) < g_network_configuration.push_to_talk_timeout * 1000.0f;
    }
    return allowed;
}
// @address 0x3c04b0 ?voice_get_mute@@YA_NJJ@Z
// @address 0x3c0b90 ?voice_get_player_controller_index@@YA?AW4e_controller_index@@J@Z
bool c_voice_observer::players_can_team_chat(long local_player_index, long remote_player_index)
{
    bool can_chat = false;
    if (voice_player_is_valid(local_player_index) && voice_player_is_valid(remote_player_index) &&
        voice_player_is_local(local_player_index) && !voice_player_is_local(remote_player_index)) {
        if (voice_enforcing_team_restrictions())
            can_chat = voice_players_are_allies(local_player_index, remote_player_index);
        else
            can_chat = true;
    }
    return can_chat;
}
// @address 0x41fdf8 ?get_player_out_of_game_mask@c_voice_observer@@QAAKJ@Z
unsigned long c_voice_observer::get_player_out_of_game_mask(long player_index)
{
    // match: 0x41fdf8: value range assumption guides branch lowering
    __assume(this == &g_voice_observer);
    unsigned long mask = 0;
    if (m_initialized) {
        bool visible = false;
        long lifecycle = g_voice_interface_globals.lifecycle;
        if (g_voice_interface_globals.initialized != 0) {
            switch (lifecycle) {
            case _voice_lifecycle_out_of_game:
            case _voice_lifecycle_out_of_game_pending:
                visible = true;
                break;
            }
        }
        if (visible) {
            if (g_voice_interface_globals.initialized != 0 && lifecycle == 4) {
                if (voice_player_is_valid(player_index)) {
                    for (long index = 0; index < 16; ++index) {
                        if (index != player_index && voice_player_is_valid(index) &&
                            voice_players_on_same_team(player_index, index))
                            mask |= 1 << index;
                    }
                }
            } else {
                for (long index = 0; index < 16; ++index) {
                    if (voice_player_is_valid(index))
                        mask |= 1 << index;
                }
            }
        }
    }
    return mask;
}
// @address 0x3c04b0 ?voice_get_mute@@YA_NJJ@Z
// @address 0x420500 ?update_player_team_masks@c_voice_observer@@AAAXXZ
void c_voice_observer::update_player_team_masks()
{
    // match: 0x420500: value range assumption guides branch lowering
    __assume(this == &g_voice_observer);
    m_player_team_mask.set_all(0);
    for (long local_player_index = 0; local_player_index < 16; ++local_player_index) {
        if (voice_player_is_valid(local_player_index)) {
            for (long remote_player_index = 0; remote_player_index < 16; ++remote_player_index) {
                if (local_player_index != remote_player_index && voice_player_is_valid(remote_player_index)) {
                    if (((unsigned char)::voice_lifecycle_in_game())) {
                        if (voice_player_is_valid_in_game(local_player_index) && voice_player_is_valid_in_game(remote_player_index) &&
                            voice_players_are_allies(local_player_index, remote_player_index))
                            m_player_team_mask[local_player_index] |= 1 << remote_player_index;
                    } else if (voice_lifecycle_out_of_game()) {
                        if (voice_players_on_same_team(local_player_index, remote_player_index))
                            m_player_team_mask[local_player_index] |= 1 << remote_player_index;
                    }
                }
            }
        }
    }
}

// @address 0x420658 ?update_local_player_proximity_masks@c_voice_observer@@AAAXXZ
void c_voice_observer::update_local_player_proximity_masks()
{
    // match: 0x420658: value range assumption guides branch lowering
    __assume(this == &g_voice_observer);
    m_player_proximity_mask.set_all(0);
    if (game_in_progress() && tls_game_globals()->options.game_mode == 2) {
        for (long local_player_index = 0; local_player_index < 16; ++local_player_index) {
            real_point3d local_position;
            if (voice_player_is_local(local_player_index) && voice_player_is_valid_in_game(local_player_index) &&
                player_get_unit_body_position(local_player_index, &local_position, true)) {
                for (long remote_player_index = 0; remote_player_index < 16; ++remote_player_index) {
                    real_point3d remote_position;
                    if (voice_player_is_valid(remote_player_index) && !voice_player_is_local(remote_player_index) &&
                        voice_player_is_valid_in_game(remote_player_index) &&
                        player_get_unit_body_position(remote_player_index, &remote_position, true)) {
                        float dx = remote_position.x - local_position.x;
                        float dy = remote_position.y - local_position.y;
                        float dz = remote_position.z - local_position.z;
                        if (sqrtf(dy * dy + (dz * dz + dx * dx)) <= 9.5f)
                            m_player_proximity_mask[local_player_index] |= 1 << remote_player_index;
                    }
                }
            }
        }
    }
}
// @address 0x421db0 ?get_proximity_voice_distance@c_voice_observer@@AAAMJJ@Z
float c_voice_observer::get_proximity_voice_distance(long local_player_index, long remote_player_index)
{
    real_point3d local_position = {0.0f};
    real_point3d remote_position = {0.0f};
    float distance = -1.0f;
    if (player_get_unit_body_position(local_player_index, &local_position, true) &&
        player_get_unit_body_position(remote_player_index, &remote_position, false))
        distance = distance3d(&local_position, &remote_position);
    return distance;
}
static __forceinline bool voice_observer_player_has_voice(long player_index)
{
    bool has_voice = false;
    if (g_voice_interface_globals.initialized)
        has_voice = g_voice_interface_globals.player_states[player_index].status_flags.test(_voice_player_has_voice);
    return has_voice;
}
static __forceinline bool voice_observer_player_is_muted(long source_player_index, long destination_player_index)
{
    bool muted = false;
    if (g_voice_interface_globals.initialized && voice_player_is_valid(source_player_index))
        muted = (g_voice_interface_globals.player_states[source_player_index].muted_players & (1 << destination_player_index)) != 0;
    return muted;
}
static __forceinline long voice_observer_controller_output_preference(long controller_index)
{
    long output_preference = 0;
    if (g_voice_interface_globals.initialized)
        output_preference = g_voice_interface_globals.controller_states[controller_index].output_preference;
    return output_preference;
}
static __forceinline long voice_observer_controller_input_preference(long controller_index)
{
    long input_preference = 0;
    if (g_voice_interface_globals.initialized)
        input_preference = g_voice_interface_globals.controller_states[controller_index].input_preference;
    return input_preference;
}
// @address 0x421e88 ?get_voice_actual_destination@c_voice_observer@@AAA?AW4e_voice_destination@@JJW42@@Z
e_voice_destination c_voice_observer::get_voice_actual_destination(long local_player_index, long remote_player_index, e_voice_destination desired_destination)
{
    e_voice_destination actual_destination = _voice_destination_none;
    if (!voice_get_mute(local_player_index, remote_player_index) && !voice_get_mute(remote_player_index, local_player_index)) {
        switch (desired_destination) {
        case _voice_destination_none:
            actual_destination = _voice_destination_none;
            break;
        case _voice_destination_voice_only:
            if (voice_observer_player_has_voice(local_player_index))
                actual_destination = _voice_destination_voice_only;
            else
                actual_destination = _voice_destination_headset;
            break;
        case _voice_destination_headset:
            actual_destination = _voice_destination_headset;
            break;
        case _voice_destination_everyone:
            actual_destination = _voice_destination_everyone;
            break;
        }
        if (actual_destination != _voice_destination_none) {
            e_controller_index controller_index = voice_get_player_controller_index(local_player_index);
            if (controller_index == _controller_none)
                actual_destination = _voice_destination_none;
            else if (voice_observer_controller_output_preference(controller_index) == 1)
                actual_destination = _voice_destination_none;
            else if (voice_observer_controller_input_preference(controller_index) == 1)
                actual_destination = _voice_destination_voice_only;
        }
    } else {
        actual_destination = _voice_destination_none;
    }
    if (actual_destination == _voice_destination_headset || actual_destination == _voice_destination_everyone) {
        for (long player_index = 0; player_index < 16; ++player_index) {
            if (voice_player_is_local(player_index) && voice_observer_player_is_muted(player_index, remote_player_index)) {
                actual_destination = _voice_destination_none;
                break;
            }
        }
    }
    return actual_destination;
}
// @address 0x4225d8 ?set_voice_destination_headset@c_voice_observer@@AAAXKJ@Z
void c_voice_observer::set_voice_destination_headset(unsigned long player_mask, long local_user_index)
{
    int remote_user_mask;
    remote_user_mask = 0;
    int player_index;
    player_index = 0;
    // match: 0x4225d8: retain the accepted controller-field cursor; a record-base cursor changes base materialization order.
    char *player_voice_data;
    player_voice_data = (char *)&g_voice_interface_globals.player_states[0].controller_index;
    unsigned char voice_initialized;
    voice_initialized = g_voice_interface_globals.initialized;
    char *destination_mask_address;
    int local_user_bit;
    local_user_bit = 1 << local_user_index;
    int remote_user_index_or_bit;
#ifdef HALO3_64BIT
    for (player_index = 0; player_index < 16; ++player_index) {
        remote_user_index_or_bit = -1;
        if (g_voice_interface_globals.initialized != 0 &&
            (g_voice_interface_globals.player_states[player_index].status_flags.m_storage & 2) != 0) {
            remote_user_index_or_bit = g_voice_interface_globals.player_states[player_index].controller_index;
        }
        if (((1u << player_index) & player_mask) != 0) {
            g_voice_observer.m_player_incoming_voice_to_headset_mask[player_index] |= local_user_bit;
            if (remote_user_index_or_bit != -1) {
                remote_user_mask |= 1 << remote_user_index_or_bit;
            }
        } else {
            g_voice_observer.m_player_incoming_voice_to_headset_mask[player_index] &= ~local_user_bit;
        }
    }
#else
    destination_mask_address = ((char *)&g_voice_observer + 388);
    do {
        remote_user_index_or_bit = -1;
        if ((g_voice_interface_globals.initialized) != 0 &&
            (unsigned int)(((s_voice_player_state *)(player_voice_data - offsetof(s_voice_player_state, controller_index)))->status_flags.m_storage & 2) != 0) {
            remote_user_index_or_bit = *(e_controller_index *)player_voice_data;
        }
        if ((unsigned int)(1 << player_index & player_mask) != 0) {
            *(int *)destination_mask_address = local_user_bit | *(int *)destination_mask_address;
            if (remote_user_index_or_bit != -1) {
                remote_user_mask = 1 << remote_user_index_or_bit | remote_user_mask;
            }
        } else {
            *(int *)destination_mask_address = *(int *)destination_mask_address & ~local_user_bit;
        }
        destination_mask_address = destination_mask_address + 4;
        player_index = player_index + 1;
        player_voice_data += sizeof(s_voice_player_state);
    } while ((ptrdiff_t)destination_mask_address < (ptrdiff_t)((char *)&g_voice_observer + 452));
#endif
    int remote_user_index;
    remote_user_index = 0;
    __int64 local_user_xuid_prefix;
    local_user_xuid_prefix = (__int64)1601693055 << 33;
    do {
        remote_user_index_or_bit = 1 << remote_user_index;
        if ((unsigned int)(remote_user_index_or_bit & remote_user_mask) != 0) {
            if (g_voice_interface_globals.initialized != 0) {
                destination_mask_address = (char *)&g_voice_interface_globals.player_states[local_user_index];
                ((s_voice_player_state *)destination_mask_address)->voice_mask =
                    ((s_voice_player_state *)destination_mask_address)->voice_mask | remote_user_index_or_bit;
                if (g_voice_engine.initialized != 0) {
                    g_voice_engine.engine->SetPlaybackPriority((__int64)local_user_index | local_user_xuid_prefix, remote_user_index,
                        0UL);
                }
            }
        } else if (g_voice_interface_globals.initialized != 0) {
            destination_mask_address = (char *)&g_voice_interface_globals.player_states[local_user_index];
            ((s_voice_player_state *)destination_mask_address)->voice_mask =
                    ((s_voice_player_state *)destination_mask_address)->voice_mask & ~remote_user_index_or_bit;
            if (g_voice_engine.initialized != 0) {
                g_voice_engine.engine->SetPlaybackPriority((__int64)local_user_index | local_user_xuid_prefix, remote_user_index,
                    0xffffffffUL);
            }
        }
        int next_remote_user_index = -1;
        if ((unsigned int)remote_user_index <= 2) {
            next_remote_user_index = remote_user_index + 1;
        }
        remote_user_index = next_remote_user_index;
    } while (remote_user_index != -1);
    return;
}

namespace {

}
// match: 0x41f8e0: Compiler intrinsic: the SDK has no declaration, and an undeclared use fails syntax; keep the prototype and intrinsic pragma.

extern "C" {
static __forceinline void voice_observer_compiler_barrier()
{
    void _ReadWriteBarrier(void);
    _ReadWriteBarrier();
}
}

static __forceinline unsigned char voice_observer_player_is_valid_in_game(long player_index)
{
    unsigned char valid = voice_player_is_valid_in_game(player_index);
    voice_observer_compiler_barrier();
    return valid;
}
bool c_voice_observer::player_is_alive_or_wounded(long player_index)
{
    bool result = false;
    if (voice_player_is_valid_in_game(player_index)) {
        const player_datum *player = voice_player_try_and_get_datum(player_index);
        if (player) {
            if (player->unit_index != -1) {
                result = true;
            } else if (player->dead_unit_index != -1) {
                const unit_datum *unit = (const unit_datum *)object_try_and_get_and_verify_type(player->dead_unit_index, 0x1003);
                if (unit && game_ticks_to_seconds((float)(game_time_get() - unit->unit.last_active_time)) <= 1.0f)
                    result = true;
            }
        }
    }
    return result;
}
bool c_voice_observer::player_ever_alive(long player_index)
{
    bool result = false;
    if (voice_player_is_valid_in_game(player_index)) {
        const player_datum *player = voice_player_try_and_get_datum(player_index);
        if (player && (player->flags & 1) != 0 && (player->flags & 8) == 0)
            result = true;
    }
    return result;
}
bool c_voice_observer::player_get_unit_body_position(long player_index, real_point3d *position, bool use_alternate_unit)
{
    bool result = false;
    if (voice_player_is_valid_in_game(player_index)) {
        const player_datum *player = voice_player_try_and_get_datum(player_index);
        if (player) {
            long unit_index = -1;
            if (player->unit_index != -1) {
                unit_index = player->unit_index;
            } else if (use_alternate_unit && player->dead_unit_index != -1) {
                unit_index = player->dead_unit_index;
            }
            if (unit_index != -1) {
                unit_get_body_position(unit_index, position);
                result = true;
            }
        }
    }
    return result;
}
// @address 0x422af8 voice_observer_get_team_chat_mask
extern "C" unsigned long voice_observer_get_team_chat_mask(void *observer, long player_index)
{
    int team_chat_mask;
    team_chat_mask = 0;
    bool player_valid;
    player_valid = 0;
    game_globals_storage *game = tls_game_globals();
    int player_state;
    if (game != 0 && game->in_progress != 0 && game->initializing == 0 &&
        game->map_active != 0)
    {
        player_valid = 1;
    }
    if ((unsigned int)player_valid != 0) {
        player_state = game->options.game_mode;
        if (player_state == 2) {
#ifdef HALO3_64BIT
            for (int local_player_index = 0; local_player_index < 16; ++local_player_index) {
                bool local_flag = g_voice_interface_globals.initialized != 0 &&
                    g_voice_interface_globals.player_states[local_player_index].status_flags.test(_voice_player_local);
                if (local_flag && voice_player_is_valid_in_game(local_player_index)) {
                    bool has_voice = g_voice_interface_globals.initialized != 0 &&
                        g_voice_interface_globals.player_states[local_player_index].status_flags.test(_voice_player_has_voice);
                    if (has_voice) {
                        for (int remote_player_index = 0; remote_player_index < 16; ++remote_player_index) {
                            bool remote_in_session = g_voice_interface_globals.initialized != 0 &&
                                g_voice_interface_globals.player_states[remote_player_index].status_flags.test(_voice_player_in_session);
                            if (remote_in_session) {
                                bool remote_local = g_voice_interface_globals.initialized != 0 &&
                                    g_voice_interface_globals.player_states[remote_player_index].status_flags.test(_voice_player_local);
                                if (!remote_local && voice_player_is_valid_in_game(remote_player_index)) {
                                    if (g_voice_observer.players_can_team_chat(local_player_index, remote_player_index)) {
                                        team_chat_mask |= 1 << remote_player_index;
                                    } else {
                                        team_chat_mask &= ~(1 << remote_player_index);
                                    }
                                }
                            }
                        }
                    }
                }
            }
#else
            int local_player_index;
            local_player_index = 0;
            char *local_player_status;
            local_player_status = ((char *)&g_voice_interface_globals + 192);
            unsigned char can_chat;
            char *remote_player_status;
            int remote_player_index;
            do {
                player_state = 0;
                if (g_voice_interface_globals.initialized != 0) {
                    player_state = (*(unsigned char *)local_player_status >> 1) & 1;
                }
                if ((unsigned int)(player_state & 255) != 0 && voice_player_is_valid_in_game(local_player_index)) {
                    player_state = 0;
                    if (g_voice_interface_globals.initialized != 0) {
                        player_state = (*(unsigned char *)local_player_status >> 3) & 1;
                    }
                    if ((unsigned int)(player_state & 255) != 0) {
                        remote_player_index = 0;
                        remote_player_status = ((char *)&g_voice_interface_globals + 192);
                        do {
                            player_state = g_voice_interface_globals.initialized;
                            player_valid = 0;
                            if ((unsigned int)player_state != 0) {
                                player_valid = *(unsigned char *)remote_player_status & 1;
                            }
                            if ((unsigned int)player_valid != 0) {
                                player_valid = 0;
                                if ((unsigned int)player_state != 0) {
                                    player_valid = (*(unsigned char *)remote_player_status >> 1) & 1;
                                }
                                if ((unsigned int)player_valid == 0 && voice_player_is_valid_in_game(remote_player_index)) {
                                    can_chat = ((unsigned char)g_voice_observer.players_can_team_chat(local_player_index, remote_player_index));
                                    if (can_chat != 0) {
                                        team_chat_mask |= 1 << remote_player_index;
                                    } else {
                                        team_chat_mask &= ~(1 << remote_player_index);
                                    }
                                }
                            }
                            remote_player_status = remote_player_status + 36;
                            remote_player_index = remote_player_index + 1;
                        } while ((ptrdiff_t)remote_player_status < (ptrdiff_t)((char *)&g_voice_interface_globals + 768));
                    }
                }
                local_player_status = local_player_status + 36;
                local_player_index = local_player_index + 1;
            } while ((ptrdiff_t)local_player_status < (ptrdiff_t)((char *)&g_voice_interface_globals + 768));
#endif
        } else if (player_state == 1) {
            team_chat_mask = 0;
            if (g_voice_observer.m_initialized != 0) {
                team_chat_mask = g_voice_observer.m_voice_state[player_index];
            }
        }
    } else {
        team_chat_mask = 0;
        if (g_voice_observer.m_initialized != 0) {
            team_chat_mask = g_voice_observer.m_player_team_mask[player_index];
        }
    }
    return team_chat_mask;
}
// @address 0x420988 voice_observer_update_local_player_in_game_masks
extern "C" void voice_observer_update_local_player_in_game_masks(c_voice_observer *observer)
{
    game_globals_storage *active_game_globals;
    int interface_initialized;
    unsigned char can_team_chat;
    unsigned char game_underway;
    unsigned char player_is_local;
    int local_index;
    unsigned char player_has_voice;
    unsigned char is_voice_repeater;
    unsigned char remote_player_valid;
    unsigned char remote_player_local;
    int remote_index;
    char *mask_clear_cursor;
    unsigned int masks_remaining;
    char *in_game_mask_slot;
    char *local_player_status;
    char *remote_player_status;
#ifdef HALO3_64BIT
    g_voice_observer.m_player_in_game_mask.set_all(0);
    game_underway = 0;
    active_game_globals = tls_game_globals();
    if (active_game_globals && active_game_globals->in_progress &&
        !active_game_globals->initializing && active_game_globals->map_active)
        game_underway = 1;
    if (!game_underway || active_game_globals->options.game_mode != 2)
        return;
    for (local_index = 0; local_index < 16; ++local_index) {
        player_is_local = g_voice_interface_globals.initialized != 0 &&
            g_voice_interface_globals.player_states[local_index].status_flags.test(_voice_player_local);
        if (player_is_local && voice_player_is_valid_in_game(local_index)) {
            player_has_voice = g_voice_interface_globals.initialized != 0 &&
                g_voice_interface_globals.player_states[local_index].status_flags.test(_voice_player_has_voice);
            if (player_has_voice) {
                is_voice_repeater = g_voice_observer.m_initialized != 0 &&
                    ((1 << local_index) & g_voice_observer.m_voice_repeater_mask) != 0;
                if (is_voice_repeater) {
                    for (remote_index = 0; remote_index < 16; ++remote_index) {
                        interface_initialized = g_voice_interface_globals.initialized;
                        remote_player_valid = interface_initialized != 0 &&
                            g_voice_interface_globals.player_states[remote_index].status_flags.test(_voice_player_in_session);
                        if (remote_player_valid) {
                            remote_player_local = interface_initialized != 0 &&
                                g_voice_interface_globals.player_states[remote_index].status_flags.test(_voice_player_local);
                            if (!remote_player_local && voice_player_is_valid_in_game(remote_index)) {
                                can_team_chat = (unsigned char)g_voice_observer.players_can_team_chat(local_index, remote_index);
                                if (can_team_chat != 0) {
                                    g_voice_observer.m_player_in_game_mask[local_index] |= 1 << remote_index;
                                } else {
                                    g_voice_observer.m_player_in_game_mask[local_index] &= ~(1 << remote_index);
                                }
                            }
                        }
                    }
                }
            }
        }
    }
#else
    mask_clear_cursor = ((char *)&g_voice_observer + 132);
    masks_remaining = 16;
    do {
        *(int *)(mask_clear_cursor) = 0;
        mask_clear_cursor = (mask_clear_cursor + 4);
        masks_remaining = (masks_remaining - 1);
    } while (masks_remaining != 0);
    game_underway = 0;
    active_game_globals = tls_game_globals();
    if (active_game_globals && active_game_globals->in_progress &&
        !active_game_globals->initializing && active_game_globals->map_active)
        game_underway = 1;
    if (!game_underway || active_game_globals->options.game_mode != 2)
        return;
    local_index = 0;
    in_game_mask_slot = ((char *)&g_voice_observer + 132);
    local_player_status = ((char *)&g_voice_interface_globals + 192);
    do {
        player_is_local = 0;
        if (!(((unsigned int)g_voice_interface_globals.initialized == 0))) {
            player_is_local = (_rotl(*(unsigned char *)(local_player_status), 31) & 1);
        }
        if (player_is_local && voice_player_is_valid_in_game(local_index)) {
            player_has_voice = 0;
            if (!(((unsigned int)g_voice_interface_globals.initialized == 0))) {
                player_has_voice = (_rotl(*(unsigned char *)(local_player_status), 29) & 1);
            }
            if (player_has_voice) {
                is_voice_repeater = 0;
                if (!(((unsigned int)g_voice_observer.m_initialized == 0))) {
                    is_voice_repeater = (((1 << local_index) & g_voice_observer.m_voice_repeater_mask) != 0);
                }
                if (is_voice_repeater) {
                    remote_index = 0;
                    remote_player_status = ((char *)&g_voice_interface_globals + 192);
                    do {
                        interface_initialized = g_voice_interface_globals.initialized;
                        remote_player_valid = 0;
                        if (!(((unsigned int)interface_initialized == 0))) {
                            remote_player_valid = (*(unsigned char *)(remote_player_status) & 1);
                        }
                        if (!((((unsigned char)remote_player_valid) == 0))) {
                            remote_player_local = 0;
                            if (!(((unsigned int)interface_initialized == 0))) {
                                remote_player_local = (_rotl(*(unsigned char *)(remote_player_status), 31) & 1);
                            }
                            if (!remote_player_local && voice_player_is_valid_in_game(remote_index)) {
                                can_team_chat = ((unsigned char)g_voice_observer.players_can_team_chat(local_index, remote_index));
                                if (!((((unsigned char)can_team_chat) == 0))) {
                                    *(int *)(in_game_mask_slot) = ((1 << remote_index) | *(int *)(in_game_mask_slot));
                                } else {
                                    *(int *)(in_game_mask_slot) = (*(int *)(in_game_mask_slot) & (~(1 << remote_index)));
                                }
                            }
                        }
                        remote_player_status = (remote_player_status + 36);
                        remote_index = (remote_index + 1);
                    } while ((ptrdiff_t)remote_player_status < (ptrdiff_t)((char *)&g_voice_interface_globals + 768));
                }
            }
        }
        in_game_mask_slot = (in_game_mask_slot + 4);
        local_index = (local_index + 1);
        local_player_status = (local_player_status + 36);
    } while ((ptrdiff_t)in_game_mask_slot < (ptrdiff_t)((char *)&g_voice_observer + 196));
#endif
    return;
}

// @address 0x4216b8 ?update_local_player_dead_masks@c_voice_observer@@AAAXXZ
void c_voice_observer::update_local_player_dead_masks()
{
    // match: 0x4216b8: value range assumption guides branch lowering
    __assume(this == &g_voice_observer);
    m_voice_state.set_all(0);
    if (game_in_progress() && tls_game_globals()->options.game_mode == 1) {
        for (long local_player_index = 0; local_player_index < 16; ++local_player_index) {
            if (voice_player_is_local(local_player_index) && voice_player_is_valid_in_game(local_player_index)) {
                voice_observer_compiler_barrier();
                for (long remote_player_index = 0; remote_player_index < 16; ++remote_player_index) {
                    if (voice_player_is_valid(remote_player_index) && !voice_player_is_local(remote_player_index) &&
                        ((unsigned char (*)(long, s_voice_interface_globals *))voice_player_is_valid_in_game)(remote_player_index, &g_voice_interface_globals))
                        {
                        m_voice_state[local_player_index] |= 1 << remote_player_index;
                    }
                    voice_observer_compiler_barrier();
                }
            }
            voice_observer_compiler_barrier();
        }
    }
}
static __forceinline e_game_team voice_observer_player_team(long player_index)
{
    e_game_team team = _game_team_none;
    if (g_voice_interface_globals.initialized && g_voice_interface_globals.player_states[player_index].status_flags.test(_voice_player_in_session))
        team = g_voice_interface_globals.player_states[player_index].team_index;
    return team;
}
static __forceinline bool voice_observer_matchmaking_in_progress()
{
    bool in_progress = false;
    if (g_voice_interface_globals.initialized)
        in_progress = g_voice_interface_globals.matchmaking_state == 2;
    return in_progress;
}
static __forceinline e_voice_lifecycle voice_observer_get_lifecycle()
{
    e_voice_lifecycle lifecycle = _voice_lifecycle_none;
    if (g_voice_interface_globals.initialized)
        lifecycle = g_voice_interface_globals.lifecycle;
    return lifecycle;
}
static __forceinline bool voice_observer_lifecycle_in_game()
{
    bool in_game = false;
    if (g_voice_interface_globals.initialized) {
        e_voice_lifecycle lifecycle = voice_observer_get_lifecycle();
        if (lifecycle > _voice_lifecycle_none && lifecycle <= _voice_lifecycle_in_game_pending)
            in_game = true;
    }
    return in_game;
}
static __forceinline bool voice_observer_player_is_voice_repeater(long player_index)
{
    bool repeater = false;
    if (g_voice_observer.m_initialized)
        repeater = ((1 << player_index) & g_voice_observer.m_voice_repeater_mask) != 0;
    return repeater;
}
// @address 0x41fb28 ?get_player_in_game_mask@c_voice_observer@@QAAKJ@Z
unsigned long c_voice_observer::get_player_in_game_mask(long player_index)
{
    // match: 0x41fb28: value range assumption guides branch lowering
    __assume(this == &g_voice_observer);
    unsigned long mask = 0;
    if (m_initialized && voice_observer_lifecycle_in_game()) {
        if (game_is_cooperative()) {
            for (long index = 0; index < 16; ++index) {
                if (voice_player_is_valid(index))
                    mask |= 1 << index;
            }
        } else if (voice_observer_player_is_valid_in_game(player_index) && voice_player_try_and_get_datum(player_index)) {
            if (simulation_is_pending_start()) {
                mask = voice_observer_get_team_chat_mask(this, player_index);
            } else if (tls_game_globals()->options.game_mode == 1) {
                mask = get_player_voice_state_mask(player_index);
            } else if (tls_game_globals()->options.game_mode == 2) {
                bool push_to_talk = voice_observer_is_push_to_talk_active(this, player_index);
                bool repeater = player_is_voice_repeater(player_index);
                if (repeater)
                    mask = get_player_in_game_mask_internal(player_index);
                else
                    mask = get_player_push_to_talk_mask(player_index);
                bool (*player_alive)(long) = player_is_alive_or_wounded;
                if (player_alive(player_index) && (repeater || push_to_talk))
                    mask |= get_player_proximity_mask(player_index);
            }
            if (voice_observer_get_lifecycle() == _voice_lifecycle_in_game_pending)
                mask &= get_player_team_mask(player_index);
        }
    }
    return mask;
}
// @address 0x4202b0 ?update_local_player_death_gurgles@c_voice_observer@@AAAXXZ
void c_voice_observer::update_local_player_death_gurgles()
{
    // match: 0x4202b0: value range assumption guides branch lowering
    __assume(this == &g_voice_observer);
    if (game_in_progress() && tls_game_globals()->options.game_mode == 2) {
        for (long player_index = 0; player_index < 16; ++player_index) {
            if (voice_player_is_valid(player_index) && !voice_player_is_local(player_index) &&
                voice_player_is_valid_in_game(player_index) && player_ever_alive(player_index) &&
                !player_is_alive_or_wounded(player_index)) {
                if ((float)network_time_since_inline(m_player_death_gurgle_mask[player_index]) < 300.0f) {
                    for (long local_player_index = 0; local_player_index < 16; ++local_player_index) {
                        if (voice_player_is_valid(local_player_index) && voice_player_is_local(local_player_index) &&
                            voice_observer_player_is_valid_in_game(local_player_index) &&
                            players_can_team_chat(local_player_index, player_index) &&
                            !voice_get_mute(local_player_index, player_index) && !voice_get_mute(player_index, local_player_index))
                            voice_observer_new_multiplayer_sound(this, local_player_index, 0);
                    }
                }
                m_player_death_gurgle_mask[player_index] = 0;
            }
        }
    }
}
// @address 0x420b88 ?update_local_player_push_to_talk_masks@c_voice_observer@@AAAXXZ
void c_voice_observer::update_local_player_push_to_talk_masks()
{
    // match: 0x420b88: value range assumption guides branch lowering
    __assume(this == &g_voice_observer);
    m_player_push_to_talk_mask.set_all(0);
    if (game_in_progress() && tls_game_globals()->options.game_mode == 2) {
        for (long local_player_index = 0; local_player_index < 16; ++local_player_index) {
            if (voice_player_is_local(local_player_index) && voice_observer_player_is_valid_in_game(local_player_index) &&
                voice_observer_player_has_voice(local_player_index) && !voice_observer_player_is_voice_repeater(local_player_index)) {
                e_controller_index controller_index = voice_get_player_controller_index(local_player_index);
                update_local_player_push_to_talk_timers(local_player_index, controller_index);
                if (voice_observer_is_push_to_talk_active(this, local_player_index)) {
                    for (long remote_player_index = 0; remote_player_index < 16; ++remote_player_index) {
                        if (voice_player_is_valid(remote_player_index) && !voice_player_is_local(remote_player_index) &&
                            voice_observer_player_is_valid_in_game(remote_player_index)) {
                            if (players_can_team_chat(local_player_index, remote_player_index))
                                m_player_push_to_talk_mask[local_player_index] |= 1 << remote_player_index;
                            else
                                m_player_push_to_talk_mask[local_player_index] &= ~(1 << remote_player_index);
                        }
                    }
                }
            }
        }
    }
}
// @address 0x4219d8 ?update_voice_repeater_mask@c_voice_observer@@AAAXXZ
void c_voice_observer::update_voice_repeater_mask()
{
    // match: 0x4219d8: value range assumption guides branch lowering
    __assume(this == &g_voice_observer);
    m_voice_repeater_mask = 0;
    if (game_is_cooperative()) {
        for (long player_index = 0; player_index < 16; ++player_index) {
            if (voice_player_is_local(player_index))
                m_voice_repeater_mask |= 1 << player_index;
        }
    } else if (!game_engine_has_teams()) {
        long player_count = 0;
        for (long player_index = 0; player_index < 16; ++player_index) {
            if (voice_observer_player_is_valid_in_game(player_index))
                ++player_count;
        }
        bool within_limit = player_count <= g_network_configuration.maximum_voice_repeater_players;
        for (long player_index = 0; player_index < 16; ++player_index) {
            if (voice_player_is_local(player_index) && voice_observer_player_is_valid_in_game(player_index)) {
                if (within_limit)
                    m_voice_repeater_mask |= 1 << player_index;
                else
                    m_voice_repeater_mask &= ~(1 << player_index);
            }
        }
    } else {
        for (long player_index = 0; player_index < 16; ++player_index) {
            if (voice_player_is_local(player_index) && voice_observer_player_is_valid_in_game(player_index)) {
                e_game_team team = voice_observer_player_team(player_index);
                long team_count = 1;
                for (long other_index = 0; other_index < 16; ++other_index) {
                    if (other_index != player_index && voice_observer_player_is_valid_in_game(other_index) && voice_observer_player_team(other_index) == team)
                        ++team_count;
                }
                if (team_count <= g_network_configuration.maximum_voice_repeater_players)
                    m_voice_repeater_mask |= 1 << player_index;
            }
        }
    }
    if (voice_observer_matchmaking_in_progress()) {
        bool clear_mask = false;
        if (voice_observer_matchmaking_in_progress()) {
            if (voice_observer_lifecycle_in_game()) {
                s_voice_hopper_settings settings;
                if (voice_try_and_get_hopper_settings(&settings) && settings.voice_option_48)
                    clear_mask = true;
            }
        }
        if (clear_mask)
            m_voice_repeater_mask = 0;
    }
}
__forceinline e_voice_destination c_voice_observer::get_voice_desired_destination_in_game(
    long local_player_index, long remote_player_index, unsigned long voice_flags)
{
    e_voice_destination desired_destination = _voice_destination_none;
    unsigned long mask = 0;
    if (m_initialized)
        mask = m_player_proximity_mask[local_player_index];
    if ((voice_flags & 1) && (!voice_enforcing_team_restrictions() || voice_players_are_allies(local_player_index, remote_player_index)))
        desired_destination = _voice_destination_voice_only;
    else if (mask & (1 << remote_player_index))
        desired_destination = _voice_destination_everyone;
    return desired_destination;
}
// @address 0x420078 ?set_local_listeners_to_incoming_remote_player_voice@c_voice_observer@@QAAXJK@Z
void c_voice_observer::set_local_listeners_to_incoming_remote_player_voice(long remote_player_index, unsigned long voice_flags)
{
    // match: 0x420078: value range assumption guides branch lowering
    __assume(this == &g_voice_observer);
    if (m_initialized && voice_observer_lifecycle_in_game() && !game_is_cooperative()) {
        unsigned char remote_valid = voice_observer_player_is_valid_in_game(remote_player_index);
        s_remote_player_voice_destination destinations[16] = {0};
        for (long local_player_index = 0; local_player_index < 16; ++local_player_index) {
            e_voice_destination actual_destination = _voice_destination_none;
            if (voice_player_is_local(local_player_index) && voice_observer_player_is_valid_in_game(local_player_index) && remote_valid) {
                e_voice_destination desired_destination = get_voice_desired_destination_in_game(local_player_index, remote_player_index, voice_flags);
                e_voice_destination incoming_destination = get_voice_actual_destination(local_player_index, remote_player_index, desired_destination);
                if (desired_destination == _voice_destination_voice_only && player_is_alive_or_wounded(remote_player_index))
                    m_player_death_gurgle_mask[remote_player_index] = network_time_get();
                if (incoming_destination == _voice_destination_everyone)
                    destinations[local_player_index].distance = get_proximity_voice_distance(local_player_index, remote_player_index);
                actual_destination = incoming_destination;
                if (incoming_destination != _voice_destination_none)
                    m_incoming_voice_mask = (1 << remote_player_index) | m_incoming_voice_mask;
            }
            destinations[local_player_index].destination = actual_destination;
            destinations[local_player_index].local_player_index = local_player_index;
        }
        set_voice_destination(remote_player_index, destinations, 16);
    }
}
static __forceinline void voice_observer_build_speaker_volume(s_voice_speaker_volume *speaker_volume, float gain)
{
    speaker_volume->channels[0] = gain;
    speaker_volume->channels[1] = gain;
    speaker_volume->channels[2] = gain;
    speaker_volume->channels[3] = 0.0f;
    speaker_volume->channels[4] = gain;
    speaker_volume->channels[5] = gain;
}
static __forceinline void voice_observer_set_speaker_enabled(long player_index, bool enabled)
{
    if (g_voice_interface_globals.initialized) {
        g_voice_interface_globals.speaker_states[player_index].enabled = enabled;
        XAudioVoice_SetEffectState(g_voice_interface_globals.speaker_states[player_index].voice, 0, enabled);
    }
}
// @address 0x422070 ?set_voice_destination@c_voice_observer@@AAAXJPAUs_remote_player_voice_destination@@J@Z
void c_voice_observer::set_voice_destination(long remote_player_index, s_remote_player_voice_destination *destinations, long destination_count)
{
    // match: 0x422070: value range assumption guides branch lowering
    __assume(this == &g_voice_observer);
    bool headset_destination_found = false;
    long closest_destination_index = -1;
    unsigned long headset_player_mask = 0;
    for (long destination_index = 0; destination_index < destination_count; ++destination_index) {
        s_remote_player_voice_destination *destination = &destinations[destination_index];
        if (destination->destination == _voice_destination_voice_only)
            headset_player_mask |= 1 << destination->local_player_index;
        else
            headset_player_mask &= ~(1 << (destinations[destination_index]).local_player_index);
        if (destination->destination == _voice_destination_headset || destination->destination == _voice_destination_everyone) {
            m_player_incoming_voice_to_speakers_mask[destination->local_player_index] |= 1 << remote_player_index;
            if (destination->destination == _voice_destination_headset) {
                headset_destination_found = true;
            } else if (!headset_destination_found && destination->destination == _voice_destination_everyone) {
                if (closest_destination_index == -1 || destination->distance < destinations[closest_destination_index].distance)
                    closest_destination_index = destination_index;
            }
        } else {
            m_player_incoming_voice_to_speakers_mask[destination->local_player_index] &= ~(1 << remote_player_index);
        }
    }
    set_voice_destination_headset(headset_player_mask, remote_player_index);
    s_voice_speaker_volume speaker_volume;
    if (headset_destination_found) {
        voice_observer_build_speaker_volume(&speaker_volume, 0.45f);
        voice_set_speaker_volume(remote_player_index, &speaker_volume);
        voice_observer_set_speaker_enabled(remote_player_index, true);
    } else if (closest_destination_index != -1) {
        real_point3d remote_position = {0.0f};
        long local_player_index = destinations[closest_destination_index].local_player_index;
        if (player_get_unit_body_position(remote_player_index, &remote_position, false))
            build_speaker_volume_for_proximity_voice(local_player_index, &remote_position, &speaker_volume);
        else
            voice_observer_build_speaker_volume(&speaker_volume, 1.0f);
        voice_set_speaker_volume(remote_player_index, &speaker_volume);
        voice_observer_set_speaker_enabled(remote_player_index, true);
    } else {
        voice_observer_set_speaker_enabled(remote_player_index, false);
    }
}
static __forceinline e_voice_destination voice_observer_desired_destination_out_of_game(long local_player_index, long remote_player_index)
{
    e_voice_destination desired_destination = _voice_destination_none;
    if (g_voice_interface_globals.initialized && g_voice_interface_globals.lifecycle == _voice_lifecycle_out_of_game_pending) {
        if (voice_player_is_valid(local_player_index) && voice_player_is_valid(remote_player_index) &&
            voice_players_on_same_team(local_player_index, remote_player_index))
            desired_destination = _voice_destination_voice_only;
    } else {
        desired_destination = _voice_destination_voice_only;
    }
    return desired_destination;
}
// @address 0x421840 ?update_local_players_out_of_game_incoming_voice_status@c_voice_observer@@AAAXXZ
void c_voice_observer::update_local_players_out_of_game_incoming_voice_status()
{
    // match: 0x421840: value range assumption guides branch lowering
    __assume(this == &g_voice_observer);
    m_incoming_voice_mask = 0;
    for (long remote_player_index = 0; remote_player_index < 16; ++remote_player_index) {
        if (!voice_player_is_local(remote_player_index) && voice_observer_player_has_voice(remote_player_index)) {
            s_remote_player_voice_destination destinations[16] = {0};
            for (long local_player_index = 0; local_player_index < 16; ++local_player_index) {
                e_voice_destination actual_destination = _voice_destination_none;
                if (voice_player_is_local(local_player_index)) {
                    e_voice_destination desired_destination = voice_observer_desired_destination_out_of_game(local_player_index, remote_player_index);
                    e_voice_destination incoming_destination = get_voice_actual_destination(local_player_index, remote_player_index, desired_destination);
                    if (incoming_destination != _voice_destination_none)
                        m_incoming_voice_mask |= 1 << remote_player_index;
                    actual_destination = incoming_destination;
                }
                destinations[local_player_index].local_player_index = local_player_index;
                destinations[local_player_index].destination = actual_destination;
            }
            set_voice_destination(remote_player_index, destinations, 16);
        }
    }
}

static __forceinline bool voice_observer_lifecycle_out_of_game()
{
    bool out_of_game = false;
    if (g_voice_interface_globals.initialized) {
        e_voice_lifecycle lifecycle = voice_observer_get_lifecycle();
        if (lifecycle >= _voice_lifecycle_out_of_game && lifecycle <= _voice_lifecycle_out_of_game_pending)
            out_of_game = true;
    }
    return out_of_game;
}

// @address 0x41fa38 ?update@c_voice_observer@@QAAXXZ
void c_voice_observer::update()
{
    // match: 0x41fa38: value range assumption guides branch lowering
    __assume(this == &g_voice_observer);
    if (m_initialized) {
        update_player_team_masks();
        if (voice_observer_lifecycle_in_game()) {
            update_local_player_death_gurgles();
            update_local_player_proximity_masks();
            update_voice_repeater_mask();
            voice_observer_update_local_player_in_game_masks(this);
            update_local_player_push_to_talk_masks();
            update_local_player_dead_masks();
            if (game_is_cooperative())
                update_local_players_out_of_game_incoming_voice_status();
        } else if (voice_observer_lifecycle_out_of_game()) {
            update_local_players_out_of_game_incoming_voice_status();
        }
    }
}

// @address 0x422770 ?build_speaker_volume_for_proximity_voice@c_voice_observer@@AAAXJPBTreal_point3d@@PAUs_voice_speaker_volume@@@Z
void c_voice_observer::build_speaker_volume_for_proximity_voice(
    long local_player_index, const real_point3d *remote_player_position, s_voice_speaker_volume *speaker_volume_out)
{
    voice_observer_build_speaker_volume(speaker_volume_out, 1.0f);
    long player_datum_index = voice_get_player_datum(local_player_index);
    if (player_datum_index == -1)
        return;
    long listener_index = -1;
    while (true)
    {
        if (player_datum_index != -1)
            listener_index = (long)director_get_perspective(player_datum_index, listener_index);
        if (listener_index == -1)
            return;
        const s_sound_listener *listener = &g_sound_manager_globals->listeners[listener_index];
        if (!listener || !listener->valid)
            continue;
        union { real_point3d point; real_vector3d vector; } localized = {0.0f};
        matrix4x3_inverse_transform_point(&listener->matrix, remote_player_position, &localized.point);
        float distance = sqrtf(localized.point.x * localized.point.x +
                               localized.point.z * localized.point.z + localized.point.y * localized.point.y);
        float attenuation;
        if (distance > 8.0f)
            attenuation = (distance - 8.0f) * 0.01875f + distance * 0.10526316f;
        else
            attenuation = distance * 0.10526316f;
        normalize3d(&localized.vector);
        c_static_array<float, 5> speaker_gains;
        calculate_positional_speaker_gain_internal(&localized.vector, &speaker_gains, true, 5);
        static const long channel_map[5] = {0, 1, 4, 5, 2};
        for (long channel = 0; channel < 5; ++channel)
        {
            float gain = (1.0f - attenuation) * speaker_gains[channel];
            gain = PIN(gain, 0.0f, 1.0f);
            speaker_volume_out->channels[channel_map[channel]] = gain;
        }
    }
}


static __forceinline s_voice_player_state *voice_observer_sound_player_state(long player_index)
{
    s_voice_player_state *player_state = 0;
    player_state = &g_voice_interface_globals.player_states[player_index];
    return player_state;
}
static __forceinline long voice_observer_sound_output_preference(long controller_index)
{
    s_voice_controller_state *controller_state = &g_voice_interface_globals.controller_states[controller_index];
    return controller_state->output_preference;
}
static __forceinline e_controller_index voice_observer_sound_player_controller(long player_index)
{
    e_controller_index controller_index = _controller_none;
    if (g_voice_interface_globals.initialized) {
        if (g_voice_interface_globals.player_states[player_index].status_flags.test(_voice_player_local)) {
            voice_observer_compiler_barrier();
            s_voice_player_state *player_state = voice_observer_sound_player_state(player_index);
            controller_index = player_state->controller_index;
        }
    }
    return controller_index;
}
static __forceinline bool voice_observer_sound_has_voice(long player_index)
{
    return g_voice_interface_globals.player_states[player_index].status_flags.test(_voice_player_has_voice);
}
// @address 0x420e78 voice_observer_new_player_sound
extern "C" long voice_observer_new_player_sound(long player_index, long sound_index, bool looping)
{
    long result = -1;
    e_controller_index controller_index = voice_observer_sound_player_controller(player_index);
    if (controller_index != _controller_none && voice_observer_sound_output_preference(controller_index) == 0 &&
        user_interface_controller_voice_allowed(controller_index)) {
        if (!voice_observer_sound_has_voice(player_index)) {
            if (looping)
                result = unattached_looping_sound_start(sound_index, -1, 1.0f);
            else
                result = unspatialized_impulse_sound_new(sound_index, 1.0f);
        } else {
            long effect_index;
            switch (controller_index) {
            case 0: effect_index = 245; break;
            case 1: effect_index = 246; break;
            case 2: effect_index = 247; break;
            case 3: effect_index = 248; break;
            // match: 0x420e78: unreachable hint omits dead branch check
            default: __assume(0);
            }
            if (looping) {
                result = unattached_looping_sound_start_with_effect(sound_index, -1, 1.0f, effect_index);
            } else {
                c_game_sound_impulse_parameters params(sound_index, 1.0f);
                params.sound_impulse_index = effect_index;
                result = params.submit_unspatialized_sound();
            }
        }
    }
    return result;
}
// @address 0x420db8 voice_observer_new_multiplayer_sound
extern "C" long voice_observer_new_multiplayer_sound(c_voice_observer *observer, long player_index, long sound_type)
{
    long result = -1;
    if (game_in_progress() && tls_game_globals()->options.game_mode == 2)
    {
        const s_multiplayer_globals_definition *multiplayer_globals =
            (const s_multiplayer_globals_definition *)tag_get(0, global_game_globals->multiplayer_globals.index);
        const s_multiplayer_runtime_globals_definition *runtime =
            (const s_multiplayer_runtime_globals_definition *)multiplayer_globals->runtime.address;
        if (sound_type < runtime->sounds.count)
        {
            long sound_index = ((const s_tag_reference *)runtime->sounds.address)[sound_type].index;
            if (sound_index != -1)
                result = voice_observer_new_player_sound(player_index, sound_index, false);
        }
    }
    return result;
}

// @address 0x7bb404 g_voice_death_gurgle_time_limit
float g_voice_death_gurgle_time_limit = 1.25f;
static __forceinline void voice_observer_game_looping_sound_stop(long looping_sound_index)
{
    game_looping_sound_datum *sound = datum_get<game_looping_sound_datum>(game_looping_sound_data, looping_sound_index);
    sound->flags.set(1, true);
}
static __forceinline long voice_observer_looping_headset_effect(long player_index)
{
    long result = -1;
    if (game_in_progress() && tls_game_globals()->options.game_mode == 2)
    {
        const s_multiplayer_globals_definition *multiplayer_globals =
            (const s_multiplayer_globals_definition *)tag_get(0, global_game_globals->multiplayer_globals.index);
        const s_multiplayer_runtime_globals_definition *runtime =
            (const s_multiplayer_runtime_globals_definition *)multiplayer_globals->runtime.address;
        if (0 < runtime->looping_sounds.count)
        {
            long sound_index = ((const s_tag_reference *)runtime->looping_sounds.address)->index;
            if (sound_index != -1)
                result = voice_observer_new_player_sound(player_index, sound_index, true);
        }
    }
    return result;
}
static __forceinline bool voice_observer_player_voice_flag(long player_index)
{
    bool result = false;
    if (g_voice_interface_globals.initialized) {
        unsigned long flags = 0;
        if (g_voice_interface_globals.player_states[player_index].status_flags.test(_voice_player_in_session))
            flags = g_voice_interface_globals.player_states[player_index].voice_flags;
        result = ((flags >> 3) & 1) != 0;
    }
    return result;
}
// @address 0x421000 ?update_local_player_push_to_talk_timers@c_voice_observer@@AAAXJW4e_controller_index@@@Z
void c_voice_observer::update_local_player_push_to_talk_timers(long player_index, e_controller_index controller_index)
{
    // match: 0x421000: value range assumption guides branch lowering
    __assume(this == &g_voice_observer);
    s_game_input_state *input_state = &input_abstraction_globals.input_states[controller_index];
    const gamepad_state *gamepad = input_get_gamepad_state((short)controller_index);
    bool user_interface_active = window_manager_is_controller_ui_active(&g_window_manager, controller_index);
    if (gamepad) {
        bool button_remapped = input_state->abstract_buttons[26] != 2;
        input_state->buttons[input_state->abstract_buttons[26]].m_button_index = 26;
        c_abstract_button *button = &input_state->buttons[input_state->abstract_buttons[26]];
        unsigned long time = network_time_get();
        bool push_to_talk_active = m_player_push_to_talk_start_time[player_index] != 0;
        bool dpad_down = gamepad->button_frames[3] > 0 || gamepad->button_frames[4] > 0 || gamepad->button_frames[5] > 0;
        bool dpad_side_down = gamepad->button_frames[4] > 0 || gamepad->button_frames[5] > 0;
        bool dead = player_ever_alive(player_index) && game_engine_in_round() && !player_is_alive_or_wounded(player_index);
        bool allowed = true;
        if (!(tls_game_globals()->options.game_mode == 2 && game_engine_sandbox_is_active()))
            dpad_down = dpad_down || gamepad->button_frames[2] > 0;
        if (m_player_mute_mask[player_index] == 0 && dead)
            m_player_mute_mask[player_index] = time;
        else if (!dead)
            m_player_mute_mask[player_index] = 0;
        long dead_time = m_player_mute_mask[player_index];
        if (dead_time != 0 && (float)network_time_since_inline(dead_time) < g_voice_death_gurgle_time_limit * 1000.0f)
            allowed = false;
        if (voice_observer_player_voice_flag(player_index))
            allowed = false;
        if (!push_to_talk_active) {
            if (!user_interface_active) {
                bool pressed;
                if (button_remapped)
                    pressed = button->down_frames() > 0;
                else
                    pressed = (dead && dpad_side_down) || (!dead && dpad_down);
                if (pressed) {
                    if (!allowed) {
                        if (m_player_voice_flags[player_index] == 0) {
                            voice_observer_new_multiplayer_sound(this, player_index, 1);
                            m_player_voice_flags[player_index] = time;
                        } else if ((float)network_time_since_inline(m_player_voice_flags[player_index]) >= k_game_engine_invalid_dead_point.z) {
                            m_player_voice_flags[player_index] = 0;
                        }
                    } else {
                        m_player_push_to_talk_start_time[player_index] = time;
                        m_player_push_to_talk_last_time[player_index] = time;
                        if (m_player_push_to_talk_sound_index[player_index] != -1) {
                            voice_observer_game_looping_sound_stop(m_player_push_to_talk_sound_index[player_index]);
                            m_player_push_to_talk_sound_index[player_index] = -1;
                        }
                        m_player_push_to_talk_sound_index[player_index] = voice_observer_looping_headset_effect(player_index);
                    }
                }
            }
        } else {
            bool timed_out = (float)network_time_since_inline(m_player_push_to_talk_start_time[player_index]) >=
                g_network_configuration.push_to_talk_timeout * 1000.0f;
            bool stop_sound = false;
            if (timed_out || !allowed) {
                stop_sound = true;
                m_player_push_to_talk_last_time[player_index] = 0;
                if (!allowed || (timed_out && !dpad_down))
                    m_player_push_to_talk_start_time[player_index] = 0;
            } else if (dpad_down || voice_player_is_talking(player_index)) {
                m_player_push_to_talk_last_time[player_index] = time;
            }
            else if ((float)network_time_since_inline(m_player_push_to_talk_last_time[player_index]) >=
                g_network_configuration.push_to_talk_idle_timeout * 1000.0f)
            {
                stop_sound = true;
                m_player_push_to_talk_start_time[player_index] = 0;
                m_player_push_to_talk_last_time[player_index] = 0;
            }
            if (stop_sound && m_player_push_to_talk_sound_index[player_index] != -1) {
                voice_observer_game_looping_sound_stop(m_player_push_to_talk_sound_index[player_index]);
                m_player_push_to_talk_sound_index[player_index] = -1;
            }
        }
    }
}
