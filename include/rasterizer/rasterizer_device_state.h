#ifndef RASTERIZER_RASTERIZER_DEVICE_STATE_H
#define RASTERIZER_RASTERIZER_DEVICE_STATE_H

struct D3DDevice;

// Runtime storage remains in rasterizer_main.obj.
extern "C" D3DDevice *g_rasterizer_device;
extern "C" long g_rasterizer_alpha_blend_mode;

#endif
