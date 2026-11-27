#ifndef BITMAPS_BITMAP_HARDWARE_RESOURCE_H
#define BITMAPS_BITMAP_HARDWARE_RESOURCE_H

#include "tag_files/tag_types.h"

// Hardware texture resource records used by bitmap_group's resource blocks.
struct s_bitmap_hardware_resource
{
    long handle;
    long pad;
};

typedef char bitmap_hardware_resource_size[(sizeof(s_bitmap_hardware_resource) == 0x8) ? 1 : -1];

struct s_bitmap_hardware_texture_block
{
    TAG_POINTER(void) descriptor;
    TAG_POINTER(char) elements;
};

#endif
