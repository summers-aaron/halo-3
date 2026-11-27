// networkinglibxbox/network_session_state.obj: from the records 0x4241a0 0x424200 0x4243e0 0x424570 0x424648 0x424708 0x424790 0x424a08 0x424c78 0x425060 0x425188 0x425828 0x425b40 0x425f90 0x425390
// @flags /TP /O2 /Oc
#include "networking/session/network_session_membership.h"
#include "networking/session/network_session_local_state_types.h"
#include "networking/online/online_managed_session_types.h"
#include <xtl.h>
#include "networking/session/network_session_peer_state_types.h"
#include "networking/network_observer_owner_types.h"
#include "memory/tls_globals.h"
#include <string.h>
#include <stdlib.h>
#include "game/game_globals_storage.h"
#include "networking/session/network_managed_session.h"
#include "networking/session/network_managed_session_status.h"
#include "networking/transport/transport_address.h"
#include "networking/transport/transport_security.h"
#include "networking/session/network_session_parameters.h"
#include "networking/messages/network_message_type_collection.h"
#include "networking/session/network_session_types.h"
#include "networking/transport/transport_security_types.h"
#include "networking/session/network_session_find_host_types.h"
#include "networking/transport/transport_platform_types.h"
#include "networking/bandwidth_configuration_host_preference_types.h"
#include "networking/logic/life_cycle/life_cycle_handler_types.h"
#include "networking/session/network_session.h"
#include "networking/session/network_session_membership_queries.h"
#include "networking/session/network_session_state_types.h"
#include "networking/session/network_session_join_request_types.h"
#include "networking/session/network_session_parameter_observer.h"
#include "networking/transport/transport_security_globals.h"
#include "networking/network_globals.h"
#include "networking/network_configuration.h"
#include "networking/network_configuration_types.h"
#include "networking/online/online_presence_accessors.h"
#include "networking/messages/network_message_gateway.h"
#include "cseries/bit_operations.h"
struct s_peer_leaving_state { int reason, time, peer_index; };
struct s_peer_established_state { long reserved; unsigned long time; };
struct s_joining_message { unsigned __int64 id; };
struct s_network_session_election_message {
    s_transport_secure_identifier identifier;
    s_transport_secure_address host_address;
    long peer_count;
    s_transport_secure_address peers[16];
    unsigned long decision_mask, peer_votes[16];
};
struct s_network_session_host_handoff_message {
    s_transport_secure_identifier session_id;
    s_transport_secure_address address;
    unsigned short peer_index;
};
struct s_transport_session_description;
enum e_transport_platform;
// @address 0x424200 ?change_local_state_peer_joining@c_network_session@@AAAXJPBUs_transport_secure_address@@PBUtransport_address@@_N_KPBUs_network_session_join_request@@@Z
// @address 0x424570 ?change_local_state_peer_established@c_network_session@@AAAXXZ
// @address 0x425060 ?idle_peer_creating@c_network_session@@AAAXXZ
// @address 0x425188 ?idle_peer_joining@c_network_session@@AAAXXZ
// @address 0x4243e0 ?change_local_state_peer_join_abort@c_network_session@@AAAXXZ
// @address 0x3a0fd8 ?transport_secure_address_compare@@YA_NPBUs_transport_secure_address@@0@Z
// @address 0x3a0e60 ?transport_secure_address_decode@@YA_NPBUs_transport_session_description@@PBUs_transport_secure_address@@GPAUtransport_address@@@Z
// @address 0x3a1338 ?transport_local_machine_nonce_get@@YA_KXZ
// @address 0x3c9730 ?reset_membership@c_network_session_membership@@QAAXXZ
// @address 0x3d0af0 ?check_to_send_peer_join_abort@c_network_session@@QAAXXZ
// @address 0x3d9568 ?observer_channel_send_message@c_network_observer@@QAAXW4e_network_observer_owner@@J_NW4e_network_message_type@@JPBX@Z
// @address 0x3db638 ?get_secure_connection_status@c_network_observer@@ABA?AW4e_transport_secure_connection@@J@Z
// @address 0x3fe418 ?managed_session_get_id@@YA_NJPAUs_transport_secure_identifier@@@Z
// @address 0x4241a0 ?change_local_state_internal@c_network_session@@AAAXW4e_network_session_state@@JPBX@Z
// @address 0x424648 ?change_local_state_peer_leaving@c_network_session@@AAAXXZ
// @address 0x080608 GetTickCount
// @address 0x424708 ?change_local_state_host_established@c_network_session@@AAAXXZ
// @address 0x3a1600 ?is_host@c_network_session@@QBA_NXZ
// @address 0x3d05b8 ?reset_local_host_state@c_network_session@@AAAX_N@Z
// @address 0x424790 ?change_local_state_host_handoff@c_network_session@@AAAXJ_N00@Z
// @address 0x3c99d8 ?remove_peer@c_network_session_membership@@QAAXJ@Z
// @address 0x3e53f8 ?get_established_peer_count@c_network_session_membership@@QBAJXZ


