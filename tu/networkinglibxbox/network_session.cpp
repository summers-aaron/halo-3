// networkinglibxbox/network_session.obj: from the records 0x122f80 0x3ca4a0 0x3cc920 0x3cca70 0x3ccf80 0x3cd3e8 0x3cd4f8 0x3cd790 0x3cd870 0x3cdb80 0x3ce2b0 0x3ce468 0x3ce568 0x3ce618 0x3ce808 0x3ce908 0x3ce9c8 0x3cea68 0x3ceb90 0x3cee70 0x3cef48 0x3cefb0 0x3cf108 0x3cf288 0x3cf3f8 0x3cf560 0x3cf5e8 0x3cf648 0x3cf700 0x3cf860 0x3cfb98 0x3cfc80 0x3cfd40 0x3cfe00 0x3cffa8 0x3d0020 0x3d0060 0x3d0148 0x3d0300 0x3d03a8 0x3d04a0 0x3d05b8 0x3d0670 0x3d06f8 0x3d0910 0x3d0af0 0x3d0bc0 0x3d0ce0 0x3d0e90 0x3d11f0 0x3d1278 0x3d1400 0x3d1528 0x3d15e8 0x3d1660 0x3d16c8 0x3d1748 0x3d1b38 0x3d1d68
// @compiler xdk6130
// @flags /TP /O2 /J /Oc
#include "cseries/crt_xdk.h"
#include "networking/session/network_session_runtime_bindings.h"
#include "networking/session/network_session_creation.h"

#include "networking/simulation/simulation_accessors.h"
#include "memory/tls_globals.h"
#include "networking/logic/life_cycle/life_cycle_globals_access.h"
// @address 0xf11308 g_network_life_cycle_globals
#include "networking/session/network_session_membership_queries.h"
#include "networking/session/network_session_queries.h"
#include "networking/session/network_session_strings.h"
#include "interface/user_interface.h"
#include "interface/gui_game_mode_types.h"
#include "networking/session/network_session_peer_state_types.h"
#include "networking/delivery/network_channel_closure_reason_types.h"
#include "networking/session/network_session_peer_properties_status_flags_types.h"
#include "networking/network_observer_owner_types.h"
#include "cseries/cseries_wrapped.h"
#include "simulation/simulation.h"
#include "simulation/simulation_world.h"
#include "networking/session/network_observer_join_queries.h"
#include "interface/user_interface_networking.h"
#include "game/multiplayer_game_hopper.h"
#include "networking/logic/network_join.h"
#include "networking/session/network_managed_session.h"
#include "networking/transport/transport_address.h"
#include "networking/transport/transport_security.h"
#include "interface/user_interface_session.h"
#include "networking/logic/network_session_interface.h"
#include "networking/messages/network_message_type_collection.h"
#include "networking/session/network_session_types.h"
#include "networking/session/network_session_class.h"
#include "networking/session/network_session_peer_types.h"
#include "shell/shell.h"
#include "networking/network_configuration_types.h"
#include "interface/user_interface_error_manager.h"
#include "networking/session/network_session_local_state_types.h"
#include "simulation/simulation_world_types.h"
#include "simulation/simulation_globals.h"
#include "networking/transport/transport_security_types.h"
#include "networking/messages/network_messages_session_protocol.h"
#include "networking/transmitted_membership_entry_types.h"
#include "networking/network_utilities.h"
#include "networking/session/network_session_find_host_types.h"
#include "networking/transport/transport_platform_types.h"
#include "networking/transport/transport_unique_identifier_types.h"
#include "networking/bandwidth_configuration_host_preference_types.h"
#include "networking/session/network_session_membership.h"
#include "networking/session/network_session.h"
class c_network_channel;
struct s_transport_session_description;
enum e_transport_platform;
// @address 0x3cffa8 ?desire_channel_heartbeat@c_network_session@@UBA_NJ@Z
// @address 0x3cd790 ?ready_for_remote_peers_to_join@c_network_session@@QBA_NXZ
// @address 0x3ce2b0 ?add_pending_join_to_session@c_network_session@@AAAX_K@Z
// @address 0x3ce808 ?disconnect@c_network_session@@QAAXXZ
// @address 0x3cf700 ?disband_and_reestablish_as_host@c_network_session@@QAAX_N@Z
// @address 0x3cee70 ?peer_request_player_add_status@c_network_session@@QAA?AW4e_network_join_refuse_reason@@JPBUs_player_identifier@@@Z
// @address 0x3cef48 ?peer_complete_player_add@c_network_session@@QAAXJPBUs_player_identifier@@@Z
// @address 0x3cf288 ?leader_request_delegate_leadership@c_network_session@@QAA_NPBUs_transport_secure_address@@@Z
// @address 0x3cc920 ?initialize_session@c_network_session@@QAA_NJW4e_network_session_type@@PAVc_network_message_gateway@@PAVc_network_observer@@PAVc_network_session_manager@@@Z
// @address 0x3d0060 ?attempt_channel_reconnection@c_network_session@@UBA_NJ_N@Z
// @address 0x3d0300 ?channel_is_authoritative@c_network_session@@QAA_NPAVc_network_channel@@@Z
// @address 0x3ca4a0 ?set_peer_properties@c_network_session_membership@@QAAXJPBUs_network_session_peer_properties@@@Z
// @address 0x3cca70 ?create_host_session@c_network_session@@QAA_NV?$c_flags@W4e_session_creation_flags@@E$03@@W4e_network_session_class@@W4e_transport_platform@@PBVc_network_session_membership@@PBUs_online_context@@J@Z
// @address 0x3cfc80 ?is_peer_joining_this_session@c_network_session@@QBA_NXZ
// @address 0x3d0020 ?channel_is_load_bearing@c_network_session@@UBAJJ@Z
// @address 0x3d05b8 ?reset_local_host_state@c_network_session@@AAAX_N@Z
// @address 0x3d0670 ?handoff_remove_peer_from_consideration@c_network_session@@AAAXJ@Z
// @address 0x3d11f0 ??0s_network_message_membership_update@@QAA@XZ
// @address 0x3d1660 network_session_get_maximum_player_count
// @address 0x3d16c8 network_session_get_join_refuse_reason
// @address 0x3cd870 ?set_default_session_parameters@c_network_session@@QAAXXZ
// @address 0x3ce9c8 ?get_secure_key@c_network_session@@QBA_NPAUs_transport_secure_identifier@@PAUs_transport_secure_key@@PAW4e_transport_platform@@@Z
// @address 0x3cf5e8 ?host_delegate_host@c_network_session@@QAA_NJ_N0@Z
// @address 0x3d0af0 ?check_to_send_peer_join_abort@c_network_session@@QAAXXZ
// @address 0x3d1d68 ?acknowledge_join_request@c_network_session@@QAAXPBUtransport_address@@W4e_network_join_refuse_reason@@@Z
// @address 0x3ce468 network_session_refuse_pending_player
// @address 0x3ce618 ?initiate_leave_protocol@c_network_session@@AAAX_N@Z
// @address 0x3cea68 ?peer_request_properties_update@c_network_session@@QAA_NPBUs_transport_secure_address@@PBUs_network_session_peer_properties@@@Z
// @address 0x3cf108 ?peer_request_player_remove@c_network_session@@QAA_NJ@Z
// @address 0x3cf3f8 ?leader_request_boot_machine@c_network_session@@QAA_NPBUs_transport_secure_address@@W4e_network_session_boot_reason@@@Z
// @address 0x3d0148 ?notify_channel_connection@c_network_session@@UAAXJK_N@Z
// @address 0x3d04a0 ?disband_peer@c_network_session@@AAAXJ@Z
// @address 0x3d0bc0 ?check_to_send_peer_leave_request@c_network_session@@AAAXXZ
// @address 0x3d0910 ?check_to_send_peer_join_request@c_network_session@@AAAXXZ
// @address 0x3d1528 ?send_message_to_peer@c_network_session@@ABAXW4e_session_message_transmission_type@1@JW4e_network_message_type@@JPBX@Z
// @address 0x3d03a8 ?boot_peer@c_network_session@@AAAXJW4e_network_session_boot_reason@@@Z
// @address 0x3d15e8 ?send_message_to_all_peers@c_network_session@@ABAXW4e_session_message_transmission_type@1@W4e_network_message_type@@JPBX@Z
// @address 0x3cf560 ?host_boot_machine@c_network_session@@QAA_NJW4e_network_session_boot_reason@@@Z
// @address 0x3cdb80 ?process_pending_joins@c_network_session@@AAAXXZ
#include <string.h>
#include <stdlib.h>
#include "cseries/crt_xdk.h"
class c_network_session;
class c_network_channel;
class c_network_session_manager;
#include "networking/session/network_session_join_request_types.h"
#include "networking/logic/network_join_queue_types.h"
#pragma pack(push, 4)
#pragma pack(pop)

#include "networking/session/network_session_checksum.h"

#include "networking/session/network_session_parameter_payload_types.h"
#include "networking/session/network_session_parameter_session_types.h"
#include "networking/session/network_session_parameters_generic.h"
#include "networking/session/network_session_parameters.h"
#include "networking/session/network_session_parameters_generic.h"
#include "networking/messages/network_message_gateway.h"
#include "networking/session/network_session_parameters_base.h"
#include "networking/network_time_types.h"
#include "networking/network_time_access.h"
struct s_network_session13_time_value { unsigned long value; };
#include "networking/online/online_managed_session_access.h"
enum e_peer_reestablish_state { _peer_reestablish_state_none };
#include "networking/session/network_session_parameter_observer.h"
#include "networking/session/network_session_interface_types.h"
#include "networking/logic/network_session_interface_globals.h"
#include "networking/network_configuration.h"
#include "networking/session/network_session_boot_reason_types.h"
#include "networking/session/network_session_manager.h"
#include "networking/messages/network_messages_session_players.h"
struct ns5_peer_properties_message {
    ns5_peer_properties_message() { properties.status_flags = 0; }
    s_transport_secure_identifier identifier;
    s_transport_secure_address address;
    s_network_session_peer_properties properties;
};
#include "networking/messages/network_messages_session_membership.h"
#include "networking/session/network_managed_session_status.h"
#include "networking/logic/life_cycle/life_cycle_globals_types.h"
#include "networking/online/online_managed_session_types.h"
#include "game/game_globals_storage.h"
#include "memory/gameplay_tls_globals.h"
#include "networking/messages/network_messages_session_protocol_types.h"

__forceinline bool c_network_session_parameter_session_size::set_max_peer_count(long maximum_peer_count)
{
    bool result = false;
    if (this->set_allowed()) {
        if (m_data.maximum_peer_count != maximum_peer_count || !(m_flags & 1)) {
            m_data.maximum_peer_count = maximum_peer_count;
            m_flags |= 1;
            for (long peer_index = 0; peer_index < 16; peer_index++)
                m_peers_updated[peer_index] = false;
            notify_set_update_required();
        }
        result = true;
    }
    return result;
}
__forceinline void c_network_session_parameters::check_parameter_registration()
{
    for (unsigned long parameter_index = 0; parameter_index < 47; parameter_index++) {
        c_network_session_parameter_base *parameter = m_parameter_interfaces[parameter_index];
        if (parameter->m_update_priority != 0)
            parameter->get_update_payload_size();
    }
}
inline void c_network_session_parameters::initialize(c_network_session *session, c_network_observer *observer)
{
    m_session = session;
    m_observer = observer;
    register_parameters();
    check_parameter_registration();
}
__forceinline void c_network_session_parameters::transition_state(bool becoming_host)
{
    if (becoming_host) {
        m_peers_reset_mask = 0;
        for (unsigned long parameter_index = 0; parameter_index < 47; parameter_index++) {
            c_network_session_parameter_base *parameter = m_parameter_interfaces[parameter_index];
            parameter->transition_state_to_become_host();
        }
    }
}


struct s_network_message_membership_update;
enum e_controller_index;
struct s_owner;

struct s_owner;
class c_network_observer;

extern "C" {
// @address 0x8c447c g_network_time_globals
}
static __forceinline s_network_session13_time_value network_session13_locked_network_time_get()
{
    s_network_session13_time_value result;
    result.value = g_network_time_globals.time;
    return result;
}
enum e_peer_reestablish_state;
struct s_online_context;
class c_network_channel_owner;
struct s_network_configuration;
static __forceinline bool network_session_get_alpha_is_locked_down()
{
    return g_user_interface_is_alpha != 0 && (g_network_configuration.alpha_locked_down == 0 || g_eula_accepted == 0);
}
static __forceinline unsigned __int64 network_session_get_local_user_xuid(long user_index) { return
    g_network_session_interface_globals.users[user_index].configuration.client.xuid; }
class c_network_session_manager;

