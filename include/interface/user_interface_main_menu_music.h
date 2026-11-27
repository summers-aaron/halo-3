#ifndef INTERFACE_USER_INTERFACE_MAIN_MENU_MUSIC_H
#define INTERFACE_USER_INTERFACE_MAIN_MENU_MUSIC_H

// Main-menu music state stored in the user-interface globals.
class c_user_interface_main_menu_music
{
public:
    c_user_interface_main_menu_music() : m_looping_sound_index(-1) {}
    void update();
    __forceinline void reset()
    {
        m_current_state = 0;
        m_requested_state = 0;
        m_state_change_time = 0;
        m_game_shell_state = 0;
        m_looping_sound_index = -1;
    }

private:
    void change_state_update();
    void update_game_shell_music_state();

    long m_current_state;
    long m_requested_state;
    long m_game_shell_state;
    long m_state_change_time;
    long m_looping_sound_index;
    long m_fade_out_start_time;
};
typedef char user_interface_main_menu_music_size[(sizeof(c_user_interface_main_menu_music) == 0x18) ? 1 : -1];

// Functions defined in tu/interfacelibxbox/user_interface_main_menu_music.cpp that other units call.

extern "C" int user_interface_main_menu_music_get_delay(void);

extern "C" bool g_user_interface_main_menu_music_active;

#endif // INTERFACE_USER_INTERFACE_MAIN_MENU_MUSIC_H
