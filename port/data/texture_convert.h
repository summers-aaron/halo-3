#ifndef PORT_DATA_TEXTURE_CONVERT_H
#define PORT_DATA_TEXTURE_CONVERT_H

/*
 * Load-time conversion of Xbox 360 (Xenos) texture data to linear little-endian PC/Vulkan layouts.
 *
 * Input is one texture as Halo 3 stores it in a bitmap resource: an Xbox 360 D3DFORMAT, its dimensions, and the
 * texture memory under its base address (level 0) and mip address (levels 1..). Output is every level and every
 * face/slice in linear row-major order, D3D11/Vulkan subresource order (level-major, then array layer, then depth
 * slice), in the PC format reported in s_pc_texture_info.
 *
 * Channel convention: the output stores the Xenos texture channels X, Y, Z, W as the PC format's R, G, B, A. The
 * D3DFORMAT's swizzle (which shader component reads which channel, e.g. A8R8G8B8 or AY8) is returned separately in
 * s_pc_texture_info::swizzle so the renderer applies it as an image-view component mapping, exactly as the Xenos
 * texture fetch does. Nothing is lost or re-quantized except where noted on the format.
 *
 * Sources: Xenos addressing and packed mip tails follow xenia (src/xenia/gpu/texture_util.cc, texture_address.h,
 * read-only reference); D3DFORMAT bit layout follows the XDK's d3d9types.h (D3DFORMAT_*_SHIFT).
 */

#include <stddef.h>
#if defined(__has_include)
#if __has_include(<stdint.h>)
#define PORT_DATA_HAVE_STDINT 1
#endif
#endif
#if defined(PORT_DATA_HAVE_STDINT)
#include <stdint.h>
#else
/* Compiled against the XDK headers (port/sdk: no <stdint.h>): the compiler's own exact-width types. */
typedef __UINT8_TYPE__ uint8_t;
typedef __UINT16_TYPE__ uint16_t;
typedef __UINT32_TYPE__ uint32_t;
#endif

#ifndef PORT_MS_ABI
#include "../sdk/runtime/port_abi.h"
#endif

