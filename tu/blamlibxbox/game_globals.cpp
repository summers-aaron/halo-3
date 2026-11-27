// blamlibxbox/game_globals.obj: from the records 0xc3ff0 0xca4e0 0x119ea0 0x21a048 0x21a0c0
// @flags /TP /O2
#include "memory/tls_globals.h"
#include "memory/data_types.h"
#include "game/game_types.h"
#include "game/game_globals_storage.h"
#include "game/players.h"
#include "game/game_globals.h"
#include "tag_files/tag_groups.h"
#include "game/game_allegiance.h"
#include "game/player_types.h"
#include "memory/datum_index.h"
#include "game/game_state_accessors.h"
// @address 0x21a048 ?game_difficulty_get_value_by_difficulty@@YAMFF@Z

#include "scenario/scenario.h"



float game_difficulty_get_value_by_difficulty(short value_type, short difficulty)
{
    float result = 1.0f;
    s_game_globals *globals = global_game_globals;
    if (globals) {
        float *values = globals->difficulty.count ?
            (float *)tag_block_get_element_with_size(&globals->difficulty, 0, sizeof(float[16])) : 0;
        if (values) {
            difficulty = (difficulty > 0 ? difficulty : 0) < 3 ? (difficulty > 0 ? difficulty : 0) : 3;
            result = values[value_type * 4 + difficulty];
        }
    }
    return result;
}

static __forceinline s_grenade_definition *grenade_block_get_element(long count,
    s_grenade_definition *grenades, long index)
{
    return count ? &grenades[index] : 0;
}

long game_get_grenade_type_index_from_item_defintion(long item_definition_index)
{
    long grenade_type_index = -1;
    s_game_globals *globals = global_game_globals;
    for (long index = 0; index < globals->grenades.s_tag_block::count; index++) {
        if (grenade_block_get_element(globals->grenades.s_tag_block::count, (s_grenade_definition *)globals->grenades.address, index)
                ->equipment.index == item_definition_index)
        {
            grenade_type_index = index;
            break;
        }
    }
    return grenade_type_index;
}



// @rdata 0x820359c0..0x820359e8
const short k_game_difficulty_friendly_remap[] =
{
    4, 5, 6, 7,
    -1, -1, -1, -1, -1, -1, -1, -1,
    -1, -1, -1, -1, -1, -1, -1, -1,
};
// @rdata-end

// @address 0xc3ff0 ?game_difficulty_get_value@@YAMF@Z
float game_difficulty_get_value(short value_type)
{
    int difficulty;
    if (tls_game_globals()->options.game_mode == 1)
        difficulty = tls_game_globals()->options.campaign_difficulty;
    else
        difficulty = 1;
    return game_difficulty_get_value_by_difficulty(value_type, (short)difficulty);
}
// @address 0xca4e0 ?game_difficulty_get_team_value@@YAMFW4e_game_team@@@Z
float game_difficulty_get_team_value(short value_type, enum e_game_team team)
{
  short difficulty = tls_game_globals()->options.game_mode == 1 ? tls_game_globals()->options.campaign_difficulty : 1;
  if (tls_game_globals()->options.game_mode == 2)
  {
    difficulty = 1;
  }
  else if (tls_game_globals()->options.game_mode == 1 && !(unsigned char)game_team_is_enemy(_game_team_player, team))
  {
    if (k_game_difficulty_friendly_remap[value_type] == -1)
      difficulty = 1;
    else
      value_type = k_game_difficulty_friendly_remap[value_type];
  }
  return game_difficulty_get_value_by_difficulty(value_type, difficulty);
}

// @address 0x119ea0 game_get_player_character_type
extern "C" int game_get_player_character_type(long player_index)
{
    game_globals_storage *globals = (game_globals_storage *)tls_game_globals();
    player_datum *player = &((player_datum *)tls_player_data()->data)[datum_index_to_absolute_index(player_index)];
    long state = player->configuration.client.appearance.player_model_choice;
    if (game_in_progress(globals) && globals->options.game_mode == 1) {
        switch (player->campaign_player_slot) {
        case 0: return 0;
        case 1: return 1;
        default: return 4;
        }
    }
    if (game_engine_is_player_in_edit_mode(player_index, 0, 0))
        return 5;
    int result = 2;
    switch (state) {
    case 0: result = 2; break;
    case 1: result = 3; break;
    }
    return result;
}
