// blamlibxbox/matrix_math.obj: from the records 0x9d068 0x9d0d0 0x9d1a0 0x9fb10 0x9fbf0 0xade58 0xbaae8 0xdb5a8 0xdb610 0xdbb28 0xed598 0xf07d0 0xf25a0 0xf2df0 0xf2fc0 0xf3050 0x107dd8 0x10ae10 0x10b0b8 0x116558 0x11a5f8 0x11a738 0x12bec8 0x12c528 0x1545c0 0x19e1f0 0x1ec898 0x1ecba0 0x1eccd0 0x1ecd58 0x1ece48 0x1ecf88 0x1ed030 0x313c18
// @flags /TP /O2
// @address 0x9d0d0 ?matrix3x3_rotation_from_quaternion@@YAPAUreal_matrix3x3@@PAU1@PBUreal_quaternion@@@Z
// @address 0x10ae10 ?matrix4x3_from_point_and_nonaxial_forward_and_left@@YAXPAUreal_matrix4x3@@PBTreal_point3d@@PBTreal_vector3d@@2@Z
// @address 0x10b0b8 ?matrix4x3_from_point_and_nonaxial_vectors@@YAXPAUreal_matrix4x3@@PBTreal_point3d@@PBTreal_vector3d@@2@Z
// @address 0x1eccd0 ?matrix4x3_from_point_and_nonaxial_vectors_using_up@@YAXPAUreal_matrix4x3@@PBTreal_point3d@@PBTreal_vector3d@@2@Z
// @address 0x9d1a0 matrix4x3_compute_product
// @address 0x9fb10 ?matrix4x3_inverse_transform_point@@YAPATreal_point3d@@PBUreal_matrix4x3@@PBT1@PAT1@@Z
// @address 0x9fbf0 ?matrix4x3_inverse_transform_vector@@YAPATreal_vector3d@@PBUreal_matrix4x3@@PBT1@PAT1@@Z
// @address 0xade58 ?matrix4x3_transform_point@@YAPATreal_point3d@@PBUreal_matrix4x3@@PBT1@PAT1@@Z
// @address 0xbaae8 ?matrix4x3_transform_plane@@YAPAUreal_plane3d@@PBUreal_matrix4x3@@PBU1@PAU1@@Z
// @address 0xdb5a8 ?matrix4x3_to_orientation@@YAXPBUreal_matrix4x3@@PAUreal_orientation@@@Z
// @address 0xdb610 ?matrix3x3_rotation_to_quaternion@@YAPAUreal_quaternion@@PBUreal_matrix3x3@@PAU1@@Z
// @address 0xed598 ?matrix4x3_transform_vector@@YAPATreal_vector3d@@PBUreal_matrix4x3@@PBT1@PAT1@@Z
// @address 0xf07d0 ?matrix4x3_rotation_from_vectors@@YAXPAUreal_matrix4x3@@PBTreal_vector3d@@1@Z
// @address 0xf25a0 ?matrix4x3_inverse@@YAXPBUreal_matrix4x3@@PAU1@@Z
// @address 0xf2df0 ?matrix3x3_multiply@@YAPAUreal_matrix3x3@@PBU1@0PAU1@@Z
// @address 0xf3050 ?matrix3x3_transform_vector@@YAPATreal_vector3d@@PBUreal_matrix3x3@@PBT1@PAT1@@Z
// @address 0x116558 ?matrix4x3_rotation_from_angles@@YAXPAUreal_matrix4x3@@MMM@Z
// @address 0x12bec8 ?matrix3x3_from_axis_and_angle@@YAPAUreal_matrix3x3@@PAU1@PBTreal_vector3d@@MM@Z
// @address 0x12c528 ?matrix4x3_rotation_from_axis_and_angle@@YAXPAUreal_matrix4x3@@PBTreal_vector3d@@MM@Z
// @address 0x19e1f0 matrix4x3_rotation_to_angles
// @address 0x1ecba0 ?matrix3x3_normalize@@YA_NPAUreal_matrix3x3@@@Z
// @address 0x1ecd58 ?matrix4x3_inverse_transform_plane@@YAPAUreal_plane3d@@PBUreal_matrix4x3@@PBU1@PAU1@@Z
// @address 0x1ece48 ?matrix3x3_inverse@@YAPAUreal_matrix3x3@@PBU1@MPAU1@@Z
// @address 0x1ecf88 ?matrix3x3_inverse_transform_vector@@YAPATreal_vector3d@@PBUreal_matrix3x3@@PBT1@PAT1@@Z
#include "math/xbox_math.h"
#include "math/matrix_math.h"
#include "math/real_math_globals.h"
#include "math/real_math.h"
#include <vectorintrinsics.h>
#include "math/matrix_math_constants.h"
#include "math/real_math_trigonometry.h"
// @address 0x11a738 ?XMVectorSinCos@@YAXPAU__vector4@@0U1@@Z

#include "math/real_matrix3x4_types.h"

const float k_literal_f3f800000 = 1.0f;
const float k_literal_f00000000 = 0.0f;

// @rdata 0x820394a0..0x820394f0
static const __vector4 matrix_math_one = { 1.0f, 1.0f, 1.0f, 1.0f };
static const __vector4 matrix_math_two = { 2.0f, 2.0f, 2.0f, 2.0f };
static const unsigned long matrix_math_product_permutes[3][4] = {
    { 0x00010203, 0x04050607, 0x08090a0b, 0x10111213 },
    { 0x04050607, 0x08090a0b, 0x10111213, 0x14151617 },
    { 0x08090a0b, 0x10111213, 0x14151617, 0x18191a1b }
};
// @rdata-end

// match: 0x107dd8: visits matrix_math_two after the permute loads so 0x394b0 stays the shared
// lis/addi anchor (one -16, permutes +16..+48); a direct load re-anchors on the permutes.
static __forceinline __vector4 matrix_math_get_two()
{
    return matrix_math_two;
}

// @address 0x9d068 ?matrix4x3_from_point_and_quaternion@@YAXPAUreal_matrix4x3@@PBTreal_point3d@@PBUreal_quaternion@@@Z
void matrix4x3_from_point_and_quaternion(real_matrix4x3 *matrix, const real_point3d *point, const real_quaternion *quaternion)
{
    matrix4x3_rotation_from_quaternion(matrix, quaternion);
    matrix->position = *point;
}

