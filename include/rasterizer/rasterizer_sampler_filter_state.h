#ifndef RASTERIZER_SAMPLER_FILTER_STATE_H
#define RASTERIZER_SAMPLER_FILTER_STATE_H

struct D3DDevice;

extern "C" {
extern long g_rasterizer_sampler_filter_modes[26];
void rasterizer_device_set_sampler_filter_modes(D3DDevice *device, long sampler, long anisotropy_mode, long filter_mode);
}

#endif
