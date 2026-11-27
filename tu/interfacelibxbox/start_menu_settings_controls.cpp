// interfacelibxbox/start_menu_settings_controls.obj: from the records 0x4a5398 0x4b92e0 0x4b9518 0x4b9600 0x4b9718 0x4b9918 0x4b99e8 0x4b9b70 0x4b9bf0 0x4ba030 0x4ba8b0 0x4ba970 0x4ba9f0 0x4bd600 0x4bd850 0x4bd8d8 0x4bd9f0 0x4bdae8 0x4bdce8 0x4bdd80 0x4bed18 0x4bed78 0x4bee58 0x4bf048 0x4bf2d8 0x4bf4d8 0x4bf560 0x4bf678 0x4bf888 0x4bf920
// @flags /TP /O2 /J /Oc
#include "memory/static_arrays.h"
#include "main/thread_globals_lock.h"
#include "main/game_preferences.h"
#include "interface/voice_output_setting_types.h"
#include "multithreading/thread_tls.h"
#include <stddef.h>
#include "interface/controller_types.h"
#include "interface/controller_inline.h"
#include "shell/shell.h"
#include "interface/gui_widget_types.h"
#include "interface/user_interface_messages.h"
#include "interface/user_interface_memory.h"
#include "text/unicode.h"
#include "interface/c_player_profile.h"
#include "interface/gui_screens/start_menu/panes/settings_controls/start_menu_settings_controls.h"
#include "interface/gui_screens/start_menu/panes/settings_display/start_menu_settings_display.h"
#include "interface/gui_screens/start_menu/panes/settings_appearance/start_menu_settings_appearance.h"
#include "saved_games/content_catalogue.h"
#include "interface/game_tag_parser.h"
#include "interface/user_interface_error_manager.h"
#include "interface/voice_mask_types.h"
#include "interface/look_sensitivity_types.h"
#include "interface/ui_allocation_marker_types.h"
#include "multithreading/threads.h"
#include "multithreading/synchronized_value.h"
#include "interface/user_interface_allocation.h"
#include "interface/c_gui_list_widget.h"
#include "interface/gui_data.h"
#include "interface/c_gui_screen_widget.h"
#include "interface/user_interface_data.h"
#include <xtl.h>
#include "interface/player_profile_global_settings.h"
#include "interface/gui_screens/start_menu/start_menu_pane_screen_widget.h"
#include "interface/gui_screens/start_menu/start_menu_string_ids.h"
#include "interface/user_interface_utilities_string_ids.h"
#include "interface/gui_widget.h"
#include "interface/c_gui_widget.h"
#include <new>
#include "main/game_preferences_types.h"
#include "main/thread_globals_lock_inlines.h"


// String ids of the settings panes' list items (cache string table, set 0 and set 1).
// (Values already in a shared string-id enum come from that header instead; the rest
// stay here because those shared headers could not be leased by this lane.)
enum
{
    _string_id_look_sensitivity = 0x1b8,
    _string_id_look_inversion = 0x10114,
    _string_id_look_centering = 0x10117,
    _string_id_crouch_lock = 0x1011a,
    _string_id_joystick_layout = 0x1011d,
    _string_id_button_layout = 0x10122,
    _string_id_voice_proximity = 0x101ec,
    _string_id_voice_matchmaking = 0x101ed,
    _string_id_voice_mute = 0x101ee,
    _string_id_voice_guide = 0x101ef,
    _string_id_film_autosave_campaign = 0x101fc,
    _string_id_film_autosave_multiplayer = 0x101fd,
    _string_id_film_autosave_editor = 0x101fe,
    _string_id_male = 0x10139,
    _string_id_female = 0x1013a,
    _string_id_voice_guide_mismatch_muted = 0x101f3,
    _string_id_voice_guide_mismatch_proximity = 0x101f4,
};

typedef bool (*t_magic_string_callback)(void *context, wchar_t *output, long capacity);

typedef c_static_wchar_string<1024> s_start_menu_settings_text;

static bool parse_xml_joystick_layout(void *widget, wchar_t *buffer, long buffer_length);
static bool parse_xml_button_layout(void *widget, wchar_t *buffer, long buffer_length);
// @address 0x489948 ?get_any_responding_controller@c_gui_widget@@QBA?AW4e_controller_index@@XZ
// @address 0x1b7030 ?in_use@c_controller_interface@@QBA_NXZ
// @address 0x4e2de8 ?set_focused_datasource_element_from_value@c_gui_list_widget@@QAA_NJJ_N@Z
// @address 0x4b92e0 ?get_string_id_value@c_settings_controls_sidebar_items_datasource@@UAA_NJJPAJ@Z
// @address 0x4b99e8 ?parse_xml_controller_setting@@YA_NPAVc_gui_screen_widget@@PA_WJJJ@Z

static __forceinline c_gui_data *start_menu_settings_find_data(c_gui_screen_widget *screen, int name, int *index)
{
    for (int datasource_index = 0; datasource_index < screen->m_datasource_count; ++datasource_index)
    {
        if (screen->m_datasource[datasource_index]->m_name == name)
        {
            if (index)
                *index = datasource_index;
            return (c_gui_data *)screen->m_datasource[datasource_index];
        }
    }
    return 0;
}

bool c_settings_controls_sidebar_items_datasource::get_string_id_value(long element,
                                                                       long value_name, long *value)
{
    bool result = false;
    if (element != -1 && value_name == _string_id_value)
    {
        long name = -1;
        c_gui_data *base = this->get_base_datasource();
        if (base->get_string_id_value(element, _string_id_name, &name))
        {
            c_gui_data *data = start_menu_settings_find_data(this->screen, (int)name, 0);
            if (data)
            {
                long setting = -1;
                switch (name)
                {
                case _string_id_look_inversion:
                    setting = this->profile->get_controller_look_inverted();
                    break;
                case _string_id_look_sensitivity:
                    setting = this->profile->controls.unknown_setting_1c;
                    break;
                case _string_id_look_centering:
                    setting = ((this->profile->flags >> 2) & 1) != 0;
                    break;
                case _string_id_crouch_lock:
                    setting = ((this->profile->flags >> 3) & 1) != 0;
                    break;
                case _string_id_joystick_layout:
                    setting = this->profile->get_joystick_preset();
                    break;
                case _string_id_button_layout:
                    setting = this->profile->get_button_preset();
                    break;
                }
                for (int item = data->get_first_element_handle(); item != -1;
                     item = data->get_next_element_handle(item))
                {
                    long integer = -1;
                    if (data->get_integer_value(item, _string_id_value, &integer) && integer == setting)
                    {
                        long string_id = -1;
                        if (data->get_string_id_value(item, _string_id_name, &string_id))
                        {
                            *value = string_id;
                            result = true;
                        }
                        break;
                    }
                }
            }
        }
        result = true;
    }
    else
    {
        c_gui_data *base = this->get_base_datasource();
        result = base->get_string_id_value(element, value_name, (long *)value);
    }
    return result;
}

