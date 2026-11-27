// networkinglibxbox/network_join.obj: from the records 0x8d9c8 0x90988 0x913e0 0x92850 0x928b0 0x929e8 0x92a18 0x406d00 0x406df8 0x406fc0 0x407250 0x4073d8 0x407670 0x4079e8 0x407b58 0x407c18
// @flags /TP /O2

#include <string.h>
#include "networking/logic/logic_qos_reply_manager.h"
#include "networking/session/network_session_parameters.h"
#include "networking/session/network_session.h"
#include "networking/session/network_session_join_services.h"
#include "cseries/crt_xdk.h"
#include "networking/logic/life_cycle/life_cycle_handler_types.h"
#include "networking/online/online_nat_types.h"
#include "networking/online/online_constants.h"
#include "networking/online/managed_session_status_flags.h"
#include "cseries/cseries_types.h"
#include "cseries/cseries.h"
#include <stdlib.h>
#include "saved_games/content_catalogue_types.h"
#include "interface/user_interface.h"
#include "networking/logic/network_join.h"
#include "networking/logic/network_life_cycle.h"
#include "networking/online/online_xenon.h"
#include "networking/session/network_managed_session.h"
#include "networking/transport/transport_address.h"
#include "networking/transport/transport_security.h"
#include "interface/user_interface_networking.h"
#include "networking/messages/network_message_type_collection.h"
#include "networking/session/network_session_types.h"
#include "networking/logic/life_cycle/life_cycle_manager.h"
#include "networking/session/network_session_class.h"
#include "networking/transport/transport_security_types.h"
#include "networking/session/network_session_remote_join_types.h"
#include "networking/logic/logic_matchmaking_desirability.h"
#include "networking/transport/transport_platform_types.h"
#include "networking/logic/network_join_dependencies.h"
#include "networking/online/online_managed_session_types.h"
#include "networking/session/network_session_membership.h"
#include "networking/logic/network_join_queue_types.h"
#include "networking/logic/life_cycle/life_cycle_globals_types.h"
#include "main/levels_globals.h"
#include "interface/controller_types.h"
#include "interface/controller_inline.h"
#include "networking/messages/network_messages_session_protocol.h"
#include "networking/messages/network_message_gateway.h"
#include "networking/session/network_session_join_request_types.h"
#include "networking/session/network_session_parameter_observer.h"
#include "networking/network_configuration_types.h"
#include "networking/network_configuration.h"
#include <ppcintrinsics.h>
#include "networking/session/network_session_manager.h"
#include "networking/session/network_session_manager_globals.h"
#include "networking/network_time_types.h"

#pragma pack(push, 4)
enum e_network_session_class;
struct s_transport_session_description;
enum e_transport_platform;
struct s_matchmaking_gather_party_properties;
class c_network_session;
struct s_transport_secure_identifier;
enum e_matchmaking_desirability_result;
struct s_matchmaking_gather_party_properties;
struct s_matchmaking_search_party_properties;
#pragma pack(pop)
class c_network_session_membership;
class c_network_message_gateway;
struct s_network_session_join_request_payload;



enum e_network_join_online_title_flags { _network_join_online_title_flag_none = 0 };

static __forceinline bool network_join_session_established(const c_network_session *session)
{
  return (unsigned long)session->m_local_state >= 4 && (unsigned long)session->m_local_state < 10;
}
static __forceinline bool network_join_session_disconnected(const c_network_session *session)
{
  return session->m_local_state == 0;
}
static __forceinline void network_join_session_disconnect(c_network_session *session)
{
  if (!network_join_session_disconnected(session)) {
    session->m_disconnect_reestablish_state = 1;
    session->disconnect();
  }
}
static __forceinline bool network_join_session_all_players_present(const c_network_session *session)
{
  bool result = false;
  if (network_join_session_established(session))
    result = session->m_session_membership.m_current_membership.peers[session->m_session_membership.m_local_peer_index].connection_state == 8;
  return result;
}
// @address 0x80a6b8 ?g_network_join_data@@3Us_networking_join_data@@A
s_networking_join_data g_network_join_data = { 0, 0, 0 };
static __forceinline c_network_session *network_life_cycle_get_squad_session(void)
{
  c_network_session *result = 0;
  if (online_presence_life_cycle_globals().initialized)
    result = online_presence_life_cycle_globals().manager.m_active_squad_session;
  return result;
}
static __forceinline long game_type_get(void)
{
  return g_network_join_data.join_result;
}

// @address 0x8d9c8 network_join_is_any_join_in_progress
long network_join_is_any_join_in_progress(void)
{
  long result = 0;
  c_network_session *session;
  if (!online_presence_life_cycle_globals().initialized
      || (session = online_presence_life_cycle_globals().manager.m_active_squad_session) == 0
      || session->m_local_state == 0
      || game_type_get() != 0)
    return game_type_get();
  if (network_join_session_established(session)) {
    const c_network_session_parameters *parameters = session->get_session_parameters();
    if (session->is_leader() && parameters->m_parameters.requested_remote_join_data.get_allowed())
      return parameters->m_parameters.requested_remote_join_data.get()->join_result;
    if (parameters->m_parameters.remote_join_data.get_allowed()) {
      const s_network_session_remote_session_join_data *value = parameters->m_parameters.remote_join_data.get();
      if (value->join_type == 0 && value->join_result == 1)
        result = 1;
    }
  }
  return result;
}

// @address 0x90988 ?network_join_is_join_to_squad_in_progress@@YA_NXZ
bool network_join_is_join_to_squad_in_progress(void)
{
  c_network_session *session = 0;
  if (online_presence_life_cycle_globals().initialized)
    session = online_presence_life_cycle_globals().manager.m_target_squad_session;
  return session->m_local_state != 0 && session->m_session_type == 1;
}