__forceinline void c_network_session_manager::register_session(c_network_session *session)
{
    m_sessions[session->session_index()] = session;
}
__forceinline void c_network_session_membership::network_session_set_slot_counts(long public_slot_count, long private_slot_count, bool friends_only)
{
    m_current_membership.private_slot_count = public_slot_count;
    m_current_membership.public_slot_count = private_slot_count;
    m_current_membership.friends_only = friends_only;
    if (get_session() != 0)
        managed_session_modify_slot_counts(get_session()->m_managed_session_index,
            m_current_membership.private_slot_count, m_current_membership.public_slot_count, m_current_membership.friends_only,
                m_current_membership.request_modification);
    increment_update();
}
bool c_network_session::initialize_session(long session_index, e_network_session_type session_type,
    c_network_message_gateway *message_gateway, c_network_observer *observer, c_network_session_manager *session_manager)
{
#if defined(_XBOX) && !defined(__clang__)
    // match: 0x3cc920: gateway, observer and manager load off the
    // g_network_session_manager base, not the argument registers
    c_network_session_manager **globals = (c_network_session_manager **)g_network_session_manager;
    observer = ((c_network_observer **)globals)[-3];
    session_manager = globals[0];
    message_gateway = ((c_network_message_gateway **)globals)[-5];
#endif
    bool success = true;
    m_session_type = session_type;
    m_session_index = session_index;
    m_message_gateway = message_gateway;
    m_observer = observer;
    m_session_manager = session_manager;
    session_manager->register_session(this);
    observer->register_owner(observer_owner(), this);
    m_session_class = _network_session_class_none;
    m_session_membership.set_session(this);
    m_session_membership.reset_membership();
    (*(c_network_session_parameters *)m_session_parameters).initialize(this, m_observer);
    m_local_state = _network_session_state_none;
    m_connection_identifier = 0;
    m_time_synchronization_end_time = 0xffffffff;
    m_time_synchronization_start_time = 0xffffffff;
    m_time_exists = false;
    m_managed_session_index = -1;
    m_close_session = false;
    m_disconnect_reestablish_state = 0;
    return success;
}
bool c_network_session::create_host_session(c_flags<e_session_creation_flags, unsigned char, 4> creation_flags, e_network_session_class
    session_class, e_transport_platform transport_platform,
    const c_network_session_membership *membership, const s_online_context *online_context, long managed_session_flags)
{
    bool success = true;
    bool group_session = m_session_type == _network_session_type_group;
    if (network_session_get_alpha_is_locked_down() || !get_alpha_is_internal())
        success = false;
    m_session_membership.reset_membership();
    m_join_refuse_reason = _network_join_refuse_reason_none;
    m_close_session_for_matchmaking = false;
    m_last_host_migration_time = 0;
    m_host_migration_deferred_time = 0;
    m_host_migration_attempted_peer_mask = 0;
    m_disconnect_reestablish_state = 0;
    memset(&m_player_we_are_adding, 0, sizeof(m_player_we_are_adding));
    memset(m_local_user_player_add, 0, sizeof(m_local_user_player_add));
    m_session_class = session_class;
    s_transport_secure_address secure_address;
    if (!transport_secure_address_get(&secure_address))
        memset(&secure_address, 0, sizeof(secure_address));
    unsigned __int64 party_nonce = transport_secure_nonce_generate();
    m_host_join_nonce = transport_secure_nonce_generate();
    if (success)
    {
        // match: 0x3cca70: platform value range keeps full-image prologue scheduling.
        __assume(transport_platform == _transport_platform_xenon);
        // match: 0x3cca70: flags value range folds the managed-session call setup.
        __assume(managed_session_flags == 2);
        c_flags<e_managed_session_creation_flags, unsigned char, 4> managed_flags;
        managed_flags.set(_managed_session_creation_master, creation_flags.test(_session_creation_flag_local));
        managed_flags.set((e_managed_session_creation_flags)_session_creation_flag_squad, creation_flags.test(_session_creation_flag_squad));
        managed_flags.set((e_managed_session_creation_flags)_session_creation_flag_matchmaking, creation_flags.test(_session_creation_flag_matchmaking));
        managed_flags.set((e_managed_session_creation_flags)_session_creation_flag_group, creation_flags.test(_session_creation_flag_group));
        m_managed_session_index = managed_session_create_host(session_class, managed_flags, transport_platform, 0, 0, online_context, managed_session_flags);
        if (m_managed_session_index == -1)
            success = false;
    }
    if (success)
    {
        if (membership)
        {
            success = m_session_membership.clone_for_connect(membership, this);
            for (long peer_index = (&m_session_membership)->get_first_peer(); peer_index != -1;
                peer_index = (&m_session_membership)->get_next_peer(peer_index))
            {
                if (peer_index != m_session_membership.m_current_membership.host_peer_index)
                    m_session_membership.set_peer_connection_state(peer_index, _network_session_peer_state_reserved);
                else
                    m_session_membership.set_peer_connection_state(peer_index, _network_session_peer_state_connected);
                m_session_membership.set_peer_party_nonce(peer_index, party_nonce);
                m_session_membership.set_join_nonce(peer_index, m_host_join_nonce);
            }
            for (long player_index = (&m_session_membership)->get_first_player(); player_index != -1;
                player_index = (&m_session_membership)->get_next_player(player_index))
            {
                s_network_session_player *player = &m_session_membership.m_current_membership.players[player_index];
                player->left_game = group_session;
            }
            m_session_membership.network_session_set_slot_counts(0, 0, false);
        }
        else if (m_session_membership.add_peer(0, _network_session_peer_state_connected, 11855, &secure_address, party_nonce, m_host_join_nonce))
        {
            m_session_membership.set_host_peer_index(0);
            m_session_membership.set_local_peer_index(0);
            m_session_membership.set_leader_peer_index(0);
            for (long user_index = 0; success && user_index < 4; user_index++)
            {
                s_player_identifier player_identifier;
                if (network_session_interface_get_local_user_identifier(user_index, &player_identifier, false))
                {
                    unsigned __int64 player_xuid = network_session_get_local_user_xuid(user_index);
                    bool silver_or_gold_live = g_network_session_interface_globals.users[user_index].configuration.client.online;
                    bool online_enabled = g_network_session_interface_globals.users[user_index].configuration.client.silver;
                    bool guest = g_network_session_interface_globals.users[user_index].configuration.client.guest;
                    bool bot = g_network_session_interface_globals.users[user_index].configuration.client.bot;
                    e_network_join_refuse_reason refuse_reason = can_accept_player_join_request(
                        &player_identifier, 0, user_index, player_xuid, online_enabled, guest, bot, false);
                    if (refuse_reason == _network_join_refuse_reason_none)
                    {
                        if (m_session_membership.find_or_add_player(0, user_index, &player_identifier, player_xuid, silver_or_gold_live,
                                online_enabled, guest, bot, group_session) == -1)
                            success = false;
                    }
                    else
                    {
                        network_session_set_player_failure_reason(user_index, refuse_reason);
                    }
                }
            }
        }
        else
        {
            success = false;
        }
    }
    if (success)
    {
        m_observer->update_owner(observer_owner(), m_managed_session_index);
        time_set(-(long)network_time_get());
        change_local_state_host_established();
        set_default_session_parameters();
    }
    if (!success)
    {
        if (m_managed_session_index != -1)
        {
            managed_session_delete(m_managed_session_index);
            m_managed_session_index = -1;
        }
        m_session_membership.reset_membership();
    }
    return success;
}
void c_network_session_membership::set_peer_properties(long peer_index, const s_network_session_peer_properties *peer_properties) {
    s_network_session_peer *peer = get_peer(peer_index);
    const c_network_session *session = get_session();
    if (memcmp(&peer->properties, peer_properties, sizeof(s_network_session_peer_properties)) != 0) {
        memcpy(&peer->properties, peer_properties, sizeof(s_network_session_peer_properties));
        long observer_channel_index = get_observer_channel_index(peer_index);
        if (observer_channel_index != -1) {
            if (!g_network_configuration.observer.disable_bad_client_anticheating)
                session->m_observer->observer_channel_set_quality_statistics_ignored(observer_channel_index, network_owner_entry_is_pending(this, peer_index));
            session->m_observer->observer_channel_set_peer_properties(observer_channel_index, peer_properties->upstream_bandwidth,
                peer_properties->downstream_bandwidth);
        }
        increment_update();
    }
}
void c_network_session::set_default_session_parameters(void)
{
    s_network_session_privacy_mode privacy_mode = {};
    if (m_session_type == _network_session_type_squad && m_session_class == _network_session_class_xbox_live)
        privacy_mode.privacy_mode = _network_game_privacy_friends_only;
    (*(c_network_session_parameters *)m_session_parameters).clear();
    (*(c_network_session_parameters *)m_session_parameters).set_initial_update_received();
    (*(c_network_session_parameters *)m_session_parameters).m_parameters.session_mode.set(_network_session_mode_idle);
    (*(c_network_session_parameters *)m_session_parameters).m_parameters.session_size.set_max_peer_count(16);
    (*(c_network_session_parameters *)m_session_parameters).m_parameters.session_size.set_max_player_count(16);
    (*(c_network_session_parameters *)m_session_parameters).m_parameters.privacy_mode.set(&privacy_mode);
    (*(c_network_session_parameters *)m_session_parameters).m_parameters.ui_game_mode.set(_gui_game_mode_none);
}
// @address 0x45a5c k_network_peer_leave_quality_reason
// @rdata 0x82045a5c..0x82045a74
const char k_network_peer_leave_quality_reason[24] = "peer left gracefully";
// @rdata-end
void c_network_session::check_to_send_peer_join_abort(void)
{
    if (network_time_since_inline(m_local_state_data.peer_join_abort.join_abort_request_timestamp) > g_network_configuration.peer_join_abort_check_interval) {
        s_network_message_join_abort message;
        memset(&message, 0, sizeof(message));
        managed_session_get_id(m_managed_session_index, &message.session_id);
        message.join_nonce = m_local_state_data.peer_join_abort.join_nonce;
        m_message_gateway->send_message_directed(
            &m_local_state_data.peer_join_abort.join_usable_address, _network_message_type_join_abort, sizeof(message), &message);
        m_local_state_data.peer_join_abort.join_abort_request_timestamp = network_time_get();
    }
}
void c_network_session::check_to_send_peer_leave_request(void)
{
    long last_leave_request_time = m_local_state_data.peer_leaving.leave_request_timestamp;
    if (last_leave_request_time == -1 ||
        network_time_since_inline(last_leave_request_time) > g_network_configuration.peer_leave_request_check_interval)
    {
        s_transport_secure_identifier message;
        memset(&message, 0, sizeof(message));
        managed_session_get_id(m_managed_session_index, &message);
        send_message_to_peer(_session_message_transmission_type_all, m_session_membership.m_current_membership.host_peer_index,
            _network_message_type_leave_session,
            sizeof(message), &message);
        m_local_state_data.peer_leaving.leave_request_timestamp = network_time_get();
    }
}
// @address 0x3d1278 ?check_to_send_time_synchronization@c_network_session@@AAAXXZ
void c_network_session::check_to_send_time_synchronization(void)
{
    c_network_session_membership *membership = &m_session_membership;
    long host_peer_index = membership->m_current_membership.host_peer_index;
    long observer_channel_index = membership->m_peer_channels[host_peer_index].observer_channel_index;
    if (m_observer->observer_channel_connected(observer_owner(), observer_channel_index)) {
        bool send = true;
        if (leaving_session())
            send = false;
        else if (m_time_synchronization_end_time != -1 && network_time_since_inline(m_time_synchronization_end_time) <=
            g_network_configuration.time_synchronization_interval_a)
            send = false;
        else if (m_time_synchronization_start_time != -1 && network_time_since_inline(m_time_synchronization_start_time) <=
            g_network_configuration.time_synchronization_interval_b)
            send = false;
        if (send) {
            s_network_message_time_synchronize message;
            memset(&message, 0, sizeof(message));
            managed_session_get_id(m_managed_session_index, &message.session_id);
            message.synchronization_stage = 0;
            memset(message.client_timestamp, -1, sizeof(message.client_timestamp));
            memset(message.authority_timestamp, -1, sizeof(message.authority_timestamp));
            m_observer->observer_channel_send_message(
                observer_owner(), observer_channel_index, true, _network_message_type_time_synchronize, sizeof(message), &message);
            m_time_synchronization_start_time = network_time_get();
        }
    }
}
bool c_network_session::get_secure_key(s_transport_secure_identifier *transport_secure_identifier,
    s_transport_secure_key *transport_secure_key, e_transport_platform *transport_platform) const
{
    bool result = false;
    if (m_managed_session_index != -1) {
        s_transport_session_description description;
        e_transport_platform platform;
        if (::managed_session_get_security_information(
                m_managed_session_index, false, &description, &platform))
        {
            if (transport_secure_identifier != 0)
                *transport_secure_identifier = description.secure_identifier;
            if (transport_secure_key != 0)
                *transport_secure_key = description.secure_key;
        }
        result = true;
    }
    return result;
}
static __forceinline bool network_session5_guide_opened()
{
    return g_network_session_interface_globals.peer.disconnected != 0;
}
extern "C" void network_session_refuse_pending_player(c_network_session *session, long reason)
{
    if (reason != 0)
        session->m_session_membership.remove_player_from_player_add_queue(&session->m_player_we_are_adding.player_identifier);
    long peer_index = session->m_player_we_are_adding.peer_index;
    if (peer_index == session->m_session_membership.m_current_membership.host_peer_index) {
        session->m_local_user_player_add[session->m_player_we_are_adding.user_index].refuse_reason = reason;
    } else {
        s_network_message_player_refuse message;
        memset(&message, 0, sizeof(message));
        managed_session_get_id(session->m_managed_session_index, &message.session_id);
        message.refuse_reason = reason;
        message.player_identifier = session->m_player_we_are_adding.player_identifier;
        message.user_index = session->m_player_we_are_adding.user_index;
        session->send_message_to_peer(c_network_session::_session_message_transmission_type_direct, peer_index,
            _network_message_type_player_refuse, sizeof(message), &message);
    }
    memset(&session->m_player_we_are_adding, 0, sizeof(session->m_player_we_are_adding));
}
bool c_network_session::peer_request_properties_update(
    const s_transport_secure_address *transport_secure_address, const s_network_session_peer_properties *network_session_peer_properties)
{
    bool result = false;
    if (established()) {
        if (is_host()) {
            m_session_membership.set_peer_address(m_session_membership.m_local_peer_index, transport_secure_address);
            m_session_membership.set_peer_properties(m_session_membership.m_local_peer_index, network_session_peer_properties);
            return true;
        }
        ns5_peer_properties_message message;
        memset(&message, 0, sizeof(message));
        managed_session_get_id(m_managed_session_index, &message.identifier);
        message.address = *transport_secure_address;
        message.properties = *network_session_peer_properties;
        send_message_to_peer(_session_message_transmission_type_direct, m_session_membership.m_current_membership.host_peer_index,
            _network_message_type_peer_properties, sizeof(message), &message);
        result = true;
    }
    return result;
}
bool c_network_session::peer_request_player_remove(long user_index)
{
    bool result = false;
    if (established()) {
        c_network_session_membership *membership = &m_session_membership;
        long player_index = membership->m_current_membership.peers[membership->m_local_peer_index].user_player_indices[user_index];
        if (m_session_class == _network_session_class_xbox_live) {
            if (network_session5_guide_opened() && m_session_membership.m_current_membership.peer_count > 1 &&
                m_session_membership.m_current_membership.player_count > 0)
                g_gui_error_manager.post_error_with_custom_message(0x20066, 0, 0, k_any_controller, false, true);
            disband_and_reestablish_as_host(false);
        }
        if (is_host()) {
            if (player_index != -1) {
                m_session_membership.remove_player(player_index);
                result = true;
            }
        } else {
            s_network_message_player_remove message;
            memset(&message, 0, sizeof(message));
            managed_session_get_id(m_managed_session_index, &message.session_id);
            message.user_index = user_index;
            send_message_to_peer(_session_message_transmission_type_direct, m_session_membership.m_current_membership.host_peer_index,
                _network_message_type_player_remove, sizeof(message), &message);
            result = true;
        }
    }
    return result;
}
bool c_network_session::leader_request_boot_machine(const s_transport_secure_address *address, e_network_session_boot_reason boot_reason)
{
    bool result = false;
    if (established()) {
        c_network_session_membership &membership = m_session_membership;
        bool leader = false;
        if (membership.has_membership())
            leader = membership.m_local_peer_index == membership.m_current_membership.leader_peer_index;
        if (leader) {
        long peer_index = membership.get_peer_from_secure_address(address);
        if (peer_index != -1 && peer_index != m_session_membership.m_local_peer_index) {
            if ((this)->is_host()) {
                boot_peer(peer_index, _network_session_boot_reason_leader_booted);
                result = true;
            }
            else {
                s_network_message_boot_machine message;
                memset(&message, 0, sizeof(message));
                managed_session_get_id(m_managed_session_index, &message.session_id);
                message.boot_peer_address = *address;
                message.reason = _network_session_boot_reason_leader_booted;
                send_message_to_peer(_session_message_transmission_type_direct, m_session_membership.m_current_membership.host_peer_index,
                    _network_message_type_boot_machine, sizeof(message), &message);
                result = true;
            }
        }
        }
    }
    return result;
}
bool c_network_session::host_boot_machine(long peer_index, e_network_session_boot_reason boot_reason)
{
    bool result = false;
    if ((this)->is_host()) {
        if (m_session_membership.m_local_peer_index == peer_index) {
            user_interface_networking_notify_booted_from_session(m_session_type, boot_reason);
            initiate_leave_protocol(false);
            result = true;
        }
        else {
            boot_peer(peer_index, boot_reason);
            result = true;
        }
    }
    return result;
}
bool c_network_session::host_delegate_host(long peer_index, bool option1, bool option2)
{
    bool result = false;
    if (host_established()) {
        change_local_state_host_handoff(peer_index, false, option1, option2);
        result = true;
    }
    return result;
}
bool c_network_session::is_peer_joining_this_session() const {
    bool joining = false;
    if (established()) {
        const c_network_session_membership *membership = get_session_membership();
        for (long peer_index = (membership)->get_first_peer(); peer_index != -1;
            peer_index = (membership)->get_next_peer(peer_index))
        {
            if (peer_index != membership->m_local_peer_index && membership->m_current_membership.peers[peer_index].connection_state != 8) {
                joining = true;
                break;
            }
        }
    }
    return joining;
}
void c_network_session::notify_channel_connection(long observer_channel_index, unsigned long connection_identifier, bool connected)
{
    long peer_index = m_session_membership.get_peer_from_observer_channel(observer_channel_index);
    if (peer_index == -1)
        return;
    if (connected) {
        if (established() && peer_index == m_session_membership.m_current_membership.host_peer_index) {
            s_transport_secure_identifier message;
            memset(&message, 0, sizeof(message));
            managed_session_get_id(m_managed_session_index, &message);
            send_message_to_peer(_session_message_transmission_type_direct, peer_index, _network_message_type_peer_establish, sizeof(message), &message);
        }
        return;
    }
    if (is_host()) {
        m_session_membership.set_membership_update_number(peer_index, -1);
        m_session_membership.set_peer_needs_reestablishment(peer_index, true);
        ((c_network_session_parameters *)m_session_parameters)->reset_peer_state(peer_index);
    } else if (peer_index == m_session_membership.m_current_membership.host_peer_index) {
        reset_connection_state();
    }
    if (m_local_state == _network_session_state_host_reestablish) {
        unsigned long mask = ~(1 << peer_index);
        m_local_state_data.host_reestablish.requested_peer_mask &= mask;
        m_local_state_data.host_reestablish.acknowledged_peer_mask &= mask;
    }
}