__forceinline bool start_menu_settings_get_inversion(const XUSER_DATA *d)
{
    bool result;
    if (d->type == 1)
        result = (unsigned long)d->nData != 0;
    else
        result = false;
    return result;
}
__forceinline bool start_menu_settings_get_auto_center(const XUSER_DATA *d)
{
    bool result;
    if (d->type == 1)
        result = (unsigned long)d->nData != 0;
    else
        result = false;
    return result;
}
__forceinline long start_menu_settings_get_sensitivity(const XUSER_DATA *d)
{
    long result;
    if (d->type == 1)
    {
        switch ((unsigned long)d->nData)
        {
        case 0:
            result = 1;
            break;
        case 1:
            result = 0;
            break;
        case 2:
            result = 3;
            break;
        default:
            result = 1;
            break;
        }
    }
    else
        result = 1;
    return result;
}
__forceinline long start_menu_settings_get_difficulty(const XUSER_DATA *d)
{
    long result;
    if (d->type == 1)
    {
        switch ((unsigned long)d->nData)
        {
        case 0:
            result = 2;
            break;
        case 1:
            result = 0;
            break;
        case 2:
            result = 7;
            break;
        default:
            result = 2;
            break;
        }
    }
    else
        result = 2;
    return result;
}

// @address 0x24684 g_xenon_profile_setting_ids
// @address 0x4a5398 player_profile_interface_apply_global_settings
extern "C" void player_profile_interface_apply_global_settings(
    c_player_profile_interface *p, const start_menu_settings_result *settings,
    const unsigned long *setting_ids, int count)
{
 if(settings->count==14) {
  for(long i=0;i<14;i++) {
   const XUSER_PROFILE_SETTING *setting=&settings->settings[i];
   if(setting->source==1) {
    const XUSER_DATA *d=&setting->data;
    switch(g_xenon_profile_setting_ids[i]) {
     case XPROFILE_GAMER_YAXIS_INVERSION: { bool value=start_menu_settings_get_inversion(d); p->set_controller_look_inverted(value,false); break; }
     case XPROFILE_OPTION_VOICE_THRU_SPEAKERS: p->set_voice_mask(voice_mask_none, false); break;
     case XPROFILE_GAMER_DIFFICULTY: { long value=start_menu_settings_get_sensitivity(d); p->set_look_sensitivity((e_look_sensitivity)value,false); break; }
     case XPROFILE_GAMER_CONTROL_SENSITIVITY: { long value=start_menu_settings_get_difficulty(d); p->set_unknown_setting_1c((int)value,false); break; }
     case XPROFILE_GAMER_ACTION_AUTO_CENTER: { bool value=start_menu_settings_get_auto_center(d); p->set_controller_flight_inverted(value,false); break; }
    }
   }
  }
 }
}

// 0x4b9518
// @address 0x4b9518 ?initialize@c_start_menu_settings_controls@@UAAXXZ
void c_start_menu_settings_controls::initialize()
{
    c_gui_screen_widget::initialize();
    add_game_tag_parser(new(0, 0, _ui_allocation_marker_dummy)
        c_magic_string_game_tag_parser(L"<joystick-layout-description", this, parse_xml_joystick_layout));
    add_game_tag_parser(new(0, 0, _ui_allocation_marker_dummy) c_magic_string_game_tag_parser(L"<button-layout-description", this, parse_xml_button_layout));
}

bool parse_xml_controller_setting(c_gui_screen_widget *screen, wchar_t *buffer, long buffer_length, long name, long value)
{
    bool result = false;
    long string_id;
    c_gui_data *data = 0;
    for (int datasource_index = 0; datasource_index < screen->m_datasource_count; ++datasource_index)
    {
        if (screen->m_datasource[datasource_index]->get_name() == name)
        {
            data = (c_gui_data *)screen->m_datasource[datasource_index];
            break;
        }
    }
    if (data)
    {
        for (int item = data->get_first_element_handle(); item != -1; item = data->get_next_element_handle(item))
        {
            long integer = -1;
            if (data->get_integer_value(item, _string_id_value, &integer) && integer == value)
            {
                if (data->get_string_id_value(item, _string_id_description, &string_id))
                {
                    s_start_menu_settings_text text;
                    if (screen->get_string_by_string_id(string_id, &text))
                    {
                        unsigned int count = buffer_length;
                        if (count > 1024)
                        {
                            count = 1024;
                        }
                        ustrnzcpy(buffer, text.m_string, count);
                        result = true;
                        break;
                    }
                }
            }
        }
    }
    return result;
}

// 0x4b9b70
// @address 0x4b9b70 ?parse_xml_button_layout@@YA_NPAXPA_WJ@Z
static bool parse_xml_button_layout(void *widget, wchar_t *buffer, long buffer_length)
{
    e_controller_index controller_index = ((c_gui_widget *)widget)->get_any_responding_controller();
    bool result = false;
    if (controller_index >= 0 && controller_index < 4 && controller_get(controller_index)->in_use())
    {
        c_player_profile_interface *profile = controller_get(controller_index)->get_player_profile_interface();
        result = parse_xml_controller_setting((c_gui_screen_widget *)widget, buffer, buffer_length, _string_id_button_layout, profile->get_button_preset());
    }
    return result;
}

