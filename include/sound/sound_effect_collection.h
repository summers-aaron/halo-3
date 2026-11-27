#ifndef SOUND_SOUND_EFFECT_COLLECTION_H
#define SOUND_SOUND_EFFECT_COLLECTION_H

// The sound effect collection tag ('sfx+') and the platform playback parameters it names. Two collections are
// searched by label (sound_manager_platform_playback.cpp): the scenario's and the game globals' sound effects.

#include <stddef.h>
#include "cseries/cseries_types.h"
#include "tag_files/tag_types.h"

struct sound_effect_manager_definition;
struct s_platform_sound_override_mixbin_definition;
struct s_sound_filter_definition;
struct s_sound_pitch_lfo_definition;
struct s_sound_filter_lfo_definition;

// Platform playback parameters for a sound effect.
struct s_platform_sound_playback_definition
{
    c_typed_tag_block<s_platform_sound_override_mixbin_definition> override_mixbins;
    unsigned long flags;
    long channel;
    unsigned long : 32; // Serialized padding between the radio channel and filters.
    c_typed_tag_block<s_sound_filter_definition> filters;
    c_typed_tag_block<s_sound_pitch_lfo_definition> pitch_lfos;
    c_typed_tag_block<s_sound_filter_lfo_definition> filter_lfos;
    c_typed_tag_block<sound_effect_manager_definition> effects;
};

struct sound_effect_collection_entry
{
    string_id name;
    s_platform_sound_playback_definition playback;
};

struct sound_effect_collection_definition
{
    c_typed_tag_block<sound_effect_collection_entry> sound_effects;
};

typedef char platform_sound_playback_definition_size[(sizeof(s_platform_sound_playback_definition) == 0x48) ? 1 : -1];
typedef char sound_effect_collection_entry_size[(sizeof(sound_effect_collection_entry) == 0x4c) ? 1 : -1];

typedef char platform_sound_playback_channel_offset[(offsetof(s_platform_sound_playback_definition, channel) == 0x10) ? 1 : -1];

typedef char platform_sound_playback_effects_offset[(offsetof(s_platform_sound_playback_definition, effects) == 0x3c) ? 1 : -1];

typedef char platform_sound_playback_filters_offset[(offsetof(s_platform_sound_playback_definition, filters) == 0x18) ? 1 : -1];

#endif // SOUND_SOUND_EFFECT_COLLECTION_H
