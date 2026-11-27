// Halo 3 PC port: clean-room Xbox 360 SDK headers (zero-XDK build).
// d3dx9tex.h: D3DX image file loading and saving.
#include "d3dx9.h"

#ifndef __D3DX9TEX_H__
#define __D3DX9TEX_H__

// Filter flags for the load functions.
#define D3DX_FILTER_NONE                            (1 << 0)
#define D3DX_FILTER_POINT                           (2 << 0)
#define D3DX_FILTER_TRIANGLE                        (4 << 0)

typedef enum _D3DXIMAGE_FILEFORMAT
{
    D3DXIFF_BMP = 0, D3DXIFF_JPG = 1, D3DXIFF_TGA = 2, D3DXIFF_PNG = 3, D3DXIFF_DDS = 4, D3DXIFF_PPM = 5,
    D3DXIFF_DIB = 6, D3DXIFF_HDR = 7, D3DXIFF_PFM = 8, D3DXIFF_FORCE_DWORD = 0x7FFFFFFF,
} D3DXIMAGE_FILEFORMAT;

// What an image file holds.
typedef struct _D3DXIMAGE_INFO
{
    UINT Width, Height, Depth, MipLevels;
    D3DFORMAT Format;
    D3DRESOURCETYPE ResourceType;
    D3DXIMAGE_FILEFORMAT ImageFileFormat;
} D3DXIMAGE_INFO;

#ifndef _PALETTEENTRY_DEFINED
#define _PALETTEENTRY_DEFINED
typedef struct tagPALETTEENTRY
{
    BYTE peRed, peGreen, peBlue, peFlags;
} PALETTEENTRY, *PPALETTEENTRY, FAR *LPPALETTEENTRY;
#endif

#ifdef __cplusplus
extern "C" {
#endif

HRESULT WINAPI D3DXGetImageInfoFromFileInMemory(LPCVOID pSrcData, UINT SrcDataSize, D3DXIMAGE_INFO* pSrcInfo);
HRESULT WINAPI D3DXLoadSurfaceFromFileInMemory(LPDIRECT3DSURFACE9 pDestSurface, CONST PALETTEENTRY* pDestPalette,
    CONST RECT* pDestRect, LPCVOID pSrcData, UINT SrcDataSize, CONST RECT* pSrcRect, DWORD Filter, D3DCOLOR ColorKey,
    D3DXIMAGE_INFO* pSrcInfo);
HRESULT WINAPI D3DXSaveSurfaceToFileInMemory(LPD3DXBUFFER* ppDestBuf, D3DXIMAGE_FILEFORMAT DestFormat,
    LPDIRECT3DSURFACE9 pSrcSurface, CONST PALETTEENTRY* pSrcPalette, CONST RECT* pSrcRect);

#ifdef __cplusplus
}
#endif

#endif // __D3DX9TEX_H__
