// port/data: Xbox 360 (Xenos) texture untiling, endian conversion and format transcoding (see texture_convert.h).
#include "texture_convert.h"

#include "data_crt.h"

#if defined(HALO3_64BIT) && defined(__SSE2__)
#ifdef _WIN32
#define __MM_MALLOC_H
#endif
#include <emmintrin.h>
#endif

namespace {

struct s_format_info {
    uint8_t block_width;      // texels per block (1 or 4)
    uint8_t block_height;
    uint8_t bytes_per_block;  // 0: unsupported
};

// GPUTEXTUREFORMAT -> block geometry (xenia FormatInfo; *_AS_16_16_16_16 share their base format's data).
static s_format_info format_info(uint32_t texture_format) {
    switch (texture_format) {
    case 2:  /* 8 */
        return { 1, 1, 1 };
    case 3:  /* 1_5_5_5 */
    case 4:  /* 5_6_5 */
    case 10: /* 8_8 */
    case 15: /* 4_4_4_4 */
    case 24: /* 16 */
    case 30: /* 16_FLOAT */
    case 27: /* 16_EXPAND */
        return { 1, 1, 2 };
    case 6:  /* 8_8_8_8 */
    case 7:  /* 2_10_10_10 */
    case 25: /* 16_16 */
    case 31: /* 16_16_FLOAT */
    case 28: /* 16_16_EXPAND */
    case 36: /* 32_FLOAT */
    case 50: /* 8_8_8_8_AS_16_16_16_16 */
    case 54: /* 2_10_10_10_AS_16_16_16_16 */
    case 63: /* 2_10_10_10_FLOAT_EDRAM */
        return { 1, 1, 4 };
    case 26: /* 16_16_16_16 */
    case 29: /* 16_16_16_16_EXPAND */
    case 32: /* 16_16_16_16_FLOAT */
    case 37: /* 32_32_FLOAT */
        return { 1, 1, 8 };
    case 38: /* 32_32_32_32_FLOAT */
        return { 1, 1, 16 };
    case 18: /* DXT1 */
    case 51: /* DXT1_AS_16_16_16_16 */
    case 58: /* DXT3A */
    case 59: /* DXT5A */
    case 60: /* CTX1 */
    case 61: /* DXT3A_AS_1_1_1_1 */
        return { 4, 4, 8 };
    case 19: /* DXT2_3 */
    case 20: /* DXT4_5 */
    case 49: /* DXN */
    case 52: /* DXT2_3_AS_16_16_16_16 */
    case 53: /* DXT4_5_AS_16_16_16_16 */
        return { 4, 4, 16 };
    default:
        return { 1, 1, 0 };
    }
}

static inline uint32_t align_up(uint32_t v, uint32_t a) { return (v + a - 1u) & ~(a - 1u); }
static inline uint32_t max_u32(uint32_t a, uint32_t b) { return a > b ? a : b; }
static inline uint32_t min_u32(uint32_t a, uint32_t b) { return a < b ? a : b; }

static inline uint32_t log2_ceil(uint32_t v) {
    uint32_t r = 0;
    while ((1u << r) < v) ++r;
    return r;
}

static inline uint32_t log2_floor(uint32_t v) {
    uint32_t r = 0;
    while (v >>= 1) ++r;
    return r;
}

static inline uint32_t next_pow2(uint32_t v) { return 1u << log2_ceil(v); }

static const uint32_t k_tile = 32;                  // macro tile width/height in blocks
static const uint32_t k_tile_depth = 4;             // 3D macro tile depth
static const uint32_t k_subresource_alignment = 4096;
static const uint32_t k_linear_row_alignment = 256; // bytes

// Level of the packed mip tail (xenia GetPackedMipLevel).
static uint32_t packed_mip_level(uint32_t width, uint32_t height) {
    uint32_t log2_size = log2_ceil(min_u32(width, height));
    return log2_size > 4 ? log2_size - 4 : 0;
}

// Position of `level` inside the packed mip tail, in blocks (xenia GetPackedMipOffset). Returns 0 if not packed.
static int packed_mip_offset(uint32_t width, uint32_t height, uint32_t depth, const s_format_info &fi, uint32_t level,
                             uint32_t *x_blocks, uint32_t *y_blocks, uint32_t *z_blocks) {
    uint32_t log2_width = log2_ceil(width);
    uint32_t log2_height = log2_ceil(height);
    uint32_t log2_size = min_u32(log2_width, log2_height);
    *x_blocks = *y_blocks = *z_blocks = 0;
    if (log2_size > 4 + level) return 0;
    uint32_t packed_base = log2_size > 4 ? log2_size - 4 : 0;
    uint32_t packed = level - packed_base;
    uint32_t x = 0, y = 0, z = 0;
    if (packed < 3) {
        if (log2_width > log2_height) y = 16u >> packed;
        else x = 16u >> packed;
    } else {
        uint32_t offset;
        if (log2_width > log2_height) {
            offset = (1u << (log2_width - packed_base)) >> (packed - 2);
            x = offset;
        } else {
            offset = (1u << (log2_height - packed_base)) >> (packed - 2);
            y = offset;
        }
        if (offset < 4) {
            uint32_t log2_depth = log2_ceil(depth);
            z = log2_depth > 1 + level ? (log2_depth - level) * 4 : 4;
        }
    }
    *x_blocks = x / fi.block_width;
    *y_blocks = y / fi.block_height;
    *z_blocks = z;
    return 1;
}

static inline uint32_t tiled_combine(uint32_t outer_inner_bytes, uint32_t bank, uint32_t pipe, uint32_t y_lsb) {
    return (y_lsb << 4) | (pipe << 6) | (bank << 11) | (outer_inner_bytes & 0xfu) |
           (((outer_inner_bytes >> 4) & 1u) << 5) | (((outer_inner_bytes >> 5) & 7u) << 8) |
           ((outer_inner_bytes >> 8) << 12);
}

static uint32_t layer_count(const s_xenos_texture_desc *desc) {
    switch (desc->dimension) {
    case k_xenos_dimension_cube:  return 6;
    case k_xenos_dimension_array: return desc->depth ? desc->depth : 1;
    default:                      return 1;
    }
}

static uint32_t volume_depth(const s_xenos_texture_desc *desc) {
    return desc->dimension == k_xenos_dimension_3d ? (desc->depth ? desc->depth : 1) : 1;
}

} // namespace

extern "C" uint32_t xenos_tiled_offset_2d(uint32_t x, uint32_t y, uint32_t pitch_blocks, uint32_t bytes_per_block_log2) {
    uint32_t outer = ((y >> 5) * (pitch_blocks >> 5) + (x >> 5)) << 6;
    uint32_t inner = (((y >> 1) & 7u) << 3) | (x & 7u);
    uint32_t bytes = (outer | inner) << bytes_per_block_log2;
    uint32_t bank = (y >> 4) & 1u;
    uint32_t pipe = ((x >> 3) & 3u) ^ (((y >> 3) & 1u) << 1);
    return tiled_combine(bytes, bank, pipe, y & 1u);
}

extern "C" uint32_t xenos_tiled_offset_3d(uint32_t x, uint32_t y, uint32_t z, uint32_t pitch_blocks, uint32_t height_blocks,
                                          uint32_t bytes_per_block_log2) {
    uint32_t outer = ((((z >> 2) * (height_blocks >> 4) + (y >> 4)) * (pitch_blocks >> 5)) + (x >> 5)) << 7;
    uint32_t inner = ((z & 3u) << 5) | (((y >> 1) & 3u) << 3) | (x & 7u);
    uint32_t bytes = (outer | inner) << bytes_per_block_log2;
    uint32_t bank = ((y >> 3) ^ (z >> 2)) & 1u;
    uint32_t pipe = ((x >> 3) & 3u) ^ (bank << 1);
    return tiled_combine(bytes, bank, pipe, y & 1u);
}

extern "C" uint32_t xenos_xg_address_2d_tiled_offset(uint32_t x, uint32_t y, uint32_t width_blocks, uint32_t bytes_per_block) {
    uint32_t aligned_width = (width_blocks + 31u) & ~31u;
    uint32_t log_bpp = (bytes_per_block >> 2) + ((bytes_per_block >> 1) >> (bytes_per_block >> 2));
    uint32_t macro = ((x >> 5) + (y >> 5) * (aligned_width >> 5)) << (log_bpp + 7);
    uint32_t micro = ((x & 7u) + ((y & 6u) << 2)) << log_bpp;
    uint32_t offset = macro + ((micro & ~15u) << 1) + (micro & 15u) + ((y & 8u) << (3 + log_bpp)) + ((y & 1u) << 4);
    return ((((offset & ~511u) << 3) + ((offset & 448u) << 2) + (offset & 63u) + ((y & 16u) << 7) +
             (((((y & 8u) >> 2) + (x >> 3)) & 3u) << 6)) >> log_bpp);
}

