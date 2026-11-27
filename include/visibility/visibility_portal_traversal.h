#ifndef VISIBILITY_VISIBILITY_PORTAL_TRAVERSAL_H
#define VISIBILITY_VISIBILITY_PORTAL_TRAVERSAL_H

#include "objects/object_reference_types.h"

// Functions defined in tu/blamlibxbox/visibility_portal_traversal.cpp.

extern "C" short *vpt_pvs_get_visible_clusters(s_cluster_reference cluster_reference, long *portal_count);

struct s_visibility_region;
struct visibility_projection;

struct s_cluster_working_portals;
struct s_working_portal_stack;

bool build_working_portal_stack_add_working_portal_to_region(s_cluster_working_portals *, s_working_portal_stack *, short (*const)[256],
    const visibility_projection *, long, s_visibility_region *, long, s_cluster_reference);

void visibility_build_region_from_pvs(const visibility_projection *projection, s_cluster_reference source, s_visibility_region *region);
void visibility_build_region_from_projections(long projection_count, const visibility_projection *projections,
    s_cluster_reference cluster, bool projections_empty, s_visibility_region *region, bool use_pvs);

#endif // VISIBILITY_VISIBILITY_PORTAL_TRAVERSAL_H
