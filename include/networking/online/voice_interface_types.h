#ifndef NETWORKING_ONLINE_VOICE_INTERFACE_TYPES_H
#define NETWORKING_ONLINE_VOICE_INTERFACE_TYPES_H

#include "cseries/cseries_wrapped.h"
#include "game/game_types.h"
#include "game/player_identifier_types.h"
#include "shell/shell_types.h"
#include "networking/voice_processing_mode_types.h"
#include <stddef.h>

enum e_voice_lifecycle
{
    _voice_lifecycle_none = 0,
    _voice_lifecycle_in_game = 1,
    _voice_lifecycle_in_game_pending = 2,
    _voice_lifecycle_out_of_game = 3,
    _voice_lifecycle_out_of_game_pending = 4
};
enum e_voice_player_status_flags { _voice_player_in_session = 0, _voice_player_local = 1, _voice_player_registered = 2, _voice_player_has_voice = 3 };
enum e_voice_controller_status_flags { _voice_controller_valid = 0 };
// match: 0x420e78: retain voice_xenon's original shared byte-flag body;
// its named Boolean result normalizes the flag test to true or false.
template<>
__forceinline bool c_flags_no_init<e_voice_player_status_flags, unsigned char, 4>::test(e_voice_player_status_flags bit) const
{
    bool result = (m_storage & (1 << bit)) != 0;
    return result;
}

struct IXAudioVoice;
struct IXAudioSourceVoice;
struct s_voice_player_state {
    // Complete session identifier for this player.
    // Keep the older word accessors for existing voice users.
    union {
        s_player_identifier identifier;
        struct { long player_index; long peer_index; };
    };
    e_game_team team_index;
    union { long user_index; long session_peer_index; };
    e_controller_index controller_index;
    e_voice_processing_mode processing_mode;
    union { IXAudioVoice *voice; int voice_mask; };
    c_flags_no_init<e_voice_player_status_flags, unsigned char, 4> status_flags;
    union {
        struct { unsigned short muted_players; unsigned short voice_flags; };
        unsigned long settings;
    };
};
struct s_voice_controller_state {
    long player_index;
    long input_preference;
    long voice_output;
    long output_preference;
    c_flags_no_init<e_voice_controller_status_flags, unsigned char, 1> status_flags;
    unsigned short muted_players;
};
struct s_voice_player_speaker_state { IXAudioVoice *voice; bool enabled; };
struct s_voice_interface_globals
{
    long repeater_peer_index;
    long repeater_usage;
    // Outgoing channel counts for game and lobby voice.
    // voice_get_outgoing_channel_count distinguishes game and lobby limits.
    long game_channel_count;
    long lobby_channel_count;
    e_voice_lifecycle lifecycle;
    long matchmaking_state;
    long local_peer_index;
    long host_peer_index;
    unsigned long local_peer_connectivity_mask;
    long observer_channel_peer_indices[32];
    s_voice_player_state player_states[16];
    s_voice_controller_state controller_states[4];
    s_voice_player_speaker_state speaker_states[16];
    IXAudioSourceVoice *speaker_source_voice;
    union { char pad3b8; bool voice_chat_away; };
    bool initialized;
    unsigned char speaker_source_packet_data[2];
};
extern s_voice_interface_globals g_voice_interface_globals;

#if defined(_M_PPCBE) || defined(_XBOX)
typedef char voice_interface_game_channels_offset[(offsetof(s_voice_interface_globals, game_channel_count) == 8) ? 1 : -1];
typedef char voice_interface_lobby_channels_offset[(offsetof(s_voice_interface_globals, lobby_channel_count) == 12) ? 1 : -1];
typedef char voice_player_state_size[(sizeof(void *) != 4 || (sizeof(s_voice_player_state) == 36)) ? 1 : -1];
typedef char voice_player_identifier_offset[(offsetof(s_voice_player_state, identifier) == 0) ? 1 : -1];
typedef char voice_player_session_peer_offset[(offsetof(s_voice_player_state, session_peer_index) == 0xc) ? 1 : -1];
typedef char voice_controller_output_offset[(offsetof(s_voice_controller_state, voice_output) == 8) ? 1 : -1];
typedef char voice_controller_state_size[(sizeof(s_voice_controller_state) == 20) ? 1 : -1];
typedef char voice_interface_globals_size[(sizeof(void *) != 4 || (sizeof(s_voice_interface_globals) == 0x3bc)) ? 1 : -1];
typedef char voice_interface_globals_players_offset[(sizeof(void *) != 4 || (offsetof(s_voice_interface_globals, player_states) == 0xa4)) ? 1 : -1];
#endif

#endif
