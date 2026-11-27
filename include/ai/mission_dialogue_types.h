#ifndef AI_MISSION_DIALOGUE_TYPES_H
#define AI_MISSION_DIALOGUE_TYPES_H

#include <stddef.h>
#include "tag_files/tag_types.h"

// Mission dialogue (mdlg) tag: lines, each with per-voice variants. Type names from the March 2007 maps (mangled actor_can_fill_role, cs_cast_internal).
struct s_ai_mission_dialogue_variant
{
    long id;
    s_tag_reference sound;
    long playback_label;
};

struct mission_dialogue_line
{
    long name;
    c_typed_tag_block<s_ai_mission_dialogue_variant> variants;
    long playback_label;
};

struct ai_mission_dialogue
{
    c_typed_tag_block<mission_dialogue_line> lines;
};

typedef char mission_dialogue_variant_size[(sizeof(s_ai_mission_dialogue_variant) == 0x18) ? 1 : -1];
typedef char mission_dialogue_variant_sound_offset[(offsetof(s_ai_mission_dialogue_variant, sound) == 4) ? 1 : -1];
typedef char mission_dialogue_variant_playback_offset[(offsetof(s_ai_mission_dialogue_variant, playback_label) == 0x14) ? 1 : -1];
typedef char mission_dialogue_line_size[(sizeof(mission_dialogue_line) == 0x14) ? 1 : -1];
typedef char mission_dialogue_line_variants_offset[(offsetof(mission_dialogue_line, variants) == 4) ? 1 : -1];
typedef char mission_dialogue_line_playback_offset[(offsetof(mission_dialogue_line, playback_label) == 0x10) ? 1 : -1];

#endif
