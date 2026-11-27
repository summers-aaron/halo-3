// blamlibxbox/bitmap_xenon_cache_builder.obj: from the records 0x15feb8
// @flags /TP /O2 /Oc
#include <xtl.h>
#include "rasterizer/xenon_d3d_library.h"
#include "bitmaps/bitmap_data.h"
#include "bitmaps/bitmap_formats.h"
#include "math/real_math_types.h"
#include <math.h>
#include <string.h>
#include "bitmaps/bitmaps.h"
#include "bitmaps/bitmap_xenon_cache_builder.h"
#include "bitmaps/s3tc/s3tc.h"
using HaloS3TC::S3TC_COLOR;
using HaloS3TC::S3TCBlockRGB;
using HaloS3TC::S3TCBlockAlpha3;


static __forceinline long bitmap_xenon_cache_builder_max(long a,long b) { return a>b?a:b; }
static __forceinline long bitmap_xenon_cache_builder_pin(long value,long lower,long upper) {
    return bitmap_xenon_cache_builder_max(value,lower)<upper ? bitmap_xenon_cache_builder_max(value,lower):upper;
}
static __forceinline unsigned char *bitmap_xenon_cache_builder_mip_buffer(D3DBaseTexture *texture,long slice,long level) {
    unsigned long offset=XGGetMipLevelOffset(texture,slice,level);
    ULONG_PTR address;
#ifdef HALO3_64BIT
    if (level && (texture->Format.MipAddress)) address = ((ULONG_PTR)texture->Format.MipAddress) << 12;
    else address = ((ULONG_PTR)texture->Format.BaseAddress) << 12;
#else
    if (level && (texture->Format.MipAddress)) address=texture->Format.MipAddress << 12;
    else address=texture->Format.BaseAddress << 12;
#endif
    address+=offset;
    return (unsigned char *)GPU_CONVERT_CPU_TO_CPU_CACHED_READONLY_ADDRESS((void *)address);
}
static __forceinline int bitmap_xenon_cache_builder_is_border_texture(const D3DBaseTexture *texture) {
    return texture->Format.BorderSize;
}
static __forceinline unsigned long bitmap_xenon_cache_builder_tail_base(const bitmap_data *bitmap,D3DBaseTexture *texture) {
    return texture->Format.PackedMips
        ? XGGetMipTailBaseLevel(bitmap->width,bitmap->height,bitmap_xenon_cache_builder_is_border_texture(texture))
        : ~0UL;
}
static __forceinline unsigned long bitmap_xenon_cache_builder_tiled_offset(
    unsigned long x, unsigned long y, unsigned long pitch, const XGTEXTURE_DESC &desc)
{
    unsigned long bytes_per_block=desc.BytesPerBlock;
    return XGAddress2DTiledOffset(x,y,pitch,bytes_per_block)*bytes_per_block;
}
static __forceinline unsigned long bitmap_xenon_cache_builder_pixel_offset(const bitmap_data *bitmap,
    D3DBaseTexture *texture, long level, unsigned long format, unsigned long x, unsigned long y,
    const unsigned int &block_width, const unsigned int &block_height, const XGTEXTURE_DESC &desc,
    unsigned char *&buffer, XGMIPTAIL_DESC &tail, bool tiled)
{
    unsigned long offset;
    if (bitmap_xenon_cache_builder_tail_base(bitmap,texture) <= (unsigned long)level) {
        XGGetMipTailDesc(texture,&tail);
        unsigned long tail_offset=XGGetMipTailLevelOffset(bitmap->width,bitmap->height,bitmap->depth,
            level,format,1,bitmap_xenon_cache_builder_is_border_texture(texture));
        buffer-=tail_offset;
        unsigned long x_offset=tail_offset/tail.BytesPerBlock%tail.WidthInBlocks;
        unsigned long y_offset=tail_offset/tail.BytesPerBlock/tail.WidthInBlocks;
        unsigned long x_block=x/block_width;
        unsigned long y_block=y/block_height;
        unsigned long bx=x_offset+x_block;
        unsigned long by=y_offset+y_block;
        if (tiled) offset=bitmap_xenon_cache_builder_tiled_offset(bx,by,tail.RowPitch/tail.BytesPerBlock,desc);
        else offset=by*desc.RowPitch+bx*desc.BytesPerBlock;
    } else {
        if (tiled) {
            unsigned long bytes_per_block=desc.BytesPerBlock;
            offset=XGAddress2DTiledOffset(x/block_width,y/block_height,desc.RowPitch/bytes_per_block,bytes_per_block)*bytes_per_block;
        }
        else offset=(y/block_height)*desc.RowPitch+(x/block_width)*desc.BytesPerBlock;
    }
    return offset;
}
static __forceinline void bitmap_xenon_cache_builder_decode_block_alpha4(const unsigned char *block,S3TC_COLOR *color,short u,short v) {
    HaloS3TC::DecodeBlockRGB__single_pixel(&((const HaloS3TC::S3TCBlockAlpha4 *)block)->rgb,color,u,v,false);
    unsigned char alpha=(unsigned char)((((const HaloS3TC::S3TCBlockAlpha4 *)block)->alpha[v]>>(unsigned short)(u*4))&15);
    color->alpha=(unsigned char)((alpha<<4)|alpha);
}
#if !defined(_XBOX) || defined(__clang__)
// The decoders below read texel memory with the PPC's big-endian loads (the u16 / u32 fields of the S3TC blocks,
// the 16- and 32-bit texel formats). A little-endian host copies the texel or block and byte-swaps each field the
// code loads wider than a byte, so its native loads return what the Xbox's did. Byte texels and the DXT5 alpha half
// (which DecodeBlockAlpha3__single_pixel swaps itself as 16-bit words) stay as stored.
static void bitmap_xenon_swap16(unsigned char *p, long count) {
    for (long i = 0; i < count; i++, p += 2) { unsigned char t = p[0]; p[0] = p[1]; p[1] = t; }
}
static void bitmap_xenon_swap32(unsigned char *p, long count) {
    for (long i = 0; i < count; i++, p += 4) { unsigned char t = p[0]; p[0] = p[3]; p[3] = t; t = p[1]; p[1] = p[2]; p[2] = t; }
}
static void bitmap_xenon_rgb_block_to_host(unsigned char *block) {
    bitmap_xenon_swap16(block, 2); // color0, color1
    bitmap_xenon_swap32(block + 4, 1); // bits
}
static void bitmap_xenon_element_to_host(long format, const unsigned char *source, unsigned long bytes, unsigned char (&element)[16]) {
    memset(element, 0, sizeof(element));
    memcpy(element, source, bytes < sizeof(element) ? bytes : sizeof(element)); // one texel or block (XGTEXTURE_DESC::BytesPerBlock)
    switch (format) {
    case _bitmap_format_a8y8: case _bitmap_format_r5g6b5: case _bitmap_format_a1r5g5b5: case _bitmap_format_a4r4g4b4:
    case _bitmap_format_g8b8:
        bitmap_xenon_swap16(element, 1); break;
    case _bitmap_format_x8r8g8b8: case _bitmap_format_a8r8g8b8:
        bitmap_xenon_swap32(element, 1); break;
    case _bitmap_format_rgbfp32: bitmap_xenon_swap32(element, 3); break;
    case _bitmap_format_abgrfp32: bitmap_xenon_swap32(element, 4); break;
    case _bitmap_format_abgrfp16: bitmap_xenon_swap16(element, 4); break;
    case _bitmap_format_dxt1: bitmap_xenon_rgb_block_to_host(element); break;
    case _bitmap_format_dxt3: bitmap_xenon_swap16(element, 4); bitmap_xenon_rgb_block_to_host(element + 8); break;
    case _bitmap_format_dxt5: bitmap_xenon_rgb_block_to_host(element + 8); break;
    case _bitmap_format_dxt5a: case _bitmap_format_dxt5a_alpha: case _bitmap_format_dxt5a_mono:
        bitmap_xenon_swap32(element, 2); break;
    case _bitmap_format_dxn_mono_alpha: bitmap_xenon_swap32(element, 4); break;
    default: break; // byte texels (a8, y8, ay8, v8u8, q8w8v8u8) and formats the decoder does not read
    }
}
#endif
// @address 0x15feb8 ?bitmap_2d_xenon_get_real_pixel@@YA?ATreal_argb_color@@PBUbitmap_data@@PAXPBTreal_point2d@@JJ_N@Z
real_argb_color bitmap_2d_xenon_get_real_pixel(const bitmap_data *bitmap,void *texture_data,
    const real_point2d *point,long slice,long level,bool clamp)
{
    real_argb_color result;
    result.alpha=1.0f;
    result.red=0.0f; result.green=0.0f; result.blue=0.0f;
    D3DBaseTexture *texture=(D3DBaseTexture *)texture_data;
    if (bitmap->type==1) return result;
    if (bitmap->type==0 && slice!=0) return result;
    if (bitmap->type==2 && (slice<0 || slice>=6)) return result;
    if (bitmap->type==3 && (slice<0 || slice>=bitmap->depth)) return result;
    if (!((texture->Format.BaseAddress) || (texture->Format.MipAddress))) return result;
    {
        level=bitmap_xenon_cache_builder_pin(level,texture->Format.MinMipLevel,texture->Format.MaxMipLevel);
        long width=bitmap_xenon_cache_builder_max(bitmap->width>>(short)level,1);
        long height=bitmap_xenon_cache_builder_max(bitmap->height>>(short)level,1);
        long x=(long)(float)floor(point->x*(float)width);
        long y=(long)(float)floor(point->y*(float)height);
        if (clamp) {
            x=bitmap_xenon_cache_builder_max(x,0)<width-1 ? bitmap_xenon_cache_builder_max(x,0):width-1;
            y=bitmap_xenon_cache_builder_max(y,0)<height-1 ? bitmap_xenon_cache_builder_max(y,0):height-1;
        } else {
            x-=(long)((float)floor((float)x/(float)width)*(float)width);
            y-=(long)((float)floor((float)y/(float)height)*(float)height);
        }
        XGTEXTURE_DESC desc;
        XGGetTextureDesc(texture,level,&desc);
        unsigned char *pixel=bitmap_xenon_cache_builder_mip_buffer(texture,slice,level);
        unsigned long format=desc.Format & 63;
        unsigned int block_width,block_height;
        XGGetBlockDimensions(format,&block_width,&block_height);
        unsigned long offset;
        XGMIPTAIL_DESC tail;
        if (bitmap->runtime_flags & 8)
            offset=bitmap_xenon_cache_builder_pixel_offset(bitmap,texture,level,format,x,y,block_width,block_height,desc,pixel,tail,true);
        else
            offset=bitmap_xenon_cache_builder_pixel_offset(bitmap,texture,level,format,x,y,block_width,block_height,desc,pixel,tail,false);
        pixel+=offset;
#if !defined(_XBOX) || defined(__clang__)
        unsigned char host_element[16];
        bitmap_xenon_element_to_host(bitmap->format, pixel, desc.BytesPerBlock, host_element);
        pixel=host_element;
#endif
        switch (bitmap->format) {
        case _bitmap_format_a8: result.alpha=(float)*pixel*(1.0f/255.0f); result.red=result.green=result.blue=0.0f; break;
        case _bitmap_format_y8: result.alpha=1.0f; result.blue=result.green=result.red=(float)*pixel*(1.0f/255.0f); break;
        case _bitmap_format_ay8: result.alpha=result.blue=result.green=result.red=(float)*pixel*(1.0f/255.0f); break;
        case _bitmap_format_a8y8: {
            unsigned short value=*(const unsigned short *)pixel;
            result.blue=result.green=result.red=(float)(value&255)*(1.0f/255.0f);
            result.alpha=(float)(unsigned short)(value>>8)*(1.0f/255.0f); break;
        }
        case _bitmap_format_r5g6b5: {
            unsigned short value=*(const unsigned short *)pixel;
            result.alpha=1.0f;
            result.red=(float)(unsigned short)(value>>11)*(1.0f/31.0f);
            result.blue=(float)(value&31)*(1.0f/31.0f);
            result.green=(float)(unsigned short)((value>>5)&63)*0.015873017f; break;
        }
        case _bitmap_format_a1r5g5b5: {
            unsigned short value=*(const unsigned short *)pixel;
            result.alpha=(float)(unsigned short)(value>>15);
            result.red=(float)(unsigned short)((value>>10)&31)*(1.0f/31.0f);
            result.green=(float)(unsigned short)((value>>5)&31)*(1.0f/31.0f);
            result.blue=(float)(value&31)*(1.0f/31.0f); break;
        }
        case _bitmap_format_a4r4g4b4: {
            unsigned short value=*(const unsigned short *)pixel;
            result.alpha=(float)(unsigned short)(value>>12)*(1.0f/15.0f);
            result.red=(float)(unsigned short)((value>>8)&15)*(1.0f/15.0f);
            result.green=(float)(unsigned short)((value>>4)&15)*(1.0f/15.0f);
            result.blue=(float)(value&15)*(1.0f/15.0f); break;
        }
        case _bitmap_format_x8r8g8b8: {
            unsigned long value=*(const unsigned long *)pixel;
            result.alpha=1.0f;
            result.red=(float)((value>>16)&255)*(1.0f/255.0f);
            result.green=(float)((value>>8)&255)*(1.0f/255.0f);
            result.blue=(float)(value&255)*(1.0f/255.0f); break;
        }
        case _bitmap_format_a8r8g8b8: {
            unsigned long value=*(const unsigned long *)pixel;
            result.red=(float)((value>>16)&255)*(1.0f/255.0f);
            result.green=(float)((value>>8)&255)*(1.0f/255.0f);
            result.blue=(float)(value&255)*(1.0f/255.0f);
            result.alpha=(float)(value>>24)*(1.0f/255.0f); break;
        }
        case _bitmap_format_g8b8: {
            unsigned short value=*(const unsigned short *)pixel;
            result.alpha=1.0f;
            result.blue=(float)(value&255)*(1.0f/255.0f);
            result.green=(float)(unsigned short)(value>>8)*(1.0f/255.0f);
            result.red=0.0f; break;
        }
        case _bitmap_format_q8w8v8u8: {
            const signed char *value=(const signed char *)pixel;
            result.alpha=(float)value[0]*(1.0f/127.0f);
            result.blue=(float)value[1]*(1.0f/127.0f);
            result.green=(float)value[2]*(1.0f/127.0f);
            result.red=(float)value[3]*(1.0f/127.0f); break;
        }
        case _bitmap_format_v8u8: {
            const signed char *value=(const signed char *)pixel;
            result.green=(float)value[1]*(1.0f/127.0f);
            result.red=(float)value[0]*(1.0f/127.0f); break;
        }
        case _bitmap_format_rgbfp32: {
            const float *value=(const float *)pixel;
            result.red=value[0]; result.green=value[1]; result.blue=value[2]; break;
        }
        case _bitmap_format_abgrfp32: {
            const float *value=(const float *)pixel;
            result.alpha=value[0]; result.blue=value[1]; result.green=value[2]; result.red=value[3]; break;
        }
        case _bitmap_format_abgrfp16: {
            const unsigned short *value=(const unsigned short *)pixel;
            result.alpha=half_to_real_all_finite(value[0]); result.blue=half_to_real_all_finite(value[1]);
            result.green=half_to_real_all_finite(value[2]); result.red=half_to_real_all_finite(value[3]); break;
        }
        case _bitmap_format_dxt1:
        case _bitmap_format_dxt3:
        case _bitmap_format_dxt5:
        case _bitmap_format_dxt5a:
        case _bitmap_format_dxt5a_alpha:
        case _bitmap_format_dxt5a_mono:
        case _bitmap_format_dxn_mono_alpha: {
            S3TC_COLOR color;
            x&=3; y&=3;
            switch (bitmap->format) {
            case _bitmap_format_dxt1:
                HaloS3TC::DecodeBlockRGB__single_pixel((const S3TCBlockRGB *)pixel,&color,(short)x,(short)y,true);
                break;
            case _bitmap_format_dxt3: bitmap_xenon_cache_builder_decode_block_alpha4(pixel,&color,(short)x,(short)y); break;
            case _bitmap_format_dxt5:
                HaloS3TC::DecodeBlockAlpha3__single_pixel((const S3TCBlockAlpha3 *)pixel,&color,(short)x,(short)y);
                break;
            case _bitmap_format_dxt5a:
                color.blue=color.green=color.red=color.alpha=
                    (unsigned char)HaloS3TC::decode_xenon_dxt5a_block_single_pixel(pixel,x,y);
                break;
            case _bitmap_format_dxt5a_mono:
                color.blue=color.green=color.red=(unsigned char)HaloS3TC::decode_xenon_dxt5a_block_single_pixel(pixel,x,y);
                color.alpha=255;
                break;
            case _bitmap_format_dxt5a_alpha:
                color.blue=color.green=color.red=0;
                color.alpha=(unsigned char)HaloS3TC::decode_xenon_dxt5a_block_single_pixel(pixel,x,y);
                break;
            case _bitmap_format_dxn_mono_alpha:
            color.blue = color.green = color.red = (unsigned char)HaloS3TC::decode_xenon_dxt5a_block_single_pixel(pixel, x, y);
            color.alpha = (unsigned char)HaloS3TC::decode_xenon_dxt5a_block_single_pixel(pixel + 8, x, y);
            break;
            }
            result.red=(float)color.red*(1.0f/255.0f);
            result.green=(float)color.green*(1.0f/255.0f);
            result.blue=(float)color.blue*(1.0f/255.0f);
            result.alpha=(float)color.alpha*(1.0f/255.0f); break;
        }
        default: result.alpha=result.green=result.blue=result.red=0.0f; break;
        }
        switch (bitmap->curve) {
        case 1:
            result.red=convert_xrgb_to_linear(result.red);
            result.green=convert_xrgb_to_linear(result.green);
            result.blue=convert_xrgb_to_linear(result.blue);
        break;
        case 5:
            result.red=pow(result.red,2.2f);
            result.green=pow(result.green,2.2f);
            result.blue=pow(result.blue,2.2f);
        break;
        case 2:
            result.red=(float)fabs(result.red)*result.red;
            result.green=(float)fabs(result.green)*result.green;
            result.blue=(float)fabs(result.blue)*result.blue;
        }
    }
    return result;
}
