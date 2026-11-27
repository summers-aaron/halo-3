#ifndef SOUND_DEFAULT_ACOUSTICS_DEFINITION_H
#define SOUND_DEFAULT_ACOUSTICS_DEFINITION_H
#include <stddef.h>
#include "tag_files/tag_types.h"
#include "sound/game_sound_definitions.h"

// Default reverb, background override and transition sounds for game_sound.
struct s_default_acoustics_definition
{
    s_tag_reference reverb;
    s_tag_reference background_sound;
    real_decibel gain;
    real_decibel background_gain;
    s_tag_reference enter_sound;
    s_tag_reference exit_sound;
};
typedef char default_acoustics_size[(sizeof(s_default_acoustics_definition) == 0x48) ? 1 : -1];
typedef char default_acoustics_background_offset[(offsetof(s_default_acoustics_definition, background_sound.index) == 0x1c) ? 1 : -1];
typedef char default_acoustics_enter_offset[(offsetof(s_default_acoustics_definition, enter_sound.index) == 0x34) ? 1 : -1];
typedef char default_acoustics_exit_offset[(offsetof(s_default_acoustics_definition, exit_sound.index) == 0x44) ? 1 : -1];
#endif
