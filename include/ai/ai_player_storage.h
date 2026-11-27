#ifndef AI_AI_PLAYER_STORAGE_H
#define AI_AI_PLAYER_STORAGE_H
#include "ai/ai_player_types.h"
extern "C" __declspec(thread) ai_player_state *g_ai_players;
static __forceinline ai_player_state *tls_ai_player_states() { return g_ai_players; }
#endif
