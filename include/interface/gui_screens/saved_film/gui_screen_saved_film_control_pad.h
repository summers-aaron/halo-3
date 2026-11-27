#ifndef INTERFACE_GUI_SCREEN_SAVED_FILM_CONTROL_PAD_H
#define INTERFACE_GUI_SCREEN_SAVED_FILM_CONTROL_PAD_H

#include "interface/c_gui_screen_widget.h"

class c_saved_film_control_pad : public c_gui_screen_widget
{
public:
    __forceinline c_saved_film_control_pad(long name) : c_gui_screen_widget(name), m_previous_state(0) {}
    virtual ~c_saved_film_control_pad() {}
    virtual void post_initialize();
    void refresh_button_keys();
    virtual void update(unsigned long current_milliseconds);
    virtual bool handle_list_item_chosen(const c_controller_input_message *message, long list_name,
        c_gui_list_item_widget *list_item_widget, c_gui_data *datasource);
    virtual bool handle_controller_input_message(const c_controller_input_message *message);
    virtual bool handle_dialog_result(const c_dialog_result_message *message);
    long m_previous_state;
};
typedef char saved_film_control_pad_size[(sizeof(void *) != 4 || ((sizeof(c_saved_film_control_pad) == 0x1a90))) ? 1 : -1];

// Control and dialog string IDs used by the film control pad.
enum e_saved_film_control_pad_string_id
{
    _film_control_skip_back = 0x10560,
    _film_control_skip_forward = 0x10561,
    _film_control_save_primary = 0x10562,
    _film_control_save_secondary = 0x10563,
    _film_control_start_recording = 0x10565,
    _film_control_play = 0x10566,
    _film_control_pause = 0x10567,
    _film_control_set_start = 0x10568,
    _film_control_exit = 0x10569,
    _film_control_reset_speed = 0x1056c,
    _film_control_set_end = 0x1056d,
    _film_control_hilite = 0x105b5,
    _film_control_exit_dialog = 0x30017,
    _film_control_need_to_select_storage_device_to_save = 0x3001e,
    _film_control_upload_exit_dialog = 0x3002f,
};

#endif
