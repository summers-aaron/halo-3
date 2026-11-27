#ifndef VISIBILITY_VISIBILITY_RENDER_OBJECTS_H
#define VISIBILITY_VISIBILITY_RENDER_OBJECTS_H

// Functions defined in tu/blamlibxbox/visibility_render_objects.cpp.

#include "rasterizer/rasterizer_memory_types.h"

struct s_model_fade_parameters;

rasterizer_main_memory_globals *rasterizer_memory_get_globals();
extern "C" void visibility_render_objects_add_first_person(long p0, long p1, bool , bool );
bool visibility_render_objects_update_geometry_cache(int target_object_index, unsigned int render_model_index,
    unsigned int render_view_index, short *object_lod, int previous_lod_value,
    unsigned char *region_mask, int *region_total, bool *geometry_changed);
bool visibility_render_objects_geometry_in_cache(long index, long region_count, const char *permutations,
    char *permutations_out, long level_of_detail, bool first_person, unsigned char *values_out, bool *any_out);
extern "C" int visibility_render_objects_compute_lod(long object_index, int view_index,
    const s_model_fade_parameters *fade, float distance, signed char previous_lod);

#endif // VISIBILITY_VISIBILITY_RENDER_OBJECTS_H
