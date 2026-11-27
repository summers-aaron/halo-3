// interfacelibxbox/gui_screen_observer_camera_list.obj: from the records 0x488df8 0x488fa0 0x4890b0 0x489198 0x4891d0 0x489200 0x4892d0 0x4892e8 0x489338 0x489420 0x4894c8 0x489568 0x4de4b8
// @compiler xdk6130
// @flags /TP /O2 /Oc
#include "interface/gui_screen_widget_legacy.h"
#include "memory/data_types.h"
#include "interface/gui_widget_tree.h"
#include "interface/c_gui_screen_widget.h"
#include "cseries/cseries.h"
#include "memory/static_arrays.h"
#include "game/player_types.h"
#include "interface/gui_widget_types.h"
#include "interface/gui_widget.h"
#include "interface/user_interface_messages.h"
#include <new>
#include "game/player_in_game_iterator.h"
#include "interface/gui_data.h"
#include "tag_files/string_ids.h"
// @address 0x486130 ?handle_controller_input_message@c_gui_screen_widget@@UAA_NPBVc_controller_input_message@@@Z
// @address 0x4891d0 ?initialize@c_camera_list_datasource@@UAA_NJJ@Z
// @address 0x489200 ?populate@c_camera_list_datasource@@QAAXJ@Z
// @address 0x489338 ?get_integer_value@c_camera_list_datasource@@UAA_NJJPAJ@Z
// @address 0x489420 ?get_text_value@c_camera_list_datasource@@UAA_NJJPAV?$c_static_wchar_string@$0EAA@@@@Z
// @address 0x4894c8 ?get_player_appearance@c_camera_list_datasource@@UAA_NJPAUs_player_appearance@@@Z
// @address 0x489568 ?get_current_item_count_internal@c_camera_list_datasource@@MAAJXZ
// @address 0x4de4b8 ?get_integer_value@c_gui_property_value_datasource@@UAA_NJJPAJ@Z
// @address 0xac7c0 ?datum_try_and_get@@YAPAXPBUs_data_array@@J@Z
// @address 0x807a0 ?game_is_cooperative@@YA_NXZ
// @address 0xa7970 ?next@c_player_in_game_iterator@@QAA_NXZ
// @address 0x1bfeb8 ?set@?$c_static_wchar_string@$0EAA@@@QAAXPB_W@Z
#include <string.h>

#include "game/game.h"
#include "interface/user_interface_memory.h"
#include "game/game_engine_team.h"
#include "interface/user_interface_utilities.h"
#include "memory/data.h"
#include "memory/tls_globals.h"
#include "game/player_mapping_types.h"
#include "interface/gui_screens/director/gui_screen_observer_camera_list.h"
#include "game/players.h"
#include "interface/c_gui_widget.h"




static __forceinline player_datum *observer_camera_list_player_get(long player_index)
{
    return (player_datum *)(tls_player_data()->data + (player_index & 0xffff) * sizeof(player_datum));
}









// @address 0x4890b0 ?update@c_observer_camera_list_screen@@UAAXK@Z
void c_observer_camera_list_screen::update(unsigned long current_milliseconds)
{
    long player_index = -1;
    if (m_user_index != -1)
        player_index = tls_player_mapping_globals()->player_indices[m_user_index];
    m_responds_to_controller_events = false;
    c_gui_screen_widget::update(current_milliseconds);
    if (m_camera_list_datasource)
        m_camera_list_datasource->populate(player_index);
    if ((!game_is_cooperative() || player_index == -1)
        && ((m_user_index != -1 && tls_player_mapping_globals()->unit_indices[m_user_index] != -1) || player_index == -1))
        transition_out_with_transition_type(0, _screen_transition_normal);
}

bool c_camera_list_datasource::initialize(long name, long player_index)
{
    m_name = name;
    populate(player_index);
    return true;
}

void c_camera_list_datasource::populate(long player_index)
{
    long team_index = player_get_team_index(player_index);
    m_cameras.clear();
    s_camera_entry entry;
    c_player_in_game_iterator iterator;
    iterator.begin();
    while (iterator.next()) {
        long player_team_index = player_get_team_index(iterator.get_index());
        if (player_team_index == team_index) {
            entry.datum_index = iterator.get_index();
            m_cameras.push_back(entry);
        }
    }
}

void c_camera_list_datasource::get_column_names(long *const column_names, long *column_count)
{
    *column_count = 0;
    ++*column_count;
    column_names[*column_count] = _string_id_camera_list_datasource;
}

bool c_camera_list_datasource::get_element(long element_handle, void *element, long element_size)
{
    bool result = false;
    if (element_handle >= 0 && element_handle < m_cameras.count()) {
        memcpy(element, &m_cameras[element_handle], sizeof(s_camera_entry));
        result = true;
    }
    return result;
}

