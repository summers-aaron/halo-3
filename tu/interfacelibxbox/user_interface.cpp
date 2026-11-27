// @address 0x50428 k_gui_data_vtable
#include "interface/gui_location_manager.h"
#include "memory/runtime_state_allocation.h"
// interfacelibxbox/user_interface.obj: from the records 0x88fa0 0x89550 0x895b0 0x8b188 0x8dcf8 0x8df10 0x8dff0 0x8e108 0x909d8 0x46e778 0x46e980 0x46eab8 0x46ec40 0x46edf0 0x46eec8 0x46f0b0 0x46f138 0x46f190 0x46f1f0 0x46f290 0x46f3a8 0x4a6330
// @compiler xdk6130
// @flags /TP /O2 /Oc

#define XVIDEO_MODE XDK_VIDEO_MODE
#define XLaunchNewImage XDK_XLaunchNewImage
#define XNotifyPositionUI XDK_XNotifyPositionUI
#include "bink/bink_playback_types.h"
#undef XNotifyPositionUI
#undef XLaunchNewImage
#undef XVIDEO_MODE

#include "main/main_flags.h"
#include "text/font_cache_lock.h"
#include "bink/bink_playback_globals.h"
#include "interface/gui_custom_bitmap_storage.h"
#include "memory/data.h"
#include "sound/game_looping_sound_data.h"
#include "interface/user_interface_clock.h"
#include "networking/logic/network_session_interface.h"
#include "cseries/crt_xdk.h"
#include "tag_files/tag_groups.h"
#include <string.h>
#include <stdlib.h>
#include "shell/shell.h"
#include "game/game_globals_storage.h"
#include "game/game_globals_interface_types.h"
#include "math/real_math_types.h"
#include "interface/interface_constants.h"
// @address 0x895b0 ?user_interface_tag_globals_try_and_get@@YAPBUs_user_interface_tag_globals@@XZ
// @address 0x8b188 ?interface_get_global_datasource@@YAPAVc_gui_data@@J@Z
// @address 0x909d8 ?get_alpha_is_locked_down@@YA_NXZ
// @address 0x46f190 ?get_alpha_is_internal_beta@@YA_NXZ
#include "cseries/cseries.h"
#include <ppcintrinsics.h>
#include "multithreading/synchronization.h"
#include "multithreading/synchronization_types.h"
#include "multithreading/synchronized_value.h"
#include "game/game_engine.h"
#include "interface/user_interface.h"
#include "interface/user_interface_memory.h"
#include "interface/user_interface_session.h"
#include "simulation/simulation.h"
#include "sound/game_sound.h"
#include "interface/user_interface_controller.h"
#include "interface/user_interface_messages.h"
#include "interface/user_interface_message_globals.h"
#include "interface/user_interface_networking.h"
#include "main/main.h"
#include "memory/memory_pool.h"
#include "rasterizer/rasterizer_main.h"
#include "text/font_cache.h"
#include "networking/session/network_session.h"
#include "bink/bink_playback.h"
#include "cache/cache_files_windows.h"
#include "interface/user_interface_main_menu_music.h"
#include "networking/logic/network_banhammer.h"
#include "saved_games/content_catalogue.h"
#include "text/font_cache_globals_types.h"
#include "tag_files/tag_types.h"
#include "memory/data_types.h"
#include "interface/gui_game_mode_types.h"
#include "networking/network_configuration_types.h"
#include "interface/gui_data.h"
#include "interface/c_gui_screen_widget.h"
#include "interface/user_interface_global_text_parsers.h"
#include "interface/user_interface_window_manager.h"
#include "render/render.h"
#include "game/players.h"
#include "saved_games/saved_game_files.h"
#include "interface/user_interface_error_manager.h"
#include "interface/controller_types.h"
#include "interface/controller_inline.h"
#include "networking/online/online_globals_types.h"
#include "interface/view_config_types.h"
#include "rasterizer/rasterizer_video_mode_types.h"
#include "cache/cache_file_globals.h"
#include "networking/network_configuration.h"
#include "rasterizer/rasterizer_render_globals_types.h"
#include "game/player_mapping_types.h"
#include "memory/tls_globals.h"
#include "scenario/scenario_tag.h"
#include "scenario/scenario.h"
#include "game/game_globals.h"
#include "interface/user_interface_data.h"
#include "cutscene/cinematic_globals_types.h"
#include "interface/user_interface_shared_globals.h"
#include "interface/c_window_manager.h"
#include "text/font_cache_thread_state_types.h"
#include "rasterizer/rasterizer.h"
#if !defined(_XBOX) || defined(__clang__)
#include "rasterizer/rasterizer_globals.h"
#endif

