// blamlibxbox/multiplayer_definitions.obj: from the records 0x2e08b8 0x2e0908 0x2e0990 0x2e0b68 0x2e0bf0 0x2e0ca0 0x2e0d18 0x2e0da0
// @flags /TP /O2
// @address 0x2e08b8 scenario_multiplayer_globals_try_and_get_universal_data
// @address 0x2e0908 ?multiplayer_universal_data_get_weapon_selection_definition_index@@YAJJ@Z
// @address 0x2e0990 ?multiplayer_universal_data_get_random_weapon_definition_index@@YAJXZ
// @address 0x2e0b68 multiplayer_universal_data_get_vehicle_selection_definition_index
// @address 0x2e0bf0 multiplayer_universal_data_get_weapon_set_absolute_index_from_string_id
// @address 0x2e0ca0 ?multiplayer_universal_data_weapon_set_try_and_get@@YAPBUs_multiplayer_weapon_set@@F@Z
// @address 0x2e0d18 multiplayer_universal_data_get_vehicle_set_absolute_index_from_string_id
// @address 0x2e0da0 multiplayer_universal_data_vehicle_set_try_and_get
#include "game/multiplayer_definitions.h"
#include "tag_files/tag_types.h"
#include "game/game_engine_settings_definitions.h"
#include "scenario/scenario.h"
#include "tag_files/tag_groups.h"
#include "memory/tls_globals.h"
#include "math/random_math_range.h"
#include "game/universal_types.h"
#include "game/weapon_set_types.h"
#include "game/game_globals.h"

struct s_multiplayer_weapon_set;

static __forceinline s_universal *multiplayer_globals_try_and_get_universal_data(const s_game_globals *game_globals)
{
    if (game_globals) {
        s_multiplayer_globals_definition *data =
            TAG_GET('mulg', s_multiplayer_globals_definition, game_globals->multiplayer_globals_index);
        if (data && data->universal.count > 0)
            return (s_universal *)data->universal.address;
    }
    return 0;
}

s_universal *scenario_multiplayer_globals_try_and_get_universal_data(void)
{
    return multiplayer_globals_try_and_get_universal_data(global_game_globals);
}

long multiplayer_universal_data_get_weapon_selection_definition_index(long key)
{
  int result = -1;
  if (key != -1 && key != 0) {
    s_universal *table = scenario_multiplayer_globals_try_and_get_universal_data();
    if (table != 0) {
      int i = 0;
      if (table->weapon_selection_count > 0) {
        s_multiplayer_weapon_selection *entry = table->weapon_selections;
        do {
          if (entry->name == key) {
            result = entry->weapon.index;
            break;
          }
          i++;
          entry++;
        } while (i < table->weapon_selection_count);
      }
    }
  }
  return result;
}

long multiplayer_universal_data_get_random_weapon_definition_index(void)
{
    int result = -1;
    s_universal *globals = multiplayer_globals_try_and_get_universal_data(global_game_globals);
    float total = 0.0f;
    if (globals) {
        for (int i = 0; i < globals->weapon_selection_count; ++i) {
            s_multiplayer_weapon_selection *weapon = &globals->weapon_selections[i];
            if (weapon->weight > 0.0f)
                total += weapon->weight;
        }
        if (total > 0.0f) {
            float random = real_random_range_from_seed(tls_random_seed(), 0.0f, total);
            total = 0.0f;
            for (int i = 0; i < globals->weapon_selection_count; ++i) {
                s_multiplayer_weapon_selection *weapon = &globals->weapon_selections[i];
                if (weapon->weight > 0.0f) {
                    if (total <= random && random <= total + weapon->weight) {
                        result = weapon->weapon.index;
                        break;
                    }
                    total += weapon->weight;
                }
            }
        }
    }
    return result;
}

int multiplayer_universal_data_get_vehicle_selection_definition_index(int id)
{
  int result = -1;
  if (id != -1 && id != 0) {
    s_universal *table = scenario_multiplayer_globals_try_and_get_universal_data();
    if (table != 0) {
      for (int i = 0; i < table->vehicle_selection_count; i++) {
        s_multiplayer_vehicle_selection *entry = &table->vehicle_selections[i];
        if (entry->name == id) {
          result = entry->vehicle.index;
          break;
        }
      }
    }
  }
  return result;
}

short multiplayer_universal_data_get_weapon_set_absolute_index_from_string_id(int string_id)
{
  short result = -1;
  if (string_id == _multiplayer_weapon_selection_name_random) {
    result = -2;
  } else if (string_id != -1 && string_id != 0) {
    s_universal *universal = scenario_multiplayer_globals_try_and_get_universal_data();
    if (universal) {
      for (int i = 0; i < universal->weapon_set_count; i++) {
        if (universal->weapon_sets[i].string_id == string_id) {
          result = (short)i;
          break;
        }
      }
    }
  }
  return result;
}

struct s_multiplayer_weapon_set const * multiplayer_universal_data_weapon_set_try_and_get(short index)
{
    s_universal *definition = multiplayer_globals_try_and_get_universal_data(global_game_globals);
    if (definition && index >= 0 && index < definition->weapon_set_count)
        return (s_multiplayer_weapon_set const *)&definition->weapon_sets[index];
    return 0;
}

short multiplayer_universal_data_get_vehicle_set_absolute_index_from_string_id(int id)
{
    short result = -1;
    if (id != -1 && id != 0) {
        s_universal *list = scenario_multiplayer_globals_try_and_get_universal_data();
        if (list != 0) {
            for (int i = 0; i < list->vehicle_set_count; i++) {
                if (list->vehicle_sets[i].name == id) {
                    result = (short)i;
                    break;
                }
            }
        }
    }
    return result;
}

extern "C" s_element *multiplayer_universal_data_vehicle_set_try_and_get(short index)
{
    s_universal *definition = multiplayer_globals_try_and_get_universal_data(global_game_globals);
    if (definition && index >= 0 && index < definition->vehicle_set_count)
        return (s_element *)&definition->vehicle_sets[index];
    return 0;
}
