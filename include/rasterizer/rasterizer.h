#ifndef RASTERIZER_RASTERIZER_H
#define RASTERIZER_RASTERIZER_H

#include "rasterizer/rasterizer_index_buffer.h"
#include "rasterizer/rasterizer_textures_cache.h"
#include "rasterizer/rasterizer_vertex_definitions.h"
#include "render_methods/render_method_definitions.h"

// Static rasterizer interface.

struct D3DDevice;
struct D3DSurface;
struct D3DTexture;
union point2d;
union real_point2d;
union real_point3d;
union real_rectangle2d;
union real_vector4d;
union rectangle2d;
struct rasterizer_vertex_screen;
struct rasterizer_vertex_transparent;
struct rasterizer_vertex_world;
struct rasterizer_dynamic_screen_geometry_parameters;
struct s_rasterizer_render_globals;
struct s_rasterizer_tiling_settings;
struct s_rasterizer_entry;
class c_rasterizer_xenon_memory;
struct s_mesh;
class c_render_method;
class c_rasterizer_pixel_shader;
class c_rasterizer_texture_ref;
class c_rasterizer_vertex_buffer;
class c_rasterizer_vertex_shader;

extern "C" void lens_flares_update(void);
extern "C" void render_initialize_loading_screen(void);
extern "C" void lens_flares_submit_occlusions(long user_index, int query_type);
extern "C" void rasterizer_set_light_render_targets(D3DSurface *, D3DSurface *, D3DSurface *);
extern "C" void rasterizer_set_back_buffer_render_target(long index, long target);
void rasterizer_render_water(bool enabled);
extern "C" void render_method_submit_reset_cached_state();
extern "C" bool render_method_submit_update_animated_state(long, const c_render_method *, long, bool, bool);
extern "C" void render_method_submit_update_externs(const c_render_method *, long, long, unsigned char);
extern "C" bool render_method_submit_set_shader_state(const c_render_method *render_method, int vertex_type, int transfer_type, int entry_point, bool force);
extern "C" void emblem_draw_screen_quad(void *loading_screen, const real_point2d *position, const real_point2d *anchor, D3DTexture *texture, float scale);
extern "C" void render_method_submit_set_cached_state(c_rasterizer_xenon_memory *, s_rasterizer_entry *, long, const c_render_method *,
    const s_mesh *, long);

class c_rasterizer
{
public:
    // Only the values units name are listed; the rest are used by number until they are settled.
    enum e_multires { _multires_full = 0 };

    enum e_surface
    {
        _surface_none = 0,
        _surface_display = 1,
        _surface_depth_stencil = 3,
        _surface_depth_stencil_multisample = 4,
        _surface_accum_LDR = 13,
        _surface_accum_HDR = 14,
        _surface_accum_LDR_secondary = 15,
        _surface_accum_HDR_secondary = 16,
        _surface_distortion = 52,
        _surface_dynamic = 55,
    };

    enum e_alpha_blend_mode
    {
        _alpha_blend_opaque = 0,
        _alpha_blend_additive,
        _alpha_blend_multiply,
        _alpha_blend_alpha_blend,
        _alpha_blend_double_multiply,
        _alpha_blend_pre_multiplied_alpha,
        _alpha_blend_maximum,
        _alpha_blend_multiply_add,
        _alpha_blend_add_src_times_dstalpha,
        _alpha_blend_add_src_times_srcalpha,
        _alpha_blend_inv_alpha_blend,
        _alpha_blend_motion_blur_static,
        _alpha_blend_motion_blur_inhibit,
    };

    enum e_separate_alpha_blend_mode
    {
        _separate_alpha_blend_off = 0,
        _separate_alpha_blend_one_zero,
        _separate_alpha_blend_one_one,
        _separate_alpha_blend_dest_alpha_zero,
        _separate_alpha_blend_constant_alpha,
    };

    enum e_z_buffer_mode
    {
        _z_buffer_mode_normal = 0,
        _z_buffer_mode_read_only,
        _z_buffer_mode_off,
        _z_buffer_mode_decal,
        _z_buffer_mode_decal_2,
        _z_buffer_mode_5,
        _z_buffer_mode_6,
        _z_buffer_mode_7,
    };

