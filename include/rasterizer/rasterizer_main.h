#ifndef RASTERIZER_RASTERIZER_MAIN_H
#define RASTERIZER_RASTERIZER_MAIN_H

#include "rasterizer/rasterizer_main_thread.h"
#include "cache/restricted_memory_regions.h"

// Functions defined in tu/graphicslibxbox/rasterizer_main.cpp.

struct XVIDEO_MODE;

// Holds g_effect_render_globals ("rasterizer game states") in restricted region 3; its allocate and deallocate
// and its region setter live in tu/graphicslibxbox/render_objects.cpp.
void __tls_set_g_effect_render_globals(void *value);
extern t_restricted_allocation_manager<3, static_cast<void (*)(void *)>(0), static_cast<void (*)(void *)>(0),
    &__tls_set_g_effect_render_globals> g_effect_render_globals_allocator;
template<> void *t_restricted_allocation_manager<3, static_cast<void (*)(void *)>(0), static_cast<void (*)(void *)>(0),
    &__tls_set_g_effect_render_globals>::reserve_memory(const char *name, const char *type, unsigned int size, long alignment_bits);

extern "C" bool g_rasterizer_video_mode_needs_init;
extern "C" XVIDEO_MODE g_rasterizer_video_mode;

XVIDEO_MODE *rasterizer_main_get_video_mode(void);
void rasterizer_main_setup_hdr_render_targets(bool use_second_target, bool hdr_alpha_blend);

struct rasterizer_main_line_vertex {
    unsigned long position;
    long type;
};
struct rasterizer_main_packed_state;
struct rasterizer_main_texture_definition;
union real_point2d;
union rectangle2d;
struct s_rasterizer_main_noise_context;
struct s_rasterizer_main_noise_vertex;
#include "gpu_particle/target_table_types.h"

extern "C" float rasterizer_main_get_video_aspect_ratio(void);
extern "C" void rasterizer_main_get_display_and_render_bounds(rectangle2d *display_bounds, rectangle2d *title_safe_bounds,
    rectangle2d *render_bounds, rectangle2d *render_title_safe_bounds);
extern "C" void rasterizer_main_compute_surface_overlaps(void);
extern "C" void rasterizer_main_decompress_texture(unsigned char *output, const rasterizer_main_texture_definition *definition);
extern "C" void rasterizer_main_build_glyph_vertices(void *context, rasterizer_main_line_vertex *vertices);
extern "C" void rasterizer_main_build_edge_vertices(void *context, rasterizer_main_line_vertex *vertices);
extern "C" void rasterizer_main_build_grid_vertices(void *context, rasterizer_main_packed_state *vertices);
extern "C" float rasterizer_main_evaluate_curve(const real_point2d *points, float x);
extern "C" void rasterizer_main_dispose_shared_shaders(void);
extern "C" void rasterizer_main_initialize_shared_shaders(void);
extern "C" void rasterizer_main_initialize_target_table(s_gpu_target_table<11> *table, long declaration_index, void *owner);
extern "C" void rasterizer_main_register_sixth_callback(void);
extern "C" void rasterizer_main_register_seventh_callback(void);
extern "C" void rasterizer_main_register_eighth_callback(void);
extern "C" void rasterizer_main_handle_first_empty_callback(void);
extern "C" void rasterizer_main_handle_second_empty_callback(void);
extern "C" void rasterizer_main_fill_glow_texture(void *context, unsigned char *pixels, long pitch);
extern "C" void rasterizer_main_fill_noise_texture_and_vertices(s_rasterizer_main_noise_context *context, unsigned char *noise,
    s_rasterizer_main_noise_vertex *vertices, long row_pitch, long slice_pitch);
extern "C" void rasterizer_main_initialize_loading_resources(s_rasterizer_main_noise_context *globals);

// Data defined in tu/graphicslibxbox/rasterizer_main.cpp that other units use.

class c_interlocked_long;

extern c_interlocked_long g_thread_owning_device;

extern "C" {
extern rasterizer_main_texture_definition k_rasterizer_texture_definitions[];
extern real_point2d k_rasterizer_curve_1[];
extern real_point2d k_rasterizer_curve_2[];
extern real_point2d k_rasterizer_curve_4[];
extern real_point2d k_rasterizer_curve_5[];
extern real_point2d k_rasterizer_curve_6[];
extern real_point2d k_rasterizer_curve_7[];
extern real_point2d k_rasterizer_curve_8[];
}

extern "C" {
void contrail_gpu_initialize_targets(void *, long, void *);
void contrail_gpu_initialize_frame_targets(void *, long, void *);
void light_volume_gpu_initialize_targets(void *, long, void *);
}

#endif