extern "C" {
// @address 0x26618 k_network_join_squad_contexts
// @rdata 0x82026618..0x82026628
const s_online_context k_network_join_squad_contexts[2] = { { 0, 22 }, { 1, 3 } };
// @rdata-end
// @address 0x73a79c g_network_join_group_contexts
s_online_context g_network_join_group_contexts[2] = { { 0, 22 }, { 1, 3 } };
}
static __forceinline unsigned long network_join_time_get();
static __forceinline long network_join_time_since(unsigned long);
static __forceinline c_network_session *network_join_get_squad_session_for_join();
static __forceinline c_network_session *network_join_get_group_session_for_join();
static __forceinline c_network_session *network_join_get_target_session_for_join()
{
  c_network_session *result = 0;
  if (online_presence_life_cycle_globals().initialized) result = online_presence_life_cycle_globals().manager.m_target_squad_session;
  return result;
}
static __forceinline bool network_join_alpha_allowed()
{
  bool result = false;
  if (!g_user_interface_is_alpha || g_eula_accepted) result = true;
  return result;
}
static __forceinline bool network_join_online_title_flag(long flag)
{
  unsigned char result = (g_qos_reply_manager_globals.live_service_qos.flags.bits >> flag) & 1;
  if (result) return true;
  return false;
}
static __forceinline bool network_join_online_title_connected()
{
  bool result = network_join_online_title_flag(1) || network_join_online_title_flag(2);
  return result;
}
static __forceinline bool network_join_managed_session_created(long index)
{
  network_join_managed_session_status status;
  return managed_session_get_status(&status, index)->test(_managed_session_status_created);
}
static __forceinline void network_join_leave_squad(c_network_session *session)
{
  s_life_cycle_globals *globals = &online_presence_life_cycle_globals();
  session->leave_session();
  if (globals->manager.m_current_state != _life_cycle_state_joining)
    globals->manager.enter_joining_state();
}
static __forceinline void network_join_swap_squad_sessions()
{
  if (online_presence_life_cycle_globals().initialized) {
    c_network_session *session = online_presence_life_cycle_globals().manager.m_active_squad_session;
    online_presence_life_cycle_globals().manager.m_active_squad_session = online_presence_life_cycle_globals().manager.m_target_squad_session;
    online_presence_life_cycle_globals().manager.m_target_squad_session = session;
  }
}
static __forceinline void network_join_swap_group_sessions()
{
  if (online_presence_life_cycle_globals().initialized) {
    c_network_session *session = online_presence_life_cycle_globals().manager.m_group_session;
    online_presence_life_cycle_globals().manager.m_group_session = online_presence_life_cycle_globals().manager.m_target_squad_session;
    online_presence_life_cycle_globals().manager.m_target_squad_session = session;
  }
}

