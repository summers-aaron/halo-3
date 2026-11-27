// @address 0x8a410 wcslen
// blamlibxbox/draw_string.obj: from the records 0x189498 0x189688 0x1898a0 0x1899c0 0x189a10 0x189c10 0x189ca0 0x189d60 0x18a350 0x18a3b0 0x18a520 0x18a7b0 0x18aca0 0x18aea0 0x18af38 0x18afd0 0x18b400 0x191810 0x191968 0x2902f8 0x290508 0x2905e8 0x2906b0 0x290818 0x290880 0x2908e8 0x290a70
#include "text/draw_string_globals.h"
#include "cseries/global_real_argb_colors.h"
#include "rasterizer/rasterizer_main.h"
// @flags /TP /O2 /Oc
#include "text/font_id_types.h"
#include "text/character_data_index_types.h"
#include "math/real_math_types.h"
#include "interface/gui_widget_types.h"
// @address 0x189688 ?font_get_header@@YAPBUs_font_header@@J@Z
// @address 0x189498 ??0c_draw_string@@QAA@XZ
// @address 0x18aca0 ?build_one_line_of_text_to_draw@c_draw_string@@AAA?AUs_line_results@1@PAVc_font_cache@@PAUs_parse_string_state@1@PBTreal_rectangle2d@@M@Z
// @address 0x191810 ?set_shadow_color@c_draw_string@@QAAXPBTreal_argb_color@@@Z
// @address 0x191968 ?draw_string_draw@@YA_NPAVc_draw_string@@PAVc_font_cache@@PBD@Z
// @address 0x2908e8 ?ascii_parse_set_color_sequence@@YAXPBDPAPBDPAUutf32@@JPAJ@Z
// @address 0x290880 ?parse_wchar_hexadecimal_digit@@YA_N_WJPAK@Z
// @address 0x2902f8 ?set_color_deprecated@c_draw_string@@QAAXPBTreal_argb_color@@_N@Z
// @address 0x18a3b0 ?compute_utf32_string_bounds@c_draw_string@@QAA_NPAVc_font_cache@@PBUutf32@@PATrectangle2d@@2@Z
// @address 0x2905e8 ?draw_more@c_draw_string@@QAA_NPAVc_font_cache@@PBD@Z
// @address 0x2906b0 ?adjust_bounds_for_vertical_alignment@c_draw_string@@AAAXPAVc_font_cache@@PBUutf32@@PATreal_rectangle2d@@_N3@Z
// @address 0x18af38 ?font_get_kerning_pair_offset@@YAFPBUs_font_header@@KK@Z
// @address 0x189c10 ?precache_utf32_string_for_font@c_draw_string@@QAA_NPAVc_font_cache@@PBUutf32@@W4e_font_id@@@Z
// @address 0x189ca0 ?parse_string_new@c_draw_string@@AAA_NW4e_font_id@@PBUutf32@@PAUs_parse_string_state@1@@Z
// @address 0x189d60 ?parse_string@c_draw_string@@AAAXPAUs_parse_string_state@1@@Z
// @address 0x1899c0 ?set_font@c_draw_string@@QAAXW4e_font_id@@@Z

#include <string.h>
#include "interface/user_interface.h"
#include "text/unicode.h"
#include "math/color_math.h"
#include "text/draw_string.h"
#include "cseries/cseries_wrapped.h"
#include "text/utf32_range_types.h"

struct s_user_interface_shared_globals;
#include "interface/interface_constants.h"
#include "rasterizer/rasterizer_video_mode_types.h"


const float k_real_one = 1.0f;
const float k_real_zero = 0.0f;

#include "cseries/cseries.h"



#include "text/escape_table_types.h"
#include "text/unicode_tables.h"
#pragma pack(push, 1)
#include "interface/chud/utf32_types.h"
#pragma pack(pop)
class c_font_cache;

#include <wchar.h>


#include "text/font_package_types.h"


 


enum
{
    _utf32_font_change_character = 0xE300,
    _utf32_color_change_character = 0xE301,
    _utf32_style_none_character = 0xE400,
    _utf32_style_bold_character = 0xE401,
    _utf32_style_plain_character = 0xE402,
    _utf32_style_italic_character = 0xE403,
    _utf32_style_condense_character = 0xE404,
    _utf32_justification_left_character = 0xE405,
    _utf32_justification_right_character = 0xE406,
    _utf32_justification_center_character = 0xE407,
    _utf32_font_restore_character = 0xE408,
    _utf32_color_restore_character = 0xE409,
    _utf32_justification_restore_character = 0xE40A,
};

#include "render/render_globals.h"
static const real_argb_color *const draw_string_global_real_argb_white = &global_real_argb_colors[_real_argb_white];
static const real_argb_color *const draw_string_global_real_argb_black = &global_real_argb_colors[_real_argb_black];

#include "text/font_character_types.h"

#include "text/draw_string_types.h"



#include "interface/user_interface_shared_globals.h"

#include "memory/lruv_cache_types.h"
#include "memory/lruv_block_types.h"
#include "text/font_cache_character_types.h"
#include "text/font_cache_globals_types.h"


#include "interface/chud/chud_draw_string.h"
#include "text/rasterizer_draw_string_types.h"


static __forceinline rectangle2d *draw_string_set_rectangle2d(rectangle2d *rectangle, short x0, short y0, short x1, short y1)
{
    rectangle->x0 = x0;
    rectangle->y0 = y0;
    rectangle->x1 = x1;
    rectangle->y1 = y1;
    return rectangle;
}

