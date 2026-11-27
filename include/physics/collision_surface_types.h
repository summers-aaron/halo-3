#ifndef PHYSICS_COLLISION_SURFACE_TYPES_H
#define PHYSICS_COLLISION_SURFACE_TYPES_H

#include "memory/data_types.h"

// Same 12-byte surfaces element as collision_surface; the middle three fields carry runtime seam/breakable indices.
struct s_collision_surface
{
    short plane;
    unsigned short first_edge;
    short seam_link_index;
    short breakable_surface_index;
    short breakable_set_index;
    unsigned char flags;
    unsigned char best_plane_calculation_vertex_index;
};

#endif // PHYSICS_COLLISION_SURFACE_TYPES_H
