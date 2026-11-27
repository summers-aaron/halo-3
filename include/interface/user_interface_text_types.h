#ifndef INTERFACE_USER_INTERFACE_TEXT_TYPES_H
#define INTERFACE_USER_INTERFACE_TEXT_TYPES_H
#include "math/real_math_types.h"
#include <string.h>
class c_font_cache;
struct s_user_interface_text_render_data;
// Shared text layout for bounds and widget rendering, with rotation origin, rotation,
// scroll/layout origin, and scale.
struct s_user_interface_text_layout
{
    unsigned long m_flags;
    long m_argb_color;
    long m_font;
    unsigned long m_controller_index;
    long m_drop_shadow_style;
    long m_style;
    long m_justification;
    short m_tabs[16];
    long m_tab_count;
    real_point2d m_rotation_origin;
    float m_rotation;
    real_point2d layout_origin;
    float m_scale;
};

class c_user_interface_text : public s_user_interface_text_layout
{
public:
    __forceinline c_user_interface_text()
    {
        m_flags = 4;
        m_argb_color = -1;
        m_controller_index = -1;
        m_font = 0;
        m_drop_shadow_style = 2;
        m_style = -1;
        m_justification = 0;
        memset(m_tabs, 0, sizeof(m_tabs));
        memset(&m_tab_count, 0, sizeof(m_tab_count) + sizeof(m_rotation_origin));
        m_rotation = 0.0f;
        memset(&layout_origin, 0, sizeof(layout_origin));
        m_scale = 1.0f;
    }
    virtual ~c_user_interface_text() {}
    virtual void set_string(const wchar_t *string, bool parse, long parameter) = 0;
    virtual const wchar_t *get_string() = 0;
    virtual void update(long user_index) = 0;
    __forceinline bool compute_text_bounds(
        c_font_cache *font_cache, long user, const rectangle2d *bounds, float scale, const rectangle2d *clip, rectangle2d *caret_bounds)
    {
        rectangle2d text_bounds;
        long length = wcslen(get_string());
        return compute_bounds_internal(font_cache, user, bounds, scale, clip, (short)length, &text_bounds, caret_bounds);
    }
    static void render(s_user_interface_text_render_data *, const rectangle2d *);
protected:
    bool compute_bounds_internal(c_font_cache *font_cache, long user, const rectangle2d *bounds, float scale, const rectangle2d *clip,
        short caret, rectangle2d *text_bounds, rectangle2d *caret_bounds);
public:
    bool has_overflowed() const { return (m_flags & 8) != 0; }
    __forceinline void set_render_uppercase(bool value) { if (value) m_flags |= 1; else m_flags &= ~1ul; }
    __forceinline void set_unknown_flag2(bool value) { if (value) m_flags |= 4; else m_flags &= ~4ul; }
    __forceinline void set_font(long font) { m_font = font; }
    __forceinline void set_drop_shadow_style(long style) { m_drop_shadow_style = style; }
    __forceinline void set_justification(long justification) { m_justification = justification; }
    __forceinline bool get_render_uppercase() const { return (m_flags & 1) != 0; }
    __forceinline bool get_wrap_horizontally() const { return (m_flags & 4) != 0; }
    __forceinline bool get_align_vertically() const { return (m_flags & 2) != 0; }
    __forceinline long get_font() const { return m_font; }
    __forceinline long get_style() const { return m_style; }
    __forceinline long get_justification() const { return m_justification; }
    __forceinline long get_drop_shadow_style() const { return m_drop_shadow_style; }
    __forceinline void get_tab_stops(short *tab_stops, short *in_out_count)
    {
        *in_out_count = (short)(m_tab_count > *in_out_count ? *in_out_count : m_tab_count);
        if (*in_out_count > 0)
            memcpy(tab_stops, m_tabs, *in_out_count * sizeof(short));
    }

};

#endif
