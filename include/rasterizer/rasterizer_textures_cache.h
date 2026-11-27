#ifndef RASTERIZER_RASTERIZER_TEXTURES_CACHE_H
#define RASTERIZER_RASTERIZER_TEXTURES_CACHE_H

// c_rasterizer_texture_ref: a texture by datum reference (rasterizer\rasterizer_textures_cache.h in the March 2007
// build; its asserts name the file and the one field). The 4-byte payload is either a cache-file texture header
// index (a value at or below MAXIMUM_TEXTURE_REF_HEADER_INDEX, 32767) or the D3D texture pointer itself; -1 and
// 0 are none. The class has a copy constructor, so a by-value parameter is passed by address (set_sampler_texture,
// resolve_entire_surface).

#include "tag_files/tag_types.h"

struct D3DBaseTexture;
struct bitmap_data;
enum e_bitmap_type;

// GPU texture format to its sRGB-sampling counterpart, indexed by the low six format bits.
extern const unsigned long g_gputextureformat_to_srgb_gputextureformat_table[64];

class c_rasterizer_texture_ref
{
public:
    c_rasterizer_texture_ref() : m_datum_ref(-1) {}
    // March asserts m_datum_ref <= MAXIMUM_TEXTURE_REF_HEADER_INDEX here (rasterizer_textures_cache.h:27).
    c_rasterizer_texture_ref(long datum_ref) : m_datum_ref(datum_ref) {}
    // March asserts m_datum_ref > MAXIMUM_TEXTURE_REF_HEADER_INDEX here (rasterizer_textures_cache.h:34).
    c_rasterizer_texture_ref(D3DBaseTexture *texture) : m_datum_ref((long)(unsigned long)(unsigned __int64)texture) {}
    c_rasterizer_texture_ref(const c_rasterizer_texture_ref &other) : m_datum_ref(other.m_datum_ref) {}
    c_rasterizer_texture_ref &operator=(c_rasterizer_texture_ref other) { m_datum_ref = other.m_datum_ref; return *this; }

    static c_rasterizer_texture_ref allocate(long, long, long, long, long, bool, e_bitmap_type, bool, void *);
    static c_rasterizer_texture_ref allocate(const bitmap_data *bitmap);

    static void release(c_rasterizer_texture_ref &texture_ref)
    {
        if (texture_ref.valid())
        {
            D3DBaseTexture *texture = texture_ref.get_d3d_texture();
            texture_ref.clear();
        }
    }

    const long get_datum_ref() const { return m_datum_ref; }
    bool valid() const { return m_datum_ref != 0 && m_datum_ref != -1; }
    void clear() { m_datum_ref = -1; }

    // Returns the D3D texture for this reference, or null when the reference is empty.
    D3DBaseTexture *get_d3d_texture() const
    {
        D3DBaseTexture *result = 0;
        if (valid())
            result = m_d3d_texture;
        return result;
    }

    bool get_width_height(long level, long *width, long *height);
    inline unsigned long get_d3d_format();

private:
    union
    {
        long m_datum_ref;
        TAG_POINTER(D3DBaseTexture) m_d3d_texture;
    };
};

typedef char rasterizer_texture_ref_size[(sizeof(c_rasterizer_texture_ref) == 4) ? 1 : -1];

#endif // RASTERIZER_RASTERIZER_TEXTURES_CACHE_H
