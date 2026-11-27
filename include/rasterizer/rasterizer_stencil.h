#ifndef RASTERIZER_RASTERIZER_STENCIL_H
#define RASTERIZER_RASTERIZER_STENCIL_H

#include "rasterizer/rasterizer.h"

extern "C" c_rasterizer::e_stencil_mode g_rasterizer_stencil_mode;
extern "C" void rasterizer_set_shadow_stencil_ref(int, int, int, int);
extern "C" void rasterizer_set_stencil_mode_with_ref(c_rasterizer::e_stencil_mode, unsigned char);

#endif // RASTERIZER_RASTERIZER_STENCIL_H
