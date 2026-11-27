#ifndef STRUCTURES_STRUCTURE_SEAMS_H
#define STRUCTURES_STRUCTURE_SEAMS_H

#include "objects/object_reference_types.h"
#include "structures/structure_seam_globals.h"

// Functions defined in tu/blamlibxbox/structure_seams.cpp.

void __tls_set_g_structure_seam_globals_allocator(void *allocator);
void structure_seams_disconnect(long scenario_index, long structure_bsp_index);
extern "C" bool structure_seams_try_and_get_connected_edge(long bsp_index, long edge_index, long *other_bsp_index, long *other_edge_index);
extern "C" void structure_seams_set_globals(void *value);

struct s_cluster_stack;

extern "C" bool structure_seams_connected_cluster_references_get(long scenario_tag, s_cluster_reference reference, bool include_self, s_cluster_stack *out);

#include "memory/static_arrays.h"

extern "C" void structure_seams_compute_bsp_flags(long scenario_tag, c_static_flags<512> *flags, long bsp_index);
c_static_flags<64> structure_seams_calculate_active_seams_mask();



static __forceinline s_structure_seam_globals *tls_structure_seam_globals()
{
    return g_structure_seam_globals;
}

#endif // STRUCTURES_STRUCTURE_SEAMS_H
