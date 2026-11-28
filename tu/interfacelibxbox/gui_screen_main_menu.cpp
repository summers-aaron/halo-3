// interfacelibxbox/gui_screen_main_menu.obj: from the records 0x4a88a0 0x4a8c18 0x4a8e80 0x4a8e88 0x4a91a8 0x4a9250 0x4a9390
#include "interface/user_interface_dialog_globals.h"
#include "bink/bink_playback_types.h"
#include "interface/gui_screen_menu_externs.h"
#include "interface/gui_screen_widget_text.h"
#include "saved_games/saved_game_files.h"
#include "main/thread_globals_lock.h"
#include "cseries/crt_xdk.h"
#include "shell/shell.h"
#include "game/game_globals_storage.h"
#include "game/player_types.h"
#include <ppcintrinsics.h>
#include <stdlib.h>
#include <string.h>
#include "game/game_options.h"
#include "interface/gui_screens/scoreboard/gui_screen_scoreboard.h"
#include "interface/user_interface.h"
#include "multithreading/synchronized_value.h"
#include "interface/user_interface_memory.h"
#include "interface/gui_pregame_setup_manager.h"
#include "networking/online/online_xenon.h"
#include "game/game.h"
#include "game/game_time.h"
#include "networking/logic/network_session_interface.h"
#include "interface/gui_screens/campaign/gui_screen_campaign_select_difficulty.h"
#include "networking/logic/network_join.h"
#include "bink/bink_playback.h"
#include "saved_games/content_catalogue.h"
#include "saved_games/game_state.h"
#include "interface/user_interface_error_manager.h"
#include "interface/user_interface_messages.h"
#include "interface/c_gui_widget.h"
#include "multithreading/threads.h"
#include "networking/network_configuration.h"
#include "networking/network_configuration_types.h"
#include "networking/online/online_guide_xenon.h"
#include "interface/user_interface_utilities.h"
#include "main/game_preferences.h"
#include "memory/tls_globals.h"
#include "cseries/placement_new.h"
#include "interface/c_player_profile.h"
#include "interface/user_interface_clock.h"
#include "interface/controller_types.h"
#include "interface/controller_inline.h"
#include "interface/gui_data.h"
#include "interface/gui_widget.h"
#include "interface/c_gui_screen_widget.h"
#include "interface/gui_screens/main_menu/gui_screen_main_menu.h"
#include "main/game_preferences_types.h"
#include "multithreading/thread_tls.h"
#include "math/real_math_constants.h"
#include "interface/user_interface_window_manager.h"
#include "interface/c_window_manager.h"
#include "interface/c_gui_list_item_widget.h"
#include "interface/gui_screen_pregame_lobby_mapeditor_types.h"
#include "interface/gui_screens/campaign/campaign_select_difficulty_message.h"
#include "main/game_preferences_game_setup.h"
#include "main/game_preferences_game_setup_copy.h"

class c_message;
class c_controller_input_message;
struct s_network_configuration;
class c_interlocked_long;
struct _bink_globals;

