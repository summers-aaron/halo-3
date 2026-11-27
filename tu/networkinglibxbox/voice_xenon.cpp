// networkinglibxbox/voice_xenon.obj: from the records 0x94a58 0x951b0 0x95390 0x957f0 0x118b80 0x1310e0 0x288518 0x3c0148 0x3c02f0 0x3c0408 0x3c04b0 0x3c0510 0x3c05a8 0x3c0648 0x3c0748 0x3c0878 0x3c0968 0x3c09b8 0x3c0a08 0x3c0a58 0x3c0b70 0x3c0b90 0x3c0bd8 0x3c0cc8 0x3c0d58 0x3c0db8 0x3c0df8 0x3c0e60 0x3c0f10 0x3c0f48 0x3c0fb0 0x3c1088 0x3c1150 0x3c13b8 0x3c14f8 0x3c18d8 0x3c19b8 0x3c1a80 0x3c1b20 0x3c1d08 0x3c1e88 0x3c2080 0x3c2168 0x3c22b0 0x3c2400 0x3c24e0 0x3c2598 0x3c2628 0x3c2700 0x3c28b0 0x3c2a98 0x3c2c00 0x3c2f70 0x41de58
// @compiler xdk6130
// @flags /TP /O2 /Oc
// @address 0x3c0148 ?voice_initialize@@YAXXZ
// @address 0x3c1150 ?voice_interface_set_voice_repeater@@YAXXZ
// @address 0x3c2168 ?voice_interface_update_from_session_membership@@YAXXZ
// @address 0x288518 ?create_remote_voice_output@c_xenon_audio_dsp_manager@@QAAPAUIXAudioSubmixVoice@@J@Z
// @address 0x3c0bd8 ?voice_player_is_valid_in_game@@YA_NJ@Z
// @address 0x3c2a98 ?voice_interface_initialize_player_speaker_states@@YAXXZ
// @address 0x3c28b0 ?voice_interface_queue_voice_data_for_sending@@YAXW4e_controller_index@@PBEJJW4e_voice_data_type@@@Z
// @address 0x951b0 ?voice_update@@YAXXZ
// @address 0x3c13b8 ?voice_update_poll_for_local_chat_data@@YAXXZ

#include "cseries/bit_operations.h"
#include "interface/controller_inline.h"
#include "game/game_engine_list.h"
#include "memory/data_types.h"
#include <stdio.h>
#include "memory/static_arrays.h"
#include <xtl.h>
#include <xaudio.h>
#include <xhv.h>
#include <stdlib.h>
#include <stdarg.h>
#include "cseries/cseries.h"
#include "memory/data.h"
#include "memory/datum_index_inlines.h"
#include "game/game.h"
#include "simulation/simulation.h"
#include "game/game_engine_team.h"
#include "networking/online/online_xenon.h"
#include "networking/online/voice_xenon.h"
#include "networking/online/voice_engine_xenon.h"
#include "networking/online/voice_routing.h"
#include "simulation/simulation_globals.h"
#include "networking/online/voice_data_types.h"
#include "networking/online/voice_speaker_volume_types.h"
#include "networking/voice_processing_mode_types.h"
#include "networking/online/voice_destination_types.h"
#include "game/game_types.h"
#include "interface/c_controller.h"
#include "interface/gui_screens/player_select/gui_player_select_screen_widget.h"
#include "interface/user_interface_session.h"
#include "networking/logic/network_session_interface.h"
#include "networking/online/voice_queue_peer_tracking_types.h"
#include "networking/online/player_voice_stream_header_types.h"
#include "networking/online/voice_packet_header_types.h"
#include "networking/online/voice_queue_full_packet_header_types.h"
#include "game/player_identifier_types.h"
#include "networking/session/network_session_parameters.h"
#include "networking/session/network_session.h"
#include "interface/user_interface_controller.h"
#include "networking/logic/network_life_cycle.h"
#include "networking/online/voice_observer.h"
#include "game/game_globals.h"
#include "networking/transport/transport_security.h"
#include "interface/controller_types.h"
#include "interface/c_player_profile.h"
#include "networking/online/voice_interface_types.h"
#include "networking/online/voice_lifecycle.h"
#include "networking/online/voice_queue_types.h"
#include "networking/online/voice_availability.h"
#include "networking/network_configuration_types.h"
#include "networking/network_configuration.h"
#include "networking/online/voice_observer_types.h"
#include "game/game_variant.h"
#include "game/game_globals_storage.h"
#include "memory/tls_globals.h"
#include "game/game_engine_scoring_globals.h"
#include "game/game_engine_current_inline.h"
#include "game/game_engine_team_inline.h"
#include "networking/online/voice_xenon_inline.h"
#include "sound/sound_xaudio_xenon_dsp_manager_types.h"
#include "sound/sound_xaudio_xenon.h"
#include "networking/session/network_session_membership.h"
#include "networking/session/network_session_parameter_matchmaking_types.h"
#include "networking/session/network_session_parameters_generic.h"
#include "simulation/simulation_world_types.h"
#include "networking/voice/voice_xenon_services.h"

__forceinline void c_voice_routing::initialize() { m_initialized = true; }

struct s_player_identifier;
class c_network_session;
s_voice_interface_globals g_voice_interface_globals;
c_xenon_voice_engine g_voice_engine;
c_voice_availability g_voice_availability;
c_voice_observer g_voice_observer;
c_voice_routing g_voice_routing;

class c_game_engine;

struct s_game_engine_globals;

static __forceinline e_game_team voice_get_player_team_index(long player_index) {
    e_game_team team_index = _game_team_none;
    if (g_voice_interface_globals.initialized) {
        if (voice_player_get_state(player_index)->status_flags.test(_voice_player_in_session)) {
            s_voice_player_state *player_state = voice_player_get_state(player_index);
            team_index = player_state->team_index;
        }
    }
    return team_index;
}
static __forceinline e_controller_index voice_player_get_controller_index(long player_index) {
    e_controller_index controller_index = _controller_none;
    if (g_voice_interface_globals.initialized) {
        if (voice_player_get_state(player_index)->status_flags.test(_voice_player_local)) {
            s_voice_player_state *player_state = voice_player_get_state(player_index);
            controller_index = player_state->controller_index;
        }
    }
    return controller_index;
}

template<long k_capacity>
char *c_static_string<k_capacity>::append_print(const char *format, ...) {
    int length = this->length();
    int size = k_capacity - length;
    char *end = m_string + length;
    va_list args;
    va_start(args, format);
    _vsnprintf(end, size - 1, format, args);
    va_end(args);
    end[size - 1] = 0;
    return m_string;
}
// @address 0x94a58 ?append_print@?$c_static_string@$0BAA@@@QAAPADPBDZZ
template char *c_static_string<256>::append_print(const char *format, ...);

IXAudioSubmixVoice *c_xenon_audio_dsp_manager::create_remote_voice_output(long ring_index) {
    // match: 0x94a58: value range assumption guides branch lowering
    __assume(this == &g_xaudio_dsp_manager);
    XAUDIOFXINIT ring_buffer_effect = { m_ring_buffer_effect_id, &m_ring_buffer };
    LPCXAUDIOFXINIT effects[] = { &ring_buffer_effect };
    XAUDIOVOICEFXCHAIN effect_chain = { 1, effects };
    XAUDIOCHANNELMAPENTRY channel_map_entries[] = {
        { 0, 0, 1.0f },
        { 0, 1, 1.0f },
        { 0, 2, 1.0f },
        { 0, 3, 0.0f },
        { 0, 4, 1.0f },
        { 0, 5, 1.0f },
    };
    XAUDIOVOICEOUTPUTENTRY voice_output_entry;
    XAUDIOCHANNELMAP channel_map;
    XAUDIOVOICEOUTPUT voice_output = { 1, &voice_output_entry };
    voice_output_entry.pDestVoice = m_remote_voice_output_destination;
    voice_output_entry.pChannelMap = &channel_map;
    channel_map.EntryCount = 6;
    channel_map.paEntries = channel_map_entries;
    XAUDIOSUBMIXVOICEINIT submix_voice_init = { 0 };
    submix_voice_init.MaxChannelMapEntryCount = 6;
    submix_voice_init.Format.SampleType = XAUDIOSAMPLETYPE_FLOAT;
    submix_voice_init.Format.SampleRate = 48000;
    submix_voice_init.Format.ChannelCount = 1;
    submix_voice_init.pVoiceOutput = &voice_output;
    submix_voice_init.pEffectChain = &effect_chain;
    submix_voice_init.SubmixStage = 1;
    IXAudioSubmixVoice *voice;
    XAudioCreateSubmixVoice(&submix_voice_init, &voice);
    XAUDIOFXPARAM ring_parameter;
    ring_parameter.Flags = ring_index;
    voice->SetEffectParam(0, 0, XAUDIOFXPARAMTYPE_FLAG, &ring_parameter);
    return voice;
}

