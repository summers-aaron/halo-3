#ifndef MATH_FUNCTION_DEFINITIONS_H
#define MATH_FUNCTION_DEFINITIONS_H

#include "tag_files/tag_types.h"

union real_rgb_color;

// Serialized scalar functions carry their compact representation in a tag-data
// descriptor. March math/function_definitions.cpp supplies these methods.
class c_function_definition
{
public:
    enum e_function_type
    {
        _function_type_linear = 4,
        _function_type_exponent = 9,
        _function_type_spline2 = 10,
    };

    float evaluate_scalar(float input, float range) const;
    float evaluate_legacy(float input, float range) const;
    float map_to_output_range_legacy(float value) const;
    unsigned long map_to_color_range_legacy(float value) const;
    void map_to_color_range_legacy(float value, real_rgb_color *out_color) const;

    __forceinline void evaluate_color(float input, float range, real_rgb_color *out_color) const
    {
        map_to_color_range_legacy(evaluate_legacy(input, range), out_color);
    }

    s_tag_data m_data;

private:
    float exclude_value(float value) const;
    float unexclude_value(float value) const;
};

typedef char function_definition_size[(sizeof(long) != 4 || sizeof(c_function_definition) == 0x14) ? 1 : -1];

#endif // MATH_FUNCTION_DEFINITIONS_H
