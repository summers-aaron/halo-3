#ifndef GAME_GAME_ENGINE_PLAYER_TRAITS_H
#define GAME_GAME_ENGINE_PLAYER_TRAITS_H

#include <stddef.h>

#include "tag_files/tag_types.h"

struct s_game_engine_player_traits
{
    long name;
    s_tag_block shield_vitality;
    s_tag_block weapons;
    s_tag_block movement;
    s_tag_block appearance;
    s_tag_block sensors;
};

typedef char game_engine_player_traits_size[(sizeof(s_game_engine_player_traits) == 0x40) ? 1 : -1];
typedef char game_engine_player_traits_off_shield[(offsetof(s_game_engine_player_traits, shield_vitality) == 4) ? 1 : -1];
typedef char game_engine_player_traits_off_sensors[(offsetof(s_game_engine_player_traits, sensors) == 0x34) ? 1 : -1];

// Functions defined in tu/blamlibxbox/game_engine_player_traits.cpp.

void game_engine_assemble_player_traits(long player_index);

class c_bitstream;
class c_player_trait_weapons;

extern "C" void player_trait_weapons_decode(c_player_trait_weapons *traits, c_bitstream *packet);

class c_player_trait_appearance;

void game_engine_apply_appearance_traits(long player_index, const c_player_trait_appearance *traits);

extern "C" const float k_player_traits_good_camo_maximum;

class c_player_traits;
enum e_multiplayer_powerup_flavor;
void game_get_powerup_traits(e_multiplayer_powerup_flavor flavor, c_player_traits *powerup_traits);

extern "C" {
extern const float k_player_traits_quarter_speed;
}

#endif // GAME_GAME_ENGINE_PLAYER_TRAITS_H