// @address 0x5bec98 ?notify_channel_died@c_network_session@@UAAXJ@Z
void c_network_session::notify_channel_died(long observer_channel_index)
{
}
void c_network_session::boot_peer(long peer_index, e_network_session_boot_reason boot_reason)
{
    bool membership_locked = m_local_state == _network_session_state_host_handoff || m_local_state == _network_session_state_host_disband ||
        m_local_state == _network_session_state_host_reestablish;
    if (membership_locked)
        return;
    s_network_message_session_boot message;
    memset(&message, 0, sizeof(message));
    managed_session_get_id(m_managed_session_index, &message.session_id);
    message.reason = boot_reason;
    send_message_to_peer(_session_message_transmission_type_all, peer_index, _network_message_type_session_boot, sizeof(message), &message);
    m_session_membership.remove_peer(peer_index);
}
void c_network_session::disband_peer(long peer_index)
{
    long connection_state = m_session_membership.m_current_membership.peers[peer_index].connection_state;
    bool peer_disconnected = connection_state == 8 || connection_state == 1;
    if (peer_disconnected) {
        s_transport_secure_identifier message;
        memset(&message, 0, sizeof(message));
        managed_session_get_id(m_managed_session_index, &message);
        send_message_to_peer(_session_message_transmission_type_all, peer_index, _network_message_type_session_disband, sizeof(message), &message);
        m_session_membership.remove_peer(peer_index);
        return;
    }
    c_network_session_membership &membership = m_session_membership;
    membership.m_current_membership.peers[peer_index].connection_state = _network_session_peer_state_disconnected;
    membership.increment_update();
}

void c_network_session::initiate_leave_protocol(bool force)
{
    e_network_session_state state = m_local_state;
    if (state == _network_session_state_none)
        return;
    if ((this)->leaving_session())
        return;
    switch (state) {
    case _network_session_state_peer_established:
        change_local_state_peer_leaving();
        break;
    case _network_session_state_peer_creating:
        break;
    case _network_session_state_peer_joining:
        change_local_state_peer_join_abort();
        return;
    case _network_session_state_host_established:
    case _network_session_state_host_handoff:
        if (force) {
            s_transport_secure_identifier key;
            memset(&key, 0, sizeof(key));
            managed_session_get_id(m_managed_session_index, &key);
            send_message_to_all_peers(_session_message_transmission_type_all, _network_message_type_session_disband, sizeof(key), &key);
            memset(&m_local_state_data, 0, sizeof(m_local_state_data));
            m_local_state = _network_session_state_host_disband;
            return;
        }
        if (state == _network_session_state_host_handoff) {
            m_local_state_data.host_handoff.host_leaving = true;
            m_local_state_data.host_handoff.peer_consideration_mask &= ~(1 << m_session_membership.m_local_peer_index);
            return;
        }
        change_local_state_host_handoff(-1, true, false, false);
        return;
    case _network_session_state_host_reestablish:
        m_local_state_data.host_reestablish.reestablish_host_leaving = true;
        return;
    case _network_session_state_election:
        if (force) {
            s_network_message_election_refuse message;
            memset(&message, 0, sizeof(message));
            managed_session_get_id(m_managed_session_index, &message.session_id);
            message.refusal_reason = (long)_network_session_state_election;
            send_message_to_all_peers(_session_message_transmission_type_all, _network_message_type_election_refuse, sizeof(message), &message);
            return;
        }
        (this)->disconnect();
        return;
    default:
        // match: 0x3ce618: default is unreachable; omits the state range check.
        __assume(0);
    }
}
void c_network_session::reset_local_host_state(bool becoming_host) {
    m_session_membership.reset_transmitted_membership_state();
    if (becoming_host)
        (*(c_network_session_parameters *)m_session_parameters).transition_state(becoming_host);
}
void c_network_session::handoff_remove_peer_from_consideration(long peer_index) {
    s_network_session_state_host_handoff *state_data = &m_local_state_data.host_handoff;
    state_data->peer_consideration_mask &= ~(1 << peer_index);
    if (state_data->handoff_peer_index == peer_index) {
        state_data->handoff_transition_sent = false;
        state_data->handoff_initiated = false;
        state_data->handoff_peer_index = -1;
        state_data->start_time = network_time_get();
    }
}

long c_network_session::channel_is_load_bearing(long observer_channel_index) const
{
    return desire_channel_heartbeat(observer_channel_index) != false;
}


s_network_message_membership_update::s_network_message_membership_update()
{
}

void c_network_session::send_message_to_peer(e_session_message_transmission_type transmission_type, long peer_index,
    e_network_message_type message_type, long message_size, const void *message) const
{
    long observer_channel = m_session_membership.get_observer_channel_index(peer_index);
    if (observer_channel == -1)
        return;
    if (transmission_type == _session_message_transmission_type_direct ||
        (transmission_type == _session_message_transmission_type_all &&
         m_observer->observer_channel_connected(observer_owner(), observer_channel)))
        m_observer->observer_channel_send_message(observer_owner(), observer_channel, false, message_type, message_size, message);
    if (transmission_type == _session_message_transmission_type_direct_only || transmission_type == _session_message_transmission_type_all)
        m_observer->observer_channel_send_message(observer_owner(), observer_channel, true, message_type, message_size, message);
}

void c_network_session::send_message_to_all_peers(e_session_message_transmission_type transmission_type,
    e_network_message_type message_type, long message_size, const void *message) const
{
    for (long peer_index = (&m_session_membership)->get_first_peer(); peer_index != -1;
        peer_index = (&m_session_membership)->get_next_peer(peer_index))
        send_message_to_peer(transmission_type, peer_index, message_type, message_size, message);
}

// 0x3d1660: the session's player-count limit, 16 unless the settings override it
static __forceinline const s_network_session_privacy_mode *network_session_limit_block_get_limits(
    const c_generic_network_session_parameter<s_network_session_privacy_mode> *block)
{
    const s_network_session_privacy_mode *limits = 0;
    if (block->m_flags & 1)
        limits = &block->m_data;
    return limits;
}

extern "C" long network_session_get_maximum_player_count(const c_network_session *session)
{
    const c_network_session_parameters *settings = session->get_session_parameters();
    long maximum_player_count = 16;
    if (settings->m_parameters.session_size.get_allowed())
        maximum_player_count = settings->m_parameters.session_size.m_data.maximum_player_count;
    if (settings->m_parameters.privacy_mode.m_flags & 1)
    {
        long limit = network_session_limit_block_get_limits(&settings->m_parameters.privacy_mode)->maximum_player_count;
        if (limit != 0 && maximum_player_count >= limit)
            maximum_player_count = limit;
    }
    return maximum_player_count;
}

// 0x3d16c8: the join refuse reason from the session privacy mode.
static __forceinline const s_network_session_privacy_mode *network_session_host_peer_get_status(
    const c_generic_network_session_parameter<s_network_session_privacy_mode> *peer)
{
    const s_network_session_privacy_mode *status = 0;
    if (peer->m_flags & 1)
        status = &peer->m_data;
    return status;
}

extern "C" long network_session_get_join_refuse_reason(const c_network_session *session)
{
    long refuse_reason = 0;
    const c_network_session_parameters *membership = session->get_session_parameters();
    if (membership->m_parameters.privacy_mode.m_flags & 1)
    {
        const s_network_session_privacy_mode *status = network_session_host_peer_get_status(&membership->m_parameters.privacy_mode);
        if (status->closed_mode == 1)
            refuse_reason = 29;
        else if (status->closed_mode == 2)
            refuse_reason = 28;
        else if (status->closed_mode == 3)
            refuse_reason = _network_join_refuse_reason_in_matchmaking;
        else if (status->is_closed_by_user)
            refuse_reason = _network_join_refuse_reason_squad_full;
    }
    return refuse_reason;
}

void c_network_session::acknowledge_join_request(const transport_address *entry, e_network_join_refuse_reason refuse_reason)
{
    s_network_message_join_refuse refusal;
    memset(&refusal, 0, sizeof(refusal));
    managed_session_get_id(m_managed_session_index, &refusal.session_id);
    refusal.reason = refuse_reason;
    m_message_gateway->send_message_directed(entry, _network_message_type_join_refuse, sizeof(refusal), &refusal);
}
bool c_network_session::attempt_channel_reconnection(long observer_channel_index, bool far_side_requested) const {
    long peer_index = m_session_membership.get_peer_from_observer_channel(observer_channel_index);
    bool attempt_reconnection = false;
    if (peer_index != -1) {
        const s_network_session_peer *peer = 0;
        if (established())
            peer = m_session_membership.get_peer(peer_index);
        if ((this)->is_host() && !far_side_requested && (peer == 0 || peer->player_count == 0))
            attempt_reconnection = false;
        else if ((this)->leaving_session() && !(this)->is_host())
            attempt_reconnection = false;
        else
            attempt_reconnection = true;
    }
    return attempt_reconnection;
}
bool c_network_session::channel_is_authoritative(c_network_channel *channel) {
    bool authoritative = false;
    if ((established() || peer_joining()) && !(this)->is_host()) {
        long observer_channel = (m_observer)
                                    ->observer_channel_find_by_network_channel(
                                        observer_owner(), channel);
        long peer_index = m_session_membership.get_peer_from_observer_channel(observer_channel);
        if (peer_index != -1 && peer_index == m_session_membership.m_current_membership.host_peer_index)
            authoritative = true;
    }
    return authoritative;
}

