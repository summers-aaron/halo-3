// interfacelibxbox/user_interface_window_manager.obj: from the records 0x8b958 0x8c860 0x8c9a8 0x8ca78 0x8cce8 0x8d8e8 0x8dbd0 0xa0198 0x18e918 0x18ec50 0x1913a0 0x2119c0 0x46f610 0x47a4e8 0x47ec30 0x47f008 0x47f168 0x47f500 0x47f640 0x47f6d8 0x47fb10 0x4818d0 0x481e40 0x482078 0x4820f8 0x482250 0x482418 0x482480 0x4825a8 0x482628 0x482698 0x4827b8 0x4829c0 0x482a40 0x499c08 0x4ae088 0x70a550 0x70cc60
// @address 0x73a248 ?render_globals@c_rasterizer@@2Us_rasterizer_render_globals@@A
// @address 0x11401e8 g_controller_globals
// @address 0xf283c8 g_network_session_interface_local_users
// @address 0x1143b50 g_gui_error_manager
// @address 0x7c341c g_lobby_location_manager
#define XVIDEO_MODE XDK_VIDEO_MODE
#include "interface/gui_screens/start_menu/panes/settings_appearance_emblem/start_menu_settings_appearance_emblem.h"
#include "interface/gui_screens/start_menu/panes/settings_appearance_colors/start_menu_settings_appearance_colors.h"
#include "interface/gui_screens/start_menu/gui_start_menu_hq_transfers_item_selected.h"
#include <xtl.h>
#undef XVIDEO_MODE
#include "interface/c_player_marketplace.h"
#include "interface/window_manager_dependencies.h"
#include "tag_files/string_ids.h"
#include "interface/gui_screens/start_menu/start_menu_string_ids.h"
#include "interface/gui_screens/player_select/gui_player_select_screen_widget.h"
#include "interface/gui_screens/game_details/gui_game_details_string_ids.h"
#include "networking/logic/network_search_types.h"
#include "interface/gui_location_manager.h"
#include "networking/session/network_session_interface_user_externs.h"
#include "interface/gui_custom_bitmap_storage_item.h"
#include "interface/gui_screen_construction.h"
#include "main/loading.h"
// @flags /TP /O2 /Oc
#include "text/font_cache_lock.h"
#include "text/font_cache_types.h"
#include "interface/gui_widget_hierarchy.h"
#include "interface/user_interface_squad_session.h"
#include "interface/gui_custom_bitmap_storage.h"
#include "interface/user_interface_utilities.h"
#include "cseries/async.h"
#include "rasterizer/rasterizer_dx9_dynamic_geometry.h"
#include "interface/gui_network_session_advertisement_types.h"
#include "memory/tls_globals.h"
#include "scenario/scenario.h"
#include "math/real_math_types.h"
#include "interface/controller_types.h"
#include "interface/c_deletable.h"
#include <string.h>
#include <stddef.h>
#include "memory/basic_buffer.h"
#include "memory/static_arrays.h"
#include "multithreading/synchronized_value.h"
#include "memory/data_types.h"
#include "tag_files/tag_types.h"
#include "shell/shell.h"
#include "game/game_globals_storage.h"
#include "multithreading/synchronization.h"
#include "cseries/crt_xdk.h"
#include "multithreading/synchronization_types.h"
#include "game/game.h"
#include "interface/gui_screens/scoreboard/gui_screen_scoreboard.h"
#include "interface/user_interface.h"
#include "interface/user_interface_controller_globals.h"
#include "interface/user_interface_memory.h"
#include "interface/user_interface_session.h"
#include "networking/logic/network_session_interface.h"
#include "simulation/simulation.h"
#include "interface/c_player_profile_storage.h"
#if !defined(_XBOX) || defined(__clang__)
#include "interface/interface_constants.h"
#endif
#include "interface/user_interface_messages.h"
#include "interface/user_interface_networking.h"
#include "rasterizer/rasterizer_main.h"
#include "rasterizer/rasterizer.h"
#include "sound/sound_classes.h"
#include "text/font_cache.h"
#include "interface/user_interface_window_manager.h"
#include "interface/gui_screens/campaign/gui_screen_campaign_options.h"
#include "interface/gui_screens/campaign/gui_screen_download_widget.h"
#include "interface/gui_screens/director/gui_screen_observer_camera_list.h"
#include "interface/gui_screens/game_details/gui_game_details.h"
#include "interface/gui_screens/start_menu/panes/settings_camera/start_menu_settings_camera.h"
#include "interface/gui_screens/start_menu/panes/settings_display/start_menu_settings_display.h"
#include "interface/gui_screens/gui_screen_in_progress.h"
#include "interface/gui_screens/start_menu/panes/hq_screenshots/gui_start_menu_hq_screenshots_submenu.h"
#include "interface/gui_screens/start_menu/panes/settings_appearance_model/start_menu_settings_appearance_model.h"
#include "interface/gui_screens/pregame_lobby/gui_screen_pregame_lobby_theater.h"
#include "interface/gui_screens/director/gui_screen_take_screenshot.h"
#include "interface/window_manager_network_api.h"
#include "networking/online/online_friend_xenon.h"
#include "memory/data.h"
#include "bink/bink_playback.h"
#include "game/players.h"
#include "saved_games/content_catalogue.h"
#include "interface/user_interface_error_manager.h"
#include "interface/gui_screens/alert/gui_screen_alert.h"
#include "interface/gui_screens/saved_film/gui_screen_saved_film_control_pad.h"
#include "sound/game_sound_definitions.h"
#include "rasterizer/rasterizer_video_mode_types.h"
#include "interface/user_interface_sound_effect_types.h"
#include "interface/gui_data.h"
#include "interface/gui_screens/pregame_lobby/gui_screen_pregame_selection.h"
#include "interface/game_tag_parser.h"
#include "interface/c_controller.h"
#include "game/game_time_types.h"
#include "interface/gui_widget.h"
#include "interface/window_manager_screen_header_types.h"
#include "text/font_cache_globals_types.h"
#include "interface/ui_allocation_marker_types.h"
#include "cache/cache_files.h"
#include "interface/c_gui_widget.h"
#include "main/game_preferences.h"
#include "interface/user_interface_allocation.h"
#include "interface/c_player_profile.h"
#include "multithreading/thread_tls.h"
static const float k_window_manager_depth_epsilon = 0.0001f;
// @address 0x4825a8 ?get_screen_below@c_window_manager@@QAAPAVc_gui_screen_widget@@W4e_window_index@@PAV2@@Z
// @address 0x47fb10 ?allocate_named_screen@c_window_manager@@AAAPAVc_gui_screen_widget@@J@Z
#include <ppcintrinsics.h>

#include "interface/gui_widget_types.h"
#include "interface/c_gui_bitmap_widget.h"
#include "interface/gui_model_widget_types.h"
#include "interface/user_interface_text_render.h"


struct s_event_record;
#include "interface/c_window_manager.h"




template<typename t_type> class c_robust_pointer;
#include "interface/gui_render_data_size_types.h"
#pragma pack(push, 4)
#include "interface/c_gui_screen_widget.h"

#pragma pack(pop)
#include "interface/gui_screens/boot_betrayer/gui_screen_boot_betrayer.h"

#include "text/font_cache_thread_state_types.h"

#include "interface/user_interface_dialog_globals.h"

#include "interface/gui_screens/dialog/gui_screen_dialog.h"
#include "interface/gui_screens/start_menu/panes/game_editor/start_menu_game_editor.h"

#include <new>

static __forceinline void window_manager_critical_section_enter(long index);
static __forceinline void window_manager_critical_section_leave(long index);
template<typename t_type> static __forceinline void store_ordered(t_type *destination, t_type value);


// @address 0x8973c0 g_loading_cache_controller_storage

#include "saved_games/content_catalogue_types.h"
#include "interface/user_interface_controller.h"

#include "rasterizer/rasterizer_render_globals_types.h"


#include <stdlib.h>



#include "sound/sound_classes_types.h"
#include "sound/sound_class_definitions.h"
#include "sound/sound_globals_reference_types.h"
#include "game/game_globals.h"
#include "bink/bink_playback_types.h"




#include "interface/gui_pregame_setup_manager_types.h"
#include "interface/gui_pregame_setup_manager.h"

#include "interface/c_gui_text_widget.h"

// the window manager's render pass for one window: snapshot each screen's assembled render list under the
// font cache lock, then draw the entries that fall between the projection planes
struct s_window_manager_render_snapshot
{
    __declspec(align(4)) unsigned char render_data_buffer[k_render_data_size_large];
    long buffer_size;
    s_window_manager_render_list_entry render_list[384];
    long current_count;
};
class window_manager_render_font_cache_lock
{
public:
    __forceinline window_manager_render_font_cache_lock()
    {
        m_state.active = 0;
        if (g_font_cache_globals == 0) {
            window_manager_critical_section_enter(37);
            m_state.active = 1;
            g_font_cache_globals = &g_internal_font_cache_globals;
        }
    }
    __forceinline ~window_manager_render_font_cache_lock() { font_cache_unlock(&m_state); }
private:
    s_font_cache_thread_state m_state;
};

template<typename t_type> struct s_ordered
{
    __forceinline s_ordered(t_type initial_value) { t_type *pointer = &value; *pointer = initial_value; }
    t_type value;
};

#include "interface/virtual_keyboard_task.h"

class __declspec(novtable) wm_string_verify_task : public c_string_verify_task
{
public:
    __forceinline wm_string_verify_task(const char *file, long line) : c_string_verify_task(file, line) {}
private:
#if !defined(_WIN64) && !defined(__LP64__)
    char m_data[0x848 - sizeof(c_string_verify_task)];
#endif
};

#include "networking/logic/storage/network_http_buffer_downloader.h"

#include "networking/logic/storage/dynamic_matchmaking_hopper_statistics_types.h"
#include "networking/logic/storage/network_http_blf_simple_downloader.h"
#include "interface/gui_screens/pregame_lobby/gui_screen_postgame_lobby.h"


// screens with out-of-line constructors
#include "interface/gui_screens/start_menu/gui_screen_start_menu.h"
typedef char window_manager_game_editor_size[(sizeof(void *) != 4 || ((sizeof(c_start_menu_game_editor) == 0x1e0d8))) ? 1 : -1];
#include "interface/gui_screens/start_menu/panes/hq_service_record_file_share/gui_start_menu_hq_service_record_file_share.h"
typedef char window_manager_dialog_screen_size[(sizeof(void *) != 4 || ((sizeof(c_gui_dialog_screen_widget) == 6848))) ? 1 : -1];

#include "interface/gui_screens/game_options/gui_screen_game_options.h"

// the start menu panes
#include "interface/gui_screens/start_menu/start_menu_pane_screen_widget.h"

#include "interface/gui_screens/start_menu/panes/game_campaign/start_menu_game_campaign.h"
#include "interface/gui_screens/start_menu/panes/game_multiplayer/start_menu_game_multiplayer.h"
#include "interface/gui_screens/start_menu/panes/settings/start_menu_settings.h"
#include "interface/gui_screens/start_menu/panes/game_saved_film/start_menu_game_saved_film.h"
#include "interface/gui_screens/start_menu/panes/settings_controls/start_menu_settings_controls.h"
#include "interface/gui_screens/start_menu/panes/settings_controls_button/start_menu_settings_controls_button.h"
#include "interface/gui_screens/start_menu/panes/settings_controls_thumbstick/start_menu_settings_controls_thumbstick.h"
#include "interface/gui_screens/start_menu/gui_start_menu_hq_transfers.h"

#include "interface/gui_screens/start_menu/panes/hq_service_record/gui_start_menu_hq_service_record.h"

#include "interface/gui_screens/start_menu/panes/settings_appearance/start_menu_settings_appearance.h"

// other screens
#include "interface/gui_screens/screen_error_dialog_ok.h"

#include "interface/gui_screens/main_menu/gui_screen_main_menu.h"

#include "interface/gui_screens/start_menu/panes/hq_service_record_file_share/gui_start_menu_hq_service_record_file_share_choose_category.h"
#include "interface/gui_screens/start_menu/panes/hq_service_record_file_share/gui_start_menu_hq_service_record_file_share_choose_item.h"
#include "interface/gui_screens/start_menu/panes/hq_service_record_file_share/gui_start_menu_hq_service_record_file_share_item_selected.h"

#include "interface/gui_screens/campaign/gui_screen_campaign_options.h"

#include "interface/gui_screens/campaign/gui_screen_campaign_select_difficulty.h"

#include "interface/gui_screens/campaign/gui_screen_campaign_select_level.h"

#include "interface/gui_screens/pregame_lobby/gui_screen_pregame_lobby_campaign.h"

#include "interface/gui_screens/pregame_lobby/gui_screen_pregame_lobby_matchmaking.h"

#include "interface/gui_screens/pregame_lobby/gui_screen_pregame_lobby_multiplayer.h"

#include "interface/gui_screens/pregame_lobby/gui_screen_pregame_lobby_mapeditor.h"

#include "interface/gui_screens/pregame_lobby/gui_screen_pregame_switch_lobby.h"
#include "interface/gui_screens/pregame_lobby/advanced_screen/gui_screen_matchmaking_advanced_options.h"
#include "interface/gui_screens/game_browser/gui_game_browser.h"

#include "interface/gui_screens/gui_screen_maximum_party_size.h"

#include "interface/gui_screens/alpha/gui_screen_alpha_legal.h"

#include "interface/gui_screens/alpha/gui_screen_alpha_motd.h"

#include "interface/gui_screens/carnage_report/gui_screen_carnage_report.h"

#include "interface/gui_screens/scoreboard/gui_screen_scoreboard_types.h"

#include "interface/gui_screens/matchmaking/gui_screen_matchmaking_searching.h"

#include "interface/gui_screens/matchmaking/gui_screen_matchmaking_match_found.h"

#include "interface/gui_screens/sandbox_menu/gui_screen_sandbox_menu.h"

#include "interface/gui_screens/sandbox_editor/gui_screen_object_properties_menu.h"

#include "interface/gui_screens/terminals/gui_screen_terminals.h"

#include "interface/event_manager_types.h"
#include "interface/user_interface_clock.h"


