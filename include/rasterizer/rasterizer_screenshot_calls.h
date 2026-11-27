#ifndef INCLUDE_RASTERIZER_RASTERIZER_SCREENSHOT_CALLS_H
#define INCLUDE_RASTERIZER_RASTERIZER_SCREENSHOT_CALLS_H

extern "C" {
bool main_screenshot_setup_tile_camera(const void *, long, long);
void main_screenshot_render_tile(const void *, long, long);
void screen_postprocess_render_flare(const void *, long, long);
}

#endif