static __forceinline bool network_session3_status_test(const unsigned short *flags, long bit)
{
    bool result = (*flags >> bit) & 1;
    return result;
}
static __forceinline const s_network_session_peer *network_session3_get_local_peer(const c_network_session_membership *membership)
{
    return &membership->m_current_membership.peers[membership->m_local_peer_index];
}
bool c_network_session::ready_for_remote_peers_to_join() const {
    bool result = false;
    unsigned short status;
    bool is_established;
    if (network_session3_status_test(managed_session_get_status(&status, m_managed_session_index), 1)) {
        is_established = ((unsigned long)m_local_state >= 4 && (unsigned long)m_local_state < 10);
        if (is_established)
            result = true;
        else if (m_local_state == _network_session_state_peer_joining && m_session_membership.has_membership()
            && (*(c_network_session_parameters *)m_session_parameters).initial_update_received()
            && network_session3_get_local_peer(&m_session_membership)->connection_state >= 4)
            result = true;
    }
    return result;
}

void c_network_session::add_pending_join_to_session(unsigned __int64 join_nonce)
{
    c_network_session_membership *membership = &m_session_membership;
    long player_count = 0;
    long peer_index;
    bool player_flags_a[16];
    bool player_flags_b[16];
    unsigned __int64 player_xuids[16];
    for (peer_index = (membership)->get_first_peer(); peer_index != -1;
        peer_index = (membership)->get_next_peer(peer_index))
    {
        s_network_session_peer *peer = &membership->m_current_membership.peers[peer_index];
        if (peer->join_nonce == join_nonce) {
            unsigned long user_index;
            for (user_index = 0; user_index < 4; user_index++) {
                long player_index = peer->user_player_indices[user_index];
                if (player_index != -1) {
                    // match: 0x3ce2b0: restrict player pointer keeps register coloring.
                    s_network_session_player *__restrict player = &membership->m_current_membership.players[player_index];
                    player_xuids[player_count] = player->configuration.client.xuid;
                    player_flags_a[player_count] = player->configuration.client.silver;
                    player_flags_b[player_count] = player->left_game == 0;
                    player_count++;
                }
            }
            peer->connection_state = _network_session_peer_state_joining;
            membership->increment_update();
        }
    }
    managed_session_add_players(m_managed_session_index, player_xuids, player_flags_a, player_flags_b, player_count);
}

void c_network_session::disconnect()
{
    e_network_session_state *state = &m_local_state;
    if (*state != _network_session_state_none) {
        if (m_disconnect_reestablish_state == 2) {
            disband_and_reestablish_as_host(m_session_type == _network_session_type_group);
            return;
        }
        initiate_leave_protocol(true);
        c_network_session_membership *membership = &m_session_membership;
        membership->reset_membership();
        (*(c_network_session_parameters *)m_session_parameters).clear();
        long *connection_number = &m_connection_identifier;
        long *managed_session = &m_managed_session_index;
        *connection_number = *connection_number + 1;
        membership->m_player_add_queue_first = 0;
        membership->m_player_add_queue_count = 0;
        long index = *managed_session;
        m_time_synchronization_end_time = 0xffffffff;
        m_time_synchronization_start_time = 0xffffffff;
        m_time_exists = false;
        m_local_state = _network_session_state_none;
        if (index != -1) {
            managed_session_delete(index);
            m_managed_session_index = -1;
            m_observer->update_owner(observer_owner(), -1);
        }
    }
}

void c_network_session::disband_and_reestablish_as_host(bool reset_time)
{
    bool initialize_parameters = false;
    network_join_flush_join_queue();
    if (!m_session_membership.has_membership())
        m_session_membership.m_current_membership.update_number = 1;
    if (!(*(c_network_session_parameters *)m_session_parameters).initial_update_received()) {
        initialize_parameters = true;
        (*(c_network_session_parameters *)m_session_parameters).set_initial_update_received();
    }
    if (reset_time)
        change_local_state_host_reestablish(false, 0);
    else
        change_local_state_host_established();
    if (initialize_parameters)
        set_default_session_parameters();
    c_network_session_membership *membership = &m_session_membership;
    long local_peer_index = m_session_membership.m_local_peer_index;
    for (long peer_index = (membership)->get_first_peer(); peer_index != -1;
        peer_index = (membership)->get_next_peer(peer_index))
    {
        if (peer_index != local_peer_index)
            m_session_membership.remove_peer(peer_index);
        else {
            m_session_membership.set_peer_connection_state(local_peer_index, _network_session_peer_state_established);
        }
    }
    if (!reset_time)
        managed_session_reset_session(m_managed_session_index, false);
    if (this == g_network_life_cycle_globals.manager.m_group_session)
        g_network_life_cycle_globals.manager.m_active_squad_session->initiate_leave_protocol(false);
}

// @address 0x3ce568 ?abort_pending_join@c_network_session@@AAAX_K@Z
void c_network_session::abort_pending_join(unsigned __int64 join_nonce)
{
    c_network_session_membership *membership = &m_session_membership;
    if (membership->m_current_membership.peers[membership->m_current_membership.host_peer_index].join_nonce != join_nonce && m_host_join_nonce != join_nonce) {
        for (long peer_index = (membership)->get_first_peer(); peer_index != -1;
            peer_index = (membership)->get_next_peer(peer_index))
        {
            if (membership->m_current_membership.peers[peer_index].join_nonce == join_nonce)
                boot_peer(peer_index, _network_session_boot_reason_invalid_join_nonce);
        }
    } else {
        disconnect();
    }
}

typedef char network_session_previous_id_offset[(sizeof(void *) != 4 || ((offsetof(s_online_managed_session, previous_description) == 0x5d6))) ? 1 : -1];
static __forceinline bool network_session5_managed_compare_id(long index, const s_transport_secure_identifier *id)
{
    bool result = false;
    if (index != -1) {
        const s_online_managed_session *session = &online_session_manager_globals.sessions[index];
        if ((bool)(_rotl(session->flags, 28) & 1))
            result = memcmp(id, &session->actual.description.secure_identifier, sizeof(*id)) == 0;
        if (!result && memcmp(id, &session->previous_description.secure_identifier, sizeof(*id)) == 0)
            result = true;
    }
    return result;
}
// @address 0x3ce908 ?compare_session_id@c_network_session@@QBA_NPBUs_transport_secure_identifier@@@Z
bool c_network_session::compare_session_id(const s_transport_secure_identifier *session_id) const
{
    return network_session5_managed_compare_id(m_managed_session_index, session_id);
}

// @address 0x3cf648 ?session_is_full@c_network_session@@QBA_NJJ@Z
bool c_network_session::session_is_full(long peers, long players) const
{
    bool result = false;
    if (established()) {
        const c_network_session_parameters *settings = get_session_parameters();
        long max_peer_count = 16;
        if (settings->m_parameters.session_size.get_allowed())
            max_peer_count = settings->m_parameters.session_size.m_data.maximum_peer_count;
        long max_player_count = network_session_get_maximum_player_count(this);
        result = result || m_session_membership.m_current_membership.peer_count + peers > max_peer_count;
        result = result || m_session_membership.m_current_membership.player_count + players > max_player_count;
    }
    return result;
}

// @address 0x3ceb90 ?peer_request_player_add@c_network_session@@QAAXJPBUs_player_identifier@@JW4e_controller_index@@PBUs_player_configuration_from_client@@K@Z
void c_network_session::peer_request_player_add(long user_index, const s_player_identifier *identifier, long player_sequence_number,
    e_controller_index controller_index, const s_player_configuration_from_client *configuration, unsigned long voice_settings)
{
    if (memcmp(&m_local_user_player_add[user_index].player_identifier, identifier, sizeof(*identifier)) != 0 ||
        m_local_user_player_add[user_index].refuse_reason == _network_join_refuse_reason_player_add_pending) {
        if (memcmp(&m_local_user_player_add[user_index].player_identifier, identifier, sizeof(*identifier)) != 0) {
            m_local_user_player_add[user_index].player_identifier = *identifier;
            m_local_user_player_add[user_index].refuse_reason = _network_join_refuse_reason_player_add_pending;
            m_local_user_player_add[user_index].time = 0;
        }
        if (established() &&
            network_time_since_inline(m_local_user_player_add[user_index].time) >=
                g_network_configuration.player_add_interval)
        {
            m_local_user_player_add[user_index].time = network_time_get();
            if (m_session_membership.add_player_to_player_add_queue(identifier, m_session_membership.m_local_peer_index, user_index, player_sequence_number,
                    controller_index, configuration, voice_settings))
            {
                if ((this)->is_host()) {
                    long local_peer_index = m_session_membership.m_local_peer_index;
                    e_network_join_refuse_reason refuse_reason = can_accept_player_join_request(identifier, local_peer_index, user_index,
                        configuration->xuid, configuration->silver, configuration->guest, configuration->bot, true);
                    if (refuse_reason != 0) {
                        m_local_user_player_add[user_index].refuse_reason = refuse_reason;
                        m_session_membership.remove_player_from_player_add_queue(identifier);
                    }
                } else if (!(this)->is_host()) {
                    s_network_message_player_add message;
                    memset(&message, 0, sizeof(message));
                    managed_session_get_id(m_managed_session_index, &message.session_id);
                    message.user_index = user_index;
                    message.player_identifier = *identifier;
                    message.player_update_number = player_sequence_number;
                    message.controller_index = controller_index;
                    message.configuration = *configuration;
                    message.voice = voice_settings;
                    long channel = m_session_membership.get_observer_channel_index(m_session_membership.m_current_membership.host_peer_index);
                    if (channel != -1)
                        m_observer->observer_channel_send_message(observer_owner(), channel, false, _network_message_type_player_add,
                            sizeof(message), &message);
                }
            }
        }
    }
}

static __forceinline bool network_session5_peer_valid(const c_network_session *session, long peer_index)
{
    return (session->m_session_membership.m_current_membership.valid_peer_mask.m_storage[peer_index >> 5] & (1 << (peer_index & 31))) != 0;
}

static __forceinline const s_player_add_queue_entry *network_session5_queue_entry(const c_network_session_membership *membership, long index)
{
    // match: 0x3d1748: reload m_player_add_queue_count on every queue probe (removal: -24B, 190w diff).
    const volatile long *queue_count = &membership->m_player_add_queue_count;
    long last = *queue_count;
    // match: 0x3d1748: reload m_player_add_queue_first on every queue probe (removal: -24B, 190w diff).
    const volatile long *queue_first = &membership->m_player_add_queue_first;
    long first = *queue_first;
    const s_player_add_queue_entry *entry = 0;
    if (first < last && first <= index && index < last)
        entry = &membership->m_player_add_queue[index];
    else if (first > last && (first <= index || index < last))
        entry = &membership->m_player_add_queue[index];
    return entry;
}
// @address 0x3d1748 ?can_accept_player_join_request@c_network_session@@QBA?AW4e_network_join_refuse_reason@@PBUs_player_identifier@@JJ_K_N222@Z
e_network_join_refuse_reason c_network_session::can_accept_player_join_request(const s_player_identifier *identifier, long peer_index,
    long user_index, unsigned __int64 xuid, bool silver, bool guest, bool bot, bool remote) const
{
    long player_count = 0;
    long result = network_session_get_join_refuse_reason(this);
    if (!result) {
        const c_network_session_membership *membership = &m_session_membership;
        long player_index = membership->get_player_from_identifier(identifier);
        unsigned long peer_flags;
        if (peer_index != -1)
            peer_flags = m_session_membership.m_current_membership.valid_peer_mask.m_storage[peer_index >> 5];
        if (peer_index != -1 && (peer_flags & (1 << (peer_index & 31)))) {
            const s_network_session_peer *peer = &m_session_membership.m_current_membership.peers[peer_index];
            player_count += peer->player_count;
            if (membership->find_player_in_player_add_queue(identifier) == -1 && peer->user_player_indices[user_index] == -1)
                player_count++;
            {
                const s_player_add_queue_entry *entry = network_session5_queue_entry(membership, 0);
                if (entry && entry->peer_index == peer_index && peer->user_player_indices[entry->peer_user_index] == -1)
                    player_count++;
            }
            {
                const s_player_add_queue_entry *entry = network_session5_queue_entry(membership, 1);
                if (entry && entry->peer_index == peer_index && peer->user_player_indices[entry->peer_user_index] == -1)
                    player_count++;
            }
            {
                const s_player_add_queue_entry *entry = network_session5_queue_entry(membership, 2);
                if (entry && entry->peer_index == peer_index && peer->user_player_indices[entry->peer_user_index] == -1)
                    player_count++;
            }
            {
                const s_player_add_queue_entry *entry = network_session5_queue_entry(membership, 3);
                if (entry && entry->peer_index == peer_index && peer->user_player_indices[entry->peer_user_index] == -1)
                    player_count++;
            }
        }
        if (peer_index != -1 && !(peer_flags & (1 << (peer_index & 31))))
            result = _network_join_refuse_reason_session_booted;
        if (!result) {
        if (m_session_type == 2 && remote)
            result = _network_join_refuse_reason_in_matchmaking;
        else if (player_count > g_network_configuration.maximum_players_per_peer)
            result = 8;
        else if (player_index != -1) {
            if (m_session_membership.m_current_membership.players[player_index].peer_index != peer_index)
                result = _network_join_refuse_reason_player_add_failed;
        }
        else if ((peer_index == -1 ||
                     (network_session5_peer_valid(this, peer_index) &&
                         m_session_membership.m_current_membership.peers[peer_index].user_player_indices[user_index] == -1)) &&
            session_is_full(0, 1))
            result = _network_join_refuse_reason_squad_full;
        }
        if (!result) {
            if (m_session_class == 2 && !silver)
                result = _network_join_refuse_reason_player_not_online_enabled;
            else if (!(simulation_starting_up() && game_globals->options.game_mode == 3) && !guest) {
                char *session = user_interface_game_settings_try_and_get_map_variant();
                if (session && !((const c_map_variant *)session)->m_built_in)
                    result = 30;
            }
        }
    }
    return (e_network_join_refuse_reason)result;
}

