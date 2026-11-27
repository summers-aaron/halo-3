#ifndef AI_DIALOGUE_EVENT_H
#define AI_DIALOGUE_EVENT_H

#include "ai/ai_information_data.h"

struct unit_datum;
struct actor_datum;

// March 82eb77a8 constructs the event; prop_dialogue.cpp consumes this layout.
class c_dialogue_event
{
public:
    c_dialogue_event() {}
    c_dialogue_event(short, long, long, long, short, ai_information_data *);
    short type;
    long subtype;
    short vocalization;
    long clump_index;
    ai_information_data information;
    long unit_index;
    unit_datum *unit;
    actor_datum *speaker;
    short unit_team, speaker_team;
    long target_index;
    unit_datum *target_unit;
    actor_datum *target_speaker;
    long other_unit_index;
    short target_team, target_speaker_team, relation;
};

typedef char c_dialogue_event_size_check[(sizeof(void *) != 4 || sizeof(c_dialogue_event) == 0x50) ? 1 : -1];

#endif
