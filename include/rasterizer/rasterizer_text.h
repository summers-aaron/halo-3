#ifndef RASTERIZER_RASTERIZER_TEXT_H
#define RASTERIZER_RASTERIZER_TEXT_H

#include "interface/chud/utf32_types.h"
#include "text/font_id_types.h"
#include "rasterizer/simple_font_types.h"
#include "text/draw_string_types.h"
#include <string.h>

namespace simple_font
{
extern s_font_data g_font_6x10;
extern s_font_data g_font_4x6;
extern s_font_data *g_fonts[2];
extern s_simple_font_globals g_simple_font_globals;
}

// Functions defined in tu/graphicslibxbox/rasterizer_text.cpp.

extern "C" int rasterizer_text_cache_initialize(void);

class c_font_cache;
struct s_rasterizer_text_bitmap_slot;

extern "C" void rasterizer_text_flush_all_glyphs(void);
extern "C" void rasterizer_text_dispose(void);
extern "C" void rasterizer_text_reset_cache(void);
extern "C" int rasterizer_text_find_entry(c_font_cache *font_cache, unsigned long key);
extern "C" void rasterizer_text_initialize_bitmap_slot(struct s_rasterizer_text_bitmap_slot *slot);
extern "C" bool simple_font_begin_rendering(float scale, bool shadow_enabled);
extern "C" void simple_font_render_text(simple_font::s_font_data *font, long x, long y, long column_limit, long flags,
    unsigned long color, const char *string, unsigned long count, bool apply_shadow);

struct s_font_character;

extern "C" long rasterizer_text_allocate_and_upload_glyph( c_font_cache *font_cache, e_font_id font, utf32 character,
    unsigned long key, long data_index, const s_font_character *font_character);

class c_rasterizer_texture_ref;

extern "C" void *rasterizer_texture_lock_bits(const c_rasterizer_texture_ref &texture_ref, int overwrite, long *pitch);

// The draw string the simple debug font renders through: characters collect in a
// buffer that flushes to simple_font_render_text. March has no such class.
class c_simple_font_draw_string : public c_draw_string
{
public:
    // match: 0x44a3b8: render_draw_pregame_text keeps the base ctor call, vptr store and field clears inline; a plain inline ctor stays an out-of-line call
    __forceinline c_simple_font_draw_string() :
        m_color(0),
        m_x(0),
        m_y(0),
        m_count(0)
    {
        memset(m_text, 0, sizeof(m_text));
    }
    virtual bool draw(c_font_cache *font_cache, const char *string)
    {
        return c_draw_string::draw(font_cache, string);
    }
    virtual bool draw(c_font_cache *font_cache, const wchar_t *string)
    {
        return c_draw_string::draw(font_cache, string);
    }
    virtual bool draw(c_font_cache *font_cache, const utf32 *string);
    virtual bool predict_character(c_font_cache *font_cache, e_font_id font, utf32 character);
    virtual bool precache_character(c_font_cache *font_cache, e_font_id font, utf32 character, const s_font_character **out_character);
    void flush();

private:
    virtual void draw_character(c_font_cache *font_cache, e_font_id font, utf32 character, unsigned long color, unsigned long shadow_color,
        float x, float y, float tx, float ty, float width, float height);

    unsigned long m_color;
    long m_x;
    long m_y;
    unsigned long m_count;
    char m_text[256];
};
#endif // RASTERIZER_RASTERIZER_TEXT_H