#include "interface/c_gui_screen_codeless_widget.h"

#include "networking/session/network_session_parameters.h"
#include "networking/logic/network_life_cycle_session_helpers.h"
// user_interface_free: critical section 19 around memory_pool_block_free
#include "memory/memory_pool_types.h"
#include "interface/user_interface_memory_globals.h"
class window_manager_ui_memory_lock
{
public:
    __forceinline window_manager_ui_memory_lock() { window_manager_critical_section_enter(19); }
    __forceinline ~window_manager_ui_memory_lock() { window_manager_critical_section_leave(19); }
};

namespace {
static __forceinline long screenCount(const c_window_manager *stacks, e_window_index window_index)
{
    return stacks->m_screen_count[window_index];
}
static __forceinline bool window_manager_screen_has_parent(c_gui_screen_widget *screen)
{
    c_gui_widget *widget = screen;
    return widget->get_parent_screen() != 0 && widget->get_parent_screen() != screen;
}
static __forceinline bool window_manager_screen_is_busy(c_gui_screen_widget *screen)
{
    c_gui_screen_in_progress *state = static_cast<c_gui_screen_in_progress *>(screen);
    bool result;
    if (screen->running_in_codeless_mode())
        result = true;
    else if (state->m_show_progress)
        result = true;
    else
        result = window_manager_screen_has_parent(screen);
    return result;
}
}
static __forceinline bool window_game_in_progress()
{
    game_globals_storage *globals = tls_game_globals();
    bool result = false;
    if (globals && globals->game_in_progress && !globals->initializing && globals->map_active)
        result = true;
    return result;
}
static __forceinline bool window_game_is_ui_shell()
{
    return tls_game_globals()->options.game_mode == 3;
}

// @address 0x8c860 ?get_responding_screen@c_window_manager@@QAAPAVc_gui_screen_widget@@W4e_controller_index@@PAW4e_window_index@@@Z
c_gui_screen_widget *c_window_manager::get_responding_screen(e_controller_index controller_index, e_window_index *window_index)
{
    c_gui_screen_widget *screen = 0;
    if (window_index)
        *window_index = _console_window;
    if (window_game_in_progress() && window_game_is_ui_shell()) {
        screen = get_responding_screen_in_window(controller_index, _console_window);
    } else {
        screen = get_responding_screen_in_window(k_any_controller, _console_window);
        if (!screen) {
            if (controller_index == 255) {
                for (long window = 0; window < 4; ++window) {
                    screen = get_responding_screen_in_window(controller_index, (e_window_index)window);
                    if (screen) {
                        if (window_index)
                            *window_index = (e_window_index)window;
                        break;
                    }
                }
            } else {
                e_window_index window = (e_window_index)player_get_user_from_controller(controller_index);
                screen = get_responding_screen_in_window(controller_index, window);
                if (window_index)
                    *window_index = window;
            }
        } else {
            if (!screen->controller_can_drive(controller_index))
                screen = 0;
        }
    }
    return screen;
}

// @address 0x8c9a8 ?get_responding_screen_in_window@c_window_manager@@QAAPAVc_gui_screen_widget@@W4e_controller_index@@W4e_window_index@@@Z
c_gui_screen_widget *c_window_manager::get_responding_screen_in_window(e_controller_index controller_index, e_window_index window_index)
{
    c_window_manager *screen_stacks = this;
    c_gui_screen_widget *result = 0;
    for (long screen_index = screenCount(screen_stacks, window_index) - 1; screen_index >= 0; --screen_index) {
        if (screen_stacks->m_screens[window_index][screen_index]->get_responds_to_controller_events() &&
            !screen_stacks->m_screens[window_index][screen_index]->transitioning_out()) {
            if (screen_stacks->m_screens[window_index][screen_index]->controller_can_drive(controller_index)) {
                result = screen_stacks->m_screens[window_index][screen_index];
                break;
            } else {
                break;
            }
        }
    }
    return result;
}

// @address 0x8d8e8 window_manager_get_alert_screen
extern "C" c_gui_screen_widget *window_manager_get_alert_screen(e_window_index window_index)
{
    c_gui_screen_widget *result = 0;
    c_window_manager *window_manager = &g_window_manager;
    c_gui_screen_widget *screen = window_manager->get_topmost_screen(window_index);
    while (screen != 0) {
        long name = screen->m_name;
        bool alert_screen = name == _string_id_gui_alert_nonblocking || name == _string_id_gui_alert_toast ||
            name == _string_id_gui_alert_ingame_full || name == _string_id_gui_alert_ingame_split;
        if (alert_screen) {
            result = screen;
            break;
        }
        screen = window_manager->get_screen_below(window_index, screen);
    }
    return result;
}

// @address 0x8dbd0 ?get_screen_by_name@c_window_manager@@QAAPAVc_gui_screen_widget@@W4e_window_index@@J@Z
c_gui_screen_widget *c_window_manager::get_screen_by_name(e_window_index window_index, long name)
{
    c_window_manager *screen_stacks = this;
    c_gui_screen_widget *result = 0;
    long screen_count = screen_stacks->m_screen_count[window_index].value;
    __lwsync();
    for (long screen_index = screen_count - 1; screen_index >= 0; screen_index--) {
        if (screen_stacks->m_screens[window_index][screen_index]->m_name == name) {
            result = screen_stacks->m_screens[window_index][screen_index];
            break;
        }
    }
    return result;
}

typedef char window_manager_bitmap_item_offset[(sizeof(void *) != 4 || ((offsetof(gui_custom_bitmap_storage_slot, item) == 12))) ? 1 : -1];
typedef char window_manager_bitmap_initialized_offset[(sizeof(void *) != 4 || ((offsetof(c_gui_custom_bitmap_storage_item, m_initialized) == 60))) ? 1 : -1];

// @address 0x18e918 window_manager_dispose_unused_bitmaps
extern "C" void window_manager_dispose_unused_bitmaps(s_data_array **bitmap_data)
{
    bool needs_rescan = true;
    while (needs_rescan) {
        c_data_iterator<gui_custom_bitmap_storage_slot> iterator;
        internal_critical_section_enter(20);
        s_data_array *data = *bitmap_data;
        iterator.begin(data);
        needs_rescan = false;
        while (iterator.next()) {
            gui_custom_bitmap_storage_slot *slot = iterator.get_datum();
            if (slot->reference_count == 0 && slot->state != 1) {
                c_gui_custom_bitmap_storage_item *item = &slot->item;
                gui_custom_bitmap_storage_release_texture(item);
                internal_critical_section_enter(20);
                if (item->m_requested_size != 0) {
                    user_interface_free((const void *)item->m_requested_size);
                    item->m_requested_size = 0;
                }
                item->m_initialized = false;
                window_manager_critical_section_leave(20);
                datum_delete(*bitmap_data, iterator.get_index());
                needs_rescan = true;
                break;
            }
        }
        window_manager_critical_section_leave(20);
    }
}

// @address 0x18ec50 ?is_screen_queued_to_be_loaded@c_window_manager@@SA_NJW4e_window_index@@@Z
bool c_window_manager::is_screen_queued_to_be_loaded(long screen_name, e_window_index window_index)
{
    bool result = false;
    if (!user_interface_message_queue_is_empty())
    {
        const c_message *message = 0;
        bool has_message = false;
        do
        {
            has_message = user_interface_messaging_get_next_message(-1, k_any_controller, window_index, &message);
            if (has_message)
            {
                if (message->get_type() == _ui_load_screen && message->get_screen_name() == screen_name)
                {
                    result = true;
                    break;
                }
            }
        } while (has_message);
    }
    return result;
}

// The bodies of internal_critical_section_enter and internal_critical_section_leave.
static __forceinline void window_manager_critical_section_enter(long index)
{
    long volatile *owner_thread;
    RtlEnterCriticalSection(&g_synch_globals.critical_sections[index].critical_section);
    owner_thread = &g_synch_globals.critical_sections[index].locking_thread.value;
    _InterlockedExchange(owner_thread, get_current_thread_index());
    _InterlockedIncrement(owner_thread + 1);
}

static __forceinline void window_manager_critical_section_leave(long index)
{
    if (index != -1) {
        _InterlockedDecrement(&g_synch_globals.critical_sections[index].lock_count.value);
        RtlLeaveCriticalSection(&g_synch_globals.critical_sections[index].critical_section);
    }
}

static __forceinline e_controller_index window_manager_controller_next_switch(e_controller_index index)
{
    e_controller_index next;
    switch (index + 1) {
    case 0: next = _controller0; break;
    case 1: next = _controller1; break;
    case 2: next = _controller2; break;
    case 3: next = _controller3; break;
    default: next = _controller_index_none; break;
    }
    return next;
}

static __forceinline e_controller_index window_manager_controller_next(e_controller_index index)
{
    e_controller_index next = _controller_index_none;
    if ((unsigned int)index <= 2)
        next = (e_controller_index)(index + 1);
    return next;
}

// @address 0x1913a0 window_manager_render_fade_overlay
extern "C" void window_manager_render_fade_overlay(c_window_manager *window_manager)
{
    float fade = window_manager->m_fade;
    if (fade > 0.0f) {
        if (fade >= 1.0f)
            fade = 1.0f;
        int alpha = (int)(fade * 255.0f);
        int color = alpha << 24;
        short width;
        short height;
        if (rasterizer_main_get_video_mode()->fIsWideScreen != 0) {
            width = (short)c_rasterizer::render_globals.width;
            height = (short)c_rasterizer::render_globals.height;
        } else {
            width = 640;
            height = 480;
        }
        point2d points[4];
        points[0].x = 0;
        points[0].y = 0;
        points[1].x = width;
        points[1].y = 0;
        points[2].x = width;
        points[2].y = height;
        points[3].x = 0;
        points[3].y = height;
        rasterizer_quad_screenspace(points, color, 0, 0, 0);
    }
}

// @address 0x2119c0 window_manager_is_profile_screen_present
extern "C" unsigned char window_manager_is_profile_screen_present(void)
{
    c_gui_screen_widget *rank_dialog = g_window_manager.get_screen_by_name(_console_window, _string_id_spartan_rank_dialog);
    c_gui_screen_widget *milestone_dialog = g_window_manager.get_screen_by_name(_console_window, _string_id_spartan_milestone_dialog);
    int result;
    if ((rank_dialog == 0 || rank_dialog->transitioning_out()) && (milestone_dialog == 0 || milestone_dialog->transitioning_out()) &&
        c_window_manager::is_screen_queued_to_be_loaded(_string_id_spartan_rank_dialog, _console_window) == 0 &&
        c_window_manager::is_screen_queued_to_be_loaded(_string_id_spartan_milestone_dialog, _console_window) == 0)
        result = 0;
    else
        result = 1;
    return result;
}

// @address 0x46f610 ?user_interface_controller_initialize@@YAXXZ
void user_interface_controller_initialize()
{
#if defined(_XBOX) && !defined(__clang__)
    // match: 0x46f610: use r28 for the cache stores and controller stores at r28+0x110 and r28+0x1c4
    loading_map_path_cache &cache = g_loading_cache_controller_storage.cache;
    s_user_interface_controller_globals *controller_globals = &g_loading_cache_controller_storage.controllers;
#else
    loading_map_path_cache &cache = g_loading_map_path_cache;
    s_user_interface_controller_globals *controller_globals = &g_user_interface_controller_globals;
#endif
    memset(controller_globals, 0, sizeof(*controller_globals));
    for (e_controller_index controller_index = _controller0; controller_index != _controller_index_none;
         controller_index = window_manager_controller_next_switch(controller_index))
        user_interface_controller_reset(controller_index);
    controller_globals->inhibited = false;
    window_manager_critical_section_enter(21);
    for (e_controller_index catalogue_index = _controller0; catalogue_index != _controller_index_none;
         catalogue_index = window_manager_controller_next(catalogue_index)) {
        content_catalogue_reset_controller_state(&content_catalogues().catalogues[catalogue_index], catalogue_index);
        content_catalogues().storage_devices[catalogue_index].set_controller_index(catalogue_index);
    }
    content_catalogues().initialized = true;
    window_manager_critical_section_leave(21);
    for (e_controller_index controller_index = _controller0; controller_index != _controller_index_none;
         controller_index = window_manager_controller_next(controller_index)) {
        c_controller_interface *controller = &g_controller_globals.controllers[controller_index];
        long index = (long)(controller - g_controller_globals.controllers);
        s_player_marketplace_user *user = &g_player_marketplace_users[index];
        controller->m_user = user;
        g_controller_globals.controllers[controller_index].player_profile.controller_index = controller_index;
        user->controller_index = index;
        g_controller_globals.controller_windows[controller_index] = 4;
    }
    g_controller_globals.controller_event_pending = true;
    xenon_player_profile_storage_initialize();
    cache.campaign = -1;
    cache.map = -1;
    memset(cache.path, 0, sizeof(cache.path));
}

// @address 0x47f008 window_manager_play_input_sound
extern "C" void window_manager_play_input_sound(int override_index, int input_type, unsigned int input_component)
{
    switch (input_type) {
    case _event_type_tab_up:
    case _event_type_tab_down:
    case _event_type_alt_stick_up:
    case _event_type_alt_stick_down:
    case _event_type_alt_tab_up:
    case _event_type_alt_tab_down:
        user_interface_play_sound(_ui_sound_vertical_navigation, override_index);
        return;
    case _event_type_tab_left:
    case _event_type_tab_right:
    case _event_type_alt_stick_left:
    case _event_type_alt_stick_right:
    case _event_type_alt_tab_left:
    case _event_type_alt_tab_right:
        user_interface_play_sound(_ui_sound_horizontal_navigation, override_index);
        return;
    case _event_type_button_press:
        switch (input_component) {
        case 0:
            user_interface_play_sound(_ui_sound_a_button, override_index);
            return;
        case _controller_component_button_b:
            user_interface_play_sound(_ui_sound_b_button, override_index);
            return;
        case _controller_component_dpad_up:
        case _controller_component_dpad_down:
            user_interface_play_sound(_ui_sound_vertical_navigation, override_index);
            return;
        case _controller_component_dpad_left:
        case _controller_component_dpad_right:
            user_interface_play_sound(_ui_sound_horizontal_navigation, override_index);
            return;
        case _controller_component_button_start:
            user_interface_play_sound(_ui_sound_start_button, override_index);
            return;
        case _controller_component_button_back:
            user_interface_play_sound(_ui_sound_back_button, override_index);
            return;
        case _controller_component_button_x:
            user_interface_play_sound(_ui_sound_x_button, override_index);
            return;
        case _controller_component_button_y:
            user_interface_play_sound(_ui_sound_y_button, override_index);
            return;
        case 5:
            user_interface_play_sound(_ui_sound_left_bumper, override_index);
            return;
        case 4:
            user_interface_play_sound(_ui_sound_right_bumper, override_index);
            return;
        case 6:
            user_interface_play_sound(_ui_sound_left_trigger, override_index);
            return;
        case 7:
            user_interface_play_sound(_ui_sound_right_trigger, override_index);
            return;
        default:
            return;
        }
    default:
        return;
    }
}


