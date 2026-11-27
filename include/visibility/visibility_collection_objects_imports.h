#ifndef VISIBILITY_VISIBILITY_COLLECTION_OBJECTS_IMPORTS_H
#define VISIBILITY_VISIBILITY_COLLECTION_OBJECTS_IMPORTS_H

#include "math/real_math_types.h"
#include "objects/object_reference_types.h"
#include "render/render_objects_static_lighting_hierarchy_levels_types.h"

struct s_visibility_region;
struct cluster_partition;

// match: 0x15c180: noalias keeps the center words on r9 with the inside stb above the shape compare; without it the loads split and the stb sinks.
__declspec(noalias) bool visibility_region_test_sphere(
    const s_visibility_region *region, long cluster_index, const real_point3d *center, float radius, bool *inside_out);
// match: 0x158d60: noalias keeps the 18-GPR frame with iterator/payload spills at 80/84; without it the frame grows to __savegprlr_20 and recolors.
__declspec(noalias) long cluster_partition_get_first_datum_and_payload(
    const cluster_partition *partition, long *iterator, s_cluster_reference cluster, long payload_size, const void **payload);
extern "C" long render_objects_add_visible_hierarchy(long object_index, const real_point3d *origin, float radius,
    long view_index, bool lit, bool shadow, bool fully_contained, long index,
    render_objects_static_lighting_hierarchy_levels alphas, bool compute_distances, bool exclude_first_person, long player_index);

#endif