    enum e_stencil_mode
    {
        _stencil_mode_off = 0,
        _stencil_mode_old_stipple,
        _stencil_mode_shadow_apply,
        _stencil_mode_shadow_generate,
        _stencil_mode_shadow_apply_write,
        _stencil_mode_shadow_apply_write_always,
        _stencil_mode_shadow_apply_write_increment,
        _stencil_mode_shadow_generate_hi,
        _stencil_mode_clear_hi,
        _stencil_mode_stipple_write,
        _stencil_mode_stipple_read,
        _stencil_mode_object_write,
        _stencil_mode_object_read,
        _stencil_mode_shadow_read,
        _stencil_mode_decal_write,
        _stencil_mode_decal_read,
        _stencil_mode_shadow_generate_hi_ref,
    };

    enum e_sampler_address_mode
    {
        _sampler_address_wrap = 0,
        _sampler_address_clamp,
        _sampler_address_mirror,
        _sampler_address_border,
    };

    enum e_sampler_filter_mode
    {
        _sampler_filter_mode_trilinear = 0,
        _sampler_filter_mode_point,
        _sampler_filter_mode_bilinear,
    };

    enum e_screen_resolution
    {
        _screen_resolution_1152x640 = 0,
        k_screen_resolution_count
    };

    // The vertex/pixel shader GPR split (set_gprs_allocation): 0 = the default, 1 = 48/80, 2 = the vertex
    // shaders' maximum, 3 = the pixel shaders' maximum, 4 = 112/16, 5 = 75/53. March names none of them.
    enum e_gpr_allocation
    {
    };

    static D3DDevice *get_device() { return g_device; }
    static void set_use_floating_point_z_buffer(bool value) { m_use_floating_point_z_buffer = value; }

    static void shell_initialize(bool, bool);
    static void initialize();
    static void dispose();
    static void initialize_for_new_map();
    static void dispose_from_old_map();
    static void initialize_for_new_structure_bsp(unsigned long structure_bsp_mask);
    static void dispose_from_old_structure_bsp(unsigned long structure_bsp_mask);
    static bool initialize_after_device_creation_or_reset();
    static bool cleanup_before_device_reset();
    static void cleanup_d3d_states(bool block_until_idle);
    static void rasterizer_device_acquire_thread();
    static void rasterizer_device_release_thread();
    static bool rasterizer_thread_owns_device();
    static void begin(rectangle2d window_bounds, rectangle2d render_bounds);
    static bool begin_frame();
    static bool end_frame();
    static void begin_high_quality_blend();
    static void end_high_quality_blend();
    static bool get_is_tiling_enabled();
    static void begin_tiling();
    static void end_tiling();
    static void refresh_tiling_render_targets();
    static inline const s_rasterizer_tiling_settings *get_tiling_settings(long screen_resolution, long tiling_configuration);
    static void set_DAC_LUT_curve();

    static void get_display_pixel_bounds(rectangle2d *bounds);
    static void get_fullscreen_render_title_safe_pixel_bounds(rectangle2d *bounds);

    static void clear(unsigned long flags, unsigned long color, float z, unsigned char stencil);
    static void clearf(unsigned long flags, const real_vector4d *color, float z, unsigned long stencil);
    static void set_viewport(const rectangle2d &bounds, float minimum_z, float maximum_z);
    static void set_scissor_rect(const rectangle2d *rectangle);

