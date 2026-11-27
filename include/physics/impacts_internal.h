#ifndef PHYSICS_IMPACTS_INTERNAL_H
#define PHYSICS_IMPACTS_INTERNAL_H
#include "physics/impact.h"
#include "game/player_mapping_globals.h"
struct s_user_globals;

extern "C" long impact_new_from_contact(const c_impact::s_contact_description *description, c_impact::e_states state);

extern "C" void impact_initialize_contact_description(c_impact::s_contact_description *description, c_impact::e_type type, bool use_marker,
    long object_index_a, long node_index_a, c_global_material_type material_a,
    long object_index_b, long node_index_b, c_global_material_type material_b,
    const real_point3d *position, const real_vector3d *normal, long vehicle_friction_point_index,
    const s_physics_model_constraint_reference *constraint_reference);

#include "effects/effect_vector_types.h"
#include "effects/effect_deterministic_types.h"
struct s_location;
extern "C" long effect_new_impact_effect(
    c_tag_index, long, long, long, long, const s_effect_vector *, const real_plane3d *, const s_location *, float, e_effect_deterministic);

#endif