// @address 0x913e0 ?network_join_update@@YAXXZ
void network_join_update()
{
  s_networking_join_data *data = &g_network_join_data;
  c_network_session_parameters *squad_parameters = 0;
  const s_network_session_remote_session_join_data *squad_host_join_data = 0;
  const s_network_session_remote_session_join_data *squad_requested_join_data = 0;
  c_network_session_parameters *group_parameters = 0;
  const s_network_session_remote_session_join_data *group_host_join_data = 0;
  bool join_disabled = network_join_is_busy() != 0;
  data->join_disabled = join_disabled;
  c_network_session *squad_session = network_join_get_squad_session_for_join();
  c_network_session *group_session = network_join_get_group_session_for_join();
  c_network_session *target_session = network_join_get_target_session_for_join();

  if (network_join_session_established(squad_session)) {
    squad_parameters = squad_session->get_session_parameters();
    if (squad_parameters->m_parameters.remote_join_data.get_allowed())
      squad_host_join_data = squad_parameters->m_parameters.remote_join_data.get();
    if (squad_session->is_host() && squad_parameters->m_parameters.requested_remote_join_data.get_allowed())
      squad_requested_join_data = squad_parameters->m_parameters.requested_remote_join_data.get();
  }
  if (network_join_session_established(group_session)) {
    group_parameters = group_session->get_session_parameters();
    if (group_parameters->m_parameters.remote_join_data.get_allowed())
      group_host_join_data = group_parameters->m_parameters.remote_join_data.get();
  }
  bool can_start_join = data->join_status == 0
    && (!squad_host_join_data || squad_host_join_data->join_state == 0)
    && (!group_host_join_data || group_host_join_data->join_state == 0)
    && (network_join_session_disconnected(target_session) || data->request_group_disband);
  if (join_disabled || get_alpha_is_locked_down() || !network_join_alpha_allowed())
    can_start_join = false;
  if (can_start_join && online_local_xuid_is_online_enabled()) {
    if (!network_join_online_title_connected())
      can_start_join = false;
  }
  if (can_start_join) {
    target_session->m_join_refuse_reason = _network_join_refuse_reason_none;
    data->peer_join_started = false;
    data->join_start_time = 0;
    if (data->request_group_disband) {
      if (group_parameters) {
        s_network_session_remote_session_join_data new_join_data;
        memset(&new_join_data, 0, sizeof(new_join_data));
        new_join_data.join_state = 6;
        new_join_data.join_from = data->group_disband_type;
        new_join_data.join_result = 0;
        group_parameters->m_parameters.remote_join_data.set(&new_join_data);
        if (data->group_disband_type == 1) {
          if (network_join_session_established(target_session) && target_session->is_host())
            target_session->get_session_parameters()->m_parameters.remote_join_data.set(&new_join_data);
        }
        group_host_join_data = group_parameters->m_parameters.remote_join_data.get();
        data->join_start_time = network_join_time_get();
        data->group_join_result = 1;
      }
      data->request_group_disband = false;
    } else if (data->remote_join_data.nonce != 0) {
      if (!user_interface_networking_is_throttled() &&
          ::managed_session_can_create_session(data->remote_join_data.session_class))
      {
        data->join_status = 1;
        data->join_result = 1;
      }
    } else if (squad_requested_join_data && squad_requested_join_data->nonce != 0) {
      if (!user_interface_networking_is_throttled() && ::managed_session_can_create_session(squad_requested_join_data->session_class)
          && network_join_session_established(squad_session) && squad_session->m_session_membership.m_current_membership.player_count > 0) {
        if (network_join_managed_session_created(squad_session->m_managed_session_index)) {
          s_network_session_remote_session_join_data new_join_data = *squad_requested_join_data;
          new_join_data.join_state = 1;
          new_join_data.join_to = 0;
          new_join_data.join_type = 0;
          new_join_data.join_from = 0;
          new_join_data.join_result = 1;
          squad_parameters->m_parameters.remote_join_data.set(&new_join_data);
          squad_host_join_data = squad_parameters->m_parameters.remote_join_data.get();
          memset(&new_join_data, 0, sizeof(new_join_data));
          new_join_data.join_result = 1;
          squad_parameters->m_parameters.requested_remote_join_data.set(&new_join_data);
          data->join_result = 0;
        }
      }
    } else if (data->request_join_squad_to_group) {
      s_network_session_remote_session_join_data new_join_data = data->queued_group_join;
      if (!network_join_session_disconnected(squad_session) && squad_session->is_host()) {
        new_join_data.join_state = 1;
        new_join_data.join_to = 0;
        new_join_data.join_type = 1;
        new_join_data.join_from = 1;
        new_join_data.join_result = 1;
        squad_parameters->m_parameters.remote_join_data.set(&new_join_data);
        squad_host_join_data = squad_parameters->m_parameters.remote_join_data.get();
        data->group_join_result = 1;
        data->join_result = 0;
      } else {
        data->group_join_result = 0;
      }
      memset(&data->queued_group_join, 0, sizeof(data->queued_group_join));
      data->request_join_squad_to_group = false;
    } else if (data->request_join_squad_to_target_group) {
      s_network_session_remote_session_join_data new_join_data = data->queued_group_join;
      if (!network_join_session_disconnected(squad_session) && squad_session->is_host()) {
        new_join_data.join_state = 1;
        new_join_data.join_to = 0;
        new_join_data.join_type = 1;
        new_join_data.join_from = 0;
        new_join_data.join_result = 1;
        squad_parameters->m_parameters.remote_join_data.set(&new_join_data);
        squad_host_join_data = squad_parameters->m_parameters.remote_join_data.get();
        data->group_join_result = 1;
        data->join_result = 0;
      }
      memset(&data->queued_group_join, 0, sizeof(data->queued_group_join));
      data->request_join_squad_to_target_group = false;
    } else if (data->request_join_group_to_group) {
      s_network_session_remote_session_join_data new_join_data = data->queued_group_join;
      if (!network_join_session_disconnected(group_session)) {
        new_join_data.join_state = 1;
        new_join_data.join_to = 1;
        new_join_data.join_type = 1;
        new_join_data.join_from = 0;
        new_join_data.join_result = 1;
        group_parameters->m_parameters.remote_join_data.set(&new_join_data);
        group_host_join_data = group_parameters->m_parameters.remote_join_data.get();
        data->group_join_result = 1;
      }
      memset(&data->queued_group_join, 0, sizeof(data->queued_group_join));
      data->request_join_group_to_group = false;
    } else if (data->join_queue_entry_count != 0 && (data->join_queue_mode == 1 || data->join_queue_mode == 3)) {
      data->join_status = 4;
    }
  }

  if (data->join_status != 0) {
    switch (data->join_status) {
    case 1: {
      bool online = data->remote_join_data.session_class == 2;
      c_flags<e_session_creation_flags, unsigned char, 4> flags;
      network_join_session_disconnect(target_session);
      target_session->m_session_type = _network_session_type_squad;
      flags.set(_session_creation_flag_local, true);
      flags.set(_session_creation_flag_squad, online);
      target_session->join_remote_session(flags, data->remote_join_data.session_class,
        (e_transport_platform)data->remote_join_data.timestamp,
        &data->remote_join_data.identifier, &data->remote_join_data.key, &data->remote_join_data.address,
        data->remote_join_data.nonce, 0, 0, k_network_join_squad_contexts, 2, data->remote_join_data.join_as_friend);
      data->join_status = 2;
      memset(&data->remote_join_data, 0, sizeof(data->remote_join_data));
      break;
    }
    case 2:
      if (network_join_session_disconnected(target_session)) {
        data->join_result = (unsigned long)target_session->m_join_refuse_reason <= 30
          ? network_join_map_refuse_reason_to_join_result(true, 0, target_session->m_join_refuse_reason) : 2;
        data->join_status = 0;
      } else if (target_session->ready_for_remote_peers_to_join()) {
        if (network_join_session_established(squad_session))
          network_join_leave_squad(squad_session);
        else
          network_join_swap_squad_sessions();
        data->join_status = 3;
      }
      break;
    case 3:
      if (network_join_session_disconnected(target_session)) {
        data->join_result = 0;
        data->join_status = 0;
      }
      break;
    case 4:
      if (!network_join_process_joins_from_queue()) data->join_status = 0;
      break;
    }
  } else {
    c_network_session *source_session = 0;
    c_network_session_parameters *source_parameters = 0;
    const s_network_session_remote_session_join_data *active_join_data = 0;
    c_network_session *destination_session = 0;
    if (squad_host_join_data && squad_host_join_data->join_state != 0) {
      source_session = squad_session;
      source_parameters = squad_parameters;
      active_join_data = squad_host_join_data;
    } else if (group_host_join_data && group_host_join_data->join_state != 0) {
      source_session = group_session;
      source_parameters = group_parameters;
      active_join_data = group_host_join_data;
    }
    if (active_join_data) {
      long new_join_state = active_join_data->join_state;
      long new_join_result = active_join_data->join_result;
      if (active_join_data->join_from == 0)
        destination_session = target_session;
      else
        destination_session = group_session;
      switch (active_join_data->join_state) {
      case 1:
        data->peer_join_started = false;
        data->join_start_time = 0;
        if (source_session->is_host()) {
          const s_online_context *contexts;
          if (active_join_data->join_type == 0) {
            destination_session->m_session_type = _network_session_type_squad;
            contexts = k_network_join_squad_contexts;
          } else {
            destination_session->m_session_type = _network_session_type_group;
            contexts = g_network_join_group_contexts;
            const s_network_session_matchmaking_hopper *hopper = source_parameters->m_parameters.matchmaking_hopper.get();
            long game_mode = online_constant_get_game_mode_by_xlast_index(hopper->xlast_index);
            long game_type = online_constant_get_game_type(hopper->is_ranked);
            g_network_join_group_contexts[0].value = game_mode;
            g_network_join_group_contexts[1].value = game_type;
          }
          network_join_session_disconnect(destination_session);
          if (active_join_data->nonce == 0) {
            c_flags<e_session_creation_flags, unsigned char, 4> flags;
            bool squad = active_join_data->join_type == 0;
            bool group = active_join_data->join_type == 1;
            flags.set(_session_creation_flag_local, squad);
            flags.set(_session_creation_flag_squad, squad);
            flags.set(_session_creation_flag_group, group);
            flags.set(_session_creation_flag_matchmaking, group);
            destination_session->create_host_session(flags, _network_session_class_xbox_live, _transport_platform_xenon,
              &source_session->m_session_membership, contexts, 2);
            data->join_start_time = network_join_time_get();
          } else {
            c_flags<e_session_creation_flags, unsigned char, 4> flags;
            bool squad = active_join_data->join_type == 0;
            bool group = active_join_data->join_type == 1;
            flags.set(_session_creation_flag_local, squad);
            flags.set(_session_creation_flag_squad, squad);
            flags.set(_session_creation_flag_group, group);
            flags.set(_session_creation_flag_matchmaking, group);
            destination_session->join_remote_session(flags, active_join_data->session_class,
              (e_transport_platform)active_join_data->timestamp, &active_join_data->identifier,
              &active_join_data->key, &active_join_data->address, active_join_data->nonce,
              &source_session->m_session_membership,
              group ? (const s_network_session_join_request_payload *)data->join_payload : 0,
              contexts, 2, active_join_data->join_as_friend);
          }
          new_join_state = 2;
        }
        break;
      case 2:
        if (source_session->is_host()) {
          if (active_join_data->nonce != 0)
            network_join_flush_our_join_from_join_queue(&active_join_data->address, active_join_data->nonce);
          if (network_join_session_disconnected(destination_session)) {
            data->join_start_time = 0;
            new_join_result = (unsigned long)destination_session->m_join_refuse_reason <= 30
              ? network_join_map_refuse_reason_to_join_result(
                    false, (const c_network_session *)source_session, destination_session->m_join_refuse_reason)
                  : 2;
            new_join_state = 5;
          } else if (data->join_start_time && network_join_time_since(data->join_start_time) > g_network_configuration.join_host_timeout) {
            data->join_start_time = 0;
            new_join_result = network_join_map_refuse_reason_to_join_result(false,
                (const c_network_session *)source_session, _network_join_refuse_reason_aborted);
            new_join_state = 5;
          } else if (destination_session->ready_for_remote_peers_to_join()) {
            data->join_start_time = 0;
            if (active_join_data->nonce == 0) {
              e_transport_platform platform;
              s_transport_session_description description;
              if (::managed_session_get_security_information(destination_session->m_managed_session_index, false,
                      &description, &platform))
              {
                s_network_session_remote_session_join_data new_join_data = *active_join_data;
                new_join_data.identifier = description.secure_identifier;
                new_join_data.key = description.secure_key;
                new_join_data.address = description.secure_address;
                new_join_data.timestamp = platform;
                new_join_data.nonce = destination_session->m_host_join_nonce;
                new_join_data.session_class = destination_session->m_session_class;
                source_parameters->m_parameters.remote_join_data.set(&new_join_data);
              } else {
                network_join_session_disconnect(destination_session);
              }
            }
            new_join_state = 3;
          }
        }
        break;
      case 3:
        if (source_session->is_host()) {
          if (active_join_data->nonce != 0)
            network_join_flush_our_join_from_join_queue(&active_join_data->address, active_join_data->nonce);
          if (network_join_session_disconnected(destination_session)) {
            new_join_result = (unsigned long)destination_session->m_join_refuse_reason <= 30
              ? network_join_map_refuse_reason_to_join_result(
                    false, (const c_network_session *)source_session, destination_session->m_join_refuse_reason)
                  : 2;
            new_join_state = 5;
          } else if (network_join_session_all_players_present(destination_session)) {
            if (active_join_data->join_type == 0) {
              new_join_state = 4;
              data->join_start_time = network_join_time_get();
            } else {
              new_join_state = 5;
            }
          }
        } else if (!data->peer_join_started && (active_join_data->join_type == 0
            || (source_parameters && source_parameters->m_parameters.matchmaking_hopper.get_allowed()))) {
          c_flags<e_session_creation_flags, unsigned char, 4> flags;
          const s_online_context *contexts;
          if (active_join_data->join_type == 0) {
            destination_session->m_session_type = _network_session_type_squad;
            contexts = k_network_join_squad_contexts;
            flags.set(_session_creation_flag_local, true);
            flags.set(_session_creation_flag_squad, true);
          } else {
            destination_session->m_session_type = _network_session_type_group;
            contexts = g_network_join_group_contexts;
            const s_network_session_matchmaking_hopper *hopper = source_parameters->m_parameters.matchmaking_hopper.get();
            long game_mode = online_constant_get_game_mode_by_xlast_index(hopper->xlast_index);
            long game_type = online_constant_get_game_type(hopper->is_ranked);
            g_network_join_group_contexts[0].value = game_mode;
            g_network_join_group_contexts[1].value = game_type;
            flags.set(_session_creation_flag_group, true);
            flags.set(_session_creation_flag_matchmaking, true);
          }
          network_join_session_disconnect(destination_session);
          destination_session->connect_peer_to_remote_session(flags, active_join_data->session_class,
            (e_transport_platform)active_join_data->timestamp, &active_join_data->identifier,
            &active_join_data->key, &active_join_data->address, active_join_data->nonce, contexts, 2);
          data->peer_join_started = true;
        }
        break;
      case 4:
        if (source_session->is_host()) {
          if (active_join_data->nonce != 0)
            network_join_flush_our_join_from_join_queue(&active_join_data->address, active_join_data->nonce);
          if (source_session->m_session_membership.m_current_membership.peer_count == 1) {
            network_join_leave_squad(source_session);
          } else if (network_join_time_since(data->join_start_time) > g_network_configuration.join_disband_timeout) {
            data->join_start_time = 0;
            network_join_leave_squad(source_session);
          }
        } else if (!source_session->leaving_session()) {
          network_join_leave_squad(source_session);
        }
        break;
      case 5:
        if (source_session->is_host()) {
          s_network_session_remote_session_join_data new_join_data;
          memset(&new_join_data, 0, sizeof(new_join_data));
          if (active_join_data->join_type == 0) {
            new_join_data.join_result = active_join_data->join_result;
            source_parameters->m_parameters.requested_remote_join_data.set(&new_join_data);
            new_join_data.join_result = 0;
          } else {
            if (active_join_data->join_result != 1)
              data->group_join_result = active_join_data->join_result;
            else
              data->group_join_result = 0;
          }
          source_parameters->m_parameters.remote_join_data.set(&new_join_data);
        }
        new_join_state = 0;
        new_join_result = 0;
        break;
      case 6: {
        bool swap_sessions = false;
        if (active_join_data->join_from == 1) {
          if (!network_join_session_established(target_session) && network_join_session_established(group_session))
            destination_session = target_session;
          else if (network_join_session_established(target_session) && network_join_session_established(group_session))
            swap_sessions = true;
        }
        if (destination_session->is_host() && network_join_session_established(destination_session)) {
          bool host_ready = destination_session->m_session_membership.m_current_membership.peer_count == 1;
          if (!host_ready) {
            if (network_join_time_since(data->join_start_time) > g_network_configuration.join_disband_timeout)
              host_ready = true;
          }
          if (host_ready) {
            if (swap_sessions) network_join_swap_group_sessions();
            destination_session->leave_session();
          }
        } else if (network_join_session_established(destination_session)) {
          if (swap_sessions) network_join_swap_group_sessions();
          destination_session->leave_session();
        }
        if (network_join_session_disconnected(destination_session)) {
          new_join_state = 0;
          data->group_join_result = 0;
        }
        break;
      }
      }
      if (source_session->is_host() && (new_join_state != active_join_data->join_state
          || new_join_result != active_join_data->join_result)) {
        s_network_session_remote_session_join_data new_join_data = *active_join_data;
        new_join_data.join_state = new_join_state;
        new_join_data.join_result = new_join_result;
        source_parameters->m_parameters.remote_join_data.set(&new_join_data);
      }
    }
  }
  network_join_queue_update();
}

