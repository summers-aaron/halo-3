#ifndef INTERFACE_GUI_SCREENS_SCOREBOARD_GUI_SCREEN_SCOREBOARD_H
#define INTERFACE_GUI_SCREENS_SCOREBOARD_GUI_SCREEN_SCOREBOARD_H

// Functions defined in tu/interfacelibxbox/gui_screen_scoreboard.cpp.

class c_message;

void user_interface_messaging_post(class c_message const * _sym_p0);

class c_gui_screen_scoreboard;

extern "C" bool gui_screen_scoreboard_is_interactive(c_gui_screen_scoreboard *screen);
extern "C" void gui_screen_scoreboard_save_text_alpha(int index);
extern "C" void gui_screen_scoreboard_update_coop_message(void);

#endif // INTERFACE_GUI_SCREENS_SCOREBOARD_GUI_SCREEN_SCOREBOARD_H
