#ifndef INTERFACE_GUI_SCREENS_TERMINALS_GUI_SCREEN_TERMINALS_H
#define INTERFACE_GUI_SCREENS_TERMINALS_GUI_SCREEN_TERMINALS_H

// Functions defined in tu/interfacelibxbox/gui_screen_terminals.cpp.

#include <stddef.h>
#include "interface/c_gui_screen_widget.h"
#include "memory/static_arrays.h"
#include "game/players_global_data.h"

// Each difficulty occupies 0x38 bytes: three tag references surround the image selectors.
struct s_terminal_difficulty_definition
{
    s_tag_reference string_list_a;
    s_tag_reference string_list_b;
    unsigned short visible_images_a;
    unsigned short visible_images_b;
    unsigned short image_a;
    unsigned short image_b;
    s_tag_reference string_list_c;
};
typedef char terminal_difficulty_definition_size[(sizeof(s_terminal_difficulty_definition) == 0x38) ? 1 : -1];


class c_gui_screen_terminal : public c_gui_screen_widget {
public:
    __forceinline c_gui_screen_terminal(long name) : c_gui_screen_widget(name)
    {
        m_terminal_index = NONE;
        m_selected_index = 0;
        m_state = 1;
        m_state_ticks = 0;
        m_text_ticks = 0;
        m_initializing = false;
        m_elapsed_pending = false;
        m_page_complete = false;
        m_page_reset_sentinel = 0;
        m_countdown = 0;
        m_text_start_time = 0;
    }
    virtual ~c_gui_screen_terminal() {}
    virtual c_gui_bitmap_widget *create_bitmap_widget(const s_runtime_bitmap_widget_definition *definition);
    __forceinline void initialize(long terminal_index)
    {
        m_terminal_index = terminal_index;
        m_exit_requested = false;
        m_initializing = true;
    }
    float get_scroll_rate() const;
    virtual bool handle_controller_input_message(const c_controller_input_message *message);
    virtual void update(unsigned long current_milliseconds);

    long m_terminal_index;
    unsigned short m_selected_index;
    long m_state;
    short m_state_ticks;
    short m_text_ticks;
    unsigned char m_initializing;
    unsigned char m_elapsed_pending;
    short m_countdown;
    short m_page_index;
    // Reset to NONE when entering the reading state or advancing a page.
    short m_page_reset_sentinel;
    bool m_page_complete;
    bool m_exit_requested;
    unsigned long m_text_start_time;
    c_static_wchar_string<1024> m_text;
    c_static_wchar_string<1024> m_translated_text;
};
typedef char gui_screen_terminal_text_start_time_offset[(sizeof(void *) != 4 || ((offsetof(c_gui_screen_terminal, m_text_start_time) == 0x1aa4))) ? 1 : -1];
typedef char gui_screen_terminal_size[(sizeof(void *) != 4 || ((sizeof(c_gui_screen_terminal) == 0x2aa8))) ? 1 : -1];

extern "C" int gui_screen_terminal_set_selected_string(c_gui_screen_terminal *terminal, short selected_index);
extern "C" void gui_screen_terminal_update_paged_text(c_gui_screen_terminal *terminal, long elapsed);
extern "C" void gui_screen_terminal_update_translated_text(c_gui_screen_terminal *terminal, float progress);
extern "C" void gui_screen_terminal_set_state(c_gui_screen_terminal *terminal, long state);

// Data the terminal screen reads (defined in gui_screen_terminals.cpp).
extern "C" {
extern const char *k_terminal_map_image_names[];
extern float k_terminal_first_page_text_rate;
extern wchar_t k_terminal_scramble_glyphs[];
extern float k_terminal_translate_threshold_max;
extern float k_terminal_translate_threshold_min;
}
extern const long *screen_state_names[];

#include "game/players_globals_types.h"
#endif // INTERFACE_GUI_SCREENS_TERMINALS_GUI_SCREEN_TERMINALS_H