static __forceinline long network_join_read_state(s_levels_globals *globals)
{
  long state = globals->enumeration_complete.peek();
  return state;
}
static __forceinline bool network_join_globals_idle(void)
{
  s_levels_globals *globals = &network_join_levels_globals();
  return globals->enumeration_task_id != -1 && network_join_read_state(globals) == 0;
}
// @address 0x92850 network_join_is_busy
extern "C" unsigned char network_join_is_busy(void)
{
  unsigned long busy_catalogue_mask;
  unsigned char result = network_join_test_catalogues_busy(&busy_catalogue_mask);
  return result | network_join_globals_idle();
}

static __forceinline const c_storage_device *network_join_storage_device_get(unsigned long controller_index)
{
  const c_storage_device *result = 0;
  if (controller_index <= 3)
    result = &content_catalogues().storage_devices[controller_index];
  return result;
}
static __forceinline bool network_join_catalogue_busy(const c_content_catalogue *catalogue)
{
  return catalogue->m_enumeration_task.get_task_state() == 1
      || catalogue->m_enumeration_task.get_task_state() == 2
      || catalogue->m_enumeration_task.get_task_state() == 3;
}
// @address 0x928b0 network_join_test_catalogues_busy
bool network_join_test_catalogues_busy(unsigned long *mask)
{
  *mask = 0;
  for (long controller_index = 0; controller_index < 4; controller_index++) {
    if (controller_in_use(controller_get((e_controller_index)controller_index))) {
      const c_content_catalogue *catalogue = &content_catalogues().catalogues[controller_index];
      const c_storage_device *storage_device = network_join_storage_device_get(catalogue->m_controller_index);
      if (storage_device && storage_device->device_id() != 0xcacacaca) {
        unsigned char busy = network_join_catalogue_busy(catalogue);
        unsigned char idle = busy == 0;
        unsigned char active = (idle == 0) && 1;
        if (active || catalogue->last_content_index != -1)
          *mask |= 1 << controller_index;
      }
    }
  }
  return *mask != 0;
}

