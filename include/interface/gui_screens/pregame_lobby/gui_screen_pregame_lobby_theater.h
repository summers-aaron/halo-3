#ifndef INTERFACE_GUI_SCREENS_PREGAME_LOBBY_GUI_SCREEN_PREGAME_LOBBY_THEATER_H
#define INTERFACE_GUI_SCREENS_PREGAME_LOBBY_GUI_SCREEN_PREGAME_LOBBY_THEATER_H

#include "interface/gui_game_mode_types.h"
#include "interface/gui_screens/pregame_lobby/gui_screen_pregame_lobby.h"

// String ids the theater lobby screen uses, named after their cache string table entries.
enum e_pregame_lobby_theater_string_id
{
    _pregame_lobby_theater_string_target = 0x10011,
    _pregame_lobby_theater_string_game_name = 0x1002a,
    _pregame_lobby_theater_string_film = 0x10213,
    _pregame_lobby_theater_string_lobby_list = 0x102a7,
    _pregame_lobby_theater_string_title_theater = 0x102ad,
    _pregame_lobby_theater_string_header_theater = 0x102b2,
    _pregame_lobby_theater_string_start_theater = 0x102bb,
    _pregame_lobby_theater_string_options_leader_theater = 0x102c3,
    _pregame_lobby_theater_string_options_member_theater = 0x102c8,
    _pregame_lobby_theater_string_recent_films = 0x102eb,
    _pregame_lobby_theater_string_theater_no_film = 0x10307,
    _pregame_lobby_theater_string_theater_invalid_film = 0x10308,
    _pregame_lobby_theater_string_gametype_image = 0x10328,
    _pregame_lobby_theater_string_map_image = 0x1032a,
    _pregame_lobby_theater_string_difficulty_image = 0x1032d,
};

// Functions defined in tu/interfacelibxbox/gui_screen_pregame_lobby_theater.cpp.

class c_gui_screen_pregame_lobby_theater : public c_gui_screen_pregame_lobby
{
public:
    __forceinline c_gui_screen_pregame_lobby_theater(long name)
        : c_gui_screen_pregame_lobby(name), m_initial_film_selected(false) {}

    virtual void initialize();
    virtual void initialize_datasource();
    virtual void update(unsigned long milliseconds);
    virtual bool handle_list_item_chosen(const c_controller_input_message *message, long list_name,
        c_gui_list_item_widget *list_item_widget, c_gui_data *data);
    virtual bool advanced_options_enabled() { return false; }
    virtual long get_lobby_header();
    virtual long get_lobby_title();
    virtual long get_start_button_name();
    virtual long get_start_status_text(bool is_leader);
    virtual long get_gui_game_mode() { return _gui_game_mode_theater; }

protected:
    virtual bool team_switching_allowed() { return false; }

private:
    bool m_initial_film_selected;
};

typedef char pregame_lobby_theater_size[(sizeof(void *) != 4 || sizeof(c_gui_screen_pregame_lobby_theater) == 0x1a98) ? 1 : -1];

extern "C" bool gui_screen_pregame_lobby_theater_get_film_name(void *context, wchar_t *output, long capacity);
extern "C" bool gui_screen_pregame_lobby_theater_get_map_name(void *context, wchar_t *output, long capacity);
extern "C" bool gui_screen_pregame_lobby_theater_get_film_duration(void *context, wchar_t *output, long capacity);

extern "C" const wchar_t k_theater_film_name_tag[];
extern "C" const wchar_t k_theater_map_name_tag[];
extern "C" const wchar_t k_theater_film_map_title_tag[];
extern "C" const wchar_t k_theater_film_duration_tag[];
#endif // INTERFACE_GUI_SCREENS_PREGAME_LOBBY_GUI_SCREEN_PREGAME_LOBBY_THEATER_H