// match: 0x9d0d0: keeps the quaternion norm/scale chain in fr11/fr8/fr10; removal recolors through fr0, sinks the epsilon load and adds 3 component reloads.
real_matrix3x3 *matrix3x3_rotation_from_quaternion(real_matrix3x3 *matrix, const real_quaternion *__restrict quaternion)
{
    float norm = quaternion->v.n[0] * quaternion->v.n[0] + quaternion->v.n[1] * quaternion->v.n[1] +
        quaternion->v.n[2] * quaternion->v.n[2] + quaternion->w * quaternion->w;
    float scale = norm > k_real_epsilon ? 2.0f / norm : 0.0f;
    float is = quaternion->v.n[0] * scale;
    float js = quaternion->v.n[1] * scale;
    float ks = quaternion->v.n[2] * scale;
    float wi = quaternion->w * is;
    float wj = quaternion->w * js;
    float wk = quaternion->w * ks;
    float ii = quaternion->v.n[0] * is;
    float ij = quaternion->v.n[0] * js;
    float ik = quaternion->v.n[0] * ks;
    float jj = quaternion->v.n[1] * js;
    float jk = quaternion->v.n[1] * ks;
    float kk = quaternion->v.n[2] * ks;
    matrix->forward.i = 1.0f - (jj + kk);
    matrix->left.i = ij - wk;
    matrix->up.i = ik + wj;
    matrix->forward.j = ij + wk;
    matrix->left.j = 1.0f - (ii + kk);
    matrix->up.j = jk - wi;
    matrix->forward.k = ik - wj;
    matrix->left.k = jk + wi;
    matrix->up.k = 1.0f - (ii + jj);
    return matrix;
}

// matrix4x3_multiply
extern "C" void matrix4x3_compute_product(const real_matrix4x3 *matrix1, const real_matrix4x3 *matrix2, real_matrix4x3 *result)
{
    __vector4 permute_forward = __lvx(matrix_math_product_permutes[0], 0);
    __vector4 permute_left = __lvx(matrix_math_product_permutes[1], 0);
    __vector4 permute_up = __lvx(matrix_math_product_permutes[2], 0);

    __vector4 m1_scale_forward = __loadunalignedvector(&matrix1->scale);
    __vector4 m2_scale_forward = __loadunalignedvector(&matrix2->scale);
    __vector4 m2_left = __loadunalignedvector(&matrix2->rotation.left);
    __vector4 m1_left = __loadunalignedvector(&matrix1->rotation.left);
    __vector4 m2_up = __loadunalignedvector(&matrix2->rotation.up);
    __vector4 m1_up = __loadunalignedvector(&matrix1->rotation.up);
    __vector4 m2_position = __loadunalignedvector(&matrix2->rotation.up.k);
    __vector4 m1_position = __loadunalignedvector(&matrix1->rotation.up.k);

    __vector4 m1_forward = __vpermwi(m1_scale_forward, VPERMWI_CONST(1, 2, 3, 0));
    m1_position = __vpermwi(m1_position, VPERMWI_CONST(1, 2, 3, 0));
    __vector4 m1_scale = __vspltw(m1_scale_forward, 0);

    __vector4 scale = __vmulfp(m1_scale, __vspltw(m2_scale_forward, 0));
    __vector4 forward = __vmaddfp(m1_up, __vspltw(m2_scale_forward, 3),
        __vmaddfp(m1_left, __vspltw(m2_scale_forward, 2), __vmulfp(m1_forward, __vspltw(m2_scale_forward, 1))));
    __vector4 left = __vmaddfp(m1_up, __vspltw(m2_left, 2),
        __vmaddfp(m1_left, __vspltw(m2_left, 1), __vmulfp(m1_forward, __vspltw(m2_left, 0))));
    __vector4 up = __vmaddfp(m1_up, __vspltw(m2_up, 2),
        __vmaddfp(m1_left, __vspltw(m2_up, 1), __vmulfp(m1_forward, __vspltw(m2_up, 0))));
    __vector4 position = __vmaddfp(m1_up, __vspltw(m2_position, 3),
        __vmaddfp(m1_left, __vspltw(m2_position, 2), __vmulfp(m1_forward, __vspltw(m2_position, 1))));
    position = __vmaddfp(position, m1_scale, m1_position);

    __vector4 out_forward = __vperm(forward, left, permute_forward);
    __vector4 out_left = __vperm(left, up, permute_left);
    __vector4 out_up = __vperm(up, position, permute_up);
    __stvewx(scale, &result->scale, 0);
    __storeunalignedvector(out_forward, &result->rotation.forward);
    __storeunalignedvector(out_left, &result->rotation.left.j);
    __storeunalignedvector(out_up, &result->rotation.up.k);
}

real_point3d *matrix4x3_inverse_transform_point(const real_matrix4x3 *matrix, const real_point3d *point, real_point3d *result)
{
    float x = point->x - matrix->position.x;
    float y = point->y - matrix->position.y;
    float z = point->z - matrix->position.z;
    if (matrix->scale != 1.0f) {
        float inverse_scale = 1.0f / (matrix->scale >= 0.0f ? MAX(matrix->scale, k_real_epsilon) :
            (matrix->scale > rasterizer_states_mode_7_depth_bias ?
                rasterizer_states_mode_7_depth_bias : matrix->scale));
        x *= inverse_scale;
        y *= inverse_scale;
        z *= inverse_scale;
    }
    result->x = x * matrix->rotation.forward.i + y * matrix->rotation.forward.j + z * matrix->rotation.forward.k;
    result->y = x * matrix->rotation.left.i + y * matrix->rotation.left.j + z * matrix->rotation.left.k;
    result->z = x * matrix->rotation.up.i + y * matrix->rotation.up.j + z * matrix->rotation.up.k;
    return result;
}

real_vector3d *matrix4x3_inverse_transform_vector(const real_matrix4x3 *matrix, const real_vector3d *vector, real_vector3d *result)
{
    float i = vector->i;
    float j = vector->j;
    float k = vector->k;
    if (matrix->scale != 1.0f) {
        float inverse_scale = 1.0f / (matrix->scale >= 0.0f ? MAX(matrix->scale, k_real_epsilon) :
            (matrix->scale > rasterizer_states_mode_7_depth_bias ?
                rasterizer_states_mode_7_depth_bias : matrix->scale));
        i *= inverse_scale;
        j *= inverse_scale;
        k *= inverse_scale;
    }
    result->i = i * matrix->rotation.forward.i + j * matrix->rotation.forward.j + k * matrix->rotation.forward.k;
    result->j = i * matrix->rotation.left.i + j * matrix->rotation.left.j + k * matrix->rotation.left.k;
    result->k = i * matrix->rotation.up.i + j * matrix->rotation.up.j + k * matrix->rotation.up.k;
    return result;
}