extern "C" {
#include "networking/network_time_types.h"
// @address 0x8c447c g_network_time_globals
}



static __forceinline unsigned long network_session_state_time_get()
{
    unsigned long result;
    if (g_network_time_globals.enabled) result = g_network_time_globals.time;
    else result = GetTickCount();
    return result;
}

// @address 0x3ca410 network_session_membership_is_synchronized

class c_network_message_gateway;


// Inline helper that clears the local state data and copies in the new state payload
static __forceinline void change_local_state_inline(c_network_session *session, e_network_session_state new_state, long state_data_size, const void *state_data)
{
    memset(((char *)&session->m_local_state_data), 0, sizeof(session->m_local_state_data));
    if (state_data != 0)
        memcpy(((char *)&session->m_local_state_data), state_data, state_data_size);
    session->m_local_state = new_state;
}
void c_network_session::change_local_state_internal(e_network_session_state new_state, long state_data_size, const void *state_data)
{
    memset(((char *)&m_local_state_data), 0, sizeof(m_local_state_data));
    if (state_data != 0)
        memcpy(((char *)&m_local_state_data), state_data, state_data_size);
    m_local_state = new_state;
}

// match: 0x424648: emit the memcpy below as an out-of-line call
#pragma function(memcpy)
void c_network_session::change_local_state_peer_leaving()
{
    s_peer_leaving_state leaving_state;
    memset(&leaving_state, 0, sizeof(leaving_state));
    int current_time;
    if (g_network_time_globals.enabled != 0)
        current_time = (int)g_network_time_globals.time;
    else
        current_time = GetTickCount();
    leaving_state.time = current_time;
    leaving_state.peer_index = -1;
    if (m_local_state == 4)
        leaving_state.reason = (int)m_local_state_data.peer_leaving.peer_reestablish_state;
    memset(((char *)&m_local_state_data), 0, sizeof(m_local_state_data));
    memcpy(((char *)&m_local_state_data), &leaving_state, sizeof(leaving_state));
    m_local_state = _network_session_state_peer_leaving;
}

// match: 0x424648: re-enable intrinsic memcpy past this function
#pragma intrinsic(memcpy)
void c_network_session::change_local_state_host_established()
{
    if (!is_host() && m_local_state != 0) {
        reset_local_host_state(true);
        m_session_membership.m_current_membership.host_peer_index = m_session_membership.m_local_peer_index;
    }
    change_local_state_inline(this, _network_session_state_host_established, 0, 0);
}

void c_network_session::change_local_state_host_handoff(long peer_index, bool leaving, bool option1, bool option2)
{
    for (long peer = (&m_session_membership)->get_first_peer(); peer != -1; ) {
        long next_peer = (&m_session_membership)->get_next_peer(peer);
        if (m_session_membership.m_local_peer_index != peer && m_session_membership.m_current_membership.host_peer_index != peer &&
            m_session_membership.is_peer_valid(peer) && m_session_membership.m_current_membership.peers[peer].connection_state != 8)
            m_session_membership.remove_peer(peer);
        peer = next_peer;
    }
    if (m_session_membership.get_established_peer_count() > 1) {
        long mode = -1;
        if ((*(c_network_session_parameters *)m_session_parameters).m_parameters.ui_game_mode.get_allowed()) mode
            = *(*(c_network_session_parameters *)m_session_parameters).m_parameters.ui_game_mode.get();
        long handoff_mode;
        if (m_session_type == 2) handoff_mode = 1;
        else {
            game_globals_storage *globals = tls_game_globals();
            bool in_progress = false;
            if (globals && globals->in_progress && !globals->initializing && globals->map_active) in_progress = true;
            if (in_progress && globals->options.game_mode == 3 && !option1) handoff_mode = 0;
            else if (mode == 0 || mode == 4) handoff_mode = 3;
            else handoff_mode = 2;
        }
        s_network_session_state_host_handoff state;
        memset(&state, 0, sizeof(state));
        state.host_leaving = leaving;
        state.handoff_peer_index = -1;
        state.start_time = network_session_state_time_get();
        state.option1 = option1;
        state.option2 = option2;
        state.handoff_mode = handoff_mode;
        if (peer_index != -1) state.peer_consideration_mask = 1 << peer_index;
        else state.peer_consideration_mask = m_session_membership.get_peer_valid_mask();
        if (leaving) state.peer_consideration_mask &= ~(1 << m_session_membership.m_local_peer_index);
        change_local_state_inline(this, _network_session_state_host_handoff, sizeof(state), &state);
    } else if (leaving) {
        initiate_leave_protocol(true);
    }
}

