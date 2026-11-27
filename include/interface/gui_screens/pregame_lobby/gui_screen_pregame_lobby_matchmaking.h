#ifndef INTERFACE_GUI_SCREENS_PREGAME_LOBBY_GUI_SCREEN_PREGAME_LOBBY_MATCHMAKING_H
#define INTERFACE_GUI_SCREENS_PREGAME_LOBBY_GUI_SCREEN_PREGAME_LOBBY_MATCHMAKING_H

#include "interface/gui_screens/pregame_lobby/gui_screen_pregame_lobby.h"
#include "networking/logic/storage/dynamic_matchmaking_hopper_statistics_types.h"
#include "networking/logic/storage/network_http_blf_simple_downloader.h"
#include "networking/logic/storage/network_http_buffer_downloader.h"
#include "networking/session/network_session_parameter_matchmaking_types.h"
#include <stddef.h>

class c_gui_screen_pregame_lobby_matchmaking : public c_gui_screen_pregame_lobby {
public:
    __forceinline c_gui_screen_pregame_lobby_matchmaking(long name) :
        c_gui_screen_pregame_lobby(name), m_hopper_statistics_downloader(), m_nightmap_version(0),
        m_nightmap_downloader(), m_nightmap_loaded(false), hopper_catalog_state(0), hopper_id(0xffff)
    {
    }
    virtual void initialize();
    virtual bool handle_dialog_result(const c_dialog_result_message *dialog_result_message);
    virtual bool handle_controller_input_message(const c_controller_input_message *message);
    virtual void update(unsigned long current_milliseconds);
    virtual bool handle_list_item_chosen(const c_controller_input_message *message, long list_name, c_gui_list_item_widget *list_item_widget, c_gui_data *data);
    virtual c_gui_bitmap_widget *create_bitmap_widget(const s_runtime_bitmap_widget_definition *definition);
    virtual long get_lobby_header();
    virtual long get_lobby_title();
    virtual long get_start_button_name();
    virtual long get_start_status_text(bool is_leader);
    virtual long get_gui_game_mode();
    virtual void start_fade_during_countdown();
    virtual void stop_fade_during_countdown();
protected:
    virtual bool advanced_options_enabled();
public:
    c_http_blf_simple_downloader<s_dynamic_matchmaking_hopper_statistics> m_hopper_statistics_downloader;
    long m_nightmap_version;
    c_http_stored_buffer_downloader<102400> m_nightmap_downloader;
    bool m_nightmap_loaded;
    long hopper_catalog_state;
    unsigned short hopper_id;
    s_matchmaking_single_hopper_statistics m_hopper_statistics;
    const s_matchmaking_single_hopper_statistics *get_hopper_statistics(void);
private:
    void update_population();
    void update_nightmap();
};
typedef char pregame_matchmaking_catalog_state_offset[
    (sizeof(void *) != 4 || offsetof(c_gui_screen_pregame_lobby_matchmaking, hopper_catalog_state) == 0x1c168) ? 1 : -1];
typedef char pregame_matchmaking_screen_size[(sizeof(void *) != 4 || sizeof(c_gui_screen_pregame_lobby_matchmaking) == 0x1c178) ? 1 : -1];

// Functions defined in tu/interfacelibxbox/gui_screen_pregame_lobby_matchmaking.cpp.

bool parse_xml_lobby_matchmaking_hopper(void *context, wchar_t *buffer, long buffer_length);
bool parse_xml_lobby_matchmaking_hopper_population(void *context, wchar_t *buffer, long buffer_length);
bool parse_xml_lobby_matchmaking_total_population(void *context, wchar_t *buffer, long buffer_length);

extern "C" const char k_matchmaking_nightmap_url_path[];

#endif // INTERFACE_GUI_SCREENS_PREGAME_LOBBY_GUI_SCREEN_PREGAME_LOBBY_MATCHMAKING_H
