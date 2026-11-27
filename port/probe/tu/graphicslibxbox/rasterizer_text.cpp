// graphicslibxbox/rasterizer_text.obj: from the records 0x18a118 0x18a1e8 0x18b558 0x191b20 0x191be8 0x191c80 0x191eb0 0x191ee0 0x191fa8 0x192118 0x454c38 0x454cb8 0x454d98 0x454e40 0x454f20 0x4550d0 0x4556f0 0x4556f8 0x4557b8 0x4557f8 0x455920 0x455a58 0x46a628 0x46a7f0 0x46ac20 0x46adf0 0x46aee0
// @compiler xdk6021
// @flags /TP /O2 /Oc
// @address 0x949acc ?g_thread_crashed@@3PAVc_interlocked_long@@A
// @address 0x73042c g_interface_aspect_ratio_scaling
// @address 0x7c6178 g_rasterizer_sampler_filter_modes
// @address 0x195e88 rasterizer_device_set_sampler_filter_modes
// @address 0x18a118 ?predict_character@c_chud_draw_string@@UAA_NPAVc_font_cache@@W4e_font_id@@Uutf32@@@Z
// @address 0x18a118 ?predict_character@c_rasterizer_draw_string@@UAA_NPAVc_font_cache@@W4e_font_id@@Uutf32@@@Z
// @address 0x18a118 ?predict_character@c_user_interface_draw_string@@UAA_NPAVc_font_cache@@W4e_font_id@@Uutf32@@@Z
// @address 0x18aea0 ?precache_character@c_user_interface_draw_string@@UAA_NPAVc_font_cache@@W4e_font_id@@Uutf32@@PAPBUs_font_character@@@Z
// @address 0x4ae58 ??_7c_user_interface_draw_string@@6B@
// @address 0x5bec98 ?draw_character@c_draw_string@@UAAXPAVc_font_cache@@W4e_font_id@@Uutf32@@KKMMMMMM@Z
// @address 0x18aea0 ?precache_character@c_rasterizer_draw_string@@UAA_NPAVc_font_cache@@W4e_font_id@@Uutf32@@PAPBUs_font_character@@@Z
// @address 0x192118 ??0c_rasterizer_draw_string@@QAA@XZ
// @address 0x189498 ??0c_draw_string@@QAA@XZ
// @address 0x10ec ??_7c_simple_font_cache@@6B@
// @address 0x1c1258 ?character_data_get_hardware_block_index@c_simple_font_cache@@UAAJW4e_character_data_index@@@Z
// @address 0x5bec98 ?character_data_set_hardware_block_index@c_simple_font_cache@@UAAXW4e_character_data_index@@J@Z
// @address 0x5bec98 ?character_data_touch@c_simple_font_cache@@UAAXW4e_character_data_index@@@Z
// @address 0x42d298 ?get_pixels@c_simple_font_cache@@UAA_NW4e_font_id@@Uutf32@@PBUs_font_character@@PAG@Z
// @address 0x42d380 ?font_character_predict@c_simple_font_cache@@UAA_NW4e_font_id@@Uutf32@@@Z
// @address 0x46adf0 ?get_character@c_simple_font_cache@@UAA_NW4e_font_id@@Uutf32@@_NPAKPAJPAPBUs_font_character@@@Z
// @address 0x18a1e8 rasterizer_text_cache_initialize
// @address 0x191be8 simple_font_begin_rendering
// @address 0x4557f8 ?add_character_to_glyph_cache@c_rasterizer_draw_string@@AAAXPAVc_font_cache@@W4e_font_id@@Uutf32@@KKMMMMMM@Z

#include "text/font_cache_lock.h"
#include "multithreading/synchronized_value.h"
#include "math/real_constant_pool_access.h"
#include "text/font_cache_thread_state_types.h"
#include "text/font_id_types.h"
#include "rasterizer/rasterizer_text_globals.h"
#include "text/draw_string_globals.h"
#include "rasterizer/rasterizer_sampler_filter_state.h"
#include "rasterizer/rasterizer_resource_definitions.h"
#include "rasterizer/rasterizer_main_globals.h"
#include "math/real_math_types.h"
#include "bitmaps/bitmap_data.h"
#include "bitmaps/bitmaps.h"
#include <xtl.h>
#include <math.h>
#include <string.h>
#include "interface/chud/chud_draw.h"
#include "interface/interface_constants.h"
#include "memory/lruv_cache.h"
#include "rasterizer/dx9/rasterizer_dx9_dynamic_geometry.h"
#include "rasterizer/rasterizer_text.h"
#include "rasterizer/rasterizer_states.h"
#include "interface/chud/utf32_types.h"
#include "rasterizer/rasterizer_text_glyph_cache_types.h"
#include "rasterizer/rasterizer_render_globals_types.h"
#include "rasterizer/rasterizer_vertex_screen_types.h"
#include "text/character_data_index_types.h"
#include "rasterizer/simple_font_types.h"
#include "text/font_character_types.h"
#include "text/font_cache_types.h"
#include "interface/chud/chud_draw_string.h"
#include "rasterizer/rasterizer_textures_cache.h"
#include "cseries/async_helpers_internal.h"
#include "cseries/pointer_utilities.h"
#include "text/rasterizer_draw_string_types.h"
#include "math/real_constant_pool_ten.h"
#include "rasterizer/rasterizer.h"
#include "rasterizer/rasterizer_sampler_texture_calls.h"
#include "objects/object_reference_types.h"
#include "render/views/player_view_types.h"
#include "rasterizer/rasterizer_vertex_definitions.h"
#include "render_methods/render_method_definitions.h"
#include "text/draw_string.h"
#include "text/font_package_types.h"
#include "../../extra/hd_text_atlas.h"

// @address 0x10b0 k_font_cache_vtable

struct s_widget_geometry;

// @address 0x7bf7d0 ?g_font_6x10@simple_font@@3Us_font_data@1@A
simple_font::s_font_data simple_font::g_font_6x10 = { false, 6, 10, 16, 32, 127, 1.0f, 128, 64, 0, -1, 0, 96, 64,
    ".............#.#............................#......#...#......................................#."
    "........#....#.#..........#..........##.....#.....#.....#.....................................#."
    "........#....#.#...#.#...####..#...##..#....#....#.......#....#.....#........................#.."
    "........#.........#####.#.#...#.#.#.#..#.........#.......#..#.#.#...#........................#.."
    "........#..........#.#...###...#.#...##..........#.......#...###..#####.......#####.........#..."
    "........#..........#.#....#.#...#.#.#..#.#.......#.......#..#.#.#...#.......................#..."
    "..................#####.####...#.#.##...#........#.......#....#.....#......................#...."
    "........#..........#.#....#...#...#..###.#........#.....#.................#...........#....#...."
    "...................................................#...#.................#................#....."
    "................................................................................................"
    "................................................................................................"
    ".###....#....###...###.....#..#####...##..#####..###...###.................................###.."
    "#...#..##...#...#.#...#...##..#......#........#.#...#.#...#.................#.......#.....#...#."
    "#...#...#.......#.....#..#.#..#.....#........#..#...#.#...#...#.....#.....##..#####..##.......#."
    "#.#.#...#.....##....##..#..#..####..####.....#...###...####.............##.............##....#.."
    "#...#...#....#........#.#####.....#.#...#...#...#...#.....#...............##..#####..##.....#..."
    "#...#...#...#.....#...#....#..#...#.#...#...#...#...#....#....#.....#.......#.......#..........."
    ".###...###..#####..###.....#...###...###...#.....###...##..........#........................#..."
    "................................................................................................"
    "................................................................................................"
    "................................................................................................"
    ".####...#...####...###..###...#####.#####..###..#...#..###....###.#...#.#.....#...#.##..#..###.."
    "#....#..#...#...#.#...#.#..#..#.....#.....#...#.#...#...#.......#.#..#..#.....#...#.##..#.#...#."
    "#.##.#.#.#..#...#.#.....#...#.#.....#.....#.....#...#...#.......#.#.#...#.....##.##.#.#.#.#...#."
    "#.##.#.#.#..####..#.....#...#.####..####..#..##.#####...#.......#.##....#.....##.##.#.#.#.#...#."
    "#.####.###..#...#.#.....#...#.#.....#.....#...#.#...#...#.......#.#.#...#.....#.#.#.#.#.#.#...#."
    "#.....#...#.#...#.#...#.#..#..#.....#.....#...#.#...#...#.......#.#..#..#.....#.#.#.#..##.#...#."
    ".######...#.####...###..###...#####.#......####.#...#..###..####..#...#.#####.#...#.#..##..###.."
    "................................................................................................"
    "................................................................................................"
    "...................................................................###..#......###.............."
    "####...###..####...###..#####.#...#.#...#.#...#.#...#.#...#.#####..#....#........#....#........."
    "#...#.#...#.#...#.#...#...#...#...#.#...#.#.#.#.#...#.#...#.....#..#.....#.......#...#.#........"
    "#...#.#...#.#...#.#.......#...#...#.#...#.#.#.#..#.#..#...#....#...#.....#.......#..#...#......."
    "####..#...#.####...###....#...#...#..#.#..#.#.#...#....###....#....#......#......#.............."
    "#.....#...#.#...#.....#...#...#...#..#.#...#.#...#.#....#....#.....#......#......#.............."
    "#.....#..##.#...#.#...#...#...#...#...#....#.#..#...#...#...#......#.......#.....#.............."
    "#......####.#...#..###....#....###....#....#.#..#...#...#...#####..#.......#.....#.............."
    "...................................................................###......#..###........######"
    "................................................................................................"
    "............#...............#.........###.......#..................#.....##....................."
    "..#.........#...............#........#..........#.......#......#...#......#....................."
    "...#........#...............#........#..........#..................#......#....................."
    ".......###..####...####..####..###..####...###..####...##.....##...#..#...#...##.#..####...###.."
    "..........#.#...#.#.....#...#.#...#..#....#...#.#...#...#......#...#.#....#...#.#.#.#...#.#...#."
    ".......####.#...#.#.....#...#.#####..#....#...#.#...#...#......#...##.....#...#.#.#.#...#.#...#."
    "......#...#.#...#.#.....#...#.#......#....#...#.#...#...#......#...#.#....#...#.#.#.#...#.#...#."
    ".......####.####...####..####..####..#.....####.#...#...#......#...#..#...#...#.#.#.#...#..###.."
    "..............................................#................#................................"
    "...........................................###..............###................................."
    ".....................................................................#....#....#................"
    ".........................#..........................................#.....#.....#..............."
    ".........................#..........................................#.....#.....#..............."
    "####...####.#.##...####..###..#...#.#...#.#...#.#...#.#...#.#####...#.....#.....#..............."
    "#...#.#...#.##..#.#......#....#...#.#...#.#.#.#..#.#..#...#....#...#......#......#...##..#......"
    "#...#.#...#.#......###...#....#...#..#.#..#.#.#...#...#...#...#.....#.....#.....#...#..##......."
    "#...#.#...#.#.........#..#....#...#..#.#...#.#...#.#..#...#..#......#.....#.....#..............."
    "####...####.#.....####....###..####...#....#.#..#...#..####.#####...#.....#.....#..............."
    "#.........#...............................................#..........#....#....#................"
    "#.........#............................................###......................................"
    "................................................................................................"
    "................................................................................................"
    "................................................................................................"
    "................................................................................................"
};

