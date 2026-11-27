#ifndef CACHE_SOUND_CACHE_HASH_SOURCE_H
#define CACHE_SOUND_CACHE_HASH_SOURCE_H

#include "memory/basic_buffer.h"

// Decoder that turns a completed sound cache read buffer into hashable blocks.
class c_sound_cache_hash_source {
public:
#ifdef HALO3_64BIT
    virtual bool open(c_basic_buffer<void> source_position) = 0;
#else
    virtual bool open(unsigned __int64 source_position) = 0;
#endif
    virtual bool read(c_basic_buffer<void> request, c_basic_buffer<void> *result) = 0;
    virtual bool finish(c_basic_buffer<void> *result) = 0;
};

#endif
