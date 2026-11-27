#ifndef XGRAPHICS_TEXTURE_CODEC_H
#define XGRAPHICS_TEXTURE_CODEC_H

#include <stddef.h>

namespace XG_D3DXTex
{
struct D3DXCOLOR;

struct D3DX_BOX
{
    unsigned int Left, Top, Right, Bottom, Front, Back;
};

// March XGraphics CCodec constructor (83356cf0), ConvertFrom (83356fe0),
// and ColorKey (833573b8) establish the state preceding the pixel buffer.
class CCodec
{
public:
    virtual ~CCodec();
    virtual void Decode(unsigned int x, unsigned int y, D3DXCOLOR *colors);
    virtual void Encode(unsigned int x, unsigned int y, D3DXCOLOR *colors);
    virtual void QuantizeColorKey();

    int m_Format;
    unsigned long m_ColorSpace;
    int m_PackedPixels;
    int m_Gamma;
    int m_SkipGamma;
    unsigned long m_ColorKey;
    int m_ColorKeyConverted;
    unsigned char *m_pBits;
    float m_ColorKeyRGBA[4];
    const float *m_pQuantizationTable;
    float (*m_pPalette)[4];
    D3DX_BOX m_Box;
    unsigned long m_uSourceColorSpace;
    float (*m_pConversionRow)[4];
    float (*m_pDiffusionErrors)[4];
    unsigned int m_uRowPitch;
    unsigned int m_uSlicePitch;
    unsigned int m_uWidth;
    unsigned int m_uHeight;
    unsigned int m_uDepth;
    unsigned int m_uRowBytes;
    unsigned int m_uBytesPerPixel;
};

// March CBlt constructor (83208e90) initializes these two codec pointers;
// its destructor (83208ed0) deletes the destination before the source.
struct CBlt
{
    CCodec *m_pSrc;
    CCodec *m_pDest;
};

#if defined(_M_PPCBE) || defined(_XBOX)
typedef char texture_codec_size[(sizeof(void *) != 4 || (sizeof(CCodec) == 0x7c)) ? 1 : -1];
typedef char texture_codec_bits_offset[(sizeof(void *) != 4 || (offsetof(CCodec, m_pBits) == 0x20)) ? 1 : -1];
typedef char texture_codec_box_offset[(sizeof(void *) != 4 || (offsetof(CCodec, m_Box) == 0x3c)) ? 1 : -1];
typedef char texture_codec_row_pitch_offset[(sizeof(void *) != 4 || (offsetof(CCodec, m_uRowPitch) == 0x60)) ? 1 : -1];
typedef char texture_blt_size[(sizeof(void *) != 4 || (sizeof(CBlt) == 8)) ? 1 : -1];
#endif
}

#endif // XGRAPHICS_TEXTURE_CODEC_H
