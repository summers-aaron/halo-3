#ifndef AI_SQUAD_AREA_ITERATOR_H
#define AI_SQUAD_AREA_ITERATOR_H

#include <stddef.h>

#include "tag_files/tag_types.h"

struct squad_area_iterator {
    short iterator_type;
    short squad_index;
    short area_index;
    short next_index;
    short zone_set_index;
    short zone_index;
    short type_flags;
    short area_type;
    long firing_position;
    bool enabled_only;
    s_tag_block *zone_set;
};

typedef char squad_area_iterator_size[(sizeof(void *) != 4 || sizeof(squad_area_iterator) == 0x1c) ? 1 : -1];
typedef char squad_area_iterator_zone_set_offset[(offsetof(squad_area_iterator, zone_set) == 0x18) ? 1 : -1];

#endif // AI_SQUAD_AREA_ITERATOR_H
