#ifndef RENDER_SCREEN_POSTPROCESS_H
#define RENDER_SCREEN_POSTPROCESS_H

#include "rasterizer/rasterizer.h"
#include "math/real_math_types.h"

class c_player_view;
class c_camera_fx_values;

class c_screen_postprocess {
public:
    static void copy(long, c_rasterizer::e_surface, c_rasterizer::e_surface, c_rasterizer::e_sampler_filter_mode,
        c_rasterizer::e_sampler_address_mode, float, float, float, float, real_rectangle2d *);
    static c_rasterizer::e_surface postprocess_bloom_buffer(c_rasterizer::e_surface, c_camera_fx_values &, c_rasterizer::e_multires);
    static c_rasterizer::e_surface blur_display(float sigma_x, float sigma_y);
    static void blit(long, c_rasterizer::e_surface, c_rasterizer::e_surface, c_rasterizer::e_sampler_filter_mode,
        c_rasterizer::e_sampler_address_mode, float, float, float, float, real_rectangle2d *, real_rectangle2d *);
    static void gaussian_blur_fixed(c_rasterizer::e_surface, c_rasterizer::e_surface, float, float, float, float);
    static void gaussian_blur(c_rasterizer::e_surface, c_rasterizer::e_surface, float, float);
    static void resolve_frame_buffers(const rectangle2d *bounds);
    static void postprocess_player_view(c_player_view *view);
    static void setup_rasterizer_for_postprocess(bool enabled);
    static void apply_binary_op_ex(long, c_rasterizer::e_surface, c_rasterizer::e_surface, c_rasterizer::e_surface,
        c_rasterizer::e_sampler_filter_mode, c_rasterizer::e_sampler_address_mode, float, float, float, float, real_rectangle2d *);

    struct s_settings
    {
        union { bool resolve_hdr; bool enabled; };
        long final_surface_mode;
        long composite_mask;
        long final_flags;
        long primary;
        long bloom_mode;
        long bling;
        bool persist;
        long display;
        float final_scale;
        bool debug;
        bool distortion;
        float exposure;
        unsigned char exposure_adapt_instantly;
        long final_blend;
    };

    static s_settings x_editable_settings;
    static s_settings x_settings_internal;
};

 

#endif // RENDER_SCREEN_POSTPROCESS_H
