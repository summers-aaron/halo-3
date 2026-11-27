// port/probe/extra (port/GAME-CHANGES.md, "In-game Xbox Guide", "Game invites and lobby codes"): the Guide (the
// console's Xbox Guide) and its toasts (achievements unlocked, game invites) drawn inside the game's frame with
// Halo 3's own UI renderer. PC-only game code, compiled like a game unit; the console build has no such unit (on the
// console the system draws the guide and its notifications itself).
//
// c_rasterizer::end_frame calls guide_ingame_ui_render once per presented frame, after the title's frame is finished
// and before the display surface is resolved to the front buffer: the place the console's system UI is composited over
// a title's frame. Whatever the title drew (gameplay, the HUD, the main menu and lobbies, cinematics, a loading screen)
// is underneath. The guide's state is port/sdk/xam/guide.c's; the view it shows comes through guide_ingame.h, and the
// regions drawn go back there for the game window's mouse.
//
// Everything is drawn the way the game draws its own UI, with the game's functions:
//   - the frosted glass under the guide's card: c_gui_bitmap_widget_render with the widget's blur flag (render data
//     flag 4), i.e. c_screen_postprocess::blur_display (the display resolved, downsampled into surface 51 and blurred
//     with the UI's blur_x / blur_y from the user interface shared globals) drawn back opaque, as the game's blurring
//     widgets draw it (surface 51 carries no alpha), then tinted by the card's fill;
//   - flat and gradient rectangles, hairlines, the selection bar: rasterizer_draw_dynamic_screen_quad with the
//     rasterizer globals' default texture 0 (color_white) and per-vertex colours, the path of draw_quad /
//     rasterizer_quad_screenspace (letterboxes, fades);
//   - text: user_interface_text_render, the text widgets' renderer (c_rasterizer_draw_string over the game's font
//     cache, with its drop shadow), in the game's own fonts from font_package.bin: conduit-32 titles (font 2),
//     conduit-23 headings (4), conduit-18 items and hints (6) and conduit-16 small text (1); the controller buttons are
//     the font's glyphs U+E100 (A) and U+E101 (B), as the game's own "(A) Select (B) Back" footers. Text is measured
//     with c_user_interface_draw_string, the UI's measuring draw string.
//     HD Text in Settings (or HALO3_HD_TEXT=1) substitutes FreeType glyphs in the UI renderer.
//     HALO3_GUIDE_HD_TEXT=0 or 1 overrides that choice for the Guide.
//     HALO3_GUIDE_HD_TEXT=sdf (or 2) uses distance fields from the disc's conduit-32 glyphs.
//     Both modes use a native-pixel atlas for lettering and retain the game's controller glyphs.
// The colours are the game's UI palette (the user interface globals' text colours: white D8D8D8, hilite D1DAEF, ice
// 698CBC, dim AFBCEB) with Halo 3's amber selection bar.
//
// State: the guide is drawn after c_screen_postprocess::setup_rasterizer_for_postprocess (the game's own reset to a
// 2D state) on the display surface, with the full-frame viewport and scissor; afterwards the render targets, the
// viewport and scissor (as the game records them, g_rasterizer_last_rectangles; the scissor test on or off as it was),
// the z-buffer, stencil and alpha blend modes and the current player view are put back (through the rasterizer's own
// setters, so its state caches stay true) and the samplers are cleared. Other render states are left as the game's 2D
// reset and the guide's draws set them, as after the game's own postprocess; the resolve and present come next. Nothing
// is drawn, and nothing changes, while the guide is closed.
#include <xtl.h>
#include <xgraphics.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "cseries/cseries.h"
#include "math/real_math_types.h"
#include "rasterizer/rasterizer.h"
#include "rasterizer/rasterizer_render_globals_types.h"
#include "rasterizer/rasterizer_vertex_screen_types.h"
#include "rasterizer/rasterizer_global_definitions.h"
#include "rasterizer/rasterizer_viewport_state.h"
#include "rasterizer/rasterizer_states.h"
#include "rasterizer/rasterizer_device_states.h"
#include "rasterizer/rasterizer_stencil.h"
#include "rasterizer/rasterizer_state_globals.h"
#include "rasterizer/dx9/rasterizer_dynamic_screen_geometry_types.h"
#include "rasterizer/dx9/rasterizer_dynamic_screen_calls.h"
#include "render/screen_postprocess.h"
#include "render/views/player_view_types.h"
#include "interface/c_gui_bitmap_widget.h"
#include "interface/user_interface_text_render.h"
#include "text/draw_string.h"
#include "text/draw_string_types.h"
#include "text/font_cache_types.h"
#include "text/font_character_types.h"
#include "text/font_package_types.h"
#include "game/game_globals.h"
#include "tag_files/tag_groups.h"
#include "cache/cache_file_tag_resource_runtime_interface.h"
#include "cache/tag_resource_cache_thread_lock.h"
#include "multithreading/thread_tls.h"
#include "d3d/d3dx_decode.h"
#include "runtime/hd_font.h"
#include "runtime/port_config.h"
#include "hd_text_atlas.h"
#include "xam/guide_ingame.h"

extern "C" void port_write_error(const char *text);

extern "C" char *getenv(const char *name);