// @address 0x7fc368 g_transport_security_globals
extern "C" {
// accepted c_network_session::disconnect;

}
struct s_network_configuration;

// @address 0x424200 ?change_local_state_peer_joining@c_network_session@@AAAXJPBUs_transport_secure_address@@PBUtransport_address@@_N_KPBUs_network_session_join_request@@@Z
void c_network_session::change_local_state_peer_joining(long channel, const s_transport_secure_address *address,
    const transport_address *usable_address, bool use_join_nonce, unsigned __int64 join_nonce,
    const s_network_session_join_request *request)
{
    s_network_session_state_peer_joining state;
    memset(&state, 0, sizeof(state));
    state.observer_channel_index = channel;
    state.join_address = *address;
    state.join_usable_address = *usable_address;
    state.join_start_timestamp = network_session_state_time_get();
    state.join_holding_in_queue_timestamp = -1;
    state.join_secure_connection_timestamp = 0;
    state.use_join_nonce = use_join_nonce;
    state.join_nonce = join_nonce;
    state.join_request = *request;
    reset_connection_state();
    if (transport_secure_address_compare(address, &g_transport_security_globals.secure_address))
    {
        if (channel != -1)
            m_observer->m_channel_observers[channel].owner_flags &= ~(1 << m_session_index);
        m_join_refuse_reason = _network_join_refuse_reason_tried_to_join_self;
        disconnect();
    }
    else
    {
        m_session_membership.reset_membership();
        m_session_membership.add_peer(0, _network_session_peer_state_established, 0, address, 0, 0);
        m_session_membership.m_current_membership.host_peer_index = 0;
        m_session_membership.add_peer(1, _network_session_peer_state_established, transport_local_machine_nonce_get(),
            &g_transport_security_globals.secure_address, 0, 0);
        m_session_membership.m_local_peer_index = 1;
        m_session_membership.m_current_membership.update_number = -1;
        change_local_state_internal(_network_session_state_peer_joining, sizeof(state), &state);
    }
}

void c_network_session::change_local_state_peer_join_abort()
{
    s_network_session_state_peer_join_abort state;
    memset(&state, 0, sizeof(state));
    state.join_address = ((s_network_session_state_peer_joining *)((char *)&m_local_state_data))->join_address;
    state.join_usable_address = ((s_network_session_state_peer_joining *)((char *)&m_local_state_data))->join_usable_address;
    state.local_address = g_transport_security_globals.secure_address;
    if (((s_network_session_state_peer_joining *)((char *)&m_local_state_data))->use_join_nonce)
        state.join_nonce = ((s_network_session_state_peer_joining *)((char *)&m_local_state_data))->join_nonce;
    else
        state.join_nonce = ((s_network_session_state_peer_joining *)((char *)&m_local_state_data))->join_request.join_nonce;
    state.join_start_timestamp = ((s_network_session_state_peer_joining *)((char *)&m_local_state_data))->join_start_timestamp;
    state.join_abort_start_timestamp = network_session_state_time_get();
    reset_connection_state();
    m_session_membership.reset_membership();
    change_local_state_inline(this, _network_session_state_peer_join_abort, sizeof(state), &state);
    check_to_send_peer_join_abort();
}
void c_network_session::change_local_state_peer_established()
{
    s_peer_established_state state;
    memset(&state, 0, sizeof(state));
    state.time = network_session_state_time_get();
    reset_connection_state();
    change_local_state_inline(this, _network_session_state_peer_established, sizeof(state), &state);
    check_to_send_time_synchronization();
}
static __forceinline bool flags_u16_test(unsigned short flags, long bit) {
    bool result = (flags >> bit) & 1;
    return result;
}
void c_network_session::idle_peer_creating()
{
    bool error = false;
    unsigned short status_buffer;
    long index = m_managed_session_index;
    unsigned short status = *managed_session_get_status(&status_buffer, index);
    if (flags_u16_test(status, 2)) {
        error = true;
    } else if (flags_u16_test(status, 1)) {
        s_transport_session_description description;
        transport_address address;
        if (!::managed_session_get_security_information(index, false, &description, 0)) {
            error = true;
        } else if (!transport_secure_address_decode(&description, &description.secure_address, 1001, &address)) {
            error = true;
        } else {
            long channel = m_observer->allocate_observer((e_network_observer_owner)m_session_index, &description.secure_address);
            if (channel == -1) {
                error = true;
            } else {
                change_local_state_peer_joining(channel, &description.secure_address, &address,
                    ((s_network_session_state_peer_creating *)((char *)&m_local_state_data))->unknown,
                    ((s_network_session_state_peer_creating *)((char *)&m_local_state_data))->join_nonce,
                    &((s_network_session_state_peer_creating *)((char *)&m_local_state_data))->join_request);
            }
        }
    } else if (!flags_u16_test(status, 0)) {
        error = true;
    }
    if (error) disconnect();
}
static __forceinline bool peer_joined_state(const c_network_session_membership *m_session_membership, long index) {
    return m_session_membership->m_current_membership.peers[index].connection_state >= 6;
}
void c_network_session::idle_peer_joining()
{
    bool abort_join = false;
    bool send_request = false;
    unsigned long last, now;
    long elapsed = -1;
    if (((s_network_session_state_peer_joining *)((char *)&m_local_state_data))->join_holding_in_queue_timestamp != -1) {
        last = ((s_network_session_state_peer_joining *)((char *)&m_local_state_data))->join_holding_in_queue_timestamp;
        now = network_session_state_time_get();
        elapsed = (long)(now - last);
    } else {
        last = ((s_network_session_state_peer_joining *)((char *)&m_local_state_data))->join_start_timestamp;
        now = network_session_state_time_get();
        elapsed = (long)(now - last);
    }
    bool secure = m_observer->get_secure_connection_status(((s_network_session_state_peer_joining *)((char *)&m_local_state_data))->observer_channel_index)
        == 2;
    if (!secure) {
        if (elapsed > g_network_configuration.peer_join_secure_connection_timeout) {
            abort_join = true;
            m_join_refuse_reason = _network_join_refuse_reason_could_not_connect;
        }
        send_request = true;
    } else {
        if (((s_network_session_state_peer_joining *)((char *)&m_local_state_data))->join_secure_connection_timestamp == 0)
            ((s_network_session_state_peer_joining *)((char *)&m_local_state_data))->join_secure_connection_timestamp = network_session_state_time_get();
        if (m_session_membership.m_current_membership.update_number != -1 &&
            peer_joined_state(&m_session_membership, m_session_membership.m_local_peer_index) &&
            (*(c_network_session_parameters *)m_session_parameters).initial_update_received()) {
            change_local_state_peer_established();
            s_joining_message message;
            memset(&message, 0, sizeof(message));
            managed_session_get_id(m_managed_session_index, (s_transport_secure_identifier *)&message);
            long channel = m_session_membership.m_peer_channels[m_session_membership.m_current_membership.host_peer_index].observer_channel_index;
            if (channel != -1)
                m_observer->observer_channel_send_message(
                    (e_network_observer_owner)m_session_index, channel, false, _network_message_type_peer_establish, sizeof(message), &message);
        } else if (elapsed > g_network_configuration.peer_join_initial_update_timeout) {
            abort_join = true;
            m_join_refuse_reason = _network_join_refuse_reason_join_timed_out;
        } else {
            send_request = true;
        }
    }
    if (abort_join) initiate_leave_protocol(false);
    else if (send_request) check_to_send_peer_join_request();
}

