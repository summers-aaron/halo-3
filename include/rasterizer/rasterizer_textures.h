#ifndef RASTERIZER_RASTERIZER_TEXTURES_H
#define RASTERIZER_RASTERIZER_TEXTURES_H

// Functions defined in tu/graphicslibxbox/rasterizer_textures.cpp.

struct D3DTexture;
extern "C" const unsigned int g_bitmap_format_to_hardware_format_table[];

extern "C" void *rasterizer_texture_lock_bits(D3DTexture **texture_slot, int overwrite, long *pitch);

#endif // RASTERIZER_RASTERIZER_TEXTURES_H
