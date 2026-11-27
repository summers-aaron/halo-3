// interfacelibxbox/user_interface_session.obj: from the records 0x8c7e8 0x952d8 0x211560 0x24a688 0x3a1660 0x3a2818 0x3b10e0 0x3b1ff8 0x3b5178 0x3b5238 0x3b52f8 0x46b1e0 0x46b2b8 0x46b330 0x46b3b8 0x46b578 0x46b698 0x46b750 0x46b7d8 0x46b830 0x46b890 0x46b8e8 0x46ba10 0x46bc58 0x46bd20 0x46be08 0x46beb0 0x46c010 0x46c0c0 0x46c170 0x46c1f8 0x46c298 0x46c2f8 0x46c3b8 0x46c470 0x46c5e0 0x46c698 0x46c800 0x46c890 0x46c928 0x46c9d0 0x46cb28 0x46cbe0 0x46cd08 0x46cd60 0x46cf38 0x46cf88 0x46d040 0x46d168 0x46d358 0x46d3a8 0x46d590 0x46d680 0x46d740 0x46dad8 0x46db58 0x46dbb8 0x46dc18 0x46dc78 0x46dd68 0x46ddd8 0x46de58 0x46ded8 0x46e080 0x478e48 0x491158 0x4f32f8
// @compiler xdk6130
// @flags /TP /O2 /Oc
#include "interface/user_interface_session_dependencies.h"
#include "networking/online/online_cached_user.h"
#include "game/campaign_metagame_player_types.h"
#include "networking/logic/network_search_types.h"
#include "networking/online/online_cached_user_globals.h"
#include "game/campaign_metagame_globals.h"
#include "game/game_engine_variant_lookup.h"
#include "interface/user_interface_squad_session.h"
#include "cseries/cseries_unicode.h"
#include "text/text_group.h"
#include "memory/data_types.h"
#include "networking/logic/squad_join_permission_types.h"
#include "interface/gui_game_mode_types.h"
#include "interface/session_game_mode_types.h"
#include "networking/session/network_game_countdown_delayed_reason_types.h"
#include "memory/tls_globals.h"
#include "memory/data.h"
#include "memory/static_arrays.h"
#include "shell/shell.h"
#include "game/game_types.h"
#include "game/player_types.h"
#include "game/map_variant.h"
#include <string.h>
#include <wchar.h>
#include <stdlib.h>
#include "interface/user_interface.h"
#include "interface/user_interface_session.h"
#include "networking/session/network_game_start_mode_types.h"
#include "networking/logic/network_session_interface.h"
#include "text/unicode.h"
#include "cseries/async_xoverlapped.h"
#include "interface/user_interface_networking.h"
#include "networking/logic/network_life_cycle.h"
#include "networking/online/online_xenon.h"
#include "main/levels.h"
#include "saved_games/saved_film.h"
#include "interface/gui_screens/carnage_report/gui_screen_carnage_report.h"
#include "interface/gui_screens/scoreboard/gui_screen_scoreboard_types.h"
#include "saved_games/content_item.h"
#include "networking/network_configuration_types.h"
#include "interface/user_interface_shared_globals.h"
#include "cseries/cseries.h"
#include "cseries/language.h"
#include "networking/online/online_guide_xenon.h"
#include "interface/gui_screens/player_select/gui_player_select_screen_widget.h"
#include "interface/user_interface_error_manager.h"
#include "interface/controller_types.h"
#include "networking/network_time_types.h"
#include "networking/session/network_session_parameters_generic.h"
#include "networking/session/network_session_types.h"
#include "networking/session/network_session_class.h"
#include "input/input.h"
#include "input/input_globals.h"
#include "cseries/maximum.h"
#include "networking/network_time_access.h"
#include "game/player_identifier_types.h"
#include "interface/online_leaderboard_id_types.h"
#include "main/player_profile_summary_types.h"
#include "networking/logic/network_leaderboard.h"
#include "networking/network_configuration.h"
#include "networking/network_globals.h"
#include "cseries/crt_xdk.h"
#include "networking/network_build_identifiers.h"
#include "networking/session/network_game_simulation_protocol_types.h"
#include "networking/session/network_session_parameters.h"
#include "networking/session/network_session.h"
#include "networking/logic/life_cycle/life_cycle_globals_types.h"
#include "networking/logic/network_session_interface_globals.h"
#include "networking/network_globals_types.h"
#include "networking/session/network_session_membership.h"
#include "networking/logic/network_leaderboard_globals_types.h"
#include "saved_games/saved_film_description.h"
#include "networking/session/network_session_interface_types.h"
#include "networking/logic/network_session_game_setup.h"
#include "interface/user_interface_controller.h"
#include "interface/c_player_profile.h"
#include "interface/user_interface_content_globals.h"
#include "game/game_engine_base_variant.h"
#include "interface/gui_screen_pregame_lobby_mapeditor_types.h"
#include "interface/gui_selected_items_game_variant.h"
#include "interface/controller_inline.h"

enum e_network_game_privacy;
struct s_saved_film_description;
enum e_session_game_start_status;
enum e_session_game_start_error;
class c_network_session_parameters;
class c_network_session;
struct utf8;
struct s_network_globals;
struct s_network_session_host_owner;

// @address 0x3a1660 ?leaving_session@c_network_session@@QBA_NXZ
// @address 0x3b1ff8 ?user_interface_squad_get_join_permission@@YA?AW4e_squad_join_permission@@XZ
// @address 0x3a2818 user_interface_leave_squad
// @address 0x46cb28 ?user_interface_get_session_game_mode@@YA?AW4e_session_game_mode@@XZ
// @address 0x8c7e8 ?user_interface_squad_exists@@YA_NXZ
// @address 0x3b5178 ?user_interface_game_settings_get_game_variant@@YAPBVc_game_variant@@XZ
// @address 0x211560 user_interface_session_get_player_profile_summary
// @address 0x478e48 ?user_interface_create_new_squad@@YAXW4e_network_session_class@@@Z


static __forceinline int game_mode(void)
{
  int mode = 0;
  if (network_life_cycle_globals_get()->initialized)
    mode = network_life_cycle_globals_get()->manager.m_current_state;
  return mode;
}

bool user_interface_squad_exists(void)
{
  bool result = false;
  c_network_session *session = 0;
  if (user_interface_squad_try_and_get_session((s_network_session **)&session))
    result = session && !(session)->leaving_session();
  return result;
}

// @address 0x211560 user_interface_session_get_player_profile_summary

bool user_interface_session_get_player_profile_summary(e_controller_index controller_index,
    s_player_profile_summary *out)
{
  bool result = false;
  do
  {
    long player_index = controller_index != _controller_none
        ? ::user_interface_session_get_player_at_local_controller(controller_index) : -1;
    if (player_index != -1)
    {
      const s_player_configuration *player_data = user_interface_session_get_player_data(player_index);
      if (player_data && player_data->host.global_stats_valid)
      {
        bool skill_available = false;
        int user_index = g_controller_globals.controllers[controller_index].user_index;
        if ((unsigned int)user_index <= 3)
        {
          s_leaderboard_user *user = &network_leaderboard_globals.users[user_index];
          if (user->valid)
          {
            long state = user->highest_skill_valid;
            if (state == 1 || state == 2)
              skill_available = true;
          }
        }
        if (skill_available)
        {
          if (out != 0)
          {
            // match: 0x211560: lets the five player_data loads issue before the stores to out; without it each lwz waits on the previous stw
            s_player_profile_summary *__restrict o = out;
            o->global_rank = player_data->host.global_rank;
            o->global_grade = player_data->host.global_grade;
            o->highest_skill = player_data->client.global_statistics.highest_skill;
            o->games_completed = player_data->client.displayed_statistics.unranked_completed + player_data->client.displayed_statistics.ranked_completed;
            o->experience_base = player_data->client.global_statistics.experience_base;
          }
          result = true;
          break;
        }
      }
    }
    out->global_rank = 0;
    out->global_grade = 0;
    out->games_completed = 0;
    out->experience_base = 0;
    out->highest_skill = -1;
  } while (false);
  return result;
}

