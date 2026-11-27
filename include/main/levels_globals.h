#ifndef MAIN_LEVELS_GLOBALS_H
#define MAIN_LEVELS_GLOBALS_H

#include "memory/data_types.h"
#include "multithreading/synchronized_value.h"
#include "main/levels_map_datum_types.h"
#include "tag_files/file_enumeration_types.h"

struct s_datum : levels_map_datum {};

struct s_levels_globals
{
    s_levels_globals()
        : default_map(),
          enumeration_task_id(-1),
          enumeration_file_data(),
          enumeration_complete(0),
          enumerate_info_files(1),
          enumerate_dlc_files(1),
          enabled(0),
          pad703(0)
    {
    }
    s_data_array *campaign_levels;
    s_data_array *map_levels;
    s_data_array *campaign_insertions;
    s_data_array *multiplayer_levels;
    levels_map_datum default_map;
    long enumeration_task_id;
    s_find_file_data enumeration_file_data;
    c_synchronized_long enumeration_complete;
    unsigned char enumerate_info_files;
    unsigned char enumerate_dlc_files;
    unsigned char enabled;
    unsigned char pad703;
};

typedef char s_levels_default_map_offset[(sizeof(void *) != 4 || ((offsetof(s_levels_globals, default_map) == 0x10))) ? 1 : -1];
typedef char s_levels_default_map_flags_offset[(offsetof(levels_map_datum, flags) == 2) ? 1 : -1];
typedef char s_levels_default_map_id_offset[(offsetof(levels_map_datum, unique_id) == 4) ? 1 : -1];
typedef char s_levels_task_id_offset[(sizeof(void *) != 4 || ((offsetof(s_levels_globals, enumeration_task_id) == 0x370))) ? 1 : -1];
typedef char s_levels_enumeration_file_data_offset[(sizeof(void *) != 4 || ((offsetof(s_levels_globals, enumeration_file_data) == 0x374))) ? 1 : -1];
typedef char s_levels_completion_offset[(sizeof(void *) != 4 || ((offsetof(s_levels_globals, enumeration_complete) == 0x6fc))) ? 1 : -1];
typedef char s_levels_info_request_offset[(sizeof(void *) != 4 || ((offsetof(s_levels_globals, enumerate_info_files) == 0x700))) ? 1 : -1];

typedef char s_levels_globals_size[(sizeof(void *) != 4 || ((sizeof(s_levels_globals) == 0x704))) ? 1 : -1];

extern s_levels_globals g_level_globals;

#endif
