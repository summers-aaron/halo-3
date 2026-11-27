// blamlibxbox/function_definitions.obj: from the records 0xea4f8 0xea558 0xea900 0xeaa18 0x116b10 0x116bf8 0x2a6a28 0x2a6ab8 0x2a6b48 0x2a6c18
// @flags /TP /O2
#include "math/real_math.h"
#include <math.h>
#include <ppcintrinsics.h>
#include "math/periodic_functions.h"
#include "math/function_definitions.h"
#include "math/function_definition_types.h"
#include "math/function_definitions_constants.h"
#include "math/power.h"
#if !defined(_XBOX) || defined(__clang__)
#include "objects/object_globals.h"
#include "animations/animation_definition_storage.h"
#include "math/real_constant_pool.h"
#include "math/real_math_constant_storage.h"
#include "math/real_math_constants.h"
#include "sound/sound_manager_globals.h"
#endif

// @address 0x9b150 ?transition_function_evaluate@@YAMFM@Z
// @address 0x1169f0 ?periodic_function_evaluate@@YAMFM@Z
// @address 0x39294 k_function_constant_pool_anchor

// @address 0x330b4 g_object_world_coordinate_bounds
#if defined(_XBOX) && !defined(__clang__)
extern "C" { const float g_object_world_coordinate_bounds[] = { -32768.0f }; }
#else
// objects.cpp reads this extern "C" one (objects/object_globals.h); a const in a braced extern "C" block is internal
const float g_object_world_coordinate_bounds[] = { -32768.0f };
#endif

// @address 0x3aa90 k_real_math_zero_storage
// @address 0x3aa90 k_real_math_origin3d
// @address 0x3aa90 k_real_math_zero_vector3d_storage
// @address 0x3aa90 k_real_math_zero_vector_storage
#if defined(_XBOX) && !defined(__clang__)
extern "C" { const float k_real_math_zero_storage[6] = { 0.0f }; }
extern "C" { const real_point3d k_real_math_origin3d = { 0.0f, 0.0f, 0.0f }; }
extern "C" { const real_vector3d k_real_math_zero_vector3d_storage = { 0.0f, 0.0f, 0.0f }; }
extern "C" { const real_vector3d k_real_math_zero_vector_storage = { 0.0f, 0.0f, 0.0f }; }
#else
const float k_real_math_zero_storage[6] = { 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f };
const real_point3d k_real_math_origin3d = { 0.0f, 0.0f, 0.0f };
const real_vector3d k_real_math_zero_vector3d_storage = { 0.0f, 0.0f, 0.0f };
const real_vector3d k_real_math_zero_vector_storage = { 0.0f, 0.0f, 0.0f };
const real_orientation k_real_math_identity_orientation_storage = { { { 0.0f, 0.0f, 0.0f }, 1.0f }, { 0.0f, 0.0f, 0.0f }, 1.0f };
const real_matrix4x3 k_real_math_identity_matrix_storage =
    { 1.0f, { { {1.0f, 0.0f, 0.0f}, {0.0f, 1.0f, 0.0f}, {0.0f, 0.0f, 1.0f} } }, {0.0f, 0.0f, 0.0f} };
const real_matrix4x3 k_negative_identity4x3 =
    { 1.0f, { { {-1.0f, 0.0f, 0.0f}, {0.0f, -1.0f, 0.0f}, {0.0f, 0.0f, -1.0f} } }, {0.0f, 0.0f, 0.0f} };
#endif

namespace {
union pixel32
{
    unsigned long value;
    struct { unsigned char alpha, red, green, blue; } channels;
};
}


namespace {
class c_periodic_function_compact
{
public:
    float evaluate(float value) const
    {
        return periodic_function_evaluate(m_type, m_frequency * value + m_phase) * (m_upper - m_lower) + m_lower;
    }
private:
    unsigned char m_type;
    float m_frequency;
    float m_phase;
    float m_lower;
    float m_upper;
};
}