// @address 0x7bf80c ?g_font_4x6@simple_font@@3Us_font_data@1@A
simple_font::s_font_data simple_font::g_font_4x6 = { false, 4, 6, 16, 32, 127, 1.0f, 128, 64, 0, -1, 0, 64, 36,
    ".....#..#.#.#.#..#..#....#..##...#..#...#.#...................#."
    ".....#..#.#.###..##...#.#.#.#...#....#...#...#................#."
    ".....#......#.#.##...#...##.....#....#..###.###.....###......#.."
    "............###..##.#...#.#.....#....#...#...#..............#..."
    ".....#......#.#.##....#.###......#..#...#.#.....##.......#..#..."
    ".................#..............................#..............."
    ".##..#..##..##....#.###..#..###..#...#............#.....#...##.."
    "#.#.##....#...#.#.#.#...#.....#.#.#.#.#..........#.......#....#."
    "#.#..#...#...#..###.##..##...##..#...##..#...#..#...###...#..#.."
    "#.#..#..#.....#...#...#.#.#..#..#.#...#..........#.......#......"
    "##..###.###.##....#.##...#...#...#...#...#..##....#.###.#....#.."
    "............................................#..................."
    "###..#..##...##.##..###.###..##.#.#.###...#.#.#.#...#.#.#.#..#.."
    "#.#.#.#.#.#.#...#.#.#...#...#...#.#..#....#.#.#.#...###.###.#.#."
    "#.#.###.##..#...#.#.##..##..#.#.###..#....#.##..#...###.#.#.#.#."
    "#...#.#.#.#.#...#.#.#...#...#.#.#.#..#..#.#.#.#.#...#.#.#.#.#.#."
    "###.#.#.##...##.##..###.#....##.#.#.###..#..#.#.###.#.#.#.#..#.."
    "................................................................"
    "##...#..##...##.###.#.#.#.#.#.#.#.#.#.#.###.##..#...##...#......"
    "#.#.#.#.#.#.#....#..#.#.#.#.#.#.#.#.#.#...#.#...#....#..#.#....."
    "##..#.#.##..###..#..#.#.#.#.###..#...#...#..#....#...#.........."
    "#...###.#.#...#..#..#.#.#.#.###.#.#..#..#...#.....#..#.........."
    "#....##.#.#.##...#..###..#..#.#.#.#..#..###.##....#.##.........."
    "...........................................................####."
    "##......#.........#.......#.....#....#...#..#....#.............."
    ".#......#.........#......#......#...........#....#.............."
    ".....##.##...##..##.###.###.###.##...#...#..#.#..#..###.##...#.."
    "....#.#.#.#.#...#.#.##...#..#.#.#.#..#...#..##...#..###.#.#.#.#."
    "....###.##...##..##..##..#....#.#.#..#...#..#.#..#..#.#.#.#..#.."
    "............................###.........##......................"
    ".............................................##..#..##..#.#....."
    ".................#...........................#...#...#.#.#......"
    "##...##.##...##.###.#.#.#.#.#.#.#.#.#.#.##..##...#...##........."
    "#.#.#.#.#....#...#..#.#.#.#.###..#..#.#..#...#...#...#.........."
    "##...##.#...##...##.###..#..###.#.#..#...##..##..#..##.........."
    "#.....#.............................#..........................."
};

// @address 0x730e38 ?g_fonts@simple_font@@3PAPAUs_font_data@1@A
simple_font::s_font_data *simple_font::g_fonts[2] = { &simple_font::g_font_4x6, &simple_font::g_font_6x10 };

extern "C" {
// @address 0x93ddf0 g_font_globals

}


struct bitmap_data;

namespace {
class slot_texture_ref
{
public:
    // Mirrors c_rasterizer_texture_ref's payload: either a cache texture header
    // index (a small long) or the D3D texture pointer itself; 0 and -1 are none.
#ifdef HALO3_64BIT
    long datum_ref;
    __forceinline slot_texture_ref &operator=(const slot_texture_ref &other) { datum_ref = other.datum_ref; return *this; }
    __forceinline slot_texture_ref &operator=(const c_rasterizer_texture_ref &other) { datum_ref = other.get_datum_ref(); return *this; }
    __forceinline long get_datum_ref() const { return datum_ref; }
    __forceinline bool valid() const { return get_datum_ref() != 0 && get_datum_ref() != -1; }
    __forceinline D3DTexture *get_d3d_texture() const
    {
        long reference = datum_ref;
        bool valid = reference != 0 && reference != -1;
        return valid ? (D3DTexture *)(unsigned long long)(unsigned long)reference : 0;
    }
#else
    union
    {
        void *texture;
        long datum_ref;
    };
    __forceinline slot_texture_ref &operator=(const slot_texture_ref &other) { texture = other.texture; return *this; }
    __forceinline slot_texture_ref &operator=(const c_rasterizer_texture_ref &other) { datum_ref = other.get_datum_ref(); return *this; }
    __forceinline long get_datum_ref() const { return datum_ref; }
    __forceinline bool valid() const { return get_datum_ref() != 0 && get_datum_ref() != -1; }
    __forceinline D3DTexture *get_d3d_texture() const
    {
        long reference = datum_ref;
        bool valid = reference != 0 && reference != -1;
        return valid ? (D3DTexture *)texture : 0;
    }
#endif
};
class bitmap_texture_ref
{
public:
    bitmap_texture_ref() {}
    explicit bitmap_texture_ref(long datum_ref) { m_datum_ref = datum_ref; }
    bitmap_texture_ref(const bitmap_texture_ref &other) { m_datum_ref = other.m_datum_ref; }
    bitmap_texture_ref &operator=(bitmap_texture_ref other) { m_datum_ref = other.m_datum_ref; return *this; }
    long m_datum_ref;
};
}
// match: 0x46a628: preserve the existing texture-reference assignment boundary;
// the canonical c_rasterizer_texture_ref changes native COFF register allocation.
struct s_rasterizer_text_bitmap_slot
{
    bool active;
    long glyph_width;
    long glyph_height;
    long characters_per_row;
    long first_character;
    long last_character;
    float scale;
    long texture_width;
    long texture_height;
    void *resource;
    slot_texture_ref texture_ref;
    long pitch;
    long width;
    long height;
    const unsigned char *pixels;
};
typedef char rasterizer_text_bitmap_slot_size[
    (sizeof(void *) != 4 || ((sizeof(s_rasterizer_text_bitmap_slot) == sizeof(simple_font::s_font_data)))) ? 1 : -1];
typedef char rasterizer_text_bitmap_slot_texture_offset[(sizeof(void *) != 4 || ((offsetof(s_rasterizer_text_bitmap_slot, texture_ref) == 0x28))) ? 1 : -1];

struct texture_bindings
{
    slot_texture_ref invalid;
    slot_texture_ref sampler;
};

// The font cache c_simple_font_draw_string::draw hands c_draw_string::draw. Its
// slots are c_font_cache's six, in the same order and with the same signatures;
// it does not derive from c_font_cache, whose object also holds the font cache
// scope lock. Every character is the simple font's fixed cell.
class c_simple_font_cache
{
public:
    c_simple_font_cache()
    {
        // match: 0x191b20: draw clears the cell twice, six sth to 100(r1) in a ctr loop
        // and then three stw; the loop alone or the memset alone drops one of them.
        short *field = &m_character.advance;
        for (long count = 6; count != 0; count--)
            *field++ = 0;
        memset(&m_character, 0, sizeof(m_character));
    }
    virtual long character_data_get_hardware_block_index(e_character_data_index character_data_index) { return -1; }
    virtual void character_data_set_hardware_block_index(e_character_data_index character_data_index, long hardware_block_index) {}
    virtual void character_data_touch(e_character_data_index character_data_index) {}
    virtual bool get_pixels(e_font_id font, utf32 character, const s_font_character *font_character, unsigned short *pixels)
    {
        return false;
    }
    virtual bool font_character_predict(e_font_id font, utf32 character) { return true; }
    virtual bool get_character(
        e_font_id font, utf32 character, bool precache, unsigned long *out_key, long *out_index, const s_font_character **out_character);
private:
    s_font_character m_character;
};

