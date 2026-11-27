#ifndef MEMORY_GAME_STATE_GLOBALS_H
#define MEMORY_GAME_STATE_GLOBALS_H

#include <stddef.h>
#include "saved_games/game_state_header_types.h"
#include "saved_games/game_state_compressed_buffer_types.h"

struct s_game_state_globals_storage
{
    __forceinline s_game_state_globals_storage()
    {
        compressed.initialize();
    }
    __forceinline ~s_game_state_globals_storage() {}
    bool shell_initialized;          // game_state_globals.initialized in the March asserts
    bool initialized;
    bool active;
    bool pending;
    long size;
    long index;
    s_game_state_header_layout headers[2];
    unsigned long last_time;
    short count;
    // Address of the game-state header.
#ifdef HALO3_64BIT
    union { char *game_state_buffer; size_t base_address; };
#else
    union { char *game_state_buffer; unsigned long base_address; };
#endif
    long : 32;                       // no March 2007 code reads these bytes
    long : 32;
    long guard_page_size;            // game_state_allocate_buffer's out_guard_page_size

    union
    {
        unsigned long allocation_checksum;   // the allocation callbacks' checksum
        long header_size;                    // the size game_state_initialize_for_new_map stores in the header
    };
    union
    {
        long current_index;
        long revert_time;                // the game time game_state_set_revert_time stores
    };
    char *current_state;
    bool loaded_from_persistent_storage; // game_state_is_blocking_due_to_shitty_hard_drive reads it in March
    union
    {
        s_game_state_compressed_buffer compressed;
        struct
        {
            bool simulation_started;
            bool simulation_start_pending;
            void *heap_buffer;
            long heap_buffer_size;
            void *compressed_buffer;
            long compressed_buffer_size;
            long compressed_size;
            long catchup_state;
        };
    };
};

typedef s_game_state_globals_storage s_game_state_globals;

extern s_game_state_globals game_state_globals;

typedef char game_state_globals_compressed_offset[(sizeof(void *) != 4 || offsetof(s_game_state_globals, compressed) == 0x1f368) ? 1 : -1];

#if defined(_M_PPCBE) || defined(_XBOX)
typedef char game_state_globals_header_offset[(offsetof(s_game_state_globals, base_address) == 0x1f348) ? 1 : -1];
typedef char game_state_globals_headers_offset[
    offsetof(s_game_state_globals, headers) == 0x10 ? 1 : -1];
typedef char game_state_globals_size_check[(sizeof(void *) != 4 || (sizeof(s_game_state_globals) == 0x1f388)) ? 1 : -1];
#endif

#endif