// @address 0x929e8 network_join_is_eula_accepted
extern "C" bool network_join_is_eula_accepted(void)
{
  bool result = false;
  if (!g_user_interface_is_alpha || g_eula_accepted)
    result = true;
  return result;
}

// @address 0x92a18 network_join_is_leaving_group
static __forceinline c_network_session *network_join_group_session_get(void)
{
  c_network_session *result = 0;
  if (online_presence_life_cycle_globals().initialized)
    result = online_presence_life_cycle_globals().manager.m_group_session;
  return result;
}
extern "C" bool network_join_is_leaving_group(void)
{
  bool result = false;
  c_network_session *session = network_join_group_session_get();
  if (g_network_join_data.join_status == 0 && network_join_session_established(session) && (session)->is_host()) {
    const c_network_session_parameters *parameters = session->get_session_parameters();
    if (parameters->m_parameters.remote_join_data.get_allowed())
      result = parameters->m_parameters.remote_join_data.get()->join_state == 6;
  }
  return result;
}

// @address 0x406d00 ?network_join_clear_squad_join_result@@YAXXZ
void network_join_clear_squad_join_result(void)
{
  c_network_session *session = network_life_cycle_get_squad_session();
  if (g_network_join_data.join_result != 0) {
    g_network_join_data.join_status = 0;
    g_network_join_data.join_result = 0;
    return;
  }
  if (network_join_session_established(session) && session->is_leader()) {
    s_network_session_remote_session_join_data join_data;
    memset(&join_data, 0, sizeof(join_data));
#ifdef HALO3_64BIT
    session->get_session_parameters()->m_parameters.requested_remote_join_data.request_change(&join_data);
#else
    (*(c_network_session_parameter_requested_remote_join_data *)(session->m_session_parameters + (0x65818 - 0x4f08))).request_change(&join_data);
#endif
  }
}


