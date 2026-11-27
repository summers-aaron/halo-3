#ifndef RASTERIZER_RASTERIZER_TILING_TYPES_H
#define RASTERIZER_RASTERIZER_TILING_TYPES_H

#include <xtl.h>
#include <stddef.h>
#include "rasterizer/rasterizer_tiling_state_header.h"

struct s_gpu_memexport_stream_constant
{
    unsigned long address;
    unsigned long format;
    unsigned long flags;
    unsigned long size;
};

// Shared tiling layout: rasterizer_tiling_initialize and rasterizer_render_screenshot.
// Both globals of this type are 16-byte aligned, which rounds its size up to 0x80.
struct __declspec(align(16)) s_rasterizer_tiling_state : s_rasterizer_tiling_state_header
{
    bool render_bloom;
    bool render_flares;
    bool skip_postprocess;
    bool tiling_initialized;
    bool composite_tiles;
    bool reserved_tiling_flag;
    long screenshot_delay;
    bool use_half_size_tiles;
    void (*end_callback)(long, long, long, long, long, long, long, long);
    bool pad_to_tile_grid;
    D3DBaseTexture texture;
    s_gpu_memexport_stream_constant export_stream;
    long volatile *fence_destination;
};

typedef char rasterizer_tiling_texture_offset[(sizeof(void *) != 4 || ((offsetof(s_rasterizer_tiling_state, texture) == 0x30))) ? 1 : -1];
typedef char rasterizer_tiling_export_offset[(sizeof(void *) != 4 || ((offsetof(s_rasterizer_tiling_state, export_stream) == 0x64))) ? 1 : -1];
typedef char rasterizer_tiling_fence_offset[(sizeof(void *) != 4 || ((offsetof(s_rasterizer_tiling_state, fence_destination) == 0x74))) ? 1 : -1];
typedef char rasterizer_tiling_size[(sizeof(s_rasterizer_tiling_state) == 0x80) ? 1 : -1];

#endif
