// blamlibxbox/convex_hulls.obj: from the records 0x155c40 0x155d58 0x155df8 0x2202a0
// @flags /TP /O2 /Oc
#include "math/real_math_types.h"
#include "math/real_math.h"
// @address 0x155c40 ?convex_hull2d_intersect_new@@YAFFPBTreal_point2d@@F0FPAT1@M@Z
// @address 0x155d58 ?plane2d_from_points@@YAPAUreal_plane2d@@PAU1@PBTreal_point2d@@1@Z
// @address 0x155df8 ?convex_polygon2d_clip_to_plane_new@@YAFJPBTreal_point2d@@PBUreal_plane2d@@FPIAT1@M@Z
// @address 0x2202a0 ?convex_hull2d_test_point@@YA_NFPBTreal_point2d@@0M@Z
#include <math.h>
#include <string.h>
#include <vectorintrinsics.h>
#include "math/convex_hulls.h"


enum { k_clip_buffer_size = 512, k_clip_maximum_count = 128 };



inline real_plane2d *plane2d_from_points(real_plane2d *plane, const real_point2d *a, const real_point2d *b)
{
  plane->n.i = b->y - a->y;
  plane->n.j = a->x - b->x;
  float length = sqrtf(plane->n.i * plane->n.i + plane->n.j * plane->n.j);
  if (fabsf(length) < 0.0001f) {
    length = 0.0f;
  } else {
    float scale = 1.0f / length;
    plane->n.i = plane->n.i * scale;
    plane->n.j = plane->n.j * scale;
  }
  if (length != 0.0f) {
    plane->d = a->x * plane->n.i + a->y * plane->n.j;
    return plane;
  }
  plane->d = 0.0f;
  return 0;
}


