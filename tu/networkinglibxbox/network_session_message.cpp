// networkinglibxbox/network_session_message.obj: from the records 0x442760 0x442888 0x442a18 0x442af0 0x442bd8 0x442d40 0x442e58 0x443110 0x443310 0x4434c0 0x443c88 0x443dc8 0x443ed0 0x443fc0 0x4440d0 0x444260 0x444340 0x444428
// @flags /TP /O2 /Oc
#include "game/player_types.h"
#include "networking/session/network_session_parameters.h"
#include "networking/session/network_session_peer_state_types.h"
#include "networking/session/network_session_types.h"
#include "cseries/crt_xdk.h"
#include "networking/messages/network_messages_session_protocol_types.h"
#include "networking/online/online_managed_session_types.h"
#include "networking/session/network_session_boot_reason_types.h"
#include "networking/network_observer_owner_types.h"
// @address 0x442760 ?handle_peer_connect@c_network_session@@QAAXPBUtransport_address@@PBUs_network_message_peer_connect@@@Z
// @address 0x442888 ?handle_time_synchronize@c_network_session@@QAA_NPBUtransport_address@@PBUs_network_message_time_synchronize@@@Z
// @address 0x442a18 ?handle_session_disband@c_network_session@@QAA_NPBUtransport_address@@PBUs_network_message_session_disband@@@Z
// @address 0x442af0 ?handle_session_boot@c_network_session@@QAA_NPBUtransport_address@@PBUs_network_message_session_boot@@@Z
// @address 0x442bd8 ?handle_host_handoff@c_network_session@@QAA_NPBUs_network_message_host_handoff@@@Z
// @address 0x442e58 ?handle_host_reestablish@c_network_session@@QAA_NPBUtransport_address@@PBUs_network_message_host_reestablish@@@Z
// @address 0x443110 ?handle_host_decline@c_network_session@@QAA_NPAVc_network_channel@@PBUs_network_message_host_decline@@@Z
// @address 0x443310 ?handle_peer_establish@c_network_session@@QAA_NPAVc_network_channel@@PBUs_network_message_peer_establish@@@Z
// @address 0x443c88 ?handle_election_refuse@c_network_session@@QAA_NPBUtransport_address@@@Z
// @address 0x443fc0 ?handle_boot_machine@c_network_session@@QAA_NPAVc_network_channel@@PBUs_network_message_boot_machine@@@Z
// @address 0x4440d0 ?handle_player_add@c_network_session@@QAA_NPAVc_network_channel@@PBUs_network_message_player_add@@@Z
// @address 0x444260 ?handle_player_refuse@c_network_session@@QAA_NPAVc_network_channel@@PBUs_network_message_player_refuse@@@Z
#include <string.h>
#include "networking/session/network_managed_session.h"
#include "networking/transport/transport_security.h"
#include "networking/messages/network_messages_session_protocol.h"
#include "networking/session/network_session_membership.h"
#include "networking/session/network_session_state_types.h"
#include "networking/transport/transport_address.h"
#include "networking/messages/network_message_type_collection.h"
#include "networking/transport/transport_security_types.h"
#include "networking/transport/transport_platform_types.h"
#include "networking/messages/network_messages_session_membership.h"

struct transport_address;
struct s_transport_secure_address;
struct s_network_session_peer_properties;
enum e_network_session_boot_reason;
enum e_network_message_type;
struct s_transport_session_description;
enum e_transport_platform;
enum e_controller_index;
struct s_player_configuration_from_client;
struct s_transport_session_description;
enum e_transport_platform;
struct transport_address;
#include "interface/user_interface_networking.h"
struct s_network_message_election_refuse;
class c_network_channel;

struct s_network_message_session_disband;
struct s_network_message_peer_handoff;
struct s_network_message_peer_properties;
struct s_network_message_delegate_leadership;
struct s_network_message_player_remove;
struct s_network_message_player_properties;
#pragma pack(push, 4)
#pragma pack(pop)

#include "networking/network_time_types.h"
// @address 0x8c447c g_network_time_globals
struct s_online_session_manager_globals;

#include "networking/session/network_session_message_state_types.h"
#include "networking/logic/network_election_refuse_reason_types.h"

class c_network_session_parameters;
#include "networking/transport/transport_security_globals.h"
#include "networking/network_globals.h"

#include "networking/session/network_session_parameter_observer.h"

#include "networking/messages/network_message_gateway.h"

static __forceinline unsigned char reestablish_state_range(const unsigned int *state)
{
    return *state >= 4 && *state < 10;
}

static __forceinline bool network_session_message_waiting_for_host_connection(const c_network_session *session, const transport_address *incoming_address)
{
    bool result = false;
    if (session->m_local_state == 2)
        result = transport_address_equivalent(incoming_address, &session->m_local_state_data.peer_joining.join_usable_address, false);
    return result;
}