real_point3d *matrix4x3_transform_point(const real_matrix4x3 *matrix, const real_point3d *point, real_point3d *result)
{
    float x = point->x;
    float y = point->y;
    float z = point->z;
    x *= matrix->scale;
    y *= matrix->scale;
    z *= matrix->scale;
    result->x = x * matrix->rotation.forward.i + y * matrix->rotation.left.i + z * matrix->rotation.up.i + matrix->position.x;
    result->y = x * matrix->rotation.forward.j + y * matrix->rotation.left.j + z * matrix->rotation.up.j + matrix->position.y;
    result->z = x * matrix->rotation.forward.k + y * matrix->rotation.left.k + z * matrix->rotation.up.k + matrix->position.z;
    return result;
}

// Transform a plane by a scaled rigid matrix.
real_plane3d *matrix4x3_transform_plane(const real_matrix4x3 *matrix, const real_plane3d *plane, real_plane3d *result)
{
  float i = plane->normal.i;
  float j = plane->normal.j;
  float k = plane->normal.k;
  result->normal.i = i * matrix->rotation.forward.i + j * matrix->rotation.left.i + k * matrix->rotation.up.i;
  result->normal.j = i * matrix->rotation.forward.j + j * matrix->rotation.left.j + k * matrix->rotation.up.j;
  result->normal.k = i * matrix->rotation.forward.k + j * matrix->rotation.left.k + k * matrix->rotation.up.k;
  result->distance = plane->distance * matrix->scale +
      result->normal.i * matrix->position.x +
      result->normal.j * matrix->position.y +
      result->normal.k * matrix->position.z;
  return result;
}

void matrix4x3_to_orientation(const real_matrix4x3 *matrix, real_orientation *orientation)
{
  matrix3x3_rotation_to_quaternion(&matrix->rotation, &orientation->rotation);
  orientation->translation = matrix->position;
  orientation->scale = matrix->scale;
}

// match: 0xdb610: pins the trace-compare chain to fr9/fr10; removal swaps the colors and flips an fmuls operand pair.
real_quaternion *matrix3x3_rotation_to_quaternion(const real_matrix3x3 *matrix, real_quaternion * __restrict quaternion)
{
  float trace = matrix->m[0][0] + matrix->m[1][1] + matrix->m[2][2];
  if (trace > k_real_zero) {
    trace = trace + k_real_one;
    float di = matrix->m[1][2] - matrix->m[2][1];
    float dj = matrix->m[2][0] - matrix->m[0][2];
    float dk = matrix->m[0][1] - matrix->m[1][0];
    float s = sqrtf(trace) * k_real_half;
    float t = 0.25f / s;
    quaternion->w = s;
    quaternion->v.n[0] = di * t;
    quaternion->v.n[1] = dj * t;
    quaternion->v.n[2] = dk * t;
  } else {
    int i = 0;
    if (matrix->m[1][1] > matrix->m[0][0])
      i = 1;
    if (matrix->m[2][2] > matrix->m[i][i])
      i = 2;
    int j = (i + 1) % 3;
    int k = (i + 2) % 3;
    float d = matrix->m[i][i] - matrix->m[j][j] - matrix->m[k][k] + k_real_one;
    float sj = matrix->m[i][j] + matrix->m[j][i];
    float sk = matrix->m[i][k] + matrix->m[k][i];
    float s = sqrtf(d) * k_real_half;
    float dw = matrix->m[j][k] - matrix->m[k][j];
    quaternion->v.n[i] = s;
    float t = 0.25f / s;
    quaternion->w = dw * t;
    quaternion->v.n[j] = sj * t;
    quaternion->v.n[k] = sk * t;
  }
  if (quaternion->w < k_real_zero) {
    quaternion->w = -quaternion->w;
    quaternion->v.n[0] = -quaternion->v.n[0];
    quaternion->v.n[1] = -quaternion->v.n[1];
    quaternion->v.n[2] = -quaternion->v.n[2];
  }
  return quaternion;
}

// @address 0xdbb28 ?matrix4x3_from_orientation@@YAXPAUreal_matrix4x3@@PBUreal_orientation@@@Z
void matrix4x3_from_orientation(real_matrix4x3 *matrix, const real_orientation *orientation)
{
    matrix3x3_rotation_from_quaternion(&matrix->rotation, &orientation->rotation);
    matrix->position = orientation->translation;
    matrix->scale = orientation->scale;
}

// Vector through a scaled rigid matrix (no translation).
real_vector3d *matrix4x3_transform_vector(const real_matrix4x3 *matrix, const real_vector3d *vector, real_vector3d *result)
{
    float x = vector->i;
    float y = vector->j;
    float z = vector->k;
    if (matrix->scale != 1.0f) {
        x = x * matrix->scale;
        y = y * matrix->scale;
        z = z * matrix->scale;
    }
    result->i = x * matrix->rotation.forward.i + y * matrix->rotation.left.i + z * matrix->rotation.up.i;
    result->j = x * matrix->rotation.forward.j + y * matrix->rotation.left.j + z * matrix->rotation.up.j;
    result->k = x * matrix->rotation.forward.k + y * matrix->rotation.left.k + z * matrix->rotation.up.k;
    return result;
}

void matrix4x3_rotation_from_vectors(real_matrix4x3 *matrix, const real_vector3d *forward, const real_vector3d *up)
{
    matrix->scale = k_literal_f3f800000;
    matrix->rotation.forward = *forward;
    cross_product3d(up, forward, &matrix->rotation.left);
    matrix->rotation.up = *up;
    set_real_point3d(&matrix->position, k_literal_f00000000, k_literal_f00000000, k_literal_f00000000);
}

