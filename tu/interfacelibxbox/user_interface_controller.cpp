#include "interface/user_interface_controller_globals.h"
// interfacelibxbox/user_interface_controller.obj: from the records 0x89300 0x89490 0x894e8 0x8b0e8 0x8d880 0x211678 0x46f9b8 0x46fc30 0x46fd00 0x46fde8 0x46fe78 0x46ff20 0x470030 0x470108 0x4701e0 0x4702c0 0x470398 0x470a18 0x470d70 0x470e68 0x470f68 0x4a8b70 0x707f28
// @flags /TP /O2 /Oc
#include "networking/logic/network_banhammer.h"
#include "interface/event_manager_globals.h"
#include "game/map_variant.h"
#include "interface/c_player_profile.h"
// @address 0x89300 ?user_interface_controller_update@@YAXXZ
// @address 0x894e8 user_interface_controller_test_button_hold
// @address 0x8b0e8 user_interface_controller_get_rumble_enabled
// @address 0x8d880 ?user_interface_controller_is_player_profile_valid@@YA_NW4e_controller_index@@@Z
// @address 0x211678 user_interface_controller_clear_signin_cache
// @address 0x46fc30 ?user_interface_controller_update_network_properties@@YAXW4e_controller_index@@@Z
// @address 0x46fd00 ?user_interface_controller_get_first_xbox_live_signed_in_non_guest@@YA?AW4e_controller_index@@XZ
// @address 0x46fde8 ?user_interface_controller_get_player_profile_name@@YAPB_WW4e_controller_index@@@Z
// @address 0x46fe78 ?user_interface_set_last_campaign_level@@YAXW4e_campaign_id@@W4e_map_id@@@Z
// @address 0x46ff20 ?user_interface_controller_voice_allowed@@YA_NW4e_controller_index@@@Z
// @address 0x470030 ?user_interface_controller_voice_is_for_friends_only@@YA_NW4e_controller_index@@@Z
// @address 0x470108 user_interface_controller_has_blocked_communications
// @address 0x4701e0 user_interface_controller_has_friends_only_communications
// @address 0x4702c0 ?user_interface_controller_update_profile_error@@YAXW4e_controller_index@@@Z
// @address 0x470398 ?event_manager_update@@YAXK@Z
// @address 0x470d70 ?event_manager_button_pressed@@YAXW4e_controller_index@@E@Z
// @address 0x470e68 ?user_interface_controller_reset@@YAXW4e_controller_index@@@Z
// @address 0x4a8b70 ?get_signed_in_controller_count@c_main_menu_screen_widget@@QAAJPAW4e_controller_index@@@Z
#include <stdlib.h>
#include <stddef.h>
#include <string.h>
#include <ppcintrinsics.h>
#include "math/real_math.h"
#include "interface/user_interface.h"
#include "networking/logic/network_session_interface.h"
#include "simulation/simulation.h"
#include "interface/gui_pregame_setup_manager.h"
#include "interface/user_interface_controller.h"
#include "interface/c_controller.h"
#include "interface/user_interface_error_manager.h"
#include "interface/controller_types.h"
#include "interface/c_window_manager.h"
#include "interface/user_interface_window_manager.h"
#include "networking/logic/storage/network_http_buffer_downloader.h"
#include "networking/logic/network_quality_statistics_types.h"
#include "interface/files_machine_banhammer_messages_types.h"
#include "networking/logic/network_banhammer_remote_data_types.h"
#include "interface/files_user_banhammer_messages_types.h"
#include "interface/files_user_recent_players_types.h"
#include "input/input_abstraction.h"
#include "networking/online/voice_xenon.h"
#include "interface/user_interface_shared_globals.h"
#include "networking/session/network_session_interface_user_externs.h"
#include "game/player_types.h"
#include "game/game_globals_storage.h"
#include "memory/tls_globals.h"
#include "networking/session/network_session_class.h"
#include "interface/gui_screen_pregame_lobby_mapeditor_types.h"
#include "networking/logic/storage/network_http_blf_simple_downloader.h"
#include "networking/logic/network_banhammer_controller_types.h"
#include "interface/event_manager_types.h"
#include "interface/user_interface_clock.h"
#include "input/input_abstraction_types.h"
#include "game/player_configuration.h"
#include "interface/gui_screens/main_menu/gui_screen_main_menu.h"
#include "interface/controller_inline.h"
#include "interface/gui_screen_mapeditor_externs.h"
#include "main/game_preferences_game_setup_inline.h"
#include "game/game_state_accessors.h"

