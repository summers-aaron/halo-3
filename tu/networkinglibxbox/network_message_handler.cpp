#include "networking/logic/network_search_types.h"
// networkinglibxbox/network_message_handler.obj: from the records 0x3d8da0 0x3f4108 0x3f45c0 0x3f5280 0x3f5440 0x3f5518 0x3f55f8 0x3f5680 0x3f5770 0x3f5848 0x443220
// @compiler xdk6130
// @flags /TP /O2 /Oc
#include "cseries/cseries_types.h"
#include "networking/delivery/network_channel_state_types.h"
#include "networking/delivery/network_channel_closure_reason_types.h"
#include "networking/session/network_session_peer_properties_status_flags_types.h"
#include "networking/network_observer_owner_types.h"
#include "game/player_types.h"
#include "networking/logic/network_session_interface.h"
#include "simulation/simulation.h"
#include "game/game_results.h"
#include "networking/logic/network_broadcast_search.h"
#include "networking/network_time.h"
#include "networking/transport/transport_address.h"
#include "networking/transport/transport_security.h"
#include "networking/messages/network_messages_session_protocol.h"
#include "networking/session/network_session_membership.h"
#include "networking/session/network_session.h"
#include "networking/session/network_session_parameters.h"
#include "networking/online/online_guide_xenon.h"
#include "networking/messages/network_message_type_collection.h"
#include "networking/session/network_session_class.h"
#include "networking/session/network_session_types.h"
#include "networking/messages/network_messages_simulation.h"
#ifdef HALO3_64BIT
#include "networking/messages/network_messages_simulation_synchronous.h"
#endif
#include "networking/transport/transport_security_types.h"
#include "networking/network_utilities.h"
#include "simulation/simulation_world.h"
#include "networking/transport/transport_unique_identifier_types.h"
#include "simulation/simulation_view_establishment_mode_types.h"
#include "networking/messages/network_message_gateway.h"
#include "networking/session/network_session_manager.h"
#include "networking/session/network_managed_session_status.h"
#include "networking/messages/network_message_handler.h"
#include "networking/messages/network_messages_out_of_band.h"
#include "networking/messages/network_message_broadcast_reply.h"
// @address 0x80a390 g_broadcast_search_globals
#include "networking/logic/network_broadcast_search_types.h"
// @address 0xf11308 g_network_life_cycle_globals
#include "networking/logic/life_cycle/life_cycle_globals_access.h"
#include "networking/messages/network_message_handler_types.h"
#include "simulation/simulation_world_types.h"
#include "simulation/simulation_view.h"
#include "simulation/simulation_distributed_view.h"
// @address 0x11ab038 ?g_network_message_decode_buffer@@3Us_network_message_queue_buffer@@A
#include "networking/messages/network_message_queue.h"
#include "networking/delivery/network_link.h"
#include "networking/logic/network_join.h"
class c_network_session;
// @address 0x3d8da0 ?established@c_network_channel@@QBA_NXZ
// @address 0x3f5440 ?handle_join_abort@c_network_message_handler@@AAAXPBUtransport_address@@PBUs_network_message_join_abort@@@Z
// @address 0x3f5518 ?handle_join_refuse@c_network_message_handler@@AAAXPBUtransport_address@@PBUs_network_message_join_refuse@@@Z
// @address 0x3f55f8 ?handle_leave_session@c_network_message_handler@@AAAXPBUtransport_address@@PBUs_network_message_leave_session@@@Z
// @address 0x3f5770 ?handle_parameters_request@c_network_message_handler@@AAAXPAVc_network_channel@@PBUs_network_message_parameters_request@@@Z
// @address 0x3f5848 ?network_message_handler_handle_connect_establish@@YAXPAVc_network_channel@@PBUs_network_message_connect_establish@@@Z
// @address 0x443220 ?handle_peer_reestablish@c_network_session@@QAA_NPAVc_network_channel@@PBUs_network_message_peer_reestablish@@@Z

struct s_network_message_peer_reestablish;
#include "networking/messages/network_messages_connect.h"
#include "networking/delivery/network_channel.h"
struct s_network_message_connect_request;
#include "networking/session/network_session_parameter_observer.h"

#include <string.h>
class c_network_session;
#define k_network_maximum_machines_per_session 16
#define BIT_VECTOR_TEST_FLAG(vector, bit) (((vector)[(bit) >> 5] & (1 << ((bit) & 31))) != 0)
#include "networking/logic/network_election_refuse_reason_types.h"
#include "networking/messages/network_messages_session_protocol_types.h"
#include "networking/messages/network_message_join_abort_types.h"

#include "networking/transport/transport_unique_identifier_accessors.h"
struct s_network_message_membership_update;