// @address 0x406df8 ?network_join_to_remote_squad@@YAE_N00W4e_network_session_class@@PBUs_transport_secure_identifier@@PBUs_transport_secure_key@@PBUs_transport_secure_address@@@Z
unsigned char network_join_to_remote_squad(bool join_directly, bool join_as_friend, bool require_leader,
    e_network_session_class session_class, const s_transport_secure_identifier *session_identifier,
    const s_transport_secure_key *session_key, const s_transport_secure_address *session_address)
{
  c_network_session *current_session;
  c_network_session_manager *manager;
  unsigned char join_started;
  s_transport_secure_identifier identifier;
  c_network_session *squad_session;
  unsigned char request_join;
  int start_join;
  s_network_session_remote_session_join_data *join_data_address;
  s_network_session_remote_session_join_data join_data;
  squad_session = 0;
  if (online_presence_life_cycle_globals().initialized) {
    current_session = online_presence_life_cycle_globals().manager.m_active_squad_session;
    if (current_session->m_local_state != _network_session_state_none)
      squad_session = current_session;
  }
  start_join = 0;
  request_join = 0;
  manager = g_network_session_manager;
  if (manager->get_session(session_identifier) == 0) {
    if (join_directly) {
      start_join = 1;
    } else if ((!require_leader || ::network_life_cycle_squad_local_peer_is_leader()) && squad_session) {
      if (squad_session->m_session_membership.m_current_membership.peer_count == 1)
        start_join = 1;
      else
        request_join = 1;
    }
  }
  join_started = ((unsigned char)start_join);
  if (join_started != 0) {
    g_network_join_data.remote_join_data.session_class = session_class;
    g_network_join_data.remote_join_data.identifier = *session_identifier;
    g_network_join_data.remote_join_data.key = *session_key;
    g_network_join_data.remote_join_data.address = *session_address;
    g_network_join_data.remote_join_data.nonce = transport_secure_nonce_generate();
    g_network_join_data.remote_join_data.timestamp = 0;
    g_network_join_data.remote_join_data.join_as_friend = join_as_friend;
  } else if (request_join != 0) {
    join_data_address = &join_data;
    memset(join_data_address, 0, sizeof(join_data));
    identifier = *session_identifier;
    join_data.session_class = session_class;
    join_data.identifier = identifier;
    join_data.key = *session_key;
    join_data.address = *session_address;
    join_data.nonce = transport_secure_nonce_generate();
    join_data.join_as_friend = join_as_friend;
    join_data.timestamp = 0;
    squad_session->get_session_parameters()->m_parameters.requested_remote_join_data.request_change(&join_data);
  }
  long result;
  if (join_started != 0 || request_join != 0)
    result = 1;
  else
    result = 0;
  return result;
}

// @address 0x406fc0 network_join_map_refuse_reason_to_join_result
extern "C" long network_join_map_refuse_reason_to_join_result(bool joining_group, const c_network_session *s, long reason)
{
  // match: 0x406fc0: value range assumption guides branch lowering
  __assume((unsigned long)reason <= 30);
  long result;
  switch (reason) {
  case _network_join_refuse_reason_tried_to_join_self:
  case 6:
  case 8:
  case _network_join_refuse_reason_address_invalid:
  case _network_join_refuse_reason_address_failed:
  case _network_join_refuse_reason_too_many_observers:
  case _network_join_refuse_reason_wrong_payload_type:
  case _network_join_refuse_reason_no_reservation:
  case _network_join_refuse_reason_player_count_zero:
  case _network_join_refuse_reason_player_add_pending:
  case _network_join_refuse_reason_player_add_failed:
  case _network_join_refuse_reason_holding_in_queue:
  result = 2;
  break;
  // match: 0x406fc0: unreachable hint omits dead branch check
  default: __assume(0);
  case _network_join_refuse_reason_could_not_connect:
    if (!joining_group && s && s->m_session_membership.m_current_membership.peer_count != 1) {
      const c_network_session_membership *members = &s->m_session_membership;
      long nat_type = _online_nat_type_open;
      for (long peer_index = ((c_network_session_membership const *)members)->get_first_peer(); peer_index != -1;
          peer_index = ((c_network_session_membership const *)members)->get_next_peer(peer_index))
      {
        long peer_nat_type = members->m_current_membership.peers[peer_index].properties.nat_type;
        nat_type = peer_nat_type > nat_type ? peer_nat_type : nat_type;
      }
      switch (nat_type) {
      case _online_nat_type_moderate: result = 14; break;
      case _online_nat_type_strict: result = 15; break;
      default: result = 13; break;
      }
    } else {
      switch (online_get_nat_type()) {
      case _online_nat_type_moderate: result = 11; break;
      case _online_nat_type_strict: result = 12; break;
      default: result = 10; break;
      }
    }
    break;
  case _network_join_refuse_reason_none:
  case _network_join_refuse_reason_join_timed_out:
  case _network_join_refuse_reason_session_booted:
  case _network_join_refuse_reason_aborted:
  case _network_join_refuse_reason_abort_ignored:
  case _network_join_refuse_reason_host_time_out: result = 7; break;
  case _network_join_refuse_reason_peer_version_too_low: result = 8; break;
  case _network_join_refuse_reason_host_version_too_low: result = 9; break;
  case 4: case _network_join_refuse_reason_session_disbanded: case _network_join_refuse_reason_rejected_by_host: result = 3; break;
  case 5: result = 5; break;
  case _network_join_refuse_reason_squad_full: result = 4; break;
  case _network_join_refuse_reason_in_matchmaking: result = 6; break;
  case _network_join_refuse_reason_player_not_online_enabled: result = 16; break;
  case _network_join_refuse_reason_alternate_host: result = 19; break;
  case _network_join_refuse_reason_still_connected: result = 17; break;
  case _network_join_refuse_reason_local_host: result = 18; break;
  }
  return result;
}