// The chud and rasterizer draw strings predict characters identically; the
// linker folded both virtuals onto one body at 0x18a118.
static __forceinline bool glyph_cache_predict_character(c_font_cache *font_cache, e_font_id font, utf32 character)
{
    bool result = false;
    if (g_rasterizer_text_glyph_cache.initialized)
    {
        long index = -1;
        if (g_font_globals.package && font >= 0 && (unsigned long)font < 16)
            index = g_font_globals.package->font_indices[font];
        unsigned long key = -1;
        if (index != -1)
            key = (character.character & 0xffff) | (index << 16);
        if (rasterizer_text_find_entry(font_cache, key) != -1)
            return true;
        result = font_cache->font_character_predict(font, character);
    }
    return result;
}

bool c_rasterizer_draw_string::predict_character(c_font_cache *font_cache, e_font_id font, utf32 character)
{
    return glyph_cache_predict_character(font_cache, font, character);
}

bool c_chud_draw_string::predict_character(c_font_cache *font_cache, e_font_id font, utf32 character)
{
    return glyph_cache_predict_character(font_cache, font, character);
}

bool c_user_interface_draw_string::predict_character(c_font_cache *font_cache, e_font_id font, utf32 character)
{
    return glyph_cache_predict_character(font_cache, font, character);
}

bool c_user_interface_draw_string::precache_character(c_font_cache *font_cache, e_font_id font, utf32 character, const s_font_character **out_character)
{
    return rasterizer_text_try_and_get_glyph(font_cache, font, character, false, (long *)out_character, 0);
}

// @address 0x191eb0 ?precache_character@c_simple_font_draw_string@@UAA_NPAVc_font_cache@@W4e_font_id@@Uutf32@@PAPBUs_font_character@@@Z
bool c_simple_font_draw_string::precache_character(c_font_cache *font_cache, e_font_id font, utf32 character, const s_font_character **out_character)
{
    return font_cache->get_character(font, character, false, NULL, NULL, out_character);
}

// @address 0x454c38 rasterizer_text_flush_all_glyphs
void rasterizer_text_flush_all_glyphs(void)
{
    c_font_cache font_cache;
    if (g_rasterizer_text_glyph_cache.initialized)
    {
        for (int index = 0; index < 512; index++)
        {
            if (g_rasterizer_text_glyph_cache.entries[index].key != 0xffffffff)
                lruv_block_delete_internal(&font_cache, index);
        }
    }
    rasterizer_text_reset_cache();
}

// @address 0x454cb8 rasterizer_text_dispose
void rasterizer_text_dispose(void)
{
    int tr11;
    if (g_rasterizer_text_glyph_cache.initialized)
    {
        rasterizer_text_flush_all_glyphs();
        bitmaps_dispose_resource(g_rasterizer_text_glyph_cache.bitmap);
        g_rasterizer_text_glyph_cache.initialized = false;
        tr11 = g_rasterizer_text_glyph_cache.handle != 0 && g_rasterizer_text_glyph_cache.handle != -1;
        if ((unsigned int)(tr11 & 255) != 0)
            g_rasterizer_text_glyph_cache.handle = -1;
    }
    int remaining = 2;
    simple_font::s_font_data **slots = simple_font::g_fonts;
    do {
        simple_font::s_font_data *slot = *slots;
        if (slot->active)
        {
            bitmaps_dispose_resource((bitmap_data *)slot->resource);
            tr11 = slot->handle != 0 && slot->handle != -1;
            if ((unsigned int)(tr11 & 255) != 0)
                slot->handle = -1;
            slot->resource = 0;
            slot->active = false;
        }
        remaining = remaining - 1;
        slots = slots + 1;
    } while ((unsigned int)remaining != 0);
}

// @address 0x454d98 rasterizer_text_reset_cache
void rasterizer_text_reset_cache(void)
{
    for (int i = 0; i < 512; i++) {
        s_hardware_character *entry = &g_rasterizer_text_glyph_cache.entries[i];
        memset(entry, 0, sizeof(*entry));
        entry->key = 0xffffffff;
        entry->character_data_index = -1;
        entry->previous = -1;
        entry->next = -1;
    }
    memset(g_rasterizer_text_glyph_cache.map, -1, sizeof(g_rasterizer_text_glyph_cache.map));
    g_rasterizer_text_glyph_cache.head_index = 0;
    g_rasterizer_text_glyph_cache.tail_index = 0;
    g_rasterizer_text_glyph_cache.cursor_x = 1;
    g_rasterizer_text_glyph_cache.cursor_y = 1;
    g_rasterizer_text_glyph_cache.row_height = 0;
}

static inline bool rasterizer_text_entry_is_free(const s_hardware_character *entry)
{
    if (entry->character_data_index != -1)
        return false;
    return true;
}

// @address 0x454e40 rasterizer_text_find_entry
int rasterizer_text_find_entry(c_font_cache *font_cache, unsigned long key)
{
    int result = -1;
    if (g_rasterizer_text_glyph_cache.initialized)
    {
        if (g_font_globals.disabled)
        {
            for (int index = 0; index < 512; index++)
            {
                s_hardware_character *entry = &g_rasterizer_text_glyph_cache.entries[index];
#ifdef HALO3_64BIT
                if (entry->font_character_pointer != 0 && entry->key == key && rasterizer_text_entry_is_free(entry))
#else
                if (entry->font_character != 0 && entry->key == key && rasterizer_text_entry_is_free(entry))
#endif
                {
                    result = index;
                    break;
                }
            }
        }
        else
        {
            result = ((short *)g_rasterizer_text_glyph_cache.map)[((key >> 16) + key * 59) & 511];
            while (result != -1)
            {
                s_hardware_character *entry = &g_rasterizer_text_glyph_cache.entries[result];
                if (entry->key == key)
                    break;
                result = entry->next;
            }
        }
    }
    return result;
}

class c_interlocked_long;

// @address 0x454f20 rasterizer_text_try_and_get_glyph
extern "C" bool rasterizer_text_try_and_get_glyph(c_font_cache *font_cache, e_font_id font, utf32 character, bool precache, long *out_value, long *out_index)
{
#ifdef HALO3_64BIT
    const s_font_character *value_ptr = 0;
#else
    long value = 0;
#endif
    long index = -1;
    bool result = false;
    if (g_rasterizer_text_glyph_cache.initialized)
    {
        long font_index = -1;
        if (g_font_globals.package && font >= 0 && (unsigned long)font < 16)
            font_index = g_font_globals.package->font_indices[font];
        unsigned long key = -1;
        if (font_index != -1)
            key = (character.character & 0xffff) | (font_index << 16);
        index = rasterizer_text_find_entry(font_cache, key);
        if (index == -1)
        {
            unsigned long found_key;
            long found_data_index;
            union
            {
                long as_value;
                const s_font_character *as_character;
            } out_font_character;
            if (font_cache->get_character(font, character, precache, &found_key, &found_data_index, &out_font_character.as_character))
            {
                if (found_key != key)
                    index = rasterizer_text_find_entry(font_cache, found_key);
                if (index == -1)
                {
                    long thread_index = g_registered_thread_index;
                    // match: 0x454f20: the volatile member read preserves memory access order
                    if (g_thread_owning_device.peek() == thread_index)
                        index = rasterizer_text_allocate_and_upload_glyph(font_cache, font, character, found_key,
                            found_data_index, out_font_character.as_character);
                    else if (!out_index)
                    {
#ifdef HALO3_64BIT
                        value_ptr = out_font_character.as_character;
#else
                        value = out_font_character.as_value;
#endif
                        result = true;
                    }
                }
            }
        }
        if (index != -1)
        {
            s_hardware_character *entry = &g_rasterizer_text_glyph_cache.entries[index];
            if (!g_font_globals.disabled)
                font_cache->character_data_touch((e_character_data_index)entry->character_data_index);
#ifdef HALO3_64BIT
            value_ptr = entry->font_character_pointer;
#else
            value = entry->font_character;
#endif
            result = true;
        }
    }
    if (out_index)
        *out_index = index;
    if (out_value)
    {
#ifdef HALO3_64BIT
        *(const s_font_character **)out_value = value_ptr;
#else
        *out_value = value;
#endif
    }
    return result;
}

static __forceinline real_rectangle2d *set_rectangle2d_corners(real_rectangle2d *rectangle, float x0, float x1, float y0, float y1)
{
    rectangle->x0 = x0;
    rectangle->x1 = x1;
    rectangle->y0 = y0;
    rectangle->y1 = y1;
    return rectangle;
}

