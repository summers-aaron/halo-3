#ifndef NETWORKING_ONLINE_VOICE_XENON_INLINE_H
#define NETWORKING_ONLINE_VOICE_XENON_INLINE_H

#include "networking/logic/life_cycle/life_cycle_globals_types.h"

static __forceinline const s_life_cycle_globals &voice_life_cycle_globals()
{
    return g_network_life_cycle_globals;
}
#include "networking/session/network_session_parameters_generic.h"
#include "networking/session/network_session_parameter_matchmaking_types.h"
template <>
bool c_generic_network_session_parameter<e_campaign_difficulty_level>::set(const e_campaign_difficulty_level *parameter);
template<typename T> unsigned long count_bits(T value);

static __forceinline s_voice_player_state *voice_player_get_state(long player_index) {
    s_voice_player_state *player_state = 0;
    player_state = &g_voice_interface_globals.player_states[player_index];
    return player_state;
}
static __forceinline bool voice_player_has_voice(long player_index) {
    bool has_voice = false;
    if (g_voice_interface_globals.initialized)
        has_voice = voice_player_get_state(player_index)->status_flags.test(_voice_player_has_voice);
    return has_voice;
}
static __forceinline unsigned long voice_get_player_flags(long player_index) {
    unsigned long flags = 0;
    if (g_voice_interface_globals.initialized) {
        if (voice_player_get_state(player_index)->status_flags.test(_voice_player_in_session)) {
            s_voice_player_state *player_state = voice_player_get_state(player_index);
            flags = player_state->voice_flags;
        }
    }
    return flags;
}
static __forceinline long voice_get_controller_player_index(e_controller_index controller_index) {
    long player_index = -1;
    if (g_voice_interface_globals.initialized && g_voice_interface_globals.controller_states[controller_index].status_flags.test(_voice_controller_valid))
        player_index = g_voice_interface_globals.controller_states[controller_index].player_index;
    return player_index;
}
static __forceinline bool voice_xenon_player_is_valid(long player_index) {
    bool valid = false;
    if (g_voice_interface_globals.initialized)
        valid = voice_player_get_state(player_index)->status_flags.test(_voice_player_in_session);
    return valid;
}
static __forceinline bool voice_xenon_player_is_local(long player_index) {
    bool local = false;
    if (g_voice_interface_globals.initialized)
        local = voice_player_get_state(player_index)->status_flags.test(_voice_player_local);
    return local;
}
#endif