static __forceinline int window_manager_read_count(volatile long *count)
{
    int result = *count;
    __lwsync();
    return result;
}
// @address 0x47f500 window_manager_is_controller_ui_active
extern "C" bool window_manager_is_controller_ui_active(c_window_manager *window_manager, e_controller_index controller_index)
{
    bool found = false;
    c_controller_interface *controller = &g_controller_globals.controllers[controller_index];
    bool eligible = controller->is_signed_in_to_machine() || (_rotl(controller->flags, 30) & 1) != 0;
    if (eligible && controller->user_index != -1) {
        long window_index = 0;
        do {
            if (window_index >= k_number_of_windows)
                break;
            for (long screen_index = 0;
                 !found && screen_index < window_manager_read_count(&window_manager->m_screen_count[window_index].value);
                 ++screen_index) {
                c_gui_screen_widget *screen = window_manager->m_screens[window_index][screen_index];
                if (screen && screen->controller_can_drive(controller_index)) {
                    long name = screen->m_name;
                    if (name != _string_id_observer_camera_list_screen && name != _string_id_observer_camera_list_splitscreen &&
                        name != _player_select_string_scoreboard && name != _string_id_scoreboard_half)
                        found = true;
                }
            }
            ++window_index;
        } while (!found);
    }
    return found;
}

static __forceinline bool window_manager_bink_playback_active()
{
    return bink_globals.active && bink_globals.enabled;
}
static const float k_window_manager_sound_class_fade_time = 0.0f;
// @address 0x8cce8 window_manager_update_sound_fade
extern "C" void window_manager_update_sound_fade(c_window_manager *state, unsigned long time)
{
    float gain = 0.0f;
    if (time)
    {
        float duration = state->m_fade_in ? 1900.0f : -2000.0f;
        float delta = (float)(time - state->m_last_update_time) / duration;
        state->m_fade = PIN(state->m_fade + delta, 0.0f, 1.0f);
    }
    if (state->m_fade_in && user_interface_squad_exists() && !state->m_fade_held && !user_interface_squad_in_or_after_countdown())
        state->m_fade_in = false;
    bool muted = window_manager_bink_playback_active() && window_game_is_ui_shell();
    if (!muted)
        gain = 1.0f - state->m_fade;
    for (short class_index = 0; class_index < 54; ++class_index)
    {
        if (sound_class_is_valid(class_index))
        {
            sound_globals_reference *sound_globals = global_game_globals->sound_globals.count() != 0
                ? global_game_globals->sound_globals.begin()
                : 0;
            sound_classes *definition = TAG_GET('sncl', sound_classes, sound_globals->sound_classes.index);
            if (!(definition->classes[class_index].flags & 1))
                sound_class_set_gain_by_index(class_index, real_gain_to_decibels(gain), k_window_manager_sound_class_fade_time, 3);
        }
    }
}

// @address 0x47f640 ?locate_screen_definition_tag_by_name@c_window_manager@@SAJJ@Z
long c_window_manager::locate_screen_definition_tag_by_name(long identifier)
{
    long result = -1;
    const s_user_interface_tag_globals *definition = user_interface_tag_globals_try_and_get();
    if (definition) {
        for (long i = 0; i < definition->screen_widgets.count(); i++) {
            const s_tag_reference *reference = &definition->screen_widgets[i];
            if (reference->index != -1 &&
                TAG_GET('scn3', const s_core_widget_definition, reference->index)->name == identifier)
            {
                result = reference->index;
                break;
            }
        }
    }
    return result;
}

// @address 0x47a4e8 window_manager_add_widgets_to_render_list_recursive
extern "C" void window_manager_add_widgets_to_render_list_recursive(void)
{
    long name = -1;
    long description = -1;
    if (g_user_interface_networking_globals.join_in_progress) {
        name = _string_id_joining_title;
        description = _string_id_joining_message;
    } else if (g_user_interface_networking_globals.start_pending) {
        name = _string_id_preparing_title;
        description = _string_id_preparing_message;
    } else if (async_test_content_catalogue_busy()) {
        name = _string_id_verifying_session_strings_title;
        description = _string_id_verifying_session_strings_message;
    }
    if (name != -1) {
        if (!g_window_manager.get_screen_by_name(_console_window, _string_id_in_progress)) {
            if (name == _string_id_joining_title && g_user_interface_networking_globals.last_join_error != -1) {
                g_gui_error_manager.clear_error(g_user_interface_networking_globals.last_join_error, k_any_controller, 0);
                g_user_interface_networking_globals.last_join_error = -1;
            }
            void *storage = user_interface_malloc_tracked(sizeof(c_load_in_progress_screen_message), 0, 0);
            c_load_in_progress_screen_message *message = storage
                ? new (storage) c_load_in_progress_screen_message(k_any_controller, _console_window, name, description)
                : 0;
            if (message)
                user_interface_messaging_post(message);
        }
    } else {
        c_gui_screen_widget *screen = g_window_manager.get_screen_by_name(_console_window, _string_id_in_progress);
        if (screen && !window_manager_screen_is_busy(screen))
            g_window_manager.dispose_screen(screen);
    }
}

// @address 0x482078 ?get_screen_by_screen_index@c_window_manager@@QAAPAVc_gui_screen_widget@@J@Z
c_gui_screen_widget *c_window_manager::get_screen_by_screen_index(long screen_index)
{
    c_gui_screen_widget *result = 0;
    c_font_cache_scope_lock font_lock;
    c_window_manager *screen_stacks = this;
    if (screen_stacks->m_active_screens) {
        s_screen_handle *screen_handle = static_cast<s_screen_handle *>(datum_try_and_get(screen_stacks->m_active_screens, screen_index));
        if (screen_handle)
            result = screen_handle->screen;
    }
    return result;
}

// @address 0x4820f8 window_manager_close_screen_animated
extern "C" void window_manager_close_screen_animated(c_window_manager *window_manager, c_gui_screen_widget *screen, int animation_mode)
{
    if (!screen->transitioning_out()) {
        if (animation_mode != 0) {
            int bit;
            bit = -1;
            switch (animation_mode) {
            case 0: bit = -1; break;
            case 1: bit = 6; break;
            case 2: bit = 8; break;
            case 4: bit = 13; break;
            case 3: bit = 12; break;
            case 5: bit = 33; break;
            case 6: bit = 35; break;
            }
            screen->start_animation_at_time((e_animation_state)bit, -1, true);
        } else {
            window_manager->dispose_screen(screen);
        }
    }
}

// @address 0x482250 ?dispose_screen@c_window_manager@@QAAXPAVc_gui_screen_widget@@@Z
void c_window_manager::dispose_screen(c_gui_screen_widget *screen)
{
    bool screen_found = false;
    c_font_cache_scope_lock font_lock;
    c_window_manager *screen_stacks = this;
    if (!screen_stacks->m_active_screens)
        return;
    long screen_index = screen->get_screen_index();
    s_screen_handle *screen_handle = static_cast<s_screen_handle *>(datum_try_and_get(screen_stacks->m_active_screens, screen_index));
    if (screen_handle) {
        screen_handle->screen = 0;
        datum_delete(screen_stacks->m_active_screens, screen_index);
    }
    for (long window_index = k_number_of_windows - 1; window_index >= 0 && !screen_found; --window_index) {
        for (long screen_position = 0; screen_position < screenCount(screen_stacks, (e_window_index)window_index); ++screen_position) {
            if (screen_stacks->m_screens[window_index][screen_position] == screen) {
                for (long position = screen_position; position < screenCount(screen_stacks, (e_window_index)window_index) - 1; ++position)
                    screen_stacks->m_screens[window_index][position] = screen_stacks->m_screens[window_index][position + 1];
                __lwsync();
                _InterlockedExchangeAdd(&screen_stacks->m_screen_count[window_index].value, -1);
                screen_found = true;
                break;
            }
        }
    }
    if (screen->get_parent() != 0)
        gui_widget_remove_child(screen->get_parent(), screen);
    screen->dispose_child_screens();
    screen->dispose();
    screen->~c_gui_screen_widget();
    user_interface_free(screen);
}

// @address 0x482418 ?reset_screens@c_window_manager@@QAAXXZ
void c_window_manager::reset_screens()
{
    c_window_manager *screen_stacks = this;
    for (long window_index = 0; window_index < k_number_of_windows; window_index++) {
        while (screenCount(screen_stacks, (e_window_index)window_index) > 0)
            dispose_screen(screen_stacks->m_screens[window_index][0]);
    }
}

// @address 0x482480 ?close_all_screens@c_window_manager@@QAAXPBQAVc_gui_screen_widget@@J@Z
void c_window_manager::close_all_screens(c_gui_screen_widget *const *excluded_screens, long excluded_screen_count)
{
    for (long window_index = 0; window_index < k_number_of_windows; ++window_index) {
        for (c_gui_screen_widget *screen = get_topmost_screen((e_window_index)window_index);
             screen != 0; screen = get_screen_below((e_window_index)window_index, screen)) {
            bool excluded = false;
            for (long i = 0; i < excluded_screen_count; ++i)
                excluded = excluded || screen == excluded_screens[i];
            if (!excluded && !screen->transitioning_out())
                screen->transition_out_with_transition_type(0, _screen_transition_normal);
        }
    }
}

c_gui_screen_widget *c_window_manager::get_screen_below(e_window_index window_index, c_gui_screen_widget *screen)
{
    c_window_manager *screen_stacks = this;
    c_gui_screen_widget *result = 0;
    c_gui_screen_widget *previous_screen = 0;
    long screen_index = 0;
    long screen_count = screen_stacks->m_screen_count[window_index].value;
    __lwsync();
    if (screen_count > 0) {
        do {
            c_gui_screen_widget *current_screen = screen_stacks->m_screens[window_index][screen_index];
            if (current_screen == screen) {
                result = previous_screen;
                break;
            }
            previous_screen = current_screen;
            screen_index++;
            screen_count = screen_stacks->m_screen_count[window_index].value;
            __lwsync();
        } while (screen_index < screen_count);
    }
    return result;
}

// @address 0x482628 ?get_screen_above@c_window_manager@@QAAPAVc_gui_screen_widget@@W4e_window_index@@PAV2@@Z
c_gui_screen_widget *c_window_manager::get_screen_above(e_window_index window_index, c_gui_screen_widget *screen)
{
    c_window_manager *screen_stacks = this;
    c_gui_screen_widget *result = 0;
    c_gui_screen_widget *previous_screen = 0;
    long screen_count = screen_stacks->m_screen_count[window_index].value;
    __lwsync();
    for (long screen_index = screen_count - 1; screen_index >= 0; screen_index--) {
        c_gui_screen_widget *current_screen = screen_stacks->m_screens[window_index][screen_index];
        if (current_screen == screen) {
            result = previous_screen;
            break;
        }
        previous_screen = current_screen;
    }
    return result;
}
__forceinline bool c_gui_pregame_setup_manager::handle_dialog_result(const c_dialog_result_message *dialog_result_message)
    {
        if (dialog_result_message->get_dialog_result() == 0) {
            user_interface_networking_set_session_advertisement(m_advertisement_mode);
            if (m_advertisement_mode == _network_advertise_xbox_live_open ||
                m_advertisement_mode == _network_advertise_xbox_live_friends_only ||
                m_advertisement_mode == _network_advertise_xbox_live_invite_only) {
                global_preferences_set_xbox_live_private_privacy_setting(m_advertisement_mode);
            }
            if (dialog_result_message->get_dispose_on_success_screen_index() != -1) {
                c_gui_screen_widget *screen = g_window_manager.get_screen_by_screen_index(dialog_result_message->get_dispose_on_success_screen_index());
                if (screen)
                    screen->transition_out_with_transition_type(0, _screen_transition_normal);
            }
        }
        return true;
    }

static __forceinline bool window_manager_storage_device_selection_handle_dialog_result(const c_dialog_result_message *dialog_result_message)
{
    bool handled = false;
    if (dialog_result_message->get_screen_name() == _string_id_controller_no_storage_device_chosen) {
        if (dialog_result_message->get_dialog_result() == 0)
            content_catalogue_display_device_selection_guide_interface(dialog_result_message->get_controller());
        handled = true;
    }
    return handled;
}