inline bool c_network_session::leaving_session() const
{
  bool leaving = false;
  switch (current_local_state())
  {
  case _network_session_state_peer_join_abort:
  case _network_session_state_peer_leaving:
  case _network_session_state_host_disband:
    leaving = true;
    break;
  case _network_session_state_host_handoff:
    leaving = m_local_state_data.host_handoff.host_leaving;
    break;
  case _network_session_state_host_reestablish:
    leaving = m_local_state_data.host_reestablish.reestablish_host_leaving;
    break;
  }
  return leaving;
}

// @address 0x24a688 ?established@c_network_session@@QBA_NXZ

extern "C" void user_interface_leave_squad(unsigned char leave_reason)
{
    unsigned char leave_data = leave_reason;
    if (network_life_cycle_globals_get()->manager.m_current_state != _life_cycle_state_leaving) {
        network_life_cycle_globals_get()->manager.m_entry_data_size = sizeof(leave_data);
        network_life_cycle_globals_get()->manager.m_pending_state_change = 1;
        network_life_cycle_globals_get()->manager.m_pending_state = _life_cycle_state_leaving;
        memset(&network_life_cycle_globals_get()->manager.m_entry_data, 0,
            sizeof(network_life_cycle_globals_get()->manager.m_entry_data));
        if (network_life_cycle_globals_get()->manager.m_entry_data_size > 0) {
            (char *)memcpy((&network_life_cycle_globals_get()->manager.m_entry_data),
        (char *)&leave_data,
        network_life_cycle_globals_get()->manager.m_entry_data_size);
        }
    }
    return;
}

static __forceinline bool user_interface_session_in_squad_session(s_network_session **session)
{
  bool result = false;
  if (network_life_cycle_globals_get()->initialized) {
    s_network_session *current = (s_network_session *)network_life_cycle_globals_get()->manager.m_active_squad_session;
    if (!((c_network_session *)current)->disconnected()) {
      if (session)
        *session = current;
      result = true;
    }
  }
  return result;
}

// @address 0x3b10e0 ?user_interface_squad_set_start_mode@@YA_NW4e_network_game_start_mode@@@Z
bool user_interface_squad_set_start_mode(enum e_network_game_start_mode value)
{
  bool result = false;
  if (network_squad_session_can_set_game_settings()) {
    s_network_session *session = 0;
    user_interface_session_in_squad_session(&session);
    result = parameter_request_change(&((c_network_session *)session)->get_session_parameters()->m_parameters.start_mode,
        (const long *)&value);
  }
  return result;
}

e_squad_join_permission user_interface_squad_get_join_permission(void)
{
    if (user_interface_networking_get_closed())
        return _squad_join_permission_closed;
    long privacy_mode = ((long)::network_squad_session_get_privacy_mode());
    if (privacy_mode == 2)
        return _squad_join_permission_invite;
    return privacy_mode == 1 ? _squad_join_permission_friends : _squad_join_permission_open;
}

const c_game_variant *user_interface_game_settings_get_game_variant(void)
{
  c_network_session *session = 0;
  const c_game_variant *game_variant = 0;
  if (network_life_cycle_in_session(&session) && session->established()) {
    const c_network_session_parameters *parameters = session->get_session_parameters();
    if (parameters->m_parameters.game_variant.get_allowed())
      game_variant = parameters->m_parameters.game_variant.get();
  }
  return game_variant;
}

static __forceinline bool user_interface_session_film_ready(const c_network_session *s)
{
  return (unsigned long)s->current_local_state() >= 4 && (unsigned long)s->current_local_state() < 10;
}

// @address 0x3b5238 ?user_interface_game_settings_get_film@@YAPBUs_saved_film_description@@XZ
struct s_saved_film_description const * user_interface_game_settings_get_film(void)
{
  char *result = 0;
  c_network_session *session = 0;
  if (network_life_cycle_in_session(&session)) {
    bool ready = user_interface_session_film_ready(session);
    result = 0;
    if (ready) {
      c_network_session_parameters *p = session->get_session_parameters();
      if (p->m_parameters.saved_film.get_allowed()) {
        c_generic_network_session_parameter<s_network_session_parameter_saved_film> *q = &p->m_parameters.saved_film;
        if (q->get_allowed())
          result = (char *)&q->m_data;
      }
    }
  }
  return (s_saved_film_description const *)result;
}

// @address 0x3b52f8 user_interface_game_settings_try_and_get_map_variant
char *user_interface_game_settings_try_and_get_map_variant(void)
{
  char *result = 0;
  c_network_session *session = 0;
  if (network_life_cycle_in_session(&session)) {
    bool ready = user_interface_session_film_ready(session);
    result = 0;
    if (ready) {
      c_network_session_parameters *p = session->get_session_parameters();
      if (p->m_parameters.map_variant.get_allowed()) {
        c_network_session_parameter_map_variant *q = &p->m_parameters.map_variant;
        if ((q->get_allowed()) && q->m_data.valid)
          result = (char *)&q->m_data.map_variant;
      }
    }
  }
  return result;
}

// @address 0x46b2b8 user_interface_group_exists
bool user_interface_group_exists(void)
{
  bool result = false;
  c_network_session *session = 0;
  if (user_interface_group_try_and_get_session((s_network_session **)&session))
    result = session && !(session)->leaving_session();
  return result;
}

static __forceinline long user_interface_session_squad_get_open_player_slot_count(void)
{
  long player_count = 0;
  long maximum_player_count = network_squad_session_get_maximum_player_count();
  network_squad_session_get_membership(0, 0, 0, 0, 0, 0, 0, &player_count, 0, 0);
  return maximum_player_count - player_count;
}
e_network_join_refuse_reason user_interface_squad_is_joinable(void)
{
  e_network_join_refuse_reason reason = network_squad_session_is_joinable();
  if (reason == _network_join_refuse_reason_none && user_interface_session_squad_get_open_player_slot_count() == 0)
    reason = _network_join_refuse_reason_squad_full;
  return reason;
}

// @address 0x46b3b8 user_interface_remote_squad_is_joinable
bool user_interface_remote_squad_is_joinable(const s_network_squad_status_data *squad)
{
  bool result = false;
  if (squad) {
    short slot_count = squad->details.slot_count_d + squad->details.slot_count_c + squad->details.slot_count_b + squad->details.slot_count_a;
    long type = squad->type;
    long build = squad->build;
    long version = squad->version;
    bool compatible = network_build_is_compatible(type, version, build);
    result = true;
    if (!compatible) {
      result = false;
    } else {
      long maximum_players = slot_count;
      if (maximum_players > 16)
        maximum_players = 16;
      if (squad->details.player_count >= maximum_players || squad->state != 1 ||
          (squad->details.kind == 1 && squad->details.variant != 2)) {
        result = false;
      } else {
        long join_mode = squad->details.join_mode;
        if (join_mode != 0) {
        if (join_mode == 1) {
          result = false;
          for (long player_index = 0; player_index < squad->details.player_count; player_index++) {
            unsigned __int64 user_id = squad->details.players[player_index].configuration.client.xuid;
            if (squad->details.players[player_index].configuration.client.silver) {
              for (long controller_index = 0; controller_index < 4; controller_index++) {
                if (g_online_cached_users[controller_index].signed_in &&
                    online_user_is_friend((e_controller_index)controller_index, user_id)) {
                  result = true;
                  break;
                }
              }
            }
          }
        } else if (join_mode == 2) {
          result = false;
        } else if (join_mode == 3) {
          result = false;
        }
        }
      }
    }
  }
  return result;
}

bool user_interface_squad_is_booting_allowed(void)
{
  bool result = false;
  if (network_globals.initialized && ::network_life_cycle_squad_local_peer_is_leader()) {
    long state = 0;
    if (network_life_cycle_globals_get()->initialized)
      state = network_life_cycle_globals_get()->manager.m_current_state;
    switch (state - 1) {
    case _life_cycle_state_pre_game - 1:
    case _life_cycle_state_in_game - 1:
    case _life_cycle_state_matchmaking_find_match_client - 1:
    case _life_cycle_state_matchmaking_find_match - 1:
    case _life_cycle_state_matchmaking_find_and_assemble_match - 1:
    case _life_cycle_state_matchmaking_assemble_match - 1:
      result = true;
      break;
    }
  }
  return result;
}

