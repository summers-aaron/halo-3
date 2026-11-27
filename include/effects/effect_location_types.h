#ifndef EFFECTS_EFFECT_LOCATION_TYPES_H
#define EFFECTS_EFFECT_LOCATION_TYPES_H

#include <stddef.h>
#include "cseries/cseries_wrapped.h"
#include "math/matrix_math_types.h"

struct effect_location_datum
{
    short identifier;
    unsigned short node_index;
    long first_render_location_index;
    long next_location_index;
    union {
        real_matrix4x3 matrix;
        struct { float scale; real_vector3d forward, left, up, vector; };
    };
};

typedef char effect_location_size[(sizeof(effect_location_datum) == 0x40) ? 1 : -1];
typedef char effect_location_vector_offset[(offsetof(effect_location_datum, vector) == 0x34) ? 1 : -1];

struct s_effect_render_location {
    short next_index;
    c_flags_no_init<long, short, 1> flags;
    char data[52];
};
struct s_effect_render_location_data {
    long location_index;
    char data[52];
};

typedef char effect_render_location_size[(sizeof(s_effect_render_location) == 0x38) ? 1 : -1];

#endif
