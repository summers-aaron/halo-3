#ifndef GAME_MULTIPLAYER_DEFINITIONS_H
#define GAME_MULTIPLAYER_DEFINITIONS_H

// Functions defined in tu/blamlibxbox/multiplayer_definitions.cpp that other units call.

struct s_multiplayer_weapon_set;

long multiplayer_universal_data_get_weapon_selection_definition_index(long key);
long multiplayer_universal_data_get_random_weapon_definition_index(void);
struct s_multiplayer_weapon_set const * multiplayer_universal_data_weapon_set_try_and_get(short index);

struct s_element;
struct s_multiplayer_universal_globals_definition;

extern "C" s_element *multiplayer_universal_data_vehicle_set_try_and_get(short index);
extern "C" s_multiplayer_universal_globals_definition *scenario_multiplayer_globals_try_and_get_universal_data(void);
extern "C" int multiplayer_universal_data_get_vehicle_selection_definition_index(int id);
extern "C" short multiplayer_universal_data_get_weapon_set_absolute_index_from_string_id(int string_id);
extern "C" short multiplayer_universal_data_get_vehicle_set_absolute_index_from_string_id(int id);

#endif // GAME_MULTIPLAYER_DEFINITIONS_H