// @address 0x830e9fcc ?g_voice_local_processing_modes@@3QBQAXB
// @address 0x830e9fd0 ?g_voice_remote_processing_modes@@3QBQAXB
// @address 0x6e3558 XHVCreateEngine

// @address 0x41de58 voice_initialize_engine
extern "C" void voice_initialize_engine(char *) {
    XHV_INIT_PARAMS parameters = {0};
    memset(&parameters, 0, sizeof(parameters));
    parameters.dwMaxLocalTalkers = 4;
    parameters.dwMaxRemoteTalkers = 15;
    parameters.localTalkerEnabledModes = g_voice_local_processing_modes;
    parameters.dwNumLocalTalkerEnabledModes = 1;
    parameters.remoteTalkerEnabledModes = g_voice_remote_processing_modes;
    parameters.dwNumRemoteTalkerEnabledModes = 1;
    parameters.bRelaxPrivileges = 1;
    parameters.pfxDefaultRemoteTalkerFX = &g_xaudio_dsp_manager.m_remote_talker_effect_chain;
    parameters.pfxOutputFX = &g_xaudio_dsp_manager.m_output_effect_chain;
    if (XHVCreateEngine(&parameters, 0, &g_voice_engine.engine) == 0)
        g_voice_engine.initialized = true;
}

bool voice_enforcing_team_restrictions(void) {
    bool enforcing = false;
    if (game_engine_has_teams() || current_game_engine() &&
        (current_game_variant()->engine_type == 7 || current_game_variant()->engine_type == 10))
        enforcing = true;
    return enforcing;
}

static __forceinline bool voice_xenon_lifecycle_in_game(void) {
    bool in_game = false;
    if (g_voice_interface_globals.initialized) {
        e_voice_lifecycle lifecycle = voice_get_lifecycle();
        if (lifecycle > _voice_lifecycle_none && lifecycle <= _voice_lifecycle_in_game_pending) in_game = true;
    }
    return in_game;
}

bool voice_lifecycle_in_game(void) {
    return voice_xenon_lifecycle_in_game();
}

void voice_initialize(void) {
    memset(&g_voice_interface_globals, 0, sizeof(g_voice_interface_globals));
    g_voice_interface_globals.repeater_peer_index = -1;
    g_voice_interface_globals.repeater_usage = 0;
    g_voice_interface_globals.lifecycle = _voice_lifecycle_none;
    g_voice_interface_globals.matchmaking_state = 0;
    for (long player_index = 0; player_index < 16; ++player_index)
        voice_interface_reset_player(player_index);
    for (e_controller_index controller_index = first_controller(); controller_index != _controller_none; controller_index = next_controller(controller_index))
        voice_interface_reset_controller(controller_index);
    voice_interface_initialize_player_speaker_states();
    voice_initialize_engine((char *)&g_voice_engine);
    g_voice_availability.initialize();
    g_voice_routing.initialize();
    g_voice_queue.initialize();
    g_voice_observer.initialize();
    for (e_controller_index controller_index = first_controller(); controller_index != _controller_none; controller_index = next_controller(controller_index)) {
        voice_interface_register_local_player(controller_index, false);
    }
    g_voice_interface_globals.initialized = true;
}

bool voice_player_is_talking(long player_index) {
    bool talking = false;
    if (g_voice_interface_globals.initialized) {
        if (voice_player_get_state(player_index)->status_flags.test(_voice_player_local)) {
            e_controller_index controller_index = voice_get_player_controller_index(player_index);
            if (controller_index != _controller_none)
                talking = g_voice_engine.local_user_talking(controller_index);
        } else {
            unsigned __int64 xuid = (__int64)player_index | 0xBEEFCAFE00000000ui64;
            talking = g_voice_engine.remote_player_talking(xuid);
        }
    }
    return talking;
}


void voice_set_mute(long player_index, long muted_player_index, bool mute) {
    if (g_voice_interface_globals.initialized) {
        s_voice_player_state *player_state = voice_player_get_state(player_index);
        s_voice_player_state *muted_player_state = voice_player_get_state(muted_player_index);
        if (muted_player_state->status_flags.test(_voice_player_in_session)) {
            if (player_state->status_flags.test(_voice_player_local)) {
                e_controller_index controller_index = voice_get_player_controller_index(player_index);
                s_voice_player_state *state = voice_player_get_state(player_index);
                if (mute)
                    state->muted_players |= 1 << muted_player_index;
                else
                    state->muted_players &= ~(1 << muted_player_index);
                voice_interface_update_controller_network_properties(controller_index);
            }
        }
    }
}

bool voice_get_mute(long source_player_index, long destination_player_index) {
    bool muted = false;
    if (g_voice_interface_globals.initialized) {
        if (voice_xenon_player_is_valid(source_player_index)) {
            s_voice_player_state *player_state = voice_player_get_state(source_player_index);
            muted = (player_state->muted_players & (1 << destination_player_index)) != 0;
        }
    }
    return muted;
}

static __forceinline bool voice_get_mute_inline(long source_player_index, long destination_player_index) {
    bool muted = false;
    if (g_voice_interface_globals.initialized) {
        if (voice_xenon_player_is_valid(source_player_index)) {
            s_voice_player_state *player_state = voice_player_get_state(source_player_index);
            muted = (player_state->muted_players & (1 << destination_player_index)) != 0;
        }
    }
    return muted;
}
bool voice_any_local_player_muting(long player_index) {
    bool muting = false;
    for (long local_player_index = 0; local_player_index < 16; ++local_player_index) {
        e_controller_index controller_index = voice_player_get_controller_index(local_player_index);
        if (controller_index != _controller_none && voice_get_mute_inline(local_player_index, player_index)) {
            muting = true;
            break;
        }
    }
    return muting;
}

bool voice_get_local_mute_through_guide(long player_index, long muted_player_index) {
    bool muted = false;
    if (g_voice_interface_globals.initialized) {
        if (voice_xenon_player_is_valid(player_index) && voice_xenon_player_is_local(player_index)) {
            e_controller_index controller_index = voice_get_player_controller_index(player_index);
            s_voice_controller_state *controller_state = &g_voice_interface_globals.controller_states[controller_index];
            muted = (controller_state->muted_players & (1 << muted_player_index)) != 0;
        }
    }
    return muted;
}

bool voice_players_on_same_team(long first_player_index, long second_player_index) {
    bool same_team = false;
    if (voice_xenon_player_is_valid(first_player_index) && voice_xenon_player_is_valid(second_player_index)) {
        e_game_team first_team = voice_get_player_team_index(first_player_index);
        e_game_team second_team = voice_get_player_team_index(second_player_index);
        if (first_team != _game_team_none && second_team != _game_team_none && first_team == second_team)
            same_team = true;
    }
    return same_team;
}

bool voice_players_are_allies(long first_player_index, long second_player_index) {
    bool allies = false;
    if (voice_xenon_player_is_valid(first_player_index) && voice_xenon_player_is_valid(second_player_index)) {
        e_game_team first_team = voice_get_player_team_index(first_player_index);
        e_game_team second_team = voice_get_player_team_index(second_player_index);
        if (first_team != _game_team_none && second_team != _game_team_none &&
            !(unsigned char)::game_engine_team_is_enemy(first_team, second_team))
            allies = true;
    }
    return allies;
}

void voice_queue_received_packet(long sender, const void *data, long size, e_voice_data_type type) {
    // match: 0x3c0878: without it c2 re-tests type (cmpwi r6,0 + bne) and swaps r9/r10 colors
    __assume(type == _voice_data_chat);
    if (g_voice_interface_globals.initialized && voice_lifecycle_valid() && voice_player_has_voice(sender)) {
        e_voice_data_type expected_type = _voice_data_chat;
        long expected_size = 10;
        if (type == expected_type && size == expected_size) {
            s_voice_player_state *player_state = voice_player_get_state(sender);
            if (player_state->voice || g_voice_interface_globals.speaker_states[sender].enabled) {
                unsigned __int64 xuid = (__int64)sender | 0xBEEFCAFE00000000ui64;
                g_voice_engine.submit_voice(xuid, (const unsigned char *)data, size, type);
            }
        }
    }
}

