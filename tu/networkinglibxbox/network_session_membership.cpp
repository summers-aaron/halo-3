// networkinglibxbox/network_session_membership.obj: from the records 0x3c95c8 0x3c96c8 0x3c9730 0x3c9868 0x3c9928 0x3c99d8 0x3c9c08 0x3c9c78 0x3c9d48 0x3c9e28 0x3c9ec8 0x3c9f90 0x3c9fe0 0x3ca038 0x3ca0e0 0x3ca298 0x3ca380 0x3ca410 0x3ca610 0x3ca6c8 0x3ca8a0 0x3ca988 0x3caa48 0x3caad8 0x3cabf8 0x3cac90 0x3cace0 0x3cad38 0x3cb3f0 0x3cbbb0 0x3cbc88 0x3cbd38 0x3cbda0 0x3cbfd8 0x3cc0b0 0x3cc438 0x3cc510 0x3cc5e0 0x3cc6c0 0x3cc748
// @flags /TP /O2 /Oc
#include "cseries/crt_xdk.h"
#include "networking/session/network_session_peer_state_types.h"
#include "networking/session/network_session_peer_properties_status_flags_types.h"
#include "networking/session/network_game_countdown_delayed_reason_types.h"
#include "networking/network_observer_owner_types.h"
#include "networking/logic/life_cycle/peer_property_flag_test_types.h"
#include "game/player_types.h"
#include "game/player_configuration.h"
#include "networking/network_configuration_types.h"
#include "shell/shell_types.h"
#include "cseries/cseries.h"
#include "cseries/bit_operations.h"
#include <string.h>
#include "game/multiplayer_game_hopper.h"
#include "networking/session/network_managed_session.h"
#include "networking/transport/transport_security.h"
#include "networking/session/network_session_parameters.h"
#include "networking/transport/transport_address.h"
#include "networking/transport/transport_security_types.h"
#include "networking/transport/transport_platform_types.h"
#include "networking/transport/transport_unique_identifier_types.h"
#include "networking/messages/network_messages_session_membership.h"
#include "networking/delivery/network_channel.h"
#include "networking/session/network_session_membership_types.h"
#include "networking/online/online_managed_session_types.h"
#include "networking/online/online_managed_session_access.h"
#include "networking/network_time_types.h"
#include "networking/network_configuration.h"
#include "networking/session/network_session_membership.h"
#include "networking/session/network_session.h"
#include "networking/network_globals.h"
#include "networking/network_time_since_access.h"
#include "networking/session/network_session_membership_queries.h"
#include "networking/session/network_session_parameter_observer.h"
// @address 0x45a5c k_network_peer_leave_quality_reason
#include "networking/session/network_session_strings.h"
#include "networking/transport/transport_security_globals.h"
#include "networking/transport/transport_unique_identifier_accessors.h"
#include "text/unicode.h"
// @address 0x3a0e00 ?transport_secure_address_get@@YA_NPAUs_transport_secure_address@@@Z
// @address 0x3a0fd8 ?transport_secure_address_compare@@YA_NPBUs_transport_secure_address@@0@Z
// @address 0x3c96c8 ?set_slot_counts@c_network_session_membership@@QAAXJJ_N@Z
// @address 0x3c9730 ?reset_membership@c_network_session_membership@@QAAXXZ
// @address 0x3c9868 ?reset_transmitted_membership_state@c_network_session_membership@@QAAXXZ
// @address 0x3c9c08 ?get_peer_from_observer_channel@c_network_session_membership@@QBAJJ@Z
// @address 0x3c9d48 ?get_peer_from_unique_identifier@c_network_session_membership@@QBAJPBUs_transport_unique_identifier@@@Z
// @address 0x3c9f90 ?get_first_peer@c_network_session_membership@@QBAJXZ
// @address 0x3c9fe0 ?get_next_peer@c_network_session_membership@@QBAJJ@Z
// @address 0x3ca038 network_session_membership_get_common_determinism_version
// @address 0x3fda08 ?managed_session_modify_slot_counts@@YAXJJJ_N0@Z
// @address 0x57b530 ?multiplayer_game_hopper_build_calculated_stats@@YAXPBUs_queried_player_global_statistics@@PBUs_queried_player_hopper_statistics@@PAUs_calculated_player_statistics@@@Z
// @address 0x80608 GetTickCount
// @address 0x827adcc8 ?online_session_manager_globals@@3Us_online_session_manager_globals@@A
// @address 0x3ca0e0 ?add_peer_internal@c_network_session_membership@@AAA_NJW4e_network_session_peer_state@@JPBUs_transport_secure_address@@_K2K@Z
// @address 0x3ca610 ?get_player_from_identifier@c_network_session_membership@@QBAJPBUs_player_identifier@@@Z
// @address 0x3cabf8 ?does_player_exist@c_network_session_membership@@QBA_NPBUs_player_identifier@@JJ@Z
// @address 0x3cac90 ?get_first_player@c_network_session_membership@@QBAJXZ
// @address 0x3cace0 ?get_next_player@c_network_session_membership@@QBAJJ@Z
// @address 0x3cbd38 ?peer_property_flag_test_any_peer@c_network_session_membership@@QBA_NW4e_network_session_peer_properties_status_flags@@@Z
// @address 0x3cbfd8 ?update_player_configuration_from_host@c_network_session_membership@@AAAXJ@Z
// @address 0x3cc6c0 ?find_player_in_player_add_queue@c_network_session_membership@@ABAJPBUs_player_identifier@@@Z
// @address 0x3ca298 ?remove_peer_internal@c_network_session_membership@@AAAXJ@Z
// @address 0x3ca8a0 ?remove_player@c_network_session_membership@@QAAXJ@Z
// @address 0x3ca988 ?add_player_internal@c_network_session_membership@@AAAPAUs_network_session_player@@JPBUs_player_identifier@@JJJ_N@Z
// @address 0x3caa48 ?remove_player_internal@c_network_session_membership@@AAAXJ@Z
// @address 0x3caad8 ?set_player_properties@c_network_session_membership@@QAAXJJW4e_controller_index@@PBUs_player_configuration_from_client@@K@Z
// @address 0x3cbc88 ?peer_property_flag_test@c_network_session_membership@@QBA_NW4e_peer_property_flag_test_type@@W4e_network_session_peer_properties_status_flags@@@Z
// @address 0x58e090 memcpy
// @address 0x3cc510 ?remove_player_from_player_add_queue@c_network_session_membership@@QAAXPBUs_player_identifier@@@Z
// @address 0x3cc748 ??$fast_checksum@Us_network_session_shared_membership@@@@YAKKPBUs_network_session_shared_membership@@@Z
// @address 0x3c99d8 ?remove_peer@c_network_session_membership@@QAAXJ@Z
// @address 0x3ca6c8 ?find_or_add_player@c_network_session_membership@@QAAJJJPBUs_player_identifier@@_K_N2222@Z


class c_network_session_membership;

#define k_network_maximum_machines_per_session 16
#define BIT_VECTOR_TEST_FLAG(vector, bit) (((vector)[(bit) >> 5] & (1 << ((bit) & 31))) != 0)
namespace {
static __forceinline void set_membership_flag(unsigned long *flags, long index, bool value)
{
    if (value)
        flags[index >> 5] |= 1 << (index & 31);
    else
        flags[index >> 5] &= ~(1 << (index & 31));
}
static __forceinline bool test_peer_property(unsigned int flags, e_network_session_peer_properties_status_flags flag)
{
    bool result = (flags & (1 << flag)) != 0;
    return result;
}
}

// @address 0x8c447c g_network_time_globals

