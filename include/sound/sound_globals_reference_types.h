#ifndef SOUND_SOUND_GLOBALS_REFERENCE_TYPES_H
#define SOUND_SOUND_GLOBALS_REFERENCE_TYPES_H

#include <stddef.h>

#include "tag_files/tag_types.h"

// Game sounds resolve their acoustics through the default acoustics reference.
struct sound_globals_reference
{
    s_tag_reference sound_classes;
    s_tag_reference sound_effects;                // sound effect collection ('sfx+'), searched by label
    union
    {
        s_tag_reference sound_mix;
        s_tag_reference first_person_stereo_identifiers;
    };
    s_tag_reference sound_dialogue_constants;
    s_tag_reference default_acoustics;
};

typedef char sound_globals_reference_size[(sizeof(sound_globals_reference) == 0x50) ? 1 : -1];
typedef char sound_globals_reference_sound_effects_offset[(sizeof(void *) != 4 || offsetof(sound_globals_reference, sound_effects) == 0x10) ? 1 : -1];

typedef char sound_globals_reference_mix_offset[(offsetof(sound_globals_reference, sound_mix.index) == 0x2c) ? 1 : -1];

typedef char sound_globals_reference_dialogue_constants_offset[
    (offsetof(sound_globals_reference, sound_dialogue_constants.index) == 0x3c) ? 1 : -1];

typedef char sound_globals_reference_acoustics_offset[(offsetof(sound_globals_reference, default_acoustics.index) == 0x4c) ? 1 : -1];

#endif // SOUND_SOUND_GLOBALS_REFERENCE_TYPES_H