static __forceinline bool color_is_valid(const real_argb_color *color)
{
    return color->alpha >= k_real_zero && color->alpha <= k_real_one
        && color->red >= k_real_zero && color->red <= k_real_one
        && color->green >= k_real_zero && color->green <= k_real_one
        && color->blue >= k_real_zero && color->blue <= k_real_one;
}

void c_draw_string::set_bounds(const rectangle2d *bounds)
{
    if (bounds) {
        if (bounds->x1 - bounds->x0 >= 0 && bounds->y1 - bounds->y0 >= 0) {
            m_bounds.x0 = (float)bounds->x0;
            m_bounds.x1 = (float)bounds->x1;
            m_bounds.y0 = (float)bounds->y0;
            m_bounds.y1 = (float)bounds->y1;
        }
    } else {
        memset(&m_bounds, 0, sizeof(m_bounds));
    }
}

void c_draw_string::set_shadow_color(const real_argb_color *color)
{
    real_argb_color adjusted = *color;
    if (!color_is_valid(&adjusted)) {
        adjusted.alpha = MIN(MAX(adjusted.alpha, k_real_zero), k_real_one);
        adjusted.red = MIN(MAX(adjusted.red, k_real_zero), k_real_one);
        adjusted.green = MIN(MAX(adjusted.green, k_real_zero), k_real_one);
        adjusted.blue = MIN(MAX(adjusted.blue, k_real_zero), k_real_one);
    }
    m_shadow_color = adjusted;
}

// 0x191968
bool draw_string_draw(c_draw_string *string_drawer, c_font_cache *font_cache, const char *string)
{
    return string_drawer->c_draw_string::draw(font_cache, string);
}

void c_draw_string::set_color_deprecated(const real_argb_color *color, bool allow_overbright)
{
    const float threshold = 0.68f;
    real_argb_color adjusted = *color;
    bool overbright = adjusted.red > threshold && adjusted.green > threshold && adjusted.blue > threshold;
    if (overbright && !allow_overbright) {
        float lowest = adjusted.red > (adjusted.green > adjusted.blue ? adjusted.blue : adjusted.green)
            ? (adjusted.green > adjusted.blue ? adjusted.blue : adjusted.green) : adjusted.red;
        lowest = PIN(lowest, threshold, k_real_one);
        float scale = threshold / lowest;
        adjusted.red *= scale;
        adjusted.green *= scale;
        adjusted.blue *= scale;
    }
    bool valid = adjusted.alpha >= k_real_zero && adjusted.alpha <= k_real_one
        && adjusted.red >= k_real_zero && adjusted.red <= k_real_one
        && adjusted.green >= k_real_zero && adjusted.green <= k_real_one
        && adjusted.blue >= k_real_zero && adjusted.blue <= k_real_one;
    if (!valid) {
        adjusted.alpha = PIN(adjusted.alpha, k_real_zero, k_real_one);
        adjusted.red = PIN(adjusted.red, k_real_zero, k_real_one);
        adjusted.green = PIN(adjusted.green, k_real_zero, k_real_one);
        adjusted.blue = PIN(adjusted.blue, k_real_zero, k_real_one);
    }
    m_color = adjusted;
}

bool c_draw_string::draw(c_font_cache *font_cache, const char *string)
{
    long string_length = strlen(string);
    long offset = 0;
    bool result = false;
    while (offset < string_length)
    {
        utf32 utf32_string[256];
        long characters_consumed;

        ascii_string_to_utf32_string(string + offset, k_unicode_escape_table, utf32_string, 256, &characters_consumed);
        if (offset == 0)
        {
            result = draw(font_cache, utf32_string);
        }
        else
        {
            m_flags.set(_text_flag_draw_more, true);
            result = draw(font_cache, utf32_string);
            m_flags.set(_text_flag_draw_more, false);
        }
        if (!result)
            break;
        offset += characters_consumed;
    }
    return result;
}

bool c_draw_string::draw_more(c_font_cache *font_cache, const char *string)
{
    long string_length = strlen(string);
    long offset = 0;
    bool result = false;
    while (offset < string_length)
    {
        utf32 utf32_string[256];
        long characters_consumed;

        ascii_string_to_utf32_string(string + offset, k_unicode_escape_table, utf32_string, 256, &characters_consumed);
        m_flags.set(_text_flag_draw_more, true);
        result = draw(font_cache, utf32_string);
        m_flags.set(_text_flag_draw_more, false);
        if (!result)
            break;
        offset += characters_consumed;
    }
    return result;
}

bool c_draw_string::compute_wchar_string_bounds(c_font_cache *font_cache, const wchar_t *string, rectangle2d *bounds, rectangle2d *cursor_bounds)
{
    utf32 utf32_string[2048];

    wchar_string_to_utf32_string(string, k_unicode_escape_table, utf32_string, 2048, 0);
    return compute_utf32_string_bounds(font_cache, utf32_string, bounds, cursor_bounds);
}

bool c_draw_string::compute_utf32_string_bounds(c_font_cache *font_cache, const utf32 *string, rectangle2d *bounds, rectangle2d *cursor_bounds)
{
    bool result = false;
    if (bounds || cursor_bounds)
    {
        m_text_bounds.y0 = 32767.0f;
        m_text_bounds.x0 = 32767.0f;
        m_text_bounds.y1 = -32768.0f;
        m_text_bounds.x1 = -32768.0f;
        m_flags.set(_text_flag_compute_bounds, true);
        result = c_draw_string::draw(font_cache, string);
        m_flags.set(_text_flag_compute_bounds, false);
        if (cursor_bounds)
        {
            cursor_bounds->x0 = (short)m_cursor.x;
            cursor_bounds->x1 = (short)(m_cursor.x + 1.0f);
            cursor_bounds->y0 = (short)(m_cursor.y - m_font_header->ascending_height);
            cursor_bounds->y1 = (short)(m_cursor.y + m_font_header->descending_height);
        }
        if (bounds)
        {
            bounds->x0 = (short)m_text_bounds.x0;
            bounds->x1 = (short)m_text_bounds.x1;
            bounds->y0 = (short)m_text_bounds.y0;
            bounds->y1 = (short)m_text_bounds.y1;
        }
    }
    return result;
}

