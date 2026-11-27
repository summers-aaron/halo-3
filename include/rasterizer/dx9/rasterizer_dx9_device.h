#ifndef RASTERIZER_DX9_RASTERIZER_DX9_DEVICE_H
#define RASTERIZER_DX9_RASTERIZER_DX9_DEVICE_H

struct D3DDevice;
struct D3DSurface;

extern "C" void D3DDevice_SetRenderTarget_External(D3DDevice *device, unsigned long index, D3DSurface *surface);
extern "C" void D3DDevice_SetDepthStencilSurface(D3DDevice *device, D3DSurface *surface);

#endif // RASTERIZER_DX9_RASTERIZER_DX9_DEVICE_H