static const long k_ui_button_left_trigger = 0;
static const long k_ui_button_right_trigger = 1;
static const long k_ui_button_dpad_up = 2;
static const long k_ui_button_dpad_down = 3;
static const long k_ui_button_dpad_left = 4;
static const long k_ui_button_dpad_right = 5;
static const long k_ui_button_start = 6;
static const long k_ui_button_back = 7;
static const long k_ui_button_left_thumb = 8;
static const long k_ui_button_right_thumb = 9;
static const long k_ui_button_a = 10;
static const long k_ui_button_b = 11;
static const long k_ui_button_x = 12;
static const long k_ui_button_y = 13;
static const long k_ui_button_left_shoulder = 14;
static const long k_ui_button_right_shoulder = 15;
static const long k_ui_component_dpad_up = 8;
static const long k_ui_component_dpad_down = 9;
static const long k_ui_component_dpad_left = 10;
static const long k_ui_component_dpad_right = 11;
static const long k_ui_component_left_stick = 16;
static const long k_ui_left_stick = 0;
static const long k_ui_right_stick = 1;

struct s_user_interface_shared_globals;

// @address 0xf283c8 g_network_session_interface_local_users
struct s_controller_signin_cache {
  __forceinline s_controller_signin_cache() { memset(entries, 0, sizeof(entries)); }
  char entries[64];
};
extern "C" {
s_controller_signin_cache g_controller_signin_cache;
}

typedef char ui_banhammer_controller_size[(sizeof(s_network_banhammer_controller_data) == 0xfd8) ? 1 : -1];
typedef char ui_banhammer_activity_offset[(offsetof(s_network_banhammer_controller_data, last_activity_time) == 0xf8c) ? 1 : -1];

static __forceinline bool user_interface_controller_voice_communications(const c_controller_interface *controller, bool *friends_only)
{
  bool voice_enabled = (bool)(_rotl((unsigned long)controller->flags, 26) & 1);
  bool chat_enabled = (bool)(_rotl((unsigned long)controller->flags, 27) & 1);
  *friends_only = voice_enabled;
  return voice_enabled || chat_enabled;
}









static __forceinline long controller_next(long index)
{
  long next = -1;
  if ((unsigned long)index <= 2)
    next = index + 1;
  return next;
}

namespace {
static __forceinline const wchar_t *controller_display_name(const c_controller_interface *controller)
{
  return controller->display_name;
}
}

bool user_interface_controller_test_button_hold(e_controller_index controller_index, unsigned char button_action, unsigned short duration)
{
  bool result = false;
  const s_user_interface_shared_globals *shared_globals = ::user_interface_shared_tag_globals_try_and_get();
  if (shared_globals) {
    if (button_action == 1) {
      result = true;
    } else if (duration >= shared_globals->hold_delay) {
      result = true;
    }
  }
  return result;
}

static __forceinline bool user_interface_controller_is_unsigned_in_user(const c_controller_interface *controller)
{
  return (_rotl(controller->flags, 30) & 1) != 0;
}

extern "C" long user_interface_controller_get_rumble_enabled(long user_index)
{
  long result = -1;
  if (user_index != -1) {
    long index;
    for (index = 0; index != -1; index = controller_next(index)) {
      c_controller_interface *controller = controller_get((e_controller_index)index);
      bool active = ((const c_controller_interface *)controller)->is_signed_in_to_machine() ||
                    user_interface_controller_is_unsigned_in_user(controller);
      if (active && controller->user_index == user_index)
        break;
    }
    result = index;
  }
  return result;
}

bool user_interface_controller_is_player_profile_valid(e_controller_index index)
{
  c_controller_interface *controller = controller_get(index);
  return controller_in_use(controller);
}

void user_interface_controller_clear_signin_cache(void)
{
  memset(&g_controller_signin_cache, 0, sizeof(g_controller_signin_cache));
}

static __forceinline e_network_interface_user_state network_session_interface_get_local_user_state(long user_index)
{
  return (e_network_interface_user_state)g_network_session_interface_local_users[user_index].state;
}

void user_interface_controller_update_network_properties(e_controller_index controller_index)
{
  c_controller_interface *controller = controller_get(controller_index);
  long user_index = controller->get_user_index();
  if (user_index != -1 && network_session_interface_get_local_user_state(user_index) == _network_session_interface_user_state_exists) {
    s_player_identifier controller_user_identifier;
    s_player_configuration player_configuration;
    unsigned long voice_settings = ::voice_get_controller_settings(controller_index);
    user_interface_controller_get_player_configuration(controller_index, &controller_user_identifier, &player_configuration);
    if (network_session_interface_get_local_user_identifier(user_index, &controller_user_identifier, false))
      network_session_interface_set_local_user_properties(user_index, controller_index, &player_configuration, voice_settings);
  }
}