// @address 0x3cee70 ?peer_request_player_add_status@c_network_session@@QAA?AW4e_network_join_refuse_reason@@JPBUs_player_identifier@@@Z
e_network_join_refuse_reason c_network_session::peer_request_player_add_status(long user_index, const s_player_identifier *identifier)
{
    long result;
    if (memcmp(identifier, &m_local_user_player_add[user_index].player_identifier, sizeof(s_player_identifier)) != 0) {
        result = _network_join_refuse_reason_player_add_pending;
    } else if (m_local_user_player_add[user_index].refuse_reason == 0) {
        long player_index = m_session_membership.get_player_from_identifier(identifier);
        result = _network_join_refuse_reason_player_add_pending;
        if (player_index != -1) {
            const s_network_session_player *player = &m_session_membership.m_current_membership.players[player_index];
            if (player->peer_index == m_session_membership.m_local_peer_index && player->peer_user_index == user_index)
                result = 0;
        }
    } else {
        result = m_local_user_player_add[user_index].refuse_reason;
    }
    return (e_network_join_refuse_reason)result;
}

static __forceinline bool network_session_get_local_user_configuration(long user_index, s_player_configuration *configuration)
{
    bool result = false;
    if (g_network_session_interface_globals.users[user_index].state == 1)
    {
        memcpy(configuration, &g_network_session_interface_globals.users[user_index].configuration, sizeof(s_player_configuration));
        result = true;
    }
    return result;
}
// @address 0x3ccf80 ?join_remote_session@c_network_session@@QAA_NV?$c_flags@W4e_session_creation_flags@@E$03@@W4e_network_session_class@@W4e_transport_platform@@PBUs_transport_secure_identifier@@PBUs_transport_secure_key@@PBUs_transport_secure_address@@_KPBVc_network_session_membership@@PBUs_network_session_join_request_payload@@PBUs_online_context@@J_N@Z
bool c_network_session::join_remote_session(c_flags<e_session_creation_flags, unsigned char, 4> creation_flags,
    e_network_session_class session_class, e_transport_platform transport_platform, const s_transport_secure_identifier *session_id,
    const s_transport_secure_key *key, const s_transport_secure_address *address, unsigned __int64 join_nonce,
    const c_network_session_membership *membership, const s_network_session_join_request_payload *payload,
    const s_online_context *online_context, long managed_session_flags, bool unknown)
{
    bool success = true;
    if (network_session_get_alpha_is_locked_down() || !get_alpha_is_internal())
        success = false;
    s_network_session_join_request join_request;
    memset(&join_request, 0, sizeof(join_request));
    join_request.join_nonce = join_nonce;
    join_request.join_from_recruiting = unknown;
    if (payload)
        join_request.payload = *payload;
    if (membership)
    {
        long player_mapping[16] = { 0 };
        long player_count = 0;
        join_request.joining_peer_count = membership->m_current_membership.peer_count;
        join_request.join_party_nonce = membership->get_peer(membership->m_current_membership.host_peer_index)->party_nonce;
        long peer_count = 0;
        for (long player_index = (membership)->get_first_player(); player_index != -1;
            player_index = (membership)->get_next_player(player_index))
        {
            const s_player_configuration *player_data = membership->get_player_data(player_index);
            join_request.joining_players[player_count].player_identifier = *membership->get_player_identifier(player_index);
            join_request.joining_players[player_count].player_xuid = player_data->client.xuid;
            join_request.joining_players[player_count].is_silver_or_gold_live = player_data->client.online;
            join_request.joining_players[player_count].is_online_enabled = player_data->client.silver;
            join_request.joining_players[player_count].is_guest = player_data->client.guest;
            join_request.joining_players[player_count].is_bot = player_data->client.bot;
            join_request.joining_players[player_count].global_statistics = player_data->client.global_statistics;
            join_request.joining_players[player_count].hopper_statistics = player_data->client.hopper_statistics;
            player_mapping[player_index] = player_count;
            player_count++;
        }
        join_request.joining_player_count = player_count;
        for (long peer_index = (membership)->get_first_peer(); peer_index != -1;
            peer_index = (membership)->get_next_peer(peer_index))
        {
            const s_network_session_peer *peer = membership->get_peer(peer_index);
            join_request.joining_peers[peer_count].address = peer->secure_address;
            join_request.joining_peers[peer_count].network_version = peer->peer_version;
            for (long user_index = 0; user_index < 4; user_index++)
            {
                long player_index = peer->user_player_indices[user_index];
                if (player_index != -1)
                    join_request.joining_peers[peer_count].player_indices[user_index] = player_mapping[player_index];
                else
                    join_request.joining_peers[peer_count].player_indices[user_index] = -1;
            }
            peer_count++;
        }
    }
    else
    {
        join_request.joining_peer_count = 1;
        join_request.join_party_nonce = transport_secure_nonce_generate();
        join_request.joining_peers[0].network_version = 11855;
        success = transport_secure_address_get(&join_request.joining_peers[0].address);
        if (success)
        {
            long player_count = 0;
            for (long user_index = 0; user_index < 4; user_index++)
            {
                if (network_session_interface_get_local_user_identifier(user_index, &join_request.joining_players[player_count].player_identifier, false))
                {
                    s_player_configuration configuration;
                    network_session_get_local_user_configuration(user_index, &configuration);
                    join_request.joining_players[player_count].player_xuid = configuration.client.xuid;
                    join_request.joining_players[player_count].is_silver_or_gold_live = configuration.client.online;
                    join_request.joining_players[player_count].is_online_enabled = configuration.client.silver;
                    join_request.joining_players[player_count].is_guest = configuration.client.guest;
                    join_request.joining_players[player_count].is_bot = configuration.client.bot;
                    join_request.joining_players[player_count].global_statistics = configuration.client.global_statistics;
                    join_request.joining_players[player_count].hopper_statistics = configuration.client.hopper_statistics;
                    join_request.joining_peers[0].player_indices[user_index] = player_count;
                    player_count++;
                }
                else
                {
                    join_request.joining_peers[0].player_indices[user_index] = -1;
                }
            }
            join_request.joining_player_count = player_count;
        }
    }
    if (success)
        success = create_session_for_remote_join(creation_flags, session_class, transport_platform, session_id, key, address, false,
            join_nonce, &join_request, online_context, 2);
    return success;
}

// @address 0x3cd3e8 ?connect_peer_to_remote_session@c_network_session@@QAA_NV?$c_flags@W4e_session_creation_flags@@E$03@@W4e_network_session_class@@W4e_transport_platform@@PBUs_transport_secure_identifier@@PBUs_transport_secure_key@@PBUs_transport_secure_address@@_KPBUs_online_context@@J@Z
bool c_network_session::connect_peer_to_remote_session(c_flags<e_session_creation_flags, unsigned char, 4> creation_flags,
    e_network_session_class session_class, e_transport_platform transport_platform, const s_transport_secure_identifier *session_id,
    const s_transport_secure_key *key, const s_transport_secure_address *address, unsigned __int64 nonce,
    const s_online_context *online_context, long unknown)
{
    bool success = true;
    if (network_session_get_alpha_is_locked_down() || !get_alpha_is_internal())
        success = false;
    if (success) {
        s_network_session_join_request join_request;
        memset(&join_request, 0, sizeof(join_request));
        success = create_session_for_remote_join(
            creation_flags, session_class, transport_platform, session_id, key, address, true, nonce, &join_request, online_context, 2);
    }
    return success;
}

// @address 0x3cd4f8 ?create_session_for_remote_join@c_network_session@@AAA_NV?$c_flags@W4e_session_creation_flags@@E$03@@W4e_network_session_class@@W4e_transport_platform@@PBUs_transport_secure_identifier@@PBUs_transport_secure_key@@PBUs_transport_secure_address@@_N_KPAUs_network_session_join_request@@PBUs_online_context@@J@Z
bool c_network_session::create_session_for_remote_join(c_flags<e_session_creation_flags, unsigned char, 4> creation_flags,
    e_network_session_class session_class, e_transport_platform transport_platform, const s_transport_secure_identifier *session_id,
    const s_transport_secure_key *key, const s_transport_secure_address *address, bool unknown, unsigned __int64 nonce,
    s_network_session_join_request *join_request, const s_online_context *online_context, long managed_session_flags)
{
    bool success = false;
    if (!m_session_manager->get_session(session_id)) {
        s_transport_secure_address local_address;
        if (transport_secure_address_get(&local_address)) {
            c_flags<e_managed_session_creation_flags, unsigned char, 4> managed_flags;
            managed_flags.set(_managed_session_creation_master, creation_flags.test(_session_creation_flag_local));
            managed_flags.set((e_managed_session_creation_flags)_session_creation_flag_squad, creation_flags.test(_session_creation_flag_squad));
            managed_flags.set((e_managed_session_creation_flags)_session_creation_flag_matchmaking, creation_flags.test(_session_creation_flag_matchmaking));
            managed_flags.set((e_managed_session_creation_flags)_session_creation_flag_group, creation_flags.test(_session_creation_flag_group));
            m_managed_session_index = managed_session_create_client(
                session_class, managed_flags, transport_platform, 0, 16, session_id, address, key, online_context, 2);
            if (m_managed_session_index != -1)
                success = true;
        }
    }
    if (success) {
        m_host_join_nonce = 0;
        m_join_refuse_reason = _network_join_refuse_reason_none;
        m_close_session_for_matchmaking = false;
        m_last_host_migration_time = 0;
        m_host_migration_deferred_time = 0;
        m_host_migration_attempted_peer_mask = 0;
        m_disconnect_reestablish_state = 0;
        memset(&m_player_we_are_adding, 0, sizeof(m_player_we_are_adding));
        memset(m_local_user_player_add, 0, sizeof(m_local_user_player_add));
        m_observer->update_owner(observer_owner(), m_managed_session_index);
        m_session_class = session_class;
        m_session_membership.reset_membership();
        (*(c_network_session_parameters *)m_session_parameters).clear();
        time_set(-(long)network_time_get());
        s_network_session_state_peer_creating state;
        unsigned int state_size = sizeof(state);
        memset(&state, 0, state_size);
        state.transport_platform = (e_transport_platform)transport_platform;
        state.unknown = unknown;
        state.join_nonce = nonce;
        state.join_request = *join_request;
        memset(&m_local_state_data, 0, sizeof(m_local_state_data));
        memcpy(&m_local_state_data, &state, state_size);
        m_local_state = _network_session_state_peer_creating;
    }
    if (!success && m_managed_session_index != -1) {
        managed_session_delete(m_managed_session_index);
        m_managed_session_index = -1;
    }
    return success;
}

// @address 0x3cfe00 ?handle_leave_internal@c_network_session@@QAA_NJ@Z
bool c_network_session::handle_leave_internal(long peer_index)
{
    bool result = false;
    long observer_channel_index = m_session_membership.get_observer_channel_index(peer_index);
    if (m_session_class == 2) {
        c_network_observer *observer = m_observer;
        c_network_observer::s_channel_observer *channel = &observer->m_channel_observers[observer_channel_index];
        if (channel->voice_active != 0 && channel->voice_talker != 0)
            observer->quality_statistics_report_badness(observer->find_observer_index_by_observer(channel), false, k_network_peer_leave_quality_reason);
    }
    long state = m_local_state;
    bool membership_locked = state == 8 || state == 7 || state == 9;
    if (!membership_locked && (m_session_membership.m_current_membership.valid_peer_mask.m_storage[peer_index >> 5] & (1 << (peer_index & 31))) != 0) {
        int message[2];
        memset(message, 0, sizeof(message));
        managed_session_get_id(m_managed_session_index, (s_transport_secure_identifier *)message);
        m_observer->observer_channel_send_message(observer_owner(), observer_channel_index, true, _network_message_type_leave_acknowledge, 8, message);
        m_session_membership.remove_peer(peer_index);
        result = true;
    } else if (state == 8) {
        handoff_remove_peer_from_consideration(peer_index);
        result = true;
    } else if (state == 9) {
        unsigned long peer_mask = 1 << peer_index;
        s_network_session_state_host_reestablish *state_data = &m_local_state_data.host_reestablish;
        if ((state_data->left_peer_mask & peer_mask) == 0)
            state_data->left_peer_mask |= peer_mask;
        result = true;
    }
    return result;
}

// @address 0x3cfd40 ?host_connection_refused@c_network_session@@QAAXPBUtransport_address@@W4e_network_join_refuse_reason@@@Z
void c_network_session::host_connection_refused(const transport_address *address, e_network_join_refuse_reason refuse_reason)
{
    long state = m_local_state;
    bool waiting_for_host = false;
    if (state == _network_session_state_peer_joining)
        waiting_for_host = transport_address_equivalent(address, &m_local_state_data.peer_joining.join_usable_address, false);
    if (waiting_for_host) {
        if (refuse_reason == _network_join_refuse_reason_holding_in_queue && state == _network_session_state_peer_joining) {
            m_local_state_data.peer_joining.join_holding_in_queue_timestamp = network_time_get();
        } else {
            m_join_refuse_reason = refuse_reason;
            disconnect();
        }
    }
}

// @address 0x3cfb98 ?join_abort@c_network_session@@QAA_NPBUtransport_address@@_K@Z
bool c_network_session::join_abort(const transport_address *address, unsigned __int64 join_nonce)
{
    bool result = false;
    long index = 0;
    long entry_count = g_network_join_data.join_queue_entry_count;
    if (entry_count > 0) {
        s_networking_join_queue_entry *entry = g_network_join_data.join_queue;
        do {
            if (entry->join_nonce == join_nonce) {
                network_join_remove_join_from_queue_internal(entry);
                entry_count = g_network_join_data.join_queue_entry_count;
            } else {
                index++;
                entry++;
            }
        } while (index < entry_count);
    }
    long state = m_local_state;
    bool membership_locked = state == 8 || state == 7 || state == 9;
    if (!membership_locked || state == 8 || state == 9) {
        abort_pending_join(join_nonce);
        result = true;
    }
    return result;
}

