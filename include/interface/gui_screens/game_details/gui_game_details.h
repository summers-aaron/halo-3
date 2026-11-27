#ifndef INTERFACE_GUI_SCREENS_GAME_DETAILS_GUI_GAME_DETAILS_H
#define INTERFACE_GUI_SCREENS_GAME_DETAILS_GUI_GAME_DETAILS_H

#include "cseries/cseries.h"
#include "interface/c_gui_screen_widget.h"
#include "game/player_configuration.h"
#include <stddef.h>
#include "networking/logic/network_search_types.h"
#include "networking/online/online_invite_task.h"

#include "interface/gui_data.h"

class c_gui_game_details_menu_datasource : public c_gui_ordered_data
{
public:
    virtual void get_column_names(long *const names, long *count);
    virtual bool get_string_id_value(long element_handle, long value_name, long *value);
    void show_only_necessary_fields(const s_network_squad_status_data *config, e_controller_index controller, long player_index);
protected:
    virtual long get_current_item_count_internal();
};

class c_gui_game_details_info_datasource : public c_gui_ordered_data
{
public:
    virtual void get_column_names(long *const names, long *count);
    virtual bool get_string_id_value(long type, long value_name, long *value);
    void show_only_necessary_fields(const s_network_squad_status_data *config, e_controller_index controller, long player_index);
protected:
    virtual long get_current_item_count_internal();
};

typedef char gui_game_details_menu_datasource_size[(sizeof(c_gui_game_details_menu_datasource) == sizeof(c_gui_ordered_data)) ? 1 : -1];
typedef char gui_game_details_info_datasource_size[(sizeof(c_gui_game_details_info_datasource) == sizeof(c_gui_ordered_data)) ? 1 : -1];

class c_gui_game_details : public c_gui_screen_widget {
protected:
  virtual e_render_data_size get_render_data_size();
public:
  __forceinline c_gui_game_details(long name) :
    c_gui_screen_widget(name),
    m_game_details()
  {
    csmemset(&m_game_details, 0, sizeof(m_game_details));
  }
  virtual bool get_emblem_info_for_widget(c_gui_widget *widget, s_emblem_info *player_id);
  virtual void initialize();
  virtual void update_render_state(unsigned long current_milliseconds);
  virtual void initialize_datasource();
  virtual c_gui_list_widget *create_list_widget(const s_list_widget_block *definition);
  virtual bool handle_controller_input_message(const c_controller_input_message *message);
  virtual bool handle_list_item_chosen(const c_controller_input_message *message, long list_name, c_gui_list_item_widget *item, c_gui_data *data);

  long m_initial_state;
  s_network_squad_status_data m_game_details;
  s_player_configuration m_configurations[16];
  c_invite_task m_invite_task;
};

typedef char game_details_selected_player_offset[(sizeof(void *) != 4 || (offsetof(c_gui_game_details, m_initial_state) == 0x1a88)) ? 1 : -1];
typedef char game_details_status_offset[(sizeof(void *) != 4 || (offsetof(c_gui_game_details, m_game_details) == 0x1a90)) ? 1 : -1];
typedef char game_details_configurations_offset[(sizeof(void *) != 4 || (offsetof(c_gui_game_details, m_configurations) == 0x2e58)) ? 1 : -1];

typedef char game_details_invite_offset[(sizeof(void *) != 4 || ((offsetof(c_gui_game_details, m_invite_task) == 0x3f58))) ? 1 : -1];
typedef char game_details_screen_size[(sizeof(void *) != 4 || sizeof(c_gui_game_details) == 0x4170) ? 1 : -1];

// Functions defined in tu/interfacelibxbox/gui_game_details.cpp.

extern "C" bool parse_lobby_delaying_player(void *self, wchar_t *buffer, long size);

class c_gui_screen_widget;

extern "C" bool parse_selected_player_name(c_gui_screen_widget *screen, wchar_t *destination, long count);
extern "C" bool parse_squad_join_status(c_gui_screen_widget *screen, wchar_t *destination, long count);
bool parse_party_player_count(void *screen, wchar_t *buffer, long size);
bool parse_party_player_max(void *screen, wchar_t *buffer, long size);
extern "C" bool parse_first_player_name(c_gui_screen_widget *screen, wchar_t *destination, long count);
extern "C" bool parse_game_mode_name(c_gui_screen_widget *screen, wchar_t *destination, long count);
extern "C" bool parse_game_status_text(c_gui_screen_widget *screen, wchar_t *destination, long count);
extern "C" bool parse_map_name_alias(c_gui_screen_widget *screen, wchar_t *destination, long count);
extern "C" bool parse_difficulty_name(c_gui_screen_widget *screen, wchar_t *destination, long count);
extern "C" bool parse_variant_name(c_gui_screen_widget *screen, wchar_t *destination, long count);
extern "C" bool parse_description_text(c_gui_screen_widget *screen, wchar_t *destination, long count);
extern "C" bool parse_time_remaining(void *screen, wchar_t *buffer, long size);
extern "C" bool parse_first_detail_number(c_gui_screen_widget *screen, wchar_t *destination, long count);
extern "C" bool parse_second_detail_number(c_gui_screen_widget *screen, wchar_t *destination, long count);
extern "C" bool parse_campaign_name(c_gui_screen_widget *screen, wchar_t *destination, long count);
extern "C" bool parse_campaign_detail_text(c_gui_screen_widget *screen, wchar_t *destination, long count);
extern "C" bool parse_map_name(c_gui_screen_widget *screen, wchar_t *destination, long count);
extern "C" bool parse_variant_short_name(c_gui_screen_widget *screen, wchar_t *destination, long count);
extern "C" bool parse_team_count(c_gui_screen_widget *screen, wchar_t *destination, long count);

extern "C" {
extern const wchar_t k_game_details_tag_selected_player_name[];
extern const wchar_t k_game_details_tag_squad_join_status[];
extern const wchar_t k_game_details_tag_party_player_count[];
extern const wchar_t k_game_details_tag_party_player_max[];
extern const wchar_t k_game_details_tag_first_player_name[];
extern const wchar_t k_game_details_tag_game_mode_name[];
extern const wchar_t k_game_details_tag_game_status_text[];
extern const wchar_t k_game_details_tag_map_name_alias[];
extern const wchar_t k_magic_string_difficulty[];
extern const wchar_t k_game_details_tag_variant_name[];
extern const wchar_t k_game_details_tag_description_text[];
extern const wchar_t k_game_details_tag_time_remaining[];
extern const wchar_t k_game_details_tag_first_detail_number[];
extern const wchar_t k_game_details_tag_second_detail_number[];
extern const wchar_t k_game_details_tag_campaign_name[];
extern const wchar_t k_game_details_tag_campaign_detail_text[];
extern const wchar_t k_magic_string_map_name[];
extern const wchar_t k_game_details_tag_variant_short_name[];
extern const wchar_t k_game_details_tag_team_count[];
}

#endif // INTERFACE_GUI_SCREENS_GAME_DETAILS_GUI_GAME_DETAILS_H
