// interfacelibxbox/user_interface_error_manager.obj: from the records 0x8c580 0x8ced8 0x8d1f0 0x8d360 0x8d7f8 0x95e28 0x95ea0 0x494598 0x494650 0x494790 0x494920 0x494990 0x4c0e18
// @flags /TP /O2 /Oc
#include "memory/static_arrays.h"
#include "interface/user_interface_clock.h"
#include "interface/controller_inline.h"
#include "memory/tls_globals.h"
#include "multithreading/synchronized_value.h"
#include <string.h>
#include "tag_files/tag_groups.h"
#include "tag_files/string_ids.h"
#include "shell/shell.h"
#include "game/game_options.h"
#include "game/game_globals_storage.h"
#include "interface/gui_widget.h"
#include "interface/user_interface_messages.h"
// @address 0x1bfe40 ?set@?$c_static_wchar_string@$0BAA@@@QAAXPB_W@Z
#include <ppcintrinsics.h>
#include <stdlib.h>
#include "interface/gui_screens/scoreboard/gui_screen_scoreboard.h"
#include "interface/user_interface.h"
#include "interface/user_interface_memory.h"
#include "interface/user_interface_window_manager.h"
#include "game/players.h"
#include "interface/user_interface_error_manager.h"
#include "interface/controller_types.h"
#include "interface/window_manager_screen_header_types.h"
#include "interface/c_gui_screen_widget.h"
#include "interface/c_window_manager.h"
#include "interface/user_interface_shared_globals.h"
#include "interface/gui_screens/alert/gui_screen_alert.h"
#include "game/game_state_accessors.h"
#include <new>

enum e_alert_display_mode { _alert_display_mode_none, _alert_display_mode_ui, _alert_display_mode_in_game };

struct s_user_interface_error_manager_window_alert
{
    c_gui_queued_error *error;
    long screen_name;
};

// 0x95e28: the first queued error that matches, or null
// @address 0x95e28 gui_error_manager_find_queued_error
extern "C" c_gui_queued_error *gui_error_manager_find_queued_error(
    c_gui_queued_error *errors, e_controller_index controller_index, long error_name, const wchar_t *custom_message)
{
    c_gui_queued_error *result = 0;
    for (unsigned int index = 0; index < 32; index++)
    {
        if (gui_queued_error_is_match(&errors[index], controller_index, error_name, custom_message))
        {
            result = &errors[index];
            break;
        }
    }
    return result;
}

// @address 0x95ea0 gui_queued_error_is_match
bool gui_queued_error_is_match(const c_gui_queued_error *error, e_controller_index controller_index, long error_name, const wchar_t *custom_message)
{
    bool result = false;
    if (error->is_valid() && error->error_name == error_name && (controller_index == k_any_controller || error->controller_index == controller_index))
    {
        if (error->custom_message.length() <= 0 && (custom_message == 0 || wcslen(custom_message) == 0))
            result = true;
        else
            result = custom_message != 0 && wcsncmp(error->custom_message.get_string(), custom_message, 256) == 0;
    }
    return result;
}

// @address 0x494598 ?set@c_gui_queued_error@@QAAXPAUs_gui_alert_description@@PB_W1W4e_controller_index@@J_N3@Z
void c_gui_queued_error::set(s_gui_alert_description *description, const wchar_t *custom_title, const wchar_t *custom_message,
    e_controller_index controller_index, long posted_time, bool requires_resolution, bool final_flag)
{
    this->error_name = description->name;
    this->category = description->category;
    this->icon = description->icon;
    this->title = description->title;
    this->message = description->message;
    this->controller_index = controller_index;
    this->auto_dismissable = (description->flags & 1) != 0;
    this->show_buttons = (description->flags & 2) != 0;
    this->custom_title.set(custom_title ? custom_title : L"");
    this->custom_message.set(custom_message ? custom_message : L"");
    this->valid = true;
    this->requires_resolution = requires_resolution;
    this->final_flag = final_flag;
    this->time = posted_time;
}

static __forceinline const s_user_interface_shared_globals *user_interface_error_manager_shared_tag_globals_try_and_get()
{
    const s_user_interface_shared_globals *result = 0;
    const s_user_interface_tag_globals *globals = user_interface_tag_globals_try_and_get();
    if (globals && globals->shared_globals.index != -1)
        result = (const s_user_interface_shared_globals *)tag_get('wigl', globals->shared_globals.index);
    return result;
}