void matrix4x3_inverse(const real_matrix4x3 *matrix, real_matrix4x3 *result)
{
    real_vector3d inverse_position;
    inverse_position.i = -matrix->position.x;
    inverse_position.j = -matrix->position.y;
    inverse_position.k = -matrix->position.z;
    if (matrix->scale != 1.0f) {
        result->scale = 1.0f / (matrix->scale >= 0.0f ? MAX(matrix->scale, k_real_epsilon) :
            (matrix->scale > rasterizer_states_mode_7_depth_bias ?
                rasterizer_states_mode_7_depth_bias : matrix->scale));
        inverse_position.i *= result->scale;
        inverse_position.j *= result->scale;
        inverse_position.k *= result->scale;
    } else {
        result->scale = 1.0f;
    }
    float temp;
    result->rotation.forward.i = matrix->rotation.forward.i;
    result->rotation.left.j = matrix->rotation.left.j;
    result->rotation.up.k = matrix->rotation.up.k;
    temp = matrix->rotation.left.i;
    result->rotation.left.i = matrix->rotation.forward.j;
    result->rotation.forward.j = temp;
    temp = matrix->rotation.up.i;
    result->rotation.up.i = matrix->rotation.forward.k;
    result->rotation.forward.k = temp;
    temp = matrix->rotation.up.j;
    result->rotation.up.j = matrix->rotation.left.k;
    result->rotation.left.k = temp;
    result->position.x = inverse_position.i * result->rotation.forward.i + inverse_position.j * result->rotation.left.i +
        inverse_position.k * result->rotation.up.i;
    result->position.y = inverse_position.i * result->rotation.forward.j + inverse_position.j * result->rotation.left.j +
        inverse_position.k * result->rotation.up.j;
    result->position.z = inverse_position.i * result->rotation.forward.k + inverse_position.j * result->rotation.left.k +
        inverse_position.k * result->rotation.up.k;
}

real_matrix3x3 *matrix3x3_multiply(const real_matrix3x3 *a, const real_matrix3x3 *b, real_matrix3x3 *out)
{
    real_matrix3x3 temp;
    if (a == out) {
        temp = *a;
        a = &temp;
    }
    if (b == out) {
        temp = *b;
        b = &temp;
    }
    out->forward.i = a->forward.i * b->forward.i + a->left.i * b->forward.j + a->up.i * b->forward.k;
    out->forward.j = a->forward.j * b->forward.i + a->left.j * b->forward.j + a->up.j * b->forward.k;
    out->forward.k = a->forward.k * b->forward.i + a->left.k * b->forward.j + a->up.k * b->forward.k;
    out->left.i = a->forward.i * b->left.i + a->left.i * b->left.j + a->up.i * b->left.k;
    out->left.j = a->forward.j * b->left.i + a->left.j * b->left.j + a->up.j * b->left.k;
    out->left.k = a->forward.k * b->left.i + a->left.k * b->left.j + a->up.k * b->left.k;
    out->up.i = a->forward.i * b->up.i + a->left.i * b->up.j + a->up.i * b->up.k;
    out->up.j = a->forward.j * b->up.i + a->left.j * b->up.j + a->up.j * b->up.k;
    out->up.k = a->forward.k * b->up.i + a->left.k * b->up.j + a->up.k * b->up.k;
    return out;
}

real_matrix3x3 *matrix3x3_transpose(const real_matrix3x3 *matrix, real_matrix3x3 *result)
{
    if (matrix == result) {
        float forward_j = matrix->forward.j;
        result->forward.j = matrix->left.i;
        result->left.i = forward_j;
        float forward_k = matrix->forward.k;
        result->forward.k = matrix->up.i;
        result->up.i = forward_k;
        float left_k = matrix->left.k;
        result->left.k = matrix->up.j;
        result->up.j = left_k;
    } else {
        result->forward.i = matrix->forward.i;
        result->forward.j = matrix->left.i;
        result->forward.k = matrix->up.i;
        result->left.i = matrix->forward.j;
        result->left.j = matrix->left.j;
        result->left.k = matrix->up.j;
        result->up.i = matrix->forward.k;
        result->up.j = matrix->left.k;
        result->up.k = matrix->up.k;
    }
    return result;
}

real_vector3d *matrix3x3_transform_vector(const real_matrix3x3 *matrix, const real_vector3d *vector, real_vector3d *result)
{
    real_vector3d temp;
    if (vector == result) {
        temp = *vector;
        vector = &temp;
    }
    result->i = matrix->forward.i * vector->i + matrix->up.i * vector->k + matrix->left.i * vector->j;
    result->j = matrix->left.j * vector->j + matrix->up.j * vector->k + matrix->forward.j * vector->i;
    result->k = matrix->left.k * vector->j + matrix->up.k * vector->k + matrix->forward.k * vector->i;
    return result;
}

static __forceinline void matrix4x3_rotation_from_nonaxial_forward_and_left(real_matrix4x3 *matrix, const real_vector3d *forward, const real_vector3d *left)
{
    matrix->scale = 1.0f;
    set_real_point3d(&matrix->position, 0.0f, 0.0f, 0.0f);
    real_vector3d_build_axes_from_forward_and_left(forward, left, &matrix->rotation.forward, &matrix->rotation.left, &matrix->rotation.up);
}

void matrix4x3_from_point_and_nonaxial_forward_and_left(
    real_matrix4x3 *matrix, const real_point3d *point, const real_vector3d *forward, const real_vector3d *left)
{
    matrix4x3_rotation_from_nonaxial_forward_and_left(matrix, forward, left);
    matrix->position = *point;
}

static __forceinline void matrix4x3_rotation_from_nonaxial_vectors(real_matrix4x3 *matrix, const real_vector3d *forward, const real_vector3d *up)
{
    matrix->scale = 1.0f;
    set_real_point3d(&matrix->position, 0.0f, 0.0f, 0.0f);
    real_vector3d_build_axes_from_vectors(forward, up, &matrix->rotation.forward, &matrix->rotation.left, &matrix->rotation.up);
}

void matrix4x3_from_point_and_nonaxial_vectors(real_matrix4x3 *matrix, const real_point3d *point, const real_vector3d *forward, const real_vector3d *up)
{
    matrix4x3_rotation_from_nonaxial_vectors(matrix, forward, up);
    matrix->position = *point;
}


// Rotation matrix from three Euler angles, applied in the order z, then y, then x.
void matrix4x3_rotation_from_angles(real_matrix4x3 *matrix, float x_angle, float y_angle, float z_angle)
{
    float cos_z = cosine(z_angle);
    float sin_z = sine(z_angle);
    float cos_y = cosine(y_angle);
    float sin_y = sine(y_angle);
    float cos_x = cosine(x_angle);
    float sin_x = sine(x_angle);
    float cos_z_sin_y = cos_z * sin_y;
    float sin_z_sin_y = sin_z * sin_y;
    matrix->scale = 1.0f;
    matrix->rotation.m[0][0] = cos_y * cos_x;
    matrix->rotation.m[0][1] = -sin_z_sin_y * cos_x + cos_z * sin_x;
    matrix->rotation.m[0][2] = cos_z_sin_y * cos_x + sin_z * sin_x;
    matrix->rotation.m[1][0] = -cos_y * sin_x;
    matrix->rotation.m[1][1] = sin_z_sin_y * sin_x + cos_z * cos_x;
    matrix->rotation.m[1][2] = -cos_z_sin_y * sin_x + sin_z * cos_x;
    matrix->rotation.m[2][0] = -sin_y;
    matrix->rotation.m[2][1] = -sin_z * cos_y;
    matrix->rotation.m[2][2] = cos_z * cos_y;
    matrix->position.x = 0.0f;
    matrix->position.y = 0.0f;
    matrix->position.z = 0.0f;
}

