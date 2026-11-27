// port/sdk/d3d: the D3DX image file calls (d3dx9tex.obj) the game's custom bitmaps use: gui_custom_bitmap_storage_xenon
// reads an image's size and decodes it into a surface (the map preview banners of the lobbies, maps/images/*.blf
// 'mapi' chunks; file share thumbnails; screenshots), and saves a surface as a JPEG (the Theater screenshot,
// gui_custom_bitmap_storage_capture_screenshot). Reading goes through the port's JPEG / PNG decoders (d3dx_decode.cpp);
// the destination surface is written as D3DX writes it on the console: resized into the destination rectangle
// (D3DX_FILTER_*), packed into the surface's format (8_8_8_8 in its channel order, or DXT1 / DXT2_3 / DXT4_5 blocks)
// at the surface's tiled or linear addresses, through the surface's own lock (so the device knows the CPU wrote that
// memory in host order). Saving reads the same formats back through a read-only lock (which also brings back what a
// Resolve last wrote there) and writes them with the port's JPEG / PNG encoders (d3dx_encode.cpp) into an
// ID3DXBuffer. Built like the game, against the XDK's d3dx9tex.h.
#include <xtl.h>
#include <xgraphics.h>
#include <d3dx9.h>
#include <stdlib.h>
#include <string.h>
#include "d3dx_decode.h"
#include "d3dx_encode.h"
#include "../runtime/port_runtime.h"