#include "interface/user_interface_externs.h"
#include "visibility/visibility_projections_and_volumes.h"
#include "shell/utility_drive.h"
// @address 0x8a1ead g_utility_drive_mounted
// @address 0x73a248 ?render_globals@c_rasterizer@@2Us_rasterizer_render_globals@@A
// @address 0x74b80c g_runtime_state_allocation

/* Finds the entry (of 32, 272 bytes each) whose identifier matches; null when none does. */



#include "interface/gui_screens/scoreboard/gui_screen_scoreboard_types.h"


extern "C" {



// @address 0x8c484f g_main_halt_requested



// match: 0x581930: SDK header adoption is blocked by this unit's current types/API arguments: '<unnamed-tag>' : 'enum' type redefinition.

// match: 0x5a0378: SDK header adoption is blocked by this unit's current types/API arguments: '<unnamed-tag>' : 'enum' type redefinition.

}
// match: intrinsic pragma selects hardware instruction form
// @address 0x70db04 XCustomRegisterDynamicActions
// @address 0x70db14 XCustomUnregisterDynamicActions
static __forceinline bool user_interface_online_dialog_active();















#if !defined(_XBOX) || defined(__clang__)
static long user_interface_elapsed_milliseconds(float elapsed_seconds)
{
    const float elapsed_milliseconds = elapsed_seconds * 1000.0f;
    if (port_d3d_vertical_blank_rate() <= 60)
        return (long)elapsed_milliseconds;

    // Preserve sub-millisecond time across high-rate passes.
    static float remainder;
    const float milliseconds = elapsed_milliseconds + remainder;
    const long whole_milliseconds = (long)milliseconds;
    remainder = milliseconds - (float)whole_milliseconds;
    return whole_milliseconds;
}
#endif

// @address 0x88fa0 ?user_interface_update@@YAXM@Z
void user_interface_update(float elapsed_seconds)
{
    s_user_interface_globals *clock = &g_user_interface_globals;
    __lwsync();
#if !defined(_XBOX) || defined(__clang__)
    _InterlockedExchangeAdd(&clock->milliseconds.value, user_interface_elapsed_milliseconds(elapsed_seconds));
#else
    _InterlockedExchangeAdd(&clock->milliseconds.value, (long)(elapsed_seconds * 1000.0f));
#endif
    unsigned long milliseconds = clock->milliseconds.peek();
    long player_count = tls_player_mapping_globals()->local_player_count;
    bool player_count_changed = clock->player_count != player_count;
    clock->player_count = player_count;
    if (g_main_halt_requested)
        main_halt_and_display_errors();
    game_globals_storage *game = tls_game_globals();
    bool in_progress = false;
    if (game && game->game_in_progress && !game->initializing && game->map_active)
        in_progress = true;
    if (in_progress)
    {
        user_interface_controller_update();
        saved_game_files_update();
        update_xenon_messages();
        game_engine_update_console_scoreboard();
        for (long controller_index = 0; controller_index < 4; controller_index++)
        {
            c_controller_interface *controller = controller_get((e_controller_index)controller_index);
            bool in_use = controller->is_signed_in_to_machine() || controller->is_unsigned_in_user();
            if (!in_use && !game_is_scoreboard_blocked(0))
                c_gui_screen_scoreboard::hide_scoreboard((e_controller_index)controller_index);
        }
        g_window_manager.update(milliseconds);
        gui_error_manager_update(&g_gui_error_manager, milliseconds);
        if (g_pregame_setup_manager.get_pending_setup()->mode != -1 && simulation_starting_up() && tls_game_globals()->options.game_mode == 3)
        {
            gui_pregame_setup_manager_apply_pending_setup(&g_pregame_setup_manager, g_pregame_setup_manager.get_pending_setup());
            g_pregame_setup_manager.get_pending_setup()->mode = -1;
        }
        user_interface_networking_start_game_when_ready_update();
        user_interface_network_dialogs_update();
        user_interface_networking_join_update();
        window_manager_add_widgets_to_render_list_recursive();
        user_interface_session_update_string_verification();
        if (user_interface_online_dialog_active())
        {
            long now = clock->milliseconds.peek();
            __lwsync();
            _InterlockedExchange(&clock->last_input.value, now);
        }
        if (user_interface_is_idle_timeout_expired())
            bink_playback_play_idle_movie();
        g_user_interface_globals.main_menu_music.update();
        if (tls_game_globals()->options.game_mode == 3)
        {
            __lwsync();
            _InterlockedExchange(&clock->shell_last_input.value, milliseconds);
        }
        do
        {
            if (tls_game_globals()->options.game_mode == 3)
            {
                if (player_count_changed)
                    user_interface_set_player_count_mode();
                if (!user_interface_squad_exists())
                    break;
                if (network_life_cycle_squad_session_get_ui_game_mode() != 0)
                    break;
            }
            else if (tls_game_globals()->options.game_mode != 1)
                break;
            if (!clock->error_posted && bink_is_idle_and_drive_empty())
            {
                g_gui_error_manager.post_error_with_custom_message(0x20087, 0, 0, k_any_controller, false, true);
                clock->error_posted = true;
            }
        } while (false);
    }
    if (clock->exit_start_time != -1 && GetTickCount() > (unsigned long)(clock->exit_start_time + 2000))
    {
        if (g_utility_drive_mounted)
            XFlushUtilityDrive();
        _internal_halt_render_thread_and_lock_resources("..\\interface\\user_interface.cpp", 400);
        XLaunchNewImage(0, 0);
    }
}

