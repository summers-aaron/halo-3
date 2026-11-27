#ifndef INCLUDE_RASTERIZER_XENON_RASTERIZER_D3D_CALLS_H
#define INCLUDE_RASTERIZER_XENON_RASTERIZER_D3D_CALLS_H

struct D3DDevice;
struct D3DTexture;
struct D3DBaseTexture;
struct D3DSurface;
struct D3DSURFACE_DESC;

extern "C" {
long Direct3D_CheckDepthStencilMatch(unsigned long adapter, unsigned long device_type, unsigned long adapter_format,
    unsigned long render_target_format, unsigned long depth_stencil_format);
long D3DDevice_SetViewportF(D3DDevice *device, void *viewport);
void D3DTexture_GetLevelDesc(D3DTexture *texture, unsigned int level, D3DSURFACE_DESC *desc);
void D3DDevice_SetTexture(D3DDevice *device, unsigned int sampler, D3DBaseTexture *texture, unsigned __int64 pending_mask);
D3DSurface *D3DDevice_CreateSurface(unsigned int, unsigned int, int, int, const void *);
}

struct XVIDEO_MODE;
struct declaration;
struct s_rasterizer_main_declaration_element;
struct _D3DLOCKED_BOX;

extern "C" {
void XGetVideoMode(XVIDEO_MODE *mode);
void D3DDevice_UnsetAll(D3DDevice *device);
void D3DDevice_BlockUntilIdle(D3DDevice *device);
unsigned long D3DQuery_Release(void *);
declaration *D3DDevice_CreateVertexDeclaration(const s_rasterizer_main_declaration_element *elements);
D3DTexture *D3DDevice_CreateTexture(unsigned long width, unsigned long height, unsigned long depth, unsigned long levels,
    unsigned long usage, unsigned long format, unsigned long pool, unsigned long type);
void D3DVolumeTexture_LockBox(D3DTexture *texture, unsigned int level, _D3DLOCKED_BOX *locked, const void *box, unsigned long flags);
}

#endif
