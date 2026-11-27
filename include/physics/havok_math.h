#ifndef PHYSICS_HAVOK_MATH_H
#define PHYSICS_HAVOK_MATH_H

#include "phys/phys_base.h"
#include "math/matrix_math_types.h"

#include "physics/havok_math_types.h"

static __forceinline void phys_vec4_from_real_point3d(phys_vec4 *v, const real_point3d *point)
{
    phys_simd_vec_union components;
    components.r[0] = point->x;
    components.r[1] = point->y;
    components.r[2] = point->z;
    components.r[3] = 0.0f;
    v->m_simd_quad = components.q;
}

static __forceinline void phys_vec4_from_real_vector3d(phys_vec4 *v, const real_vector3d *vector)
{
    phys_simd_vec_union components;
    components.r[0] = vector->i;
    components.r[1] = vector->j;
    components.r[2] = vector->k;
    components.r[3] = 0.0f;
    v->m_simd_quad = components.q;
}

static __forceinline void real_vector3d_from_hkVector4(real_vector3d *vector, const phys_vec4 *v)
{
    vector->i = (*v)(0);
    vector->j = (*v)(1);
    vector->k = (*v)(2);
}

static __forceinline void matrix4x3_from_hkTransform(real_matrix4x3 *matrix, const phys_transform *transform)
{
    matrix->scale = 1.0f;
    matrix->forward.i = transform->m_rotation.m_column0(0);
    matrix->forward.j = transform->m_rotation.m_column0(1);
    matrix->forward.k = transform->m_rotation.m_column0(2);
    matrix->left.i = transform->m_rotation.m_column1(0);
    matrix->left.j = transform->m_rotation.m_column1(1);
    matrix->left.k = transform->m_rotation.m_column1(2);
    matrix->up.i = transform->m_rotation.m_column2(0);
    matrix->up.j = transform->m_rotation.m_column2(1);
    matrix->up.k = transform->m_rotation.m_column2(2);
    matrix->position.x = transform->m_translation(0);
    matrix->position.y = transform->m_translation(1);
    matrix->position.z = transform->m_translation(2);
}

#endif // PHYSICS_HAVOK_MATH_H