__forceinline void c_network_session_membership::clear()
{
    c_network_session *saved_session = m_session;
    long saved_update_number = m_local_membership_update_number;
    memset(this, 0, sizeof(*this));
    m_session = saved_session;
    m_local_membership_update_number = saved_update_number;
    m_current_membership.update_number = -1;
    m_current_membership.host_peer_index = -1;
    m_transmitted_membership.update_number = -1;
    for (unsigned long peer_index = 0; peer_index < 16; ++peer_index) {
        memset(m_current_membership.peers[peer_index].user_player_indices, -1, sizeof(m_current_membership.peers[peer_index].user_player_indices));
        memset(m_transmitted_membership.peers[peer_index].user_player_indices, -1, sizeof(m_transmitted_membership.peers[peer_index].user_player_indices));
    }
    m_host_transition_peer_index = -1;
    m_local_peer_index = -1;
    for (unsigned long peer_index = 0; peer_index < 16; ++peer_index) {
        m_peer_channels[peer_index].membership_update_number = -1;
        m_peer_channels[peer_index].observer_channel_index = -1;
    }
}

void c_network_session_membership::set_slot_counts(long public_slot_count, long private_slot_count, bool friends_only)
{
    m_current_membership.private_slot_count = public_slot_count;
    m_current_membership.public_slot_count = private_slot_count;
    m_current_membership.friends_only = friends_only;
    if (get_session() != 0)
        managed_session_modify_slot_counts(get_session()->managed_session_index(),
            m_current_membership.private_slot_count, m_current_membership.public_slot_count,
            m_current_membership.friends_only, m_current_membership.request_modification);
    increment_update();
}

void c_network_session_membership::reset_membership()
{
    for (long player_index = 0; player_index < 16; ++player_index) {
        if (is_player_valid(player_index))
            remove_player_internal(player_index);
    }
    for (long peer_index = 0; peer_index < 16; ++peer_index) {
        if (is_peer_valid(peer_index))
            remove_peer_internal(peer_index);
    }
    clear();
}

// c_network_session_membership::reset_transmitted_membership_state
void c_network_session_membership::reset_transmitted_membership_state()
{
    memset(&m_transmitted_membership, 0, sizeof(m_transmitted_membership));
    m_transmitted_membership.update_number = -1;
    m_host_transition_peer_index = -1;

    for (long peer_index = 0; peer_index < 16; peer_index++)
        memset(m_transmitted_membership.peers[peer_index].user_player_indices, -1,
            sizeof(m_transmitted_membership.peers[peer_index].user_player_indices));

    s_peer_channel *entry = m_peer_channels;
    int index = 0;
    do {
        if ((1U << (index & 31)) & m_current_membership.valid_peer_mask.m_storage[index >> 5]) {
            entry->membership_update_number = -1;
            entry->peer_needs_reestablishment = 0;
        }
        index++;
        entry++;
    } while ((unsigned int)index < 16U);
}

// @address 0x3c9928 ?add_peer@c_network_session_membership@@QAA_NJW4e_network_session_peer_state@@JPBUs_transport_secure_address@@_K2@Z
// c_network_session_membership::add_peer
bool c_network_session_membership::add_peer(long peer_index, e_network_session_peer_state connection_state, long peer_version,
    const s_transport_secure_address *secure_address, unsigned __int64 join_nonce, unsigned __int64 party_nonce)
{
    unsigned long join_start_time = network_time_get();
    bool success = add_peer_internal(peer_index, connection_state, peer_version, secure_address, join_nonce, party_nonce, join_start_time);
    if (success) {
        get_session()->get_session_parameters()->reset_peer_state(peer_index);
        increment_update();
    }
    return success;
}

void c_network_session_membership::remove_peer(long peer_index)
{
    s_network_session_peer *peer = get_raw_peer(peer_index);
    c_network_session *network_session = get_session();

    {
        long xuid_count = 0;
        unsigned __int64 xuids[4];
        for (unsigned long user_index = 0; user_index < 4; user_index++) {
            long player_index = peer->user_player_indices[user_index];
            if (player_index != -1) {
                xuids[xuid_count] = m_current_membership.players[player_index].configuration.client.xuid;
                xuid_count++;
            }
        }
        s_online_managed_session *managed_session = online_session_get(get_session()->managed_session_index());
        remove_from_player_list(managed_session->desired.players, 16, xuids, xuid_count);
        operation_flag_set(managed_session->secondary_flags, 6, true);
        operation_flag_set(managed_session->secondary_flags, 7, true);
        session_flag_set(managed_session->flags, 2, false);
        managed_session->modification_time = 0;
    }

    for (long user_index = 0; user_index < 4; user_index++) {
        long player_index = peer->user_player_indices[user_index];
        if (player_index != -1)
            remove_player_internal(player_index);
    }
    remove_peer_internal(peer_index);

    network_session->get_session_parameters()->reset_peer_state(peer_index);
    if (network_session->established())
        network_session->get_session_parameters()->m_parameters.countdown_timer.set(false, 0, _network_game_countdown_delayed_reason_none, 0);

    if (m_current_membership.leader_peer_index == peer_index) {
        long new_leader_peer_index = -1;
        long new_leader_time = -1;
        for (long next_peer_index = (this)->get_first_peer(); next_peer_index != -1;
            next_peer_index = (this)->get_next_peer(next_peer_index))
        {
            s_network_session_peer *next_peer = get_peer(next_peer_index);
            if (new_leader_peer_index == -1) {
                new_leader_peer_index = next_peer_index;
                new_leader_time = network_time_since(next_peer->join_start_time);
            } else {
                long time = network_time_since(next_peer->join_start_time);
                if (time < new_leader_time) {
                    new_leader_peer_index = next_peer_index;
                    new_leader_time = time;
                }
            }
        }
        if (new_leader_peer_index != -1) {
            m_current_membership.leader_peer_index = new_leader_peer_index;
        }
    }

    increment_update();
}

// c_network_session_membership::get_peer_from_observer_channel
long c_network_session_membership::get_peer_from_observer_channel(long observer_channel) const
{
    long peer_index = -1;
    if (observer_channel != -1) {
        peer_index = (this)->get_first_peer();
        while (peer_index != -1 && m_peer_channels[peer_index].observer_channel_index != observer_channel) {
            peer_index = (this)->get_next_peer(peer_index);
        }
    }
    return peer_index;
}

// c_network_session_membership::get_peer_from_unique_identifier
long c_network_session_membership::get_peer_from_unique_identifier(const s_transport_unique_identifier *identifier) const
{
    long result = -1;
    const c_network_session *network_session = get_session();
    if (!network_session->disconnected() && has_membership()) {
        for (long peer_index = (this)->get_first_peer(); peer_index != -1;
            peer_index = (this)->get_next_peer(peer_index))
        {
            s_transport_unique_identifier peer_identifier;
            transport_secure_address_extract_identifier(get_peer_address(peer_index), &peer_identifier);
            if (memcmp(&peer_identifier, identifier, sizeof(peer_identifier)) == 0) { result = peer_index; break; }
        }
    }
    return result;
}

// @address 0x3c9e28 ?get_peer_from_secure_address@c_network_session_membership@@QBAJPBUs_transport_secure_address@@@Z
long c_network_session_membership::get_peer_from_secure_address(const s_transport_secure_address *secure_address) const
{
    long result = -1;
    const c_network_session *network_session = get_session();
    if (!network_session->disconnected() && has_membership()) {
        for (long peer_index = (this)->get_first_peer(); peer_index != -1;
            peer_index = (this)->get_next_peer(peer_index))
        {
            if (transport_secure_address_compare(secure_address, get_peer_address(peer_index))) {
                result = peer_index;
                break;
            }
        }
    }
    return result;
}

