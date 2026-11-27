#ifndef SOUND_GAME_SOUND_TYPES_H
#define SOUND_GAME_SOUND_TYPES_H

#include "cseries/cseries_wrapped.h"
#include "math/real_math_types.h"
#include <stddef.h>

// Position, orientation, and velocity of a sound in the world.
struct s_sound_location { real_point3d position; real_quaternion rotation; real_vector3d velocity; };

struct s_sound_attachment_data {
    unsigned char node_index, flags;
    real_point3d position;
    real_vector3d forward;
    union
    {
        long identifier[2];
        real_bounds additional_gain_range;
    };
    long object_index;
};

typedef char sound_location_size[(sizeof(s_sound_location) == 0x28) ? 1 : -1];
typedef char sound_attachment_data_size[(sizeof(s_sound_attachment_data) == 0x28) ? 1 : -1];
typedef char sound_attachment_object_offset[(offsetof(s_sound_attachment_data, object_index) == 0x24) ? 1 : -1];
typedef char sound_attachment_gain_offset[(offsetof(s_sound_attachment_data, additional_gain_range) == 0x1c) ? 1 : -1];

// Looping sound record; UI and CHUD sounds share the flags and scale fields.
struct game_looping_sound_datum
{
    short identifier;
    signed char state;
    signed char type;
    // March: c_flags_no_init<e_game_looping_sound_flag, unsigned short, 11>; the enumerator names are not in any debug map.
    c_flags_no_init<long, unsigned short, 11> flags;
    short location;
    float scale;
    long definition_index;
    long object_index;
    long timeout;
    long unique_identifier;
};

typedef char game_looping_sound_size[(sizeof(game_looping_sound_datum) == 28) ? 1 : -1];
typedef char game_looping_sound_scale_offset[(offsetof(game_looping_sound_datum, scale) == 8) ? 1 : -1];

struct s_data_array;


#endif