// @address 0x424a08 ?change_local_state_host_reestablish@c_network_session@@AAAX_NJ@Z

void c_network_session::change_local_state_host_reestablish(bool use_election, long reestablishment_mode)
{
    long original_host_peer_index = m_session_membership.m_current_membership.host_peer_index;
    bool set_default_parameters = false;
    if (original_host_peer_index != m_session_membership.m_local_peer_index) {
        if (m_session_membership.m_current_membership.update_number == -1)
            m_session_membership.m_current_membership.update_number = 1;
        c_network_session_parameters *parameters = &(*(c_network_session_parameters *)m_session_parameters);
        if (!parameters->initial_update_received()) {
            set_default_parameters = true;
            parameters->set_initial_update_received();
        }
        m_session_membership.m_current_membership.host_peer_index = m_session_membership.m_local_peer_index;
    }
    s_network_session_state_host_reestablish state;
    memset(&state, 0, sizeof(state));
    state.phase = 1;
    state.reestablish_host_leaving = false;
    state.start_time = network_session_state_time_get();
    state.reestablishment_mode = reestablishment_mode;
    state.requested_peer_mask = 1 << m_session_membership.m_local_peer_index;
    state.acknowledged_peer_mask = 1 << m_session_membership.m_local_peer_index;
    state.use_election = use_election;
    state.original_host_peer_index = original_host_peer_index;
    change_local_state_inline(this, _network_session_state_host_reestablish, sizeof(state), &state);
    if (set_default_parameters)
        set_default_session_parameters();
    for (long peer_index = (&m_session_membership)->get_first_peer(); peer_index != -1;
        peer_index = (&m_session_membership)->get_next_peer(peer_index))
    {
        if (peer_index != m_session_membership.m_local_peer_index) {
            m_session_membership.set_peer_connection_state(peer_index, _network_session_peer_state_rejoining);
            m_session_membership.m_current_membership.peers[peer_index].join_start_time = network_session_state_time_get();
        }
    }
    reset_local_host_state(true);
    if (m_session_membership.m_current_membership.peer_count > 1) {
        for (long peer_index = (&m_session_membership)->get_first_peer(); peer_index != -1;
            peer_index = (&m_session_membership)->get_next_peer(peer_index))
        {
            long observer_channel_index = m_session_membership.m_peer_channels[peer_index].observer_channel_index;
            if (observer_channel_index != -1) {
                m_session_membership.m_peer_channels[peer_index].peer_needs_reestablishment = true;
                if (m_observer->m_channel_observers[observer_channel_index].state == 1)
                    m_observer->observer_channel_initiate_connection((e_network_observer_owner)m_session_index, observer_channel_index);
            }
        }
    }
    unsigned short status_buffer;
    long index = m_managed_session_index;
    managed_session_get_status(&status_buffer, index);
    if (flags_u16_test(status_buffer, 3))
        managed_session_cancel_new_host(index);
}

