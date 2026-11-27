#ifndef OBJECTS_DAMAGE_INLINE_H
#define OBJECTS_DAMAGE_INLINE_H

#include "objects/damage.h"

static __forceinline void object_cause_damage_simple(s_damage_data *damage_data, long object_index)
{
    object_cause_damage(damage_data, object_index, -1, -1, -1, 0);
}

static __forceinline long damage_reporting_info_get_type(unsigned char reporting_info)
{
    return reporting_info >> 2;
}

#endif // OBJECTS_DAMAGE_INLINE_H
