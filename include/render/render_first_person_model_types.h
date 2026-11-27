#ifndef RENDER_RENDER_FIRST_PERSON_MODEL_TYPES_H
#define RENDER_RENDER_FIRST_PERSON_MODEL_TYPES_H

#include "math/matrix_math_types.h"
#include "memory/data_types.h"

struct render_first_person_model
{
    long render_model_index;
    long object_index;
    unsigned long flags;
    real_matrix4x3 nodes[64];
};

#endif // RENDER_RENDER_FIRST_PERSON_MODEL_TYPES_H
