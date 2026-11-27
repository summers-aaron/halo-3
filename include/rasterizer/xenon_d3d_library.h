#ifndef RASTERIZER_XENON_D3D_LIBRARY_H
#define RASTERIZER_XENON_D3D_LIBRARY_H

#include <xtl.h>

// XDK xgraphics.h's complete output descriptors and out-of-line entry points.
// The game owns XGAddress2DTiledOffset in xenon_d3d_library.cpp; including the
// SDK's inline definition would introduce a second body for that entry point.
typedef struct _XGTEXTURE_DESC
{
    D3DRESOURCETYPE ResourceType;
    unsigned int Width, Height, Depth;
    D3DFORMAT Format;
    unsigned int RowPitch, SlicePitch, BitsPerPixel;
    unsigned int WidthInBlocks, HeightInBlocks, DepthInBlocks, BytesPerBlock;
    int ExpBias;
    unsigned long Flags;
    D3DMULTISAMPLE_TYPE MultiSampleType;
} XGTEXTURE_DESC;

typedef struct _XGMIPTAIL_DESC
{
    D3DRESOURCETYPE ResourceType;
    unsigned int Width, Height, Depth;
    D3DFORMAT Format;
    unsigned int RowPitch, SlicePitch, BitsPerPixel;
    unsigned int WidthInBlocks, HeightInBlocks, DepthInBlocks, BytesPerBlock;
    int ExpBias;
    unsigned long Flags;
    unsigned int BaseLevel, Size;
} XGMIPTAIL_DESC;

extern "C"
{
    void __stdcall XGGetTextureDesc(D3DBaseTexture *texture, unsigned int level, XGTEXTURE_DESC *description);
    unsigned int __stdcall XGGetMipLevelOffset(D3DBaseTexture *texture, unsigned int array_index, unsigned int level);
    void __stdcall XGGetBlockDimensions(unsigned long format, unsigned int *width, unsigned int *height);
    unsigned int __stdcall XGGetMipTailBaseLevel(unsigned int width, unsigned int height, int border);
    void __stdcall XGGetMipTailDesc(D3DBaseTexture *texture, XGMIPTAIL_DESC *description);
    unsigned int __stdcall XGGetMipTailLevelOffset(unsigned int width, unsigned int height, unsigned int depth,
        unsigned int level, unsigned long format, int tiled, int border);
    unsigned int __stdcall XGAddress2DTiledOffset(unsigned int x, unsigned int y, unsigned int width, unsigned int texel_pitch);
}

#if defined(_M_PPCBE) || defined(_XBOX)
typedef char xenon_texture_description_size[(sizeof(XGTEXTURE_DESC) == 60) ? 1 : -1];
typedef char xenon_mip_tail_description_size[(sizeof(XGMIPTAIL_DESC) == 64) ? 1 : -1];
#endif

#endif // RASTERIZER_XENON_D3D_LIBRARY_H