// @address 0x18b558 ?draw_character@c_chud_draw_string@@EAAXPAVc_font_cache@@W4e_font_id@@Uutf32@@KKMMMMMM@Z
void c_chud_draw_string::draw_character(c_font_cache *font_cache, e_font_id font, utf32 character, unsigned long color, unsigned long shadow_color, float x,
    float y, float tx, float ty, float width, float height)
{
    long index;
    if (rasterizer_text_try_and_get_glyph(font_cache, font, character, true, 0, &index))
    {
        const s_hardware_character *hardware = &g_rasterizer_text_glyph_cache.entries[index];
        real_rectangle2d texture;
        texture.x0 = (float)hardware->x + tx;
        texture.y0 = (float)hardware->y + ty;
        texture.x1 = texture.x0 + width;
        texture.y1 = texture.y0 + height;
        real_rectangle2d position;
        position.x0 = x;
        position.y0 = y;
        position.x1 = position.x0 + width;
        position.y1 = position.y0 + height;
        long pixels = (long)(position.x1 - position.x0);
        if (pixels <= 0)
            return;
        if (position.x1 < m_clip.x0 || position.x0 > m_clip.x1 || position.y1 < m_clip.y0 || position.y0 > m_clip.y1)
            return;
        real_rectangle2d fraction;
        set_rectangle2d_corners(&fraction, real_constant_pool_zero(), real_constant_pool_one(),
            real_constant_pool_zero(), real_constant_pool_one());
        if (position.x0 < m_clip.x0)
            fraction.x0 = (m_clip.x0 - position.x0) / (position.x1 - position.x0);
        if (position.x1 > m_clip.x1)
            fraction.x1 = (m_clip.x1 - position.x0) / (position.x1 - position.x0);
        if (position.y0 < m_clip.y0)
            fraction.y0 = (m_clip.y0 - position.y0) / (position.y1 - position.y0);
        if (position.y1 > m_clip.y1)
            fraction.y1 = (m_clip.y1 - position.y0) / (position.y1 - position.y0);
        set_rectangle2d_corners(&texture,
            texture.x0 + fraction.x0 * (texture.x1 - texture.x0),
            texture.x0 + fraction.x1 * (texture.x1 - texture.x0),
            texture.y0 + fraction.y0 * (texture.y1 - texture.y0),
            texture.y0 + fraction.y1 * (texture.y1 - texture.y0));
        set_rectangle2d_corners(&position,
            position.x0 + fraction.x0 * (position.x1 - position.x0),
            position.x0 + fraction.x1 * (position.x1 - position.x0),
            position.y0 + fraction.y0 * (position.y1 - position.y0),
            position.y0 + fraction.y1 * (position.y1 - position.y0));
        chud_widget_geometry_add_character(m_geometry, &texture, &position, m_scale);
    }
}

// @address 0x46aee0 ?predict_character@c_simple_font_draw_string@@UAA_NPAVc_font_cache@@W4e_font_id@@Uutf32@@@Z
bool c_simple_font_draw_string::predict_character(c_font_cache *font_cache, e_font_id font, utf32 character)
{
    return font_cache->get_character(font, character, false, NULL, NULL, NULL);
}

bool c_simple_font_cache::get_character(
    e_font_id font, utf32 character, bool precache, unsigned long *out_key, long *out_index, const s_font_character **out_character)
{
    m_character.advance = (short)(int)((float)g_simple_font_display->width * g_simple_font_display->scale);
    m_character.packed_size = 0;
    m_character.width = (short)(int)((float)g_simple_font_display->width * g_simple_font_display->scale);
    int h = g_simple_font_display->height;
    float sc = g_simple_font_display->scale;
    m_character.bearing = 0;
    m_character.top = 0;
    m_character.height = (short)(int)((float)h * sc);
    if (out_key)
        *out_key = (unsigned long)-1;
    if (out_index)
        *out_index = -1;
    if (out_character == 0)
        return true;
    *out_character = &m_character;
    return true;
}

// @address 0x4557b8 ?draw_character@c_rasterizer_draw_string@@EAAXPAVc_font_cache@@W4e_font_id@@Uutf32@@KKMMMMMM@Z
void c_rasterizer_draw_string::draw_character(c_font_cache *font_cache, e_font_id font, utf32 character, unsigned long color,
    unsigned long shadow_color, float x, float y, float tx, float ty, float width, float height)
{
    long adjusted_shadow_color = (long)shadow_color;
    if ((adjusted_shadow_color & 0xff000000) > (color & 0xff000000))
    {
        long merged_shadow_color = (color & 0xff000000) | (adjusted_shadow_color & 0xffffff);
        shadow_color = (unsigned long)merged_shadow_color;
    }
    add_character_to_glyph_cache(font_cache, font, character, color, shadow_color, x, y, tx, ty, width, height);
}

c_rasterizer_draw_string::c_rasterizer_draw_string() :
    c_draw_string(),
    m_rotation_origin(),
    m_rotation(0.0f),
    m_sine(0.0f),
    m_cosine(1.0f),
    m_glyph_cache_dirty(false),
    m_glyph_group_render_data()
{
    m_rotation_origin.x = 0.0f;
    m_rotation_origin.y = 0.0f;
}

// @address 0x4556f0 ?draw@c_rasterizer_draw_string@@UAA_NPAVc_font_cache@@PB_W@Z
bool c_rasterizer_draw_string::draw(c_font_cache *font_cache, const wchar_t *string)
{
    return this->c_draw_string::draw(font_cache, string);
}

// @address 0x88e36c ?g_simple_font_globals@simple_font@@3Us_simple_font_globals@1@A
simple_font::s_simple_font_globals simple_font::g_simple_font_globals;

extern "C" int rasterizer_text_cache_initialize(void)
{
    int result = 1;
    if (g_rasterizer_device != 0)
    {
        bitmap_data *resource = (bitmap_data *)RtlAllocateHeap(GetProcessHeap(), 0, 48);
        if (resource != 0)
        {
            bitmap_2d_initialize(resource, 0x200, 0x200, 0, 17, 48, 0, 0);
            memset(&g_rasterizer_text_glyph_cache, 0, sizeof(g_rasterizer_text_glyph_cache));
            c_rasterizer_texture_ref texture_ref = c_rasterizer_texture_ref::allocate(resource);
            int handle = texture_ref.get_datum_ref();
            bool valid = handle != 0 && handle != -1;
            if (!valid)
                result = 0;
            else
            {
                // match: 0x88e36c: volatile pointer cast preserves memory access order
                ((volatile bitmap_data *)resource)->texture_ref = handle;
                // match: 0x88e36c: volatile pointer cast preserves memory access order
                ((volatile bitmap_data *)resource)->flags |= 128;
                g_rasterizer_text_glyph_cache.bitmap = resource;
                g_rasterizer_text_glyph_cache.handle = handle;
                g_rasterizer_text_glyph_cache.texture_scale_x = real_constant_pool_one() / (float)resource->width;
                g_rasterizer_text_glyph_cache.texture_scale_y = real_constant_pool_one() / (float)resource->height;
            }
        }
        else
            result = 0;
    }
    rasterizer_text_reset_cache();
    if ((unsigned char)result != 0)
        g_rasterizer_text_glyph_cache.initialized = 1;
    unsigned int count = 2;
    simple_font::s_font_data **slot = simple_font::g_fonts;
    do
    {
        rasterizer_text_initialize_bitmap_slot((s_rasterizer_text_bitmap_slot *)*slot);
        count--;
        slot++;
    }
    while (count != 0);
    simple_font::g_simple_font_globals.shadow_enabled = false;
    return result;
}


struct D3DTexture;
static __forceinline c_rasterizer_texture_ref rasterizer_text_slot_make_ref(long texture)
{
    c_rasterizer_texture_ref ref;
    ref = c_rasterizer_texture_ref(texture);
    return ref;
}
// @address 0x46a628 rasterizer_text_initialize_bitmap_slot
extern "C" void rasterizer_text_initialize_bitmap_slot(s_rasterizer_text_bitmap_slot *slot)
{
    if (!slot->active && g_rasterizer_device)
    {
        short height = (short)slot->texture_height;
        short width = (short)slot->texture_width;
        bitmap_data *bitmap = (bitmap_data *)RtlAllocateHeap(GetProcessHeap(), 0, sizeof(bitmap_data));
        if (bitmap)
            bitmap_2d_initialize(bitmap, width, height, 0, 2, 48, 0, 0);
        slot->resource = bitmap;
        if (bitmap)
        {
            slot->texture_ref = c_rasterizer_texture_ref::allocate(bitmap);
            if (slot->texture_ref.valid())
            {
                // match: 0x46a628: retain the texture-reference assignment boundary for register allocation.
                (bitmap_texture_ref &)((bitmap_data *)slot->resource)->texture_ref =
                    bitmap_texture_ref(slot->texture_ref.get_datum_ref());
                ((bitmap_data *)slot->resource)->flags |= 128;
                long *pitch = &slot->pitch;
                unsigned char *destination = (unsigned char *)rasterizer_texture_lock_bits(
                    rasterizer_text_slot_make_ref(slot->texture_ref.get_datum_ref()), 0, pitch);
                long index = 0;
                for (long y = 0; y < slot->height; ++y)
                {
                    for (long x = 0; x < slot->width; ++x)
                    {
                        *destination = slot->pixels[index++] == '#' ? 255 : 0;
                        if (x == slot->width - 1)
                        {
                            destination = destination - slot->width + *pitch;
                        }
                        ++destination;
                    }
                }
                D3DTexture *unlocked_texture = slot->texture_ref.get_d3d_texture();
                if (unlocked_texture)
                    D3DTexture_UnlockRect(unlocked_texture, 0);
            }
        }
        slot->active = true;
    }
}
extern "C" bool simple_font_begin_rendering(float scale, bool shadow_enabled)
{
    bool result = false;
    if (g_simple_font_display->active)
    {
        bool rendering = c_rasterizer_draw_string::begin_rendering();
        result = rendering;
        c_rasterizer_texture_ref texture_ref((long)g_simple_font_display->handle);
        reinterpret_cast<rasterizer_sampler_texture_reference_call>(&c_rasterizer::set_sampler_texture)(0, texture_ref);
#if defined(_XBOX) && !defined(__clang__)
        // match: 0x191be8: 0.1f (+0x56c) and 10.0f load off one 0x82022850 base (one lis/addi); spelled literals take a lis each
        float minimum_scale = literal_10_0f[347];
        float maximum_scale = literal_10_0f[0];
#else
        float minimum_scale = 0.1f;
        float maximum_scale = 10.0f;
#endif
        float clamped_scale = minimum_scale;
        if (scale > clamped_scale)
        {
            clamped_scale = maximum_scale;
            if (scale < clamped_scale)
                clamped_scale = scale;
        }
        g_simple_font_display->scale = clamped_scale;
        simple_font::g_simple_font_globals.shadow_enabled = shadow_enabled;
    }
    return result;
}

