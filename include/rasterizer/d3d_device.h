#ifndef RASTERIZER_D3D_DEVICE_H
#define RASTERIZER_D3D_DEVICE_H

// XDK device entry points used by the render path without importing xbox.h.
struct D3DDevice;

extern "C" {
#ifndef _D3D9_H_
void D3DDevice_SetRenderState_ColorWriteEnable(D3DDevice *device, unsigned long color_write_mask);
void D3DDevice_Clear(D3DDevice *device, unsigned long count, const void *rects, unsigned long flags, unsigned long color, float z,
    unsigned long stencil, int edram_clear);
#endif
unsigned long D3DDevice_InsertFence(D3DDevice *);
void D3DDevice_SetShaderGPRAllocation(D3DDevice *, unsigned long, unsigned long, unsigned long);
void D3DDevice_SetRenderState_HighPrecisionBlendEnable(D3DDevice *, unsigned long);
void D3DDevice_SetRenderState_HighPrecisionBlendEnable1(D3DDevice *, unsigned long);
void D3DDevice_SetRenderState_HighPrecisionBlendEnable2(D3DDevice *, unsigned long);
void D3DDevice_SetRenderState_HighPrecisionBlendEnable3(D3DDevice *, unsigned long);
}

#endif // RASTERIZER_D3D_DEVICE_H
