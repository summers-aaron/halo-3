#ifndef RASTERIZER_RASTERIZER_GLOBALS_H
#define RASTERIZER_RASTERIZER_GLOBALS_H

#include <stddef.h>
#include "multithreading/synchronized_value.h"

// Functions defined in tu/graphicslibxbox/rasterizer_globals.cpp that other units call.

void rasterizer_globals_initialize(void);
void rasterizer_globals_vblank_callback(unsigned long swap_index);
float rasterizer_get_average_frame_delta(void);

#if !defined(_XBOX) || defined(__clang__)
// The vertical blank rate exposed by the PC D3D device.
extern "C" unsigned long port_d3d_vertical_blank_rate(void);
#endif

// Data defined in tu/graphicslibxbox/rasterizer_globals.cpp that other units use.

extern float global_z_near;

enum
{
    k_frame_delta_count = 15
};

// The vblank state the vblank callback keeps. March 2007 rasterizer_globals_initialize clears 120 bytes, sets the
// vblank index, stores the presentation interval at +0x5c (rasterizer_set_presentation_interval) and sets +0x60.
struct s_rasterizer_globals
{
    c_synchronized_int64 vblank_index;
    c_synchronized_int64 last_vblank_index;
    c_synchronized_long frame_delta;
    c_synchronized_long swap_index;
    volatile long frame_delta_index;
    long frame_deltas[k_frame_delta_count];
    unsigned long flip_index;
    long presentation_interval;
    bool vblank_callback_installed;
    bool gpu_crashed;
    __int64 render_begin_timestamp;
    __int64 render_end_timestamp;
};

#if defined(_M_PPCBE) || defined(_XBOX)
typedef char rasterizer_globals_presentation_interval_offset[(offsetof(s_rasterizer_globals, presentation_interval) == 0x5c) ? 1 : -1];
typedef char rasterizer_globals_vblank_callback_installed_offset[(offsetof(s_rasterizer_globals, vblank_callback_installed) == 0x60) ? 1 : -1];
typedef char rasterizer_globals_render_begin_offset[(offsetof(s_rasterizer_globals, render_begin_timestamp) == 0x68) ? 1 : -1];
typedef char rasterizer_globals_size[(sizeof(s_rasterizer_globals) == 0x78) ? 1 : -1];
#endif

#include "rasterizer/rasterizer_timing_types.h"

extern s_rasterizer_globals rasterizer_globals;
extern s_rasterizer_timing_globals g_rasterizer_timing_globals;

#endif // RASTERIZER_RASTERIZER_GLOBALS_H