void c_draw_string::adjust_bounds_for_vertical_alignment(
    c_font_cache *font_cache, const utf32 *string, real_rectangle2d *bounds, bool center, bool align_bottom)
{
    rectangle2d string_bounds;
    compute_utf32_string_bounds(font_cache, string,
        draw_string_set_rectangle2d(&string_bounds, (short)m_bounds.x0, (short)m_bounds.y0, (short)m_bounds.x1, (short)m_bounds.y1),
        0);
    float string_height = (float)(string_bounds.y1 - string_bounds.y0);
    bounds->x0 = (float)string_bounds.x0;
    bounds->x1 = (float)string_bounds.x1;
    bounds->y0 = (float)string_bounds.y0;
    bounds->y1 = (float)string_bounds.y1;
    if (center)
    {
        float bounds_height = m_bounds.y1 - m_bounds.y0;
        bounds->y0 = m_bounds.y0 + bounds_height * 0.5f - string_height * 0.5f;
        bounds->y1 = bounds->y0 + string_height;
    }
    else if (align_bottom)
    {
        bounds->y1 = m_bounds.y1;
        bounds->y0 = bounds->y1 - string_height;
    }
}

// @address 0x290818 ?parse_ascii_hexadecimal_digit@@YA_NDJPAK@Z
bool parse_ascii_hexadecimal_digit(char character, long shift, unsigned long *value)
{
    long digit;
    if (character >= '0' && character <= '9') {
        digit = character - '0';
    } else if (character >= 'a' && character <= 'f') {
        digit = character - 'a' + 10;
    } else if (character >= 'A' && character <= 'F') {
        digit = character - 'A' + 10;
    } else {
        return false;
    }
    *value |= digit << shift;
    return true;
}

bool parse_wchar_hexadecimal_digit(wchar_t c, long shift, unsigned long *value)
{
    int digit;
    if (c >= 48 && c <= 57) {
        digit = c - 48;
    } else if (c >= 97 && c <= 102) {
        digit = c - 87;
    } else if (c >= 65 && c <= 70) {
        digit = c - 55;
    } else {
        return 0;
    }
    *value = (digit << shift) | *value;
    return 1;
}

void ascii_parse_set_color_sequence(const char *in_src, const char **out_src, utf32 *result, long result_max_length, long *result_length)
{
    unsigned long value = 0;
    bool valid = true;
    valid &= parse_ascii_hexadecimal_digit(in_src[2], 28, &value);
    valid &= parse_ascii_hexadecimal_digit(in_src[3], 24, &value);
    valid &= parse_ascii_hexadecimal_digit(in_src[4], 20, &value);
    valid &= parse_ascii_hexadecimal_digit(in_src[5], 16, &value);
    valid &= parse_ascii_hexadecimal_digit(in_src[6], 12, &value);
    valid &= parse_ascii_hexadecimal_digit(in_src[7], 8, &value);
    valid &= parse_ascii_hexadecimal_digit(in_src[8], 4, &value);
    valid &= parse_ascii_hexadecimal_digit(in_src[9], 0, &value);
    if (valid)
    {
        result[0].character = 0xE301;
        result[1].character = 0xB100 | (value >> 24);
        result[2].character = 0xB100 | ((value >> 16) & 0xff);
        result[3].character = 0xB100 | ((value >> 8) & 0xff);
        result[4].character = 0xB100 | (value & 0xff);
        *out_src = in_src + 10;
        *result_length = 5;
    }
    else
    {
        *out_src = in_src + 2;
        *result_length = 0;
    }
}

// @address 0x290a70 ?wchar_parse_set_color_sequence@@YAXPB_WPAPB_WPAUutf32@@JPAJ@Z
void wchar_parse_set_color_sequence(const wchar_t *source, const wchar_t **end, utf32 *output, long maximum_length, long *length)
{
    unsigned long value = 0;
    bool valid = true;
    valid &= parse_wchar_hexadecimal_digit(source[2], 28, &value);
    valid &= parse_wchar_hexadecimal_digit(source[3], 24, &value);
    valid &= parse_wchar_hexadecimal_digit(source[4], 20, &value);
    valid &= parse_wchar_hexadecimal_digit(source[5], 16, &value);
    valid &= parse_wchar_hexadecimal_digit(source[6], 12, &value);
    valid &= parse_wchar_hexadecimal_digit(source[7], 8, &value);
    valid &= parse_wchar_hexadecimal_digit(source[8], 4, &value);
    valid &= parse_wchar_hexadecimal_digit(source[9], 0, &value);
    if (valid) {
        output[0].character = 0xE301;
        output[1].character = 0xB100 | (value >> 24);
        output[2].character = 0xB100 | ((value >> 16) & 0xFF);
        output[3].character = 0xB100 | ((value >> 8) & 0xFF);
        output[4].character = 0xB100 | (value & 0xFF);
        *end = source + 10;
        *length = 5;
    } else {
        *end = source + 2;
        *length = 0;
    }
}

struct s_font_header;
extern "C" {
// @address 0x93ddf0 g_font_globals
}