// 0x4b9bf0
// @address 0x4b9bf0 ?parse_xml_joystick_layout@@YA_NPAXPA_WJ@Z
static bool parse_xml_joystick_layout(void *widget, wchar_t *buffer, long buffer_length)
{
    e_controller_index controller_index = ((c_gui_widget *)widget)->get_any_responding_controller();
    bool result = false;
    if (controller_index >= 0 && controller_index < 4 && controller_get(controller_index)->in_use())
    {
        c_player_profile_interface *profile = controller_get(controller_index)->get_player_profile_interface();
        result = parse_xml_controller_setting((c_gui_screen_widget *)widget, buffer, buffer_length, _string_id_joystick_layout, profile->get_joystick_preset());
    }
    return result;
}


// 0x4ba030
// @address 0x4ba030 ?initialize@c_start_menu_settings_appearance@@UAAXXZ
void c_start_menu_settings_appearance::initialize()
{
    c_gui_screen_widget::initialize();
    add_game_tag_parser(new(0, 0, _ui_allocation_marker_dummy) c_magic_string_game_tag_parser(L"<service-tag", this, parse_xml_profile_name));
    add_game_tag_parser(new (0, 0, _ui_allocation_marker_dummy) c_magic_string_game_tag_parser(
        L"<gender", this, (t_magic_string_callback)parse_xml_crouch_lock_description));
}

// 0x4ba8b0
// @address 0x4ba8b0 ?handle_dialog_result@c_start_menu_settings_appearance@@UAA_NPBVc_dialog_result_message@@@Z
bool c_start_menu_settings_appearance::handle_dialog_result(const c_dialog_result_message *message)
{
    bool handled = false;
    if (message->get_dialog_name() == 0x30007)
    {
        c_player_profile_interface *profile =
            controller_get((e_controller_index)message->get_controller())->get_player_profile_interface();
        if (message->get_dialog_result() == _gui_dialog_choice_first)
        {
            profile->set_controller_crouch_lock_enabled(false, true);
        }
        else if (message->get_dialog_result() == _gui_dialog_choice_second)
        {
            profile->set_controller_crouch_lock_enabled(true, true);
        }
        handled = true;
    }
    if (!handled)
    {
        handled = false;
        if (message->get_dialog_name() == _user_interface_utilities_string_need_to_select_storage_device_to_save)
        {
            if (message->get_dialog_result() == _gui_dialog_choice_first)
            {
                content_catalogue_display_device_selection_guide_interface((e_controller_index)message->get_controller());
            }
            handled = true;
        }
    }
    return handled;
}

// @address 0x4ba970 parse_xml_profile_name
extern "C" bool parse_xml_profile_name(void *context, wchar_t *output, long capacity)
{
    bool result = false;
    wchar_t text[256];
    text[0] = 0;
    e_controller_index controller = ((c_gui_widget *)context)->get_any_responding_controller();
    c_player_profile_interface *profile = controller_get(controller)->get_player_profile_interface();
    if (profile)
    {
        ((c_static_wchar_string<256> *)text)->set(profile->customization.desired_service_tag);
        unsigned int count = capacity;
        if (count > 256)
        {
            count = 256;
        }
        ustrnzcpy(output, text, count);
        result = true;
    }
    return result;
}

// 0x4ba9f0
// @address 0x4ba9f0 parse_xml_crouch_lock_description
extern "C" bool parse_xml_crouch_lock_description(void *context, wchar_t *output, long capacity)
{
    bool result = false;
    s_start_menu_settings_text text;
    e_controller_index controller = ((c_gui_widget *)context)->get_any_responding_controller();
    c_player_profile_interface *profile = controller_get(controller)->get_player_profile_interface();
    if (profile)
    {
        long string_id = profile->get_controller_crouch_lock_enabled() ? _string_id_female : _string_id_male;
        c_gui_screen_widget *screen = (c_gui_screen_widget *)context;
        result = screen->get_string_by_string_id(string_id, &text);
        if (result)
        {
            unsigned int count = capacity;
            if (count > 1024)
            {
                count = 1024;
            }
            ustrnzcpy(output, text.m_string, count);
        }
    }
    return result;
}

// @address 0x522a0 ??_7c_settings_controls_sidebar_items_datasource@@6B@
// @address 0x4c0ee0 ??_Gc_settings_controls_sidebar_items_datasource@@UAAPAXI@Z
// @address 0x4c0ee0 ??_Ec_settings_controls_sidebar_items_datasource@@UAAPAXI@Z

// @address 0x4b92c0 ?get_column_names@c_settings_controls_sidebar_items_datasource@@UAAXQAJPAJ@Z
void c_settings_controls_sidebar_items_datasource::get_column_names(long *const column_names, long *column_count)
{
    *column_count = 0;
    column_names[*column_count] = _string_id_value;
    ++*column_count;
}

// @address 0x4b9600 ?initialize_datasource@c_start_menu_settings_controls@@UAAXXZ
void c_start_menu_settings_controls::initialize_datasource()
{
    c_gui_screen_widget::initialize_datasource();
    e_controller_index controller = get_any_responding_controller();
    if ((unsigned int)controller <= 3 && controller_get(controller)->in_use())
    {
        long index = -1;
        c_gui_data *widget = (c_gui_data *)get_data(_start_menu_string_start_menu_game_items, &index);
        if (widget != 0)
        {
            void *memory = user_interface_malloc_tracked(sizeof(c_settings_controls_sidebar_items_datasource), 0, 0);
            if (memory != 0)
            {
                c_settings_controls_sidebar_items_datasource *datasource = new (memory)
                    c_settings_controls_sidebar_items_datasource(
                        widget, controller_get(controller)->get_player_profile_interface(), this);
                if (datasource->initialize())
                {
                    m_datasource[m_datasource_count] = datasource;
                    m_datasource_count++;
                    return;
                }
                datasource->~c_settings_controls_sidebar_items_datasource();
                user_interface_free(datasource);
            }
        }
    }
}

