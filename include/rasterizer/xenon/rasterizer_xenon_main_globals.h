#include "rasterizer/rasterizer_surface_globals.h"
#ifndef RASTERIZER_XENON_MAIN_GLOBALS_H
#define RASTERIZER_XENON_MAIN_GLOBALS_H
#include <xtl.h>
#include "rasterizer/rasterizer_render_globals_types.h"
class c_render_surface_group;
#include "rasterizer/xenon/rasterizer_tiling_settings_types.h"
#include "rasterizer/rasterizer_shader_gpr_types.h"
extern "C" {
extern const s_rasterizer_tiling_settings k_rasterizer_tiling_settings[][6];
extern bool g_rasterizer_frame_has_ended;
extern bool g_rasterizer_requested_buffer_2_frames;
extern bool g_rasterizer_current_buffer_2_frames;
extern unsigned char g_rasterizer_requested_tiling_enabled;
}
#endif