namespace {

// ---- the look ----

// The layout is authored for the game's 1152 x 640 frame (k_authored_ui_dimensions[0]) and scaled to another.
const float k_authored_width = 1152.0f;
const float k_authored_height = 640.0f;

// font_package.bin's font ids (font_get_header): only conduit-16 and conduit-18 have the controller button glyphs.
const long k_font_title = 2;   // conduit-32
const long k_font_heading = 4; // conduit-23
const long k_font_body = 6;    // conduit-18
const long k_font_small = 1;   // conduit-16

const wchar_t k_glyph_a = 0xe100; // _ui_character_a_button
const wchar_t k_glyph_b = 0xe101; // _ui_character_b_button
const wchar_t k_glyph_lb = 0xe104; // _ui_character_left_bumper
const wchar_t k_glyph_rb = 0xe105; // _ui_character_right_bumper

// Colours, 0xAARRGGBB. Text takes the user interface globals' text colours (wigl text_colors) as the game's text
// widgets do, in the UI's own colour space: its dialogs title in "hilite", write their text and unselected items in
// "ice" and the selected item in "white".
const unsigned long k_white = 0xffd8d8d8;       // "white"
const unsigned long k_hilite = 0xffd1daef;      // "hilite"
const unsigned long k_ice = 0xff698cbc;         // "ice"
const unsigned long k_dim = 0x99afbceb;         // "dim" (0x66afbceb), a little stronger for small text
const unsigned long k_disabled = 0x66afbceb;    // "dim"
const unsigned long k_text_shadow = 0x80000000; // the text widgets' shadow
// Flat colours are given as they show on the display and taken into the UI's colour space by ui_fill_color: the
// display surface holds linear 7e3 colour (the UI's bitmaps are degammaed as they are sampled; a vertex colour is not)
// and the display's gamma ramp lifts it, about out = in^0.585, measured on the game's own dialog text colours.
const unsigned long k_backdrop_top = 0xa8000000;
const unsigned long k_backdrop_bottom = 0xc4000000;
const unsigned long k_card_top = 0xcc142034;    // over the frosted glass
const unsigned long k_card_bottom = 0xe60a101c;
const unsigned long k_card_border = 0x736e8cbc;
const unsigned long k_header_top = 0x4d3c608c;
const unsigned long k_header_bottom = 0x0c3c608c;
const unsigned long k_rule = 0x59afbceb;
const unsigned long k_footer = 0x66000000;
const unsigned long k_pane = 0x330a1220;
const unsigned long k_pane_focused = 0x59203a5c;
const unsigned long k_pane_border = 0x40698cbc;
const unsigned long k_amber = 0xffe38b4f;       // the caret, the focused pane, earned marks, the header's accent
const unsigned long k_bar_unfocused = 0x59698cbc;
// Halo 3's selection bar, measured on the main menu's own frames (a list item's focused bar, 308 x 25 of 1152 x 640):
// flat for its first 2%, then darkening to the right, its last 5% flat again; the colours (at its top and bottom 3
// rows; the rows between are 3.5% darker) at the peak of its pulse and at the trough, as they show on the display.
// The pulse (the list item's focus animation): a cosine of 2000 ms of the UI's clock, at its peak as an item takes
// the focus; the item that loses it flashes back to the peak and fades out in 128 ms (linearly, in linear light).
struct s_bar_stop
{
    float at;
    unsigned long peak, trough;
};
const s_bar_stop k_bar_stops[] = {
    { 0.00f, 0xff7a4d2b, 0xff5c3d30 }, // (122, 77, 43)  (92, 61, 48)
    { 0.02f, 0xff7a4d2b, 0xff5c3d30 },
    { 0.23f, 0xff73492c, 0xff573a30 }, // (115, 73, 44)  (87, 58, 48)
    { 0.40f, 0xff70472b, 0xff55382f }, // (112, 71, 43)  (85, 56, 47)
    { 0.56f, 0xff6b452c, 0xff51372f }, // (107, 69, 44)  (81, 55, 47)
    { 0.72f, 0xff623f2d, 0xff4a3330 }, // ( 98, 63, 45)  (74, 51, 48)
    { 0.85f, 0xff513730, 0xff3d2d31 }, // ( 81, 55, 48)  (61, 45, 49)
    { 0.95f, 0xff433032, 0xff342933 }, // ( 67, 48, 50)  (52, 41, 51)
    { 1.00f, 0xff433032, 0xff342933 },
};
const float k_bar_edge_rows = 3.0f / 25.0f; // the brighter top and bottom rows, of the bar's height
const float k_bar_middle_level = 0.965f;
const unsigned long k_pulse_milliseconds = 2000;
const unsigned long k_unfocus_milliseconds = 128;
const unsigned long k_mark = 0xff98b3d5;        // a list's "more" marks: ice as the text shows it
const unsigned long k_field = 0xbf040810;
const unsigned long k_field_border = 0x99698cbc;

const unsigned long k_fade_in_milliseconds = 140;
const unsigned long k_fade_out_milliseconds = 110;
const unsigned long k_caret_blink_milliseconds = 530;

// Layout, in authored pixels.
const float k_card_width = 600.0f;           // one pane
const float k_card_margin = 40.0f;           // several panes: from the frame's sides
const float k_header_height = 66.0f;
const float k_footer_height = 44.0f;
const float k_padding = 22.0f;
const float k_pane_gap = 10.0f;
const float k_pane_padding = 12.0f;
const float k_row_height = 30.0f;
const float k_line_small = 21.0f;            // conduit-16's line
const float k_line_body = 24.0f;             // conduit-18's line
const float k_tabs_height = 30.0f;           // LB / RB's blades, under the header
const float k_slide = 10.0f;                 // the card rises this far as it fades in
const long k_single_rows = 9;                // rows a one-pane card shows at most
const long k_hint_lines = 3;

// ---- the frame being drawn ----

struct s_frame
{
    float width, height;          // the frame (render_globals), in its pixels
    float scale, x, y;            // authored -> frame pixels: x + a * scale
    float text_scale;             // the draw strings' scale for that: scale over their own display glyph scale
                                  // (draw_string: the UI's authored -> render scale at a custom resolution)
    bool sdf_text;
    float alpha;                  // the fade
    unsigned long now;            // GetTickCount
    c_rasterizer_texture_ref white;
    guide_ingame_layout *layout;
};

static __forceinline float fx(const s_frame *frame, float a) { return frame->x + a * frame->scale; }
static __forceinline float fy(const s_frame *frame, float a) { return frame->y + a * frame->scale; }

// A flat colour as it is to show on the display, in the UI's colour space (see the colours above).
static unsigned long ui_fill_color(unsigned long color)
{
    static unsigned char table[256];
    static bool built = false;
    if (!built) {
        for (long i = 0; i < 256; i++)
            table[i] = (unsigned char)(255.0 * pow((double)i / 255.0, 1.0 / 0.585) + 0.5);
        built = true;
    }
    return (color & 0xff000000) | ((unsigned long)table[(color >> 16) & 0xff] << 16) |
           ((unsigned long)table[(color >> 8) & 0xff] << 8) | table[color & 0xff];
}

static unsigned long faded(const s_frame *frame, unsigned long color)
{
    unsigned long alpha = (unsigned long)((float)(color >> 24) * frame->alpha + 0.5f);
    if (alpha > 255)
        alpha = 255;
    return (alpha << 24) | (color & 0x00ffffff);
}

static void add_hit(const s_frame *frame, float x0, float y0, float x1, float y1, long target, long pane, long item)
{
    guide_ingame_layout *layout = frame->layout;
    if (layout->hit_count >= GUIDE_INGAME_HITS)
        return;
    guide_ingame_hit *hit = &layout->hits[layout->hit_count++];
    hit->x0 = fx(frame, x0);
    hit->y0 = fy(frame, y0);
    hit->x1 = fx(frame, x1);
    hit->y1 = fy(frame, y1);
    hit->target = target;
    hit->pane = pane;
    hit->item = item;
}

// ---- drawing with the game's UI functions ----

// The game's dynamic screen quad, alpha blended: four screen vertices (display pixels), each with its fill colour
// (display sRGB, faded and converted here). The parameters start zeroed, as the
// game's own callers start them (rasterizer_quad_screenspace_explicit, c_gui_bitmap_widget_render).
static void screen_quad(const s_frame *frame, const c_rasterizer_texture_ref &texture, const float (&xs)[4],
    const float (&ys)[4], const float (&us)[4], const float (&vs)[4], const unsigned long (&colors)[4],
    bool native_text = false)
{
    if (!texture.valid())
        return;
    rasterizer_dynamic_screen_geometry_parameters parameters;
    memset(static_cast<void *>(&parameters), 0, sizeof(parameters));
    parameters.scale0.i = parameters.scale0.j = 1.0f;
    parameters.scale1.i = parameters.scale1.j = 1.0f;
    parameters.textures[0] = texture;
    parameters.explicit_shader_index = NONE;
    parameters.alpha_blend_mode = c_rasterizer::_alpha_blend_alpha_blend;
    parameters.point_sampled = native_text;
    rasterizer_vertex_screen vertices[4];
    for (long i = 0; i < 4; i++) {
        vertices[i].x = xs[i];
        vertices[i].y = ys[i];
        vertices[i].u = us[i];
        vertices[i].v = vs[i];
        // Text colors are already in the game's UI color space. Sample the native coverage once at 1:1.
        vertices[i].color = faded(frame, native_text ? colors[i] : ui_fill_color(colors[i]));
    }
    rasterizer_draw_dynamic_screen_quad(&parameters, vertices);
}

static void white_quad(const s_frame *frame, const float (&xs)[4], const float (&ys)[4], const float (&us)[4],
    const float (&vs)[4], const unsigned long (&colors)[4])
{
    screen_quad(frame, frame->white, xs, ys, us, vs, colors);
}

// A rectangle in authored pixels, its corners' colours top left, top right, bottom right, bottom left.
static void fill_corners(const s_frame *frame, float x0, float y0, float x1, float y1, unsigned long top_left,
    unsigned long top_right, unsigned long bottom_right, unsigned long bottom_left)
{
    if (x1 <= x0 || y1 <= y0)
        return;
    const float left = fx(frame, x0), right = fx(frame, x1), top = fy(frame, y0), bottom = fy(frame, y1);
    const float xs[4] = { left, right, right, left };
    const float ys[4] = { top, top, bottom, bottom };
    const float us[4] = { 0.0f, 1.0f, 1.0f, 0.0f };
    const float vs[4] = { 0.0f, 0.0f, 1.0f, 1.0f };
    const unsigned long colors[4] = { top_left, top_right, bottom_right, bottom_left };
    white_quad(frame, xs, ys, us, vs, colors);
}

static void fill(const s_frame *frame, float x0, float y0, float x1, float y1, unsigned long color)
{
    fill_corners(frame, x0, y0, x1, y1, color, color, color, color);
}

static void fill_vertical(const s_frame *frame, float x0, float y0, float x1, float y1, unsigned long top,
    unsigned long bottom)
{
    fill_corners(frame, x0, y0, x1, y1, top, top, bottom, bottom);
}

static void fill_horizontal(const s_frame *frame, float x0, float y0, float x1, float y1, unsigned long left,
    unsigned long right)
{
    fill_corners(frame, x0, y0, x1, y1, left, right, right, left);
}

// A 1-pixel frame around the rectangle (in frame pixels: a hairline at any scale).
static void outline(const s_frame *frame, float x0, float y0, float x1, float y1, unsigned long color)
{
    const float pixel = 1.0f / frame->scale;
    fill(frame, x0, y0, x1, y0 + pixel, color);
    fill(frame, x0, y1 - pixel, x1, y1, color);
    fill(frame, x0, y0 + pixel, x0 + pixel, y1 - pixel, color);
    fill(frame, x1 - pixel, y0 + pixel, x1, y1 - pixel, color);
}

static void rule(const s_frame *frame, float x0, float x1, float y, unsigned long color)
{
    fill_horizontal(frame, x0, y, (x0 + x1) * 0.5f, y + 1.0f / frame->scale, color & 0x00ffffff, color);
    fill_horizontal(frame, (x0 + x1) * 0.5f, y, x1, y + 1.0f / frame->scale, color, color & 0x00ffffff);
}

// A triangle pointing up or down (a quad with two corners at its tip): a list's "more" marks.
static void arrow(const s_frame *frame, float x, float y, float size, bool up, unsigned long color)
{
    const float half = size * 0.5f;
    const float base = up ? y + half : y - half, tip = up ? y - half : y + half;
    const float xs[4] = { fx(frame, x - size), fx(frame, x + size), fx(frame, x), fx(frame, x) };
    const float ys[4] = { fy(frame, base), fy(frame, base), fy(frame, tip), fy(frame, tip) };
    const float uvs[4] = { 0.5f, 0.5f, 0.5f, 0.5f };
    const unsigned long colors[4] = { color, color, color, color };
    white_quad(frame, xs, ys, uvs, uvs, colors);
}

// ---- text ----

// The guide's UTF-8 as the game's UTF-16 text: '|' escaped (the draw string's escape character), '\n' as '\r' (its
// line break), what UTF-16 cannot hold or the fonts do not have as '?'. Upper case for titles.
static void to_wide(const char *text, wchar_t *out, long capacity, bool upper)
{
    long length = 0;
    const unsigned char *c = (const unsigned char *)text;
    while (*c != 0 && length < capacity - 2) {
        unsigned long code = *c++;
        if (code >= 0x80) {
            long extra = code >= 0xf0 ? 3 : code >= 0xe0 ? 2 : code >= 0xc0 ? 1 : 0;
            code &= extra == 3 ? 0x07 : extra == 2 ? 0x0f : 0x1f;
            if (extra == 0)
                code = '?';
            for (; extra > 0 && (*c & 0xc0) == 0x80; extra--)
                code = (code << 6) | (*c++ & 0x3f);
            if (extra != 0 || code > 0xffff || (code >= 0xd800 && code < 0xe000) || (code >= 0xe000 && code < 0xf900))
                code = '?'; // the private use area holds the game's glyphs: never from text
        }
        if (code == '\n')
            code = '\r';
        else if (code < 0x20 && code != '\r')
            code = ' ';
        if (upper && code >= 'a' && code <= 'z')
            code -= 'a' - 'A';
        if (code == '|')
            out[length++] = '|';
        out[length++] = (wchar_t)code;
    }
    out[length] = 0;
}

static long wide_length(const wchar_t *text)
{
    long length = 0;
    while (text[length] != 0)
        length++;
    return length;
}

static void wide_copy(wchar_t *out, long capacity, const wchar_t *text)
{
    long i = 0;
    for (; i < capacity - 1 && text[i] != 0; i++)
        out[i] = text[i];
    out[i] = 0;
}

// The text's size in authored pixels (with wrapping when `wrap_width` > 0), measured by the UI's measuring draw string.
static void measure_bitmap(const s_frame *frame, const wchar_t *text, long font, float wrap_width, float *width,
    float *height)
{
    *width = *height = 0.0f;
    if (text[0] == 0)
        return;
    c_user_interface_draw_string draw;
    c_font_cache cache;
    draw.set_font((e_font_id)font);
    draw.m_justification = _text_justification_left;
    draw.m_scale = frame->text_scale;
    draw.m_flags.set(c_draw_string::_text_flag_wrap_horizontally, wrap_width > 0.0f);
    real_rectangle2d bounds;
    bounds.x0 = 0.0f;
    bounds.y0 = 0.0f;
    bounds.x1 = wrap_width > 0.0f ? wrap_width * frame->scale : 8192.0f;
    bounds.y1 = 8192.0f;
    draw.m_bounds = bounds;
    draw.m_clip_bounds = bounds;
    rectangle2d measured;
    if (draw.compute_wchar_string_bounds(&cache, text, &measured, NULL) && measured.x1 >= measured.x0) {
        *width = (float)(measured.x1 - measured.x0) / frame->scale;
        *height = (float)(measured.y1 - measured.y0) / frame->scale;
    }
}

static bool layout_sdf_text(const s_frame *frame, const wchar_t *string, long font, float wrap_width,
    float *width, float *height, const real_rectangle2d *bounds, unsigned long color, long justification);

static void measure(const s_frame *frame, const wchar_t *text, long font, float wrap_width, float *width, float *height)
{
    if (frame->sdf_text && layout_sdf_text(frame, text, font, wrap_width * frame->scale, width, height, NULL, 0,
                                         _text_justification_left)) {
        *width /= frame->scale;
        *height /= frame->scale;
        return;
    }
    measure_bitmap(frame, text, font, wrap_width, width, height);
}

static float text_width(const s_frame *frame, const wchar_t *text, long font)
{
    float width, height;
    measure(frame, text, font, 0.0f, &width, &height);
    return width;
}

// `text` cut to `width` authored pixels with "..." when it does not fit (the longest prefix that does).
static void fit(const s_frame *frame, const wchar_t *text, long font, float width, wchar_t *out, long capacity)
{
    wide_copy(out, capacity, text);
    if (text_width(frame, out, font) <= width)
        return;
    long low = 0, high = wide_length(text);
    if (high > capacity - 4)
        high = capacity - 4;
    while (low < high) { // the longest prefix + "..." that fits
        const long middle = (low + high + 1) / 2;
        wide_copy(out, middle + 1, text);
        out[middle] = '.';
        out[middle + 1] = '.';
        out[middle + 2] = '.';
        out[middle + 3] = 0;
        if (text_width(frame, out, font) <= width)
            low = middle;
        else
            high = middle - 1;
    }
    while (low > 0 && text[low - 1] == ' ')
        low--;
    wide_copy(out, low + 1, text);
    out[low] = '.';
    out[low + 1] = '.';
    out[low + 2] = '.';
    out[low + 3] = 0;
}

enum e_guide_text_mode { k_guide_text_setting, k_guide_text_original, k_guide_text_hd, k_guide_text_sdf };

static e_guide_text_mode guide_text_mode()
{
    static const e_guide_text_mode mode = [] {
        const char *setting = getenv("HALO3_GUIDE_HD_TEXT");
        if (setting && !strcmp(setting, "0"))
            return k_guide_text_original;
        if (setting && !strcmp(setting, "1"))
            return k_guide_text_hd;
        if (setting && (!strcmp(setting, "sdf") || !strcmp(setting, "2")))
            return k_guide_text_sdf;
        return k_guide_text_setting;
    }();
    return mode;
}

static bool g_guide_text_scope;

// Text in the box (authored pixels), as a text widget draws it: user_interface_text_render.
static void text_bitmap(const s_frame *frame, const wchar_t *string, long font, float x0, float y0, float x1, float y1,
    unsigned long color, long justification, bool wrap)
{
    if (string[0] == 0)
        return;
    // the text widget render data with the 1024-character text (the upstream sync moved `text` out of the base type)
    static s_gui_text_widget_extra_large_render_data data; // 2 KB of text: not on the presenting thread's stack
    memset(&data, 0, sizeof(data));
    data.font = font;
    data.style = _text_style_none;
    data.justification = justification;
    data.shadow_style = 1; // a drop shadow
    data.color = faded(frame, color);
    const float shadow_alpha = (float)(k_text_shadow >> 24) * (float)(color >> 24) / 255.0f;
    data.shadow_color = faded(frame, (unsigned long)shadow_alpha << 24);
    data.wrap_horizontally = wrap;
    data.scale = frame->text_scale;
    data.bounds.x0 = fx(frame, x0);
    data.bounds.y0 = fy(frame, y0);
    data.bounds.x1 = fx(frame, x1);
    data.bounds.y1 = fy(frame, y1);
    data.clip_bounds = data.bounds;
    wide_copy(data.text, sizeof(data.text) / sizeof(data.text[0]), string);
    rectangle2d viewport;
    viewport.x0 = 0;
    viewport.y0 = 0;
    viewport.x1 = (short)frame->width;
    viewport.y1 = (short)frame->height;
    g_guide_text_scope = guide_text_mode() != k_guide_text_setting;
    user_interface_text_render(&data, &viewport);
    g_guide_text_scope = false;
}

const int k_hd_atlas_width = 2048;
const int k_hd_atlas_height = 2048;
const int k_hd_atlas_entries = 2048;
const int k_hd_max_pixel_height = 192;

static const s_font_header *sdf_font_header()
{
    const s_font_header *header = font_get_header(k_font_title);
    return header && !strcmp(header->name, "conduit-32") ? header : NULL;
}

const float k_sdf_nominal_height = 32.0f; // Conduit-32's size label. Its ascent and descent total 45 pixels.
const int k_sdf_subpixels = 4;
const int k_sdf_padding = 2;
const int k_sdf_source_width = 256, k_sdf_source_height = 64; // font cache limits
const int k_sdf_grid_width = (k_sdf_source_width + 2 * k_sdf_padding) * k_sdf_subpixels;
const int k_sdf_grid_height = (k_sdf_source_height + 2 * k_sdf_padding) * k_sdf_subpixels;
const float k_sdf_far = 1.0e9f;

static float clamp(float value, float low, float high);

static float bilinear(float a, float b, float c, float d, float x, float y)
{
    const float top = a + (b - a) * x, bottom = c + (d - c) * x;
    return top + (bottom - top) * y;
}

static float sdf_source_alpha(const unsigned short *pixels, int width, int height, int x, int y)
{
    return x >= 0 && x < width && y >= 0 && y < height ? (pixels[y * width + x] >> 12) / 15.0f : 0.0f;
}

// Find the nearest 0.5-alpha crossing along a grid line. Interpolating between coverage samples
// places each site at a fractional coordinate even on the 4x grid.
static void sdf_seed_line(const float *alpha, float *distance, int count, int stride)
{
    for (int step = 1; step >= -1; step -= 2) {
        float site = -k_sdf_far;
        for (int i = step > 0 ? 0 : count - 1; i >= 0 && i < count; i += step) {
            const int j = i - step;
            const float a = alpha[i * stride];
            if (j >= 0 && j < count) {
                const float b = alpha[j * stride];
                if ((a >= 0.5f) != (b >= 0.5f))
                    site = j + step * (0.5f - b) / (a - b);
            }
            const float delta = i - site;
            const float squared = site == -k_sdf_far ? k_sdf_far : delta * delta;
            float &value = distance[i * stride];
            if (step > 0 || squared < value)
                value = squared;
        }
    }
}

// Felzenszwalb-Huttenlocher squared EDT. Each input supplies the squared distance along the first
// axis, and the lower envelope adds the squared distance along the perpendicular axis.
static void sdf_transform_line(float *distance, int count, int stride)
{
    float cost[k_sdf_grid_width], boundary[k_sdf_grid_width];
    int site[k_sdf_grid_width];
    int last = -1;
    for (int i = 0; i < count; i++) {
        const float value = distance[i * stride];
        if (value == k_sdf_far)
            continue;
        float cut = -k_sdf_far;
        while (last >= 0) {
            const int previous = site[last];
            cut = (value - cost[last]) / (2 * (i - previous)) + (i + previous) * 0.5f;
            if (cut > boundary[last])
                break;
            last--;
        }
        last++;
        site[last] = i;
        cost[last] = value;
        boundary[last] = last == 0 ? -k_sdf_far : cut;
    }
    if (last < 0)
        return;
    for (int i = 0, nearest = 0; i < count; i++) {
        while (nearest < last && boundary[nearest + 1] < i)
            nearest++;
        const int delta = i - site[nearest];
        distance[i * stride] = cost[nearest] + delta * delta;
    }
}

// Smooth out the disc bitmap's quantized diagonals over one source pixel. Filtering the field
// before rasterization keeps the final antialiasing width independent of the output size.
static void sdf_smooth_line(const float *source, float *dest, int count, int stride)
{
    for (int i = 0; i < count; i++) {
        float total = 0.0f;
        for (int k = -k_sdf_subpixels; k <= k_sdf_subpixels; k++) {
            const int sample = i + k < 0 ? 0 : i + k >= count ? count - 1 : i + k;
            const int weight = k_sdf_subpixels + 1 - abs(k);
            total += source[sample * stride] * weight;
        }
        dest[i * stride] = total / ((k_sdf_subpixels + 1) * (k_sdf_subpixels + 1));
    }
}

static bool sdf_glyph(uint32_t pixel_height, uint32_t codepoint, s_port_hd_glyph *glyph,
    float *advance, uint8_t *coverage, size_t capacity)
{
    static unsigned short pixels[k_sdf_source_width * k_sdf_source_height];
    int width, height, origin_x, origin_y;
    const float scale = pixel_height / k_sdf_nominal_height;
    {
        // The cache owns the packed bytes. Decode into scratch space while its lock is held.
        c_font_cache cache;
        const utf32 character = { (long)codepoint };
        const e_font_id font = (e_font_id)k_font_title;
        const s_font_character *source = NULL;
        if (!cache.get_character(font, character, true, NULL, NULL, &source) || !source)
            return false;
        width = source->width;
        height = source->height;
        if (width <= 0 || width > k_sdf_source_width || height <= 0 || height > k_sdf_source_height ||
            !cache.get_pixels(font, character, source, pixels))
            return false;
        origin_x = source->bearing;
        origin_y = source->top;
        // The disc's stored advance starts at the bitmap origin.
        *advance = (source->advance + origin_x) * scale;
    }
    *glyph = {};
    int left = width, top = height, right = 0, bottom = 0;
    for (int y = 0; y < height; y++)
        for (int x = 0; x < width; x++) {
            if (!(pixels[y * width + x] >> 12))
                continue;
            if (x < left) left = x;
            if (y < top) top = y;
            if (x >= right) right = x + 1;
            if (y >= bottom) bottom = y + 1;
        }
    if (left >= right)
        return true; // A space has an advance and no atlas tile.
    left -= k_sdf_padding;
    top -= k_sdf_padding;
    right += k_sdf_padding;
    bottom += k_sdf_padding;

    const int grid_width = (right - left) * k_sdf_subpixels;
    const int grid_height = (bottom - top) * k_sdf_subpixels;
    static float alpha[k_sdf_grid_width * k_sdf_grid_height];
    static float horizontal[k_sdf_grid_width * k_sdf_grid_height];
    static float vertical[k_sdf_grid_width * k_sdf_grid_height];
    for (int y = 0; y < grid_height; y++) {
        const float sy = top + (y + 0.5f) / k_sdf_subpixels - 0.5f;
        const int iy = (int)floorf(sy);
        for (int x = 0; x < grid_width; x++) {
            const float sx = left + (x + 0.5f) / k_sdf_subpixels - 0.5f;
            const int ix = (int)floorf(sx);
            alpha[y * grid_width + x] = bilinear(sdf_source_alpha(pixels, width, height, ix, iy),
                sdf_source_alpha(pixels, width, height, ix + 1, iy),
                sdf_source_alpha(pixels, width, height, ix, iy + 1),
                sdf_source_alpha(pixels, width, height, ix + 1, iy + 1), sx - ix, sy - iy);
        }
    }
    for (int y = 0; y < grid_height; y++)
        sdf_seed_line(alpha + y * grid_width, horizontal + y * grid_width, grid_width, 1);
    for (int x = 0; x < grid_width; x++)
        sdf_seed_line(alpha + x, vertical + x, grid_height, grid_width);
    for (int x = 0; x < grid_width; x++)
        sdf_transform_line(horizontal + x, grid_height, grid_width);
    for (int y = 0; y < grid_height; y++)
        sdf_transform_line(vertical + y * grid_width, grid_width, 1);
    const bool has_contour = horizontal[0] < k_sdf_far || vertical[0] < k_sdf_far;
    const float screen_distance = scale / k_sdf_subpixels;
    if (has_contour) {
        for (int i = 0; i < grid_width * grid_height; i++) {
            const float squared = horizontal[i] < vertical[i] ? horizontal[i] : vertical[i];
            const float distance = sqrtf(squared) * screen_distance;
            alpha[i] = alpha[i] >= 0.5f ? distance : -distance;
        }
        for (int y = 0; y < grid_height; y++)
            sdf_smooth_line(alpha + y * grid_width, horizontal + y * grid_width, grid_width, 1);
        for (int x = 0; x < grid_width; x++)
            sdf_smooth_line(horizontal + x, alpha + x, grid_height, grid_width);
    }

    const int x0 = (int)floorf((left + origin_x) * scale), x1 = (int)ceilf((right + origin_x) * scale);
    const int y0 = (int)floorf((top - origin_y) * scale), y1 = (int)ceilf((bottom - origin_y) * scale);
    glyph->width = x1 - x0;
    glyph->height = y1 - y0;
    glyph->bearing_x = x0;
    glyph->bearing_y = -y0;
    if ((size_t)glyph->width * glyph->height > capacity)
        return false;
    for (int y = 0; y < glyph->height; y++) {
        const float sy = clamp((y0 + y + 0.5f) / screen_distance + (origin_y - top) * k_sdf_subpixels - 0.5f,
            0.0f, (float)(grid_height - 2));
        const int iy = (int)sy;
        for (int x = 0; x < glyph->width; x++) {
            const float sx = clamp((x0 + x + 0.5f) / screen_distance - (left + origin_x) * k_sdf_subpixels - 0.5f,
                0.0f, (float)(grid_width - 2));
            const int ix = (int)sx;
            const float *cell = alpha + iy * grid_width + ix;
            const float sample = bilinear(cell[0], cell[1], cell[grid_width], cell[grid_width + 1], sx - ix, sy - iy);
            // Offset by a fraction of a source pixel to retain thin strokes after smoothing.
            const float opacity = has_contour ? clamp(0.5f + (sample + 0.10f * scale) / 1.2f, 0.0f, 1.0f) : sample;
            coverage[y * glyph->width + x] = (uint8_t)(opacity * 255.0f + 0.5f);
        }
    }
    return true;
}

struct s_hd_atlas_glyph
{
    uint32_t key;
    s_port_hd_glyph glyph;
    float advance;
    int x, y;
    bool missing;
};

struct s_hd_atlas
{
    IDirect3DDevice9 *device;
    IDirect3DTexture9 *texture;
    int x, y, row_height;
    bool full;
    bool reported_failure;
    unsigned long failed_at; // GetTickCount of the last failed allocation; 0 when none
    s_hd_atlas_glyph entries[k_hd_atlas_entries];
};

static s_hd_atlas g_hd_atlas, g_sdf_atlas;

// A failed atlas allocation waits this long before the next try. Texture memory has to sit below 4 GB (the fetch
// constants hold 32-bit addresses), and finding no 16 MB gap there walks the whole range with VirtualQuery: retried
// for every glyph, that cost whole frames.
const unsigned long k_hd_atlas_retry_milliseconds = 10000;

static bool create_hd_atlas_texture(s_hd_atlas &atlas, const char *name)
{
    if (atlas.texture)
        return true;
    const unsigned long now = GetTickCount();
    if (atlas.failed_at != 0 && now - atlas.failed_at < k_hd_atlas_retry_milliseconds)
        return false;
    atlas.texture = static_cast<IDirect3DTexture9 *>(D3DDevice_CreateTexture(k_hd_atlas_width,
        k_hd_atlas_height, 1, 1, 0, D3DFMT_A8R8G8B8, D3DPOOL_MANAGED, D3DRTYPE_TEXTURE));
    if (atlas.texture) {
        atlas.failed_at = 0;
        return true;
    }
    atlas.failed_at = now ? now : 1;
    if (!atlas.reported_failure) {
        atlas.reported_failure = true;
        port_write_error(name[0] == 'S'
            ? "port: HD text: no room below 4 GB for the 16 MB SDF glyph atlas; the Guide uses the game's font\n"
            : "port: HD text: no room below 4 GB for the 16 MB HD glyph atlas; menus use the game's font\n");
    }
    return false;
}

static bool prepare_hd_atlas(s_hd_atlas &atlas)
{
    IDirect3DDevice9 *device = c_rasterizer::get_device();
    if (!create_hd_atlas_texture(atlas, &atlas == &g_sdf_atlas ? "SDF" : "HD"))
        return false;
    if (atlas.device != device || atlas.full) {
        // The port's texture allocation is device-independent. Reset packing and upload fresh
        // tiles on demand, including after the backend device changes.
        atlas.device = device;
        atlas.x = atlas.y = atlas.row_height = 0;
        atlas.full = false;
        memset(atlas.entries, 0, sizeof(atlas.entries));
    }
    return true;
}

static const s_hd_atlas_glyph *hd_atlas_glyph(s_hd_atlas &atlas, bool sdf, uint32_t pixel_height, uint32_t codepoint)
{
    const uint32_t key = (pixel_height << 16) | codepoint;
    const uint32_t start = (key * 2654435761u) >> 21;
    s_hd_atlas_glyph *entry = NULL;
    for (uint32_t i = 0; i < k_hd_atlas_entries; i++) {
        s_hd_atlas_glyph &candidate = atlas.entries[(start + i) % k_hd_atlas_entries];
        if (candidate.key == key)
            return candidate.missing ? NULL : &candidate;
        if (candidate.key == 0) {
            entry = &candidate;
            break;
        }
    }
    if (!entry)
        atlas.full = true;
    if (atlas.full)
        return NULL;

    static uint8_t coverage[4 * k_hd_max_pixel_height * k_hd_max_pixel_height];
    s_port_hd_glyph glyph;
    float advance;
    if (sdf) {
        if (!sdf_glyph(pixel_height, codepoint, &glyph, &advance, coverage, sizeof(coverage))) {
            entry->key = key;
            entry->missing = true;
            return NULL;
        }
    } else {
        if (!port_hd_font_glyph(pixel_height, codepoint, &glyph, coverage, sizeof(coverage))) {
            entry->key = key;
            entry->missing = true;
            return NULL;
        }
        advance = (float)glyph.advance_x;
    }
    if (glyph.width > 0 && glyph.height > 0) {
        const int width = glyph.width + 2, height = glyph.height + 2;
        if (width > k_hd_atlas_width || height > k_hd_atlas_height)
            return NULL;
        if (atlas.x + width > k_hd_atlas_width) {
            atlas.x = 0;
            atlas.y += atlas.row_height;
            atlas.row_height = 0;
        }
        if (atlas.y + height > k_hd_atlas_height) {
            // Keep cached entries valid until this frame finishes.
            atlas.full = true;
            return NULL;
        }
        D3DLOCKED_RECT lock = {};
        atlas.texture->LockRect(0, &lock, NULL, 0);
        if (!lock.pBits || lock.Pitch < k_hd_atlas_width * (int)sizeof(uint32_t)) {
            atlas.texture->UnlockRect(0);
            return NULL;
        }
        uint32_t *dest = static_cast<uint32_t *>(lock.pBits);
        const unsigned int pitch = lock.Pitch / sizeof(*dest);
        for (int y = 0; y < height; y++)
            for (int x = 0; x < width; x++) {
                const uint32_t alpha = (x > 0 && x < width - 1 && y > 0 && y < height - 1)
                    ? coverage[(y - 1) * glyph.width + x - 1] : 0;
                dest[XGAddress2DTiledOffset(atlas.x + x, atlas.y + y, pitch, sizeof(*dest))] =
                    (alpha << 24) | 0x00ffffffu;
            }
        atlas.texture->UnlockRect(0);
        entry->x = atlas.x + 1;
        entry->y = atlas.y + 1;
        atlas.x += width;
        if (height > atlas.row_height)
            atlas.row_height = height;
    }
    entry->key = key;
    entry->glyph = glyph;
    entry->advance = advance;
    return entry;
}

static void draw_sdf_glyph(const s_frame *frame, const s_hd_atlas_glyph &entry, float x, float baseline,
    const real_rectangle2d &bounds, unsigned long color)
{
    const float left = x + entry.glyph.bearing_x, top = baseline - entry.glyph.bearing_y;
    const float right = left + entry.glyph.width, bottom = top + entry.glyph.height;
    const float x0 = left < bounds.x0 ? bounds.x0 : left, x1 = right > bounds.x1 ? bounds.x1 : right;
    const float y0 = top < bounds.y0 ? bounds.y0 : top, y1 = bottom > bounds.y1 ? bounds.y1 : bottom;
    if (x0 >= x1 || y0 >= y1)
        return;
    const float u0 = (entry.x + x0 - left) / k_hd_atlas_width, u1 = (entry.x + x1 - left) / k_hd_atlas_width;
    const float v0 = (entry.y + y0 - top) / k_hd_atlas_height, v1 = (entry.y + y1 - top) / k_hd_atlas_height;
    const float xs[] = { x0, x1, x1, x0 }, ys[] = { y0, y0, y1, y1 };
    const float us[] = { u0, u1, u1, u0 }, vs[] = { v0, v0, v1, v1 };
    const unsigned long colors[] = { color, color, color, color };
    screen_quad(frame, c_rasterizer_texture_ref(g_sdf_atlas.texture), xs, ys, us, vs, colors, true);
}

struct s_hd_character
{
    wchar_t code;
    float advance;
    const s_hd_atlas_glyph *entry;
};

static bool layout_sdf_text(const s_frame *frame, const wchar_t *string, long font, float wrap_width,
    float *width, float *height, const real_rectangle2d *bounds, unsigned long color, long justification)
{
    *width = *height = 0.0f;
    const int authored_px = font == k_font_title ? 32 : font == k_font_heading ? 23 : font == k_font_body ? 18 : 16;
    const float scaled_px = authored_px * frame->scale + 0.5f;
    const uint32_t pixel_height = scaled_px < 1.0f ? 1u
        : scaled_px > k_hd_max_pixel_height ? k_hd_max_pixel_height : (uint32_t)scaled_px;
    const s_font_header *header = sdf_font_header();
    if (!header)
        return false;
    const float scale = pixel_height / k_sdf_nominal_height;
    const float ascender = header->ascending_height * scale;
    const float descender = -header->descending_height * scale;
    const float line_height = ascender - descender + header->leading_height * scale;
    // Keep the line spacing used by the Guide's body and hint boxes, with the face's baseline metrics.
    const float line = font == k_font_body ? k_line_body * frame->scale
        : font == k_font_small ? k_line_small * frame->scale : line_height;
    const float baseline = (line + ascender + descender) * 0.5f;
    static s_hd_character characters[GUIDE_TEXT];
    int count = 0;
    for (; *string && count < GUIDE_TEXT - 1; string++) {
        const wchar_t code = *string;
        if (code == '|' && string[1] == '|')
            string++; // to_wide escapes literal pipes for the bitmap path.
        s_hd_character &character = characters[count++];
        character = { code, 0.0f, NULL };
        if (code == '\r' || code == '\n')
            continue;
        if (code >= 0xe100 && code <= 0xe1ff) {
            const wchar_t button[] = { code, 0 };
            float button_height;
            measure_bitmap(frame, button, font, 0.0f, &character.advance, &button_height);
            character.advance *= frame->scale;
        } else {
            character.entry = hd_atlas_glyph(g_sdf_atlas, true, pixel_height, code);
            if (!character.entry)
                return false;
            character.advance = character.entry->advance;
        }
    }

    const float shadow = (float)floor(0.65f * frame->scale + 0.5f);
    const unsigned long shadow_color = (((k_text_shadow >> 24) * (color >> 24)) / 255) << 24;
    int start = 0;
    while (start < count) {
        int end = start, next = start, space = -1;
        float advance = 0.0f, space_advance = 0.0f;
        for (; end < count; end++) {
            const s_hd_character &character = characters[end];
            if (character.code == '\r' || character.code == '\n')
                break;
            if (character.code == ' ' && (end == start || characters[end - 1].code != ' ')) {
                space = end;
                space_advance = advance;
            }
            if (wrap_width > 0 && end > start && advance + character.advance > wrap_width)
                break;
            advance += character.advance;
        }
        next = end;
        if (end < count) {
            if (characters[end].code == '\r' || characters[end].code == '\n') {
                next++;
            } else {
                if (space > start) {
                    end = next = space;
                    advance = space_advance;
                }
                while (next < count && characters[next].code == ' ')
                    next++;
            }
        }
        if (advance > *width)
            *width = advance;

        if (bounds && bounds->y0 + *height < bounds->y1) {
            const float top = bounds->y0 + *height;
            const float y = (float)floor(top + baseline + 0.5f);
            float left = bounds->x0;
            if (justification == _text_justification_right)
                left = bounds->x1 - advance;
            else if (justification == _text_justification_center)
                left = (bounds->x0 + bounds->x1 - advance) * 0.5f;
            left = (float)floor(left + 0.5f);
            for (int pass = 0; pass < 2; pass++) {
                float x = left;
                for (int i = start; i < end; i++) {
                    const s_hd_character &character = characters[i];
                    if (character.entry) {
                        const float glyph_x = floorf(x + 0.5f);
                        draw_sdf_glyph(frame, *character.entry, glyph_x + (pass == 0 ? shadow : 0.0f),
                            y + (pass == 0 ? shadow : 0.0f), *bounds, pass == 0 ? shadow_color : color);
                    } else if (pass == 1 && character.advance > 0.0f) {
                        const wchar_t button[] = { character.code, 0 };
                        const float x0 = (x - frame->x) / frame->scale;
                        const float y0 = (top - frame->y) / frame->scale;
                        const float right = x + character.advance + frame->scale;
                        text_bitmap(frame, button, font, x0, y0,
                            ((right < bounds->x1 ? right : bounds->x1) - frame->x) / frame->scale,
                            (bounds->y1 - frame->y) / frame->scale, color, _text_justification_left, false);
                    }
                    x += character.advance;
                }
            }
        }
        *height += line;
        start = next;
    }
    return true;
}

static void text(const s_frame *frame, const wchar_t *string, long font, float x0, float y0, float x1, float y1,
    unsigned long color, long justification, bool wrap)
{
    if (frame->sdf_text && x1 > x0 && y1 > y0) {
        real_rectangle2d bounds;
        bounds.x0 = fx(frame, x0);
        bounds.y0 = fy(frame, y0);
        bounds.x1 = fx(frame, x1);
        bounds.y1 = fy(frame, y1);
        float width, height;
        if (layout_sdf_text(frame, string, font, wrap ? bounds.x1 - bounds.x0 : 0.0f, &width, &height,
                           &bounds, color, justification))
            return;
    }
    text_bitmap(frame, string, font, x0, y0, x1, y1, color, justification, wrap);
}

// One line, cut with "..." to fit; returns the width drawn.
static float text_line(const s_frame *frame, const char *utf8, long font, float x0, float y, float x1, float line,
    unsigned long color, long justification, bool upper)
{
    wchar_t wide[GUIDE_LABEL * 2], fitted[GUIDE_LABEL * 2];
    to_wide(utf8, wide, GUIDE_LABEL * 2, upper);
    fit(frame, wide, font, x1 - x0, fitted, GUIDE_LABEL * 2);
    text(frame, fitted, font, x0, y, x1, y + line, color, justification, false);
    return text_width(frame, fitted, font);
}

// Wrapped text, at most `lines` lines; returns the height it took.
static float text_block(const s_frame *frame, const char *utf8, long font, float x0, float y, float x1, float line,
    long lines, unsigned long color)
{
    static wchar_t wide[GUIDE_TEXT];
    to_wide(utf8, wide, GUIDE_TEXT, false);
    if (wide[0] == 0)
        return 0.0f;
    float width, height;
    measure(frame, wide, font, x1 - x0, &width, &height);
    long count = (long)((height + line * 0.5f) / line);
    if (count < 1)
        count = 1;
    if (count > lines)
        count = lines;
    text(frame, wide, font, x0, y, x1, y + count * line + 2.0f, color, _text_justification_left, true);
    return count * line;
}

// ---- the guide ----

struct s_pane_box
{
    float x0, y0, x1, y1;
};

// `a` to `b` by `w` (0..1), channel by channel.
static unsigned long mix(unsigned long a, unsigned long b, float w)
{
    unsigned long out = 0;
    for (int shift = 0; shift < 32; shift += 8) {
        const float ca = (float)((a >> shift) & 0xff), cb = (float)((b >> shift) & 0xff);
        out |= (unsigned long)(ca + (cb - ca) * w + 0.5f) << shift;
    }
    return out;
}

static unsigned long scaled(unsigned long color, float level)
{
    return mix(color & 0xff000000, color, level) | (color & 0xff000000);
}

// The focused list's selection bar, Halo 3's (k_bar_stops): `pulse` 1 at the pulse's peak, 0 at its trough; `alpha`
// its own opacity (an item losing the focus fades out).
static void focus_bar(s_frame *frame, float x0, float y0, float x1, float y1, float pulse, float alpha)
{
    const float frame_alpha = frame->alpha;
    frame->alpha *= alpha;
    const float edge = (y1 - y0) * k_bar_edge_rows;
    const long stops = sizeof(k_bar_stops) / sizeof(k_bar_stops[0]);
    for (long i = 0; i + 1 < stops; i++) {
        const float sx0 = x0 + (x1 - x0) * k_bar_stops[i].at, sx1 = x0 + (x1 - x0) * k_bar_stops[i + 1].at;
        if (sx1 <= sx0)
            continue;
        const unsigned long left = mix(k_bar_stops[i].trough, k_bar_stops[i].peak, pulse);
        const unsigned long right = mix(k_bar_stops[i + 1].trough, k_bar_stops[i + 1].peak, pulse);
        fill_horizontal(frame, sx0, y0, sx1, y0 + edge, left, right);
        fill_horizontal(frame, sx0, y0 + edge, sx1, y1 - edge, scaled(left, k_bar_middle_level),
                        scaled(right, k_bar_middle_level));
        fill_horizontal(frame, sx0, y1 - edge, sx1, y1, left, right);
    }
    frame->alpha = frame_alpha;
}

// The pulse at `elapsed` milliseconds of the focus: 1 at its peak (as the item takes it), 0 at the trough.
static float focus_pulse(unsigned long elapsed)
{
    const float phase = (float)(elapsed % k_pulse_milliseconds) / (float)k_pulse_milliseconds;
    return 0.5f + 0.5f * cosf(phase * 6.28318531f);
}

// Which item of a pane's list has had the focus since when, and which lost it when (the pulse and the fade), kept
// from frame to frame; a new opening or page starts afresh, its selected item at the pulse's peak.
struct s_list_focus
{
    unsigned long session, page_serial;
    long item;                  // -1 none
    unsigned long since;
    long leaving;               // the item that lost the focus, fading out; -1 none
    unsigned long leaving_since;
};

static void focus_follow(s_list_focus *focus, const guide_view_pane *pane, unsigned long session,
    unsigned long page_serial, unsigned long now)
{
    if (focus->session != session || focus->page_serial != page_serial) {
        focus->session = session;
        focus->page_serial = page_serial;
        focus->item = pane->selected;
        focus->since = now;
        focus->leaving = -1;
    } else if (focus->item != pane->selected) {
        focus->leaving = focus->item;
        focus->leaving_since = now;
        focus->item = pane->selected;
        focus->since = now;
    }
    if (focus->leaving >= 0 && now - focus->leaving_since >= k_unfocus_milliseconds)
        focus->leaving = -1;
}

// The list of pane `index` in rows from `y` down to `bottom`, scrolled to keep the selection in view (`first` is the
// pane's first row shown, kept from frame to frame). Menu options in capitals, as Halo 3's lists show theirs; a
// player's name as it is.
// A triangle pointing left or right (a quad with two corners at its tip): a selected setting's marks.
static void side_arrow(const s_frame *frame, float x, float y, float size, bool left, unsigned long color)
{
    const float half = size * 0.5f;
    const float base = left ? x + half : x - half, tip = left ? x - half : x + half;
    const float xs[4] = { fx(frame, base), fx(frame, base), fx(frame, tip), fx(frame, tip) };
    const float ys[4] = { fy(frame, y - size), fy(frame, y + size), fy(frame, y), fy(frame, y) };
    const float uvs[4] = { 0.5f, 0.5f, 0.5f, 0.5f };
    const unsigned long colors[4] = { color, color, color, color };
    white_quad(frame, xs, ys, uvs, uvs, colors);
}

// A list row's place: the row's ends, where its text starts and its detail ends, its top and height, and the top of
// its line of text.
struct s_row
{
    float x0, x1, text_left, detail_right, top, height, line_top;
};

// A setting of the Settings page (guide_view_item::setting): its label at the left and its value at the right, in the
// items' font and in capitals as Halo 3's option lists show a value, between left / right marks while it is the
// focused selection (left / right step it; a click on a mark steps it once). A change that applies only at the next
// launch shows "(Restart Required)" beside the value, in amber (in white on the amber bar), when the row has room for
// it beside the label (the hint says so too).
static void draw_setting(s_frame *frame, const guide_view_item *item, long pane, long item_index, bool selected,
    bool active, const s_row *row, unsigned long label_color)
{
    const float mark = 5.0f;        // a mark's half height
    const float mark_room = 16.0f;  // the room a mark takes beside the value
    const float middle = row->top + (row->height - 2.0f) * 0.5f;
    wchar_t value[GUIDE_LABEL * 2];
    to_wide(item->detail, value, GUIDE_LABEL * 2, true);
    const float value_right = row->detail_right - mark_room;
    const float value_left = value_right - text_width(frame, value, k_font_body);
    const unsigned long value_color = !item->enabled ? k_disabled : selected ? k_white : k_hilite;
    text(frame, value, k_font_body, row->text_left, row->line_top, value_right, row->line_top + k_line_body + 4.0f,
         value_color, _text_justification_right, false);
    // the label, in the room the value leaves
    const float label_right = value_left - mark_room - 12.0f;
    wchar_t wide[GUIDE_LABEL * 2], fitted[GUIDE_LABEL * 2];
    to_wide(item->text, wide, GUIDE_LABEL * 2, true);
    fit(frame, wide, k_font_body, label_right - row->text_left, fitted, GUIDE_LABEL * 2);
    const float label_width = text_width(frame, fitted, k_font_body);
    text(frame, fitted, k_font_body, row->text_left, row->line_top, label_right, row->line_top + k_line_body + 4.0f,
         label_color, _text_justification_left, false);
    if (item->setting == GUIDE_SETTING_RESTART) {
        wchar_t tag[GUIDE_LABEL];
        to_wide("(Restart Required)", tag, GUIDE_LABEL, true);
        const float tag_width = text_width(frame, tag, k_font_small);
        if (row->text_left + label_width + 18.0f + tag_width <= label_right)
            text(frame, tag, k_font_small, label_right - tag_width - 4.0f, row->line_top + 2.0f, label_right,
                 row->line_top + k_line_small + 6.0f, selected ? 0xd8ffffff : 0xd9e38b4f, _text_justification_right,
                 false);
    }
    add_hit(frame, row->x0, row->top, row->x1, row->top + row->height, GUIDE_TARGET_ITEM, pane, item_index);
    if (active && item->enabled) {
        const float left_x = value_left - mark_room * 0.5f - 2.0f, right_x = value_right + mark_room * 0.5f + 2.0f;
        side_arrow(frame, left_x, middle, mark, true, k_white);
        side_arrow(frame, right_x, middle, mark, false, k_white);
        add_hit(frame, left_x - 10.0f, row->top, left_x + 8.0f, row->top + row->height, GUIDE_TARGET_VALUE_PREVIOUS,
                pane, item_index);
        add_hit(frame, right_x - 8.0f, row->top, right_x + 10.0f, row->top + row->height, GUIDE_TARGET_VALUE_NEXT,
                pane, item_index);
    }
}

static bool draw_spa_icon(const s_frame *frame, unsigned int image_id, float x, float y, float authored_size,
    unsigned long color);

static void draw_list(s_frame *frame, const guide_view_pane *pane, long index, float x0, float x1, float y,
    float bottom, float row_height, long *first, s_list_focus *focus)
{
    long rows = (long)((bottom - y) / row_height);
    if (rows < 1)
        rows = 1;
    if (pane->selected >= 0) {
        if (pane->selected < *first)
            *first = pane->selected;
        if (pane->selected >= *first + rows)
            *first = pane->selected - rows + 1;
    }
    if (*first + rows > pane->item_count)
        *first = pane->item_count > rows ? pane->item_count - rows : 0;
    if (*first < 0)
        *first = 0;
    const bool focused = pane->focused != 0;
    const float text_left = x0 + 16.0f, detail_right = x1 - 10.0f;
    for (long row = 0; row < rows && *first + row < pane->item_count; row++) {
        const long item_index = *first + row;
        const guide_view_item *item = &pane->items[item_index];
        const float top = y + row * row_height, row_bottom = top + row_height - 2.0f;
        const bool selected = item_index == pane->selected;
        if (selected && focused) {
            focus_bar(frame, x0, top, x1, row_bottom, focus_pulse(frame->now - focus->since), 1.0f);
        } else if (selected) { // another pane's selection: quieter, still
            fill_horizontal(frame, x0, top, x1, row_bottom, k_bar_unfocused, k_bar_unfocused & 0x00ffffff);
        } else if (focused && item_index == focus->leaving) { // losing the focus: the peak, fading out
            const float fade = (float)(frame->now - focus->leaving_since) / (float)k_unfocus_milliseconds;
            focus_bar(frame, x0, top, x1, row_bottom, 1.0f, fade >= 1.0f ? 0.0f : 1.0f - fade);
        }
        if (item->checked) // signed in, earned: the amber mark
            fill(frame, x0 + 6.0f, top + row_height * 0.5f - 3.0f, x0 + 11.0f, top + row_height * 0.5f + 2.0f,
                 selected ? 0xffffffff : k_amber);
        const unsigned long color = !item->enabled ? k_disabled : selected ? k_white : k_ice;
        const float line_top = top + (row_height - 2.0f - k_line_body) * 0.5f;
        if (item->setting != GUIDE_SETTING_NONE) {
            const s_row row = { x0, x1, text_left, detail_right, top, row_height, line_top };
            draw_setting(frame, item, index, item_index, selected, selected && focused, &row, color);
            continue;
        }
        float item_text_left = text_left;
        if (item->image_id != 0) {
            const float icon_size = 24.0f;
            const float icon_x = text_left + icon_size * 0.5f;
            const float icon_y = top + (row_height - 2.0f) * 0.5f;
            const unsigned long icon_color = item->checked ? 0xffffffff : selected ? 0x99b8b8b8 : 0x66999999;
            if (draw_spa_icon(frame, item->image_id, icon_x, icon_y, icon_size, icon_color))
                item_text_left = text_left + icon_size + 8.0f;
        }
        // the label first; the detail (right-aligned, smaller) in the room it leaves
        wchar_t wide[GUIDE_LABEL * 2], fitted[GUIDE_LABEL * 2];
        to_wide(item->text, wide, GUIDE_LABEL * 2, item->name == 0);
        const float room = detail_right - item_text_left;
        fit(frame, wide, k_font_body, room, fitted, GUIDE_LABEL * 2);
        const float label_width = text_width(frame, fitted, k_font_body);
        text(frame, fitted, k_font_body, item_text_left, line_top, detail_right, line_top + k_line_body + 4.0f, color,
             _text_justification_left, false);
        // shown whole or not at all (a narrow pane: the hint has it)
        wchar_t detail[GUIDE_LABEL * 2];
        to_wide(item->detail, detail, GUIDE_LABEL * 2, false);
        if (detail[0] != 0 && label_width + 18.0f + text_width(frame, detail, k_font_small) <= room) {
            const unsigned long detail_color = !item->enabled ? k_disabled : selected ? 0xccd8d8d8 : 0xcc698cbc;
            text(frame, detail, k_font_small, item_text_left + label_width + 18.0f, line_top + 2.0f, detail_right,
                 line_top + k_line_small + 6.0f, detail_color, _text_justification_right, false);
        }
        add_hit(frame, x0, top, x1, top + row_height, GUIDE_TARGET_ITEM, index, item_index);
    }
    const float mark_x = x1 - 8.0f;
    if (*first > 0) {
        arrow(frame, mark_x, y - 6.0f, 5.0f, true, k_mark);
        add_hit(frame, mark_x - 12.0f, y - 14.0f, mark_x + 12.0f, y + 2.0f, GUIDE_TARGET_SCROLL_UP, index, -1);
    }
    if (*first + rows < pane->item_count) {
        const float mark_y = y + rows * row_height + 6.0f;
        arrow(frame, mark_x, mark_y, 5.0f, false, k_mark);
        add_hit(frame, mark_x - 12.0f, mark_y - 8.0f, mark_x + 12.0f, mark_y + 8.0f, GUIDE_TARGET_SCROLL_DOWN,
                index, -1);
    }
}

// The lines of a read-only field's text ('\n' between them).
static long field_lines(const guide_view_pane *pane)
{
    long lines = 1;
    for (const char *c = pane->field; *c != 0; c++)
        lines += *c == '\n';
    return lines;
}

// The height a pane's field takes.
static float field_height(const guide_view_pane *pane)
{
    return pane->field_readonly ? field_lines(pane) * k_line_body + 16.0f : 34.0f;
}

// A text field: Create Profile's gamertag or a lobby code being typed, with its caret, the end of a text too long for
// it in view (as a text box scrolls); or, read-only, a lobby code to copy down, centred in its lines.
static float draw_field(const s_frame *frame, const guide_view_pane *pane, float x0, float x1, float y)
{
    const float height = field_height(pane);
    fill(frame, x0, y, x1, y + height, k_field);
    static wchar_t wide[GUIDE_FIELD]; // the presenting thread's
    to_wide(pane->field, wide, GUIDE_FIELD, false);
    if (pane->field_readonly) {
        outline(frame, x0, y, x1, y + height, k_field_border);
        text(frame, wide, k_font_body, x0 + 10.0f, y + 7.0f, x1 - 10.0f, y + height - 4.0f, k_white,
             _text_justification_center, false);
        return height;
    }
    outline(frame, x0, y, x1, y + height, pane->focused ? k_card_border : k_field_border);
    // the longest end of the text that fits
    const float room = x1 - x0 - 24.0f;
    const wchar_t *shown = wide;
    if (text_width(frame, wide, k_font_body) > room) {
        long low = 1, high = wide_length(wide); // the first character shown
        while (low < high) {
            const long middle = (low + high) / 2;
            if (text_width(frame, wide + middle, k_font_body) <= room)
                high = middle;
            else
                low = middle + 1;
        }
        shown = wide + low;
    }
    const float text_y = y + (height - k_line_body) * 0.5f - 1.0f;
    text(frame, shown, k_font_body, x0 + 10.0f, text_y, x1 - 10.0f, text_y + k_line_body + 4.0f, k_white,
         _text_justification_left, false);
    if (pane->focused && (frame->now / k_caret_blink_milliseconds) % 2 == 0) {
        const float caret = x0 + 10.0f + text_width(frame, shown, k_font_body) + 2.0f;
        fill(frame, caret, y + 7.0f, caret + 2.0f, y + height - 7.0f, k_amber);
    }
    return height;
}

// The contents of a pane (below its header): its page's title (when the card's is another page's), text, field, list,
// hint. `narrow`: one of several panes side by side.
static void draw_pane_body(s_frame *frame, const guide_view_pane *pane, long index, float x0, float y, float x1,
    float bottom, bool show_title, bool narrow, long *first, s_list_focus *focus)
{
    if (show_title && pane->title[0] != 0) {
        text_line(frame, pane->title, k_font_heading, x0, y, x1, 30.0f, k_hilite, _text_justification_left, true);
        y += 34.0f;
    }
    if (pane->body[0] != 0)
        y += text_block(frame, pane->body, k_font_body, x0, y, x1, k_line_body, narrow ? 4 : 5, k_ice) + 10.0f;
    if (pane->has_field)
        y += draw_field(frame, pane, x0, x1, y) + 12.0f;
    // the hint, from the bottom up
    float hint_height = 0.0f;
    if (pane->hint[0] != 0) {
        static wchar_t wide[GUIDE_TEXT / 4];
        to_wide(pane->hint, wide, GUIDE_TEXT / 4, false);
        float width, height;
        measure(frame, wide, k_font_small, x1 - x0, &width, &height);
        long lines = (long)((height + k_line_small * 0.5f) / k_line_small);
        lines = lines < 1 ? 1 : lines > k_hint_lines ? k_hint_lines : lines;
        hint_height = lines * k_line_small + 10.0f;
        rule(frame, x0, x1, bottom - hint_height - 2.0f, k_rule);
        text(frame, wide, k_font_small, x0, bottom - hint_height + 6.0f, x1, bottom + 2.0f, k_dim,
             _text_justification_left, true);
    }
    if (pane->item_count > 0)
        draw_list(frame, pane, index, x0 - 6.0f, x1 + 6.0f, y + 4.0f, bottom - hint_height - 8.0f, k_row_height, first,
                  focus);
}

// A legend's button glyph and its label ("(A) SELECT"), as Halo 3's footers write theirs; returns its width.
static float legend(const s_frame *frame, wchar_t glyph, const char *label, float x, float y, unsigned long color)
{
    wchar_t wide[GUIDE_LABEL + 2];
    wide[0] = glyph;
    wide[1] = ' ';
    to_wide(label, wide + 2, GUIDE_LABEL, true);
    const float width = text_width(frame, wide, k_font_body);
    text(frame, wide, k_font_body, x, y, x + width + 8.0f, y + k_line_body + 4.0f, color, _text_justification_left,
         false);
    return width;
}

// The card's footer: "(A) SELECT  (B) BACK", each a button for the mouse too; the keyboard's keys at the right.
static void draw_footer(const s_frame *frame, float x0, float y0, float x1, float y1, bool closes, bool panes)
{
    fill(frame, x0, y0, x1, y1, k_footer);
    rule(frame, x0, x1, y0, k_rule);
    const float line_top = y0 + (y1 - y0 - k_line_body) * 0.5f - 1.0f;
    float x = x0 + k_padding;
    const wchar_t glyphs[2] = { k_glyph_a, k_glyph_b };
    const char *const labels[2] = { "Select", closes ? "Close" : "Back" };
    const long targets[2] = { GUIDE_TARGET_ACCEPT, GUIDE_TARGET_BACK };
    for (long i = 0; i < 2; i++) {
        const float width = legend(frame, glyphs[i], labels[i], x, line_top, k_white);
        add_hit(frame, x - 6.0f, y0 + 4.0f, x + width + 6.0f, y1 - 4.0f, targets[i], -1, -1);
        x += width + 26.0f;
    }
    // the keyboard's keys, quieter, at the right
    const wchar_t keys_panes[] = L"TAB  NEXT CONTROLLER     HOME  CLOSE";
    const wchar_t keys[] = L"HOME  CLOSE";
    const wchar_t *right = panes ? keys_panes : keys;
    const float width = text_width(frame, right, k_font_small);
    const float right_x = x1 - k_padding;
    text(frame, right, k_font_small, right_x - width - 4.0f, line_top + 2.0f, right_x, line_top + k_line_small + 6.0f,
         k_dim, _text_justification_right, false);
    if (panes) {
        const float home_width = text_width(frame, keys, k_font_small);
        add_hit(frame, right_x - width - 4.0f, y0 + 4.0f, right_x - home_width - 16.0f, y1 - 4.0f,
                GUIDE_TARGET_NEXT_PANE, -1, -1);
    }
    add_hit(frame, right_x - text_width(frame, keys, k_font_small) - 6.0f, y0 + 4.0f, right_x + 4.0f, y1 - 4.0f,
            GUIDE_TARGET_CLOSE, -1, -1);
}

// LB / RB's blades, under the header of a page the menu leads to: each in its cell, the one shown lit and underlined
// in amber, LB and RB at the ends (each a button for the mouse: the blade before, after).
static void draw_tabs(const s_frame *frame, const guide_view_pane *pane, float x0, float y0, float x1)
{
    const float y1 = y0 + k_tabs_height;
    fill_vertical(frame, x0, y0, x1, y1, 0x40000000, 0x18000000);
    rule(frame, x0, x1, y1 - 1.0f, k_rule);
    const wchar_t lb[] = { k_glyph_lb, 0 }, rb[] = { k_glyph_rb, 0 };
    const float left = x0 + k_padding, right = x1 - k_padding;
    const float lb_width = text_width(frame, lb, k_font_body), rb_width = text_width(frame, rb, k_font_body);
    const float glyph_top = y0 + (k_tabs_height - k_line_body) * 0.5f - 1.0f;
    text(frame, lb, k_font_body, left, glyph_top, left + lb_width + 8.0f, glyph_top + k_line_body + 4.0f, k_white,
         _text_justification_left, false);
    text(frame, rb, k_font_body, right - rb_width - 4.0f, glyph_top, right + 4.0f, glyph_top + k_line_body + 4.0f,
         k_white, _text_justification_right, false);
    add_hit(frame, left - 8.0f, y0, left + lb_width + 8.0f, y1, GUIDE_TARGET_PREVIOUS_TAB, -1, -1);
    add_hit(frame, right - rb_width - 8.0f, y0, right + 8.0f, y1, GUIDE_TARGET_NEXT_TAB, -1, -1);
    const float inner0 = left + lb_width + 12.0f, inner1 = right - rb_width - 12.0f;
    // each label its own width and an even share of the room left, whole while at least 6 pixels separate two (the
    // five blades of a one-pane card); cells of one width, cut to fit, when the labels are too long even for that
    wchar_t labels[GUIDE_TABS][GUIDE_LABEL * 2];
    float widths[GUIDE_TABS], total = 0.0f;
    for (long i = 0; i < pane->tab_count; i++) {
        to_wide(pane->tabs[i], labels[i], GUIDE_LABEL * 2, true);
        widths[i] = text_width(frame, labels[i], k_font_small);
        total += widths[i];
    }
    const float room = inner1 - inner0;
    const bool natural = total + 6.0f * (float)pane->tab_count <= room;
    const float share = natural ? (room - total) / (float)pane->tab_count : room / (float)pane->tab_count;
    const float label_top = y0 + (k_tabs_height - k_line_small) * 0.5f - 2.0f;
    float cx0 = inner0;
    for (long i = 0; i < pane->tab_count; i++) {
        const bool current = i == pane->tab_current;
        const float cell = natural ? widths[i] + share : share;
        wchar_t fitted[GUIDE_LABEL * 2];
        fit(frame, labels[i], k_font_small, natural ? widths[i] + 1.0f : cell - 8.0f, fitted, GUIDE_LABEL * 2);
        const float width = text_width(frame, fitted, k_font_small);
        const float middle = cx0 + cell * 0.5f;
        text(frame, fitted, k_font_small, cx0 + 2.0f, label_top, cx0 + cell - 2.0f, label_top + k_line_small + 4.0f,
             current ? k_white : k_ice, _text_justification_center, false);
        cx0 += cell;
        if (current)
            fill_horizontal(frame, middle - width * 0.5f - 4.0f, y1 - 4.0f, middle + width * 0.5f + 4.0f, y1 - 2.0f,
                            k_amber, k_amber);
    }
}

// The card's header: the title at the left; one pane: its controller and player at the right.
static void draw_header(const s_frame *frame, const guide_view *view, float x0, float y0, float x1, const char *title)
{
    const float y1 = y0 + k_header_height;
    fill_vertical(frame, x0, y0, x1, y1, k_header_top, k_header_bottom);
    rule(frame, x0, x1, y1 - 1.0f, k_rule);
    float right = x1 - k_padding;
    if (view->pane_count == 1) {
        const guide_view_pane *pane = &view->panes[0];
        const float width = (x1 - x0) * 0.38f;
        text_line(frame, pane->heading, k_font_small, right - width, y0 + 10.0f, right, k_line_small + 4.0f, k_ice,
                  _text_justification_right, true);
        text_line(frame, pane->status, k_font_body, right - width, y0 + 31.0f, right, k_line_body + 4.0f, k_hilite,
                  _text_justification_right, false);
        right -= width + 12.0f;
    }
    // conduit-32, or conduit-23 for a title too long for it (cut with "..." only if even that does not fit)
    wchar_t wide[GUIDE_LABEL * 2];
    to_wide(title, wide, GUIDE_LABEL * 2, true);
    const bool large = text_width(frame, wide, k_font_title) <= right - (x0 + k_padding);
    text_line(frame, title, large ? k_font_title : k_font_heading, x0 + k_padding, y0 + (large ? 12.0f : 18.0f), right,
              large ? 44.0f : 32.0f, k_hilite, _text_justification_left, true);
}

// The rows a one-pane card needs: its text, field, list and hint, for its height.
static float single_content_height(const s_frame *frame, const guide_view_pane *pane, float inner_width)
{
    float height = 8.0f;
    if (pane->body[0] != 0) {
        static wchar_t wide[GUIDE_TEXT];
        to_wide(pane->body, wide, GUIDE_TEXT, false);
        float width, text_height;
        measure(frame, wide, k_font_body, inner_width, &width, &text_height);
        long lines = (long)((text_height + k_line_body * 0.5f) / k_line_body);
        height += (lines < 1 ? 1 : lines > 5 ? 5 : lines) * k_line_body + 10.0f;
    }
    if (pane->has_field)
        height += field_height(pane) + 12.0f;
    const long rows = pane->item_count < k_single_rows ? pane->item_count : k_single_rows;
    height += rows * k_row_height + 12.0f;
    height += k_hint_lines * k_line_small + 12.0f; // room for the hint, so the card does not jump as it changes
    return height;
}

struct s_guide_render_state
{
    guide_view view;              // the view drawn (guide_ingame_frame keeps it up to date)
    unsigned int session;         // the view's 32-bit opening counter; 0 none drawn yet
    unsigned long opened_at;      // GetTickCount when that opening was first drawn
    unsigned long closed_at;      // GetTickCount when it closed (fading out); 0 while it shows
    long first[GUIDE_PANES];      // each pane's first row shown
    s_list_focus focus[GUIDE_PANES]; // ... and its selection's pulse and fade
    guide_ingame_layout layout;   // where the parts were drawn (guide_ingame_set_layout)
};

static s_guide_render_state g_guide_render;

// Draws the guide; `card` gets its card's top and bottom (authored pixels).
static void draw_guide(s_frame *frame, const guide_view *view, bool interactive, float card[2])
{
    const bool multi = view->pane_count > 1;
    for (long i = 0; i < view->pane_count; i++)
        focus_follow(&g_guide_render.focus[i], &view->panes[i], view->session, view->page_serial, frame->now);
    const bool tabs = !multi && view->panes[0].tab_count > 1;
    // the card
    float x0, x1, y0, y1;
    if (multi) {
        x0 = k_card_margin;
        x1 = k_authored_width - k_card_margin;
        y0 = 56.0f;
        y1 = k_authored_height - 56.0f;
    } else {
        x0 = (k_authored_width - k_card_width) * 0.5f;
        x1 = x0 + k_card_width;
        const float content = single_content_height(frame, &view->panes[0], k_card_width - 2.0f * k_padding);
        float height = k_header_height + (tabs ? k_tabs_height : 0.0f) + content + k_footer_height;
        height = height < 260.0f ? 260.0f : height > 560.0f ? 560.0f : height;
        y0 = (float)(long)((k_authored_height - height) * 0.5f);
        y1 = y0 + height;
    }
    // the title: one pane's page; several: theirs when they show the same page
    const char *title = view->panes[0].title;
    for (long i = 1; i < view->pane_count; i++)
        if (strcmp(view->panes[i].title, title) != 0)
            title = view->caption;
    // the game blurred and dimmed behind the Guide card
    {
        s_gui_bitmap_widget_render_data blur_data;
        memset(&blur_data, 0, sizeof(blur_data));
        blur_data.flags = 4; // c_screen_postprocess::blur_display under bounds
        blur_data.corners[0].x = 0.0f;
        blur_data.corners[0].y = 0.0f;
        blur_data.corners[1].x = 0.0f;
        blur_data.corners[1].y = frame->height;
        blur_data.corners[2].x = frame->width;
        blur_data.corners[2].y = 0.0f;
        blur_data.corners[3].x = frame->width;
        blur_data.corners[3].y = frame->height;
        blur_data.bitmap_definition_index = NONE;
        const unsigned long blur_a = (unsigned long)(frame->alpha * 255.0f + 0.5f);
        blur_data.tint = ((blur_a > 255u ? 255u : blur_a) << 24) | 0x00ffffffu;
        blur_data.blend_method = c_rasterizer::_alpha_blend_alpha_blend;
        blur_data.shader_definition_index = NONE;
        rectangle2d window;
        window.x0 = 0;
        window.y0 = 0;
        window.x1 = (short)frame->width;
        window.y1 = (short)frame->height;
        c_gui_bitmap_widget_render(&blur_data, &window);
    }
    fill_vertical(frame, -frame->x / frame->scale, -frame->y / frame->scale, (frame->width - frame->x) / frame->scale,
                  (frame->height - frame->y) / frame->scale, k_backdrop_top, k_backdrop_bottom);
    fill_vertical(frame, x0, y0, x1, y1, k_card_top, k_card_bottom);
    outline(frame, x0, y0, x1, y1, k_card_border);
    draw_header(frame, view, x0, y0, x1, title);
    if (tabs)
        draw_tabs(frame, &view->panes[0], x0, y0 + k_header_height, x1);
    const float body_top = y0 + k_header_height + (tabs ? k_tabs_height : 0.0f) + 12.0f;
    const float body_bottom = y1 - k_footer_height - 10.0f;
    if (multi) {
        const float width = (x1 - x0 - 2.0f * k_pane_padding - (view->pane_count - 1) * k_pane_gap) / view->pane_count;
        for (long i = 0; i < view->pane_count; i++) {
            const guide_view_pane *pane = &view->panes[i];
            const float px0 = x0 + k_pane_padding + i * (width + k_pane_gap), px1 = px0 + width;
            fill_vertical(frame, px0, body_top, px1, body_bottom, pane->focused ? k_pane_focused : k_pane, k_pane);
            outline(frame, px0, body_top, px1, body_bottom, pane->focused ? k_card_border : k_pane_border);
            add_hit(frame, px0, body_top, px1, body_bottom, GUIDE_TARGET_PANE, i, -1);
            // the controller and its player
            const float inner0 = px0 + k_pane_padding, inner1 = px1 - k_pane_padding;
            text_line(frame, pane->heading, k_font_small, inner0, body_top + 8.0f, inner1, k_line_small + 4.0f,
                      pane->focused ? k_hilite : k_ice, _text_justification_left, true);
            text_line(frame, pane->status, k_font_body, inner0, body_top + 28.0f, inner1, k_line_body + 4.0f, k_hilite,
                      _text_justification_left, false);
            rule(frame, inner0, inner1, body_top + 58.0f, k_rule);
            draw_pane_body(frame, pane, i, inner0, body_top + 66.0f, inner1, body_bottom - 8.0f,
                           strcmp(pane->title, title) != 0, true, &g_guide_render.first[i], &g_guide_render.focus[i]);
        }
    } else {
        draw_pane_body(frame, &view->panes[0], 0, x0 + k_padding, body_top, x1 - k_padding, body_bottom, false, false,
                       &g_guide_render.first[0], &g_guide_render.focus[0]);
    }
    draw_footer(frame, x0, y1 - k_footer_height, x1, y1, view->pane_count == 1 && view->panes[0].depth == 0, multi);
    card[0] = y0;
    card[1] = y1;
    if (!interactive)
        frame->layout->hit_count = 0; // fading out: nothing to click
}

// ---- toasts (guide_ingame.h): the console's notification pop-ups ----

// The banner: the guide's card in small, at the bottom of the frame above the title-safe edge, over the HUD's free
// middle, and over the guide when it shows.
const float k_toast_width = 488.0f;
const float k_toast_height = 72.0f;      // title and text
const float k_toast_hint_height = 18.0f; // and a hint line
const float k_toast_bottom = 584.0f;     // 640 less the bottom title-safe margin
const float k_toast_emblem = 56.0f;      // the emblem's column
const unsigned long k_toast_gold = 0xfff0b65a;      // the accent bar's middle, the gamerscore
const unsigned long k_toast_gold_edge = 0x60e38b4f; // the accent bar's ends
const unsigned long k_toast_invite = 0xff7fa6d8;    // a game invite's emblem: ice, brighter

const int k_spa_icon_size = 64;
const int k_icon_scale = 4;
const int k_toast_icon_size = k_spa_icon_size * k_icon_scale;
const int k_lanczos_taps = 6;

struct s_lanczos_sample
{
    int first;
    float weight[k_lanczos_taps];
};

static float sinc(float x)
{
    const float radians = 3.14159265358979323846f * x;
    return radians == 0.0f ? 1.0f : (float)sin(radians) / radians;
}

static float clamp(float value, float low, float high)
{
    return value < low ? low : value > high ? high : value;
}

static void build_icon_filter(s_lanczos_sample (&filter)[k_icon_scale])
{
    // At 4x, the sample offsets repeat every four destination pixels.
    for (int phase = 0; phase < k_icon_scale; phase++) {
        const float source = (phase + 0.5f) / k_icon_scale - 0.5f;
        s_lanczos_sample &sample = filter[phase];
        sample.first = (int)floor(source) - 2;
        float total = 0.0f;
        for (int tap = 0; tap < k_lanczos_taps; tap++) {
            const float distance = source - (sample.first + tap);
            sample.weight[tap] = sinc(distance) * sinc(distance / 3.0f);
            total += sample.weight[tap];
        }
        for (int tap = 0; tap < k_lanczos_taps; tap++)
            sample.weight[tap] /= total;
    }
}

struct s_icon_rgba
{
    float channel[4]; // B, G, R, A
};

static unsigned int icon_byte(float value)
{
    return (unsigned int)(clamp(value, 0.0f, 255.0f) + 0.5f);
}

static int icon_source_index(int index)
{
    return index < 0 ? 0 : index >= k_spa_icon_size ? k_spa_icon_size - 1 : index;
}

static void resample_icon(const uint32_t *source, s_icon_rgba *horizontal, uint32_t *output)
{
    s_lanczos_sample filter[k_icon_scale];
    build_icon_filter(filter);
    // Filter premultiplied RGB so hidden colors in transparent texels do not bleed onto the visible icon.
    for (int y = 0; y < k_spa_icon_size; y++) {
        for (int x = 0; x < k_toast_icon_size; x++) {
            const s_lanczos_sample &sample = filter[x % k_icon_scale];
            const int first = x / k_icon_scale + sample.first;
            s_icon_rgba pixel = {};
            for (int tap = 0; tap < k_lanczos_taps; tap++) {
                const uint32_t argb = source[y * k_spa_icon_size + icon_source_index(first + tap)];
                const float alpha = (float)(argb >> 24);
                const float weight = sample.weight[tap] * alpha / 255.0f;
                for (int c = 0; c < 3; c++)
                    pixel.channel[c] += ((argb >> (8 * c)) & 255) * weight;
                pixel.channel[3] += alpha * sample.weight[tap];
            }
            horizontal[y * k_toast_icon_size + x] = pixel;
        }
    }
    for (int y = 0; y < k_toast_icon_size; y++) {
        const s_lanczos_sample &sample = filter[y % k_icon_scale];
        const int first = y / k_icon_scale + sample.first;
        const s_icon_rgba *rows[k_lanczos_taps];
        for (int tap = 0; tap < k_lanczos_taps; tap++)
            rows[tap] = horizontal + icon_source_index(first + tap) * k_toast_icon_size;
        for (int x = 0; x < k_toast_icon_size; x++) {
            s_icon_rgba pixel = {};
            for (int tap = 0; tap < k_lanczos_taps; tap++) {
                for (int c = 0; c < 4; c++)
                    pixel.channel[c] += rows[tap][x].channel[c] * sample.weight[tap];
            }
            const float alpha = pixel.channel[3];
            const unsigned int a = icon_byte(alpha);
            uint32_t argb = a << 24;
            if (a != 0)
                for (int c = 0; c < 3; c++)
                    argb |= icon_byte(pixel.channel[c] * 255.0f / alpha) << (8 * c);
            output[y * k_toast_icon_size + x] = argb;
        }
    }
}

static uint32_t sharpen_icon_pixel(uint32_t center, const uint32_t (&neighbors)[4])
{
    // Unsharp mask with a 4-neighbor blur. Weight it by coverage and keep the correction within the local
    // range so it strengthens detail without ringing past edges or introducing fringes around transparency.
    const float alpha = (float)(center >> 24);
    float weights[4];
    for (int i = 0; i < 4; i++)
        weights[i] = (float)(neighbors[i] >> 24) / 255.0f;
    uint32_t result = center & 0xff000000u;
    for (int shift = 0; shift < 24; shift += 8) {
        const float value = (float)((center >> shift) & 255);
        float low = value, high = value, difference = 0.0f;
        for (int i = 0; i < 4; i++) {
            const float delta = ((float)((neighbors[i] >> shift) & 255) - value) * weights[i];
            const float neighbor = value + delta;
            difference -= delta;
            low = neighbor < low ? neighbor : low;
            high = neighbor > high ? neighbor : high;
        }
        const float amount = clamp(difference * (0.35f / 8.0f) * alpha / 255.0f, -4.0f, 4.0f);
        result |= icon_byte(clamp(value + amount, low, high)) << shift;
    }
    return result;
}

static bool upload_toast_icon(IDirect3DTexture9 *texture, const uint32_t *image)
{
    D3DLOCKED_RECT lock = {};
    texture->LockRect(0, &lock, NULL, 0);
    const bool writable = lock.pBits != NULL && lock.Pitch >= k_toast_icon_size * (int)sizeof(uint32_t);
    if (writable) {
        uint32_t *dest = static_cast<uint32_t *>(lock.pBits);
        const unsigned int pitch = lock.Pitch / sizeof(*dest);
        // A8R8G8B8 is tiled on Xbox. The write lock marks the texels as host-endian for the backend.
        for (int y = 0; y < k_toast_icon_size; y++) {
            const uint32_t *row = image + y * k_toast_icon_size;
            const uint32_t *above = y > 0 ? row - k_toast_icon_size : row;
            const uint32_t *below = y + 1 < k_toast_icon_size ? row + k_toast_icon_size : row;
            for (int x = 0; x < k_toast_icon_size; x++) {
                const uint32_t neighbors[] = {
                    row[x > 0 ? x - 1 : x], row[x + 1 < k_toast_icon_size ? x + 1 : x], above[x], below[x],
                };
                const uint32_t texel = sharpen_icon_pixel(row[x], neighbors);
                dest[XGAddress2DTiledOffset(x, y, pitch, sizeof(*dest))] = ui_fill_color(texel);
            }
        }
    }
    texture->UnlockRect(0);
    return writable;
}

static IDirect3DTexture9 *toast_icon(IDirect3DDevice9 *device, unsigned int image_id)
{
    struct s_cached_icon
    {
        unsigned int image_id;
        unsigned long long last_used;
        IDirect3DTexture9 *storage;
        bool uploaded;
    };
    static s_cached_icon cache[96];
    static IDirect3DDevice9 *cache_device;
    static unsigned long long serial;
    if (device != cache_device) {
        // The port's texture headers and physical memory are device-independent. Reuse the bounded storage,
        // but upload again on the new device so its backend sees the write. The port's D3D Reset is a no-op.
        for (s_cached_icon &entry : cache) {
            entry.image_id = 0;
            entry.last_used = 0;
        }
        cache_device = device;
    }

    s_cached_icon *slot = &cache[0];
    for (s_cached_icon &entry : cache) {
        if (entry.image_id == image_id) {
            entry.last_used = ++serial;
            return entry.uploaded ? entry.storage : NULL;
        }
        if (entry.last_used < slot->last_used)
            slot = &entry;
    }
    slot->image_id = image_id;
    slot->last_used = ++serial;
    slot->uploaded = false;

    const void *data;
    unsigned int size;
    s_port_image_info info;
    uint32_t *source;
    if (!port_spa_image_get(image_id, &data, &size) || !port_image_decode(data, size, &info, &source))
        return NULL;
    if (info.width == k_spa_icon_size && info.height == k_spa_icon_size) {
        const size_t horizontal_count = k_spa_icon_size * k_toast_icon_size;
        s_icon_rgba *horizontal = (s_icon_rgba *)malloc(horizontal_count * sizeof(s_icon_rgba));
        uint32_t *scaled = (uint32_t *)malloc(k_toast_icon_size * k_toast_icon_size * sizeof(uint32_t));
        if (horizontal && scaled) {
            resample_icon(source, horizontal, scaled);
            if (!slot->storage)
                slot->storage = static_cast<IDirect3DTexture9 *>(D3DDevice_CreateTexture(k_toast_icon_size,
                    k_toast_icon_size, 1, 1, 0, D3DFMT_A8R8G8B8, D3DPOOL_MANAGED, D3DRTYPE_TEXTURE));
            if (slot->storage)
                slot->uploaded = upload_toast_icon(slot->storage, scaled);
        }
        free(scaled);
        free(horizontal);
    }
    free(source);
    return slot->uploaded ? slot->storage : NULL;
}

static bool draw_spa_icon(const s_frame *frame, unsigned int image_id, float x, float y, float authored_size,
    unsigned long color)
{
    if (image_id == 0 || !frame->white.valid())
        return false;
    IDirect3DTexture9 *texture = toast_icon(c_rasterizer::get_device(), image_id);
    if (!texture)
        return false;

    // The screen quad's -0.5 offset cancels the viewport's +0.5 pixel-center offset. Snap to frame pixels.
    const float size = (float)floor(authored_size * frame->scale + 0.5f);
    if (size <= 0.0f)
        return false;
    const float left = (float)floor(fx(frame, x) - size * 0.5f + 0.5f), right = left + size;
    const float top = (float)floor(fy(frame, y) - size * 0.5f + 0.5f), bottom = top + size;

    const float xs[] = { left, right, right, left };
    const float ys[] = { top, top, bottom, bottom };
    const float us[] = { 0.0f, 1.0f, 1.0f, 0.0f }, vs[] = { 0.0f, 0.0f, 1.0f, 1.0f };
    const unsigned long colors[] = { color, color, color, color };
    // Restore the white texture binding through the rasterizer so its sampler cache and the device agree
    // before later UI draws.
    screen_quad(frame, c_rasterizer_texture_ref(texture), xs, ys, us, vs, colors);
    c_rasterizer::set_sampler_texture(0, frame->white);
    return true;
}

static bool draw_toast_icon(const s_frame *frame, unsigned int image_id, float x, float y)
{
    return draw_spa_icon(frame, image_id, x, y, 48.0f, 0xffffffff);
}

static float ease_out(float t) { return 1.0f - (1.0f - t) * (1.0f - t); }

// A diamond (a square on its corner) centred at (x, y), `radius` from centre to corner.
static void diamond(const s_frame *frame, float x, float y, float radius, unsigned long color)
{
    const float xs[4] = { fx(frame, x), fx(frame, x + radius), fx(frame, x), fx(frame, x - radius) };
    const float ys[4] = { fy(frame, y - radius), fy(frame, y), fy(frame, y + radius), fy(frame, y) };
    const float uvs[4] = { 0.5f, 0.5f, 0.5f, 0.5f };
    const unsigned long colors[4] = { color, color, color, color };
    white_quad(frame, xs, ys, uvs, uvs, colors);
}

// The toast as it shows `elapsed` milliseconds after it began: the banner opens from its centre (its frosted glass with
// it, which is drawn opaque and so cannot fade), then the contents fade in; it holds; the contents fade and it closes.
static void draw_toast(s_frame *frame, const guide_toast *toast, const float *card)
{
    const float in = (float)GUIDE_TOAST_FADE_IN_MILLISECONDS, out = (float)GUIDE_TOAST_FADE_OUT_MILLISECONDS;
    const float elapsed = (float)toast->elapsed, total = (float)GUIDE_TOAST_MILLISECONDS;
    float open = 1.0f;
    if (elapsed < in)
        open = ease_out(elapsed / in);
    else if (elapsed > total - out)
        open = ease_out((total - elapsed) / out);
    open = open < 0.0f ? 0.0f : open > 1.0f ? 1.0f : open;
    if (open <= 0.02f)
        return;
    const bool hint = toast->hint[0] != 0;
    const float height = k_toast_height + (hint ? k_toast_hint_height : 0.0f);
    const float middle = k_authored_width * 0.5f, half = k_toast_width * 0.5f * open;
    // over the guide's card only when there is no room under or over it (its hints stay in view)
    float y1 = k_toast_bottom;
    if (card != NULL && card[1] + 8.0f + height <= k_authored_height - 4.0f)
        y1 = card[1] + 8.0f + height;
    else if (card != NULL && card[0] - 8.0f - height >= 4.0f)
        y1 = card[0] - 8.0f;
    const float x0 = middle - half, x1 = middle + half, y0 = y1 - height;
    // the card's colours open with it; the contents follow once it is mostly open
    frame->alpha = 1.0f;
    fill_vertical(frame, x0, y0, x1, y1, k_card_top, k_card_bottom);
    outline(frame, x0, y0, x1, y1, k_card_border);
    fill_horizontal(frame, x0, y0, middle, y0 + 3.0f, k_toast_gold_edge, k_toast_gold);
    fill_horizontal(frame, middle, y0, x1, y0 + 3.0f, k_toast_gold, k_toast_gold_edge);
    const float contents = (open - 0.65f) / 0.35f;
    if (contents <= 0.0f)
        return;
    frame->alpha = contents > 1.0f ? 1.0f : contents;
    // The emblem: the disc's achievement icon when available, or an invite's (A) to accept.
    const float left = middle - k_toast_width * 0.5f;
    const float emblem_x = left + 14.0f + k_toast_emblem * 0.5f, emblem_y = y0 + 3.0f + (height - 3.0f) * 0.5f;
    if (toast->kind != GUIDE_TOAST_INVITE) {
        if (!draw_toast_icon(frame, toast->image_id, emblem_x, emblem_y)) {
            diamond(frame, emblem_x, emblem_y, 22.0f, k_amber);
            diamond(frame, emblem_x, emblem_y, 16.0f, k_card_bottom | 0xff000000);
            diamond(frame, emblem_x, emblem_y, 9.0f, k_toast_gold);
        }
    } else {
        diamond(frame, emblem_x, emblem_y, 22.0f, k_toast_invite);
        diamond(frame, emblem_x, emblem_y, 19.0f, k_card_bottom | 0xff000000);
        const wchar_t accept[] = { k_glyph_a, 0 };
        text(frame, accept, k_font_body, emblem_x - 16.0f, emblem_y - 13.0f, emblem_x + 16.0f, emblem_y + 15.0f,
             k_white, _text_justification_center, false);
    }
    const float separator = left + 14.0f + k_toast_emblem + 4.0f; // a hairline between the emblem and the text
    fill_vertical(frame, separator, y0 + 12.0f, separator + 1.0f / frame->scale, y1 - 10.0f, k_rule,
                  k_rule & 0x00ffffff);
    // the title over the text (and the gamerscore at its right), the hint under it
    const float tx0 = left + 14.0f + k_toast_emblem + 14.0f, tx1 = left + k_toast_width - 18.0f;
    float detail_width = 0.0f;
    if (toast->detail[0] != 0) {
        wchar_t detail[GUIDE_LABEL * 2];
        to_wide(toast->detail, detail, GUIDE_LABEL * 2, false);
        detail_width = text_width(frame, detail, k_font_body) + 14.0f;
        text(frame, detail, k_font_body, tx1 - detail_width, y0 + 33.0f, tx1, y0 + 33.0f + k_line_body + 4.0f,
             k_toast_gold, _text_justification_right, false);
    }
    text_line(frame, toast->title, k_font_small, tx0, y0 + 12.0f, tx1, k_line_small + 4.0f,
              toast->kind == GUIDE_TOAST_ACHIEVEMENT ? k_toast_gold : k_toast_invite, _text_justification_left, true);
    text_line(frame, toast->text, k_font_body, tx0, y0 + 33.0f, tx1 - detail_width, k_line_body + 4.0f, k_white,
              _text_justification_left, false);
    if (hint)
        text_line(frame, toast->hint, k_font_small, tx0, y0 + 60.0f, tx1, k_line_small + 4.0f, k_dim,
                  _text_justification_left, false);
}

// Tag-resource render access on the presenting thread: get_default_texture_rasterizer_texture_ref and the UI's shaders
// resolve bitmap/geometry handles through g_geometry_globals.manager->m_resource_cache.m_render_access for the current
// thread (tag_resource_available). During boot splashes, map loading and crash frames (main_render_loading_frame,
// main_render_blank_frame, main_crash_render_message), end_frame runs on the main thread without
// tag_resources_lock_render_access (or before a map's resource gestalt is loaded), leaving the thread's access cache
// NULL; attempting to draw the in-game guide there crashes in tag_resource_available at NULL + 0x14.
static bool tag_resources_render_ready()
{
    const c_cache_file_tag_resource_runtime_manager *manager = g_geometry_globals.manager;
    if (manager == NULL || manager->m_cache_file_resource_gestalt_index == NONE)
        return false;
    const long thread = get_current_thread_index();
    const c_tag_resource_thread_access &access = manager->m_resource_cache.m_render_access;
    if (thread < 0 || thread >= access.m_access_cache_count || access.m_per_thread_access_cache == NULL)
        return false;
    const c_tag_resource_cache_file_access_cache *cache = access.m_per_thread_access_cache[thread];
    return cache != NULL && cache->m_resource_handles.begin() != NULL && cache->m_resource_values.begin() != NULL;
}

} // namespace