// @address 0x50cb0 ??_7c_load_campaign_select_difficulty_screen_message@@6B@
// @address 0x4c4bb8 ??_Gc_load_campaign_select_difficulty_screen_message@@UAAPAXI@Z
// @address 0x4c4bb8 ??_Ec_load_campaign_select_difficulty_screen_message@@UAAPAXI@Z
// @flags /TP /O2 /Oc
// @address 0x483310 ??0c_load_dialog_screen_message@@QAA@W4e_controller_index@@W4e_window_index@@JJJ@Z
// @address 0x486578 ?get_render_window@c_gui_screen_widget@@QAA?AW4e_window_index@@XZ
// @address 0x486130 ?handle_controller_input_message@c_gui_screen_widget@@UAA_NPBVc_controller_input_message@@@Z
// @address 0x39e078 ?online_guide_show_friends_ui@@YAKW4e_controller_index@@@Z
// @address 0x485350 ?dispose@c_gui_screen_widget@@UAAXXZ
// @address 0x4a8c18 ?update_main_menu@c_main_menu_screen_widget@@QAAXXZ
// @address 0x282ce8 ?content_catalogue_display_device_selection_guide_interface@@YAXW4e_controller_index@@@Z
// @address 0x47bd98 ?set_as_unsigned_in_user@c_controller_interface@@QAAX_N@Z
// @address 0x46f190 ?get_alpha_is_internal_beta@@YA_NXZ
// @address 0x909d8 ?get_alpha_is_locked_down@@YA_NXZ
// @address 0x48bfa0 ?get_child_widget@c_gui_widget@@QAAPAV1@W4e_gui_widget_type@@J@Z
// @address 0x8a4c8 ?is_signed_in_to_machine@c_controller_interface@@QBA_NXZ
// @address 0x1e9d80 ?saved_game_files_get_personal_storage_content_item@@YAJW4e_controller_index@@_N@Z
// @address 0x4a8b70 ?get_signed_in_controller_count@c_main_menu_screen_widget@@QAAJPAW4e_controller_index@@@Z
// @address 0x4a8e80 ?dispose@c_main_menu_screen_widget@@UAAXXZ
// @address 0x4a8e88 ?update@c_main_menu_screen_widget@@UAAXK@Z
// @address 0x4a91a8 ?handle_dialog_result@c_main_menu_screen_widget@@UAA_NPBVc_dialog_result_message@@@Z
// @address 0x19f3b0 ??0c_thread_globals_lock@@QAA@XZ
// @address 0x19f408 ??1c_thread_globals_lock@@QAA@XZ
// @address 0x485560 ?post_initialize@c_gui_screen_widget@@UAAXXZ
// @address 0x483240 ??0c_load_screen_message@@QAA@JW4e_controller_index@@W4e_window_index@@J@Z
// @address 0x8dbd0 ?get_screen_by_name@c_window_manager@@QAAPAVc_gui_screen_widget@@W4e_window_index@@J@Z



// c_gui_screen_widget::handle_dialog_result (inlined)
static __forceinline bool gui_screen_main_menu_widget_handle_dialog_result(const c_dialog_result_message *message)
{
    bool result = false;
    if (message->m_dialog_name == _main_menu_string_need_to_select_storage_device_to_save) {
        if (message->m_dialog_result == 0)
            content_catalogue_display_device_selection_guide_interface((e_controller_index)message->m_controller);
        result = true;
    }
    return result;
}


// @address 0x221fc k_real_constant_pool
// @address 0x11401e8 g_controller_globals

// @address 0x4a88a0 ?post_initialize@c_main_menu_screen_widget@@UAAXXZ
void c_main_menu_screen_widget::post_initialize()
{
    c_gui_data *found = 0;
    for (long i = 0; i < this->m_datasource_count; i++) {
        if (this->m_datasource[i]->get_name() == _main_menu_string_main_menu) {
            found = this->m_datasource[i];
            break;
        }
    }
    long mode = 1;
    if (g_user_interface_is_alpha) {
        mode = g_network_configuration.alpha_locked_down;
        bool eula_accepted = false;
        if (g_eula_accepted)
            eula_accepted = true;
        if (!eula_accepted)
            mode = 0;
    }
    this->mode = mode;
    controller_get_signed_in_player_identifiers(this, this->block);
    if (found) {
        long id = 0;
        bool skip = g_thread_system_initialized.value == 0 || get_current_thread_index() == 3;
        if (!skip) {
            c_thread_globals_lock lock;
            id = thread_global_preferences->primary_file.data.last_menu_selection;
        }
        this->c_main_menu_screen_widget::update_main_menu();
        bool active = bink_globals.active && bink_globals.enabled;
        this->active = active;
        if (active) {
            g_window_manager.m_fade_in = 1;
            g_window_manager.m_fade = 1.0f;
        }
        long handle;
        if (id != -1 && (handle = found->get_first_element_handle()) != -1) {
            for (;;) {
                long value = -1;
                if (found->get_string_id_value(handle, _main_menu_string_name, &value) && value == id)
                    break;
                handle = found->get_next_element_handle(handle);
                if (handle == -1) {
                    this->c_gui_screen_widget::post_initialize();
                    return;
                }
            }
            this->set_initial_focus(_main_menu_string_main_menu, handle);
        }
    }
    this->c_gui_screen_widget::post_initialize();
}

static __forceinline long gui_screen_main_menu_signed_in_count()
{
    long signed_in_count = 0;
    for (long controller_index = 0; controller_index < 4; controller_index++) {
        if (controller_in_use(controller_get((e_controller_index)controller_index)))
            signed_in_count++;
    }
    return signed_in_count;
}

#ifdef HALO3_64BIT
extern "C" unsigned long g_saved_game_personal_storage_generation;
extern "C" bool saved_game_files_read_game_options_for_player_count(
    e_controller_index controller_index, long player_count, game_options *options);