    static void set_render_target(long index, e_surface surface);
    static e_surface get_render_target(long index) { return g_color_surfaces[index]; }
    static void set_depth_stencil_surface(e_surface surface);
    static void resolve_surface(e_surface surface, long target, const rectangle2d *bounds, long x, long y);
    static void resolve_surface_tiled(e_surface surface, long source_render_target, bool, bool force_samples);
    static e_gpr_allocation set_gprs_allocation(e_gpr_allocation allocation);
    static void resolve_entire_surface(c_rasterizer_texture_ref texture, long source_target);
    static void set_aliased_surface_as_texture(long sampler_index, e_surface surface);
    static long get_surface_width(e_surface surface);
    static long get_surface_height(e_surface surface);
    static bool lock_surface_as_texture(e_surface surface, unsigned long flags, char **pixels, long *pitch);
    static float get_render_target_alpha_multiplier(long render_target);
    static void set_using_albedo_sampler(bool using_albedo_sampler);
    static void setup_targets_simple();
    static void setup_targets_dynamic_light();
    static void setup_targets_static_lighting(float exposure, float multiplier, bool hdr_alpha_blend, float stops, bool clear_targets);

    static __forceinline e_alpha_blend_mode get_current_alpha_blend_mode() { return g_current_alpha_blend_mode; }
    static __forceinline void set_current_alpha_blend_mode(e_alpha_blend_mode mode) { g_current_alpha_blend_mode = mode; }
    static void set_alpha_blend_mode(e_alpha_blend_mode mode);
    static void set_separate_alpha_blend_mode(e_separate_alpha_blend_mode mode);
    static void set_z_buffer_mode(e_z_buffer_mode z_buffer_mode);
    static void set_stencil_mode(e_stencil_mode stencil_mode);
    static unsigned long bitmap_format_to_hardware_format(short bitmap_format, bool tiled, bool gamma, bool alpha_gamma);
    static void set_sampler_texture(long sampler, c_rasterizer_texture_ref texture_ref);
    static void set_sampler_texture_direct(long sampler, D3DTexture *texture);
    static void set_sampler_address_mode(long sampler, e_sampler_address_mode u, e_sampler_address_mode v, e_sampler_address_mode w);
    // Apply the same address mode to all three texture axes.
    static void set_sampler_address_mode(long sampler, e_sampler_address_mode address_mode)
    {
        set_sampler_address_mode(sampler, address_mode, address_mode, address_mode);
    }
    static void set_sampler_filter_mode(long sampler, e_sampler_filter_mode filter_mode);
    static void clear_sampler_textures(unsigned long unused);

    static bool set_vertex_shader(const c_rasterizer_vertex_shader *shader, e_vertex_type vertex_type,
        e_transfer_vector_vertex_types transfer_vector_vertex_type, e_entry_point entry_point);
    static bool set_pixel_shader(const c_rasterizer_pixel_shader *shader, e_entry_point entry_point);
    static bool set_explicit_shaders(long explicit_shader_index, e_vertex_type vertex_type, e_transfer_vector_vertex_types transfer_vector_vertex_type,
        e_entry_point entry_point);
    static void set_vertex_shader_constant(long start_register, long vector4f_count, const real_vector4d *constant_data);
    // Inline body for inlined call sites, named apart from the out-of-line function above:
    // this build emits bl for cross-unit calls, so inlined sites need this copy.
    // Defined in the calling unit, which sees the full D3D headers; any other
    // inlined caller needs its own unit-local definition.
    static __forceinline void inl_set_vertex_shader_constant(long start_register, long vector4f_count, const real_vector4d *constant_data);
    static void set_pixel_shader_constant(long start_register, long vector4f_count, const real_vector4d *constant_data);
    static inline void set_pixel_shader_constant_bool(long start, long count, const int *values);
    static void disown_vertex_shader_constants(long start_register, long vector4f_count);
    static bool set_vertex_stream(const c_rasterizer_vertex_buffer *vertex_buffer);
    static bool clear_vertex_stream(const c_rasterizer_vertex_buffer *vertex_buffer);