// Rotation matrix about a unit axis, as a 3x3 matrix, from the angle's sine and cosine.
real_matrix3x3 *matrix3x3_from_axis_and_angle(real_matrix3x3 *matrix, const real_vector3d *axis, float sine, float cosine)
{
    float xx = axis->i * axis->i;
    float yy = axis->j * axis->j;
    float zz = axis->k * axis->k;
    float sx = sine * axis->i;
    float sy = sine * axis->j;
    float sz = sine * axis->k;
    matrix->m[0][0] = xx + cosine * (1.0f - xx);
    matrix->m[0][1] = axis->i * axis->j * (1.0f - cosine);
    matrix->m[1][0] = matrix->m[0][1];
    matrix->m[1][0] = matrix->m[1][0] - sz;
    matrix->m[0][1] = matrix->m[0][1] + sz;
    matrix->m[1][1] = yy + cosine * (1.0f - yy);
    matrix->m[0][2] = axis->k * axis->i * (1.0f - cosine);
    matrix->m[2][0] = matrix->m[0][2];
    matrix->m[2][0] = matrix->m[2][0] + sy;
    matrix->m[0][2] = matrix->m[0][2] - sy;
    matrix->m[2][2] = zz + cosine * (1.0f - zz);
    matrix->m[1][2] = axis->j * axis->k * (1.0f - cosine);
    matrix->m[2][1] = matrix->m[1][2];
    matrix->m[2][1] = matrix->m[2][1] - sx;
    matrix->m[1][2] = matrix->m[1][2] + sx;
    return matrix;
}

// Rotation matrix about a unit axis, from the angle's sine and cosine.
void matrix4x3_rotation_from_axis_and_angle(real_matrix4x3 *matrix, const real_vector3d *axis, float sine, float cosine)
{
    float xx = axis->i * axis->i;
    float yy = axis->j * axis->j;
    float zz = axis->k * axis->k;
    float sx = sine * axis->i;
    float sy = sine * axis->j;
    float sz = sine * axis->k;
    matrix->scale = 1.0f;
    matrix->rotation.m[0][0] = xx + cosine * (1.0f - xx);
    matrix->rotation.m[0][1] = axis->i * axis->j * (1.0f - cosine);
    matrix->rotation.m[1][0] = matrix->rotation.m[0][1];
    matrix->rotation.m[1][0] = matrix->rotation.m[1][0] - sz;
    matrix->rotation.m[0][1] = matrix->rotation.m[0][1] + sz;
    matrix->rotation.m[1][1] = yy + cosine * (1.0f - yy);
    matrix->rotation.m[0][2] = axis->k * axis->i * (1.0f - cosine);
    matrix->rotation.m[2][0] = matrix->rotation.m[0][2];
    matrix->rotation.m[2][0] = matrix->rotation.m[2][0] + sy;
    matrix->rotation.m[0][2] = matrix->rotation.m[0][2] - sy;
    matrix->rotation.m[2][2] = zz + cosine * (1.0f - zz);
    matrix->rotation.m[1][2] = axis->j * axis->k * (1.0f - cosine);
    matrix->rotation.m[2][1] = matrix->rotation.m[1][2];
    matrix->rotation.m[2][1] = matrix->rotation.m[2][1] - sx;
    matrix->rotation.m[1][2] = matrix->rotation.m[1][2] + sx;
    matrix->position.z = 0.0f;
    matrix->position.y = 0.0f;
    matrix->position.x = 0.0f;
}

namespace {
static __forceinline __vector4 transform_point_vector(__vector4 point, __vector4 scale,
    __vector4 forward, __vector4 left, __vector4 up, __vector4 position)
{
    __vector4 scaled_point = __vmulfp(point, scale);
    __vector4 result = __vmaddfp(__vspltw(scaled_point, 0), forward, position);
    result = __vmaddfp(__vspltw(scaled_point, 1), left, result);
    result = __vmaddfp(__vspltw(scaled_point, 2), up, result);
    return result;
}
static __forceinline void store_point_vector(real_point3d *point, __vector4 value)
{
    __vector4 x = __vspltw(value, 0);
    __vector4 y = __vspltw(value, 1);
    __vector4 z = __vspltw(value, 2);
    __stvewx(x, point, 0);
    __stvewx(y, point, 4);
    __stvewx(z, point, 8);
}
}
// @address 0x1545c0 ?matrix4x3_transform_points@@YAXPBUreal_matrix4x3@@JQBTreal_point3d@@QAT2@@Z
void matrix4x3_transform_points(const real_matrix4x3 *matrix, long point_count,
    const real_point3d points[], real_point3d transformed_points[])
{
    __vector4 scale_and_forward = __loadunalignedvector(&matrix->scale);
    __vector4 scale = __vspltw(scale_and_forward, 0);
    __vector4 forward = __vpermwi(scale_and_forward, VPERMWI_CONST(1, 2, 3, 0));
    __vector4 left = __loadunalignedvector(&matrix->rotation.left);
    __vector4 up = __loadunalignedvector(&matrix->rotation.up);
    __vector4 position = __vpermwi(__loadunalignedvector(&matrix->rotation.up.k), VPERMWI_CONST(1, 2, 3, 0));
    long batch_count = point_count >> 2;
    for (long batch_index = 0; batch_index < batch_count; ++batch_index) {
        __vector4 point0 = transform_point_vector(__loadunalignedvector(&points[batch_index * 4]), scale, forward, left, up, position);
        __vector4 point1 = transform_point_vector(__loadunalignedvector(&points[batch_index * 4 + 1]), scale, forward, left, up, position);
        __vector4 point2 = transform_point_vector(__loadunalignedvector(&points[batch_index * 4 + 2]), scale, forward, left, up, position);
        __vector4 point3 = transform_point_vector(__loadunalignedvector(&points[batch_index * 4 + 3]), scale, forward, left, up, position);
        store_point_vector(&transformed_points[batch_index * 4], point0);
        store_point_vector(&transformed_points[batch_index * 4 + 1], point1);
        store_point_vector(&transformed_points[batch_index * 4 + 2], point2);
        store_point_vector(&transformed_points[batch_index * 4 + 3], point3);
    }
    for (long point_index = batch_count * 4; point_index < point_count; ++point_index) {
        __vector4 point = transform_point_vector(__loadunalignedvector(&points[point_index]), scale, forward, left, up, position);
        store_point_vector(&transformed_points[point_index], point);
    }
}



