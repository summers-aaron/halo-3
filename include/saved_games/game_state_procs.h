#ifndef SAVED_GAMES_GAME_STATE_PROCS_H
#define SAVED_GAMES_GAME_STATE_PROCS_H

#include "saved_games/game_state_procs_types.h"

// Functions defined in tu/blamlibxbox/game_state_procs.cpp.

void game_state_call_before_load_procs();
void game_state_call_after_load_procs(long index, void *buffer);

struct s_game_state_compressed_buffer;

class c_game_state_autosave_listener : public c_game_state_proc_listener
{
public:
    virtual void set_mode(long mode, s_game_state_buffer_description *description);
    virtual void update(void *);
    virtual void dispose(void *);
};

extern c_game_state_autosave_listener g_game_state_autosave_listener;

struct s_async_xoverlapped_memory;
extern s_async_xoverlapped_memory g_game_state_proc_default_handler;

extern "C" unsigned char game_state_try_and_get_compressed_buffer(s_game_state_compressed_buffer *buffer);
extern "C" void game_state_dispose_compressed_buffer(s_game_state_compressed_buffer *buffer);
extern "C" void game_state_unlock_and_resume_render_thread(long arg);
extern "C" void game_state_lock_and_halt_render_thread(int lock_mode);
extern "C" void game_state_procs_dispose_active_entries(void);
extern "C" void *game_state_procs_try_and_get_slot(long index, long allocation_parameter, long allocation_size, c_game_state_proc_listener *listener);
extern "C" void cache_files_windows_update_copy_buffer(long arg);
extern "C" void game_state_dispose_shared_lock(void);
extern "C" void game_state_dispose_stream_state(void);

enum e_map_memory_configuration;

extern "C" void game_state_procs_initialize(void);
extern "C" void game_state_procs_set_mode_handler(e_map_memory_configuration configuration);
extern "C" void game_state_procs_reset_current_handler(void);
extern "C" void game_state_procs_set_autosave_listener(void);
extern "C" void game_state_procs_reset_autosave_listener(void);

#endif // SAVED_GAMES_GAME_STATE_PROCS_H