// 0x442760
void c_network_session::handle_peer_connect(const transport_address *transport_address, const s_network_message_peer_connect *network_message_peer_connect)
{
    c_network_session_membership *slots = &m_session_membership;
    int index = (int)((const c_network_session_membership *)slots)
                    ->get_peer_from_incoming_address(transport_address);
    int reason;
    if (!(this)->is_host()) {
        reason = 4;
    } else if (index == slots->m_local_peer_index) {
        reason = 11;
    } else if (index != -1) {
        if (slots->m_current_membership.peers[index].join_nonce != network_message_peer_connect->join_nonce
            || slots->m_current_membership.peers[index].connection_state != 2)
            return;
        if (slots->m_peer_channels[index].observer_channel_index == -1)
            return;
        slots->m_current_membership.peers[index].connection_state = _network_session_peer_state_connected;
        slots->m_current_membership.update_number = slots->m_current_membership.update_number + 1;
        slots->m_local_membership_update_number = slots->m_local_membership_update_number + 1;
        m_observer
            ->observer_channel_initiate_connection((e_network_observer_owner)m_session_index, slots->m_peer_channels[index].observer_channel_index);
        return;
    } else {
        reason = 17;
    }
    int key[3];
    memset(key, 0, sizeof(key));
    managed_session_get_id(m_managed_session_index, (s_transport_secure_identifier *)key);
    key[2] = reason;
    ((c_network_message_gateway *)m_message_gateway)->send_message_directed(transport_address, _network_message_type_join_refuse, 12, key);
}

// 0x442888
bool c_network_session::handle_time_synchronize(
    const transport_address *transport_address, const s_network_message_time_synchronize *network_message_time_synchronize)
{
    c_network_session *session = this;
    bool result = false;
    bool established = ((unsigned int)m_local_state >= 4 && (unsigned int)m_local_state < 10);
    if (established || m_local_state == 2) {
        if (is_host()) {
            int peer_index = (&m_session_membership)->get_peer_from_incoming_address(transport_address);
            if (peer_index != -1 && peer_index != m_session_membership.m_local_peer_index) {
                if (network_message_time_synchronize->synchronization_stage == 0) {
                    if (!leaving_session()) {
                        s_network_message_time_synchronize reply;
                        memcpy(&reply, network_message_time_synchronize, sizeof(reply));
                        reply.synchronization_stage = 1;
                        ((c_network_message_gateway *)m_message_gateway)
                            ->send_message_directed(transport_address, _network_message_type_time_synchronize, sizeof(reply), &reply);
                        result = true;
                    }
                }
            }
        } else {
            if ((&m_session_membership)->host_exists_at_incoming_address(transport_address)) {
                if (network_message_time_synchronize->synchronization_stage == 1) {
                    // match: 0x442888: volatile keeps 1 r11 load + subf/add before stbx; plain splits 3 ways, swaps r10/r11
                    int first = (int)((const volatile s_network_message_time_synchronize *)network_message_time_synchronize)
                                    ->authority_timestamp[0] -
                        // match: 0x442888: volatile keeps 1 r11 load + subf/add before stbx; plain splits 3 ways, swaps r10/r11
                        (int)((const volatile s_network_message_time_synchronize *)network_message_time_synchronize)->client_timestamp[0];
                    // match: 0x442888: volatile keeps 1 r11 load + subf/add before stbx; plain splits 3 ways, swaps r10/r11
                    int second = (int)((const volatile s_network_message_time_synchronize *)network_message_time_synchronize)
                                     ->authority_timestamp[1] -
                        (int)network_message_time_synchronize->client_timestamp[1];
                    int offset = (first + second) / 2;
                    time_set(offset);
                    m_time_synchronization_end_time = g_network_time_globals.enabled ? g_network_time_globals.time : GetTickCount();
                    result = true;
                }
            }
        }
    }
    return result;
}

// 0x442a18
bool c_network_session::handle_session_disband(const transport_address *transport_address, const s_network_message_session_disband *message)
{
    bool result = false;
    if (m_session_membership.host_exists_at_incoming_address(transport_address))
    {
        if (network_session_message_waiting_for_host_connection(this, transport_address))
        {
            if (network_session_message_waiting_for_host_connection(this, transport_address))
            {
                m_join_refuse_reason = _network_join_refuse_reason_session_disbanded;
                disconnect();
            }
        }
        else
        {
            handle_disconnection();
        }
        result = true;
    }
    return result;
}

// 0x442af0
bool c_network_session::handle_session_boot(const transport_address *incoming_address, const s_network_message_session_boot *message)
{
    bool result = false;
    if (m_session_membership.host_exists_at_incoming_address(incoming_address))
    {
        if (network_session_message_waiting_for_host_connection(this, incoming_address))
        {
            if (network_session_message_waiting_for_host_connection(this, incoming_address))
            {
                m_join_refuse_reason = _network_join_refuse_reason_session_booted;
                disconnect();
            }
        }
        else
        {
            user_interface_networking_notify_booted_from_session(m_session_type, (e_network_session_boot_reason)message->reason);
            initiate_leave_protocol(false);
        }
        result = true;
    }
    return result;
}

