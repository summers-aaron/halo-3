#ifndef SAVED_GAMES_GAME_STATE_RUNTIME_H
#define SAVED_GAMES_GAME_STATE_RUNTIME_H

struct s_game_state_compressed_buffer;

extern "C" { void game_state_lock_and_halt_render_thread(int); }
extern "C" { void game_state_unlock_and_resume_render_thread(long); }
extern "C" { int game_state_process_pending_changes(void); }
extern "C" { unsigned char game_state_load_from_compressed_buffer(s_game_state_compressed_buffer *state, int, int, int); }

#endif // SAVED_GAMES_GAME_STATE_RUNTIME_H