// @address 0x8a1eae g_utility_drive_was_formatted

// @address 0x494650 ?initialize_for_new_map@c_gui_error_manager@@QAAXXZ
void c_gui_error_manager::initialize_for_new_map()
{
    const s_user_interface_shared_globals *shared_globals = ::user_interface_shared_tag_globals_try_and_get();
    if (shared_globals)
    {
        for (unsigned int index = 0; index < 32; index++)
        {
            if (errors[index].is_valid())
            {
                for (long description_index = 0; description_index < shared_globals->alert_descriptions.count; description_index++)
                {
                    s_gui_alert_description *description = &((s_gui_alert_description *)shared_globals->alert_descriptions.address)[description_index];
                    if (description->name == errors[index].get_error_name())
                    {
                        errors[index].set(description, errors[index].get_custom_title(), errors[index].get_custom_message(),
                            errors[index].get_controller_index(), errors[index].get_posted_time(), errors[index].get_requires_resolution(),
                            errors[index].get_final_flag());
                        break;
                    }
                }
            }
        }
    }
    if (!warning_posted && g_utility_drive_was_formatted)
    {
        post_error_with_custom_message(0x20080, 0, 0, k_any_controller, false, true);
        warning_posted = true;
    }
}

// @address 0x494790 ?post_error_with_custom_message@c_gui_error_manager@@QAAXJPB_W0W4e_controller_index@@_N2@Z
void c_gui_error_manager::post_error_with_custom_message(long error_name, const wchar_t *custom_title, const wchar_t *custom_message,
    e_controller_index controller_index, bool requires_resolution, bool final_flag)
{
    bool found = false;
    for (unsigned int index = 0; index < 32; index++)
    {
        if (gui_queued_error_is_match(&errors[index], controller_index, error_name, custom_message) &&
            errors[index].get_controller_index() == controller_index && errors[index].get_error_name() == error_name)
        {
            found = true;
            if (requires_resolution && errors[index].get_resolved())
            {
                errors[index].set_resolved(false);
                errors[index].set_posted_time(g_user_interface_globals.milliseconds.peek());
            }
            break;
        }
    }
    if (!found)
    {
        for (unsigned int index = 0; index < 32; index++)
        {
            if (!errors[index].is_valid())
            {
                const s_user_interface_shared_globals *shared_globals = user_interface_error_manager_shared_tag_globals_try_and_get();
                if (shared_globals)
                {
                    for (long description_index = 0; description_index < shared_globals->alert_descriptions.count; description_index++)
                    {
                        s_gui_alert_description *description = &((s_gui_alert_description *)shared_globals->alert_descriptions.address)[description_index];
                        if (description->name == error_name)
                        {
                            errors[index].set(description, 0, custom_message, controller_index, g_user_interface_globals.milliseconds.peek(),
                                requires_resolution, final_flag);
                            break;
                        }
                    }
                }
                break;
            }
        }
    }
}

// @address 0x494920 ?clear_error@c_gui_error_manager@@QAAXJW4e_controller_index@@PB_W@Z
void c_gui_error_manager::clear_error(long error_name, e_controller_index controller_index, const wchar_t *custom_message)
{
    for (unsigned int index = 0; index < 32; index++)
    {
        if (gui_queued_error_is_match(&errors[index], controller_index, error_name, custom_message))
        {
            errors[index].clear();
            return;
        }
    }
}

int queued_error_sort_proc(const void *first, const void *second)
{
    const c_gui_queued_error *a = (const c_gui_queued_error *)first;
    const c_gui_queued_error *b = (const c_gui_queued_error *)second;
    int result;
    if (a->is_valid() && b->is_valid())
    {
        if (a->get_error_category() > b->get_error_category()) result = -1;
        else if (a->get_error_category() < b->get_error_category()) result = 1;
        else if (a->get_posted_time() < b->get_posted_time()) result = -1;
        else if (a->get_posted_time() > b->get_posted_time()) result = 1;
        else result = 0;
    }
    else if (a->is_valid()) result = -1;
    else if (b->is_valid()) result = 1;
    else result = 0;
    return result;
}

