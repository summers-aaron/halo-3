#ifndef BITMAPS_S3TC_S3TC_H
#define BITMAPS_S3TC_S3TC_H

#include <stddef.h>

namespace HaloS3TC
{
struct S3TC_COLOR
{
    union
    {
        struct { unsigned char blue, green, red, alpha; };
        unsigned char c[4];
    };
};

struct S3TCBlockRGB
{
    unsigned short color0;
    unsigned short color1;
    unsigned long bits;
};

struct S3TCBlockAlpha3
{
    unsigned char alpha0;
    unsigned char alpha1;
    unsigned char indices[6];
    S3TCBlockRGB rgb;
};

// DXT3's explicit alpha: four rows of four four-bit values, followed by RGB.
struct S3TCBlockAlpha4
{
    unsigned short alpha[4];
    S3TCBlockRGB rgb;
};

typedef char s3tc_color_size[(sizeof(S3TC_COLOR) == 4) ? 1 : -1];
#if !defined(_WIN64) && !defined(__LP64__)
typedef char s3tc_rgb_size[(sizeof(S3TCBlockRGB) == 8) ? 1 : -1];
typedef char s3tc_alpha3_size[(sizeof(S3TCBlockAlpha3) == 16) ? 1 : -1];
typedef char s3tc_alpha3_rgb_offset[(offsetof(S3TCBlockAlpha3, rgb) == 8) ? 1 : -1];
#endif

void DecodeBlockRGB__single_pixel(const S3TCBlockRGB *block, S3TC_COLOR *color,
    short u, short v, bool allow_color_key);
void DecodeBlockAlpha3__single_pixel(const S3TCBlockAlpha3 *source, S3TC_COLOR *color, short u, short v);
long decode_xenon_dxt5a_block_single_pixel(unsigned char *const block, long x, long y);
}

#endif // BITMAPS_S3TC_S3TC_H