namespace {

// ---- resizing (D3DX_FILTER_*) ----

uint32_t channel(uint32_t pixel, uint32_t shift)
{
    return (pixel >> shift) & 0xffu;
}

// An 8_8_8_8 format whose X / Y / Z swizzle is A8B8G8R8's (the host word is ABGR, red and blue swapped from D3DCOLOR).
// W does not count: X8B8G8R8 is A8B8G8R8 with W = 1.
bool swizzled_abgr(uint32_t format)
{
    const uint32_t xyz = D3DFORMAT_SWIZZLEX_MASK | D3DFORMAT_SWIZZLEY_MASK | D3DFORMAT_SWIZZLEZ_MASK;
    return (format & xyz) == ((uint32_t)D3DFMT_A8B8G8R8 & xyz);
}

// The source rectangle resized to w x h: POINT picks the nearest texel; the others filter, bilinearly when enlarging
// and by area (box) when shrinking along an axis.
uint32_t *resize(const uint32_t *src, uint32_t src_pitch, uint32_t sw, uint32_t sh, uint32_t w, uint32_t h,
                 uint32_t filter)
{
    uint32_t *out = (uint32_t *)malloc((size_t)w * h * 4u);
    if (out == 0)
        return 0;
    if (filter == D3DX_FILTER_POINT || (sw == w && sh == h)) {
        for (uint32_t y = 0; y < h; y++)
            for (uint32_t x = 0; x < w; x++)
                out[(size_t)y * w + x] = src[(size_t)((uint64_t)y * sh / h) * src_pitch + (uint32_t)((uint64_t)x * sw / w)];
        return out;
    }
    for (uint32_t y = 0; y < h; y++) {
        // the source span of this output row
        const float y0 = (float)y * (float)sh / (float)h, y1 = (float)(y + 1) * (float)sh / (float)h;
        for (uint32_t x = 0; x < w; x++) {
            const float x0 = (float)x * (float)sw / (float)w, x1 = (float)(x + 1) * (float)sw / (float)w;
            float sum[4] = { 0, 0, 0, 0 }, weight = 0.0f;
            if (x1 - x0 <= 1.0f || y1 - y0 <= 1.0f) {
                // enlarging (along either axis): bilinear at the output texel's centre
                float fx = (x0 + x1) * 0.5f - 0.5f, fy = (y0 + y1) * 0.5f - 0.5f;
                fx = fx < 0.0f ? 0.0f : fx;
                fy = fy < 0.0f ? 0.0f : fy;
                uint32_t ix = (uint32_t)fx, iy = (uint32_t)fy;
                ix = ix >= sw ? sw - 1 : ix;
                iy = iy >= sh ? sh - 1 : iy;
                const uint32_t ix1 = ix + 1 < sw ? ix + 1 : ix, iy1 = iy + 1 < sh ? iy + 1 : iy;
                const float ax = fx - (float)ix, ay = fy - (float)iy;
                const uint32_t t[4] = { src[(size_t)iy * src_pitch + ix], src[(size_t)iy * src_pitch + ix1],
                                        src[(size_t)iy1 * src_pitch + ix], src[(size_t)iy1 * src_pitch + ix1] };
                const float wt[4] = { (1 - ax) * (1 - ay), ax * (1 - ay), (1 - ax) * ay, ax * ay };
                for (int k = 0; k < 4; k++)
                    for (int c = 0; c < 4; c++)
                        sum[c] += wt[k] * (float)channel(t[k], (uint32_t)c * 8u);
                weight = 1.0f;
            } else {
                for (uint32_t sy = (uint32_t)y0; sy < sh && (float)sy < y1; sy++) {
                    const float wy = ((float)(sy + 1) < y1 ? (float)(sy + 1) : y1) - ((float)sy > y0 ? (float)sy : y0);
                    for (uint32_t sx = (uint32_t)x0; sx < sw && (float)sx < x1; sx++) {
                        const float wx = ((float)(sx + 1) < x1 ? (float)(sx + 1) : x1) - ((float)sx > x0 ? (float)sx : x0);
                        const uint32_t t = src[(size_t)sy * src_pitch + sx];
                        for (int c = 0; c < 4; c++)
                            sum[c] += wx * wy * (float)channel(t, (uint32_t)c * 8u);
                        weight += wx * wy;
                    }
                }
            }
            uint32_t pixel = 0;
            for (int c = 0; c < 4; c++) {
                float v = weight > 0.0f ? sum[c] / weight + 0.5f : 0.0f;
                v = v < 0.0f ? 0.0f : v > 255.0f ? 255.0f : v;
                pixel |= (uint32_t)v << (c * 8);
            }
            out[(size_t)y * w + x] = pixel;
        }
    }
    return out;
}

// ---- block compression (DXT1 / DXT2_3 / DXT4_5) ----

uint16_t to_565(uint32_t r, uint32_t g, uint32_t b)
{
    return (uint16_t)(((r * 31u + 127u) / 255u) << 11 | ((g * 63u + 127u) / 255u) << 5 | ((b * 31u + 127u) / 255u));
}

void from_565(uint16_t c, int32_t out[3])
{
    const int32_t r = (c >> 11) & 31, g = (c >> 5) & 63, b = c & 31;
    out[0] = (r << 3) | (r >> 2);
    out[1] = (g << 2) | (g >> 4);
    out[2] = (b << 3) | (b >> 2);
}

// The colour half of a block (BC1 layout, host order): endpoints at the ends of the texels' bounding box along its
// longest diagonal, four colours, each texel to the nearest. `transparent`: DXT1 with alpha texels (3 colours + clear).
void encode_color_block(const uint32_t texels[16], uint8_t out[8], bool dxt1_alpha)
{
    int32_t lo[3] = { 255, 255, 255 }, hi[3] = { 0, 0, 0 };
    bool any_transparent = false;
    for (int i = 0; i < 16; i++) {
        if (dxt1_alpha && channel(texels[i], 24) < 128) {
            any_transparent = true;
            continue;
        }
        for (int c = 0; c < 3; c++) {
            const int32_t v = (int32_t)channel(texels[i], 16u - 8u * (uint32_t)c);
            lo[c] = v < lo[c] ? v : lo[c];
            hi[c] = v > hi[c] ? v : hi[c];
        }
    }
    if (lo[0] > hi[0]) { // every texel transparent
        lo[0] = lo[1] = lo[2] = hi[0] = hi[1] = hi[2] = 0;
    }
    // inset the box by 1/16 of its size (fewer endpoint outliers), as common range-fit encoders do
    for (int c = 0; c < 3; c++) {
        const int32_t inset = (hi[c] - lo[c]) >> 4;
        lo[c] += inset;
        hi[c] -= inset;
    }
    uint16_t c0 = to_565((uint32_t)hi[0], (uint32_t)hi[1], (uint32_t)hi[2]);
    uint16_t c1 = to_565((uint32_t)lo[0], (uint32_t)lo[1], (uint32_t)lo[2]);
    const bool three = dxt1_alpha && any_transparent;
    if (three ? c0 > c1 : c0 < c1) {
        const uint16_t t = c0;
        c0 = c1;
        c1 = t;
    }
    if (!three && c0 == c1) {
        // a flat block: index 0 everywhere (c0 > c1 is not needed when all indices are 0)
        out[0] = (uint8_t)c0; out[1] = (uint8_t)(c0 >> 8); out[2] = (uint8_t)c1; out[3] = (uint8_t)(c1 >> 8);
        out[4] = out[5] = out[6] = out[7] = 0;
        return;
    }
    int32_t palette[4][3];
    from_565(c0, palette[0]);
    from_565(c1, palette[1]);
    for (int c = 0; c < 3; c++) {
        if (three) {
            palette[2][c] = (palette[0][c] + palette[1][c]) / 2;
            palette[3][c] = 0;
        } else {
            palette[2][c] = (2 * palette[0][c] + palette[1][c]) / 3;
            palette[3][c] = (palette[0][c] + 2 * palette[1][c]) / 3;
        }
    }
    uint32_t indices = 0;
    for (int i = 0; i < 16; i++) {
        uint32_t best = 0;
        if (three && channel(texels[i], 24) < 128) {
            best = 3;
        } else {
            int32_t best_error = 0x7fffffff;
            for (uint32_t p = 0; p < (three ? 3u : 4u); p++) {
                int32_t e = 0;
                for (int c = 0; c < 3; c++) {
                    const int32_t d = (int32_t)channel(texels[i], 16u - 8u * (uint32_t)c) - palette[p][c];
                    e += d * d;
                }
                if (e < best_error) {
                    best_error = e;
                    best = p;
                }
            }
        }
        indices |= best << (2 * i);
    }
    out[0] = (uint8_t)c0; out[1] = (uint8_t)(c0 >> 8);
    out[2] = (uint8_t)c1; out[3] = (uint8_t)(c1 >> 8);
    out[4] = (uint8_t)indices; out[5] = (uint8_t)(indices >> 8);
    out[6] = (uint8_t)(indices >> 16); out[7] = (uint8_t)(indices >> 24);
}

// BC3's alpha half: min / max endpoints, eight levels.
void encode_alpha_block(const uint32_t texels[16], uint8_t out[8])
{
    uint32_t lo = 255, hi = 0;
    for (int i = 0; i < 16; i++) {
        const uint32_t a = channel(texels[i], 24);
        lo = a < lo ? a : lo;
        hi = a > hi ? a : hi;
    }
    out[0] = (uint8_t)hi;
    out[1] = (uint8_t)lo;
    uint32_t levels[8] = { hi, lo };
    for (uint32_t k = 1; k < 7; k++)
        levels[k + 1] = ((7 - k) * hi + k * lo) / 7;
    uint64_t bits = 0;
    for (int i = 0; i < 16; i++) {
        const int32_t a = (int32_t)channel(texels[i], 24);
        uint32_t best = 0;
        int32_t best_error = 1 << 30;
        for (uint32_t k = 0; k < 8; k++) {
            const int32_t d = a - (int32_t)levels[k];
            if (d * d < best_error) {
                best_error = d * d;
                best = k;
            }
        }
        bits |= (uint64_t)best << (3 * i);
    }
    for (int b = 0; b < 6; b++)
        out[2 + b] = (uint8_t)(bits >> (8 * b));
}

// BC2's explicit 4-bit alpha.
void encode_explicit_alpha(const uint32_t texels[16], uint8_t out[8])
{
    memset(out, 0, 8);
    for (int i = 0; i < 16; i++) {
        const uint32_t a = (channel(texels[i], 24) * 15u + 127u) / 255u;
        out[i / 2] |= (uint8_t)(a << ((i & 1) * 4));
    }
}

// The surface's memory offset of texel / block (x, y).
uint32_t surface_offset(uint32_t x, uint32_t y, uint32_t pitch_bytes, uint32_t width_units, uint32_t unit_bytes,
                        bool tiled)
{
    return tiled ? XGAddress2DTiledOffset(x, y, width_units, unit_bytes) * unit_bytes : y * pitch_bytes + x * unit_bytes;
}

// ---- block decompression (the blocks as encode_* above writes them, host order) ----

// The colour half: four colours from the two 565 endpoints; with `three` (DXT1 when c0 <= c1) three and transparent.
void decode_color_block(const uint8_t block[8], bool dxt1, uint32_t texels[16])
{
    const uint16_t c0 = (uint16_t)(block[0] | block[1] << 8), c1 = (uint16_t)(block[2] | block[3] << 8);
    int32_t palette[4][3];
    from_565(c0, palette[0]);
    from_565(c1, palette[1]);
    const bool three = dxt1 && c0 <= c1;
    for (int c = 0; c < 3; c++) {
        if (three) {
            palette[2][c] = (palette[0][c] + palette[1][c]) / 2;
            palette[3][c] = 0;
        } else {
            palette[2][c] = (2 * palette[0][c] + palette[1][c]) / 3;
            palette[3][c] = (palette[0][c] + 2 * palette[1][c]) / 3;
        }
    }
    const uint32_t indices = (uint32_t)block[4] | (uint32_t)block[5] << 8 | (uint32_t)block[6] << 16 |
                             (uint32_t)block[7] << 24;
    for (int i = 0; i < 16; i++) {
        const uint32_t index = (indices >> (2 * i)) & 3u;
        const uint32_t alpha = three && index == 3 ? 0u : 0xffu;
        texels[i] = alpha << 24 | (uint32_t)palette[index][0] << 16 | (uint32_t)palette[index][1] << 8 |
                    (uint32_t)palette[index][2];
    }
}

// BC2's explicit 4-bit alpha, into the texels' alpha bytes.
void decode_explicit_alpha(const uint8_t block[8], uint32_t texels[16])
{
    for (int i = 0; i < 16; i++) {
        const uint32_t a = (block[i / 2] >> ((i & 1) * 4)) & 15u;
        texels[i] = (texels[i] & 0x00ffffffu) | (a * 17u) << 24;
    }
}

// BC3's alpha half: eight levels when a0 > a1, else six and 0 and 255.
void decode_alpha_block(const uint8_t block[8], uint32_t texels[16])
{
    const uint32_t a0 = block[0], a1 = block[1];
    uint32_t levels[8] = { a0, a1 };
    if (a0 > a1) {
        for (uint32_t k = 1; k < 7; k++)
            levels[k + 1] = ((7 - k) * a0 + k * a1) / 7;
    } else {
        for (uint32_t k = 1; k < 5; k++)
            levels[k + 1] = ((5 - k) * a0 + k * a1) / 5;
        levels[6] = 0;
        levels[7] = 255;
    }
    uint64_t bits = 0;
    for (int b = 0; b < 6; b++)
        bits |= (uint64_t)block[2 + b] << (8 * b);
    for (int i = 0; i < 16; i++)
        texels[i] = (texels[i] & 0x00ffffffu) | levels[(bits >> (3 * i)) & 7u] << 24;
}

// The rectangle's texels of a surface (8_8_8_8 in either channel order, DXT1, DXT2_3, DXT4_5) as D3DCOLOR host words,
// row-major, malloc'd. *has_alpha: the format's W channel is one of its components (A8R8G8B8, the DXTs), not the
// constant 1 (X8R8G8B8, whose alpha bytes are then read as 0xff).
HRESULT read_surface(LPDIRECT3DSURFACE9 surface, const RECT &rect, uint32_t **out_pixels, bool *has_alpha)
{
    *out_pixels = 0;
    if ((surface->Common & D3DCOMMON_SURFACE_FROM_TEXTURE) == 0) {
        // an EDRAM render target or depth surface: the port's lock gives no memory of the surface's size to read
        static int reported;
        if (!reported++)
            port_write_error("d3dx: D3DXSaveSurfaceToFileInMemory from a surface that is not a texture level (EDRAM): "
                             "nothing saved\n");
        return E_NOTIMPL;
    }
    D3DSURFACE_DESC desc;
    D3DSurface_GetDesc(surface, &desc);
    const uint32_t format = (uint32_t)desc.Format, texture_format = format & D3DFORMAT_TEXTUREFORMAT_MASK;
    const bool dxt = texture_format == GPUTEXTUREFORMAT_DXT1 || texture_format == GPUTEXTUREFORMAT_DXT2_3 ||
                     texture_format == GPUTEXTUREFORMAT_DXT4_5;
    if (texture_format != GPUTEXTUREFORMAT_8_8_8_8 && !dxt) {
        static int reported;
        if (!reported++)
            port_write_error("d3dx: D3DXSaveSurfaceToFileInMemory from a surface format the port does not read "
                             "(only 8_8_8_8, DXT1, DXT2_3, DXT4_5): nothing saved\n");
        return E_NOTIMPL;
    }
    const uint32_t swizzle_w = (format & D3DFORMAT_SWIZZLEW_MASK) >> D3DFORMAT_SWIZZLEW_SHIFT;
    *has_alpha = swizzle_w <= GPUSWIZZLE_W;
    const uint32_t w = (uint32_t)(rect.right - rect.left), h = (uint32_t)(rect.bottom - rect.top);
    uint32_t *pixels = (uint32_t *)malloc((size_t)w * h * 4u);
    if (pixels == 0)
        return E_OUTOFMEMORY;
    D3DLOCKED_RECT locked;
    locked.pBits = 0;
    D3DSurface_LockRect(surface, &locked, 0, D3DLOCK_READONLY);
    const uint8_t *bits = (const uint8_t *)locked.pBits;
    if (bits == 0) {
        free(pixels);
        return D3DERR_INVALIDCALL;
    }
    const bool tiled = XGIsTiledFormat((D3DFORMAT)format) != 0;
    if (!dxt) {
        // the words D3DXLoadSurfaceFromFileInMemory writes (and a Resolve's write-back leaves): ARGB, or ABGR
        const bool abgr = swizzled_abgr(format);
        const uint32_t width_units = (uint32_t)locked.Pitch / 4u;
        for (uint32_t y = 0; y < h; y++) {
            for (uint32_t x = 0; x < w; x++) {
                uint32_t p;
                const uint32_t at = surface_offset((uint32_t)rect.left + x, (uint32_t)rect.top + y,
                                                   (uint32_t)locked.Pitch, width_units, 4u, tiled);
                memcpy(&p, bits + at, 4);
                if (abgr)
                    p = (p & 0xff00ff00u) | (p >> 16 & 0xffu) | (p & 0xffu) << 16;
                if (!*has_alpha)
                    p |= 0xff000000u;
                pixels[(size_t)y * w + x] = p;
            }
        }
    } else {
        const uint32_t block_bytes = texture_format == GPUTEXTUREFORMAT_DXT1 ? 8u : 16u;
        const uint32_t width_blocks = (uint32_t)locked.Pitch / block_bytes;
        const uint32_t bx0 = (uint32_t)rect.left / 4u, by0 = (uint32_t)rect.top / 4u;
        const uint32_t bx1 = ((uint32_t)rect.right + 3u) / 4u, by1 = ((uint32_t)rect.bottom + 3u) / 4u;
        for (uint32_t by = by0; by < by1; by++) {
            for (uint32_t bx = bx0; bx < bx1; bx++) {
                const uint8_t *block = bits + surface_offset(bx, by, (uint32_t)locked.Pitch, width_blocks, block_bytes,
                                                             tiled);
                uint32_t texels[16];
                if (texture_format == GPUTEXTUREFORMAT_DXT1) {
                    decode_color_block(block, true, texels);
                } else {
                    decode_color_block(block + 8, false, texels);
                    if (texture_format == GPUTEXTUREFORMAT_DXT2_3)
                        decode_explicit_alpha(block, texels);
                    else
                        decode_alpha_block(block, texels);
                }
                for (uint32_t i = 0; i < 16; i++) {
                    const int32_t x = (int32_t)(bx * 4u + (i & 3u)) - rect.left;
                    const int32_t y = (int32_t)(by * 4u + (i >> 2)) - rect.top;
                    if (x >= 0 && y >= 0 && x < (int32_t)w && y < (int32_t)h)
                        pixels[(size_t)y * w + (uint32_t)x] = *has_alpha ? texels[i] : texels[i] | 0xff000000u;
                }
            }
        }
    }
    *out_pixels = pixels;
    return S_OK;
}

// d3dx9.lib's IID_ID3DXBuffer, as the retail image holds it (0x8200fef4).
const IID k_iid_d3dx_buffer = { 0x932e6a7e, 0xc68e, 0x45dd, { 0xa7, 0xbf, 0x53, 0xd1, 0x9c, 0x86, 0xdb, 0x1f } };
const IID k_iid_unknown = { 0x00000000, 0x0000, 0x0000, { 0xc0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x46 } };

// The ID3DXBuffer D3DXSaveSurfaceToFileInMemory returns: the encoded file, freed with the last Release.
class c_port_d3dx_buffer : public ID3DXBuffer
{
public:
    static void *operator new(size_t, void *memory) { return memory; }
    static void operator delete(void *) {}
    static void operator delete(void *, void *) {}

