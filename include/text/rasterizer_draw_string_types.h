#ifndef TEXT_RASTERIZER_DRAW_STRING_TYPES_H
#define TEXT_RASTERIZER_DRAW_STRING_TYPES_H

#include "text/draw_string_types.h"
#include <string.h>

// predict_character and precache_character share their bodies with c_chud_draw_string
// (identical code folded by the linker), so the image holds one copy of each.
class c_rasterizer_draw_string : public c_draw_string
{
public:
    c_rasterizer_draw_string();
    void set_rotation(float rotation);
    static bool begin_rendering();
    virtual bool predict_character(c_font_cache *, e_font_id, utf32);
    virtual bool precache_character(c_font_cache *, e_font_id, utf32, const s_font_character **);
    virtual bool draw(c_font_cache *font_cache, const utf32 *string);
    virtual bool draw(c_font_cache *font_cache, const wchar_t *string);
private:
    void add_character_to_glyph_cache(c_font_cache *font_cache, e_font_id font, utf32 character, unsigned long color,
        unsigned long shadow_color, float x, float y, float tx, float ty, float width, float height);
    bool render_and_flush_glyph_cache(c_font_cache *font_cache);
    bool render_glyphs_with_color_and_offset(c_font_cache *font_cache, unsigned long color, const real_vector2d *offset);
    __forceinline bool render_glyph_drop_shadows(c_font_cache *font_cache)
    {
        real_vector2d offset = { 1.0f, 1.0f };
        return render_glyphs_with_color_and_offset(font_cache, m_glyph_group_render_data.vertex_count, &offset);
    }
    __forceinline bool render_glyph_outline_shadows(c_font_cache *font_cache)
    {
        real_vector2d offsets[4] = { { 1.0f, 1.0f }, { -1.0f, 1.0f }, { -1.0f, -1.0f }, { 1.0f, -1.0f } };
        bool success = true;
        for (long i = 0; success && (unsigned long)i < 4; ++i)
            success &= render_glyphs_with_color_and_offset(font_cache, m_glyph_group_render_data.vertex_count, &offsets[i]);
        return success;
    }
    __forceinline bool render_glyphs(c_font_cache *font_cache)
    {
        real_vector2d offset = { 0.0f, 0.0f };
        return render_glyphs_with_color_and_offset(font_cache, m_glyph_group_render_data.glyph_count, &offset);
    }
    virtual void draw_character(c_font_cache *font_cache, e_font_id font, utf32 character, unsigned long color,
        unsigned long shadow_color, float x, float y, float tx, float ty, float width, float height);
public:
    struct s_glyph_render_data
    {
        unsigned long character;
        float x;
        float y;
        float tx;
        float ty;
        float width;
        float height;
    };

    struct s_glyph_group_render_data
    {
        s_glyph_group_render_data() :
            texture_index(-1),
            glyph_count(0),
            vertex_count(0)
        {
            memset(glyphs, 0, sizeof(glyphs));
            index_count = 0;
        }

        __forceinline void reset()
        {
            texture_index = -1;
            glyph_count = 0;
            vertex_count = 0;
            index_count = 0;
        }

        __forceinline bool is_full() const
        {
            return index_count == sizeof(glyphs) / sizeof(glyphs[0]);
        }

        long texture_index;
        long glyph_count;
        long vertex_count;
        s_glyph_render_data glyphs[64];
        long index_count;
    };

public:
    real_point2d m_rotation_origin;
    float m_rotation;
    float m_sine;
    float m_cosine;
    bool m_glyph_cache_dirty;
    s_glyph_group_render_data m_glyph_group_render_data;
};

#endif // TEXT_RASTERIZER_DRAW_STRING_TYPES_H