bool voice_player_has_voice_disabled(long player_index) {
    bool disabled = false;
    if (g_voice_interface_globals.initialized) {
        unsigned long player_flags = voice_get_player_flags(player_index);
        disabled = (player_flags & 2) != 0;
    }
    return disabled;
}

// @address 0x3c09b8 voice_player_has_voice_disallowed
extern "C" bool voice_player_has_voice_disallowed(long player_index) {
    bool flagged = false;
    if (g_voice_interface_globals.initialized) {
        unsigned long player_flags = voice_get_player_flags(player_index);
        flagged = (player_flags & 4) != 0;
    }
    return flagged;
}

// @address 0x3c0a08 voice_player_has_alternate_voice_output
extern "C" bool voice_player_has_alternate_voice_output(long player_index) {
    bool flagged = false;
    if (g_voice_interface_globals.initialized) {
        unsigned long player_flags = voice_get_player_flags(player_index);
        flagged = (player_flags & 16) != 0;
    }
    return flagged;
}

static __forceinline bool voice_xenon_get_active_session(c_network_session **session) {
    bool result = false;
    if (g_voice_interface_globals.initialized) {
        switch (g_voice_interface_globals.matchmaking_state) {
        case 1: result = network_life_cycle_try_and_get_squad_session(session); break;
        case 2: result = network_life_cycle_in_group_session(session); break;
        }
    }
    return result;
}

unsigned long voice_get_squad_session_peer_mask_by_group_session(void) {
    unsigned long peer_mask = 0;
    if (g_voice_interface_globals.initialized && g_voice_interface_globals.matchmaking_state == 2) {
        c_network_session *squad_session = 0;
        c_network_session *group_session = 0;
        if (network_life_cycle_in_group_session(&group_session) && network_life_cycle_try_and_get_squad_session(&squad_session)) {
            const c_network_session_membership *group_membership = 0;
            if (!group_session->disconnected() && group_session->m_session_membership.has_membership())
                group_membership = &group_session->m_session_membership;
            long peer_index = (group_membership)->get_first_peer();
            if (peer_index != -1) {
                do {
                    if (group_membership->is_peer_valid(peer_index) &&
                        squad_session->m_session_membership.get_peer_from_secure_address(group_membership->get_peer_address(peer_index)) != -1)
                        peer_mask |= 1 << peer_index;
                    peer_index = (group_membership)->get_next_peer(peer_index);
                } while (peer_index != -1);
            }
        }
    }
    return peer_mask;
}

// @address 0x95390 ?voice_get_player_peer_mask@@YAJJ@Z
long voice_get_player_peer_mask(long player_index) {
    long peer_mask = 0;
    unsigned char initialized = g_voice_interface_globals.initialized;
    if (voice_xenon_player_is_valid(player_index)) {
        c_network_session *session = 0;
        bool session_valid = false;
        if ((unsigned int)initialized != 0) {
            long matchmaking_state = g_voice_interface_globals.matchmaking_state;
            if (matchmaking_state != 1) {
                if (matchmaking_state == 2)
                    session_valid = network_life_cycle_in_group_session(&session);
            } else {
                session_valid = network_life_cycle_try_and_get_squad_session(&session);
            }
        }
        if (session_valid != 0) {
            c_network_session_membership *membership = 0;
            if (!session->disconnected() && session->m_session_membership.has_membership())
                membership = &session->m_session_membership;
            long session_type = session->m_session_type;
            if (session_type == 1) {
                for (long bit = 0; bit < 16; bit++) {
                    if (voice_xenon_player_is_valid(bit))
                        peer_mask |= 1 << bit;
                }
            } else {
                if (session_type == 2) {
                    long target_peer = -1;
                    if ((unsigned int)initialized != 0 && (g_voice_interface_globals.player_states[player_index].status_flags.m_storage & 1) != 0)
                        target_peer = g_voice_interface_globals.player_states[player_index].user_index;
                    if (membership->is_peer_valid(target_peer)) {
                        const s_network_session_peer *target = membership->get_peer(target_peer);
                        for (long bit = 0; bit < 16; bit++) {
                            if (voice_xenon_player_is_valid(bit)) {
                                long current_peer = -1;
                                if ((unsigned int)initialized != 0 && (g_voice_interface_globals.player_states[bit].status_flags.m_storage & 1) != 0)
                                    current_peer = g_voice_interface_globals.player_states[bit].user_index;
                                if (membership->is_peer_valid(current_peer) && target->party_nonce == membership->get_peer(current_peer)->party_nonce)
                                    peer_mask |= 1 << bit;
                            }
                        }
                    }
                }
            }
        }
    }
    return peer_mask;
}

static __forceinline long voice_xenon_local_peer_index(void) {
    long peer_index = -1;
    if (g_voice_interface_globals.initialized) peer_index = g_voice_interface_globals.local_peer_index;
    return peer_index;
}

// @address 0x3c0b70 ?voice_get_local_peer_index@@YAJXZ
long voice_get_local_peer_index(void) {
    return voice_xenon_local_peer_index();
}

e_controller_index voice_get_player_controller_index(long player_index) {
    return voice_player_get_controller_index(player_index);
}


bool voice_lifecycle_in_matchmaking(void) {
    bool in_matchmaking = false;
    bool matchmaking = false;
    if (g_voice_interface_globals.initialized) matchmaking = g_voice_interface_globals.matchmaking_state == 2;
    if (matchmaking) in_matchmaking = true;
    return in_matchmaking;
}

bool voice_lifecycle_out_of_game(void) {
    bool out_of_game = false;
    if (g_voice_interface_globals.initialized) {
        e_voice_lifecycle lifecycle = voice_get_lifecycle();
        if (lifecycle >= _voice_lifecycle_out_of_game && lifecycle <= _voice_lifecycle_out_of_game_pending) out_of_game = true;
    }
    return out_of_game;
}

unsigned long voice_get_controller_settings(e_controller_index controller_index) {
    unsigned long settings = 0;
    if (g_voice_interface_globals.initialized) {
        long player_index = voice_get_controller_player_index(controller_index);
        if (player_index != -1) {
            s_voice_player_state *player_state = voice_player_get_state(player_index);
            settings = player_state->settings;
        }
    }
    return settings;
}

bool voice_set_speaker_volume(long player_index, const s_voice_speaker_volume *volume) {
    bool success = false;
    if (g_voice_interface_globals.initialized) {
        XAUDIOCHANNELVOLUMEENTRY channel_volumes[6];
        for (long channel_index = 0; channel_index < 6; ++channel_index) {
            channel_volumes[channel_index].EntryIndex = (unsigned char)channel_index;
            channel_volumes[channel_index].Volume = volume->channels[channel_index];
        }
        XAUDIOCHANNELVOLUME channel_volume;
        channel_volume.EntryCount = 6;
        channel_volume.paEntries = channel_volumes;
        XAUDIOVOICEOUTPUTVOLUMEENTRY output_entry;
        output_entry.OutputVoiceIndex = 0;
        output_entry.pVolume = &channel_volume;
        XAUDIOVOICEOUTPUTVOLUME output_volume;
        output_volume.EntryCount = 1;
        output_volume.paEntries = &output_entry;
        g_voice_interface_globals.speaker_states[player_index].voice->SetVoiceOutputVolume(&output_volume);
    }
    return success;
}


void voice_interface_update_controller_network_properties(e_controller_index controller_index) {
    long user_index = (*controller_get(controller_index)).user_index;
    if (user_index != -1) {
        s_player_configuration configuration;
        e_controller_index network_controller_index;
        unsigned long voice_settings;
        voice_settings = 0;
        network_controller_index = _controller_none;
        if (network_session_interface_get_local_user_properties(user_index, &network_controller_index, &configuration, &voice_settings)) {
            voice_settings = voice_get_controller_settings(controller_index);
            network_session_interface_set_local_user_properties(user_index, controller_index, &configuration, voice_settings);
        }
    }
}