// 0x442bd8
bool c_network_session::handle_host_handoff(const s_network_message_host_handoff *network_message_host_handoff)
{
    c_network_session *session = this;
    const s_network_message_host_handoff *message = network_message_host_handoff;
    bool result = false;
    const s_transport_secure_address *peer_address;
    if (m_session_membership.m_current_membership.valid_peer_mask.test(message->host_peer_index)) {
        const s_transport_secure_address *address = &message->host_address;
        peer_address = &m_session_membership.m_current_membership.peers[message->host_peer_index].secure_address;
        if (transport_secure_address_compare(address, peer_address)) {
            int peer = message->host_peer_index;
            int host = m_session_membership.m_current_membership.host_peer_index;
            if (peer != host) {
                bool accept = true;
                if (peer == m_session_membership.m_local_peer_index) {
                    if ((this)->leaving_session())
                        accept = false;
                    if (!m_session_membership.m_committed)
                        accept = false;
                }
                if (accept) {
                    s_network_message_host_handoff response;
                    int channel;
                    m_session_membership.m_host_transition_peer_index = peer;
                    memset(&response, 0, sizeof(response));
                    managed_session_get_id(m_managed_session_index, (s_transport_secure_identifier *)&response);
                    response.host_peer_index = network_message_host_handoff->host_peer_index;
                    memcpy(&response.host_address, (void *)address, sizeof(s_transport_secure_address));
                    channel = m_session_membership.m_peer_channels[host].observer_channel_index;
                    if (channel != -1)
                        m_observer
                            ->observer_channel_send_message((e_network_observer_owner)m_session_index, channel, 0,
                                _network_message_type_peer_handoff, 46, (void const *)(&response));
                }
                result = true;
            }
        }
    }
    return result;
}

// @address 0x442d40 ?handle_peer_handoff@c_network_session@@QAA_NPAVc_network_channel@@PBUs_network_message_peer_handoff@@@Z
bool c_network_session::handle_peer_handoff(c_network_channel *channel, const s_network_message_peer_handoff *message)
{
    c_network_session *session = this;
    bool result;
    int observer_channel;
    int peer;
    short index;
    int one;

    result = false;
    if (m_local_state == 8 && m_local_state_data.host_handoff.handoff_initiated != 0) {
        observer_channel = m_observer->observer_channel_find_by_network_channel((e_network_observer_owner)m_session_index, channel);
        peer = (int)m_session_membership.get_peer_from_observer_channel(observer_channel);
        if (peer != -1 && peer != m_session_membership.m_local_peer_index && peer != m_session_membership.m_current_membership.host_peer_index) {
            index = ((const s_network_message_host_handoff *)message)->host_peer_index;
            if (index >= 0) {
                one = 1;
                if (m_session_membership.m_current_membership.valid_peer_mask.test(index)
                    && index == m_local_state_data.host_handoff.handoff_peer_index
                    && transport_secure_address_compare(&((const s_network_message_host_handoff *)message)->host_address,
                        &m_session_membership.m_current_membership.peers[index].secure_address)) {
                    m_local_state_data.host_handoff.acknowledged_peer_mask = (one << peer) | m_local_state_data.host_handoff.acknowledged_peer_mask;
                    result = one;
                }
            }
        }
    }
    return result;
}

// 0x442e58
bool c_network_session::handle_host_reestablish(
    const transport_address *transport_address, const s_network_message_host_reestablish *network_message_host_reestablish)
{
    c_network_session *session = this;
    c_network_session_membership *membership = &m_session_membership;
    int peer = m_session_membership.get_peer_from_incoming_address(transport_address);
    bool result = false;
    bool was_host = false;
    bool leaving = false;
    bool reestablished = false;
    if (peer != -1 && peer != m_session_membership.m_local_peer_index) {
        bool accept = false;
        e_network_session_state *state = &m_local_state;
        if (*state == _network_session_state_election) {
            accept = true;
        } else if (reestablish_state_range((const unsigned int *)state)) {
            if (peer != m_session_membership.m_current_membership.host_peer_index) {
                if (peer == m_session_membership.m_host_transition_peer_index) {
                    accept = true;
                } else if (!is_host()) {
                    int channel = membership->m_peer_channels[membership->m_current_membership.host_peer_index].observer_channel_index;
                    if (!m_observer
                            ->observer_channel_currently_alive((e_network_observer_owner)m_session_index, channel))
                        accept = true;
                }
            }
        }
        if (accept) {
            if (is_host()) {
                s_network_message_peer_establish response;
                memset(&response, 0, sizeof(response));
                managed_session_get_id(m_managed_session_index, (s_transport_secure_identifier *)&response);
                response.session_exists = 1;
                response.peer_exists = 1;
                response.host_exists = 1;
                memcpy(&response.host_address, &m_session_membership.m_current_membership.peers[peer].secure_address, sizeof(s_transport_secure_address));
                session->send_message_to_all_peers(_session_message_transmission_type_direct, _network_message_type_host_decline, 48, &response);
                was_host = true;
                if (*state == _network_session_state_host_handoff && m_local_state_data.host_handoff.host_leaving)
                    leaving = true;
            }
            m_session_membership.m_current_membership.host_peer_index = peer;
            m_session_membership.m_host_reestablished = false;
            m_session_membership.m_host_transition_peer_index = -1;
            if (was_host)
                reset_local_host_state(false);
            ::managed_session_connect_client_to_new_host(m_managed_session_index,
                (const s_transport_session_description *)&network_message_host_reestablish->new_session_id,
                    (e_transport_platform)network_message_host_reestablish->secure_key_platform);
            if (*state == _network_session_state_peer_leaving || leaving) {
                change_local_state_peer_leaving();
                m_local_state_data.peer_leaving.peer_reestablish_state = (e_peer_reestablish_state)1;
            } else {
                change_local_state_peer_established();
                m_local_state_data.peer_leaving.peer_reestablish_state = (e_peer_reestablish_state)1;
            }
            reestablished = true;
            result = true;
        }
    }
    if (!reestablished && m_session_membership.host_exists_at_incoming_address(transport_address)) {
        unsigned char response[8];
        int channel;
        memset(response, 0, sizeof(response));
        managed_session_get_id(m_managed_session_index, (s_transport_secure_identifier *)response);
        channel = m_session_membership.m_peer_channels[m_session_membership.m_current_membership.host_peer_index].observer_channel_index;
        if (channel != -1)
            m_observer
                ->observer_channel_send_message((e_network_observer_owner)m_session_index, channel, 0,
                    _network_message_type_peer_reestablish, 8, (void const *)(response));
        if (leaving)
            initiate_leave_protocol(0);
        result = true;
    }
    return result;
}

