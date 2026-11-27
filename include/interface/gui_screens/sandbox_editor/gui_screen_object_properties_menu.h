#ifndef INTERFACE_GUI_SCREENS_SANDBOX_EDITOR_GUI_SCREEN_OBJECT_PROPERTIES_MENU_H
#define INTERFACE_GUI_SCREENS_SANDBOX_EDITOR_GUI_SCREEN_OBJECT_PROPERTIES_MENU_H

#include "interface/user_interface_data.h"
#include "interface/c_gui_screen_widget.h"
#include "memory/static_arrays.h"

struct s_variant_object_datum;
struct s_tag_block;
class c_gui_list_item_widget;
class c_controller_input_message;

// String ids the object properties screen names its lists, datasources, columns and properties by.
enum e_object_properties_string_id
{
    _object_properties_string_value = 0x10a,
    _object_properties_string_name = 0x29c,
    _object_properties_string_object_properties_list = 0x10539,
    _object_properties_string_object_properties_datasource = 0x1053b,
    _object_properties_string_common_spinner = 0x10544,
    _object_properties_string_respawn_rate = 0x1054e,
    _object_properties_string_spare_clips = 0x1054f,
    _object_properties_string_unique_object = 0x10550,
    _object_properties_string_minimum_count = 0x10551,
    _object_properties_string_maximum_count = 0x10552,
    _object_properties_string_on_map_at_start = 0x10553,
    _object_properties_string_symmetry = 0x10554,
    _object_properties_string_teleporter_channel = 0x10555,
    _object_properties_string_team_affiliation = 0x10556,
    _object_properties_string_shape_type = 0x10557,
    _object_properties_string_shape_radius = 0x10558,
    _object_properties_string_shape_top = 0x10559,
    _object_properties_string_shape_bottom = 0x1055a,
    _object_properties_string_shape_width = 0x1055b,
    _object_properties_string_shape_depth = 0x1055c,
    _object_properties_string_spawn_order = 0x1055d,
};

class c_sandbox_object_properties_screen_widget : public c_gui_screen_widget
{
public:
    __forceinline c_sandbox_object_properties_screen_widget(long name) :
        c_gui_screen_widget(name), source(0), map_variant_index(-1) {}
    virtual void post_initialize();
    virtual void initialize_datasource();
    void show_only_necessary_fields(const s_variant_object_datum *variant_object);
    virtual bool handle_controller_input_message(const c_controller_input_message *message);
    virtual bool handle_list_item_chosen(const c_controller_input_message *message, long list_name, c_gui_list_item_widget *item, c_gui_data *data);
    virtual void update(unsigned long current_milliseconds);
    c_gui_data *source;
    long map_variant_index;
};

typedef char sandbox_object_properties_screen_size[
    (sizeof(void *) != 4 || ((sizeof(c_sandbox_object_properties_screen_widget) == 0x1a90))) ? 1 : -1];

class c_sandbox_object_properties_items_datasource : public c_gui_data_proxy
{
public:
    __forceinline c_sandbox_object_properties_items_datasource(c_gui_data *base_datasource,
        c_sandbox_object_properties_screen_widget *screen)
    {
        m_base_datasource = base_datasource;
        owner = screen;
    }
    virtual void get_column_names(long *const column_names, long *column_count);
    virtual bool get_text_value(long element_handle, long value_name, c_static_wchar_string<1024> *value);
    virtual bool get_string_id_value(long element_handle, long value_name, long *value);
    c_sandbox_object_properties_screen_widget *owner;
};

extern "C" long object_properties_find_category(s_tag_block *block, long definition_index);
#endif // INTERFACE_GUI_SCREENS_SANDBOX_EDITOR_GUI_SCREEN_OBJECT_PROPERTIES_MENU_H