static __forceinline long voice_get_host_peer_index(void) {
    long peer_index = -1;
    if (g_voice_interface_globals.initialized) peer_index = g_voice_interface_globals.host_peer_index;
    return peer_index;
}
void voice_interface_set_voice_repeater(void) {
    g_voice_interface_globals.repeater_peer_index = -1;
    if (g_voice_interface_globals.initialized) {
        c_network_session *session = 0;
        if (voice_xenon_get_active_session(&session)) {
            const c_network_session_membership *membership = voice_get_session_membership();
            c_network_session_parameters *parameters = &(*(c_network_session_parameters *)session->m_session_parameters);
            long local_peer_index = voice_xenon_local_peer_index();
            long host_peer_index = voice_get_host_peer_index();
            if (membership && local_peer_index != -1 && host_peer_index != -1) {
                if (local_peer_index == host_peer_index) {
                    long repeater_peer_index = -1;
                    long best_downstream_bandwidth = 0;
                    for (long peer_index = (membership)->get_first_peer(); peer_index != -1;
                        peer_index = (membership)->get_next_peer(peer_index))
                    {
                        if (membership->is_peer_valid(peer_index)) {
                            const s_network_session_peer *peer = membership->get_peer(peer_index);
                            if (local_peer_index != peer_index && peer->properties.nat_type != 3 &&
                                peer->properties.upstream_bandwidth >= g_network_configuration.minimum_voice_repeater_upstream_bandwidth &&
                                peer->properties.downstream_bandwidth >= g_network_configuration.minimum_voice_repeater_downstream_bandwidth) {
                                if (peer->properties.connectivity.peer_connectivity_mask == membership->get_peer_valid_mask() &&
                                    peer->properties.downstream_bandwidth > best_downstream_bandwidth) {
                                    repeater_peer_index = peer_index;
                                    best_downstream_bandwidth = peer->properties.downstream_bandwidth;
                                }
                            }
                        }
                    }
                    if (repeater_peer_index == -1 && (!voice_xenon_lifecycle_in_game() ||
                        (simulation_starting_up() && tls_game_globals()->options.game_mode == 1)))
                        repeater_peer_index = local_peer_index;
                    if (!((c_generic_network_session_parameter<e_campaign_difficulty_level> *)
                            &parameters->m_parameters.voice_repeater)->get_allowed()
                        || *((c_generic_network_session_parameter<e_campaign_difficulty_level> *)
                            &parameters->m_parameters.voice_repeater)->get() != repeater_peer_index)
                        ((c_generic_network_session_parameter<e_campaign_difficulty_level> *)
                            &parameters->m_parameters.voice_repeater)->set(
                                (const e_campaign_difficulty_level *)&repeater_peer_index);
                    g_voice_interface_globals.repeater_peer_index = repeater_peer_index;
                } else if (((c_generic_network_session_parameter<e_campaign_difficulty_level> *)&parameters->m_parameters.voice_repeater)->get_allowed()) {
                    g_voice_interface_globals.repeater_peer_index =
                        *((c_generic_network_session_parameter<e_campaign_difficulty_level> *)
                            &parameters->m_parameters.voice_repeater)->get();
                }
            }
        }
    }
}

static __forceinline void voice_xenon_update_session_type(void) {
    g_voice_interface_globals.matchmaking_state = 0;
    c_network_session *session = 0;
    if (user_interface_get_interactive_session(&session)) {
        long session_type = session->m_session_type;
        if (session_type == 2 || session_type == 1) g_voice_interface_globals.matchmaking_state = session_type;
    }
}
static __forceinline long voice_xenon_get_session_type(void) {
    long session_type = 0;
    if (g_voice_interface_globals.initialized) session_type = g_voice_interface_globals.matchmaking_state;
    return session_type;
}
static __forceinline void voice_xenon_update_player_voice(void) {
    for (long player_index = 0; player_index < 16; player_index++) {
        s_voice_player_state *player_state = voice_player_get_state(player_index);
        if (player_state->status_flags.test(_voice_player_registered)) {
            if (player_state->status_flags.test(_voice_player_local)) voice_interface_update_local_player_voice(player_index);
            else voice_interface_update_remote_player_voice(player_index);
        }
    }
}
void voice_update(void) {
    voice_xenon_update_session_type();
    voice_interface_update_voice_lifecycle();
    if (voice_xenon_get_session_type() == 0) g_voice_interface_globals.lifecycle = _voice_lifecycle_none;
    voice_interface_update_from_session_membership();
    if (voice_lifecycle_valid()) {
        voice_interface_update_player_voice_registration();
        voice_xenon_update_player_voice();
        voice_interface_set_voice_repeater();
        g_voice_observer.update();
        if (g_voice_availability.initialized)
            c_voice_availability::update_local_outgoing_player_masks_bridge(&g_voice_availability);
        voice_update_poll_for_local_chat_data();
    }
}

void voice_update_poll_for_local_chat_data(void) {
    for (long player_index = 0; player_index < 16; player_index++) {
        s_voice_player_state *player_state = voice_player_get_state(player_index);
        if (player_state->status_flags.test(_voice_player_local) && player_state->status_flags.test(_voice_player_has_voice)) {
            e_controller_index controller_index = _controller_none;
            if (g_voice_interface_globals.initialized) controller_index = player_state->controller_index;
            bool ready = false;
            if (g_voice_engine.initialized) {
                DWORD ready_flags = g_voice_engine.engine->GetDataReadyFlags();
                ready = (ready_flags & (1 << controller_index)) != 0;
            }
            if (ready) {
                unsigned char packet_data[104];
                unsigned long packet_size = 0;
                unsigned long packet_count = 0;
                bool success = false;
                if (g_voice_engine.initialized) {
                    unsigned long size = 100;
                    HRESULT result = g_voice_engine.engine->GetLocalChatData(controller_index, packet_data, &size, &packet_count);
                    if (result != -0x7ffffff6 && result >= 0) {
                        packet_size = size;
                        success = true;
                    }
                }
                if (success) voice_interface_queue_voice_data_for_sending(controller_index, packet_data, packet_size, packet_count, _voice_data_chat);
            }
        }
    }
}

static __forceinline bool slot_valid(long index)
{
    c_simulation_world *state = g_simulation_globals.world;
    bool result = false;
    if (index >= 0 && index < 16U && state->m_players[index].m_control_index != -1)
        result = true;
    return result;
}
static __forceinline unsigned char slot_flag(long index)
{
    c_simulation_world *state = g_simulation_globals.world;
    bool flag = false;
    if (state->m_players[index].m_control_index != -1)
        flag = state->m_players[index].m_active;
    return flag;
}
bool voice_player_is_valid_in_game(long player_index)
{
    bool result = false;
    if (g_voice_interface_globals.initialized && simulation_starting_up() && g_simulation_globals.world != 0) {
        long index = player_index & 0xffff;
        result = slot_valid(index) && slot_flag(index);
    }
    return result;
}


// @address 0x3c0cc8 voice_get_player_datum
extern "C" long voice_get_player_datum(long player_index) {
    long player_datum_index = -1;
    if (g_voice_interface_globals.initialized && voice_player_is_valid_in_game(player_index))
        player_datum_index = datum_absolute_index_to_index(tls_player_data(), player_index);
    return player_datum_index;
}

// @address 0x3c0d58 voice_player_try_and_get_datum
extern "C" player_datum *voice_player_try_and_get_datum(long index) {
    player_datum *result = 0;
    long player_index = voice_get_player_datum(index);
    if (player_index != -1)
        result = datum_get<player_datum>(tls_player_data(), player_index);
    return result;
}

static __forceinline unsigned char *voice_sync_player_flags(long player_index)
{
    return &g_voice_interface_globals.player_states[player_index].status_flags.m_storage;
}
// @address 0x3c2080 voice_interface_update_players
extern "C" void voice_interface_update_players(const c_network_session_membership *session_membership)
{
    for (long player_index = 0; player_index < 16; player_index++)
    {
        unsigned char *player_status_flags = voice_sync_player_flags(player_index);
        if ((*player_status_flags & 1) != 0)
        {
            const s_network_session_player *session_player = &session_membership->m_current_membership.players[player_index];
            s_voice_player_state *player_state = &g_voice_interface_globals.player_states[player_index];
            player_state->team_index = (e_game_team)session_player->configuration.host.team_index;
            if ((*player_status_flags & 2) == 0)
                player_state->settings = session_player->voice_settings;
        }
    }
}