__forceinline float c_function_definition::exclude_value(float value) const
{
  float result = value;
  const s_function_definition_internal *definition = (const s_function_definition_internal *)(const void *)m_data.address;
  float lower = definition->excluded_lower;
  if (!(value <= lower))
    result = definition->excluded_upper - lower + value;
  return result;
}
__forceinline float c_function_definition::unexclude_value(float value) const
{
  float result = value;
  if (((const s_function_definition_internal *)(const void *)m_data.address)->flags & 8)
    result = exclude_value(value);
  return result;
}


__forceinline long c_multi_part_function_compact::get_size_in_bytes() const
{
    long size = 4;
    const s_function_part *part = m_parts;
    for (byte index = 0; index < m_part_count; ++index)
    {
        size += get_size_of_part((c_function_definition::e_function_type)part->type);
        part = increment_function_part_pointer(part);
    }
    return size;
}


namespace {
static __forceinline unsigned long function_definitions_blend_pixel32s(float fraction, unsigned long lower, unsigned long upper)
{
    pixel32 result, end;
    result.value = lower;
    end.value = upper;
    result.channels.alpha = (unsigned char)(long)((1.0f - fraction) * result.channels.alpha + fraction * end.channels.alpha + 0.5f);
    result.channels.red = (unsigned char)(long)((1.0f - fraction) * result.channels.red + fraction * end.channels.red + 0.5f);
    result.channels.green = (unsigned char)(long)((1.0f - fraction) * result.channels.green + fraction * end.channels.green + 0.5f);
    result.channels.blue = (unsigned char)(long)((1.0f - fraction) * result.channels.blue + fraction * end.channels.blue + 0.5f);
    return result.value;
}
}

// @address 0xea4f8 ?evaluate_scalar@c_function_definition@@QBAMMM@Z
float c_function_definition::evaluate_scalar(float input, float range) const
{
  float result = 0.0f;
  if (m_data.address != 0 && m_data.size > 0)
    result = map_to_output_range_legacy((this)->evaluate_legacy(input, range));
  return result;
}

// @address 0xea558 ?evaluate_legacy@c_function_definition@@QBAMMM@Z
float c_function_definition::evaluate_legacy(float input, float range) const
{
  float result = 0.0f;
  const s_function_definition_internal *definition = (const s_function_definition_internal *)(const void *)m_data.address;
  if (definition != 0 && m_data.size > 0)
  {
    switch (definition->type)
    {
    case 0:
      result = input;
      break;
    case 1:
      if (((const s_function_definition_internal *)(const void *)m_data.address)->flags & 1)
        result = range;
      else
        result = 0.0f;
      break;
    case 2:
    {
      const c_transition_function_compact *function = (const c_transition_function_compact *)definition->compact_data;
      result = function->evaluate(input);
      if (definition->flags & 1)
        result = interpolate_linear(result, (function + 1)->evaluate(input), range);
      break;
    }
    case 9:
    {
      const c_exponent_function_compact *function =
          (const c_exponent_function_compact *)((const s_function_definition_internal *)(const void *)m_data.address)->compact_data;
      result = function->evaluate(input);
      if (((const s_function_definition_internal *)(const void *)m_data.address)->flags & 1)
        result = interpolate_linear(result, (function + 1)->evaluate(input), range);
      break;
    }
    case 3:
    {
      const c_periodic_function_compact *function = (const c_periodic_function_compact *)definition->compact_data;
      result = function->evaluate(input);
      if (definition->flags & 1)
        result = interpolate_linear(result, (function + 1)->evaluate(input), range);
      break;
    }
    case 4:
    {
      const c_linear_function_compact *function = (const c_linear_function_compact *)definition->compact_data;
      result = function->evaluate(input);
      if (definition->flags & 1)
        result = interpolate_linear(result, (function + 1)->evaluate(input), range);
      break;
    }
    case 5:
    {
      const c_linear_key_function *function = (const c_linear_key_function *)definition->compact_data;
      result = function->evaluate(input);
      if (definition->flags & 1)
        result = interpolate_linear(result, (function + 1)->evaluate(input), range);
      break;
    }
    case 7:
    {
      const c_spline_function_compact *function = (const c_spline_function_compact *)definition->compact_data;
      result = function->evaluate(input);
      if (definition->flags & 1)
        result = interpolate_linear(result, (function + 1)->evaluate(input), range);
      break;
    }
    case c_function_definition::_function_type_spline2:
    {
      const c_spline2_function_compact *function =
          (const c_spline2_function_compact *)((const s_function_definition_internal *)(const void *)m_data.address)->compact_data;
      result = function->evaluate(input);
      if (((const s_function_definition_internal *)(const void *)m_data.address)->flags & 1)
        result = interpolate_linear(result, (function + 1)->evaluate(input), range);
      break;
    }
    case 8:
    {
      const c_multi_part_function_compact *function =
          (const c_multi_part_function_compact *)((const s_function_definition_internal *)(const void *)m_data.address)->compact_data;
      result = function->evaluate(input);
      if (((const s_function_definition_internal *)(const void *)m_data.address)->flags & 1)
      {
        const c_multi_part_function_compact *next = c_multi_part_function_iterator::increment_multi_part_function_pointer(
            const_cast<c_multi_part_function_compact *>(function), 1);
        result = interpolate_linear(result, next->evaluate(input), range);
      }
      break;
    }
    case 6:
    default:
      break;
    }
    definition = (const s_function_definition_internal *)(const void *)m_data.address;
    result = unexclude_value(result);
  }
  if (definition->flags & 4)
  {
    result = (float)__fsel(result - 0.0f, result, 0.0f);
    result = (float)__fsel(result - 1.0f, 1.0f, result);
  }
  return result;
}

