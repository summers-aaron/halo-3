#ifndef NETWORKING_ONLINE_ONLINE_GUIDE_H
#define NETWORKING_ONLINE_ONLINE_GUIDE_H

enum e_controller_index;
struct _XOVERLAPPED;
unsigned long online_guide_show_virtual_keyboard_ui(e_controller_index controller_index, unsigned long character_flags,
    const wchar_t *default_text, const wchar_t *title_text, const wchar_t *description_text, wchar_t *result_text,
    unsigned long maximum_character_count, _XOVERLAPPED *overlapped);

#endif // NETWORKING_ONLINE_ONLINE_GUIDE_H
