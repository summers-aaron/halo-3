#ifndef INTERFACE_GUI_SELECTED_ITEMS_H
#define INTERFACE_GUI_SELECTED_ITEMS_H

#include "interface/user_interface_messages.h"

#include <stddef.h>

#include "interface/gui_selected_item_metadata.h"

// Selection kinds used by the pregame setup manager and the selected-item datasources.
enum e_gui_selected_item_type
{
    _gui_selected_item_none = -1,
    _gui_selected_item_campaign = 0,
    _gui_selected_item_difficulty = 1,
    _gui_selected_item_map = 2,
    _gui_selected_item_game_variant = 3,
    _gui_selected_item_hopper = 4,
    _gui_selected_item_film = 5,
    _gui_selected_item_screenshot = 6,
    _gui_selected_item_advertisement = 7,
};

enum e_gui_selected_item_location
{
    _gui_selected_item_location_none = -1,
    _gui_stored_item_location_built_in,
    _gui_stored_item_location_saved_game_file,
    _gui_stored_item_location_autosave_queue,
};

class c_load_pregame_selection_screen_message : public c_load_screen_message
{
public:
    c_load_pregame_selection_screen_message(e_controller_index controller, e_window_index window, long layered_position,
        e_gui_selected_item_type selection_type);
    virtual void apply_initial_state(c_gui_screen_widget *screen) const;

    e_gui_selected_item_type m_selection_type;
};
typedef char load_pregame_selection_screen_message_size[(sizeof(void *) != 4 || ((sizeof(c_load_pregame_selection_screen_message) == 0x40))) ? 1 : -1];

#endif // INTERFACE_GUI_SELECTED_ITEMS_H
