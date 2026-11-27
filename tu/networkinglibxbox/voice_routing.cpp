// networkinglibxbox/voice_routing.obj: from the records 0x41f0f0 0x41f158 0x41f430 0x41f6b0
// @flags /TP /O2 /Oc
// @address 0x41f430 ?consume_voice_channels_by_desired_peer_mask@c_voice_routing@@CAXPAKKPAGPAJ_NJPA_N@Z
// @address 0x3c0a58 ?voice_get_squad_session_peer_mask_by_group_session@@YAKXZ
// @address 0x809e78 ?g_voice_interface_globals@@3Us_voice_interface_globals@@A
// @address 0x108eee4 ?g_voice_routing@@3Vc_voice_routing@@A
// @address 0x41f0f0 ?get_voice_route_by_player_mask@c_voice_routing@@QAAXKPAUs_voice_route@@@Z
// @address 0x41f158 ?get_voice_route_by_peer_mask@c_voice_routing@@QAAXKPAUs_voice_route@@@Z
// @address 0x41f6b0 ?get_desired_peer_mask_by_player_mask@c_voice_routing@@AAAKK@Z

#include "networking/online/voice_xenon.h"
#include "networking/online/voice_routing.h"
#include "networking/online/voice_interface_types.h"



static __forceinline bool player_valid(long player_index)
{
    bool result = false;
    if (g_voice_interface_globals.initialized)
        result = g_voice_interface_globals.player_states[player_index].status_flags.m_storage & 1;
    return result;
}
static __forceinline bool player_hidden(long player_index)
{
    bool result = false;
    if (g_voice_interface_globals.initialized)
        result = (g_voice_interface_globals.player_states[player_index].status_flags.m_storage >> 1) & 1;
    return result;
}
static __forceinline long player_peer_index(long player_index)
{
    long result = -1;
    if (g_voice_interface_globals.initialized && (g_voice_interface_globals.player_states[player_index].status_flags.m_storage & 1))
        result = g_voice_interface_globals.player_states[player_index].user_index;
    return result;
}

unsigned long c_voice_routing::get_desired_peer_mask_by_player_mask(unsigned long player_mask)
{
    unsigned long peer_mask = 0;
    for (long player_index = 0; player_index < 16; player_index++) {
        if ((player_mask & (1 << player_index)) && player_valid(player_index) && !player_hidden(player_index))
            peer_mask |= 1 << player_peer_index(player_index);
    }
    return peer_mask;
}

void c_voice_routing::consume_voice_channels_by_desired_peer_mask(unsigned long * desired_peer_mask,
    // match: 0x41f430: keeps *desired_peer_mask in r30 across the store (andc reuses it, no lwz) and the decremented count in a register
    unsigned long eligible_peer_mask, unsigned short *__restrict routed_peer_mask, long *__restrict available_outgoing_voice_channel_count,
    // match: 0x41f430: removing it swaps the r30/r31 color assignment across the loop body and reorders the callers' init stores
    bool voice_repeater_available, long repeater_peer_index, bool *__restrict use_voice_repeater)
{
    for (long peer_index = 0; peer_index < 16; peer_index++) {
        if ((*desired_peer_mask & (1 << peer_index)) && (eligible_peer_mask & (1 << peer_index)) && *available_outgoing_voice_channel_count > 0) {
            if (voice_repeater_available && repeater_peer_index == peer_index)
                continue;
            if (*available_outgoing_voice_channel_count == 1 && voice_repeater_available) {
                bool more_peers = (1 << peer_index) != *desired_peer_mask;
                *use_voice_repeater = *use_voice_repeater || more_peers;
                if (*use_voice_repeater)
                    break;
            }
            *routed_peer_mask |= 1 << peer_index;
            *desired_peer_mask &= ~(1 << peer_index);
            (*available_outgoing_voice_channel_count)--;
        }
    }
}

// match: 0x41f0f0: hoists the m_initialized lbz/cmplwi between the two voice_route zero-init sths (without it the load sinks below both stores)
void c_voice_routing::get_voice_route_by_player_mask(unsigned long player_mask, s_voice_route *__restrict voice_route)
{
    voice_route->repeater_peer_mask = 0;
    voice_route->routed_peer_mask = 0;
    if (g_voice_routing.m_initialized && player_mask != 0) {
        unsigned long peer_mask = g_voice_routing.get_desired_peer_mask_by_player_mask(player_mask);
        if (peer_mask != 0)
            g_voice_routing.get_voice_route_by_peer_mask(peer_mask, voice_route);
    }
}


