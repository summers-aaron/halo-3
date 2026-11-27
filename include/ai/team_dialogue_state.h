#ifndef AI_TEAM_DIALOGUE_STATE_H
#define AI_TEAM_DIALOGUE_STATE_H
#include <stddef.h>
// ai_dialogue.cpp indexes the prefix by vocalization and stores cooldown expiry
// ticks. ai.cpp initializes and updates obsessed_object at +0x7d8.
struct team_dialogue_state
{
    long vocalization_cooldown_times[500];
    long first;
    short count;
    long obsessed_object;
};
typedef char team_dialogue_state_size[(sizeof(team_dialogue_state) == 2012) ? 1 : -1];
typedef char team_dialogue_state_queue_offset[(offsetof(team_dialogue_state, first) == 0x7d0) ? 1 : -1];
typedef char team_dialogue_state_count_offset[(offsetof(team_dialogue_state, count) == 0x7d4) ? 1 : -1];
typedef char team_dialogue_state_obsessed_object_offset[(offsetof(team_dialogue_state, obsessed_object) == 0x7d8) ? 1 : -1];
#endif
