#ifndef RASTERIZER_XENON_RASTERIZER_TILING_SETTINGS_TYPES_H
#define RASTERIZER_XENON_RASTERIZER_TILING_SETTINGS_TYPES_H

// XDK d3d9types.h uses this guard for the signed-long resolve rectangle.
#ifndef D3DRECT_DEFINED
typedef struct _D3DRECT
{
    long x1;
    long y1;
    long x2;
    long y2;
} D3DRECT;
#define D3DRECT_DEFINED
#endif

struct s_rasterizer_tiling_settings
{
    long width;
    long height;
    long resolve_width;
    long resolve_height;
    long tile_count;
    long multisample;
    D3DRECT tile_rects[8];
};

#endif
