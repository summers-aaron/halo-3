#ifndef EFFECTS_LIGHT_VOLUME_SYSTEM_CREATION_H
#define EFFECTS_LIGHT_VOLUME_SYSTEM_CREATION_H

#include "tag_files/tag_index.h"
extern "C" long light_volume_system_new(
    unsigned char type, c_tag_index effect_index, long handle, unsigned char pad8, long pad_a, long unused, unsigned char pad_1a);

#endif // EFFECTS_LIGHT_VOLUME_SYSTEM_CREATION_H
