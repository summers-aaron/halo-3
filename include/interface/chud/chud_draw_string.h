#ifndef INTERFACE_CHUD_CHUD_DRAW_STRING_H
#define INTERFACE_CHUD_CHUD_DRAW_STRING_H
#include "text/draw_string_types.h"
#include "interface/chud/chud_navpoints.h"
struct s_widget_geometry;
class c_chud_draw_string : public c_draw_string
{
    friend bool chud_compute_text_geometry(chud_navpoints_widget_geometry *, const wchar_t *, long, unsigned char, float, const real_rectangle2d *);
public:
    c_chud_draw_string();
    virtual ~c_chud_draw_string() {}
    virtual bool predict_character(c_font_cache *, e_font_id, utf32);
    virtual bool precache_character(c_font_cache *, e_font_id, utf32, const s_font_character **);
private:
    virtual void draw_character(c_font_cache *, e_font_id, utf32, unsigned long, unsigned long, float, float, float, float, float, float);
    s_widget_geometry *m_geometry;
    float m_scale;
    real_rectangle2d m_clip;
};
#endif
