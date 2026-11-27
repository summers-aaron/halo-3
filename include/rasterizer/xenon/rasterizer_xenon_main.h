#ifndef RASTERIZER_XENON_RASTERIZER_XENON_MAIN_H
#define RASTERIZER_XENON_RASTERIZER_XENON_MAIN_H

// Functions defined in tu/graphicslibxbox/rasterizer_xenon_main.cpp.

void rasterizer_vblank_callback(const void *callback_data);
extern "C" bool rasterizer_save_screenshot(const char *path, bool supersample);

class c_rasterizer_xenon_memory;

extern "C" unsigned short rasterizer_xenon_encode_rgb565(const unsigned short *channels);
extern "C" float rasterizer_xenon_decode_10bit_float(unsigned short index);
extern "C" void rasterizer_xenon_memory_initialize_masks(c_rasterizer_xenon_memory *);
extern "C" c_rasterizer_xenon_memory *rasterizer_xenon_memory_create();

struct D3DSurface;
extern "C" void rasterizer_set_light_render_targets(D3DSurface *render_target0, D3DSurface *render_target1, D3DSurface *depth_stencil);

extern "C" void rasterizer_xenon_memory_reset(unsigned long structure_bsp_mask);

#endif // RASTERIZER_XENON_RASTERIZER_XENON_MAIN_H