// @address 0x3c9ec8 ?host_exists_at_incoming_address@c_network_session_membership@@QBA_NPBUtransport_address@@@Z
bool c_network_session_membership::host_exists_at_incoming_address(const transport_address *incoming_address) const
{
    bool result = false;
    const c_network_session *network_session = get_session();
    bool waiting_for_host_connection = false;
    if (network_session->m_local_state == 2)
        waiting_for_host_connection = transport_address_equivalent(incoming_address,
            &network_session->m_local_state_data.peer_joining.join_usable_address, false);
    if (waiting_for_host_connection) {
        result = true;
    } else if (network_session->established() || network_session->m_local_state == 2) {
        long peer_index = get_peer_from_incoming_address(incoming_address);
        if (peer_index != -1 && peer_index != m_local_peer_index && peer_index == host_peer_index())
            result = true;
    }
    return result;
}

// c_network_session_membership::get_first_peer
long c_network_session_membership::get_first_peer(void) const
{
    long result = -1;
    for (long peer_index = 0; peer_index < k_network_maximum_machines_per_session; peer_index++) {
        if (BIT_VECTOR_TEST_FLAG(this->m_current_membership.valid_peer_mask.m_storage, peer_index)) {
            result = peer_index;
            break;
        }
    }
    return result;
}

// c_network_session_membership::get_next_peer
long c_network_session_membership::get_next_peer(long peer_index) const
{
    long result = -1;
    for (long next_peer_index = peer_index + 1; next_peer_index < k_network_maximum_machines_per_session; next_peer_index++) {
        if (BIT_VECTOR_TEST_FLAG(this->m_current_membership.valid_peer_mask.m_storage, next_peer_index)) {
            result = next_peer_index;
            break;
        }
    }
    return result;
}

extern "C" {
// c_network_session_membership: the determinism version every peer can run, or -1
extern "C" long network_session_membership_get_common_determinism_version(const c_network_session_membership *membership)
{
    long result = -1;
    long determinism_version = -1;
    long determinism_compatible_version = -1;
    for (long peer_index = (membership)->get_first_peer(); peer_index != -1;
        peer_index = (membership)->get_next_peer(peer_index))
    {
        const s_network_session_peer *peer = &membership->m_current_membership.peers[peer_index];
        if (determinism_version == -1 || peer->properties.determinism_version < determinism_version)
            determinism_version = peer->properties.determinism_version;
        if (determinism_compatible_version == -1 || peer->properties.determinism_compatible_version > determinism_compatible_version)
            determinism_compatible_version = peer->properties.determinism_compatible_version;
    }
    if (determinism_compatible_version <= determinism_version)
        result = determinism_version;
    return result;
}
}

static __forceinline long network_session_membership_find_observer_channel(c_network_observer *observer, e_network_observer_owner owner,
    const s_transport_secure_address *secure_address)
{
    long channel_index = -1;
    for (long index = 0; index < 32; index++) {
        c_network_observer::s_channel_observer *channel = &observer->m_channel_observers[index];
        if (channel->state && (channel->owner_flags & (1 << owner)) && transport_secure_address_compare(&channel->secure_identifier, secure_address)) {
            channel_index = index;
            break;
        }
    }
    return channel_index;
}

bool c_network_session_membership::add_peer_internal(long peer_index, e_network_session_peer_state connection_state, long peer_version,
    const s_transport_secure_address *secure_address, unsigned __int64 join_nonce, unsigned __int64 party_nonce, unsigned long join_start_time)
{
    bool success = true;
    s_network_session_peer *peer = &m_current_membership.peers[peer_index];
    s_peer_channel *transmitted = &m_peer_channels[peer_index];
    c_network_session *network_session = m_session;
    c_network_observer *observer = network_session ? network_session->m_observer : 0;
    long observer_index = -1;
    s_transport_secure_address own_address;
    if (!transport_secure_address_get(&own_address))
        memset(&own_address, 0, sizeof(own_address));
    if (network_session && !transport_secure_address_compare(secure_address, &own_address)) {
        observer_index = network_session_membership_find_observer_channel(observer, network_session->observer_owner(), secure_address);
        if (observer_index == -1)
            observer_index = observer->allocate_observer(network_session->observer_owner(), secure_address);
        if (observer_index == -1)
            success = false;
    }
    if (success) {
        m_current_membership.peer_count++;
        set_membership_flag(m_current_membership.valid_peer_mask.m_storage, peer_index, true);
        peer->connection_state = connection_state;
        peer->peer_version = peer_version;
        memcpy(&peer->secure_address, secure_address, sizeof(*secure_address));
        peer->party_nonce = join_nonce;
        peer->join_nonce = party_nonce;
        peer->join_start_time = join_start_time;
        transmitted->observer_channel_index = observer_index;
    }
    return success;
}

void c_network_session_membership::remove_peer_internal(long peer_index)
{
    s_network_session_peer *peer = get_raw_peer(peer_index);
    s_peer_channel *transmitted = &m_peer_channels[peer_index];
    const c_network_session *network_session = get_session();
    c_network_observer *observer = (network_session ? network_session->m_observer : 0);
    for (unsigned int player_index = 0; player_index < 4; ++player_index) {}
    if (transmitted->observer_channel_index != -1)
        observer->observer_channel_release(network_session->observer_owner(), transmitted->observer_channel_index);
    memset(transmitted, 0, sizeof(*transmitted));
    transmitted->membership_update_number = -1;
    transmitted->observer_channel_index = -1;
    memset(peer, 0, sizeof(*peer));
    memset(peer->user_player_indices, -1, sizeof(peer->user_player_indices));
    set_membership_flag(m_current_membership.valid_peer_mask.m_storage, peer_index, false);
    --m_current_membership.peer_count;
}

// @address 0x3ca380 ?set_peer_address@c_network_session_membership@@QAAXJPBUs_transport_secure_address@@@Z
// c_network_session_membership::set_peer_address
void c_network_session_membership::set_peer_address(long peer_index, const s_transport_secure_address *address)
{
    s_network_session_peer *peer = get_peer(peer_index);
    if (memcmp(&peer->secure_address, address, sizeof(s_transport_secure_address)) != 0)
    {
        memcpy(&peer->secure_address, address, sizeof(s_transport_secure_address));
        increment_update();
    }
}

// @address 0x3ca410 network_session_membership_is_synchronized
extern "C" bool network_session_membership_is_synchronized(const c_network_session_membership *membership)
{
    bool result = true;
    unsigned long valid_peer_mask = membership->m_current_membership.valid_peer_mask.m_storage[0];
    for (long peer_index = (membership)->get_first_peer(); peer_index != -1;
        peer_index = (membership)->get_next_peer(peer_index))
    {
        const s_network_session_peer *peer = &membership->m_current_membership.peers[peer_index];
        if ((valid_peer_mask & ~peer->properties.connectivity.peer_probe_mask) != 0 || peer->properties.downstream_bandwidth == 0) {
            result = false;
            break;
        }
    }
    return result;
}

// c_network_session_membership::get_player_from_identifier
long c_network_session_membership::get_player_from_identifier(const s_player_identifier *identifier) const
{
    long player_index = -1;
    const c_network_session *network_session = get_session();
    if ((network_session == 0 || network_session->m_local_state != 0) &&
        m_current_membership.update_number != -1) {
        player_index = (this)->get_first_player();
        while (player_index != -1) {
            if (memcmp(identifier->data, m_current_membership.players[player_index].identifier.data, sizeof(identifier->data)) == 0)
                break;
            player_index = (this)->get_next_player(player_index);
        }
    }
    return player_index;
}

