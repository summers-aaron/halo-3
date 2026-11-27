// port/sdk/d3d: Xbox 360 XGraphics resource-header and texture-layout routines.
// Built in halo3_sdk against <xtl.h> and <xgraphics.h> so the fetch-constant bitfields inside D3DTexture,
// D3DVertexBuffer, D3DIndexBuffer, and D3DSurface match what compiled game code inspects directly.
#include <xtl.h>
#include <xgraphics.h>
#include <string.h>
#include "d3d_port_buffer.h"
#include "d3d_port_texture.h"
#include "vulkan_device.h"

namespace {

UINT align_up(UINT value, UINT alignment)
{
    return (value + alignment - 1u) & ~(alignment - 1u);
}

UINT max_u(UINT a, UINT b)
{
    return a > b ? a : b;
}

UINT calc_max_levels(UINT width, UINT height, UINT depth)
{
    UINT dim = max_u(max_u(width, height), depth);
    UINT levels = 1;
    while (dim > 1) {
        dim >>= 1;
        levels++;
    }
    return levels;
}

void block_dims_of_gpu_format(DWORD gpu_format, UINT *block_w, UINT *block_h, UINT *bytes_per_block)
{
    switch (gpu_format) {
    case GPUTEXTUREFORMAT_DXT1:
    case GPUTEXTUREFORMAT_DXT1_AS_16_16_16_16:
    case GPUTEXTUREFORMAT_DXT3A:
    case GPUTEXTUREFORMAT_DXT5A:
    case GPUTEXTUREFORMAT_CTX1:
    case GPUTEXTUREFORMAT_DXT3A_AS_1_1_1_1:
        *block_w = 4; *block_h = 4; *bytes_per_block = 8;
        return;
    case GPUTEXTUREFORMAT_DXT2_3:
    case GPUTEXTUREFORMAT_DXT2_3_AS_16_16_16_16:
    case GPUTEXTUREFORMAT_DXT4_5:
    case GPUTEXTUREFORMAT_DXT4_5_AS_16_16_16_16:
    case GPUTEXTUREFORMAT_DXN:
        *block_w = 4; *block_h = 4; *bytes_per_block = 16;
        return;
    case GPUTEXTUREFORMAT_Cr_Y1_Cb_Y0_REP:
    case GPUTEXTUREFORMAT_Y1_Cr_Y0_Cb_REP:
        *block_w = 2; *block_h = 1; *bytes_per_block = 4;
        return;
    default: {
        UINT bpp = XGBitsPerPixelFromGpuFormat(gpu_format);
        *block_w = 1; *block_h = 1;
        *bytes_per_block = bpp >= 8 ? (bpp >> 3) : 1;
        return;
    }
    }
}

void level_footprint(UINT w, UINT h, UINT block_w, UINT block_h, UINT bytes_per_block, BOOL tiled,
    UINT explicit_pitch, UINT *out_row_pitch, UINT *out_slice_pitch, UINT *out_w_blocks, UINT *out_h_blocks)
{
    UINT w_blocks = (w + block_w - 1u) / block_w;
    UINT h_blocks = (h + block_h - 1u) / block_h;

    UINT aligned_w_blocks = tiled ? align_up(w_blocks, 32u) : w_blocks;
    UINT aligned_h_blocks = tiled ? align_up(h_blocks, 32u) : h_blocks;

    UINT row_pitch = explicit_pitch != 0
        ? explicit_pitch
        : (tiled ? aligned_w_blocks * bytes_per_block : align_up(w_blocks * bytes_per_block, 256u));
    UINT slice_pitch = align_up(row_pitch * aligned_h_blocks, 4096u);
    if (out_row_pitch) *out_row_pitch = row_pitch;
    if (out_slice_pitch) *out_slice_pitch = slice_pitch;
    if (out_w_blocks) *out_w_blocks = w_blocks;
    if (out_h_blocks) *out_h_blocks = h_blocks;
}

void init_resource_header(D3DResource *res, DWORD type_flags)
{
    memset(res, 0, sizeof(*res));
    res->Common = type_flags | D3DCOMMON_CPU_CACHED_MEMORY;
    res->ReferenceCount = 1;
    res->BaseFlush = D3DFLUSH_INITIAL_VALUE;
}

UINT init_base_texture_header(D3DBaseTexture *tex, GPUDIMENSION dim, UINT width, UINT height, UINT depth_or_slices,
    UINT levels, DWORD usage, D3DFORMAT format, INT exp_bias, DWORD flags, UINT base_offset, UINT mip_offset,
    UINT pitch, UINT *out_base_size, UINT *out_mip_size)
{
    (void)usage;
    if (width == 0) width = 1;
    if (height == 0) height = 1;
    if (depth_or_slices == 0) depth_or_slices = 1;
    UINT total_levels = levels == 0 ? calc_max_levels(width, height, dim == GPUDIMENSION_3D ? depth_or_slices : 1u) : levels;

    BOOL tiled = XGIsTiledFormat(format);
    DWORD gpu_format = XGGetGpuFormat(format);
    UINT block_w = 1, block_h = 1, bytes_per_block = 4;
    block_dims_of_gpu_format(gpu_format, &block_w, &block_h, &bytes_per_block);

    UINT slices = (dim == GPUDIMENSION_CUBEMAP) ? 6u : ((dim == GPUDIMENSION_2D && depth_or_slices > 1u) ? depth_or_slices : 1u);
    UINT vol_depth = (dim == GPUDIMENSION_3D) ? depth_or_slices : 1u;

    UINT row_pitch0 = 0, slice_pitch0 = 0;
    level_footprint(width, height, block_w, block_h, bytes_per_block, tiled, pitch, &row_pitch0, &slice_pitch0, 0, 0);
    UINT base_size = align_up(slice_pitch0 * slices * vol_depth, 4096u);

    UINT mip_size = 0;
    for (UINT lv = 1; lv < total_levels; lv++) {
        UINT sp = 0;
        UINT ld = max_u(vol_depth >> lv, 1u);
        level_footprint(max_u(width >> lv, 1u), max_u(height >> lv, 1u), block_w, block_h, bytes_per_block,
            tiled, 0, 0, &sp, 0, 0);
        mip_size += align_up(sp * slices * ld, 4096u);
    }

    if (tex != 0) {
        init_resource_header(tex, D3DCOMMON_TYPE_TEXTURE);
        tex->MipFlush = D3DFLUSH_INITIAL_VALUE;
        memset(&tex->Format, 0, sizeof(tex->Format));

        tex->Format.Type = GPUCONSTANTTYPE_TEXTURE;
        tex->Format.SignX = (format & D3DFORMAT_SIGNX_MASK) >> D3DFORMAT_SIGNX_SHIFT;
        tex->Format.SignY = (format & D3DFORMAT_SIGNY_MASK) >> D3DFORMAT_SIGNY_SHIFT;
        tex->Format.SignZ = (format & D3DFORMAT_SIGNZ_MASK) >> D3DFORMAT_SIGNZ_SHIFT;
        tex->Format.SignW = (format & D3DFORMAT_SIGNW_MASK) >> D3DFORMAT_SIGNW_SHIFT;
        UINT texel_pitch = (row_pitch0 / bytes_per_block) * block_w;
        tex->Format.Pitch = texel_pitch >> 5;
        tex->Format.Tiled = tiled ? 1 : 0;

        tex->Format.DataFormat = gpu_format;
        tex->Format.Endian = (format & D3DFORMAT_ENDIAN_MASK) >> D3DFORMAT_ENDIAN_SHIFT;
        tex->Format.RequestSize = GPUREQUESTSIZE_256BIT;
        tex->Format.Stacked = (dim == GPUDIMENSION_2D && slices > 1u) ? 1 : 0;
        tex->Format.BaseAddress = base_offset >> 12;

        if (dim == GPUDIMENSION_1D) {
            tex->Format.Size.OneD.Width = width - 1u;
        } else if (dim == GPUDIMENSION_3D) {
            tex->Format.Size.ThreeD.Width = width - 1u;
            tex->Format.Size.ThreeD.Height = height - 1u;
            tex->Format.Size.ThreeD.Depth = vol_depth - 1u;
        } else if (slices > 1u) {
            tex->Format.Size.Stack.Width = width - 1u;
            tex->Format.Size.Stack.Height = height - 1u;
            tex->Format.Size.Stack.Depth = slices - 1u;
        } else {
            tex->Format.Size.TwoD.Width = width - 1u;
            tex->Format.Size.TwoD.Height = height - 1u;
        }

        tex->Format.NumFormat = (format & D3DFORMAT_NUMFORMAT_MASK) >> D3DFORMAT_NUMFORMAT_SHIFT;
        tex->Format.SwizzleX = (format & D3DFORMAT_SWIZZLEX_MASK) >> D3DFORMAT_SWIZZLEX_SHIFT;
        tex->Format.SwizzleY = (format & D3DFORMAT_SWIZZLEY_MASK) >> D3DFORMAT_SWIZZLEY_SHIFT;
        tex->Format.SwizzleZ = (format & D3DFORMAT_SWIZZLEZ_MASK) >> D3DFORMAT_SWIZZLEZ_SHIFT;
        tex->Format.SwizzleW = (format & D3DFORMAT_SWIZZLEW_MASK) >> D3DFORMAT_SWIZZLEW_SHIFT;
        tex->Format.ExpAdjust = exp_bias;
        tex->Format.BorderSize = (flags & XGHEADEREX_BORDER) ? 1 : 0;

        tex->Format.MinMipLevel = 0;
        tex->Format.MaxMipLevel = total_levels > 0 ? total_levels - 1u : 0;

        tex->Format.Dimension = dim;
        tex->Format.PackedMips = (total_levels > 1u && !(flags & XGHEADEREX_NONPACKED)) ? 1 : 0;
        if (total_levels > 1u) {
            UINT resolved_mip = (mip_offset == XGHEADER_CONTIGUOUS_MIP_OFFSET) ? (base_offset + base_size) : mip_offset;
            tex->Format.MipAddress = resolved_mip >> 12;
        }
    }

    if (out_base_size) *out_base_size = base_size;
    if (out_mip_size) *out_mip_size = mip_size;
    return base_size + mip_size;
}

// The D3DFORMAT a texture header's fetch constant describes.
D3DFORMAT header_format(const D3DBaseTexture *texture)
{
    const GPUTEXTURE_FETCH_CONSTANT &f = texture->Format;
    return (D3DFORMAT)(
        (f.DataFormat << D3DFORMAT_TEXTUREFORMAT_SHIFT)
        | (f.Tiled ? D3DFORMAT_TILED_MASK : 0)
        | (f.SignX << D3DFORMAT_SIGNX_SHIFT)
        | (f.SignY << D3DFORMAT_SIGNY_SHIFT)
        | (f.SignZ << D3DFORMAT_SIGNZ_SHIFT)
        | (f.SignW << D3DFORMAT_SIGNW_SHIFT)
        | (f.NumFormat << D3DFORMAT_NUMFORMAT_SHIFT)
        | (f.SwizzleX << D3DFORMAT_SWIZZLEX_SHIFT)
        | (f.SwizzleY << D3DFORMAT_SWIZZLEY_SHIFT)
        | (f.SwizzleZ << D3DFORMAT_SWIZZLEZ_SHIFT)
        | (f.SwizzleW << D3DFORMAT_SWIZZLEW_SHIFT)
        | (f.Endian << D3DFORMAT_ENDIAN_SHIFT));
}

// One texture of an XGSet{,Cube,Array}TextureHeaderPair call.
struct s_pair_member
{
    UINT width;
    UINT height;
    UINT array_size;   // array textures: slices; 1 otherwise
    UINT levels;       // 0: the full chain
    DWORD usage;
    D3DFORMAT format;
    INT exp_bias;
    DWORD flags;
    UINT pitch;
    D3DBaseTexture *texture;
};

// The XDK's fallback for textures it cannot interleave (3D, or two different dimensions): two ordinary textures, the
// second after the first in both allocations.
UINT set_texture_header_sequence(GPUDIMENSION dim, UINT base_offset, UINT mip_offset, const s_pair_member m[2],
    UINT *out_base_size, UINT *out_mip_size)
{
    UINT b1 = 0, m1 = 0, b2 = 0, m2 = 0;
    UINT slices1 = dim == GPUDIMENSION_CUBEMAP ? 6u : m[0].array_size;
    UINT slices2 = dim == GPUDIMENSION_CUBEMAP ? 6u : m[1].array_size;
    init_base_texture_header(m[0].texture, dim, m[0].width, m[0].height, slices1, m[0].levels, m[0].usage, m[0].format,
        m[0].exp_bias, m[0].flags, base_offset, mip_offset, m[0].pitch, &b1, &m1);
    init_base_texture_header(m[1].texture, dim, m[1].width, m[1].height, slices2, m[1].levels, m[1].usage, m[1].format,
        m[1].exp_bias, m[1].flags, base_offset + b1,
        mip_offset == XGHEADER_CONTIGUOUS_MIP_OFFSET ? XGHEADER_CONTIGUOUS_MIP_OFFSET : mip_offset + m1, m[1].pitch, &b2, &m2);
    if (out_base_size) *out_base_size = b1 + b2;
    if (out_mip_size) *out_mip_size = m1 + m2;
    return b1 + b2 + m1 + m2;
}

// XGSet{,Cube,Array}TextureHeaderPair: two textures interleaved in one allocation, the smaller one's chain inside the
// larger one's unused tile space. The placement is the retail XDK's (port/data/texture_convert.cpp,
// xenos_texture_pair_placement, checked against every interleaved bitmap of the retail maps). Each header is an
// ordinary texture header at its own base and mip addresses, plus the pair record in Identifier (d3d_port_texture.h).
UINT set_texture_header_pair(GPUDIMENSION dim, UINT base_offset, UINT mip_offset, s_pair_member m[2],
    UINT *out_base_size, UINT *out_mip_size)
{
    s_xenos_texture_desc desc[2];
    for (int i = 0; i < 2; i++) {
        if (m[i].width == 0) m[i].width = 1;
        if (m[i].height == 0) m[i].height = 1;
        if (m[i].array_size == 0) m[i].array_size = 1;
        if (m[i].levels == 0) m[i].levels = calc_max_levels(m[i].width, m[i].height, 1u);
        desc[i].d3d_format = (uint32_t)m[i].format;
        desc[i].dimension = dim == GPUDIMENSION_CUBEMAP ? k_xenos_dimension_cube
            : (dim == GPUDIMENSION_3D ? k_xenos_dimension_3d
            : (m[i].array_size > 1 ? k_xenos_dimension_array : k_xenos_dimension_2d));
        desc[i].width = m[i].width;
        desc[i].height = m[i].height;
        desc[i].depth = desc[i].dimension == k_xenos_dimension_array ? m[i].array_size : 1u;
        desc[i].levels = m[i].levels;
        desc[i].packed_mips = 1;
    }
    s_xenos_texture_pair_placement place[2];
    uint32_t base_size = 0, mip_size = 0;
    if (!xenos_texture_pair_placement(base_offset, mip_offset, desc, place, &base_size, &mip_size))
        return set_texture_header_sequence(dim, base_offset, mip_offset, m, out_base_size, out_mip_size);

    const BOOL mip_allocation = mip_offset != XGHEADER_CONTIGUOUS_MIP_OFFSET;
    for (int i = 0; i < 2; i++) {
        // Halo 3 marks its separate mip allocation with MipOffset bit 31; the GPU drops it with the other bits above
        // its physical address range, and on PC it would overflow the fetch constant once XGOffsetBaseTextureAddress
        // adds the host address, so the header keeps the offset without it.
        const UINT base_address = place[i].base_address & 0x7fffffffu;
        const UINT mip_address = place[i].mip_address & 0x7fffffffu;
        const BOOL mips_at_base = m[i].levels > 1 && place[i].mip_address == 0;
        const UINT header_mip = mips_at_base ? base_address : mip_address;
        if (m[i].texture) {
            UINT slices = dim == GPUDIMENSION_CUBEMAP ? 6u : m[i].array_size;
            init_base_texture_header(m[i].texture, dim, m[i].width, m[i].height, slices, m[i].levels, m[i].usage,
                m[i].format, m[i].exp_bias, m[i].flags, base_address & ~0xfffu, header_mip & ~0xfffu, m[i].pitch, 0, 0);
            m[i].texture->Format.PackedMips = place[i].header_levels > 1 ? 1 : 0;
            m[i].texture->Identifier = PORT_D3D_TEXTURE_ID_PAIR
                | (i == 1 ? PORT_D3D_TEXTURE_ID_PAIR_SECOND : 0)
                | (place[i].outer ? PORT_D3D_TEXTURE_ID_PAIR_OUTER : 0)
                | (mips_at_base ? PORT_D3D_TEXTURE_ID_MIPS_AT_BASE : 0)
                | (mip_allocation ? PORT_D3D_TEXTURE_ID_MIP_ALLOCATION : 0)
                | (((base_address & 0xfffu) / PORT_D3D_TEXTURE_ID_SUBPAGE_UNIT) << PORT_D3D_TEXTURE_ID_BASE_SUBPAGE_SHIFT)
                | (((header_mip & 0xfffu) / PORT_D3D_TEXTURE_ID_SUBPAGE_UNIT) << PORT_D3D_TEXTURE_ID_MIP_SUBPAGE_SHIFT)
                | ((place[i].header_levels < PORT_D3D_TEXTURE_ID_LEVELS_MASK ? place[i].header_levels
                    : PORT_D3D_TEXTURE_ID_LEVELS_MASK) << PORT_D3D_TEXTURE_ID_LEVELS_SHIFT);
        }
    }
    if (out_base_size) *out_base_size = base_size;
    if (out_mip_size) *out_mip_size = mip_size;
    return base_size + mip_size;
}

} // namespace