// 0x443110
bool c_network_session::handle_host_decline(c_network_channel *network_channel, const s_network_message_host_decline *network_message_host_decline)
{
    const volatile c_network_session *session = this;
    int observer_channel = -1;
    int peer;
    int state;
    bool handled;
    bool result;
    bool established;
    const c_network_observer::s_channel_observer *observer = m_observer->find_observer_by_channel(network_channel);
    // match: 0x443110: volatile pins the owner load above the branch on channel state
    long owner = session->m_session_index;
    if (observer->state != 0 && (observer->owner_flags & (1 << owner)) != 0)
        observer_channel = m_observer->find_observer_index_by_observer(observer);
    peer = (int)(&m_session_membership)->get_peer_from_observer_channel(observer_channel);
    handled = false;
    state = m_local_state;
    established = ((unsigned int)m_local_state >= 4 && (unsigned int)m_local_state < 10);
    if (established && !(this)->is_host()) {
        if (peer == m_session_membership.m_current_membership.host_peer_index) {
            handle_disconnection();
            handled = true;
        }
    } else if (state == 2) {
        if (peer == m_session_membership.m_current_membership.host_peer_index) {
            initiate_leave_protocol(0);
            handled = true;
        }
    } else {
        handled = true;
    }
    result = true;
    if (handled) result = handled;
    return result;
}

// 0x443310
bool c_network_session::handle_peer_establish(c_network_channel *network_channel, const s_network_message_peer_establish *network_message_peer_establish)
{
    const volatile c_network_session *session = this;
    bool result = false;
    int observer_channel = -1;
    int peer;
    const c_network_observer::s_channel_observer *observer = m_observer->find_observer_by_channel(network_channel);
    int channel_state = observer->state;
    // match: 0x443310: volatile keeps the owner load in r11 before the branch on channel state
    long owner = session->m_session_index;
    if (channel_state != 0 && (observer->owner_flags & (1 << owner)) != 0)
        observer_channel = m_observer->find_observer_index_by_observer(observer);
    peer = (int)(&m_session_membership)->get_peer_from_observer_channel(observer_channel);
    if ((this)->is_host()) {
        if (peer != -1 && peer != m_session_membership.m_local_peer_index) {
            result = true;
            m_session_membership.m_peer_channels[peer].peer_needs_reestablishment = false;
        }
    }
    if (!result && !(this)->is_host() && observer_channel != -1) {
        s_network_message_peer_establish decline;
        bool established;
        memset(&decline, 0, sizeof(decline));
        managed_session_get_id(m_managed_session_index, (s_transport_secure_identifier *)&decline);
        decline.session_exists = 1;
        decline.peer_exists = peer != -1;
        established = ((unsigned int)m_local_state >= 4 && (unsigned int)m_local_state < 10);
        if (established) {
            decline.host_exists = 1;
            memcpy(&decline.host_address,
                &m_session_membership.m_current_membership.peers[m_session_membership.m_current_membership.host_peer_index].secure_address,
                    sizeof(s_transport_secure_address));
        }
        m_observer
            ->observer_channel_send_message((e_network_observer_owner)m_session_index, observer_channel, 0,
                _network_message_type_host_decline, sizeof(decline), (void const *)(&decline));
        result = true;
    }
    return result;
}

static __forceinline unsigned char network_session_message_election_flag_test(unsigned int flags, int bit)
{
    return (unsigned char)((flags >> bit) & 1);
}

static __forceinline long network_session_message_find_election_peer(
    const s_network_session_message_election_state *election, const s_transport_secure_address *address)
{
    for (long peer = 0; peer < election->peer_count; ++peer)
        if (transport_secure_address_compare(address, &election->peers[peer]))
            return peer;
    return -1;
}