// quaternion_to_euler_angles: yaw, pitch and roll of a quaternion (roll comes from the asin term, zero for a null quaternion).
// match: 0x19e1f0: keeps the yaw/pitch stores below the quaternion loads; removal hoists the stores above the loads and recolors the chain through fr0/fr13.
extern "C" void matrix4x3_rotation_to_angles(const real_quaternion *quaternion, real_euler_angles3d *__restrict angles)
{
    float w2 = quaternion->w * quaternion->w;
    float i2 = quaternion->v.n[0] * quaternion->v.n[0];
    float j2 = quaternion->v.n[1] * quaternion->v.n[1];
    float k2 = quaternion->v.n[2] * quaternion->v.n[2];
    angles->yaw = arctangent(2.0f * (quaternion->v.n[2] * quaternion->w) + 2.0f * (quaternion->v.n[1] * quaternion->v.n[0]), i2 - j2 - k2 + w2);
    angles->pitch = arctangent(2.0f * (quaternion->v.n[0] * quaternion->w) + 2.0f * (quaternion->v.n[2] * quaternion->v.n[1]), k2 - (j2 + i2) + w2);
    float sine = (quaternion->v.n[2] * quaternion->v.n[0] - quaternion->v.n[1] * quaternion->w) * -2.0f;
    angles->roll = 0.0f;
    float norm = k2 + j2 + i2 + w2;
    if (norm != 0.0f) {
        angles->roll = (float)asin((double)MIN((-1.0f >= sine / norm ? -1.0f : sine / norm), 1.0f));
    }
}

// Rotation taking unit vector `from` onto unit vector `to`, applied to the identity basis; a degenerate basis is repaired.
// @address 0x1ec898 ?matrix4x3_rotation_between_vectors@@YAXPAUreal_matrix4x3@@PBTreal_vector3d@@1@Z
void matrix4x3_rotation_between_vectors(real_matrix4x3 *matrix, const real_vector3d *from, const real_vector3d *to)
{
    float cosine = from->j * to->j + to->i * from->i + from->k * to->k;
    cosine = (cosine > -1.0f ? cosine : -1.0f) < 1.0f ? (cosine > -1.0f ? cosine : -1.0f) : 1.0f;
    float half_cosine = sqrtf(0.5f * (1.0f + cosine));
    float half_sine = sqrtf(0.5f * (1.0f - cosine));
    float sine = half_sine * half_cosine * 2.0f;
    real_quaternion rotation;
    if (sine != 0.0f)
    {
        cross_product3d(from, to, &rotation.v);
        scale_vector3d(&rotation.v, half_sine / sine, &rotation.v);
        rotation.w = half_cosine;
    }
    else if (cosine < 0.0f)
    {
        perpendicular3d(from, &rotation.v);
        rotation.w = 0.0f;
    }
    else
    {
        rotation = *global_identity_quaternion;
    }
    memcpy(matrix, global_identity4x3, sizeof(*matrix));
    quaternion_transform_point(&rotation, &matrix->rotation.forward_as_point, &matrix->rotation.forward_as_point);
    quaternion_transform_point(&rotation, &matrix->rotation.up_as_point, &matrix->rotation.up_as_point);
    quaternion_transform_point(&rotation, &matrix->rotation.left_as_point, &matrix->rotation.left_as_point);
    if (!valid_real_vector3d_axes3(&matrix->rotation.forward, &matrix->rotation.left, &matrix->rotation.up))
    {
        normalize3d(&matrix->rotation.up);
        cross_product3d(&matrix->rotation.up, &matrix->rotation.forward, &matrix->rotation.left);
        normalize3d(&matrix->rotation.left);
        cross_product3d(&matrix->rotation.left, &matrix->rotation.up, &matrix->rotation.forward);
    }
}


// Orthonormalize a matrix's rows: forward, then up made perpendicular to forward, then left from their cross product.
bool matrix3x3_normalize(real_matrix3x3 *matrix3x3)
{
    bool failed = false;
    failed |= normalize3d(&matrix3x3->forward) == 0.0f;
    normalize3d(&matrix3x3->up);
    float projection = dot_product3d(&matrix3x3->up, &matrix3x3->forward);
    real_vector3d scaled;
    scale_vector3d(&matrix3x3->forward, projection, &scaled);
    subtract_vectors3d(&matrix3x3->up, &scaled, &matrix3x3->up);
    failed |= normalize3d(&matrix3x3->up) == 0.0f;
    cross_product3d(&matrix3x3->up, &matrix3x3->forward, &matrix3x3->left);
    failed |= normalize3d(&matrix3x3->left) == 0.0f;
    return !failed;
}

static __forceinline void matrix4x3_rotation_from_nonaxial_vectors_using_up(real_matrix4x3 *matrix, const real_vector3d *forward, const real_vector3d *up)
{
    matrix->scale = 1.0f;
    set_real_point3d(&matrix->position, 0.0f, 0.0f, 0.0f);
    real_vector3d_build_axes_from_vectors_using_up(forward, up, &matrix->rotation.forward, &matrix->rotation.left, &matrix->rotation.up);
}

void matrix4x3_from_point_and_nonaxial_vectors_using_up(
    real_matrix4x3 *matrix, const real_point3d *point, const real_vector3d *forward, const real_vector3d *up)
{
    matrix4x3_rotation_from_nonaxial_vectors_using_up(matrix, forward, up);
    matrix->position = *point;
}

// Inverse of a scaled rigid transform applied to a plane.
real_plane3d *matrix4x3_inverse_transform_plane(const real_matrix4x3 *matrix, const real_plane3d *plane, real_plane3d *result)
{
    result->distance = plane->distance - dot_product3d(&plane->normal, &matrix->position);
    if (matrix->scale != 1.0f) {
        result->distance /= (matrix->scale >= 0.0f ? MAX(matrix->scale, k_real_epsilon) :
            (matrix->scale > rasterizer_states_mode_7_depth_bias ?
                rasterizer_states_mode_7_depth_bias : matrix->scale));
    }
    float i = plane->normal.i;
    float j = plane->normal.j;
    float k = plane->normal.k;
    result->normal.i = i * matrix->rotation.forward.i + j * matrix->rotation.forward.j + k * matrix->rotation.forward.k;
    result->normal.j = i * matrix->rotation.left.i + j * matrix->rotation.left.j + k * matrix->rotation.left.k;
    result->normal.k = i * matrix->rotation.up.i + j * matrix->rotation.up.j + k * matrix->rotation.up.k;
    return result;
}