bool port_menu_hd_text_enabled()
{
    return port_config_hd_text() != 0;
}

bool port_ui_hd_text_enabled()
{
    if (g_guide_text_scope)
        return guide_text_mode() == k_guide_text_hd;
    return port_menu_hd_text_enabled();
}

bool port_ui_hd_text_prepare_atlas()
{
    return port_hd_font_available() && prepare_hd_atlas(g_hd_atlas);
}

void port_ui_hd_text_reserve_atlases()
{
    // At shell start, before maps fill the memory below 4 GB; drawing packs glyphs in later.
    create_hd_atlas_texture(g_hd_atlas, "HD");
    create_hd_atlas_texture(g_sdf_atlas, "SDF");
}

D3DTexture *port_ui_hd_text_glyph(unsigned int pixel_height, unsigned int codepoint,
    s_port_ui_hd_glyph *out_glyph, real_rectangle2d *uv)
{
    if (!g_hd_atlas.texture || !pixel_height || pixel_height > k_hd_max_pixel_height)
        return NULL;
    const s_hd_atlas_glyph *entry = hd_atlas_glyph(g_hd_atlas, false, pixel_height, codepoint);
    if (!entry || !entry->glyph.width || !entry->glyph.height)
        return NULL;

    out_glyph->width = entry->glyph.width;
    out_glyph->height = entry->glyph.height;
    out_glyph->bearing_x = entry->glyph.bearing_x;
    out_glyph->bearing_y = entry->glyph.bearing_y;
    out_glyph->advance = entry->advance;
    uv->x0 = (float)entry->x / k_hd_atlas_width;
    uv->x1 = (float)(entry->x + entry->glyph.width) / k_hd_atlas_width;
    uv->y0 = (float)entry->y / k_hd_atlas_height;
    uv->y1 = (float)(entry->y + entry->glyph.height) / k_hd_atlas_height;
    return g_hd_atlas.texture;
}