// @address 0x3c2400 voice_get_session_peer_indices
// match: 0x3c2400: pointer __restrict qualifier prevents alias reload
extern "C" bool voice_get_session_peer_indices(long *__restrict local_peer_index, long *__restrict host_peer_index) {
    bool result = false;
    *host_peer_index = -1;
    *local_peer_index = -1;
    if (g_voice_interface_globals.initialized) {
        c_network_session *session = 0;
        if (voice_xenon_get_active_session(&session)) {
            if (!session->disconnected() && session->m_session_membership.has_membership()) {
                const c_network_session_membership *membership = (session ? &session->m_session_membership : 0);
                if (membership) {
                    *local_peer_index = membership->m_local_peer_index;
                    *host_peer_index = membership->host_peer_index();
                    result = true;
                }
            }
        }
    }
    return result;
}

// @address 0x3c24e0 voice_get_session_membership
extern "C" c_network_session_membership *voice_get_session_membership(void) {
    c_network_session_membership *membership = 0;
    if (g_voice_interface_globals.initialized) {
        c_network_session *session = 0;
        if (voice_xenon_get_active_session(&session)) {
            membership = const_cast<c_network_session_membership *>(session->get_session_membership_unsafe());
        }
    }
    return membership;
}

static __forceinline bool voice_register_member_valid(const c_network_session_membership *members, long i)
{
    return (members->m_current_membership.valid_player_mask.get_bits_direct()[i >> 5] & (1UL << (i & 31))) != 0;
}
static __forceinline unsigned char *voice_register_player_flags(long player_index)
{
    return &g_voice_interface_globals.player_states[player_index].status_flags.m_storage;
}
static __forceinline s_voice_player_state *voice_register_player_state(long player_index)
{
    return &g_voice_interface_globals.player_states[player_index];
}
static __forceinline unsigned char *voice_register_controller_flags(long controller_index)
{
    return &g_voice_interface_globals.controller_states[controller_index].status_flags.m_storage;
}
static __forceinline void voice_register_player(long i, bool local, const s_network_session_player *player)
{
    s_voice_player_state *state = voice_register_player_state(i);
    state->team_index = (e_game_team)player->configuration.host.team_index;
    state->identifier = player->identifier;
    state->session_peer_index = player->peer_index;
}
static __forceinline void voice_register_controller(long controller, long player)
{
    s_voice_controller_state *state = &g_voice_interface_globals.controller_states[controller];
    state->player_index = player;
    voice_register_player_state(player)->controller_index = (e_controller_index)controller;
}

// @address 0x3c1d08 voice_interface_add_players_with_membership
extern "C" void voice_interface_add_players_with_membership(const c_network_session_membership *members)
{
    long i = 0;
    do
    {
        unsigned char *flags = voice_register_player_flags(i);
        bool add = false;
        if (members && voice_register_member_valid(members, i))
        {
            if (!(*flags & 1))
            {
                const s_network_session_player *player = &members->m_current_membership.players[i];
                bool local = player->peer_index == g_voice_interface_globals.local_peer_index;
                if (local)
                {
                    if (player->controller_index != -1 && !(*voice_register_controller_flags(player->controller_index) & 1))
                        add = true;
                }
                else
                    add = true;
            }
        }
        if (add)
        {
            const s_network_session_player *player = &members->m_current_membership.players[i];
            bool local = player->peer_index == g_voice_interface_globals.local_peer_index;
            *flags |= 1;
            voice_register_player(i, local, player);
            if (local)
            {
                unsigned char *controller_flags = voice_register_controller_flags(player->controller_index);
                *flags |= 2;
                *controller_flags |= 1;
                voice_register_controller(player->controller_index, i);
            }
        }
    } while (++i < 16);
}

static __forceinline bool voice_interface_register_remote_player(long player_index) {
    bool result = false;
    unsigned __int64 xuid = (__int64)player_index | 0xBEEFCAFE00000000ui64;
    if (g_voice_engine.register_remote_talker(xuid, player_index)) result = true;
    return result;
}


void voice_interface_update_from_session_membership(void) {
    c_network_session_membership *membership = voice_get_session_membership();
    c_network_session *session = 0;
    long local_peer_index = -1;
    long host_peer_index = -1;
    if (voice_xenon_get_active_session(&session) && membership && voice_get_session_peer_indices(&local_peer_index, &host_peer_index)) {
        g_voice_interface_globals.local_peer_index = local_peer_index;
        g_voice_interface_globals.host_peer_index = host_peer_index;
        g_voice_interface_globals.local_peer_connectivity_mask = membership->get_peer(local_peer_index)->properties.connectivity.peer_connectivity_mask;
        g_voice_interface_globals.local_peer_connectivity_mask &= ~(1 << local_peer_index);
        for (long observer_channel = 0; observer_channel < 32; ++observer_channel)
            g_voice_interface_globals.observer_channel_peer_indices[observer_channel] = membership->get_peer_from_observer_channel(observer_channel);
        voice_interface_remove_players_with_membership(membership);
        voice_interface_add_players_with_membership(membership);
    } else {
        g_voice_interface_globals.local_peer_index = -1;
        g_voice_interface_globals.host_peer_index = -1;
        g_voice_interface_globals.local_peer_connectivity_mask = 0;
        for (long observer_channel = 0; observer_channel < 32; ++observer_channel)
            g_voice_interface_globals.observer_channel_peer_indices[observer_channel] = -1;
        voice_interface_remove_players_with_membership(membership);
    }
    if (membership) voice_interface_update_players(membership);
}

void voice_interface_update_player_voice_registration(void) {
    for (long player_index = 0; player_index < 16; player_index++) {
        unsigned char *player_status_flags = voice_register_player_flags(player_index);
        if ((*player_status_flags & FLAG(_voice_player_in_session)) && !(*player_status_flags & FLAG(_voice_player_registered))) {
            bool registered;
            e_controller_index controller_index = voice_player_get_controller_index(player_index);
            if (controller_index != _controller_none)
                registered = voice_interface_register_local_player(controller_index, true);
            else
                registered = voice_interface_register_remote_player(player_index);
            if (registered) *player_status_flags |= FLAG(_voice_player_registered);
            else *player_status_flags &= ~FLAG(_voice_player_registered);
        }
    }
}
// @address 0x3c2598 voice_interface_register_local_player
extern "C" bool voice_interface_register_local_player(unsigned int controller_to_register, unsigned char skip_talker_registration)
{
    bool observer_registered;
    int observer_result;
    bool result = true;
    if (skip_talker_registration == 0)
    {
        observer_registered = false;
        if (g_voice_engine.initialized &&
            (observer_result = g_voice_engine.engine->RegisterLocalTalker(controller_to_register), 0 <= observer_result))
            observer_registered = true;
        if (!observer_registered)
            result = false;
    }
    return result;
}


bool voice_interface_unregister_remote_player(long player_index) {
    bool result = false;
    s_voice_speaker_volume speaker_volume;
    speaker_volume.channels[0] = 0.0f;
    speaker_volume.channels[1] = 0.0f;
    speaker_volume.channels[2] = 0.0f;
    speaker_volume.channels[3] = 0.0f;
    speaker_volume.channels[4] = 0.0f;
    speaker_volume.channels[5] = 0.0f;
    unsigned __int64 xuid = (__int64)player_index | 0xBEEFCAFE00000000ui64;
    voice_set_speaker_volume(player_index, &speaker_volume);
    if (g_voice_engine.unregister_remote_talker(xuid)) result = true;
    return result;
}

static __forceinline bool voice_xenon_local_start(long controller_index, e_voice_processing_mode mode) {
    bool result = false;
    if (g_voice_engine.local_user_start_processing_mode(controller_index, mode)) result = true;
    return result;
}
static __forceinline bool voice_xenon_local_stop(long controller_index, e_voice_processing_mode mode) {
    bool result = false;
    if (g_voice_engine.local_user_stop_processing_mode(controller_index, mode)) result = true;
    return result;
}
static __forceinline bool voice_xenon_remote_start(long player_index, e_voice_processing_mode mode) {
    bool result = false;
    unsigned __int64 xuid = (__int64)player_index | 0xBEEFCAFE00000000ui64;
    if (g_voice_engine.remote_player_start_processing_mode(xuid, mode)) result = true;
    return result;
}
static __forceinline bool voice_xenon_remote_stop(long player_index, e_voice_processing_mode mode) {
    bool result = false;
    unsigned __int64 xuid = (__int64)player_index | 0xBEEFCAFE00000000ui64;
    if (g_voice_engine.remote_player_start_processing_mode(xuid, mode)) result = true;
    return result;
}

