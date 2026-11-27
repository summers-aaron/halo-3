#ifndef RASTERIZER_SURFACE_GLOBALS_H
#define RASTERIZER_SURFACE_GLOBALS_H
class c_render_surface_group;
extern "C" {
extern c_render_surface_group g_rasterizer_surface_groups[];
extern bool g_rasterizer_using_albedo_sampler;
}
#endif
