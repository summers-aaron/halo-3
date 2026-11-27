#ifndef PORT_SDK_D3D_D3D_PORT_TEXTURE_H
#define PORT_SDK_D3D_D3D_PORT_TEXTURE_H

/*
 * The port's texture-header contract between the Xbox D3D/XGraphics layer (port/sdk/d3d/xgraphics.cpp, built against
 * the XDK headers) and the PC renderer (port/sdk/d3d/vulkan_*, host code that never sees the XDK's D3DBaseTexture).
 *
 * Texture headers are the game's own D3DBaseTexture structures (in tag data or the game's globals), built by the
 * XGSet*TextureHeader* calls and pointed at their data by XGOffset*Address, exactly as on the Xbox: the fetch
 * constant's BaseAddress / MipAddress hold host addresses >> 12, so texture data must be 4 KB aligned, as it is on the
 * Xbox. Two things the fetch constant cannot say are kept in D3DResource::Identifier (an application-owned dword the
 * game never sets on textures; it uses it only on shaders and constant buffers):
 *   - that the texture is one of an interleaved pair (XGSet{,Cube,Array}TextureHeaderPair), so the renderer knows the
 *     data is shared with another texture and is untiled with port/data/texture_convert.h's pair-aware layout;
 *   - the sub-4 KB part of a pair member's base and mip addresses (a small texture can start half a 32x32 tile into
 *     the outer texture's level: a multiple of 512 bytes), and whether its mips live in the packed tail at its base.
 * port_d3d_texture_get_source() turns a header into the plain description texture_convert.h takes.
 */

#include "../../data/texture_convert.h"

#ifdef __cplusplus
extern "C" {
#endif

/* D3DResource::Identifier of a texture header (byte offset 0x10 of the header). 0 for an ordinary texture. */
#define PORT_D3D_TEXTURE_ID_KIND_MASK          0xf0000000u
#define PORT_D3D_TEXTURE_ID_PAIR               0x50000000u /* one texture of an interleaved pair */
#define PORT_D3D_TEXTURE_ID_PAIR_SECOND        0x00000001u /* the second texture of the XG pair call */
#define PORT_D3D_TEXTURE_ID_PAIR_OUTER         0x00000002u /* its chain hosts the other texture's chain */
#define PORT_D3D_TEXTURE_ID_MIPS_AT_BASE       0x00000004u /* levels 1.. are in the packed tail at the base address */
#define PORT_D3D_TEXTURE_ID_MIP_ALLOCATION     0x00000008u /* the mips are in a separate allocation (MipOffset bit 31) */
#define PORT_D3D_TEXTURE_ID_BASE_SUBPAGE_SHIFT 4           /* 3 bits: (base address & 0xfff) / 512 */
#define PORT_D3D_TEXTURE_ID_MIP_SUBPAGE_SHIFT  7           /* 3 bits: (mip address & 0xfff) / 512 */
#define PORT_D3D_TEXTURE_ID_SUBPAGE_MASK       0x7u
#define PORT_D3D_TEXTURE_ID_SUBPAGE_UNIT       512u
#define PORT_D3D_TEXTURE_ID_LEVELS_SHIFT       10          /* 5 bits: the levels the XDK built the header from */
#define PORT_D3D_TEXTURE_ID_LEVELS_MASK        0x1fu

struct s_port_d3d_texture_source {
    const void *base;                  /* host address of level 0 (NULL: no base address, or MinMipLevel excludes level 0) */
    const void *mip;                   /* host address of levels 1.. (NULL for one level) */
    struct s_xenos_texture_desc desc;  /* format, dimension, size and levels as the header says */
    uint32_t identifier;               /* the PORT_D3D_TEXTURE_ID_* word (0 for an ordinary texture) */
    uint32_t fetch_constant[6];        /* the header's GPUTEXTURE_FETCH_CONSTANT dwords in the GPU register layout (what
                                          translated Xenos shaders read: sizes, exp_adjust, dimension; xenos_spirv_abi.h) */
};

/* Describes a D3DBaseTexture header (any texture type) for texture_convert.h. Returns 0 if `texture_header` is not a
 * texture header. Implemented in xgraphics.cpp, which knows the XDK's fetch-constant layout. */
int PORT_MS_ABI port_d3d_texture_get_source(const void *texture_header, struct s_port_d3d_texture_source *out);

#ifdef __cplusplus
}
#endif

#endif /* PORT_SDK_D3D_D3D_PORT_TEXTURE_H */
