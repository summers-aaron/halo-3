#ifndef AI_AI_PLAYER_TLS_H
#define AI_AI_PLAYER_TLS_H

#include "ai/ai_player_types.h"

// Four campaign-player records in the AI player allocator's TLS slot.
extern "C" { extern __declspec(thread) ai_player_state *g_ai_players; }

static __forceinline ai_player_state *tls_ai_players() { return g_ai_players; }
static __forceinline void tls_ai_players_set(ai_player_state *players) { g_ai_players = players; }

#define TLS_AI_PLAYERS g_ai_players

#endif // AI_AI_PLAYER_TLS_H
