#ifndef CACHE_CACHE_FILE_BULK_READER_SET_BITS_TYPES_H
#define CACHE_CACHE_FILE_BULK_READER_SET_BITS_TYPES_H

#include "memory/data_types.h"

union s_cache_file_bulk_reader_set_bits {
    unsigned long value;
#if defined(_XBOX) && !defined(__clang__)
    struct {
        unsigned long index : 15;
        unsigned long offset : 15;
        unsigned long flags : 2;
    };
#else // little-endian hosts allocate bit-fields from the low bit: declare them low to high so each field keeps its Xbox bit position
    struct {
        unsigned long flags : 2;
        unsigned long offset : 15;
        unsigned long index : 15;
    };
#endif
};

#endif // CACHE_CACHE_FILE_BULK_READER_SET_BITS_TYPES_H
