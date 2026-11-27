// blamlibxbox/bitmap_group_xenon_interop.obj: from the records 0x2872e0 0x287530 0x2874a8 0x2877e8
// @flags /TP /O2 /Oc
#include <xtl.h>
#include <xgraphics.h>
#include <ppcintrinsics.h>
#include <stdlib.h>
#include "tag_files/tag_types.h"
#include "tag_files/tag_api_interop.h"

struct s_render_texture_format { short width,height; signed char depth,levels,type; unsigned char packed; long format; };

struct s_render_texture_descriptor
{
    s_tag_data base_pixel_data;
    s_tag_data high_res_pixel_data;
    s_render_texture_format format;
};
struct s_tag_d3d_texture : D3DBaseTexture {};
class c_tag_interop_verifier;


namespace
{
    template <typename t_descriptor, typename t_object>
    struct c_typed_tag_interop : s_tag_interop
    {
        __forceinline t_descriptor *get_descriptor() const
        {
            return (t_descriptor *)descriptor;
        }
        __forceinline t_object *get() const
        {
            return (t_object *)address;
        }
    };
}

struct s_render_texture_interleaved_descriptor
{
    s_tag_data base_pixel_data;
    s_tag_data high_res_pixel_data;
    s_render_texture_format formats[2];
};
struct s_tag_d3d_texture_interleaved
{
    D3DBaseTexture textures[2];
};

// @address 0x2872e0 ?initialize_xenon_texture_header_internal@@YAXPBUs_render_texture_format@@PAUD3DBaseTexture@@_N@Z
void initialize_xenon_texture_header_internal(const s_render_texture_format *texture,D3DBaseTexture *header,bool packed) {
 D3DFORMAT format=(D3DFORMAT)texture->format;
 unsigned char packed_mips=texture->packed;
 long mip_offset=packed_mips ? 0 : -1;
 long first_level=0;
 UINT base_size=0;
 UINT mip_size=0;
 switch(texture->type) {
 case 0:
  XGSetTextureHeader(texture->width >> first_level, texture->height >> first_level, texture->levels - first_level, 4, format, D3DPOOL_DEFAULT, 0,
      mip_offset, 0, (D3DTexture *)header, &base_size, &mip_size);
  break;
 case 1:
  XGSetVolumeTextureHeader(texture->width >> first_level, texture->height >> first_level,
      (texture->depth >> first_level) > 1 ? (texture->depth >> first_level) : 1, texture->levels - first_level, 4, format, D3DPOOL_DEFAULT, 0, mip_offset,
      (D3DVolumeTexture *)header, &base_size, &mip_size);
  break;
 case 2:
  XGSetCubeTextureHeader(texture->width >> first_level, texture->levels - first_level, 4, format, D3DPOOL_DEFAULT, 0,
      mip_offset, (D3DCubeTexture *)header, &base_size, &mip_size);
  break;
 case 3:
  XGSetArrayTextureHeader(texture->width >> first_level, texture->height >> first_level, texture->depth, texture->levels - first_level, 4,
      format, D3DPOOL_DEFAULT, 0, mip_offset, 0, (D3DArrayTexture *)header, &base_size, &mip_size);
  break;
 // match: 0x2872e0: drops the cmplwi type,3 / bgt bounds check before the type jump table
 default: __assume(0);
 }
 if(packed_mips && !packed) header->Format.dword[4]=(header->Format.dword[4] & ~60u)|(_rotl(1,2)&60);
}

struct s_xenon_texture_interleaved_layout;

// @address 0x287530 ?initialize_xenon_texture_interleaved_header_internal@@YAXPBUs_render_texture_format@@PAUD3DBaseTexture@@_N012PAUs_xenon_texture_interleaved_layout@@@Z
static __forceinline bool bitmap_interleaved_tiled(const s_render_texture_format *texture, const s_render_texture_format *texture2)
{
 return !(texture->width>64 || texture2->width>64) ? false : (texture->packed!=0 || texture2->packed!=0);
}

static __forceinline bool bitmap_interleaved_header_tiled(const s_render_texture_format *texture, bool tiled)
{
 return texture->packed!=0 && tiled;
}

