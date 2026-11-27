#ifndef MATH_SPHERICAL_HARMONICS_H
#define MATH_SPHERICAL_HARMONICS_H

// Functions defined in tu/blamlibxbox/spherical_harmonics.cpp that other units call.

union real_vector3d;

extern "C" bool sh_eval_cone_light(unsigned long order, const real_vector3d *direction, float radius,
    float red, float green, float blue, float *red_sh, float *green_sh, float *blue_sh);

float *sh_scale(float *result, unsigned long order, const float *values, float scale);
float sh_reconstruct_order_3(real_vector3d *direction, float *coefficients);
// match: 0x1626b0: b is __restrict in the mangled name ?sh_add@@YAPAMPAMKPBMPIBM@Z (PIBM)
float *sh_add(float *result, unsigned long order, const float *a, const float *__restrict b);
bool sh_eval_directional_light(unsigned long order, const real_vector3d *direction,
    float red, float green, float blue, float *red_sh, float *green_sh, float *blue_sh);
float *sh_eval_direction(float *result, unsigned long order, const real_vector3d *direction);

#endif // MATH_SPHERICAL_HARMONICS_H