static __forceinline bool user_interface_controller_xbox_live_account_signed_in(e_controller_index controller_index)
{
  return controller_get(controller_index)->is_silver_or_gold_live();
}

static __forceinline bool user_interface_controller_xbox_live_is_guest_account(e_controller_index controller_index)
{
  return controller_get(controller_index)->is_guest();
}

e_controller_index user_interface_controller_get_first_xbox_live_signed_in_non_guest(void)
{
  e_controller_index result = _controller_none;
  for (long index = 0; index != -1; index = controller_next(index)) {
    if (user_interface_controller_xbox_live_account_signed_in((e_controller_index)index) &&
        !user_interface_controller_xbox_live_is_guest_account((e_controller_index)index)) {
      result = (e_controller_index)index;
      break;
    }
  }
  return result;
}

const wchar_t *user_interface_controller_get_player_profile_name(e_controller_index index)
{
  c_controller_interface *controller = controller_get(index);
  if (controller_in_use(controller))
    return controller_display_name(controller);
  else
    return (const wchar_t *)L"";
}


void user_interface_set_last_campaign_level(e_campaign_id campaign_id, e_map_id map_id)
{
  // Initialize each stored mode independently of the outer setup storage.
  unsigned char last_game_setup_storage[sizeof(s_gui_game_setup_storage)];
  s_gui_game_setup_storage *last_game_setup = reinterpret_cast<s_gui_game_setup_storage *>(last_game_setup_storage);
  last_game_setup->multiplayer_settings.s_gui_game_setup_storage::s_multiplayer_settings::s_multiplayer_settings();
  last_game_setup->map_editor_settings.map_variant.c_map_variant::c_map_variant();
  last_game_setup->theater_settings.options.game_options::game_options();
  global_preferences_get_last_game_setup(last_game_setup);
  last_game_setup->campaign_settings.campaign_id = campaign_id;
  last_game_setup->campaign_settings.map_id = map_id;
  last_game_setup->campaign_settings.valid = static_cast<unsigned long>(last_game_setup->campaign_settings.difficulty_level) <= 3;
  global_preferences_set_last_game_setup(last_game_setup);
}





static __forceinline bool user_interface_controller_is_allowed_communications(const c_controller_interface *controller, bool *friends_only)
{
  short flags = (short)((const c_controller_interface *)controller)->flags;
  *friends_only = (bool)(_rotl(flags, 26) & 1);
  bool allowed = (bool)(_rotl(flags, 26) & 1) || (bool)(_rotl(flags, 27) & 1);
  return allowed;
}

static __forceinline unsigned long user_interface_controller_get_ban_flags(e_controller_index controller_index)
{
  s_network_banhammer_controller_data *controller = &g_network_banhammer_globals.controllers[controller_index];
  unsigned long result = 0;
  if (controller->flags & 1)
    result = controller->ban_flags | g_network_banhammer_globals.ban_flags;
  return result;
}

bool user_interface_controller_voice_allowed(e_controller_index controller_index)
{
  bool result = false;
  if (network_squad_session_get_session_class() == 1) {
    result = true;
  } else {
    c_controller_interface *controller = controller_get(controller_index);
    bool friends_only = false;
    result = !controller->is_guest() && user_interface_controller_is_allowed_communications(controller, &friends_only);
    if (result && network_life_cycle_in_matchmaking()) {
      unsigned long ban_flags = user_interface_controller_get_ban_flags(controller_index);
      if (ban_flags & 128)
        result = false;
      else
        result = true;
    }
  }
  return result;
}

bool user_interface_controller_voice_is_for_friends_only(e_controller_index controller_index)
{
  bool result = false;
  if (network_squad_session_get_session_class() != 1) {
    c_controller_interface *controller = controller_get(controller_index);
    bool friends_only = false;
    result = user_interface_controller_voice_allowed(controller_index) && user_interface_controller_voice_communications(controller, &friends_only);
    result = result && friends_only;
  }
  return result;
}

extern "C" unsigned char user_interface_controller_has_blocked_communications(void)
{
  bool result = false;
  if (network_squad_session_get_session_class() != 1) {
    for (long controller_index = 0; controller_index != -1; controller_index = controller_next(controller_index)) {
      c_controller_interface *controller = controller_get((e_controller_index)controller_index);
      bool friends_only = false;
      if (controller->is_signed_in_to_machine() && controller->is_signed_in_to_live() && !controller->is_guest() &&
          !user_interface_controller_is_allowed_communications(controller, &friends_only)) {
        result = true;
        break;
      }
    }
  }
  return result;
}

