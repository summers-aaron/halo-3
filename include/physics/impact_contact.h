#ifndef PHYSICS_IMPACT_CONTACT_H
#define PHYSICS_IMPACT_CONTACT_H

#include "physics/impact.h"

struct s_contact_constraint_reference;

extern "C" long impact_new_from_contact(const c_impact::s_contact_description *contact_description, long type);
extern "C" void impact_initialize_contact_description(c_impact::s_contact_description *description, long type, bool use_marker, long object_a,
    long rigid_body_a, c_global_material_type material_a, long object_b, long rigid_body_b, c_global_material_type material_b,
    const real_point3d *position, const real_vector3d *normal, long friction_point_index,
    const s_contact_constraint_reference *constraint_reference);

#endif // PHYSICS_IMPACT_CONTACT_H