#ifdef __cplusplus
extern "C" {
#endif

/* GPUTEXTUREFORMAT (D3DFORMAT bits 0..5). */
enum e_xenos_texture_format {
    k_xenos_format_8                    = 2,
    k_xenos_format_1_5_5_5              = 3,
    k_xenos_format_5_6_5                = 4,
    k_xenos_format_8_8_8_8              = 6,
    k_xenos_format_2_10_10_10           = 7,
    k_xenos_format_8_8                  = 10,
    k_xenos_format_16_16_edram          = 13, /* render targets only: signed fixed point, [-32, 32) (G16R16_EDRAM) */
    k_xenos_format_4_4_4_4              = 15,
    k_xenos_format_dxt1                 = 18,
    k_xenos_format_dxt2_3               = 19,
    k_xenos_format_dxt4_5               = 20,
    k_xenos_format_16_16_16_16_edram    = 21, /* render targets only: as 16_16_edram (A16B16G16R16_EDRAM) */
    k_xenos_format_16                   = 24,
    k_xenos_format_16_16                = 25,
    k_xenos_format_16_16_16_16          = 26,
    k_xenos_format_16_float             = 30,
    k_xenos_format_16_16_float          = 31,
    k_xenos_format_16_16_16_16_float    = 32,
    k_xenos_format_32_float             = 36,
    k_xenos_format_32_32_float          = 37,
    k_xenos_format_32_32_32_32_float    = 38,
    k_xenos_format_dxn                  = 49,
    k_xenos_format_dxt3a                = 58,
    k_xenos_format_dxt5a                = 59,
    k_xenos_format_ctx1                 = 60,
    k_xenos_format_dxt3a_as_1_1_1_1     = 61,
    k_xenos_format_2_10_10_10_float     = 63, /* 7e3 floats (2_10_10_10_FLOAT_EDRAM) */
};

/* GPUENDIAN (D3DFORMAT bits 6..7): byte order of the data in memory. */
enum e_xenos_endian {
    k_xenos_endian_none   = 0,
    k_xenos_endian_8in16  = 1,
    k_xenos_endian_8in32  = 2,
    k_xenos_endian_16in32 = 3,
};

/* s_render_texture_format::type (bitmap_group_xenon_interop.cpp). */
enum e_xenos_texture_dimension {
    k_xenos_dimension_2d    = 0,
    k_xenos_dimension_3d    = 1,
    k_xenos_dimension_cube  = 2,
    k_xenos_dimension_array = 3,
};

/* Output formats; the values are VkFormat's. */
enum e_pc_texture_format {
    k_pc_format_undefined                = 0,
    k_pc_format_b4g4r4a4_unorm_pack16    = 3,
    k_pc_format_b5g6r5_unorm_pack16      = 5,
    k_pc_format_a1r5g5b5_unorm_pack16    = 8,
    k_pc_format_r8_unorm                 = 9,
    k_pc_format_r8_snorm                 = 10,
    k_pc_format_r8g8_unorm               = 16,
    k_pc_format_r8g8_snorm               = 17,
    k_pc_format_r8g8b8a8_unorm           = 37,
    k_pc_format_r8g8b8a8_snorm           = 38,
    k_pc_format_b8g8r8a8_unorm           = 44,
    k_pc_format_a2b10g10r10_unorm_pack32 = 64,
    k_pc_format_r16_unorm                = 70,
    k_pc_format_r16_snorm                = 71,
    k_pc_format_r16_sfloat               = 76,
    k_pc_format_r16g16_unorm             = 77,
    k_pc_format_r16g16_snorm             = 78,
    k_pc_format_r16g16_sfloat            = 83,
    k_pc_format_r16g16b16a16_unorm       = 91,
    k_pc_format_r16g16b16a16_snorm       = 92,
    k_pc_format_r16g16b16a16_sfloat      = 97,
    k_pc_format_r32_sfloat               = 100,
    k_pc_format_r32g32_sfloat            = 103,
    k_pc_format_r32g32b32a32_sfloat      = 109,
    k_pc_format_bc1_rgba_unorm           = 133,
    k_pc_format_bc1_rgba_srgb            = 134,
    k_pc_format_bc2_unorm                = 135,
    k_pc_format_bc2_srgb                 = 136,
    k_pc_format_bc3_unorm                = 137,
    k_pc_format_bc3_srgb                 = 138,
    k_pc_format_bc4_unorm                = 139,
    k_pc_format_bc5_unorm                = 141,
    k_pc_format_bc5_snorm                = 142,
};

/* Component sources for s_pc_texture_info::swizzle (GPUSWIZZLE). */
enum e_xenos_swizzle {
    k_xenos_swizzle_x = 0, /* output R */
    k_xenos_swizzle_y = 1, /* output G */
    k_xenos_swizzle_z = 2, /* output B */
    k_xenos_swizzle_w = 3, /* output A */
    k_xenos_swizzle_0 = 4,
    k_xenos_swizzle_1 = 5,
};

/* Fields of an Xbox 360 D3DFORMAT. */
static inline uint32_t xenos_d3dformat_texture_format(uint32_t d3d_format) { return d3d_format & 0x3fu; }
static inline uint32_t xenos_d3dformat_endian(uint32_t d3d_format) { return (d3d_format >> 6) & 0x3u; }
static inline uint32_t xenos_d3dformat_tiled(uint32_t d3d_format) { return (d3d_format >> 8) & 0x1u; }
/* GPUSIGN of component c (0..3): 0 unsigned, 1 signed, 2 unsigned biased, 3 gamma */
static inline uint32_t xenos_d3dformat_sign(uint32_t d3d_format, uint32_t c) { return (d3d_format >> (9u + 2u * c)) & 0x3u; }
static inline uint32_t xenos_d3dformat_swizzle(uint32_t d3d_format, uint32_t c) { return (d3d_format >> (18u + 3u * c)) & 0x7u; }

struct s_xenos_texture_desc {
    uint32_t d3d_format;   /* s_render_texture_format::format */
    uint32_t dimension;    /* e_xenos_texture_dimension */
    uint32_t width;
    uint32_t height;
    uint32_t depth;        /* depth of a 3D texture, slice count of an array, 1 otherwise */
    uint32_t levels;       /* mip level count including level 0 */
    uint32_t packed_mips;  /* the fetch constant's PackedMips: levels whose shorter side is <= 16 share one tail */
};

/* Where one level lives in the Xenos source data. */
struct s_xenos_level_layout {
    uint32_t from_mip_address;  /* 0: under the base address (level 0), 1: under the mip address */
    uint32_t offset;            /* byte offset of the level (of its packed tail) from that address */
    uint32_t row_pitch_bytes;   /* bytes between rows of blocks (tile-aligned) */
    uint32_t height_blocks;     /* block rows between depth slices (tile-aligned) */
    uint32_t slice_stride;      /* bytes between array layers / cube faces at this level */
    uint32_t packed_x_blocks;   /* position inside the packed mip tail (0 if not packed) */
    uint32_t packed_y_blocks;
    uint32_t packed_z;
    uint32_t packed;            /* 1 if the level is stored in a packed mip tail */
};

struct s_xenos_texture_layout {
    uint32_t block_width;
    uint32_t block_height;
    uint32_t bytes_per_block;
    uint32_t layers;            /* array layers (6 for cube) */
    uint32_t packed_level;      /* first level of the packed mip tail, or 0xffffffff */
    uint32_t base_size;         /* bytes under the base address, 4 KB aligned (as XGSetTextureHeader reports) */
    uint32_t mip_size;          /* bytes under the mip address, 4 KB aligned */
    uint32_t contiguous_mip_offset; /* mip address - base address when both live in one allocation */
    struct s_xenos_level_layout levels[16];
};

struct s_pc_texture_level {
    uint32_t width;             /* texels */
    uint32_t height;
    uint32_t depth;             /* 3D depth slices at this level, 1 otherwise */
    uint32_t offset;            /* byte offset of layer 0 of this level in the output */
    uint32_t row_pitch;         /* bytes per row of blocks */
    uint32_t slice_pitch;       /* bytes per depth slice */
    uint32_t layer_pitch;       /* bytes per array layer / cube face (all depth slices) */
};

struct s_pc_texture_info {
    uint32_t pc_format;         /* e_pc_texture_format */
    uint32_t block_width;       /* 4 for block-compressed output, 1 otherwise */
    uint32_t block_height;
    uint32_t bytes_per_block;
    uint8_t  swizzle[4];        /* e_xenos_swizzle source of output R, G, B, A */
    uint8_t  signs[4];          /* GPUSIGN of channels X, Y, Z, W (3 = GPUSIGN_GAMMA: the fetch linearizes it) */
    uint32_t gamma_channels;    /* bit c: stored channel c is GPUSIGN_GAMMA */
    uint32_t gamma_mask;        /* bit c: GPUSIGN_GAMMA channel c is linear in the output, exactly (the Xenos PWL
                                   degamma into 16-bit UNORM) or, for BC1-3, through the *_SRGB format */
    uint32_t levels;
    uint32_t layers;
    uint32_t total_size;        /* bytes of output */
    struct s_pc_texture_level level[16];
};

/* Computes where every level lives in Xenos memory. Returns 0 for unsupported dimensions or formats. */
int xenos_texture_get_layout(const struct s_xenos_texture_desc *desc, struct s_xenos_texture_layout *out_layout);

/* Bytes actually read under the base and mip addresses: one past the last byte of any block of any level/layer
 * (tile padding beyond it is never read). Halo 3 stores exactly this much per bitmap. */
int xenos_texture_get_source_extent(const struct s_xenos_texture_desc *desc, uint32_t *out_base_extent, uint32_t *out_mip_extent);

/* Chooses the PC format and output layout. Returns 0 if the Xenos format is not supported. */
int xenos_texture_get_pc_info(const struct s_xenos_texture_desc *desc, struct s_pc_texture_info *out_info);

/*
 * Converts a whole texture. `base` holds level 0 (may be NULL when the high-resolution level is not loaded: the
 * output then starts at level 1 and info->level[0] describes level 1), `mip` holds levels 1.. (may be NULL when
 * levels == 1 or when base_size already contains contiguous mips, see xenos_texture_get_layout). Every read is
 * bounds-checked against the given sizes. Returns 1 on success, 0 on unsupported input or short source data.
 */
int xenos_texture_convert(
    const struct s_xenos_texture_desc *desc,
    const void *base, size_t base_size,
    const void *mip, size_t mip_size,
    void *out, size_t out_size,
    struct s_pc_texture_info *out_info);

/*
 * Halo 3 bitmap resources (s_render_texture_descriptor, bitmap_group_xenon_interop.cpp): where a texture's base and
 * mip data live. `split` is s_render_texture_format::packed: level 0 is in the secondary (optional, high-resolution)
 * data and the mips start at the primary data; otherwise one allocation holds the base with the mips at
 * contiguous_mip_offset. A missing secondary (not loaded) leaves base NULL: convert then starts at level 1, as the game
 * samples from MinMipLevel 1. Returns 0 if the data is shorter than the layout needs.
 */
struct s_xenos_texture_sources {
    const void *base;
    size_t      base_size;
    const void *mip;
    size_t      mip_size;
};
int halo3_bitmap_texture_sources(
    const struct s_xenos_texture_desc *desc, int split,
    const void *primary, size_t primary_size,
    const void *secondary, size_t secondary_size,
    struct s_xenos_texture_sources *out_sources);

/*
 * Interleaved texture pairs: where XGSet{,Cube,Array}TextureHeaderPair (retail SetBaseTextureHeaderPair, 0x825F1880)
 * places two textures that share one allocation. base_offset / mip_offset are the XG call's BaseOffset / MipOffset
 * (mip_offset 0xffffffff = XGHEADER_CONTIGUOUS_MIP_OFFSET; Halo 3 passes 0x80000000 for a separate mip allocation, and
 * bit 31 is kept in the returned mip addresses). desc[i].packed_mips is ignored. Returns 0 for 3D textures or textures
 * of different dimensions (which the XDK cannot interleave).
 */
struct s_xenos_texture_pair_placement {
    uint32_t base_address;   /* byte address of level 0 (base allocation) */
    uint32_t mip_address;    /* byte address of level 1.. (mip allocation, bit 31 kept); 0: the mips are in the
                                packed tail at the base address (a narrow texture) or the texture has one level */
    uint32_t header_levels;  /* levels the XDK builds the header from: PackedMips = header_levels > 1 */
    uint32_t outer;          /* 1 for the texture whose chain hosts the other's (the wider one) */
};
int PORT_MS_ABI xenos_texture_pair_placement(
    uint32_t base_offset, uint32_t mip_offset, const struct s_xenos_texture_desc desc[2],
    struct s_xenos_texture_pair_placement out[2], uint32_t *out_base_size, uint32_t *out_mip_size);

/*
 * Interleaved Halo 3 bitmaps (s_render_texture_interleaved_descriptor): two textures share one allocation, placed by
 * the XDK's XGSet{,Cube,Array}TextureHeaderPair (retail SetBaseTextureHeaderPair, 0x825F1880, reconstructed from the
 * retail code and checked against all 967 interleaved retail bitmaps). Returns each texture's descriptor as its
 * header was built (packed_mips can differ from a lone texture) and where its base and mip data are.
 */
int halo3_bitmap_interleaved_sources(
    const struct s_xenos_texture_desc in_desc[2], const int split[2],
    const void *primary, size_t primary_size,
    const void *secondary, size_t secondary_size,
    struct s_xenos_texture_desc out_desc[2],
    struct s_xenos_texture_sources out_sources[2]);

/* Xenos tiled addressing, in bytes, for a block at (x, y[, z]) of a surface whose pitch (and height) in blocks is
 * aligned to 32. bytes_per_block_log2 is 0..4. Exposed for tests. */
uint32_t xenos_tiled_offset_2d(uint32_t x, uint32_t y, uint32_t pitch_blocks, uint32_t bytes_per_block_log2);
uint32_t xenos_tiled_offset_3d(uint32_t x, uint32_t y, uint32_t z, uint32_t pitch_blocks, uint32_t height_blocks,
                               uint32_t bytes_per_block_log2);
/* XGAddress2DTiledOffset: the XDK's formula, in blocks (elements). */
uint32_t xenos_xg_address_2d_tiled_offset(uint32_t x, uint32_t y, uint32_t width_blocks, uint32_t bytes_per_block);

/* Converts one Xenos 7e3 float (10 bits: 3-bit exponent, 7-bit mantissa) or 2-bit alpha to IEEE half. */
uint16_t xenos_float7e3_to_half(uint32_t bits10);

#ifdef __cplusplus
}
#endif

#endif /* PORT_DATA_TEXTURE_CONVERT_H */