// @address 0x3cf288 ?leader_request_delegate_leadership@c_network_session@@QAA_NPBUs_transport_secure_address@@@Z
bool c_network_session::leader_request_delegate_leadership(const s_transport_secure_address *player)
{
    bool result = false;
    if (established()) {
        c_network_session_membership *membership = &m_session_membership;
        bool leader = false;
        if (membership->has_membership())
            leader = membership->m_local_peer_index == membership->m_current_membership.leader_peer_index;
        if (leader) {
            long peer_index = membership->get_peer_from_secure_address(player);
            if (peer_index != -1 && peer_index != m_session_membership.m_current_membership.leader_peer_index) {
                if ((this)->is_host()) {
                    membership->set_leader_peer_index(peer_index);
                    result = true;
                } else {
                    struct leadership_request_message {
                        s_transport_secure_identifier session_id;
                        s_transport_secure_address player;
                    } message;
                    memset(&message, 0, sizeof(message));
                    managed_session_get_id(m_managed_session_index, &message.session_id);
                    memcpy(&message.player, player, sizeof(message.player));
                    long channel = m_session_membership.get_observer_channel_index(m_session_membership.m_current_membership.host_peer_index);
                    if (channel != -1)
                        m_observer->observer_channel_send_message(observer_owner(), channel, false, _network_message_type_delegate_leadership,
                            sizeof(message), &message);
                    result = true;
                }
            }
        }
    }
    return result;
}

// @address 0x3cef48 ?peer_complete_player_add@c_network_session@@QAAXJPBUs_player_identifier@@@Z
void c_network_session::peer_complete_player_add(long user_index, const s_player_identifier *identifier)
{
    if (memcmp(identifier, &m_local_user_player_add[user_index].player_identifier, sizeof(s_player_identifier)) == 0) {
        memset(&m_local_user_player_add[user_index], 0, sizeof(m_local_user_player_add[user_index]));
        m_local_user_player_add[user_index].time = 0;
    }
}

static __forceinline bool network_session7_join_allowed_by_privacy(const c_network_session *session)
{
    bool allowed = true;
    if ((*(c_network_session_parameters *)session->m_session_parameters).m_parameters.privacy_mode.get_allowed()) {
        const s_network_session_privacy_mode *privacy = (*(c_network_session_parameters *)session->m_session_parameters).m_parameters.privacy_mode.get();
        if (privacy->closed_mode != 0 || privacy->is_closed_by_user)
            allowed = false;
    }
    return allowed;
}

static __forceinline bool network_session7_player_identifier_is_valid(const s_player_identifier *identifier)
{
    unsigned char invalid[8];
    int i;
    invalid[0] = 0;
    for (i = 1; i < 8; i++)
        invalid[i] = 0;
    return memcmp(invalid, identifier, sizeof(*identifier)) != 0;
}