// @address 0x4434c0 ?handle_election@c_network_session@@QAA_NPBUtransport_address@@PBUs_network_message_election@@PAW4e_network_election_refuse_reason@@PA_NPAUs_transport_secure_address@@@Z
bool c_network_session::handle_election(const transport_address *incoming_address,
    const s_network_message_election *message, e_network_election_refuse_reason *refuse_reason,
    bool *alternate_host_valid, s_transport_secure_address *alternate_host_address)
{
    c_network_session_membership *membership = &m_session_membership;
    long peer_index = membership->get_peer_from_incoming_address(incoming_address);
    bool result = false;
    bool process_election = false;
    bool ignore = false;
    bool reestablish = false;
    long state = m_local_state;
    if (state == 0)
        *refuse_reason = _network_election_refuse_unknown_session;
    else if (state == 10)
        process_election = true;
    else
    {
        bool established = (unsigned long)m_local_state >= 4 && (unsigned long)m_local_state < 10;
        if (!established || m_session_membership.m_current_membership.update_number == -1
            || m_session_membership.m_current_membership.peer_count <= 1 ||
            m_session_membership.m_current_membership.peers[m_session_membership.m_local_peer_index].connection_state != 8)
            *refuse_reason = _network_election_refuse_not_established;
        else if ((this)->leaving_session() && !is_host())
            *refuse_reason = _network_election_refuse_leaving;
        else if (state == 9)
        {
            ignore = true;
            if (peer_index != -1)
                ((s_network_session_message_election_state &)m_local_state_data).handoff_mask &= ~(1 << peer_index);
        }
        else if (state == 4 && (((s_network_session_message_election_state &)m_local_state_data).mode == 1
            || ((s_network_session_message_election_state &)m_local_state_data).mode == 2))
        {
            *alternate_host_valid = true;
            *refuse_reason = _network_election_refuse_alternate_host;
            memcpy(alternate_host_address, &membership->m_current_membership.peers[membership->m_current_membership.host_peer_index].secure_address,
                sizeof(s_transport_secure_address));
        }
        else
        {
            const s_network_session_peer *host_peer = m_session_membership.get_host_peer();
            long host_channel = m_session_membership.get_host_observer_channel_index();
            const s_transport_secure_address *host_address = &host_peer->secure_address;
            bool from_host = transport_secure_address_compare(host_address, &message->host_address);
            if (is_host())
            {
                if (peer_index != -1)
                {
                    ignore = true;
                    reestablish = true;
                }
                else
                {
                    *alternate_host_valid = true;
                    *refuse_reason = _network_election_refuse_local_host;
                    memcpy(alternate_host_address, host_address, sizeof(s_transport_secure_address));
                }
            }
            else if (m_observer->observer_channel_currently_alive((e_network_observer_owner)m_session_index, host_channel))
            {
                *alternate_host_valid = true;
                if (from_host)
                {
                    *refuse_reason = _network_election_refuse_still_connected;
                    memcpy(alternate_host_address, host_address, sizeof(s_transport_secure_address));
                }
                else
                {
                    *refuse_reason = _network_election_refuse_alternate_host;
                    memcpy(alternate_host_address, host_address, sizeof(s_transport_secure_address));
                }
            }
            else if (m_observer->m_channel_observers[host_channel].state != 7)
                process_election = true;
            else if (from_host)
                process_election = true;
            else
                ignore = true;
        }
    }
    if (process_election && m_local_state != 10)
        change_local_state_election();
    if (process_election)
    {
        s_network_session_message_election_state *election = &((s_network_session_message_election_state &)m_local_state_data);
        bool changed = false;
        if (election->peer_count != message->peer_count ||
            memcmp(election->peers, message->peer_addresses, election->peer_count * sizeof(s_transport_secure_address)))
            changed = true;
        if (changed)
        {
            long local_peer = -1;
            for (long incoming_peer = 0; incoming_peer < message->peer_count; ++incoming_peer)
            {
                const s_transport_secure_address *address = &message->peer_addresses[incoming_peer];
                long old_count = election->peer_count;
                long insertion;
                int comparison = 0;
                for (insertion = 0; insertion < election->peer_count; ++insertion)
                {
                    comparison = memcmp(address, &election->peers[insertion], sizeof(s_transport_secure_address));
                    if (comparison <= 0)
                        break;
                }
                if (comparison != 0)
                {
                    if (insertion < old_count)
                        memmove(&election->peers[insertion + 1], &election->peers[insertion],
                            (old_count - insertion) * sizeof(s_transport_secure_address));
                    memcpy(&election->peers[insertion], address, sizeof(s_transport_secure_address));
                    ++election->peer_count;
                }
            }
            for (long peer = 0; peer < election->peer_count; ++peer)
                if (transport_secure_address_compare(&g_transport_security_globals.secure_address, &election->peers[peer]))
                {
                    local_peer = peer;
                    break;
                }
            memset(election->connectivity, 0, sizeof(election->connectivity));
            election->local_peer = local_peer;
            election->decision = 0;
            election->connectivity[local_peer] |= 1 << local_peer;
        }
        s_transport_secure_address address;
        e_transport_platform platform;
        bool valid = false;
        s_online_managed_session *managed = &online_session_manager_globals.sessions[m_managed_session_index];
        if (m_managed_session_index != -1 && network_session_message_election_flag_test(managed->flags, 4))
        {
            platform = (e_transport_platform)managed->platform;
            valid = true;
        }
        long election_peer = -1;
        if (valid && transport_secure_address_retrieve(incoming_address, platform, &address))
            election_peer = network_session_message_find_election_peer(election, &address);
        if (election_peer == -1)
            *refuse_reason = _network_election_refuse_unknown_stranger;
        else if (election_peer == election->local_peer)
            ignore = true;
        else
        {
            bool connectivity_changed = false;
            unsigned long peer_mask = 1 << election_peer;
            if (election->peer_count == message->peer_count)
            {
                unsigned long mask = 1;
                const unsigned long &refused = election->refused;
                election->decision |= message->host_update_number;
                for (long peer = 0; peer < election->peer_count; mask <<= 1, ++peer)
                {
                    if (!(refused & mask) && peer != election->local_peer)
                    {
                        // match: 0x4434c0: pointer __restrict qualifier prevents alias reload
                        const s_network_session_message_election_state * __restrict election_alias = election;
                        unsigned long old_connectivity = election_alias->connectivity[peer];
                        unsigned long new_connectivity = message->peer_update_numbers[peer];
                        if (new_connectivity & ~old_connectivity)
                        {
                            connectivity_changed = true;
                            election->connectivity[peer] = new_connectivity | old_connectivity;
                        }
                    }
                }
                if (connectivity_changed)
                    election->agreement &= ~peer_mask;
                else
                    election->agreement |= peer_mask;
            }
            if (!(election->connectivity[election->local_peer] & peer_mask))
                election->connectivity[election->local_peer] |= peer_mask;
            election->refused &= ~peer_mask;
            election->received_time[election_peer] = g_network_time_globals.enabled ? g_network_time_globals.time : GetTickCount();
            result = true;
        }
    }
    if (reestablish)
    {
        s_network_session_message_reestablish response;
        memset(&response, 0, sizeof(response));
        long managed_index = m_managed_session_index;
        managed_session_get_id(managed_index, &response.session_id);
        ::managed_session_get_security_information(managed_index, false, &response.description,
            &response.platform);
        long channel = m_session_membership.m_peer_channels[peer_index].observer_channel_index;
        if (channel != -1)
        {
            if (m_observer->m_channel_observers[channel].state == 7)
                m_observer
                    ->observer_channel_send_message((e_network_observer_owner)m_session_index, channel, 0,
                        _network_message_type_host_reestablish, 72, (void const *)(&response));
            m_observer
                ->observer_channel_send_message((e_network_observer_owner)m_session_index, channel, 1, _network_message_type_host_reestablish,
                    72, (void const *)(&response));
        }
        m_session_membership.m_peer_channels[peer_index].membership_update_number = -1;
        ((c_network_session_parameters *)m_session_parameters)->reset_peer_state(peer_index);
        m_session_membership.m_peer_channels[peer_index].peer_needs_reestablishment = true;
        if (m_local_state == 8)
            handoff_remove_peer_from_consideration(peer_index);
    }
    if (result)
        return result;
    if (ignore)
        result = true;
    return result;
}

