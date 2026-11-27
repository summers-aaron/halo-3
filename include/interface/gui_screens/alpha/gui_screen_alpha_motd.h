#ifndef INTERFACE_GUI_SCREENS_ALPHA_GUI_SCREEN_ALPHA_MOTD_H
#define INTERFACE_GUI_SCREENS_ALPHA_GUI_SCREEN_ALPHA_MOTD_H

#include "interface/c_gui_screen_widget.h"

class c_gui_screen_alpha_motd : public c_gui_screen_widget
{
public:
    __forceinline c_gui_screen_alpha_motd(long name) : c_gui_screen_widget(name) {}
    virtual ~c_gui_screen_alpha_motd() {}
    virtual void post_initialize();
    virtual bool handle_controller_input_message(const c_controller_input_message *message);

private:
    char m_text[4096];
    long m_text_length;
};

typedef char gui_screen_alpha_motd_size[(sizeof(void *) != 4 || ((sizeof(c_gui_screen_alpha_motd) == 0x2a90))) ? 1 : -1];

class c_gui_screen_alpha_locked_down : public c_gui_screen_widget
{
public:
    __forceinline c_gui_screen_alpha_locked_down(long name) : c_gui_screen_widget(name), m_timeout_time(0x7fffffffL) {}
    virtual void post_initialize();
    virtual bool handle_controller_input_message(const c_controller_input_message *message);

private:
    long m_timeout_time;
};

typedef char gui_screen_alpha_locked_down_size[(sizeof(void *) != 4 || ((sizeof(c_gui_screen_alpha_locked_down) == 0x1a90))) ? 1 : -1];

class c_start_menu_game_editor;

// The save-variant prompt the game editor opens (screen 0x100ca); the load message hands it the editor and the
// save-as choice.
class c_gui_screen_alpha_motd_editor : public c_gui_screen_widget
{
public:
    __forceinline c_gui_screen_alpha_motd_editor(long name) : c_gui_screen_widget(name) {}
    virtual ~c_gui_screen_alpha_motd_editor() {}
    virtual void post_initialize() {}
    virtual bool handle_controller_input_message(const c_controller_input_message *message);

    c_start_menu_game_editor *m_editor;
    bool m_save_as;
};

typedef char gui_screen_alpha_motd_editor_size[(sizeof(void *) != 4 || ((sizeof(c_gui_screen_alpha_motd_editor) == 0x1a90))) ? 1 : -1];

#endif
