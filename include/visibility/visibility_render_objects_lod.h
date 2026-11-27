#ifndef VISIBILITY_VISIBILITY_RENDER_OBJECTS_LOD_H
#define VISIBILITY_VISIBILITY_RENDER_OBJECTS_LOD_H

struct s_render_object_info;

// match: This caller-facing declaration uses long previous_lod to preserve the
// caller's value without narrowing.
extern "C" bool visibility_render_objects_compute_lod_and_zsort(long object_index, float distance, long view_index,
    s_render_object_info *info, long previous_lod, signed char *lod);

#endif // VISIBILITY_VISIBILITY_RENDER_OBJECTS_LOD_H
