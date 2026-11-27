#ifndef OBJECTS_WIDGETS_MUFFINS_RENDER_H
#define OBJECTS_WIDGETS_MUFFINS_RENDER_H
#include "geometry/render_geometry_types.h"
#include "geometry/render_geometry_mesh_types.h"
#include "geometry/render_geometry_part_types.h"
#include "geometry/geometry_material_types.h"
typedef s_mesh muffins_render_mesh;
typedef s_render_part muffins_render_part;
typedef geometry_material muffins_render_material;
extern "C" void render_method_submit_draw_mesh_part(unsigned __int64 key, s_render_geometry *owner,
    muffins_render_mesh *mesh, muffins_render_part *part, long mode, unsigned long *selection,
    muffins_render_material *materials);
#endif