// 0x8c580: how many controllers are in use (signed in, or bit 2 of their flags)
static __forceinline e_controller_index user_interface_error_manager_controller_from_index(long index)
{
    e_controller_index controller_index;
    switch (index)
    {
    case 0: controller_index = _controller0; break;
    case 1: controller_index = _controller1; break;
    case 2: controller_index = _controller2; break;
    case 3: controller_index = _controller3; break;
    default: controller_index = k_no_controller; break;
    }
    return controller_index;
}

// @address 0x8c580 user_interface_error_manager_count_active_controllers
extern "C" unsigned int user_interface_error_manager_count_active_controllers(void)
{
    unsigned int count = 0;
    for (e_controller_index controller_index = first_controller();
        controller_index != k_no_controller;
        controller_index = user_interface_error_manager_controller_from_index(controller_index + 1))
    {
        if (controller_in_use(controller_get(controller_index)))
            count++;
    }
    return count;
}

static __forceinline bool user_interface_error_manager_is_alert_dialog(long name)
{
    return name == _string_id_gui_alert_nonblocking || name == _string_id_gui_alert_toast
        || name == _string_id_gui_alert_ingame_full || name == _string_id_gui_alert_ingame_split;
}

// 0x8d1f0: how errors show (in the UI, or in game while a game runs outside the UI shell), and the controller of the
// topmost screen that is not an alert dialog
// @address 0x8d1f0 user_interface_error_manager_get_display_mode
extern "C" e_alert_display_mode user_interface_error_manager_get_display_mode(e_controller_index *controller_index)
{
    e_alert_display_mode display_mode = _alert_display_mode_ui;
    c_window_manager *window_manager = &g_window_manager;
    if (controller_index)
        *controller_index = k_any_controller;
    if (game_in_progress() && !game_is_ui_shell())
        display_mode = _alert_display_mode_in_game;
    for (c_gui_screen_widget *screen = window_manager->get_topmost_screen(_console_window);
        screen != 0;
        screen = window_manager->get_screen_below(_console_window, screen))
    {
        if (!user_interface_error_manager_is_alert_dialog(screen->get_name()))
        {
            display_mode = _alert_display_mode_ui;
            if (controller_index)
                *controller_index = screen->get_driving_controller();
            break;
        }
    }
    return display_mode;
}

// 0x8d360: drops the queued errors of controllers no longer in use, and while errors show in the UI, the errors of
// other controllers that are auto-dismissable (March c_gui_error_manager::clean_out_error_queue)
// @address 0x8d360 user_interface_error_manager_clear_stale_errors
extern "C" void user_interface_error_manager_clear_stale_errors(c_gui_error_manager *error_manager)
{
    e_controller_index controller_index;
    e_alert_display_mode display_mode = user_interface_error_manager_get_display_mode(&controller_index);
    for (unsigned int index = 0; index < 32; index++)
    {
        c_gui_queued_error *error = &error_manager->errors[index];
        if (error->is_valid() && error->get_controller_index() != k_any_controller)
        {
            if (!controller_in_use(controller_get(error->get_controller_index())))
                error->clear();
            else if (display_mode == _alert_display_mode_ui && controller_index != k_any_controller &&
                error->get_controller_index() != controller_index && error->get_auto_dismissable())
                error->clear();
        }
    }
}
// 0x8d7f8: the first valid queued error for this window, or null
// @address 0x8d7f8 gui_error_manager_find_window_error
extern "C" c_gui_queued_error *gui_error_manager_find_window_error(c_gui_error_manager *error_manager, e_window_index window_index)
{
    c_gui_queued_error *result = 0;
    for (unsigned int index = 0; index < 32; index++)
    {
        c_gui_queued_error *error = &error_manager->errors[index];
        if (!error->is_valid())
            continue;
        if (error->get_controller_index() == k_any_controller)
        {
            if (window_index != 4)
                continue;
        }
        else if (player_get_user_from_controller(error->get_controller_index()) != window_index)
            continue;
        result = &error_manager->errors[index];
        break;
    }
    return result;
}