// @address 0x482698 ?window_manager_handle_dialog_result_message@@YA_NPBVc_dialog_result_message@@@Z
bool window_manager_handle_dialog_result_message(const c_dialog_result_message *dialog_result_message)
{
    bool handled = false;
    switch (dialog_result_message->get_screen_name()) {
    case _string_id_networking:
        handled = user_interface_networking_dialogs_handle_results(dialog_result_message);
        break;
    case _string_id_pregame_setup_manager:
        handled = g_pregame_setup_manager.handle_dialog_result(dialog_result_message);
        break;
    case _string_id_joining_message:
        handled = user_interface_networking_handle_join_dialog_result(dialog_result_message);
        break;
    case _string_id_controller_no_storage_device_chosen:
        handled = window_manager_storage_device_selection_handle_dialog_result(dialog_result_message);
        break;
    }
    return handled;
}
static __forceinline short window_manager_get_render_depth_bias(c_gui_widget *widget)
{
    return (short)widget->get_core_definition()->font;
}
// @address 0x4827b8 window_manager_assemble_widget_render_data
extern "C" void window_manager_assemble_widget_render_data(const rectangle2d *viewport_bounds, c_gui_widget *widget,
    s_window_manager_screen_render_data *render_data, long controller_index)
{
    for (c_gui_widget *child = widget->get_children(); child; child = child->get_next()) {
        if (child->get_type() != _gui_screen_widget && child->should_render(0))
            window_manager_assemble_widget_render_data(viewport_bounds, child, render_data, controller_index);
    }
    bool visible;
    if (widget->should_render(&visible) && visible) {
        if (render_data->current_count < NUMBEROF(render_data->render_list)) {
            long render_data_size = 0;
            switch (widget->get_type()) {
            case _gui_text_widget:
                switch (static_cast<c_gui_text_widget *>(widget)->get_text_buffer_size()) {
                case NUMBEROF(static_cast<s_gui_text_widget_extra_large_render_data *>(0)->text):
                    render_data_size = sizeof(s_gui_text_widget_extra_large_render_data);
                    break;
                case NUMBEROF(static_cast<s_gui_text_widget_large_render_data *>(0)->text):
                    render_data_size = sizeof(s_gui_text_widget_large_render_data);
                    break;
                case NUMBEROF(static_cast<s_gui_text_widget_small_render_data *>(0)->text):
                    render_data_size = sizeof(s_gui_text_widget_small_render_data);
                    break;
                }
                break;
            case _gui_bitmap_widget: render_data_size = sizeof(s_gui_bitmap_widget_render_data); break;
            case _gui_model_widget: render_data_size = sizeof(s_gui_model_widget_render_data); break;
            default: render_data_size = sizeof(s_gui_widget_render_data); break;
            }
            if (render_data->current_buffer_offset + render_data_size < render_data->buffer_size) {
                c_basic_buffer<void> buffer(render_data->render_data_buffer, render_data->buffer_size);
                s_gui_widget_render_data *render_data_assemble =
                    buffer.get_as_type_from_offset<s_gui_widget_render_data>(render_data->current_buffer_offset);
                widget->assemble_render_data(render_data_assemble, viewport_bounds, (e_controller_index)controller_index, true, true, true);
                render_data->render_list[render_data->current_count].type = widget->get_type();
                render_data->render_list[render_data->current_count].render_data_offset = render_data->current_buffer_offset;
                render_data->render_list[render_data->current_count].rendered_depth = widget->get_animated_state()->position.z;
                render_data->render_list[render_data->current_count].render_depth_bias = window_manager_get_render_depth_bias(widget);
                render_data->current_buffer_offset += render_data_size;
                render_data->current_count++;
            }
        }
    }
}
static __forceinline bool window_manager_bink_blocks_rendering()
{
    return bink_globals.active && (bink_globals.control_flags & 8) != 0;
}
static __forceinline s_window_manager_screen_render_data *window_manager_screen_get_render_state(c_gui_screen_widget *screen)
{
    return screen->m_render_data.valid() ? &screen->m_render_data : 0;
}
// @address 0x47f168 window_manager_render_window
extern "C" void window_manager_render_window(
    c_window_manager *window_manager, e_window_index window_index, long user_index, const rectangle2d *window_bounds, bool is_screenshot)
{
    if (window_manager_bink_blocks_rendering())
        return;
    c_window_manager *stacks = window_manager;
    long screen_indices[10];
    long screen_count = 0;
    {
        s_font_cache_thread_state font_lock;
        font_cache_lock(&font_lock);
        window_manager_dispose_unused_bitmaps(&g_gui_custom_bitmap_storage_data.data);
        memcpy(&stacks->m_window_bounds[window_index], window_bounds, sizeof(rectangle2d));
        for (long screen_position = 0; screen_position < screenCount(stacks, window_index); screen_position++) {
            c_gui_screen_widget *screen = stacks->m_screens[window_index][screen_position];
            if (screen)
                screen_indices[screen_count++] = screen->get_screen_index();
        }
        font_cache_unlock(&font_lock);
    }
    s_window_manager_render_snapshot snapshot;
    for (long screen_index = 0; screen_index < screen_count; screen_index++) {
        snapshot.current_count = 0;
        {
            window_manager_render_font_cache_lock font_lock;
            s_screen_handle *screen_handle = static_cast<s_screen_handle *>(
                inl_datum_try_and_get(stacks->m_active_screens, screen_indices[screen_index]));
            if (screen_handle && screen_handle->screen) {
                screen_handle->screen->update_render();
                s_window_manager_screen_render_data *render_state = window_manager_screen_get_render_state(screen_handle->screen);
                if (render_state && memcmp(&render_state->render_bounds, window_bounds, sizeof(rectangle2d)) == 0) {
                    memcpy(snapshot.render_data_buffer, render_state->render_data_buffer, render_state->current_buffer_offset);
                    snapshot.buffer_size = render_state->current_buffer_offset;
                    memcpy(snapshot.render_list, render_state->render_list, sizeof(snapshot.render_list));
                    snapshot.current_count = render_state->current_count;
                }
            }
        }
        long render_count = snapshot.current_count;
        if (render_count > 0) {
            c_basic_buffer<const void> buffer(snapshot.render_data_buffer, snapshot.buffer_size);
            long entry_count = render_count & 0x1ff;
            float near_plane;
            float camera_distance;
            float far_plane;
            user_interface_get_projection_plane_distances(&near_plane, &camera_distance, &far_plane);
            near_plane -= camera_distance;
            far_plane -= camera_distance;
            for (long entry_index = 0; entry_index < entry_count; entry_index++) {
                s_window_manager_render_list_entry *entry = &snapshot.render_list[entry_index];
                if (!(near_plane > entry->rendered_depth) && !(entry->rendered_depth > far_plane)) {
                    const s_gui_widget_render_data *render_data =
                        buffer.get_as_type_from_offset<const s_gui_widget_render_data>(entry->render_data_offset);
                    if (render_data)
                        gui_widget_render_data(user_index, render_data, window_bounds, is_screenshot);
                }
            }
        }
    }
    if (window_index == 4)
        window_manager_render_fade_overlay(window_manager);
}

// @address 0x482a40 ?get_render_data_size@c_gui_screen_widget@@MAA?AW4e_render_data_size@@XZ
e_render_data_size c_gui_screen_widget::get_render_data_size()
{
    return _render_data_size_normal;
}

// @address 0x4818d0 ?named_screen_defined_in_code@c_window_manager@@CA_NJ@Z
bool c_window_manager::named_screen_defined_in_code(long name)
{
    bool result = false;
    if (name == _string_id_error_ok ||
        name == _main_menu_string_main_menu ||
        name == _start_menu_string_start_menu ||
        name == _start_menu_string_start_menu_game_campaign ||
        name == _start_menu_string_start_menu_game_multiplayer ||
        name == _start_menu_string_start_menu_game_editor ||
        name == _start_menu_string_start_menu_game_cooperative ||
        name == _start_menu_string_start_menu_game_saved_film ||
        name == _start_menu_string_start_menu_service_record ||
        name == _start_menu_string_start_menu_file_share ||
        name == _start_menu_string_start_menu_bungie_file_share ||
        name == _string_id_start_menu_hq_service_record_file_share_choose_category ||
        name == _string_id_start_menu_hq_service_record_file_share_choose_item ||
        name == _string_id_start_menu_hq_service_record_file_share_item_selected ||
        name == _start_menu_string_start_menu_hq_screenshots ||
        name == _string_id_start_menu_hq_screenshots_item_options ||
        name == _start_menu_string_start_menu_hq_transfers ||
        name == _string_id_start_menu_hq_transfers_item_selected ||
        name == _start_menu_string_start_menu_hq ||
        name == _start_menu_string_start_menu_settings_controls ||
        name == _start_menu_string_start_menu_settings_controls_button ||
        name == _start_menu_string_start_menu_settings_controls_thumbstick ||
        name == _start_menu_string_start_menu_settings_appearance ||
        name == _start_menu_string_start_menu_settings_appearance_model ||
        name == _start_menu_string_start_menu_settings_appearance_emblem ||
        name == _start_menu_string_start_menu_settings_appearance_colors ||
        name == _start_menu_string_start_menu_settings_voice ||
        name == _start_menu_string_start_menu_settings_camera ||
        name == _start_menu_string_start_menu_settings_display ||
        name == _start_menu_string_start_menu_settings_film_autosave ||
        name == _string_id_screenshots_viewer ||
        name == _string_id_screenshots_file_share_previewer ||
        name == _string_id_gui_alert_nonblocking ||
        name == _string_id_gui_alert_toast ||
        name == _string_id_gui_alert_ingame_full ||
        name == _string_id_gui_alert_ingame_split ||
        name == _string_id_gui_dialog_two_items ||
        name == _string_id_gui_dialog_four_items ||
        name == _string_id_in_progress ||
        name == _string_id_in_progress_mini ||
        name == _string_id_in_progress_mini_me ||
        name == _string_id_spartan_milestone_dialog ||
        name == _string_id_spartan_rank_dialog ||
        name == _player_select_string_campaign_settings ||
        name == _string_id_campaign_select_difficulty ||
        name == _string_id_campaign_select_level ||
        name == _string_id_pregame_lobby_campaign ||
        name == _string_id_pregame_lobby_matchmaking ||
        name == _string_id_pregame_lobby_multiplayer ||
        name == _string_id_pregame_lobby_mapeditor ||
        name == _string_id_pregame_lobby_theater ||
        name == _string_id_pregame_switch_lobby ||
        name == _string_id_pregame_selection ||
        name == _string_id_pregame_advanced_options_matchmaking ||
        name == _string_id_postgame_lobby ||
        name == _player_select_string_player_select ||
        name == _string_id_game_browser ||
        name == _string_id_boot_betrayer ||
        name == _string_id_boot_betrayer_splitscreen ||
        name == _string_id_maximum_party_size ||
        name == _string_id_game_details ||
        name == _string_id_game_options ||
        name == _main_menu_string_alpha_legal ||
        name == _string_id_alpha_motd ||
        name == _start_menu_string_alpha_locked_down ||
        name == _string_id_forge_legal ||
        name == _string_id_carnage_report ||
        name == _player_select_string_carnage_report_player_details ||
        name == _player_select_string_scoreboard ||
        name == _string_id_scoreboard_half ||
        name == _game_details_string_scoreboard_player_select ||
        name == _string_id_scoreboard_player_select_splitscreen ||
        name == _string_id_matchmaking_searching ||
        name == _string_id_matchmaking_match_found ||
        name == _string_id_sandbox_budget_screen ||
        name == _string_id_sandbox_budget_screen_splitscreen ||
        name == _string_id_sandbox_objects_menu ||
        name == _string_id_sandbox_objects_menu_splitscreen ||
        name == _string_id_sandbox_object_properties_panel ||
        name == _string_id_sandbox_object_properties_panel_splitscreen ||
        name == _string_id_observer_camera_list_screen ||
        name == _string_id_observer_camera_list_splitscreen ||
        name == _string_id_saved_film_control_pad ||
        name == _string_id_saved_film_take_screenshot ||
        name == _string_id_saved_film_snippet ||
        name == _string_id_terminal_screen)
        result = true;
    return result;
}

// @address 0x481e40 ?insert_screen_after_named_screen@c_window_manager@@AAAXW4e_window_index@@PAVc_gui_screen_widget@@J@Z
void c_window_manager::insert_screen_after_named_screen(e_window_index window_index, c_gui_screen_widget *screen, long name)
{
    c_window_manager *screen_stacks = &g_window_manager;
    bool inserted = false;
    if (name == _string_id_bottom_most) {
        for (long screen_index = screenCount(screen_stacks, window_index); screen_index > 0; screen_index--)
            screen_stacks->m_screens[window_index][screen_index] = screen_stacks->m_screens[window_index][screen_index - 1];
        screen_stacks->m_screens[window_index][0] = screen;
        __lwsync();
        _InterlockedExchangeAdd(&screen_stacks->m_screen_count[window_index].value, 1);
        inserted = true;
    } else if (name != _string_id_top_most) {
        for (long screen_index = 0; screen_index < screenCount(screen_stacks, window_index); screen_index++) {
            c_gui_screen_widget *current_screen = screen_stacks->m_screens[window_index][screen_index];
            if (current_screen->m_name == name && !current_screen->transitioning_out()) {
                for (long shift_index = screenCount(screen_stacks, window_index); shift_index > screen_index + 1; shift_index--)
                    screen_stacks->m_screens[window_index][shift_index] = screen_stacks->m_screens[window_index][shift_index - 1];
                screen_stacks->m_screens[window_index][screen_index + 1] = screen;
                __lwsync();
                _InterlockedExchangeAdd(&screen_stacks->m_screen_count[window_index].value, 1);
                inserted = true;
                break;
            }
        }
    }
    if (!inserted) {
        screen_stacks->m_screens[window_index][screenCount(screen_stacks, window_index)] = screen;
        __lwsync();
        _InterlockedExchangeAdd(&screen_stacks->m_screen_count[window_index].value, 1);
    }
}

// @address 0x518f0 k_file_share_keyboard_task_name
// @address 0x51ce8 k_gui_start_menu_hq_screenshots_submenu_source_file

