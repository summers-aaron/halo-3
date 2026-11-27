#ifndef RASTERIZER_RENDER_TARGET_RESOURCES_H
#define RASTERIZER_RENDER_TARGET_RESOURCES_H

#include "rasterizer/xenon/rasterizer_xenon_resource.h"
#include "rasterizer/xenon/rasterizer_device_types.h"

struct D3DDevice;
struct D3DSurface;
struct D3DBaseTexture;
struct s_texture_header;

struct s_render_targets_surface_desc {
    unsigned long format;
    unsigned long type;
    unsigned long usage;
    unsigned long size;
    unsigned long multisample_type;
    unsigned long multisample_quality;
    unsigned long width;
    unsigned long height;
};
// XDK XGTEXTURE_DESC layout (xgraphics.h); XGGetTextureDesc fills this descriptor.
struct s_render_surface_texture_desc {
    unsigned long resource_type;
    unsigned long width;
    unsigned long height;
    unsigned long depth;
    int format;
    unsigned long row_pitch;
    unsigned long slice_pitch;
    unsigned long bits_per_pixel;
    unsigned long width_in_blocks;
    unsigned long height_in_blocks;
    unsigned long depth_in_blocks;
    unsigned long bytes_per_block;
    int exp_bias;
    unsigned long flags;
    unsigned long multisample_type;
};
typedef char render_surface_texture_desc_size[(sizeof(s_render_surface_texture_desc) == 60) ? 1 : -1];
extern "C" {
void D3DSurface_GetDesc(D3DSurface *surface, s_render_targets_surface_desc *desc);
unsigned int XGSetTextureHeader(unsigned int, unsigned int, unsigned int, unsigned int, int, int, unsigned int, unsigned int, unsigned int,
    void *, unsigned int *, unsigned int *);
void D3DDevice_Resolve(D3DDevice *device, unsigned long flags, const void *rect, D3DBaseTexture *texture, const void *point,
    unsigned int level, unsigned int slice, const void *clear_color, float clear_z, unsigned long clear_stencil, const void *parameters);
}
__forceinline long D3DDevice::SetDepthStencilSurface(D3DSurface *surface) {
    D3DDevice_SetDepthStencilSurface(this, surface);
    return 0;
}
__forceinline long D3DDevice::SetTexture(unsigned long sampler, D3DTexture *texture) {
    D3DDevice_SetTexture(this, sampler, (D3DBaseTexture *)texture, 0x8000000000000000ui64 >> (sampler + 32));
    return 0;
}
__forceinline long D3DDevice::SetRenderTarget(unsigned long index, D3DSurface *surface)
{
    D3DDevice_SetRenderTarget_External(this, index, surface);
    return 0;
}
struct D3DTexture {
    __forceinline long LockRect(unsigned int level, _D3DLOCKED_RECT *locked,
                               const tagRECT *rectangle, unsigned long flags)
    {
        D3DTexture_LockRect(this, level, locked, rectangle, flags);
        return 0;
    }
};
struct s_surface_parameters {
    unsigned int base;
    unsigned int hierarchical_z;
    unsigned int flags;
};
#endif
