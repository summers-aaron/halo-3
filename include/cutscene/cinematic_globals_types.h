#ifndef CUTSCENE_CINEMATIC_GLOBALS_TYPES_H
#define CUTSCENE_CINEMATIC_GLOBALS_TYPES_H

#include <stddef.h>
#include "cutscene/cinematic_title_types.h"

// The subtitle on screen: its string, its full display time, and the time it has left.
struct s_cinematic_subtitle
{
    long string_id;
    union
    {
        float duration;
        long duration_bits;
    };
    union
    {
        float time_remaining;
        long time_remaining_bits;
    };
};

// Global cinematic playback state: letterbox, titles, subtitles, and skip control.
struct s_cinematic_globals_definition
{
    float letterbox_fraction;
    bool letterbox_closing;
    bool in_progress;
    bool skip_in_progress;
    bool suppress_bsp_object_creation;
    s_cinematic_title titles[4];
    s_cinematic_subtitle subtitle;
    short pad;
    bool old_map_active;
    char pad1;
    bool skip;
    long start_time;
    long skip_end_time;
};

typedef char cinematic_globals_in_progress_offset[(offsetof(s_cinematic_globals_definition, in_progress) == 5) ? 1 : -1];
typedef char cinematic_globals_suppress_offset[(offsetof(s_cinematic_globals_definition, suppress_bsp_object_creation) == 7) ? 1 : -1];
typedef char cinematic_globals_titles_offset[(offsetof(s_cinematic_globals_definition, titles) == 8) ? 1 : -1];
typedef char cinematic_globals_subtitle_offset[(offsetof(s_cinematic_globals_definition, subtitle) == 0x20) ? 1 : -1];
typedef char cinematic_globals_old_map_active_offset[(offsetof(s_cinematic_globals_definition, old_map_active) == 0x2e) ? 1 : -1];
typedef char cinematic_globals_skip_offset[(offsetof(s_cinematic_globals_definition, skip) == 0x30) ? 1 : -1];
typedef char cinematic_globals_start_time_offset[(offsetof(s_cinematic_globals_definition, start_time) == 0x34) ? 1 : -1];
typedef char cinematic_globals_size[(sizeof(s_cinematic_globals_definition) == 0x3c) ? 1 : -1];

// March 2007 TLS slot 0x831df6b8.
extern __declspec(thread) s_cinematic_globals_definition *cinematic_globals;

#endif // CUTSCENE_CINEMATIC_GLOBALS_TYPES_H
