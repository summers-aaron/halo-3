#ifndef INTERFACE_GUI_SCREENS_START_MENU_GUI_SCREEN_START_MENU_H
#define INTERFACE_GUI_SCREENS_START_MENU_GUI_SCREEN_START_MENU_H

// The start menu screen (Bungie's interface\gui_screens\start_menu\gui_screen_start_menu.cpp; the March 2007 debug
// build).
// load_pane takes the outgoing pane's translation and rotation, and
// load_start_menu_internal takes a trailing bool. The constructor, reconstruct
// and the pane load at
// 0x498880, which hands the four leading fields to every pane (c_start_menu_pane_screen_widget::setup).

#include "interface/c_gui_screen_widget.h"
#if !defined(_XBOX) || defined(__clang__)
#include "interface/gui_render_data_size_types.h"
#endif
#include "interface/screen_transition_types.h"
#include "memory/static_arrays.h"
#include "networking/online/online_service_record.h"
#include "shell/shell_types.h"

extern "C" long start_menu_get_game_screen_name(void);

class c_controller_input_message;

struct s_start_menu_breadcrumb
{
    long screen;
    long index;

    __forceinline s_start_menu_breadcrumb() : screen(-1), index(-1) {}
    __forceinline s_start_menu_breadcrumb(long screen_name, long item_index) : screen(screen_name), index(item_index) {}
    __forceinline void reset() { screen = -1; index = -1; }
};

class c_start_menu_screen_widget : public c_gui_screen_widget {
public:
    c_start_menu_screen_widget(long name);
    virtual ~c_start_menu_screen_widget() {}
    virtual void initialize(); // slot 2
    virtual void post_initialize(); // slot 3
    virtual void dispose(); // slot 4
    virtual void update(unsigned long current_milliseconds); // slot 23
    virtual void update_render_state(unsigned long current_milliseconds); // slot 24
    virtual void reconstruct(); // slot 34
    virtual void initialize_datasource(); // slot 35
    virtual bool process_message(const c_message *message); // slot 40
    virtual bool handle_controller_input_message(const c_controller_input_message *message); // slot 43
    virtual void transition_out_with_transition_type(long transition_out, e_screen_transition_type transition_type); // slot 48
#if !defined(_XBOX) || defined(__clang__)
protected:
    // Shares the alert screens' body (0x4c0e90): alert-sized render data.
    virtual e_render_data_size get_render_data_size() { return _render_data_size_alert; }
public:
#endif
    // Both loaders take a trailing bool; every caller passes 1 in it (online_guide_xenon,
    // gui_screen_pregame_selection). The bodies at 0x498ef0 / 0x499030 do not read it; both pass
    // load_start_menu_internal a constant true in its new last bool.
    static bool load_start_menu_for_service_record(e_controller_index controller, const s_player_identifier *identifier,
        const unsigned __int64 *xuid, const s_service_record_identity *identity, bool immediate);
    static bool load_start_menu_for_file_share(e_controller_index controller, const s_player_identifier *identifier,
        const unsigned __int64 *xuid, const s_service_record_identity *identity, long slot_index, bool immediate);
    static bool handle_global_start_button_press(const c_controller_input_message *controller_input_message);
    void back_out_current_pane();
    void load_pane_direct(long name, e_screen_transition_type transition, long focus, long item, float translation, float rotation);
    void load_pane(long pane, bool clear, e_screen_transition_type transition, long focus, long item,
        float translation, float rotation);

    // Small helpers kept inline in their callers.
    __forceinline void set_target_player_id(const s_player_identifier *identifier) { m_target_player_identifier = *identifier; }
    __forceinline void set_target_player_xuid(const unsigned __int64 *xuid) { m_target_xuid = *xuid; }
    __forceinline void set_service_record_identity(const s_service_record_identity *identity) { m_service_record_identity = *identity; }
    __forceinline void add_breadcrumb(const s_start_menu_breadcrumb *breadcrumb)
    {
        if (m_breadcrumbs.count() < 7)
            m_breadcrumbs.push_back(*breadcrumb);
    }

private:
    static bool load_start_menu_internal(e_controller_index controller, const s_player_identifier *identifier,
        const unsigned __int64 *xuid, const s_service_record_identity *identity,
        const s_start_menu_breadcrumb *breadcrumbs, long breadcrumb_count, bool force_reload, bool immediate);
    void update_pane();
    void clean_up_old_panes();

public:
    // Units read some of these directly; Bungie's access for data members is not attested.
    s_player_identifier m_owner_player_identifier;          // pane setup()'s first argument
    s_player_identifier m_target_player_identifier;         // March: set_target_player_id
    unsigned __int64 m_target_xuid;                         // March: set_target_player_xuid
    s_service_record_identity m_service_record_identity;    // March: set_service_record_identity
    float m_saved_film_playback_speed;                      // use: saved by initialize, restored by dispose
    c_static_stack<s_start_menu_breadcrumb, 8> m_breadcrumbs;
    long m_pane;                                            // name of the current pane, -1 for none
    bool m_returning;                                       // use: set by the back handler with m_return_breadcrumb
    s_start_menu_breadcrumb m_return_breadcrumb;
    bool m_root_pane_shown;                                 // use: update_pane found no breadcrumb (sidebar pane)
    bool m_pause_game;                                      // use: update() pauses game time once, then clears it
};

typedef char start_menu_screen_widget_size[(sizeof(void *) != 4 || ((sizeof(c_start_menu_screen_widget) == 0x1b58) )) ? 1 : -1];

// Functions defined in tu/interfacelibxbox/gui_screen_start_menu.cpp.


#endif // INTERFACE_GUI_SCREENS_START_MENU_GUI_SCREEN_START_MENU_H