// 0x443c88
bool c_network_session::handle_election_refuse(const transport_address *transport_address)
{
    bool result = false;
    s_network_session_message_election *election = &((s_network_session_message_election &)m_local_state_data);
    long session_index = m_managed_session_index;
    s_online_managed_session *session = &online_session_manager_globals.sessions[session_index];
    bool valid = false;
    e_transport_platform platform;
    s_transport_secure_address address;

    if (session_index != -1 && network_session_message_election_flag_test(session->flags, 4) != 0)
    {
        platform = (e_transport_platform)session->platform;
        valid = true;
    }
    if (valid && transport_secure_address_retrieve(transport_address, platform, &address) != 0)
    {
        long peer_index = 0;
        if (election->peer_count > 0)
        {
            const s_transport_secure_address *peer = election->peers;
            do
            {
                if (transport_secure_address_compare(&address, peer))
                {
                    if (peer_index != -1 && peer_index != election->local_peer_index)
                    {
                        unsigned long participation = election->participation_mask;
                        election->participation_mask = ~(1 << peer_index) & participation;
                        election->refusal_mask |= (1 << peer_index);
                        election->peer_slot[peer_index] = 0;
                        election->peer_slot[election->local_peer_index] &= ~(1 << peer_index);
                        result = true;
                    }
                    break;
                }
                ++peer_index;
                ++peer;
            } while (peer_index < election->peer_count);
        }
    }
    return result;
}