const s_user_interface_tag_globals *user_interface_tag_globals_try_and_get(void)
{
    void *result = 0;
    if (global_scenario && global_game_globals && global_game_globals->interface_tags.count != 0) {
        s_game_globals_interface_tags *interface_tags = (s_game_globals_interface_tags *)global_game_globals->interface_tags.address;
        if (interface_tags) {
            int index = -1;
            switch (global_scenario->type) {
            case 0:
                index = interface_tags->campaign.index;
                break;
            case 1:
                index = interface_tags->multiplayer.index;
                break;
            case 2:
                index = interface_tags->mainmenu.index;
                break;
            }
            if (index != -1)
                result = tag_get('wgtz', index);
        }
    }
    return (const s_user_interface_tag_globals *)result;
}

c_gui_data *interface_get_global_datasource(long identifier)
{
    c_gui_data *result = 0;
    for (int i = 0; i < g_user_interface_globals.count; i++) {
        if (g_user_interface_globals.entries[i].m_name == identifier) {
            result = (c_gui_data *)&g_user_interface_globals.entries[i];
            break;
        }
    }
    return result;
}

extern "C" {
// @address 0x72c4e1 g_user_interface_main_menu_music_active
#if !defined(_XBOX) || defined(__clang__)
bool g_user_interface_main_menu_music_active = true;
#endif
// @address 0x730434 g_interface_authoring_to_target_dimensions_scaling
real_vector2d g_interface_authoring_to_target_dimensions_scaling = { 1.0f, 1.0f };
}

// @address 0x8c48d8 ?g_user_interface_is_alpha@@3_NA
bool g_user_interface_is_alpha;

// @address 0x72c4e3 ?g_eula_accepted@@3_NA
#if !defined(_XBOX) || defined(__clang__)
bool g_eula_accepted = true;
#endif

// @address 0x8df10 ?update@c_user_interface_main_menu_music@@QAAXXZ
void c_user_interface_main_menu_music::update()
{
  update_game_shell_music_state();
  int value = m_game_shell_state;
  if (!g_user_interface_main_menu_music_active)
    value = 0;
  bool flags_set = bink_globals.active && bink_globals.enabled;
  if (flags_set)
    value = 0;
  if (g_user_interface_globals.exit_start_time != -1)
    value = 0;
  bool locked = render_is_load_recently_complete();
  if (locked)
    value = 0;
  c_gui_screen_widget *screen = g_window_manager.get_screen_by_name(_console_window, 0x100b6);
  if (screen != 0)
    value = 0;
  m_requested_state = value;
  change_state_update();
}