void initialize_xenon_texture_interleaved_header_internal(const s_render_texture_format *texture, D3DBaseTexture *header, bool packed,
    const s_render_texture_format *texture2, D3DBaseTexture *header2, bool packed2, s_xenon_texture_interleaved_layout *layout)
{
 D3DFORMAT format2=(D3DFORMAT)texture2->format;
 short width=texture->width;
 D3DFORMAT format=(D3DFORMAT)texture->format;
 bool tiled=bitmap_interleaved_tiled(texture,texture2);
 bool header_tiled=bitmap_interleaved_header_tiled(texture,tiled);
 bool header2_tiled=bitmap_interleaved_header_tiled(texture2,tiled);
 long mip_offset=tiled ? (long)0x80000000 : -1;
 UINT base_size;
 UINT mip_size;
 if(texture->type!=0) {
  if(texture->type!=2) {
   XGSetArrayTextureHeaderPair(0, mip_offset, width, texture->height, texture->depth, texture->levels, 4, format, 0, 0, 0, (D3DTexture *)header,
       texture2->width, texture2->height, texture2->depth, texture2->levels, 4, format2, 0, 0, 0, (D3DTexture *)header2, &base_size, &mip_size);
  } else {
   XGSetCubeTextureHeaderPair(0, mip_offset, width, texture->levels, 4, format, 0, 0, (D3DTexture *)header, texture2->width, texture2->levels, 4, format2,
       0, 0, (D3DTexture *)header2, &base_size, &mip_size);
  }
 } else {
  XGSetTextureHeaderPair(0, mip_offset, width, texture->height, texture->levels, 4, format, 0, 0, 0, (D3DTexture *)header, texture2->width,
      texture2->height, texture2->levels, 4, format2, 0, 0, 0, (D3DTexture *)header2, &base_size, &mip_size);
 }
 if(header_tiled && !packed) header->Format.dword[4]=(header->Format.dword[4] & ~60u)|(_rotl(1,2)&60);
 if(header2_tiled && !packed2) header2->Format.dword[4]=(header2->Format.dword[4] & ~60u)|(_rotl(1,2)&60);
}

namespace
{
    static __forceinline void bitmap_xenon_texture_build_header(
        c_typed_tag_interop<s_render_texture_descriptor, s_tag_d3d_texture> *interop,
        D3DBaseTexture *header)
    {
        s_render_texture_descriptor *descriptor = interop->get_descriptor();
        initialize_xenon_texture_header_internal(&descriptor->format, header, descriptor->high_res_pixel_data.address != 0);
        if (descriptor->format.packed != 0)
        {
            if (descriptor->high_res_pixel_data.address != 0)
                XGOffsetBaseTextureAddress(header, descriptor->high_res_pixel_data.address, descriptor->base_pixel_data.address);
            else
                XGOffsetBaseTextureAddress(header, descriptor->base_pixel_data.address, descriptor->base_pixel_data.address);
        }
        else
        {
            XGOffsetResourceAddress(header, descriptor->base_pixel_data.address);
        }
    }
}

// @address 0x2874a8 ?tag_d3d_texture_fixup@@YA_NPAUs_tag_interop@@PAVc_tag_interop_verifier@@@Z
bool tag_d3d_texture_fixup(s_tag_interop *tag_interop, c_tag_interop_verifier *verifier)
{
    c_typed_tag_interop<s_render_texture_descriptor, s_tag_d3d_texture> *interop =
        (c_typed_tag_interop<s_render_texture_descriptor, s_tag_d3d_texture> *)tag_interop;
    s_render_texture_descriptor *descriptor = interop->get_descriptor();
    s_tag_d3d_texture *texture = interop->get();
    bitmap_xenon_texture_build_header(interop, texture);
    return true;
}

// @address 0x2877e8 tag_d3d_texture_interleaved_fixup
extern "C" bool tag_d3d_texture_interleaved_fixup(s_tag_interop *interop, c_tag_interop_verifier *interop_verifier)
{
    c_typed_tag_interop<s_render_texture_interleaved_descriptor, s_tag_d3d_texture_interleaved> *typed_interop =
        (c_typed_tag_interop<s_render_texture_interleaved_descriptor, s_tag_d3d_texture_interleaved> *)interop;
    s_render_texture_interleaved_descriptor *descriptor = typed_interop->get_descriptor();
    s_tag_d3d_texture_interleaved *tag_texture_interleaved = typed_interop->get();
    const s_render_texture_format *format0 = &descriptor->formats[0];
    const s_render_texture_format *format1 = &descriptor->formats[1];
    initialize_xenon_texture_interleaved_header_internal(format0, &tag_texture_interleaved->textures[0], descriptor->high_res_pixel_data.address != 0,
        format1, &tag_texture_interleaved->textures[1], descriptor->high_res_pixel_data.address != 0, 0);
    for (long texture_index = 0; texture_index < 2; texture_index++)
    {
        const s_render_texture_format *format = &descriptor->formats[texture_index];
        D3DBaseTexture *texture = &tag_texture_interleaved->textures[texture_index];
        if (format->packed)
            XGOffsetBaseTextureAddress(texture, descriptor->high_res_pixel_data.address, descriptor->base_pixel_data.address);
        else
            XGOffsetResourceAddress(texture, descriptor->base_pixel_data.address);
    }
    return true;
}

// @address 0x42d380 ?tag_d3d_texture_teardown@@YA_NPAUs_tag_interop@@@Z
bool tag_d3d_texture_teardown(s_tag_interop *)
{
    return true;
}

// @address 0x42d380 ?tag_d3d_texture_interleaved_teardown@@YA_NPAUs_tag_interop@@@Z
bool tag_d3d_texture_interleaved_teardown(s_tag_interop *)
{
    return true;
}