// @address 0x443dc8 ?handle_peer_properties@c_network_session@@QAA_NPAVc_network_channel@@PBUs_network_message_peer_properties@@@Z
bool c_network_session::handle_peer_properties(c_network_channel *channel, const s_network_message_peer_properties *message)
{
    c_network_session *session = this;
    unsigned char established;
    c_network_session_membership *membership;
    int observer_channel;
    int peer;
    int peer_count;
    int peer_serial;
    bool result;

    result = false;
    established = ((unsigned int)m_local_state >= 4 && (unsigned int)m_local_state < 10);
    if (established)
    {
        if (session->is_host())
        {
            membership = &m_session_membership;
            observer_channel
                = m_observer->observer_channel_find_by_network_channel((e_network_observer_owner)m_session_index, channel);
            peer = (int)membership->get_peer_from_observer_channel(observer_channel);
            if (peer != -1 && peer != m_session_membership.m_local_peer_index)
            {
                if (session->m_session_membership.m_current_membership.peers[peer].connection_state == 6)
                {
                    membership->m_current_membership.peers[peer].connection_state = _network_session_peer_state_waiting;
                    peer_count = membership->m_current_membership.update_number;
                    peer_serial = membership->m_local_membership_update_number;
                    membership->m_current_membership.update_number = peer_count + 1;
                    membership->m_local_membership_update_number = peer_serial + 1;
                }
                membership->set_peer_address(peer, &message->secure_address);
                membership->set_peer_properties(peer, (const s_network_session_peer_properties *)&message->properties);
                result = true;
            }
        }
    }
    return result;
}

// @address 0x443ed0 ?handle_delegate_leadership@c_network_session@@QAA_NPAVc_network_channel@@PBUs_network_message_delegate_leadership@@@Z
bool c_network_session::handle_delegate_leadership(c_network_channel *channel, const s_network_message_delegate_leadership *message)
{
    c_network_session *session = this;
    unsigned char established;
    c_network_session_membership *membership;
    int observer_channel;
    int peer;
    int address_peer;
    int host_peer;
    int peer_count;
    int peer_serial;
    bool result;

    result = false;
    established = ((unsigned int)m_local_state >= 4 && (unsigned int)m_local_state < 10);
    if (established)
    {
        if (session->is_host())
        {
            membership = &m_session_membership;
            observer_channel
                = m_observer->observer_channel_find_by_network_channel((e_network_observer_owner)m_session_index, channel);
            peer = (int)membership->get_peer_from_observer_channel(observer_channel);
            address_peer = membership
                               ->get_peer_from_secure_address(&message->desired_leader_address);
            host_peer = m_session_membership.m_current_membership.leader_peer_index;
            if (peer == host_peer && peer != m_session_membership.m_local_peer_index && address_peer != -1)
            {
                if (host_peer != address_peer)
                {
                    peer_count = membership->m_current_membership.update_number;
                    peer_serial = membership->m_local_membership_update_number;
                    membership->m_current_membership.leader_peer_index = address_peer;
                    membership->m_current_membership.update_number = peer_count + 1;
                    membership->m_local_membership_update_number = peer_serial + 1;
                }
                result = true;
            }
        }
    }
    return result;
}

// 0x443fc0
bool c_network_session::handle_boot_machine(c_network_channel *network_channel, const s_network_message_boot_machine *network_message_boot_machine)
{
    c_network_session *session = this;
    bool result = false;
    unsigned char established = ((unsigned int)m_local_state >= 4 && (unsigned int)m_local_state < 10);
    if (established)
    {
        if (session->is_host())
        {
            c_network_session_membership *membership = &m_session_membership;
            int observer_channel
                = m_observer->observer_channel_find_by_network_channel((e_network_observer_owner)m_session_index, network_channel);
            int peer = membership->get_peer_from_observer_channel(observer_channel);
            int address_peer = membership
                                   ->get_peer_from_secure_address(
                                       &network_message_boot_machine->boot_peer_address);
            if (peer == m_session_membership.m_current_membership.leader_peer_index)
            {
                int local_peer = m_session_membership.m_local_peer_index;
                if (peer != local_peer && address_peer != -1 && address_peer != peer)
                {
                    if (address_peer == local_peer)
                    {
                        user_interface_networking_notify_booted_from_session((e_network_session_type)m_session_type,
                            (e_network_session_boot_reason)network_message_boot_machine->reason);
                        initiate_leave_protocol(0);
                    }
                    else
                    {
                        boot_peer(address_peer,
                            (e_network_session_boot_reason)network_message_boot_machine->reason);
                    }
                    result = true;
                }
            }
        }
    }
    return result;
}