static __forceinline bool network_session_state_in_squad(c_network_session **session)
{
    bool result = false;
    if (online_presence_life_cycle_globals().initialized) {
        c_network_session *current = online_presence_life_cycle_globals().manager.get_active_squad_session();
        if (!current->disconnected()) {
            if (session) *session = current;
            result = true;
        }
    }
    return result;
}
// @address 0x424c78 ?change_local_state_election@c_network_session@@AAAXXZ
void c_network_session::change_local_state_election()
{
    unsigned short status;
    managed_session_get_status(&status, m_managed_session_index);
    if (flags_u16_test(status, 4) != true || m_session_class != 2) {
        if (flags_u16_test(status, 3)) {
            managed_session_cancel_new_host(m_managed_session_index);
            m_observer->observer_clear_security_flags();
        }
    }
    m_session_membership.reset_transmitted_membership_state();
    c_network_session *squad;
    const c_network_session_membership *election_membership;
    if (m_close_session) {
        if (network_session_state_in_squad(&squad)) {
            if (squad->established()) election_membership = &squad->m_session_membership;
            else election_membership = &m_session_membership;
        } else election_membership = &m_session_membership;
    } else election_membership = &m_session_membership;
    s_network_session_election_state state;
    memset(&state, 0, sizeof(state));
    state.start_time = network_session_state_time_get();
    state.decision_time = network_session_state_time_get();
    network_session_state_generate_election_data(election_membership, &state);
    reset_connection_state();
    change_local_state_inline(this, _network_session_state_election, sizeof(state), &state);
}
static __forceinline long network_session_state_host_peer_index(const c_network_session_membership *m_session_membership)
{
    return m_session_membership->m_current_membership.host_peer_index;
}
static __forceinline const s_transport_secure_address *network_session_state_host_peer_address(const c_network_session_membership *m_session_membership)
{
    return m_session_membership->get_peer_address(network_session_state_host_peer_index(m_session_membership));
}
static __forceinline const s_transport_secure_address *network_session_state_local_peer_address(const c_network_session_membership *m_session_membership)
{
    return m_session_membership->get_peer_address(m_session_membership->m_local_peer_index);
}
__forceinline void c_network_session::network_session_state_generate_election_data(
    const c_network_session_membership *election_membership, s_network_session_election_state *state) const
{
    long count = 0;
    long local_peer = -1, host_peer = -1;
    for (long peer = (election_membership)->get_first_peer(); peer != -1;
        peer = (election_membership)->get_next_peer(peer))
    {
        const s_transport_secure_address *address = election_membership->get_peer_address(peer);
        long insertion;
        for (insertion = 0; insertion < count; ++insertion) {
            if (memcmp(address, &state->peers[insertion], sizeof(*address)) < 0) break;
        }
        if (insertion < count)
            memmove(&state->peers[insertion + 1], &state->peers[insertion], (count - insertion) * sizeof(*address));
        state->peers[insertion] = *address;
        ++count;
    }
    const s_transport_secure_address *local_address = network_session_state_local_peer_address(election_membership);
    const s_transport_secure_address *host_address = network_session_state_host_peer_address(election_membership);
    for (long i = 0; i < count; ++i) {
        if (transport_secure_address_compare(local_address, &state->peers[i])) local_peer = i;
        if (transport_secure_address_compare(host_address, &state->peers[i])) host_peer = i;
    }
    state->host_address = *network_session_state_host_peer_address(&m_session_membership);
    state->local_peer_index = local_peer;
    state->peer_count = count;
    state->local_mask = 1 << local_peer;
    state->peer_votes[local_peer] = 1 << local_peer;
    if (host_peer != -1 && host_peer != local_peer) state->host_mask = 1 << host_peer;
    for (long i = 0; i < count; ++i)
        state->peer_times[i] = network_session_state_time_get();
}
static __forceinline bool network_session_state_decode_election_address(
    const s_transport_secure_address *secure_address, const s_transport_session_description *description, transport_address *address)
{
    in_addr ipv4;
    bool valid = false;
    if (XNetXnAddrToInAddr((const XNADDR *)secure_address, (const XNKID *)&description->secure_identifier, &ipv4) == 0) valid = true;
    if (valid) {
        address->address.ipv4_address = ipv4.s_addr;
        address->address_length = 4;
        address->port = 1000;
        valid = transport_address_valid(address);
    }
    return valid;
}
static __forceinline long network_session_state_peer_channel(const c_network_session_membership *m_session_membership, const long &peer)
{
    return m_session_membership->m_peer_channels[peer].observer_channel_index;
}
static __forceinline bool network_session_state_host_connected(c_network_observer *m_observer, const c_network_session_membership *m_session_membership)
{
    return m_observer->m_channel_observers[network_session_state_peer_channel(m_session_membership,
        m_session_membership->m_current_membership.host_peer_index)].state == 7;
}
static __forceinline bool network_session_state_has_vote(const unsigned long &votes, unsigned long peer_mask)
{
    return (peer_mask & votes) != 0;
}


