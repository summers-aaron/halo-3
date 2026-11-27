#ifndef RASTERIZER_XENON_RASTERIZER_XENON_DEVICE_INFO_TYPES_H
#define RASTERIZER_XENON_RASTERIZER_XENON_DEVICE_INFO_TYPES_H

#include <xtl.h>

struct s_rasterizer_xenon_device_info
{
    D3DDISPLAYMODE display_mode;
    long last_skinning_matrix_count;
    D3DCAPS9 caps;
};

#endif // RASTERIZER_XENON_RASTERIZER_XENON_DEVICE_INFO_TYPES_H