// @address 0x8e108 ?change_state_update@c_user_interface_main_menu_music@@AAAXXZ
void c_user_interface_main_menu_music::change_state_update()
{
  if (m_current_state == m_requested_state)
    return;
  int now = g_user_interface_globals.milliseconds.peek();
  if (m_state_change_time != 0)
  {
    int delay = user_interface_main_menu_music_get_delay();
    if (now - m_state_change_time <= delay)
      return;
  }
  if (m_requested_state == 1)
  {
    const s_user_interface_shared_globals *globals = ::user_interface_shared_tag_globals_try_and_get();
    if (globals != 0 && globals->looping_sound.index != -1)
      m_looping_sound_index = (int)unattached_looping_sound_start(globals->looping_sound.index, -1, 1.0f);
  }
  else if (m_looping_sound_index != -1)
  {
    game_looping_sound_datum *datum = datum_get<game_looping_sound_datum>(game_looping_sound_data, m_looping_sound_index);
    datum->flags.set(1, true);
    m_looping_sound_index = -1;
    m_fade_out_start_time = (int)GetTickCount();
  }
  m_state_change_time = now;
  m_current_state = m_requested_state;
}



// @address 0x8dff0 ?update_game_shell_music_state@c_user_interface_main_menu_music@@AAAXXZ
void c_user_interface_main_menu_music::update_game_shell_music_state()
{
  if (global_scenario == 0 || global_scenario->type != 2) {
    m_game_shell_state = 0;
    return;
  }
  c_gui_screen_widget *screen = c_gui_location_manager::get_location_screen();
  if (screen == 0)
    return;
  e_gui_location state = screen->get_gui_location();
  switch (state) {
  case 0:
    if (g_user_interface_globals.exit_start_time != -1)
      return;
    m_game_shell_state = 1;
    return;
  case 1:
    break;
  case 2:
  case 3:
  case 4:
  case 5:
    m_game_shell_state = 0;
    return;
  default:
    return;
  }
  bool any = false;
  for (long i = 0; i < 16; i++) {
    if (user_interface_session_is_player_valid(i) && !user_interface_session_is_local_peer_player(i))
      any = true;
  }
  if (user_interface_squad_in_or_after_countdown())
    any = true;
  if (!any)
    return;
  m_game_shell_state = 0;
}


bool get_alpha_is_locked_down(void)
{
    return g_user_interface_is_alpha != 0 && (g_network_configuration.alpha_locked_down == 0 || g_eula_accepted == 0);
}


static __forceinline bool user_interface_online_dialog_active()
{
    return g_message_globals.flag != 0;
}




// @address 0x46ec40 user_interface_dispose_from_old_map
extern "C" void user_interface_dispose_from_old_map(void)
{
  c_message_globals *messages = &g_message_globals;
  c_message_globals::s_message_queue_node *node = messages->m_next_read;
  while (node->m_message != 0) {
    c_message *message = const_cast<c_message *>(node->m_message);
    messages->m_next_read = node->m_next;
    c_message_globals::s_message_queue_node *previous = node->m_previous;
    node->m_next->m_previous = previous;
    previous->m_next = node->m_next;
    node->m_message = 0;
    c_message_globals::s_message_queue_node *after_write = messages->m_next_write->m_next;
    after_write->m_previous = node;
    messages->m_next_write->m_next = node;
    c_message_globals::s_message_queue_node *write = messages->m_next_write;
    node->m_next = after_write;
    node->m_previous = write;
    message->~c_message();
    user_interface_free(message);
    node = messages->m_next_read;
  }
  {
    s_font_cache_thread_state lock;
    lock.active = 0;
    if (g_font_cache_globals == 0) {
      internal_critical_section_enter(37);
      lock.active = 1;
      g_font_cache_globals = &g_internal_font_cache_globals;
    }
    user_interface_dispose_custom_bitmaps(&g_gui_custom_bitmap_storage_data);
    for (long window_index = 0; window_index < k_number_of_windows; window_index++) {
      while (g_window_manager.m_screen_count[window_index].peek() > 0)
        g_window_manager.dispose_screen(g_window_manager.m_screens[window_index][0]);
    }
    g_window_manager.m_script_thread_index = -1;
    font_cache_unlock(&lock);
  }
  g_user_interface_globals.main_menu_music.reset();
  for (long i = 0; i < g_user_interface_globals.count; i++) {
    c_gui_data *entry = &g_user_interface_globals.entries[i];
    entry->dispose();
  }
  g_user_interface_globals.count = 0;
}

