#ifndef SAVED_GAMES_GAME_STATE_COMPRESSED_BUFFER_TYPES_H
#define SAVED_GAMES_GAME_STATE_COMPRESSED_BUFFER_TYPES_H

struct s_game_state_compression_buffer;

struct s_game_state_compressed_buffer
{
    __forceinline void initialize()
    {
        buffer = 0;
        buffer_size = 0;
        compressed_buffer = 0;
        compressed_size = 0;
        extra = 0;
        last = 0;
    }

    unsigned char active;
    unsigned char acquire_requested;
    void *buffer;
    long buffer_size;
    s_game_state_compression_buffer *compressed_buffer;
    long compressed_size;
    long extra;
    long last;
};

typedef char game_state_compressed_buffer_size[(sizeof(void *) != 4 || sizeof(s_game_state_compressed_buffer) == 0x1c) ? 1 : -1];

#endif