struct s_network_message_broadcast_search;
struct s_network_message_leave_session;
struct s_network_message_parameters_request;
struct s_network_message_session_disband;
struct s_network_message_session_boot;
struct s_network_message_host_handoff;
struct s_network_message_host_reestablish;
struct s_network_message_host_decline;
struct s_network_message_peer_establish;
struct s_network_message_boot_machine;
struct s_network_message_player_add;
struct s_network_message_player_refuse;
struct s_network_message_peer_connect;
struct s_network_message_time_synchronize;
struct s_network_message_join_request;
struct s_network_message_peer_handoff;
struct s_network_message_peer_properties;
struct s_network_message_delegate_leadership;
struct s_network_message_player_remove;
struct s_network_message_player_properties;
bool c_network_channel::established() const {
    return get_state() >= 4;
}
bool c_network_session::handle_peer_reestablish(c_network_channel *channel, const s_network_message_peer_reestablish *message) {
    bool handled = false;
    if (m_local_state == 9) {
        long observer_channel = -1;
        const c_network_observer::s_channel_observer *observer = (const c_network_observer::s_channel_observer *)channel;
        if (observer->state != _observer_state_none && (observer->owner_flags & (1 << observer_owner())) != 0)
            observer_channel = observer - m_observer->m_channel_observers;
        long peer_index = m_session_membership.get_peer_from_observer_channel(observer_channel);
        if (peer_index != -1 && peer_index != m_session_membership.local_peer_index() && peer_index != m_session_membership.host_peer_index()) {
            m_local_state_data.host_reestablish.acknowledged_peer_mask |= 1 << peer_index;
            m_session_membership.set_peer_needs_reestablishment(peer_index, false);
            handled = true;
        }
    }
    return handled;
}
void network_message_handler_handle_connect_establish(c_network_channel *channel, const s_network_message_connect_establish *message)
{
    e_network_channel_state state = channel->get_state();
    bool closed = state <= _network_channel_state_closed;
    if (closed) {
        return;
    }
    if (channel->m_identifier != message->remote_identifier) {
        return;
    }
    bool established_flag = state >= _network_channel_state_established;
    if (established_flag && channel->m_remote_identifier != message->identifier) {
        transport_address remote_address;
        unsigned long channel_identifier = -1;
        if (channel->m_message_queue.m_message_sent == 0) {
            channel_identifier = channel->m_identifier;
        }
        if (state != _network_channel_state_none && state != _network_channel_state_empty) {
            remote_address = channel->m_remote_address;
        }
        channel->close(_network_channel_closure_reason_connect_reinitiate);
        channel->open(&remote_address, false, channel_identifier);
    }
    closed = channel->get_state() <= _network_channel_state_closed;
    if (!closed) {
        channel->establish(message->identifier);
    }
}