// @address 0x1ff8 ??_7c_gui_game_details@@6B@
// @address 0x499c08 ??_Gc_gui_game_details@@UAAPAXI@Z
// @address 0x42d298 ?can_receive_focus@c_gui_widget@@UAA_NXZ
// @address 0x42d298 ?handle_widget_back_out@c_gui_widget@@UAA_NXZ
// @address 0x42d298 ?handle_widget_selected@c_gui_widget@@UAA_NXZ
// @address 0x42d298 ?can_receive_focus@c_gui_screen_widget@@UBA_NXZ
// @address 0x5bec98 ?submenu_invoked@c_gui_screen_widget@@UAAXPAVc_gui_list_widget@@@Z
// @address 0x50b20 ??_7c_error_dialog_ok_screen_widget@@6B@
// @address 0x4a8838 ??_Ec_error_dialog_ok_screen_widget@@UAAPAXI@Z
// @address 0x42d380 ?handle_focused_widget_selected@c_error_dialog_ok_screen_widget@@UAA_NPBVc_controller_input_message@@PAVc_gui_widget@@@Z
// @address 0x50be8 ??_7c_main_menu_screen_widget@@6B@
// @address 0x4a8838 ??_Ec_main_menu_screen_widget@@UAAPAXI@Z
// @address 0x42d298 ?get_gui_location@c_main_menu_screen_widget@@UBA?AW4e_gui_location@@XZ
// @address 0x50d38 ??_7c_start_menu_game_campaign@@6B@
// @address 0x4a8838 ??_Ec_start_menu_game_campaign@@UAAPAXI@Z
// @address 0x50e48 ??_7c_start_menu_game_multiplayer@@6B@
// @address 0x4a8838 ??_Ec_start_menu_game_multiplayer@@UAAPAXI@Z
// @address 0x510c0 ??_7c_start_menu_game_saved_film@@6B@
// @address 0x4a8838 ??_Ec_start_menu_game_saved_film@@UAAPAXI@Z
// @address 0x511d8 ??_7c_gui_start_menu_hq_service_record@@6B@
// @address 0x4ae088 ??_Gc_gui_start_menu_hq_service_record@@UAAPAXI@Z
// @address 0x51308 ??_7c_gui_start_menu_hq_service_record_pane@@6B@
// @address 0x4a8838 ??_Ec_gui_start_menu_hq_service_record_pane@@UAAPAXI@Z
// @address 0x4a8838 ??_Gc_gui_start_menu_hq_service_record_pane@@UAAPAXI@Z
// @address 0x51668 ??_7c_gui_start_menu_hq_service_record_file_share_choose_category@@6B@
// @address 0x4b2ba8 ?initialize@c_gui_start_menu_hq_service_record_file_share_choose_category@@UAAXXZ
// @address 0x517b8 ??_7c_gui_start_menu_hq_service_record_file_share_choose_item@@6B@
// @address 0x4b2af8 ??_Ec_gui_start_menu_hq_service_record_file_share_choose_item@@UAAPAXI@Z
// @address 0x51988 ??_7c_gui_start_menu_hq_service_record_file_share_item_selected@@6B@
// @address 0x4b42b0 ??_Ec_gui_start_menu_hq_service_record_file_share_item_selected@@UAAPAXI@Z
// @address 0x51b98 ??_7s_gui_start_menu_settings_screenshots@@6B@
// @address 0x4a8838 ??_Es_gui_start_menu_settings_screenshots@@UAAPAXI@Z
// @address 0x4a8838 ??_Gs_gui_start_menu_settings_screenshots@@UAAPAXI@Z
// @address 0x42d298 ?controller_event_should_go_to_parent@s_gui_start_menu_settings_screenshots@@MAA_NPBVc_controller_input_message@@@Z
// @address 0x51d50 ??_7c_gui_start_menu_hq_screenshots_submenu@@6B@
// @address 0x4a8838 ??_Ec_gui_start_menu_hq_screenshots_submenu@@UAAPAXI@Z
// @address 0x4a8838 ??_Gc_gui_start_menu_hq_screenshots_submenu@@UAAPAXI@Z
// @address 0x51e18 ??_7c_gui_start_menu_hq_transfers@@6B@
// @address 0x4a8838 ??_Ec_gui_start_menu_hq_transfers@@UAAPAXI@Z
// @address 0x42d298 ?controller_event_should_go_to_parent@c_gui_start_menu_hq_transfers@@MAA_NPBVc_controller_input_message@@@Z
// @address 0x51f58 ??_7c_gui_start_menu_hq_transfers_item_selected@@6B@
// @address 0x4a8838 ??_Ec_gui_start_menu_hq_transfers_item_selected@@UAAPAXI@Z
// @address 0x4c0e90 ?get_render_data_size@c_gui_start_menu_hq_transfers_item_selected@@MAA?AW4e_render_data_size@@XZ
// @address 0x52090 ??_7c_start_menu_settings@@6B@
// @address 0x4a8838 ??_Ec_start_menu_settings@@UAAPAXI@Z
// @address 0x42d380 ?controller_event_should_go_to_parent@c_start_menu_settings@@MAA_NPBVc_controller_input_message@@@Z
// @address 0x521d0 ??_7c_start_menu_settings_controls@@6B@
// @address 0x4a8838 ??_Ec_start_menu_settings_controls@@UAAPAXI@Z
// @address 0x52310 ??_7c_start_menu_settings_controls_button@@6B@
// @address 0x4a8838 ??_Ec_start_menu_settings_controls_button@@UAAPAXI@Z
// @address 0x42d298 ?controller_event_should_go_to_parent@c_start_menu_settings_controls_button@@MAA_NPBVc_controller_input_message@@@Z
// @address 0x523e0 ??_7c_start_menu_settings_controls_thumbstick@@6B@
// @address 0x4a8838 ??_Ec_start_menu_settings_controls_thumbstick@@UAAPAXI@Z
// @address 0x42d298 ?controller_event_should_go_to_parent@c_start_menu_settings_controls_thumbstick@@MAA_NPBVc_controller_input_message@@@Z
// @address 0x42d298 ?controller_event_should_go_to_parent@c_start_menu_settings_appearance@@MAA_NPBVc_controller_input_message@@@Z
// @address 0x52540 ??_7c_start_menu_settings_appearance@@6B@
// @address 0x4a8838 ??_Ec_start_menu_settings_appearance@@UAAPAXI@Z
// @address 0x4a8838 ??_Gc_start_menu_settings_appearance@@UAAPAXI@Z
// @address 0x52680 ??_7c_start_menu_settings_appearance_model@@6B@
// @address 0x4a8838 ??_Ec_start_menu_settings_appearance_model@@UAAPAXI@Z
// @address 0x4a8838 ??_Gc_start_menu_settings_appearance_model@@UAAPAXI@Z
// @address 0x42d298 ?controller_event_should_go_to_parent@c_start_menu_settings_appearance_model@@MAA_NPBVc_controller_input_message@@@Z
// @address 0x52830 ??_7c_start_menu_settings_appearance_emblem@@6B@
// @address 0x4a8838 ??_Ec_start_menu_settings_appearance_emblem@@UAAPAXI@Z
// @address 0x529b0 ??_7c_start_menu_settings_appearance_colors@@6B@
// @address 0x4a8838 ??_Ec_start_menu_settings_appearance_colors@@UAAPAXI@Z
// @address 0x52b40 ??_7c_start_menu_settings_voice@@6B@
// @address 0x4a8838 ??_Ec_start_menu_settings_voice@@UAAPAXI@Z
// @address 0x4a8838 ??_Gc_start_menu_settings_voice@@UAAPAXI@Z
// @address 0x42d298 ?controller_event_should_go_to_parent@c_start_menu_settings_voice@@MAA_NPBVc_controller_input_message@@@Z
// @address 0x52cb8 ??_7c_start_menu_settings_camera@@6B@
// @address 0x4a8838 ??_Ec_start_menu_settings_camera@@UAAPAXI@Z
// @address 0x4a8838 ??_Gc_start_menu_settings_camera@@UAAPAXI@Z
// @address 0x42d298 ?controller_event_should_go_to_parent@c_start_menu_settings_camera@@MAA_NPBVc_controller_input_message@@@Z
// @address 0x52e38 ??_7c_start_menu_settings_display@@6B@
// @address 0x4a8838 ??_Ec_start_menu_settings_display@@UAAPAXI@Z
// @address 0x4a8838 ??_Gc_start_menu_settings_display@@UAAPAXI@Z
// @address 0x42d298 ?controller_event_should_go_to_parent@c_start_menu_settings_display@@MAA_NPBVc_controller_input_message@@@Z
// @address 0x52fc0 ??_7c_start_menu_settings_film_autosave@@6B@
// @address 0x4a8838 ??_Ec_start_menu_settings_film_autosave@@UAAPAXI@Z
// @address 0x4a8838 ??_Gc_start_menu_settings_film_autosave@@UAAPAXI@Z
// @address 0x42d298 ?controller_event_should_go_to_parent@c_start_menu_settings_film_autosave@@MAA_NPBVc_controller_input_message@@@Z
// @address 0x53130 ??_7c_gui_screen_campaign_options@@6B@
// @address 0x4a8838 ??_Ec_gui_screen_campaign_options@@UAAPAXI@Z
// @address 0x4a8838 ??_Gc_gui_screen_campaign_options@@UAAPAXI@Z
// @address 0x53200 ??_7c_gui_screen_download_widget@@6B@
// @address 0x4c0310 ??_Ec_gui_screen_download_widget@@UAAPAXI@Z
// @address 0x4c0310 ??_Gc_gui_screen_download_widget@@UAAPAXI@Z
// @address 0x532d0 ??_7c_gui_alert_screen_widget@@6B@
// @address 0x4a8838 ??_Ec_gui_alert_screen_widget@@UAAPAXI@Z
// @address 0x4c0f98 ?post_initialize@c_gui_alert_screen_widget@@UAAXXZ
// @address 0x534d0 ??_7c_gui_screen_in_progress@@6B@
// @address 0x4a8838 ??_Ec_gui_screen_in_progress@@UAAPAXI@Z
// @address 0x53598 ??_7c_gui_screen_spartan_milestone_dialog@@6B@
// @address 0x4a8838 ??_Ec_gui_screen_spartan_milestone_dialog@@UAAPAXI@Z
// @address 0x4a8838 ??_Gc_gui_screen_spartan_milestone_dialog@@UAAPAXI@Z
// @address 0x53660 ??_7c_gui_screen_spartan_rank_dialog@@6B@
// @address 0x4a8838 ??_Ec_gui_screen_spartan_rank_dialog@@UAAPAXI@Z
// @address 0x4a8838 ??_Gc_gui_screen_spartan_rank_dialog@@UAAPAXI@Z
// @address 0x53728 ??_7c_gui_screen_campaign_settings@@6B@
// @address 0x4a8838 ??_Ec_gui_screen_campaign_settings@@UAAPAXI@Z
// @address 0x4a8838 ??_Gc_gui_screen_campaign_settings@@UAAPAXI@Z
// @address 0x538d0 ??_7c_gui_screen_campaign_select_difficulty@@6B@
// @address 0x53998 ??_7c_gui_screen_campaign_select_level@@6B@
// @address 0x53b68 ??_7c_gui_screen_pregame_lobby_campaign@@6B@
// @address 0x4a8838 ??_Ec_gui_screen_pregame_lobby_campaign@@UAAPAXI@Z
// @address 0x53d30 ??_7c_gui_screen_pregame_lobby_matchmaking@@6B@
// @address 0x4c5708 ??_Gc_gui_screen_pregame_lobby_matchmaking@@UAAPAXI@Z
// @address 0x4c5708 ??_Ec_gui_screen_pregame_lobby_matchmaking@@UAAPAXI@Z
// @address 0x53e80 ??_7c_gui_screen_pregame_lobby_multiplayer@@6B@
// @address 0x4a8838 ??_Ec_gui_screen_pregame_lobby_multiplayer@@UAAPAXI@Z
// @address 0x3fc598 ?get_gui_game_mode@c_gui_screen_pregame_lobby_multiplayer@@UAAJXZ
// @address 0x53f98 ??_7c_gui_screen_pregame_lobby_mapeditor@@6B@
// @address 0x4a8838 ??_Ec_gui_screen_pregame_lobby_mapeditor@@UAAPAXI@Z
// @address 0x45fc28 ?get_gui_game_mode@c_gui_screen_pregame_lobby_mapeditor@@UAAJXZ
// @address 0x54138 ??_7c_gui_screen_pregame_lobby_theater@@6B@
// @address 0x4a8838 ??_Ec_gui_screen_pregame_lobby_theater@@UAAPAXI@Z
// @address 0x4a8838 ??_Gc_gui_screen_pregame_lobby_theater@@UAAPAXI@Z
// @address 0x42d298 ?team_switching_allowed@c_gui_screen_pregame_lobby_theater@@MAA_NXZ
// @address 0x42d298 ?advanced_options_enabled@c_gui_screen_pregame_lobby_theater@@UAA_NXZ
// @address 0x3fc550 ?get_gui_game_mode@c_gui_screen_pregame_lobby_theater@@UAAJXZ
// @address 0x54228 ??_7c_gui_screen_pregame_switch_lobby@@6B@
// @address 0x544e0 ??_7c_gui_screen_matchmaking_advanced_options@@6B@
// @address 0x4cad10 ??_Ec_gui_screen_matchmaking_advanced_options@@UAAPAXI@Z
// @address 0x545e8 ??_7c_gui_screen_postgame_lobby@@6B@
// @address 0x4cb920 ??_Ec_gui_screen_postgame_lobby@@UAAPAXI@Z
// @address 0x54b08 ??_7c_gui_game_browser@@6B@
// @address 0x4a8838 ??_Ec_gui_game_browser@@UAAPAXI@Z
// @address 0x4a8838 ??_Gc_gui_game_browser@@UAAPAXI@Z
// @address 0x54c58 ??_7c_gui_screen_maximum_party_size@@6B@
// @address 0x54f60 ??_7c_gui_screen_alpha_legal@@6B@
// @address 0x4a8838 ??_Ec_gui_screen_alpha_legal@@UAAPAXI@Z
// @address 0x55028 ??_7c_gui_screen_alpha_motd@@6B@
// @address 0x4a8838 ??_Ec_gui_screen_alpha_motd@@UAAPAXI@Z
// @address 0x550f0 ??_7c_gui_screen_alpha_locked_down@@6B@
// @address 0x4a8838 ??_Ec_gui_screen_alpha_locked_down@@UAAPAXI@Z
// @address 0x551b8 ??_7c_gui_screen_alpha_motd_editor@@6B@
// @address 0x4a8838 ??_Ec_gui_screen_alpha_motd_editor@@UAAPAXI@Z
// @address 0x5bec98 ?post_initialize@c_gui_screen_alpha_motd_editor@@UAAXXZ
// @address 0x554c0 ??_7c_gui_screen_carnage_report@@6B@
// @address 0x4a8838 ??_Ec_gui_screen_carnage_report@@UAAPAXI@Z
// @address 0x4ebb8 ??_7c_gui_screen_scoreboard@@6B@
// @address 0x4a8838 ??_Ec_gui_screen_scoreboard@@UAAPAXI@Z
// @address 0x499198 ?get_render_data_size@c_gui_screen_scoreboard@@MAA?AW4e_render_data_size@@XZ
// @address 0x55730 ??_7c_gui_screen_matchmaking_searching@@6B@
// @address 0x4d9e70 ??_Ec_gui_screen_matchmaking_searching@@UAAPAXI@Z
// @address 0x3fc598 ?get_gui_location@c_gui_screen_matchmaking_searching@@UBA?AW4e_gui_location@@XZ
// @address 0x45fc28 ?get_gui_location@c_gui_screen_matchmaking_match_found@@UBA?AW4e_gui_location@@XZ
// @address 0x55988 ??_7c_gui_screen_matchmaking_match_found@@6B@
// @address 0x4a8838 ??_Ec_gui_screen_matchmaking_match_found@@UAAPAXI@Z
// @address 0x4a8838 ??_Gc_gui_screen_matchmaking_match_found@@UAAPAXI@Z
// @address 0x50818 ??_7c_sandbox_budget_summary_screen_widget@@6B@
// @address 0x508e0 ??_7c_sandbox_object_menu_screen_widget@@6B@
// @address 0x4a8838 ??_Ec_sandbox_object_menu_screen_widget@@UAAPAXI@Z
// @address 0x55a50 ??_7c_sandbox_object_properties_screen_widget@@6B@
// @address 0x4e800 ??_7c_observer_camera_list_screen@@6B@
// @address 0x4a8838 ??_Ec_observer_camera_list_screen@@UAAPAXI@Z
// @address 0x4a8838 ??_Gc_observer_camera_list_screen@@UAAPAXI@Z
// @address 0x55bf8 ??_7c_saved_film_control_pad@@6B@
// @address 0x4a8838 ??_Ec_saved_film_control_pad@@UAAPAXI@Z
// @address 0x55d28 ??_7c_gui_screen_take_screenshot@@6B@
// @address 0x4a8838 ??_Ec_gui_screen_take_screenshot@@UAAPAXI@Z
// @address 0x4a8838 ??_Gc_gui_screen_take_screenshot@@UAAPAXI@Z
// @address 0x55df0 ??_7c_gui_screen_saved_film_snippet@@6B@
// @address 0x4a8838 ??_Ec_gui_screen_saved_film_snippet@@UAAPAXI@Z
// @address 0x4a8838 ??_Gc_gui_screen_saved_film_snippet@@UAAPAXI@Z
// @address 0x4e3b8 ??_7c_gui_screen_terminal@@6B@
// @address 0x4a8838 ??_Ec_gui_screen_terminal@@UAAPAXI@Z