static long g_main_menu_last_signed_in_count = -1;
static unsigned long g_main_menu_last_storage_generation = 0;

static e_controller_index gui_screen_main_menu_find_resume_controller(
    e_controller_index preferred_controller,
    game_options *out_options)
{
    long signed_in_count = gui_screen_main_menu_signed_in_count();
    if (signed_in_count < 1 || signed_in_count > 4)
        return k_no_controller;

    game_options local_options;
    game_options *target_options = out_options ? out_options : &local_options;

    if ((unsigned long)preferred_controller < 4 &&
        controller_in_use(controller_get(preferred_controller)) &&
        saved_game_files_get_personal_storage_content_item(preferred_controller, false) != -1 &&
        saved_game_files_read_game_options_for_player_count(preferred_controller, signed_in_count, target_options))
    {
        return preferred_controller;
    }

    for (long i = 0; i < 4; ++i) {
        e_controller_index ctrl = (e_controller_index)i;
        if (ctrl == preferred_controller)
            continue;
        if (controller_in_use(controller_get(ctrl)) &&
            saved_game_files_get_personal_storage_content_item(ctrl, false) != -1 &&
            saved_game_files_read_game_options_for_player_count(ctrl, signed_in_count, target_options))
        {
            return ctrl;
        }
    }
    return k_no_controller;
}
#endif

// 0x4a8c18: main menu post-initialize helper
// @address 0x4a8c18 ?update_main_menu@c_main_menu_screen_widget@@QAAXXZ
void c_main_menu_screen_widget::update_main_menu()
{
    bool has_storage = false;
    for (long i = 0; i < this->m_datasource_count; i++) {
        if (this->m_datasource[i]->get_name() == _main_menu_string_main_menu) {
            c_gui_data *menu = this->m_datasource[i];
            if (menu) {
#ifdef HALO3_64BIT
                g_main_menu_last_signed_in_count = gui_screen_main_menu_signed_in_count();
                g_main_menu_last_storage_generation = g_saved_game_personal_storage_generation;
                has_storage = (gui_screen_main_menu_find_resume_controller(k_no_controller, 0) != k_no_controller);
#else
                e_controller_index controller_index = k_no_controller;
                if (this->get_signed_in_controller_count(&controller_index) == 1
                    && saved_game_files_get_personal_storage_content_item(controller_index, false) != -1)
                    has_storage = true;
#endif
                menu->clear_disabled_elements();
                if (has_storage)
                    menu->set_disabled_element(_main_menu_string_name, _main_menu_string_start_new_campaign);
                else
                    menu->set_disabled_element(_main_menu_string_name, _main_menu_string_resume_campaign);
                if (get_alpha_is_internal_beta()) {
                    menu->set_disabled_element(_main_menu_string_name, _main_menu_string_campaign);
                    menu->set_disabled_element(_main_menu_string_name, _main_menu_string_mapeditor);
                } else {
                    menu->set_disabled_element(_main_menu_string_name, _main_menu_string_leave_game);
                }
                if (get_alpha_custom_games_disabled())
                    menu->set_disabled_element(_main_menu_string_name, _main_menu_string_multiplayer);
                if (get_alpha_is_locked_down()) {
                    menu->set_disabled_element(_main_menu_string_name, _main_menu_string_matchmaking);
                    menu->set_disabled_element(_main_menu_string_name, _main_menu_string_theater);
                } else {
                    menu->set_disabled_element(_main_menu_string_name, _main_menu_string_locked);
                }
            }
            break;
        }
    }
    c_gui_widget *child = this->get_child_widget(_gui_list_widget, _main_menu_string_main_menu);
    if (child) {
        c_gui_widget *target = gui_widget_find_child_by_name(child, 5);
        if (target) {
            long signed_in_count = gui_screen_main_menu_signed_in_count();
#ifdef HALO3_64BIT
            gui_widget_set_alternate_state(target, !has_storage && signed_in_count > 1);
#else
            gui_widget_set_alternate_state(target, signed_in_count > 1);
#endif
        }
    }
}


