#ifndef INTERFACE_GUI_SCREENS_BOOT_BETRAYER_GUI_SCREEN_BOOT_BETRAYER_H
#define INTERFACE_GUI_SCREENS_BOOT_BETRAYER_GUI_SCREEN_BOOT_BETRAYER_H

#include "interface/user_interface_messages.h"
#include "game/player_types.h"
#include "interface/c_gui_screen_widget.h"
#include <string.h>

class c_load_boot_betrayer_screen_message : public c_load_screen_message
{
public:
#if !defined(_XBOX) || defined(__clang__)
    c_load_boot_betrayer_screen_message(long screen_name, e_controller_index controller, e_window_index window,
        long layered_position, const s_player_identifier &current_player, const s_player_identifier &target_player)
        : c_load_screen_message(screen_name, controller, window, layered_position),
          m_current_player(current_player), m_target_player(target_player)
    {
    }
#endif
    virtual void apply_initial_state(c_gui_screen_widget *screen) const;
    __forceinline void set_current_player(const s_player_identifier &player) { m_current_player = player; }
    __forceinline void set_target_player(const s_player_identifier &player) { m_target_player = player; }
private:
    s_player_identifier m_current_player;
    s_player_identifier m_target_player;
};

class c_gui_screen_boot_betrayer : public c_gui_screen_widget {
    s_player_identifier m_current_player;
    s_player_identifier m_target_player;
    bool m_player_booted;
    void boot_player();
public:
    __forceinline c_gui_screen_boot_betrayer(long name) : c_gui_screen_widget(name)
    {
        memset(m_current_player.data, 0, sizeof(m_current_player.data));
        memset(m_target_player.data, 0, sizeof(m_target_player.data));
        m_player_booted = false;
    }
    virtual void initialize();
    virtual void dispose();
    virtual bool handle_controller_input_message(const c_controller_input_message *message);
    virtual bool handle_dialog_result(const c_dialog_result_message *dialog_result_message);
    __forceinline void set_current_player_id(const s_player_identifier *identifier) {
        m_current_player = *identifier;
    }
    __forceinline void set_target_player_id(const s_player_identifier *identifier) {
        m_target_player = *identifier;
    }
    __forceinline const s_player_identifier *get_target_player_id() {
        return &m_target_player;
    }
};

typedef char gui_screen_boot_betrayer_size[(sizeof(void *) != 4 || sizeof(c_gui_screen_boot_betrayer) == 0x1aa0) ? 1 : -1];

bool parse_griefer_name(void *this_ptr, wchar_t *buffer, long buffer_length);

extern "C" bool parse_lobby_privacy(void *context, wchar_t *output, long capacity);

extern "C" const wchar_t k_boot_betrayer_griefer_tag[];
#endif // INTERFACE_GUI_SCREENS_BOOT_BETRAYER_GUI_SCREEN_BOOT_BETRAYER_H
