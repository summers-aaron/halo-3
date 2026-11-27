#ifndef PHYSICS_PHYSICS_MODEL_DEFINITIONS_H
#define PHYSICS_PHYSICS_MODEL_DEFINITIONS_H

// Functions defined in tu/blamlibxbox/physics_model_definitions.cpp.

class c_global_material_type;

long physics_model_compute_best_material_rigid_body(long model_index, long body_index, c_global_material_type *global_material);
void physics_model_fixup(long physics_model_index, bool vtables_only);

struct s_physics_model_constraint_reference;
struct s_physics_model_definition;
struct s_physics_model_constraint_bodies;

s_physics_model_constraint_bodies *physics_model_try_and_get_constraint(s_physics_model_definition *model,
    const s_physics_model_constraint_reference *reference);
class phys_shape;
extern "C" bool physics_model_is_shape_static(const phys_shape *target_shape);

struct physics_model_default_constraint;
extern "C" {
extern int g_physics_model_default_constraint_guard;
extern physics_model_default_constraint g_physics_model_default_constraint;
}

#endif // PHYSICS_PHYSICS_MODEL_DEFINITIONS_H