void c_main_menu_screen_widget::update(unsigned long current_milliseconds)
{
    bool dirty = false;
    unsigned char old_flag = this->m_catalogues_busy;
    unsigned long busy_catalogue_mask;
    s_player_identifier identifiers[4];
    unsigned char flag = network_join_test_catalogues_busy(&busy_catalogue_mask);
    this->m_catalogues_busy = flag;
    if (old_flag != flag)
        dirty = true;

    controller_get_signed_in_player_identifiers(this, identifiers);
    if (memcmp(identifiers, this->block, sizeof(identifiers)) != 0) {
        memcpy(this->block, identifiers, sizeof(identifiers));
        dirty = true;
    }
#ifdef HALO3_64BIT
    long current_signed_in_count = gui_screen_main_menu_signed_in_count();
    if (current_signed_in_count != g_main_menu_last_signed_in_count ||
        g_saved_game_personal_storage_generation != g_main_menu_last_storage_generation)
    {
        dirty = true;
    }
#endif

    bool locked_down = g_user_interface_is_alpha && (!g_network_configuration.alpha_locked_down || !g_eula_accepted);
    if (locked_down && !get_alpha_is_internal()) {
        if (!g_window_manager.get_screen_by_name(_console_window, _main_menu_string_alpha_legal)) {
            void *memory = ::user_interface_malloc_tracked(sizeof(c_load_screen_message), (char const *)(0), 0);
            if (memory) {
                c_load_screen_message *message =
                    new (memory) c_load_screen_message(_main_menu_string_alpha_legal, k_any_controller, _console_window, this->m_name);
                if (message) {
                    message->set_parent_screen_index(this->m_screen_index);
                    ::user_interface_messaging_post(message);
                }
            }
        }
    } else {
        bool playing = bink_globals.active && bink_globals.enabled;
        if (!playing && !g_main_render_loading_globals.initialized) {
            bool faded_in = g_window_manager.m_fade == 1.0f;
            if (faded_in)
                g_window_manager.m_fade_in = 0;
        }
    }
    gui_screen_widget_set_display_group(
        this, 1, ::online_local_xuid_is_online_enabled() ? _main_menu_string_main_menu_online : _main_menu_string_main_menu_offline);

    long mode = 1;
    if (g_user_interface_is_alpha) {
        mode = g_network_configuration.alpha_locked_down;
        bool eula_accepted = false;
        if (g_eula_accepted)
            eula_accepted = true;
        if (!eula_accepted)
            mode = 0;
    }
    if (this->mode != mode)
        dirty = true;

    bool waiting_for_intro = ((unsigned long)g_user_interface_globals.flags.peek() >> 3) & 1;
    if (waiting_for_intro) {
        bool playing = bink_globals.active && bink_globals.enabled;
        if (!playing)
            g_user_interface_globals.flags.set_bit(3, false);
    }
    if (dirty)
        this->c_main_menu_screen_widget::update_main_menu();
    if (this->m_catalogues_busy) {
        c_gui_widget *child = this->get_child_widget(_gui_list_widget, _main_menu_string_main_menu);
        if (child) {
            c_gui_widget *target = gui_widget_find_child_by_name(child, 5);
            if (target)
                gui_widget_set_alternate_state(target, 1);
        }
    }
    c_gui_screen_widget::update(current_milliseconds);
}

void c_main_menu_screen_widget::dispose()
{
    c_gui_screen_widget::dispose();
}


bool c_main_menu_screen_widget::handle_dialog_result(const c_dialog_result_message *message)
{
    bool handled = false;
    if (message->m_dialog_name == _main_menu_string_back_out_main_menu_no_profile) {
        if (message->m_dialog_result == 0)
            controller_get((e_controller_index)message->m_controller)->set_as_unsigned_in_user(false);
        handled = true;
    }
    if (!handled)
        handled = gui_screen_main_menu_widget_handle_dialog_result(message);
    return handled;
}


static __forceinline bool gui_screen_main_menu_input_suppressed()
{
    return g_window_manager.m_fade_in || g_window_manager.m_fade > 0.0001f;
}
static __forceinline bool gui_screen_main_menu_unsigned_in_user(e_controller_index controller)
{
    return (_rotl(g_controller_globals.controllers[controller].flags, 30) & 1) != 0;
}

