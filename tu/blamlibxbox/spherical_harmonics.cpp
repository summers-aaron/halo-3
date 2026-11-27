// blamlibxbox/spherical_harmonics.obj: from the records 0x162240 0x162340 0x1623c0 0x1624a0 0x1626b0 0x300760 0x300828 0x3008e8
// @flags /TP /O2
// @address 0x300760 ?compute_cap_int@SH_MATH_UTILITY@@YAXKMPAM@Z
#include "math/real_math.h"
#include "math/spherical_harmonics.h"

namespace SH_MATH_UTILITY
{
struct matrix4x4 { float m[4][4]; };
}

// @address 0x162340 ?sh_scale@@YAPAMPAMKPBMM@Z
float *sh_scale(float *result, unsigned long order, const float *values, float scale)
{
    order = 3;
    float factor = scale;
    unsigned long coefficient_count = order * order;
    for (unsigned long coefficient = 0; coefficient < coefficient_count; ++coefficient)
        result[coefficient] = values[coefficient] * factor;
    return result;
}

namespace SH_MATH_UTILITY
{
void compute_cap_int(unsigned long order, float angle, float *result)
{
    float root_pi = (float)sqrt(3.1415929794311523);
    float cosine = (float)cos((double)angle);
    float scaled_cosine = root_pi * cosine;
    float sine = (float)sin((double)angle);
    float root_three = (float)sqrt(3.0);
    float sine_squared = sine * sine;
    result[0] = -scaled_cosine + root_pi;
    result[1] = root_three * sine_squared * root_pi * 0.5f;
    float root_five = (float)sqrt(5.0);
    float cosine_squared = cosine * cosine;
    float *cosine_square = &cosine_squared;
    result[2] = root_five * (*cosine_square - 1.0f) * cosine * root_pi * -0.5f;
}
}

// @address 0x1623c0 ?sh_reconstruct_order_3@@YAMPATreal_vector3d@@PAM@Z
float sh_reconstruct_order_3(real_vector3d *direction, float *coefficients)
{
    float x = direction->i;
    float y = direction->j;
    float z = direction->k;
    float band0 = coefficients[0];
    float band1 = -x * coefficients[3] - y * coefficients[1] + z * coefficients[2];
    float xy = x * y;
    float yz = y * z;
    float xz = x * z;
    float difference = x * x - y * y;
    float zonal = 3.0f * z * z - 1.0f;
    float band2 = xy * coefficients[4] - yz * coefficients[5] - xz * coefficients[7];
    float band3 = zonal * coefficients[6];
    float band4 = difference * coefficients[8];
    float c0 = 0.282095f;
    float c1 = 0.325735271f;
    float c2 = 0.273136914f;
    float c3 = 0.0788479745f;
    float c4 = 0.136568457f;
    return c0 * band0 + c1 * band1 + c2 * band2 + c3 * band3 + c4 * band4;
}

// @address 0x300828 ?build_light_rot_vec@SH_MATH_UTILITY@@YAXKPBUmatrix4x4@1@PIAM@Z
namespace SH_MATH_UTILITY
{
// match: 0x300828: pointer __restrict qualifier prevents alias reload
inline void build_light_rot_vec(unsigned long order, const matrix4x4 *matrix, float *__restrict result)
{
    float x = matrix->m[2][0];
    float y = matrix->m[2][1];
    float z = matrix->m[2][2];
    result[0] = 1.0f;
    float *linear = result + 1;
    linear[0] = -y;
    linear[1] = z;
    linear[2] = -x;
    float root_fifteen = (float)sqrt(15.0);
    float root_five = (float)sqrt(5.0);
    float root_product = root_fifteen * root_five;
    float xx = x * x;
    float zz = z * z;
    float yy = y * y;
    float *quadratic = result + 4;
    quadratic[0] = root_product * x * y / 5.0f;
    quadratic[1] = -root_product * y * z / 5.0f;
    quadratic[2] = -xx / 2.0f + zz - yy / 2.0f;
    quadratic[3] = -root_product * x * z / 5.0f;
    quadratic[4] = root_product * xx / 10.0f - root_product * yy / 10.0f;
}
}

