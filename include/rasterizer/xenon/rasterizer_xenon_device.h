#ifndef RASTERIZER_XENON_RASTERIZER_XENON_DEVICE_H
#define RASTERIZER_XENON_RASTERIZER_XENON_DEVICE_H


struct D3DDevice;
struct real_plane3d;

extern "C" {
void __stdcall D3DDevice_SetClipPlane(D3DDevice *device, unsigned long index, const float *plane);
void __stdcall D3DDevice_SetRenderState_ClipPlaneEnable(D3DDevice *device, unsigned long enable);
void __stdcall D3DDevice_Suspend(D3DDevice *device);
void D3DDevice_BlockUntilIdle(D3DDevice *device);
}

#endif