// @address 0x46b750 user_interface_squad_get_join_mode
long user_interface_squad_get_join_mode(void)
{
  switch (user_interface_squad_get_join_permission()) {
  case _squad_join_permission_open:
    return 0;
  case _squad_join_permission_friends:
    return 1;
  case _squad_join_permission_invite:
    return 2;
  case _squad_join_permission_closed:
    return 3;
  default:
    // match: 0x46b750: without it a cmplwi 3 / bgt range check guards the join-permission jump table
    __assume(0);
  }
}

long user_interface_squad_get_countdown_timer(void)
{
    long status = ((long)::user_interface_get_session_game_start_status(0, (unsigned long *)(0)));
    long countdown_timer = -1;
    if (status == 6)
        countdown_timer = network_squad_session_get_countdown_timer();
    return countdown_timer;
}

long user_interface_squad_get_player_index(const s_player_identifier *identifier)
{
  long result = -1;
  s_network_session *session = 0;
  if (user_interface_squad_try_and_get_session(&session))
    result = ::user_interface_session_get_player_index(
        identifier, (const c_network_session *)session);
  return result;
}

static __forceinline bool user_interface_session_membership_player_is_valid(const c_network_session_membership *membership,
        long player_index)
{
  return (membership->m_current_membership.valid_player_mask.get_bits_direct()[player_index >> 5] & (1 << (player_index & 31))) != 0;
}

long user_interface_squad_get_player_count(void)
{
  long player_count = 0;
  network_squad_session_get_membership(0, 0, 0, 0, 0, 0, 0, &player_count, 0, 0);
  return player_count;
}

long user_interface_squad_get_local_player_count(void)
{
  long local_player_count = 0;
  long local_peer_index;
  unsigned long player_valid_mask;
  const s_network_session_player *players;
  if (network_squad_session_get_membership(0, &local_peer_index, 0, 0, 0, 0, 0, 0, &player_valid_mask, &players)) {
    for (long player_index = 0; player_index < 16; player_index++) {
      if ((player_valid_mask & (1 << player_index)) && players[player_index].peer_index == local_peer_index)
        local_player_count++;
    }
  }
  return local_player_count;
}

static __forceinline bool user_interface_session_xuid_is_guest_account(unsigned __int64 xuid)
{
  unsigned long flags = (unsigned long)(xuid >> 48);
  return (flags & 15) == 9 && (flags & 192) != 0;
}
long user_interface_squad_get_leader_player_index(void)
{
  long leader_player_index = -1;
  long lowest_controller_index = -1;
  long leader_peer_index = -1;
  const s_network_session_player *players = 0;
  unsigned long player_valid_mask = 0;
  if (network_squad_session_get_membership(0, 0, 0, &leader_peer_index, 0, 0, 0, 0, &player_valid_mask, &players)) {
    for (long player_index = 0; player_index < 16; player_index++) {
      if ((player_valid_mask & (1 << player_index)) && players[player_index].peer_index == leader_peer_index &&
          (lowest_controller_index == -1 || players[player_index].controller_index < lowest_controller_index) &&
          !user_interface_session_xuid_is_guest_account(players[player_index].configuration.client.xuid)) {
        leader_player_index = player_index;
        lowest_controller_index = players[player_index].controller_index;
      }
    }
  }
  return leader_player_index;
}

long user_interface_squad_get_player_at_local_controller(e_controller_index controller_index)
{
  long player_index = -1;
  long local_peer_index;
  long peer_count;
  unsigned long peer_valid_mask;
  const s_network_session_peer *peers;
  long player_count;
  unsigned long player_valid_mask;
  const s_network_session_player *players;
  if (network_squad_session_get_membership(0,
        &local_peer_index,
        0,
        0,
        &peer_count,
        &peer_valid_mask,
        &peers,
        &player_count,
        &player_valid_mask,
        &players)) {
    for (long user_index = 0; user_index < 4; user_index++) {
      long peer_player_index = peers[local_peer_index].user_player_indices[user_index];
      if (peer_player_index != -1 && players[peer_player_index].controller_index == controller_index) {
        player_index = peer_player_index;
        break;
      }
    }
  }
  return player_index;
}

// @address 0x46bd20 ?user_interface_session_get_player_at_local_controller@@YAJW4e_controller_index@@@Z
long user_interface_session_get_player_at_local_controller(enum e_controller_index controller_index)
{
  long player_index = -1;
  c_network_session *session = 0;
  if (user_interface_session_get_usable_player_xuid(&session)) {
    long local_peer_index;
    long peer_count;
    unsigned long peer_valid_mask;
    const s_network_session_peer *peers;
    long player_count;
    unsigned long player_valid_mask;
    const s_network_session_player *players;
    if (network_session_get_membership((const s_network_session *)session, 0, &local_peer_index, 0, 0, &peer_count,
        &peer_valid_mask,
            &peers, &player_count, &player_valid_mask, &players))
    {
      for (long user_index = 0; user_index < 4; user_index++) {
        long peer_player_index = peers[local_peer_index].user_player_indices[user_index];
        if (peer_player_index != -1 && players[peer_player_index].controller_index == controller_index) {
          player_index = peer_player_index;
          break;
        }
      }
    }
  }
  return player_index;
}

e_controller_index user_interface_squad_get_controller_index(long player_index)
{
  unsigned long absolute_index = player_index & 0xffff;
  e_controller_index controller_index = _controller_none;
  unsigned long player_valid_mask;
  const s_network_session_player *players;
  if (network_squad_session_get_membership(0,
        0,
        0,
        0,
        0,
        0,
        0,
        0,
        &player_valid_mask,
        &players) && (player_valid_mask & (1 << absolute_index)))
    controller_index = (e_controller_index)players[absolute_index].controller_index;
  return controller_index;
}

bool user_interface_squad_is_local_player(long player_index)
{
  bool result = false;
  long local_peer_index;
  unsigned long player_valid_mask;
  const s_network_session_player *players;
  if (network_squad_session_get_membership(0, &local_peer_index, 0, 0, 0, 0, 0, 0, &player_valid_mask, &players) &&
      (player_valid_mask & (1 << player_index)) && players[player_index].peer_index == local_peer_index)
    result = true;
  return result;
}

bool user_interface_group_is_local_player(long player_index)
{
  bool result = false;
  long local_peer_index;
  unsigned long player_valid_mask;
  const s_network_session_player *players;
  if (network_group_session_get_membership(0, &local_peer_index, 0, 0, 0, 0, 0, 0, &player_valid_mask, &players) &&
      (player_valid_mask & (1 << player_index)) && players[player_index].peer_index == local_peer_index)
    result = true;
  return result;
}

// @address 0x46c170 user_interface_session_is_local_peer_player
extern "C" bool user_interface_session_is_local_peer_player(long player_index)
{
  bool result = false;
  c_network_session *session = 0;
  if (user_interface_session_get_usable_player_xuid(&session)) {
    const c_network_session_membership *membership = session->get_session_membership();
    if (user_interface_session_membership_player_is_valid(membership, player_index) &&
        membership->m_local_peer_index == membership->m_current_membership.players[player_index].peer_index)
      result = true;
  }
  return result;
}

// @address 0x46c1f8 user_interface_session_is_silver_or_gold_live
extern "C" __int64 user_interface_session_is_silver_or_gold_live(long player_index)
{
  __int64 result = -1;
  c_network_session *session = 0;
  if (user_interface_session_get_usable_player_xuid(&session)) {
    const c_network_session_membership *membership = session->get_session_membership();
    if (user_interface_session_membership_player_is_valid(membership, player_index))
      result = membership->m_current_membership.peers[membership->m_current_membership.players[player_index].peer_index].party_nonce;
  }
  return result;
}

