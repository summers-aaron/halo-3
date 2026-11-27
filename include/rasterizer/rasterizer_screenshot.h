#ifndef RASTERIZER_RASTERIZER_SCREENSHOT_H
#define RASTERIZER_RASTERIZER_SCREENSHOT_H

extern "C" void rasterizer_export_screenshot_tile(long tile_x, long tile_y, long tile_width, long tile_height, long image_width,
	long image_height, long unused, long surface);

#endif // RASTERIZER_RASTERIZER_SCREENSHOT_H