struct s_network_message_parameters_update;
static __forceinline c_network_session_parameters *network_message_handler_session_parameters(c_network_session *session)
{
    return (c_network_session_parameters *)session->m_session_parameters;
}
struct s_network_squad_status_data;
extern "C" {

}
typedef char network_message_handler_reply_status_offset[(offsetof(s_network_message_broadcast_reply, status) == 16) ? 1 : -1];
static __forceinline bool network_broadcast_search_active(unsigned __int64 *search_nonce)
{
    if (g_broadcast_search_globals.searching)
        *search_nonce = g_broadcast_search_globals.search_nonce;
    return g_broadcast_search_globals.searching;
}
static __forceinline bool transport_secure_nonce_compare(unsigned __int64 a, unsigned __int64 b)
{
    return a == b;
}
static __forceinline bool network_life_cycle_in_system_link_advertisable_session(c_network_session **session_out)
{
    bool advertisable = false;
    if (g_network_life_cycle_globals.initialized)
    {
        c_network_session *session = g_network_life_cycle_globals.manager.m_active_squad_session;
        if (session->is_host() && session->m_session_class == _network_session_class_system_link)
        {
            unsigned short status;
            bool advertised = ((*managed_session_get_status(&status, session->m_managed_session_index) >> 1) & 1) != 0;
            if (advertised)
            {
                *session_out = session;
                advertisable = true;
            }
        }
    }
    return advertisable;
}
void c_network_message_handler::handle_broadcast_search(const transport_address *address, const s_network_message_broadcast_search *message)
{
    if (message->protocol == 7)
    {
        s_network_squad_status_data status;
        bool ignore = false;
        unsigned __int64 search_nonce;
        if (network_broadcast_search_active(&search_nonce))
            ignore = transport_secure_nonce_compare(message->search_nonce, search_nonce);
        if (!ignore)
        {
            c_network_session *session;
            if (!network_life_cycle_in_system_link_advertisable_session(&session))
                ignore = true;
        }
        if (!ignore && !network_squad_session_build_status(&status))
            ignore = true;
        if (!ignore)
        {
            s_transport_secure_address secure_address;
            if (!transport_secure_address_get(&secure_address))
                ignore = true;
        }
        if (!ignore)
        {
            s_network_message_broadcast_reply reply;
            memset(&reply, 0, sizeof(reply));
            reply.protocol = 7;
            reply.search_nonce = message->search_nonce;
            reply.status = status;
            m_message_gateway->send_message_broadcast(_network_message_type_broadcast_reply, sizeof(reply), &reply);
        }
    }
}
void c_network_message_handler::handle_join_abort(const transport_address *address, const s_network_message_join_abort *message)
{
    c_network_session *session = m_session_manager->get_session(&message->session_id);
    if (session != 0) {
        bool session_established = (unsigned long)session->m_local_state >= 4 && (unsigned long)session->m_local_state < 10;
        if (session_established && session->is_host()) {
            e_network_join_refuse_reason reason;
            if (session->join_abort(address, message->join_nonce)) {
                reason = _network_join_refuse_reason_aborted;
            }
            else {
                reason = _network_join_refuse_reason_abort_ignored;
            }

            s_network_message_join_refuse refusal;
            memset(&refusal, 0, sizeof(refusal));
            refusal.session_id = message->session_id;
            refusal.reason = reason;
            m_message_gateway->send_message_directed(address, _network_message_type_join_refuse, sizeof(refusal), &refusal);
        }
    }
}
void c_network_message_handler::handle_join_refuse(const transport_address *address, const s_network_message_join_refuse *message)
{
    c_network_session *session = m_session_manager->get_session((const s_transport_secure_identifier *)message);
    if (session != 0) {
        long state = session->m_local_state;
        bool waiting_for_host = false;
        if (state == 2) {
            waiting_for_host = transport_address_equivalent(address, &session->m_local_state_data.peer_joining.join_usable_address, false);
        }
        if (waiting_for_host) {
            session->host_connection_refused(address, message->reason);
            return;
        }
        bool join_abort_in_progress = false;
        if (state == 3) {
            join_abort_in_progress = transport_address_equivalent(address, &session->m_local_state_data.peer_join_abort.join_usable_address, false);
        }
        if (join_abort_in_progress && state == 3) {
            join_abort_in_progress = transport_address_equivalent(address, &session->m_local_state_data.peer_join_abort.join_usable_address, false);
            if (join_abort_in_progress) {
                session->disconnect();
            }
        }
    }
}
void c_network_message_handler::handle_leave_session(const transport_address *address, const s_network_message_leave_session *message)
{
    c_network_session *session = m_session_manager->get_session((const s_transport_secure_identifier *)message);
    if (session != 0 && session->is_host()) {
        long peer_index = session->m_session_membership.get_peer_from_incoming_address(address);
        if (peer_index != -1 && peer_index != session->m_session_membership.local_peer_index()) {
            session->handle_leave_internal(peer_index);
        }
    }
}
void c_network_message_handler::handle_parameters_request(
    c_network_channel *network_channel, const s_network_message_parameters_request *network_message_parameters_request)
{
    c_network_session *session = m_session_manager->get_session((const s_transport_secure_identifier *)network_message_parameters_request);
    if (session != 0) {
        if ((unsigned char)session->is_host() != 0) {
            bool state_in_range = (unsigned long)session->m_local_state >= 4 && (unsigned long)session->m_local_state < 10;
            if (state_in_range && (unsigned char)session->is_host() != 0) {
                long observer_channel = session->m_observer->observer_channel_find_by_network_channel(session->observer_owner(), network_channel);
                long peer_index = session->m_session_membership.get_peer_from_observer_channel(observer_channel);
                if (peer_index != -1 && peer_index != session->m_session_membership.local_peer_index()) {
                    network_message_handler_session_parameters(session)->handle_change_request(
                        (e_parameter_change_request_source)(peer_index == session->m_session_membership.leader_peer_index()),
                        network_message_parameters_request);
                }
            }
        }
    }
}

// @address 0x3f5680 ?handle_election@c_network_message_handler@@AAAXPBUtransport_address@@PBUs_network_message_election@@@Z
void c_network_message_handler::handle_election(const transport_address *address, const s_network_message_election *message)
{
    c_network_session *session = m_session_manager->get_session(&message->session_id);
    e_network_election_refuse_reason reason = _network_election_refuse_none;
    bool has_alternate = false;
    s_transport_secure_address alternate;
    bool handled = false;
    if (session) {
        if (session->handle_election(address, message, &reason, &has_alternate, &alternate))
            handled = true;
    } else {
        reason = _network_election_refuse_unknown_session;
    }
    if (!handled) {
        s_network_message_election_refuse reply;
        memset(&reply, 0, sizeof(reply));
        reply.session_id = message->session_id;
        reply.refusal_reason = reason;
        reply.alternate_host_valid = has_alternate;
        if (has_alternate) reply.alternate_host_address = alternate;
        m_message_gateway->send_message_directed(address, _network_message_type_election_refuse, sizeof(reply), &reply);
    }
}

