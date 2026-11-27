#ifndef INCLUDE_PHYS_SIMPLEX_H
#define INCLUDE_PHYS_SIMPLEX_H

// Functions defined in tu/hkinternal/hkGsk.cpp that other units call.

class phys_vec4;

extern "C" void phys_simplex_recalc_contact_internal(phys_vec4 *vertices_a, phys_vec4 *vertices_b, int dim_a, int dim_b,
	const phys_vec4 &master_normal, phys_vec4 &simplex_point_a, phys_vec4 &support);

#endif // HAVOK_HK_GSK_H