extern "C" unsigned long g_rasterizer_resolved_surface_mask[7];

extern "C" void guide_ingame_ui_render(void)
{
    s_guide_render_state *state = &g_guide_render;
    // drawable: the map's globals (the rasterizer globals' white texture, the UI's blur), a live tag-resource render
    // access cache on this thread, and a frame to draw into; a guide opened while not (at boot or between maps) shows
    // in its own window instead (guide_ingame.h)
    const bool drawable = global_game_globals && global_game_globals->rasterizer_globals_ref.index != NONE &&
                          tag_resources_render_ready() && c_rasterizer::get_device() != NULL &&
                          c_rasterizer::render_globals.width > 0 && c_rasterizer::render_globals.height > 0;
    const bool showing = guide_ingame_frame(&state->view, drawable) != 0;
    const unsigned long now = GetTickCount();
    // the guide: showing, fading out, or not drawn
    bool guide = true;
    if (showing) {
        if (state->session != state->view.session) {
            state->session = state->view.session;
            state->opened_at = now;
            for (long i = 0; i < GUIDE_PANES; i++)
                state->first[i] = 0;
        }
        state->closed_at = 0;
    } else if (state->session == 0) {
        guide = false; // nothing drawn yet: the usual case
    } else {
        if (state->closed_at == 0)
            state->closed_at = now;
        if (now - state->closed_at >= k_fade_out_milliseconds || state->view.pane_count <= 0) {
            state->session = 0;
            guide = false;
        }
    }
    guide = guide && state->view.pane_count > 0 && drawable;
    // a toast, over the guide (asked every frame, the guide shown or not: its clock runs in drawable frames)
    guide_toast toast;
    const bool toast_shows = guide_ingame_toast_frame(now, drawable, &toast) != 0;
    if (!guide && !toast_shows)
        return; // nothing to draw: no state touched
    s_frame frame;
    frame.width = (float)c_rasterizer::render_globals.width;
    frame.height = (float)c_rasterizer::render_globals.height;
    const float scale_x = frame.width / k_authored_width, scale_y = frame.height / k_authored_height;
    frame.scale = scale_x < scale_y ? scale_x : scale_y;
    frame.x = (frame.width - k_authored_width * frame.scale) * 0.5f;
    frame.y = (frame.height - k_authored_height * frame.scale) * 0.5f;
    frame.sdf_text = guide_text_mode() == k_guide_text_sdf && sdf_font_header() && prepare_hd_atlas(g_sdf_atlas);
    {
        // a draw string scales its glyphs by the display's glyph scale itself (1 at 1152x640; at a custom resolution the
        // UI's own authored -> render scale, the same fit as frame.scale): text takes the rest of frame.scale
        const c_user_interface_draw_string reference;
        const float display_scale = reference.get_scale_internal() / reference.get_scale();
        frame.text_scale = display_scale > 0.0f ? frame.scale / display_scale : frame.scale;
    }
    frame.now = now;
    if (showing) {
        const unsigned long elapsed = now - state->opened_at;
        frame.alpha = elapsed >= k_fade_in_milliseconds ? 1.0f : (float)elapsed / (float)k_fade_in_milliseconds;
    } else {
        frame.alpha = 1.0f - (float)(now - state->closed_at) / (float)k_fade_out_milliseconds;
    }
    frame.alpha = frame.alpha < 0.0f ? 0.0f : frame.alpha > 1.0f ? 1.0f : frame.alpha;
    const c_rasterizer_globals *globals =
        (const c_rasterizer_globals *)tag_get('rasg', global_game_globals->rasterizer_globals_ref.index);
    frame.white = globals != NULL && globals->m_default_textures_refs.count() > 0
                      ? globals->get_default_texture_rasterizer_texture_ref(0)
                      : c_rasterizer_texture_ref();
    memset(&state->layout, 0, sizeof(state->layout));
    state->layout.session = state->view.session;
    state->layout.page_serial = state->view.page_serial;
    state->layout.width = frame.width;
    state->layout.height = frame.height;
    frame.layout = &state->layout;

    // the state the frame left, put back afterwards
    const c_rasterizer::e_surface targets[4] = {
        c_rasterizer::get_render_target(0), c_rasterizer::get_render_target(1),
        c_rasterizer::get_render_target(2), c_rasterizer::get_render_target(3),
    };
    unsigned long saved_resolved_mask[7];
    memcpy(saved_resolved_mask, g_rasterizer_resolved_surface_mask, sizeof(saved_resolved_mask));
    const s_rasterizer_last_rectangles rectangles = g_rasterizer_last_rectangles;
    DWORD scissor_test = FALSE, cull_mode = D3DCULL_CCW;
    c_rasterizer::get_device()->GetRenderState(D3DRS_SCISSORTESTENABLE, &scissor_test);
    c_rasterizer::get_device()->GetRenderState(D3DRS_CULLMODE, &cull_mode);
    const long z_buffer_mode = g_rasterizer_current_z_buffer_mode;
    const c_rasterizer::e_stencil_mode stencil_mode = g_rasterizer_stencil_mode;
    const c_rasterizer::e_alpha_blend_mode alpha_blend_mode = c_rasterizer::get_current_alpha_blend_mode();
    c_player_view *player_view = c_player_view::get_current(); // the guide covers the whole frame, as the UI view
    c_player_view::set_current(NULL);

    c_screen_postprocess::setup_rasterizer_for_postprocess(false);
    c_rasterizer::set_render_target(0, c_rasterizer::_surface_display);
    for (long i = 1; i < 4; i++)
        c_rasterizer::set_render_target(i, c_rasterizer::_surface_none);
    c_rasterizer::set_stencil_mode(c_rasterizer::_stencil_mode_off);
    rectangle2d bounds;
    bounds.x0 = 0;
    bounds.y0 = 0;
    bounds.x1 = (short)frame.width;
    bounds.y1 = (short)frame.height;
    c_rasterizer::begin(bounds, bounds);

    float card[2] = { 0.0f, 0.0f };
    if (guide) {
        // the card rises into place as it fades in (easing out), and sinks back a little as it fades out
        const float frame_y = frame.y;
        frame.y += (showing ? 1.0f - ease_out(frame.alpha) : 0.5f * (1.0f - frame.alpha)) * k_slide * frame.scale;
        draw_guide(&frame, &state->view, showing, card);
        frame.y = frame_y;
        if (showing)
            guide_ingame_set_layout(&state->layout);
    }
    if (toast_shows)
        draw_toast(&frame, &toast, guide ? card : NULL);

    c_rasterizer::clear_sampler_textures(0);
    for (long i = 0; i < 4; i++)
        c_rasterizer::set_render_target(i, targets[i]);
    c_rasterizer::set_depth_stencil_surface(c_rasterizer::_surface_none);
    memcpy(g_rasterizer_resolved_surface_mask, saved_resolved_mask, sizeof(saved_resolved_mask));
    // the viewport and scissor as the game records them (its own setters' z range is always 0..1), the scissor test
    // on or off as it was
    c_rasterizer::set_viewport(rectangles.viewport, 0.0f, 1.0f);
    c_rasterizer::set_scissor_rect(scissor_test ? &rectangles.scissor : NULL);
    c_rasterizer::get_device()->SetRenderState(D3DRS_CULLMODE, cull_mode);
    g_rasterizer_last_rectangles = rectangles;
    c_rasterizer::set_z_buffer_mode((c_rasterizer::e_z_buffer_mode)z_buffer_mode);
    c_rasterizer::set_stencil_mode(stencil_mode);
    if (c_rasterizer::get_current_alpha_blend_mode() != alpha_blend_mode) { // the rasterizer's cache (retail 0x81dab8)
        c_rasterizer::set_current_alpha_blend_mode(alpha_blend_mode);
        rasterizer_set_device_alpha_blend_mode(c_rasterizer::get_device(), alpha_blend_mode);
    }
    c_player_view::set_current(player_view);
}
