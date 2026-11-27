#ifndef MEMORY_LRUV_BLOCK_HEADER_TYPES_H
#define MEMORY_LRUV_BLOCK_HEADER_TYPES_H

#include "memory/data_types.h"

struct s_lruv_block_header { s_data_array *block_data; unsigned long signature; };

#endif // MEMORY_LRUV_BLOCK_HEADER_TYPES_H
