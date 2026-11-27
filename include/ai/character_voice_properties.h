#ifndef AI_CHARACTER_VOICE_PROPERTIES_H
#define AI_CHARACTER_VOICE_PROPERTIES_H

#include <stddef.h>
#include "tag_files/tag_types.h"

// Character voice block: actor_select_character_variant reads the reference index at +0xc,
// designator at +0x10 and weight at +0x14; cl_engine uses the same 24-byte entries.
struct s_character_voice
{
    s_tag_reference dialogue;
    long designator;
    float weight;
};

struct character_voice_properties
{
    c_typed_tag_block<s_character_voice> voices;
};

typedef char character_voice_size[(sizeof(s_character_voice) == 0x18) ? 1 : -1];
typedef char character_voice_designator_offset[(offsetof(s_character_voice, designator) == 0x10) ? 1 : -1];
typedef char character_voice_weight_offset[(offsetof(s_character_voice, weight) == 0x14) ? 1 : -1];
typedef char character_voice_properties_size[(sizeof(character_voice_properties) == 0xc) ? 1 : -1];

#endif // AI_CHARACTER_VOICE_PROPERTIES_H
