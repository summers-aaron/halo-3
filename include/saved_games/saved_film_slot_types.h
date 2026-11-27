#ifndef SAVED_GAMES_SAVED_FILM_SLOT_TYPES_H
#define SAVED_GAMES_SAVED_FILM_SLOT_TYPES_H
#include "cseries/async_double_buffer.h"
struct s_saved_film_async_state
{
    s_saved_film_async_state();

    long state;
    long count;
    void *memory;
    long memory_size;
    c_async_double_buffer async_buffer;
};

typedef char saved_film_async_state_size[(sizeof(void *) != 4 || ((sizeof(s_saved_film_async_state) == 0x98))) ? 1 : -1];

struct s_saved_film_slot_globals
{
    __forceinline s_saved_film_slot_globals()
    {
        unsigned long count = 2;
        long index = 0;
        do
        {
            --count;
            streams[index].state = 0;
            streams[index].count = 0;
            streams[index].memory = 0;
            streams[index].memory_size = 0;
            ++index;
        } while (count != 0);
    }
    ~s_saved_film_slot_globals() {}

    bool initialized;
    void *memory_blocks[1];
    long memory_size;
    s_saved_film_async_state streams[2];
};


typedef char saved_film_slot_globals_size[(sizeof(void *) != 4 || sizeof(s_saved_film_slot_globals) == 0x13c) ? 1 : -1];
extern "C" s_saved_film_slot_globals g_saved_film_slots;
#endif
