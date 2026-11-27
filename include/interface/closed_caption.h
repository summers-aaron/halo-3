#ifndef INTERFACE_CLOSED_CAPTION_H
#define INTERFACE_CLOSED_CAPTION_H

#include "interface/closed_caption_types.h"

extern "C" { extern s_closed_caption_globals g_closed_caption_globals; }

class c_draw_string;
short closed_caption_get_line_height(c_draw_string *draw);
extern "C" wchar_t g_closed_caption_text_buffer[128];
extern "C" const wchar_t k_closed_caption_format[];
void closed_caption_render();
void closed_caption_initialize();
void closed_caption_dispose();
void closed_caption_initialize_for_new_map();
void closed_caption_dispose_from_old_map();
void closed_caption_scroll_one_entry();

#endif // INTERFACE_CLOSED_CAPTION_H
