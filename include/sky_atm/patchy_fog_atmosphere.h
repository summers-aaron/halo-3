#ifndef SKY_ATM_PATCHY_FOG_ATMOSPHERE_H
#define SKY_ATM_PATCHY_FOG_ATMOSPHERE_H

#include <stddef.h>
#include "tag_files/tag_types.h"

// The 'skya' tag fields consumed by the patchy-fog renderer.
struct patchy_fog_atmosphere
{
    long flags;
    s_tag_reference bitmap;
    float rotation;
    float sheet_spacing;
    float texture_scale;
};

typedef char patchy_fog_atmosphere_bitmap_offset[(offsetof(patchy_fog_atmosphere, bitmap) == 4) ? 1 : -1];
typedef char patchy_fog_atmosphere_spacing_offset[(offsetof(patchy_fog_atmosphere, sheet_spacing) == 0x18) ? 1 : -1];

#endif