static __forceinline float draw_string_get_glyph_scaling_for_display_settings()
{
    long aspect_ratio = ::interface_get_closest_authored_ui_aspect_ratio();
    float scale = 1.0f;
    if (aspect_ratio != _authored_ui_aspect_ratio_four_by_three)
    {
        const s_user_interface_shared_globals *globals = ::user_interface_shared_tag_globals_try_and_get();
        if (globals)
        {
            scale = globals->authoring_aspect_conversion_scaling[aspect_ratio].i > globals->authoring_aspect_conversion_scaling[aspect_ratio].j
                ? globals->authoring_aspect_conversion_scaling[aspect_ratio].j
                : globals->authoring_aspect_conversion_scaling[aspect_ratio].i;
        }
        else
        {
            float default_scales[k_authored_ui_aspect_ratio_count] = { 1.0f, 0.73f };
            scale = default_scales[aspect_ratio];
        }
    }
#if !defined(_XBOX) || defined(__clang__)
    // PC (port/GAME-CHANGES.md, "Custom resolutions"): the menus' authored -> render scale (1 at 1152x640, the 360's
    // render size; at a custom resolution the uniform fit of 1152x640 into it: calculate_authoring_to_target_dimensions_
    // scaling, which scales the widgets' bounds and animations), so text keeps its size in the layout. The HUD's draw
    // strings lay out in its own virtual space and set 1 (c_chud_draw_string).
    if (rasterizer_main_get_video_mode()->fIsWideScreen != 0)
    {
        calculate_authoring_to_target_dimensions_scaling();
        scale *= g_interface_authoring_to_target_dimensions_scaling.i;
    }
#endif
    return scale;
}


c_draw_string::c_draw_string() :
    m_flags(),
    m_font_id(_terminal_font),
    m_font_header(font_get_header(_terminal_font)),
    m_style(_text_style_none),
    m_justification(_text_justification_left),
    m_initial_line(1),
    m_color(*draw_string_global_real_argb_white),
    m_shadow_color(*draw_string_global_real_argb_black),
    m_scale(1.0f),
    m_display_scale(draw_string_get_glyph_scaling_for_display_settings()),
    m_line_spacing(0),
    m_tab_stop_count(0),
    m_tab_stops(),
    m_bounds(),
    m_text_bounds(),
    m_clip_bounds(),
    m_cursor(),
    m_maximum_lines(0),
    m_line_index(0),
    m_initial_indent(0),
    m_paragraph_indent(0),
    m_saved_parse_state(),
    m_saved_tab_stop_index(0),
    m_saved_line_count(0),
    m_saved_maximum_lines(0),
    m_saved_indent(0)
{
    {
    rectangle2d render_bounds;
    rectangle2d render_title_safe_bounds;
    rasterizer_main_get_display_and_render_bounds(NULL, NULL, &render_bounds, &render_title_safe_bounds);
    set_bounds(&render_title_safe_bounds);
    memset(&m_saved_parse_state, 0, sizeof(m_saved_parse_state));
    }
}
static __forceinline const s_font_header *draw_string_font_get_header_internal(long font_index)
{
    const s_font_package *package = g_font_globals.package;
    const s_font_header *header = 0;
    if (package && font_index >= 0 && font_index < package->font_count)
        header = (const s_font_header *)((const char *)package + package->fonts[font_index].header_offset);
    return header;
}

const s_font_header *font_get_header(long font_id)
{
    long font_index = -1;
    const s_font_package *package = g_font_globals.package;
    if (package && font_id >= 0 && font_id < sizeof(package->font_indices) / sizeof(package->font_indices[0]))
        font_index = package->font_indices[font_id];
    return draw_string_font_get_header_internal(font_index);
}

inline bool c_draw_string::recache_font_header()
{
    m_font_header = font_get_header(m_font_id);
    return m_font_header != 0;
}

// 0x1899c0
void c_draw_string::set_font(e_font_id font)
{
    m_font_id = (e_font_id)PIN(font, _terminal_font, k_public_font_id_count - 1);
    m_font_header = font_get_header(m_font_id);
}

bool c_draw_string::precache_wchar_string_for_font(c_font_cache *font_cache, const wchar_t *string, e_font_id font)
{
    utf32 utf32_string[2048];

    wchar_string_to_utf32_string(string, k_unicode_escape_table, utf32_string, 2048, 0);
    return precache_utf32_string_for_font(font_cache, utf32_string, font);
}

bool c_draw_string::precache_utf32_string_for_font(c_font_cache *font_cache, const utf32 *string, e_font_id font)
{
    s_parse_string_state parse_state;
    bool result = parse_string_new(font, string, &parse_state);
    if (result)
    {
        long token;
        do
        {
            parse_string(&parse_state);
            token = parse_state.token;
            if (token == _parse_token_character)
                result &= predict_character(font_cache, font, parse_state.character);
        } while (token != _parse_token_end);
    }
    return result;
}

bool c_draw_string::parse_string_new(e_font_id font, const utf32 *string, s_parse_string_state *parse_state)
{
    memset(parse_state, 0, sizeof(s_parse_string_state));
    parse_state->string = string;
    parse_state->font = font;
    parse_state->previous_font = parse_state->font;
    get_color(&parse_state->initial_color);
    parse_state->color = *draw_string_global_real_argb_white;
    parse_state->justification = get_justification();
    parse_state->previous_justification = parse_state->justification;
    return recache_font_header();
}

static __forceinline bool draw_string_utf32_get_font_change_character_sequence(const utf32 *sequence, long *sequence_length, e_font_id *font)
{
    *sequence_length = 0;
    if (sequence[0].character == _utf32_font_change_character
        && (sequence[1].character & 0xFF00) == 0xA100
        && (e_font_id)(sequence[1].character & 0xFF) >= 0 && (e_font_id)(sequence[1].character & 0xFF) < k_public_font_id_count)
    {
        *sequence_length = 2;
        *font = (e_font_id)(sequence[1].character & 0xFF);
        return true;
    }
    return false;
}