// @address 0x46c298 user_interface_group_get_player_index
long user_interface_group_get_player_index(const s_player_identifier *identifier)
{
  long result = -1;
  s_network_session *session = 0;
  if (user_interface_group_try_and_get_session(&session))
    result = ::user_interface_session_get_player_index(
        identifier, (const c_network_session *)session);
  return result;
}

bool user_interface_squad_is_player_valid(long player_index)
{
  bool valid = false;
  if (player_index != -1) {
    unsigned long player_valid_mask = 0;
    if (network_squad_session_get_membership(0, 0, 0, 0, 0, 0, 0, 0, &player_valid_mask, 0)) {
      bool in_range = player_index >= 0 && player_index < 16;
      valid = in_range && (player_valid_mask & (1 << player_index)) != 0;
    }
  }
  return valid;
}

bool user_interface_group_is_player_valid(long player_index)
{
  bool valid = false;
  unsigned long player_valid_mask;
  if (player_index != -1 && network_group_session_get_membership(0, 0, 0, 0, 0, 0, 0, 0, &player_valid_mask, 0)) {
    bool in_range = player_index >= 0 && player_index < 16;
    valid = in_range && (player_valid_mask & (1 << player_index)) != 0;
  }
  return valid;
}

static __forceinline bool user_interface_session_get_membership(
    const c_network_session *session, unsigned long *player_valid_mask, const s_network_session_player **players);
bool user_interface_session_is_player_valid(long player_index)
{
  bool valid = false;
  if (player_index != -1) {
    c_network_session *session = 0;
    if (user_interface_session_get_usable_player_xuid(&session)) {
      unsigned long player_valid_mask;
      const s_network_session_player *players;
      if (user_interface_session_get_membership(session, &player_valid_mask, &players)) {
        bool in_range = player_index >= 0 && player_index < 16;
        valid = in_range && (player_valid_mask & (1 << player_index)) != 0;
      }
    }
  }
  return valid;
}

// @address 0x46c698 user_interface_session_is_local_player
extern "C" const s_player_identifier *user_interface_session_is_local_player(long player_index)
{
  user_interface_session_identifier_provider provider = network_session_interface_identifier_provider();
  if (provider != user_interface_session_is_local_player)
    return provider(player_index);
  const s_player_identifier *result = 0;
  c_network_session *session = 0;
  if (user_interface_session_get_usable_player_xuid(&session) && user_interface_session_is_player_valid(player_index)) {
    unsigned long player_valid_mask;
    const s_network_session_player *players;
    if (user_interface_session_get_membership(session, &player_valid_mask, &players)) {
      if (player_valid_mask & (1 << player_index))
        result = &players[player_index].identifier;
    }
  }
  return result;
}

const s_player_identifier *user_interface_squad_get_player_identifier(long player_index)
{
  const s_player_identifier *result = 0;
  unsigned long player_valid_mask;
  const s_network_session_player *players;
  if (user_interface_squad_is_player_valid(player_index) &&
      network_squad_session_get_membership(0, 0, 0, 0, 0, 0, 0, 0, &player_valid_mask, &players) &&
      ((1 << player_index) & player_valid_mask))
  {
    result = &players[player_index].identifier;
  }
  return result;
}

// @address 0x46c800 user_interface_session_player_get_usable_player_xuid
extern "C" unsigned __int64 user_interface_session_player_get_usable_player_xuid(long player_index)
{
  unsigned __int64 result = 0;
  c_network_session *session = 0;
  if (user_interface_session_get_usable_player_xuid(&session)) {
    const c_network_session_membership *membership = session->get_session_membership();
    if (user_interface_session_membership_player_is_valid(membership, player_index))
      result = membership->m_current_membership.players[player_index].configuration.client.xuid;
  }
  return result;
}

static __forceinline unsigned __int64 user_interface_session_membership_get_usable_player_xuid(
    const c_network_session_membership *membership, long player_index)
{
  unsigned __int64 xuid = 0;
  const s_network_session_player *player = &membership->m_current_membership.players[player_index];
  if (player->configuration.client.online || membership->m_local_peer_index == player->peer_index)
    xuid = player->configuration.client.xuid;
  return xuid;
}
// @address 0x46c890 user_interface_session_get_player_xuid_if_usable
extern "C" unsigned __int64 user_interface_session_get_player_xuid_if_usable(long player_index)
{
  unsigned __int64 result = 0;
  c_network_session *session = 0;
  if (user_interface_session_get_usable_player_xuid(&session)) {
    const c_network_session_membership *membership = session->get_session_membership();
    if (user_interface_session_membership_player_is_valid(membership, player_index))
      result = user_interface_session_membership_get_usable_player_xuid(membership, player_index);
  }
  return result;
}

const s_player_configuration *user_interface_squad_get_player_data(long player_index)
{
  const s_player_configuration *result = 0;
  unsigned long player_valid_mask;
  const s_network_session_player *players;
  if (network_squad_session_get_membership(0,
        0,
        0,
        0,
        0,
        0,
        0,
        0,
        &player_valid_mask,
        &players) && (player_valid_mask & (1 << player_index)))
    result = &players[player_index].configuration;
  return result;
}

static __forceinline bool user_interface_session_peer_visible(const c_network_session_membership *membership,
    long peer_index)
{
  long state = membership->m_current_membership.peers[peer_index].connection_state;
  return state == 8 || state == 1;
}
static __forceinline bool user_interface_session_get_membership(
    const c_network_session *session, unsigned long *player_valid_mask, const s_network_session_player **players)
{
  bool result = false;
  if (session->established()) {
    const c_network_session_membership *membership = session->get_session_membership();
    if (players)
      *players = membership->m_current_membership.players;
    if (player_valid_mask) {
      unsigned long current_player_valid_mask = membership->m_current_membership.valid_player_mask.get_bits_direct()[0];
      for (long peer_index = (membership)->get_first_peer(); peer_index != -1;
          peer_index = (membership)->get_next_peer(peer_index))
      {
        if (!user_interface_session_peer_visible(membership,
            peer_index) && peer_index != membership->m_local_peer_index) {
          const s_network_session_peer *peer = &membership->m_current_membership.peers[peer_index];
          for (unsigned long player = 0; player < 4; player++) {
            long player_index = peer->user_player_indices[player];
            if (player_index != -1)
              current_player_valid_mask &= ~(1 << player_index);
          }
        }
      }
      *player_valid_mask = current_player_valid_mask;
    }
    result = true;
  }
  return result;
}

// @address 0x46c9d0 ?user_interface_session_get_player_data@@YAPBUs_player_configuration@@J@Z
const s_player_configuration *user_interface_session_get_player_data(long session_player_index)
{
  const s_player_configuration *result = 0;
  c_network_session *session = 0;
  if (user_interface_session_get_usable_player_xuid(&session)) {
    unsigned long player_valid_mask;
    const s_network_session_player *players;
    if (user_interface_session_get_membership(session, &player_valid_mask, &players)) {
      if (player_valid_mask & (1 << session_player_index))
        result = &players[session_player_index].configuration;
    }
  }
  return result;
}

// @address 0x46beb0 user_interface_session_get_controller_index
long user_interface_session_get_controller_index(short player_index)
{
  long result = -1;
  c_network_session *session = 0;
  if (user_interface_session_get_usable_player_xuid(&session)) {
    unsigned int absolute_index = (unsigned short)player_index;
    unsigned long player_valid_mask;
    const s_network_session_player *players;
    if (user_interface_session_get_membership(session, &player_valid_mask, &players)) {
      if (player_valid_mask & (1 << absolute_index))
        result = players[absolute_index].controller_index;
    }
  }
  return result;
}

