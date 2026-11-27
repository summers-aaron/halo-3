#ifndef AI_PATHFINDING_OBJECT_REFERENCE_H
#define AI_PATHFINDING_OBJECT_REFERENCE_H
#include <stddef.h>
#include "tag_files/tag_types.h"

struct pathfinding_object_bsp_reference
{
    unsigned long bsp_index;
    short node_index;
    short unused;
    s_tag_block nodes;
    long vertex_offset;
};
struct pathfinding_object_reference
{
    unsigned short flags;
    short unused;
    long count;
    TAG_POINTER(pathfinding_object_bsp_reference) entries;
    long bsp_block_definition;
    long object_unique_id;
    short origin_bsp_index;
    unsigned char object_type;
    unsigned char source;
};
typedef char pathfinding_object_reference_size[(sizeof(pathfinding_object_reference) == 0x18) ? 1 : -1];
typedef char pathfinding_object_bsp_reference_size[(sizeof(pathfinding_object_bsp_reference) == 0x18) ? 1 : -1];
#endif
