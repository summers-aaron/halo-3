#ifndef PHYSICS_PHYSICS_MODEL_CONSTRAINT_REFERENCE_H
#define PHYSICS_PHYSICS_MODEL_CONSTRAINT_REFERENCE_H

// Identifies a constraint by type and index, packed into a single word.
struct s_physics_model_constraint_reference {
    short type;
    short index;
};

typedef char physics_model_constraint_reference_size[(sizeof(s_physics_model_constraint_reference) == 4) ? 1 : -1];

#endif