long c_network_session_membership::find_or_add_player(long peer_index, long user_index, const s_player_identifier *player_identifier,
    unsigned __int64 player_xuid, bool is_online, bool is_silver, bool is_guest, bool is_bot, bool left_game)
{
    c_network_session *network_session = get_session();
    s_network_session_peer *peer = get_peer(peer_index);
    long player_index = peer->user_player_indices[user_index];
    if (player_index != -1) {
        if (memcmp(&m_current_membership.players[player_index].identifier, player_identifier, sizeof(s_player_identifier)) != 0) {
            remove_player(player_index);
            player_index = -1;
        }
    }

    if (player_index == -1) {
        long existing_player_index = get_player_from_identifier(player_identifier);
        if (existing_player_index != -1)
            remove_player(existing_player_index);

        for (long new_player_index = 0; new_player_index < 16; new_player_index++) {
            if (!is_player_valid(new_player_index)) {
                player_index = new_player_index;
                break;
            }
        }

        add_player_internal(player_index, player_identifier, peer_index, user_index, m_current_membership.player_sequence_number, left_game);
        m_current_membership.player_sequence_number = (m_current_membership.player_sequence_number + 1) % 0x100000;

        s_network_session_player *player = get_player(player_index);
        player->configuration.client.xuid = player_xuid;
        player->configuration.client.online = is_online;
        player->configuration.client.silver = is_silver;
        player->configuration.client.guest = is_guest;
        player->configuration.client.bot = is_bot;
        player->configuration.client.desired_team_index = -1;
        player->configuration.host.team_index = -1;
        player->configuration.host.user_team_index = -1;

        if (get_session()->established())
            get_session()->get_session_parameters()->m_parameters.countdown_timer.set(false, 0, _network_game_countdown_delayed_reason_none, 0);

        increment_update();
    }

    return player_index;
}

void c_network_session_membership::remove_player(long player_index)
{
    c_network_session *session = get_session();
    s_network_session_player *player = get_player(player_index);
    if (get_peer_connection_state(player->peer_index) >= 5)
        managed_session_remove_players(session->managed_session_index(), &player->configuration.client.xuid, 1);
    remove_player_internal(player_index);
    if (get_session()->established())
        get_session()->get_session_parameters()->m_parameters.countdown_timer.set(false, 0, _network_game_countdown_delayed_reason_none, 0);
    increment_update();
}

s_network_session_player *c_network_session_membership::add_player_internal(long player_index, const s_player_identifier *identifier,
    long peer_index, long peer_user_index, long sequence_number, bool left_game)
{
    s_network_session_player *player = get_raw_player(player_index);
    set_membership_flag(m_current_membership.valid_player_mask.m_storage, player_index, true);
    ++m_current_membership.player_count;
    player->identifier = *identifier;
    player->peer_index = peer_index;
    player->peer_user_index = peer_user_index;
    player->player_sequence_number = sequence_number;
    player->left_game = left_game;
    player->controller_index = -1;
    if (peer_index != -1 && peer_user_index != -1) {
        s_network_session_peer *player_peer = get_peer(peer_index);
        player_peer->user_player_indices[peer_user_index] = player_index;
        ++player_peer->player_count;
    }
    player->configuration.host.team_index = -1;
    player->configuration.host.user_team_index = -1;
    return player;
}

void c_network_session_membership::remove_player_internal(long player_index)
{
    s_network_session_player *player = get_player(player_index);
    if (player->peer_index != -1 && player->peer_user_index != -1) {
        s_network_session_peer *player_peer = get_peer(player->peer_index);
        player_peer->user_player_indices[player->peer_user_index] = -1;
        --player_peer->player_count;
    }
    set_membership_flag(m_current_membership.valid_player_mask.m_storage, player_index, false);
    --m_current_membership.player_count;
    memset(player, 0, sizeof(*player));
}


void c_network_session_membership::set_player_properties(long player_index, long desired_configuration_version, e_controller_index controller_index,
    const s_player_configuration_from_client *player_data_from_client, unsigned long player_voice)
{
    s_network_session_player *player = &m_current_membership.players[player_index];
    bool changed = false;

    if (player->desired_configuration_version != desired_configuration_version) {
        changed = true;
        player->desired_configuration_version = desired_configuration_version;
    }
    if (player->controller_index != controller_index) {
        changed = true;
        player->controller_index = controller_index;
    }
    if (memcmp(&player->configuration.client, player_data_from_client, sizeof(player->configuration.client)) != 0) {
        memcpy(&player->configuration.client, player_data_from_client, sizeof(player->configuration.client));
        bool should_update = false;
        if (m_current_membership.update_number != -1 && m_current_membership.host_peer_index != -1) {
            should_update = m_current_membership.host_peer_index - m_local_peer_index == 0;
        }
        if (should_update) {
            update_player_configuration_from_host(player_index);
        }
        changed = true;
    }
    if (player->voice_settings != player_voice) {
        changed = true;
        player->voice_settings = player_voice;
    }
    if (changed) {
        m_current_membership.update_number++;
        m_local_membership_update_number++;
    }
}

bool c_network_session_membership::does_player_exist(struct s_player_identifier const * identifier, long peer_index, long user_index) const
{
    bool result = false;
    if (is_peer_valid(peer_index))
    {
        long player_index = m_current_membership.peers[peer_index].user_player_indices[user_index];
        if (player_index != -1)
        {
            if (!identifier || player_identifier_compare(&this->m_current_membership.players[player_index].identifier, identifier))
                result = true;
        }
    }
    return result;
}

// c_network_session_membership::get_first_player
long c_network_session_membership::get_first_player(void) const
{
    long result = -1;
    for (long player_index = 0; player_index < k_network_maximum_machines_per_session; player_index++) {
        if (BIT_VECTOR_TEST_FLAG(this->m_current_membership.valid_player_mask.m_storage, player_index)) {
            result = player_index;
            break;
        }
    }
    return result;
}

// c_network_session_membership::get_next_player
long c_network_session_membership::get_next_player(long player_index) const
{
    long result = -1;
    for (long next_player_index = player_index + 1; next_player_index < k_network_maximum_machines_per_session; next_player_index++) {
        if (BIT_VECTOR_TEST_FLAG(this->m_current_membership.valid_player_mask.m_storage, next_player_index)) {
            result = next_player_index;
            break;
        }
    }
    return result;
}

bool c_network_session_membership::peer_property_flag_test(e_peer_property_flag_test_type type, e_network_session_peer_properties_status_flags flags) const
{
    bool result = true;
    for (long peer_index = 0; peer_index < 16; ++peer_index) {
        if (is_peer_valid(peer_index)) {
            const s_network_session_peer *peer = get_peer(peer_index);
            bool test = true;
            switch (type) {
            case _peer_property_flag_test_all:
                test = true;
                break;
            case _peer_property_flag_test_host:
                if (host_peer_index() != peer_index) test = false;
                break;
            case _peer_property_flag_test_non_host:
                if (host_peer_index() == peer_index) test = false;
                break;
            default:
                // match: 0x3cbc88: test type is 0..2; the hint omits the dead default branch
                __assume(0);
            }
            if (test && !test_peer_property(peer->properties.status_flags.m_storage, flags)) {
                result = false;
                break;
            }
        }
    }
    return result;
}

// c_network_session_membership::peer_property_flag_test_any_peer
bool c_network_session_membership::peer_property_flag_test_any_peer(e_network_session_peer_properties_status_flags flags) const
{
    bool result = false;
    for (int peer_index = 0; peer_index < 16; peer_index++) {
        if (((1 << (peer_index & 31)) & m_current_membership.valid_peer_mask.m_storage[peer_index >> 5]) != 0
            && ((1 << flags) & m_current_membership.peers[peer_index].properties.status_flags.m_storage) != 0) {
            result = true;
            break;
        }
    }
    return result;
}

