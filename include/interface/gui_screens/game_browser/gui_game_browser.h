#ifndef INTERFACE_GUI_SCREENS_GAME_BROWSER_GUI_GAME_BROWSER_H
#define INTERFACE_GUI_SCREENS_GAME_BROWSER_GUI_GAME_BROWSER_H

#include "interface/c_gui_screen_widget.h"
#include "interface/user_interface_messages.h"

enum e_browser_type { _browser_type_default = 0 };

class c_gui_game_browser : public c_gui_screen_widget
{
public:
    __forceinline c_gui_game_browser(long name) :
        c_gui_screen_widget(name), m_squad_search_flags(0), m_browser_type(_browser_type_default)
    {
    }

    void show_list_item_details(e_controller_index controller);
    virtual void initialize();
    virtual void initialize_datasource();
    virtual void dispose();
    virtual void update_render_state(unsigned long current_milliseconds);
    virtual void update(unsigned long current_milliseconds);
    virtual bool handle_list_item_chosen(
        const c_controller_input_message *message, long name, c_gui_list_item_widget *item, c_gui_data *data);
    virtual bool handle_controller_input_message(const c_controller_input_message *message);

    __forceinline long get_squad_search_flags() const { return m_squad_search_flags; }
    __forceinline void set_squad_search_flags(long flags) { m_squad_search_flags = flags; }
    __forceinline void set_type(e_browser_type type) { m_browser_type = type; }
    __forceinline e_browser_type get_type() const { return m_browser_type; }

private:
    long m_squad_search_flags;
    e_browser_type m_browser_type;
};

typedef char gui_game_browser_size[(sizeof(void *) != 4 || sizeof(c_gui_game_browser) == 0x1a90) ? 1 : -1];

class c_load_game_browser_screen_message : public c_load_screen_message
{
public:
    c_load_game_browser_screen_message(long screen_name, e_controller_index controller, e_window_index window,
        long layered_position, long squad_search_flags, e_browser_type browser_type);
    virtual void apply_initial_state(c_gui_screen_widget *screen) const;

    long m_squad_search_flags;
    e_browser_type m_browser_type;
};

// Functions defined in tu/interfacelibxbox/gui_game_browser.cpp.

bool parse_game_browser_title(void *context, wchar_t *buffer, long buffer_length);

#endif // INTERFACE_GUI_SCREENS_GAME_BROWSER_GUI_GAME_BROWSER_H