extern "C" {

VOID WINAPI XGGetBlockDimensions(DWORD GpuFormat, UINT *pWidth, UINT *pHeight)
{
    UINT bw = 1, bh = 1, bpb = 4;
    block_dims_of_gpu_format(GpuFormat, &bw, &bh, &bpb);
    if (pWidth) *pWidth = bw;
    if (pHeight) *pHeight = bh;
}

UINT WINAPI XGGetMipTailBaseLevel(UINT Width, UINT Height, BOOL Border)
{
    UINT b = Border ? 1u : 0u;
    UINT level = 0;
    while ((Width >> level) > (16u >> b) && (Height >> level) > (16u >> b))
        level++;
    return level;
}

UINT WINAPI XGGetMipTailLevelOffset(UINT Width, UINT Height, UINT Depth, UINT Level, DWORD GpuFormat, BOOL Tiled, BOOL Border)
{
    (void)Tiled;
    UINT base_level = XGGetMipTailBaseLevel(Width, Height, Border);
    if (Level <= base_level)
        return 0;
    UINT bw = 1, bh = 1, bpb = 4;
    block_dims_of_gpu_format(GpuFormat, &bw, &bh, &bpb);
    UINT offset = 0;
    for (UINT lv = base_level; lv < Level; lv++) {
        UINT w = max_u(Width >> lv, 1u);
        UINT h = max_u(Height >> lv, 1u);
        UINT d = max_u(Depth >> lv, 1u);
        UINT wb = align_up((w + bw - 1u) / bw, 4u);
        UINT hb = align_up((h + bh - 1u) / bh, 4u);
        offset += wb * hb * d * bpb;
    }
    return offset;
}

VOID WINAPI XGGetMipTailDesc(D3DBaseTexture *pTexture, XGMIPTAIL_DESC *pDesc)
{
    if (!pTexture || !pDesc)
        return;
    memset(pDesc, 0, sizeof(*pDesc));
    XGTEXTURE_DESC base_desc;
    XGGetTextureDesc(pTexture, 0, &base_desc);
    UINT tail_level = XGGetMipTailBaseLevel(base_desc.Width, base_desc.Height, pTexture->Format.BorderSize != 0);
    XGGetTextureDesc(pTexture, tail_level, (XGTEXTURE_DESC *)pDesc);
    pDesc->BaseLevel = tail_level;
    pDesc->Size = base_desc.SlicePitch;
}

UINT WINAPI XGGetMipLevelOffset(D3DBaseTexture *pTexture, UINT ArrayIndex, UINT Level)
{
    if (!pTexture)
        return 0;
    XGTEXTURE_DESC base_desc;
    XGGetTextureDesc(pTexture, 0, &base_desc);
    UINT slices = pTexture->Format.Dimension == GPUDIMENSION_CUBEMAP ? 6u
        : (pTexture->Format.Stacked ? pTexture->Format.Size.Stack.Depth + 1u : 1u);
    if (Level == 0)
        return ArrayIndex * base_desc.SlicePitch;

    UINT offset = 0;
    UINT start_level = (pTexture->Format.MipAddress != 0) ? 1u : 0u;
    for (UINT lv = start_level; lv < Level; lv++) {
        XGTEXTURE_DESC d;
        XGGetTextureDesc(pTexture, lv, &d);
        offset += align_up(d.SlicePitch * slices * d.Depth, 4096u);
    }
    XGTEXTURE_DESC cur;
    XGGetTextureDesc(pTexture, Level, &cur);
    return offset + ArrayIndex * cur.SlicePitch;
}

VOID WINAPI XGGetTextureDesc(D3DBaseTexture *pTexture, UINT Level, XGTEXTURE_DESC *pDesc)
{
    if (!pTexture || !pDesc)
        return;
    memset(pDesc, 0, sizeof(*pDesc));

    DWORD resource_type = pTexture->Common & D3DCOMMON_TYPE_MASK;
    if (resource_type == D3DCOMMON_TYPE_SURFACE) {
        D3DSurface *surf = (D3DSurface *)pTexture;
        pDesc->ResourceType = D3DRTYPE_SURFACE;
        pDesc->Width = surf->Width ? surf->Width : 1u;
        pDesc->Height = surf->Height ? surf->Height : 1u;
        pDesc->Depth = 1u;
        pDesc->Format = surf->Format;
        DWORD gpu_fmt = XGGetGpuFormat(surf->Format);
        UINT bw = 1, bh = 1, bpb = 4;
        block_dims_of_gpu_format(gpu_fmt, &bw, &bh, &bpb);
        pDesc->BitsPerPixel = XGBitsPerPixelFromGpuFormat(gpu_fmt);
        pDesc->BytesPerBlock = bpb;
        pDesc->WidthInBlocks = (pDesc->Width + bw - 1u) / bw;
        pDesc->HeightInBlocks = (pDesc->Height + bh - 1u) / bh;
        pDesc->DepthInBlocks = 1u;
        pDesc->RowPitch = pDesc->WidthInBlocks * bpb;
        pDesc->SlicePitch = pDesc->RowPitch * pDesc->HeightInBlocks;
        return;
    }

    UINT base_w = 1, base_h = 1, base_d = 1;
    D3DRESOURCETYPE rtype = D3DRTYPE_TEXTURE;
    switch (pTexture->Format.Dimension) {
    case GPUDIMENSION_1D:
        rtype = D3DRTYPE_LINETEXTURE;
        base_w = pTexture->Format.Size.OneD.Width + 1u;
        break;
    case GPUDIMENSION_3D:
        rtype = D3DRTYPE_VOLUMETEXTURE;
        base_w = pTexture->Format.Size.ThreeD.Width + 1u;
        base_h = pTexture->Format.Size.ThreeD.Height + 1u;
        base_d = pTexture->Format.Size.ThreeD.Depth + 1u;
        break;
    case GPUDIMENSION_CUBEMAP:
        rtype = D3DRTYPE_CUBETEXTURE;
        base_w = pTexture->Format.Size.Stack.Width + 1u;
        base_h = pTexture->Format.Size.Stack.Height + 1u;
        break;
    default:
        if (pTexture->Format.Stacked) {
            rtype = D3DRTYPE_ARRAYTEXTURE;
            base_w = pTexture->Format.Size.Stack.Width + 1u;
            base_h = pTexture->Format.Size.Stack.Height + 1u;
            base_d = pTexture->Format.Size.Stack.Depth + 1u;
        } else {
            rtype = D3DRTYPE_TEXTURE;
            base_w = pTexture->Format.Size.TwoD.Width + 1u;
            base_h = pTexture->Format.Size.TwoD.Height + 1u;
        }
        break;
    }

    DWORD gpu_fmt = pTexture->Format.DataFormat;
    BOOL tiled = pTexture->Format.Tiled != 0;
    UINT bw = 1, bh = 1, bpb = 4;
    block_dims_of_gpu_format(gpu_fmt, &bw, &bh, &bpb);
    UINT explicit_pitch = Level == 0 && pTexture->Format.Pitch != 0
        ? ((pTexture->Format.Pitch << 5) / bw) * bpb
        : 0;
    UINT width = max_u(base_w >> Level, 1u);
    UINT height = max_u(base_h >> Level, 1u);
    UINT rp = 0, sp = 0, wb = 0, hb = 0;
    level_footprint(width, height, bw, bh, bpb, tiled, explicit_pitch, &rp, &sp, &wb, &hb);

    pDesc->ResourceType = rtype;
    pDesc->Width = width;
    pDesc->Height = height;
    pDesc->Depth = rtype == D3DRTYPE_VOLUMETEXTURE ? max_u(base_d >> Level, 1u) : base_d;
    pDesc->Format = header_format(pTexture);
    pDesc->RowPitch = rp;
    pDesc->SlicePitch = sp;
    pDesc->BitsPerPixel = XGBitsPerPixelFromGpuFormat(gpu_fmt);
    pDesc->WidthInBlocks = wb;
    pDesc->HeightInBlocks = hb;
    pDesc->DepthInBlocks = pDesc->Depth;
    pDesc->BytesPerBlock = bpb;
    pDesc->ExpBias = pTexture->Format.ExpAdjust;
    pDesc->Flags = (tiled ? XGTDESC_TILED : 0)
        | (pTexture->Format.PackedMips ? XGTDESC_PACKED : 0)
        | (pTexture->Format.BorderSize ? XGTDESC_BORDERED : 0);
    pDesc->MultiSampleType = D3DMULTISAMPLE_NONE;
}

UINT WINAPI XGSetSurfaceHeader(UINT Width, UINT Height, D3DFORMAT Format, D3DMULTISAMPLE_TYPE MultiSampleType,
    CONST D3DSURFACE_PARAMETERS *pParameters, IDirect3DSurface9 *pSurface, UINT *pHierarchicalZSize)
{
    UINT tiles = XGSurfaceSize(Width, Height, Format, MultiSampleType);
    UINT hiz = XGHierarchicalZSize(Width, Height, MultiSampleType);
    if (pHierarchicalZSize)
        *pHierarchicalZSize = hiz;
    if (pSurface) {
        init_resource_header(pSurface, D3DCOMMON_TYPE_SURFACE);
        pSurface->Width = Width;
        pSurface->Height = Height;
        pSurface->Format = Format;
        pSurface->Size = tiles * GPU_EDRAM_TILE_SIZE;
        // RB_SURFACE_INFO as the retail XGSetSurfaceHeader (0x825a4758) writes it: the pitch in pixels, whole EDRAM
        // tiles wide (80 fragments: 80 pixels, 40 with 4x multisampling, whose pixels are 2x2 fragments), the sample
        // count, and the hierarchical Z pitch (32 pixels, 16 with 4x). The renderer reads the sample count to know how
        // a multisampled view's pixels lie over another surface's EDRAM (vulkan_device.cpp edram_sync_owner).
        const UINT tile_width = MultiSampleType == D3DMULTISAMPLE_4_SAMPLES ? 40u : 80u;
        const UINT hiz_width = MultiSampleType == D3DMULTISAMPLE_4_SAMPLES ? 16u : 32u;
        pSurface->SurfaceInfo.SurfacePitch = XGNextMultiple(Width, tile_width);
        pSurface->SurfaceInfo.MsaaSamples = (DWORD)MultiSampleType & 3u;
        pSurface->SurfaceInfo.HiZPitch = XGNextMultiple(Width, hiz_width);
        if (pParameters) {
            pSurface->ColorInfo.ColorBase = pParameters->Base;
            // RB_COLOR_INFO: color writes are scaled by 2^ColorExpBias (-32..31); bits the depth view leaves unused
            pSurface->ColorInfo.ColorExpBias = (DWORD)pParameters->ColorExpBias & 0x3fu;
            pSurface->HiControl.HiBaseAddr = pParameters->HierarchicalZBase;
        }
    }
    return tiles;
}

UINT WINAPI XGSetTextureHeaderEx(UINT Width, UINT Height, UINT Levels, DWORD Usage, D3DFORMAT Format, INT ExpBias,
    DWORD Flags, UINT BaseOffset, UINT MipOffset, UINT Pitch, IDirect3DTexture9 *pTexture, UINT *pBaseSize, UINT *pMipSize)
{
    return init_base_texture_header(pTexture, GPUDIMENSION_2D, Width, Height, 1u, Levels, Usage, Format, ExpBias, Flags,
        BaseOffset, MipOffset, Pitch, pBaseSize, pMipSize);
}

UINT WINAPI XGSetTextureHeader(UINT Width, UINT Height, UINT Levels, DWORD Usage, D3DFORMAT Format, D3DPOOL Pool,
    UINT BaseOffset, UINT MipOffset, UINT Pitch, IDirect3DTexture9 *pTexture, UINT *pBaseSize, UINT *pMipSize)
{
    (void)Pool;
    return XGSetTextureHeaderEx(Width, Height, Levels, Usage, Format, 0, 0, BaseOffset, MipOffset, Pitch, pTexture,
        pBaseSize, pMipSize);
}

UINT WINAPI XGSetTextureHeaderPair(UINT BaseOffset, UINT MipOffset,
    UINT Width1, UINT Height1, UINT Levels1, DWORD Usage1, D3DFORMAT Format1, INT ExpBias1, DWORD Flags1, UINT Pitch1, IDirect3DTexture9 *pTexture1,
    UINT Width2, UINT Height2, UINT Levels2, DWORD Usage2, D3DFORMAT Format2, INT ExpBias2, DWORD Flags2, UINT Pitch2, IDirect3DTexture9 *pTexture2,
    UINT *pBaseSize, UINT *pMipSize)
{
    s_pair_member m[2] = {
        { Width1, Height1, 1u, Levels1, Usage1, Format1, ExpBias1, Flags1, Pitch1, pTexture1 },
        { Width2, Height2, 1u, Levels2, Usage2, Format2, ExpBias2, Flags2, Pitch2, pTexture2 },
    };
    return set_texture_header_pair(GPUDIMENSION_2D, BaseOffset, MipOffset, m, pBaseSize, pMipSize);
}

UINT WINAPI XGSetCubeTextureHeaderEx(UINT EdgeLength, UINT Levels, DWORD Usage, D3DFORMAT Format, INT ExpBias,
    DWORD Flags, UINT BaseOffset, UINT MipOffset, IDirect3DCubeTexture9 *pCubeTexture, UINT *pBaseSize, UINT *pMipSize)
{
    return init_base_texture_header(pCubeTexture, GPUDIMENSION_CUBEMAP, EdgeLength, EdgeLength, 6u, Levels, Usage, Format,
        ExpBias, Flags, BaseOffset, MipOffset, 0, pBaseSize, pMipSize);
}

UINT WINAPI XGSetCubeTextureHeader(UINT EdgeLength, UINT Levels, DWORD Usage, D3DFORMAT Format, D3DPOOL Pool,
    UINT BaseOffset, UINT MipOffset, IDirect3DCubeTexture9 *pCubeTexture, UINT *pBaseSize, UINT *pMipSize)
{
    (void)Pool;
    return XGSetCubeTextureHeaderEx(EdgeLength, Levels, Usage, Format, 0, 0, BaseOffset, MipOffset, pCubeTexture,
        pBaseSize, pMipSize);
}

UINT WINAPI XGSetCubeTextureHeaderPair(UINT BaseOffset, UINT MipOffset,
    UINT EdgeLength1, UINT Levels1, DWORD Usage1, D3DFORMAT Format1, INT ExpBias1, DWORD Flags1, IDirect3DTexture9 *pCubeTexture1,
    UINT EdgeLength2, UINT Levels2, DWORD Usage2, D3DFORMAT Format2, INT ExpBias2, DWORD Flags2, IDirect3DTexture9 *pCubeTexture2,
    UINT *pBaseSize, UINT *pMipSize)
{
    s_pair_member m[2] = {
        { EdgeLength1, EdgeLength1, 1u, Levels1, Usage1, Format1, ExpBias1, Flags1, 0u, pCubeTexture1 },
        { EdgeLength2, EdgeLength2, 1u, Levels2, Usage2, Format2, ExpBias2, Flags2, 0u, pCubeTexture2 },
    };
    return set_texture_header_pair(GPUDIMENSION_CUBEMAP, BaseOffset, MipOffset, m, pBaseSize, pMipSize);
}

UINT WINAPI XGSetArrayTextureHeaderEx(UINT Width, UINT Height, UINT ArraySize, UINT Levels, DWORD Usage, D3DFORMAT Format,
    INT ExpBias, DWORD Flags, UINT BaseOffset, UINT MipOffset, UINT Pitch, IDirect3DArrayTexture9 *pArrayTexture,
    UINT *pBaseSize, UINT *pMipSize)
{
    return init_base_texture_header(pArrayTexture, GPUDIMENSION_2D, Width, Height, max_u(ArraySize, 1u), Levels, Usage,
        Format, ExpBias, Flags, BaseOffset, MipOffset, Pitch, pBaseSize, pMipSize);
}

UINT WINAPI XGSetArrayTextureHeader(UINT Width, UINT Height, UINT ArraySize, UINT Levels, DWORD Usage, D3DFORMAT Format,
    D3DPOOL Pool, UINT BaseOffset, UINT MipOffset, UINT Pitch, IDirect3DArrayTexture9 *pArrayTexture,
    UINT *pBaseSize, UINT *pMipSize)
{
    (void)Pool;
    return XGSetArrayTextureHeaderEx(Width, Height, ArraySize, Levels, Usage, Format, 0, 0, BaseOffset, MipOffset, Pitch,
        pArrayTexture, pBaseSize, pMipSize);
}

UINT WINAPI XGSetArrayTextureHeaderPair(UINT BaseOffset, UINT MipOffset,
    UINT Width1, UINT Height1, UINT ArraySize1, UINT Levels1, DWORD Usage1, D3DFORMAT Format1, INT ExpBias1, DWORD Flags1, UINT Pitch1, IDirect3DTexture9 *pArrayTexture1,
    UINT Width2, UINT Height2, UINT ArraySize2, UINT Levels2, DWORD Usage2, D3DFORMAT Format2, INT ExpBias2, DWORD Flags2, UINT Pitch2, IDirect3DTexture9 *pArrayTexture2,
    UINT *pBaseSize, UINT *pMipSize)
{
    s_pair_member m[2] = {
        { Width1, Height1, ArraySize1, Levels1, Usage1, Format1, ExpBias1, Flags1, Pitch1, pArrayTexture1 },
        { Width2, Height2, ArraySize2, Levels2, Usage2, Format2, ExpBias2, Flags2, Pitch2, pArrayTexture2 },
    };
    return set_texture_header_pair(GPUDIMENSION_2D, BaseOffset, MipOffset, m, pBaseSize, pMipSize);
}

UINT WINAPI XGSetVolumeTextureHeaderEx(UINT Width, UINT Height, UINT Depth, UINT Levels, DWORD Usage, D3DFORMAT Format,
    INT ExpBias, DWORD Flags, UINT BaseOffset, UINT MipOffset, IDirect3DVolumeTexture9 *pVolumeTexture,
    UINT *pBaseSize, UINT *pMipSize)
{
    return init_base_texture_header(pVolumeTexture, GPUDIMENSION_3D, Width, Height, max_u(Depth, 1u), Levels, Usage,
        Format, ExpBias, Flags, BaseOffset, MipOffset, 0, pBaseSize, pMipSize);
}

UINT WINAPI XGSetVolumeTextureHeader(UINT Width, UINT Height, UINT Depth, UINT Levels, DWORD Usage, D3DFORMAT Format,
    D3DPOOL Pool, UINT BaseOffset, UINT MipOffset, IDirect3DVolumeTexture9 *pVolumeTexture, UINT *pBaseSize, UINT *pMipSize)
{
    (void)Pool;
    return XGSetVolumeTextureHeaderEx(Width, Height, Depth, Levels, Usage, Format, 0, 0, BaseOffset, MipOffset,
        pVolumeTexture, pBaseSize, pMipSize);
}

UINT WINAPI XGSetLineTextureHeaderEx(UINT Width, UINT Levels, DWORD Usage, D3DFORMAT Format, INT ExpBias, DWORD Flags,
    UINT BaseOffset, UINT MipOffset, IDirect3DLineTexture9 *pTexture, UINT *pBaseSize, UINT *pMipSize)
{
    return init_base_texture_header(pTexture, GPUDIMENSION_1D, Width, 1u, 1u, Levels, Usage, Format, ExpBias, Flags,
        BaseOffset, MipOffset, 0, pBaseSize, pMipSize);
}

UINT WINAPI XGSetLineTextureHeader(UINT Width, UINT Levels, DWORD Usage, D3DFORMAT Format, D3DPOOL Pool,
    UINT BaseOffset, UINT MipOffset, IDirect3DLineTexture9 *pTexture, UINT *pBaseSize, UINT *pMipSize)
{
    (void)Pool;
    return XGSetLineTextureHeaderEx(Width, Levels, Usage, Format, 0, 0, BaseOffset, MipOffset, pTexture, pBaseSize, pMipSize);
}

VOID WINAPI XGSetVertexBufferHeader(UINT Length, DWORD Usage, D3DPOOL Pool, UINT Offset, IDirect3DVertexBuffer9 *pVertexBuffer)
{
    (void)Usage; (void)Pool;
    if (!pVertexBuffer)
        return;
    init_resource_header(pVertexBuffer, D3DCOMMON_TYPE_VERTEXBUFFER);
    pVertexBuffer->Format.Type = GPUCONSTANTTYPE_VERTEX;
    pVertexBuffer->Format.BaseAddress = Offset >> 2;
    pVertexBuffer->Format.Endian = GPUENDIAN_8IN32;
    pVertexBuffer->Format.Size = (Length + 3u) >> 2;
    // memory D3D did not write: Xenos byte order (d3d_port_buffer.h)
    pVertexBuffer->Identifier = PORT_D3D_BUFFER_ID_XENOS_ORDER;
    if (Offset != 0)
        port_vk_memory_placed((const void *)(size_t)Offset, Length);
}

VOID WINAPI XGSetIndexBufferHeader(UINT Length, DWORD Usage, D3DFORMAT Format, D3DPOOL Pool, UINT Offset,
    IDirect3DIndexBuffer9 *pIndexBuffer)
{
    (void)Usage; (void)Pool;
    if (!pIndexBuffer)
        return;
    DWORD extra = (Format == D3DFMT_INDEX32 ? D3DINDEXBUFFER_INDEX32 : 0)
        | ((Format == D3DFMT_INDEX32 ? GPUENDIAN_8IN32 : GPUENDIAN_8IN16) << D3DINDEXBUFFER_ENDIAN_SHIFT);
    init_resource_header(pIndexBuffer, D3DCOMMON_TYPE_INDEXBUFFER | extra);
    pIndexBuffer->Address = Offset;
    pIndexBuffer->Size = Length;
    pIndexBuffer->Identifier = PORT_D3D_BUFFER_ID_XENOS_ORDER;
    if (Offset != 0)
        port_vk_memory_placed((const void *)(size_t)Offset, Length);
}

VOID WINAPI XGSetConstantBufferHeader(UINT VectorCount, DWORD Usage, UINT Offset, IDirect3DConstantBuffer9 *pConstantBuffer)
{
    (void)Usage;
    if (!pConstantBuffer)
        return;
    init_resource_header((D3DResource *)pConstantBuffer, D3DCOMMON_TYPE_CONSTANTBUFFER);
    DWORD *words = (DWORD *)pConstantBuffer;
    words[6] = Offset;
    words[7] = VectorCount * 16u;
}

VOID WINAPI XGOffsetResourceAddress(IDirect3DResource9 *pResource, VOID *pBaseAddress)
{
    if (!pResource)
        return;
    DWORD base = (DWORD)(size_t)pBaseAddress;
    switch (pResource->Common & D3DCOMMON_TYPE_MASK) {
    case D3DCOMMON_TYPE_VERTEXBUFFER: {
        D3DVertexBuffer *vb = (D3DVertexBuffer *)pResource;
        vb->Format.BaseAddress += (base >> 2);
        port_vk_memory_placed((const void *)((size_t)(DWORD)vb->Format.BaseAddress << 2), vb->Format.Size * 4u);
        break;
    }
    case D3DCOMMON_TYPE_INDEXBUFFER: {
        D3DIndexBuffer *ib = (D3DIndexBuffer *)pResource;
        ib->Address += base;
        port_vk_memory_placed((const void *)(size_t)ib->Address, ib->Size);
        break;
    }
    case D3DCOMMON_TYPE_TEXTURE: {
        D3DBaseTexture *tex = (D3DBaseTexture *)pResource;
        tex->Format.BaseAddress += (base >> 12);
        if (tex->Format.MipAddress != 0)
            tex->Format.MipAddress += (base >> 12);
        break;
    }
    default:
        break;
    }
}

VOID WINAPI XGOffsetBaseTextureAddress(IDirect3DBaseTexture9 *pTexture, VOID *pBaseAddress, VOID *pMipAddress)
{
    if (!pTexture)
        return;
    if (pBaseAddress)
        pTexture->Format.BaseAddress += ((DWORD)(size_t)pBaseAddress >> 12);
    if (pMipAddress && pTexture->Format.MaxMipLevel > 0)
        pTexture->Format.MipAddress += ((DWORD)(size_t)pMipAddress >> 12);
}

VOID WINAPI XGOffsetSurfaceAddress(IDirect3DSurface9 *pSurface, INT SurfaceOffset, INT HierarchicalZOffset)
{
    if (!pSurface)
        return;
    pSurface->ColorInfo.ColorBase += SurfaceOffset;
    pSurface->HiControl.HiBaseAddr += HierarchicalZOffset;
}


// d3d_port_texture.h: a texture header as texture_convert.h describes textures, with its data's host addresses.
int port_d3d_texture_get_source(const void *texture_header, s_port_d3d_texture_source *out)
{
    const D3DBaseTexture *texture = (const D3DBaseTexture *)texture_header;
    if (!texture || !out || (texture->Common & D3DCOMMON_TYPE_MASK) != D3DCOMMON_TYPE_TEXTURE)
        return 0;
    memset(out, 0, sizeof(*out));
    const GPUTEXTURE_FETCH_CONSTANT &f = texture->Format;
    s_xenos_texture_desc &d = out->desc;
    d.d3d_format = (uint32_t)header_format(texture);
    d.depth = 1;
    switch (f.Dimension) {
    case GPUDIMENSION_1D:
        d.dimension = k_xenos_dimension_2d;
        d.width = f.Size.OneD.Width + 1u;
        d.height = 1;
        break;
    case GPUDIMENSION_3D:
        d.dimension = k_xenos_dimension_3d;
        d.width = f.Size.ThreeD.Width + 1u;
        d.height = f.Size.ThreeD.Height + 1u;
        d.depth = f.Size.ThreeD.Depth + 1u;
        break;
    case GPUDIMENSION_CUBEMAP:
        d.dimension = k_xenos_dimension_cube;
        d.width = f.Size.Stack.Width + 1u;
        d.height = f.Size.Stack.Height + 1u;
        break;
    default:
        if (f.Stacked) {
            d.dimension = k_xenos_dimension_array;
            d.width = f.Size.Stack.Width + 1u;
            d.height = f.Size.Stack.Height + 1u;
            d.depth = f.Size.Stack.Depth + 1u;
        } else {
            d.dimension = k_xenos_dimension_2d;
            d.width = f.Size.TwoD.Width + 1u;
            d.height = f.Size.TwoD.Height + 1u;
        }
        break;
    }
    d.levels = f.MaxMipLevel + 1u;
    d.packed_mips = f.PackedMips;
    for (int i = 0; i < 6; i++)
        out->fetch_constant[i] = f.dword[i]; // LSB-first bitfields (d3d9gpu.h bitfield_order): the register layout

    const DWORD id = texture->Identifier;
    const BOOL pair = (id & PORT_D3D_TEXTURE_ID_KIND_MASK) == PORT_D3D_TEXTURE_ID_PAIR;
    out->identifier = pair ? id : 0;
    const DWORD base_subpage = pair
        ? ((id >> PORT_D3D_TEXTURE_ID_BASE_SUBPAGE_SHIFT) & PORT_D3D_TEXTURE_ID_SUBPAGE_MASK) * PORT_D3D_TEXTURE_ID_SUBPAGE_UNIT : 0;
    const DWORD mip_subpage = pair
        ? ((id >> PORT_D3D_TEXTURE_ID_MIP_SUBPAGE_SHIFT) & PORT_D3D_TEXTURE_ID_SUBPAGE_MASK) * PORT_D3D_TEXTURE_ID_SUBPAGE_UNIT : 0;
    if (f.BaseAddress)
        out->base = (const void *)(size_t)(((DWORD)f.BaseAddress << 12) + base_subpage);
    if (d.levels > 1) {
        if (pair && (id & PORT_D3D_TEXTURE_ID_MIPS_AT_BASE))
            out->mip = out->base;
        else if (f.MipAddress)
            out->mip = (const void *)(size_t)(((DWORD)f.MipAddress << 12) + mip_subpage);
        // MinMipLevel 1: the GPU never addresses level 0. A texture whose high-resolution level is not loaded is set up
        // this way, with the base left at its unrelocated offset (XGOffsetBaseTextureAddress with no base address), so
        // its base is no memory at all; texture_convert.h starts such a texture at level 1. That holds when the mips
        // live at the base address too (PORT_D3D_TEXTURE_ID_MIPS_AT_BASE: mip == base): level 0 there is still never
        // fetched, and converting it read stale texels into the 3D Spartan preview's armour and the emblems.
        if (f.MinMipLevel > 0 && out->mip != 0)
            out->base = 0;
    }
    return 1;
}
} // extern "C"
