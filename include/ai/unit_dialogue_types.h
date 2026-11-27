#ifndef AI_UNIT_DIALOGUE_TYPES_H
#define AI_UNIT_DIALOGUE_TYPES_H

#include <stddef.h>
#include "units/unit_speech_item.h"

struct s_unit_dialogue
{
    long dialogue_index;
    long designator;
    long playback_label; // mission_dialogue_play_line: March 82f9bddc reads +8
    unit_speech_item current; // unit_dialogue.cpp: active request begins at +0xc
    short pain_time;
    short pain_count;
    short minor_time;
    short major_time;
    long pad48;
    bool started;
    bool notified;
    bool finished;
    short duration;
    short notify_delay;
    short finish_delay;
    short pad56;
    long sound_handle;
};

typedef char unit_dialogue_playback_offset[(offsetof(s_unit_dialogue, playback_label) == 8) ? 1 : -1];

typedef char unit_dialogue_current_type_offset[(offsetof(s_unit_dialogue, current) + offsetof(unit_speech_item, type) == 0xe) ? 1 : -1];

typedef char unit_dialogue_duration_offset[(offsetof(s_unit_dialogue, duration) == 0x50) ? 1 : -1];
typedef char unit_dialogue_state_size[(sizeof(s_unit_dialogue) == 0x5c) ? 1 : -1];

#endif // AI_UNIT_DIALOGUE_TYPES_H