static __forceinline int voice_routing_lifecycle_outside_lobby(long lifecycle)
{
    if (lifecycle < 3)
        return true;
    if (lifecycle > 4)
        return true;
    return false;
}
static __forceinline int voice_routing_lifecycle_outside_game(long lifecycle)
{
    if (lifecycle <= 0)
        return true;
    if (lifecycle > 2)
        return true;
    return false;
}
static __forceinline long voice_routing_get_outgoing_channel_count()
{
    long result = 0;
    if (g_voice_interface_globals.initialized) {
        long lifecycle = g_voice_interface_globals.lifecycle;
        bool in_lobby = false;
        if (!voice_routing_lifecycle_outside_lobby(lifecycle))
            in_lobby = true;
        if (in_lobby)
            result = g_voice_interface_globals.lobby_channel_count;
        else {
            bool in_game = false;
            if (!voice_routing_lifecycle_outside_game(lifecycle))
                in_game = true;
            if (in_game)
                result = g_voice_interface_globals.game_channel_count;
            else
                result = 0;
        }
    }
    return result;
}
static __forceinline long voice_routing_get_repeater_peer_index()
{
    long result = 0;
    if (g_voice_interface_globals.initialized)
        result = g_voice_interface_globals.repeater_peer_index;
    return result;
}
static __forceinline bool voice_routing_local_machine_is_voice_repeater()
{
    bool result = false;
    if (g_voice_interface_globals.initialized) {
        long local_peer_index = g_voice_interface_globals.local_peer_index;
        result = local_peer_index != -1 && local_peer_index == voice_routing_get_repeater_peer_index();
    }
    return result;
}
static __forceinline bool voice_routing_repeater_available()
{
    bool result = false;
    if (g_voice_interface_globals.initialized && !voice_routing_local_machine_is_voice_repeater())
        result = voice_routing_get_repeater_peer_index() != -1;
    return result;
}
static __forceinline long voice_routing_get_repeater_usage()
{
    long result = 0;
    if (g_voice_interface_globals.initialized)
        result = g_voice_interface_globals.repeater_usage;
    return result;
}
static __forceinline unsigned long voice_routing_get_local_peer_connectivity_mask()
{
    unsigned long result = 0;
    if (g_voice_interface_globals.initialized)
        result = g_voice_interface_globals.local_peer_connectivity_mask;
    return result;
}

// match: 0x41f158: hoists the m_initialized lbz above the voice_route zero-init stores (without it the load sinks below both sths)
void c_voice_routing::get_voice_route_by_peer_mask(unsigned long peer_mask, s_voice_route *__restrict voice_route)
{
    voice_route->repeater_peer_mask = 0;
    voice_route->routed_peer_mask = 0;
    if (g_voice_routing.m_initialized && peer_mask != 0) {
        long available_outgoing_voice_channel_count = voice_routing_get_outgoing_channel_count();
        if (available_outgoing_voice_channel_count > 0) {
            bool voice_repeater_available = voice_routing_repeater_available();
            long repeater_peer_index = voice_routing_get_repeater_peer_index();
            long repeater_usage = voice_routing_get_repeater_usage();
            unsigned long local_peer_connectivity_mask = voice_routing_get_local_peer_connectivity_mask();
            unsigned short routed_peer_mask = 0;
            unsigned short repeater_peer_mask = 0;
            bool use_voice_repeater = false;
            if (!(local_peer_connectivity_mask & (1 << repeater_peer_index)))
                voice_repeater_available = false;
            if (voice_repeater_available) {
                if (repeater_usage == 1 || repeater_usage == 2) {
                    use_voice_repeater = true;
                    available_outgoing_voice_channel_count = 1;
                }
            } else if (repeater_usage == 2) {
                available_outgoing_voice_channel_count = 1;
            }
            if (!use_voice_repeater) {
                long session = 0;
                if (g_voice_interface_globals.initialized)
                    session = g_voice_interface_globals.matchmaking_state;
                repeater_peer_mask = (unsigned short)(peer_mask & ~local_peer_connectivity_mask);
                peer_mask &= local_peer_connectivity_mask;
                if (repeater_peer_mask)
                    use_voice_repeater = true;
                unsigned long squad_session_peer_mask = 0;
                if (session == 2)
                    squad_session_peer_mask = voice_get_squad_session_peer_mask_by_group_session();
                if (session == 2) {
                    consume_voice_channels_by_desired_peer_mask(&peer_mask, squad_session_peer_mask, &routed_peer_mask,
                        &available_outgoing_voice_channel_count, voice_repeater_available, repeater_peer_index, &use_voice_repeater);
                }
                if (!(available_outgoing_voice_channel_count == 1 && voice_repeater_available && use_voice_repeater)) {
                        consume_voice_channels_by_desired_peer_mask(&peer_mask, ~0u, &routed_peer_mask,
                            &available_outgoing_voice_channel_count, voice_repeater_available, repeater_peer_index, &use_voice_repeater);
                }
            }
            if (voice_repeater_available) {
                if (use_voice_repeater) {
                    routed_peer_mask |= 1 << repeater_peer_index;
                    repeater_peer_mask |= peer_mask;
                } else if (peer_mask & (1 << repeater_peer_index)) {
                    routed_peer_mask |= 1 << repeater_peer_index;
                    repeater_peer_mask |= 1 << repeater_peer_index;
                }
            } else {
                repeater_peer_mask = 0;
            }
            voice_route->routed_peer_mask = routed_peer_mask;
            voice_route->repeater_peer_mask = repeater_peer_mask;
        }
    }
}