bool c_rasterizer_draw_string::draw(c_font_cache *font_cache, const utf32 *string)
{
    bool result = false;
    if (string && string->character)
    {
        unsigned char async_usable = ((g_thread_crashed[3].peek() - 1) == 0) ^ 1;
        if (async_usable && begin_rendering())
        {
        bool drawn = c_draw_string::draw(font_cache, string);
        bool flushed = render_and_flush_glyph_cache(font_cache);
        result = drawn & flushed;
        c_rasterizer_texture_ref texture;
        texture = c_rasterizer_texture_ref(-1);
        reinterpret_cast<rasterizer_sampler_texture_reference_call>(&c_rasterizer::set_sampler_texture)(0, texture);
        }
    }
    return result;
}

void c_rasterizer_draw_string::add_character_to_glyph_cache(c_font_cache *font_cache, e_font_id font, utf32 character, unsigned long color,
    unsigned long shadow_color, float x, float y, float tx, float ty, float width, float height)
{
    bool same_group = m_glyph_group_render_data.texture_index == font &&
        (unsigned long)m_glyph_group_render_data.glyph_count == color &&
        (unsigned long)m_glyph_group_render_data.vertex_count == shadow_color;
    if (m_glyph_group_render_data.is_full() || !same_group)
    {
        render_and_flush_glyph_cache(font_cache);
        m_glyph_group_render_data.texture_index = font;
        m_glyph_group_render_data.glyph_count = color;
        m_glyph_group_render_data.vertex_count = shadow_color;
    }
    m_glyph_group_render_data.glyphs[m_glyph_group_render_data.index_count].character = character.character;
    m_glyph_group_render_data.glyphs[m_glyph_group_render_data.index_count].x = x;
    m_glyph_group_render_data.glyphs[m_glyph_group_render_data.index_count].y = y;
    m_glyph_group_render_data.glyphs[m_glyph_group_render_data.index_count].tx = tx;
    m_glyph_group_render_data.glyphs[m_glyph_group_render_data.index_count].ty = ty;
    m_glyph_group_render_data.glyphs[m_glyph_group_render_data.index_count].width = width;
    m_glyph_group_render_data.glyphs[m_glyph_group_render_data.index_count].height = height;
    m_glyph_group_render_data.index_count++;
}

bool c_rasterizer_draw_string::render_and_flush_glyph_cache(c_font_cache *font_cache)
{
    bool success = true;
    if (m_glyph_group_render_data.index_count > 0)
    {
        switch (get_drop_shadow_style())
        {
        case 0:
            break;
        case 1:
            success &= render_glyph_drop_shadows(font_cache);
            break;
        case 2:
            success &= render_glyph_outline_shadows(font_cache);
            break;
        default:
            break;
        }
        if (success)
            success &= render_glyphs(font_cache);
    }
    m_glyph_group_render_data.reset();
    return success;
}

// @address 0x191fa8 ?flush@c_simple_font_draw_string@@QAAXXZ
void c_simple_font_draw_string::flush()
{
    if ((long)m_count > 0)
    {
        m_text[m_count] = 0;
        m_count++;
        simple_font_render_text(g_simple_font_display, m_x, m_y, 0x400, 0, m_color, m_text, m_count, true);
    }
    m_color = 0;
    m_y = 0;
    m_count = 0;
}

// @address 0x191ee0 ?draw_character@c_simple_font_draw_string@@EAAXPAVc_font_cache@@W4e_font_id@@Uutf32@@KKMMMMMM@Z
void c_simple_font_draw_string::draw_character(c_font_cache *font_cache, e_font_id font, utf32 character, unsigned long color,
    unsigned long shadow_color, float x, float y, float tx, float ty, float width, float height)
{
    long iy = (long)y;
    bool unchanged = m_color == color && m_y == iy;
    if (m_count == 255 || !unchanged) {
        flush();
        m_color = color;
        m_y = iy;
        m_x = (long)x;
    }
    unsigned char text_character;
    if ((unsigned char)character.character < 32 || (unsigned char)character.character > 126)
        text_character = 63;
    else
        text_character = (unsigned char)character.character;
    m_text[m_count] = text_character;
    m_count++;
}

extern "C" {
// @address 0x119a507 g_simple_font_scaling_initialized
bool g_simple_font_scaling_initialized;
// @address 0x119b020 g_simple_font_vertex_scale
real_vector2d g_simple_font_vertex_scale;
// @address 0x119b028 g_simple_font_inverse_viewport_width
float g_simple_font_inverse_viewport_width;
// @address 0x119b02c g_simple_font_inverse_viewport_height
float g_simple_font_inverse_viewport_height;

// @address 0x8c4830 g_tiled_render_x_offset
// @address 0x8c4834 g_tiled_render_y_offset
// @address 0x737504 g_tiled_render_x_scale
// @address 0x737508 g_tiled_render_y_scale
}

static __forceinline rectangle2d *rasterizer_text_set_rectangle2d(rectangle2d *rectangle, short y0, short x0, short x1, short y1)
{
    rectangle->y0 = y0;
    rectangle->x0 = x0;
    rectangle->x1 = x1;
    rectangle->y1 = y1;
    return rectangle;
}
// @address 0x46ac20 simple_font_submit_vertices
extern "C" void simple_font_submit_vertices(rasterizer_vertex_screen *vertices, long vertex_count)
{
    if (!g_simple_font_scaling_initialized)
    {
        rectangle2d bounds;
        if (c_player_view::get_current())
        {
            bounds = c_player_view::get_current()->rasterizer_camera.window_pixel_bounds;
        }
        else
        {
            rasterizer_text_set_rectangle2d(&bounds, 0, 0, (short)c_rasterizer::render_globals.width, (short)c_rasterizer::render_globals.height);
        }
        calculate_aspect_ratio_scaling();
        g_simple_font_vertex_scale = g_interface_aspect_ratio_scaling;
        g_simple_font_scaling_initialized = true;
        g_simple_font_inverse_viewport_width = 1.0f / (float)(bounds.x1 - bounds.x0);
        g_simple_font_inverse_viewport_height = 1.0f / (float)(bounds.y1 - bounds.y0);
    }
    float scale_x = g_simple_font_inverse_viewport_width;
    float scale_y = g_simple_font_inverse_viewport_height;
    rasterizer_scale_screen_vertices(vertices, vertex_count, &g_simple_font_vertex_scale);
    float x_scale = scale_x * 2.0f;
    float y_scale = scale_y * 2.0f;
    float x_offset = scale_x * -0.0f;
    float y_offset = scale_y * 0.0f;
    long i = vertex_count - 1;
    if (i >= 0)
    {
        do
        {
            vertices[i].x = (vertices[i].x * g_tiled_render_x_scale + g_tiled_render_x_offset - 0.5f) * x_scale - 1.0f + x_offset;
            vertices[i].y = y_offset - ((vertices[i].y * g_tiled_render_y_scale + g_tiled_render_y_offset - 0.5f) * y_scale - 1.0f);
        } while (--i >= 0);
    }
    rasterizer_draw_quads(vertices, vertex_count / 4);
}

struct D3DTexture;
extern "C" {
// @address 0x836908 g_rasterizer_shared_shader_cache

}

static __forceinline const bitmap_data *rasterizer_text_bitmap_get()
{
    return g_rasterizer_text_glyph_cache.initialized ? g_rasterizer_text_glyph_cache.bitmap : NULL;
}

static __forceinline const c_rasterizer_texture_ref &rasterizer_text_texture_ref_get(slot_texture_ref &invalid)
{
    if (g_rasterizer_text_glyph_cache.initialized)
        return *(const c_rasterizer_texture_ref *)&g_rasterizer_text_glyph_cache.handle;
#ifdef HALO3_64BIT
    invalid.datum_ref = -1;
#else
    invalid.texture = (void *)-1;
#endif
    return (const c_rasterizer_texture_ref &)invalid;
}

static __forceinline bool rasterizer_text_texture_ref_valid(const c_rasterizer_texture_ref &texture_ref)
{
    long reference = texture_ref.get_datum_ref();
    return reference != 0 && reference != -1;
}

// @address 0x191c80 ?begin_rendering@c_rasterizer_draw_string@@SA_NXZ
bool c_rasterizer_draw_string::begin_rendering()
{
    const bitmap_data *bitmap = rasterizer_text_bitmap_get();
    texture_bindings bindings;
    c_rasterizer_texture_ref texture_ref = rasterizer_text_texture_ref_get(bindings.invalid);
    if (bitmap && bitmap->width > 0 && bitmap->height > 0 && rasterizer_text_texture_ref_valid(texture_ref))
    {
        D3DDevice_SetPixelShader(c_rasterizer::g_device, g_rasterizer_shared_shader_cache.pixel_shaders[0]);
        D3DDevice_SetVertexShader(c_rasterizer::g_device, g_rasterizer_shared_shader_cache.vertex_shaders[0]);
        bindings.sampler = texture_ref;
        reinterpret_cast<rasterizer_sampler_texture_reference_call>(&c_rasterizer::set_sampler_texture)(0, (const c_rasterizer_texture_ref &)bindings.sampler);
        c_rasterizer::set_sampler_address_mode(
            0, c_rasterizer::_sampler_address_clamp, c_rasterizer::_sampler_address_clamp, c_rasterizer::_sampler_address_clamp);
        if (g_rasterizer_sampler_filter_modes[0] != 2)
        {
            g_rasterizer_sampler_filter_modes[0] = 2;
            rasterizer_device_set_sampler_filter_modes(c_rasterizer::g_device, 0, 2, 2);
        }
        float constants[4];
        constants[0] = 1.0f;
        constants[1] = 1.0f;
        constants[2] = 1.0f;
        constants[3] = c_rasterizer::get_render_target_alpha_multiplier(0);
        c_rasterizer::g_device->SetPixelShaderConstantF(1, constants, 1);
        if (c_rasterizer::g_current_alpha_blend_mode != c_rasterizer::_alpha_blend_alpha_blend)
        {
            c_rasterizer::g_current_alpha_blend_mode = c_rasterizer::_alpha_blend_alpha_blend;
            D3DDevice *device = c_rasterizer::g_device;
            D3DDevice_SetRenderState_AlphaBlendEnable(device, TRUE);
            D3DDevice_SetRenderState_BlendOp(device, D3DBLENDOP_ADD);
            D3DDevice_SetRenderState_SrcBlend(device, D3DBLEND_SRCALPHA);
            D3DDevice_SetRenderState_DestBlend(device, D3DBLEND_INVSRCALPHA);
        }
        D3DDevice_SetRenderState_CullMode(c_rasterizer::g_device, D3DCULL_NONE);
        D3DDevice_SetVertexDeclaration(c_rasterizer::g_device,
            rasterizer_get_vertex_declaration(_vertex_type_screen, _transfer_vector_vertex_none, _entry_point_default));
        return true;
    }
    return false;
}
extern "C" {
}

