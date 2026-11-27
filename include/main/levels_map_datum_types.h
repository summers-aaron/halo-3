#ifndef MAIN_LEVELS_MAP_DATUM_TYPES_H
#define MAIN_LEVELS_MAP_DATUM_TYPES_H

#include "memory/data_types.h"
#include <stddef.h>

struct levels_map_datum
{
    short identifier;
    unsigned short flags;
    long unique_id;
    // levels.cpp copies 32 name characters and 128 description characters.
    wchar_t display_name[32];
    wchar_t description[128];
    char name[256];
    char image_file_base[256];
    unsigned long presence_context_id;
    unsigned long sort_order;
    unsigned char mp_minimum_desired_players;
    unsigned char mp_maximum_desired_players;
    signed char maximum_teams[11];
};

typedef char levels_map_datum_size[(sizeof(levels_map_datum) == 0x360) ? 1 : -1];
typedef char levels_map_datum_description_offset[(offsetof(levels_map_datum, description) == 0x48) ? 1 : -1];
typedef char levels_map_datum_name_offset[(offsetof(levels_map_datum, name) == 0x148) ? 1 : -1];

typedef char levels_map_maximum_teams_offset[(offsetof(levels_map_datum, maximum_teams) == 0x352) ? 1 : -1];

#endif // MAIN_LEVELS_MAP_DATUM_TYPES_H
