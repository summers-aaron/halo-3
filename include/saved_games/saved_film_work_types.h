#ifndef SAVED_GAMES_SAVED_FILM_WORK_TYPES_H
#define SAVED_GAMES_SAVED_FILM_WORK_TYPES_H

#include <stddef.h>

struct s_saved_film_work_entry
{
    union {
        long words[6];                          // words 0, 2 and 3 start at -1, the rest at 0
        struct {
            long pad0;
            long value;
            long buffer_offset;
            long padc;
            unsigned long checksum;
            char flags[3];
            unsigned char active;
        };
    };
};

typedef char saved_film_work_entry_size[(sizeof(s_saved_film_work_entry) == 0x18) ? 1 : -1];
typedef char saved_film_work_entry_value_offset[(offsetof(s_saved_film_work_entry, value) == 4) ? 1 : -1];
typedef char saved_film_work_entry_flags_offset[(offsetof(s_saved_film_work_entry, flags) == 0x14) ? 1 : -1];
typedef char saved_film_work_entry_active_offset[(offsetof(s_saved_film_work_entry, active) == 0x17) ? 1 : -1];

class c_async_double_buffer;

struct s_saved_film_snippet_marker
{
    long identifier;
    long tick;
    union { long buffer_offset; long reserved0; };
    long reserved1;
    union { unsigned long checksum; long reserved2; };
    bool seekable;
    bool reserved3;
    bool recorded;
    bool valid;
};

struct s_saved_film_work_globals
{
    s_saved_film_work_globals();

    c_async_double_buffer *buffer;
    union {
        s_saved_film_work_entry entries[16];
        s_saved_film_snippet_marker markers[16];
    };
    long playback_length;
    unsigned long tick_start;
    bool recording_pending;
};

extern "C" s_saved_film_work_globals g_saved_film_work_globals;

#endif
