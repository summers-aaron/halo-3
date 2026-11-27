#ifndef EFFECTS_LIGHT_VOLUME_LOCATION_H
#define EFFECTS_LIGHT_VOLUME_LOCATION_H

#include "effects/light_volume_types.h"
#include "effects/light_volumes_globals.h"
#include "memory/data.h"
#include "memory/data_types.h"
#include "math/real_math_types.h"

#include "effects/light_volume_location_types.h"

__forceinline c_light_volume_location *c_light_volume_location::get(long index)
    {
        s_data_array *locations = (s_data_array *)g_light_volume_location_data;
        return datum_get<c_light_volume_location>(locations, index);
    }

__forceinline const s_light_volume_definition *c_light_volume::get_definition() const
{
    c_light_volume_location *location = c_light_volume_location::get(m_location_index);
    c_light_volume_system *system = c_light_volume_system::get(location->m_system_index);
    s_light_volume_system_definition *block = TAG_GET('ltvl', s_light_volume_system_definition, system->m_definition_index);
    return block->get_light_volume_definition(m_definition_index);
}

#endif
