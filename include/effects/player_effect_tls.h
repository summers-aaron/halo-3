#ifndef EFFECTS_PLAYER_EFFECT_TLS_H
#define EFFECTS_PLAYER_EFFECT_TLS_H

struct s_player_effect_globals;
struct s_rumble_globals;

extern "C" {
extern __declspec(thread) s_player_effect_globals *g_player_effect_globals;
extern __declspec(thread) s_rumble_globals *g_rumble_globals;
}

#endif // EFFECTS_PLAYER_EFFECT_TLS_H
