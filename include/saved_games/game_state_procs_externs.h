#ifndef SAVED_GAMES_GAME_STATE_PROCS_EXTERNS_H
#define SAVED_GAMES_GAME_STATE_PROCS_EXTERNS_H



extern "C" { extern void (*g_game_state_unlock_procs[3])(long); }
extern "C" { extern void (*g_game_state_lock_procs[4])(long); }

struct s_game_state_procs_globals;
extern "C" {
extern volatile int g_game_state_render_fence;
extern s_game_state_procs_globals g_game_state_procs;
}

#endif // SAVED_GAMES_GAME_STATE_PROCS_EXTERNS_H
