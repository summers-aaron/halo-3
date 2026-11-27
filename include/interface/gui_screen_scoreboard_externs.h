#ifndef INTERFACE_GUI_SCREEN_SCOREBOARD_EXTERNS_H
#define INTERFACE_GUI_SCREEN_SCOREBOARD_EXTERNS_H

extern "C" unsigned char game_is_scoreboard_blocked(long *reason);
extern "C" long user_interface_session_get_controller_index(long player_index);

class c_gui_screen_widget;
class c_gui_widget;


#endif // INTERFACE_GUI_SCREEN_SCOREBOARD_EXTERNS_H