extern "C" {
// @address 0x8c4928 g_bink_last_activity_time

}
static __forceinline bool user_interface_idle_screen_is_open()
{
    c_window_manager *stacks = &g_window_manager;
    c_gui_screen_widget *screen;
    return stacks->m_screen_count[4].peek() > 0
        && (screen = stacks->m_screens[4][stacks->m_screen_count[4].peek() - 1]) != 0
        && screen->get_name() == 0x10080;
}

// @address 0x8dcf8 user_interface_is_idle_timeout_expired
extern "C" bool user_interface_is_idle_timeout_expired(void)
{
    bool timeout_expired = false;
    game_globals_storage *game = tls_game_globals();
    bool game_active = false;
    if (game != 0 && game->game_in_progress && !game->initializing && game->map_active)
        game_active = true;
    bool in_main_menu = game_active && game->options.game_mode == 3;
    if (in_main_menu) {
        bool copy_idle = true;
        if (cache_files_windows_get_active_copy_progress(0))
            copy_idle = false;
        if (copy_idle) {
            if (!user_interface_squad_exists()) {
                bool bink_busy = bink_globals.active && bink_globals.enabled;
                if (!bink_busy) {
                    if (!g_message_globals.flag) {
                        const s_user_interface_shared_globals *shared = ::user_interface_shared_tag_globals_try_and_get();
                        long timeout_seconds;
                        if (shared != 0)
                            timeout_seconds = shared->timeout;
                        else
                            timeout_seconds = 75;
                        s_user_interface_globals *clock
                            = &g_user_interface_globals;
                        long timeout_ms = timeout_seconds * 1000;
                        unsigned long milliseconds = clock->milliseconds.peek();
                        unsigned long last_input = clock->last_input.peek();
                        unsigned long last = MAX(g_bink_last_activity_time, last_input);
                        unsigned long elapsed = milliseconds - last;
                        bool open = user_interface_idle_screen_is_open();
                        long fade = 0;
                        const s_user_interface_shared_globals *shared2 = ::user_interface_shared_tag_globals_try_and_get();
                        if (shared2 != 0)
                            fade = shared2->fade_time;
                        if (render_is_load_recently_complete())
                            fade = 3000;
                        if (elapsed < (unsigned long)(timeout_ms - fade) || !open)
                            g_user_interface_main_menu_music_active = true;
                        else
                            g_user_interface_main_menu_music_active = false;
                        if (elapsed >= (unsigned long)timeout_ms && open)
                            timeout_expired = true;
                    }
                }
            }
        }
    }
    return timeout_expired;
}

static __forceinline bool user_interface_unit_cinematic_in_progress()
{
  bool result = false;
  if (cinematic_globals)
    result = cinematic_globals->in_progress;
  return result;
}

static __forceinline long user_interface_unit_pick_index()
{
  if (tls_game_globals()->options.game_mode != 3) {
    if (user_interface_unit_cinematic_in_progress())
      return 2;
    if (tls_player_mapping_globals()->local_player_count > 1)
      return 2;
  }
  return 7;
}

// @address 0x46edf0 user_interface_set_player_count_mode
extern "C" void user_interface_set_player_count_mode(void)
{
  long index = user_interface_unit_pick_index();
  long values[9] = { 5, 1, 9, 4, 0, 8, 6, 2, 10 };
  XNotifyPositionUI(values[index]);
}

// @address 0x46f0b0 user_interface_get_projection_plane_distances
extern "C" void user_interface_get_projection_plane_distances(float *first, float *second, float *third)
{
  const s_user_interface_shared_globals *globals = ::user_interface_shared_tag_globals_try_and_get();
  *first = globals ? globals->first : 10.0f;
  *second = globals ? globals->second : 100.0f;
  *third = globals ? globals->third : 1000.0f;
}

// @address 0x46f138 get_alpha_is_external_beta
extern "C" bool get_alpha_is_external_beta(void)
{
  long mode = g_network_configuration.alpha_locked_down;
  return g_user_interface_is_alpha && (mode == 0 || mode == 3 || !g_eula_accepted);
}

bool get_alpha_is_internal_beta(void)
{
  long mode = g_network_configuration.alpha_locked_down;
  return g_user_interface_is_alpha && (mode == 0 || mode == 3 || mode == 2 || !g_eula_accepted);
}

