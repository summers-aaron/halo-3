#ifndef RASTERIZER_RASTERIZER_MEMORY_TYPES_H
#define RASTERIZER_RASTERIZER_MEMORY_TYPES_H

#include <stddef.h>

struct rasterizer_main_memory_globals {
    bool initialized;
    char initialized_padding[7];
    long current_offset;
    char buffer[0x29000];
    long high_watermark;
    long maximum_offset;
    bool allocation_failed;
    long allocation_count;
    long allocation_bytes;
    long flags;
    char flags_padding[0x1c];
    float GPU_frontend_remaining;
};

typedef char rasterizer_memory_current_offset[(offsetof(rasterizer_main_memory_globals, current_offset) == 8) ? 1 : -1];
typedef char rasterizer_memory_maximum_offset[(offsetof(rasterizer_main_memory_globals, maximum_offset) == 0x29010) ? 1 : -1];
typedef char rasterizer_memory_allocation_bytes_offset[(offsetof(rasterizer_main_memory_globals, allocation_bytes) == 0x2901c) ? 1 : -1];
typedef char rasterizer_memory_GPU_frontend_remaining_offset[(offsetof(rasterizer_main_memory_globals, GPU_frontend_remaining) == 0x29040) ? 1 : -1];

typedef char rasterizer_memory_size[(sizeof(rasterizer_main_memory_globals) == 0x29044) ? 1 : -1];

#endif