// 0x4c0e18: the queued error the alert screen shows, as the error manager still has it (null when it is gone)
// @address 0x4c0e18 gui_error_manager_find_alert_screen_error
extern "C" c_gui_queued_error *gui_error_manager_find_alert_screen_error(c_gui_alert_screen_widget *alert_screen)
{
    c_gui_queued_error *result = 0;
    if (alert_screen->m_error.is_valid())
        result = gui_error_manager_find_queued_error(g_gui_error_manager.errors, alert_screen->m_error.get_controller_index(),
            alert_screen->m_error.get_error_name(), alert_screen->m_error.get_custom_message());
    return result;
}

// @address 0x8ced8 gui_error_manager_update
extern "C" void gui_error_manager_update(c_gui_error_manager *error_manager)
{
    e_controller_index display_controller;
    e_alert_display_mode display_mode = user_interface_error_manager_get_display_mode(&display_controller);
    s_user_interface_error_manager_window_alert window_alerts[5] = {0};
    user_interface_error_manager_clear_stale_errors(error_manager);
    qsort(error_manager->errors, 32, sizeof(c_gui_queued_error), queued_error_sort_proc);
    if (display_mode == _alert_display_mode_ui)
    {
        c_gui_queued_error *current_error = 0;
        for (unsigned int index = 0; index < 32; ++index)
        {
            if (error_manager->errors[index].is_valid() && (display_controller == k_any_controller ||
                error_manager->errors[index].controller_index == k_any_controller ||
                error_manager->errors[index].controller_index == display_controller))
            {
                current_error = &error_manager->errors[index];
                break;
            }
        }
        window_alerts[4].error = current_error;
        window_alerts[4].screen_name = current_error && !current_error->get_final_flag() ? _string_id_gui_alert_nonblocking : _string_id_gui_alert_toast;
    }
    else
    {
        for (long window_index = 0; window_index < 5; ++window_index)
        {
            window_alerts[window_index].error = gui_error_manager_find_window_error(error_manager, (e_window_index)window_index);
            if (window_index == 4)
            {
                c_gui_queued_error *error = window_alerts[4].error;
                window_alerts[4].screen_name = error && !error->get_final_flag() ? _string_id_gui_alert_nonblocking : _string_id_gui_alert_ingame_full;
            }
            else if (user_interface_error_manager_count_active_controllers() >= 2)
                window_alerts[window_index].screen_name = _string_id_gui_alert_ingame_split;
            else
                window_alerts[window_index].screen_name = _string_id_gui_alert_ingame_full;
        }
    }
    for (long window_index = 0; window_index < 5; ++window_index)
    {
        c_gui_queued_error *error = window_alerts[window_index].error;
        c_gui_alert_screen_widget *current_screen = static_cast<c_gui_alert_screen_widget *>(window_manager_get_alert_screen((e_window_index)window_index));
        unsigned char mode_changed = error_manager->previous_display_mode != display_mode;
        int higher_priority_pending;
        if (error && current_screen)
        {
            int category = error->get_error_category();
            c_gui_queued_error *current_error = gui_error_manager_find_alert_screen_error(current_screen);
            int current_category = current_error ? current_error->get_error_category() : 0;
            higher_priority_pending = category > current_category ? 1 : 0;
        }
        else
            higher_priority_pending = 0;
        unsigned char refresh_required = mode_changed | higher_priority_pending;
        c_gui_screen_widget *topmost_screen = g_window_manager.get_topmost_screen((e_window_index)window_index);
        unsigned char alert_needs_update = (current_screen != topmost_screen) | refresh_required;
        if (current_screen && alert_needs_update)
            current_screen->transition_out_with_transition_type(0, _screen_transition_normal);
        if (error && (!current_screen || alert_needs_update))
        {
            c_load_alert_screen_message *message = (c_load_alert_screen_message *)user_interface_malloc_tracked(sizeof(c_load_alert_screen_message), 0, 0);
            if (message)
            {
                new (message) c_load_alert_screen_message(window_alerts[window_index].screen_name,
                    error->controller_index, (e_window_index)window_index, error);
                ::user_interface_messaging_post(message);
            }
        }
    }
    error_manager->previous_display_mode = (c_gui_error_manager::e_alert_display_mode)display_mode;
}

__forceinline c_gui_error_manager::c_gui_error_manager()
    : previous_display_mode(_alert_display_mode_ui), warning_posted(false)
{
}

c_gui_error_manager g_gui_error_manager;