void c_network_session_membership::update_player_configuration_from_host(long player_index)
{
    s_network_session_player *player = get_player(player_index);
    s_calculated_player_statistics calculated_statistics;
    memset(&calculated_statistics, 0, sizeof(calculated_statistics));
    multiplayer_game_hopper_build_calculated_stats(&player->configuration.client.global_statistics,
        &player->configuration.client.hopper_statistics, &calculated_statistics);
    if (memcmp(&calculated_statistics, &player->configuration.host.calculated_statistics, sizeof(s_calculated_player_statistics)) != 0) {
        memcpy(&player->configuration.host.calculated_statistics, &calculated_statistics, sizeof(s_calculated_player_statistics));
        increment_update();
    }
}
// @address 0x3cc438 ?add_player_to_player_add_queue@c_network_session_membership@@QAA_NPBUs_player_identifier@@JJJW4e_controller_index@@PBUs_player_configuration_from_client@@K@Z
bool c_network_session_membership::add_player_to_player_add_queue(const s_player_identifier *player_identifier, long peer_index, long peer_user_index,
    long local_user_index, e_controller_index controller_index,
    const s_player_configuration_from_client *player_data_from_client, unsigned long flags)
{
    int index = find_player_in_player_add_queue(player_identifier);
    int next = m_player_add_queue_count + 1;
    next = next - ((next / 4) << 2);

    if (index == -1 && next != m_player_add_queue_first) {
        index = m_player_add_queue_count;
        m_player_add_queue_count = next;
    }

    if (index != -1) {
        s_player_add_queue_entry *entry = &m_player_add_queue[index];
        memcpy(entry->player_identifier.data, player_identifier->data, sizeof(entry->player_identifier.data));
        entry->peer_index = peer_index;
        entry->peer_user_index = peer_user_index;
        entry->local_user_index = local_user_index;
        entry->controller_index = controller_index;
        memcpy(&entry->player_data, player_data_from_client, sizeof(entry->player_data));
        entry->flags = flags;
    }
    return index != -1;
}


void c_network_session_membership::remove_player_from_player_add_queue(const s_player_identifier *identifier)
{
    long index = find_player_in_player_add_queue(identifier);
    if (index != -1) {
        if (index == m_player_add_queue_first) {
            m_player_add_queue_first = (m_player_add_queue_first + 1) % 4;
        } else if ((index + 1) % 4 == m_player_add_queue_count) {
            if (m_player_add_queue_count == 0)
                m_player_add_queue_count = 4;
            --m_player_add_queue_count;
            return;
        } else {
            void *(*const copy)(void *, const void *, size_t) = memcpy;
            copy(&m_player_add_queue[index], &m_player_add_queue[m_player_add_queue_first], sizeof(s_player_add_queue_entry));
            m_player_add_queue_first = (m_player_add_queue_first + 1) % 4;
        }
    }
}

// 0x3cc5e0
void c_network_session_membership::commit_player_from_player_add_queue(const s_player_identifier *player_identifier)
{
    s_player_add_queue_entry *entry = &m_player_add_queue[m_player_add_queue_first];
    s_network_session_peer *peer = get_peer(entry->peer_index);
    bool left_game = false;
    for (unsigned long user_index = 0; user_index < 4; ++user_index) {
        long peer_player_index = peer->user_player_indices[user_index];
        if (peer_player_index != -1) {
            if (get_player(peer_player_index)->left_game) {
                left_game = true;
                break;
            }
        }
    }
    long player_index = find_or_add_player(entry->peer_index, entry->peer_user_index, &entry->player_identifier,
        entry->player_data.xuid, entry->player_data.online, entry->player_data.silver, entry->player_data.guest,
        entry->player_data.bot, left_game);
    set_player_properties(player_index, entry->local_user_index, (e_controller_index)entry->controller_index,
        (const s_player_configuration_from_client *)&entry->player_data, entry->flags);
    m_player_add_queue_first = (m_player_add_queue_first + 1) % 4;
}

// c_network_session_membership::find_player_in_player_add_queue
long c_network_session_membership::find_player_in_player_add_queue(const s_player_identifier *identifier) const
{
    int result = -1;
    int index = m_player_add_queue_first;
    int count = m_player_add_queue_count;
    if (index != count) {
        do {
            if (memcmp(identifier->data, m_player_add_queue[index].player_identifier.data, sizeof(identifier->data)) == 0) {
                result = index;
                break;
            }
            int next = index + 1;
            index = next - ((next / 4) << 2);
        } while (index != count);
    }
    return result;
}


template <class T> unsigned long fast_checksum(unsigned long checksum, const T *data)
{
    unsigned long a, b, c;
    const unsigned long *words = reinterpret_cast<const unsigned long *>(data);
    a = b = c = checksum + 0xdeadbeef + sizeof(T);
    for (unsigned long index = 0; index < sizeof(T) / (3 * sizeof(*words)); ++index) {
        a += words[0];
        b += words[1];
        c += words[2];
        a -= c; a ^= (c << 4) ^ (c >> 28); c += b;
        b -= a; b ^= (a << 6) ^ (a >> 26); a += c;
        c -= b; c ^= (b << 8) ^ (b >> 24); b += a;
        a -= c; a ^= (c << 16) ^ (c >> 16); c += b;
        b -= a; b ^= (a << 19) ^ (a >> 13); a += c;
        c -= b; c ^= (b << 4) ^ (b >> 28); b += a;
        words += 3;
    }
    switch (sizeof(T) % (3 * sizeof(*words))) {
    case 3 * sizeof(*words) - 1: a += words[0]; b += words[1]; c += words[2] >> 8; break;
    case 2 * sizeof(*words) + sizeof(unsigned short): a += words[0]; b += words[1]; c += words[2] >> 16; break;
    case 2 * sizeof(*words) + 1: a += words[0]; b += words[1]; c += words[2] >> 24; break;
    case 2 * sizeof(*words): a += words[0]; b += words[1]; break;
    case 2 * sizeof(*words) - 1: a += words[0]; b += words[1] >> 8; break;
    case sizeof(*words) + sizeof(unsigned short): a += words[0]; b += words[1] >> 16; break;
    case sizeof(*words) + 1: a += words[0]; b += words[1] >> 24; break;
    case sizeof(*words): a += words[0]; break;
    case 3: a += words[0] >> 8; break;
    case 2: a += words[0] >> 16; break;
    case 1: a += words[0] >> 24; break;
    }
    c ^= b; c -= (b << 14) ^ (b >> 18);
    a ^= c; a -= (c << 11) ^ (c >> 21);
    b ^= a; b -= (a << 25) ^ (a >> 7);
    c ^= b; c -= (b << 16) ^ (b >> 16);
    a ^= c; a -= (c << 4) ^ (c >> 28);
    b ^= a; b -= (a << 14) ^ (a >> 18);
    c ^= b; c -= (b << 24) ^ (b >> 8);
    return c;
}
template unsigned long fast_checksum<s_network_session_shared_membership>(unsigned long, const s_network_session_shared_membership *);