e_session_game_mode user_interface_get_session_game_mode(void)
{
  int result = 0;
  if (network_globals.initialized) {
    switch (game_mode()) {
    case _life_cycle_state_none:
    case _life_cycle_state_leaving:
    case _life_cycle_state_joining:
      result = 0;
      break;
    case _life_cycle_state_pre_game:
    case _life_cycle_state_start_game:
      result = 2;
      break;
    case _life_cycle_state_matchmaking_start:
    case _life_cycle_state_matchmaking_find_match_client:
    case _life_cycle_state_matchmaking_find_match:
    case _life_cycle_state_matchmaking_find_and_assemble_match:
    case _life_cycle_state_matchmaking_assemble_match:
    case _life_cycle_state_matchmaking_arbitration:
    case _life_cycle_state_matchmaking_select_host:
    case _life_cycle_state_matchmaking_prepare_map:
      result = 1;
      break;
    case _life_cycle_state_in_game:
    case _life_cycle_state_end_game_write_stats:
    case _life_cycle_state_in_match:
    case _life_cycle_state_end_match_write_stats:
      result = 3;
      break;
    case _life_cycle_state_post_match:
      result = 4;
      break;
    default:
      // match: 0x46cb28: without it a cmplwi 17 / bgtlr range check guards the life-cycle-state jump table
      __assume(0);
    }
  }
  return (e_session_game_mode)result;
}

static __forceinline bool user_interface_session_in_group_session(s_network_session **session)
{
  bool result = false;
  if (network_life_cycle_globals_get()->initialized) {
    s_network_session *current = (s_network_session *)network_life_cycle_globals_get()->manager.m_group_session;
    if (!((c_network_session *)current)->disconnected()) {
      if (session)
        *session = current;
      result = true;
    }
  }
  return result;
}

static __forceinline bool user_interface_session_peer_established(const s_network_session *session)
{
  bool result = false;
  switch (((const c_network_session *)session)->current_local_state()) {
  case _network_session_state_none:
  case _network_session_state_peer_creating:
  case _network_session_state_peer_joining:
  case _network_session_state_peer_join_abort:
    result = false;
    break;
  case _network_session_state_peer_established:
  case _network_session_state_peer_leaving:
  case _network_session_state_host_established:
  case _network_session_state_host_disband:
  case _network_session_state_host_handoff:
  case _network_session_state_host_reestablish:
  case _network_session_state_election:
    result = true;
    break;
  default:
    // match: 0x46ddd8: without it a cmplwi 10 / bgt range check guards the session-state jump table (every inlined caller)
    __assume(0);
  }
  return result;
}

namespace {
static __forceinline long network_life_cycle_get_state()
{
    long state = 0;
    if (network_life_cycle_globals_get()->initialized)
        state = network_life_cycle_globals_get()->manager.m_current_state;
    return state;
}
}

namespace {
static __forceinline e_campaign_difficulty_level network_life_cycle_session_get_campaign_difficulty()
{
  c_network_session *session = 0;
  e_campaign_difficulty_level difficulty = (e_campaign_difficulty_level)NONE;
  if (network_life_cycle_in_session(&session) && session->established()) {
    c_network_session_parameters *parameters = session->get_session_parameters();
    if (parameters->m_parameters.campaign_difficulty.get_allowed())
      difficulty = *parameters->m_parameters.campaign_difficulty.get();
  }
  return difficulty;
}
}
// @address 0x46d740 ?user_interface_game_settings_get_campaign_difficulty@@YA?AW4e_campaign_difficulty_level@@XZ
e_campaign_difficulty_level user_interface_game_settings_get_campaign_difficulty()
{
  e_campaign_difficulty_level difficulty = network_life_cycle_session_get_campaign_difficulty();
  return difficulty;
}

bool user_interface_squad_in_or_after_countdown(void)
{
    long life_cycle_state = 0;
    long game_start_status = ((long)::user_interface_get_session_game_start_status(0, (unsigned long *)(0)));
    bool result = false;
    if (network_globals.initialized)
        life_cycle_state = network_life_cycle_get_state();
    if (game_start_status == 6)
        result = true;
    else if (life_cycle_state == 3 || life_cycle_state == 2)
        result = true;
    return result;
}

long user_interface_session_player_get_player_index(const s_player_identifier *player_identifier)
{
  long player_index = -1;
  c_network_session *session = 0;
  if (user_interface_session_get_usable_player_xuid(&session))
    player_index = ::user_interface_session_get_player_index(player_identifier, session);
  return player_index;
}

bool user_interface_interactive_session_is_group(void)
{
  bool result = false;
  c_network_session *session = 0;
  if (user_interface_session_get_usable_player_xuid(&session))
    result = session->session_type() == 2;
  return result;
}

// @address 0x46dc18 user_interface_interactive_session_is_squad
extern "C" bool user_interface_interactive_session_is_squad(void)
{
  bool result = false;
  c_network_session *session = 0;
  if (user_interface_session_get_usable_player_xuid(&session))
    result = session->session_type() == 1;
  return result;
}

// @address 0x46dc78 user_interface_session_set_name
bool user_interface_session_set_name(const wchar_t *name, const wchar_t *session_name)
{
  if (network_globals.initialized) {
    wchar_t machine_name[16];
    if (name)
      ustrnzcpy_inline(machine_name, name, 16);
    else
      usnzprintf(machine_name, 16, L"%S", "");
    wchar_t display_name[32];
    ustrnzcpy_inline(display_name, session_name, 32);
    network_session_interface_set_player_and_session_names(machine_name, display_name);
    return true;
  }
  return false;
}

// @address 0x46ddd8 user_interface_squad_try_and_get_session
bool user_interface_squad_try_and_get_session(s_network_session **session)
{
  return user_interface_session_in_squad_session(session) && user_interface_session_peer_established(*session);
}

// @address 0x46de58 user_interface_group_try_and_get_session
bool user_interface_group_try_and_get_session(s_network_session **session)
{
  return user_interface_session_in_group_session(session) && user_interface_session_peer_established(*session);
}

// @address 0x46cbe0 ?user_interface_get_session_game_start_status@@YA?AW4e_session_game_start_status@@PAW4e_session_game_start_error@@PAK@Z
enum e_session_game_start_status user_interface_get_session_game_start_status(enum e_session_game_start_error * _sym_p0,
        unsigned long * players_out)
{
    long status = 0;
    long error = 0;
    long mode = user_interface_get_session_game_mode();
    unsigned long players = 0;
    s_network_session *current = 0;
    if (mode == 0 || !user_interface_squad_try_and_get_session(&current))
        status = 0;
    else if (mode != 2)
        status = 1;
    else {
        const s_network_session_parameter_game_start_status *data = 0;
        c_network_session_parameters *parameters = ((c_network_session *)current)->get_session_parameters();
        if (parameters->m_parameters.game_start_status.get_allowed()) {
            data = parameters->m_parameters.game_start_status.get();
            status = data->status;
            error = data->error;
            players = data->player_error_mask;
            if (status == 5 && !((c_network_session *)current)->is_leader())
                status = 2;
        }
    }
    if (((long *)_sym_p0))
        *((long *)_sym_p0) = error;
    if (players_out)
        *players_out = players;
    return (e_session_game_start_status)status;
}

// 0x46cd08
bool user_interface_squad_set_map(e_campaign_id campaign_id, e_map_id map_id)
{
  bool result = false;
  char scenario_path[256];
  levels_get_map_scenario_path(campaign_id, map_id, scenario_path, sizeof(scenario_path));
  if (scenario_path[0])
    result = network_squad_session_set_map(campaign_id, map_id, scenario_path);
  return result;
}

// @address 0x46cf88 ?user_interface_session_parameters_are_valid_for_ui_game_mode@@YA_NPBVc_network_session_parameters@@@Z
bool user_interface_session_parameters_are_valid_for_ui_game_mode(class c_network_session_parameters const * parameters)
{
  bool valid = false;
  switch (*parameters->m_parameters.ui_game_mode.get())
  {
  case _gui_game_mode_campaign:
    valid = parameters->parameters_valid(0x7ff80);
    break;
  case _gui_game_mode_matchmaking:
    valid = parameters->parameters_valid(0xbfffc000000ui64);
    break;
  case _gui_game_mode_multiplayer:
    valid = parameters->parameters_valid(0x182780);
    break;
  case _gui_game_mode_theater:
    valid = parameters->parameters_valid(0x600080);
    break;
  case _gui_game_mode_mapeditor:
    valid = parameters->parameters_valid(0x182780);
    break;
  }
  return valid;
}

