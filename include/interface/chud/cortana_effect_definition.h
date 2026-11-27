#ifndef INTERFACE_CHUD_CORTANA_EFFECT_DEFINITION_H
#define INTERFACE_CHUD_CORTANA_EFFECT_DEFINITION_H
#include "tag_files/tag_types.h"
#include <stddef.h>

struct s_cortana_effect_postprocessing
{
    s_tag_block field_of_view;
    s_tag_block hue;
    s_tag_block saturation;
    s_tag_block rumble;
    s_tag_block hud_brightness;
    s_tag_block hud_shake_amount;
};
struct s_cortana_effect_sound
{
    long time;
    s_tag_reference sound;
};

struct s_cortana_effect_definition
{
    long scenario_name;
    long frame_count;
    c_typed_tag_block<s_cortana_effect_sound> sounds;
    s_tag_data data1;
    s_tag_data data2;
    s_tag_reference cinematic_scene;
    long anchor_name;
    c_typed_tag_block<s_cortana_effect_postprocessing> postprocessing;
    s_tag_block gravemind;
    s_tag_block unused;
    s_tag_block cortana;
};
typedef char cortana_effect_sound_size[(sizeof(s_cortana_effect_sound) == 0x14) ? 1 : -1];
typedef char cortana_effect_sound_reference_offset[(offsetof(s_cortana_effect_sound, sound.index) == 0x10) ? 1 : -1];
typedef char cortana_effect_definition_size[(sizeof(s_cortana_effect_definition) == 0x80) ? 1 : -1];
typedef char cortana_effect_postprocessing_offset[(offsetof(s_cortana_effect_definition, postprocessing) == 0x50) ? 1 : -1];
typedef char cortana_effect_rumble_offset[(offsetof(s_cortana_effect_postprocessing, rumble) == 0x24) ? 1 : -1];
#endif
