#ifndef INCLUDE_PHYSICS_PHYSICS_MODEL_CONSTRAINT_BODIES_ITERATOR_H
#define INCLUDE_PHYSICS_PHYSICS_MODEL_CONSTRAINT_BODIES_ITERATOR_H
#include "physics/physics_model_constraint_reference.h"
#include "physics/physics_model_constraint_types.h"
#include "physics/physics_model_definitions.h"
#include "physics/physics_model_constraint_definition.h"
struct s_physics_model_definition;
class c_physics_model_constraint_bodies_iterator
{
public:
    __forceinline c_physics_model_constraint_bodies_iterator(s_physics_model_definition *definition)
        : model(definition) { init(); }
    __forceinline s_physics_model_constraint_bodies *get()
    {
        return physics_model_try_and_get_constraint(model, &reference);
    }
    __forceinline e_physics_model_constraint_type get_constraint_type() const { return (e_physics_model_constraint_type)reference.type; }
    __forceinline long get_constraint_type_index() const { return reference.index; }
    void next();
private:
    void init();
    s_physics_model_definition *model;
    s_physics_model_constraint_reference reference;
};
#endif
