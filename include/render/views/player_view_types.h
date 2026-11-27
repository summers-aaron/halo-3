#ifndef RENDER_VIEWS_PLAYER_VIEW_TYPES_H
#define RENDER_VIEWS_PLAYER_VIEW_TYPES_H

#include "render/views/world_view_types.h"
#include "render/views/simple_view_types.h"
#include "render/ui_view.h"
#include "render/views/occlusion_view.h"

extern "C" void player_view_setup_lighting(long view_index, long user_index);

class c_player_view : public c_world_view
{
    friend void player_view_setup_lighting(long view_index, long user_index);
protected:
    static c_player_view *x_current_player_view;
public:
    virtual void render();
    static c_player_view x_global_player_views[4];
#if defined(_XBOX) && !defined(__clang__)
    static void (*get_patchy_fog_callback())(const void *, long)
    {
        union
        {
            void (c_player_view::*member)();
            void (*callback)(const void *, long);
        } address;
        address.member = &c_player_view::render_patchy_fog;
        return address.callback;
    }
#else
    // The transparent list calls a cdecl (render data, user data) function; an x86 member is thiscall.
    static void render_patchy_fog_callback(const void *view, long)
    {
        const_cast<c_player_view *>(static_cast<const c_player_view *>(view))->render_patchy_fog();
    }
    static void (*get_patchy_fog_callback())(const void *, long) { return &render_patchy_fog_callback; }
#endif
    static __forceinline c_player_view *get_current() { return x_current_player_view; }
    static __forceinline void set_current(c_player_view *view) { x_current_player_view = view; }
    c_player_view();
    void create_frame_textures(long view_index);
    void setup_camera(long user_index, long window_count, long window_index, long controller_index,
        const struct s_observer_result *observer, bool unused);
    render_player_view_camera_settings *camera_settings;
    float exposure, exposure_multiplier;
    render_player_view_observer_flags observer_flags;
    c_patchy_fog patchy_fog;
    real_point3d last_position;
    real_vector3d last_forward;
    real_vector3d last_up;
    unsigned long : 32;
    real_matrix4x3 last_world_to_view;
    float last_projection_matrix[4][4];
    c_first_person_view first_person_view;
    c_ui_view ui_view;
    c_lights_view lights_view;
    c_lightmap_shadows_view lightmap_shadows_view;
    c_reflection_view reflection_view;
    c_occlusion_view occlusion_view;
    __forceinline render_projection &get_occlusion_projection() { return occlusion_view.render_projection_state; }
    __declspec(property(get=get_occlusion_projection)) render_projection occlusion_projection;
    long user_index;
    long window_count, window_index;
    long view_index;
    long controller_index;
    long antialiasing_mode;
    long window_type;

    bool last_view;
    void setup_camera_fx_parameters(float value);
    long get_player_view_user_index();
    static void get_player_render_camera_orientation(real_matrix4x3 *camera);
    virtual void compute_visibility();
    virtual void render_submit_visibility();
protected:
    static void static_lighting_setup_internals(long user_index);
    void render_static_lighting();
    void submit_occlusion_tests(bool render_objects, bool render_decals);
protected:
  void render_patchy_fog();
protected:
  void render_first_person();
  bool render_albedo();
  void render_lens_flares();
protected:
  void submit_attachments();
public:
  virtual void render_setup();
};

typedef char player_view_occlusion_projection_offset[
    (sizeof(void *) != 4 || ((offsetof(c_player_view, occlusion_view) + offsetof(c_occlusion_view, render_projection_state) == 0x28b0))) ? 1 : -1];

typedef char player_view_last_view_offset[(sizeof(void *) != 4 || ((offsetof(c_player_view, last_view) == 0x29d0))) ? 1 : -1];
typedef char player_view_size[(sizeof(void *) != 4 || ((sizeof(c_player_view) == 0x29e0))) ? 1 : -1];

typedef char verified_exposure_range[
    (sizeof(void *) != 4 || ((offsetof(c_player_view, lights_view) + offsetof(c_lights_view, exposure) == 0x2168))) ? 1 : -1];

typedef char verified_values[(offsetof(render_player_view_camera_settings, values) == 0xe4) ? 1 : -1];
typedef char verified_camera_settings[(sizeof(void *) != 4 || ((offsetof(c_player_view, camera_settings) == 0x298))) ? 1 : -1];
typedef char verified_last_position[(sizeof(void *) != 4 || ((offsetof(c_player_view, last_position) == 0x60c))) ? 1 : -1];
typedef char verified_last_world_to_view[(sizeof(void *) != 4 || ((offsetof(c_player_view, last_world_to_view) == 0x634))) ? 1 : -1];
typedef char verified_last_projection[(sizeof(void *) != 4 || ((offsetof(c_player_view, last_projection_matrix) == 0x668))) ? 1 : -1];

typedef char verified_exposure[(sizeof(void *) != 4 || ((offsetof(c_player_view, exposure) == 0x29c))) ? 1 : -1];

#endif // RENDER_VIEWS_PLAYER_VIEW_TYPES_H