static __forceinline void network_session_state_start_host_migration(long index)
{
    s_online_managed_session *session = &online_session_manager_globals.sessions[index];
    if ((bool)(_rotl(session->flags, 26) & 1))
        managed_session_cancel_new_host(index);
    session->host_migration_requires_new_key = true;
    session->previous_host = (bool)((session->desired.flags.m_storage >> 0) & 1);
    operation_flag_set(session->pending_operations, 4, true);
    session_flag_set(session->flags, 2, false);
    session->modification_time = 0;
    session_flag_set(session->flags, 6, true);
}

// @address 0x425828 ?idle_host_reestablish@c_network_session@@AAAXXZ
void c_network_session::idle_host_reestablish()
{
    unsigned short status[8];
    s_network_session_state_host_reestablish *state = (s_network_session_state_host_reestablish *)((char *)&m_local_state_data);
    bool reestablished = false;
    managed_session_get_status(status, m_managed_session_index);
    if (state->phase != 1)
    {
        if (state->phase == 2 && flags_u16_test(status[0], 4))
        {
            check_to_send_host_reestablish();
            unsigned long notmask = ~state->reestablishment_mode;
            if (((state->acknowledged_peer_mask | state->left_peer_mask) & notmask) == (m_session_membership.get_peer_valid_mask() & notmask))
            {
                reestablished = true;
                state->phase = 0;
            }
            else
            {
                unsigned long start = state->start_time;
                unsigned long now = network_session_state_time_get();
                if ((long)(now - start) >= g_network_configuration.host_reestablish_acknowledge_timeout)
                {
                    reestablished = true;
                    state->phase = 0;
                }
            }
        }
    }
    else if (!flags_u16_test(status[0], 4))
    {
        network_session_state_start_host_migration(m_managed_session_index);
        state->phase = 2;
    }
    if (reestablished)
    {
        bool complete = state->reestablish_host_leaving;
        bool use_election = state->use_election;
        m_host_migration_attempted_peer_mask = 0;
        unsigned long now = network_session_state_time_get();
        m_last_host_migration_time = now;
        managed_session_use_new_host(m_managed_session_index);
        managed_session_commit_new_host(m_managed_session_index);
        m_observer->observer_clear_security_flags();
        unsigned long combined = (m_session_membership.get_peer_valid_mask() & ~state->acknowledged_peer_mask) | state->left_peer_mask;
        change_local_state_host_established();
        if (combined != 0)
        {
            for (long peer = 15; peer >= 0; peer--)
            {
                if (m_session_membership.is_peer_valid(peer) && peer != m_session_membership.m_local_peer_index && (combined & (1 << peer)) != 0)
                    disband_peer(peer);
            }
        }
        for (long peer = (&m_session_membership)->get_first_peer(); peer != -1;
            peer = (&m_session_membership)->get_next_peer(peer))
            m_session_membership.set_peer_connection_state(peer, _network_session_peer_state_established);
        network_session_update_membership(this);
        (*(c_network_session_parameters *)m_session_parameters).update();
        if (complete)
        {
            initiate_leave_protocol(false);
            return;
        }
        if (use_election)
            change_local_state_host_handoff(-1, false, false, false);
    }
}

