#include "render/shared_shader_constants.h"
#ifndef RENDER_RENDER_WATER_GLOBALS_H
#define RENDER_RENDER_WATER_GLOBALS_H

#include "math/real_math_types.h"

struct s_water_ripple_render_state
{
    float depth_bias;
    char reserved[0x1C];
    unsigned long ripple_index;
    long ripple_buffer_index;
};

extern "C" {
extern const real_vector4d *g_water_memexport_stream_constants;
extern const float k_random_word_normalization;
extern long g_water_tessellation_primitive_type;
extern const unsigned int k_water_tessellation_primitive_counts[][2];
extern bool g_water_interaction_enabled;
extern int g_water_camera_in_water;
extern long g_water_part_count;
extern long g_water_cluster_part_indices[];
extern int g_water_ripple_write_index;
extern s_water_ripple_render_state g_water_ripple_render_state;
extern void *g_water_ripple_update_buffer_address;
extern unsigned char g_water_refraction_enabled;
extern real_vector4d g_water_inverse_view_projection_matrix[4];
extern float g_render_far_plane;
extern unsigned char g_water_wireframe_enabled;
extern void *g_water_ripple_append_buffer_address;
}

extern "C" long g_water_interaction_last_time;
extern "C" {
extern long g_rasterizer_current_z_buffer_mode;
extern long g_rasterizer_alpha_blend_mode;
}

#endif
