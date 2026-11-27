#ifndef SCENARIO_SCENARIO_MAP_VARIANT_UTILITIES_H
#define SCENARIO_SCENARIO_MAP_VARIANT_UTILITIES_H

// Functions defined in tu/blamlibxbox/scenario_map_variant_utilities.cpp that other units call.

long map_variant_value_pair_get_value_index(long property_name, long search_value);
long map_variant_value_pair_get_value_index(long property_name, float search_value);
float map_variant_value_pair_get_real(long property_name, long value_index);
long map_variant_value_pair_get_integer(long property_name, long value_index);

#endif // SCENARIO_SCENARIO_MAP_VARIANT_UTILITIES_H