// @address 0x425b40 ?idle_election@c_network_session@@AAAXXZ
void c_network_session::idle_election()
{
    unsigned long peer_mask = 0;
    s_network_session_election_state *state = &m_local_state_data.election;
    unsigned long mask = 1;
    for (long peer = 0; peer < state->peer_count; mask <<= 1, ++peer) {
        if (peer != state->local_peer_index && !(state->host_mask & mask)) {
            unsigned long last = state->peer_times[peer];
            if ((long)(network_session_state_time_get() - last) >= g_network_configuration.election_peer_timeout) {
                state->host_mask |= mask;
                state->peer_votes[peer] = 0;
            }
        }
    }
    c_network_session_membership *election_membership = &m_session_membership;
    if (network_session_state_peer_channel(election_membership, election_membership->m_current_membership.host_peer_index) != -1 &&
        !network_session_state_host_connected(m_observer, &m_session_membership))
        state->decision_mask |= 1 << state->local_peer_index;
    unsigned long last_message = state->decision_time;
    if ((long)(network_session_state_time_get() - last_message) >= g_network_configuration.election_message_interval) {
        s_network_session_election_message message;
        memset(&message, 0, sizeof(message));
        long index = m_managed_session_index;
        managed_session_get_id(index, &message.identifier);
        memcpy(&message.host_address, &state->host_address, sizeof(message) - sizeof(message.identifier));
        s_transport_session_description description;
        e_transport_platform platform;
        if (::managed_session_get_security_information(
                index, false, &description, &platform))
        {
            for (long peer = 0; peer < state->peer_count; ++peer) {
                if (peer != state->local_peer_index) {
                    transport_address address;
                    if (network_session_state_decode_election_address(&state->peers[peer], &description, &address)) {
                        e_transport_secure_connection status = transport_secure_connection_status(&address);
                        switch (status) {
                        case _transport_secure_connection_idle:
                        case _transport_secure_connection_lost: {
                            in_addr insecure;
                            if (transport_address_get_insecure_xbox_address(&address, &insecure))
                                XNetConnect(insecure);
                            break;
                        }
                        case _transport_secure_connection_connected:
                            m_message_gateway->send_message_directed(&address, _network_message_type_election, sizeof(message), &message);
                            break;
                        }
                    }
                }
            }
        }
        state->decision_time = network_session_state_time_get();
    }
    unsigned long start_time = state->start_time;
    if ((long)(network_session_state_time_get() - start_time) >= g_network_configuration.election_timeout) {
        handle_disconnection();
    } else if ((state->host_mask | state->local_mask) == (1u << state->peer_count) - 1) {
        long best_count = 0;
        long best_peer = -1;
        for (long peer = 0; peer < state->peer_count; ++peer) {
            unsigned long vote_bits = state->peer_votes[peer];
            vote_bits = ((vote_bits >> 1) & 0x55555555) + (vote_bits & 0x55555555);
            vote_bits = ((vote_bits >> 2) & 0x33333333) + (vote_bits & 0x33333333);
            vote_bits = ((vote_bits >> 4) & 0x0f0f0f0f) + (vote_bits & 0x0f0f0f0f);
            vote_bits = ((vote_bits >> 8) & 0x00ff00ff) + (vote_bits & 0x00ff00ff);
            long count = (vote_bits >> 16) + (vote_bits & 0xffff);
            if (count > best_count) {
                best_count = count;
                best_peer = peer;
            }
        }
        if (best_peer == state->local_peer_index) {
            unsigned long agreeing_peers = 0;
            unsigned long votes = state->peer_votes[state->local_peer_index];
            peer_mask = 1;
            for (long peer = 0; peer < state->peer_count; peer_mask <<= 1, ++peer) {
                if (network_session_state_has_vote(votes, peer_mask)) {
                    long membership_peer = election_membership->get_peer_from_secure_address(&state->peers[peer]);
                    if (membership_peer != -1) agreeing_peers |= 1 << membership_peer;
                }
            }
            change_local_state_host_reestablish(true, ~agreeing_peers);
        }
    }
}

// @address 0x425f90 ?idle_observer_state@c_network_session@@AAAXXZ
void c_network_session::idle_observer_state(void)
{
    if (this->disconnected()) return;
    for (long peer = (&this->m_session_membership)->get_first_peer(); peer != -1;
        peer = (&this->m_session_membership)->get_next_peer(peer))
    {
        long channel = this->m_session_membership.m_peer_channels[peer].observer_channel_index;
        if (channel != -1) {
            bool connect;
            if ((this)->is_host()) connect = true;
            else if ((this)->leaving_session()) connect = false;
            else if (this->m_session_membership.m_current_membership.update_number == -1) connect = false;
            else if (!this->established() && this->m_local_state != 2) connect = true;
            else connect = this->m_session_membership.m_current_membership.peers[peer].connection_state == 8;
            if (connect && this->m_observer->m_channel_observers[channel].state != 1)
                this
                    ->m_observer->observer_channel_initiate_connection(
                        (e_network_observer_owner)this->m_session_index, channel);
        }
    }
}


static __forceinline bool network_session_state_handoff_stabilized(const c_network_session_membership *m_session_membership)
{
    bool result = false;
    unsigned long valid_mask = m_session_membership->get_peer_valid_mask();
    long peer = m_session_membership->get_first_peer();
    if (peer != -1) {
        long host_peer = m_session_membership->m_current_membership.host_peer_index;
        do {
            if (peer != host_peer && m_session_membership->m_current_membership.peers[peer].properties.connectivity.peer_connectivity_mask == valid_mask) {
                result = true;
                break;
            }
            peer = m_session_membership->get_next_peer(peer);
        } while (peer != -1);
    }
    return result;
}