inline bool get_is_alpha_version() { return g_user_interface_is_alpha; }

// @address 0x46f1f0 get_alpha_custom_games_disabled
extern "C" bool get_alpha_custom_games_disabled(void)
{
  bool result = false;
  if (get_is_alpha_version() && get_alpha_is_internal_beta() && g_network_configuration.custom_games_disabled)
    result = true;
  else if (!get_alpha_is_internal())
    result = true;
  return result;
}

// @address 0x89550 ?user_interface_shared_tag_globals_try_and_get@@YAPBUs_user_interface_shared_globals@@XZ
struct s_user_interface_shared_globals const * user_interface_shared_tag_globals_try_and_get(void)
{
  const s_user_interface_shared_globals *result = 0;
  const s_user_interface_tag_globals *definition = user_interface_tag_globals_try_and_get();
  if (definition && definition->shared_globals.index != -1)
    result = (const s_user_interface_shared_globals *)global_tag_instances[(unsigned short)definition->shared_globals.index].definition;
  return result;
}

// @address 0x46f290 ?user_interface_initialize_global_datasources@@YAXXZ
void user_interface_initialize_global_datasources(void)
{
    const s_user_interface_shared_globals *tag = ::user_interface_shared_tag_globals_try_and_get();
    g_user_interface_globals.count = 0;
    if (tag != 0) {
        for (int i = 0; i < tag->global_datasources.count; i++) {
            int index = ((s_tag_reference *)tag->global_datasources.address)[i].index;
            if (index != -1) {
                int *p = TAG_GET('dsrc', int, index);
                g_user_interface_globals.entries[g_user_interface_globals.count] = c_gui_tag_datasource(index);
                g_user_interface_globals.entries[g_user_interface_globals.count].initialize(*p);
                g_user_interface_globals.count++;
            }
        }
    }
}

// match: intrinsic pragma selects hardware instruction form

// @address 0x46eab8 ?user_interface_initialize_for_new_map@@YAXXZ
void user_interface_initialize_for_new_map(void)
{
    unsigned char zero;
    int clock_value;
    g_message_globals.initialize_for_new_map();
    g_gui_error_manager.initialize_for_new_map();
    if (tls_game_globals()->options.game_mode == 3) {
        g_gui_custom_bitmap_storage_data.buffer_size = 0xa00000;
        g_gui_custom_bitmap_storage_data.buffer = user_interface_malloc_tracked(0xa00000, 0, 0);
    }
    zero = 0;
    g_window_manager.m_fade = visibility_projections_and_volumes_zero;
    g_window_manager.m_fade_in = zero;
    g_window_manager.m_fade_held = zero;
    // match: 0x46eab8: volatile pointer cast preserves memory access order
    clock_value = g_user_interface_globals.milliseconds.peek();
    __lwsync();
    // match: 0x46eab8: volatile pointer cast preserves memory access order
    _InterlockedExchange(&g_user_interface_globals.new_map_milliseconds.value, clock_value);
    __lwsync();
    // match: 0x46eab8: volatile pointer cast preserves memory access order
    _InterlockedExchange(&g_user_interface_globals.shell_last_input.value, 0);
    user_interface_initialize_global_datasources();
    user_interface_set_player_count_mode();
    if (tls_game_globals()->options.game_mode == 2)
        network_banhammer_test_map_signature(cache_files_get_header()->signature);
    g_user_interface_networking_globals.start_pending = 0;
    content_catalogue_close_items_for_scenario();
}