// @address 0x46d040 user_interface_squad_can_user_join
// whether the local user user_index may take part: not while the squad refuses joins, not past the local player limit,
// and not while the user's slot is in state 1
bool user_interface_squad_can_user_join(long user_index)
{
  bool result = true;
  long local_player_count = 0;
  s_network_session *session;
  if (user_interface_squad_try_and_get_session(&session)
      && network_session_get_join_refuse_reason((s_network_session_host_owner *)session) != 0)
    result = false;
  if (result == true)
  {
    for (e_controller_index controller_index = first_controller();
      controller_index != _controller_none;
      controller_index = next_controller(controller_index))
    {
      if (input_has_gamepad((short)controller_index))
      {
        if (g_controller_globals.controllers[controller_index].user_index == user_index)
          break;
        local_player_count++;
        if (local_player_count > g_network_configuration.maximum_players_per_peer)
        {
          result = false;
          break;
        }
      }
    }
  }
  if (result == true)
  {
    if (user_index != -1)
      return g_network_session_interface_globals.users[user_index].state != 1;
  }
  return result;
}

static __forceinline long user_interface_session_controller_user_index(e_controller_index controller_index)
{
  return (short)g_controller_globals.controllers[controller_index].user_index;
}

bool user_interface_squad_start_countdown_timer(e_controller_index controller_index, long countdown_timer,
    long minimum_timer)
{
  long game_start_error;
  long game_start_status = ((long)::user_interface_get_session_game_start_status((e_session_game_start_error *)&game_start_error,
        (unsigned long *)(0)));
  long user_index = user_interface_session_controller_user_index(controller_index);
  bool result = false;
  if (user_index != -1) {
    if (game_start_status == 5) {
      if (::network_life_cycle_squad_local_peer_is_leader()) {
        if (network_squad_session_set_countdown_timer(true, countdown_timer,
            _network_game_countdown_delayed_reason_none, user_index))
          result = true;
      }
    }
    else if (game_start_status == 6) {
      long current_timer = maximum(user_interface_squad_get_countdown_timer(), 0L);
      if (current_timer > minimum_timer) {
        if (network_squad_session_set_countdown_timer(true, current_timer - 1,
            _network_game_countdown_delayed_reason_none, user_index))
          result = true;
      }
    }
  }
  return result;
}

bool user_interface_squad_stop_countdown_timer(e_controller_index controller_index, long countdown_timer,
    bool keep_countdown)
{
  bool result = false;
  long user_index = user_interface_session_controller_user_index(controller_index);
  if (user_index != -1 && network_squad_session_get_countdown_timer() != -1) {
    bool allowed = false;
    e_network_game_countdown_delayed_reason delayed_reason = _network_game_countdown_delayed_reason_none;
    bool delayed;
    if (::network_life_cycle_squad_local_peer_is_leader()) {
      allowed = true;
      delayed = false;
    }
    else if (((long)::user_interface_get_session_game_start_status(0, (unsigned long *)(0))) == 6) {
      allowed = true;
      delayed = true;
      delayed_reason = _network_game_countdown_delayed_reason_player;
    }
    if (allowed) {
      if (network_squad_session_set_countdown_timer(delayed, 6, delayed_reason, user_index))
        result = true;
    }
  }
  return result;
}

bool user_interface_get_interactive_session(c_network_session **session)
{
  bool result = false;
  long life_cycle_state = network_life_cycle_get_state();
  switch (life_cycle_state - 1) {
  case _life_cycle_state_pre_game - 1:
  case _life_cycle_state_start_game - 1:
  case _life_cycle_state_in_game - 1:
  case _life_cycle_state_end_game_write_stats - 1:
  case _life_cycle_state_matchmaking_start - 1:
  case _life_cycle_state_matchmaking_find_match_client - 1:
  case _life_cycle_state_matchmaking_find_match - 1:
  case _life_cycle_state_matchmaking_find_and_assemble_match - 1:
  case _life_cycle_state_matchmaking_assemble_match - 1:
  case _life_cycle_state_matchmaking_arbitration - 1:
  case _life_cycle_state_matchmaking_select_host - 1:
    result = network_life_cycle_try_and_get_squad_session(session);
    break;
  case _life_cycle_state_matchmaking_prepare_map - 1:
  case _life_cycle_state_in_match - 1:
  case _life_cycle_state_end_match_write_stats - 1:
  case _life_cycle_state_post_match - 1:
    result = network_life_cycle_in_group_session(session);
    break;
  case _life_cycle_state_leaving - 1:
  case _life_cycle_state_joining - 1:
  default:
    break;
  }
  return result;
}

// @address 0x46dd68 user_interface_session_get_usable_player_xuid
bool user_interface_session_get_usable_player_xuid(c_network_session **session)
{
  return user_interface_get_interactive_session(session) && (*session)->established();
}

// @address 0x46ded8 ?user_interface_session_get_player_index@@YAJPBUs_player_identifier@@PBVc_network_session@@@Z
long user_interface_session_get_player_index(struct s_player_identifier const * identifier,
    class c_network_session const * session)
{
  long result = -1;
  if (identifier && session && session->established()) {
    unsigned long valid_mask;
    const s_network_session_player *players;
    if (user_interface_session_get_membership(session, &valid_mask, &players)) {
      for (long index = 0; index < 16; index++) {
        if ((valid_mask & (1 << index))
            && player_identifier_compare(&players[index].identifier, identifier))
          result = index;
      }
    }
  }
  return result;
}

// @address 0x478e48 ?user_interface_create_new_squad@@YAXW4e_network_session_class@@@Z

static __forceinline long user_interface_controller_get_next_controller(long controller)
{
  long next;
  switch (controller)
  {
  case _controller_none: next = _controller0; break;
  case _controller0: next = _controller1; break;
  case _controller1: next = _controller2; break;
  case _controller2: next = _controller3; break;
  default: next = _controller_none; break;
  }
  return next;
}

// @address 0x46b1e0 user_interface_squad_restart
extern "C" int user_interface_squad_restart(int session_class)
{
  network_life_cycle_end();
  int result = ((int (*)(e_network_session_class))network_life_cycle_create_local_squad)((e_network_session_class)session_class);
  unsigned char created = (unsigned char)result;
  if (created != 0)
  {
    long controller = 0;
    do
    {
      const wchar_t *name = user_interface_controller_get_player_profile_name((e_controller_index)controller);
      if (name && wcslen(name))
      {
        user_interface_session_set_name(name, name);
        break;
      }
      long next_controller;
      switch (controller + 1)
      {
      case _controller0: next_controller = _controller0; break;
      case _controller1: next_controller = _controller1; break;
      case _controller2: next_controller = _controller2; break;
      case _controller3: next_controller = _controller3; break;
      default: next_controller = _controller_none; break;
      }
      controller = next_controller;
    } while (controller != -1);
  }
  return result;
}

void user_interface_create_new_squad(e_network_session_class session_class)
{
  for (long controller = user_interface_controller_get_next_controller(-1);
       controller != -1;
       controller = user_interface_controller_get_next_controller(controller))
  {
    const wchar_t *name = user_interface_controller_get_player_profile_name((e_controller_index)controller);
    if (name && wcslen(name))
    {
      user_interface_session_set_name(name, name);
      break;
    }
  }

  if (session_class == _network_session_class_offline)
  {
    ::user_interface_squad_set_start_mode(_network_game_start_mode_custom_game);
    if (::network_life_cycle_squad_local_peer_is_leader())
    {
      e_network_game_simulation_protocol protocol = _network_game_simulation_protocol_synchronous;
      if (network_squad_session_can_set_game_settings())
      {
        s_network_session *squad = 0;
        if (user_interface_session_in_squad_session(&squad))
          (&((c_network_session *)squad)->get_session_parameters()->m_parameters.simulation_protocol)->request_change(&protocol);
      }
    }
  }
  else if (session_class == _network_session_class_system_link)
  {
    ::user_interface_squad_set_start_mode(_network_game_start_mode_custom_game);
  }
  else if (session_class == _network_session_class_xbox_live)
  {
    ::user_interface_squad_set_start_mode(_network_game_start_mode_matchmaking);
  }
  else if (session_class == k_network_session_class_count)
  {
    ::user_interface_squad_set_start_mode(_network_game_start_mode_film);
  }
}