template<long flag>
static __forceinline bool membership_managed_session_flag_test(const s_online_managed_session *entry)
{
    return ((entry->flags >> (flag + 1)) & 1) != 0;
}
static __forceinline bool managed_session_get_security_information(long index, bool include_private,
    s_transport_session_description *description, e_transport_platform *platform)
{
    s_online_managed_session *entry = &online_session_manager_globals.sessions[index];
    bool result = false;
    if (index != -1 && membership_managed_session_flag_test<3>(entry)) {
        if (platform)
            *platform = (e_transport_platform)entry->platform;
        result = true;
    }
    return result;
}
// @address 0x3c9c78 ?get_peer_from_incoming_address@c_network_session_membership@@QBAJPBUtransport_address@@@Z
long c_network_session_membership::get_peer_from_incoming_address(const transport_address *incoming_address) const
{
    long peer_index = -1;
    const c_network_session *network_session = get_session();
    if (!network_session->disconnected()) {
        e_transport_platform platform;
        if (managed_session_get_security_information(network_session->managed_session_index(), false, 0, &platform)) {
            s_transport_secure_address secure_address;
            if (transport_secure_address_retrieve(incoming_address, platform, &secure_address))
                peer_index = get_peer_from_secure_address(&secure_address);
        }
    }
    return peer_index;
}
// @address 0x3cbda0 ?apply_peer_properties_update@c_network_session_membership@@CAXPAUs_network_session_peer_properties@@PBUs_network_message_membership_update_peer_properties@@@Z
void c_network_session_membership::apply_peer_properties_update(
    s_network_session_peer_properties *properties, const s_network_message_membership_update_peer_properties *update)
{
    if (update->name_updated) {
        memset(properties->name, 0, sizeof(properties->name));
        ustrnzcpy_inline(properties->name, (const wchar_t *)update->name, 16);
        memset(properties->session_name, 0, sizeof(properties->session_name));
        ustrnzcpy_inline(properties->session_name, (const wchar_t *)update->session_name, 32);
    }
    properties->has_gamestate_bonus = update->has_hard_drive;
    properties->hopper_identifier = update->ready_hopper_id;
    properties->hopper_error = update->game_start_error;
    if (update->unknown_updated)
        properties->map_id = update->unknown_value;
    if (update->map_updated) {
        properties->map_status = update->map_status;
        properties->map_progress = update->map_progress_percentage;
    }
    if (update->bandwidth_updated)
        properties->map_content_mask = update->bandwidth_value;
    if (update->connection_updated)
        properties->id = update->bandwidth;
    if (update->addresses_updated) {
        properties->upstream_bandwidth = update->address.s_addr;
        properties->downstream_bandwidth = update->online_address.s_addr;
        properties->is_peer_flagged = update->address_flag;
        properties->nat_type = update->nat_type;
        properties->connection_quality = update->connectivity_badness_rating;
        properties->connectivity_badness = update->host_badness_rating;
        properties->host_badness = update->client_badness_rating;
        memcpy(&properties->connectivity, &update->connectivity_mask, sizeof(properties->connectivity));
        properties->language = update->language;
    }
    if (update->values_updated) {
        properties->determinism_version = update->determinism_version;
        properties->determinism_compatible_version = update->determinism_compatible_version;
    }
    if (update->flags_updated)
        properties->status_flags.m_storage = update->flags;
}

// @address 0x3cc0b0 ?build_peer_properties_update@c_network_session_membership@@CAXPBUs_network_session_peer_properties@@0PAUs_network_message_membership_update_peer_properties@@@Z
void c_network_session_membership::build_peer_properties_update(const s_network_session_peer_properties *properties,
    const s_network_session_peer_properties *previous, s_network_message_membership_update_peer_properties *update)
{
    if (previous == 0 || wcsncmp(properties->name, previous->name, 16) != 0 || wcsncmp(properties->session_name, previous->session_name, 32) != 0) {
        update->name_updated = true;
        ustrnzcpy_inline((wchar_t *)update->name, properties->name, 16);
        ustrnzcpy_inline((wchar_t *)update->session_name, properties->session_name, 32);
    }
    update->has_hard_drive = properties->has_gamestate_bonus;
    update->ready_hopper_id = (unsigned short)properties->hopper_identifier;
    update->game_start_error = properties->hopper_error;
    if (previous == 0 || properties->map_id != previous->map_id) {
        update->unknown_updated = true;
        update->unknown_value = properties->map_id;
    }
    if (previous == 0 || properties->map_status != previous->map_status || properties->map_progress != previous->map_progress) {
        update->map_updated = true;
        update->map_status = properties->map_status;
        update->map_progress_percentage = properties->map_progress;
    }
    if (previous == 0 || (unsigned long)properties->map_content_mask != (unsigned long)previous->map_content_mask) {
        update->bandwidth_updated = true;
        update->bandwidth_value = properties->map_content_mask;
    }
    if (previous == 0 || (unsigned __int64)properties->id != (unsigned __int64)previous->id) {
        update->connection_updated = true;
        update->bandwidth = properties->id;
    }
    if (previous == 0 || properties->upstream_bandwidth != previous->upstream_bandwidth || properties->downstream_bandwidth != previous->downstream_bandwidth ||
        properties->is_peer_flagged != previous->is_peer_flagged || properties->nat_type != previous->nat_type ||
        (long)properties->connection_quality != (long)previous->connection_quality || properties->connectivity_badness != previous->connectivity_badness ||
        properties->host_badness != previous->host_badness
        || memcmp(&properties->connectivity, &previous->connectivity, sizeof(properties->connectivity)) != 0
        || properties->language != previous->language)
    {
        update->addresses_updated = true;
        update->address.s_addr = properties->upstream_bandwidth;
        update->online_address.s_addr = properties->downstream_bandwidth;
        update->address_flag = properties->is_peer_flagged;
        update->nat_type = properties->nat_type;
        update->connectivity_badness_rating = properties->connection_quality;
        update->host_badness_rating = properties->connectivity_badness;
        update->client_badness_rating = properties->host_badness;
        memcpy(&update->connectivity_mask, &properties->connectivity, sizeof(properties->connectivity));
        update->language = properties->language;
    }
    if (previous == 0 || properties->determinism_version != previous->determinism_version
        || properties->determinism_compatible_version != previous->determinism_compatible_version) {
        update->values_updated = true;
        update->determinism_version = properties->determinism_version;
        update->determinism_compatible_version = properties->determinism_compatible_version;
    }
    if (previous == 0 || (unsigned long)properties->status_flags.m_storage != (unsigned long)previous->status_flags.m_storage) {
        update->flags_updated = true;
        update->flags = properties->status_flags.m_storage;
    }
}

// @address 0x3c95c8 ?clone_for_connect@c_network_session_membership@@QAA_NPBV1@PAVc_network_session@@@Z
bool c_network_session_membership::clone_for_connect(const c_network_session_membership *membership, c_network_session *network_session)
{
    bool success = true;
    unsigned long time = network_time_get();
    c_network_observer *observer = network_session->m_observer;
    memcpy(this, membership, sizeof(*this));
    m_session = network_session;
    m_current_membership.update_number = 1;
    memset(&m_transmitted_membership, 0, sizeof(m_transmitted_membership));
    for (long peer_index = (this)->get_first_peer(); peer_index != -1;
        peer_index = (this)->get_next_peer(peer_index))
    {
        s_network_session_peer *peer = get_peer(peer_index);
        s_peer_channel *transmitted = &m_peer_channels[peer_index];
        peer->join_start_time = time;
        transmitted->peer_needs_reestablishment = 0;
        transmitted->membership_update_number = -1;
        if (transmitted->observer_channel_index != -1) {
            long observer_index = observer->allocate_observer(network_session->observer_owner(), &peer->secure_address);
            if (observer_index == -1)
                success = false;
        }
    }
    return success;
}

// @address 0x3cbbb0 ?commit_players@c_network_session_membership@@QAA_NXZ
bool c_network_session_membership::commit_players()
{
    bool result = true;
    if (m_dirty) {
        c_network_session *network_session = m_session;
        if (network_session) {
            if (m_committed || !managed_session_is_master_session(network_session->managed_session_index()) || managed_session_count_master_sessions() == 1) {
                result = managed_session_synchronize_to_player_list(network_session->managed_session_index(), this);
                m_committed = true;
                m_dirty = false;
            }
        }
    }
    return result;
}