// 0x4440d0
bool c_network_session::handle_player_add(c_network_channel *network_channel, const s_network_message_player_add *network_message_player_add)
{
    c_network_session *session = this;
    c_network_session_membership *membership;
    int observer_channel;
    int peer;
    bool result = false;
    unsigned char established = ((unsigned int)m_local_state >= 4 && (unsigned int)m_local_state < 10);
    if (established)
    {
        if (session->is_host())
        {
            membership = &m_session_membership;
            observer_channel
                = m_observer->observer_channel_find_by_network_channel((e_network_observer_owner)m_session_index, network_channel);
            peer = (int)membership->get_peer_from_observer_channel(observer_channel);
            if (peer != -1 && peer != m_session_membership.m_local_peer_index)
            {
                const s_player_identifier *identifier = &network_message_player_add->player_identifier;
                int reason = session->can_accept_player_join_request(identifier, peer, network_message_player_add->user_index,
                    network_message_player_add->configuration.xuid,
                    network_message_player_add->configuration.silver,
                    network_message_player_add->configuration.guest,
                    network_message_player_add->configuration.bot, 1);
                if (reason == 0)
                {
                    membership->add_player_to_player_add_queue(identifier, peer, network_message_player_add->user_index,
                        network_message_player_add->player_update_number,
                        (::e_controller_index)network_message_player_add->controller_index,
                        &network_message_player_add->configuration,
                        network_message_player_add->voice);
                }
                else
                {
                    s_network_message_player_refuse refusal;
                    int observer_peer;
                    memset(&refusal, 0, sizeof(refusal));
                    managed_session_get_id(m_managed_session_index, &refusal.session_id);
                    refusal.player_identifier = *identifier;
                    refusal.user_index = network_message_player_add->user_index;
                    refusal.refuse_reason = reason;
                    observer_peer = m_session_membership.m_peer_channels[peer].observer_channel_index;
                    if (observer_peer != -1)
                        m_observer
                            ->observer_channel_send_message((e_network_observer_owner)m_session_index, observer_peer, 0,
                                _network_message_type_player_refuse, sizeof(refusal), (void const *)(&refusal));
                }
                result = true;
            }
        }
    }
    return result;
}

bool c_network_session::handle_player_refuse(c_network_channel *channel, const s_network_message_player_refuse *message)
{
    bool established = (unsigned long)m_local_state >= 4 && (unsigned long)m_local_state < 10;
    if (established && channel_is_authoritative(channel))
    {
        if (memcmp(&message->player_identifier, &m_local_user_player_add[message->user_index].player_identifier, sizeof(s_player_identifier)) == 0)
        {
            m_local_user_player_add[message->user_index].refuse_reason = message->refuse_reason;
        }
        m_session_membership.remove_player_from_player_add_queue(&message->player_identifier);
    }
    return true;
}

static __forceinline int network_session_message_player_remove_index_invalid(int index)
{
    if (index < 0) return 1;
    if (index >= 4) return 1;
    return 0;
}

// @address 0x444340 ?handle_player_remove@c_network_session@@QAA_NPAVc_network_channel@@PBUs_network_message_player_remove@@@Z
bool c_network_session::handle_player_remove(c_network_channel *channel, const s_network_message_player_remove *message)
{
    c_network_session *session = this;
    unsigned char established;
    int observer_channel;
    int peer;
    c_network_session_membership *membership;
    int player_index;
    int player;
    bool result;

    result = false;
    established = ((unsigned int)m_local_state >= 4 && (unsigned int)m_local_state < 10);
    if (established)
    {
        if (session->is_host())
        {
            membership = &m_session_membership;
            observer_channel
                = m_observer->observer_channel_find_by_network_channel((e_network_observer_owner)m_session_index, channel);
            peer = (int)membership->get_peer_from_observer_channel(observer_channel);
            if (peer != -1 && peer != m_session_membership.m_local_peer_index)
            {
                player_index = message->user_index;
                if (!network_session_message_player_remove_index_invalid(player_index))
                {
                    player = m_session_membership.m_current_membership.peers[peer].user_player_indices[player_index];
                    if (player != -1)
                    {
                        membership->remove_player(player);
                        result = true;
                    }
                }
            }
        }
    }
    return result;
}

static __forceinline int network_session_message_player_properties_index_invalid(int index)
{
    if (index < 0) return 1;
    if (index >= 4) return 1;
    return 0;
}

// @address 0x444428 ?handle_player_properties@c_network_session@@QAA_NPAVc_network_channel@@PBUs_network_message_player_properties@@@Z
bool c_network_session::handle_player_properties(c_network_channel *channel, const s_network_message_player_properties *message)
{
    c_network_session *session = this;
    unsigned char established;
    c_network_session_membership *membership;
    int observer_channel;
    int peer;
    int player_index;
    int player;
    bool result;

    result = false;
    established = ((unsigned int)m_local_state >= 4 && (unsigned int)m_local_state < 10);
    if (established)
    {
        if (session->is_host())
        {
            membership = &m_session_membership;
            observer_channel
                = m_observer->observer_channel_find_by_network_channel((e_network_observer_owner)m_session_index, channel);
            peer = (int)membership->get_peer_from_observer_channel(observer_channel);
            if (peer != -1 && peer != m_session_membership.m_local_peer_index)
            {
                player_index = message->user_index;
                if (!network_session_message_player_properties_index_invalid(player_index))
                {
                    player = m_session_membership.m_current_membership.peers[peer].user_player_indices[player_index];
                    if (player != -1)
                    {
                        membership->set_player_properties(player, message->update_number, (e_controller_index)message->controller_index,
                            &message->configuration, message->voice);
                        result = true;
                    }
                }
            }
        }
    }
    return result;
}
