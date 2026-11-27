#ifndef AI_SQUAD_GROUP_DATUM_H
#define AI_SQUAD_GROUP_DATUM_H

#include <stddef.h>

// squads_build_hierarchy reads the parent at +0x10 and child links at +4/+0xc.
struct squad_group_datum {          /* 0x24 */
    short identifier, status;
    long next_sibling_index;        /* heads the child list (squads_build_hierarchy) */
    long first_squad_index;
    long first_child_index;         /* links the children of one parent */
    long parent_index;
    bool active;
    bool placed;
    bool alerted;
    union { unsigned short strength; short initial_strength; };
    short active_count;
    short previous_strength;
    union { short reserved_count; unsigned short fighting_count; };
    float weighted_status;
};

typedef char squad_group_datum_size[(sizeof(squad_group_datum) == 0x24) ? 1 : -1];
typedef char squad_group_parent_offset[(offsetof(squad_group_datum, parent_index) == 0x10) ? 1 : -1];
typedef char squad_group_fighting_count_offset[(offsetof(squad_group_datum, fighting_count) == 0x1e) ? 1 : -1];

#endif
