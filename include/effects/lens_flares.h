#ifndef EFFECTS_LENS_FLARES_H
#define EFFECTS_LENS_FLARES_H

#include "effects/lens_flare_attachment_types.h"

union real_point3d;
union real_vector3d;
union real_rgb_color;

extern "C" long lens_flares_create_internal(long lens_flare_index, e_lens_flare_attachment_types attachment_type, long attachment_index,
    unsigned short marker_index, const real_point3d *position, const real_vector3d *forward, const real_rgb_color *color, float age,
    float scale, float intensity, bool first_person, long player_index);

#endif // EFFECTS_LENS_FLARES_H
