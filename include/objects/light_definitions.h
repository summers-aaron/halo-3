#ifndef OBJECTS_LIGHT_DEFINITIONS_H
#define OBJECTS_LIGHT_DEFINITIONS_H

#include <stddef.h>
#include "math/function_definitions.h"

struct light_function_property {
    long input_name;
    long range_name;
    char spare_before_function[8];
    c_function_definition function;
};
struct light_definition {
    unsigned long flags;
    short type;
    short pad6;
    float radius;
    float near_distance;
    float far_distance;
    float field_of_view;
    light_function_property color;
    light_function_property intensity;
    char spare_before_gel_index[108-96];
    long gel_index;
    float simple_light_intensity;
    float simple_light_exponent;
    float simple_light_diffuse;
    float duration;
    char fade_near_index;
    char fade_far_index;
    char fade_exponent_index;
    char spare_before_lens_flare_index[144-131];
    long lens_flare_index;
};

typedef char light_definition_size_check[(sizeof(light_definition) == 0x94) ? 1 : -1];
typedef char light_definition_gel_check[(offsetof(light_definition, gel_index) == 0x6c) ? 1 : -1];
typedef char light_definition_simple_check[(offsetof(light_definition, simple_light_intensity) == 0x70) ? 1 : -1];

#endif
