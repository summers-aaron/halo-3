#include "rasterizer/rasterizer_tiling_globals.h"
#include "render/shared_shader_constants.h"
#include "render/depth_of_field_globals.h"
#include "rasterizer/rasterizer_surface_globals.h"
#include "rasterizer/rasterizer_bound_render_targets.h"
#include "render_methods/render_method_submit_cache.h"
#ifndef RENDER_SCREEN_POSTPROCESS_XENON_GLOBALS_H
#define RENDER_SCREEN_POSTPROCESS_XENON_GLOBALS_H
#include "render/camera_fx_globals_types.h"
#include "rasterizer/rasterizer_device_states.h"
#include "render/render_lightmap_shadows.h"

#include "rasterizer/rasterizer.h"
#include "math/real_math_types.h"
#include "render/shader_reference_types.h"
#include "effects/effect_render_globals.h"

class c_render_surface;
class c_render_surface_group;
struct s_depth_of_field;
struct s_range_source;
struct s_camera_fx_globals;
struct s_screen_postprocess_tiling_globals;
struct s_screen_postprocess_motion_blur_globals;

struct s_displacement_motion_blur_transforms
{
    real_vector4d transform0[4];
    real_vector4d transform1[4];
    real_vector4d transform2[4];
};

extern "C" {
extern const float k_postprocess_real_constants[];
extern long g_postprocess_frame_index;
extern float g_postprocess_exposure_offset;
extern long g_motion_blur_max_view_count;
extern long g_distortion_occlusion_context;
extern bool g_rasterizer_distortion_visible;
extern __declspec(thread) s_effect_render_globals *g_effect_render_globals;
float depth_of_field_compute_blur_sigma(const s_range_source *focus);
void rasterizer_mark_surface_resolved(c_render_surface *surface);
void rasterizer_render_displacement_pass(const rectangle2d *bounds, s_displacement_motion_blur_transforms transforms, bool motion_blur);
}

#endif