// @address 0x407250 ?network_join_queue_update@@YAXXZ
extern "C" {
// @address 0x8c447c g_network_time_globals
}
static __forceinline unsigned long network_join_time_get(void)
{
  if (g_network_time_globals.enabled)
    return g_network_time_globals.time;
  return GetTickCount();
}
static __forceinline long network_join_time_since(unsigned long time)
{
  return network_join_time_get() - time;
}
static __forceinline c_network_session *network_join_get_squad_session_for_join(void)
{
  c_network_session *result = 0;
  if (online_presence_life_cycle_globals().initialized)
    result = online_presence_life_cycle_globals().manager.m_active_squad_session;
  return result;
}
static __forceinline c_network_session *network_join_get_group_session_for_join(void)
{
  c_network_session *result = 0;
  if (online_presence_life_cycle_globals().initialized)
    result = online_presence_life_cycle_globals().manager.m_group_session;
  return result;
}
void network_join_queue_update(void)
{
  s_networking_join_data *join_data = &g_network_join_data;
  c_network_session *session = 0;
  if (join_data->join_queue_mode == 1)
    session = network_join_get_squad_session_for_join();
  else if (join_data->join_queue_mode == 3)
    session = network_join_get_group_session_for_join();

  long queue_index = 0;
  while (queue_index < join_data->join_queue_entry_count) {
    s_networking_join_queue_entry *entry = &join_data->join_queue[queue_index];
    if (network_join_time_since(entry->time_received) > g_network_configuration.join_queue_timeout) {
      if (session)
        session->acknowledge_join_request(&entry->address, _network_join_refuse_reason_host_time_out);
      network_join_remove_join_from_queue_internal(entry);
    } else if (entry->time_last_acknowledged != -1
        && network_join_time_since(entry->time_last_acknowledged) <= g_network_configuration.join_queue_acknowledge_period) {
      queue_index++;
    } else {
      if (session)
        session->acknowledge_join_request(&entry->address, _network_join_refuse_reason_holding_in_queue);
      entry->time_last_acknowledged = network_join_time_get();
      queue_index++;
    }
  }
}

// Local copy of c_network_session::acknowledge_join_request: the image inlines the
// refuse-send body at the 3 call sites below while keeping out-of-line calls at the other
// sites in this file, and the linker does not reproduce that selective inline from calls.
static __forceinline void network_join_acknowledge_join_request(c_network_session *session, const transport_address *address, long reason)
{
  s_network_message_join_refuse refuse;
  memset(&refuse, 0, sizeof(refuse));
  managed_session_get_id(session->m_managed_session_index, &refuse.session_id);
  refuse.reason = (e_network_join_refuse_reason)reason;
  c_network_message_gateway *gateway = session->m_message_gateway;
  gateway->send_message_directed(address, _network_message_type_join_refuse, sizeof(refuse), &refuse);
}
static __forceinline bool network_join_get_matchmaking_gather_party_properties(s_matchmaking_gather_party_properties *properties)
{
  bool result = false;
  if (online_presence_life_cycle_globals().initialized) {
    switch (online_presence_life_cycle_globals().manager.m_current_state) {
    case _life_cycle_state_matchmaking_find_and_assemble_match:
    result = (&online_presence_life_cycle_globals().find_and_gather)->get_gather_party_properties(properties);
    break;
    case _life_cycle_state_matchmaking_assemble_match:
    result = (&online_presence_life_cycle_globals().assemble)->get_gather_party_properties(properties);
    break;
    }
  }
  return result;
}
// @address 0x407670 ?network_join_add_join_to_queue@@YAXPAVc_network_session@@PBUtransport_address@@PBUs_network_session_join_request@@@Z
struct s_network_session_host_owner;
void network_join_add_join_to_queue(c_network_session *session, const transport_address *address, const s_network_session_join_request *join_request)
{
  s_networking_join_data *data = &g_network_join_data;
  long entry_count = data->join_queue_entry_count;
  long refuse_reason = network_session_get_join_refuse_reason((s_network_session_host_owner *)session);
  if (refuse_reason == _network_join_refuse_reason_none) {
    switch (data->join_queue_mode) {
    case 0:
      refuse_reason = 6;
      break;
    case 1:
      if (session->m_session_type != 1)
        refuse_reason = _network_join_refuse_reason_in_matchmaking;
      break;
    case 2:
    case 3:
      if (session->m_session_type != 2)
        refuse_reason = _network_join_refuse_reason_in_matchmaking;
      break;
    case 4:
      refuse_reason = _network_join_refuse_reason_in_matchmaking;
      break;
    }
  }
  if (refuse_reason == _network_join_refuse_reason_none) {
    const c_network_session_membership *membership = &session->m_session_membership;
    bool already_joined = false;
    for (long peer_index = ((c_network_session_membership const *)membership)->get_first_peer(); peer_index != -1;
        peer_index = ((c_network_session_membership const *)membership)->get_next_peer(peer_index))
    {
      if (join_request->join_nonce == membership->m_current_membership.peers[peer_index].join_nonce) {
        already_joined = true;
        break;
      }
    }
    if (!already_joined) {
      for (long entry_index = 0; entry_index < entry_count; entry_index++) {
        if (join_request->join_nonce == data->join_queue[entry_index].join_nonce) {
          already_joined = true;
          break;
        }
      }
    }
    if (!already_joined) {
      s_networking_join_queue_entry *entry = &data->join_queue[entry_count];
      memset(entry, 0, sizeof(s_networking_join_queue_entry));
      entry->address = *address;
      memcpy(&entry->join_nonce, join_request, sizeof(*join_request));
      entry->time_received = network_join_time_get();
      entry->time_last_acknowledged = -1;
      data->join_queue_entry_count++;
      data->membership_peer_count = -1;
      if (data->join_queue_entry_count == 32) {
        long worst_desirability = -1;
        unsigned long oldest_time = -1;
        s_networking_join_queue_entry *entry_to_delete = 0;
        if (data->join_queue_mode != 1)
          network_join_calculate_full_queue_desirability(membership);
        for (unsigned long entry_index = 0; entry_index < 32; entry_index++) {
          s_networking_join_queue_entry *queued = &data->join_queue[entry_index];
          if (queued->desirability > worst_desirability) {
            entry_to_delete = queued;
            worst_desirability = queued->desirability;
            oldest_time = queued->time_received;
          } else if (queued->desirability == worst_desirability && oldest_time > queued->time_received) {
            entry_to_delete = queued;
            oldest_time = queued->time_received;
          }
        }
        network_join_acknowledge_join_request(session, &entry_to_delete->address, _network_join_refuse_reason_rejected_by_host);
        network_join_remove_join_from_queue_internal(entry_to_delete);
      }
    }
  }
  else
    network_join_acknowledge_join_request(session, address, refuse_reason);
}

