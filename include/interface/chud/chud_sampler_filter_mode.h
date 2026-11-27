#include "rasterizer/rasterizer_device_states.h"
#ifndef INTERFACE_CHUD_CHUD_SAMPLER_FILTER_MODE_H
#define INTERFACE_CHUD_CHUD_SAMPLER_FILTER_MODE_H

struct D3DDevice;
extern "C" {
extern D3DDevice *g_rasterizer_device;
}

static __forceinline void chud_draw_set_sampler_filter_mode(long sampler_index, long filter_mode)
{
    if (g_rasterizer_sampler_filter_modes[sampler_index] != filter_mode)
    {
        g_rasterizer_sampler_filter_modes[sampler_index] = filter_mode;
        rasterizer_device_set_sampler_filter_modes(g_rasterizer_device, sampler_index, filter_mode, filter_mode);
    }
}

#endif