    c_port_d3dx_buffer(void *data, DWORD size) : m_references(1), m_data(data), m_size(size) {}

    HRESULT STDMETHODCALLTYPE QueryInterface(REFIID iid, LPVOID *ppv)
    {
        if (ppv == 0)
            return E_POINTER;
        if (memcmp(&iid, &k_iid_d3dx_buffer, sizeof(IID)) != 0 && memcmp(&iid, &k_iid_unknown, sizeof(IID)) != 0) {
            *ppv = 0;
            return E_NOINTERFACE;
        }
        AddRef();
        *ppv = this;
        return S_OK;
    }
    ULONG STDMETHODCALLTYPE AddRef() { return (ULONG)InterlockedIncrement(&m_references); }
    ULONG STDMETHODCALLTYPE Release()
    {
        const LONG references = InterlockedDecrement(&m_references);
        if (references == 0) {
            free(m_data);
            free(this);
        }
        return (ULONG)references;
    }
    LPVOID STDMETHODCALLTYPE GetBufferPointer() { return m_data; }
    DWORD STDMETHODCALLTYPE GetBufferSize() { return m_size; }

private:
    LONG m_references;
    void *m_data;
    DWORD m_size;
};

} // namespace

extern "C" {

HRESULT WINAPI D3DXGetImageInfoFromFileInMemory(LPCVOID pSrcData, UINT SrcDataSize, D3DXIMAGE_INFO *pSrcInfo)
{
    if (pSrcInfo != 0)
        memset(pSrcInfo, 0, sizeof(*pSrcInfo));
    s_port_image_info info;
    if (pSrcData == 0 || !port_image_get_info(pSrcData, SrcDataSize, &info))
        return D3DXERR_INVALIDDATA;
    if (pSrcInfo != 0) {
        pSrcInfo->Width = info.width;
        pSrcInfo->Height = info.height;
        pSrcInfo->Depth = 1;
        pSrcInfo->MipLevels = 1;
        pSrcInfo->Format = info.channels == 1 ? D3DFMT_L8 : info.channels == 4 ? D3DFMT_A8R8G8B8 : D3DFMT_X8R8G8B8;
        pSrcInfo->ResourceType = D3DRTYPE_TEXTURE;
        pSrcInfo->ImageFileFormat = info.file == k_port_image_file_png ? D3DXIFF_PNG : D3DXIFF_JPG;
    }
    return S_OK;
}

HRESULT WINAPI D3DXLoadSurfaceFromFileInMemory(LPDIRECT3DSURFACE9 pDestSurface, CONST PALETTEENTRY *pDestPalette,
    CONST RECT *pDestRect, LPCVOID pSrcData, UINT SrcDataSize, CONST RECT *pSrcRect, DWORD Filter, D3DCOLOR ColorKey,
    D3DXIMAGE_INFO *pSrcInfo)
{
    (void)pDestPalette;
    HRESULT result = D3DXGetImageInfoFromFileInMemory(pSrcData, SrcDataSize, pSrcInfo);
    if (FAILED(result) || pDestSurface == 0)
        return FAILED(result) ? result : D3DERR_INVALIDCALL;
    s_port_image_info info;
    uint32_t *pixels = 0;
    if (!port_image_decode(pSrcData, SrcDataSize, &info, &pixels))
        return D3DXERR_INVALIDDATA;

    D3DSURFACE_DESC desc;
    D3DSurface_GetDesc(pDestSurface, &desc);
    const D3DFORMAT format = desc.Format;
    const uint32_t texture_format = (uint32_t)format & D3DFORMAT_TEXTUREFORMAT_MASK;
    const bool dxt = texture_format == GPUTEXTUREFORMAT_DXT1 || texture_format == GPUTEXTUREFORMAT_DXT2_3 ||
                     texture_format == GPUTEXTUREFORMAT_DXT4_5;
    if (texture_format != GPUTEXTUREFORMAT_8_8_8_8 && !dxt) {
        static int reported;
        if (!reported++)
            port_write_error("d3dx: D3DXLoadSurfaceFromFileInMemory into a surface format the port does not pack "
                             "(only 8_8_8_8, DXT1, DXT2_3, DXT4_5): nothing written\n");
        free(pixels);
        return D3DXERR_INVALIDDATA;
    }
    // the rectangles: the source's (whole image by default), the destination's (whole surface by default), clipped
    RECT src = { 0, 0, (LONG)info.width, (LONG)info.height };
    if (pSrcRect != 0)
        src = *pSrcRect;
    RECT dst = { 0, 0, (LONG)desc.Width, (LONG)desc.Height };
    if (pDestRect != 0)
        dst = *pDestRect;
    src.left = src.left < 0 ? 0 : src.left;
    src.top = src.top < 0 ? 0 : src.top;
    src.right = src.right > (LONG)info.width ? (LONG)info.width : src.right;
    src.bottom = src.bottom > (LONG)info.height ? (LONG)info.height : src.bottom;
    dst.left = dst.left < 0 ? 0 : dst.left;
    dst.top = dst.top < 0 ? 0 : dst.top;
    dst.right = dst.right > (LONG)desc.Width ? (LONG)desc.Width : dst.right;
    dst.bottom = dst.bottom > (LONG)desc.Height ? (LONG)desc.Height : dst.bottom;
    if (src.right <= src.left || src.bottom <= src.top || dst.right <= dst.left || dst.bottom <= dst.top) {
        free(pixels);
        return D3DERR_INVALIDCALL;
    }
    uint32_t sw = (uint32_t)(src.right - src.left), sh = (uint32_t)(src.bottom - src.top);
    const uint32_t dw = (uint32_t)(dst.right - dst.left), dh = (uint32_t)(dst.bottom - dst.top);
    const uint32_t mode = Filter == D3DX_DEFAULT ? D3DX_FILTER_TRIANGLE : (Filter & 0xffu);
    uint32_t *image;
    if (mode == D3DX_FILTER_NONE) {
        // no resize: the overlap, copied
        sw = sw < dw ? sw : dw;
        sh = sh < dh ? sh : dh;
        image = (uint32_t *)calloc((size_t)dw * dh, 4u);
        if (image != 0)
            for (uint32_t y = 0; y < sh; y++)
                memcpy(image + (size_t)y * dw, pixels + (size_t)(src.top + (LONG)y) * info.width + src.left, sw * 4u);
    } else {
        image = resize(pixels + (size_t)src.top * info.width + src.left, info.width, sw, sh, dw, dh, mode);
    }
    free(pixels);
    if (image == 0)
        return E_OUTOFMEMORY;
    if (ColorKey != 0) {
        for (size_t i = 0; i < (size_t)dw * dh; i++)
            if (image[i] == ColorKey)
                image[i] = 0;
    }

    D3DLOCKED_RECT locked;
    D3DSurface_LockRect(pDestSurface, &locked, 0, 0);
    uint8_t *bits = (uint8_t *)locked.pBits;
    if (bits == 0) {
        free(image);
        return D3DERR_INVALIDCALL;
    }
    const bool tiled = XGIsTiledFormat(format) != 0;
    if (!dxt) {
        // 8_8_8_8 as a host word, in the format's channel order (ARGB, or ABGR when X and Z are swizzled the other way)
        const bool abgr = swizzled_abgr((uint32_t)format);
        const uint32_t width_units = (uint32_t)locked.Pitch / 4u;
        for (uint32_t y = 0; y < dh; y++) {
            for (uint32_t x = 0; x < dw; x++) {
                uint32_t p = image[(size_t)y * dw + x];
                if (abgr)
                    p = (p & 0xff00ff00u) | (p >> 16 & 0xffu) | (p & 0xffu) << 16;
                const uint32_t at = surface_offset((uint32_t)dst.left + x, (uint32_t)dst.top + y, (uint32_t)locked.Pitch,
                                                   width_units, 4u, tiled);
                memcpy(bits + at, &p, 4);
            }
        }
    } else {
        // whole 4x4 blocks: the rectangle is rounded out to the blocks it touches (the game aligns it to 4)
        const uint32_t block_bytes = texture_format == GPUTEXTUREFORMAT_DXT1 ? 8u : 16u;
        const uint32_t width_blocks = (uint32_t)locked.Pitch / block_bytes;
        const uint32_t bx0 = (uint32_t)dst.left / 4u, by0 = (uint32_t)dst.top / 4u;
        const uint32_t bx1 = ((uint32_t)dst.right + 3u) / 4u, by1 = ((uint32_t)dst.bottom + 3u) / 4u;
        for (uint32_t by = by0; by < by1; by++) {
            for (uint32_t bx = bx0; bx < bx1; bx++) {
                uint32_t texels[16];
                for (uint32_t i = 0; i < 16; i++) {
                    int32_t x = (int32_t)(bx * 4u + (i & 3u)) - dst.left, y = (int32_t)(by * 4u + (i >> 2)) - dst.top;
                    x = x < 0 ? 0 : x >= (int32_t)dw ? (int32_t)dw - 1 : x;
                    y = y < 0 ? 0 : y >= (int32_t)dh ? (int32_t)dh - 1 : y;
                    texels[i] = image[(size_t)y * dw + (uint32_t)x];
                }
                uint8_t block[16];
                if (texture_format == GPUTEXTUREFORMAT_DXT1) {
                    encode_color_block(texels, block, true);
                } else {
                    if (texture_format == GPUTEXTUREFORMAT_DXT2_3)
                        encode_explicit_alpha(texels, block);
                    else
                        encode_alpha_block(texels, block);
                    encode_color_block(texels, block + 8, false);
                }
                memcpy(bits + surface_offset(bx, by, (uint32_t)locked.Pitch, width_blocks, block_bytes, tiled), block,
                       block_bytes);
            }
        }
    }
    free(image);
    return S_OK;
}

HRESULT WINAPI D3DXSaveSurfaceToFileInMemory(LPD3DXBUFFER *ppDestBuf, D3DXIMAGE_FILEFORMAT DestFormat,
    LPDIRECT3DSURFACE9 pSrcSurface, CONST PALETTEENTRY *pSrcPalette, CONST RECT *pSrcRect)
{
    (void)pSrcPalette; // no paletted formats are read
    if (ppDestBuf != 0)
        *ppDestBuf = 0;
    if (ppDestBuf == 0 || pSrcSurface == 0)
        return D3DERR_INVALIDCALL;
    if (DestFormat != D3DXIFF_JPG && DestFormat != D3DXIFF_PNG) {
        static int reported;
        if (!reported++)
            port_write_error("d3dx: D3DXSaveSurfaceToFileInMemory writes JPEG and PNG only: nothing saved\n");
        return E_NOTIMPL;
    }
    D3DSURFACE_DESC desc;
    D3DSurface_GetDesc(pSrcSurface, &desc);
    // the source rectangle (the whole surface by default), clipped to the surface
    RECT rect = { 0, 0, (LONG)desc.Width, (LONG)desc.Height };
    if (pSrcRect != 0)
        rect = *pSrcRect;
    rect.left = rect.left < 0 ? 0 : rect.left;
    rect.top = rect.top < 0 ? 0 : rect.top;
    rect.right = rect.right > (LONG)desc.Width ? (LONG)desc.Width : rect.right;
    rect.bottom = rect.bottom > (LONG)desc.Height ? (LONG)desc.Height : rect.bottom;
    if (rect.right <= rect.left || rect.bottom <= rect.top)
        return D3DERR_INVALIDCALL;

    uint32_t *pixels = 0;
    bool has_alpha = false;
    HRESULT result = read_surface(pSrcSurface, rect, &pixels, &has_alpha);
    if (FAILED(result))
        return result;
    const uint32_t w = (uint32_t)(rect.right - rect.left), h = (uint32_t)(rect.bottom - rect.top);
    uint8_t *data = 0;
    uint32_t size = 0;
    const int encoded = port_image_encode(DestFormat == D3DXIFF_JPG ? k_port_image_file_jpeg : k_port_image_file_png,
                                          pixels, w, h, w, has_alpha ? 4u : 3u, PORT_IMAGE_JPEG_DEFAULT_QUALITY, &data,
                                          &size);
    free(pixels);
    if (!encoded)
        return E_OUTOFMEMORY; // a surface (at most 8192 x 8192) is within both formats' limits: memory ran out
    void *memory = malloc(sizeof(c_port_d3dx_buffer));
    if (memory == 0) {
        free(data);
        return E_OUTOFMEMORY;
    }
    *ppDestBuf = new (memory) c_port_d3dx_buffer(data, size);
    return S_OK;
}

} // extern "C"
