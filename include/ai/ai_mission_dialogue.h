#ifndef AI_AI_MISSION_DIALOGUE_H
#define AI_AI_MISSION_DIALOGUE_H

// Functions defined in tu/ailibxbox/ai_mission_dialogue.cpp that other units call.

extern "C" bool mission_dialogue_play_line(long p0, long p1, bool p2, long p3, float *p4);
void __tls_set_g_scenes_played_allocator(void *allocator);

#endif // AI_AI_MISSION_DIALOGUE_H