// @address 0x162240 ?sh_eval_direction@@YAPAMPAMKPBTreal_vector3d@@@Z
float *sh_eval_direction(float * result, unsigned long order, const real_vector3d *direction)
{
    // match: 0x162240: value range assumption guides branch lowering
    __assume(order == 3);
    float x = direction->i;
    float y = direction->j;
    float z = direction->k;
    float inverse_root_pi = 1.0f / sqrtf(3.1415927f);
    float band1_scale = sqrtf(3.0f) * inverse_root_pi;
    result[0] = inverse_root_pi / 2.0f;
    result[1] = -band1_scale * y / 2.0f;
    result[2] = band1_scale * z / 2.0f;
    result[3] = -band1_scale * x / 2.0f;
    if (order > 2)
    {
        float root_fifteen = sqrtf(15.0f);
        float band2_scale = root_fifteen * inverse_root_pi;
        float yx = y * x;
        float root_five = sqrtf(5.0f);
        float zz = z * z;
        float zonal = 3.0f * zz - 1.0f;
        float xz = x * z;
        float xx = x * x;
        float yy = y * y;
        float difference = xx - yy;
        result[4] = band2_scale * yx / 2.0f;
        result[5] = -band2_scale * y * z / 2.0f;
        result[6] = root_five * zonal * inverse_root_pi / 4.0f;
        result[7] = -band2_scale * xz / 2.0f;
        result[8] = root_fifteen * difference * inverse_root_pi / 4.0f;
        if (order > 3)
        {
            float root_two = sqrtf(2.0f);
            float root_thirty_five = sqrtf(35.0f);
            float band3_scale_a = root_two * root_thirty_five;
            float cubic_a = 3.0f * xx - yy;
            float root_one_oh_five = sqrtf(105.0f);
            float root_twenty_one = sqrtf(21.0f);
            float band3_scale_b = root_two * root_twenty_one;
            float five_zz = 5.0f * zz;
            float zonal3 = -1.0f + five_zz;
            float root_seven = sqrtf(7.0f);
            float scaled_x = inverse_root_pi * x;
            float scaled_z = inverse_root_pi * z;
            float cubic_b = xx - 3.0f * yy;
            result[9] = -band3_scale_a * y * cubic_a * inverse_root_pi / 8.0f;
            result[10] = root_one_oh_five * inverse_root_pi * yx * z / 2.0f;
            result[11] = -band3_scale_b * inverse_root_pi * y * zonal3 / 8.0f;
            result[12] = root_seven * z * (five_zz - 3.0f) * inverse_root_pi / 4.0f;
            result[13] = -band3_scale_b * scaled_x * zonal3 / 8.0f;
            result[14] = root_one_oh_five * difference * scaled_z / 4.0f;
            result[15] = -band3_scale_a * x * cubic_b * inverse_root_pi / 8.0f;
        }
    }
    return result;
}

// @address 0x1626b0 ?sh_add@@YAPAMPAMKPBMPIBM@Z
// match: 0x1626b0: pointer __restrict qualifier prevents alias reload
float *sh_add(float *result, unsigned long order, const float *a, const float *__restrict b)
{
    // match: 0x1626b0: value range assumption guides branch lowering
    __assume(order == 3);
    unsigned long count = order * order;
    unsigned long *coefficient_count = &count;
    for (unsigned long coefficient = 0; coefficient < *coefficient_count; ++coefficient)
        { result[coefficient] = a[coefficient] + b[coefficient]; }
    return result;
}

static __forceinline float spherical_harmonics_cos_wt_int(unsigned long order)
{
    float band0 = 0.25f;
    float band1 = 0.5f;
    float band2 = 0.3125f;
    float result = band0 + band1;
    if (order > 2)
        result += band2;
    return result;
}

