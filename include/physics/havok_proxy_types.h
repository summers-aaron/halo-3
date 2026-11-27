#ifndef PHYSICS_HAVOK_PROXY_TYPES_H
#define PHYSICS_HAVOK_PROXY_TYPES_H

#include "math/matrix_math_types.h"
class phys_rigid_body;

struct s_havok_proxy_datum
{
    long salt;
    phys_rigid_body *body;
    real_matrix4x3 matrix;
    long object_index;
    long creation_time;
};

#endif // PHYSICS_HAVOK_PROXY_TYPES_H
