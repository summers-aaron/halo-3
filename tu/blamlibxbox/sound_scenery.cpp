// blamlibxbox/sound_scenery.obj: from the records 0x577950
// @flags /TP /O2 /Oc
#include "objects/object_header.h"
#include "sound/sound_scenery.h"

struct s_scenario_sound_scenery {
    char object_placement[84];
    long volume_type;
    float height;
    float minimum_distance;
    float maximum_distance;
    float inner_cone_angle;
    float outer_cone_angle;
    real_decibel outer_cone_gain;
};
// @address 0x577950 ?sound_scenery_place@@YAXJPAUs_scenario_sound_scenery@@@Z
void sound_scenery_place(long object_index, s_scenario_sound_scenery *placement) {
    s_sound_scenery_datum *sound_scenery = OBJECT_GET(object_index, s_sound_scenery_datum, _object_mask_sound_scenery);
    sound_scenery->volume_type = placement->volume_type;
    sound_scenery->height = placement->height;
    sound_scenery->minimum_distance = placement->minimum_distance;
    sound_scenery->maximum_distance = placement->maximum_distance;
    sound_scenery->inner_cone_angle = placement->inner_cone_angle;
    sound_scenery->outer_cone_angle = placement->outer_cone_angle;
    sound_scenery->outer_cone_gain = placement->outer_cone_gain;
}
