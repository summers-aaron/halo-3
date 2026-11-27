#ifndef SAVED_GAMES_GAME_STATE_MEMORY_MAPPING_TYPES_H
#define SAVED_GAMES_GAME_STATE_MEMORY_MAPPING_TYPES_H

#include <stddef.h>
#include "multithreading/synchronized_value.h"

struct s_game_state_memory_mapping {
    unsigned char allocated;
    void *address;
    unsigned long size;
    char *primary_buffer_address;
    unsigned long primary_buffer_capacity;
    unsigned long save_buffer_size;
    long storage_mode;
    void *memory_save_address;
    unsigned long memory_save_size;
    bool memory_save_valid;
    short save_slot;
    unsigned char slot_valid[0x2c - 0x28];
    void *file_handles[2];
    long async_save_state;
    void *async_save_address;
    long async_task_id;
    c_synchronized_long hash_done;
    c_synchronized_long write_done;
    c_synchronized_long bytes_written;
    void *load_buffer_address;
    __forceinline s_game_state_memory_mapping() {}
    __forceinline ~s_game_state_memory_mapping() {}
};

typedef char game_state_memory_mapping_file_handles_offset[(sizeof(void *) != 4 || ((offsetof(s_game_state_memory_mapping, file_handles) == 0x2c))) ? 1 : -1];
typedef char game_state_memory_mapping_slot_valid_offset[(sizeof(void *) != 4 || ((offsetof(s_game_state_memory_mapping, slot_valid) == 0x28))) ? 1 : -1];
typedef char game_state_memory_mapping_size[(sizeof(void *) != 4 || ((sizeof(s_game_state_memory_mapping) == 0x50))) ? 1 : -1];

#endif