// @address 0xea900 ?evaluate@c_multi_part_function_compact@@QBAMM@Z
inline float c_multi_part_function_compact::evaluate(float value) const
{
    bool found = false;
    float result = 0.0f;
    const s_function_part *part = m_parts;
    for (long index = 0; index < m_part_count && !found; ++index)
    {
        switch (part->type)
        {
        case 4:
            if (value <= part->domain_max)
            {
                result = ((const c_linear_function_compact *)&part[1])->evaluate(value);
                found = true;
            }
            break;
        case 7:
            if (value <= part->domain_max)
            {
                result = ((const c_spline_function_compact *)&part[1])->evaluate(value);
                found = true;
            }
            break;
        case c_function_definition::_function_type_spline2:
            if (value <= part->domain_max)
            {
                result = ((const c_spline2_function_compact *)&part[1])->evaluate(value);
                found = true;
            }
            break;
        }
        part = increment_function_part_pointer(part);
    }
    return result;
}

// @address 0xeaa18 ?map_to_output_range_legacy@c_function_definition@@QBAMM@Z
float c_function_definition::map_to_output_range_legacy(float value) const
{
  const s_function_definition_internal *definition = (const s_function_definition_internal *)(const void *)m_data.address;
  if (!definition->constant) {
    float lower = definition->lower;
    float upper = definition->upper;
    if (definition->flags & 4)
      value = MIN(MAX(value, k_real_zero), k_real_one);
    value = lower + (upper - lower) * value;
  }
  return value;
}

static __forceinline void function_definitions_pixel32_to_real_rgb_color(unsigned long pixel, real_rgb_color *color)
{
    const float scale = 1.0f / 255.0f;
    color->red = (float)((pixel >> 16) & 255) * scale;
    color->green = (float)((pixel >> 8) & 255) * scale;
    color->blue = (float)(pixel & 255) * scale;
}


// @address 0x116b10 ?map_to_color_range_legacy@c_function_definition@@QBAXMPATreal_rgb_color@@@Z
void c_function_definition::map_to_color_range_legacy(float value, real_rgb_color *out_color) const
{
    if (m_data.address && m_data.size > 0)
        function_definitions_pixel32_to_real_rgb_color(map_to_color_range_legacy(value), out_color);
    else
    {
        const real_argb_color *white = function_get_default_white_color();
        *out_color = white->rgb;
    }
}

