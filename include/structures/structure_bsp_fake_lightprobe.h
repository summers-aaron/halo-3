#ifndef STRUCTURES_STRUCTURE_BSP_FAKE_LIGHTPROBE_H
#define STRUCTURES_STRUCTURE_BSP_FAKE_LIGHTPROBE_H

#include <stddef.h>
#include "objects/object_reference_types.h"

// objects.cpp indexes this serialized block at a 92-byte stride and compares
// the leading identifier. Preserve the unread lighting payload verbatim.
struct s_structure_bsp_fake_lightprobe
{
    c_object_identifier object_identifier;
    char lighting_payload[92 - 8];
};

typedef char structure_bsp_fake_lightprobe_size[(sizeof(s_structure_bsp_fake_lightprobe) == 92) ? 1 : -1];
typedef char structure_bsp_fake_lightprobe_identifier_offset[(offsetof(s_structure_bsp_fake_lightprobe, object_identifier) == 0) ? 1 : -1];

#endif
