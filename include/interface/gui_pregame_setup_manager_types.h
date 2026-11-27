#ifndef INTERFACE_GUI_PREGAME_SETUP_MANAGER_TYPES_H
#define INTERFACE_GUI_PREGAME_SETUP_MANAGER_TYPES_H
#include "shell/shell.h"
#include <string.h>
#include "interface/gui_pregame_setup_manager.h"
#include "interface/gui_pregame_setup_types.h"
class c_dialog_result_message;
#include "interface/gui_selected_items.h"
#include "interface/gui_game_mode_types.h"
#include "interface/gui_network_session_advertisement_types.h"
#include "networking/session/network_session_class.h"
class c_gui_selected_item;
extern "C" void gui_pregame_setup_manager_apply_pending_setup(
    c_gui_pregame_setup_manager *manager, s_pregame_setup_params *setup);

class c_gui_pregame_setup_manager {
    friend void gui_pregame_setup_manager_apply_pending_setup(
        c_gui_pregame_setup_manager *manager, s_pregame_setup_params *setup);
    friend bool gui_pregame_setup_manager_initialize_squad_for_mode(long game_mode);
public:
    c_gui_pregame_setup_manager()
    {
        for (long i = 0; i < 8; i++)
            m_item_locked[i] = false;
    }
    __forceinline bool handle_dialog_result(const c_dialog_result_message *dialog_result_message);
    static bool get_selected_item_title(wchar_t *title, long title_length, e_gui_selected_item_type type);
    long get_selected_item_category_id(e_gui_selected_item_type type);
    void set_selected_item(e_controller_index controller_index, const c_gui_selected_item *selected_item, bool locked, long parent_screen_index);
public:
    __forceinline s_pregame_setup_params *get_pending_setup() { return &m_pending_setup; }
    __forceinline void set_pending_setup(const void *setup)
    {
        memcpy(&m_pending_setup, setup, sizeof(m_pending_setup));
        m_pending_setup_valid = false;
    }
private:
    void change_advertisement_mode(e_gui_network_session_advertisement_mode advertisement_mode, e_network_session_class session_class,
        e_network_session_class session_class2, long parent_screen_index);
    static void set_default_privacy(e_gui_game_mode game_mode, bool is_new_session);
    unsigned char m_item_locked[8];
    e_gui_network_session_advertisement_mode m_advertisement_mode;
    s_pregame_setup_params m_pending_setup;
    bool m_pending_setup_valid;
};
extern c_gui_pregame_setup_manager g_pregame_setup_manager;
#endif
