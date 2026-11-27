#ifndef RASTERIZER_XENON_RASTERIZER_TEXTURE_DESCRIPTION_H
#define RASTERIZER_XENON_RASTERIZER_TEXTURE_DESCRIPTION_H

struct s_texture_header;
struct s_render_surface_texture_desc;

extern "C" void XGGetTextureDesc(s_texture_header *, unsigned int, s_render_surface_texture_desc *);

#endif // RASTERIZER_XENON_RASTERIZER_TEXTURE_DESCRIPTION_H
