#ifndef SAVED_GAMES_GAME_STATE_COMPRESSION_H
#define SAVED_GAMES_GAME_STATE_COMPRESSION_H

struct s_game_state_compressed_buffer;

extern "C" bool game_state_compress_buffer(s_game_state_compressed_buffer *context, unsigned long flags, const void *data,
    unsigned long data_size, void **buffer_out, unsigned long *size_out);

#endif // SAVED_GAMES_GAME_STATE_COMPRESSION_H