static __forceinline bool draw_string_utf32_get_color_change_character_sequence(const utf32 *sequence, long *sequence_length, unsigned long *color)
{
    bool result;
    *sequence_length = 0;
    if (sequence[0].character == _utf32_color_change_character
        && (sequence[1].character & 0xFF00) == 0xB100
        && (sequence[2].character & 0xFF00) == 0xB100
        && (sequence[3].character & 0xFF00) == 0xB100
        && (sequence[4].character & 0xFF00) == 0xB100)
    {
        *color = ((((sequence[1].character << 8) | (sequence[2].character & 0xFF)) << 8 | (sequence[3].character & 0xFF)) << 8) 
            | (sequence[4].character & 0xFF);
        result = true;
        *sequence_length = 5;
    }
    else
    {
        result = false;
    }
    return result;
}

void c_draw_string::parse_string(s_parse_string_state *parse_state)
{
    e_parse_token token = _parse_token_none;
    utf32 character;
    do
    {
        character = parse_state->string[parse_state->string_index];
        parse_state->string_index++;
        switch (character.character)
        {
        case _utf32_style_none_character:
            set_style(_text_style_none);
            token = _parse_token_style;
            break;
        case _utf32_style_bold_character:
            set_style(_text_style_bold);
            token = _parse_token_style;
            break;
        case _utf32_style_plain_character:
            set_style(_text_style_plain);
            token = _parse_token_style;
            break;
        case _utf32_style_italic_character:
            set_style(_text_style_italic);
            token = _parse_token_style;
            break;
        case _utf32_style_condense_character:
            set_style(_text_style_condense);
            token = _parse_token_style;
            break;
        case _utf32_justification_left_character:
            parse_state->previous_justification = parse_state->justification;
            parse_state->justification = _text_justification_left;
            token = _parse_token_justification;
            break;
        case _utf32_justification_right_character:
            parse_state->previous_justification = parse_state->justification;
            parse_state->justification = _text_justification_right;
            token = _parse_token_justification;
            break;
        case _utf32_justification_center_character:
            parse_state->previous_justification = parse_state->justification;
            parse_state->justification = _text_justification_center;
            token = _parse_token_justification;
            break;
        case _utf32_justification_restore_character:
            parse_state->justification = parse_state->previous_justification;
            token = _parse_token_justification;
            break;
        case _utf32_font_change_character:
        {
            long sequence_length;
            e_font_id font;
            const utf32 *sequence = &parse_state->string[parse_state->string_index - 1];
            if (draw_string_utf32_get_font_change_character_sequence(sequence, &sequence_length, &font))
            {
                parse_state->previous_font = parse_state->font;
                parse_state->font = font;
                parse_state->string_index += sequence_length - 1;
                token = _parse_token_font;
            }
            else
            {
                parse_state->can_break_line = m_flags.test(_text_flag_wrap_horizontally) ? utf32_can_line_break(parse_state->character, character) : false;
                token = _parse_token_character;
            }
            break;
        }
        case _utf32_font_restore_character:
            parse_state->font = parse_state->previous_font;
            token = _parse_token_font;
            break;
        case _utf32_color_change_character:
        {
            long sequence_length;
            unsigned long color;
            const utf32 *sequence = &parse_state->string[parse_state->string_index - 1];
            if (draw_string_utf32_get_color_change_character_sequence(sequence, &sequence_length, &color))
            {
                pixel32_to_real_argb_color(color, &parse_state->color);
                parse_state->string_index += sequence_length - 1;
                token = _parse_token_color;
            }
            else
            {
                parse_state->can_break_line = m_flags.test(_text_flag_wrap_horizontally) ? utf32_can_line_break(parse_state->character, character) : false;
                token = _parse_token_character;
            }
            break;
        }
        case _utf32_color_restore_character:
            parse_state->color = *draw_string_global_real_argb_white;
            token = _parse_token_color;
            break;
        case '\r':
            token = _parse_token_new_line;
            break;
        case '\t':
            token = _parse_token_tab;
            break;
        case '\n':
            break;
        case 0:
            token = _parse_token_end;
            break;
        default:
            parse_state->can_break_line = m_flags.test(_text_flag_wrap_horizontally) ? utf32_can_line_break(parse_state->character, character) : false;
            token = _parse_token_character;
            break;
        }
    } while (token == _parse_token_none);

    switch (parse_state->previous_token)
    {
    case _parse_token_character:
        parse_state->previous_character = parse_state->character;
        break;
    default:
        parse_state->previous_character.character = 0;
        break;
    }
    parse_state->previous_token = parse_state->token;
    parse_state->token = token;
    parse_state->character = character;
}