template<typename t_type> static __forceinline void store_ordered(t_type *destination, t_type value)
{
    t_type *pointer = destination;
    *pointer = value;
}

#define UI_MALLOC_NEW new(0, 0, _ui_allocation_marker_dummy)
#define WM_ALLOCATE(type) ((type *)operator new(sizeof(type), 0, 0, _ui_allocation_marker_dummy))
typedef char window_manager_game_options_size[(sizeof(void *) != 4 || ((sizeof(c_gui_screen_game_options) == 0x11568))) ? 1 : -1];

__forceinline c_start_menu_settings_appearance::c_start_menu_settings_appearance(long name) :
    c_start_menu_pane_screen_widget(name)
{
    m_automatic_button_key = NONE;
    m_translation = 0.0f;
    m_rotation = 0.0f;
    m_virtual_keyboard_task = c_virtual_keyboard_task::get_instance(
        "..\\interface\\gui_screens\\start_menu\\panes\\"
        "settings_appearance\\start_menu_settings_appearance.cpp",
        29, _controller_index_none, 0, 0, 0, 256, 0);
}

__forceinline c_gui_screen_matchmaking_match_found::c_gui_screen_matchmaking_match_found(long name) :
    c_gui_screen_widget(name), previous_countdown(0), countdown_started(false), previous_veto_count(0)
{
}

__forceinline c_start_menu_settings_appearance_colors::c_start_menu_settings_appearance_colors(long name) :
    c_start_menu_pane_screen_widget(name)
{
    m_translation = 0.0f;
    m_rotation = 0.0f;
    m_automatic_button_key = NONE;
}

__forceinline c_main_menu_screen_widget::c_main_menu_screen_widget(long name) :
    c_gui_screen_widget(name), mode(0), pad1a8c(false), active(false), m_catalogues_busy(false), m_1ab0(0)
{
}

__forceinline c_gui_screen_carnage_report::c_gui_screen_carnage_report(long name) :
    c_gui_screen_widget(name), current_value_name(0), player_index(NONE)
{
    if (tls_game_globals()->options.game_mode == 1)
        report_type = 2;
    else if (user_interface_networking_get_final_game_results())
        report_type = 1;
    else
        report_type = 0;
}

c_gui_screen_widget *c_window_manager::allocate_named_screen(long screen_name)
{
    c_gui_screen_widget *screen = 0;
    if (c_window_manager::locate_screen_definition_tag_by_name(screen_name) != NONE)
    {
        if (screen_name == _string_id_error_ok)
            screen = UI_MALLOC_NEW c_error_dialog_ok_screen_widget(screen_name);
        else if (screen_name == _main_menu_string_main_menu)
            screen = UI_MALLOC_NEW c_main_menu_screen_widget(screen_name);
        else if (screen_name == _start_menu_string_start_menu)
            screen = UI_MALLOC_NEW c_start_menu_screen_widget(screen_name);
        else if (screen_name == _start_menu_string_start_menu_game_campaign)
            screen = UI_MALLOC_NEW c_start_menu_game_campaign(screen_name);
        else if (screen_name == _start_menu_string_start_menu_game_multiplayer)
            screen = UI_MALLOC_NEW c_start_menu_game_multiplayer(screen_name);
        else if (screen_name == _start_menu_string_start_menu_game_editor)
            screen = UI_MALLOC_NEW c_start_menu_game_editor(screen_name);
        else if (screen_name == _start_menu_string_start_menu_game_cooperative)
            screen = UI_MALLOC_NEW c_start_menu_game_saved_film(screen_name);
        else if (screen_name == _start_menu_string_start_menu_game_saved_film)
            screen = UI_MALLOC_NEW c_gui_start_menu_hq_service_record(screen_name);
        else if (screen_name == _start_menu_string_start_menu_service_record)
            screen = UI_MALLOC_NEW c_gui_start_menu_hq_service_record_pane(screen_name);
        else if (screen_name == _start_menu_string_start_menu_file_share)
            screen = UI_MALLOC_NEW c_gui_start_menu_hq_service_record_file_share(screen_name);
        else if (screen_name == _start_menu_string_start_menu_bungie_file_share)
            screen = UI_MALLOC_NEW c_gui_start_menu_hq_service_record_file_share(screen_name);
        else if (screen_name == _string_id_start_menu_hq_service_record_file_share_choose_category)
            screen = UI_MALLOC_NEW c_gui_start_menu_hq_service_record_file_share_choose_category(screen_name);
        else if (screen_name == _string_id_start_menu_hq_service_record_file_share_choose_item)
            screen = UI_MALLOC_NEW c_gui_start_menu_hq_service_record_file_share_choose_item(screen_name);
        else if (screen_name == _string_id_start_menu_hq_service_record_file_share_item_selected)
            screen = UI_MALLOC_NEW c_gui_start_menu_hq_service_record_file_share_item_selected(screen_name);
        else if (screen_name == _start_menu_string_start_menu_hq_screenshots)
            screen = UI_MALLOC_NEW s_gui_start_menu_settings_screenshots(screen_name);
        else if (screen_name == _string_id_start_menu_hq_screenshots_item_options)
            screen = UI_MALLOC_NEW c_gui_start_menu_hq_screenshots_submenu(screen_name);
        else if (screen_name == _start_menu_string_start_menu_hq_transfers)
            screen = UI_MALLOC_NEW c_gui_start_menu_hq_transfers(screen_name);
        else if (screen_name == _string_id_start_menu_hq_transfers_item_selected)
            screen = UI_MALLOC_NEW c_gui_start_menu_hq_transfers_item_selected(screen_name);
        else if (screen_name == _start_menu_string_start_menu_hq)
            screen = UI_MALLOC_NEW c_start_menu_settings(screen_name);
        else if (screen_name == _start_menu_string_start_menu_settings_controls)
            screen = UI_MALLOC_NEW c_start_menu_settings_controls(screen_name);
        else if (screen_name == _start_menu_string_start_menu_settings_controls_button)
            screen = UI_MALLOC_NEW c_start_menu_settings_controls_button(screen_name);
        else if (screen_name == _start_menu_string_start_menu_settings_controls_thumbstick)
            screen = UI_MALLOC_NEW c_start_menu_settings_controls_thumbstick(screen_name);
        else if (screen_name == _start_menu_string_start_menu_settings_appearance)
            screen = UI_MALLOC_NEW c_start_menu_settings_appearance(screen_name);
        else if (screen_name == _start_menu_string_start_menu_settings_appearance_model)
            screen = UI_MALLOC_NEW c_start_menu_settings_appearance_model(screen_name);
        else if (screen_name == _start_menu_string_start_menu_settings_appearance_emblem)
            screen = UI_MALLOC_NEW c_start_menu_settings_appearance_emblem(screen_name);
        else if (screen_name == _start_menu_string_start_menu_settings_appearance_colors)
            screen = UI_MALLOC_NEW c_start_menu_settings_appearance_colors(screen_name);
        else if (screen_name == _start_menu_string_start_menu_settings_voice)
            screen = UI_MALLOC_NEW c_start_menu_settings_voice(screen_name);
        else if (screen_name == _start_menu_string_start_menu_settings_camera)
            screen = UI_MALLOC_NEW c_start_menu_settings_camera(screen_name);
        else if (screen_name == _start_menu_string_start_menu_settings_display)
            screen = UI_MALLOC_NEW c_start_menu_settings_display(screen_name);
        else if (screen_name == _start_menu_string_start_menu_settings_film_autosave)
            screen = UI_MALLOC_NEW c_start_menu_settings_film_autosave(screen_name);
        else if (screen_name == _string_id_screenshots_viewer)
            screen = UI_MALLOC_NEW c_gui_screen_campaign_options(screen_name);
        else if (screen_name == _string_id_screenshots_file_share_previewer)
            screen = UI_MALLOC_NEW c_gui_screen_download_widget(screen_name);
        else if (screen_name == _string_id_gui_alert_nonblocking)
            screen = UI_MALLOC_NEW c_gui_alert_screen_widget(screen_name);
        else if (screen_name == _string_id_gui_alert_toast)
            screen = UI_MALLOC_NEW c_gui_alert_screen_widget(screen_name);
        else if (screen_name == _string_id_gui_alert_ingame_full)
            screen = UI_MALLOC_NEW c_gui_alert_screen_widget(screen_name);
        else if (screen_name == _string_id_gui_alert_ingame_split)
            screen = UI_MALLOC_NEW c_gui_alert_screen_widget(screen_name);
        else if (screen_name == _string_id_gui_dialog_two_items)
            screen = UI_MALLOC_NEW c_gui_dialog_screen_widget(screen_name);
        else if (screen_name == _string_id_gui_dialog_four_items)
            screen = UI_MALLOC_NEW c_gui_dialog_screen_widget(screen_name);
        else if (screen_name == _string_id_in_progress)
            screen = UI_MALLOC_NEW c_gui_screen_in_progress(screen_name);
        else if (screen_name == _string_id_in_progress_mini)
            screen = UI_MALLOC_NEW c_gui_screen_in_progress(screen_name);
        else if (screen_name == _string_id_in_progress_mini_me)
            screen = UI_MALLOC_NEW c_gui_screen_in_progress(screen_name);
        else if (screen_name == _string_id_spartan_milestone_dialog)
            screen = UI_MALLOC_NEW c_gui_screen_spartan_milestone_dialog(screen_name);
        else if (screen_name == _string_id_spartan_rank_dialog)
            screen = UI_MALLOC_NEW c_gui_screen_spartan_rank_dialog(screen_name);
        else if (screen_name == _player_select_string_campaign_settings)
            screen = UI_MALLOC_NEW c_gui_screen_campaign_settings(screen_name);
        else if (screen_name == _string_id_campaign_select_difficulty)
            screen = UI_MALLOC_NEW c_gui_screen_campaign_select_difficulty(screen_name);
        else if (screen_name == _string_id_campaign_select_level)
            screen = UI_MALLOC_NEW c_gui_screen_campaign_select_level(screen_name);
        else if (screen_name == _string_id_pregame_lobby_campaign)
            screen = UI_MALLOC_NEW c_gui_screen_pregame_lobby_campaign(screen_name);
        else if (screen_name == _string_id_pregame_lobby_matchmaking)
            screen = UI_MALLOC_NEW c_gui_screen_pregame_lobby_matchmaking(screen_name);
        else if (screen_name == _string_id_pregame_lobby_multiplayer)
            screen = UI_MALLOC_NEW c_gui_screen_pregame_lobby_multiplayer(screen_name);
        else if (screen_name == _string_id_pregame_lobby_mapeditor)
            screen = UI_MALLOC_NEW c_gui_screen_pregame_lobby_mapeditor(screen_name);
        else if (screen_name == _string_id_pregame_lobby_theater)
            screen = UI_MALLOC_NEW c_gui_screen_pregame_lobby_theater(screen_name);
        else if (screen_name == _string_id_pregame_switch_lobby)
            screen = UI_MALLOC_NEW c_gui_screen_pregame_switch_lobby(screen_name);
        else if (screen_name == _string_id_pregame_selection)
            {
                c_gui_screen_pregame_selection *object = WM_ALLOCATE(c_gui_screen_pregame_selection);
                screen = object ? new (object) c_gui_screen_pregame_selection(screen_name) : 0;
            }
        else if (screen_name == _string_id_pregame_advanced_options_matchmaking)
            screen = UI_MALLOC_NEW c_gui_screen_matchmaking_advanced_options(screen_name);
        else if (screen_name == _string_id_postgame_lobby)
            screen = UI_MALLOC_NEW c_gui_screen_postgame_lobby(screen_name);
        else if (screen_name == _player_select_string_player_select)
            {
                c_gui_player_select_screen_widget *object = WM_ALLOCATE(c_gui_player_select_screen_widget);
                screen = object ? new (object) c_gui_player_select_screen_widget(screen_name) : 0;
            }
        else if (screen_name == _string_id_game_browser)
            screen = UI_MALLOC_NEW c_gui_game_browser(screen_name);
        else if (screen_name == _string_id_boot_betrayer)
            {
                c_gui_screen_boot_betrayer *object = WM_ALLOCATE(c_gui_screen_boot_betrayer);
                screen = object ? gui_screen_boot_betrayer_construct(object, screen_name) : 0;
            }
        else if (screen_name == _string_id_boot_betrayer_splitscreen)
            {
                c_gui_screen_boot_betrayer *object = WM_ALLOCATE(c_gui_screen_boot_betrayer);
                screen = object ? gui_screen_boot_betrayer_construct(object, screen_name) : 0;
            }
        else if (screen_name == _string_id_maximum_party_size)
            screen = UI_MALLOC_NEW c_gui_screen_maximum_party_size(screen_name);
        else if (screen_name == _string_id_game_details)
            screen = UI_MALLOC_NEW c_gui_game_details(screen_name);
        else if (screen_name == _string_id_game_options)
            screen = UI_MALLOC_NEW c_gui_screen_game_options(screen_name);
        else if (screen_name == _main_menu_string_alpha_legal)
            screen = UI_MALLOC_NEW c_gui_screen_alpha_legal(screen_name);
        else if (screen_name == _string_id_alpha_motd)
            screen = UI_MALLOC_NEW c_gui_screen_alpha_motd(screen_name);
        else if (screen_name == _start_menu_string_alpha_locked_down)
            screen = UI_MALLOC_NEW c_gui_screen_alpha_locked_down(screen_name);
        else if (screen_name == _string_id_forge_legal)
            screen = UI_MALLOC_NEW c_gui_screen_alpha_motd_editor(screen_name);
        else if (screen_name == _string_id_carnage_report)
            screen = UI_MALLOC_NEW c_gui_screen_carnage_report(screen_name);
        else if (screen_name == _player_select_string_carnage_report_player_details)
            {
                c_gui_player_select_screen_widget *object = WM_ALLOCATE(c_gui_player_select_screen_widget);
                screen = object ? new (object) c_gui_player_select_screen_widget(screen_name) : 0;
            }
        else if (screen_name == _player_select_string_scoreboard)
            screen = UI_MALLOC_NEW c_gui_screen_scoreboard(screen_name);
        else if (screen_name == _string_id_scoreboard_half)
            screen = UI_MALLOC_NEW c_gui_screen_scoreboard(screen_name);
        else if (screen_name == _game_details_string_scoreboard_player_select)
            {
                c_gui_player_select_screen_widget *object = WM_ALLOCATE(c_gui_player_select_screen_widget);
                screen = object ? new (object) c_gui_player_select_screen_widget(screen_name) : 0;
            }
        else if (screen_name == _string_id_scoreboard_player_select_splitscreen)
            {
                c_gui_player_select_screen_widget *object = WM_ALLOCATE(c_gui_player_select_screen_widget);
                screen = object ? new (object) c_gui_player_select_screen_widget(screen_name) : 0;
            }
        else if (screen_name == _string_id_matchmaking_searching)
            screen = UI_MALLOC_NEW c_gui_screen_matchmaking_searching(screen_name);
        else if (screen_name == _string_id_matchmaking_match_found)
            screen = UI_MALLOC_NEW c_gui_screen_matchmaking_match_found(screen_name);
        else if (screen_name == _string_id_sandbox_budget_screen)
            screen = UI_MALLOC_NEW c_sandbox_budget_summary_screen_widget(screen_name);
        else if (screen_name == _string_id_sandbox_budget_screen_splitscreen)
            screen = UI_MALLOC_NEW c_sandbox_budget_summary_screen_widget(screen_name);
        else if (screen_name == _string_id_sandbox_objects_menu)
            screen = UI_MALLOC_NEW c_sandbox_object_menu_screen_widget(screen_name);
        else if (screen_name == _string_id_sandbox_objects_menu_splitscreen)
            screen = UI_MALLOC_NEW c_sandbox_object_menu_screen_widget(screen_name);
        else if (screen_name == _string_id_sandbox_object_properties_panel)
            screen = UI_MALLOC_NEW c_sandbox_object_properties_screen_widget(screen_name);
        else if (screen_name == _string_id_sandbox_object_properties_panel_splitscreen)
            screen = UI_MALLOC_NEW c_sandbox_object_properties_screen_widget(screen_name);
        else if (screen_name == _string_id_observer_camera_list_screen)
            screen = UI_MALLOC_NEW c_observer_camera_list_screen(screen_name);
        else if (screen_name == _string_id_observer_camera_list_splitscreen)
            screen = UI_MALLOC_NEW c_observer_camera_list_screen(screen_name);
        else if (screen_name == _string_id_saved_film_control_pad)
            screen = UI_MALLOC_NEW c_saved_film_control_pad(screen_name);
        else if (screen_name == _string_id_saved_film_take_screenshot)
            screen = UI_MALLOC_NEW c_gui_screen_take_screenshot(screen_name);
        else if (screen_name == _string_id_saved_film_snippet)
            screen = UI_MALLOC_NEW c_gui_screen_saved_film_snippet(screen_name);
        else if (screen_name == _string_id_terminal_screen)
            screen = UI_MALLOC_NEW c_gui_screen_terminal(screen_name);
    }
    return screen;
}