// 0x155df8
static __forceinline __vector4 convex_hulls_four_distances(const real_point2d *first, const real_point2d *second, __vector4 normal, __vector4 offset)
{
    __vector4 zero = __vspltisw(0);
    __vector4 first_pair = __lvx(first, 0);
    __vector4 second_pair = __lvx(second, 0);
    __vector4 a = __vsubfp(__vmsum4fp(__vrlimi(first_pair, zero, 3, 0), normal), offset);
    __vector4 b = __vsubfp(__vmsum4fp(__vsldoi(first_pair, zero, 8), normal), offset);
    __vector4 c = __vsubfp(__vmsum4fp(__vrlimi(second_pair, zero, 3, 0), normal), offset);
    __vector4 d = __vsubfp(__vmsum4fp(__vsldoi(second_pair, zero, 8), normal), offset);
    return __vsldoi(__vmrghw(a, b), __vmrghw(c, d), 8);
}
static __forceinline void convex_hulls_classify_four(__vector4 distance, __vector4 positive_epsilon, __vector4 negative_epsilon,
    __vector4 *any_front, __vector4 *any_back, long *sides, long index, __vector4 zero, __vector4 one, __vector4 minus_one)
{
    __vector4 front = __vcmpgtfp(distance, positive_epsilon);
    __vector4 back = __vcmpgtfp(negative_epsilon, distance);
    *any_front = __vor(*any_front, front);
    *any_back = __vor(*any_back, back);
    __stvx(__vsel(__vsel(minus_one, one, front), zero, back), &sides[index], 0);
}
short convex_polygon2d_clip_to_plane_new(
    // match: 0x155df8: mangled FPIAT1 needs the restrict; without it the name changes and the epilogue gains alias-reload lfs plus r7/r8, fr12/fr13 swaps
    long count, const real_point2d *points, const real_plane2d *plane, short maximum_count, real_point2d *__restrict result, float epsilon)
{
    long batch_count = count >> 3;
    long half_batch_count = (count >> 2) & 1;
    long scalar_start = ((batch_count << 1) + half_batch_count) << 2;
    epsilon = 0.0001f;
    union {
        real_point2d temporary_points[128];
        long sides[128];
    } scratch;
    if (points == result) {
        memcpy(scratch.temporary_points, points, count * sizeof(real_point2d));
        points = scratch.temporary_points;
    }
    __vector4 zero = __vspltisw(0);
    __vector4 one = __vspltisw(1);
    __vector4 minus_one = __vspltisw(-1);
    __vector4 any_front = zero;
    __vector4 any_back = zero;
    __vector4 plane_vector = __vor(__lvlx(plane, 0), __lvrx(plane, 16));
    __vector4 offset = __vspltw(plane_vector, 2);
    __vector4 normal = __vrlimi(plane_vector, __vspltisw(0), 3, 0);
    for (long index = scalar_start; index < count; ++index) {
        float distance = plane2d_distance_to_point(plane, &points[index]);
        if (distance > epsilon) {
            scratch.sides[index] = 1;
            any_front = __vor(any_front, one);
        } else if (distance < -0.0001f) {
            scratch.sides[index] = 0;
            any_back = __vor(any_back, one);
        } else {
            scratch.sides[index] = -1;
        }
    }
    __vector4 positive_epsilon = __vspltw(__lvlx(&epsilon, 0), 0);
    __vector4 negative_epsilon = __vsubfp(__vspltisw(0), positive_epsilon);
    for (long batch = 0; batch < batch_count; ++batch) {
        long index = batch << 3;
        __vector4 pair0 = __lvx(&points[index + 0], 0);
        __vector4 pair1 = __lvx(&points[index + 2], 0);
        __vector4 pair2 = __lvx(&points[index + 4], 0);
        __vector4 pair3 = __lvx(&points[index + 6], 0);
        __vector4 even0 = __vsubfp(__vmsum4fp(__vrlimi(pair0, zero, 3, 0), normal), offset);
        __vector4 odd0 = __vsubfp(__vmsum4fp(__vsldoi(pair0, zero, 8), normal), offset);
        __vector4 even1 = __vsubfp(__vmsum4fp(__vrlimi(pair1, zero, 3, 0), normal), offset);
        __vector4 odd1 = __vsubfp(__vmsum4fp(__vsldoi(pair1, zero, 8), normal), offset);
        __vector4 even2 = __vsubfp(__vmsum4fp(__vrlimi(pair2, zero, 3, 0), normal), offset);
        __vector4 odd2 = __vsubfp(__vmsum4fp(__vsldoi(pair2, zero, 8), normal), offset);
        __vector4 even3 = __vsubfp(__vmsum4fp(__vrlimi(pair3, zero, 3, 0), normal), offset);
        __vector4 odd3 = __vsubfp(__vmsum4fp(__vsldoi(pair3, zero, 8), normal), offset);
        __vector4 first = __vsldoi(__vmrghw(even0, odd0), __vmrghw(even1, odd1), 8);
        __vector4 second = __vsldoi(__vmrghw(even2, odd2), __vmrghw(even3, odd3), 8);
        __vector4 front_first = __vcmpgtfp(first, positive_epsilon);
        __vector4 back_first = __vcmpgtfp(negative_epsilon, first);
        __vector4 front_second = __vcmpgtfp(second, positive_epsilon);
        __vector4 back_second = __vcmpgtfp(negative_epsilon, second);
        any_front = __vor(__vor(any_front, front_first), front_second);
        any_back = __vor(__vor(any_back, back_first), back_second);
        __stvx(__vsel(__vsel(minus_one, one, front_first), zero, back_first), &scratch.sides[index], 0);
        __stvx(__vsel(__vsel(minus_one, one, front_second), zero, back_second), &scratch.sides[index + 4], 0);

    }
    if (half_batch_count > 0) {
        long index = batch_count * 8;
        __vector4 distances = convex_hulls_four_distances(&points[index], &points[index + 2], normal, offset);
        convex_hulls_classify_four(distances, positive_epsilon, negative_epsilon, &any_front, &any_back, scratch.sides, index, zero, one, minus_one);
    }
    unsigned int front_condition, back_condition;
    __vcmpequwR(any_front, __vspltisw(0), &front_condition);
    __vcmpequwR(any_back, __vspltisw(0), &back_condition);
    long result_count;
    if (back_condition & 0x80) {
        result_count = count;
        memcpy(result, points, result_count * sizeof(real_point2d));
        return (short)result_count;
    } else if (front_condition & 0x80) {
        return 0;
    } else {
        long previous_index = count - 1; result_count = 0;
        for (long index = 0; result_count < 128 && index < count; ++index) {
            long side = scratch.sides[index];
            if (side == 0) {
                if (scratch.sides[previous_index] == 1) {
                    const real_point2d *previous = &points[previous_index];
                    const real_point2d *current = &points[index];
                    real_vector2d edge;
                    vector_from_points2d(previous, current, &edge);
                    // match: 0x155df8: keep the edge components grouped during plane projection.
                    const real_vector2d *const &edge_pointer = &edge;
                    float denominator = plane->n.i * edge_pointer->i + plane->n.j * edge_pointer->j;
                    if (denominator < -0.0001f || denominator > 0.0001f) {
                        float scale = plane2d_distance_to_point(plane, current) / denominator;
                        result[result_count].x = current->x - edge.i * scale;
                        result[result_count].y = current->y - edge.j * scale;
                        ++result_count;
                    }
                }
            } else {
                const real_point2d *current = &points[index];
                if (side == 1 && scratch.sides[previous_index] == 0) {
                    const real_point2d *previous = &points[previous_index];
                    real_vector2d edge;
                    vector_from_points2d(current, previous, &edge);
                    // match: 0x155df8: keep the edge components grouped during plane projection.
                    const real_vector2d *const &edge_pointer = &edge;
                    float denominator = plane->n.i * edge_pointer->i + plane->n.j * edge_pointer->j;
                    if (denominator < -0.0001f || denominator > 0.0001f) {
                        float scale = plane2d_distance_to_point(plane, previous) / denominator;
                        result[result_count].x = previous->x - edge.i * scale;
                        result[result_count].y = previous->y - edge.j * scale;
                        ++result_count;
                    }
                }
                if (result_count < 128) {
                    result[result_count] = *current;
                    ++result_count;
                }
            }
            previous_index = index;
        }
        return (short)result_count;
    }
}

