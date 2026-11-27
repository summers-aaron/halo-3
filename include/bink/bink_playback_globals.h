#ifndef BINK_BINK_PLAYBACK_GLOBALS_H
#define BINK_BINK_PLAYBACK_GLOBALS_H

#include "saved_games/game_state_procs_types.h"

union real_vector4d;

// Game-state proc listener of the bink movie buffer (slot 1, vtable 0x3e298).
class c_bink_game_state_listener : public c_game_state_proc_listener
{
public:
    virtual void set_mode(long mode, s_game_state_buffer_description *description);
    virtual void update(void *);
    virtual void dispose(void *);
};

extern "C" {
extern unsigned long g_bink_last_activity_time;
extern c_bink_game_state_listener g_bink_game_state_listener;
extern long g_bink_idle_movie_index;
extern real_vector4d g_bink_pixel_shader_constants[4];
}

extern "C" {
extern unsigned long g_bink_available_physical_memory;
extern void *g_bink_allocations[16];
extern long g_bink_allocation_count;
}


#endif // BINK_BINK_PLAYBACK_GLOBALS_H
