#ifndef PHYSICS_HAVOK_COLLISION_DAMAGE_H
#define PHYSICS_HAVOK_COLLISION_DAMAGE_H

#include "objects/object_datum.h"
#include "tag_files/tag_groups.h"
#include "units/vehicle_definitions.h"
#include "units/vehicle_types.h"

// Functions defined in tu/blamlibxbox/havok_collision_damage.cpp.

extern "C" void havok_collision_damage_update_object(long object_index, long havok_component_index);
extern "C" bool havok_collision_damage_can_damage_objects(long object_index1, long object_index2);
extern "C" bool havok_collision_damage_test_biped_state(long object_index);

// Datum-based spelling of vehicle_get_type (the index-based one lives in
// tu/objectslibxbox/vehicles.cpp) for callers that already hold the datum.
static __forceinline e_vehicle_type vehicle_get_type(const object_datum *vehicle)
{
    const vehicle_definition *definition = (const vehicle_definition *)tag_get(0, vehicle->definition_index);
    long result = _vehicle_type_none;
    for (long type = 0; type < k_vehicle_type_count; type++) {
        if (definition->types[type].count != 0) {
            result = type;
            break;
        }
    }
    return (e_vehicle_type)result;
}

#endif // PHYSICS_HAVOK_COLLISION_DAMAGE_H