static __forceinline unsigned __int64 network_session_membership_local_machine_nonce_get()
{
    if (!g_transport_security_globals.local_machine_nonce_valid) {
        g_transport_security_globals.local_machine_nonce = transport_secure_nonce_generate();
        g_transport_security_globals.local_machine_nonce_valid = true;
    }
    return g_transport_security_globals.local_machine_nonce;
}

// @address 0x3cad38 ?build_membership_update@c_network_session_membership@@QBAXPBUs_network_session_shared_membership@@0PAUs_network_message_membership_update@@@Z
void c_network_session_membership::build_membership_update(const s_network_session_shared_membership *current,
    const s_network_session_shared_membership *previous, s_network_message_membership_update *update) const
{
    unsigned long removed_peer_mask = 0;
    unsigned long changed_peer_mask = 0;
    c_network_session *network_session = m_session;
    memset(update, 0, sizeof(*update));
    managed_session_get_id(network_session->managed_session_index(), (s_transport_secure_identifier *)update);
    update->update_number = current->update_number;
    update->incremental_update_number = -1;
    if (previous != 0) {
        update->incremental_update_number = previous->update_number;
        update->baseline_checksum = m_transmitted_baseline_checksum;
    }
    unsigned long peer_bit = 1;
    for (long peer_index = 0; peer_index < 16; peer_index++, peer_bit <<= 1) {
        const s_network_session_peer *peer = &current->peers[peer_index];
        const s_network_session_peer *previous_peer;
        if (previous != 0 && BIT_VECTOR_TEST_FLAG(previous->valid_peer_mask.m_storage, peer_index))
            previous_peer = &previous->peers[peer_index];
        else
            previous_peer = 0;
        if (previous_peer != 0 && !BIT_VECTOR_TEST_FLAG(current->valid_peer_mask.m_storage, peer_index)) {
            removed_peer_mask |= peer_bit;
            s_network_message_membership_update_peer *peer_update = &update->peers[update->peer_count++];
            peer_update->peer_index = peer_index;
            peer_update->update_type = 0;
            continue;
        }
        if (!BIT_VECTOR_TEST_FLAG(current->valid_peer_mask.m_storage, peer_index))
            continue;
        bool info_updated = previous_peer == 0 || peer->peer_version != previous_peer->peer_version ||
            memcmp(&peer->secure_address, &previous_peer->secure_address, sizeof(peer->secure_address)) != 0 ||
            peer->party_nonce != previous_peer->party_nonce || peer->join_nonce != previous_peer->join_nonce ||
            peer->join_start_time != previous_peer->join_start_time;
        bool properties_updated = info_updated ||
            memcmp(&peer->properties, &previous_peer->properties, sizeof(peer->properties)) != 0;
        if (!info_updated && !properties_updated && peer->connection_state == previous_peer->connection_state)
            continue;
        s_network_message_membership_update_peer *peer_update = &update->peers[update->peer_count++];
        peer_update->peer_index = peer_index;
        peer_update->update_type = 1;
        peer_update->connection_state = (e_network_session_peer_state)peer->connection_state;
        peer_update->info_updated = info_updated;
        if (info_updated) {
            peer_update->network_version_number = peer->peer_version;
            peer_update->address = peer->secure_address;
            changed_peer_mask |= peer_bit;
            peer_update->party_nonce = peer->party_nonce;
            peer_update->join_nonce = peer->join_nonce;
            peer_update->creation_timestamp = (long)peer->join_start_time;
        }
        peer_update->properties_updated = properties_updated;
        if (properties_updated)
            build_peer_properties_update(&peer->properties, info_updated ? 0 : &previous_peer->properties, &peer_update->properties);
    }
    for (long player_index = 0; player_index < 16; player_index++) {
        const s_network_session_player *player = &current->players[player_index];
        const s_network_session_player *previous_player;
        if (previous != 0 && BIT_VECTOR_TEST_FLAG(previous->valid_player_mask.m_storage, player_index))
            previous_player = &previous->players[player_index];
        else previous_player = 0;
        unsigned long peer_mask = 1 << player->peer_index;
        if (previous_player != 0 && !BIT_VECTOR_TEST_FLAG(current->valid_player_mask.m_storage, player_index)) {
            if ((peer_mask & removed_peer_mask) != 0 || (peer_mask & changed_peer_mask) != 0)
                continue;
            s_network_message_membership_update_player *player_update = &update->players[update->player_count++];
            player_update->player_index = player_index;
            player_update->update_type = 0;
            continue;
        }
        if (!BIT_VECTOR_TEST_FLAG(current->valid_player_mask.m_storage, player_index))
            continue;
        if (previous_player != 0 && (peer_mask & changed_peer_mask) == 0) {
            if (memcmp(player, previous_player, sizeof(*player)) == 0)
                continue;
        }
        s_network_message_membership_update_player *player_update = &update->players[update->player_count++];
        player_update->player_index = player_index;
        player_update->update_type = 1;
        bool location_updated = previous_player == 0 || (peer_mask & changed_peer_mask) != 0 ||
            memcmp(&player->identifier.data, &previous_player->identifier.data, sizeof(player->identifier.data)) != 0 ||
            player->peer_index != previous_player->peer_index ||
            player->peer_user_index != previous_player->peer_user_index ||
            player->player_sequence_number != previous_player->player_sequence_number ||
            player->left_game != previous_player->left_game;
        player_update->location_updated = location_updated;
        if (location_updated) {
            player_update->identifier = player->identifier;
            player_update->peer_index = (short)player->peer_index;
            player_update->peer_user_index = (short)player->peer_user_index;
            player_update->player_addition_number = player->player_sequence_number;
            player_update->occupies_a_public_slot = player->left_game;
        }
        bool properties_updated = location_updated ||
            player->desired_configuration_version != previous_player->desired_configuration_version ||
            player->controller_index != previous_player->controller_index ||
            memcmp(&player->configuration, &previous_player->configuration, sizeof(player->configuration)) != 0 ||
            (unsigned long)player->voice_settings != (unsigned long)previous_player->voice_settings;
        player_update->properties_updated = properties_updated;
        if (properties_updated) {
            player_update->player_update_number = player->desired_configuration_version;
            player_update->controller_index = player->controller_index;
            player_update->configuration = player->configuration;
            player_update->voice = (unsigned long)player->voice_settings;
        }
    }
    if (previous == 0 || current->player_sequence_number != previous->player_sequence_number) {
        update->player_addition_number_updated = true;
        update->player_addition_number = current->player_sequence_number;
    }
    if (previous == 0 || current->leader_peer_index != previous->leader_peer_index) {
        update->leader_updated = true;
        update->leader_peer_index = current->leader_peer_index;
    }
    if (previous == 0 || current->host_peer_index != previous->host_peer_index) {
        update->host_updated = true;
        update->host_peer_index = current->host_peer_index;
    }
    if (previous == 0 || current->private_slot_count != previous->private_slot_count ||
        current->public_slot_count != previous->public_slot_count ||
        current->friends_only != previous->friends_only ||
        current->request_modification != previous->request_modification) {
        update->slot_counts_updated = true;
        update->private_slot_count = current->private_slot_count;
        update->public_slot_count = current->public_slot_count;
        update->are_slots_locked = current->friends_only;
        update->request_modification = current->request_modification;
    }
    update->checksum = (long)fast_checksum<s_network_session_shared_membership>(-1, current);
}