// @address 0x1624a0 ?sh_eval_directional_light@@YA_NKPBTreal_vector3d@@MMMPAM11@Z
bool sh_eval_directional_light(unsigned long order, const real_vector3d *direction,
    float red, float green, float blue, float *red_sh, float *green_sh, float *blue_sh)
{
    // match: 0x1624a0: value range assumption guides branch lowering
    __assume(order == 3);
    float values[16];
    sh_eval_direction(values, order, direction);
    float scale = 3.141593f / spherical_harmonics_cos_wt_int(order);
    unsigned long coefficient_order = order;
    unsigned long i;
    float red_scale = scale * red;
    for (i = 0; i < coefficient_order * coefficient_order; ++i)
        red_sh[i] = values[i] * red_scale;
    if (green_sh)
    {
        float green_value_scale = scale;
        float green_scale = green_value_scale * green;
        for (i = 0; i < coefficient_order * coefficient_order; ++i)
            green_sh[i] = values[i] * green_scale;
    }
    if (blue_sh)
    {
        float blue_scale = scale * blue;
        for (i = 0; i < coefficient_order * coefficient_order; ++i)
            blue_sh[i] = values[i] * blue_scale;
    }
    return true;
}

// @address 0x3008e8 sh_eval_cone_light
extern "C" bool sh_eval_cone_light(unsigned long order, const real_vector3d *direction, float radius,
    float red, float green, float blue, float *red_sh, float *green_sh, float *blue_sh)
{
    order = 3;
    if (radius < 0.0001f)
        return sh_eval_directional_light(order, direction, red, green, blue, red_sh, green_sh, blue_sh);

    float cone_angle = radius;
    float clamped_angle = cone_angle > 1.5707964f ? 1.5707964f : cone_angle;
    float norm = 1.0f / (sinf(clamped_angle) * sinf(clamped_angle));

    SH_MATH_UTILITY::matrix4x4 matrix;
    matrix.m[2][0] = direction->i;
    matrix.m[2][1] = direction->j;
    matrix.m[2][2] = direction->k;

    real_vector3d x_axis;
    set_real_vector3d(&x_axis, 1.0f, 0.0f, 0.0f);
    real_vector3d y_axis;
    set_real_vector3d(&y_axis, 0.0f, 1.0f, 0.0f);
    float x_dot = dot_product3d(&x_axis, direction);
    float y_dot = dot_product3d(&y_axis, direction);
    if (fabs(x_dot) < fabs(y_dot))
    {
        cross_product3d(direction, &x_axis, &y_axis);
        scale_vector3d(&y_axis, 1.0f / magnitude3d(&y_axis), &y_axis);
        cross_product3d(&y_axis, direction, &x_axis);
    }
    else
    {
        cross_product3d(direction, &y_axis, &y_axis);
        scale_vector3d(&y_axis, 1.0f / magnitude3d(&y_axis), &y_axis);
        cross_product3d(&y_axis, direction, &x_axis);
    }
    matrix.m[0][0] = x_axis.i;
    matrix.m[0][1] = x_axis.j;
    matrix.m[0][2] = x_axis.k;
    matrix.m[1][0] = y_axis.i;
    matrix.m[1][1] = y_axis.j;
    matrix.m[1][2] = y_axis.k;

    float rotation[16];
    SH_MATH_UTILITY::build_light_rot_vec(order, &matrix, rotation);
    float cap[4];
    SH_MATH_UTILITY::compute_cap_int(order, cone_angle, cap);

    unsigned long coefficient_order = order;
    unsigned long i;
    unsigned long j;
    for (i = 0; i < coefficient_order; ++i)
    {
        unsigned long count = 2 * i + 1;
        unsigned long start = i * i;
        float scale = cap[i] * red * norm;
        for (j = 0; j < count; ++j)
            red_sh[start + j] = rotation[start + j] * scale;
    }
    if (green_sh)
    {
        for (i = 0; i < coefficient_order; ++i)
        {
            unsigned long count = 2 * i + 1;
            unsigned long start = i * i;
            float scale = cap[i] * green * norm;
            for (j = 0; j < count; ++j)
                green_sh[start + j] = rotation[start + j] * scale;
        }
    }
    if (blue_sh)
    {
        for (i = 0; i < coefficient_order; ++i)
        {
            unsigned long count = 2 * i + 1;
            unsigned long start = i * i;
            float scale = cap[i] * blue * norm;
            for (j = 0; j < count; ++j)
                blue_sh[start + j] = rotation[start + j] * scale;
        }
    }
    return true;
}