bool voice_interface_set_player_processing_mode(long player_index, e_voice_processing_mode voice_processing_mode) {
    bool result = true;
    s_voice_player_state *player_state = voice_player_get_state(player_index);
    e_voice_processing_mode current_mode = player_state->processing_mode;
    if (current_mode != voice_processing_mode) {
        e_controller_index controller_index = voice_player_get_controller_index(player_index);
        bool local = controller_index != _controller_none;
        if (current_mode != _voice_processing_mode_none) {
            if (local) voice_xenon_local_stop(controller_index, current_mode);
            else voice_xenon_remote_stop(player_index, current_mode);
            player_state->processing_mode = _voice_processing_mode_none;
        }
        if (voice_processing_mode != _voice_processing_mode_none) {
            bool started = false;
            if (local) started = voice_xenon_local_start(controller_index, voice_processing_mode);
            else started = voice_xenon_remote_start(player_index, voice_processing_mode);
            if (started) player_state->processing_mode = voice_processing_mode;
            else result = false;
        }
    }
    return result;
}

static __forceinline bool voice_should_process(long i) {
    bool result = false;
    if (g_voice_interface_globals.initialized) {
        unsigned long flags = 0;
        if (voice_player_get_state(i)->status_flags.m_storage & 1)
            flags = voice_player_get_state(i)->voice_flags;
        result = (flags & 1) != 0 && (flags & 2) == 0 && (flags & 4) == 0;
    }
    return result;
}
static __forceinline void voice_set_process_status(unsigned char *p, bool value) {
    if (value) *p |= 8; else *p &= ~8;
}
// @address 0x3c18d8 ?voice_interface_update_remote_player_voice@@YAXJ@Z
void voice_interface_update_remote_player_voice(long player_index) {
    unsigned char *p = &voice_player_get_state(player_index)->status_flags.m_storage;
    bool process = voice_should_process(player_index);
    if (process) {
        bool success = voice_interface_set_player_processing_mode(player_index, _voice_processing_mode_chat);
        voice_set_process_status(p, success);
    } else {
        voice_interface_set_player_processing_mode(player_index, _voice_processing_mode_none);
        voice_set_process_status(p, false);
    }
}


// @address 0x3c19b8 voice_get_out_of_game_lifecycle
extern "C" long voice_get_out_of_game_lifecycle(void) {
    long result = 0;
    s_voice_hopper_settings settings;
    if (voice_try_and_get_hopper_settings(&settings)) {
        switch (settings.voice_mode) {
        case 0:
            result = settings.teams ? 4 : 3;
            break;
        case 1:
            result = 3;
            break;
        case 2:
            result = 4;
            break;
        case 3:
            result = 0;
            break;
        }
    }
    return result;
}

// @address 0x3c1a80 voice_get_in_game_lifecycle
extern "C" long voice_get_in_game_lifecycle(void) {
    long result = 0;
    s_voice_hopper_settings settings;
    if (voice_try_and_get_hopper_settings(&settings)) {
        switch (settings.voice_option_40) {
        case 0:
        case 1:
            result = 1;
            break;
        case 2:
            result = 2;
            break;
        case 3:
            result = 0;
            break;
        }
    }
    return result;
}


static __forceinline long voice_xenon_life_cycle_get_state(void) {
    long result = 0;
    if (voice_life_cycle_globals().initialized) result = voice_life_cycle_globals().manager.m_current_state;
    return result;
}

// @address 0x3c1b20 ?voice_interface_update_voice_lifecycle@@YAXXZ
void voice_interface_update_voice_lifecycle(void) {
    long state = voice_xenon_life_cycle_get_state();
    switch (state) {
    case _life_cycle_state_pre_game:
    case _life_cycle_state_start_game:
    case _life_cycle_state_end_game_write_stats:
    case _life_cycle_state_matchmaking_start:
    case _life_cycle_state_matchmaking_find_match_client:
    case _life_cycle_state_matchmaking_find_match:
    case _life_cycle_state_matchmaking_find_and_assemble_match:
    case _life_cycle_state_matchmaking_assemble_match:
    case _life_cycle_state_matchmaking_arbitration:
    case _life_cycle_state_matchmaking_select_host:
        g_voice_interface_globals.lifecycle = _voice_lifecycle_out_of_game;
        break;
    case _life_cycle_state_matchmaking_prepare_map:
        g_voice_interface_globals.lifecycle = (e_voice_lifecycle)voice_get_out_of_game_lifecycle();
        break;
    case _life_cycle_state_in_game:
        g_voice_interface_globals.lifecycle = simulation_starting_up() ? _voice_lifecycle_in_game : _voice_lifecycle_out_of_game;
        break;
    case _life_cycle_state_in_match:
        if (simulation_starting_up())
            g_voice_interface_globals.lifecycle = (e_voice_lifecycle)voice_get_in_game_lifecycle();
        else
            g_voice_interface_globals.lifecycle = (e_voice_lifecycle)voice_get_out_of_game_lifecycle();
        break;
    case _life_cycle_state_end_match_write_stats:
        g_voice_interface_globals.lifecycle = (e_voice_lifecycle)voice_get_in_game_lifecycle();
        break;
    case _life_cycle_state_post_match: {
        long lifecycle = _voice_lifecycle_none;
        s_voice_hopper_settings settings;
        if (voice_try_and_get_hopper_settings(&settings)) {
            switch (settings.voice_option_44) {
            case 0:
            case 1:
                lifecycle = _voice_lifecycle_out_of_game;
                break;
            case 2:
                lifecycle = _voice_lifecycle_out_of_game_pending;
                break;
            case 3:
                lifecycle = _voice_lifecycle_none;
                break;
            }
        }
        g_voice_interface_globals.lifecycle = (e_voice_lifecycle)lifecycle;
        break;
    }
    default:
        g_voice_interface_globals.lifecycle = _voice_lifecycle_none;
        break;
    }
}


static __forceinline void voice_interface_reset_player(long i) {
    s_voice_player_state *p = voice_register_player_state(i);
    memset(p, 0, sizeof(*p));
    p->controller_index = _controller_none; p->team_index = _game_team_none; p->session_peer_index = -1;
}
static __forceinline void voice_interface_reset_controller(long i) {
    s_voice_controller_state *c = &g_voice_interface_globals.controller_states[i];
    memset(c, 0, sizeof(*c)); c->player_index = -1;
}
static __forceinline long voice_remove_controller_index(long i) {
    long result = -1;
    if (g_voice_interface_globals.initialized) result = voice_register_player_state(i)->controller_index;
    return result;
}
static __forceinline void voice_queue_clear() {
    if (g_voice_queue.enabled) for (long i = 0; i < 16; ++i) {
        c_player_voice_queue *p = &g_voice_queue.players[i];
        p->packet_count = 0; p->pending_peer_mask = 0;
        memset(p->packets, 0, sizeof(p->packets)); memset(p->peers, 0, sizeof(p->peers));
    }
}
// @address 0x3c1e88 voice_interface_remove_players_with_membership
extern "C" void voice_interface_remove_players_with_membership(const c_network_session_membership *m) {
    bool changed = false;
    for (long i = 0; i < 16; ++i) {
        unsigned char *flags = voice_register_player_flags(i);
        bool remove = false;
        if (m && voice_register_member_valid(m, i)) {
            if (*flags & 1) {
                const s_network_session_player *player = &m->m_current_membership.players[i];
                s_voice_player_state *state = voice_register_player_state(i);
                if (memcmp(&state->identifier, &player->identifier, sizeof(state->identifier)) != 0
                    || state->session_peer_index != player->peer_index) remove = true;
            }
        } else if (*flags & 1) remove = true;
        if (remove) {
            if ((*flags & 4) && !(*flags & 2)) voice_interface_unregister_remote_player(i);
            if (*flags & 2) { long controller = voice_remove_controller_index(i); voice_interface_reset_controller(controller); }
            voice_interface_reset_player(i); changed = true;
        }
    }
    if (changed) voice_queue_clear();
}


static __forceinline bool voice_xenon_observer_has_push_to_talk(long player_index) {
    bool result = false;
    if (g_voice_observer.m_initialized) result = (g_voice_observer.m_voice_repeater_mask & (1UL << player_index)) != 0;
    return result;
}

