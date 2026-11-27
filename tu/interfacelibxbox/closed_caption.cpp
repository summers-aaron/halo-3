// interfacelibxbox/closed_caption.obj: from the records 0x191480 0x191788 0x471240 0x471268
// @compiler xdk6130
// @flags /TP /O2 /Oc
#include "cseries/cseries_unicode.h"
#include "text/font_id_types.h"
#include "interface/user_interface_clock.h"
#include "text/font_cache_thread_state_types.h"
#include "text/character_data_index_types.h"
#include "math/real_math.h"
#include "interface/closed_caption.h"
#include "rasterizer/rasterizer_main.h"
#include "text/draw_string.h"
#include "rasterizer/rasterizer_video_mode_types.h"
#include "interface/chud/utf32_types.h"
#include "rasterizer/rasterizer.h"
#include "math/real_math_types.h"
#include "text/font_package_types.h"
#include "text/draw_string_types.h"
#include "text/font_cache_types.h"
#include "text/font_cache_lock.h"
#include "text/rasterizer_draw_string_types.h"
namespace {
static __forceinline void closed_caption_reset() {
    g_closed_caption_globals.font = 9;
    g_closed_caption_globals.justification = 0;
    memset(g_closed_caption_globals.captions, 0, sizeof(g_closed_caption_globals.captions));
}
}
// @address 0x191788 ?closed_caption_get_line_height@@YAFPAVc_draw_string@@@Z
short closed_caption_get_line_height(c_draw_string *draw) {
    const s_font_header *header = font_get_header(draw->m_font_id);
    int line_height = 10;
    if (header != 0)
        line_height = header->ascending_height + header->descending_height + header->leading_height;
    return (short)(draw->m_display_scale * draw->m_scale * (float)line_height);
}
// @address 0x471240 ?closed_caption_initialize@@YAXXZ
void closed_caption_initialize() {
    closed_caption_reset();
}
// @address 0x471240 ?closed_caption_dispose@@YAXXZ
void closed_caption_dispose() {
    closed_caption_reset();
}
// @address 0x471240 ?closed_caption_initialize_for_new_map@@YAXXZ
void closed_caption_initialize_for_new_map() {
    closed_caption_reset();
}
// @address 0x471240 ?closed_caption_dispose_from_old_map@@YAXXZ
void closed_caption_dispose_from_old_map() {
    closed_caption_reset();
}
// @address 0x471268 ?closed_caption_scroll_one_entry@@YAXXZ
void closed_caption_scroll_one_entry() {
    memmove(&g_closed_caption_globals.captions[0], &g_closed_caption_globals.captions[1], 4 * sizeof(s_closed_caption_entry));
    memset(&g_closed_caption_globals.captions[4], 0, sizeof(s_closed_caption_entry));
    for (unsigned int i = 0; i < 5; i++) {
        g_closed_caption_globals.captions[i].start_y = 0;
    }
}

// @address 0x10b0 k_font_cache_vtable

extern "C" {
// @address 0x119aea0 g_closed_caption_text_buffer
wchar_t g_closed_caption_text_buffer[128];
}
// @address 0x4dd38 k_closed_caption_format
// @rdata 0x8204dd38..0x8204dd6c
extern "C" {
const wchar_t k_closed_caption_format[] = {
    't', 'e', 's', 't', ' ', 'c', 'a', 'p', 't', 'i', 'o', 'n', ' ', 'l', 'i', 'n',
    'e', ' ', '[', ' ', '%', 'l', 'd', ' ', ']', 0
};
}
// @rdata-end

static __forceinline void closed_caption_get_bounds(rectangle2d *bounds) {
    XVIDEO_MODE *mode = rasterizer_main_get_video_mode();
    if (mode->fIsWideScreen)
        ::c_rasterizer::get_fullscreen_render_title_safe_pixel_bounds(bounds);
    else
    {
        if (rasterizer_main_get_video_mode()->fIsHiDef) { bounds->x0=32; bounds->y0=24; bounds->x1=608; bounds->y1=456; }
        else { bounds->x0=48; bounds->y0=36; bounds->x1=592; bounds->y1=444; }
    }
}
// @address 0x191480
void closed_caption_render()
{
    if (!(g_closed_caption_globals.flags & 1)) return;
    unsigned long now = g_user_interface_globals.milliseconds.peek();
    c_rasterizer_draw_string rasterizer_draw;
    c_draw_string &draw = rasterizer_draw;
    c_font_cache cache;
    bool overflow = false;
    real_argb_color color;
    color.red = 0.760784f; color.green = 0.929412f; color.blue = 1.0f;
    real_argb_color shadow;
    shadow.red = 0.0f; shadow.green = 0.0f; shadow.blue = 0.0f;
    draw.set_font((e_font_id)g_closed_caption_globals.font);
    draw.m_justification = (e_text_justification)g_closed_caption_globals.justification;
    draw.m_flags.set(c_draw_string::_text_flag_wrap_horizontally, true);
    long height = closed_caption_get_line_height(&draw);
    rectangle2d bounds;
    closed_caption_get_bounds(&bounds);
    long caption_height = (long)((float)(bounds.y1-bounds.y0)*0.2f+0.5f);
    long caption_width = (bounds.x1-bounds.x0)-(long)((float)(bounds.x1-bounds.x0)*0.8f+0.5f);
    bounds.y0=(short)(bounds.y1-caption_height);
    bounds.x0=(short)(bounds.x0+(caption_width>>1));
    bounds.x1=(short)(bounds.x1-(caption_width>>1));
    for (unsigned long i = 0; i < 5; ++i)
    {
        s_closed_caption_entry *entry = &g_closed_caption_globals.captions[i];
        if (entry->flags & 1)
        {
            if (overflow) { closed_caption_scroll_one_entry(); break; }
            usnzprintf(g_closed_caption_text_buffer, 128, k_closed_caption_format, entry->creation_time);
            unsigned long age = now - entry->creation_time;
            float alpha;
            if (age <= 3500) alpha = 1.0f;
            else alpha = 1.0f - (float)(age - 3500) * 0.00066666666f;
            color.alpha = alpha; shadow.alpha = alpha;
            draw.set_color_deprecated(&color, true);
            draw.set_shadow_color(&shadow);
            if (entry->start_y > 0) bounds.y0 = entry->start_y;
            else entry->start_y = bounds.y0;
            draw.set_bounds(&bounds);
            draw.c_draw_string::draw(&cache, g_closed_caption_text_buffer);
            point2d cursor;
            cursor.x = (short)draw.m_cursor.x;
            cursor.y = (short)draw.m_cursor.y;
            cursor.y += 2;
            if (cursor.y + height >= bounds.y1) overflow = true;
            bounds.y0 = cursor.y;
        }
    }
}