// @address 0x4a9250 ?handle_controller_input_message@c_main_menu_screen_widget@@UAA_NPBVc_controller_input_message@@@Z
bool c_main_menu_screen_widget::handle_controller_input_message(const c_controller_input_message *message)
{
    bool suppressed = gui_screen_main_menu_input_suppressed();
    bool handled = false;
    if (!suppressed && message->m_event_type == _event_type_button_press) {
        switch (message->m_component) {
        case 1:
            if (gui_screen_main_menu_unsigned_in_user(message->m_controller)) {
                void *memory = ::user_interface_malloc_tracked(sizeof(c_load_dialog_screen_message), (char const *)(0), 0);
                if (memory) {
                    long name = m_name;
                    e_controller_index controller = message->m_controller;
                    c_load_dialog_screen_message *dialog = new (memory)
                        c_load_dialog_screen_message(controller, get_render_window(), name, _main_menu_string_back_out_main_menu_no_profile, name);
                    if (dialog)
                        ::user_interface_messaging_post(dialog);
                }
                handled = true;
            }
            break;
        case 3:
            if (::online_local_xuid_is_online_enabled()) {
                online_guide_show_friends_ui(message->m_controller);
                handled = true;
            }
            break;
        }
    }
    if (!handled)
        handled = c_gui_screen_widget::handle_controller_input_message(message);
    return handled;
}


static __forceinline s_user_interface_globals *gui_screen_main_menu_ui_clock_get()
{
    return &g_user_interface_globals;
}
static __forceinline void gui_screen_main_menu_set_campaign_controller(long value)
{
    __lwsync();
    _InterlockedExchange(&gui_screen_main_menu_ui_clock_get()->active_controller.value, value);
}
static __forceinline bool gui_screen_main_menu_check_blocked()
{
    bool blocked = false;
    gui_screen_campaign_select_difficulty_check_blocked(&blocked);
    return blocked;
}

