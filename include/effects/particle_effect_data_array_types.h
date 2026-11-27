#ifndef EFFECTS_PARTICLE_EFFECT_DATA_ARRAY_TYPES_H
#define EFFECTS_PARTICLE_EFFECT_DATA_ARRAY_TYPES_H

#include "memory/data_types.h"

#include "effects/effect_datum.h"


#include "effects/effect_data.h"

static __forceinline s_data_array *tls_particle_effect_data_array()
{
    return effect_data;
}


#endif // EFFECTS_PARTICLE_EFFECT_DATA_ARRAY_TYPES_H
