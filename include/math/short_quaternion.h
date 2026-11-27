#ifndef MATH_SHORT_QUATERNION_H
#define MATH_SHORT_QUATERNION_H

struct real_quaternion;
struct real_matrix4x3;

struct c_short_quaternion
{
    short i, j, k, w;
    void set_real_quaternion(real_quaternion *quaternion);
    __forceinline void get_real_quaternion(real_quaternion *quaternion) const;
    void get_real_matrix4x3(real_matrix4x3 *matrix) const;
    __forceinline void set_real_matrix4x3(const real_matrix4x3 *matrix);
};

#endif // MATH_SHORT_QUATERNION_H
