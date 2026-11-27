#ifndef INTERFACE_GUI_SCREENS_PREGAME_LOBBY_GUI_SCREEN_PREGAME_LOBBY_CAMPAIGN_H
#define INTERFACE_GUI_SCREENS_PREGAME_LOBBY_GUI_SCREEN_PREGAME_LOBBY_CAMPAIGN_H

#include "interface/gui_screens/pregame_lobby/gui_screen_pregame_lobby.h"
#include "game/player_types.h"
#include <string.h>

// String ids the campaign lobby screen uses, named after their cache string table entries.
enum e_pregame_lobby_campaign_string_id
{
    _pregame_lobby_campaign_string_level = 0x271,
    _pregame_lobby_campaign_string_difficulty = 0x272,
    _pregame_lobby_campaign_string_target = 0x10011,
    _pregame_lobby_campaign_string_in_progress = 0x100b3,
    _pregame_lobby_campaign_string_pregame_selection_enumeration_in_progress_title = 0x100d9,
    _pregame_lobby_campaign_string_pregame_selection_enumeration_in_progress_message = 0x100da,
    _pregame_lobby_campaign_string_lobby_list = 0x102a7,
    _pregame_lobby_campaign_string_title_campaign = 0x102a9,
    _pregame_lobby_campaign_string_header_campaign = 0x102ae,
    _pregame_lobby_campaign_string_start_campaign_new = 0x102b4,
    _pregame_lobby_campaign_string_start_campaign_resume = 0x102b5,
    _pregame_lobby_campaign_string_options_leader_campaign = 0x102bf,
    _pregame_lobby_campaign_string_options_member_campaign = 0x102c4,
    _pregame_lobby_campaign_string_difficulty_image = 0x1032d,
};

struct s_gui_screen_pregame_lobby_campaign_players { s_player_identifier identifiers[16]; };
class c_gui_screen_pregame_lobby_campaign : public c_gui_screen_pregame_lobby
{
public:
    __forceinline c_gui_screen_pregame_lobby_campaign(long name) : c_gui_screen_pregame_lobby(name), m_player_count(NONE)
    {
        memset(&m_players, 0, sizeof(m_players));
        m_saved_game_controller = _controller_index_none;
        m_loading_screen_time = 0;
    }
    virtual bool handle_controller_input_message(const c_controller_input_message *message);
    virtual void update(unsigned long milliseconds);
    virtual long get_lobby_header();
    virtual long get_lobby_title();
    virtual long get_start_status_text(bool is_leader);
    virtual long get_start_button_name();
    virtual long get_gui_game_mode();
    virtual void initialize();
    virtual bool handle_list_item_chosen(const c_controller_input_message *message, long list, c_gui_list_item_widget *list_item_widget, c_gui_data *data);
    long m_player_count;
    s_gui_screen_pregame_lobby_campaign_players m_players;
    e_controller_index m_saved_game_controller;
    unsigned long m_loading_screen_time;
};
typedef char pregame_lobby_campaign_size[(sizeof(void *) != 4 || sizeof(c_gui_screen_pregame_lobby_campaign) == 0x1b20) ? 1 : -1];

// Functions defined in tu/interfacelibxbox/gui_screen_pregame_lobby_campaign.cpp.

extern "C" bool parse_xml_lobby_campaign_map_name(void *context, wchar_t *output, long capacity);
extern "C" bool parse_xml_lobby_campaign_difficulty(void *context, wchar_t *output, long capacity);
extern "C" bool parse_xml_lobby_campaign_insertion_point(void *context, wchar_t *output, long capacity);


#endif // INTERFACE_GUI_SCREENS_PREGAME_LOBBY_GUI_SCREEN_PREGAME_LOBBY_CAMPAIGN_H
