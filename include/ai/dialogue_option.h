#ifndef AI_DIALOGUE_OPTION_H
#define AI_DIALOGUE_OPTION_H
#include <stddef.h>
#include "ai/dialogue_event.h"

// March 82eb9ce0 copies the complete 80-byte event at +20.
class c_dialogue_option
{
public:
    short vocalization_index;
    long speaker_object_index;
    long listener_object_index;
    long parameter;
    float weight;
    c_dialogue_event event;
    void set(c_dialogue_event *, short, long, long, long, float);
};
typedef char dialogue_option_size[(sizeof(void *) != 4 || sizeof(c_dialogue_option) == 100) ? 1 : -1];
typedef char dialogue_option_event_offset[(sizeof(void *) != 4 || ((offsetof(c_dialogue_option, event) == 20))) ? 1 : -1];
typedef char dialogue_option_speaker_offset[
    (sizeof(void *) != 4 || ((offsetof(c_dialogue_option, event) + offsetof(c_dialogue_event, speaker) == 68))) ? 1 : -1];
typedef char dialogue_option_target_offset[
    (sizeof(void *) != 4 || ((offsetof(c_dialogue_option, event) + offsetof(c_dialogue_event, target_index) == 76))) ? 1 : -1];
#endif
