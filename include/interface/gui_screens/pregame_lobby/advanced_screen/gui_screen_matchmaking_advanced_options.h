#ifndef INTERFACE_GUI_SCREENS_PREGAME_LOBBY_ADVANCED_SCREEN_GUI_SCREEN_MATCHMAKING_ADVANCED_OPTIONS_H
#define INTERFACE_GUI_SCREENS_PREGAME_LOBBY_ADVANCED_SCREEN_GUI_SCREEN_MATCHMAKING_ADVANCED_OPTIONS_H

// Functions defined in tu/interfacelibxbox/gui_screen_matchmaking_advanced_options.cpp.

#include "interface/gui_screens/pregame_lobby/advanced_screen/matchmaking_advanced_options_parser.h"

#include "cseries/cseries.h"
#include "interface/c_gui_screen_widget.h"
#include "networking/logic/storage/network_http_buffer_downloader.h"
#include "networking/logic/storage/network_http_blf_simple_downloader.h"
#include "networking/logic/storage/dynamic_matchmaking_hopper_statistics_types.h"
#include "networking/session/network_session_parameter_matchmaking_types.h"
#include "networking/logic/network_session_interface.h"

class c_gui_screen_matchmaking_advanced_options : public c_gui_screen_widget
{
public:
    __forceinline c_gui_screen_matchmaking_advanced_options(long name) :
        c_gui_screen_widget(name),
        m_histogram_downloader(),
        m_statistics_downloader(),
        m_histogram_hopper(NONE),
        m_histogram_loaded(false)
    {
    }
    virtual void initialize();
    virtual void post_initialize();
    virtual void dispose();
    virtual c_gui_bitmap_widget *create_bitmap_widget(const s_runtime_bitmap_widget_definition *definition);
    virtual void update(unsigned long current_milliseconds);
    virtual bool handle_list_item_chosen(const c_controller_input_message *message, long list_name, c_gui_list_item_widget *item, c_gui_data *data);

    __forceinline const s_matchmaking_single_hopper_statistics *get_hopper_statistics()
    {
        const s_matchmaking_single_hopper_statistics *statistics = 0;
        if (m_hopper_identifier != 0xffff && m_hopper_identifier == network_squad_session_get_hopper())
            statistics = &m_hopper_statistics;
        return statistics;
    }

    c_http_stored_buffer_downloader<0x19000> m_histogram_downloader;
    c_http_blf_simple_downloader<s_dynamic_matchmaking_hopper_statistics> m_statistics_downloader;
    unsigned short m_hopper_identifier;

    s_matchmaking_single_hopper_statistics m_hopper_statistics;
    unsigned short m_histogram_hopper;
    bool m_histogram_loaded;
private:
    void update_description();
    void update_histogram();
};

typedef char advanced_options_statistics_offset[
    (sizeof(void *) != 4 || ((offsetof(c_gui_screen_matchmaking_advanced_options, m_statistics_downloader) == 0x1b118))) ? 1 : -1];
typedef char advanced_options_hopper_offset[
    (sizeof(void *) != 4 || ((offsetof(c_gui_screen_matchmaking_advanced_options, m_hopper_identifier) == 0x1c158))) ? 1 : -1];
typedef char advanced_options_histogram_hopper_offset[
    (sizeof(void *) != 4 || ((offsetof(c_gui_screen_matchmaking_advanced_options, m_histogram_hopper) == 0x1c164))) ? 1 : -1];

#endif // INTERFACE_GUI_SCREENS_PREGAME_LOBBY_ADVANCED_SCREEN_GUI_SCREEN_MATCHMAKING_ADVANCED_OPTIONS_H