// @address 0x46e778 ?user_interface_initialize@@YAXXZ
void user_interface_initialize(void)
{
    s_data_array *first_array;
    s_data_array *second_array;
    c_allocation_base *heap = (c_allocation_base *)&g_runtime_state_allocation;
    memset(g_parse_manager.m_parsers, 0, sizeof(g_parse_manager.m_parsers));
    user_interface_register_global_text_parsers();
    g_message_globals.notification_handle = XNotifyCreateListener(111);
    XCustomRegisterDynamicActions();
    first_array = data_new("active screens list", 32, sizeof(s_screen_handle), 0, heap);
    g_window_manager.m_active_screens = first_array;
    first_array->valid = 1;
    data_delete_all(first_array);
    internal_critical_section_enter(20);
    second_array = data_new("gui bitmap storage", 32, 148, 0, heap);
    g_gui_custom_bitmap_storage_data.data = second_array;
    second_array->valid = 1;
    data_delete_all(second_array);
    internal_critical_section_leave(20);
    g_gui_error_manager.initialize();
    __lwsync();
    _InterlockedExchange(&g_user_interface_globals.flags.value, 0);
    __lwsync();
    _InterlockedExchange(&g_user_interface_globals.active_controller.value, -1);
    user_interface_controller_initialize();
    __lwsync();
    _InterlockedExchange(&g_user_interface_globals.campaign_id.value, -1);
    __lwsync();
    _InterlockedExchange(&g_user_interface_globals.map_id.value, -1);
    __lwsync();
    _InterlockedExchange(&g_user_interface_globals.campaign_difficulty.value, -1);
    __lwsync();
    _InterlockedExchange(&g_user_interface_globals.shell_last_input.value, 0);
    g_user_interface_globals.exit_start_time = -1;
    g_user_interface_globals.player_count = 0;
    g_user_interface_globals.error_posted = 0;
    memset(&g_user_interface_networking_globals, 0, sizeof(g_user_interface_networking_globals));
    g_user_interface_networking_globals.controller = -1;
    g_user_interface_networking_globals.screen_name = -1;
    g_user_interface_networking_globals.error = -1;
    g_user_interface_networking_globals.dialog = -1;
    g_user_interface_networking_globals.last_join_error = -1;
    g_user_interface_globals.flags.set_bit(3, true);
}

static __forceinline const view_config *get_view_bounds(long window,long count,bool wide) {
 const view_config (*aspect)[4] = k_interface_splitscreen_view_configs[wide];
 return &aspect[count-1][window];
}
// @address 0x46f3a8 user_interface_get_number_of_render_windows
// match: 0x46f3a8: pointer __restrict qualifier prevents alias reload
extern "C" void user_interface_get_number_of_render_windows(long user,long *__restrict horizontal,long *vertical) {
 long count = players_get_active_user_count();
 if(count>1) {
  long window = player_get_user_from_controller(user_interface_controller_get_rumble_enabled(user));
  if(window<0 || window>=count) window=0;
  const view_config *view=get_view_bounds(window,count,rasterizer_main_get_video_mode()->fIsWideScreen!=0);
  *horizontal = view->x1-view->x0>0.5f ? 1:2;
  *vertical = view->y1-view->y0>0.5f ? 1:2;
 } else {
  *horizontal=1;
  *vertical=1;
 }
}

struct s_synchronization_globals;
// match: intrinsic pragma selects hardware instruction form
// @address 0x4a6330 user_interface_dispose_custom_bitmaps
extern "C" void user_interface_dispose_custom_bitmaps(s_gui_custom_bitmap_storage_globals *storage)
{
  c_data_iterator<gui_custom_bitmap_storage_slot> iterator;
  bool freed;
  do {
    internal_critical_section_enter(20);
    iterator.begin(storage->data);
    freed = false;
    if (iterator.next()) {
      gui_custom_bitmap_storage_slot *slot = iterator.get_datum();
      if (slot->state != 1) {
        c_gui_custom_bitmap_storage_item *item = &slot->item;
        gui_custom_bitmap_storage_release_texture(item);
        internal_critical_section_enter(20);
        void *buffer = (void *)item->m_requested_size;
        if (buffer != 0) {
          user_interface_free(buffer);
          item->m_requested_size = 0;
        }
        item->m_initialized = false;
        _InterlockedDecrement(&g_synch_globals.critical_sections[20].lock_count.value);
        RtlLeaveCriticalSection(&g_synch_globals.critical_sections[20].critical_section);
        ::datum_delete(storage->data, iterator.get_index());
      }
      freed = true;
    }
    _InterlockedDecrement(&g_synch_globals.critical_sections[20].lock_count.value);
    RtlLeaveCriticalSection(&g_synch_globals.critical_sections[20].critical_section);
    if (freed)
      SwitchToThread();
  } while (freed);
  if (storage->buffer != 0) {
    user_interface_free(storage->buffer);
    storage->buffer = 0;
  }
}

