#ifndef AI_EVENT_HANDLING_H
#define AI_EVENT_HANDLING_H

// Functions defined in tu/ailibxbox/event_handling.cpp.

struct ai_sound_data;

void handle_sound(long actor_index, long object_index, short clump_count, bool test_audibility, ai_sound_data *sound);
void handle_intuition(long actor_index, long object_index, long prop_index, short clump_count);
void actor_handle_bump(long actor_index, long object_index);

struct ai_information_data;

bool actor_handle_speech(long actor_index, long other_actor_index, ai_information_data *information);

#endif // AI_EVENT_HANDLING_H