static __forceinline utf32 rasterizer_text_make_utf32(long value)
{
    utf32 result = { value };
    return result;
}

static __forceinline void rasterizer_text_hash_insert(long index)
{
    s_hardware_character *entry = &g_rasterizer_text_glyph_cache.entries[index];
    long bucket = ((entry->key >> 16) + entry->key * 59) & 511;
    long head = ((short *)g_rasterizer_text_glyph_cache.map)[bucket];
    if (head != -1)
        g_rasterizer_text_glyph_cache.entries[head].previous = (short)index;
    entry->next = (short)head;
    entry->previous = -1;
    ((short *)g_rasterizer_text_glyph_cache.map)[bucket] = (short)index;
}

static __forceinline bool rasterizer_text_handle_valid(long handle)
{
    return handle != 0 && handle != -1;
}

static __forceinline D3DTexture *rasterizer_text_get_d3d_texture(long handle)
{
    D3DTexture *result = 0;
    if (rasterizer_text_handle_valid(handle))
    {
#ifdef HALO3_64BIT
        result = (D3DTexture *)(uintptr_t)(unsigned long)handle;
#else
        // The handle payload is either a cache texture header index or the D3D
        // texture pointer itself (c_rasterizer_texture_ref's union).
        union
        {
            long as_handle;
            D3DTexture *as_texture;
        } view;
        view.as_handle = handle;
        result = view.as_texture;
#endif
    }
    return result;
}

static __forceinline void rasterizer_text_texture_unlock(long handle)
{
    D3DTexture *d3d_texture = rasterizer_text_get_d3d_texture(handle);
    if (d3d_texture != 0)
        D3DTexture_UnlockRect(d3d_texture, 0);
}

struct s_rasterizer_text_ink_bounds
{
    bool measured;
    real_rectangle2d bounds;
};

static s_rasterizer_text_ink_bounds g_rasterizer_text_ink_bounds[512];

static const real_rectangle2d &rasterizer_text_get_ink_bounds(
    c_font_cache *font_cache, e_font_id font, utf32 character, long hardware_index)
{
    s_rasterizer_text_ink_bounds &entry = g_rasterizer_text_ink_bounds[hardware_index];
    if (!entry.measured)
    {
        entry.measured = true;
        const s_font_character *metrics =
            g_rasterizer_text_glyph_cache.entries[hardware_index].font_character_pointer;
        unsigned short pixels[16384];
        if (metrics && metrics->width > 0 && metrics->height > 0 &&
            metrics->width * metrics->height <= (int)(sizeof(pixels) / sizeof(*pixels)) &&
            font_cache->get_pixels(font, character, metrics, pixels))
        {
            long left = metrics->width, top = metrics->height;
            long right = 0, bottom = 0;
            for (long y = 0; y < metrics->height; ++y)
                for (long x = 0; x < metrics->width; ++x)
                    if (pixels[y * metrics->width + x] & 0xf000)
                    {
                        if (x < left)
                            left = x;
                        if (y < top)
                            top = y;
                        if (right <= x)
                            right = x + 1;
                        if (bottom <= y)
                            bottom = y + 1;
                    }
            set_rectangle2d_corners(&entry.bounds, (float)left, (float)right, (float)top, (float)bottom);
        }
        else
            entry.bounds = {};
    }
    return entry.bounds;
}

// @address 0x4550d0 rasterizer_text_allocate_and_upload_glyph
extern "C" long rasterizer_text_allocate_and_upload_glyph(
    c_font_cache *font_cache, e_font_id font, utf32 character, unsigned long key, long data_index, const s_font_character *font_character)
{
    bool flushed = false;
    if (g_rasterizer_text_glyph_cache.cursor_x + font_character->width > 512)
    {
        g_rasterizer_text_glyph_cache.cursor_x = 1;
        g_rasterizer_text_glyph_cache.cursor_y += g_rasterizer_text_glyph_cache.row_height;
        g_rasterizer_text_glyph_cache.row_height = 0;
    }
    if (g_rasterizer_text_glyph_cache.cursor_y + font_character->height > 512)
    {
        g_rasterizer_text_glyph_cache.cursor_x = g_rasterizer_text_glyph_cache.cursor_y = 1;
        g_rasterizer_text_glyph_cache.row_height = 0;
        for (;
             g_rasterizer_text_glyph_cache.head_index != g_rasterizer_text_glyph_cache.tail_index;
             g_rasterizer_text_glyph_cache.head_index = (g_rasterizer_text_glyph_cache.head_index + 1) & 511)
        {
            s_hardware_character *hardware_character = &g_rasterizer_text_glyph_cache.entries[g_rasterizer_text_glyph_cache.head_index];
            if (hardware_character->y > 0)
            {
                flushed = true;
                if (hardware_character->key != 0xffffffff)
                    lruv_block_delete_internal(font_cache, g_rasterizer_text_glyph_cache.head_index);
            }
            else
            {
                break;
            }
        }
    }
    if (font_character->height + 1 > g_rasterizer_text_glyph_cache.row_height)
    {
        long min_y = g_rasterizer_text_glyph_cache.cursor_y + g_rasterizer_text_glyph_cache.row_height;
        long max_y = g_rasterizer_text_glyph_cache.cursor_y + font_character->height;
        for (;
             g_rasterizer_text_glyph_cache.head_index != g_rasterizer_text_glyph_cache.tail_index;
             g_rasterizer_text_glyph_cache.head_index = (g_rasterizer_text_glyph_cache.head_index + 1) & 511)
        {
            s_hardware_character *hardware_character = &g_rasterizer_text_glyph_cache.entries[g_rasterizer_text_glyph_cache.head_index];
            if (hardware_character->y >= min_y && hardware_character->y < max_y)
            {
                flushed = true;
                if (hardware_character->key != 0xffffffff)
                    lruv_block_delete_internal(font_cache, g_rasterizer_text_glyph_cache.head_index);
            }
            else
            {
                break;
            }
        }
        g_rasterizer_text_glyph_cache.row_height = font_character->height + 1;
    }
    long next_index = (g_rasterizer_text_glyph_cache.tail_index + 1) & 511;
    if (next_index == g_rasterizer_text_glyph_cache.head_index)
    {
        flushed = true;
        if (g_rasterizer_text_glyph_cache.entries[g_rasterizer_text_glyph_cache.head_index].key != 0xffffffff)
            lruv_block_delete_internal(font_cache, g_rasterizer_text_glyph_cache.head_index);
        g_rasterizer_text_glyph_cache.head_index = (g_rasterizer_text_glyph_cache.head_index + 1) & 511;
    }
    s_hardware_character *hardware_character = &g_rasterizer_text_glyph_cache.entries[g_rasterizer_text_glyph_cache.tail_index];
    long hardware_character_index = g_rasterizer_text_glyph_cache.tail_index;
    g_rasterizer_text_ink_bounds[hardware_character_index].measured = false;
    if (!g_font_globals.disabled)
        font_cache->character_data_set_hardware_block_index((e_character_data_index)data_index, g_rasterizer_text_glyph_cache.tail_index);
    hardware_character->key = key;
    hardware_character->character_data_index = data_index;
    hardware_character->font_character_pointer = font_character;
    hardware_character->x = (short)g_rasterizer_text_glyph_cache.cursor_x;
    hardware_character->y = (short)g_rasterizer_text_glyph_cache.cursor_y;
    rasterizer_text_hash_insert(hardware_character_index);
    unsigned short pixels[16384];
    if (font_cache->get_pixels((e_font_id)key, rasterizer_text_make_utf32(data_index), font_character, pixels))
    {
        c_rasterizer_texture_ref texture_ref(-1);
        reinterpret_cast<rasterizer_sampler_texture_reference_call>(&c_rasterizer::set_sampler_texture)(0, texture_ref);
        long flags = 0;
        void *base = 0;
        if (flushed)
            flags = 1;
        long pitch;
        texture_ref = c_rasterizer_texture_ref(g_rasterizer_text_glyph_cache.handle);
        base = rasterizer_texture_lock_bits(texture_ref, flags, &pitch);
        if (base != 0)
        {
            rectangle2d dirty_rect;
            dirty_rect.x0 = (hardware_character->x - 1 > 0) ? (hardware_character->x - 1) : 0;
            dirty_rect.y0 = (hardware_character->y - 1 > 0) ? (hardware_character->y - 1) : 0;
            dirty_rect.x1 = (hardware_character->x + font_character->width + 1 >
                                g_rasterizer_text_glyph_cache.bitmap->width)
                ? g_rasterizer_text_glyph_cache.bitmap->width
                : (hardware_character->x + font_character->width + 1);
            dirty_rect.y1 = (hardware_character->y + font_character->height + 1 >
                                g_rasterizer_text_glyph_cache.bitmap->height)
                ? g_rasterizer_text_glyph_cache.bitmap->height
                : (hardware_character->y + font_character->height + 1);
            for (long y = dirty_rect.y0; y < dirty_rect.y1; y++)
            {
                for (long x = dirty_rect.x0; x < dirty_rect.x1; x++)
                {
                    void *pixel = offset_pointer(base, pitch * y);
                    pixel = offset_pointer(pixel, x * 2);
                    *(unsigned short *)pixel = 0;
                }
            }
            long y = 0;
            long source_index = 0;
            for (; y < font_character->height; y++)
            {
                void *row_base = offset_pointer(base, pitch * (hardware_character->y + y));
                row_base = offset_pointer(row_base, hardware_character->x * 2);
                for (long x = 0; x < font_character->width; x++)
                {
                    ((unsigned short *)row_base)[x] = pixels[source_index + x];
                }
                source_index += font_character->width;
            }
            rasterizer_text_texture_unlock(g_rasterizer_text_glyph_cache.handle);
        }
    }
    g_rasterizer_text_glyph_cache.cursor_x += font_character->width + 1;
    g_rasterizer_text_glyph_cache.tail_index = (g_rasterizer_text_glyph_cache.tail_index + 1) & 511;
    return hardware_character_index;
}