// @address 0x4b9918 ?submenu_invoked@c_start_menu_settings_controls@@UAAXPAVc_gui_list_widget@@@Z
void c_start_menu_settings_controls::submenu_invoked(c_gui_list_widget *list_widget)
{
    e_controller_index controller_index = get_any_responding_controller();
    if ((unsigned int)controller_index <= 3 && controller_get(controller_index)->in_use())
    {
        c_player_profile_interface *profile = controller_get(controller_index)->get_player_profile_interface();
        long name = list_widget->m_name;
        long value;
        if (name <= _string_id_look_centering)
        {
            if (name != _string_id_look_centering)
            {
                if (name != _string_id_look_sensitivity)
                {
                    if (name != _string_id_look_inversion)
                        return;
                    value = (unsigned char)(profile->flags & 1);
                }
                else
                {
                    value = profile->controls.unknown_setting_1c;
                }
            }
            else
            {
                value = (unsigned char)(((unsigned int)profile->flags >> 2) & 1);
            }
        }
        else
        {
            if (name != _string_id_crouch_lock)
                return;
            value = (unsigned char)(((unsigned int)profile->flags >> 3) & 1);
        }
        if (value != -1)
            list_widget->set_focused_datasource_element_from_value(_string_id_value, value, false);
    }
}

// 0x4bd850
// @address 0x4bd850 ?initialize@c_start_menu_settings_voice@@UAAXXZ
void c_start_menu_settings_voice::initialize()
{
    c_gui_screen_widget::initialize();
    add_game_tag_parser(new (0, 0, _ui_allocation_marker_dummy)
            c_magic_string_game_tag_parser(L"<voice_setting_description", this, parse_xml_appearance_description));
}

// @address 0x52c10 ??_7c_settings_voice_sidebar_items_datasource@@6B@
// @address 0x4c0ee0 ??_Gc_settings_voice_sidebar_items_datasource@@UAAPAXI@Z
// @address 0x4c0ee0 ??_Ec_settings_voice_sidebar_items_datasource@@UAAPAXI@Z

// @address 0x4b92c0 ?get_column_names@c_settings_voice_sidebar_items_datasource@@UAAXQAJPAJ@Z
void c_settings_voice_sidebar_items_datasource::get_column_names(long *const column_names, long *column_count)
{
    *column_count = 0;
    column_names[*column_count] = _string_id_value;
    ++*column_count;
}

// @address 0x4bd600 ?get_string_id_value@c_settings_voice_sidebar_items_datasource@@UAA_NJJPAJ@Z
bool c_settings_voice_sidebar_items_datasource::get_string_id_value(long element, long value_name, long *value)
{
    bool result = false;
    if (element != -1 && (value_name == _string_id_value || value_name == _string_id_description))
    {
        long name = -1;
        if (m_base_datasource->get_string_id_value(element, _string_id_name, &name))
        {
            c_gui_data *data = start_menu_settings_find_data(screen, (int)name, 0);
            if (data)
            {
                long setting = -1;
                switch (name)
                {
                case _string_id_voice_mute:
                    setting = profile->voice.voice_output_setting;
                    break;
                case _string_id_voice_proximity:
                    setting = profile->voice.voice_mask;
                    break;
                case _string_id_voice_matchmaking:
                    setting = profile->voice.unknown_setting_104;
                    break;
                case _string_id_voice_guide:
                    setting = profile->voice.xbox_voice_muted ? 0 : profile->voice.xbox_voice_output_setting + 1;
                    break;
                }
                for (int item = data->get_first_element_handle(); item != -1; item = data->get_next_element_handle(item))
                {
                    long integer = -1;
                    if (data->get_integer_value(item, _string_id_value, &integer) && integer == setting)
                    {
                        long string_id = -1;
                        if (data->get_string_id_value(item, value_name == _string_id_value ? _string_id_name : _string_id_description, &string_id))
                        {
                            *value = string_id;
                            result = true;
                        }
                        break;
                    }
                }
            }
        }
    }
    else
    {
        result = m_base_datasource->get_string_id_value(element, value_name, value);
    }
    return result;
}

// 0x4bdce8
// @address 0x4bdce8 ?submenu_invoked@c_start_menu_settings_voice@@UAAXPAVc_gui_list_widget@@@Z
void c_start_menu_settings_voice::submenu_invoked(c_gui_list_widget *list_widget)
{
    e_controller_index controller_index = get_any_responding_controller();
    if ((unsigned int)controller_index > 3)
    {
        return;
    }
    if (!controller_get(controller_index)->in_use())
    {
        return;
    }
    c_player_profile_interface *profile = controller_get(controller_index)
        ->get_player_profile_interface();
    long value;
    switch (list_widget->m_name)
    {
    case _string_id_voice_mute:
        value = profile->voice.voice_output_setting;
        break;
    case _string_id_voice_matchmaking:
        value = profile->voice.unknown_setting_104;
        break;
    case _string_id_voice_proximity:
        value = profile->voice.voice_mask;
        break;
    default:
        return;
    }
    if (value != -1)
    {
        list_widget->set_focused_datasource_element_from_value(_string_id_value, value, false);
    }
}

// 0x4bd9f0
// @address 0x4bd9f0 ?update@c_start_menu_settings_voice@@UAAXK@Z
void c_start_menu_settings_voice::update(unsigned long current_milliseconds)
{
    c_start_menu_pane_screen_widget::update(current_milliseconds);
    c_player_profile_interface *profile = (c_player_profile_interface *)controller_get(
        get_any_responding_controller())
                                                        ->get_player_profile_interface();
    bool muted_visible = profile->voice.xbox_voice_muted && profile->voice.voice_output_setting == 0;
    long output_setting = profile->voice.xbox_voice_output_setting;
    bool mask_visible = output_setting == 2 || (output_setting == 1 && profile->voice.voice_mask == 1);
    c_gui_widget *muted_text = get_child_widget(_gui_text_widget, _string_id_voice_guide_mismatch_muted);
    if (muted_text != 0)
        muted_text->set_visible(muted_visible);
    // match: 0x4bd9f0: the reference binding passes the byte to set_visible without a clrlwi
    const bool &mask_shown = !muted_visible && mask_visible;
    c_gui_widget *mask_text = get_child_widget(_gui_text_widget, _string_id_voice_guide_mismatch_proximity);
    if (mask_text != 0)
        mask_text->set_visible(mask_shown);
}