struct player_action;
struct simulation_update;
struct s_game_results_incremental;
struct s_game_results_incremental_update;

struct s_simulation_distributed_view;

static __forceinline const void *network_message_offset_pointer(const void *message_storage, long offset)
{
    return (const char *)message_storage + offset;
}

__forceinline bool c_network_session::handle_join_request(const transport_address *address, const s_network_message_join_request *message)
{
    if (message->current_network_version < k_network_compatible_version)
        acknowledge_join_request(address, _network_join_refuse_reason_peer_version_too_low);
    else if (message->minimum_network_version > 11855)
        acknowledge_join_request(address, _network_join_refuse_reason_host_version_too_low);
    else
        network_join_add_join_to_queue(this, address, (const s_network_session_join_request *)&message->join_nonce);
    return true;
}

// @address 0x3f4108
void c_network_message_handler::handle_out_of_band_message(
    const transport_address *address, e_network_message_type message_type, long message_storage_size, const void *message_storage)
{
    switch (message_type)
    {
    case _network_message_type_ping:
    {
        const void *const *message_ref = &message_storage;
        const s_network_message_ping *message = (const s_network_message_ping *)*message_ref;
        s_network_message_pong reply;
        const unsigned long &id = message->id;
        reply.id = id;
        reply.timestamp = message->timestamp;
        reply.qos_response = 2;
        m_message_gateway->send_message_directed(address, _network_message_type_pong, sizeof(reply), &reply);
        break;
    }
    case _network_message_type_broadcast_search:
        handle_broadcast_search(address, (const s_network_message_broadcast_search *)message_storage);
        break;
    case _network_message_type_broadcast_reply:
    {
        const s_network_message_broadcast_reply *message = (const s_network_message_broadcast_reply *)message_storage;
        if (message->protocol == 7)
        {
            unsigned __int64 search_nonce;
            if (network_broadcast_search_active(&search_nonce) && transport_secure_nonce_compare(message->search_nonce, search_nonce))
                network_broadcast_search_handle_reply(address, message);
        }
        break;
    }
    case _network_message_type_connect_request:
        m_observer->handle_connect_request(address, (const s_network_message_connect_request *)message_storage);
        break;
    case _network_message_type_join_request:
    {
        const s_network_message_join_request *message = (const s_network_message_join_request *)message_storage;
        if (message->protocol == 7)
        {
            c_network_session *session = m_session_manager->get_session(&message->session_id);
            if (session && session->is_host())
            {
                session->handle_join_request(address, message);
            }
        }
        break;
    }
    case _network_message_type_peer_connect:
    {
        const s_network_message_peer_connect *message = (const s_network_message_peer_connect *)message_storage;
        if (message->protocol == 7)
        {
            c_network_session *session = m_session_manager->get_session(&message->session_id);
            if (session)
                session->handle_peer_connect(address, message);
        }
        break;
    }
    case _network_message_type_join_abort:
        handle_join_abort(address, (const s_network_message_join_abort *)message_storage);
        break;
    case _network_message_type_join_refuse:
        handle_join_refuse(address, (const s_network_message_join_refuse *)message_storage);
        break;
    case _network_message_type_leave_acknowledge:
    {
        c_network_session *session = m_session_manager->get_session((const s_transport_secure_identifier *)message_storage);
        if (session && session->leaving_session() && session->m_local_state == 5)
        {
            if (session->m_session_membership.get_peer_from_incoming_address(address) == session->m_session_membership.m_current_membership.host_peer_index)
                session->disconnect();
        }
        break;
    }
    case _network_message_type_session_disband:
    {
        c_network_session *session = m_session_manager->get_session((const s_transport_secure_identifier *)message_storage);
        if (session)
            session->handle_session_disband(address, (const s_network_message_session_disband *)message_storage);
        break;
    }
    case _network_message_type_session_boot:
    {
        c_network_session *session = m_session_manager->get_session((const s_transport_secure_identifier *)message_storage);
        if (session)
            session->handle_session_boot(address, (const s_network_message_session_boot *)message_storage);
        break;
    }
    case _network_message_type_election:
        handle_election(address, (const s_network_message_election *)message_storage);
        break;
    case _network_message_type_election_refuse:
    {
        c_network_session *session = m_session_manager->get_session((const s_transport_secure_identifier *)message_storage);
        if (session && session->m_local_state == 10)
            session->handle_election_refuse(address);
        break;
    }
    case _network_message_type_time_synchronize:
    {
        c_network_session *session = m_session_manager->get_session((const s_transport_secure_identifier *)message_storage);
        if (session)
            session->handle_time_synchronize(address, (const s_network_message_time_synchronize *)message_storage);
        break;
    }
    case _network_message_type_host_reestablish:
    {
        c_network_session *session = m_session_manager->get_session((const s_transport_secure_identifier *)message_storage);
        if (session)
            session->handle_host_reestablish(address, (const s_network_message_host_reestablish *)message_storage);
        break;
    }
    case _network_message_type_leave_session:
        handle_leave_session(address, (const s_network_message_leave_session *)message_storage);
        break;
    case _network_message_type_connect_refuse:
    {
        c_network_channel *channel = c_network_link::get_associated_channel(address);
        const s_network_message_connect_refuse *message = (const s_network_message_connect_refuse *)message_storage;
        if (channel && !channel->closed() && channel->get_state() != _network_channel_state_connected && channel->m_identifier == message->remote_identifier)
        {
            switch (message->reason)
            {
            case 0:
            case 1:
            case 2:
            case 5:
            case 6:
                channel->close(_network_channel_closure_reason_connect_refused);
                break;
            case 3:
            case 4:
            case 7:
                break;
            default:
                // match: 0x3f4108: unreachable hint omits dead branch check
                __assume(0);
            }
        }
        break;
    }
    case _network_message_type_connect_establish:
    {
        c_network_channel *channel = c_network_link::get_associated_channel(address);
        if (channel)
            network_message_handler_handle_connect_establish(channel, (const s_network_message_connect_establish *)message_storage);
        break;
    }
    case _network_message_type_connect_closed:
    {
        c_network_channel *channel = c_network_link::get_associated_channel(address);
        const s_network_message_connect_closed *message = (const s_network_message_connect_closed *)message_storage;
        if (channel && !channel->closed() && channel->m_identifier == message->remote_identifier)
            channel->close(_network_channel_closure_reason_remote_closure);
        break;
    }
    default:
        break;
    }
}