static __forceinline float user_interface_window_manager_absolute(const float &value)
{
    if (value < 0.0f)
        return -value;
    return value;
}

// @address 0x4829c0 ?gui_widget_depth_sort_proc@@YAHPBX0@Z
int gui_widget_depth_sort_proc(const void *first_entry, const void *second_entry)
{
    const s_window_manager_render_list_entry *first = static_cast<const s_window_manager_render_list_entry *>(first_entry);
    const s_window_manager_render_list_entry *second = static_cast<const s_window_manager_render_list_entry *>(second_entry);
    float epsilon = k_window_manager_depth_epsilon;
    float first_depth = first->rendered_depth;
    float second_depth = second->rendered_depth;
    float depth_difference = user_interface_window_manager_absolute(first_depth - second_depth);
    if (depth_difference <= epsilon) {
        if (first->render_depth_bias <= second->render_depth_bias) {
            if (first->render_depth_bias < second->render_depth_bias)
                return -1;
            if (first->type >= second->type)
                return first->type > second->type ? -1 : 0;
        }
        return 1;
    }
    return first_depth < second_depth ? 1 : (first_depth > second_depth ? -1 : 0);
}
// @address 0xa0198 window_manager_is_controller_input_suppressed
extern "C" bool window_manager_is_controller_input_suppressed(c_window_manager *window_manager, e_controller_index controller_index)
{
    c_gui_screen_widget *responding_screen = window_manager->get_responding_screen(controller_index, 0);
    c_gui_screen_widget *start_menu = window_manager->get_screen_by_name(_console_window, _start_menu_string_start_menu);
    c_window_manager *screen_stacks = window_manager;
    bool result = true; result = result && screen_stacks->m_fade_held != 0;
    if (responding_screen &&
        (responding_screen->m_name == _player_select_string_scoreboard ||
         responding_screen->m_name == _string_id_scoreboard_half))
    {
        result = result || start_menu != 0;
    }
    else
    {
        result = result || responding_screen != 0;
        result = result || start_menu != 0;
        result = result || screen_stacks->m_controller_screen_counts[controller_index] <= 1;
    }
    return result;
}

static __forceinline void window_manager_update_last_input_time()
{
    long milliseconds = g_user_interface_globals.milliseconds.peek();
    __lwsync();
    _InterlockedExchange(&g_user_interface_globals.last_input.value, milliseconds);
}
static __forceinline void window_manager_accept_sign_in_dialog(e_controller_index controller)
{
    g_user_interface_networking_globals.start_pending = false;
    g_user_interface_networking_globals.start_state = 0;
    g_user_interface_networking_globals.start_controller = controller;
}
static __forceinline bool window_manager_player_slot_in_state(short user_index, long state)
{
    bool result = false;
    if (user_index >= 0 && user_index < 4)
        result = g_network_session_interface_local_users[user_index].state == state;
    return result;
}
static __forceinline bool window_manager_screen_accepts_any_controller(long screen_name)
{
    bool result;
    if (screen_name == _string_id_error_ok || (screen_name > _start_menu_string_start_menu_settings_display && screen_name <= _string_id_gui_dialog_four_items))
        result = true;
    else
        result = false;
    return result;
}

// @address 0x47ec30 ?handle_global_controller_event@c_window_manager@@QAAXPAUs_event_record@@@Z
void c_window_manager::handle_global_controller_event(s_event_record *event)
{
    c_window_manager *screen_stacks = this;
    if (g_user_interface_globals.exit_start_time != -1 || screen_stacks->m_fade_held)
        return;
    e_window_index window_index;
    c_gui_screen_widget *screen = g_window_manager.get_responding_screen(event->controller, &window_index);
    long screen_name = screen ? screen->m_name : -1;
    c_controller_interface *controller = &g_controller_globals.controllers[event->controller];
    window_manager_update_last_input_time();
    if (simulation_starting_up() && window_game_is_ui_shell() && g_user_interface_networking_globals.start_pending)
    {
        if (event->type == 13 && event->component == 1)
        {
            window_manager_accept_sign_in_dialog(event->controller);
            user_interface_leave_squad(1);
        }
        return;
    }
    bool respond = controller->in_use() && !window_manager_player_slot_in_state(controller->user_index, 2);
    respond |= screen && screen->get_driving_controller() == event->controller;
    respond |= screen && screen->get_driving_controller() == k_any_controller
        && window_manager_screen_accepts_any_controller(screen->m_name);
    respond |= g_user_interface_networking_globals.error != -1 || g_user_interface_networking_globals.dialog != -1;
    respond |= screen && screen->m_name == _main_menu_string_main_menu && (event->type == 1 || event->type == 3);
    if (respond && !window_manager_bink_blocks_rendering())
    {
        void *storage = user_interface_malloc_tracked(sizeof(c_controller_input_message), 0, 0);
        c_controller_input_message *message = storage
            ? new (storage) c_controller_input_message(screen_name, event->controller, window_index, (e_event_type)event->type,
                  (e_controller_component)event->component, event->value)
            : 0;
        if (message)
            user_interface_messaging_post(message);
        return;
    }
    if (event->type == 13 && (event->component == 0 || event->component == 12))
    {
        long user_index = controller_get_or_allocate_user_index(controller);
        window_manager_play_input_sound(-1, event->type, event->component);
        long in_game = 0;
        e_network_join_refuse_reason reason = user_interface_squad_is_joinable();
        long slot_index = 0;
        do
        {
            if (g_network_session_interface_local_users[slot_index].state >= 3)
                in_game = 1;
            slot_index++;
        } while (slot_index < 4);
        if (!in_game)
        {
            if (reason != _network_join_refuse_reason_none)
            {
                network_session_set_player_failure_reason(user_index, reason);
                return;
            }
            if (g_network_session_interface_local_users[user_index].state != 1)
                g_network_session_interface_local_users[user_index].state = 12;
        }
    }
}


// @address 0x4e710 ??_7c_gui_screen_codeless_widget@@6B@

static __forceinline long window_manager_transition_in_animation_state(long transition_type)
{
    long animation_state = -1;
    switch (transition_type) {
    case 0: animation_state = -1; break;
    case 1: animation_state = 7; break;
    case 2: animation_state = 9; break;
    case 4: animation_state = 11; break;
    case 3: animation_state = 10; break;
    case 5: animation_state = 32; break;
    case 6: animation_state = 34; break;
    }
    return animation_state;
}

static __forceinline c_gui_screen_widget *window_manager_allocate_codeless_screen(long screen_name)
{
    c_gui_screen_widget *screen = 0;
    if (c_window_manager::locate_screen_definition_tag_by_name(screen_name) != NONE)
        screen = UI_MALLOC_NEW c_gui_screen_codeless_widget(screen_name);
    return screen;
}