extern "C" int xenos_texture_get_layout(const s_xenos_texture_desc *desc, s_xenos_texture_layout *out) {
    memset(out, 0, sizeof(*out));
    s_format_info fi = format_info(xenos_d3dformat_texture_format(desc->d3d_format));
    if (!fi.bytes_per_block || !desc->width || !desc->height || !desc->levels || desc->levels > 16) return 0;
    if (desc->dimension > k_xenos_dimension_array) return 0;

    const uint32_t width = desc->width, height = desc->height, depth = volume_depth(desc);
    const uint32_t layers = layer_count(desc);
    const bool tiled = xenos_d3dformat_tiled(desc->d3d_format) != 0;
    const bool is_3d = desc->dimension == k_xenos_dimension_3d;
    const uint32_t bpb = fi.bytes_per_block;

    out->block_width = fi.block_width;
    out->block_height = fi.block_height;
    out->bytes_per_block = bpb;
    out->layers = layers;
    out->packed_level = desc->packed_mips ? packed_mip_level(width, height) : 0xffffffffu;

    // D3D's FindTextureSize aligns non-base array levels to four slices (xenia GetGuestTextureLayout).
    const uint32_t mip_layers = desc->dimension == k_xenos_dimension_array && layers > 1 ? align_up(layers, k_tile_depth) : layers;

    uint32_t mip_offset = 0;
    for (uint32_t level = 0; level < desc->levels; ++level) {
        s_xenos_level_layout &l = out->levels[level];
        const bool is_base = level == 0;
        // levels at or past the packed tail share the tail's storage
        const uint32_t storage_level = min_u32(level, out->packed_level);

        uint32_t pitch_texels, rows_texels, slices;
        if (is_base) {
            pitch_texels = width;
            rows_texels = height;
            slices = depth;
        } else {
            pitch_texels = max_u32(next_pow2(width) >> storage_level, 1u);
            rows_texels = max_u32(next_pow2(height) >> storage_level, 1u);
            slices = max_u32(next_pow2(depth) >> storage_level, 1u);
        }
        uint32_t pitch_alignment = k_tile;
        if (!tiled) pitch_alignment = max_u32(k_linear_row_alignment / bpb, k_tile);
        uint32_t pitch_blocks = align_up(align_up(pitch_texels, fi.block_width) / fi.block_width, pitch_alignment);
        uint32_t height_blocks = align_up(align_up(rows_texels, fi.block_height) / fi.block_height, k_tile);

        l.from_mip_address = is_base ? 0u : 1u;
        l.row_pitch_bytes = pitch_blocks * bpb;
        l.height_blocks = height_blocks;
        uint32_t slice_stride = l.row_pitch_bytes * height_blocks;
        if (is_3d) slice_stride *= align_up(slices, k_tile_depth);
        l.slice_stride = align_up(slice_stride, k_subresource_alignment);

        if (level >= out->packed_level) {
            l.packed = 1;
            packed_mip_offset(width, height, depth, fi, level, &l.packed_x_blocks, &l.packed_y_blocks, &l.packed_z);
        }
        if (is_base) {
            l.offset = 0;
            out->base_size = l.slice_stride * layers;
        } else {
            // the tail is one storage level: every level after its first shares its offset
            const uint32_t tail_first = out->packed_level == 0 ? 1u : out->packed_level;
            if (level <= tail_first) {
                l.offset = mip_offset;
                mip_offset += l.slice_stride * mip_layers;
            } else {
                l.offset = out->levels[tail_first].offset;
            }
        }
    }
    out->mip_size = mip_offset;
    // XGHEADER_CONTIGUOUS_MIP_OFFSET: mips follow the 4 KB-aligned base, except that a texture whose base is already a
    // packed tail (shorter side <= 16) keeps its mip tail at the base address (all 31579 retail Halo 3 bitmaps agree).
    out->contiguous_mip_offset = out->packed_level == 0 ? 0u : out->base_size;
    return 1;
}

namespace {

// Where a level's blocks are in its source: byte offset, from the level's address (base or mip), of block (bx, by) of
// depth slice z, with the per-level constants computed once (texture_view converts on the draw thread: a 2048x2048
// texture has a third of a million blocks, and per block the division and the log2 loop cost more than the copy).
struct s_level_addressing {
    uint32_t base;          // the level's offset plus the layer's
    uint32_t x0, y0, z0;    // the level's position in a packed tail
    uint32_t pitch_blocks, height_blocks, row_pitch_bytes, bpb_log2;
    uint32_t mode;          // 0 linear, 1 tiled 2D, 2 tiled 3D

    inline uint32_t offset(uint32_t bx, uint32_t by, uint32_t z) const {
        const uint32_t x = bx + x0, y = by + y0, zz = z + z0;
        uint32_t o;
        if (mode == 1) o = xenos_tiled_offset_2d(x, y, pitch_blocks, bpb_log2);
        else if (mode == 2) o = xenos_tiled_offset_3d(x, y, zz, pitch_blocks, height_blocks, bpb_log2);
        else o = (zz * height_blocks + y) * row_pitch_bytes + (x << bpb_log2);
        return base + o;
    }
};

static s_level_addressing level_addressing(const s_xenos_texture_desc *desc, const s_xenos_texture_layout &layout,
                                           uint32_t level, uint32_t layer) {
    const s_xenos_level_layout &l = layout.levels[level];
    s_level_addressing a;
    a.base = l.offset + layer * l.slice_stride;
    a.x0 = l.packed_x_blocks;
    a.y0 = l.packed_y_blocks;
    a.z0 = l.packed_z;
    a.pitch_blocks = l.row_pitch_bytes / layout.bytes_per_block;
    a.height_blocks = l.height_blocks;
    a.row_pitch_bytes = l.row_pitch_bytes;
    a.bpb_log2 = log2_floor(layout.bytes_per_block);
    a.mode = !xenos_d3dformat_tiled(desc->d3d_format) ? 0u : desc->dimension == k_xenos_dimension_3d ? 2u : 1u;
    return a;
}

// The largest tiled 2D offset (xenos_tiled_offset_2d) over the blocks [x0, x1] x [y0, y1], found without visiting
// them all. The offset is ((tile * 64 + inner) << log2 >> 8) * 4096 plus bank, pipe and low address bits below 4096,
// so it grows with the tile index but for those low bits: the tiles are visited from the largest index down, each
// one's blocks in full, until a tile's largest possible offset is below the largest found. The answer is the full
// scan's (xenos_texture_get_source_extent compares them for every retail bitmap: tmp/perf_hitch/tests). Returns 0
// when the rows of tiles are narrower than the rectangle (not a layout D3D makes): the caller then scans every block.
static int tiled_2d_max_offset(uint32_t x0, uint32_t x1, uint32_t y0, uint32_t y1, uint32_t pitch_blocks, uint32_t log2,
                               uint32_t *out) {
    const uint32_t tiles_per_row = pitch_blocks >> 5;
    if (tiles_per_row == 0 || (x1 >> 5) >= tiles_per_row) return 0;
    uint32_t best = 0;
    bool found = false;
    for (uint32_t t = (y1 >> 5) * tiles_per_row + (x1 >> 5) + 1u; t-- > 0;) {
        const uint32_t ty = t / tiles_per_row, tx = t % tiles_per_row;
        if (ty < (y0 >> 5)) break;
        if (tx < (x0 >> 5) || tx > (x1 >> 5)) continue;
        const uint64_t bound = ((((uint64_t)t << 6 | 63u) << log2) >> 8) * 4096u + 4095u;
        if (found && bound <= best) break;
        const uint32_t xa = max_u32(x0, tx << 5), xb = min_u32(x1, (tx << 5) | 31u);
        const uint32_t ya = max_u32(y0, ty << 5), yb = min_u32(y1, (ty << 5) | 31u);
        for (uint32_t y = ya; y <= yb; ++y)
            for (uint32_t x = xa; x <= xb; ++x) {
                const uint32_t o = xenos_tiled_offset_2d(x, y, pitch_blocks, log2);
                if (!found || o > best) best = o;
                found = true;
            }
    }
    *out = best;
    return found ? 1 : 0;
}


static inline uint32_t level_dim(uint32_t base, uint32_t level) { return max_u32(base >> level, 1u); }

} // namespace

extern "C" int xenos_texture_get_source_extent(const s_xenos_texture_desc *desc, uint32_t *out_base_extent, uint32_t *out_mip_extent) {
    s_xenos_texture_layout layout;
    if (!xenos_texture_get_layout(desc, &layout)) return 0;
    uint32_t extent[2] = { 0, 0 };
    const uint32_t depth = volume_depth(desc);
    for (uint32_t level = 0; level < desc->levels; ++level) {
        const s_xenos_level_layout &l = layout.levels[level];
        const uint32_t wb = align_up(level_dim(desc->width, level), layout.block_width) / layout.block_width;
        const uint32_t hb = align_up(level_dim(desc->height, level), layout.block_height) / layout.block_height;
        const uint32_t d = level_dim(depth, level);
        uint32_t &e = extent[l.from_mip_address];
        // the last layer's blocks end furthest (the layers are slice_stride apart)
        const s_level_addressing a = level_addressing(desc, layout, level, layout.layers - 1u);
        uint32_t largest = 0;
        if (a.mode == 0) {
            // linear: the last block of the last slice
            largest = a.offset(wb - 1u, hb - 1u, d - 1u);
        } else if (a.mode == 1 && tiled_2d_max_offset(a.x0, a.x0 + wb - 1u, a.y0, a.y0 + hb - 1u, a.pitch_blocks,
                                                       a.bpb_log2, &largest)) {
            largest += a.base;
        } else {
            for (uint32_t z = 0; z < d; ++z)
                for (uint32_t by = 0; by < hb; ++by)
                    for (uint32_t bx = 0; bx < wb; ++bx)
                        largest = max_u32(largest, a.offset(bx, by, z));
        }
        e = max_u32(e, largest + layout.bytes_per_block);
    }
    *out_base_extent = extent[0];
    *out_mip_extent = extent[1];
    return 1;
}