// @address 0x4f32f8 parse_xml_lobby_film_party_leader_requirement
bool parse_xml_lobby_film_party_leader_requirement(void *context, wchar_t *output, long capacity)
{
  bool success = false;
  if (user_interface_squad_exists()) {
    long player_index = network_squad_session_get_host_player_index();
    if (player_index != -1) {
      const s_player_configuration *player_data = user_interface_squad_get_player_data(player_index);
      if (player_data) {
        ustrnzcpy(output, player_data->host.name, capacity);
        success = true;
      }
    }
  }
  return success;
}

// @address 0x491158 user_interface_session_find_player_xuid

extern "C" unsigned __int64 user_interface_session_find_player_xuid(const s_player_identifier *identifier)
{
  c_network_session *session = 0;
  unsigned __int64 result = 0;
  long player_index;
  if (user_interface_session_get_usable_player_xuid(&session)) {
    player_index = ::user_interface_session_get_player_index(identifier, session);
    if (player_index != -1)
      result = user_interface_session_get_player_xuid_if_usable(player_index);
  }
  // match: 0x491158: retain the existing compiler barrier across the optional XUID lookup.
  void _ReadWriteBarrier(void);
  _ReadWriteBarrier();
  if (result == 0) {
    c_data_iterator<player_datum> players;
    players.begin(tls_player_data());
    while (players.next()) {
      if (player_identifier_compare(&players.get_datum()->player_identifier, identifier)) {
        result = players.get_datum()->configuration.client.xuid;
        break;
      }
    }
  }
  return result;
}

// @address 0x46cf38 user_interface_squad_set_map_variant_with_verify
extern "C" unsigned char user_interface_squad_set_map_variant_with_verify(const c_map_variant *request)
{
  bool enabled = network_squad_session_get_session_class() == _network_session_class_xbox_live && !request->m_built_in;
  return user_interface_squad_set_map_variant(request, enabled);
}

static __forceinline bool user_interface_session_global_string(long id, c_static_wchar_string<1024> *text)
{
  bool result = false;
  text->m_string[0] = 0;
  const s_user_interface_shared_globals *globals = ::user_interface_shared_tag_globals_try_and_get();
  if (globals && globals->global_strings.index != -1)
    result = text_group_get_string(globals->global_strings.index, id, text);
  return result;
}

static __forceinline c_game_engine_base_variant *user_interface_session_game_writeable(c_game_variant *variant)
{
  ((c_game_engine_base_variant *)variant->m_variant_storage)->m_flags.set(_base_variant_flags_built_in, false);
  return (c_game_engine_base_variant *)variant->m_variant_storage;
}

// match: 0x46e080: calling the converter directly merges the two map description buffers and shrinks the frame by 0x100.
static __forceinline void user_interface_session_description_to_utf8(const wchar_t *source, char *destination)
{
  wchar_string_to_utf8_string(source, destination, 128, 0);
}
static __forceinline void user_interface_session_map_description(c_map_variant *variant, const wchar_t *text)
{
  char description[128];
  user_interface_session_description_to_utf8(text, description);
  memset(variant->metadata.description, 0, sizeof(variant->metadata.description));
  strncpy(variant->metadata.description, description, 128);
  variant->metadata.description[127] = 0;
  variant->built_in = false;
}
extern "C"
{
  // @address 0x1710 k_user_interface_session_source_file
  const char k_user_interface_session_source_file[] = "..\\interface\\user_interface_session.cpp";
}

static __forceinline void user_interface_session_post_verification_error(long error)
{
  g_gui_error_manager.post_error_with_custom_message(error, 0, 0, static_cast<e_controller_index>(255), false, true);
}

// @address 0x46e080 user_interface_session_update_string_verification
extern "C" void user_interface_session_update_string_verification()
{
  e_gui_game_mode mode = ::network_life_cycle_squad_session_get_ui_game_mode();
  const s_gui_game_setup_storage *preferences = global_preferences_get_game_setup_storage();
  for (long index = 0; index < 3; index++)
  {
    s_user_interface_content_globals::s_task_slot &task_slot = g_user_interface_content_globals.tasks[index];
    c_string_verify_task *task = &task_slot.task;
    if (task_slot.state == 2 && !task->busy())
    {
      const c_static_wchar_string<16> *name = &task_slot.name;
      const c_static_wchar_string<128> *description = &task_slot.description;
      if ((name->length() && wcsncmp(name->m_string, task->m_strings[0], 16)) ||
          (static_cast<long>(content_item_get_string_length(reinterpret_cast<const unsigned short *>(description->m_string))) &&
           wcsncmp(description->m_string, task->m_strings[1], 128)))
      {
        task_slot.state = 1;
      }
      else
      {
        c_static_wchar_string<1024> text;
        bool name_valid = !task_slot.name.length() || task->m_string_verified[0];
        bool description_valid =
            !static_cast<long>(content_item_get_string_length(reinterpret_cast<const unsigned short *>(task_slot.description.m_string))) ||
            task->m_string_verified[1];
        if (index == 0)
        {
          bool error_posted = false;
          const c_game_variant *source =
              mode == _gui_game_mode_multiplayer ? &preferences->multiplayer_settings.game_variant.variant : 0;
          if (source)
          {
            c_game_variant variant;
            variant.copy_from_and_validate(source);
            if (name_valid)
              gui_screen_game_options_set_variant_name(user_interface_session_game_writeable(&variant),
                  task_slot.name.m_string);
            else
            {
              user_interface_session_post_verification_error(0x20036);
              error_posted = true;
              if (user_interface_session_global_string(0x10319, &text))
              {
                gui_screen_game_options_set_variant_name(user_interface_session_game_writeable(&variant),
                    text.m_string);
                name_valid = true;
              }
            }
            char description[128];
            if (description_valid)
            {
              wchar_string_to_utf8_string(task_slot.description.m_string, description, 128, 0);
              game_engine_variant_set_description(user_interface_session_game_writeable(&variant), description);
            }
            else
            {
              if (!error_posted)
                user_interface_session_post_verification_error(0x20035);
              if (user_interface_session_global_string(0x1031a, &text))
              {
                wchar_string_to_utf8_string(text.m_string, description, 128, 0);
                game_engine_variant_set_description(user_interface_session_game_writeable(&variant), description);
                description_valid = true;
              }
            }
            if (name_valid || description_valid)
              user_interface_squad_set_game_variant(&variant, false);
          }
        }
        else if (index == 1)
        {
          bool error_posted = false;
          const c_map_variant *source =
              mode == _gui_game_mode_multiplayer ? &preferences->multiplayer_settings.map_variant.variant :
              mode == _gui_game_mode_mapeditor ? &preferences->map_editor_settings.map_variant : 0;
          if (source)
          {
            c_map_variant variant;
            source->save_to(&variant);
            if (name_valid)
              game_engine_set_map_info_name(&variant.metadata, task_slot.name.m_string);
            else
            {
              user_interface_session_post_verification_error(0x20038);
              error_posted = true;
              if (user_interface_session_global_string(0x10321, &text))
              {
                game_engine_set_map_info_name(&variant.metadata, text.m_string);
                name_valid = true;
              }
            }
            if (description_valid)
            {
              user_interface_session_map_description(&variant, task_slot.description.m_string);
            }
            else
            {
              if (!error_posted)
                user_interface_session_post_verification_error(0x20037);
              if (user_interface_session_global_string(0x10322, &text))
              {
                user_interface_session_map_description(&variant, text.m_string);
                description_valid = true;
              }
            }
            if (name_valid || description_valid)
              user_interface_squad_set_map_variant(&variant, false);
          }
        }
        else if (index == 2)
        {
          const s_saved_film_description *source = ::user_interface_game_settings_get_film();
          if (source)
          {
            s_saved_film_description film = *source;
            if (name_valid)
            {
              memset(film.name, 0, sizeof(film.name));
              ustrnzcpy(film.name, task_slot.name.m_string, 16);
            }
            else
            {
              user_interface_session_post_verification_error(0x20034);
              if (user_interface_session_global_string(0x10313, &text))
              {
                memset(film.name, 0, sizeof(film.name));
                ustrnzcpy(film.name, text.m_string, 128);
                name_valid = true;
              }
            }
            if (name_valid)
              user_interface_squad_set_film(&film, false);
          }
        }
        task_slot.state = 0;
      }
    }
    if (task_slot.state == 1 && !task->busy())
    {
      const wchar_t *strings[2] = { task_slot.name.m_string, task_slot.description.m_string };
      string_verify_task_set_strings(task, get_current_language(), strings, 2);
      if (overlapped_task_start_internal(task, k_user_interface_session_source_file, 3218))
        task_slot.state = 2;
    }
  }
}

