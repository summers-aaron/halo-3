// Halo 3 PC port: clean-room Xbox 360 SDK headers (zero-XDK build).
// xgraphics.h: XG texture-layout helpers: resource header setup, tiling math, surface sizes.
#ifndef _XGRAPHICS_H_
#define _XGRAPHICS_H_

#include <xboxmath.h>
#include <PPCIntrinsics.h>

#ifdef __cplusplus
extern "C"
{
#endif

// XGTEXTURE_DESC::Flags
#define XGTDESC_TILED                               0x0001
#define XGTDESC_PACKED                              0x0002
#define XGTDESC_BORDERED                            0x0008

typedef struct _XGTEXTURE_DESC
{
    D3DRESOURCETYPE ResourceType;
    UINT Width, Height, Depth;
    D3DFORMAT Format;
    UINT RowPitch, SlicePitch, BitsPerPixel, WidthInBlocks, HeightInBlocks, DepthInBlocks, BytesPerBlock;
    INT ExpBias;
    DWORD Flags;
    D3DMULTISAMPLE_TYPE MultiSampleType;
} XGTEXTURE_DESC;

typedef struct _XGMIPTAIL_DESC
{
    D3DRESOURCETYPE ResourceType;
    UINT Width, Height, Depth;
    D3DFORMAT Format;
    UINT RowPitch, SlicePitch, BitsPerPixel, WidthInBlocks, HeightInBlocks, DepthInBlocks, BytesPerBlock;
    INT ExpBias;
    DWORD Flags;
    UINT BaseLevel, Size;
} XGMIPTAIL_DESC;

// The GPUTEXTUREFORMAT in the low bits of a D3DFORMAT.
__forceinline DWORD WINAPI XGGetGpuFormat(D3DFORMAT Format)
{
    return (DWORD) ((Format & D3DFORMAT_TEXTUREFORMAT_MASK) >> D3DFORMAT_TEXTUREFORMAT_SHIFT);
}

__forceinline BOOL WINAPI XGIsTiledFormat(D3DFORMAT Format)
{
    return (Format & D3DFORMAT_TILED_MASK) != 0;
}

// Bits per texel (per 4x4 block texel for the compressed formats), indexed by GPUTEXTUREFORMAT.
extern CONST __declspec(selectany) BYTE g_XGTextureFormatBitsPerPixel[] =
{
    1, 1, 8, 16, 16, 16, 32, 32, 8, 8, 16, 16, 16, 32, 32, 16,
    32, 32, 4, 8, 8, 64, 32, 32, 16, 32, 64, 16, 32, 64, 16, 32,
    64, 32, 64, 128, 32, 64, 128, 8, 16, 16, 32, 8, 8, 16, 16, 16,
    32, 8, 32, 4, 8, 8, 32, 32, 32, 96, 4, 4, 4, 4, 32, 32,
};

__inline UINT WINAPI XGBitsPerPixelFromGpuFormat(DWORD GpuFormat)
{
    return g_XGTextureFormatBitsPerPixel[GpuFormat];
}

// Value rounded up to a multiple of Multiple.
__inline UINT XGNextMultiple(UINT Value, UINT Multiple)
{
    return ((Value + Multiple - 1) / Multiple) * Multiple;
}

// EDRAM a render target needs, in EDRAM tiles: multisampling doubles the height (2x) and then the width (4x);
// the allocation is whole tiles of 80x16 fragments of 4 bytes, or 8 bytes for the 64-bit formats.
__inline UINT WINAPI XGSurfaceSize(UINT Width, UINT Height, D3DFORMAT Format, D3DMULTISAMPLE_TYPE MultiSample)
{
    GPUTEXTUREFORMAT GpuFormat = (GPUTEXTUREFORMAT) XGGetGpuFormat(Format);
    DWORD width = Width;
    DWORD height = Height;
    DWORD bytesPerFragment = 4;

    if (MultiSample >= D3DMULTISAMPLE_2_SAMPLES)
        height *= 2;
    if (MultiSample == D3DMULTISAMPLE_4_SAMPLES)
        width *= 2;

    width = XGNextMultiple(width, GPU_EDRAM_TILE_WIDTH_IN_FRAGMENTS);
    height = XGNextMultiple(height, GPU_EDRAM_TILE_HEIGHT_IN_FRAGMENTS);

    if (GpuFormat == GPUTEXTUREFORMAT_16_16_16_16_EDRAM || GpuFormat == GPUTEXTUREFORMAT_16_16_16_16_FLOAT ||
        GpuFormat == GPUTEXTUREFORMAT_32_32_FLOAT)
        bytesPerFragment = 8;

    return (UINT) (width * height * bytesPerFragment / GPU_EDRAM_TILE_SIZE);
}

// Hierarchical Z a depth surface needs, in hierarchical Z tiles (same multisample scaling, 32x16 granularity).
__inline UINT WINAPI XGHierarchicalZSize(UINT Width, UINT Height, D3DMULTISAMPLE_TYPE MultiSample)
{
    DWORD width = Width;
    DWORD height = Height;

    if (MultiSample >= D3DMULTISAMPLE_2_SAMPLES)
        height *= 2;
    if (MultiSample == D3DMULTISAMPLE_4_SAMPLES)
        width *= 2;

    width = XGNextMultiple(width, GPU_HIERARCHICAL_Z_TILE_WIDTH_IN_FRAGMENTS);
    height = XGNextMultiple(height, GPU_HIERARCHICAL_Z_TILE_HEIGHT_IN_FRAGMENTS);

    return width * height / GPU_HIERARCHICAL_Z_TILE_SIZE;
}

// log2 of a power-of-two texel pitch of at most 16 bytes.
__forceinline UINT WINAPI XGLog2LE16(UINT TexelPitch)
{
    return 31 - _CountLeadingZeros(TexelPitch);
}

// Offset, in texels (or blocks), of texel (x, y) of a tiled 2D surface Width texels wide: 32x32 macro tiles in row
// order, each a fixed interleave of 8x2-texel micro tiles with y bits 1..4 and x bits 3..4 swizzled into the
// address, all scaled by the texel size.
__inline UINT WINAPI XGAddress2DTiledOffset(UINT x, UINT y, UINT Width, UINT TexelPitch)
{
    UINT alignedWidth = (Width + 31) & ~31;
    UINT logBpp = XGLog2LE16(TexelPitch);
    UINT macro = ((x >> 5) + (y >> 5) * (alignedWidth >> 5)) << (logBpp + 7);
    UINT micro = ((x & 7) + ((y & 6) << 2)) << logBpp;
    UINT offset = macro + ((micro & ~15) << 1) + (micro & 15) + ((y & 8) << (3 + logBpp)) + ((y & 1) << 4);

    return (((offset & ~511) << 3) + ((offset & 448) << 2) + (offset & 63) +
            ((y & 16) << 7) + (((((y & 8) >> 2) + (x >> 3)) & 3) << 6)) >> logBpp;
}

// Header setup for resources whose memory the title manages (offsets are relocated later by XGOffset*Address).
#define XGHEADER_CONTIGUOUS_MIP_OFFSET              0xFFFFFFFF
#define XGHEADEREX_NONPACKED                        0x1
#define XGHEADEREX_BORDER                           0x2

VOID WINAPI XGGetTextureDesc(D3DBaseTexture* pTexture, UINT Level, XGTEXTURE_DESC* pDesc);
UINT WINAPI XGGetMipLevelOffset(D3DBaseTexture* pTexture, UINT ArrayIndex, UINT Level);
VOID WINAPI XGGetMipTailDesc(D3DBaseTexture* pTexture, XGMIPTAIL_DESC* pDesc);
UINT WINAPI XGGetMipTailBaseLevel(UINT Width, UINT Height, BOOL Border);
UINT WINAPI XGGetMipTailLevelOffset(UINT Width, UINT Height, UINT Depth, UINT Level, DWORD GpuFormat, BOOL Tiled, BOOL Border);
VOID WINAPI XGGetBlockDimensions(DWORD GpuFormat, UINT* pWidth, UINT* pHeight);
UINT WINAPI XGSetSurfaceHeader(UINT Width, UINT Height, D3DFORMAT Format, D3DMULTISAMPLE_TYPE MultiSampleType, CONST D3DSURFACE_PARAMETERS* pParameters, IDirect3DSurface9* pSurface, UINT* pHierarchicalZSize);
UINT WINAPI XGSetTextureHeader(UINT Width, UINT Height, UINT Levels, DWORD Usage, D3DFORMAT Format, D3DPOOL Pool, UINT BaseOffset, UINT MipOffset, UINT Pitch, IDirect3DTexture9* pTexture, UINT* pBaseSize, UINT* pMipSize);
UINT WINAPI XGSetTextureHeaderEx(UINT Width, UINT Height, UINT Levels, DWORD Usage, D3DFORMAT Format, INT ExpBias, DWORD Flags, UINT BaseOffset, UINT MipOffset, UINT Pitch, IDirect3DTexture9* pTexture, UINT* pBaseSize, UINT* pMipSize);
UINT WINAPI XGSetTextureHeaderPair(UINT BaseOffset, UINT MipOffset, UINT Width1, UINT Height1, UINT Levels1, DWORD Usage1, D3DFORMAT Format1, INT ExpBias1, DWORD Flags1, UINT Pitch1, IDirect3DTexture9* pTexture1, UINT Width2, UINT Height2, UINT Levels2, DWORD Usage2, D3DFORMAT Format2, INT ExpBias2, DWORD Flags2, UINT Pitch2, IDirect3DTexture9* pTexture2, UINT* pBaseSize, UINT* pMipSize);
UINT WINAPI XGSetLineTextureHeader(UINT Width, UINT Levels, DWORD Usage, D3DFORMAT Format, D3DPOOL Pool, UINT BaseOffset, UINT MipOffset, IDirect3DLineTexture9* pTexture, UINT* pBaseSize, UINT* pMipSize);
UINT WINAPI XGSetLineTextureHeaderEx(UINT Width, UINT Levels, DWORD Usage, D3DFORMAT Format, INT ExpBias, DWORD Flags, UINT BaseOffset, UINT MipOffset, IDirect3DLineTexture9* pTexture, UINT* pBaseSize, UINT* pMipSize);
UINT WINAPI XGSetCubeTextureHeader(UINT EdgeLength, UINT Levels, DWORD Usage, D3DFORMAT Format, D3DPOOL Pool, UINT BaseOffset, UINT MipOffset, IDirect3DCubeTexture9* pCubeTexture, UINT* pBaseSize, UINT* pMipSize);
UINT WINAPI XGSetCubeTextureHeaderEx(UINT EdgeLength, UINT Levels, DWORD Usage, D3DFORMAT Format, INT ExpBias, DWORD Flags, UINT BaseOffset, UINT MipOffset, IDirect3DCubeTexture9* pCubeTexture, UINT* pBaseSize, UINT* pMipSize);
UINT WINAPI XGSetCubeTextureHeaderPair(UINT BaseOffset, UINT MipOffset, UINT EdgeLength1, UINT Levels1, DWORD Usage1, D3DFORMAT Format1, INT ExpBias1, DWORD Flags1, IDirect3DTexture9* pCubeTexture1, UINT EdgeLength2, UINT Levels2, DWORD Usage2, D3DFORMAT Format2, INT ExpBias2, DWORD Flags2, IDirect3DTexture9* pCubeTexture2, UINT* pBaseSize, UINT* pMipSize);
UINT WINAPI XGSetArrayTextureHeader(UINT Width, UINT Height, UINT ArraySize, UINT Levels, DWORD Usage, D3DFORMAT Format, D3DPOOL Pool, UINT BaseOffset, UINT MipOffset, UINT Pitch, IDirect3DArrayTexture9* pArrayTexture, UINT* pBaseSize, UINT* pMipSize);
UINT WINAPI XGSetArrayTextureHeaderEx(UINT Width, UINT Height, UINT ArraySize, UINT Levels, DWORD Usage, D3DFORMAT Format, INT ExpBias, DWORD Flags, UINT BaseOffset, UINT MipOffset, UINT Pitch, IDirect3DArrayTexture9* pArrayTexture, UINT* pBaseSize, UINT* pMipSize);
UINT WINAPI XGSetArrayTextureHeaderPair(UINT BaseOffset, UINT MipOffset, UINT Width1, UINT Height1, UINT ArraySize1, UINT Levels1, DWORD Usage1, D3DFORMAT Format1, INT ExpBias1, DWORD Flags1, UINT Pitch1, IDirect3DTexture9* pArrayTexture1, UINT Width2, UINT Height2, UINT ArraySize2, UINT Levels2, DWORD Usage2, D3DFORMAT Format2, INT ExpBias2, DWORD Flags2, UINT Pitch2, IDirect3DTexture9* pArrayTexture2, UINT* pBaseSize, UINT* pMipSize);
UINT WINAPI XGSetVolumeTextureHeader(UINT Width, UINT Height, UINT Depth, UINT Levels, DWORD Usage, D3DFORMAT Format, D3DPOOL Pool, UINT BaseOffset, UINT MipOffset, IDirect3DVolumeTexture9* pVolumeTexture, UINT* pBaseSize, UINT* pMipSize);
UINT WINAPI XGSetVolumeTextureHeaderEx(UINT Width, UINT Height, UINT Depth, UINT Levels, DWORD Usage, D3DFORMAT Format, INT ExpBias, DWORD Flags, UINT BaseOffset, UINT MipOffset, IDirect3DVolumeTexture9* pVolumeTexture, UINT* pBaseSize, UINT* pMipSize);
VOID WINAPI XGSetVertexBufferHeader(UINT Length, DWORD Usage, D3DPOOL Pool, UINT Offset, IDirect3DVertexBuffer9* pVertexBuffer);
VOID WINAPI XGSetIndexBufferHeader(UINT Length, DWORD Usage, D3DFORMAT Format, D3DPOOL Pool, UINT Offset, IDirect3DIndexBuffer9* pIndexBuffer);
VOID WINAPI XGSetConstantBufferHeader(UINT VectorCount, DWORD Usage, UINT Offset, IDirect3DConstantBuffer9* pConstantBuffer);
VOID WINAPI XGOffsetSurfaceAddress(IDirect3DSurface9* pSurface, INT SurfaceOffset, INT HierarchicalZOffset);
VOID WINAPI XGOffsetResourceAddress(IDirect3DResource9* pResource, VOID* pBaseAddress);
VOID WINAPI XGOffsetBaseTextureAddress(IDirect3DBaseTexture9* pTexture, VOID* pBaseAddress, VOID* pMipAddress);
VOID WINAPI XGRegisterPixelShader(IDirect3DPixelShader9* pShader, VOID* pPhysicalPart);
VOID WINAPI XGRegisterVertexShader(IDirect3DVertexShader9* pShader, VOID* pPhysicalPart);

#ifdef __cplusplus
}
#endif

#endif // _XGRAPHICS_H_