// @address 0x425390 ?idle_host_handoff@c_network_session@@AAAXXZ
void c_network_session::idle_host_handoff()
{
    s_transport_secure_identifier transition_id;
    bool finish = false;
    s_network_session_state_host_handoff *state = &m_local_state_data.host_handoff;
    if (!state->stabilized) {
        if (!is_peer_joining_this_session() && network_session_membership_is_synchronized(&m_session_membership)) {
            state->stabilized = true;
        } else if (is_peer_joining_this_session() || state->handoff_mode != 0 || !state->host_leaving
            || !network_session_state_handoff_stabilized(&m_session_membership)) {
            unsigned long start = state->start_time;
            if ((long)(network_session_state_time_get() - start) >
                g_network_configuration.host_handoff_stabilize_timeout)
                state->stabilized = true;
        } else {
            state->stabilized = true;
        }
    }
    if (state->stabilized && state->handoff_peer_index == -1) {
        if (state->peer_consideration_mask != 0) {
            if (state->host_leaving)
                state->peer_consideration_mask &= ~(1 << m_session_membership.m_local_peer_index);
            long best_peer = find_good_peer_to_be_host((e_bandwidth_configuration_host_preference)state->handoff_mode, false,
                state->host_leaving, true, -1, (long)~state->peer_consideration_mask, 0, true)
                                 .host_peer_index;
            if (best_peer != m_session_membership.m_local_peer_index) {
                if (best_peer != -1) {
                    state->handoff_peer_index = best_peer;
                    state->peer_consideration_mask &= ~(1 << best_peer);
                }
            } else {
                finish = true;
            }
        } else {
            finish = true;
        }
    }
    long peer_index = state->handoff_peer_index;
    if (peer_index != -1) {
        s_network_session_host_handoff_message message;
        if (!state->handoff_initiated) {
            memset(&message, 0, sizeof(message));
            managed_session_get_id(m_managed_session_index, &message.session_id);
            message.peer_index = (unsigned short)peer_index;
            memcpy(&message.address, &m_session_membership.m_current_membership.peers[peer_index].secure_address, sizeof(message.address));
            send_message_to_all_peers(_session_message_transmission_type_direct, _network_message_type_host_handoff, sizeof(message), &message);
            state->handoff_initiated = true;
            state->handoff_initiated_time = network_session_state_time_get();
            state->acknowledged_peer_mask = 1 << m_session_membership.m_local_peer_index;
            for (long peer = (&m_session_membership)->get_first_peer(); peer != -1;
                peer = (&m_session_membership)->get_next_peer(peer))
            {
                long channel = m_session_membership.m_peer_channels[peer].observer_channel_index;
                if (channel != -1 && m_observer->m_channel_observers[channel].state == 1)
                    state->acknowledged_peer_mask |= 1 << peer;
            }
        }
        if (state->handoff_initiated && !state->handoff_transition_sent) {
            unsigned long reply_start = state->handoff_initiated_time;
            unsigned long now = network_session_state_time_get();
            bool send_transition = false;
            if (state->acknowledged_peer_mask == m_session_membership.get_peer_valid_mask())
                send_transition = true;
            else if ((long)(now - reply_start) >= g_network_configuration.host_handoff_acknowledge_timeout) {
                if (((1 << state->handoff_peer_index) & state->acknowledged_peer_mask) != 0)
                    send_transition = true;
                else
                    handoff_remove_peer_from_consideration(state->handoff_peer_index);
            }
            if (send_transition) {
                memset(&transition_id, 0, sizeof(transition_id));
                managed_session_get_id(m_managed_session_index, &transition_id);
                long channel = m_session_membership.m_peer_channels[state->handoff_peer_index].observer_channel_index;
                if (channel != -1)
                    m_observer->observer_channel_send_message((e_network_observer_owner)m_session_index, channel, false,
                        _network_message_type_host_transition, sizeof(transition_id), &transition_id);
                state->handoff_transition_sent = true;
                state->handoff_transition_time = network_session_state_time_get();
                m_session_membership.m_host_transition_peer_index = state->handoff_peer_index;
            }
        }
        if (state->handoff_transition_sent) {
            unsigned long transition_start = state->handoff_transition_time;
            if ((long)(network_session_state_time_get() - transition_start) >=
                g_network_configuration.host_handoff_transition_timeout)
                handoff_remove_peer_from_consideration(state->handoff_peer_index);
        }
    } else {
        unsigned long start = state->start_time;
        if ((long)(network_session_state_time_get() - start) >= g_network_configuration.host_handoff_timeout)
            finish = true;
    }
    if (finish) {
        bool leaving = state->host_leaving;
        change_local_state_host_established();
        if (leaving)
            initiate_leave_protocol(true);
    }
}
