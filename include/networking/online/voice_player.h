#ifndef NETWORKING_ONLINE_VOICE_PLAYER_H
#define NETWORKING_ONLINE_VOICE_PLAYER_H

struct player_datum;

// Player lookup defined in tu/networkinglibxbox/voice_xenon.cpp.
extern "C" player_datum *voice_player_try_and_get_datum(long player_index);

#include "networking/online/voice_interface_types.h"

static __forceinline bool voice_player_is_valid(long player_index)
{
    bool valid = false;
    if (g_voice_interface_globals.initialized)
        valid = g_voice_interface_globals.player_states[player_index].status_flags.test(_voice_player_in_session);
    return valid;
}
static __forceinline bool voice_player_is_local(long player_index)
{
    bool local = false;
    if (g_voice_interface_globals.initialized)
        local = g_voice_interface_globals.player_states[player_index].status_flags.test(_voice_player_local);
    return local;
}

#endif // NETWORKING_ONLINE_VOICE_PLAYER_H
