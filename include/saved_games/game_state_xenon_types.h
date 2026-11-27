#ifndef SAVED_GAMES_GAME_STATE_XENON_TYPES_H
#define SAVED_GAMES_GAME_STATE_XENON_TYPES_H

#include "cache/cache_file_header.h"
#include "cseries/cseries.h"
#include "saved_games/game_state_xenon_globals.h"

struct s_xenon_game_state_file
{
    s_xenon_game_state_file()
    {
        header.source_path[0] = false;
        header.build[0] = 0;
        header.name[0] = 0;
        header.scenario_path[0] = 0;
        header.copyright[0] = 0;
        header.checksums.game_state_xenon_checksum_zero();
    }
    void *file_handle;
    union
    {
        char buffer[0x3000];
        s_cache_file_header header;
    };
    long io_file_index;
    void *overlapped_handle;
};

struct s_failed_map { c_static_string<256> name; long timestamp; long failure_count; };

struct s_xenon_game_state_globals_base
{
    s_xenon_game_state_globals_base(s_xenon_game_state_globals *self)
    {
        // match: 0x1f3318: singleton global constructor specialization
        __assume(self == &xenon_game_state_globals);
    }
};

struct s_xenon_game_state_globals : s_xenon_game_state_globals_base
{
    s_xenon_game_state_globals();
    s_xenon_game_state_file files[15];
    long active_file_index;
    long pending_file_index;
    long cached_map_file_index;
    long cached_map_level;
    s_failed_map failed_maps[8];
};

typedef char xenon_game_state_file_size_check[(sizeof(void *) != 4 || ((sizeof(s_xenon_game_state_file) == 0x3010))) ? 1 : -1];
typedef char xenon_game_state_file_header_offset_check[(offsetof(s_xenon_game_state_file, header) == 8) ? 1 : -1];
typedef char xenon_game_state_file_io_index_offset_check[(offsetof(s_xenon_game_state_file, io_file_index) == 0x3008) ? 1 : -1];

#endif