// 0x4bed18
// @address 0x4bed18 ?update_pending_value@c_start_menu_settings_display@@AAAXXZ
void c_start_menu_settings_display::update_pending_value()
{
    if (m_pending_value != -1)
    {
        get_any_responding_controller();
        long value = global_preferences_get_brightness_setting();
        long old_value = m_pending_value;
        if (value != old_value && old_value >= 0)
        {
            player_profile_set_global_value(old_value);
        }
        m_pending_value = -1;
    }
}

// 0x4bed78
// @address 0x4bed78 ?update@c_start_menu_settings_display@@UAAXK@Z
void c_start_menu_settings_display::update(unsigned long current_milliseconds)
{
    c_start_menu_pane_screen_widget::update(current_milliseconds);
    if (!m_suppress_focus)
    {
        c_gui_widget *focused = m_focused_widget;
        if (focused != 0 && focused->m_type == _gui_list_item_widget)
        {
            c_gui_list_widget *list = (c_gui_list_widget *)gui_widget_get_parent_list_widget(focused);
            if (list != 0)
            {
                c_gui_data *data = list->get_data();
                if (list->m_name == _start_menu_string_display_brightness)
                {
                    long value;
                    if (data->get_integer_value(list->get_focused_element_handle(), _string_id_value, &value))
                    {
                        get_any_responding_controller();
                        int current = global_preferences_get_brightness_setting();
                        if (current != value && value >= 0)
                        {
                            player_profile_set_global_value(value);
                        }
                    }
                    return;
                }
            }
        }
    }
    update_pending_value();
}

// 0x4bf048
// @address 0x4bf048 ?submenu_invoked@c_start_menu_settings_display@@UAAXPAVc_gui_list_widget@@@Z
void c_start_menu_settings_display::submenu_invoked(c_gui_list_widget *list_widget)
{
    e_controller_index controller = get_any_responding_controller();
    if ((unsigned int)controller > 3)
    {
        return;
    }
    if (!controller_get(controller)->in_use())
    {
        return;
    }
    c_player_profile_interface *profile = controller_get(controller)->get_player_profile_interface();
    long value;
    switch (list_widget->m_name)
    {
    case _start_menu_string_display_subtitles:
        value = ((int (*)(c_player_profile_interface *))global_preferences_get_subtitle_setting)(profile);
        break;
    case _start_menu_string_display_brightness:
        value = global_preferences_get_brightness_setting();
        m_pending_value = global_preferences_get_brightness_setting();
        break;
    default:
        return;
    }
    if (value != -1)
    {
        list_widget->set_focused_datasource_element_from_value(_string_id_value, value, false);
    }
}

// @address 0x53090 ??_7c_settings_film_autosave_sidebar_items_datasource@@6B@
// @address 0x4c0ee0 ??_Gc_settings_film_autosave_sidebar_items_datasource@@UAAPAXI@Z
// @address 0x4c0ee0 ??_Ec_settings_film_autosave_sidebar_items_datasource@@UAAPAXI@Z

// @address 0x4b92c0 ?get_column_names@c_settings_film_autosave_sidebar_items_datasource@@UAAXQAJPAJ@Z
void c_settings_film_autosave_sidebar_items_datasource::get_column_names(long *const column_names, long *column_count)
{
    *column_count = 0;
    column_names[*column_count] = _string_id_value;
    ++*column_count;
}

// @address 0x4bf2d8 ?get_string_id_value@c_settings_film_autosave_sidebar_items_datasource@@UAA_NJJPAJ@Z
bool c_settings_film_autosave_sidebar_items_datasource::get_string_id_value(long index, long id, long *out)
{
    if (index != -1 && (id == _string_id_value || id == _string_id_description))
    {
        long found = -1;
        if (m_base_datasource->get_string_id_value(index, _string_id_name, &found))
        {
            c_gui_screen_widget *owner_screen = screen;
            int count = owner_screen->m_datasource_count;
            for (int i = 0; i < count; i++)
            {
                if (owner_screen->m_datasource[i]->get_name() == found)
                {
                    c_gui_data *item = (c_gui_data *)owner_screen->m_datasource[i];
                    if (item)
                    {
                        long expect = -1;
                        switch (found)
                        {
                        case _string_id_film_autosave_campaign:
                            expect = profile->unknown_bool_114;
                            break;
                        case _string_id_film_autosave_multiplayer:
                            expect = profile->unknown_bool_115;
                            break;
                        case _string_id_film_autosave_editor:
                            expect = profile->unknown_bool_116;
                            break;
                        }
                        int k = item->get_first_element_handle();
                        while (k != -1)
                        {
                            long value = -1;
                            if (item->get_integer_value(k, _string_id_value, &value) && value == expect)
                            {
                                long result = -1;
                                if (item->get_string_id_value(k, id == _string_id_value ? _string_id_name : _string_id_description, &result))
                                {
                                    *out = result;
                                }
                                return true;
                            }
                            k = item->get_next_element_handle(k);
                        }
                    }
                    return true;
                }
            }
        }
        return true;
    }
    return m_base_datasource->get_string_id_value(index, id, out);
}

// 0x4bf4d8
// @address 0x4bf4d8 ?initialize@c_start_menu_settings_film_autosave@@UAAXXZ
void c_start_menu_settings_film_autosave::initialize()
{
    c_gui_screen_widget::initialize();
    add_game_tag_parser(new (0, 0, _ui_allocation_marker_dummy)
            c_magic_string_game_tag_parser(L"<film_autosave_setting_description", this, parse_xml_flags_description));
}

