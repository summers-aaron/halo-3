// graphicslibxbox/rasterizer_textures_cache.obj: from the records 0x45eee0 0x45ef68 0x45f090 0x45f0f0
// @flags /TP /O2 /J /Oc
#include <ppcintrinsics.h>
#include <math.h>
#include <stdlib.h>
#include <xtl.h>
#include "bitmaps/bitmaps.h"
#include "rasterizer/rasterizer.h"
#include "rasterizer/rasterizer_textures_cache.h"
#include "rasterizer/rasterizer_d3d_allocations.h"
#include "bitmaps/bitmap_data.h"
// @address 0x45eee0 ?allocate@c_rasterizer_texture_ref@@SA?AV1@JJJJJ_NW4e_bitmap_type@@0PAX@Z
// @address 0x45ef68 ?allocate@c_rasterizer_texture_ref@@SA?AV1@PBUbitmap_data@@@Z
// @address 0x45f090 get_texture_level_desc
// @address 0x45f0f0 ?get_width_height@c_rasterizer_texture_ref@@QAA_NJPAJ0@Z
// @address 0x5a4b28 D3DTexture_GetLevelDesc
// @address 0x5a4b28 D3DCubeTexture_GetLevelDesc

// @address 0x24a48 ?g_gputextureformat_to_srgb_gputextureformat_table@@3QBKB
// @rdata 0x82024a48..0x82024b48
const unsigned long g_gputextureformat_to_srgb_gputextureformat_table[64] = {
    0x00, 0x01, 0x02, 0x03,
    0x04, 0x05, 0x32, 0x36,
    0x08, 0x09, 0x0a, 0x0b,
    0x0c, 0x0d, 0x0e, 0x0f,
    0x37, 0x38, 0x33, 0x34,
    0x35, 0x15, 0x16, 0x17,
    0x18, 0x19, 0x1a, 0x1b,
    0x1c, 0x1d, 0x1e, 0x1f,
    0x20, 0x21, 0x22, 0x23,
    0x24, 0x25, 0x26, 0x27,
    0x28, 0x29, 0x2a, 0x2b,
    0x2c, 0x2d, 0x2e, 0x2f,
    0x30, 0x31, 0x32, 0x33,
    0x34, 0x35, 0x36, 0x37,
    0x38, 0x39, 0x3a, 0x3b,
    0x3c, 0x3d, 0x3e, 0x3f
};
// @rdata-end

c_rasterizer_texture_ref c_rasterizer_texture_ref::allocate(
    long width, long height, long levels, long d3d_format, long unused_allocation_mode,
    bool use_srgb_curve, e_bitmap_type bitmap_type, bool unused_option, void *initial_data)
{
    D3DTexture *allocated_texture = 0;
    D3DTexture *texture = 0;
    if (c_d3d_resource_allocator::create_texture(0x180, 0x180, 1, 0, (_D3DFORMAT)d3d_format, 1,
        &texture, 0, (unsigned char *)initial_data, initial_data ? 3 : 0) >= 0)
    {
        allocated_texture = texture;
    }
    return c_rasterizer_texture_ref(allocated_texture);
}

c_rasterizer_texture_ref c_rasterizer_texture_ref::allocate(const bitmap_data *bitmap)
{
    bool color_srgb = false;
    bool alpha_srgb = false;
    if (bitmap->curve == 0)
        color_srgb = ::bitmap_format_is_default_srgb(bitmap->format);
    else
        color_srgb = bitmap->curve == 1 || bitmap->curve == 5;
    unsigned long format = c_rasterizer::bitmap_format_to_hardware_format(
        bitmap->format, (bitmap->runtime_flags & 8) != 0, color_srgb, alpha_srgb);
    void *texture = 0;
    if (format != -1) {
        switch (bitmap->type) {
        case 0: {
            void *out_texture = 0;
            int hr = c_d3d_resource_allocator::create_texture(bitmap->width, bitmap->height,
                bitmap->mipmap_count + 1, 0, (_D3DFORMAT)format, 1, (D3DTexture **)&out_texture, 0, 0, 0);
            texture = hr >= 0 ? out_texture : 0;
            break;
        }
        case 2: {
            void *out_texture = 0;
            int hr = c_d3d_resource_allocator::create_cube_texture(bitmap->width, bitmap->mipmap_count + 1,
                0, (_D3DFORMAT)format, 1, (D3DCubeTexture **)&out_texture, 0, 0, 0);
            texture = hr >= 0 ? out_texture : 0;
            break;
        }
        }
    }
    c_rasterizer_texture_ref ref((D3DBaseTexture *)texture);
    return ref;
}

extern "C" bool get_texture_level_desc(D3DBaseTexture *texture, int unused, D3DSURFACE_DESC *desc)
{
    if (D3DResource_GetType(texture) != D3DRTYPE_TEXTURE) {
        D3DCubeTexture_GetLevelDesc((D3DCubeTexture *)texture, 0, desc);
    } else {
        D3DTexture_GetLevelDesc((D3DTexture *)texture, 0, desc);
    }
    return true;
}

bool c_rasterizer_texture_ref::get_width_height(long level, long *width, long *height)
{
    c_rasterizer_texture_ref reference(*this);
    bool success = false;
    *width = 0;
    *height = 0;
    unsigned char reference_valid = reference.valid();
    if (reference_valid)
    {
        D3DBaseTexture *texture = reference.get_d3d_texture();
        D3DSURFACE_DESC surface_desc;
        if (texture && get_texture_level_desc(texture, 0, &surface_desc))
        {
            long level_height = surface_desc.Height;
            *width = surface_desc.Width;
            *height = level_height;
            success = true;
        }
    }
    return success;
}