extern "C" unsigned char user_interface_controller_has_friends_only_communications(void)
{
  bool result = false;
  if (network_squad_session_get_session_class() != 1) {
    for (long controller_index = 0; controller_index != -1; controller_index = controller_next(controller_index)) {
      c_controller_interface *controller = controller_get((e_controller_index)controller_index);
      bool friends_only = false;
      if (controller->is_signed_in_to_machine() && controller->is_signed_in_to_live() && !controller->is_guest() &&
          user_interface_controller_is_allowed_communications(controller, &friends_only) && friends_only) {
        result = true;
        break;
      }
    }
  }
  return result;
}

class c_window_manager;

enum e_gamepad_stick;
enum e_controller_component;
extern "C" {
// @address 0x119a508 g_event_manager_tab_active
bool g_event_manager_tab_active[4];
// @address 0x119a50c g_event_manager_last_tab_times
unsigned long g_event_manager_last_tab_times[4];
// @address 0x119a51c g_event_manager_repeat_start_times
unsigned long g_event_manager_repeat_start_times[4];
// @address 0x119a52c g_event_manager_previous_stick
point2d g_event_manager_previous_stick[4];
// @address 0x119a53c g_event_manager_tab_enabled
bool g_event_manager_tab_enabled;
}

static __forceinline long event_manager_abs(long value)
{
  return value >= 0 ? value : -value;
}

// @address 0x470a18 ?event_manager_tab@@YAXW4e_gamepad_stick@@W4e_controller_index@@PBTpoint2d@@KW4e_controller_component@@@Z
// match: 0x470a18: pointer __restrict qualifier prevents alias reload
void event_manager_tab(e_gamepad_stick gamepad_stick, e_controller_index controller_index, const point2d *__restrict point,
    unsigned long timestamp, e_controller_component controller_component)
{
  const s_user_interface_shared_globals *shared_globals = ::user_interface_shared_tag_globals_try_and_get();
  if (!shared_globals)
    return;
  bool moved[2]; bool previous_moved[2];
  long x = point->x;
  moved[0] = event_manager_abs(x) >= 15000;
  previous_moved[0] = event_manager_abs(g_event_manager_previous_stick[controller_index].x) > 15000;
  long y = point->y;
  moved[1] = event_manager_abs(y) >= 15000;
  previous_moved[1] = event_manager_abs(g_event_manager_previous_stick[controller_index].y) > 15000;
  bool repeating = g_event_manager_repeat_start_times[controller_index] != 0 &&
      timestamp - g_event_manager_repeat_start_times[controller_index] >= (unsigned long)shared_globals->repeat_delay;
  bool held = false;
  if (g_event_manager_last_tab_times[controller_index] != 0)
    held = timestamp - g_event_manager_last_tab_times[controller_index] >=
        (unsigned long)(repeating ? shared_globals->repeat_interval : shared_globals->hold_delay);
  bool tab;
  if (g_event_manager_tab_enabled) {
    bool fresh_x = moved[0] && !previous_moved[0];
    bool fresh_y = moved[1] && !previous_moved[1];
    tab = (fresh_x | held) | fresh_y;
  } else {
    bool tab_x = moved[0] && (!previous_moved[0] || held) && !moved[1] && !previous_moved[1];
    tab = tab_x | (moved[1] && (!previous_moved[1] || held) && !moved[0] && !previous_moved[0]);
  }
  if (x == 0 && y == 0) {
    g_event_manager_last_tab_times[controller_index] = 0;
    g_event_manager_repeat_start_times[controller_index] = 0;
  } else if (tab) {
    s_event_record event;
    event.controller = controller_index;
    event.component = controller_component;
    event.value = -1;
    if (event_manager_abs(x) >= 15000) {
      if (x != 0 && x >= 0)
        event.type = gamepad_stick ? 12 : 4;
      else
        event.type = gamepad_stick ? 10 : 2;
      g_window_manager.handle_global_controller_event(&event);
      g_event_manager_last_tab_times[controller_index] = timestamp;
      if (g_event_manager_repeat_start_times[controller_index] == 0)
        g_event_manager_repeat_start_times[controller_index] = timestamp;
    } else if (event_manager_abs(y) >= 15000) {
      if (y != 0 && y >= 0)
        event.type = gamepad_stick ? 9 : 1;
      else
        event.type = gamepad_stick ? 11 : 3;
      g_window_manager.handle_global_controller_event(&event);
      g_event_manager_last_tab_times[controller_index] = timestamp;
      if (g_event_manager_repeat_start_times[controller_index] == 0)
        g_event_manager_repeat_start_times[controller_index] = timestamp;
    }
  }
  g_event_manager_previous_stick[controller_index] = *point;
}