// 0x4bf560
// @address 0x4bf560 ?initialize_datasource@c_start_menu_settings_film_autosave@@UAAXXZ
void c_start_menu_settings_film_autosave::initialize_datasource()
{
    c_gui_screen_widget::initialize_datasource();
    e_controller_index controller = get_any_responding_controller();
    if ((unsigned int)controller <= 3 && controller_get(controller)->in_use())
    {
        long index = -1;
        c_gui_data *widget = get_data(_start_menu_string_start_menu_game_items, &index);
        if (widget != 0)
        {
            void *memory = user_interface_malloc_tracked(sizeof(c_settings_film_autosave_sidebar_items_datasource), 0, 0);
            if (memory != 0)
            {
                c_settings_film_autosave_sidebar_items_datasource *datasource =
                    new (memory) c_settings_film_autosave_sidebar_items_datasource(
                        widget, controller_get(controller)->get_player_profile_interface(), this);
                if (datasource->initialize())
                {
                    m_datasource[m_datasource_count] = datasource;
                    m_datasource_count++;
                    return;
                }
                datasource->~c_settings_film_autosave_sidebar_items_datasource();
                user_interface_free(datasource);
            }
        }
    }
}

// 0x4bd8d8
// @address 0x4bd8d8 ?initialize_datasource@c_start_menu_settings_voice@@UAAXXZ
void c_start_menu_settings_voice::initialize_datasource()
{
    c_gui_screen_widget::initialize_datasource();
    e_controller_index controller = get_any_responding_controller();
    if ((unsigned int)controller <= 3 && controller_get(controller)->in_use())
    {
        long index = -1;
        c_gui_data *widget = get_data(_start_menu_string_start_menu_game_items, &index);
        if (widget != 0)
        {
            void *memory = user_interface_malloc_tracked(sizeof(c_settings_voice_sidebar_items_datasource), 0, 0);
            if (memory != 0)
            {
                c_settings_voice_sidebar_items_datasource *datasource = new (memory) c_settings_voice_sidebar_items_datasource(
                    widget, controller_get(controller)->get_player_profile_interface(), this);
                if (datasource->initialize())
                {
                    m_datasource[m_datasource_count] = datasource;
                    m_datasource_count++;
                    return;
                }
                datasource->~c_settings_voice_sidebar_items_datasource();
                user_interface_free(datasource);
            }
        }
    }
}

// 0x4bf888
// @address 0x4bf888 ?submenu_invoked@c_start_menu_settings_film_autosave@@UAAXPAVc_gui_list_widget@@@Z
void c_start_menu_settings_film_autosave::submenu_invoked(c_gui_list_widget *list_widget)
{
    e_controller_index controller = get_any_responding_controller();
    if ((unsigned int)controller > 3)
    {
        return;
    }
    if (!controller_get(controller)->in_use())
    {
        return;
    }
    c_player_profile_interface *values =
        (c_player_profile_interface *)controller_get(controller)->get_player_profile_interface();
    long value;
    switch (list_widget->m_name)
    {
    case _string_id_film_autosave_editor:
        value = values->unknown_bool_116;
        break;
    case _string_id_film_autosave_multiplayer:
        value = values->unknown_bool_115;
        break;
    case _string_id_film_autosave_campaign:
        value = values->unknown_bool_114;
        break;
    default:
        return;
    }
    if (value != -1)
    {
        list_widget->set_focused_datasource_element_from_value(_string_id_value, value, false);
    }
}

// @address 0x4bdae8 ?handle_controller_input_message@c_start_menu_settings_voice@@UAA_NPBVc_controller_input_message@@@Z
bool c_start_menu_settings_voice::handle_controller_input_message(const c_controller_input_message *message)
{
    bool handled = false;
    c_gui_widget *focused;
    c_gui_list_widget *list;
    c_gui_data *data;
    c_gui_widget *candidate = !m_suppress_focus ? m_focused_widget : 0;
    if (candidate && candidate->m_type == _gui_list_item_widget)
    {
        focused = candidate;
        list = (c_gui_list_widget *)gui_widget_get_parent_list_widget((c_gui_widget *)candidate);
        data = list ? ((c_gui_data *)(list)->get_data()) : 0;
    }
    else
    {
        focused = 0;
        list = 0;
        data = 0;
    }
    e_controller_index controller = message->get_controller();
    long type = message->get_event_type();
    long button = message->get_component();
    if ((unsigned long)controller <= 3 && controller_get(controller)->in_use())
    {
        c_player_profile_interface *profile = controller_get(controller)->get_player_profile_interface();
        if (list && data && type == _event_type_button_press && button == 0)
        {
            long value = -1;
            if (data->get_integer_value((list)->get_focused_element_handle(), _string_id_value, &value))
            {
                switch (list->m_name)
                {
                case _string_id_voice_mute: if (value >= 0) profile->set_voice_output_setting((e_voice_output_setting)value, true); break;
                case _string_id_voice_proximity: if (value >= 0) profile->set_voice_mask((e_voice_mask)value, true); break;
                case _string_id_voice_matchmaking: if (value >= 0) profile->set_unknown_setting_104(value, true); break;
                case _string_id_voice_guide: g_gui_error_manager.post_error_with_custom_message(0x20060, 0, 0, message->get_controller(), false, true); break;
                }
                focused->handle_widget_back_out();
                handled = true;
            }
        }
    }
    if (!handled)
        handled = c_start_menu_pane_screen_widget::handle_controller_input_message(message);
    return handled;
}
static __forceinline bool start_menu_settings_controls_selected(c_gui_list_widget *list)
{
    return list && list->within_focus_chain();
}

