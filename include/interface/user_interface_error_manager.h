#ifndef INTERFACE_USER_INTERFACE_ERROR_MANAGER_H
#define INTERFACE_USER_INTERFACE_ERROR_MANAGER_H

#include "memory/static_arrays.h"
#include "cseries/cseries_wrapped.h"
#include "shell/shell.h"
#include <stddef.h>

// March queued-error asserts attest interface/user_interface_error_manager.h.
// Categories for grouping queued GUI errors.
enum e_gui_error_category
{
    k_gui_error_category_count = 4
};

// The category enum fits in a single byte; alert lookup
// retains its offset and also reads the unsigned icon byte at +6.
struct s_gui_alert_description
{
    long name;
    unsigned char flags;
    c_enum<e_gui_error_category, unsigned char, 0, k_gui_error_category_count> category;
    unsigned char icon;
    unsigned char alignment;
    long title;
    long message;
};

typedef char gui_alert_description_size[(sizeof(s_gui_alert_description) == 0x10) ? 1 : -1];
typedef char gui_alert_description_category_offset[(offsetof(s_gui_alert_description, category) == 5) ? 1 : -1];
typedef char gui_alert_description_icon_offset[(offsetof(s_gui_alert_description, icon) == 6) ? 1 : -1];
typedef char gui_alert_description_title_offset[(offsetof(s_gui_alert_description, title) == 8) ? 1 : -1];
typedef char gui_alert_description_message_offset[(offsetof(s_gui_alert_description, message) == 12) ? 1 : -1];

class c_gui_queued_error
{
public:
    __forceinline c_gui_queued_error()
        : valid(false), custom_title(), custom_message()
    {
    }
    bool valid;
    bool requires_resolution;
    bool auto_dismissable;
    bool show_buttons;
    long error_name;
    int category;
    int icon;
    long time;
    long title;
    long message;
    e_controller_index controller_index;
    c_static_wchar_string<256> custom_title;
    c_static_wchar_string<256> custom_message;
    bool final_flag;
    __forceinline bool is_valid() const { return valid; }
    __forceinline int get_error_category() const { return category; }
    __forceinline long get_posted_time() const { return time; }
    void set(s_gui_alert_description *description, const wchar_t *custom_title, const wchar_t *custom_message,
        e_controller_index controller_index, long posted_time, bool requires_resolution, bool final_flag);
    __forceinline void clear() { valid = false; }
    __forceinline long get_error_name() const { return error_name; }
    __forceinline bool get_auto_dismissable() const { return auto_dismissable; }
    __forceinline long get_title() const { return title; }
    __forceinline long get_message() const { return message; }
    __forceinline bool get_requires_resolution() const { return requires_resolution; }
    __forceinline bool get_final_flag() const { return final_flag; }
    __forceinline const wchar_t *get_custom_title() const
    {
        if (custom_title.length() > 0)
            return custom_title.get_string();
        else
            return 0;
    }
    __forceinline bool get_resolved() const { return !requires_resolution; }
    __forceinline void set_resolved(bool resolved) { requires_resolution = !resolved; }
    __forceinline void set_posted_time(long posted_time) { time = posted_time; }
    __forceinline e_controller_index get_controller_index() const { return controller_index; }
    __forceinline const wchar_t *get_custom_message() const
    {
        if (custom_message.length() > 0)
            return custom_message.get_string();
        else
            return 0;
    }
};

class c_gui_error_manager
{
public:
    // Lifecycle, per-frame update and error queue management.
    c_gui_error_manager();
    __forceinline void initialize()
    {
        for (unsigned long index = 0; index < sizeof(errors) / sizeof(errors[0]); index++)
            errors[index].clear();
    }
    void dispose();
    void initialize_for_new_map();
    void dispose_from_old_map();
    void update(unsigned long current_milliseconds);
    // Delegates to the custom-message overload, supplying defaults for the
    // custom-title argument and final flag; its callers use null/null/true.
    void post_error(long error_name, e_controller_index controller_index, bool requires_resolution)
    {
        post_error_with_custom_message(error_name, 0, 0, controller_index, requires_resolution, true);
    }
    void post_error_with_custom_message(long error_name, const wchar_t *custom_title, const wchar_t *custom_message,
        e_controller_index controller_index, bool requires_resolution, bool final_flag);
    void clear_error(long error_name, e_controller_index controller_index, const wchar_t *custom_message);
    void resolve_error(long error_name, e_controller_index controller_index);
    void clear_all_errors();
    void dismiss_auto_dismissable_errors(e_controller_index controller_index);
    const c_gui_queued_error *get_error(e_controller_index controller_index, long error_name);
    __forceinline bool any_error_active_for_controller(e_controller_index controller_index)
    {
        return get_current_for_user(controller_index) != 0;
    }
    bool any_error_active_for_window(e_window_index window_index);

    enum e_alert_display_mode { _alert_display_mode_none, _alert_display_mode_ui, _alert_display_mode_in_game };
    c_gui_queued_error errors[32];
    e_alert_display_mode previous_display_mode;            // update compares/stores it
    bool warning_posted;

private:
    e_alert_display_mode get_error_display_mode(e_controller_index *controller_index);
    __forceinline const c_gui_queued_error *get_current_for_user(e_controller_index controller_index)
    {
        const c_gui_queued_error *result = 0;
        for (unsigned long index = 0; index < sizeof(errors) / sizeof(errors[0]); index++) {
            if (errors[index].is_valid()) {
                if (controller_index == k_any_controller || errors[index].get_controller_index() == k_any_controller ||
                    errors[index].get_controller_index() == controller_index)
                {
                    result = &errors[index];
                    break;
                }
            }
        }
        return result;
    }
    const c_gui_queued_error *get_current_for_window(e_window_index window_index);
    void clean_out_error_queue();
    void sort_queue();
};

typedef char c_gui_queued_error_size[(sizeof(c_gui_queued_error) == 0x424) ? 1 : -1];
typedef char c_gui_queued_error_custom_title_offset[(offsetof(c_gui_queued_error, custom_title) == 0x20) ? 1 : -1];
typedef char c_gui_queued_error_custom_message_offset[(offsetof(c_gui_queued_error, custom_message) == 0x220) ? 1 : -1];
typedef char c_gui_queued_error_final_flag_offset[(offsetof(c_gui_queued_error, final_flag) == 0x420) ? 1 : -1];
typedef char c_gui_error_manager_display_mode_offset[(offsetof(c_gui_error_manager, previous_display_mode) == 0x8480) ? 1 : -1];
typedef char c_gui_error_manager_warning_offset[(offsetof(c_gui_error_manager, warning_posted) == 0x8484) ? 1 : -1];
typedef char c_gui_error_manager_size[(sizeof(c_gui_error_manager) == 0x8488) ? 1 : -1];

// Functions defined in tu/interfacelibxbox/user_interface_error_manager.cpp.

// The shared GUI error queue and its display state.
extern "C" c_gui_error_manager g_gui_error_manager;

__forceinline c_gui_error_manager *user_interface_error_manager_get()
{
    return &g_gui_error_manager;
}

extern "C" bool gui_queued_error_is_match(const c_gui_queued_error *error, e_controller_index controller_index, long error_name, const wchar_t *custom_message);

extern "C" bool g_utility_drive_was_formatted;
class c_gui_alert_screen_widget;
extern "C" c_gui_queued_error *gui_error_manager_find_alert_screen_error(c_gui_alert_screen_widget *alert_screen);

#endif // INTERFACE_USER_INTERFACE_ERROR_MANAGER_H