bool convex_hull2d_test_point(short count, const real_point2d *points, const real_point2d *point, float epsilon)
{
  bool result = true;
  for (short i = 0; i < count; i++) {
    const real_point2d *a = &points[i];
    const real_point2d *b = &points[i + 1 < count ? i + 1 : 0];
    real_vector2d to_point, edge;
    vector_from_points2d(a, point, &to_point);
    vector_from_points2d(a, b, &edge);
    if (cross_product2d(&edge, &to_point) < -epsilon) {
      result = false;
      break;
    }
  }
  return result;
}

short convex_hull2d_intersect_new(
    short p_count, const real_point2d *p, short q_count, const real_point2d *q, short maximum_count, real_point2d *result, float epsilon)
{
    real_point2d clip_buffers[2][k_clip_buffer_size];
    short current_count = q_count;
    const real_point2d *current = q;
    for (short i = 0; i < p_count && current_count > 0; i++) {
        short previous = i != 0 ? i - 1 : p_count - 1;
        real_point2d *buffer = i == p_count - 1 ? result : clip_buffers[i & 1];
        real_plane2d plane;
        if (plane2d_from_points(&plane, &p[i], &p[previous])) {
            current_count = convex_polygon2d_clip_to_plane_new(current_count, current, &plane, k_clip_maximum_count, buffer, 0.0001f);
            if (current_count == -1)
                return -1;
        } else {
            memcpy(buffer, current, current_count * sizeof(real_point2d));
        }
        current = buffer;
    }
    return current_count;
}
