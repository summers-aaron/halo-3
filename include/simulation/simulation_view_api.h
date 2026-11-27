#ifndef SIMULATION_SIMULATION_VIEW_API_H
#define SIMULATION_SIMULATION_VIEW_API_H

struct s_game_state_compressed_buffer;

extern "C" bool game_state_compress_current_state(
    s_game_state_compressed_buffer *context, long unused, unsigned long flags, void **buffer_out, unsigned long *size_out);
extern "C" long g_simulation_catchup_threshold;

#endif // SIMULATION_SIMULATION_VIEW_API_H