// Inverse of a matrix whose determinant the caller supplies.
real_matrix3x3 *matrix3x3_inverse(const real_matrix3x3 *matrix3x3, float determinant, real_matrix3x3 *result)
{
    real_matrix3x3 copy;
    if (matrix3x3 == result)
    {
        copy = *matrix3x3;
        matrix3x3 = &copy;
    }
    float oodeterminant = 1.0f / determinant;
    for (short row = 0; row < 3; row++)
    {
        for (short column = 0; column < 3; column++)
        {
            short next_row = row < 2 ? row + 1 : 0;
            short previous_row = row > 0 ? row - 1 : 2;
            short next_column = column < 2 ? column + 1 : 0;
            short previous_column = column > 0 ? column - 1 : 2;
            result->m[column][row] = oodeterminant *
                (matrix3x3->m[next_row][next_column] * matrix3x3->m[previous_row][previous_column] -
                    matrix3x3->m[next_row][previous_column] * matrix3x3->m[previous_row][next_column]);
        }
    }
    return result;
}

real_vector3d *matrix3x3_inverse_transform_vector(const real_matrix3x3 *matrix, const real_vector3d *vector, real_vector3d *result)
{
    real_vector3d temp;
    if (vector == result) {
        temp = *vector;
        vector = &temp;
    }
    result->i = matrix->forward.i * vector->i + matrix->forward.j * vector->j + matrix->forward.k * vector->k;
    result->j = matrix->left.i * vector->i + matrix->left.j * vector->j + matrix->left.k * vector->k;
    result->k = matrix->up.i * vector->i + matrix->up.j * vector->j + matrix->up.k * vector->k;
    return result;
}


// @address 0x1ed030 ?valid_real_vector3d_axes3@@YA_NPBTreal_vector3d@@00@Z
bool valid_real_vector3d_axes3(const real_vector3d *a, const real_vector3d *b, const real_vector3d *c)
{
    return valid_real_normal3d(a) && valid_real_normal3d(b) && valid_real_normal3d(c)
        && valid_realcmp(dot_product3d(a, b), 0.0f)
        && valid_realcmp(dot_product3d(b, c), 0.0f)
        && valid_realcmp(dot_product3d(c, a), 0.0f);
}

static __forceinline void matrix_math_store_row(void *row, __vector4 value)
{
    __storeunalignedvector(value, row);
}

static __forceinline __vector4 matrix_math_rotate_basis(__vector4 forward, __vector4 left, __vector4 up, __vector4 vector)
{
    return __vmaddfp(up, __vspltw(vector, 2),
        __vmaddfp(left, __vspltw(vector, 1), __vmulfp(forward, __vspltw(vector, 0))));
}

// matrix4x3_multiply, stored transposed as a 3x4 matrix
// @address 0x313c18 matrix4x3_multiply_to_matrix3x4
extern "C" void matrix4x3_multiply_to_matrix3x4(const real_matrix4x3 *matrix1, const real_matrix4x3 *matrix2, real_matrix3x4 *result)
{
    __vector4 m1_scale_forward = __loadunalignedvector(&matrix1->scale);
    __vector4 m2_scale_forward = __loadunalignedvector(&matrix2->scale);
    __vector4 m1_left = __loadunalignedvector(&matrix1->rotation.left);
    __vector4 m1_up = __loadunalignedvector(&matrix1->rotation.up);
    __vector4 m2_position = __loadunalignedvector(&matrix2->rotation.up.k);
    __vector4 m2_left = __loadunalignedvector(&matrix2->rotation.left);
    __vector4 m2_up = __loadunalignedvector(&matrix2->rotation.up);

    __vector4 m1_forward = __vpermwi(m1_scale_forward, VPERMWI_CONST(1, 2, 3, 0));
    __vector4 m1_scale = __vspltw(m1_scale_forward, 0);
    __vector4 scale = __vmulfp(m1_scale, __vspltw(m2_scale_forward, 0));

    __vector4 forward = __vmaddfp(m1_up, __vspltw(m2_scale_forward, 3),
        __vmaddfp(m1_left, __vspltw(m2_scale_forward, 2), __vmulfp(m1_forward, __vspltw(m2_scale_forward, 1))));
    __vector4 left = matrix_math_rotate_basis(m1_forward, m1_left, m1_up, m2_left);
    __vector4 up = matrix_math_rotate_basis(m1_forward, m1_left, m1_up, m2_up);
    __vector4 position = __vmaddfp(m1_up, __vspltw(m2_position, 3),
        __vmaddfp(m1_left, __vspltw(m2_position, 2), __vmulfp(m1_forward, __vspltw(m2_position, 1))));
    forward = __vmulfp(forward, scale);
    left = __vmulfp(left, scale);
    up = __vmulfp(up, scale);
    __vector4 m1_position = __vpermwi(__loadunalignedvector(&matrix1->rotation.up.k), VPERMWI_CONST(1, 2, 3, 0));
    position = __vmaddfp(position, m1_scale, m1_position);

    __vector4 forward_up_high = __vmrghw(forward, up);
    __vector4 left_position_high = __vmrghw(left, position);
    __vector4 row0 = __vmrghw(forward_up_high, left_position_high);
    __vector4 row1 = __vmrglw(forward_up_high, left_position_high);
    __vector4 row2 = __vmrghw(__vmrglw(forward, up), __vmrglw(left, position));
    matrix_math_store_row(result->m[0], row0);
    matrix_math_store_row(result->m[1], row1);
    matrix_math_store_row(result->m[2], row2);
}

static __forceinline void matrix_math_store_rotation(real_matrix3x3 *matrix, __vector4 forward, __vector4 left, __vector4 up)
{
    __stvewx(__vspltw(forward, 0), matrix, 0);
    __stvewx(__vspltw(forward, 1), matrix, 4);
    __stvewx(__vspltw(forward, 2), matrix, 8);
    __stvewx(__vspltw(left, 0), matrix, 12);
    __stvewx(__vspltw(left, 1), matrix, 16);
    __stvewx(__vspltw(left, 2), matrix, 20);
    __stvewx(__vspltw(up, 0), matrix, 24);
    __stvewx(__vspltw(up, 1), matrix, 28);
    __stvewx(__vspltw(up, 2), matrix, 32);
}

