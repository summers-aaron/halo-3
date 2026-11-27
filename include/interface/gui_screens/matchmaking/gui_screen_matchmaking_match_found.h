#ifndef INTERFACE_GUI_SCREEN_MATCHMAKING_MATCH_FOUND_H
#define INTERFACE_GUI_SCREEN_MATCHMAKING_MATCH_FOUND_H

#include <stddef.h>
#include "interface/c_gui_screen_widget.h"
#include "networking/logic/life_cycle/life_cycle_matchmaking_progress.h"

// The update handler refreshes the complete matchmaking progress record.
// The timer and veto bookkeeping immediately follow that record.
class c_gui_screen_matchmaking_match_found : public c_gui_screen_widget
{
public:
    c_gui_screen_matchmaking_match_found(long name);
    virtual ~c_gui_screen_matchmaking_match_found() {}
    virtual c_gui_bitmap_widget *create_bitmap_widget(const s_runtime_bitmap_widget_definition *definition);
    virtual c_gui_list_widget *create_list_widget(const s_list_widget_block *definition);
    virtual e_gui_location get_gui_location() const { return _gui_location_matchmaking_match_found; }
    virtual void initialize_datasource();
    virtual bool handle_list_item_chosen(const c_controller_input_message *message, long list_name,
        c_gui_list_item_widget *list_item_widget, c_gui_data *data);
    virtual void initialize();
    virtual void post_initialize();
    virtual void update(unsigned long current_milliseconds);
    virtual bool handle_controller_input_message(const c_controller_input_message *message);
    virtual bool process_message(const c_message *message);

    // Preparing-map progress lives in configuring_match;
    // ready_player_mask is the veto mask.
    s_life_cycle_matchmaking_progress m_matchmaking_progress;
    long previous_countdown;
    bool countdown_started;
    long previous_veto_count;

private:
    void update_status();
    void update_countdown();
};

typedef char match_found_progress_offset[
    (sizeof(void *) != 4 || ((offsetof(c_gui_screen_matchmaking_match_found, m_matchmaking_progress) == 0x1a88))) ? 1 : -1];
typedef char match_found_countdown_offset[(sizeof(void *) != 4 || ((offsetof(c_gui_screen_matchmaking_match_found, previous_countdown) == 0x1b80))) ? 1 : -1];
typedef char match_found_veto_offset[(sizeof(void *) != 4 || ((offsetof(c_gui_screen_matchmaking_match_found, previous_veto_count) == 0x1b88))) ? 1 : -1];

#endif
