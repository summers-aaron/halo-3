#ifndef INCLUDE_TEXT_DRAW_STRING_TYPES_H
#define INCLUDE_TEXT_DRAW_STRING_TYPES_H

#include "text/font_id_types.h"
#include "interface/chud/utf32_types.h"
#include "math/real_math_types.h"
#include "cseries/cseries_wrapped.h"
#include "text/draw_string.h"
#include "interface/chud/chud_navpoints.h"
#include "interface/chud/chud_draw.h"

class c_font_cache;
struct s_font_character;
struct s_font_header;
enum e_text_style
{
    _text_style_none = -1,
    _text_style_plain = 0,
    _text_style_bold,
    _text_style_italic,
    _text_style_condense,
    k_text_style_count
};

class c_draw_string {
    friend float chud_draw_get_string_width(long, const wchar_t *);
    friend bool chud_compute_text_geometry(chud_navpoints_widget_geometry *, const wchar_t *, long, unsigned char, float, const real_rectangle2d *);
    friend void subtitle_render();
    friend void cinematic_render(bool unused, bool render_titles);
    friend void closed_caption_render();
    friend short closed_caption_get_line_height(c_draw_string *draw);
public:
    enum e_text_flags
    {
        _text_flag_wrap_horizontally = 0,
        _text_flag_center_vertically = 2,
        _text_flag_align_bottom = 3,
        _text_flag_compute_bounds = 4,
        _text_flag_precache = 5,
        _text_flag_draw_more = 6,
        k_text_flags_count = 7
    };

    c_draw_string();
    virtual ~c_draw_string() {}
    virtual bool draw(c_font_cache *font_cache, const char *string);
    virtual bool draw(c_font_cache *font_cache, const wchar_t *string);
    virtual bool draw(c_font_cache *font_cache, const utf32 *string);
    virtual bool predict_character(c_font_cache *font_cache, e_font_id font, utf32 character) = 0;
    virtual bool precache_character(c_font_cache *font_cache, e_font_id font, utf32 character, const s_font_character **out_character) = 0;
    virtual void draw_character(c_font_cache *font_cache, e_font_id font, utf32 character, unsigned long color, unsigned long shadow_color,
        float x, float y, float tx, float ty, float width, float height) {}

    void set_bounds(const rectangle2d *bounds);
    void set_font(e_font_id font);
    void set_shadow_color(const real_argb_color *color);
    void set_color_deprecated(const real_argb_color *color, bool allow_overbright);
    bool draw_more(c_font_cache *font_cache, const char *string);
    bool compute_utf32_string_bounds(c_font_cache *font_cache, const utf32 *string, rectangle2d *bounds, rectangle2d *cursor_bounds);
    bool compute_wchar_string_bounds(c_font_cache *font_cache, const wchar_t *string, rectangle2d *bounds, rectangle2d *cursor_bounds);
    bool precache_wchar_string_for_font(c_font_cache *font_cache, const wchar_t *string, e_font_id font);
    bool precache_utf32_string_for_font(c_font_cache *font_cache, const utf32 *string, e_font_id font);

    float get_scale() const { return m_scale; }
    float get_scale_internal() const { return m_scale * m_display_scale; }
    void set_scale(float scale) { m_scale = scale; }
    void set_display_resolution_scale_adjustment(float scale) { m_display_scale = scale; }
    long get_drop_shadow_style() const { return m_initial_line; }
    void set_style(e_text_style style) { m_style = style; }
    e_text_justification get_justification() const { return m_justification; }
    real_argb_color *get_color(real_argb_color *color) const { *color = m_color; return color; }

private:
    enum e_parse_token
    {
        _parse_token_none = -1,
        _parse_token_end = 0,
        _parse_token_new_line,
        _parse_token_tab,
        _parse_token_justification,
        _parse_token_style,
        _parse_token_font,
        _parse_token_color,
        _parse_token_character
    };

    struct s_parse_string_state
    {
        long string_index;
        const utf32 *string;
        long token;
        long previous_token;
        utf32 character;
        utf32 previous_character;
        e_font_id font;
        e_font_id previous_font;
        real_argb_color initial_color;
        real_argb_color color;
        e_text_justification justification;
        e_text_justification previous_justification;
        bool can_break_line;
    };

    void adjust_bounds_for_vertical_alignment(c_font_cache *font_cache, const utf32 *string, real_rectangle2d *bounds, bool center, bool align_bottom);
    bool parse_string_new(e_font_id font, const utf32 *string, s_parse_string_state *parse_state);
    void parse_string(s_parse_string_state *parse_state);
    bool recache_font_header();

    struct s_line_results
    {
        float width;
        long index;
    };

    s_line_results build_one_line_of_text_to_draw(c_font_cache *font_cache, s_parse_string_state *parse_state, const real_rectangle2d *bounds, float scale);
    void draw_partial(c_font_cache *font_cache, s_parse_string_state *parse_state, long length);
    void text_bounds_draw_character(float x, float y, float width, float height);
    bool draw_internal(c_font_cache *font_cache, const utf32 *string);

public:
    c_flags<e_text_flags, long, k_text_flags_count> m_flags;
    e_font_id m_font_id;
    const s_font_header *m_font_header;
    e_text_style m_style;
    e_text_justification m_justification;
    long m_initial_line;
    real_argb_color m_color;
    real_argb_color m_shadow_color;
    float m_scale;
    float m_display_scale;
    short m_line_spacing;
    union
    {
        short m_tab_stop_count;
        unsigned short m_tab_count;
    };
    long m_tab_stops[8];
    real_rectangle2d m_bounds;
    real_rectangle2d m_text_bounds;
    real_rectangle2d m_clip_bounds;
    real_point2d m_cursor;
    long m_maximum_lines;
    long m_line_index;
    short m_initial_indent;
    short m_paragraph_indent;
    s_parse_string_state m_saved_parse_state;
    short m_saved_tab_stop_index;
    short m_saved_line_count;
    short m_saved_maximum_lines;
    short m_saved_indent;
};

// The draw string the user interface measures and precaches text with: it predicts and
// precaches through the rasterizer glyph cache (the bodies it shares with
// c_rasterizer_draw_string) and draws nothing. March builds c_draw_string itself here.
class c_user_interface_draw_string : public c_draw_string
{
public:
    __forceinline c_user_interface_draw_string() {}
    virtual bool predict_character(c_font_cache *font_cache, e_font_id font, utf32 character);
    virtual bool precache_character(c_font_cache *font_cache, e_font_id font, utf32 character, const s_font_character **out_character);
};
#endif