void voice_interface_queue_voice_data_for_sending(
    e_controller_index controller_index, const unsigned char *data, long size, long packet_count, e_voice_data_type type)
{
    if (g_voice_interface_globals.initialized) {
        long player_index = voice_get_controller_player_index(controller_index);
        if (player_index != -1 && !voice_player_has_voice_disabled(player_index) && !voice_player_has_voice_disallowed(player_index)) {
            unsigned long desired_mask = g_voice_availability.get_outgoing_player_mask(player_index);
            if (desired_mask) {
                s_voice_route route;
                s_voice_packet_header header = { 0 };
                route.routed_peer_mask = 0;
                memset(&route.repeater_peer_mask, 0, sizeof(route.repeater_peer_mask));
                g_voice_routing.get_voice_route_by_player_mask(desired_mask, &route);
                if (route.routed_peer_mask) {
                    bool team_only = false;
                    if (simulation_is_pending_start()) team_only = true;
                    else if (simulation_starting_up()) {
                        if (game_is_cooperative() || tls_game_globals()->options.game_mode == 1) team_only = true;
                        else if (tls_game_globals()->options.game_mode == 2) {
                            unsigned long recent = voice_observer_is_push_to_talk_active(&g_voice_observer, player_index);
                            if (voice_xenon_observer_has_push_to_talk(player_index) || recent) team_only = true;
                        }
                    }
                    if (team_only) header.sequence = 1;
                    const unsigned char *packet = data;
                    for (long packet_index = 0; packet_index < packet_count; packet_index++) {
                        if (g_voice_queue.enabled)
                            g_voice_queue.players[player_index].add_packet(player_index, packet, 10, &route, &header);
                        packet += 10;
                    }
                }
            }
        }
    }
}

void voice_interface_initialize_player_speaker_states(void) {
    for (long speaker_index = 0; speaker_index < 16; speaker_index++) {
        s_voice_player_speaker_state *speaker_state = &g_voice_interface_globals.speaker_states[speaker_index];
        speaker_state->voice = g_xaudio_dsp_manager.create_remote_voice_output(speaker_index);
        speaker_state->voice->SetEffectState(0, FALSE);
        speaker_state->enabled = false;
    }

    XAUDIOSOURCEVOICEINIT source_voice_init;
    XAUDIOCHANNELMAPENTRY channel_map_entry;
    XAUDIOCHANNELMAP channel_map;
    XAUDIOVOICEOUTPUTENTRY voice_output_entries[16];
    XAUDIOVOICEOUTPUT voice_output;
    memset(&source_voice_init, 0, sizeof(source_voice_init));
    channel_map_entry.InputChannel = 0;
    channel_map_entry.OutputChannel = 0;
    source_voice_init.pEffectChain = 0;
    channel_map_entry.Volume = 0.0f;
    channel_map.EntryCount = 1;
    source_voice_init.Format.SampleType = XAUDIOSAMPLETYPE_8BITPCM;
    source_voice_init.Format.SampleRate = 1;
    source_voice_init.Format.ChannelCount = 1;
    channel_map.paEntries = &channel_map_entry;
    source_voice_init.MaxPacketCount = 1;
    for (long output_index = 0; output_index < 16; output_index++) {
        voice_output_entries[output_index].pChannelMap = &channel_map;
        voice_output_entries[output_index].pDestVoice = g_voice_interface_globals.speaker_states[output_index].voice;
    }
    source_voice_init.MaxChannelMapEntryCount = 1;
    voice_output.EntryCount = 16;
    voice_output.paEntries = voice_output_entries;
    source_voice_init.MaxOutputVoiceCount = 16;
    source_voice_init.pVoiceOutput = &voice_output;
    XAudioCreateSourceVoice(&source_voice_init, &g_voice_interface_globals.speaker_source_voice);

    XAUDIOPACKET packet = { 0 };
    packet.BufferSize = 1;
    packet.LoopStart = 0;
    packet.LoopLength = 1;
    packet.pBuffer = g_voice_interface_globals.speaker_source_packet_data;
    packet.pContext = 0;
    packet.LoopCount = XAUDIOLOOPCOUNT_INFINITE;
    g_voice_interface_globals.speaker_source_voice->Start(0);
    g_voice_interface_globals.speaker_source_voice->SubmitPacket(&packet, 0);
}

// @address 0x3c2f70 voice_try_and_get_hopper_settings
// match: 0x3c2f70: pointer __restrict qualifier prevents alias reload
extern "C" bool voice_try_and_get_hopper_settings(s_voice_hopper_settings *__restrict settings) {
    c_network_session *session = 0;
    bool success = false;
    memset(settings, 0, sizeof(*settings));
    if (voice_xenon_get_active_session(&session)) {
        const c_network_session_parameters *parameters = &(*(c_network_session_parameters *)session->m_session_parameters);
        if (parameters->m_parameters.matchmaking_hopper.get_allowed()) {
            const s_network_session_matchmaking_hopper *hopper = parameters->m_parameters.matchmaking_hopper.get();
            settings->voice_mode = hopper->stats_write;
            settings->voice_option_40 = hopper->language_filter;
            settings->voice_option_44 = hopper->country_code_filter;
            settings->voice_option_48 = hopper->gamerzone_filter;
            settings->teams = hopper->team_game;
            success = true;
        }
    }
    return success;
}

// @address 0x3c0df8 voice_controller_can_communicate
// @address 0x3c0e60 voice_any_local_player_can_communicate

extern "C" unsigned char voice_controller_can_communicate(e_controller_index controller_index, long param) {
    bool result = false;
    if (voice_device_is_connected(controller_index)) {
        long player_index = g_voice_interface_globals.controller_states[controller_index].player_index;
        if (player_index != -1) {
            result = voice_players_can_communicate(player_index, param);
        }
    }
    return result;
}

extern "C" unsigned char voice_any_local_player_can_communicate(long param) {
    bool result = false;
    unsigned char initialized = g_voice_interface_globals.initialized;
    for (long player_index = 0; player_index < 16; player_index++) {
        bool in_session = false;
        if (initialized) in_session = voice_player_get_state(player_index)->status_flags.test(_voice_player_in_session);
        if (in_session) {
            bool local = false;
            if (initialized) local = voice_player_get_state(player_index)->status_flags.test(_voice_player_local);
            if (local && voice_players_can_communicate(player_index, param)) {
                result = true;
                break;
            }
        }
    }
    return result;
}

