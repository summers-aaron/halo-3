#ifndef INTERFACE_USER_INTERFACE_TEXT_RENDER_H
#define INTERFACE_USER_INTERFACE_TEXT_RENDER_H

#include "interface/gui_widget.h"
struct s_user_interface_text_render_data : s_gui_widget_render_data
{
    long font;
    long style;
    long justification;
    long shadow_style;
    unsigned long color;
    unsigned long shadow_color;
    bool wrap_horizontally;
    bool align_vertically;
    short tab_count;
    short tabs[16];
    real_point2d rotation_origin;
    float rotation;
    float scale;
    real_rectangle2d bounds;
    real_rectangle2d clip_bounds;
};

struct s_gui_text_widget_small_render_data : s_user_interface_text_render_data
{
    wchar_t text[48];
};

struct s_gui_text_widget_large_render_data : s_user_interface_text_render_data
{
    wchar_t text[256];
};

struct s_gui_text_widget_extra_large_render_data : s_user_interface_text_render_data
{
    wchar_t text[1024];
};

typedef char user_interface_text_render_data_size[(sizeof(s_user_interface_text_render_data) == 0x98) ? 1 : -1];
typedef char gui_text_widget_small_render_data_text_offset[(offsetof(s_gui_text_widget_small_render_data, text) == 0x98) ? 1 : -1];
typedef char gui_text_widget_large_render_data_text_offset[(offsetof(s_gui_text_widget_large_render_data, text) == 0x98) ? 1 : -1];
typedef char gui_text_widget_extra_large_render_data_text_offset[(offsetof(s_gui_text_widget_extra_large_render_data, text) == 0x98) ? 1 : -1];

union rectangle2d;

extern "C" void user_interface_text_render(const s_user_interface_text_render_data *render_data, const rectangle2d *window_bounds);

#endif // INTERFACE_USER_INTERFACE_TEXT_RENDER_H
