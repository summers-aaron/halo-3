#ifndef PHYSICS_PHYSICS_MODEL_CONSTRAINT_TYPES_H
#define PHYSICS_PHYSICS_MODEL_CONSTRAINT_TYPES_H

enum e_physics_model_constraint_type
{
    _physics_model_constraint_type_none = -1,
    _physics_model_constraint_type_hinge = 0,
    _physics_model_constraint_type_limited_hinge = 1,
    _physics_model_constraint_type_ragdoll = 2,
    _physics_model_constraint_type_stiff_spring = 3,
    _physics_model_constraint_type_ball_and_socket = 4,
    _physics_model_constraint_type_prismatic = 5,
    k_physics_model_constraint_type_count = 6
};

#endif