void event_manager_button_pressed(e_controller_index controller_index, unsigned char button)
{
  s_event_record event;
  event.type = 13;
  event.controller = controller_index;
  switch (button) {
  case k_ui_button_a: event.component = 0; break;
  case k_ui_button_b: event.component = 1; break;
  case k_ui_button_x: event.component = 2; break;
  case k_ui_button_y: event.component = 3; break;
  case k_ui_button_left_shoulder: event.component = 5; break;
  case k_ui_button_right_shoulder: event.component = 4; break;
  case k_ui_button_left_trigger: event.component = 6; break;
  case k_ui_button_right_trigger: event.component = 7; break;
  case k_ui_button_start: event.component = 12; break;
  case k_ui_button_back: event.component = 13; break;
  case k_ui_button_left_thumb: event.component = 14; break;
  case k_ui_button_right_thumb: event.component = 15; break;
  // match: 0x470d70: __assume(0) keeps the button switch an unchecked jump table; without it c2 emits a cmplwi-15 guard plus a default store.
  default: __assume(0);
  }
  event.value = 255;
  g_window_manager.handle_global_controller_event(&event);
}

// @address 0x8974d0 g_user_interface_controller_globals
extern "C" { s_user_interface_controller_globals g_user_interface_controller_globals; }
static __forceinline void user_interface_controller_activate_as_unsigned_in_user(short controller_index, bool activate)
{
  if (controller_index >= 0 && controller_index < 4) {
    c_controller_interface *controller = controller_get((e_controller_index)controller_index);
    if (activate) {
      if (!user_interface_controller_is_unsigned_in_user(controller) && !controller->is_signed_in_to_machine())
        controller->set_as_unsigned_in_user(activate);
    } else {
      if (user_interface_controller_is_unsigned_in_user(controller))
        controller->set_as_unsigned_in_user(activate);
    }
  }
}
static __forceinline void user_interface_controller_switch_to_offline(e_controller_index controller_index)
{
  s_user_interface_controller *controller = &g_user_interface_controller_globals.controllers[controller_index];
  controller->hopper_access_flags = 0;
  controller->flags = 0;
  controller->griefer = 0;
  controller->notification_pending = 0;
  controller->reserved10 = 0;
  memset(controller->reserved11, 0, sizeof(controller->reserved11));
}

extern "C" {

}

void user_interface_controller_update(void)
{
  unsigned long milliseconds = g_user_interface_globals.milliseconds.value;
  __lwsync();
  if (game_in_progress()) {
    s_user_interface_controller_globals *ui_globals = &g_user_interface_controller_globals;
    if (ui_globals->events[0].type) {
      g_window_manager.handle_global_controller_event(&ui_globals->events[0]);
      ui_globals->events[0] = ui_globals->events[1];
      ui_globals->events[1] = ui_globals->events[2];
      ui_globals->events[2] = ui_globals->events[3];
      memset(&ui_globals->events[3], 0, sizeof(ui_globals->events[3]));
    } else if (!ui_globals->inhibited) {
      event_manager_update(milliseconds);
    }
    for (long controller = 0; controller != -1; controller = controller_next(controller)) {
    }
  }
  controller_update_all(milliseconds);
}


void user_interface_controller_update_profile_error(e_controller_index controller_index)
{
  if (user_interface_controller_is_player_profile_valid(controller_index) &&
      (user_interface_controller_is_unsigned_in_user(controller_get(controller_index)) ||
       !simulation_starting_up() || tls_game_globals()->options.game_mode != 3)) {
    g_user_interface_controller_globals.error_posted[controller_index] = 1;
    g_gui_error_manager.post_error_with_custom_message(0x20014, 0, 0, controller_index, true, true);
  } else {
    g_user_interface_controller_globals.error_posted[controller_index] = 0;
  }
  user_interface_controller_update_network_properties(controller_index);
}

// @address 0x89490 ?down_msec@c_abstract_button@@QAAGXZ
// match: 0x470398: inline lets event_manager_update inline down_msec; out of line it shifts the register allocation
#if defined(_XBOX) && !defined(__clang__)
inline unsigned short c_abstract_button::down_msec()
#else
unsigned short c_abstract_button::down_msec()
#endif
{
  unsigned short milliseconds = 0;
  if (!latched() && access_valid())
    milliseconds = m_down_milliseconds;
  return milliseconds;
}

extern "C" {
// @address 0x119a508 g_event_manager_tab_active

}

static __forceinline void input_abstraction_get_input_state(e_controller_index controller_index, s_game_input_state **input_state)
{
  *input_state = &input_abstraction_globals.input_states[controller_index];
}

