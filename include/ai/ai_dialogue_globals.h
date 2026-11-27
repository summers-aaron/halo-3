#ifndef AI_AI_DIALOGUE_GLOBALS_H
#define AI_AI_DIALOGUE_GLOBALS_H

struct s_data_array;
struct team_dialogue_state;

extern "C" {
extern __declspec(thread) s_data_array *g_ai_dialogue_records;
extern __declspec(thread) team_dialogue_state *g_team_dialogue_state;
}

#endif // AI_AI_DIALOGUE_GLOBALS_H