// @address 0x3cb3f0 ?handle_membership_update@c_network_session_membership@@QAA_NPBUs_network_message_membership_update@@@Z
bool c_network_session_membership::handle_membership_update(const s_network_message_membership_update * message)
{
    c_network_session *network_session = m_session;
    bool failure = false;
    s_transport_secure_address host_address;
    host_address.ina[0] = 0;
    memset(host_address.ina + 1, 0, sizeof(host_address) - 1);
    if (m_current_membership.host_peer_index != -1)
        memcpy(&host_address, get_peer_address(m_current_membership.host_peer_index), sizeof(host_address));

    if (message->incremental_update_number == -1) {
        reset_membership();
        m_host_reestablished = true;
    } else if (message->incremental_update_number != m_current_membership.update_number) {
        failure = true;
    } else {
            for (long player_index = 0; player_index < message->player_count; ++player_index) {
                const s_network_message_membership_update_player *player = &message->players[player_index];
                if (is_player_valid(player->player_index)) {
                    if (player->update_type == 0 || player->location_updated)
                        remove_player_internal(player->player_index);
                }
            }
            for (long peer_index = 0; peer_index < message->peer_count; ++peer_index) {
                const s_network_message_membership_update_peer *peer = &message->peers[peer_index];
                if (is_peer_valid(peer->peer_index)) {
                    long observer_channel = m_peer_channels[peer->peer_index].observer_channel_index;
                    if (peer->update_type == 0) {
                        if (observer_channel != -1 && network_session->m_session_class == 2) {
                            c_network_observer *observer = network_session->m_observer;
                            c_network_observer::s_channel_observer *channel = &observer->m_channel_observers[observer_channel];
                            if (channel->voice_active && channel->voice_talker)
                                observer->quality_statistics_report_badness(
                                    observer->find_observer_index_by_observer(channel), false, k_network_peer_leave_quality_reason);
                        }
                    }
                    if (peer->update_type == 0 || peer->info_updated) {
                        s_network_session_peer *session_peer = &m_current_membership.peers[peer->peer_index];
                        for (long user_index = 0; user_index < 4; ++user_index) {
                            if (session_peer->user_player_indices[user_index] != -1)
                                remove_player_internal(session_peer->user_player_indices[user_index]);
                        }
                        remove_peer_internal(peer->peer_index);
                        if (peer->peer_index == m_local_peer_index)
                            m_local_peer_index = -1;
                    }
                }
            }
    }

    if (message->slot_counts_updated) {
        m_current_membership.request_modification = message->request_modification;
        bool friends_only = message->are_slots_locked;
        long private_slot_count = message->public_slot_count;
        long public_slot_count = message->private_slot_count;
        m_current_membership.friends_only = friends_only;
        m_current_membership.public_slot_count = private_slot_count;
        m_current_membership.private_slot_count = public_slot_count;
        // match: 0x3cb3f0: volatile reloads the just-stored count instead of forwarding the register
        volatile long *private_slot_count_field = &m_current_membership.private_slot_count;
        if (get_session() != 0)
            managed_session_modify_slot_counts(
                get_session()->managed_session_index(), *private_slot_count_field,
                private_slot_count, friends_only, m_current_membership.request_modification);
        increment_update();
    }

    if (!failure) {
        m_current_membership.update_number = message->update_number;
        ++m_local_membership_update_number;
        for (long peer_index = 0; !failure && peer_index < message->peer_count; ++peer_index) {
            const s_network_message_membership_update_peer *peer = &message->peers[peer_index];
            if (peer->update_type != 1)
                continue;
            long peer_index_value = peer->peer_index;
            if (peer->info_updated) {
                if (!add_peer_internal(peer_index_value, (e_network_session_peer_state)peer->connection_state,
                        peer->network_version_number, (const s_transport_secure_address *)&peer->address,
                        peer->party_nonce, peer->join_nonce, peer->creation_timestamp))
                    failure = true;
            } else {
                m_current_membership.peers[peer_index_value].connection_state = (e_network_session_peer_state)peer->connection_state;
            }
            if (!failure) {
                if (peer->properties_updated) {
                    s_network_session_peer *session_peer = &m_current_membership.peers[peer_index_value];
                    apply_peer_properties_update(&session_peer->properties, &peer->properties);
                    long observer_channel = m_peer_channels[peer_index_value].observer_channel_index;
                    if (observer_channel != -1) {
                        if (!g_network_configuration.observer.disable_bad_client_anticheating)
                            network_session->m_observer->observer_channel_set_quality_statistics_ignored(
                                observer_channel, network_owner_entry_is_pending(this, peer_index_value));
                        network_session->m_observer->observer_channel_set_peer_properties(observer_channel,
                            session_peer->properties.upstream_bandwidth, session_peer->properties.downstream_bandwidth);
                    }
                }
            }
        }
    }

    if (!failure) {
        for (long player_index = 0; player_index < message->player_count; ++player_index) {
            const s_network_message_membership_update_player *player = &message->players[player_index];
            if (player->update_type != 1)
                continue;
            long player_index_value = player->player_index;
            if (player->location_updated)
                add_player_internal(player_index_value, (const s_player_identifier *)player->identifier.data,
                    (short)player->peer_index, (short)player->peer_user_index, player->player_addition_number, player->occupies_a_public_slot);
            if (player->properties_updated) {
                set_player_properties(player_index_value, player->player_update_number, (e_controller_index)player->controller_index,
                    &player->configuration.client, player->voice);
                memcpy(&m_current_membership.players[player_index_value].configuration.host,
                    player->configuration.host.data, sizeof(player->configuration.host.data));
                increment_update();
            }
        }
        if (message->player_addition_number_updated)
            m_current_membership.player_sequence_number = message->player_addition_number;
        if (message->leader_updated) {
            if (is_peer_valid(message->leader_peer_index))
                m_current_membership.leader_peer_index = message->leader_peer_index;
            else
                failure = true;
        }
        if (message->host_updated) {
            if (is_peer_valid(message->host_peer_index))
                m_current_membership.host_peer_index = message->host_peer_index;
            else
                failure = true;
        }
    }

    if (!failure) {
        if (m_local_peer_index == -1) {
            for (long peer_index = 0; peer_index < 16; ++peer_index) {
                if (is_peer_valid(peer_index) && transport_secure_address_compare(&m_current_membership.peers[peer_index].secure_address,
                    &g_transport_security_globals.secure_address)) {
                    m_local_peer_index = peer_index;
                    break;
                }
            }
        }
        if (m_local_peer_index == -1 || !is_peer_valid(m_local_peer_index))
            failure = true;
    }

    if (!failure) {
        if (m_current_membership.host_peer_index == -1 || !is_peer_valid(m_current_membership.host_peer_index))
            failure = true;
        m_current_membership.update_number = message->update_number;
    }

    if (!failure) {
        if (fast_checksum(-1, (const s_network_session_shared_membership *)&m_current_membership.update_number) != message->checksum)
            failure = true;
    }

    if (!failure && network_session != 0) {
        m_dirty = true;
        failure = !commit_players();
    }

    if (failure) {
        reset_membership();
        m_current_membership.host_peer_index = 0;
        unsigned long start_time = network_time_get();
        add_peer_internal(0, _network_session_peer_state_established, 0, &host_address, 0, 0, start_time);
        m_local_peer_index = 1;
        start_time = network_time_get();
        unsigned __int64 local_machine_nonce = network_session_membership_local_machine_nonce_get();
        add_peer_internal(1, _network_session_peer_state_established, (long)(unsigned long)(local_machine_nonce & 0xffffffff),
            &g_transport_security_globals.secure_address, 0, 0, start_time);
        if (network_session != 0)
            network_session->disband_and_reestablish_as_host(network_session->m_session_type == 2);
    }
    return !failure;
}