static __forceinline long event_manager_sign(long value)
{
  return value != 0 ? (value < 0 ? -1 : 1) : 0;
}

static __forceinline void event_manager_stick_moved(e_controller_index controller_index, const point2d *vector, e_controller_component component)
{
  s_event_record event;
  event.controller = controller_index;
  event.component = component;
  event.value = -1;
  if (event_manager_abs(vector->x) >= 15000) {
    event.type = event_manager_sign(vector->x) > 0 ? 8 : 6;
    g_window_manager.handle_global_controller_event(&event);
  } else if (event_manager_abs(vector->y) >= 15000) {
    event.type = event_manager_sign(vector->y) > 0 ? 5 : 7;
    g_window_manager.handle_global_controller_event(&event);
  }
}

// 0x470398
void event_manager_update(unsigned long milliseconds)
{
  for (long controller = 0; controller != -1; controller = controller_next(controller)) {
    e_controller_index controller_index = (e_controller_index)controller;
    s_game_input_state *input_state;
    input_abstraction_get_input_state(controller_index, &input_state);
    e_controller_component component = (e_controller_component)k_ui_component_left_stick;
    bool stop = true;
    point2d vector = { 0 };
    bool input_ignored = false;
    if (!input_ignored) {
      if (input_state->buttons[k_ui_button_a].down_frames() == 1) {
        event_manager_button_pressed(controller_index, k_ui_button_a);
      } else if (input_state->buttons[k_ui_button_b].down_frames() == 1) {
        event_manager_button_pressed(controller_index, k_ui_button_b);
      } else if (input_state->buttons[k_ui_button_x].down_frames() == 1) {
        event_manager_button_pressed(controller_index, k_ui_button_x);
      } else if (input_state->buttons[k_ui_button_y].down_frames() == 1) {
        event_manager_button_pressed(controller_index, k_ui_button_y);
      } else if (input_state->buttons[k_ui_button_left_shoulder].down_frames() == 1) {
        event_manager_button_pressed(controller_index, k_ui_button_left_shoulder);
      } else if (input_state->buttons[k_ui_button_right_shoulder].down_frames() == 1) {
        event_manager_button_pressed(controller_index, k_ui_button_right_shoulder);
      } else if (input_state->buttons[k_ui_button_start].down_frames() == 1) {
        event_manager_button_pressed(controller_index, k_ui_button_start);
      } else if (input_state->buttons[k_ui_button_back].down_frames() == 1) {
        event_manager_button_pressed(controller_index, k_ui_button_back);
      } else if (input_state->buttons[k_ui_button_left_trigger].down_frames() == 1) {
        event_manager_button_pressed(controller_index, k_ui_button_left_trigger);
      } else if (input_state->buttons[k_ui_button_right_trigger].down_frames() == 1) {
        event_manager_button_pressed(controller_index, k_ui_button_right_trigger);
      } else if (input_state->buttons[k_ui_button_left_thumb].down_frames() == 1) {
        event_manager_button_pressed(controller_index, k_ui_button_left_thumb);
      } else if (input_state->buttons[k_ui_button_right_thumb].down_frames() == 1) {
        event_manager_button_pressed(controller_index, k_ui_button_right_thumb);
      } else {
        stop = false;
        if (input_state->thumbsticks[0].x != 0 || input_state->thumbsticks[0].y != 0) {
          vector.x = event_manager_abs(input_state->thumbsticks[0].x) >= 15000 ? input_state->thumbsticks[0].x : 0;
          vector.y = event_manager_abs(input_state->thumbsticks[0].y) >= 15000 ? input_state->thumbsticks[0].y : 0;
        }
        if (vector.x == 0 && vector.y == 0) {
          long direction_count = 0;
          if (user_interface_controller_test_button_hold(_controller1,
                  input_state->buttons[k_ui_button_dpad_up].down_frames(),
                  input_state->buttons[k_ui_button_dpad_up].down_msec())) {
            vector.x = 0;
            vector.y = 1;
            component = (e_controller_component)k_ui_component_dpad_up;
            direction_count++;
          }
          if (user_interface_controller_test_button_hold(_controller1,
                  input_state->buttons[k_ui_button_dpad_down].down_frames(),
                  input_state->buttons[k_ui_button_dpad_down].down_msec())) {
            vector.x = 0;
            vector.y = -1;
            component = (e_controller_component)k_ui_component_dpad_down;
            direction_count++;
          }
          if (user_interface_controller_test_button_hold(_controller0,
                  input_state->buttons[k_ui_button_dpad_right].down_frames(),
                  input_state->buttons[k_ui_button_dpad_right].down_msec())) {
            vector.x = 1;
            vector.y = 0;
            component = (e_controller_component)k_ui_component_dpad_right;
            direction_count++;
          }
          if (user_interface_controller_test_button_hold(_controller0,
                  input_state->buttons[k_ui_button_dpad_left].down_frames(),
                  input_state->buttons[k_ui_button_dpad_left].down_msec())) {
            vector.x = -1;
            vector.y = 0;
            component = (e_controller_component)k_ui_component_dpad_left;
            direction_count++;
          }
          if (direction_count > 1) {
            vector.x = 0;
            vector.y = 0;
          }
          if (vector.x != 0 || vector.y != 0) {
            vector.x *= 32767;
            vector.y *= 32767;
          }
        }
        if (vector.x != 0 || vector.y != 0) {
          event_manager_tab((e_gamepad_stick)k_ui_left_stick, controller_index, &vector, milliseconds, component);
          g_event_manager_tab_active[controller] = true;
        } else if (g_event_manager_tab_active[controller]) {
          event_manager_tab((e_gamepad_stick)k_ui_left_stick, controller_index, &vector, milliseconds, component);
          g_event_manager_tab_active[controller] = false;
        }
        if (input_state->thumbsticks[1].x != 0 || input_state->thumbsticks[1].y != 0) {
          vector.x = event_manager_abs(input_state->thumbsticks[1].x) >= 15000 ? input_state->thumbsticks[1].x : 0;
          vector.y = event_manager_abs(input_state->thumbsticks[1].y) >= 15000 ? input_state->thumbsticks[1].y : 0;
          if (vector.x != 0 || vector.y != 0) {
            event_manager_tab((e_gamepad_stick)k_ui_right_stick, controller_index, &vector, milliseconds, component);
            event_manager_stick_moved(controller_index, &vector, component);
          }
        }
      }
    } else {
      stop = false;
    }
    if (stop)
      break;
  }
}