// @address 0x4bdd80 parse_xml_appearance_description
extern "C" bool parse_xml_appearance_description(void *context, wchar_t *output, long capacity)
{
    bool success = false;
    do
    {
        c_gui_screen_widget *screen = (c_gui_screen_widget *)context;
        c_gui_widget *widget = (c_gui_widget *)context;
        c_gui_screen_widget *state = (c_gui_screen_widget *)context;
        c_gui_list_widget *focused;
        if (!state->m_suppress_focus && state->m_focused_widget)
            focused = (c_gui_list_widget *)gui_widget_get_parent_list_widget(state->m_focused_widget);
        else
            focused = 0;
        c_gui_list_widget *sidebar = (c_gui_list_widget *)widget->get_child_widget(_gui_list_widget, _start_menu_string_start_menu_items);
        c_gui_data *data = sidebar ? ((c_gui_data *)(sidebar)->get_data()) : 0;
        long value;
        if (focused == sidebar && data)
        {
            if (!data->get_string_id_value((sidebar)->get_focused_element_handle(), _string_id_description, &value))
                break;
        }
        else
        {
            c_gui_list_widget *model = (c_gui_list_widget *)widget->get_child_widget(_gui_list_widget, _string_id_voice_mute);
            c_gui_list_widget *primary = (c_gui_list_widget *)widget->get_child_widget(_gui_list_widget, _string_id_voice_proximity);
            c_gui_list_widget *secondary = (c_gui_list_widget *)widget->get_child_widget(_gui_list_widget, _string_id_voice_matchmaking);
            c_gui_data *selected_data;
            long element;
            if (model && model->within_focus_chain())
            {
                selected_data = ((c_gui_data *)(model)->get_data());
                element = (model)->get_focused_element_handle();
            }
            else if (primary && primary->within_focus_chain())
            {
                selected_data = ((c_gui_data *)(primary)->get_data());
                element = (primary)->get_focused_element_handle();
            }
            else if (secondary && secondary->within_focus_chain())
            {
                selected_data = ((c_gui_data *)(secondary)->get_data());
                element = (secondary)->get_focused_element_handle();
            }
            else
                break;
            if (!selected_data || element == -1 || !selected_data->get_string_id_value(element, _string_id_description, &value))
                break;
        }
        if (value != -1)
        {
            s_start_menu_settings_text text;
            if (screen->get_string_by_string_id(value, &text))
            {
                unsigned int count = capacity;
                if (count > 1024)
                    count = 1024;
                ustrnzcpy(output, text.m_string, count);
                success = true;
            }
        }
    } while (false);
    return success;
}
// @address 0x4bf920 parse_xml_flags_description
extern "C" bool parse_xml_flags_description(void *context, wchar_t *output, long capacity)
{
    bool success = false;
    do
    {
        c_gui_screen_widget *screen = (c_gui_screen_widget *)context;
        c_gui_widget *widget = (c_gui_widget *)context;
        c_gui_screen_widget *state = (c_gui_screen_widget *)context;
        c_gui_list_widget *focused;
        if (!state->m_suppress_focus && state->m_focused_widget)
            focused = (c_gui_list_widget *)gui_widget_get_parent_list_widget(state->m_focused_widget);
        else
            focused = 0;
        c_gui_list_widget *sidebar = (c_gui_list_widget *)widget->get_child_widget(_gui_list_widget, _start_menu_string_start_menu_items);
        c_gui_data *data = sidebar ? ((c_gui_data *)(sidebar)->get_data()) : 0;
        long value;
        if (focused == sidebar && data)
        {
            if (!data->get_string_id_value((sidebar)->get_focused_element_handle(), _string_id_description, &value))
                break;
        }
        else
        {
            c_gui_list_widget *model = (c_gui_list_widget *)widget->get_child_widget(_gui_list_widget, _string_id_film_autosave_campaign);
            c_gui_list_widget *primary = (c_gui_list_widget *)widget->get_child_widget(_gui_list_widget, _string_id_film_autosave_multiplayer);
            c_gui_list_widget *secondary = (c_gui_list_widget *)widget->get_child_widget(_gui_list_widget, _string_id_film_autosave_editor);
            c_gui_data *selected_data;
            long element;
            if (model && model->within_focus_chain())
            {
                selected_data = ((c_gui_data *)(model)->get_data());
                element = (model)->get_focused_element_handle();
            }
            else if (primary && primary->within_focus_chain())
            {
                selected_data = ((c_gui_data *)(primary)->get_data());
                element = (primary)->get_focused_element_handle();
            }
            else if (secondary && secondary->within_focus_chain())
            {
                selected_data = ((c_gui_data *)(secondary)->get_data());
                element = (secondary)->get_focused_element_handle();
            }
            else
                break;
            if (!selected_data || element == -1 || !selected_data->get_string_id_value(element, _string_id_description, &value))
                break;
        }
        if (value != -1)
        {
            s_start_menu_settings_text text;
            if (screen->get_string_by_string_id(value, &text))
            {
                unsigned int count = capacity;
                if (count > 1024)
                    count = 1024;
                ustrnzcpy(output, text.m_string, count);
                success = true;
            }
        }
    } while (false);
    return success;
}

// 0x4b9718
// @address 0x4b9718 ?handle_controller_input_message@c_start_menu_settings_controls@@UAA_NPBVc_controller_input_message@@@Z
bool c_start_menu_settings_controls::handle_controller_input_message(const c_controller_input_message *message)
{
    c_gui_widget *widget = this;
    bool handled = false;
    c_gui_widget *focused;
    c_gui_list_widget *list;
    c_gui_data *data;
    c_gui_screen_widget *state = (c_gui_screen_widget *)widget;
    c_gui_widget *candidate = !state->m_suppress_focus ? (c_gui_widget *)state->m_focused_widget : 0;
    focused = candidate && candidate->m_type == _gui_list_item_widget ? candidate : 0;
    if (focused)
    {
        list = (c_gui_list_widget *)gui_widget_get_parent_list_widget((c_gui_widget *)candidate);
        data = list ? ((c_gui_data *)(list)->get_data()) : 0;
    }
    else
    {
        list = 0;
        data = 0;
    }
    e_controller_index controller = message->get_controller();
    long type = message->get_event_type();
    long button = message->get_component();
    if ((unsigned long)controller <= 3 && controller_get(controller)->in_use())
    {
        c_player_profile_interface *profile = controller_get(controller)->get_player_profile_interface();
        if (list && data && type == _event_type_button_press && button == 0)
        {
            long value = -1;
            if (data->get_integer_value((list)->get_focused_element_handle(), _string_id_value, &value))
            {
                switch (list->m_name)
                {
                case _string_id_look_inversion: profile->set_controller_look_inverted(value != 0, true); break;
                case _string_id_look_sensitivity: if (value >= 0) profile->set_unknown_setting_1c(value, true); break;
                case _string_id_look_centering: profile->set_controller_flight_inverted(value != 0, true); break;
                case _string_id_crouch_lock: profile->set_controller_auto_center_look(value != 0, true); break;
                }
                focused->handle_widget_back_out();
                handled = true;
            }
        }
    }
    if (!handled)
        handled = ((c_start_menu_pane_screen_widget *)widget)->c_start_menu_pane_screen_widget::handle_controller_input_message(message);
    return handled;
}