// @address 0x3cf860 ?join_accept@c_network_session@@QAAXPBUs_network_session_join_request@@PBUtransport_address@@@Z
void c_network_session::join_accept(const s_network_session_join_request *join_request, const transport_address *address)
{
    e_network_join_refuse_reason result = can_accept_join_request(join_request);
    long peer;
    for (peer = 0; !result && peer < join_request->joining_peer_count; peer++) {
        const s_network_session_join_request_peer *request_peer = &join_request->joining_peers[peer];
        c_network_session_membership *membership = &m_session_membership;
        long peer_index = membership->get_peer_from_secure_address(&request_peer->address);
        if (peer_index != -1) {
            result = _network_join_refuse_reason_address_invalid;
            continue;
        }
        long i;
        for (i = 0; i < 16; i++) {
#ifdef HALO3_64BIT
            if (!membership->m_current_membership.valid_peer_mask.test(i)) {
#else
            if ((((const unsigned long *)this)[(i >> 5) + 17] & (1 << (i & 31))) == 0) {
#endif
                peer_index = i;
                break;
            }
        }
        if (peer_index == -1) {
            result = _network_join_refuse_reason_squad_full;
        } else {
            if (membership->add_peer(peer_index, _network_session_peer_state_reserved, request_peer->network_version, &request_peer->address,
                    join_request->join_party_nonce, join_request->join_nonce))
#ifdef HALO3_64BIT
                m_observer->observer_channel_initiate_connection(observer_owner(), membership->get_observer_channel_index(peer_index));
#else
                m_observer->observer_channel_initiate_connection(observer_owner(), *(const long *)((const char *)this + (peer_index + 0x638) * 12));
#endif
            else
                result = _network_join_refuse_reason_too_many_observers;
            if (!result) {
                long user;
                for (user = 0; user < 4; user++) {
                    long requested_player_index = request_peer->player_indices[user];
                    if (requested_player_index != -1) {
                        long player_index = membership->find_or_add_player(peer_index, user,
                            &join_request->joining_players[requested_player_index].player_identifier,
                            join_request->joining_players[requested_player_index].player_xuid,
                            join_request->joining_players[requested_player_index].is_silver_or_gold_live,
                                join_request->joining_players[requested_player_index].is_online_enabled,
                                join_request->joining_players[requested_player_index].is_guest,
                            join_request->joining_players[requested_player_index].is_bot, join_request->join_from_recruiting);
                        s_player_configuration configuration;
                        memset(&configuration, 0, sizeof(configuration));
                        configuration.client.xuid = join_request->joining_players[requested_player_index].player_xuid;
                        configuration.client.online = join_request->joining_players[requested_player_index].is_silver_or_gold_live;
                        configuration.client.silver = join_request->joining_players[requested_player_index].is_online_enabled;
                        configuration.client.desired_team_index = -1;
                        configuration.client.guest = join_request->joining_players[requested_player_index].is_guest;
                        configuration.client.bot = join_request->joining_players[requested_player_index].is_bot;
                        configuration.client.global_statistics = join_request->joining_players[requested_player_index].global_statistics;
                        configuration.client.hopper_statistics = join_request->joining_players[requested_player_index].hopper_statistics;
                        configuration.host.team_index = -1;
                        configuration.host.user_team_index = -1;
                        multiplayer_game_hopper_build_calculated_stats(&configuration.client.global_statistics,
                            &configuration.client.hopper_statistics, &configuration.host.calculated_statistics);
                        memcpy(&membership->m_current_membership.players[player_index].configuration, &configuration, sizeof(configuration));
                        membership->m_current_membership.update_number = membership->m_current_membership.update_number + 1;
                        membership->m_local_membership_update_number = membership->m_local_membership_update_number + 1;
                    }
                }
            }
        }
    }
    if (!result) {
        c_network_session_membership *membership = &m_session_membership;
        long peer_index = membership->get_peer_from_incoming_address(address);
        if (peer_index != -1) {
#ifdef HALO3_64BIT
            long channel = membership->get_observer_channel_index(peer_index);
#else
            long channel = *(const long *)((const char *)this + (peer_index + 0x638) * 12);
#endif
            if (channel != -1) {
                if (m_session_membership.m_current_membership.peers[peer_index].connection_state == 2) {
                    membership->m_current_membership.peers[peer_index].connection_state = _network_session_peer_state_connected;
                    membership->m_current_membership.update_number = membership->m_current_membership.update_number + 1;
                    membership->m_local_membership_update_number = membership->m_local_membership_update_number + 1;
                }
                m_observer->observer_channel_initiate_connection(observer_owner(), channel);
            } else {
                result = _network_join_refuse_reason_too_many_observers;
            }
        } else {
            result = _network_join_refuse_reason_address_invalid;
        }
    }
    if (result) {
        acknowledge_join_request(address, result);
        abort_pending_join(join_request->join_nonce);
    }
}

// @address 0x3d1b38 ?can_accept_join_request@c_network_session@@QBA?AW4e_network_join_refuse_reason@@PBUs_network_session_join_request@@@Z
e_network_join_refuse_reason c_network_session::can_accept_join_request(const s_network_session_join_request *join_request) const
{
    int result = 0;
    if (!(this)->is_host())
        result = 6;
    if (result == 0 && !network_session7_join_allowed_by_privacy(this))
        result = 5;
    if (result == 0) {
        long peer_count = join_request->joining_peer_count;
        long player_count = 0;
        long peer;
        for (peer = 0; result == 0 && peer < join_request->joining_peer_count; peer++) {
            const s_network_session_join_request_peer *request_peer = &join_request->joining_peers[peer];
            long existing_peer = m_session_membership.get_peer_from_secure_address(&request_peer->address);
            long peer_player_count = 0;
            long user;
            for (user = 0; result == 0 && user < 4; user++) {
                long player_index = request_peer->player_indices[user];
                if (player_index != -1) {
                    if (network_session7_player_identifier_is_valid(&join_request->joining_players[player_index].player_identifier)) {
                        player_count++;
                        peer_player_count++;
                        result = can_accept_player_join_request(&join_request->joining_players[player_index].player_identifier, existing_peer, user,
                            join_request->joining_players[player_index].player_xuid, join_request->joining_players[player_index].is_online_enabled,
                            join_request->joining_players[player_index].is_guest, join_request->joining_players[player_index].is_bot, false);
                    }
                }
            }
            if (result == 0) {
                if (peer_player_count > g_network_configuration.maximum_players_per_peer) {
                    result = 8;
                } else {
                    if (existing_peer != -1) {
                        peer_count--;
                        player_count -= m_session_membership.m_current_membership.peers[existing_peer].player_count;
                    }
                    if (peer_player_count == 0)
                        result = _network_join_refuse_reason_player_count_zero;
                }
            }
        }
        if (result == 0 && session_is_full(peer_count, player_count))
            result = _network_join_refuse_reason_squad_full;
    }
    return (e_network_join_refuse_reason)result;
}

// @address 0x3cffa8 ?desire_channel_heartbeat@c_network_session@@UBA_NJ@Z
bool c_network_session::desire_channel_heartbeat(long observer_channel_index) const
{
    bool result = false;
    if (m_session_membership.has_membership()) {
        long peer_index = m_session_membership.get_peer_from_observer_channel(observer_channel_index);
        if (peer_index == m_session_membership.m_current_membership.host_peer_index)
            result = 1;
        else if (m_session_membership.m_local_peer_index == m_session_membership.m_current_membership.host_peer_index && peer_index != -1)
            result = 1;
    }
    return result;
}

void c_network_session::check_to_send_peer_join_request(void)
{
    if (m_session_membership.m_current_membership.update_number != -1 || m_local_state_data.peer_joining.join_holding_in_queue_timestamp != -1)
        return;
    if (m_observer->get_secure_connection_status(m_local_state_data.peer_joining.observer_channel_index) != 2)
        return;
    long last_time = m_local_state_data.peer_joining.join_request_timestamp;
    long now;
    if (g_network_time_globals.enabled)
        now = g_network_time_globals.time;
    else
        now = GetTickCount();
    if (now - last_time <= g_network_configuration.peer_join_request_check_interval)
        return;
    if (m_local_state_data.peer_joining.use_join_nonce) {
        struct s_peer_join_request_message {
            unsigned short message_type;
            char pad02[2];
            s_transport_secure_identifier identifier;
            unsigned __int64 join_nonce;
        } message;
        memset(&message, 0, sizeof(message));
        message.message_type = 7;
        managed_session_get_id(m_managed_session_index, &message.identifier);
        message.join_nonce = m_local_state_data.peer_joining.join_nonce;
        m_message_gateway->send_message_directed(
            &m_local_state_data.peer_joining.join_usable_address, _network_message_type_peer_connect, sizeof(message), &message);
        ++m_local_state_data.peer_joining.join_request_count;
        m_local_state_data.peer_joining.join_request_timestamp = network_time_get();
        return;
    } else {
        struct s_peer_join_abort_message {
            unsigned short message_type;
            char pad02[2];
            long compatible_version;
            long unknown;
            s_transport_secure_identifier identifier;
            char pad14[4];
            s_network_session_join_request join_request;
        } message;
        memset(&message, 0, sizeof(message));
        message.message_type = 7;
        message.compatible_version = k_network_compatible_version;
        message.unknown = 0x2e4f;
        managed_session_get_id(m_managed_session_index, &message.identifier);
        message.join_request = m_local_state_data.peer_joining.join_request;
        m_message_gateway->send_message_directed(
            &m_local_state_data.peer_joining.join_usable_address, _network_message_type_join_request, sizeof(message), &message);
        ++m_local_state_data.peer_joining.join_request_count;
        m_local_state_data.peer_joining.join_request_timestamp = network_time_get();
        return;
    }
}

// @address 0x3d06f8 ?check_to_send_peer_reestablish@c_network_session@@AAA?AW4e_peer_reestablish_state@@W42@@Z
e_peer_reestablish_state c_network_session::check_to_send_peer_reestablish(e_peer_reestablish_state peer_reestablish_state)
{
    long managed_session_index = m_managed_session_index;
    unsigned short status;
    managed_session_get_status(&status, managed_session_index);
    switch (peer_reestablish_state) {
    case 1:
        if (network_session3_status_test(&status, 4)) {
            managed_session_use_new_host(managed_session_index);
            if (m_session_class == 1) {
                c_network_session_membership *membership = &m_session_membership;
                c_network_observer *observer = m_observer;
                long channel_index = membership->m_peer_channels[membership->m_current_membership.host_peer_index].observer_channel_index;
                c_network_observer::s_channel_observer *channel = &observer->m_channel_observers[channel_index];
                unsigned char closed = channel->channel.m_state <= 2;
                if (!closed) {
                    observer->discard_secure_connection(channel_index, false, _network_channel_closure_reason_observer_reset_security);
                    channel->flags &= ~4;
                }
                m_observer->observer_channel_initiate_connection((e_network_observer_owner)m_session_index, channel_index);
            }
            peer_reestablish_state = (e_peer_reestablish_state)2;
        }
        break;
    case 2:
        {
            c_network_session_membership *membership = &m_session_membership;
            c_network_observer *observer = m_observer;
            if (observer->m_channel_observers[
                    membership->m_peer_channels[membership->m_current_membership.host_peer_index].observer_channel_index].state == 7) {
                s_transport_secure_identifier message;
                long channel;
                memset(&message, 0, sizeof(message));
                managed_session_get_id(managed_session_index, &message);
                channel = m_session_membership.m_peer_channels[m_session_membership.m_current_membership.host_peer_index].observer_channel_index;
                if (channel != -1)
                    observer->observer_channel_send_message((e_network_observer_owner)m_session_index, channel, false,
                        _network_message_type_peer_reestablish, sizeof(message), &message);
                peer_reestablish_state = (e_peer_reestablish_state)3;
            }
        }
        break;
    case 3:
        if (m_session_membership.m_host_reestablished) {
            unsigned int i;
            unsigned char *flags;
            managed_session_commit_new_host(managed_session_index);
            flags = &m_observer->m_channel_observers[0].flags;
            for (i = 32; i != 0; i--) {
                *flags &= ~0x20;
                flags += sizeof(c_network_observer::s_channel_observer);
            }
            peer_reestablish_state = (e_peer_reestablish_state)4;
        }
        break;
    case 4:
        if (m_session_membership.all_peers_established())
            peer_reestablish_state = _peer_reestablish_state_none;
        break;
    }
    return peer_reestablish_state;
}

// @address 0x3d0ce0 ?check_to_send_host_reestablish@c_network_session@@AAAXXZ
void c_network_session::check_to_send_host_reestablish(void)
{
    s_network_session_state_host_reestablish *local_state = &m_local_state_data.host_reestablish;
    bool send_to_all = false;
    int last_time;
    int now;
    long peer_index;
    if ((unsigned int)((1 << local_state->original_host_peer_index) | local_state->left_peer_mask | local_state->acknowledged_peer_mask) == (unsigned
        int)m_session_membership.m_current_membership.valid_peer_mask.m_storage[0]
        || (last_time = local_state->start_time, now = network_time_get(), now - last_time >= g_network_configuration.host_reestablish_message_interval))
        send_to_all = true;
    for (peer_index = (&m_session_membership)->get_first_peer(); peer_index != -1;
        peer_index = (&m_session_membership)->get_next_peer(peer_index))
    {
        if ((peer_index != local_state->original_host_peer_index || send_to_all) && (((unsigned int)local_state->requested_peer_mask & (1 <<
            peer_index)) == 0)) {
            long channel = m_session_membership.get_observer_channel_index(peer_index);
            if (channel != -1) {
                if (m_observer->m_channel_observers[channel].state == 7) {
                    s_network_message_host_reestablish message;
                    long managed_session_index;
                    memset(&message, 0, sizeof(message));
                    managed_session_index = m_managed_session_index;
                    managed_session_get_id(managed_session_index, &message.session_id);
                    const s_online_managed_session *managed_session =
                        &online_session_manager_globals.sessions[managed_session_index];
                    message.previous_description = managed_session->previous_description;
                    message.secure_key_platform = managed_session->platform;
                    if (m_session_membership.get_observer_channel_index(peer_index) != -1)
                        m_observer->observer_channel_send_message(observer_owner(), channel, false, _network_message_type_host_reestablish,
                            sizeof(message), &message);
                    local_state->requested_peer_mask |= (1 << peer_index);
                }
            }
        }
    }
}

__forceinline void c_network_session::update_membership()
{
    s_network_message_membership_update message1;
    s_network_message_membership_update message2;
    unsigned long message1_peers = 0;
    unsigned long message2_peers = 0;
    for (long peer_index = (&m_session_membership)->get_first_peer(); peer_index != -1;
        peer_index = (&m_session_membership)->get_next_peer(peer_index))
    {
        long observer_channel = m_session_membership.m_peer_channels[peer_index].observer_channel_index;
        long update_number = m_session_membership.m_peer_channels[peer_index].membership_update_number;
        bool needs_update = observer_channel != -1;
        if (needs_update)
        {
            if (m_session_membership.m_current_membership.peers[peer_index].connection_state >= 4)
                needs_update = true;
            else
                needs_update = false;
        }
        else
            needs_update = !(!needs_update);
        needs_update = needs_update && m_session_membership.m_peer_channels[peer_index].peer_needs_reestablishment == 0;
        needs_update = needs_update && m_observer->m_channel_observers[observer_channel].state == 7;
        needs_update = needs_update && update_number != m_session_membership.m_current_membership.update_number;
        if (needs_update && m_observer->observer_channel_backlogged(observer_owner(), observer_channel, _network_message_type_membership_update)) {
            needs_update = false;
            m_observer->network_session_mark_message_waiting(observer_channel, 26);
        }
        if (needs_update && m_local_state == _network_session_state_host_reestablish)
            needs_update = false;
        if (needs_update) {
            if (update_number != -1 && update_number == m_session_membership.m_transmitted_membership.update_number)
                message1_peers |= 1 << peer_index;
            else
                message2_peers |= 1 << peer_index;
        }
    }
    if (message2_peers != 0)
        m_session_membership.build_membership_update((const s_network_session_shared_membership
            *)&m_session_membership.m_current_membership.update_number, 0, &message2);
    if (message1_peers != 0)
        m_session_membership.build_membership_update((const s_network_session_shared_membership *)&m_session_membership.m_current_membership.update_number,
            (const s_network_session_shared_membership *)&m_session_membership.m_transmitted_membership.update_number, &message1);
    for (long peer_index = (&m_session_membership)->get_first_peer(); peer_index != -1;
        peer_index = (&m_session_membership)->get_next_peer(peer_index))
    {
        unsigned long peer_bit = 1 << peer_index;
        long observer_channel = m_session_membership.m_peer_channels[peer_index].observer_channel_index;
        s_network_message_membership_update *message;
        if (peer_bit & message1_peers)
            message = &message1;
        else if (peer_bit & message2_peers)
            message = &message2;
        else
            continue;
        if (m_observer->m_channel_observers[observer_channel].state == 7) {
            if (m_observer->observer_channel_backlogged(observer_owner(), observer_channel, _network_message_type_membership_update)) {
                m_observer->network_session_mark_message_waiting(observer_channel, 26);
            }
            else {
                m_session_membership.set_membership_update_number(peer_index, m_session_membership.m_current_membership.update_number);
                if (observer_channel != -1)
                    m_observer->observer_channel_send_message(observer_owner(), observer_channel, false, _network_message_type_membership_update,
                        sizeof(*message), message);
            }
        }
    }
    c_network_session_membership *membership = &m_session_membership;
    if (message2_peers != 0 || message1_peers != 0) {
        memcpy(&membership->m_transmitted_membership.update_number, &membership->m_current_membership.update_number,
            sizeof(s_network_session_shared_membership));
        membership->m_transmitted_baseline_checksum = fast_checksum(-1, (const s_network_session_shared_membership
            *)&membership->m_transmitted_membership.update_number);
    }
}

// @address 0x3d0e90 network_session_update_membership
extern "C" void network_session_update_membership(c_network_session *session)
{
    session->update_membership();
}

// @address 0x3d1400 ?handle_disconnection@c_network_session@@AAAXXZ
void c_network_session::handle_disconnection()
{
    if (!leaving_session()) {
        bool is_established = ((unsigned long)m_local_state >= 4 && (unsigned long)m_local_state < 10);
        if (is_established && !is_host() && m_session_membership.has_membership() && m_session_membership.m_current_membership.peer_count > 1
            && m_session_membership.m_current_membership.peers[m_session_membership.m_local_peer_index].connection_state == 8) {
            c_network_session_membership *membership = &m_session_membership;
            c_network_observer *observer = m_observer;
            long channel_index = membership->m_peer_channels[membership->m_current_membership.host_peer_index].observer_channel_index;
            c_network_observer::s_channel_observer *channel_observer = &observer->m_channel_observers[channel_index];
            unsigned char closed = channel_observer->channel.m_state <= 2;
            if (!closed)
                channel_observer->channel.close(_network_channel_closure_reason_observer_reset);
            change_local_state_election();
            return;
        }
    }
    disconnect();
}

// @address 0x3cefb0 ?peer_request_player_desired_properties_update@c_network_session@@QAA_NJJW4e_controller_index@@PBUs_player_configuration_from_client@@K@Z
bool c_network_session::peer_request_player_desired_properties_update(long user_index, long peer_index, e_controller_index controller_index,
    const s_player_configuration_from_client *configuration, unsigned long voice_settings)
{
    bool result = false;
    if (established()) {
        if ((this)->is_host()) {
            c_network_session_membership *membership = &m_session_membership;
            long player_index = membership->m_current_membership.peers[membership->m_local_peer_index].user_player_indices[user_index];
            if (player_index != -1) {
                membership->set_player_properties(player_index, peer_index, controller_index, configuration, voice_settings);
                result = true;
            }
        } else {
            s_network_message_player_properties message;
            memset(&message, 0, sizeof(message));
            managed_session_get_id(m_managed_session_index, &message.session_id);
            message.user_index = user_index;
            message.update_number = peer_index;
            message.controller_index = controller_index;
            message.voice = voice_settings;
            message.configuration = *configuration;
            long channel = m_session_membership.m_peer_channels[m_session_membership.m_current_membership.host_peer_index].observer_channel_index;
            if (channel != -1)
                m_observer->observer_channel_send_message(observer_owner(), channel, false, _network_message_type_player_properties, sizeof(message), &message);
            result = true;
        }
    }
    return result;
}

static __forceinline bool network_session_status_test(unsigned short flags, long bit)
{
    bool result = (flags >> bit) & 1;
    return result;
}

static __forceinline void network_session_status_set(unsigned short &flags, long bit, bool enable)
{
    if (enable)
        flags |= (1 << bit);
    else
        flags &= ~(1 << bit);
}

static __forceinline void network_session_managed_flag_set(unsigned long &flags, long bit, bool value)
{
    if (value)
        flags |= (1 << bit);
    else
        flags &= ~(1 << bit);
}

static __forceinline void network_session_managed_session_reset_players_add_status(long index)
{
    s_online_managed_session *session = &online_session_manager_globals.sessions[index];
    network_session_managed_flag_set(session->flags, 11, false);
    network_session_managed_flag_set(session->flags, 12, false);
}

static __forceinline bool network_session_player_identifier_is_valid(const s_player_identifier *identifier)
{
    s_player_identifier zero_identifier = { 0 };
    return memcmp(&zero_identifier, identifier, sizeof(s_player_identifier)) != 0;
}


void c_network_session::process_pending_joins()
{
    c_network_session_membership *membership = &m_session_membership;
    unsigned short session_flags;
    managed_session_get_status(&session_flags, m_managed_session_index);

    if (network_session_status_test(session_flags, 6) || network_session_status_test(session_flags, 7))
    {
        network_session_managed_session_reset_players_add_status(m_managed_session_index);
        if (network_session_player_identifier_is_valid(&m_player_we_are_adding.player_identifier))
        {
            e_network_join_refuse_reason refuse_reason = _network_join_refuse_reason_player_add_failed;
            if (network_session_status_test(session_flags, 6))
            {
                refuse_reason = can_accept_player_join_request(
                    &m_player_we_are_adding.player_identifier,
                    m_player_we_are_adding.peer_index,
                    m_player_we_are_adding.user_index,
                    m_player_we_are_adding.xuid,
                    m_player_we_are_adding.silver,
                    m_player_we_are_adding.guest,
                    m_player_we_are_adding.bot,
                    true);
                if (refuse_reason == _network_join_refuse_reason_none &&
                    !membership->is_player_in_player_add_queue(&m_player_we_are_adding.player_identifier))
                    refuse_reason = _network_join_refuse_reason_player_add_failed;
                if (refuse_reason == _network_join_refuse_reason_none)
                    membership->commit_player_from_player_add_queue(&m_player_we_are_adding.player_identifier);
                else
                    managed_session_remove_players(m_managed_session_index, (const unsigned __int64 *)&m_player_we_are_adding.player_identifier, 1);
            }
            network_session_refuse_pending_player(this, refuse_reason);
        }
        else
        {
            long peer_index;
            for (peer_index = (membership)->get_first_peer(); peer_index != -1;
                peer_index = (membership)->get_next_peer(peer_index))
            {
                if (membership->get_peer_connection_state(peer_index) == 5)
                    break;
            }
            if (peer_index != -1)
            {
                unsigned __int64 join_nonce = membership->get_join_nonce(peer_index);
                if (network_session_status_test(session_flags, 6))
                {
                    for (; peer_index != -1; peer_index = (membership)->get_next_peer(peer_index))
                    {
                        if (membership->get_peer_connection_state(peer_index) == 5 && join_nonce == membership->get_join_nonce(peer_index))
                        {
                            if (peer_index == membership->host_peer_index())
                                membership->set_peer_connection_state(peer_index, _network_session_peer_state_waiting);
                            else
                                membership->set_peer_connection_state(peer_index, _network_session_peer_state_joined);
                        }
                    }
                }
                else if (join_nonce == m_host_join_nonce && !managed_session_is_master_session(m_managed_session_index))
                {
                    session_flags = 0;
                    disconnect();
                }
                else if (join_nonce == m_host_join_nonce)
                {
                    for (long kick_peer_index = (membership)->get_first_peer();
                        kick_peer_index != -1;
                        kick_peer_index = (membership)->get_next_peer(kick_peer_index))
                    {
                        if (kick_peer_index != membership->host_peer_index())
                            boot_peer(kick_peer_index, _network_session_boot_reason_recreating_session);
                        else
                            membership->set_peer_connection_state(kick_peer_index, _network_session_peer_state_connected);
                    }
                    managed_session_reset_session(m_managed_session_index, true);
                }
                else
                {
                    abort_pending_join(join_nonce);
                }
            }
        }
    }

    if (!disconnected())
    {
        unsigned long peer_waiting_mask = 0;
        for (long peer_index = (membership)->get_first_peer(); peer_index != -1;
            peer_index = (membership)->get_next_peer(peer_index))
        {
            long connection_state = membership->get_peer_connection_state(peer_index);
            if (connection_state == 7)
                peer_waiting_mask |= (1 << peer_index);
            else if (connection_state == 4 && !network_session_status_test(session_flags, 5))
                peer_waiting_mask |= (1 << peer_index);
        }

        long peer_index;
        unsigned long peer_bit;
        for (peer_index = 0, peer_bit = 1; peer_waiting_mask != 0; peer_index++, peer_bit <<= 1)
        {
            if (peer_waiting_mask & peer_bit)
            {
                bool can_advance = true;
                long connection_state = membership->get_peer_connection_state(peer_index);
                unsigned __int64 join_nonce = membership->get_join_nonce(peer_index);
                peer_waiting_mask &= ~peer_bit;
                for (long check_peer_index = (membership)->get_first_peer();
                    check_peer_index != -1;
                    check_peer_index = (membership)->get_next_peer(check_peer_index))
                {
                    if (membership->get_join_nonce(check_peer_index) == join_nonce)
                    {
                        peer_waiting_mask &= ~(1 << check_peer_index);
                        if (membership->get_peer_connection_state(check_peer_index) != connection_state)
                            can_advance = false;
                    }
                }
                if (can_advance)
                {
                    if (connection_state == 4)
                    {
                        add_pending_join_to_session(join_nonce);
                        network_session_status_set(session_flags, 5, true);
                    }
                    else
                    {
                        for (long advance_peer_index = peer_index; advance_peer_index != -1;
                            advance_peer_index = (membership)->get_next_peer(advance_peer_index))
                        {
                            if (membership->get_join_nonce(advance_peer_index) == join_nonce)
                                membership->set_peer_connection_state(advance_peer_index, _network_session_peer_state_established);
                        }
                        m_host_migration_deferred_time = network_time_get();
                    }
                    break;
                }
            }
        }
    }

    if (!disconnected() && !network_session_status_test(session_flags, 5))
    {
        const s_player_add_queue_entry *add_entry = membership->get_first_player_from_player_add_queue();
        bool player_occupies_public_slot = true;
        if (add_entry != 0)
        {
            m_player_we_are_adding.player_identifier = add_entry->player_identifier;
            m_player_we_are_adding.peer_index = add_entry->peer_index;
            m_player_we_are_adding.user_index = add_entry->peer_user_index;
            m_player_we_are_adding.xuid = add_entry->player_data.xuid;
            m_player_we_are_adding.silver = add_entry->player_data.silver;
            m_player_we_are_adding.guest = add_entry->player_data.guest;
            m_player_we_are_adding.bot = add_entry->player_data.bot;
            e_network_join_refuse_reason refuse_reason = can_accept_player_join_request(
                &m_player_we_are_adding.player_identifier,
                m_player_we_are_adding.peer_index,
                m_player_we_are_adding.user_index,
                m_player_we_are_adding.xuid,
                m_player_we_are_adding.silver,
                m_player_we_are_adding.guest,
                m_player_we_are_adding.bot,
                true);
            if (refuse_reason != _network_join_refuse_reason_none)
                network_session_refuse_pending_player(this, refuse_reason);
            else
                managed_session_add_players(m_managed_session_index, &add_entry->player_data.xuid, &m_player_we_are_adding.silver,
                    &player_occupies_public_slot, 1);
        }
    }

    if (!disconnected() && !membership_is_locked())
    {
        unsigned long timeout = (session_type() == _network_session_type_squad)
            ? g_network_configuration.squad_join_timeout
            : g_network_configuration.group_join_timeout;
        for (long peer_index = (membership)->get_first_peer(); peer_index != -1;
            peer_index = (membership)->get_next_peer(peer_index))
        {
            unsigned __int64 join_nonce = membership->get_join_nonce(peer_index);
            unsigned long creation_timestamp = membership->get_creation_timestamp(peer_index);
            if ((join_nonce != m_host_join_nonce
                    && membership->get_peer_connection_state(peer_index) < 8
                    && timeout != (unsigned long)-1
                    && network_time_since_inline(creation_timestamp) >= timeout)
                || (join_nonce != m_host_join_nonce
                    && membership->get_peer_connection_state(peer_index) == 3))
            {
                abort_pending_join(join_nonce);
            }
        }
    }
}

extern "C" {


}

__forceinline void c_network_session_membership::idle()
{
    commit_players();
    if (is_host())
    {
        for (long player_index = get_first_player(); player_index != -1; player_index = get_next_player(player_index))
            update_player_configuration_from_host(player_index);
    }
}

__forceinline void c_network_session::idle_peer_join_abort()
{
    long elapsed = network_time_since_inline(m_local_state_data.peer_join_abort.join_abort_start_timestamp);
    if (elapsed > g_network_configuration.peer_join_abort_timeout)
        disconnect();
    else
        check_to_send_peer_join_abort();
}

__forceinline void c_network_session::idle_peer_established()
{
    if (m_local_state_data.peer_leaving.peer_reestablish_state != _peer_reestablish_state_none)
        m_local_state_data.peer_leaving.peer_reestablish_state = check_to_send_peer_reestablish(m_local_state_data.peer_leaving.peer_reestablish_state);
}

__forceinline void c_network_session::idle_peer_leaving()
{
    if (m_local_state_data.peer_leaving.peer_reestablish_state != _peer_reestablish_state_none)
    {
        m_local_state_data.peer_leaving.peer_reestablish_state = check_to_send_peer_reestablish(m_local_state_data.peer_leaving.peer_reestablish_state);
        if (m_local_state_data.peer_leaving.peer_reestablish_state == _peer_reestablish_state_none)
            m_local_state_data.peer_leaving.leave_start_timestamp = network_time_get();
    }
    if (m_local_state_data.peer_leaving.peer_reestablish_state == _peer_reestablish_state_none)
    {
        long elapsed = network_time_since_inline(m_local_state_data.peer_leaving.leave_start_timestamp);
        if (elapsed > g_network_configuration.peer_leave_timeout)
            disconnect();
        else
            check_to_send_peer_leave_request();
    }
}

__forceinline void c_network_session::idle()
{
    if (!disconnected())
    {
        m_session_membership.idle();

        if (established() && m_session_membership.all_peers_established() && m_disconnect_reestablish_state == 0)
            m_disconnect_reestablish_state = 2;

        switch (current_local_state())
        {
        case _network_session_state_peer_creating:
            idle_peer_creating();
            break;
        case _network_session_state_peer_joining:
            idle_peer_joining();
            break;
        case _network_session_state_peer_join_abort:
            idle_peer_join_abort();
            break;
        case _network_session_state_peer_established:
            idle_peer_established();
            break;
        case _network_session_state_peer_leaving:
            idle_peer_leaving();
            break;
        case _network_session_state_host_disband:
            disconnect();
            break;
        case _network_session_state_host_handoff:
            idle_host_handoff();
            break;
        case _network_session_state_host_reestablish:
            idle_host_reestablish();
            break;
        case _network_session_state_election:
            idle_election();
            break;
        }

        if (is_host() && !membership_is_locked())
            process_pending_joins();

        if (is_host())
        {
            if (!membership_is_locked())
            {
                for (long peer_index = m_session_membership.get_first_peer(); peer_index != -1; peer_index = m_session_membership.get_next_peer(peer_index))
                {
                    long observer_channel_index = m_session_membership.get_observer_channel_index(peer_index);
                    if (observer_channel_index != -1 && m_observer->observer_channel_dead(observer_owner(), observer_channel_index))
                    {
                        bool disband = true;
                        long host_badness = m_session_membership.get_host_peer()->properties.connectivity_badness +
                            g_network_configuration.bandwidth.latency_tier_delta;
                        if (g_network_configuration.host_migration_on_channel_death != 0
                            && !network_owner_entry_is_pending(&m_session_membership, peer_index)
                            && m_session_membership.get_peer(peer_index)->connection_state == 8
                            && host_badness >= m_session_membership.get_peer(peer_index)->properties.host_badness)
                        {
                            do
                            {
                                long game_mode = -1;
                                if ((*(c_network_session_parameters *)m_session_parameters).m_parameters.ui_game_mode.get_allowed())
                                    game_mode = *(*(c_network_session_parameters *)m_session_parameters).m_parameters.ui_game_mode.get();
                                e_bandwidth_configuration_host_preference host_preference;
                                if (session_type() == _network_session_type_group)
                                    host_preference = _bandwidth_configuration_host_preference_latency;
                                else if (simulation_starting_up() && game_globals->options.game_mode == 3)
                                    host_preference = _bandwidth_configuration_host_preference_connectivity;
                                else
                                {
                                    if (game_mode == 0 || game_mode == 4)
                                        break;
                                    host_preference = _bandwidth_configuration_host_preference_quality;
                                }
                                s_network_session_find_host result = find_good_peer_to_be_host(
                                    host_preference, false, false, false, -1, 1 << peer_index, 1 << peer_index, true);
                                long host_peer_index = result.host_peer_index;
                                if (host_peer_index != -1 && host_peer_index != m_session_membership.m_current_membership.host_peer_index)
                                {
                                    host_delegate_host(host_peer_index, false, false);
                                    disband = false;
                                }
                            } while (0);
                        }
                        if (disband)
                            disband_peer(peer_index);
                    }
                }

                if (established() && session_type() == _network_session_type_squad)
                {
                    long public_slot_count = 0;
                    long private_slot_count = 0;
                    bool friends_only = false;
                    long maximum_player_count = 0;
                    if ((*(c_network_session_parameters *)m_session_parameters).m_parameters.session_size.get_allowed())
                    {
                        maximum_player_count = network_session_get_maximum_player_count(this);
                        if (maximum_player_count < 0)
                            maximum_player_count = 0;
                    }
                    if ((*(c_network_session_parameters *)m_session_parameters).m_parameters.privacy_mode.get_allowed())
                    {
                        const s_network_session_privacy_mode *privacy = (*(c_network_session_parameters *)m_session_parameters).m_parameters.privacy_mode.get();
                        if (privacy->closed_mode == 0 && !privacy->is_closed_by_user)
                        {
                            switch (privacy->privacy_mode)
                            {
                            case _network_game_privacy_friends_only:
                                friends_only = true;
                            case _network_game_privacy_open:
                                private_slot_count = maximum_player_count;
                                public_slot_count = 0;
                                break;
                            case _network_game_privacy_invite_only:
                                private_slot_count = 0;
                                public_slot_count = maximum_player_count;
                                break;
                            }
                        }
                    }
                    if (m_session_membership.m_current_membership.private_slot_count != public_slot_count
                        || m_session_membership.m_current_membership.public_slot_count != private_slot_count
                        || m_session_membership.m_current_membership.friends_only != friends_only)
                        m_session_membership.set_slot_counts(public_slot_count, private_slot_count, friends_only);
                }
            }
        }
        else if (established())
        {
            long observer_channel_index = m_session_membership.get_host_observer_channel_index();
            if (m_observer->observer_channel_dead(observer_owner(), observer_channel_index))
                handle_disconnection();
        }

        if (established() && !is_host())
            check_to_send_time_synchronization();

        idle_observer_state();

        if (is_host())
            network_session_update_membership(this);

        (*(c_network_session_parameters *)m_session_parameters).update();

        if (simulation_is_initialized())
        {
            c_simulation_world *world = simulation_get_world();
            if (world && world->m_world_type != 0 && !simulation_world_test_all_client_views_active(world))
                m_host_migration_deferred_time = network_time_get();
        }

        bool migrate = host_established() && m_close_session_for_matchmaking && m_session_class == _network_session_class_xbox_live;
        if (migrate)
        {
            migrate = false;
            if (!network_observer_is_join_in_progress())
                migrate = true;
        }

        if (migrate)
        {
            bool find_host = false;
            long excluded_peer_mask = m_host_migration_attempted_peer_mask;
            long migration_interval;
            e_bandwidth_configuration_host_preference host_preference;
            if (session_type() == _network_session_type_group)
            {
                migration_interval = g_network_configuration.bandwidth.host_migration_interval_group;
                host_preference = _bandwidth_configuration_host_preference_latency;
            }
            else if (simulation_starting_up() && game_globals->options.game_mode == 3)
            {
                migration_interval = g_network_configuration.bandwidth.host_migration_interval_campaign;
                host_preference = _bandwidth_configuration_host_preference_connectivity;
            }
            else
            {
                migration_interval = g_network_configuration.bandwidth.host_migration_interval_matchmaking;
                host_preference = _bandwidth_configuration_host_preference_quality;
            }

            if (migration_interval != 0
                && network_time_since_inline(m_last_host_migration_time) > g_network_configuration.bandwidth.host_migration_minimum_interval
                && network_time_since_inline(m_host_migration_deferred_time) > migration_interval)
                find_host = true;

            if (find_host)
            {
                s_network_session_find_host result = find_good_peer_to_be_host(host_preference, true, false, false, -1, excluded_peer_mask, 0, true);
                long host_peer_index = result.host_peer_index;
                if (host_peer_index != -1 && host_peer_index != m_session_membership.m_local_peer_index)
                {
                    m_last_host_migration_time = network_time_get();
                    m_host_migration_attempted_peer_mask |= 1 << host_peer_index;
                    host_delegate_host(host_peer_index, false, false);
                }
                else
                {
                    m_host_migration_deferred_time = network_time_get();
                }
            }
        }
    }
}

// @address 0x122f80 network_session_idle
extern "C" void network_session_idle(c_network_session *session)
{
    session->idle();
}
