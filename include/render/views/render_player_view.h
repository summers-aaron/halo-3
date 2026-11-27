#ifndef RENDER_VIEWS_RENDER_PLAYER_VIEW_H
#define RENDER_VIEWS_RENDER_PLAYER_VIEW_H

// Functions defined in tu/graphicslibxbox/render_player_view.cpp.

class c_lightmap_shadows_view;
class c_player_view;
union rectangle2d;
union real_point3d;
union real_vector3d;

extern "C" void render_player_view_update_particle_systems(int particle_user);
void render_texture_camera_set_object_marker(long object_index, long marker_name, float field_of_view_degrees);
extern "C" void player_view_render_transparent_objects(bool render_objects, bool render_decals);
extern "C" void render_player_view_sort_transparents();
extern "C" void render_player_view_find_transparent_boundary(void *buffer, unsigned short first, unsigned short last, unsigned short *boundary);
extern "C" void render_player_view_sort_transparents_recursive(void *buffer, unsigned short first, unsigned short last,
    unsigned short boundary, void *scratch, unsigned short original_boundary, unsigned short depth);
extern "C" void player_view_render_opaque_objects(long p1);
extern "C" void render_player_view_set_atmosphere_index(long atmosphere_index);
extern "C" void player_view_render_frame(c_player_view *frame, long p1);
extern "C" void player_view_setup_lighting(long view_index, long user_index);
extern "C" void player_view_render_pass(unsigned int p0, unsigned int p1);
extern "C" void player_view_render_distortion(c_player_view *player_view);
extern "C" void player_view_render_first_person_effects(c_player_view *view, unsigned char render_effects);
extern "C" void render_texture_camera_set_position_object(long object_index, long marker_index);

class c_camera_fx_values;

extern "C" float render_player_view_compute_pixel_scale(void);
extern "C" void render_player_view_setup_albedo_targets(bool unused);
extern "C" void render_player_view_set_exposure_constants(float exposure, float multiplier, float stops, bool disable);
extern "C" float render_player_view_compute_exposure(c_camera_fx_values *camera_fx_values);
extern "C" float render_player_view_compute_exposure_multiplier(c_camera_fx_values *camera_fx_values);
void __tls_set_g_render_texture_camera_globals_allocator(void *allocator);
extern "C" void render_texture_camera_set_field_of_view(float field_of_view_degrees);
extern "C" void render_texture_camera_set_position_and_direction(bool enable, const real_point3d *position, const real_vector3d *forward,
    const real_vector3d *up, float field_of_view);
extern "C" void lightmap_shadows_view_render_object(c_lightmap_shadows_view *view, long object_index, long user_index);

extern float k_first_person_far_clip_scale;

extern "C" void player_view_draw_static_lighting_composite(const c_player_view *view);
extern "C" void player_view_resolve_depth_of_field_source(long user_index, const rectangle2d *bounds, bool unused);

#endif // RENDER_VIEWS_RENDER_PLAYER_VIEW_H
