#ifndef SOUND_GAME_SOUND_LISTENER_DEFINITION_H
#define SOUND_GAME_SOUND_LISTENER_DEFINITION_H

#include "cseries/cseries_types.h"
#include "math/real_math_types.h"
#include "tag_files/tag_types.h"

struct s_planar_fog_patchy_fog;

enum { k_number_of_fog_sound_events = 2 };

struct s_planar_fog_definition
{
    word flags;
    short priority;
    string_id global_material_name;
    short runtime_global_material_index;
    real maximum_density;
    real opaque_distance;
    real opaque_depth;
    real_bounds atmospheric_planar_depth;
    real eye_offset_scale;
    real_rgb_color color;
    c_typed_tag_block<s_planar_fog_patchy_fog> patchy_fog;
    s_tag_reference background_sound;
    s_tag_reference sound_environment;
    real environment_damping_factor;
    real background_sound_gain;
    s_tag_reference sound_events[k_number_of_fog_sound_events]; // Enter, then exit.
};

typedef char planar_fog_definition_size[(sizeof(s_planar_fog_definition) == 0x84) ? 1 : -1];
typedef char planar_fog_maximum_density_offset[(offsetof(s_planar_fog_definition, maximum_density) == 0xc) ? 1 : -1];
typedef char planar_fog_sound_events_offset[(offsetof(s_planar_fog_definition, sound_events) == 0x64) ? 1 : -1];

#endif
