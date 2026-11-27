#ifndef MATH_FUNCTION_DEFINITION_TYPES_H
#define MATH_FUNCTION_DEFINITION_TYPES_H

#include "math/real_math.h"
#include "math/periodic_functions.h"
#include "math/function_definitions.h"

struct s_function_definition_internal {
  unsigned char type;
  unsigned char flags;
  // One byte, two readings: nonzero for the scalar path, the color count (1..4) for color functions.
  union { unsigned char constant; unsigned char color_count; };
  union {
    struct { float lower; float upper; };
    unsigned long colors[4];
  };
  float excluded_lower;
  float excluded_upper;
  long compact_data_size; // March get_editor_data adds this to get_data().
  char compact_data[1];
};

// Compact per-type function layouts (March math/function_definitions.cpp).
// Each function type interprets the tag-data blob after
// s_function_definition_internal through one of these classes.
class c_exponent_function_compact
{
public:
    float evaluate(float value) const;
private:
    float m_lower;
    float m_upper;
    float m_exponent;
};

class c_linear_key_function
{
public:
    float evaluate(float value) const;
private:
    // Control-point source data precedes the precomputed evaluation vectors.
    // March identifies k_max_control_points; its byte-swap table spans 0x50 bytes.
    unsigned char m_control_point_data[36];
    real_vector4d m_keys;
    struct { float x, y, z; } m_inverse_ranges;
    real_vector4d m_coefficients;
};

class c_multi_part_function_compact
{
public:
    struct s_function_part { unsigned char type; float domain_max; };
    float evaluate(float value) const;
    __forceinline long get_size_in_bytes() const;
private:
    static __forceinline long get_size_of_part(c_function_definition::e_function_type type)
    {
        long size;
        switch (type)
        {
        case 4: size = 16; break;
        case 7: size = 24; break;
        default: size = 36; break;
        }
        return size;
    }
    static __forceinline const s_function_part *increment_function_part_pointer(const s_function_part *part)
    {
        return (const s_function_part *)((const char *)part + get_size_of_part((c_function_definition::e_function_type)part->type));
    }
    long m_part_count;
    s_function_part m_parts[1];
};

class c_multi_part_function_iterator
{
public:
    static c_multi_part_function_compact *increment_multi_part_function_pointer(c_multi_part_function_compact *function, unsigned char count);
};

class c_spline_function_compact
{
public:
    __forceinline float evaluate(float value) const
    {
        real_vector4d powers;
        powers.x = value;
        powers.y = value;
        powers.z = value;
        powers.w = 1.0f;
        powers.y *= value;
        powers.x *= powers.y;
        return dot_product4d(&m_coefficients, &powers);
    }
private:
    real_vector4d m_coefficients;
};

class c_spline2_function_compact : public c_spline_function_compact
{
public:
    float evaluate(float value) const;
private:
    float m_offset;
    float m_scale;
    float m_exponent;
};

class c_transition_function_compact
{
public:
    float evaluate(float value) const
    {
        return transition_function_evaluate(m_type, value) * (m_upper - m_lower) + m_lower;
    }
private:
    unsigned char m_type;
    float m_lower;
    float m_upper;
};

class c_linear_function_compact
{
public:
    __forceinline float evaluate(float value) const
    {
        return m_slope * value + m_offset;
    }
private:
    float m_slope;
    float m_offset;
};

#endif // MATH_FUNCTION_DEFINITION_TYPES_H
