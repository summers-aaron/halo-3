#ifndef INTERFACE_GUI_SCREENS_MATCHMAKING_GUI_SCREEN_MATCHMAKING_SEARCHING_H
#define INTERFACE_GUI_SCREENS_MATCHMAKING_GUI_SCREEN_MATCHMAKING_SEARCHING_H

#include <stddef.h>
#include "cseries/cseries.h"
#include "interface/c_gui_screen_widget.h"
#include "interface/gui_screens/pregame_lobby/advanced_screen/hopper_statistics_types.h"
#include "networking/logic/network_session_interface.h"
#include "networking/logic/storage/dynamic_matchmaking_hopper_statistics_types.h"
#include "networking/logic/storage/network_http_buffer_downloader.h"
#include "networking/logic/storage/network_http_blf_simple_downloader.h"

class c_gui_list_item_widget;

enum e_parse_xml_stat
{
    _parse_xml_their_skill_minimum = 0,
    _parse_xml_their_skill_range = 1,
    _parse_xml_their_skill_dynamic = 2,
    _parse_xml_their_size_minimum = 3,
    _parse_xml_their_size_range = 4,
    _parse_xml_their_size_dynamic = 5,
    _parse_xml_hopper_name = 6,
    _parse_xml_hopper_population = 7,
    _parse_xml_current_players = 8,
    _parse_xml_games_found = 9,
    _parse_xml_possible_matches = 10,
    _parse_xml_need_players = 11,
    _parse_xml_want_players = 12,
    _parse_xml_give_up_time = 13,
    _parse_xml_search_preference = 14,
    _parse_xml_party_leader = 15,
};

#include "networking/logic/life_cycle/life_cycle_matchmaking_progress.h"

struct s_matchmaking_tips
{
    long count;
    unsigned char tips[40][256];
};

struct gui_screen_matchmaking_searching_tip_downloader
{
    __forceinline gui_screen_matchmaking_searching_tip_downloader() :
        downloader(), last_retry_count(downloader.get_retry_count()), data(0), data_size(0)
    {
    }
    long get_data(const s_matchmaking_tips **data);
    c_http_stored_buffer_downloader<10401> downloader;
    long last_retry_count;
    const s_matchmaking_tips *data;
    unsigned long data_size;
};

class c_gui_screen_matchmaking_searching : public c_gui_screen_widget
{
public:
    __forceinline c_gui_screen_matchmaking_searching(long name) :
        c_gui_screen_widget(name), m_statistics_downloader(), m_tip_downloader(), m_tip_time(0), m_tip_index(NONE), m_progress(0.0f)
    {
        memset(&m_matchmaking_progress, 0, sizeof(m_matchmaking_progress));
    }
    virtual void dispose();
    virtual c_gui_list_widget *create_list_widget(const s_list_widget_block *definition);
    virtual e_gui_location get_gui_location() const { return _gui_location_matchmaking_searching; }
    virtual bool handle_dialog_result(const c_dialog_result_message *message);
    virtual bool handle_controller_input_message(const c_controller_input_message *message);
    virtual void initialize();
    virtual void post_initialize();
    virtual void initialize_datasource();
    virtual bool handle_list_item_chosen(
        const c_controller_input_message *message, long list_name, c_gui_list_item_widget *list_item, c_gui_data *data);
    virtual void update(unsigned long time);
protected:
    virtual e_render_data_size get_render_data_size();
public:

    const s_life_cycle_matchmaking_progress *get_current_matchmaking_progress() const { return &m_matchmaking_progress; }
    const s_hopper_statistics *get_hopper_statistics() const
    {
        const s_hopper_statistics *statistics = 0;
        if (m_hopper_identifier != 0xffff && m_hopper_identifier == network_squad_session_get_hopper())
            statistics = &m_hopper_statistics;
        return statistics;
    }

    union
    {
        s_life_cycle_matchmaking_progress m_matchmaking_progress;
        struct
        {
            char progress_prefix[0x8c];
            long m_ping_current;
        };
    };
    c_http_blf_simple_downloader<s_dynamic_matchmaking_hopper_statistics> m_statistics_downloader;
    gui_screen_matchmaking_searching_tip_downloader m_tip_downloader;
    unsigned long m_tip_time;
    long m_tip_index;
    unsigned short m_hopper_identifier;
    s_hopper_statistics m_hopper_statistics;
    long m_previous_state;
    long m_ping_previous;
    long m_ping_pending;
    long m_pong_previous;
    long m_pong_pending;
    float m_progress;

private:
    void update_status();
    void update_tips();
    void update_ping_pong();
    void update_progress(unsigned long time);
};

// Functions defined in tu/interfacelibxbox/gui_screen_matchmaking_searching.cpp.

bool parse_xml_their_skill_minimum(void *context, wchar_t *buffer, long buffer_length);

#endif // INTERFACE_GUI_SCREENS_MATCHMAKING_GUI_SCREEN_MATCHMAKING_SEARCHING_H
