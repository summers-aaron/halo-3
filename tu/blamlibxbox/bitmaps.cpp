// blamlibxbox/bitmaps.obj: from the records 0x161ef8 0x2a6cb0 0x2a6d50 0x2a6df0 0x2a6e60 0x2a6f68
// @compiler xdk6130
// @flags /TP /O2 /Oc
#include "bitmaps/bitmaps.h"
#include "bitmaps/bitmap_data.h"
#include "bitmaps/bitmap_formats.h"
#include "cseries/crt_xdk.h"


float convert_xrgb_to_linear(float value) {
 float a = value / 4.000039f;
 float b = (value - 0.12499412f) / 2.0000315f;
 float result = a > b ? a : b;
 float c = (value - 0.25000352f) / 1.0000157f;
 result = result > c ? result : c;
 float d = (value - 0.49999216f) / 0.50000781f;
 result = result > d ? result : d;
 return result;
}
float half_to_real_all_finite(unsigned short half) {
 long sign = (half >> 15) & 1;
 long exponent = (half >> 10) & 31;
 long mantissa = half & 1023;
 if (exponent == 0) {
  if (mantissa != 0) {
   while ((mantissa & 1024) == 0) {
    mantissa <<= 1;
    exponent -= 1;
   }
   exponent += 1;
   mantissa &= ~1024;
   exponent += 112;
   mantissa <<= 13;
  }
 } else if (exponent == 31) {
  if (mantissa == 0) {
   exponent = 142;
   mantissa = 8380416;
  } else {
   mantissa = 0;
   exponent = mantissa;
  }
 } else {
  exponent += 112;
  mantissa <<= 13;
 }
 union
 {
  unsigned long bits;
  float real;
 } result;
 result.bits = (sign << 31) | (exponent << 23) | mantissa;
 return result.real;
}
bool bitmap_format_is_compressed(short format) {
 bool result = false;
 switch (format) {
 case _bitmap_format_dxt1:
 case _bitmap_format_dxt3:
 case _bitmap_format_dxt5:
 case _bitmap_format_dxt3a:
 case _bitmap_format_dxt5a:
 case _bitmap_format_dxt3a_1111:
 case _bitmap_format_dxn:
 case _bitmap_format_ctx1:
 case _bitmap_format_dxt3a_alpha:
 case _bitmap_format_dxt3a_mono:
 case _bitmap_format_dxt5a_alpha:
 case _bitmap_format_dxt5a_mono:
 case _bitmap_format_dxn_mono_alpha:
  result = true;
  break;
 }
 return result;
}
bool bitmap_format_is_default_srgb(short format) {
 bool result = true;
 switch (format) {
 case _bitmap_format_rgbfp32:
 case _bitmap_format_v8u8:
 case _bitmap_format_abgrfp32:
 case _bitmap_format_abgrfp16:
 case _bitmap_format_q8w8v8u8:
 case _bitmap_format_a2r10g10b10:
 case _bitmap_format_a16b16g16r16:
 case _bitmap_format_v16u16:
 case _bitmap_format_dxn:
 case _bitmap_format_ctx1:
  result = false;
  break;
 }
 return result;
}

// @address 0x2a6e60 bitmap_2d_initialize
extern "C" void bitmap_2d_initialize(bitmap_data *bitmap, short width, short height, short mipmap_count,
                  short format, unsigned short flags, unsigned char external, unsigned char tiled)
{
    memset(bitmap, 0, sizeof(*bitmap));
    bitmap->signature = 0x6269746d;
    bitmap->width = width;
    bitmap->height = height;
    bitmap->depth = 1;
    bitmap->type = 0;
    bitmap->format = format;
    bitmap->flags = 48 | (!external ? 8 : 0);
    bitmap->mipmap_count = 0;
    bitmap->curve = 0;
    bitmap->texture_ref = -1;
    if (!(width & (width - 1)) && !(height & (height - 1)))
        bitmap->flags |= 1;
    if (bitmap_format_is_compressed(format))
        bitmap->flags |= 2;
    bitmap->runtime_flags |= 2;
    bitmap->runtime_flags |= 4;
    if (!(bitmap->flags & 16))
        bitmap->runtime_flags |= 8;
    else
        bitmap->runtime_flags &= ~8;
    bitmap->base_address = 0;
}
static __forceinline bool bitmaps_handle_valid(long handle)
{
    return handle != 0 && handle != -1;
}
static __forceinline void bitmaps_clear_handle(long *handle)
{
    if (bitmaps_handle_valid(*handle))
        *handle = -1;
}
// @address 0x2a6f68 bitmaps_dispose_resource
extern "C" void bitmaps_dispose_resource(bitmap_data *resource)
{
    if (resource) {
        if (!(resource->flags & 0x80))
            bitmaps_clear_handle(&resource->texture_ref);
        if (resource->flags & 8) {
            void *buffer = resource->base_address;
            if (buffer)
                RtlFreeHeap(GetProcessHeap(), 0, buffer);
            RtlFreeHeap(GetProcessHeap(), 0, resource);
        }
    }
}
