#ifndef RASTERIZER_RASTERIZER_DEVICE_GLOBALS_H
#define RASTERIZER_RASTERIZER_DEVICE_GLOBALS_H

struct D3DDevice;
struct D3DVertexDeclaration;
struct Direct3D;
struct s_rasterizer_xenon_device_info;
struct s_surface_group_description;
class c_interlocked_long;
class c_rasterizer_LUT_control;

// Shared device globals declared here for use across translation units.
extern "C" {
extern "C++" c_interlocked_long g_rasterizer_device_reset_flag;
extern const float k_widescreen_aspect_ratio;
extern D3DVertexDeclaration *g_rasterizer_vertex_declarations[22][4][4];
extern long g_rasterizer_surface_count;
extern s_surface_group_description k_render_surface_group_descriptions[68];
extern unsigned long g_rasterizer_resolved_surface_mask[7];
extern unsigned char g_rasterizer_shared_shaders_initialized;
extern bool g_rasterizer_main_initialized;
extern unsigned long g_trilinear_threshold;
extern unsigned long g_rasterizer_present_immediate_threshold;
extern long g_last_skinning_matrix_count;
extern const double k_rasterizer_gamma_decode_exponent;
extern unsigned long g_rasterizer_max_vs_gprs;
extern unsigned long g_rasterizer_max_ps_gprs;
extern int g_antialiasing_mode;
extern unsigned char g_rasterizer_tiling_enabled;
extern c_rasterizer_LUT_control g_rasterizer_lut_desired;
extern c_rasterizer_LUT_control g_rasterizer_lut_current;
extern s_rasterizer_xenon_device_info g_rasterizer_xenon_device_info;
extern void *g_d3d_resource_allocator_device;
extern Direct3D *g_rasterizer_direct3d;
extern long g_rasterizer_resolve_samples;
extern unsigned char g_rasterizer_in_ui_shell;
extern D3DDevice *g_command_buffer_device;
}

#endif // RASTERIZER_RASTERIZER_DEVICE_GLOBALS_H