    static void draw_primitive(c_rasterizer_index_buffer::e_primitive_type primitive_type, long start_vertex, long primitive_count);
    static void draw_primitive_up(c_rasterizer_index_buffer::e_primitive_type primitive_type, unsigned long primitive_count, const void *vertex_data,
        unsigned long vertex_stride);
    static void draw_indexed_primitive(const c_rasterizer_index_buffer *indices, long base_vertex_index, long vertex_count, long min_index, long index_count);
    static void draw_fullscreen_quad();
    static void draw_fullscreen_quad(int width, int height);
    static void draw_fullscreen_quad_with_texture_xform(int width, int height, const real_rectangle2d *texture_bounds);
    static void draw_screen_quad_with_texture_transform(int width, int height, const real_rectangle2d *screen_bounds, const real_rectangle2d *texture_bounds);
    static void draw_textured_screen_quad(const rasterizer_vertex_screen *vertices, bool unused);
    static void draw_textured_screen_triangle_list(const rasterizer_vertex_screen *vertices, long triangle_count);
    static bool draw_textured_transparent_polygon(long explicit_shader);
    static void draw_textured_transparent_quad(const rasterizer_vertex_transparent *vertices, e_alpha_blend_mode alpha_blend_mode);
    static void draw_worldspace_polygon(const rasterizer_vertex_world *vertices, long vertex_count);
    static void draw_debug_line2d(const real_point3d &point0, const real_point3d &point1, unsigned long color0, unsigned long color1);
    static void draw_debug_linestrip2d(const point2d *points, long point_count, unsigned long color);

    enum e_tiling_configuration
    {
    };

    enum e_render_mode
    {
    };

    static s_rasterizer_render_globals render_globals;
    static e_render_mode g_LDR_render_mode;
    static e_render_mode g_HDR_render_mode;
    static bool g_HDR_alpha_blend;
    static e_screen_resolution g_screen_resolution;
    static e_tiling_configuration g_tiling_configuration;
    static e_screen_resolution g_screen_resolution_debug;
    static e_tiling_configuration g_tiling_configuration_debug;

private:
    struct s_sampler
    {
        long address_mode;
        long filter_mode;
        c_rasterizer_texture_ref texture;
    };

    friend void rasterizer_stipple_initialize(void);
    friend class c_patchy_fog;
    friend class c_decal;
    friend class c_lights_view;
    friend class c_rasterizer_draw_string;
    friend class c_tag_resource_cache_dpc_event;
    friend class c_rasterizer_xenon_memory;
    friend void lens_flares_update(void);
    friend void render_initialize_loading_screen(void);
    friend void lens_flares_submit_occlusions(long, int);
    friend void lens_flares_reset_rasterizer_parameters(void);
    friend void lens_flares_render(long);
    friend void rasterizer_render_water(bool enabled);
    friend void rasterizer_set_light_render_targets(D3DSurface *, D3DSurface *, D3DSurface *);
    friend void rasterizer_set_back_buffer_render_target(long index, long target);
    friend void rasterizer_psuedo_dynamic_screen_quad_draw(const rasterizer_dynamic_screen_geometry_parameters *parameters);
    friend void render_method_submit_reset_cached_state();
    friend bool render_method_submit_update_animated_state(long, const c_render_method *, long, bool, bool);
    friend void render_method_submit_update_externs(const c_render_method *, long, long, unsigned char);
    friend void render_method_submit_invalidate_cache();
    friend void breakable_surface_render_cracks(long, long, long);
    friend bool render_method_submit_set_shader_state(const c_render_method *, int, int, int, bool);
    friend void emblem_draw_screen_quad(void *, const real_point2d *, const real_point2d *, D3DTexture *, float);
    friend void render_method_submit_set_cached_state(c_rasterizer_xenon_memory *, s_rasterizer_entry *, long, const c_render_method *,
        const s_mesh *, long);
    static bool initialize_device(bool, bool);
    static void update_tiling_configuration();

    static D3DDevice *g_device;
    static e_surface g_depth_stencil_surface;
    static e_surface g_color_surfaces[4];
    static e_z_buffer_mode g_current_z_buffer_mode;
    static e_separate_alpha_blend_mode g_current_separate_alpha_blend_mode;
    static e_alpha_blend_mode g_current_alpha_blend_mode;
    static bool m_use_floating_point_z_buffer;
    static long x_last_sampler_textures[26];
    static s_sampler g_samplers[26];
};

#endif // RASTERIZER_RASTERIZER_H