// @address 0x4079e8 network_join_calculate_full_queue_desirability
extern "C" void network_join_calculate_full_queue_desirability(const c_network_session_membership *membership)
{
  s_networking_join_data *data = &g_network_join_data;
  c_network_session *group = network_join_get_group_session_for_join();
  s_matchmaking_gather_party_properties properties;
  bool valid = network_join_get_matchmaking_gather_party_properties(&properties);
  long index = 0;
  while (index < data->join_queue_entry_count) {
    s_networking_join_queue_entry *entry = &data->join_queue[index];
    long score = 0;
    long result;
    if (!valid)
      result = 1;
    else
      result = ::network_matchmaking_compute_session_desirability(&properties,
          &entry->search_properties,
          (long *)(&score));
    if (result == 0) {
      network_join_acknowledge_join_request(group, &entry->address, _network_join_refuse_reason_squad_full);
      network_join_remove_join_from_queue_internal(entry);
    } else {
      if (result == 1)
        entry->desirability = -1;
      else if (result == 2)
        entry->desirability = score;
      index++;
    }
  }
  data->membership_peer_count = membership->m_current_membership.peer_count;
}

// @address 0x407b58 ?network_join_flush_join_queue@@YAXXZ
void network_join_flush_join_queue(void)
{
  s_networking_join_data *join_data = &g_network_join_data;
  if (join_data->join_queue_mode != 0 && join_data->join_queue_mode != 4) {
    c_network_session *session = 0;
    if (join_data->join_queue_mode == 1)
      session = network_join_get_squad_session_for_join();
    else if (join_data->join_queue_mode == 3)
      session = network_join_get_group_session_for_join();
    if (session) {
      for (long queue_index = 0; queue_index < join_data->join_queue_entry_count; queue_index++)
        session->acknowledge_join_request(&join_data->join_queue[0].address, _network_join_refuse_reason_session_disbanded);
    }
    join_data->join_queue_entry_count = 0;
  }
}

// @address 0x407c18 ?network_join_flush_our_join_from_join_queue@@YAXPBUs_transport_secure_address@@_K@Z
void network_join_flush_our_join_from_join_queue(const s_transport_secure_address *our_address, unsigned __int64 our_join_nonce)
{
  s_networking_join_data *join_data = &g_network_join_data;
  if (join_data->join_queue_mode != 0 && join_data->join_queue_mode != 4) {
    c_network_session *session = 0;
    if (join_data->join_queue_mode == 1)
      session = network_join_get_squad_session_for_join();
    else if (join_data->join_queue_mode == 3)
      session = network_join_get_group_session_for_join();
    if (session) {
      for (long queue_index = 0; queue_index < join_data->join_queue_entry_count; queue_index++) {
        s_networking_join_queue_entry *entry = &join_data->join_queue[0];
        for (long peer_index = 0; peer_index < entry->joining_peer_count; peer_index++) {
          if (transport_secure_address_compare(&entry->joining_peers[peer_index].address, our_address)) {
            if (our_join_nonce > entry->join_nonce) {
              session->acknowledge_join_request(&entry->address, _network_join_refuse_reason_session_booted);
              network_join_remove_join_from_queue_internal(entry);
            }
            return;
          }
        }
      }
    }
  }
}

static __forceinline bool network_join_process_session_transition(long state)
{
  return state == 8 || state == 7 || state == 9;
}
// @address 0x4073d8 ?network_join_process_joins_from_queue@@YA_NXZ
bool network_join_process_joins_from_queue()
{
  bool join_in_progress = false;
  s_networking_join_data *join_data = &g_network_join_data;
  c_network_session *session = 0;
  s_networking_join_queue_entry *chosen = 0;
  bool observer_ready = false;
  if (join_data->join_queue_mode == 1)
    session = network_join_get_squad_session_for_join();
  else if (join_data->join_queue_mode == 3)
    session = network_join_get_group_session_for_join();
  if (online_presence_life_cycle_globals().initialized && online_presence_life_cycle_globals().manager.get_observer())
    observer_ready = online_presence_life_cycle_globals().manager.get_observer()->m_stream_expansion_state == 0;
  if (join_data->join_queue_entry_count != 0 && session && (!network_join_session_established(session) || !session->is_host())) {
    network_join_flush_join_queue();
  } else if (session) {
    if (network_join_session_established(session) && session->is_host() && !network_join_process_session_transition(session->m_local_state)) {
      const c_network_session *join_session = (const c_network_session *)session;
      const c_network_session_membership *membership = &join_session->m_session_membership;
      join_in_progress = session->is_peer_joining_this_session();
      bool ready = !join_in_progress && observer_ready && join_data->join_queue_entry_count != 0;
      if (ready && join_data->join_queue_mode == 3 && membership->m_current_membership.peer_count != join_data->membership_peer_count) {
        network_join_calculate_full_queue_desirability(membership);
        if (!join_data->join_queue_entry_count)
          ready = false;
      }
      if (ready) {
        if (join_data->join_queue_mode == 3) {
          long best_score = -1;
          for (long i = 0; i < join_data->join_queue_entry_count; ++i) {
            s_networking_join_queue_entry *entry = &join_data->join_queue[i];
            if (entry->desirability > best_score) { chosen = entry; best_score = entry->desirability; }
          }
        } else {
          chosen = &join_data->join_queue[0];
        }
        if (chosen) {
          session->join_accept((const s_network_session_join_request *)&chosen->join_nonce, &chosen->address);
          network_join_remove_join_from_queue_internal(chosen);
          join_in_progress = true;
        }
      }
    }
  }
  return join_in_progress;
}