static __forceinline void start_menu_settings_controls_apply_lock(long value)
{
    if (value < 3)
    {
        bool skip = g_thread_system_initialized.peek() == 0 || get_current_thread_index() == 3;
        if (!skip)
        {
            c_thread_globals_lock lock;
            s_global_preferences *preferences = thread_global_preferences;
            preferences->primary_file.data.subtitle_setting = value;
            preferences->dirty = 1;
        }
    }
}

// 0x4bee58
// @address 0x4bee58 ?handle_controller_input_message@c_start_menu_settings_display@@UAA_NPBVc_controller_input_message@@@Z
bool c_start_menu_settings_display::handle_controller_input_message(const c_controller_input_message *message)
{
    bool handled = false;
    c_gui_widget *focused;
    c_gui_list_widget *list;
    c_gui_data *data;
    c_gui_widget *candidate = get_focused_widget();
    focused = candidate && candidate->m_type == _gui_list_item_widget ? candidate : 0;
    if (focused)
    {
        list = (c_gui_list_widget *)gui_widget_get_parent_list_widget(candidate);
        data = list ? list->get_data() : 0;
    }
    else
    {
        list = 0;
        data = 0;
    }
    e_controller_index controller = message->get_controller();
    long type = message->get_event_type();
    long button = message->get_component();
    if ((unsigned long)controller <= 3 && controller_get(controller)->in_use())
    {
        if (list && data && type == _event_type_button_press && button == 0)
        {
            long value = -1;
            if (data->get_integer_value(list->get_focused_element_handle(), _string_id_value, &value))
            {
                switch (list->m_name)
                {
                case _start_menu_string_display_subtitles:
                {
                    long lock_value = value;
                    if (value >= 0)
                        start_menu_settings_controls_apply_lock(lock_value);
                    break;
                }
                case _start_menu_string_display_brightness:
                    if (value >= 0)
                        player_profile_set_global_value(value);
                    m_pending_value = -1;
                    break;
                }
                handled = true;
                focused->handle_widget_back_out();
            }
        }
    }
    if (!handled)
        handled = c_start_menu_pane_screen_widget::handle_controller_input_message(message);
    return handled;
}
// 0x4bf678
// @address 0x4bf678 ?handle_controller_input_message@c_start_menu_settings_film_autosave@@UAA_NPBVc_controller_input_message@@@Z
bool c_start_menu_settings_film_autosave::handle_controller_input_message(const c_controller_input_message *message)
{
    bool handled = false;
    c_gui_widget *focused;
    c_gui_list_widget *list;
    c_gui_data *data;
    c_gui_widget *candidate = !m_suppress_focus ? m_focused_widget : 0;
    focused = candidate && candidate->m_type == _gui_list_item_widget ? candidate : 0;
    if (focused)
    {
        list = (c_gui_list_widget *)gui_widget_get_parent_list_widget((c_gui_widget *)candidate);
        data = list ? ((c_gui_data *)(list)->get_data()) : 0;
    }
    else
    {
        list = 0;
        data = 0;
    }
    e_controller_index controller = message->get_controller();
    long type = message->get_event_type();
    long button = message->get_component();
    if ((unsigned long)controller <= 3 && controller_get(controller)->in_use())
    {
        c_player_profile_interface *profile = controller_get(controller)->get_player_profile_interface();
        if (list && data && type == _event_type_button_press && button == 0)
        {
            long value = -1;
            if (data->get_integer_value((list)->get_focused_element_handle(), _string_id_value, &value))
            {
                switch (list->m_name)
                {
                case _string_id_film_autosave_campaign: profile->set_unknown_bool_114(value != 0, true); break;
                case _string_id_film_autosave_multiplayer: profile->set_unknown_bool_115(value != 0, true); break;
                case _string_id_film_autosave_editor: profile->set_unknown_bool_116(value != 0, true); break;
                }
                focused->handle_widget_back_out();
                handled = true;
            }
        }
    }
    if (!handled)
        handled = c_start_menu_pane_screen_widget::handle_controller_input_message(message);
    return handled;
}

// Typed replacements for the former controls/profile partial views.
typedef char settings_controls_desired_service_tag_offset[(offsetof(c_player_profile_interface, customization.desired_service_tag) == 0xf0) ? 1 : -1];
typedef char settings_controls_voice_output_setting_offset[(offsetof(c_player_profile_interface, voice.voice_output_setting) == 0xfc) ? 1 : -1];
typedef char settings_controls_voice_mask_offset[(offsetof(c_player_profile_interface, voice.voice_mask) == 0x100) ? 1 : -1];
typedef char settings_controls_unknown_setting_104_offset[(offsetof(c_player_profile_interface, voice.unknown_setting_104) == 0x104) ? 1 : -1];
typedef char settings_controls_xbox_voice_output_setting_offset[(offsetof(c_player_profile_interface, voice.xbox_voice_output_setting) == 0x10c) ? 1 : -1];
typedef char settings_controls_xbox_voice_muted_offset[(offsetof(c_player_profile_interface, voice.xbox_voice_muted) == 0x110) ? 1 : -1];
typedef char settings_controls_unknown_bool_114_offset[(offsetof(c_player_profile_interface, unknown_bool_114) == 0x114) ? 1 : -1];
typedef char settings_controls_unknown_bool_115_offset[(offsetof(c_player_profile_interface, unknown_bool_115) == 0x115) ? 1 : -1];
typedef char settings_controls_unknown_bool_116_offset[(offsetof(c_player_profile_interface, unknown_bool_116) == 0x116) ? 1 : -1];

typedef char settings_controls_xuser_data_size[(sizeof(void *) != 4 || ((sizeof(XUSER_DATA) == 16))) ? 1 : -1];
typedef char settings_controls_xuser_setting_data_offset[(offsetof(XUSER_PROFILE_SETTING, data) == 24) ? 1 : -1];