extern "C" {
}
c_draw_string::s_line_results c_draw_string::build_one_line_of_text_to_draw(
    c_font_cache *font_cache, s_parse_string_state *parse_state, const real_rectangle2d *bounds, float scale)
{
    // match: 0x18a7b0: preserve the caller memory summary through the bounds-field alias.
    const float *right_bound = &bounds->x1;
    long line_index = 0;
    float line_width = 0;
    long break_index = 0;
    float break_width;
    long characters = 0;
    bool done = false;
    while (!done)
    {
        bool backed_up = false;
        parse_string(parse_state);
        switch (parse_state->token)
        {
        case _parse_token_justification:
        case _parse_token_style:
        case _parse_token_font:
        case _parse_token_color:
            break;
        case _parse_token_character:
        {
            const s_font_character *character = 0;
            if (precache_character(font_cache, m_font_id, parse_state->character, &character))
            {
                float kerning = scale *
                    font_get_kerning_pair_offset(
                        m_font_header, parse_state->previous_character.character, parse_state->character.character);
                float bearing = scale * character->bearing;
                float advance = scale * character->advance;
                float width = scale * character->advance;
                if (parse_state->can_break_line)
                {
                    break_index = line_index;
                    break_width = line_width;
                }
                if (m_cursor.x + line_width + kerning + bearing + width < *right_bound || characters == 0)
                {
                    line_width += kerning + bearing + advance;
                    characters++;
                }
                else if (m_flags.test(_text_flag_wrap_horizontally))
                {
                    if (break_index > 0)
                    {
                        line_index = break_index;
                        line_width = break_width;
                        parse_state->token = _parse_token_new_line;
                        if (utf32_in_list(parse_state->string[break_index], k_unicode_line_break_ranges, 12))
                            line_index++;
                        backed_up = true;
                    }
                    done = true;
                }
            }
            break;
        }
        default:
            done = true;
            break;
        }
        if (!backed_up)
            line_index = parse_state->string_index;
    }
    s_line_results result;
    result.width = line_width;
    result.index = line_index;
    return result;
}

short font_get_kerning_pair_offset(const s_font_header *header, unsigned long first_character, unsigned long second_character)
{
    short offset = 0;
    if (first_character != 0 && second_character != 0 && first_character <= 255 && second_character <= 255)
    {
        const s_kerning_pair *kerning_pairs = (const s_kerning_pair *)((const char *)header + header->kerning_pairs_offset);
        long first_index = header->character_first_kerning_pair_index[first_character];
        long last_index = first_character < 255 ? header->character_first_kerning_pair_index[first_character + 1] : header->kerning_pair_count;
        for (long index = first_index; index < last_index; index++)
        {
            if (kerning_pairs[index].second_character == second_character)
            {
                offset = kerning_pairs[index].offset;
                break;
            }
            if (kerning_pairs[index].second_character > second_character)
                break;
        }
    }
    return offset;
}

bool c_draw_string::draw(c_font_cache *font_cache, const wchar_t *string)
{
    long string_length = wcslen(string);
    long offset = 0;
    bool result = false;
    while (offset < string_length)
    {
        utf32 utf32_string[256];
        long characters_consumed;

        wchar_string_to_utf32_string(string + offset, k_unicode_escape_table, utf32_string, 256, &characters_consumed);
        if (offset == 0)
        {
            result = draw(font_cache, utf32_string);
        }
        else
        {
            m_flags.set(_text_flag_draw_more, true);
            result = draw(font_cache, utf32_string);
            m_flags.set(_text_flag_draw_more, false);
        }
        if (!result)
            break;
        offset += characters_consumed;
    }
    return result;
}



// @address 0x18aea0 ?precache_character@c_chud_draw_string@@UAA_NPAVc_font_cache@@W4e_font_id@@Uutf32@@PAPBUs_font_character@@@Z
bool c_chud_draw_string::precache_character(c_font_cache *font_cache, e_font_id font, utf32 character, const s_font_character **out_character)
{
    return rasterizer_text_try_and_get_glyph(font_cache, font, character, false, (long *)out_character, 0);
}

// @address 0x18aea0 ?precache_character@c_rasterizer_draw_string@@UAA_NPAVc_font_cache@@W4e_font_id@@Uutf32@@PAPBUs_font_character@@@Z
bool c_rasterizer_draw_string::precache_character(c_font_cache *font_cache, e_font_id font, utf32 character, const s_font_character **out_character)
{
    return rasterizer_text_try_and_get_glyph(font_cache, font, character, false, (long *)out_character, 0);
}

static __forceinline bool draw_string_utf32_isprint(long character)
{
    bool result = true;
    if ((unsigned long)character < 32)
        result = false;
    return result;
}

static __forceinline bool draw_string_utf32_ismonochrome(const utf32 &character)
{
    bool result;
    if ((unsigned long)(character.character - 0xE112) <= 32)
        result = false;
    else
        result = (unsigned long)(character.character - 0xE000) <= 6399;
    return result;
}


static __forceinline void draw_string_real_rgb_colors_multiply(const real_rgb_color *const &a, const real_rgb_color *const &b, real_rgb_color *const &result)
{
    result->red = a->red * b->red;
    result->green = a->green * b->green;
    result->blue = a->blue * b->blue;
}

static __forceinline void draw_string_real_argb_colors_multiply(
    const real_argb_color *const &a, const real_argb_color *const &b, real_argb_color *const &result)
{
    draw_string_real_rgb_colors_multiply((const real_rgb_color *)&a->red, (const real_rgb_color *)&b->red, (real_rgb_color *)&result->red);
    result->alpha = a->alpha * b->alpha;
}


__forceinline void c_draw_string::text_bounds_draw_character(float x, float y, float width, float height)
{
    float x1 = x + get_scale_internal() * width + 1.0f;
    float y1 = y + get_scale_internal() * height + 1.0f;
    if (x < m_text_bounds.x0)
        m_text_bounds.x0 = x;
    if (y < m_text_bounds.y0)
        m_text_bounds.y0 = y;
    if (x1 > m_text_bounds.x1)
        m_text_bounds.x1 = x1;
    if (y1 > m_text_bounds.y1)
        m_text_bounds.y1 = y1;
}

