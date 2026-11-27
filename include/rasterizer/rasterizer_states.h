#ifndef RASTERIZER_RASTERIZER_STATES_H
#define RASTERIZER_RASTERIZER_STATES_H

// Functions defined in tu/graphicslibxbox/rasterizer_states.cpp that other units call.

extern "C" void rasterizer_set_global_shader_parameter(long index);
extern "C" void rasterizer_draw_quads(const void *vertices, long quad_count);
// Rebuild HiZ from the resolved depth surface after the static lighting pass.
extern "C" void rasterizer_rebuild_hierarchical_z_buffer(void);

union rectangle2d;

extern "C" void rasterizer_setup_distortion_targets(const rectangle2d *bounds, unsigned char special);

struct D3DDevice;

extern "C" void rasterizer_set_device_alpha_blend_mode(D3DDevice *device, long alpha_blend_mode);

extern "C" unsigned char g_rasterizer_stencil_ref;
extern "C" bool g_render_static_lighting_active;
extern "C" float k_rasterizer_depth_bias_table[];
extern "C" float k_decal_slope_scale_bias;
extern "C" long g_rasterizer_sampler_address_modes[26];
extern "C" unsigned long k_d3d_texture_address_modes[];

extern "C" void rasterizer_set_back_buffer_render_target(long index, long target);

#endif // RASTERIZER_RASTERIZER_STATES_H