// @address 0x3f45c0 ?handle_channel_message@c_network_message_handler@@QAAXPAVc_network_channel@@W4e_network_message_type@@JPBX@Z
void c_network_message_handler::handle_channel_message(
    c_network_channel *network_channel, e_network_message_type network_message_type, long message_storage_size, const void *)
{
    switch (network_message_type)
    {
    case _network_message_type_leave_session:
    {
        if (network_channel->get_state() != _network_channel_state_connected)
            return;
        transport_address remote_address = network_channel->m_remote_address;
        handle_leave_session(&remote_address, (const s_network_message_leave_session *)&g_network_message_decode_buffer);
        return;
    }
    case _network_message_type_session_disband:
    {
        if (network_channel->get_state() != _network_channel_state_connected)
            return;
        transport_address remote_address = network_channel->m_remote_address;
        c_network_session *session = m_session_manager->get_session((const s_transport_secure_identifier *)&g_network_message_decode_buffer);
        if (!session)
            return;
        session->handle_session_disband(&remote_address, (const s_network_message_session_disband *)&g_network_message_decode_buffer);
        return;
    }
    case _network_message_type_session_boot:
    {
        if (network_channel->get_state() != _network_channel_state_connected)
            return;
        transport_address remote_address = network_channel->m_remote_address;
        c_network_session *session = m_session_manager->get_session((const s_transport_secure_identifier *)&g_network_message_decode_buffer);
        if (!session)
            return;
        session->handle_session_boot(&remote_address, (const s_network_message_session_boot *)&g_network_message_decode_buffer);
        return;
    }
    case _network_message_type_host_handoff:
    {
        if (network_channel->get_state() != _network_channel_state_connected)
            return;
        c_network_session *session = m_session_manager->get_session((const s_transport_secure_identifier *)&g_network_message_decode_buffer);
        if (!session || !session->channel_is_authoritative(network_channel))
            return;
        session->handle_host_handoff((const s_network_message_host_handoff *)&g_network_message_decode_buffer);
        return;
    }
    case _network_message_type_peer_handoff:
    {
        if (network_channel->get_state() != _network_channel_state_connected)
            return;
        c_network_session *session = m_session_manager->get_session((const s_transport_secure_identifier *)&g_network_message_decode_buffer);
        if (!session)
            return;
        bool session_active = (unsigned long)session->m_local_state >= 4 && (unsigned long)session->m_local_state < 10;
        if (!session_active)
            return;
        if (!session->is_host())
            return;
        session->handle_peer_handoff(network_channel, (const s_network_message_peer_handoff *)&g_network_message_decode_buffer);
        return;
    }
    case _network_message_type_host_transition:
    {
        if (network_channel->get_state() != _network_channel_state_connected)
            return;
        c_network_session *session = m_session_manager->get_session((const s_transport_secure_identifier *)&g_network_message_decode_buffer);
        if (!session || !session->channel_is_authoritative(network_channel))
            return;
        if (session->m_session_membership.host_transition_peer_index() != session->m_session_membership.local_peer_index())
            return;
        if (session->m_local_state == 5)
            return;
        session->change_local_state_host_reestablish(false, 0);
        return;
    }
    case _network_message_type_host_reestablish:
    {
        if (network_channel->get_state() != _network_channel_state_connected)
            return;
        transport_address remote_address = network_channel->m_remote_address;
        c_network_session *session = m_session_manager->get_session((const s_transport_secure_identifier *)&g_network_message_decode_buffer);
        if (!session)
            return;
        session->handle_host_reestablish(&remote_address, (const s_network_message_host_reestablish *)&g_network_message_decode_buffer);
        return;
    }
    case _network_message_type_host_decline:
    {
        if (network_channel->get_state() != _network_channel_state_connected)
            return;
        c_network_session *session = m_session_manager->get_session((const s_transport_secure_identifier *)&g_network_message_decode_buffer);
        if (!session)
            return;
        session->handle_host_decline(network_channel, (const s_network_message_host_decline *)&g_network_message_decode_buffer);
        return;
    }
    case _network_message_type_peer_reestablish:
    {
        if (network_channel->get_state() != _network_channel_state_connected)
            return;
        c_network_session *session = m_session_manager->get_session((const s_transport_secure_identifier *)&g_network_message_decode_buffer);
        if (!session || !session->is_host())
            return;
        session->handle_peer_reestablish(network_channel, (const s_network_message_peer_reestablish *)&g_network_message_decode_buffer);
        return;
    }
    case _network_message_type_peer_establish:
    {
        if (network_channel->get_state() != _network_channel_state_connected)
            return;
        c_network_session *session = m_session_manager->get_session((const s_transport_secure_identifier *)&g_network_message_decode_buffer);
        bool handled = false;
        if (session)
            handled = session->handle_peer_establish(network_channel, (const s_network_message_peer_establish *)&g_network_message_decode_buffer);
        if (handled)
            return;
        s_network_message_host_decline response;
        memset(&response, 0, sizeof(response));
        response.session_id = ((const s_network_message_peer_establish *)&g_network_message_decode_buffer)->session_id;
        network_channel->m_message_queue.send_message(_network_message_type_host_decline, sizeof(response), &response);
        return;
    }
    case _network_message_type_election:
    {
        if (network_channel->get_state() != _network_channel_state_connected)
            return;
        transport_address remote_address = network_channel->m_remote_address;
        handle_election(&remote_address, (const s_network_message_election *)&g_network_message_decode_buffer);
        return;
    }
    case _network_message_type_election_refuse:
    {
        if (network_channel->get_state() != _network_channel_state_connected)
            return;
        transport_address remote_address = network_channel->m_remote_address;
        c_network_session *session = m_session_manager->get_session((const s_transport_secure_identifier *)&g_network_message_decode_buffer);
        if (!session || session->m_local_state != 10)
            return;
        session->handle_election_refuse(&remote_address);
        return;
    }
    case _network_message_type_membership_update:
    {
        if (network_channel->get_state() != _network_channel_state_connected)
            return;
        c_network_session *session = m_session_manager->get_session((const s_transport_secure_identifier *)&g_network_message_decode_buffer);
        if (!session || !session->channel_is_authoritative(network_channel))
            return;
        session->m_session_membership.handle_membership_update((const s_network_message_membership_update *)&g_network_message_decode_buffer);
        return;
    }
    case _network_message_type_peer_properties:
    {
        if (network_channel->get_state() != _network_channel_state_connected)
            return;
        c_network_session *session = m_session_manager->get_session((const s_transport_secure_identifier *)&g_network_message_decode_buffer);
        if (!session || !session->is_host())
            return;
        session->handle_peer_properties(network_channel, (const s_network_message_peer_properties *)&g_network_message_decode_buffer);
        return;
    }
    case _network_message_type_delegate_leadership:
    {
        if (network_channel->get_state() != _network_channel_state_connected)
            return;
        c_network_session *session = m_session_manager->get_session((const s_transport_secure_identifier *)&g_network_message_decode_buffer);
        if (!session || !session->is_host())
            return;
        session->handle_delegate_leadership(network_channel, (const s_network_message_delegate_leadership *)&g_network_message_decode_buffer);
        return;
    }
    case _network_message_type_boot_machine:
    {
        if (network_channel->get_state() != _network_channel_state_connected)
            return;
        c_network_session *session = m_session_manager->get_session((const s_transport_secure_identifier *)&g_network_message_decode_buffer);
        if (!session || !session->is_host())
            return;
        session->handle_boot_machine(network_channel, (const s_network_message_boot_machine *)&g_network_message_decode_buffer);
        return;
    }
    case _network_message_type_player_add:
    {
        if (network_channel->get_state() != _network_channel_state_connected)
            return;
        c_network_session *session = m_session_manager->get_session((const s_transport_secure_identifier *)&g_network_message_decode_buffer);
        if (!session || !session->is_host())
            return;
        session->handle_player_add(network_channel, (const s_network_message_player_add *)&g_network_message_decode_buffer);
        return;
    }
    case _network_message_type_player_refuse:
    {
        if (network_channel->get_state() != _network_channel_state_connected)
            return;
        c_network_session *session = m_session_manager->get_session((const s_transport_secure_identifier *)&g_network_message_decode_buffer);
        if (!session || !session->channel_is_authoritative(network_channel))
            return;
        session->handle_player_refuse(network_channel, (const s_network_message_player_refuse *)&g_network_message_decode_buffer);
        return;
    }
    case _network_message_type_player_remove:
    {
        if (network_channel->get_state() != _network_channel_state_connected)
            return;
        c_network_session *session = m_session_manager->get_session((const s_transport_secure_identifier *)&g_network_message_decode_buffer);
        if (!session || !session->is_host())
            return;
        session->handle_player_remove(network_channel, (const s_network_message_player_remove *)&g_network_message_decode_buffer);
        return;
    }
    case _network_message_type_player_properties:
    {
        if (network_channel->get_state() != _network_channel_state_connected)
            return;
        c_network_session *session = m_session_manager->get_session((const s_transport_secure_identifier *)&g_network_message_decode_buffer);
        if (!session || !session->is_host())
            return;
        session->handle_player_properties(network_channel, (const s_network_message_player_properties *)&g_network_message_decode_buffer);
        return;
    }
    case _network_message_type_parameters_update:
    {
        if (network_channel->get_state() != _network_channel_state_connected)
            return;
        c_network_session *session = m_session_manager->get_session((const s_transport_secure_identifier *)&g_network_message_decode_buffer);
        if (!session || !session->channel_is_authoritative(network_channel))
            return;
        if (session->is_host())
            return;
        bool session_active = (unsigned long)session->m_local_state >= 4 && (unsigned long)session->m_local_state < 10;
        if (!session_active)
        {
            if (session->m_local_state != 2)
                return;
        }
        if (session->leaving_session())
            return;
        if (network_message_handler_session_parameters(session)->handle_update((const s_network_message_parameters_update *)&g_network_message_decode_buffer))
            return;
        session->handle_disconnection();
        return;
    }
    case _network_message_type_parameters_request:
        if (network_channel->get_state() != _network_channel_state_connected)
            return;
        handle_parameters_request(network_channel, (const s_network_message_parameters_request *)&g_network_message_decode_buffer);
        return;
    case _network_message_type_view_establishment:
    {
        if (network_channel->get_state() != _network_channel_state_connected)
            return;
        c_simulation_view *view = simulation_get_remote_view_by_channel(network_channel);
        if (!view)
            return;
        const s_network_message_view_establishment *message = (const s_network_message_view_establishment *)&g_network_message_decode_buffer;
        const void *gamestate_data = message->data;
        if (!message->has_data)
            gamestate_data = 0;
        if (message->has_data)
            view->handle_remote_establishment((e_simulation_view_establishment_mode)message->establishment_mode,
                message->establishment_identifier, message->data_size, gamestate_data);
        else
            view->handle_remote_establishment((e_simulation_view_establishment_mode)message->establishment_mode,
                message->establishment_identifier, -1, gamestate_data);
        return;
    }
    case _network_message_type_player_acknowledge:
    {
        if (network_channel->get_state() != _network_channel_state_connected)
            return;
        c_simulation_view *view = simulation_get_remote_view_by_channel(network_channel);
        if (!view || ((1u << view->m_view_type) & 20) == 0)
            return;
        const s_network_message_player_acknowledge *message = (const s_network_message_player_acknowledge *)&g_network_message_decode_buffer;
        view->handle_player_acknowledge(message->valid_mask, message->in_game_mask, message->identifiers);
        return;
    }
    case _network_message_type_synchronous_update:
    {
        if (network_channel->get_state() != _network_channel_state_connected)
            return;
        c_simulation_view *view = simulation_get_remote_view_by_channel(network_channel);
        bool handled = false;
        if (view && view->m_view_type == 1)
        {
            c_simulation_world *world = view->m_world;
            bool accepted = false;
#ifdef HALO3_64BIT
            if (world->m_world_state == 4)
                accepted = simulation_world_handle_synchronous_update(world, (const simulation_update *)&g_network_message_decode_buffer,
                    (const s_simulation_update_metadata *)network_message_offset_pointer(&g_network_message_decode_buffer,
                        offsetof(s_network_message_synchronous_update, metadata)));
#else
            if (world->m_world_state == 4)
                accepted = simulation_world_handle_synchronous_update(world, (const simulation_update *)&g_network_message_decode_buffer,
                    (const s_simulation_update_metadata *)network_message_offset_pointer(&g_network_message_decode_buffer, 5288));
#endif
            if (accepted)
                handled = true;
        }
        if (!handled)
            simulation_destroy_update((simulation_update *)&g_network_message_decode_buffer);
        return;
    }
    case _network_message_type_synchronous_playback_control:
    {
        if (network_channel->get_state() != _network_channel_state_connected)
            return;
        c_simulation_view *view = simulation_get_remote_view_by_channel(network_channel);
        if (!view || view->m_view_type != 1)
            return;
        if (!view->m_world->is_playback() || view->m_world->is_authority())
            return;
        c_simulation_world *world = view->m_world;
        const s_network_message_synchronous_update_ack_message *message =
            (const s_network_message_synchronous_update_ack_message *)&g_network_message_decode_buffer;
        simulation_world_handle_synchronous_update_ack(world, message->mode, message->received_time, message->update_number);
        return;
    }
    case _network_message_type_synchronous_actions:
    {
        if (network_channel->get_state() != _network_channel_state_connected)
            return;
        c_simulation_view *view = simulation_get_remote_view_by_channel(network_channel);
        if (!view || view->m_view_type != 2)
            return;
        const s_network_message_synchronous_actions *message = (const s_network_message_synchronous_actions *)&g_network_message_decode_buffer;
        view->handle_synchronous_actions(message->action_number, message->update_number, message->action_mask,
            (const player_action *)message->actions,
            *(const unsigned long *)network_message_offset_pointer(&g_network_message_decode_buffer, 476));
        return;
    }
    case _network_message_type_synchronous_acknowledge:
    {
        if (network_channel->get_state() != _network_channel_state_connected)
            return;
        c_simulation_view *view = simulation_get_remote_view_by_channel(network_channel);
        if (!view || view->m_view_type != 2 || view->m_world->m_world_state != 4)
            return;
        view->m_synchronous_update_number = *(const long *)network_message_offset_pointer(&g_network_message_decode_buffer, 0);
        return;
    }
    case _network_message_type_distributed_game_results:
    {
        if (network_channel->get_state() != _network_channel_state_connected)
            return;
        c_simulation_view *view = simulation_get_remote_view_by_channel(network_channel);
        if (!view || view->m_view_type != 3)
            return;
        const s_network_message_distributed_game_results *message = (const s_network_message_distributed_game_results *)&g_network_message_decode_buffer;
        long establishment_identifier = message->establishment_identifier;
        long update_number = message->update_number;
        if (establishment_identifier != view->m_establishment_identifier)
            return;
        c_game_results_replicator *replicator = &((s_simulation_distributed_view *)view->m_distributed_view)->game_results;
        if (!replicator->m_receiving_updates)
            return;
        if (update_number != 0 && update_number != replicator->m_next_update_number)
        {
            replicator->fatal_error = true;
            return;
        }
        s_game_results_incremental *incremental = &replicator->m_incremental;
        game_results_apply_incremental_update(incremental, (const s_game_results_incremental_update *)message->update_data);
        game_results_set_incremental(incremental);
        replicator->m_next_update_number = update_number + 1;
        return;
    }
    case _network_message_type_connect_establish:
        network_message_handler_handle_connect_establish(network_channel, (const s_network_message_connect_establish *)&g_network_message_decode_buffer);
        return;
    case _network_message_type_synchronous_gamestate:
    {
        if (network_channel->get_state() != _network_channel_state_connected)
            return;
        c_simulation_view *view = simulation_get_remote_view_by_channel(network_channel);
        const s_network_message_synchronous_gamestate *message = (const s_network_message_synchronous_gamestate *)&g_network_message_decode_buffer;
        if (message_storage_size - 16 != message->chunk_size)
            return;
        if (!view || view->m_view_type != 1)
            return;
        view->handle_synchronous_gamestate(message, network_message_offset_pointer(&g_network_message_decode_buffer, 16));
        return;
    }
    case _network_message_type_synchronous_client_ready:
    {
        c_simulation_view *view = simulation_get_remote_view_by_channel(network_channel);
        if (!view || view->m_view_type != 2 || view->m_catchup_attempt == -1)
            return;
        network_time_since(view);
        return;
    }
    case _network_message_type_test:
        break;
    default:
        return;
    }
}
