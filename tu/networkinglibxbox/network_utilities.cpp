// networkinglibxbox/network_utilities.obj: from the records 0x39d5f8 0x39d6e0 0x39d880 0x39d920
// @flags /TP /O2
#include "math/real_math_types.h"
#include "game/game_globals.h"
#include "game/player_color_types.h"
#include "math/random_math.h"
#include "math/random_math_range.h"
#include "memory/datum_index.h"
#include "memory/static_arrays.h"
#include "cseries/cseries_wrapped.h"
#include "tag_files/tag_types.h"
#include "game/game_types.h"
// @address 0x39d5f8 network_player_name_is_random
// @address 0x39d6e0 network_get_random_player_name
// @address 0x39d880 ?network_get_team_color@@YA?ATreal_rgb_color@@W4e_game_team@@@Z
// @address 0x39d920 network_get_player_colors
// @address 0x58f528 wcscmp
// @address 0x3ac90 global_real_argb_colors
#include "cseries/cseries_types.h"
#include "text/unicode.h"
#include "tag_files/string_ids.h"
#include "networking/network_utilities.h"
#include "game/players.h"
#include "cache/cache_files.h"
#include "scenario/scenario.h"


#include "cseries/global_real_argb_colors.h"
#include "game/game_engine_settings_definitions.h"
#include "text/string_list_strings.h"
#include "text/wide_string_comparison.h"
#include "text/unicode_constants.h"

// @address 0xa74 k_empty_wide_string
const wchar_t k_empty_wide_string[] = { 0 };

// @address 0x79e200 k_network_compatible_version
const long k_network_compatible_version = 11855;

extern "C" bool network_player_name_is_random(const wchar_t *name)
{
  bool found = false;
  s_game_globals *game = (s_game_globals *)global_game_globals;
  if (game && game->multiplayer_globals_index != -1) {
    const s_multiplayer_globals_definition *globals =
        (const s_multiplayer_globals_definition *)global_tag_instances[datum_index_to_absolute_index(game->multiplayer_globals_index)].definition;
    if (globals->universal.count > 0) {
      const s_multiplayer_universal_globals_definition *universal =
          (const s_multiplayer_universal_globals_definition *)globals->universal.address;
      if (universal->random_player_names.index != -1) {
        for (long index = 0; index < 83; ++index) {
          c_static_wchar_string<256> text;
          string_list_get_normal_string(universal->random_player_names.index, string_id_number(index), &text);
          wchar_t player_name[16];
          ustrnzcpy_inline(player_name, text.get_string(), 16);
          if (wcscmp(player_name, name) == 0) found = true;
        }
      }
    }
  }
  return found;
}

extern "C" bool network_get_random_player_name(wchar_t *name, long size)
{
  bool result = false;
  s_game_globals *game = (s_game_globals *)global_game_globals;
  if (game && game->multiplayer_globals_index != -1) {
    const s_multiplayer_globals_definition *globals =
        (const s_multiplayer_globals_definition *)global_tag_instances[datum_index_to_absolute_index(game->multiplayer_globals_index)].definition;
    if (globals->universal.count > 0) {
      const s_multiplayer_universal_globals_definition *universal =
          (const s_multiplayer_universal_globals_definition *)globals->universal.address;
      if (universal->random_player_names.index != -1) {
        c_static_wchar_string<256> text;
        unsigned long *seed = get_local_random_seed_address();
        random_seed_next(seed);
        short index = (short)((83 * (*seed >> 16)) >> 16);
        string_list_get_normal_string(universal->random_player_names.index, string_id_number(index), &text);
        ustrnzcpy(name, text.get_string(), 16);
        result = true;
      }
    }
  }
  return result;
}

real_rgb_color network_get_team_color(e_game_team team_index)
{
  real_rgb_color result;
  result = global_real_argb_colors[_real_argb_pink].rgb;
  long tag_index = ((const s_game_globals *)global_game_globals)->multiplayer_globals_index;
  if (tag_index != -1) {
    const s_multiplayer_globals_definition *globals =
        (const s_multiplayer_globals_definition *)global_tag_instances[datum_index_to_absolute_index(tag_index)].definition;
    if (globals->universal.count > 0) {
      const s_multiplayer_universal_globals_definition *universal =
          (const s_multiplayer_universal_globals_definition *)globals->universal.address;
      if (team_index >= 0 && team_index < universal->team_colors.count())
        result = universal->team_colors.begin()[team_index];
    }
  }
  return result;
}

static __forceinline real_rgb_color network_utilities_player_color(long index)
{
  real_rgb_color color;
  return *player_profile_get_rgb_color(&color, index);
}

extern "C" void network_get_player_colors(
    // match: 0x39d920: without __restrict on colors the color_indices[1]/[2] lbz+extsb loads sink below the colors[0]/[1] stores
    e_game_team team_index, bool unused, const c_enum<e_player_color_index, char, -1, 32> *color_indices, real_rgb_color *__restrict colors)
{
  colors[0] = network_utilities_player_color(color_indices[0]);
  colors[1] = network_utilities_player_color(color_indices[1]);
  colors[2] = network_utilities_player_color(color_indices[2]);
  colors[3] = network_utilities_player_color(0);
  if (team_index != _game_team_none) {
    colors[0] = network_get_team_color(team_index);
    colors[1] = network_get_team_color(team_index);
  }
}