void user_interface_controller_reset(e_controller_index controller_index)
{
  c_controller_interface *controller = controller_get(controller_index);
  user_interface_controller_is_player_profile_valid(controller_index);
  user_interface_controller_switch_to_offline(controller_index);
  if (controller_in_use(controller))
    network_session_interface_remove_local_user(controller->get_user_index());
  if (user_interface_controller_is_unsigned_in_user(controller))
    user_interface_controller_activate_as_unsigned_in_user((short)controller_index, false);
  controller->reset_user_index();
}

// match: 0x46f9b8: #pragma function keeps bl memset out-of-line; without it c2 inlines the zeroing as a stw/bdnz store loop.
#pragma function(memset)
static __forceinline void user_interface_controller_configuration_initialize(s_player_configuration *configuration)
{
  memset(configuration, 0, sizeof(*configuration));
  memset(&configuration->client.appearance, 0, sizeof(configuration->client.appearance));
  configuration->host.team_index = 0;
  configuration->client.desired_team_index = 0;
  configuration->client.repeated_play_coefficient = -1;
}
// match: 0x46f9b8: restores intrinsic memset so the later name-clearing memset inlines as stores; without it c2 emits bl memset and swaps r24/r25.
#pragma intrinsic(memset)
static __forceinline bool user_interface_controller_user_valid(long user_index)
{
  return user_index >= 0 && user_index < 4;
}
static __forceinline unsigned long user_interface_controller_cheats(e_controller_index index)
{
  unsigned long result = 0;
  s_network_banhammer_controller_data *controller = &g_network_banhammer_globals.controllers[index];
  if (controller->flags & 1)
    result = g_network_banhammer_globals.controllers[index].cheats | g_network_banhammer_globals.cheats;
  return result;
}
static __forceinline long user_interface_controller_repeated_play(e_controller_index index)
{
  long result = 0;
  s_network_banhammer_controller_data *controller = &g_network_banhammer_globals.controllers[index];
  if (controller->flags & 1)
    result = controller->repeated_play_index;
  return result;
}
static __forceinline bool user_interface_controller_growth_banned(e_controller_index index)
{
  bool result = 0;
  s_network_banhammer_controller_data *controller = &g_network_banhammer_globals.controllers[index];
  if (controller->flags & 1)
    result = (controller->flags >> 1) & 1;
  return result;
}
static __forceinline void user_interface_controller_name_copy(wchar_t *destination, const wchar_t *source)
{
  for (long index = 0; index <= 15; index++, source++) {
    if ((destination[index] = *source) == 0)
      break;
  }
  destination[15] = 0;
}

