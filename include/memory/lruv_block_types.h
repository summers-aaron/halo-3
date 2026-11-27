#ifndef MEMORY_LRUV_BLOCK_TYPES_H
#define MEMORY_LRUV_BLOCK_TYPES_H

#include "memory/data_types.h"

struct s_lruv_block
{
    unsigned short identifier;
    unsigned short flags;
    long page_count;
    long page_index;
    long next_block_index;
    long previous_block_index;
    long last_used_frame;
};

#endif // MEMORY_LRUV_BLOCK_TYPES_H