// @address 0x46d168 user_interface_squad_set_game_variant
extern "C" bool user_interface_squad_set_game_variant(const c_game_variant *source, bool enabled)
{
  bool result;
  if (source)
  {
    c_game_variant variant;
    result = true;
    variant.copy_from_and_validate(source);
    if (enabled)
    {
      if (online_local_xuid_is_online_enabled())
      {
        c_static_wchar_string<1024> description;
        result = false;
        c_static_wchar_string<1024> name;
        if (user_interface_session_global_string(0x1031b, &name) &&
            user_interface_session_global_string(0x1031c, &description))
        {
          g_user_interface_content_globals.tasks[0].name.set(
              ((const c_game_engine_base_variant *)source->m_variant_storage)->m_metadata.name);
          ::utf8_string_to_wchar_string((utf8 const *)((const c_game_engine_base_variant *)source->m_variant_storage)->m_metadata.description,
        g_user_interface_content_globals.tasks[0].description.m_string,
        128,
        (long *)(0));
          gui_screen_game_options_set_variant_name(user_interface_session_game_writeable(&variant), name.m_string);
          char utf8_description[128];
          wchar_string_to_utf8_string(description.m_string, utf8_description, 128, 0);
          game_engine_variant_set_description(user_interface_session_game_writeable(&variant), utf8_description);
          result = true;
          g_user_interface_content_globals.tasks[0].state = 1;
        }
      }
      else
      {
        e_game_engine_type type = (e_game_engine_type)source->engine_type;
        if (c_gui_game_variant_selected_item::get_game_engine_item_count(type) > 0)
          game_engine_tag_defined_variant_get_built_in_variant(type, 0, &variant);
        else
          game_engine_variant_initialize_default(&variant, type);
        result = network_squad_session_set_game_variant(&variant);
      }
    }
    if (result)
      result = network_squad_session_set_game_variant(&variant);
  }
  else
    result = network_squad_session_set_game_variant(0);
  return result;
}

// @address 0x46d358 user_interface_squad_set_game_variant_with_verify
extern "C" bool user_interface_squad_set_game_variant_with_verify(const c_game_variant *variant)
{
  bool enabled = network_squad_session_get_session_class() == _network_session_class_xbox_live &&
                 !(((const c_game_engine_base_variant *)variant->m_variant_storage)->m_flags.test(_base_variant_flags_built_in));
  return user_interface_squad_set_game_variant(variant, enabled);
}
// @address 0x46cd60 user_interface_squad_set_map_variant
extern "C" unsigned char user_interface_squad_set_map_variant(const c_map_variant *source, bool enabled)
{
  char utf8_description[128];
  char scenario_path[256];
  bool ready = true;
  c_map_variant variant;
  source->save_to(&variant);
  if (enabled)
  {
    ready = false;
    if (online_local_xuid_is_online_enabled())
    {
      c_static_wchar_string<1024> description;
      c_static_wchar_string<1024> name;
      if (user_interface_session_global_string(0x10323, &name) &&
          user_interface_session_global_string(0x10324, &description))
      {
        g_user_interface_content_globals.tasks[1].name.set(source->metadata.name);
        ::utf8_string_to_wchar_string((utf8 const *)source->metadata.description,
        g_user_interface_content_globals.tasks[1].description.m_string,
        128,
        (long *)(0));
        game_engine_set_map_info_name(&variant.metadata, name.m_string);
        wchar_string_to_utf8_string(description.m_string, utf8_description, 128, 0);
        memset(variant.metadata.description, 0, sizeof(variant.metadata.description));
        strncpy(variant.metadata.description, utf8_description, 128);
        variant.metadata.description[127] = 0;
        variant.built_in = false;
        g_user_interface_content_globals.tasks[1].state = 1;
        ready = true;
      }
    }
    else
    {
      variant.create_default(source->m_map_id);
      ready = true;
    }
  }
  unsigned char result;
  if (ready)
  {
    result = 0;
    levels_get_map_scenario_path(_campaign_id_none, variant.map_id, scenario_path, sizeof(scenario_path));
    if (scenario_path[0])
    {
      bool map_set = network_squad_session_set_map(_campaign_id_none, variant.m_map_id, scenario_path);
      result = network_squad_session_set_map_variant(&variant) & map_set;
    }
  }
  else
    result = ready;
  return result;
}

// @address 0x46d3a8 user_interface_squad_set_film
extern "C" bool user_interface_squad_set_film(const s_saved_film_description *source, bool enabled)
{
  bool result = false;
  bool valid = true;
  if (source->film_type != 0)
  {
    if (source->film_type == 6)
      valid = false;
    else if (source->controller_index != -1)
      valid = saved_film_can_load(source->controller_index, source->file_path);
  }
  s_saved_film_description film;
  if (valid)
  {
    result = true;
    film = *source;
    if (enabled)
    {
      result = false;
      if (online_local_xuid_is_online_enabled())
      {
        c_static_wchar_string<1024> name;
        if (user_interface_session_global_string(0x10314, &name))
        {
          g_user_interface_content_globals.tasks[2].name.set(source->name);
          g_user_interface_content_globals.tasks[2].description.set(L"");
          memset(film.name, 0, sizeof(film.name));
          ustrnzcpy(film.name, name.m_string, 128);
          g_user_interface_content_globals.tasks[2].state = 1;
          result = true;
        }
      }
    }
    if (result)
    {
      result = false;
      if (network_squad_session_set_film(&film))
      {
        g_network_session_interface_globals.saved_film = film;
        unsigned long now = network_time_get();
        g_network_session_interface_globals.film_reset_time = now;
        result = true;
      }
    }
  }
  if (!result)
  {
    memset(&film, 0, sizeof(film));
    film.difficulty = _campaign_difficulty_level_normal;
    film.controller_index = -1;
    film.film_type = valid ? 0 : 6;
    return network_squad_session_set_film(&film);
  }
  return result;
}

bool (c_network_session::*user_interface_session_leaving_session_ref())(void) const
{
  return &c_network_session::leaving_session;
}

// @address 0x46b578 ?user_interface_remote_squad_contains_local_peer@@YA_NPBUs_network_squad_status_data@@@Z
bool user_interface_remote_squad_contains_local_peer(const s_network_squad_status_data *squad)
{
    bool result = false;
    if (squad)
    {
        for (long player_index = 0; player_index < squad->details.player_count && !result; player_index++)
        {
            const s_network_squad_status_player *player = &squad->details.players[player_index];
            for (e_controller_index index = first_controller(); index != _controller_none && !result;
                index = next_controller(index))
            {
                const c_controller_interface *controller = (const c_controller_interface *)&g_controller_globals.controllers[index];
                if (controller_in_use(controller))
                {
                    s_player_identifier identifier;
                    result = player_identifier_compare(&player->identifier, controller->get_player_identifier(&identifier));
                }
            }
        }
    }
    return result;
}

__forceinline s_user_interface_content_globals::s_user_interface_content_globals()
{
}

// @address 0x113e5a4 g_user_interface_content_globals
extern "C" {
s_user_interface_content_globals g_user_interface_content_globals;
}

