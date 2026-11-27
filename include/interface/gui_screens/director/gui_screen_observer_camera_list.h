#ifndef INTERFACE_GUI_SCREENS_DIRECTOR_GUI_SCREEN_OBSERVER_CAMERA_LIST_H
#define INTERFACE_GUI_SCREENS_DIRECTOR_GUI_SCREEN_OBSERVER_CAMERA_LIST_H

#include "interface/gui_data.h"
#include "memory/static_arrays.h"
#include "game/player_types.h"

#include <stddef.h>
#include "interface/c_gui_screen_widget.h"

class c_camera_list_datasource;

class c_observer_camera_list_screen : public c_gui_screen_widget
{
public:
    __forceinline c_observer_camera_list_screen(long name) :
        c_gui_screen_widget(name), m_camera_list_datasource(0), m_user_index(NONE)
    {
        m_responds_to_controller_events = false;
        m_render_in_screenshot = true;
    }

    virtual void update(unsigned long current_milliseconds);
    virtual void update_render_state(unsigned long current_milliseconds);
    virtual void initialize_datasource();

    c_camera_list_datasource *m_camera_list_datasource;
    long m_user_index;
};

typedef char observer_camera_list_datasource_offset[
    (sizeof(void *) != 4 || offsetof(c_observer_camera_list_screen, m_camera_list_datasource) == 0x1a88) ? 1 : -1];
typedef char observer_camera_list_user_offset[
    (sizeof(void *) != 4 || offsetof(c_observer_camera_list_screen, m_user_index) == 0x1a8c) ? 1 : -1];
typedef char observer_camera_list_screen_size[
    (sizeof(void *) != 4 || sizeof(c_observer_camera_list_screen) == 0x1a90) ? 1 : -1];

enum e_camera_type {
    _camera_type_player = 2
};

enum e_camera_column {
    _camera_column_player_color = 0x10127,
    _camera_column_team = 0x10013
};

struct s_camera_entry {
    __forceinline s_camera_entry() : datum_index(-1), type(_camera_type_player) {}
    long datum_index;
    long type;
};

class c_camera_list_datasource : public c_gui_ordered_data {
public:
    __forceinline c_camera_list_datasource() {}
    virtual void get_column_names(long *const column_names, long *column_count);
    virtual bool get_element(long element_handle, void *element, long element_size);
    virtual bool get_integer_value(long element_handle, long column_name, long *value);
    virtual bool get_text_value(long element_handle, long column_name, c_static_wchar_string<1024> *buffer);
    virtual bool get_player_appearance(long element_handle, s_player_appearance *appearance);
    virtual bool initialize(long name, long player_index);
    void populate(long player_index);
protected:
    virtual long get_current_item_count_internal();
public:
    c_static_stack<s_camera_entry, 20> m_cameras;
};

#include "interface/gui_property_value_datasource.h"

#endif // INTERFACE_GUI_SCREENS_DIRECTOR_GUI_SCREEN_OBSERVER_CAMERA_LIST_H
