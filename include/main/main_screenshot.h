#ifndef MAIN_MAIN_SCREENSHOT_H
#define MAIN_MAIN_SCREENSHOT_H

#include "math/real_math_types.h"
class c_player_view;
class c_camera_fx_values;

struct s_screenshot_options
{
    long flags;
    c_player_view *view;
    // Same object as rasterizer_dx9_dynamic_geometry_screenshot_parameters: the camera fx slot
    // rasterizer_render_screenshot fills, then the first screen-effect channel (channels 1..7
    // follow as hue/saturation/brightness/exposure, then filter/offset).
    c_camera_fx_values *camera_fx;
    float first_screen_effect_channel;
    float hue_start, hue_end;
    float saturation_end, saturation_start;
    float brightness, exposure_offset, exposure_bias;
    real_rgb_color filter, offset;
    long width, height;
    long tile_width, tile_height;
    long tile_count_x, tile_count_y;
    bool depth_of_field;
};
extern "C" bool main_screenshot_compute_tile_layout(s_screenshot_options *parameters);

extern "C" float g_tiled_render_x_scale, g_tiled_render_y_scale;
extern "C" float g_tiled_render_x_offset, g_tiled_render_y_offset;

#endif
