#ifndef RENDER_VIEWS_WORLD_VIEW_TYPES_H
#define RENDER_VIEWS_WORLD_VIEW_TYPES_H

#include "render/render_camera_types.h"
#include "objects/cluster_reference.h"
#include "render/render_view.h"
#include "render/render_player_view_observer_flags_types.h"
#include "render/render_simple_lights.h"
#include "render/camera_fx_values.h"

#include "render/render_player_view_camera_settings.h"

#include "render/views/patchy_fog.h"
#include "visibility/visibility_projection_types.h"
struct light_geometry;
struct light_state;
struct D3DSurface;

#include "render/views/world_view.h"

class c_lights_view : public c_world_view {
public:
    void clear_simple_light_draw_list();
    __forceinline void build_simple_light_draw_list(long window, const float &scale);
    __forceinline void submit_simple_light_draw_list_to_shader() { render_lights_submit_simple_light_draw_list_to_shader(simple_light_data,
        simple_light_count); }
 static void initialize_simple_light(simple_light *,real_point3d,real_rgb_color,float,float,real_vector3d,float,float,float);
public:
  virtual void render() {}
  virtual void render_setup();
  virtual void compute_visibility();
  virtual void render_submit_visibility();
    public: void setup_clip_planes(void);
    public: void compute_visibility(long light_index, light_geometry *geometry);
    public: bool setup_shadow_camera_and_projection(light_state *, light_geometry *, long, long);
    public: void submit_visibility_and_render(long light_index, float scale, light_geometry *geometry);
public:
  D3DSurface *render_target0, *render_target1, *depth_stencil;
  visibility_projection m_visibility_projections[6];
  short m_visibility_projection_count;
  /* implicit padding keeps texture_matrix aligned */
  float texture_matrix[16];
  bool orthographic;
  float near_size, near_aspect, near_plane, far_plane;
  long width;
  long height;
  union { long light_index; long user_index; };
public:
  union { long light_count; long simple_light_count; };
  simple_light single_light;
  union { simple_light lights[16]; float simple_light_data[320]; };
  float exposure;
};


typedef char verified_lights_size[(sizeof(void *) != 4 || ((sizeof(c_lights_view) == 0x1590))) ? 1 : -1];
typedef char verified_lights_user[(sizeof(void *) != 4 || ((offsetof(c_lights_view, user_index) == 0x1030))) ? 1 : -1];
typedef char verified_lights_data[(sizeof(void *) != 4 || ((offsetof(c_lights_view, simple_light_data) == 0x1088))) ? 1 : -1];

#endif