// @rdata 0x8202251c
static const float k_rasterizer_text_zero = 0.0f;
static __forceinline real_vector2d rasterizer_text_get_aspect_ratio_scaling()
{
    calculate_aspect_ratio_scaling();
    return g_interface_aspect_ratio_scaling;
}

static int rasterizer_text_conduit_size(long font)
{
    switch (font)
    {
    case 1: case 2: case 3: case 4: case 6: case 8: case 9:
        break;
    default:
        return 0;
    }
    const s_font_header *header = font_get_header(font);
    if (!header)
        return 0;
    if (!strcmp(header->name, "conduit-16"))
        return 16;
    if (!strcmp(header->name, "conduit-18"))
        return 18;
    if (!strcmp(header->name, "conduit-23"))
        return 23;
    if (!strcmp(header->name, "conduit-32"))
        return 32;
    return 0;
}

// @address 0x455a58 ?render_glyphs_with_color_and_offset@c_rasterizer_draw_string@@AAA_NPAVc_font_cache@@KPBTreal_vector2d@@@Z
bool c_rasterizer_draw_string::render_glyphs_with_color_and_offset(c_font_cache *font_cache, unsigned long color, const real_vector2d *offset)
{
    bool success = true;
    if ((color & 0xff000000) != 0)
    {
        long texture_index = m_glyph_group_render_data.texture_index;
        long triangle_count = 0;
        float sine_rotation = m_sine;
        float scale = get_scale_internal();
        float cosine_rotation = m_cosine;
        real_vector2d render_scale = rasterizer_text_get_aspect_ratio_scaling();
        const int conduit_size = port_ui_hd_text_enabled() ? rasterizer_text_conduit_size(texture_index) : 0;
        const unsigned int pixel_height = conduit_size > 0 && port_ui_hd_text_prepare_atlas()
            ? (unsigned int)(conduit_size * scale * render_scale.j + 0.5f) : 0;
        rectangle2d bounds;
        if (c_player_view::get_current())
            bounds = c_player_view::get_current()->rasterizer_camera.window_pixel_bounds;
        else
            rasterizer_text_set_rectangle2d(&bounds, 0, 0, (short)c_rasterizer::render_globals.width, (short)c_rasterizer::render_globals.height);
        short window_y0 = bounds.y0;
        short window_x0 = bounds.x0;
        float inv_width = 1.0f / (float)(double)(bounds.x1 - window_x0);
        float inv_height = 1.0f / (float)(double)(bounds.y1 - window_y0);
        rasterizer_vertex_screen corners[4];
        rasterizer_vertex_screen out_vertices[384];
        D3DTexture *hd_textures[64];
        bool has_hd_glyphs = false;
        for (long i = 0; i < m_glyph_group_render_data.index_count; i++)
        {
            long hardware_index;
            utf32 character;
            character.character = m_glyph_group_render_data.glyphs[i].character;
            // Halo 3 already advanced the cursor for spaces. They have no outline to draw.
            if (pixel_height && (character.character == ' ' || character.character == 0xa0))
                continue;
            if (!rasterizer_text_try_and_get_glyph(font_cache, (e_font_id)texture_index, character, true, 0, &hardware_index))
            {
                success = false;
                break;
            }
            s_hardware_character *hardware_character = &g_rasterizer_text_glyph_cache.entries[hardware_index];
            float x = m_glyph_group_render_data.glyphs[i].x;
            float y = m_glyph_group_render_data.glyphs[i].y;
            float left = offset->i * scale + x;
            float top = offset->j * scale + y;
            float tx = m_glyph_group_render_data.glyphs[i].tx;
            float ty = m_glyph_group_render_data.glyphs[i].ty;
            float width = m_glyph_group_render_data.glyphs[i].width;
            float height = m_glyph_group_render_data.glyphs[i].height;
            float right = width * scale + left;
            float bottom = height * scale + top;
            float u_base = (float)(double)hardware_character->x + tx;
            float v_base = (float)(double)hardware_character->y + ty;
            real_rectangle2d hd_uv;
            D3DTexture *hd_texture = NULL;
            if (pixel_height && character.character >= 0x20 && character.character < 0xe000 &&
                hardware_character->font_character_pointer)
            {
                const s_font_character *metrics = hardware_character->font_character_pointer;
                s_port_ui_hd_glyph hd = {};
                hd_texture = port_ui_hd_text_glyph(pixel_height, character.character, &hd, &hd_uv);
                if (hd_texture && hd.width > 0 && hd.height > 0 && render_scale.i > 0.0f && render_scale.j > 0.0f)
                {
                    // Place every HD vector glyph at its true undistorted FreeType size on Halo 3's exact
                    // mathematical baseline (m_cursor.y = y + metrics->top * scale) and centered gently inside
                    // Halo 3's exact character advance box so baselines are dead-flat and stroke weight is uniform.
                    float off_i = offset->i * scale;
                    float off_j = offset->j * scale;
                    if (get_drop_shadow_style() == 2 && (off_i != 0.0f || off_j != 0.0f))
                    {
                        const float outline_px = 1.0f + (render_scale.j > 1.0f ? (render_scale.j - 1.0f) * 0.25f : 0.0f);
                        const float max_i = outline_px / render_scale.i;
                        const float max_j = outline_px / render_scale.j;
                        if (off_i > max_i) off_i = max_i;
                        else if (off_i < -max_i) off_i = -max_i;
                        if (off_j > max_j) off_j = max_j;
                        else if (off_j < -max_j) off_j = -max_j;
                    }
                    const float cursor_x = (off_i + x) - ((float)metrics->bearing + tx) * scale;
                    float baseline_y = (off_j + y) + ((float)metrics->top - ty) * scale;
                    const float gx_w = (float)hd.width / render_scale.i;
                    const float gx_h = (float)hd.height / render_scale.j;
                    const float gx_bx = (float)hd.bearing_x / render_scale.i;
                    const float gx_by = (float)hd.bearing_y / render_scale.j;
                    const float hd_adv = hd.advance / render_scale.i;
                    const float orig_adv = (float)metrics->advance * scale;
                    const float center_shift = (orig_adv - hd_adv) * 0.5f;
                    float gx0 = cursor_x + center_shift + gx_bx;
                    if (sine_rotation == 0.0f)
                    {
                        baseline_y = floorf(baseline_y * render_scale.j + 0.5f) / render_scale.j;
                        gx0 = floorf(gx0 * render_scale.i + 0.5f) / render_scale.i;
                    }
                    const float gx1 = gx0 + gx_w;
                    const float gy0 = baseline_y - gx_by;
                    const float gy1 = gy0 + gx_h;

                    // Preserve any horizontal or vertical scissor clipping applied by draw_string.
                    const bool clipped_left = tx > 0.0f;
                    const bool clipped_right = tx + width < (float)metrics->width - 0.5f;
                    const bool clipped_top = ty > 0.0f;
                    const bool clipped_bottom = ty + height < (float)metrics->height - 0.5f;
                    const float u0 = hd_uv.x0, u1 = hd_uv.x1, v0 = hd_uv.y0, v1 = hd_uv.y1;
                    float cx0 = clipped_left && left > gx0 ? left : gx0;
                    float cx1 = clipped_right && right < gx1 ? right : gx1;
                    float cy0 = clipped_top && top > gy0 ? top : gy0;
                    float cy1 = clipped_bottom && bottom < gy1 ? bottom : gy1;
                    if (cx0 < cx1 && cy0 < cy1)
                    {
                        hd_uv.x0 = u0 + (u1 - u0) * ((cx0 - gx0) / gx_w);
                        hd_uv.x1 = u0 + (u1 - u0) * ((cx1 - gx0) / gx_w);
                        hd_uv.y0 = v0 + (v1 - v0) * ((cy0 - gy0) / gx_h);
                        hd_uv.y1 = v0 + (v1 - v0) * ((cy1 - gy0) / gx_h);
                        left = cx0;
                        right = cx1;
                        top = cy0;
                        bottom = cy1;
                    }
                    else
                    {
                        hd_texture = NULL;
                    }
                }
            }
            if (pixel_height)
            {
                hd_textures[triangle_count / 2] = hd_texture;
                has_hd_glyphs |= hd_texture != NULL;
            }
            corners[0].color = corners[1].color = corners[2].color = corners[3].color = color;
            corners[0].x = corners[3].x = left;
            corners[1].x = corners[2].x = right;
            corners[0].y = corners[1].y = top;
            corners[2].y = corners[3].y = bottom;
            if (sine_rotation != k_rasterizer_text_zero)
            {
                float delta_x;
                float delta_y;
                const real_point2d *origin = &m_rotation_origin;
                float *vertex = &corners[0].y;
                unsigned long count = 4;
                do
                {
                    count--;
                    delta_x = vertex[-1] - origin->x;
                    delta_y = vertex[0] - origin->y;
                    float rotated_y = delta_y * cosine_rotation + delta_x * sine_rotation;
                    delta_x = delta_x * cosine_rotation - delta_y * sine_rotation;
                    delta_y = rotated_y;
                    vertex[-1] = origin->x + delta_x;
                    vertex[0] = origin->y + delta_y;
                    vertex += 5;
                } while (count != 0);
            }
            corners[0].u = corners[3].u = u_base * g_rasterizer_text_glyph_cache.texture_scale_x;
            corners[1].u = corners[2].u = (u_base + width) * g_rasterizer_text_glyph_cache.texture_scale_x;
            corners[0].v = corners[1].v = v_base * g_rasterizer_text_glyph_cache.texture_scale_y;
            corners[2].v = corners[3].v = (v_base + height) * g_rasterizer_text_glyph_cache.texture_scale_y;
            if (hd_texture)
            {
                corners[0].u = corners[3].u = hd_uv.x0;
                corners[1].u = corners[2].u = hd_uv.x1;
                corners[0].v = corners[1].v = hd_uv.y0;
                corners[2].v = corners[3].v = hd_uv.y1;
            }
            rasterizer_scale_screen_vertices(corners, 4, &render_scale);
            float x_scale = inv_width * 2.0f;
            float y_scale = inv_height * 2.0f;
            float x_offset = (float)(double)window_x0 * inv_width * -2.0f;
            float y_offset = (float)(double)window_y0 * inv_height * 2.0f;
            long normal = 3;
            do
            {
                corners[normal].x = (corners[normal].x * g_tiled_render_x_scale + g_tiled_render_x_offset - 0.5f) * x_scale - 1.0f + x_offset;
                corners[normal].y = y_offset - ((g_tiled_render_y_scale * corners[normal].y + g_tiled_render_y_offset - 0.5f) * y_scale - 1.0f);
                normal--;
            } while (normal >= 0);
            long vertex_index = triangle_count * 3;
            out_vertices[vertex_index + 0] = corners[0];
            out_vertices[vertex_index + 1] = corners[1];
            out_vertices[vertex_index + 2] = corners[3];
            out_vertices[vertex_index + 3] = corners[1];
            out_vertices[vertex_index + 4] = corners[2];
            out_vertices[vertex_index + 5] = corners[3];
            triangle_count += 2;
        }
        if (success && triangle_count > 0)
        {
            if (!has_hd_glyphs)
                c_rasterizer::draw_textured_screen_triangle_list(out_vertices, triangle_count);
            else
            {
                texture_bindings bindings;
                const c_rasterizer_texture_ref bitmap = rasterizer_text_texture_ref_get(bindings.invalid);
                const long glyph_count = triangle_count / 2;
                for (long first = 0; first < glyph_count;)
                {
                    D3DTexture *texture = hd_textures[first];
                    long end = first + 1;
                    while (end < glyph_count && hd_textures[end] == texture)
                        ++end;
                    c_rasterizer::set_sampler_texture(0, texture ? c_rasterizer_texture_ref(texture) : bitmap);
                    c_rasterizer::draw_textured_screen_triangle_list(out_vertices + first * 6, (end - first) * 2);
                    first = end;
                }
                c_rasterizer::set_sampler_texture(0, bitmap);
            }
        }
    }
    return success;
}