// @address 0x18afd0 ?draw_partial@c_draw_string@@AAAXPAVc_font_cache@@PAUs_parse_string_state@1@J@Z
void c_draw_string::draw_partial(c_font_cache *font_cache, s_parse_string_state *parse_state, long length)
{
    float scale = get_scale_internal();
    real_rectangle2d visible;
    visible.x0 = -32768.0f;
    visible.y0 = -32768.0f;
    visible.x1 = 32767.0f;
    visible.y1 = 32767.0f;
    if (scale > 1.0f)
        scale = 1.0f;
    if (m_bounds.x0 > visible.x0)
        visible.x0 = m_bounds.x0;
    if (m_bounds.x1 < visible.x1)
        visible.x1 = m_bounds.x1;
    if (m_bounds.y0 > visible.y0)
        visible.y0 = m_bounds.y0;
    if (m_bounds.y1 < visible.y1)
        visible.y1 = m_bounds.y1;
    if (m_clip_bounds.x0 > visible.x0)
        visible.x0 = m_clip_bounds.x0;
    if (m_clip_bounds.x1 < visible.x1)
        visible.x1 = m_clip_bounds.x1;
    if (m_clip_bounds.y0 > visible.y0)
        visible.y0 = m_clip_bounds.y0;
    if (m_clip_bounds.y1 < visible.y1)
        visible.y1 = m_clip_bounds.y1;

    if (visible.x0 < visible.x1 && visible.y0 < visible.y1)
    {
        while (parse_state->string_index < length)
        {
            const s_font_character *font_character = 0;
            parse_string(parse_state);
            long character_code = parse_state->character.character;
            if (character_code == 0)
                break;
            if (parse_state->token == _parse_token_character && draw_string_utf32_isprint(character_code))
            {
                utf32 character_arg = { character_code };
                if (precache_character(font_cache, m_font_id, character_arg, &font_character))
                {
                    float tx = 0.0f;
                    float ty = 0.0f;
                    float width = (float)font_character->width;
                    float height = (float)font_character->height;
                    long glyph_code = parse_state->character.character;
                    float kerning = get_scale_internal() *
                        (float)font_get_kerning_pair_offset(m_font_header, parse_state->previous_character.character, glyph_code);
                    float bearing = get_scale_internal() * (float)font_character->bearing;
                    float x = m_cursor.x + bearing + kerning;
                    float y = m_cursor.y - get_scale_internal() * (float)font_character->top;
                    if ((x + width) * scale > visible.x1)
                        width = visible.x1 - x;
                    if (x < visible.x0)
                    {
                        tx = visible.x0 - x;
                        x = visible.x0;
                        width -= tx;
                    }
                    if ((y + height) * scale > visible.y1)
                        height = visible.y1 - y;
                    if (y < visible.y0)
                    {
                        ty = visible.y0 - y;
                        y = visible.y0;
                        height -= ty;
                    }
                    if (width > 0.0f && height > 0.0f)
                    {
                        if (m_flags.test(_text_flag_compute_bounds))
                        {
                            text_bounds_draw_character(x, y, width, height);
                        }
                        else
                        {
                            real_argb_color color;
                            draw_string_real_argb_colors_multiply(&parse_state->initial_color, &parse_state->color, &color);
                            unsigned long pixel = real_argb_color_to_pixel32(&color);
                            unsigned long shadow_pixel = real_argb_color_to_pixel32(&m_shadow_color);
                            if (draw_string_utf32_ismonochrome(parse_state->character))
                                pixel |= 0x00FFFFFF;
                            utf32 glyph = { glyph_code };
                            draw_character(font_cache, m_font_id, glyph, pixel, shadow_pixel, x, y, tx, ty, width, height);
                        }
                    }
                    m_cursor.x += get_scale_internal() * (float)font_character->advance + bearing + kerning;
                }
            }
        }
    }
}







