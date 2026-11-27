#ifndef SIMULATION_GAME_INTERFACE_SIMULATION_GAME_OBJECT_CREATION_INLINE_H
#define SIMULATION_GAME_INTERFACE_SIMULATION_GAME_OBJECT_CREATION_INLINE_H

#include "simulation/game_interface/simulation_game_object_entity_definition.h"
#include "simulation/game_interface/simulation_game_generic_data_types.h"
#include "objects/object_header.h"

__forceinline void c_simulation_object_entity_definition::object_build_creation_data(
    long object_index, s_simulation_object_creation_data *data)
{
    object_datum *object = (object_datum *)object_get_and_verify_type(object_index, ~0);
    data->definition_index = object->definition_index;
    data->scenario_datum_index = object->scenario_datum_index;
    data->map_variant_index = object->map_variant_index;
    data->allow_duplicate_scenario_objects = (object->flags_d6 >> 2) & 1;
    data->child_variant_index = object->marker;
}

#endif
