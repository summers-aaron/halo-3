#ifndef RENDER_RENDER_OBJECTS_VISIBLE_HIERARCHY_H
#define RENDER_RENDER_OBJECTS_VISIBLE_HIERARCHY_H

#include "render/render_objects_static_lighting_hierarchy_levels_types.h"

union real_point3d;

extern "C" long render_objects_add_visible_hierarchy(long object_index, const real_point3d *origin, float distance_scale,
    unsigned long view_index, bool lit, bool shadow, bool fully_contained, long parent_index,
    render_objects_static_lighting_hierarchy_levels levels, bool compute_distances,
    bool exclude_first_person, long player_index);

#endif // RENDER_RENDER_OBJECTS_VISIBLE_HIERARCHY_H
