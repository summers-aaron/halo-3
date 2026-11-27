// xgraphics/cblt.obj: from the records 0x5f8cd0 0x5f8d38
// @flags /TP /O2 /Os
// @address 0x5f8cd0 blt_dispose
// @address 0x5f8d38 blt_copy_dxt_blocks
#include <xtl.h>
#include <string.h>
#include "xgraphics/texture_codec.h"

using XG_D3DXTex::CCodec;
using XG_D3DXTex::CBlt;

extern "C" void blt_dispose(CBlt *blt)
{
    CCodec *p;

    if ((p = blt->m_pDest) != 0)
        delete p;
    if ((p = blt->m_pSrc) != 0)
        delete p;
}

extern "C" long blt_copy_dxt_blocks(CBlt *blt)
{
    unsigned int uBlockSize;

    if ((blt->m_pSrc->m_Box.Left | blt->m_pSrc->m_Box.Top | blt->m_pSrc->m_Box.Right | blt->m_pSrc->m_Box.Bottom) & 3)
        return E_FAIL;
    if ((blt->m_pDest->m_Box.Left | blt->m_pDest->m_Box.Top | blt->m_pDest->m_Box.Right | blt->m_pDest->m_Box.Bottom) & 3)
        return E_FAIL;

    switch (MAKELINFMT(blt->m_pDest->m_Format))
    {
    case D3DFMT_LIN_DXT1:
        uBlockSize = 8;
        break;

    case D3DFMT_LIN_DXT2:
        uBlockSize = 16;
        break;

    case D3DFMT_LIN_DXT4:
        uBlockSize = 16;
        break;

    case D3DFMT_LIN_DXT3A:
        uBlockSize = 8;
        break;

    case D3DFMT_LIN_DXT5A:
        uBlockSize = 8;
        break;

    default:
        return E_FAIL;
    }

    unsigned char *pSrc = blt->m_pSrc->m_pBits + (blt->m_pSrc->m_Box.Top >> 2) * blt->m_pSrc->m_uRowPitch
                          + blt->m_pSrc->m_uSlicePitch * blt->m_pSrc->m_Box.Front
                          + (blt->m_pSrc->m_Box.Left >> 2) * uBlockSize;
    unsigned char *pDest = blt->m_pDest->m_pBits + (blt->m_pDest->m_Box.Top >> 2) * blt->m_pDest->m_uRowPitch
                           + blt->m_pDest->m_uSlicePitch * blt->m_pDest->m_Box.Front
                           + (blt->m_pDest->m_Box.Left >> 2) * uBlockSize;
    unsigned int uRowBytes = (blt->m_pDest->m_uWidth >> 2) * uBlockSize;

    for (unsigned int z = 0; z < blt->m_pDest->m_uDepth; z++)
    {
        unsigned char *pD = pDest;
        unsigned char *pS = pSrc;

        for (unsigned int y = 0; y < blt->m_pDest->m_uHeight; y += 4)
        {
            memcpy(pD, pS, uRowBytes);
            pS += blt->m_pSrc->m_uRowPitch;
            pD += blt->m_pDest->m_uRowPitch;
        }

        pSrc += blt->m_pSrc->m_uSlicePitch;
        pDest += blt->m_pDest->m_uSlicePitch;
    }

    return 0;
}