// @address 0x47f6d8 ?load_screen@c_window_manager@@AAAPAVc_gui_screen_widget@@W4e_controller_index@@_NPBVc_load_screen_message@@W4e_window_index@@@Z
c_gui_screen_widget *c_window_manager::load_screen(
    e_controller_index controller_index, bool unused, const c_load_screen_message *message, e_window_index window_index)
{
    c_window_manager *window_manager = &g_window_manager;
    s_font_cache_thread_state font_state;
    c_window_manager *screen_stacks = window_manager;
    c_gui_screen_widget *screen = 0;
    bool success;
    {
        font_state.active = 0;
        if (g_font_cache_globals == 0) {
            internal_critical_section_enter(37);
            font_state.active = 1;
            g_font_cache_globals = &g_internal_font_cache_globals;
        }
        success = screen_stacks->m_active_screens && simulation_starting_up();
        font_cache_unlock(&font_state);
    }
    if (success) {
        {
            font_cache_lock(&font_state);
            long found_count = 0;
            for (long screen_position = screenCount(screen_stacks, window_index) - 1; screen_position >= 0; screen_position--) {
                c_gui_screen_widget *existing_screen = screen_stacks->m_screens[window_index][screen_position];
                if (existing_screen->m_name == message->get_screen_name()) {
                    if (found_count == 0) {
                        if (!existing_screen->transitioning_out())
                            screen_stacks->m_screens[window_index][screen_position]->transition_out_with_transition_type(0, message->get_transition_type());
                    }
                    else {
                        window_manager->dispose_screen(existing_screen);
                    }
                    found_count++;
                }
            }
            font_cache_unlock(&font_state);
        }
        {
            font_state.active = 0;
            if (g_font_cache_globals == 0) {
                internal_critical_section_enter(37);
                font_state.active = 1;
                g_font_cache_globals = &g_internal_font_cache_globals;
            }
            success = screenCount(screen_stacks, window_index) < 10;
            font_cache_unlock(&font_state);
        }
        if (success) {
            long screen_name = message->get_screen_name();
            if (named_screen_defined_in_code(screen_name))
                screen = window_manager->allocate_named_screen(screen_name);
            else
                screen = window_manager_allocate_codeless_screen(screen_name);
            long animation_state = NONE;
            if (screen) {
                long screen_index = datum_new(screen_stacks->m_active_screens);
                // match: 0x47f6d8: value range assumption guides branch lowering
                __assume(screen_index != NONE);
                datum_get<s_screen_handle>(screen_stacks->m_active_screens, screen_index)->screen = screen;
                screen->set_screen_index(screen_index);
                animation_state = window_manager_transition_in_animation_state(message->get_transition_type());
                screen->m_name = screen_name;
                {
                    font_cache_lock(&font_state);
                    window_manager->insert_screen_after_named_screen(window_index, screen, message->get_layered_position());
                    font_cache_unlock(&font_state);
                }
                screen->m_controller_mask &= ~0xfui64;
                if ((unsigned long)controller_index >= 4)
                    screen->m_controller_mask |= 0xfui64;
                else
                    screen->m_controller_mask |= 1ui64 << controller_index;
                if (!screen->running_in_codeless_mode() || message->get_applies_even_to_codeless_screens())
                    message->apply_initial_state(screen);
                if (message->get_focus_on_load_list_name() != NONE) {
                    if (message->get_focus_on_load_column_name() == NONE)
                        screen->set_initial_focus(message->get_focus_on_load_list_name(), message->get_focus_on_load_element_handle());
                    else
                        screen->set_initial_focus_by_name(message->get_focus_on_load_list_name(), message->get_focus_on_load_column_name(),
                            message->get_focus_on_load_column_value());
                }
                screen->initialize();
                screen->post_initialize();
                screen->set_responds_to_controller_events(message->get_respond_to_controller_events());
                long parent_screen_index = message->get_parent_screen_index();
                if (parent_screen_index != NONE) {
                    s_screen_handle *parent_handle = static_cast<s_screen_handle *>(datum_try_and_get(screen_stacks->m_active_screens, parent_screen_index));
                    if (parent_handle)
                        gui_widget_append_child(parent_handle->screen, screen);
                }
                gui_screen_widget_reload_and_update_datasources(screen);
                long current_milliseconds = g_user_interface_globals.milliseconds.peek();
                screen->update(current_milliseconds);
                screen->swap_display_group_block();
            }
            if (animation_state != NONE)
                screen->start_animation_at_time((e_animation_state)animation_state, -1, true);
        }
    }
    return screen;
}

static __forceinline bool window_manager_game_time_initialized()
{
    return game_time_globals != 0 && game_time_globals->initialized != 0;
}
static __forceinline long window_manager_game_time_get()
{
    return game_time_globals->time;
}
// @address 0x8ca78 ?process_unhandled_events@c_window_manager@@AAAXK@Z
void c_window_manager::process_unhandled_events(unsigned long current_milliseconds)
{
    if (user_interface_message_queue_is_empty())
        return;
    long current_time = window_manager_game_time_initialized() ? window_manager_game_time_get() : 0;
    const c_message *message = 0;
    bool more = user_interface_messaging_get_next_message(-1, k_any_controller, k_any_window, &message);
    if (!more)
        return;
    const c_message *current = message;
    do
    {
        long type = current->get_type();
        e_controller_index controller = current->get_controller();
        bool remove = (current_time - current->get_game_time_at_creation()) > 1;
        const c_message *next = current;
        more = user_interface_messaging_get_next_message(-1, k_any_controller, k_any_window, &next);
        switch (type)
        {
        case _ui_controller_input:
        {
            bool handled = false;
            bool allow = true;
            if (game_is_scoreboard_blocked(0) != 0 && !(simulation_starting_up() && game_is_cooperative()))
                allow = false;
            if (allow)
            {
                const c_controller_input_message *input = static_cast<const c_controller_input_message *>(current);
                bool start = c_start_menu_screen_widget::handle_global_start_button_press(input);
                handled = c_sandbox_budget_summary_screen_widget::handle_global_messages(input) | start;
            }
            if (handled)
            {
                const c_controller_input_message *input = static_cast<const c_controller_input_message *>(current);
                window_manager_play_input_sound(-1, input->get_event_type(), input->get_component());
            }
            screenCount((const c_window_manager *)&g_window_manager, _console_window);
            remove = true;
            break;
        }
        case _ui_load_screen:
            g_window_manager.load_screen(controller, false, (const c_load_screen_message *)current, current->get_window());
            remove = true;
            break;
        case _message_type_dialog_result:
            remove = window_manager_handle_dialog_result_message((const c_dialog_result_message *)current);
            break;
        }
        if (remove)
        {
            user_interface_messaging_pop(current);
            current->~c_message();
            user_interface_free(current);
        }
        current = next;
    } while (more);
}
static __forceinline c_gui_location_manager *location_manager_get()
{
    return &g_lobby_location_manager;
}
static __forceinline bool window_manager_game_options_valid()
{
    game_globals_storage *globals = tls_game_globals();
    return globals && (globals->initializing || globals->map_active);
}
static __forceinline bool window_manager_session_established(const c_network_session *session)
{
    switch (session->current_local_state()) {
    case _network_session_state_none:
    case _network_session_state_peer_creating:
    case _network_session_state_peer_joining:
    case _network_session_state_peer_join_abort:
        return false;
    case _network_session_state_peer_established:
    case _network_session_state_peer_leaving:
    case _network_session_state_host_established:
    case _network_session_state_host_disband:
    case _network_session_state_host_handoff:
    case _network_session_state_host_reestablish:
    case _network_session_state_election:
        return true;
    // match: 0x8b958: the switch assumes a valid session state (0..10).
    default: __assume(0);
    }
}



// @address 0x8c492c g_user_interface_memory_pool
static __forceinline void window_manager_ui_heap_free(s_memory_pool *heap, long handle)
{
    long header_offset = handle - 16;
    s_memory_pool_block *block = header_offset ? (s_memory_pool_block *)((char *)heap + header_offset) : 0;
    long block_size = block->size;
    heap->free_size += block_size;
    if (block->previous != 0)
        ((s_memory_pool_block *)((char *)heap + block->previous))->next = block->next;
    else
        heap->first_block_handle = block->next;
    if (block->next != 0)
        ((s_memory_pool_block *)((char *)heap + block->next))->previous = block->previous;
    else
        heap->last_block_handle = block->previous;
}
static __forceinline void window_manager_memory_pool_block_free(s_memory_pool *pool, const void **pointer)
{
    long handle = (const char *)*pointer - (const char *)pool;
    window_manager_ui_heap_free(pool, handle);
}
static __forceinline s_memory_pool *window_manager_ui_memory_pool()
{
    return g_user_interface_memory_pool;
}
static __forceinline void window_manager_user_interface_free(const void *pointer)
{
    window_manager_ui_memory_lock lock;
    window_manager_memory_pool_block_free(window_manager_ui_memory_pool(), &pointer);
}
#if !defined(_XBOX) || defined(__clang__)
static void window_manager_rescale_widget_tree(c_gui_widget *widget)
{
    if (!widget)
        return;
    s_runtime_core_widget_definition *definition = widget->get_core_definition();
    if (definition)
    {
        definition->current_bounds = definition->authored_bounds;
        real_rectangle2d bounds;
        widget->get_current_bounds(&bounds);
        scale_and_position_rectangle_for_display_resolution(
            widget->m_type, &bounds, (e_widget_anchoring)definition->unknown);
        definition->current_bounds = bounds;
    }
    for (c_gui_widget *child = widget->get_children(); child != 0; child = child->get_next())
    {
        if (child->m_type != _gui_screen_widget)
            window_manager_rescale_widget_tree(child);
    }
}
#endif
// @address 0x8b958 ?update@c_window_manager@@QAAXK@Z
void c_window_manager::update(unsigned long current_milliseconds)
{
    c_window_manager *screen_stacks = this;
    c_window_manager *fade_state = this;
    if (window_manager_game_options_valid())
    {
        location_manager_get()->update();
        for (long controller_index = 0; controller_index < 4; controller_index++)
        {
            if (get_responding_screen((e_controller_index)controller_index, 0))
                screen_stacks->m_controller_screen_counts[controller_index] = 0;
            else
                screen_stacks->m_controller_screen_counts[controller_index]++;
        }
        for (long window_index = 0; window_index < k_number_of_windows; window_index++)
        {
            for (long screen_index = 0; screen_index < screenCount(screen_stacks, (e_window_index)window_index); screen_index++)
            {
                const c_message *message = 0;
                bool more = user_interface_messaging_get_next_message(
                    screen_stacks->m_screens[window_index][screen_index]->m_name,
                    k_any_controller, (e_window_index)window_index, &message);
                while (more)
                {
                    const c_message *next_message = message;
                    more = user_interface_messaging_get_next_message(
                        screen_stacks->m_screens[window_index][screen_index]->m_name,
                        k_any_controller, (e_window_index)window_index, &next_message);
                    if (screen_stacks->m_screens[window_index][screen_index]->process_message(message))
                    {
                        user_interface_messaging_pop(message);
                        message->~c_message();
                        window_manager_user_interface_free(message);
                    }
                    message = next_message;
                }
            }
        }
        process_unhandled_events(current_milliseconds);
        for (long window_index = 0; window_index < k_number_of_windows; window_index++)
        {
            for (long screen_index = 0; screen_index < screenCount(screen_stacks, (e_window_index)window_index); screen_index++)
            {
                if (!screen_stacks->m_screens[window_index][screen_index]->transitioning_out())
                {
                    gui_screen_widget_reload_and_update_datasources(screen_stacks->m_screens[window_index][screen_index]);
                    screen_stacks->m_screens[window_index][screen_index]->update(current_milliseconds);
                    screen_stacks->m_screens[window_index][screen_index]->swap_display_group_block();
                }
            }
        }
#if !defined(_XBOX) || defined(__clang__)
        static long s_last_ui_render_width = 0;
        static long s_last_ui_render_height = 0;
        const long cur_ui_w = c_rasterizer::render_globals.width;
        const long cur_ui_h = c_rasterizer::render_globals.height;
        if (cur_ui_w != s_last_ui_render_width || cur_ui_h != s_last_ui_render_height)
        {
            if (s_last_ui_render_width != 0 && s_last_ui_render_height != 0)
            {
                calculate_authoring_to_target_dimensions_scaling();
                calculate_aspect_ratio_scaling();
                for (long window_index = 0; window_index < k_number_of_windows; window_index++)
                {
                    for (long screen_index = 0; screen_index < screenCount(screen_stacks, (e_window_index)window_index); screen_index++)
                        window_manager_rescale_widget_tree(screen_stacks->m_screens[window_index][screen_index]);
                }
            }
            s_last_ui_render_width = cur_ui_w;
            s_last_ui_render_height = cur_ui_h;
        }
#endif
        for (long window_index = 0; window_index < k_number_of_windows; window_index++)
        {
            for (long screen_index = 0; screen_index < screenCount(screen_stacks, (e_window_index)window_index); screen_index++)
            {
                window_manager_render_font_cache_lock font_lock;
                screen_stacks->m_screens[window_index][screen_index]->update_animation(current_milliseconds);
                screen_stacks->m_screens[window_index][screen_index]->update_render_state(current_milliseconds);
                c_gui_screen_widget *screen = screen_stacks->m_screens[window_index][screen_index];
                s_window_manager_screen_render_data *render_state = window_manager_screen_get_render_state(screen);
                if (render_state)
                {
                    e_controller_index controller_index = screen->get_driving_controller();
                    render_state->current_buffer_offset = 0;
                    render_state->current_count = 0;
                    memcpy(&render_state->render_bounds, &screen_stacks->m_window_bounds[window_index], sizeof(rectangle2d));
                    if (screen->should_render(0))
                        window_manager_assemble_widget_render_data(&screen_stacks->m_window_bounds[window_index],
                            screen, render_state, controller_index);
                    long render_count = render_state->current_count;
                    if (render_count > 0)
                        qsort(render_state->render_list, render_count, sizeof(s_window_manager_render_list_entry), gui_widget_depth_sort_proc);
                }
            }
        }
        bool screen_disposed;
        do
        {
            screen_disposed = false;
            for (long window_index = 0; window_index < k_number_of_windows; window_index++)
            {
                for (long screen_index = 0; screen_index < screenCount(screen_stacks, (e_window_index)window_index); screen_index++)
                {
                    if (screen_stacks->m_screens[window_index][screen_index]->can_all_children_be_disposed())
                    {
                        dispose_screen(screen_stacks->m_screens[window_index][screen_index]);
                        screen_disposed = true;
                        break;
                    }
                }
            }
        } while (screen_disposed);
        window_manager_update_sound_fade(fade_state, current_milliseconds);
    }
    if (fade_state->m_fade_held)
    {
        float fade = fade_state->m_fade;
        if (fade == 1.0f)
        {
        bool join_in_progress = false;
        c_network_session *session = 0;
        if (network_life_cycle_in_squad_session(&session) && window_manager_session_established(session))
        {
            const c_network_session_parameters *parameters = session->get_session_parameters();
            if (parameters->m_parameters.request_campaign_quit.get_allowed())
                join_in_progress = parameters->m_parameters.request_campaign_quit.get()->quit_request_type != 0;
        }
        if (!join_in_progress && !network_squad_session_get_matchmaking_tip(1, -1))
            fade_state->m_fade_held = false;
        }
    }
    fade_state->m_last_update_time = current_milliseconds;
}

static __forceinline unsigned long window_manager_get_ui_milliseconds()
{
    long milliseconds = g_user_interface_globals.milliseconds.peek();
    return milliseconds;
}

__forceinline c_window_manager::c_window_manager() :
    m_last_update_time(window_manager_get_ui_milliseconds()),
    m_script_thread_index(NONE),
    m_fade(0.0f),
    m_fade_in(false),
    m_fade_held(false)
{
    for (long window_index = 0; window_index < k_number_of_windows; window_index++)
        m_screen_count[window_index].set(0);
    for (long controller_index = 0; controller_index < 4; controller_index++)
        m_controller_screen_counts[controller_index] = 0;
}

c_window_manager g_window_manager;