// @address 0x116bf8 ?map_to_color_range_legacy@c_function_definition@@QBAKM@Z
unsigned long c_function_definition::map_to_color_range_legacy(float value) const
{
    unsigned long result = 0xffffffff;
    if (m_data.address != 0 && m_data.size > 0)
    {
        const s_function_definition_internal *definition = (const s_function_definition_internal *)(const void *)m_data.address;
        if (definition->type == 1 && (!(definition->flags & 1) || definition->color_count <= 1))
            result = definition->colors[0];
        else
        {
            float fraction = MIN(MAX(value, 0.0f), 1.0f);
            switch (definition->color_count)
            {
            case 0:
                {
                    unsigned char channel = (unsigned char)(long)(fraction * 255.0f + 0.5f);
                    result = 0xff000000 | (channel << 16) | (channel << 8) | channel;
                }
                break;
            case 1:
                result = definition->colors[0];
                break;
            case 2:
                {
                    unsigned long lower = definition->colors[0];
                    unsigned long upper = definition->colors[3];
                    result = function_definitions_blend_pixel32s(fraction, lower, upper);
                }
                break;
            case 3:
                {
                    fraction *= 2.0f;
                    long index = (long)((float)floor(fraction) + 0.5f);
                    fraction -= (float)index;
                    if (index >= 2)
                    {
                        index = 1;
                        fraction = 1.0f;
                    }
                    unsigned long lower, upper;
                    if (index == 0)
                    {
                        lower = definition->colors[0];
                        upper = definition->colors[1];
                    }
                    else
                    {
                        lower = definition->colors[1];
                        upper = definition->colors[3];
                    }
                    result = function_definitions_blend_pixel32s(fraction, lower, upper);
                }
                break;
            case 4:
                {
                    fraction *= 3.0f;
                    long index = (long)((float)floor(fraction) + 0.5f);
                    fraction -= (float)index;
                    if (index >= 3)
                    {
                        index = 2;
                        fraction = 1.0f;
                    }
                    unsigned long lower = definition->colors[index];
                    unsigned long upper = definition->colors[index + 1];
                    result = function_definitions_blend_pixel32s(fraction, lower, upper);
                }
                break;
            }
        }
    }
    return result;
}

// @address 0x2a6a28 ?evaluate@c_spline2_function_compact@@QBAMM@Z
inline float c_spline2_function_compact::evaluate(float value) const
{
    value = (value - m_offset) / m_scale;
    if (value < 0.0f)
        value = -power(-value, m_exponent);
    else
        value = power(value, m_exponent);
    return c_spline_function_compact::evaluate(value);
}

// @address 0x2a6ab8 ?evaluate@c_exponent_function_compact@@QBAMM@Z
inline float c_exponent_function_compact::evaluate(float value) const
{
    float result = 0.0f;
    if (fabs(m_exponent - 0.0f) < 0.0001f ||
        (m_exponent < 0.0f && fabs(value - 0.0f) < 0.0001f))
        result = 1.0f;
    else
    {
        result = power(value, m_exponent);
        result = interpolate_linear(m_lower, m_upper, result);
    }
    return result;
}

// @address 0x2a6b48 ?evaluate@c_linear_key_function@@QBAMM@Z
inline float c_linear_key_function::evaluate(float value) const
{
    real_vector4d weights;
    weights.x = 1.0f;
    weights.y = (value - m_keys.x) * m_inverse_ranges.x;
    weights.y = MIN(MAX(weights.y, 0.0f), 1.0f);
    weights.z = (value - m_keys.y) * m_inverse_ranges.y;
    weights.z = MIN(MAX(weights.z, 0.0f), 1.0f);
    weights.w = (value - m_keys.z) * m_inverse_ranges.z;
    weights.w = MIN(MAX(weights.w, 0.0f), 1.0f);
    return dot_product4d(&weights, &m_coefficients);
}

// @address 0x2a6c18 ?increment_multi_part_function_pointer@c_multi_part_function_iterator@@SAPAVc_multi_part_function_compact@@PAV2@E@Z
inline c_multi_part_function_compact *c_multi_part_function_iterator::increment_multi_part_function_pointer(
    c_multi_part_function_compact *function, unsigned char count)
{
    count = 1;
    for (byte index = 0; index < count; ++index)
        function = (c_multi_part_function_compact *)((char *)function + function->get_size_in_bytes());
    return function;
}
