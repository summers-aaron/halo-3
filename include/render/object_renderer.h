#ifndef RENDER_OBJECT_RENDERER_H
#define RENDER_OBJECT_RENDERER_H

#include "rasterizer/rasterizer.h"
#include "render_methods/render_method_definitions.h"

struct s_render_geometry;
struct s_mesh;
struct s_part;
struct s_object_render_context;
struct s_context_mesh_part;
struct s_mesh_part_counter;
struct s_render_object_info;
union real_point3d;

class c_object_renderer {
public:
  static void render_active_camo_object_context(const void *context, long active_camo_value);
  static void submit_object_attachments(long, long);
  static long add_object_render_context(void);
  static bool add_transparent_mesh_part(const s_render_geometry *render_geometry, const s_mesh *mesh, const s_part *part, long p3, long p4,
      const s_object_render_context *context, long p6, real_point3d *point, bool p8, unsigned char p9, void (*p10)(const void *, long));
  static void pop_marker(void);
  static void render_albedo(long p1);
  static void render_unattached_widgets(e_entry_point entry_point);
  static void render_object_contexts(e_entry_point entry_point, long p1);
  static void render_object_widgets(e_entry_point entry_point);
  static void render_shadows_generate(e_entry_point entry_point);
  static void render_static_lighting(c_rasterizer::e_z_buffer_mode z_buffer_mode);
  static void submit_visibility(long render);
  static void render_object_context_mesh_part(s_context_mesh_part *part, e_entry_point entry_point);
  static void render_object_context_instances(
      s_context_mesh_part *part, e_entry_point entry_point, unsigned long *instance_mask, long stencil_index, long flags);
  static long add_context_mesh_part(struct s_mesh_part_counter *counter, struct s_render_object_info *info, unsigned long *data,
      long mesh_index, long part_index, long region_index, long render_method_index, long render_mask_flags);
};

#endif // RENDER_OBJECT_RENDERER_H