namespace {

// How a source block becomes output.
enum e_transcode {
    k_transcode_copy,          // endian swap only (BCn blocks, 8/16/32-bit channels, 5_6_5, 2_10_10_10)
    k_transcode_1555,          // X5 Y5 Z5 W1 (X in the low bits) -> A1R5G5B5
    k_transcode_4444,          // X4 Y4 Z4 W4 (X in the low bits) -> B4G4R4A4
    k_transcode_dxt3a,         // 4x4 block of 4-bit values -> R8
    k_transcode_dxt3a_1111,    // 4x4 block of 4-bit values, one bit per channel -> R8G8B8A8
    k_transcode_ctx1,          // 4x4 two-channel block -> R8G8
    k_transcode_ctx1_bias,     // CTX1 with GPUSIGN_BIAS: decoded, then u8 -> s8 (^0x80) -> R8G8_SNORM
    k_transcode_bc5_bias,      // DXN (BC5) with GPUSIGN_BIAS: endpoints u8 -> s8 (^0x80) -> BC5_SNORM
    k_transcode_7e3,           // 2_10_10_10 with 7e3 floats -> R16G16B16A16_SFLOAT
    k_transcode_bc4,           // DXT5A (BC4) block -> 8-bit texels (only for GPUSIGN_GAMMA, see choose_conversion)
    k_transcode_bc5,           // DXN (BC5) block -> 8-bit texel pairs (likewise)
    k_transcode_packed_gamma,  // 1_5_5_5 / 5_6_5 / 4_4_4_4 with GPUSIGN_GAMMA channels -> R16G16B16A16_UNORM, linear
};

struct s_conversion {
    uint32_t pc_format;
    uint32_t transcode;
    uint32_t out_block_width;   // texels per output block
    uint32_t out_block_height;
    uint32_t out_bytes;         // bytes per output block (texel for decoded formats)
    uint32_t gamma;             // bit c: channel c goes through the PWL degamma into 16 bits (8-bit channels only)
    uint32_t channels;          // with gamma: 8-bit channels per decoded texel
    uint32_t gamma_channels;    // bit c: stored channel c is GPUSIGN_GAMMA
    uint32_t linear;            // bit c: GPUSIGN_GAMMA channel c is linear in the output (gamma, or an *_SRGB format)
};

static bool all_signed(uint32_t d3d_format, uint32_t channels) {
    for (uint32_t c = 0; c < channels; ++c)
        if (xenos_d3dformat_sign(d3d_format, c) != 1u) return false;
    return true;
}

// GPUSIGN_BIAS on every channel (Halo 3 tags its DXN / CTX1 normal maps so): the fetch returns 2x - 1. Flipping the top
// bit of the 8-bit values and reading them as SNORM gives (u - 128) / 127, within 1/127 of it; the block formats
// interpolate the same way in either encoding, as the flip preserves the order of the endpoints.
static bool all_biased(uint32_t d3d_format, uint32_t channels) {
    for (uint32_t c = 0; c < channels; ++c)
        if (xenos_d3dformat_sign(d3d_format, c) != 2u) return false;
    return true;
}

// Components a texture format stores (GPUTEXTUREFORMAT); the fetch fills the others from the last one.
static uint32_t native_component_count(uint32_t texture_format) {
    switch (texture_format) {
    case 2:  // 8
    case 24: // 16
    case 27: // 16_EXPAND
    case 30: // 16_FLOAT
    case 36: // 32_FLOAT
    case 58: // DXT3A
    case 59: // DXT5A
        return 1;
    case 10: // 8_8
    case 25: // 16_16
    case 28: // 16_16_EXPAND
    case 31: // 16_16_FLOAT
    case 37: // 32_32_FLOAT
    case 49: // DXN
    case 60: // CTX1
        return 2;
    case 4:  // 5_6_5
        return 3;
    default:
        return 4;
    }
}

// 8-bit channels per texel of the formats whose texels (as stored, or as decoded) are 8 bits a channel; 0 otherwise.
static uint32_t eight_bit_channels(uint32_t texture_format) {
    switch (texture_format) {
    case 2:  // 8
    case 58: // DXT3A
    case 59: // DXT5A
        return 1;
    case 10: // 8_8
    case 49: // DXN
    case 60: // CTX1
        return 2;
    case 6:  // 8_8_8_8
    case 50: // 8_8_8_8_AS_16_16_16_16
    case 61: // DXT3A_AS_1_1_1_1
        return 4;
    default:
        return 0;
    }
}

static int choose_conversion(uint32_t d3d_format, s_conversion *out) {
    const uint32_t tf = xenos_d3dformat_texture_format(d3d_format);
    s_conversion c = { k_pc_format_undefined, k_transcode_copy, 1, 1, 0, 0, 0, 0, 0 };
    switch (tf) {
    case 2:  c.pc_format = all_signed(d3d_format, 1) ? k_pc_format_r8_snorm : k_pc_format_r8_unorm; c.out_bytes = 1; break;
    case 3:  c.pc_format = k_pc_format_a1r5g5b5_unorm_pack16; c.transcode = k_transcode_1555; c.out_bytes = 2; break;
    case 4:  c.pc_format = k_pc_format_b5g6r5_unorm_pack16; c.out_bytes = 2; break;
    case 6:
    case 50: c.pc_format = all_signed(d3d_format, 4) ? k_pc_format_r8g8b8a8_snorm : k_pc_format_r8g8b8a8_unorm; c.out_bytes = 4; break;
    case 7:
    case 54: c.pc_format = k_pc_format_a2b10g10r10_unorm_pack32; c.out_bytes = 4; break;
    case 10: c.pc_format = all_signed(d3d_format, 2) ? k_pc_format_r8g8_snorm : k_pc_format_r8g8_unorm; c.out_bytes = 2; break;
    case 15: c.pc_format = k_pc_format_b4g4r4a4_unorm_pack16; c.transcode = k_transcode_4444; c.out_bytes = 2; break;
    case 24: c.pc_format = all_signed(d3d_format, 1) ? k_pc_format_r16_snorm : k_pc_format_r16_unorm; c.out_bytes = 2; break;
    case 25: c.pc_format = all_signed(d3d_format, 2) ? k_pc_format_r16g16_snorm : k_pc_format_r16g16_unorm; c.out_bytes = 4; break;
    case 26: c.pc_format = all_signed(d3d_format, 4) ? k_pc_format_r16g16b16a16_snorm : k_pc_format_r16g16b16a16_unorm; c.out_bytes = 8; break;
    // Xenos half floats keep exponent 31 finite (no inf/NaN); values that large do not occur in texture data
    case 27:
    case 30: c.pc_format = k_pc_format_r16_sfloat; c.out_bytes = 2; break;
    case 28:
    case 31: c.pc_format = k_pc_format_r16g16_sfloat; c.out_bytes = 4; break;
    case 29:
    case 32: c.pc_format = k_pc_format_r16g16b16a16_sfloat; c.out_bytes = 8; break;
    case 36: c.pc_format = k_pc_format_r32_sfloat; c.out_bytes = 4; break;
    case 37: c.pc_format = k_pc_format_r32g32_sfloat; c.out_bytes = 8; break;
    case 38: c.pc_format = k_pc_format_r32g32b32a32_sfloat; c.out_bytes = 16; break;
    case 63: c.pc_format = k_pc_format_r16g16b16a16_sfloat; c.transcode = k_transcode_7e3; c.out_bytes = 8; break;
    // block-compressed formats whose blocks are the PC BCn blocks once byte-swapped
    case 18:
    case 51: c.pc_format = k_pc_format_bc1_rgba_unorm; c.out_block_width = c.out_block_height = 4; c.out_bytes = 8; break;
    case 19:
    case 52: c.pc_format = k_pc_format_bc2_unorm; c.out_block_width = c.out_block_height = 4; c.out_bytes = 16; break;
    case 20:
    case 53: c.pc_format = k_pc_format_bc3_unorm; c.out_block_width = c.out_block_height = 4; c.out_bytes = 16; break;
    case 49:
        if (all_biased(d3d_format, 2)) { c.pc_format = k_pc_format_bc5_snorm; c.transcode = k_transcode_bc5_bias; }
        else { c.pc_format = k_pc_format_bc5_unorm; }
        c.out_block_width = c.out_block_height = 4; c.out_bytes = 16; break;
    case 59: c.pc_format = k_pc_format_bc4_unorm; c.out_block_width = c.out_block_height = 4; c.out_bytes = 8; break;
    // Xenos-only block formats, decoded exactly. CTX1 interpolates in thirds and DXT3A stores 16 free 4-bit levels;
    // neither fits BC4/BC5 (sevenths/fifths, two endpoints) without re-quantizing, so they become plain texels.
    case 58: c.pc_format = k_pc_format_r8_unorm; c.transcode = k_transcode_dxt3a; c.out_bytes = 1; break;
    case 60:
        if (all_biased(d3d_format, 2)) { c.pc_format = k_pc_format_r8g8_snorm; c.transcode = k_transcode_ctx1_bias; }
        else { c.pc_format = k_pc_format_r8g8_unorm; c.transcode = k_transcode_ctx1; }
        c.out_bytes = 2; break;
    case 61: c.pc_format = k_pc_format_r8g8b8a8_unorm; c.transcode = k_transcode_dxt3a_1111; c.out_bytes = 4; break;
    default: return 0;
    }
    // GPUSIGN_GAMMA (D3DFMT_*_GAMMA; Halo 3 tags its gamma bitmaps `| 0x7e00`: X, Y and Z): the fetch returns the
    // Xenos PWL degamma of the channel (xenia PWLGammaToLinear). Formats of 8-bit channels widen to 16-bit UNORM with
    // that curve applied exactly; BC1-3 take the *_SRGB format, the closest a block format can get (the sRGB curve is
    // within 0.056 of the PWL, worst at code 191: 0.521 against 0.465; RGB only, which is what `| 0x7e00` asks for).
    uint32_t gamma = 0;
    for (uint32_t ch = 0; ch < native_component_count(tf); ++ch)
        if (xenos_d3dformat_sign(d3d_format, ch) == 3u) gamma |= 1u << ch;
    c.gamma_channels = gamma;
    if (gamma != 0) {
        const uint32_t channels = eight_bit_channels(tf);
        if (channels != 0) {
            static const uint32_t k_unorm16[5] = { 0, k_pc_format_r16_unorm, k_pc_format_r16g16_unorm, 0,
                                                   k_pc_format_r16g16b16a16_unorm };
            c.pc_format = k_unorm16[channels];
            c.out_block_width = c.out_block_height = 1;
            c.out_bytes = channels * 2u;
            if (tf == 59) c.transcode = k_transcode_bc4;
            if (tf == 49) c.transcode = k_transcode_bc5;
            c.gamma = c.linear = gamma;
            c.channels = channels;
        } else if (tf == 3 || tf == 4 || tf == 15) {
            // Packed 16-bit texels: the fetch degammas the channel's own n-bit value (xenia applies PWLGammaToLinear to
            // the fetched value whatever the format). Halo 3's emblem and medal sheets are 4_4_4_4 `| 0x7e00`, one
            // illumination map 5_6_5. Each channel widens to 16-bit UNORM, through the curve where it is gamma.
            c.pc_format = k_pc_format_r16g16b16a16_unorm;
            c.transcode = k_transcode_packed_gamma;
            c.out_bytes = 8;
            c.linear = gamma;
        } else if ((gamma & 7u) == 7u && (c.pc_format == k_pc_format_bc1_rgba_unorm || c.pc_format == k_pc_format_bc2_unorm ||
                                          c.pc_format == k_pc_format_bc3_unorm)) {
            c.pc_format += 1u; // VK_FORMAT_BCn_*_SRGB_BLOCK follows its UNORM format
            c.linear = 7u;
        }
    }
    *out = c;
    return 1;
}

static void endian_swap(uint8_t *p, uint32_t n, uint32_t endian) {
    switch (endian) {
    case k_xenos_endian_8in16:
        for (uint32_t i = 0; i + 1 < n; i += 2) { uint8_t t = p[i]; p[i] = p[i + 1]; p[i + 1] = t; }
        break;
    case k_xenos_endian_8in32:
        for (uint32_t i = 0; i + 3 < n; i += 4) {
            uint8_t a = p[i], b = p[i + 1];
            p[i] = p[i + 3]; p[i + 1] = p[i + 2]; p[i + 2] = b; p[i + 3] = a;
        }
        break;
    case k_xenos_endian_16in32:
        for (uint32_t i = 0; i + 3 < n; i += 4) {
            uint8_t a = p[i], b = p[i + 1];
            p[i] = p[i + 2]; p[i + 1] = p[i + 3]; p[i + 2] = a; p[i + 3] = b;
        }
        break;
    default:
        break;
    }
}

// endian_swap of a whole block: blocks of 4, 8 and 16 bytes a word at a time (the same bytes as endian_swap).
static inline void swap_block(uint8_t *p, uint32_t n, uint32_t endian) {
    if (endian == 0) return;
    if (n < 4) { endian_swap(p, n, endian); return; }
    for (uint32_t i = 0; i < n; i += 4) {
        uint32_t w;
        memcpy(&w, p + i, 4);
        if (endian == k_xenos_endian_8in16) w = ((w & 0x00ff00ffu) << 8) | ((w >> 8) & 0x00ff00ffu);
        else if (endian == k_xenos_endian_8in32)
            w = (w >> 24) | ((w >> 8) & 0xff00u) | ((w << 8) & 0xff0000u) | (w << 24);
        else if (endian == k_xenos_endian_16in32) w = (w >> 16) | (w << 16);
        memcpy(p + i, &w, 4);
    }
}

// The blocks of one slice of a level copied as they are but for their byte order (BCn blocks, plain texels), with the
// block size a constant: the same bytes the general block loop writes. False when a block lies outside the source.
template <uint32_t k_bpb>
static bool copy_blocks(uint8_t *slice, uint32_t row_pitch, const s_level_addressing &a, uint32_t wb, uint32_t hb,
                        uint32_t z, const uint8_t *s, size_t ssize, uint32_t endian) {
    for (uint32_t by = 0; by < hb; ++by) {
        uint8_t *row = slice + by * row_pitch;
        for (uint32_t bx = 0; bx < wb; ++bx) {
            const uint32_t so = a.offset(bx, by, z);
            if ((uint64_t)so + k_bpb > ssize) return false;
            uint8_t block[k_bpb];
            memcpy(block, s + so, k_bpb);
            swap_block(block, k_bpb, endian);
            memcpy(row + bx * k_bpb, block, k_bpb);
        }
    }
    return true;
}

static inline uint16_t rd16le(const uint8_t *p) { return (uint16_t)(p[0] | (p[1] << 8)); }
static inline uint32_t rd32le(const uint8_t *p) { return (uint32_t)p[0] | ((uint32_t)p[1] << 8) | ((uint32_t)p[2] << 16) | ((uint32_t)p[3] << 24); }
static inline void wr16le(uint8_t *p, uint16_t v) { p[0] = (uint8_t)v; p[1] = (uint8_t)(v >> 8); }

#if defined(HALO3_64BIT) && defined(__SSE2__)
template <uint32_t k_bpb, e_transcode k_transcode>
[[gnu::noinline]] static bool copy_tiled_2d_tail(uint8_t *row, const s_level_addressing &a, uint32_t bx, uint32_t wb,
                                                uint32_t by, const uint8_t *src, size_t ssize, uint32_t endian) {
    for (; bx < wb; ++bx) {
        const uint32_t so = a.offset(bx, by, 0);
        if ((uint64_t)so + k_bpb > ssize) return false;
        uint8_t block[k_bpb];
        memcpy(block, src + so, k_bpb);
        swap_block(block, k_bpb, endian);
        if (k_transcode == k_transcode_bc5_bias) {
            block[0] ^= 0x80u;
            block[1] ^= 0x80u;
            block[8] ^= 0x80u;
            block[9] ^= 0x80u;
        }
        if (k_transcode == k_transcode_1555) {
            const uint32_t v = rd16le(block);
            wr16le(block, (uint16_t)((v & 0x83e0u) | ((v & 0x1fu) << 10) | ((v >> 10) & 0x1fu)));
        }
        if (k_transcode == k_transcode_4444) {
            const uint32_t v = rd16le(block);
            wr16le(block, (uint16_t)((v << 4) | (v >> 12)));
        }
        memcpy(row + bx * k_bpb, block, k_bpb);
    }
    return true;
}

template <uint32_t k_bpb, e_transcode k_transcode, uint32_t k_endian>
[[gnu::noinline]] static bool copy_tiled_2d(uint8_t *slice, uint32_t row_pitch, const s_level_addressing &a,
                                           uint32_t wb, uint32_t hb, const uint8_t *src, size_t ssize) {
    constexpr uint32_t k_log_bpb = k_bpb == 16 ? 4 : k_bpb == 8 ? 3 : k_bpb == 4 ? 2 : k_bpb == 2 ? 1 : 0;
    constexpr uint32_t k_run_bytes = k_bpb == 1 ? 8 : 16;
    constexpr uint32_t k_runs = 8 * k_bpb / k_run_bytes;
    // Eight horizontal blocks share the tile, bank and pipe. Within them, tiled_combine keeps byte bits 0..3
    // contiguous and maps bits 4..6 to bits 5, 8 and 9. Thus BC1 has two blocks per load and BC2/3/5 one.
    constexpr uint32_t k_last_run = (((k_runs - 1) & 1u) << 5) | (((k_runs - 1) & 6u) << 7);
    const uint64_t limit = ssize < 0x100000000ull ? ssize : 0x100000000ull;
    const uint32_t group_end = wb & ~7u;
    for (uint32_t by = 0; by < hb; ++by) {
        uint8_t *row = slice + by * row_pitch;
        uint32_t bx = 0;
        for (; bx < group_end; bx += 8) {
            const uint32_t so = a.base + xenos_tiled_offset_2d(bx, by, a.pitch_blocks, k_log_bpb);
            // The scalar tail checks truncated or wrapping groups in the original block order.
            if ((uint64_t)so + k_last_run + k_run_bytes > limit) break;
            for (uint32_t i = 0; i < k_runs; ++i) {
                const uint32_t run_offset = ((i & 1u) << 5) | ((i & 6u) << 7);
                const uint8_t *s = src + so + run_offset;
                uint8_t *d = row + bx * k_bpb + i * k_run_bytes;
                if (k_bpb == 1) {
                    memcpy(d, s, k_run_bytes);
                    continue;
                }
                __m128i v = _mm_loadu_si128((const __m128i *)s);
                if (k_transcode == k_transcode_4444) {
                    constexpr int k_rotate = k_endian == k_xenos_endian_8in16 ? 12 : 4;
                    v = _mm_or_si128(_mm_slli_epi16(v, k_rotate), _mm_srli_epi16(v, 16 - k_rotate));
                } else if (k_endian == k_xenos_endian_8in16 || k_endian == k_xenos_endian_8in32) {
                    v = _mm_or_si128(_mm_slli_epi16(v, 8), _mm_srli_epi16(v, 8));
                }
                if (k_endian == k_xenos_endian_8in32 || k_endian == k_xenos_endian_16in32)
                    v = _mm_or_si128(_mm_slli_epi32(v, 16), _mm_srli_epi32(v, 16));
                if (k_transcode == k_transcode_bc5_bias)
                    v = _mm_xor_si128(v, _mm_set1_epi64x(0x8080));
                if (k_transcode == k_transcode_1555) {
                    const __m128i channel_mask = _mm_set1_epi16(0x1f);
                    const __m128i x = _mm_slli_epi16(_mm_and_si128(v, channel_mask), 10);
                    const __m128i z = _mm_and_si128(_mm_srli_epi16(v, 10), channel_mask);
                    v = _mm_or_si128(_mm_and_si128(v, _mm_set1_epi16((short)0x83e0)), _mm_or_si128(x, z));
                }
                _mm_storeu_si128((__m128i *)d, v);
            }
        }
        if (bx < wb && !copy_tiled_2d_tail<k_bpb, k_transcode>(row, a, bx, wb, by, src, ssize, k_endian)) return false;
    }
    return true;
}

template <uint32_t k_bpb, e_transcode k_transcode = k_transcode_copy>
static bool copy_tiled_2d(uint8_t *slice, uint32_t row_pitch, const s_level_addressing &a, uint32_t wb, uint32_t hb,
                          const uint8_t *src, size_t ssize, uint32_t endian) {
    if (k_bpb > 1 && endian == k_xenos_endian_8in16)
        return copy_tiled_2d<k_bpb, k_transcode, k_xenos_endian_8in16>(slice, row_pitch, a, wb, hb, src, ssize);
    if (k_bpb >= 4 && endian == k_xenos_endian_8in32)
        return copy_tiled_2d<k_bpb, k_transcode, k_xenos_endian_8in32>(slice, row_pitch, a, wb, hb, src, ssize);
    if (k_bpb >= 4 && endian == k_xenos_endian_16in32)
        return copy_tiled_2d<k_bpb, k_transcode, k_xenos_endian_16in32>(slice, row_pitch, a, wb, hb, src, ssize);
    return copy_tiled_2d<k_bpb, k_transcode, k_xenos_endian_none>(slice, row_pitch, a, wb, hb, src, ssize);
}
#endif

// Writes a decoded 4x4 block's texels (texel (x, y) at texels + (y * 4 + x) * bytes), clipped to the level size.
// A decoded 4x4 block's texels (k_bytes each) into the output, clipped to the level: a block wholly inside it is
// four row copies of a size the compiler knows (the texel at a time copy of a runtime size was a call per texel).
template <uint32_t k_bytes>
static inline void store_texel_block(uint8_t *dst, uint32_t row_pitch, uint32_t x0, uint32_t y0, uint32_t width,
                                     uint32_t height, const uint8_t *texels) {
    if (x0 + 4 <= width && y0 + 4 <= height) {
        for (uint32_t y = 0; y < 4; ++y)
            memcpy(dst + (y0 + y) * row_pitch + x0 * k_bytes, texels + y * 4 * k_bytes, 4 * k_bytes);
        return;
    }
    for (uint32_t y = 0; y < 4 && y0 + y < height; ++y)
        for (uint32_t x = 0; x < 4 && x0 + x < width; ++x)
            memcpy(dst + (y0 + y) * row_pitch + (x0 + x) * k_bytes, texels + (y * 4 + x) * k_bytes, k_bytes);
}

#if defined(HALO3_64BIT) && defined(__SSE2__)
static inline void store_full_ctx1(uint8_t *dst, uint32_t pitch, uint32_t indices, const uint8_t palette[4][2],
                                   bool biased) {
    const uint32_t bias = biased ? 0x8080u : 0;
    const __m128i p0 = _mm_set1_epi16((short)(rd16le(palette[0]) ^ bias));
    const __m128i p1 = _mm_set1_epi16((short)(rd16le(palette[1]) ^ bias));
    const __m128i p2 = _mm_set1_epi16((short)(rd16le(palette[2]) ^ bias));
    const __m128i p3 = _mm_set1_epi16((short)(rd16le(palette[3]) ^ bias));
    const __m128i low_delta = _mm_xor_si128(p0, p1), high_delta = _mm_xor_si128(p2, p3);
    // Each product moves one texel's two-bit index to the top of its 16-bit lane.
    const __m128i shifts = _mm_setr_epi16((short)0x4000, 0x1000, 0x400, 0x100, 0x40, 0x10, 4, 1);
    for (uint32_t y = 0; y < 4; y += 2) {
        const __m128i codes = _mm_mullo_epi16(_mm_set1_epi16((short)(indices >> (8 * y))), shifts);
        const __m128i lo = _mm_srai_epi16(_mm_slli_epi16(codes, 1), 15), hi = _mm_srai_epi16(codes, 15);
        const __m128i a = _mm_xor_si128(p0, _mm_and_si128(lo, low_delta));
        const __m128i b = _mm_xor_si128(p2, _mm_and_si128(lo, high_delta));
        const __m128i texels = _mm_xor_si128(a, _mm_and_si128(hi, _mm_xor_si128(a, b)));
        _mm_storel_epi64((__m128i *)(dst + y * pitch), texels);
        _mm_storel_epi64((__m128i *)(dst + (y + 1) * pitch), _mm_srli_si128(texels, 8));
    }
}
#endif

// CTX1 (endian-swapped): bytes 0..3 = X0 Y0 X1 Y1 endpoints, bytes 4..7 = 2-bit codes, texel (x, y) at bit 2(4y + x).
// Codes are DXT's: 0 -> e0, 1 -> e1, 2 -> (2 e0 + e1) / 3, 3 -> (e0 + 2 e1) / 3, truncated (xenia texture_load_ctx1).
#if defined(HALO3_64BIT) && defined(__SSE2__)
template <bool k_full_block = false>
static void decode_ctx1(const uint8_t *block, uint8_t *texels, uint32_t pitch = 0, bool biased = false) {
#else
static void decode_ctx1(const uint8_t *block, uint8_t texels[16 * 2]) {
#endif
    // stored big-endian as X0 Y0 X1 Y1; after the 8in16 swap endpoint k is (X = byte 2k + 1, Y = byte 2k)
    const uint8_t e0x = block[1], e0y = block[0], e1x = block[3], e1y = block[2];
    const uint32_t codes = rd32le(block + 4);
    // the four values of the block, once (code c weighs e1 by k_weight_high[c] / 3)
    static const uint8_t k_weight_high[4] = { 0, 3, 1, 2 };
    uint8_t palette[4][2];
    for (uint32_t c = 0; c < 4; ++c) {
        const uint32_t wh = k_weight_high[c], wl = 3u - wh;
        palette[c][0] = (uint8_t)((wl * e0x + wh * e1x) / 3u);
        palette[c][1] = (uint8_t)((wl * e0y + wh * e1y) / 3u);
    }
#if defined(HALO3_64BIT) && defined(__SSE2__)
    if (k_full_block) {
        store_full_ctx1(texels, pitch, codes, palette, biased);
        return;
    }
#endif
    for (uint32_t i = 0; i < 16; ++i) {
        const uint32_t c = (codes >> (2 * i)) & 3u;
        texels[i * 2 + 0] = palette[c][0];
        texels[i * 2 + 1] = palette[c][1];
    }
}

// DXT3A (endian-swapped): four little-endian 16-bit rows, texel x at bits 4x..4x+3.
static void decode_dxt3a(const uint8_t *block, uint8_t texels[16]) {
    for (uint32_t y = 0; y < 4; ++y) {
        uint16_t row = rd16le(block + y * 2);
        for (uint32_t x = 0; x < 4; ++x) {
            uint32_t v = (row >> (4 * x)) & 0xfu;
            texels[y * 4 + x] = (uint8_t)(v | (v << 4));
        }
    }
}

// DXT3A_AS_1_1_1_1: as DXT3A, with the 4 bits of a texel being X (bit 3), Y (bit 2), Z (bit 1), W (bit 0)
// (xenia texture_load_dxt3aas1111: "alpha in LSB and red in MSB").
static void decode_dxt3a_1111(const uint8_t *block, uint8_t texels[16 * 4]) {
    for (uint32_t y = 0; y < 4; ++y) {
        uint16_t row = rd16le(block + y * 2);
        for (uint32_t x = 0; x < 4; ++x) {
            uint32_t v = (row >> (4 * x)) & 0xfu;
            uint8_t *t = texels + (y * 4 + x) * 4;
            t[0] = (v & 8u) ? 0xff : 0;
            t[1] = (v & 4u) ? 0xff : 0;
            t[2] = (v & 2u) ? 0xff : 0;
            t[3] = (v & 1u) ? 0xff : 0;
        }
    }
}

// BC4 (DXT5A, one half of DXN; PC layout once endian-swapped): two 8-bit endpoints, then 3-bit codes, texel i at
// bit 16 + 3i. e0 > e1: eight levels, else six and 0 / 255. Interpolated levels round to nearest.
static void decode_bc4(const uint8_t *block, uint8_t *texels, uint32_t stride) {
    const uint32_t e0 = block[0], e1 = block[1];
    uint32_t levels[8] = { e0, e1, 0, 0, 0, 0, 0, 255 };
    if (e0 > e1) {
        for (uint32_t k = 1; k < 7; ++k) levels[k + 1] = ((7u - k) * e0 + k * e1 + 3u) / 7u;
    } else {
        for (uint32_t k = 1; k < 5; ++k) levels[k + 1] = ((5u - k) * e0 + k * e1 + 2u) / 5u;
        levels[6] = 0;
    }
    for (uint32_t half = 0; half < 2; ++half) { // eight 3-bit codes in each 24-bit half
        const uint8_t *b = block + 2 + 3 * half;
        const uint32_t codes = (uint32_t)b[0] | ((uint32_t)b[1] << 8) | ((uint32_t)b[2] << 16);
        for (uint32_t i = 0; i < 8; ++i) texels[(half * 8 + i) * stride] = (uint8_t)levels[(codes >> (3u * i)) & 7u];
    }
}

// Xenos GPUSIGN_GAMMA fetch: the texture unit's piecewise-linear degamma (xenia xenos.cc PWLGammaToLinear: codes
// 0-63 slope 1/4, 64-95 slope 1/2, 96-191 slope 1, 192-255 slope 2, in 10-bit units), as 16-bit UNORM.
static uint16_t pwl_unit_to_linear16(float gamma) {
    float scale, offset;
    if (gamma >= 96.0f / 255.0f) {
        if (gamma >= 192.0f / 255.0f) { scale = 8.0f / 1024.0f; offset = -1024.0f; }
        else                          { scale = 4.0f / 1024.0f; offset = -256.0f; }
    } else {
        if (gamma >= 64.0f / 255.0f)  { scale = 2.0f / 1024.0f; offset = -64.0f; }
        else                          { scale = 1.0f / 1024.0f; offset = 0.0f; }
    }
    float linear = gamma * ((255.0f * 1024.0f) * scale) + offset;
    linear += (float)(int32_t)(linear * scale); // trunc (linear >= 0)
    linear *= 1.0f / 1023.0f;
    return (uint16_t)(linear * 65535.0f + 0.5f);
}

static uint16_t pwl_gamma_to_linear16(uint32_t code) { return pwl_unit_to_linear16((float)code * (1.0f / 255.0f)); }

// Channel fields (shift, bits) of the packed 16-bit formats, X in the low bits (as k_transcode_1555 / 4444 read them);
// 5_6_5 has no W.
static void packed16_fields(uint32_t texture_format, uint32_t shift[4], uint32_t bits[4]) {
    static const uint32_t k_1555[2][4] = { { 0, 5, 10, 15 }, { 5, 5, 5, 1 } };
    static const uint32_t k_565[2][4] = { { 0, 5, 11, 0 }, { 5, 6, 5, 0 } };
    static const uint32_t k_4444[2][4] = { { 0, 4, 8, 12 }, { 4, 4, 4, 4 } };
    const uint32_t (*f)[4] = texture_format == 3 ? k_1555 : texture_format == 4 ? k_565 : k_4444;
    for (uint32_t c = 0; c < 4; ++c) { shift[c] = f[0][c]; bits[c] = f[1][c]; }
}

// Decodes one source block of an 8-bit-channel format into 8-bit texels (channels bytes each, row-major 4x4 for
// block formats). Returns the block's side in texels: 1 or 4.
static uint32_t decode_8bit_texels(uint32_t transcode, const uint8_t *block, uint32_t channels, uint8_t *texels) {
    switch (transcode) {
    case k_transcode_dxt3a: decode_dxt3a(block, texels); return 4;
    case k_transcode_dxt3a_1111: decode_dxt3a_1111(block, texels); return 4;
    case k_transcode_ctx1: decode_ctx1(block, texels); return 4;
    case k_transcode_bc4: decode_bc4(block, texels, 1); return 4;
    case k_transcode_bc5: decode_bc4(block, texels, 2); decode_bc4(block + 8, texels + 1, 2); return 4;
    default: memcpy(texels, block, channels); return 1;
    }
}

// Writes 8-bit texels as 16-bit UNORM, the channels in `gamma` through the PWL table, the others exactly (v * 257).
static void store_gamma_texels(uint8_t *dst, uint32_t row_pitch, uint32_t x0, uint32_t y0, uint32_t width,
                               uint32_t height, const uint8_t *texels, uint32_t side, uint32_t channels,
                               uint32_t gamma, const uint16_t *pwl) {
    for (uint32_t y = 0; y < side && y0 + y < height; ++y)
        for (uint32_t x = 0; x < side && x0 + x < width; ++x) {
            const uint8_t *t = texels + (y * side + x) * channels;
            uint8_t *o = dst + (y0 + y) * row_pitch + (x0 + x) * channels * 2u;
            for (uint32_t ch = 0; ch < channels; ++ch)
                wr16le(o + ch * 2u, (gamma >> ch) & 1u ? pwl[t[ch]] : (uint16_t)(t[ch] * 257u));
        }
}

static inline uint16_t float_to_half_unit(uint32_t numerator, uint32_t denominator) {
    // exact for 0 and 1, nearest half for 1/3 and 2/3 (2-bit alpha)
    if (numerator == 0) return 0;
    if (numerator >= denominator) return 0x3c00;
    float f = (float)numerator / (float)denominator;
    uint32_t bits;
    memcpy(&bits, &f, 4);
    int32_t e = (int32_t)((bits >> 23) & 0xffu) - 127 + 15;
    uint32_t m = bits & 0x7fffffu;
    uint32_t half = ((uint32_t)e << 10) | (m >> 13);
    if ((m & 0x1fffu) > 0x1000u || ((m & 0x1fffu) == 0x1000u && (half & 1u))) ++half;
    return (uint16_t)half;
}

} // namespace

extern "C" uint16_t xenos_float7e3_to_half(uint32_t bits10) {
    // 7e3: 3-bit exponent (bias 3), 7-bit mantissa, no infinities; max 31.875 (DirectXTex XMFLOAT3PK-style 7e3)
    const uint32_t e = (bits10 >> 7) & 7u, m = bits10 & 0x7fu;
    if (e != 0) return (uint16_t)(((e + 12u) << 10) | (m << 3));
    if (m == 0) return 0;
    uint32_t p = log2_floor(m);                     // value = m * 2^-9 = 2^(p - 9) * (1.f)
    return (uint16_t)(((p + 6u) << 10) | ((m << (10u - p)) & 0x3ffu));
}

extern "C" int xenos_texture_get_pc_info(const s_xenos_texture_desc *desc, s_pc_texture_info *info) {
    memset(info, 0, sizeof(*info));
    s_xenos_texture_layout layout;
    s_conversion conv;
    if (!xenos_texture_get_layout(desc, &layout) || !choose_conversion(desc->d3d_format, &conv)) return 0;

    info->pc_format = conv.pc_format;
    info->block_width = conv.out_block_width;
    info->block_height = conv.out_block_height;
    info->bytes_per_block = conv.out_bytes;
    info->gamma_channels = conv.gamma_channels;
    info->gamma_mask = conv.linear;
    // A fetch of a format with fewer than four components returns its last component in the missing ones (one
    // component: RRRR, two: RGGG, three: RGBB; xenia's host format swizzles), and the fetch constant's swizzle
    // selects from those four. Vulkan fills them with 0 / 1 instead, so the swizzle names the replicated component:
    // a DXT5A detail map whose constant asks for XYZW reads (r, r, r, r), not (r, 0, 0, 1).
    const uint32_t last = native_component_count(xenos_d3dformat_texture_format(desc->d3d_format)) - 1u;
    for (uint32_t c = 0; c < 4; ++c) {
        uint32_t s = xenos_d3dformat_swizzle(desc->d3d_format, c);
        s = s <= k_xenos_swizzle_1 ? s : c;   // GPUSWIZZLE_KEEP (7): identity
        if (s <= k_xenos_swizzle_w && s > last)
            s = last;
        info->swizzle[c] = (uint8_t)s;
        info->signs[c] = (uint8_t)xenos_d3dformat_sign(desc->d3d_format, c);
    }
    // A8R8G8B8-style swizzles of 8_8_8_8 are B8G8R8A8 storage: report that with an identity mapping
    if (conv.pc_format == k_pc_format_r8g8b8a8_unorm && info->swizzle[0] == k_xenos_swizzle_z &&
        info->swizzle[1] == k_xenos_swizzle_y && info->swizzle[2] == k_xenos_swizzle_x &&
        (info->swizzle[3] == k_xenos_swizzle_w || info->swizzle[3] == k_xenos_swizzle_1))
    {
        info->pc_format = k_pc_format_b8g8r8a8_unorm;
        info->swizzle[0] = k_xenos_swizzle_x;
        info->swizzle[1] = k_xenos_swizzle_y;
        info->swizzle[2] = k_xenos_swizzle_z;
    }

    info->levels = desc->levels;
    info->layers = layout.layers;
    const uint32_t depth = volume_depth(desc);
    uint32_t offset = 0;
    for (uint32_t level = 0; level < desc->levels; ++level) {
        s_pc_texture_level &l = info->level[level];
        l.width = level_dim(desc->width, level);
        l.height = level_dim(desc->height, level);
        l.depth = level_dim(depth, level);
        const uint32_t wb = align_up(l.width, conv.out_block_width) / conv.out_block_width;
        const uint32_t hb = align_up(l.height, conv.out_block_height) / conv.out_block_height;
        l.offset = offset;
        l.row_pitch = wb * conv.out_bytes;
        l.slice_pitch = l.row_pitch * hb;
        l.layer_pitch = l.slice_pitch * l.depth;
        offset += l.layer_pitch * layout.layers;
    }
    info->total_size = offset;
    return 1;
}

extern "C" int xenos_texture_convert(
    const s_xenos_texture_desc *desc,
    const void *base, size_t base_size,
    const void *mip, size_t mip_size,
    void *out, size_t out_size,
    s_pc_texture_info *out_info)
{
    s_xenos_texture_layout layout;
    s_conversion conv;
    s_pc_texture_info info;
    if (!xenos_texture_get_layout(desc, &layout) || !choose_conversion(desc->d3d_format, &conv) ||
        !xenos_texture_get_pc_info(desc, &info))
    {
        return 0;
    }
    const uint32_t first_level = base ? 0u : 1u;
    if (first_level >= desc->levels) return 0;

    // output omits level 0 when the base is missing: shift the level table
    const uint32_t skipped = info.level[first_level].offset;
    if (first_level) {
        for (uint32_t level = 0; level + 1 < desc->levels; ++level) {
            info.level[level] = info.level[level + 1];
            info.level[level].offset -= skipped;
        }
        memset(&info.level[desc->levels - 1], 0, sizeof(info.level[0]));
        info.levels = desc->levels - 1;
    }
    info.total_size -= skipped;
    if (out_size < info.total_size) return 0;

    const uint8_t *src[2] = { (const uint8_t *)base, (const uint8_t *)mip };
    const size_t src_size[2] = { base ? base_size : 0, mip ? mip_size : 0 };
    const uint32_t endian = xenos_d3dformat_endian(desc->d3d_format);
    const uint32_t bpb = layout.bytes_per_block;
    const uint32_t depth = volume_depth(desc);
    uint8_t *dst = (uint8_t *)out;
    uint16_t pwl[256];
    if (conv.gamma)
        for (uint32_t code = 0; code < 256; ++code) pwl[code] = pwl_gamma_to_linear16(code);
    // k_transcode_packed_gamma: each channel's n-bit values as 16-bit UNORM (an absent W reads 1)
    uint32_t packed_shift[4] = { 0, 0, 0, 0 }, packed_bits[4] = { 0, 0, 0, 0 };
    uint16_t packed_lut[4][64];
    if (conv.transcode == k_transcode_packed_gamma) {
        packed16_fields(xenos_d3dformat_texture_format(desc->d3d_format), packed_shift, packed_bits);
        for (uint32_t ch = 0; ch < 4; ++ch) {
            const uint32_t max = (1u << packed_bits[ch]) - 1u;
            for (uint32_t v = 0; v <= max; ++v)
                packed_lut[ch][v] = max == 0 ? 0xffffu
                                  : (conv.gamma_channels >> ch) & 1u ? pwl_unit_to_linear16((float)v / (float)max)
                                                                     : (uint16_t)((v * 65535u + max / 2u) / max);
        }
    }

    for (uint32_t level = first_level; level < desc->levels; ++level) {
        const s_xenos_level_layout &sl = layout.levels[level];
        const s_pc_texture_level &dl = info.level[level - first_level];
        const uint8_t *s = src[sl.from_mip_address];
        const size_t ssize = src_size[sl.from_mip_address];
        if (!s) return 0;
        const uint32_t w = level_dim(desc->width, level), h = level_dim(desc->height, level), d = level_dim(depth, level);
        const uint32_t wb = align_up(w, layout.block_width) / layout.block_width;
        const uint32_t hb = align_up(h, layout.block_height) / layout.block_height;

        for (uint32_t layer = 0; layer < layout.layers; ++layer) {
            const s_level_addressing addressing = level_addressing(desc, layout, level, layer);
            for (uint32_t z = 0; z < d; ++z) {
                uint8_t *slice = dst + dl.offset + layer * dl.layer_pitch + z * dl.slice_pitch;
#ifdef HALO3_64BIT
                // Linear 4_4_4_4 (the menu's UI textures): rotate each 16-bit texel to B4G4R4A4,
                // folding an 8-in-16 byte swap into the same rotation. Process contiguous rows.
                if (addressing.mode == 0 && conv.transcode == k_transcode_4444 && !conv.gamma &&
                    (endian == 0 || endian == k_xenos_endian_8in16)) {
                    const uint32_t rotate = endian == 0 ? 4u : 12u;
                    for (uint32_t by = 0; by < hb; ++by) {
                        const uint32_t so = addressing.offset(0, by, z);
                        if ((uint64_t)so + (uint64_t)wb * 2u > ssize) return 0;
                        const uint8_t *source_row = s + so;
                        uint8_t *dest_row = slice + by * dl.row_pitch;
                        for (uint32_t bx = 0; bx < wb; ++bx) {
                            const uint32_t texel = rd16le(source_row + bx * 2u);
                            wr16le(dest_row + bx * 2u, (uint16_t)((texel << rotate) | (texel >> (16u - rotate))));
                        }
                    }
                    continue;
                }
#if defined(__SSE2__)
                if (addressing.mode == 1 && !sl.packed && wb >= 8 && !conv.gamma && conv.out_bytes == bpb) {
                    switch (conv.transcode) {
                    case k_transcode_copy: {
                        const bool copied =
                            bpb == 16 ? copy_tiled_2d<16>(slice, dl.row_pitch, addressing, wb, hb, s, ssize, endian)
                            : bpb == 8 ? copy_tiled_2d<8>(slice, dl.row_pitch, addressing, wb, hb, s, ssize, endian)
                            : bpb == 4 ? copy_tiled_2d<4>(slice, dl.row_pitch, addressing, wb, hb, s, ssize, endian)
                            : bpb == 2 ? copy_tiled_2d<2>(slice, dl.row_pitch, addressing, wb, hb, s, ssize, endian)
                                       : copy_tiled_2d<1>(slice, dl.row_pitch, addressing, wb, hb, s, ssize, endian);
                        if (!copied) return 0;
                        continue;
                    }
                    case k_transcode_bc5_bias:
                        if (!copy_tiled_2d<16, k_transcode_bc5_bias>(slice, dl.row_pitch, addressing, wb, hb, s, ssize, endian)) return 0;
                        continue;
                    case k_transcode_1555:
                        if (!copy_tiled_2d<2, k_transcode_1555>(slice, dl.row_pitch, addressing, wb, hb, s, ssize, endian)) return 0;
                        continue;
                    case k_transcode_4444:
                        if (!copy_tiled_2d<2, k_transcode_4444>(slice, dl.row_pitch, addressing, wb, hb, s, ssize, endian)) return 0;
                        continue;
                    default:
                        break;
                    }
                }
#endif
#endif
                // A linear texture whose blocks are texels copied unchanged (no endian swap, transcode or degamma: L8,
                // the Bink planes the CPU rewrites every movie frame): each row is one copy, the same bytes the
                // block loop below writes.
                if (!xenos_d3dformat_tiled(desc->d3d_format) && conv.transcode == k_transcode_copy && !conv.gamma &&
                    endian == 0 && layout.block_width == 1 && layout.block_height == 1 && conv.out_bytes == bpb) {
                    for (uint32_t by = 0; by < hb; ++by) {
                        const uint32_t so = addressing.offset(0, by, z);
                        if ((uint64_t)so + (uint64_t)wb * bpb > ssize) return 0;
                        memcpy(slice + by * dl.row_pitch, s + so, (size_t)wb * bpb);
                    }
                    continue;
                }
                // blocks copied as they are but for the byte order (BCn, plain texels): the specialized loop
                if (conv.transcode == k_transcode_copy && !conv.gamma && conv.out_bytes == bpb &&
                    (bpb == 1 || bpb == 2 || bpb == 4 || bpb == 8 || bpb == 16)) {
                    const bool copied =
                        bpb == 16 ? copy_blocks<16>(slice, dl.row_pitch, addressing, wb, hb, z, s, ssize, endian)
                        : bpb == 8 ? copy_blocks<8>(slice, dl.row_pitch, addressing, wb, hb, z, s, ssize, endian)
                        : bpb == 4 ? copy_blocks<4>(slice, dl.row_pitch, addressing, wb, hb, z, s, ssize, endian)
                        : bpb == 2 ? copy_blocks<2>(slice, dl.row_pitch, addressing, wb, hb, z, s, ssize, endian)
                                   : copy_blocks<1>(slice, dl.row_pitch, addressing, wb, hb, z, s, ssize, endian);
                    if (!copied) return 0;
                    continue;
                }
                for (uint32_t by = 0; by < hb; ++by) {
                    for (uint32_t bx = 0; bx < wb; ++bx) {
                        const uint32_t so = addressing.offset(bx, by, z);
                        if ((uint64_t)so + bpb > ssize) return 0;
                        uint8_t block[16];
                        memcpy(block, s + so, bpb);
                        swap_block(block, bpb, endian);
                        if (conv.gamma) {
                            uint8_t texels[16 * 4];
                            const uint32_t side = decode_8bit_texels(conv.transcode, block, conv.channels, texels);
                            store_gamma_texels(slice, dl.row_pitch, bx * side, by * side, w, h, texels, side,
                                               conv.channels, conv.gamma, pwl);
                            continue;
                        }
#if defined(HALO3_64BIT) && defined(__SSE2__)
                        if ((conv.transcode == k_transcode_ctx1 || conv.transcode == k_transcode_ctx1_bias) &&
                            addressing.mode == 1 && !sl.packed && wb >= 8 && bx < w / 4 && by < h / 4) {
                            decode_ctx1<true>(block, slice + by * 4 * dl.row_pitch + bx * 8, dl.row_pitch,
                                              conv.transcode == k_transcode_ctx1_bias);
                            continue;
                        }
#endif
                        switch (conv.transcode) {
                        case k_transcode_copy:
                            memcpy(slice + by * dl.row_pitch + bx * conv.out_bytes, block, conv.out_bytes);
                            break;
                        case k_transcode_1555: {
                            uint32_t v = rd16le(block);
                            uint32_t x = v & 0x1fu, y = (v >> 5) & 0x1fu, zc = (v >> 10) & 0x1fu, a = v >> 15;
                            wr16le(slice + by * dl.row_pitch + bx * 2, (uint16_t)((a << 15) | (x << 10) | (y << 5) | zc));
                            break;
                        }
                        case k_transcode_4444: {
                            uint32_t v = rd16le(block);
                            uint32_t x = v & 0xfu, y = (v >> 4) & 0xfu, zc = (v >> 8) & 0xfu, a = v >> 12;
                            wr16le(slice + by * dl.row_pitch + bx * 2, (uint16_t)((zc << 12) | (y << 8) | (x << 4) | a));
                            break;
                        }
                        case k_transcode_7e3: {
                            uint32_t v = rd32le(block);
                            uint8_t *t = slice + by * dl.row_pitch + bx * 8;
                            wr16le(t + 0, xenos_float7e3_to_half(v & 0x3ffu));
                            wr16le(t + 2, xenos_float7e3_to_half((v >> 10) & 0x3ffu));
                            wr16le(t + 4, xenos_float7e3_to_half((v >> 20) & 0x3ffu));
                            wr16le(t + 6, float_to_half_unit(v >> 30, 3u));
                            break;
                        }
                        case k_transcode_dxt3a: {
                            uint8_t texels[16];
                            decode_dxt3a(block, texels);
                            store_texel_block<1>(slice, dl.row_pitch, bx * 4, by * 4, w, h, texels);
                            break;
                        }
                        case k_transcode_dxt3a_1111: {
                            uint8_t texels[64];
                            decode_dxt3a_1111(block, texels);
                            store_texel_block<4>(slice, dl.row_pitch, bx * 4, by * 4, w, h, texels);
                            break;
                        }
                        case k_transcode_ctx1: {
                            uint8_t texels[32];
                            decode_ctx1(block, texels);
                            store_texel_block<2>(slice, dl.row_pitch, bx * 4, by * 4, w, h, texels);
                            break;
                        }
                        case k_transcode_ctx1_bias: {
                            uint8_t texels[32];
                            decode_ctx1(block, texels);
                            for (uint32_t i = 0; i < 32; ++i) texels[i] ^= 0x80u;
                            store_texel_block<2>(slice, dl.row_pitch, bx * 4, by * 4, w, h, texels);
                            break;
                        }
                        case k_transcode_bc5_bias:
                            block[0] ^= 0x80u; // the red and green endpoints: BC4 index bits are unsigned in both
                            block[1] ^= 0x80u;
                            block[8] ^= 0x80u;
                            block[9] ^= 0x80u;
                            memcpy(slice + by * dl.row_pitch + bx * conv.out_bytes, block, conv.out_bytes);
                            break;
                        case k_transcode_packed_gamma: {
                            const uint32_t v = rd16le(block);
                            uint8_t *t = slice + by * dl.row_pitch + bx * 8;
                            for (uint32_t ch = 0; ch < 4; ++ch)
                                wr16le(t + ch * 2u, packed_lut[ch][(v >> packed_shift[ch]) & ((1u << packed_bits[ch]) - 1u)]);
                            break;
                        }
                        }
                    }
                }
            }
        }
    }
    if (out_info) *out_info = info;
    return 1;
}

extern "C" int halo3_bitmap_texture_sources(
    const s_xenos_texture_desc *desc, int split,
    const void *primary, size_t primary_size,
    const void *secondary, size_t secondary_size,
    s_xenos_texture_sources *out)
{
    memset(out, 0, sizeof(*out));
    s_xenos_texture_layout layout;
    uint32_t base_extent = 0, mip_extent = 0;
    if (!xenos_texture_get_layout(desc, &layout) || !xenos_texture_get_source_extent(desc, &base_extent, &mip_extent)) return 0;
    const uint8_t *p = (const uint8_t *)primary;
    if (split) {
        if (secondary) {
            if (secondary_size < base_extent) return 0;
            out->base = secondary;
            out->base_size = secondary_size;
        }
        if (desc->levels > 1) {
            if (!p || primary_size < mip_extent) return 0;
            out->mip = p;
            out->mip_size = primary_size;
        }
    } else {
        if (!p || primary_size < base_extent) return 0;
        out->base = p;
        out->base_size = primary_size;
        if (desc->levels > 1) {
            const uint32_t mip_offset = layout.contiguous_mip_offset;
            if (primary_size < (size_t)mip_offset + mip_extent) return 0;
            out->mip = p + mip_offset;
            out->mip_size = primary_size - mip_offset;
        }
    }
    return 1;
}

namespace {

// D3D::GetMaxMipLevels(width, height, 1, no border)
static uint32_t max_mip_levels(uint32_t w, uint32_t h) { return log2_floor(max_u32(max_u32(w, h), 1u)) + 1u; }

// D3D::SetBaseTextureHeader sizes with BaseOffset = MipOffset = 0. A mipmapped texture whose level 0 already is the
// packed tail has no separate base: the whole chain is one tail at the base address.
static void single_header_sizes(const s_xenos_texture_desc &t, uint32_t width, uint32_t height, uint32_t levels,
                                uint32_t *base, uint32_t *mip, s_xenos_texture_layout *layout) {
    s_xenos_texture_desc d = t;
    d.width = width;
    d.height = height;
    d.levels = levels;
    d.packed_mips = levels > 1;
    s_xenos_texture_layout l;
    xenos_texture_get_layout(&d, &l);
    if (l.packed_level == 0 && levels > 1) { *base = 0; *mip = l.mip_size; }
    else { *base = l.base_size; *mip = levels > 1 ? l.mip_size : 0; }
    if (layout) *layout = l;
}

// SetBaseTextureHeaderPair (retail 0x825F1880): where XGSet{,Cube,Array}TextureHeaderPair puts two textures that share
// one allocation. Addresses are relative to the XG arguments: base_offset is added to everything in the base
// allocation; the mips are at mip_offset (Halo 3 passes 0x80000000 for a separate mip allocation: bit 31 marks it and is
// kept here) or, for XGHEADER_CONTIGUOUS_MIP_OFFSET (0xffffffff), right after the outer texture's base. Retail Halo 3
// always passes base_offset 0 and only formats shared by both textures.
static void set_base_texture_header_pair(uint32_t base_offset, uint32_t mip_offset, const s_xenos_texture_desc t[2],
                                         s_xenos_texture_pair_placement out[2], uint32_t *out_base_size,
                                         uint32_t *out_mip_size) {
    uint32_t lw[2], lh[2], max_lv[2], lv[2], depth[2], base[2], mip[2];
    s_xenos_texture_layout layout[2];
    for (int i = 0; i < 2; ++i) {
        lw[i] = log2_ceil(t[i].width);
        lh[i] = log2_ceil(t[i].height);
        max_lv[i] = max_mip_levels(t[i].width, t[i].height);
        lv[i] = t[i].levels;
        depth[i] = layer_count(&t[i]);
    }
    // one 32x32-block tile of texture 1's format
    const uint32_t tile = format_info(xenos_d3dformat_texture_format(t[0].d3d_format)).bytes_per_block << 10;
    const uint32_t half = tile >> 1;
    // the outer ("big") texture is the wider one; ties go to the deeper one, then to texture 1
    const int b = (lw[0] > lw[1] || (lw[0] == lw[1] && depth[0] >= depth[1])) ? 0 : 1, s = 1 - b;
    const uint32_t diff = lw[b] - lw[s];
    // the big chain is extended to reach as deep as the small chain does inside it
    uint32_t build_levels[2] = { lv[0], lv[1] };
    build_levels[b] = max_u32(lv[b] > diff ? lv[b] - diff : 0u, lv[s]) + diff;
    for (int i = 0; i < 2; ++i) {
        if (build_levels[i] > max_lv[i]) {
            // non-power-of-two: mip size of the power-of-two padded texture, header capped at the real chain
            uint32_t padded_base, unused;
            single_header_sizes(t[i], 1u << lw[i], 1u << lh[i], build_levels[i], &padded_base, &mip[i], NULL);
            build_levels[i] = max_lv[i];
            single_header_sizes(t[i], t[i].width, t[i].height, build_levels[i], &base[i], &unused, &layout[i]);
        } else {
            single_header_sizes(t[i], t[i].width, t[i].height, build_levels[i], &base[i], &mip[i], &layout[i]);
        }
    }
    const bool contiguous = mip_offset == 0xffffffffu;
    const bool small_narrow = (1u << lw[s]) <= 16, big_narrow = (1u << lw[b]) <= 16;
    uint32_t base_size = base[b], big_mip, small_base, small_mip;
    if (lw[b] >= 7) {
        // XGGetMipLevelOffset(big, 0, lw - 6): the big level that is 64 texels wide hosts the small chain
        const uint32_t off = layout[b].levels[lw[b] - 6].offset;
        if (lw[s] == 7) {
            // a 128-wide small level 0 gets its own storage after the big base
            base_size = align_up(base[b], k_subresource_alignment) + base[s];
            if (contiguous) mip_offset = base_offset + base_size;
            big_mip = lv[b] > 1 ? mip_offset : 0;
            small_base = base_offset + align_up(base[b], k_subresource_alignment);
            small_mip = lv[s] > 1 ? mip_offset + off + half : 0;
        } else {
            // the small level 0 sits in the right half of the big 64-wide level
            if (contiguous) mip_offset = base_offset + base[b];
            big_mip = lv[b] > 1 ? mip_offset : 0;
            small_base = mip_offset + off + half;
            small_mip = (!small_narrow && lv[s] != 1) ? small_base + tile * depth[b] : 0;
        }
    } else {
        // k big mip levels (one tile per face each) come before the one hosting small level 0
        const uint32_t k = min_u32(diff, lw[b] >= 4 ? lw[b] - 4 : 0u);
        if (contiguous) mip_offset = base_offset + base[b];
        big_mip = (!big_narrow && lv[b] != 1) ? mip_offset : 0;
        small_base = k == 0 ? base_offset + half : mip_offset + (k - 1) * tile * depth[b] + half;
        small_mip = (!small_narrow && lv[s] != 1) ? mip_offset + k * tile * depth[b] + half : 0;
    }
    out[b].base_address = base_offset;
    out[b].mip_address = big_mip;
    out[b].header_levels = build_levels[b];
    out[b].outer = 1;
    out[s].base_address = small_base;
    out[s].mip_address = small_mip;
    out[s].header_levels = build_levels[s];
    out[s].outer = 0;
    // the small chain lives inside the big chain's (extended) levels, so the big texture's sizes are the pair's
    if (out_base_size) *out_base_size = base_size;
    if (out_mip_size) *out_mip_size = max_u32(lv[0], lv[1]) > 1 ? mip[b] : 0;
    // (The XDK falls back to two separate textures when interleaving would not save memory; no retail pair does.)
}

} // namespace

extern "C" int PORT_MS_ABI xenos_texture_pair_placement(
    uint32_t base_offset, uint32_t mip_offset, const s_xenos_texture_desc desc[2],
    s_xenos_texture_pair_placement out[2], uint32_t *out_base_size, uint32_t *out_mip_size)
{
    memset(out, 0, 2 * sizeof(*out));
    if (desc[0].dimension != desc[1].dimension || desc[0].dimension == k_xenos_dimension_3d) return 0;
    for (int i = 0; i < 2; ++i) {
        s_xenos_texture_layout layout;
        if (!desc[i].width || !desc[i].height || !desc[i].levels || !xenos_texture_get_layout(&desc[i], &layout)) return 0;
    }
    set_base_texture_header_pair(base_offset, mip_offset, desc, out, out_base_size, out_mip_size);
    return 1;
}

extern "C" int halo3_bitmap_interleaved_sources(
    const s_xenos_texture_desc in_desc[2], const int split[2],
    const void *primary, size_t primary_size,
    const void *secondary, size_t secondary_size,
    s_xenos_texture_desc out_desc[2],
    s_xenos_texture_sources out[2])
{
    memset(out, 0, 2 * sizeof(*out));
    // initialize_xenon_texture_interleaved_header_internal: separate mip allocation when tiled
    const bool tiled = (in_desc[0].width > 64 || in_desc[1].width > 64) && (split[0] || split[1]);
    s_xenos_texture_pair_placement headers[2];
    if (!xenos_texture_pair_placement(0, tiled ? 0x80000000u : 0xffffffffu, in_desc, headers, NULL, NULL)) return 0;

    const uint8_t *p = (const uint8_t *)primary, *sec = (const uint8_t *)secondary;
    for (int i = 0; i < 2; ++i) {
        out_desc[i] = in_desc[i];
        out_desc[i].packed_mips = headers[i].header_levels > 1;
        // tag_d3d_texture_interleaved_fixup: a split texture's base is in the high-resolution (secondary) data and its
        // mips in the primary data; anything else is in the primary data. Bit 31 marks the separate mip allocation.
        const uint32_t base_address = headers[i].base_address & 0x7fffffffu;
        if (split[i]) {
            if (sec) {
                if (base_address > secondary_size) return 0;
                out[i].base = sec + base_address;
                out[i].base_size = secondary_size - base_address;
            }
        } else {
            if (!p || base_address > primary_size) return 0;
            out[i].base = p + base_address;
            out[i].base_size = primary_size - base_address;
        }
        if (in_desc[i].levels > 1) {
            if (headers[i].mip_address) {
                const uint32_t mip_address = headers[i].mip_address & 0x7fffffffu;
                if (!p || mip_address > primary_size) return 0;
                out[i].mip = p + mip_address;
                out[i].mip_size = primary_size - mip_address;
            } else {
                out[i].mip = out[i].base;           // narrow texture: the mips are in the tail at its base
                out[i].mip_size = out[i].base_size;
            }
        }
    }
    return 1;
}
