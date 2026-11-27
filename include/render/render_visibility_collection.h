#ifndef RENDER_RENDER_VISIBILITY_COLLECTION_H
#define RENDER_RENDER_VISIBILITY_COLLECTION_H

#include "objects/object_reference_types.h"

// Functions defined in tu/graphicslibxbox/render_visibility_collection.cpp that other units call.

long render_visibility_add_object_to_items(long object_index, long view_index, bool lit, bool shadow);

struct s_render_object_info;

extern "C" char *render_visibility_get_cached_lod(long object_index);
extern "C" int render_visibility_allocate_reset_memory(int a, int b, int size);
extern "C" bool render_visibility_update_object(long object_index, float distance, float lighting_distance,
	unsigned long view_index, s_render_object_info *info);

struct render_camera;
struct render_projection;

extern "C" void render_visibility_camera_collection_compute(const render_camera *camera, s_cluster_reference cluster,
	const render_projection *projection, long a, long b, bool flag);

// Data defined in tu/graphicslibxbox/render_visibility_collection.cpp that other units use.

struct s_render_visibility_collection_globals;

extern s_render_visibility_collection_globals render_visibility_collection_globals;

extern "C" void render_visibility_compute_object_state(long object_index, s_render_object_info *info);

#endif // RENDER_RENDER_VISIBILITY_COLLECTION_H
