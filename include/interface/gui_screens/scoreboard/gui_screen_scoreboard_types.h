#ifndef INTERFACE_GUI_SCREEN_SCOREBOARD_TYPES_H
#define INTERFACE_GUI_SCREEN_SCOREBOARD_TYPES_H

#include "cseries/cseries.h"
#include "game/player_types.h"
#include "interface/gui_data.h"
#include "interface/c_gui_screen_widget.h"
#include "interface/gui_render_data_size_types.h"

enum e_scoreboard_column
{
    _scoreboard_column_dead = 69,
    _scoreboard_column_alternate_state = 152,
    _scoreboard_column_has_teams = 278,
    _scoreboard_column_type = 0x103a8,
    _scoreboard_column_team = 0x10013,
    _scoreboard_column_base_color = 0x10127,
    _scoreboard_column_player_index = 0x10014,
    _scoreboard_column_session_player_index = 0x10015,
    _scoreboard_column_controller = 0x10016,
    _scoreboard_column_voice = 0x101eb,
    _scoreboard_column_rating = 0x1033b,
    _scoreboard_column_name = 0x2ae,
    _scoreboard_column_service_tag = 0x103cd,
    _scoreboard_column_place = 0x103cf,
    _scoreboard_column_score = 0x103d1,
    _scoreboard_column_round_score = 0x105bf,
};

// String ids of the screens, widgets, button keys and busy texts the scoreboard
// screen answers (mainmenu string table: message, ring_of_light, button_key_empty,
// button_key_a_select, button_key_x_mute_a_select, button_key_x_unmute_a_select,
// scoreboard_player_select, film_snippet_preparing_title,
// film_snippet_preparing_message, team_bar, player_emblem, observer, game,
// game_film, session, session_film, scoreboard_busy_start_game,
// scoreboard_busy_end_game, scoreboard_busy_selecting_new_host,
// scoreboard_busy_connecting_new_host, back).
enum e_scoreboard_string_id
{
    _scoreboard_string_message = 0x10005,
    _scoreboard_string_ring_of_light = 0x1000b,
    _scoreboard_string_button_key_empty = 0x10044,
    _scoreboard_string_button_key_a_select = 0x10052,
    _scoreboard_string_button_key_x_mute_a_select = 0x10053,
    _scoreboard_string_button_key_x_unmute_a_select = 0x10054,
    _scoreboard_string_scoreboard_player_select = 0x100bd,
    _scoreboard_string_film_snippet_preparing_title = 0x100f1,
    _scoreboard_string_film_snippet_preparing_message = 0x100f2,
    _scoreboard_string_team_bar = 0x103d5,
    _scoreboard_string_player_emblem = 0x103b6,
    _scoreboard_string_observer = 0x103d3,
    _scoreboard_string_game = 0x10342,
    _scoreboard_string_game_film = 0x104c5,
    _scoreboard_string_session = 0x104c6,
    _scoreboard_string_session_film = 0x104c7,
    _scoreboard_string_scoreboard_busy_start_game = 0x104c8,
    _scoreboard_string_scoreboard_busy_end_game = 0x104c9,
    _scoreboard_string_scoreboard_busy_selecting_new_host = 0x104ca,
    _scoreboard_string_scoreboard_busy_connecting_new_host = 0x104cb,
    _scoreboard_string_back = 0x9a,
};

struct s_gui_scoreboard_player_row {
 long type, player_index, session_player_index;
 s_player_appearance appearance;
 c_static_wchar_string<48> text_2a;
 c_static_wchar_string<48> text_8a;
 long rank, team_index;
 unsigned char flag;
 long controller, voice, rating;
 c_static_wchar_string<48> text_104;
 c_static_wchar_string<48> text_164;
 c_static_wchar_string<48> text_1c4;
 unsigned char dead, alternate_state;
 char end[2];
};

#pragma pack(push, 4)
class c_font_cache;
class c_gui_screen_scoreboard;
class c_gui_screen_scoreboard : public c_gui_screen_widget
{
public:
 __forceinline c_gui_screen_scoreboard(long name) : c_gui_screen_widget(name), m_scoreboard_mode(0)
 {
  m_interactive = false;
  m_render_in_screenshot = true;
 }
 virtual ~c_gui_screen_scoreboard() {}
 virtual void update(unsigned long current_milliseconds);
 virtual void initialize_datasource();
 virtual void post_initialize();
 virtual bool handle_list_item_chosen(const c_controller_input_message *message, long list_name, c_gui_list_item_widget *item, c_gui_data *data);
 virtual bool handle_controller_input_message(const c_controller_input_message *message);
 virtual void update_render_state(unsigned long current_milliseconds);
 enum e_scoreboard_mode {
  _scoreboard_mode_none = -1,
  _scoreboard_mode_session_playing = 3,
  _scoreboard_mode_session_film = 4,
 };
 static c_gui_screen_scoreboard *get_scoreboard_screen(e_controller_index index);
 static void hide_scoreboard(e_controller_index index);
 static bool is_scoreboard_displayed(e_controller_index controller_index);
 static void show_scoreboard(e_controller_index controller_index, bool interactive);
 static float get_scoreboard_alpha(e_controller_index controller_index)
 {
  float alpha = m_scoreboard_alpha[controller_index];
  if (!(alpha > m_any_controller_scoreboard_alpha))
   alpha = m_any_controller_scoreboard_alpha;
  return alpha;
 }
 static float m_scoreboard_alpha[k_number_of_controllers];
 static float m_any_controller_scoreboard_alpha;
private:
 void set_scoreboard_mode(e_scoreboard_mode scoreboard_mode);
 void update_score_message();
 void update_player_list();
 void update_button_key();
 void translate_widget_recursive(c_gui_widget *gui_widget, long x, long y);
 bool select_player_in_scoreboard(e_controller_index controller_index, const s_player_identifier *player_identifier);
public:
 long m_scoreboard_mode;
 bool m_interactive;
 __forceinline bool screen_transitioning_out() { return transitioning_out(); }
 __forceinline bool get_is_interactive() { return m_interactive && !screen_transitioning_out(); }
protected:
 virtual e_render_data_size get_render_data_size() { return k_render_data_size_large; }
};
#pragma pack(pop)
class c_gui_scoreboard_data : public c_gui_ordered_data {
public:
 enum e_voice_talking_state
 {
  _voice_none = 0,
  _voice_available,
  _voice_talking,
  _voice_private_chat,
  _voice_muted
 };
 c_gui_scoreboard_data(e_controller_index controller) : m_scoreboard_mode(0), count(0), m_controller_index(controller) {}
 void update_for_scoreboard_mode(bool teams, bool round);
 e_voice_talking_state get_voice_for_player(long player_index);
 virtual void update();
 virtual long get_current_item_count_internal();
 virtual void get_column_names(long *const columns, long *count);
 virtual bool get_integer_value(long index, long column, long *value);
 virtual bool get_text_value(long index, long column, c_static_wchar_string<1024> *string);
 virtual bool get_player_appearance(long element_handle, s_player_appearance *appearance);
 long m_scoreboard_mode;
 s_gui_scoreboard_player_row rows[25];
 long count;
 e_controller_index m_controller_index;
 static int scoreboard_sort_proc(const void *a, const void *b);
};

typedef char scoreboard_row_session_index_offset[(offsetof(s_gui_scoreboard_player_row, session_player_index) == 8) ? 1 : -1];
typedef char scoreboard_row_team_index_offset[(offsetof(s_gui_scoreboard_player_row, team_index) == 0xf0) ? 1 : -1];

#endif