static __forceinline unsigned long voice_xenon_team_mask(long player_index, bool initialized) {
    unsigned long result = 0;
    if (initialized) result = g_voice_observer.m_player_team_mask[player_index];
    return result;
}
static __forceinline bool voice_xenon_flag(long player_index, long bit, bool initialized) {
    bool result = false;
    if (initialized) {
        unsigned long flags = 0;
        if (voice_player_get_state(player_index)->status_flags.test(_voice_player_in_session))
            flags = voice_player_get_state(player_index)->voice_flags;
        result = ((flags >> bit) & 1) != 0;
    }
    return result;
}
static __forceinline bool voice_xenon_lifecycle_matchmaking(bool initialized, long state) {
    bool result = false;
    bool matchmaking = false;
    if (initialized) matchmaking = state == 2;
    if (matchmaking) result = true;
    return result;
}
static __forceinline bool voice_xenon_game_lifecycle(bool initialized) {
    e_voice_lifecycle lifecycle = g_voice_interface_globals.lifecycle;
    bool result = false;
    if (initialized) {
        if (lifecycle > _voice_lifecycle_none && lifecycle <= _voice_lifecycle_in_game_pending) result = true;
    }
    return result;
}
static __forceinline bool voice_xenon_out_of_game_lifecycle(bool initialized) {
    bool result = false;
    if (initialized) {
        e_voice_lifecycle lifecycle = voice_get_lifecycle();
        if (lifecycle >= _voice_lifecycle_out_of_game && lifecycle <= _voice_lifecycle_out_of_game_pending) result = true;
    }
    return result;
}
// @address 0x3c2c00 voice_players_can_communicate
extern "C" bool voice_players_can_communicate(long player_index, long other_player_index) {
    bool communication = false;
    bool initialized = g_voice_interface_globals.initialized;
    bool observer_initialized = g_voice_observer.m_initialized;
    if (player_index == other_player_index) communication = true;
    else if (voice_xenon_lifecycle_in_game()) {
        if (g_voice_interface_globals.initialized && g_voice_interface_globals.lifecycle == _voice_lifecycle_in_game_pending) {
            unsigned long player_mask = voice_xenon_team_mask(player_index, observer_initialized);
            unsigned long other_mask = voice_xenon_team_mask(other_player_index, observer_initialized);
            communication = (player_mask & (1 << other_player_index)) != 0 && (other_mask & (1 << player_index)) != 0;
        } else communication = true;
    } else if (voice_xenon_out_of_game_lifecycle(initialized)) {
        communication = (g_voice_observer.get_player_out_of_game_mask(player_index) & (1 << other_player_index)) != 0 &&
            (g_voice_observer.get_player_out_of_game_mask(other_player_index) & (1 << player_index)) != 0;
    }
    if (voice_xenon_flag(player_index, 1, initialized) || voice_player_has_voice_disabled(other_player_index)) communication = false;
    if (voice_xenon_flag(player_index, 2, initialized) || voice_player_has_voice_disallowed(other_player_index)) communication = false;
    const long matchmaking_state = g_voice_interface_globals.matchmaking_state;
    if (communication && voice_xenon_lifecycle_matchmaking(initialized, matchmaking_state) && voice_player_has_alternate_voice_output(player_index)) {
        unsigned long team_mask = voice_xenon_team_mask(player_index, observer_initialized);
        communication = ((unsigned long)voice_get_player_peer_mask(player_index) & (1 << other_player_index)) != 0 ||
            (team_mask & (1 << other_player_index)) != 0;
    }
    if (communication && voice_xenon_lifecycle_matchmaking(initialized, matchmaking_state) && voice_player_has_alternate_voice_output(other_player_index)) {
        unsigned long team_mask = voice_xenon_team_mask(other_player_index, observer_initialized);
        communication = ((unsigned long)voice_get_player_peer_mask(other_player_index) & (1 << player_index)) != 0 ||
            (team_mask & (1 << player_index)) != 0;
    }
    return communication;
}

static __forceinline unsigned __int64 voice_xenon_player_xuid(const s_voice_player_state *player) {
    unsigned __int64 xuid = 0;
    c_network_session *session = 0;
    if (user_interface_session_get_usable_player_xuid(&session)) {
        long player_index = ::user_interface_session_get_player_index(
            (const s_player_identifier *)player, session);
        if (player_index != -1) xuid = user_interface_session_player_get_usable_player_xuid(player_index);
    }
    return xuid;
}
static __forceinline bool voice_xenon_online_muted(e_controller_index controller_index, unsigned __int64 xuid) {
    bool muted = false;
    if (::online_local_xuid_is_online_enabled()) {
        BOOL user_muted = FALSE;
        if (XUserMuteListQuery(controller_index, xuid, &user_muted) == ERROR_SUCCESS)
            muted = user_muted != FALSE;
    }
    return muted;
}

// @address 0x3c14f8 ?voice_interface_update_local_player_voice@@YAXJ@Z
void voice_interface_update_local_player_voice(long player_index) {
    e_controller_index controller_index = voice_player_get_controller_index(player_index);
    s_voice_controller_state *controller_state = &g_voice_interface_globals.controller_states[controller_index];
    const c_player_profile_interface *profile = (*controller_get(controller_index)).get_player_profile_interface();
    c_controller_interface *controller = &(*controller_get(controller_index));
    unsigned char *status_flags = &voice_player_get_state(player_index)->status_flags.m_storage;
    s_voice_player_state *player = voice_player_get_state(player_index);
    controller_state->input_preference = profile->voice.voice_mask;
    controller_state->voice_output = profile->voice.unknown_setting_104;
    controller_state->output_preference = profile->voice.voice_output_setting;
    bool disabled = profile->voice.voice_output_setting == 1;
    bool allowed = true;
    if (controller->is_silver_or_gold_live())
        allowed = user_interface_controller_voice_allowed(controller_index) && !user_interface_controller_has_blocked_communications();
    bool process = voice_device_is_connected(controller_index) && !disabled && allowed;
    if (!process) {
        voice_interface_set_player_processing_mode(player_index, _voice_processing_mode_none);
        voice_set_process_status(status_flags, false);
    } else {
        bool processing = voice_interface_set_player_processing_mode(player_index, _voice_processing_mode_chat);
        voice_set_process_status(status_flags, processing);
    }
    SET_BIT(player->voice_flags, 1, disabled);
    SET_BIT(player->voice_flags, 0, voice_device_is_connected(controller_index));
    SET_BIT(player->voice_flags, 2, !allowed);
    SET_BIT(player->voice_flags, 3, g_voice_interface_globals.pad3b8 != 0);
    SET_BIT(player->voice_flags, 4, controller_state->voice_output == 1);
    unsigned short muted_players = 0;
    for (long other_index = 0; other_index < 16; ++other_index) {
        s_voice_player_state *other = &g_voice_interface_globals.player_states[other_index];
        if (other_index != player_index && other->status_flags.test(_voice_player_in_session)) {
            unsigned __int64 xuid = voice_xenon_player_xuid(other);
            bool muted = controller->is_silver_or_gold_live() && !controller->is_guest() && voice_xenon_online_muted(controller_index, xuid);
            if ((user_interface_controller_voice_is_for_friends_only(controller_index) ||
                    user_interface_controller_has_friends_only_communications()) &&
                !online_user_is_friend(controller_index, xuid))
                muted = true;
            SET_BIT(muted_players, other_index, muted);
            if (muted_players & FLAG(other_index)) player->muted_players |= FLAG(other_index);
            else if (controller_state->muted_players & FLAG(other_index)) player->muted_players &= ~FLAG(other_index);
        }
    }
    controller_state->muted_players = muted_players;
    voice_interface_update_controller_network_properties(controller_index);
}

static __forceinline bool voice_xenon_lifecycle_in_matchmaking(void) {
    bool matchmaking = false;
    if (g_voice_interface_globals.initialized) matchmaking = g_voice_interface_globals.matchmaking_state == 2;
    return matchmaking;
}

static __forceinline long voice_xenon_get_observer_voice_state(long player_index, long *talking_state)
{
    long voice_state;
    if (voice_xenon_observer_has_push_to_talk(player_index))
        voice_state = 4;
    else
        voice_state = 3;

    if (voice_state == 3) {
        if (voice_observer_is_push_to_talk_active(&g_voice_observer, player_index) && voice_player_is_talking(player_index))
            *talking_state = 1;
    } else {
        if (voice_player_is_talking(player_index))
            *talking_state = 2;
    }
    return voice_state;
}

// @address 0x118b80 voice_get_player_voice_state
// match: 0x118b80: pointer __restrict qualifier prevents alias reload
extern "C" void voice_get_player_voice_state(long player_index, long * state, long *__restrict talking)
{
    long voice_state = 0;
    long talking_state = 0;
    do {
    if (g_voice_interface_globals.initialized && simulation_starting_up()) {
        if (voice_xenon_player_is_valid(player_index) && voice_xenon_player_is_local(player_index)) {
            if (!voice_xenon_lifecycle_in_game()) {
                voice_state = 2;
                break;
            }
            unsigned long flags = voice_get_player_flags(player_index);
            if (flags & 1) {
                if (flags & 2) {
                    voice_state = 1;
                    break;
                }
                if (flags & 8) {
                    voice_state = 1;
                    break;
                }
                if (flags & 4) {
                    voice_state = 2;
                    break;
                }
                s_voice_hopper_settings settings;
                if (voice_xenon_lifecycle_in_matchmaking() && voice_try_and_get_hopper_settings(&settings)) {
                    if (settings.voice_option_40 == 3 ||
                        (!settings.teams && !voice_enforcing_team_restrictions() && settings.voice_option_40 == 2)) {
                        voice_state = 2;
                        break;
                    }
                    if (!settings.teams && !voice_enforcing_team_restrictions() && voice_player_has_alternate_voice_output(player_index) &&
                        count_bits<unsigned long>(voice_get_player_peer_mask(player_index)) <= 1) {
                        voice_state = 1;
                        break;
                    }
                }
                voice_state = voice_xenon_get_observer_voice_state(player_index, &talking_state);
            }
        }
    }
    } while (false);
    *state = voice_state;
    *talking = talking_state;
}

// @address 0x808560 g_simulation_globals