// @address 0x4a9390 ?handle_list_item_chosen@c_main_menu_screen_widget@@UAA_NPBVc_controller_input_message@@JPAVc_gui_list_item_widget@@PAVc_gui_data@@@Z
bool c_main_menu_screen_widget::handle_list_item_chosen(
    const c_controller_input_message *message, long list_name, c_gui_list_item_widget *list_item_widget, c_gui_data *data)
{
    bool suppressed = gui_screen_main_menu_input_suppressed();
    bool handled = false;
    if (!suppressed && list_name == _main_menu_string_main_menu) {
        long value = -1;
        if (data->get_string_id_value(list_item_widget->get_element_handle(), _main_menu_string_name, &value)) {
            long game_mode = _gui_game_mode_none;
            switch (value) {
            case _main_menu_string_campaign:
                if (!gui_screen_main_menu_check_blocked())
                    game_mode = _gui_game_mode_campaign;
                break;
            case _main_menu_string_matchmaking:
                game_mode = _gui_game_mode_matchmaking;
                break;
            case _main_menu_string_multiplayer:
                game_mode = _gui_game_mode_multiplayer;
                break;
            case _main_menu_string_locked:
            {
                void *memory = ::user_interface_malloc_tracked(sizeof(c_load_screen_message), (char const *)(0), 0);
                if (memory) {
                    c_load_screen_message *load =
                        new (memory) c_load_screen_message(_main_menu_string_alpha_locked_down, k_any_controller, _console_window, _main_menu_string_top_most);
                    if (load) {
                        ::user_interface_messaging_post(load);
                        handled = true;
                    }
                }
                break;
            }
            case _main_menu_string_leave_game:
                if (!message->m_handled && gui_screen_main_menu_ui_clock_get()->exit_start_time == -1) {
                    g_window_manager.m_fade_in = 1;
                    gui_screen_main_menu_ui_clock_get()->exit_start_time = GetTickCount();
                }
                handled = true;
                break;
            case _main_menu_string_start_new_campaign:
                if (!this->m_catalogues_busy) {
                    global_preferences_set_last_menu_selection(value);
                    if (gui_screen_main_menu_signed_in_count() == 1) {
                        c_player_profile_interface *profile =
                            g_controller_globals.controllers[message->m_controller].get_player_profile_interface();
                        if (profile) {
                            gui_screen_main_menu_set_campaign_controller(-1);
                            if (!gui_screen_main_menu_check_blocked()) {
                                c_load_campaign_select_difficulty_screen_message *load =
                                    (c_load_campaign_select_difficulty_screen_message *)::user_interface_malloc_tracked(
                                        sizeof(c_load_campaign_select_difficulty_screen_message), (char const *)(0), 0);
                                if (load) {
                                    long difficulty = profile->campaign.look_sensitivity;
                                    long name = this->m_name;
                                    e_controller_index controller = message->m_controller;
                                    e_window_index window = this->get_render_window();
                                    c_load_screen_message *posted = new (load) c_load_campaign_select_difficulty_screen_message(
                                        controller, window, name, 0, _campaign_id_pre_game_fallback, _map_id_pre_game_fallback, difficulty);
                                    load->set_parent_screen_index(this->m_screen_index);
                                    ::user_interface_messaging_post(posted);
                                }
                            }
                        }
                    } else {
                        g_gui_error_manager.post_error_with_custom_message(0x2000e, 0, 0, message->m_controller, false, true);
                    }
                } else {
                    user_interface_play_sound(_ui_sound_error, -1);
                }
                handled = true;
                break;
            case _main_menu_string_resume_campaign:
                if (!this->m_catalogues_busy) {
                    global_preferences_set_last_menu_selection(value);
#ifdef HALO3_64BIT
                    long signed_in_count = gui_screen_main_menu_signed_in_count();
                    if (signed_in_count >= 1 && signed_in_count <= 4) {
                        e_controller_index caller_ctrl = (e_controller_index)message->m_controller;
                        if ((unsigned long)caller_ctrl < 4 && controller_in_use(controller_get(caller_ctrl))) {
                            game_options options;
                            e_controller_index save_controller =
                                gui_screen_main_menu_find_resume_controller(caller_ctrl, &options);
                            if (save_controller != k_no_controller) {
                                if (!gui_screen_main_menu_check_blocked()) {
                                    e_controller_index start_controller = save_controller;
                                    for (long i = 0; i < 4; ++i) {
                                        if (controller_in_use(controller_get((e_controller_index)i))) {
                                            start_controller = (e_controller_index)i;
                                            break;
                                        }
                                    }
                                    s_gui_game_setup_storage setup;
                                    global_preferences_get_last_game_setup(&setup);
                                    setup.campaign_settings.valid = true;
                                    setup.campaign_settings.difficulty_level = options.campaign_difficulty;
                                    setup.campaign_settings.campaign_id = options.campaign_id;
                                    setup.campaign_settings.map_id = options.map_id;
                                    global_preferences_set_last_game_setup(&setup);
                                    gui_screen_main_menu_set_campaign_controller(save_controller);
                                    if (gui_pregame_setup_manager_initialize_squad_for_mode(0)) {
                                        g_user_interface_networking_globals.start_pending = true;
                                        g_user_interface_networking_globals.start_controller = start_controller;
                                        g_user_interface_networking_globals.start_state = 1;
                                    }
                                }
                            } else {
                                g_gui_error_manager.post_error_with_custom_message(0x2000b, 0, 0, message->m_controller, false, true);
                            }
                        }
                    } else {
                        g_gui_error_manager.post_error_with_custom_message(0x2000c, 0, 0, message->m_controller, false, true);
                    }
#else
                    e_controller_index first_controller;
                    if (this->get_signed_in_controller_count(&first_controller) == 1) {
                        e_controller_index controller = first_controller;
                        if (message->m_controller == controller) {
                            game_options options;
                            if (game_state_read_game_options_from_persistent_storage_blocking(controller, &options)) {
                                if (!gui_screen_main_menu_check_blocked()) {
                                    s_gui_game_setup_storage setup;
                                    global_preferences_get_last_game_setup(&setup);
                                    setup.campaign_settings.valid = true;
                                    setup.campaign_settings.difficulty_level = options.campaign_difficulty;
                                    setup.campaign_settings.campaign_id = options.campaign_id;
                                    setup.campaign_settings.map_id = options.map_id;
                                    global_preferences_set_last_game_setup(&setup);
                                    gui_screen_main_menu_set_campaign_controller(controller);
                                    if (gui_pregame_setup_manager_initialize_squad_for_mode(0)) {
                                        g_user_interface_networking_globals.start_pending = true;
                                        g_user_interface_networking_globals.start_controller = controller;
                                        g_user_interface_networking_globals.start_state = 1;
                                    }
                                }
                            } else {
                                g_gui_error_manager.post_error_with_custom_message(0x2000b, 0, 0, message->m_controller, false, true);
                            }
                        }
                    } else {
                        g_gui_error_manager.post_error_with_custom_message(0x2000c, 0, 0, message->m_controller, false, true);
                    }
#endif
                } else {
                    user_interface_play_sound(_ui_sound_error, -1);
                }
                handled = true;
                break;
            case _main_menu_string_mapeditor:
                game_mode = _gui_game_mode_mapeditor;
                break;
            case _main_menu_string_theater:
                game_mode = _gui_game_mode_theater;
                break;
            }
            if (game_mode != _gui_game_mode_none) {
                global_preferences_set_last_menu_selection(value);
                gui_pregame_setup_manager_initialize_squad_for_mode(game_mode);
                handled = true;
            }
        }
    }
    return handled;
}
