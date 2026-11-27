#ifndef SOUND_SOUND_SCENERY_H
#define SOUND_SOUND_SCENERY_H

#include "objects/object_datum.h"
#include "sound/game_sound_definitions.h"

// Functions defined in tu/blamlibxbox/sound_scenery.cpp.

struct s_scenario_sound_scenery;

struct s_sound_scenery_datum
{
    object_datum object;
    long volume_type;
    float height;
    float minimum_distance;
    float maximum_distance;
    float inner_cone_angle;
    float outer_cone_angle;
    real_decibel outer_cone_gain;
};

void sound_scenery_place(long object_index, s_scenario_sound_scenery *placement);

#endif // SOUND_SOUND_SCENERY_H