// @address 0x46f9b8 ?user_interface_controller_get_player_configuration@@YAXW4e_controller_index@@PAUs_player_identifier@@PAUs_player_configuration@@@Z
void user_interface_controller_get_player_configuration(
    e_controller_index controller_index, s_player_identifier *identifier, s_player_configuration *configuration)
{
  s_user_interface_controller *ui_controller = &g_user_interface_controller_globals.controllers[controller_index];
  c_controller_interface *controller = controller_get(controller_index);
  c_player_profile_interface *profile = controller->get_player_profile_interface();
  long user_index = controller->get_user_index();
  bool user_valid = user_interface_controller_user_valid(user_index);
  if (identifier)
    controller->get_player_identifier(identifier);
  if (configuration) {
    unsigned long voice_settings = 0;
    e_controller_index local_controller;
    s_player_configuration *result = configuration;
    user_interface_controller_configuration_initialize(result);
    if (!user_valid || !network_session_interface_get_local_user_properties(user_index, &local_controller, configuration, &voice_settings))
      user_interface_controller_configuration_initialize(result);
    memset(result, 0, sizeof(result->client.name));
    user_interface_controller_name_copy(result->client.name, controller_display_name(controller));
    profile->get_player_appearance(&result->client.appearance);
    result->client.gamer_zone = profile->gamer_zone;
    result->client.gamer_region = profile->gamer_region;
    memcpy(&result->client.is_griefer, &ui_controller->griefer, sizeof(result->client.is_griefer));
    result->client.bungienet_user_role = (unsigned char)user_interface_controller_get_campaign_progress(profile);
    result->client.hopper_access_flags = ui_controller->hopper_access_flags;
    result->client.bungienet_user = ui_controller->flags;
    result->client.is_content_creator = ((c_controller_interface *)controller)->flags & 1;
    result->client.desired_team_index = -1;
    memcpy(&result->client.ready, &ui_controller->desires_veto, sizeof(result->client.ready));
    memcpy(&result->client.desires_rematch, &ui_controller->desires_rematch, sizeof(result->client.desires_rematch));
    result->client.cheat_flags = user_interface_controller_cheats(controller_index);
    result->client.player_flags = user_interface_controller_get_ban_flags(controller_index);
    result->client.repeated_play_coefficient = user_interface_controller_repeated_play(controller_index);
    result->client.experience_growth_banned = user_interface_controller_growth_banned(controller_index);
  }
}

static __forceinline bool user_interface_controller_progress_test(const c_player_profile_interface *profile, long table, long level, long difficulty)
{
  bool result = false;
  if ((level == -1 || level >= 0 && level < sizeof(profile->campaign.map_completed_flags[table])) && difficulty >= 0 && difficulty < 4)
    result = level != -1 ? (profile->campaign.map_completed_flags[table][level] & (1 << difficulty)) != 0 : false;
  return result;
}



// @address 0x470f68 user_interface_controller_get_campaign_progress
extern "C" long user_interface_controller_get_campaign_progress(c_player_profile_interface *configuration_profile)
{
  long result = 4;
  long map_count = levels_get_main_campaign_map_count(1);
  if (0 < map_count) {
    bool completed[4][64];
    bool any_completed[4];
    bool all_completed[4];
    for (long difficulty = 3; difficulty >= 0; difficulty--) {
      long level = 0;
      any_completed[difficulty] = false;
      all_completed[difficulty] = true;
      if (map_count <= 0)
        continue;
      do {
        completed[difficulty][level] = user_interface_controller_progress_test(configuration_profile, 0, level, difficulty) ||
            user_interface_controller_progress_test(configuration_profile, 1, level, difficulty);
        bool higher = false;
        any_completed[difficulty] = any_completed[difficulty] || completed[difficulty][level];
        for (long higher_difficulty = difficulty; higher_difficulty < 4; higher_difficulty++)
          higher = higher || completed[higher_difficulty][level];
        all_completed[difficulty] = all_completed[difficulty] && higher;
        level++;
      } while (level < map_count);
    }
    if (all_completed[3])
      result = 3;
    else if (all_completed[2])
      result = 2;
    else if (all_completed[1])
      result = 1;
    else if (all_completed[0])
      result = 0;
    else if (any_completed[3])
      result = 8;
    else if (any_completed[2])
      result = 7;
    else if (any_completed[1])
      result = 6;
    else if (any_completed[0])
      result = 5;
  }
  return result;
}

long c_main_menu_screen_widget::get_signed_in_controller_count(e_controller_index *first_controller_index)
{
  long count = 0;
  if (first_controller_index)
    *first_controller_index = _controller_none;
  for (long index = 0; index < 4; index++) {
    if (controller_in_use(controller_get((e_controller_index)index))) {
      if (first_controller_index && *first_controller_index == _controller_none)
        *first_controller_index = (e_controller_index)index;
      count++;
    }
  }
  return count;
}
