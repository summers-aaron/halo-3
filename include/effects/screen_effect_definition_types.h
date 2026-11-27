#ifndef EFFECTS_SCREEN_EFFECT_DEFINITION_TYPES_H
#define EFFECTS_SCREEN_EFFECT_DEFINITION_TYPES_H

#include <stddef.h>
#include "math/function_definitions.h"
#include "tag_files/tag_types.h"

struct s_light_response_function
{
    c_function_definition definition;
};

struct s_screen_effect_response
{
    long pad0;
    unsigned long flags;
    float input_limit;
    s_light_response_function input_function;
    union { float lifespan; float divisor; };
    s_light_response_function scale_function;
    s_light_response_function angle_function;
};

struct render_objects_static_lighting_screen_effect
{
    s_screen_effect_response response;
    float values[14];
};

struct render_objects_static_lighting_screen_effect_definition
{
    long count;
    TAG_POINTER(render_objects_static_lighting_screen_effect) effects;
};

typedef char screen_effect_response_lifespan_offset[(offsetof(s_screen_effect_response, lifespan) == 0x20) ? 1 : -1];
typedef char screen_effect_size[(sizeof(render_objects_static_lighting_screen_effect) == 0x84) ? 1 : -1];

#endif