// @address 0x18a7b0 ?draw_internal@c_draw_string@@AAA_NPAVc_font_cache@@PBUutf32@@@Z
bool c_draw_string::draw_internal(c_font_cache *font_cache, const utf32 *string)
{
    bool result = false;
    s_parse_string_state parse_state;
    real_rectangle2d bounds = m_bounds;
    short tab_stop_index;
    short line_index;
    short wrapped_line_index;
    short max_wrapped_line_index;
    bool ready;


    if (m_flags.test(_text_flag_draw_more))
    {
        memcpy(&parse_state, &m_saved_parse_state, sizeof(parse_state));
        float cursor_x = m_cursor.x;
        e_font_id font = m_font_id;
        tab_stop_index = m_saved_tab_stop_index;
        line_index = m_saved_line_count;
        wrapped_line_index = m_saved_maximum_lines;
        max_wrapped_line_index = m_saved_indent;
        bounds.x0 = cursor_x;
        parse_state.string_index = 0;
        parse_state.string = string;
        m_font_header = font_get_header(font);
        ready = m_font_header != 0;
    }
    else
    {
        m_cursor.x = bounds.x0;
        m_cursor.y = bounds.y0;
        tab_stop_index = 0;
        line_index = 0;
        wrapped_line_index = 0;
        max_wrapped_line_index = 0;
        ready = parse_string_new(m_font_id, string, &parse_state);
    }

    if (ready)
    {
        do
        {
            real_rectangle2d line_bounds = bounds;
            e_text_justification justification = m_justification;

            if (m_tab_stop_count > 0)
            {
                if (tab_stop_index != 0)
                {
                    line_bounds.x0 = get_scale_internal() * (float)((short *)m_tab_stops)[tab_stop_index - 1];
                }
                else
                {
                    if (line_index != 0)
                        line_bounds.x0 += get_scale_internal() * (float)m_paragraph_indent;
                    else
                        line_bounds.x0 += get_scale_internal() * (float)m_initial_indent;
                }
                if (tab_stop_index < m_tab_stop_count)
                    line_bounds.x1 = get_scale_internal() * (float)((short *)m_tab_stops)[tab_stop_index];
            }
            else
            {
                if (line_index != 0)
                    line_bounds.x0 += get_scale_internal() * (float)m_paragraph_indent;
                else
                    line_bounds.x0 += get_scale_internal() * (float)m_initial_indent;
            }

            const s_font_header *header = m_font_header;
            m_cursor.x = line_bounds.x0 + get_scale_internal() * (float)header->leading_width;
            m_cursor.y = line_bounds.y0 +
                get_scale_internal() *
                    (float)((header->leading_height + header->descending_height + m_line_spacing + header->ascending_height) *
                            ((long)wrapped_line_index + (long)line_index) +
                        header->ascending_height);

            s_parse_string_state line_start_state;
            memcpy(&line_start_state, &parse_state, sizeof(parse_state));
            s_line_results line_results;
            line_results = build_one_line_of_text_to_draw(font_cache, &parse_state, &line_bounds, get_scale_internal());
            parse_state.string_index = line_results.index;

            switch (justification)
            {
            case _text_justification_right:
                m_cursor.x = (line_bounds.x1 - line_bounds.x0) + line_bounds.x0 - line_results.width - (float)m_font_header->leading_width;
                break;
            case _text_justification_center:
                m_cursor.x = (line_bounds.x1 - line_bounds.x0 - line_results.width) * 0.5f + line_bounds.x0;
                break;
            }

            if (m_cursor.y < line_bounds.y1)
            {
                real_rectangle2d saved_bounds = m_bounds;
                m_bounds = line_bounds;
                draw_partial(font_cache, &line_start_state, line_results.index);
                m_bounds = saved_bounds;
            }

            long token = parse_state.token;
            switch (token)
            {
            case _parse_token_character:
                wrapped_line_index = (short)(wrapped_line_index + 1);
                if (wrapped_line_index > max_wrapped_line_index)
                    max_wrapped_line_index = wrapped_line_index;
                break;
            case _parse_token_tab:
                if (tab_stop_index < m_tab_stop_count)
                {
                    tab_stop_index++;
                    wrapped_line_index = 0;
                }
                break;
            case _parse_token_new_line:
                tab_stop_index = 0;
                line_index = (short)(max_wrapped_line_index + line_index + 1);
                max_wrapped_line_index = 0;
                wrapped_line_index = 0;
                break;
            case _parse_token_end:
            case _parse_token_style:
            case _parse_token_font:
            case _parse_token_color:
                break;
            case _parse_token_justification:
                wrapped_line_index = 0;
                break;
            default:
                // match: 0x18a7b0: unreachable hint omits dead branch check
                __assume(0);
            }

            bounds = m_bounds;
        } while (parse_state.token != _parse_token_end);

        memcpy(&m_saved_parse_state, &parse_state, sizeof(parse_state));
        result = true;
        m_saved_tab_stop_index = tab_stop_index;
        m_saved_line_count = line_index;
        m_saved_maximum_lines = wrapped_line_index;
        m_saved_indent = max_wrapped_line_index;
    }
    return result;
}
static __forceinline real_rectangle2d *draw_string_offset_real_rectangle2d(real_rectangle2d *rectangle, float x, float y)
{
    rectangle->x1 += x;
    rectangle->y0 += y;
    rectangle->x0 += x;
    rectangle->y1 += y;
    return rectangle;
}

// @address 0x18a520 ?draw@c_draw_string@@UAA_NPAVc_font_cache@@PBUutf32@@@Z
bool c_draw_string::draw(c_font_cache *font_cache, const utf32 *string)
{
    // match: 0x18a520: value range assumption guides branch lowering
    __assume(font_cache != 0);
    bool precached = true;
    bool result = false;
    if (m_flags.test(_text_flag_precache))
        precached = precache_utf32_string_for_font(font_cache, string, m_font_id);
    if (precached)
    {
        real_rectangle2d saved_bounds = m_bounds;
        real_rectangle2d saved_clip_bounds = m_clip_bounds;
        float clip_width = m_clip_bounds.x1 - m_clip_bounds.x0;
        float clip_height = m_clip_bounds.y1 - m_clip_bounds.y0;
        bool center = m_flags.test(_text_flag_center_vertically);
        bool align_bottom = m_flags.test(_text_flag_align_bottom);
        if (clip_width <= 0.0f || clip_height <= 0.0f)
            m_clip_bounds = m_bounds;
        m_shadow_color.alpha = m_shadow_color.alpha > m_color.alpha ? m_color.alpha : m_shadow_color.alpha;
        if (center || align_bottom)
        {
            m_flags.set(_text_flag_center_vertically, false);
            m_flags.set(_text_flag_align_bottom, false);
            real_rectangle2d aligned_bounds;
            adjust_bounds_for_vertical_alignment(font_cache, string, &aligned_bounds, center, align_bottom);
            m_flags.set(_text_flag_center_vertically, center);
            m_flags.set(_text_flag_align_bottom, align_bottom);
            float y = aligned_bounds.y0 - m_bounds.y0;
            draw_string_offset_real_rectangle2d(&m_bounds, 0.0f, y);
            draw_string_offset_real_rectangle2d(&m_clip_bounds, 0.0f, y);
        }
        result = draw_internal(font_cache, string);
        if (!(saved_bounds.x1 - saved_bounds.x0 < 0.0f) && !(saved_bounds.y1 - saved_bounds.y0 < 0.0f))
            m_bounds = saved_bounds;
        if (!(saved_clip_bounds.x1 - saved_clip_bounds.x0 < 0.0f) && !(saved_clip_bounds.y1 - saved_clip_bounds.y0 < 0.0f))
            m_clip_bounds = saved_clip_bounds;
    }
    return result;
}
