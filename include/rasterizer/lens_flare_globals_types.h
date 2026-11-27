#ifndef RASTERIZER_LENS_FLARE_GLOBALS_TYPES_H
#define RASTERIZER_LENS_FLARE_GLOBALS_TYPES_H

#include "rasterizer/lens_flare_entry_types.h"

struct lens_flare_globals
{
    long entry_count;
    lens_flare_entry entries[256];
    short buckets[256];
    short links[256];
    int frame;
};

#endif // RASTERIZER_LENS_FLARE_GLOBALS_TYPES_H
