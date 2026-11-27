// blamlibxbox/s3tc.obj: from the records 0x1619c0 0x161c58 0x2feac8
// @flags /TP /O2 /Oc
// @address 0x2feac8 ?decode_xenon_dxt5a_block_single_pixel@HaloS3TC@@YAJQAEJJ@Z
#include <ppcintrinsics.h>
#include <stdlib.h>
#include "bitmaps/s3tc/s3tc.h"
#include <string.h>
#include "memory/data_inline.h"

using HaloS3TC::S3TC_COLOR;
using HaloS3TC::S3TCBlockRGB;
using HaloS3TC::S3TCBlockAlpha3;
static __forceinline unsigned __int64 swap_bits_mask_shift(unsigned __int64 value, unsigned __int64 mask, long shift)
{
    unsigned __int64 difference = ((value >> shift) ^ value) & mask;
    return value ^ (difference | (difference << shift));
}

long HaloS3TC::decode_xenon_dxt5a_block_single_pixel(unsigned char *const block, long x, long y)
{
    union
    {
        unsigned __int64 packed;
        unsigned long words[2];
    } bits;
    bits.packed = *(unsigned __int64 *)block;
    // match: 0x2feac8: preserve word-based byte swapping; natural shifts changed this tuned body in check 3.
    unsigned __int64 data = ((unsigned __int64)_byteswap_ulong(bits.words[1]) << 32) | _byteswap_ulong(bits.words[0]);
    data = swap_bits_mask_shift(data, 0x00FF00FF00FF00FFui64, 8);

    long alpha0 = (long)(data & 0xFF);
    long alpha1 = (long)((data >> 8) & 0xFF);
    long range = alpha1 - alpha0;
    long index = (long)((data >> (((y * 4) + x) * 3 + 16)) & 7);
    long interpolated7 = alpha0 + ((index - 1) * range + 3) / 7;
    long interpolated5 = alpha0 + ((index - 1) * range + 2) / 5;
    long result = interpolated5;
    if (index == 6)
        result = 0;
    else if (index == 7)
        result = 255;
    long alpha = range < 0 ? interpolated7 : result;
    if (index == 0)
        alpha = alpha0;
    else if (index == 1)
        alpha = alpha1;
    return alpha;
}
static __forceinline void decode_color(const unsigned short *packed, S3TC_COLOR *color) {
 S3TC_COLOR result;
 unsigned short value = *packed;
 result.c[0] = (unsigned char)value;
 value >>= 5;
 result.c[1] = (unsigned char)value;
 value >>= 6;
 result.c[2] = (unsigned char)value;
 result.c[0] <<= 3;
 result.c[1] <<= 2;
 result.c[2] <<= 3;
 result.c[0] |= result.c[0] >> 5;
 result.c[1] |= result.c[1] >> 6;
 result.c[2] |= result.c[2] >> 5;
 *color = result;
}
// @address 0x161c58 ?DecodeBlockRGB__single_pixel@HaloS3TC@@YAXPBUS3TCBlockRGB@1@PAUS3TC_COLOR@1@FF_N@Z
void HaloS3TC::DecodeBlockRGB__single_pixel(const S3TCBlockRGB *block, S3TC_COLOR *color, short u, short v, bool allow_color_key) {
 if (block == 0) {
  memset(color, 0, sizeof(S3TC_COLOR));
 } else {
  bool transparent = allow_color_key && block->color0 <= block->color1;
  S3TC_COLOR colors[4];
  decode_color(&block->color0, &colors[0]);
  decode_color(&block->color1, &colors[1]);
  colors[2].c[3] = 255;
  colors[1].c[3] = 255;
  colors[0].c[3] = 255;
  if (!transparent) {
   for (long i = 0; i < 3; i++) {
    short a = colors[0].c[i];
    short b = colors[1].c[i];
    colors[2].c[i] = (unsigned char)((a * 2 + b + 1) / 3);
    colors[3].c[i] = (unsigned char)((a + b * 2 + 1) / 3);
   }
   colors[3].c[3] = 255;
  } else {
   for (long i = 0; i < 3; i++) {
    short a = colors[0].c[i];
    short b = colors[1].c[i];
    colors[2].c[i] = (unsigned char)((a + b) / 2);
    colors[3].c[i] = 0;
   }
   colors[3].c[3] = 0;
  }
  v ^= 2;
  *color = colors[(block->bits >> (u * 2 + v * 8)) & 3];
 }
}
// @address 0x1619c0 ?DecodeBlockAlpha3__single_pixel@HaloS3TC@@YAXPBUS3TCBlockAlpha3@1@PAUS3TC_COLOR@1@FF@Z
void HaloS3TC::DecodeBlockAlpha3__single_pixel(const S3TCBlockAlpha3 *source,S3TC_COLOR *color,short u,short v) {
 S3TCBlockAlpha3 block;
 memcpy(&block,source,sizeof(block));
 byte_swap_memory(&block,4,-2);
 const S3TCBlockAlpha3 *b=&block;
 HaloS3TC::DecodeBlockRGB__single_pixel(&b->rgb,color,u,v,false);
 unsigned short alpha[8];
 alpha[0]=b->alpha0;
 alpha[1]=b->alpha1;
 if(alpha[0]>alpha[1]) {
  alpha[2]=(6*alpha[0]+alpha[1])/7;
  alpha[3]=(5*alpha[0]+2*alpha[1])/7;
  alpha[4]=(4*alpha[0]+3*alpha[1])/7;
  alpha[5]=(3*alpha[0]+4*alpha[1])/7;
  alpha[6]=(2*alpha[0]+5*alpha[1])/7;
  alpha[7]=(alpha[0]+6*alpha[1])/7;
 } else {
  alpha[2]=(4*alpha[0]+alpha[1])/5;
  alpha[3]=(3*alpha[0]+2*alpha[1])/5;
  alpha[4]=(2*alpha[0]+3*alpha[1])/5;
  alpha[5]=(alpha[0]+4*alpha[1])/5;
  alpha[6]=0;
  alpha[7]=255;
 }
 if(v<2) {
  unsigned long indices=b->indices[0]|(b->indices[1]<<8)|(b->indices[2]<<16);
  color->c[3]=(unsigned char)alpha[(indices>>((u+v*4)*3))&7];
 } else {
  unsigned long indices=b->indices[3]|(b->indices[4]<<8)|(b->indices[5]<<16);
  color->c[3]=(unsigned char)alpha[(indices>>((u+(v-2)*4)*3))&7];
 }
}