// @address 0x191b20 ?draw@c_simple_font_draw_string@@UAA_NPAVc_font_cache@@PBUutf32@@@Z
bool c_simple_font_draw_string::draw(c_font_cache *font_cache, const utf32 *string)
{
    bool result = false;
    float scale = get_scale();
    long initial = get_drop_shadow_style();
    if (simple_font_begin_rendering(scale, initial != 0))
    {
        c_simple_font_cache cache;
        result = c_draw_string::draw((c_font_cache *)&cache, string);
        flush();
        if (*(signed char *)g_simple_font_display != 0)
        {
            c_rasterizer_texture_ref texture;
            texture.clear();
            reinterpret_cast<rasterizer_sampler_texture_reference_call>(&c_rasterizer::set_sampler_texture)(0, texture);
        }
    }
    return result;
}

// March c_rasterizer::get_device.
static __forceinline D3DDevice *simple_font_get_device(void)
{
    return (D3DDevice *)g_rasterizer_device;
}

// March simple_font::get_width / get_height: the active font's cell size at the current scale.
static __forceinline long simple_font_get_width(void)
{
    return (long)((float)g_simple_font_display->width * g_simple_font_display->scale);
}

static __forceinline long simple_font_get_height(void)
{
    return (long)((float)g_simple_font_display->height * g_simple_font_display->scale);
}

// March simple_font.obj <static@8242b600>: one character quad plus its optional drop shadow.
static __forceinline long simple_font_add_quad(long x0, long y0, float u0, float texture_top, long x1, long y1, float u1, float texture_bottom,
    unsigned long color, long shadow_offset, rasterizer_vertex_screen *vertices, bool apply_shadow)
{
    long vertex_count = 4;
    vertices[0].x = (float)x0;
    vertices[0].y = (float)y0;
    vertices[0].u = u0;
    vertices[0].v = texture_top;
    vertices[0].color = color;
    vertices[1].x = (float)x0;
    vertices[1].y = (float)y1;
    vertices[1].u = u0;
    vertices[1].v = texture_bottom;
    vertices[1].color = color;
    vertices[2].x = (float)x1;
    vertices[2].y = (float)y1;
    vertices[2].u = u1;
    vertices[2].v = texture_bottom;
    vertices[2].color = color;
    vertices[3].x = (float)x1;
    vertices[3].y = (float)y0;
    vertices[3].u = u1;
    vertices[3].v = texture_top;
    vertices[3].color = color;
    if (apply_shadow && simple_font::g_simple_font_globals.shadow_enabled)
    {
        unsigned long shadow_color = (color & 0xff000000) | 0x00010101;
        memcpy(&vertices[vertex_count], vertices, vertex_count * sizeof(rasterizer_vertex_screen));
        for (long index = 0; index < vertex_count; index++)
        {
            vertices[index].x = (float)shadow_offset + vertices[index].x;
            vertices[index].y = (float)shadow_offset + vertices[index].y;
            vertices[index].color = shadow_color;
        }
        vertex_count *= 2;
    }
    return vertex_count;
}

// @address 0x46a7f0 simple_font_render_text
extern "C" void simple_font_render_text(simple_font::s_font_data *font, long x, long y, long column_limit, long flags, unsigned long color,
    const char *string, unsigned long count, bool apply_shadow)
{
    font = g_simple_font_display;
    column_limit = 1024;
    flags = 0;
    apply_shadow = true;
    if (font->active && simple_font_get_device())
    {
        float char_width = (float)simple_font_get_width();
        float char_height = (float)simple_font_get_height();
        float one_over_texture_width = 1.0f / (float)font->texture_width;
        float one_over_texture_height = 1.0f / (float)font->texture_height;
        bool vertical = (flags & 8) != 0;
        if (((flags & 16) != 0))
        {
            if (vertical)
                y -= (long)((float)count * char_height);
            else
                x -= (long)((float)count * char_width);
        }
        long column = 0;
        long row = 0;
        const char *cursor = string;
        long visible_count = 0;
        for (unsigned long index = 0; index < count; index++)
        {
            char character = *cursor++;
            if ((flags & 2) && character == '\n')
                continue;
            if ((flags & 1) && character == '\t')
                continue;
            visible_count++;
        }
        cursor = string;
        rasterizer_vertex_screen vertices[3073];
        long verts_used = 0;
        count = count > 256 ? 256 : count;
        for (unsigned long index = 0; index < count; index++)
        {
            char character = *cursor++;
            if ((flags & 2) && character == '\n')
            {
                column = 0;
                row++;
                continue;
            }
            if ((flags & 1) && character == '\t')
            {
                column = (column + 8) & ~7;
                continue;
            }
            if (character < font->first_character || character > font->last_character)
                character = ' ';
            float char_u = (float)font->width * one_over_texture_width;
            float char_v = (float)font->height * one_over_texture_height;
            float u0 = char_u * (float)((character - font->first_character) % font->characters_per_row);
            float texture_top = char_v * (float)((character - font->first_character) / font->characters_per_row);
            float u1 = u0 + char_u;
            float texture_bottom = texture_top + char_v;
            long x0;
            long y0;
            if (vertical)
            {
                x0 = (long)((float)x + (float)row * char_width);
                y0 = (long)((float)y + (float)column * char_height);
            }
            else
            {
                x0 = (long)((float)x + (float)column * char_width);
                y0 = (long)((float)y + (float)row * char_height);
            }
            long x1 = (long)((float)x0 + char_width);
            long y1 = (long)((float)y0 + char_height);
            long shadow_offset = (long)font->scale;
            verts_used += simple_font_add_quad(x0, y0, u0, texture_top, x1, y1, u1, texture_bottom, color, shadow_offset, &vertices[verts_used], true);
            column++;
            if (column >= column_limit)
            {
                column = 0;
                row++;
            }
        }
        if (verts_used > 0)
            ((void (*)(rasterizer_vertex_screen *, long, long))simple_font_submit_vertices)(vertices, verts_used, apply_shadow);
    }
}

extern "C" {
simple_font::s_font_data *g_simple_font_display = simple_font::g_fonts[1];
}

