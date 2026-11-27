#ifndef RASTERIZER_XENON_RASTERIZER_XENON_DEVICE_CAPS_H
#define RASTERIZER_XENON_RASTERIZER_XENON_DEVICE_CAPS_H

#if !defined(_XBOX) || defined(__clang__)
#include "rasterizer/xenon/rasterizer_xenon_device_info_types.h"
#include "rasterizer/rasterizer_device_globals.h"

#define g_rasterizer_device_caps (g_rasterizer_xenon_device_info.caps)

__forceinline D3DFORMAT rasterizer_get_display_mode_format()
{
    return g_rasterizer_xenon_device_info.display_mode.Format;
}
#else
#include <xtl.h>

extern "C" D3DCAPS9 g_rasterizer_device_caps;

__forceinline D3DFORMAT rasterizer_get_display_mode_format()
{
    const D3DFORMAT *caps_words = (const D3DFORMAT *)&g_rasterizer_device_caps;
    return caps_words[-2];
}
#endif

#endif