// @address 0x46e980 ?user_interface_dispose@@YAXXZ
struct s_memory_pool;
void user_interface_dispose(void)
{
  if (network_life_cycle_globals_get()->initialized != 0) {
    c_network_session *session = network_life_cycle_globals_get()->manager.m_target_squad_session;
    if (session != 0 && !session->disconnected()) {
      session->m_disconnect_reestablish_state = 1;
      session->disconnect();
    }
  }
  memset(&g_user_interface_networking_globals, 0, sizeof(g_user_interface_networking_globals));
  g_user_interface_networking_globals.controller = -1;
  g_user_interface_networking_globals.screen_name = -1;
  g_user_interface_networking_globals.error = -1;
  g_user_interface_networking_globals.dialog = -1;
  g_user_interface_networking_globals.last_join_error = -1;
  content_catalogue_dispose_all();
  internal_critical_section_enter(20);
  if (g_gui_custom_bitmap_storage_data.data != 0) {
    data_dispose(g_gui_custom_bitmap_storage_data.data);
    g_gui_custom_bitmap_storage_data.data = 0;
  }
  internal_critical_section_leave(20);
  s_data_array *active_screens = g_window_manager.m_active_screens;
  if (active_screens != 0) {
    data_dispose(active_screens);
    g_window_manager.m_active_screens = 0;
  }
  XCustomUnregisterDynamicActions();
  void *handle = g_message_globals.notification_handle;
  if (handle != 0 && handle != (void *)-1) {
    CloseHandle(handle);
    g_message_globals.notification_handle = 0;
  }
  user_interface_unregister_global_text_parsers();
  if (g_user_interface_memory_pool != 0) {
    memory_pool_dispose(g_user_interface_memory_pool);
    g_user_interface_memory_pool = 0;
  }
}


// @address 0x46eec8 user_interface_render_windows
extern "C" void user_interface_render_windows(
    long user_index, long controller_index, long render_flags, const rectangle2d *window_bounds, long render_target, bool is_screenshot)
{
    c_rasterizer::begin_high_quality_blend();
    c_rasterizer::set_render_target(0, (c_rasterizer::e_surface)render_target);
    c_rasterizer::set_render_target(1, c_rasterizer::_surface_none);
    if (VALID_INDEX(render_flags, 5))
        window_manager_render_window(&g_window_manager, (e_window_index)render_flags, controller_index, window_bounds, is_screenshot);
    if (render_flags == 4 && TEST_BIT((unsigned long)g_user_interface_globals.flags.peek(), 0))
    {
        struct user_interface_safe_area
        {
            float margin;
            unsigned long color;
        } safe_areas[] = {
            { 0.0f, 0xffff0000 },
            { 0.05f, 0xffff7f00 },
            { 0.075f, 0xffffff00 },
            { 0.1f, 0xff00ff00 }
        };
        short width = (short)c_rasterizer::render_globals.width;
        short height = (short)c_rasterizer::render_globals.height;
        for (unsigned long index = 0; index < NUMBEROF(safe_areas); index++)
        {
            point2d points[5];
            points[0].x = (short)(safe_areas[index].margin * width);
            points[0].y = (short)(safe_areas[index].margin * height);
            points[1].x = points[0].x;
            points[1].y = (short)((1.0f - safe_areas[index].margin) * height - 1.0f);
            points[2].x = (short)((1.0f - safe_areas[index].margin) * width - 1.0f);
            points[2].y = (short)((1.0f - safe_areas[index].margin) * height - 1.0f);
            points[3].x = (short)((1.0f - safe_areas[index].margin) * width - 1.0f);
            points[3].y = points[0].y;
            points[4].x = points[0].x;
            points[4].y = points[0].y;
            c_rasterizer::draw_debug_linestrip2d(points, NUMBEROF(points), safe_areas[index].color);
        }
    }
}

typedef char ui_datasource_registry_entries_offset[(offsetof(s_user_interface_globals, entries) == 0x40) ? 1 : -1];
typedef char ui_datasource_registry_count_offset[(sizeof(void *) != 4 || (offsetof(s_user_interface_globals, count) == 0x2240)) ? 1 : -1];

s_user_interface_globals g_user_interface_globals;

extern "C" {
// @address 0x115fb70 g_gui_custom_bitmap_storage_data
s_gui_custom_bitmap_storage_globals g_gui_custom_bitmap_storage_data;
}
