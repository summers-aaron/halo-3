#ifndef RASTERIZER_RASTERIZER_TEXT_GLOBALS_H
#define RASTERIZER_RASTERIZER_TEXT_GLOBALS_H

#include "rasterizer/rasterizer_device_states.h"
#include "math/real_math_types.h"
#include "multithreading/registered_thread_index.h"
#include "multithreading/threads.h"

#include "text/font_package_types.h"
#include "rasterizer/simple_font_types.h"
#include "main/main_screenshot.h"

extern "C" {
extern "C++" s_font_globals g_font_globals;
extern simple_font::s_font_data *g_simple_font_display;
extern bool g_simple_font_scaling_initialized;
extern real_vector2d g_simple_font_vertex_scale;
extern float g_simple_font_inverse_viewport_width;
extern float g_simple_font_inverse_viewport_height;
}
#endif