bool c_camera_list_datasource::get_integer_value(long element_handle, long column_name, long *value)
{
    bool result = false;
    if (element_handle >= 0 && element_handle < m_cameras.count()) {
        switch (column_name) {
        case _camera_column_player_color:
            if (m_cameras[element_handle].type == _camera_type_player && m_cameras[element_handle].datum_index != NONE) {
                player_datum *player = (player_datum *)datum_try_and_get(tls_player_data(), m_cameras[element_handle].datum_index);
                if (player) {
                    *value = player->configuration.client.appearance.primary_color;
                    result = true;
                }
            }
            break;
        case _camera_column_team:
            if (m_cameras[element_handle].type == _camera_type_player && m_cameras[element_handle].datum_index != NONE) {
                player_datum *player = (player_datum *)datum_try_and_get(tls_player_data(), m_cameras[element_handle].datum_index);
                if (player) {
                    *value = player->configuration.host.team_index;
                    result = true;
                }
            }
            break;
        }
    }
    return result;
}

bool c_camera_list_datasource::get_text_value(long element_handle, long column_name, c_static_wchar_string<1024> *buffer)
{
    bool result = false;
    if (element_handle >= 0 && element_handle < m_cameras.count()) {
        if (m_cameras[element_handle].type == _camera_type_player) {
            long player_index = m_cameras[element_handle].datum_index;
            if (datum_try_and_get(tls_player_data(), player_index)) {
                buffer->set(observer_camera_list_player_get(player_index)->configuration.host.name);
                result = true;
            }
        }
    }
    return result;
}

bool c_camera_list_datasource::get_player_appearance(long element_handle, s_player_appearance *appearance)
{
    bool result = false;
    if (element_handle >= 0 && element_handle < m_cameras.count()) {
        if (m_cameras[element_handle].type == _camera_type_player) {
            player_datum *player = (player_datum *)datum_try_and_get(tls_player_data(), m_cameras[element_handle].datum_index);
            if (player) {
                *appearance = player->configuration.client.appearance;
                result = true;
            }
        }
    }
    return result;
}

long c_camera_list_datasource::get_current_item_count_internal()
{
    return m_cameras.count();
}

bool c_gui_property_value_datasource::get_integer_value(long element_handle, long column_name, long *value)
{
    *value = 0;
    return true;
}

// @address 0x484e90 ?user_interface_malloc_tracked@@YAPAXIPBDJ@Z
// @address 0x484f88 ?user_interface_free@@YAXPBX@Z
// @address 0x4e8c8 ??_7c_camera_list_datasource@@6B@

template<class t_type> static __forceinline void observer_camera_list_track_delete(t_type *data)
{
    data->~t_type();
    user_interface_free(data);
}

// @address 0x488df8 ?initialize_datasource@c_observer_camera_list_screen@@UAAXXZ
void c_observer_camera_list_screen::initialize_datasource()
{
    gui_screen_widget_initialize_datasource(this);
    void *memory = user_interface_malloc_tracked(sizeof(c_camera_list_datasource), 0, 0);
    c_camera_list_datasource *datasource = memory ? new(memory) c_camera_list_datasource() : 0;
    m_camera_list_datasource = datasource;
    if (datasource) {
        long player_index = -1;
        if (m_user_index != -1)
            player_index = tls_player_mapping_globals()->player_indices[m_user_index];
        if (datasource->initialize(_string_id_camera_list_datasource, player_index)) {
            m_datasource[m_datasource_count] = m_camera_list_datasource;
            m_datasource_count++;
            if (player_index != -1) {
                long camera_index = -1;
                c_camera_list_datasource *list = m_camera_list_datasource;
                for (long index = 0; index < list->m_cameras.count(); index++) {
                    if (list->m_cameras[index].datum_index == player_index && list->m_cameras[index].type == _camera_type_player) {
                        camera_index = index;
                        break;
                    }
                }
                m_initial_focus_element_handle = camera_index;
                m_initial_focus_list_name = _string_id_camera_list;
            }
        } else {
            observer_camera_list_track_delete(m_camera_list_datasource);
        }
    }
}

// @address 0x488fa0 gui_screen_observer_camera_list_update_colors
extern "C" void gui_screen_observer_camera_list_update_colors(c_observer_camera_list_screen *screen, long user_index)
{
    c_gui_widget *list = ((c_gui_widget *)screen)->get_child_widget(_gui_list_widget, _string_id_camera_list);
    if (list != 0 && screen->m_camera_list_datasource != 0) {
        c_gui_widget *widget = gui_widget_find_child_by_name(list, _gui_list_item_widget);
        if (widget != 0) {
            do {
                c_gui_widget *child = widget->get_child_widget(_gui_bitmap_widget, _camera_column_player_color);
                if (child != 0) {
                    long column;
                    if ((unsigned char)game_engine_has_teams() != 0)
                        column = _camera_column_team;
                    else
                        column = _camera_column_player_color;
                    long color_index = -1;
                    if (screen->m_camera_list_datasource->get_integer_value(widget->get_element_handle(), column, &color_index))
                        tint_widget_to_change_color(child, color_index, game_engine_has_teams());
                }
                widget = gui_widget_get_next_list_item(widget, true);
            } while (widget != 0);
        }
    }
}

// @address 0x489198 ?update_render_state@c_observer_camera_list_screen@@UAAXK@Z
void c_observer_camera_list_screen::update_render_state(unsigned long current_milliseconds)
{
    c_gui_widget::update_render_state(current_milliseconds);
    gui_screen_observer_camera_list_update_colors(this, m_user_index);
}
