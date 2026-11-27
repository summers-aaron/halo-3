#ifndef INTERFACE_VIRTUAL_KEYBOARD_TASK_H
#define INTERFACE_VIRTUAL_KEYBOARD_TASK_H

#include "cseries/async_xoverlapped_types.h"
#include "shell/shell.h"

// Shared virtual keyboard task holding result and prompt text with input limits.
class c_virtual_keyboard_task : public c_overlapped_task
{
public:
    c_virtual_keyboard_task(const char *file, long line, e_controller_index controller_index,
        const wchar_t *default_text, const wchar_t *title_text, const wchar_t *description_text,
        unsigned long maximum_input_characters, unsigned long character_flags);
    virtual const char *get_context_string() const;
    virtual unsigned long start(void *overlapped);
    virtual void success(unsigned long status);
    static c_virtual_keyboard_task *get_instance(const char *file, long line,
        e_controller_index controller_index, const wchar_t *default_text,
        const wchar_t *title_text, const wchar_t *description_text,
        unsigned long maximum_input_characters, unsigned long character_flags);
    static void dispose_instance();
    void set_default_text(const wchar_t *text);
    void set_title_text(const wchar_t *text);
    void set_description_text(const wchar_t *text);
    __forceinline void set_controller_index(e_controller_index index) { m_controller_index = index; }
    __forceinline e_controller_index get_controller_index() const { return m_controller_index; }
    __forceinline const wchar_t *get_keyboard_results() const { return m_result_text; }
    __forceinline void set_character_flags(unsigned long flags) { m_character_flags = flags; }
    __forceinline void set_maximum_input_characters(unsigned long count) { m_maximum_input_characters = count; }

    e_controller_index m_controller_index;
    unsigned long m_character_flags;
    wchar_t m_result_text[256];
    wchar_t m_default_text[256];
    wchar_t m_title_text[64];
    wchar_t m_description_text[256];
    unsigned long m_maximum_input_characters;
};

typedef char virtual_keyboard_task_size[(sizeof(void *) != 4 || sizeof(c_virtual_keyboard_task) == 0x694) ? 1 : -1];

extern "C" { extern c_virtual_keyboard_task *g_virtual_keyboard_task_instance; }

// Source-file anchors used when starting the shared keyboard task.
extern "C" {
extern const char k_gui_start_menu_hq_screenshots_submenu_source_file[];
}

extern "C" char k_virtual_keyboard_task_source_file[];

#endif