// @address 0x11a5f8 matrix3x3_rotation_from_angles
extern "C" void matrix3x3_rotation_from_angles(real_matrix3x3 *matrix, float yaw, float pitch, float roll)
{
    __vector4 cosines;
    cosines.v[0] = roll;
    cosines.v[1] = pitch;
    cosines.v[2] = yaw;
    cosines.v[3] = 0.0f;
    __vector4 sines;
    XMVectorSinCos(&sines, &cosines, cosines);
    __vector4 one = __vspltw(cosines, 3);
    __vector4 cosine_roll = __vspltw(cosines, 0);
    __vector4 cosine_sine_low = __vmrglw(cosines, sines);
    __vector4 sine_cosine_low = __vmrglw(sines, cosines);
    __vector4 cosine_sine_high = __vmrghw(cosines, sines);
    __vector4 sine_pitch = __vspltw(sines, 1);
    __vector4 sine_roll = __vspltw(sines, 0);
    __vector4 negative_one = __vsubfp(__vspltw(sines, 3), one);
    __vector4 mixed = __vrlimi(__vor(cosine_sine_low, cosine_sine_low), cosine_sine_high, 3, 0);
    __vector4 forward = __vmulfp(__vmrglw(cosine_sine_high, cosine_sine_high), cosine_sine_low);
    __vector4 signed_pitch = __vrlimi(__vmulfp(sine_pitch, negative_one), one, 3, 0);
    __vector4 middle = __vmulfp(mixed, signed_pitch);
    __vector4 up_term = __vmulfp(__vmulfp(sine_roll, __vmrglw(one, negative_one)), sine_cosine_low);
    __vector4 left_term = __vmulfp(__vmulfp(cosine_roll, __vrlimi(__vor(negative_one, negative_one), one, 7, 0)), sine_cosine_low);
    __vector4 left = __vmaddfp(sine_roll, middle, left_term);
    __vector4 up = __vmaddfp(cosine_roll, middle, up_term);
    matrix_math_store_rotation(matrix, forward, left, up);
}

// @address 0x107dd8 matrix4x3_multiply_by_orientation
extern "C" void matrix4x3_multiply_by_orientation(const real_matrix4x3 *matrix, const real_orientation *orientation, real_matrix4x3 *result)
{
    __vector4 quaternion = __lvx(&orientation->rotation, 0);
    __vector4 norm = __vmsum4fp(quaternion, quaternion);
    __vector4 one = matrix_math_one;
    __vector4 two = matrix_math_get_two();
    __vector4 permuted_quaternion = __vpermwi(quaternion, VPERMWI_CONST(0, 0, 1, 1));
    __vector4 quaternion_w = __vspltw(quaternion, 3);
    __vector4 scale_forward = __loadunalignedvector(&matrix->scale);
    __vector4 matrix_left = __loadunalignedvector(&matrix->rotation.left);
    __vector4 matrix_up = __loadunalignedvector(&matrix->rotation.up);
    __vector4 translation_scale = __lvx(&orientation->translation, 0);
    __vector4 matrix_scale = __vspltw(scale_forward, 0);
    scale_forward = __vpermwi(scale_forward, VPERMWI_CONST(1, 2, 3, 0));
    __vector4 reciprocal = __vrefp(norm);
    reciprocal = __vmaddfp(reciprocal, __vnmsubfp(reciprocal, norm, one), reciprocal);
    __vector4 scaled_quaternion = __vmulfp(quaternion, __vmulfp(reciprocal, two));
    __vector4 squares = __vmulfp(quaternion, scaled_quaternion);
    __vector4 cross_products = __vmulfp(__vpermwi(scaled_quaternion, VPERMWI_CONST(1, 2, 2, 2)), permuted_quaternion);
    __vector4 w_products = __vmulfp(quaternion_w, scaled_quaternion);
    __vector4 ii = __vspltw(squares, 0);
    __vector4 jj = __vspltw(squares, 1);
    __vector4 kk = __vspltw(squares, 2);
    __vector4 ij = __vspltw(cross_products, 0);
    __vector4 ik = __vspltw(cross_products, 1);
    __vector4 jk = __vspltw(cross_products, 2);
    __vector4 wi = __vspltw(w_products, 0);
    __vector4 wj = __vspltw(w_products, 1);
    __vector4 wk = __vspltw(w_products, 2);

    __stvewx(__vmulfp(matrix_scale, __vspltw(translation_scale, 3)), &result->scale, 0);

    __vector4 forward = __vmaddfp(matrix_up, __vsubfp(ik, wj),
        __vmaddfp(matrix_left, __vaddfp(ij, wk),
        __vmulfp(scale_forward, __vsubfp(one, __vaddfp(jj, kk)))));
    __vector4 left = __vmaddfp(matrix_up, __vaddfp(jk, wi),
        __vmaddfp(matrix_left, __vsubfp(one, __vaddfp(ii, kk)),
        __vmulfp(scale_forward, __vsubfp(ij, wk))));
    __vector4 up = __vmaddfp(matrix_up, __vsubfp(one, __vaddfp(ii, jj)),
        __vmaddfp(matrix_left, __vsubfp(jk, wi),
        __vmulfp(scale_forward, __vaddfp(ik, wj))));
    __vector4 out_forward = __vperm(forward, left, __lvx(matrix_math_product_permutes[0], 0));
    __vector4 out_left = __vperm(left, up, __lvx(matrix_math_product_permutes[1], 0));
    __vector4 position = __vmaddfp(matrix_up, __vspltw(translation_scale, 2),
        __vmaddfp(matrix_left, __vspltw(translation_scale, 1),
        __vmulfp(scale_forward, __vspltw(translation_scale, 0))));
    __vector4 matrix_position = __vpermwi(__loadunalignedvector(&matrix->rotation.up.k), VPERMWI_CONST(1, 2, 3, 0));
    position = __vmaddfp(position, matrix_scale, matrix_position);
    __vector4 out_up = __vperm(up, position, __lvx(matrix_math_product_permutes[2], 0));
    __storeunalignedvector(out_forward, &result->rotation.forward);
    __storeunalignedvector(out_left, &result->rotation.left.j);
    __storeunalignedvector(out_up, &result->rotation.up.k);
}

void matrix4x3_identity(real_matrix4x3 *matrix)
{
    matrix->scale = 1.0f;
    matrix->forward.i = 1.0f;
    matrix->forward.j = 0.0f;
    matrix->forward.k = 0.0f;
    matrix->left.i = 0.0f;
    matrix->left.j = 1.0f;
    matrix->left.k = 0.0f;
    matrix->up.i = 0.0f;
    matrix->up.j = 0.0f;
    matrix->up.k = 1.0f;
    matrix->position.x = 0.0f;
    matrix->position.y = 0.0f;
    matrix->position.z = 0.0f;
}
