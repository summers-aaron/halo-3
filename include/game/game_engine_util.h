#ifndef GAME_GAME_ENGINE_UTIL_H
#define GAME_GAME_ENGINE_UTIL_H

#include "game/weapon_set_types.h"

// Functions defined in tu/blamlibxbox/game_engine_util.cpp.

class c_game_engine;
struct s_game_engine_event_data;
struct s_player_appearance;
union real_rgb_color;
enum e_game_team;
template<long k_maximum_count> class c_static_wchar_string;

void game_engine_get_universal_string(long string_index, c_static_wchar_string<256> *string);
void game_engine_get_change_colors(long player_index, const s_player_appearance *appearance, e_game_team team, real_rgb_color *colors, bool *valid);

const c_game_engine *current_game_engine(void);
void build_multiplayer_string(long player_index, wchar_t const * string,
    struct s_game_engine_event_data const * event_data, long buffer_size, wchar_t * buffer);
#include "game/game_engine_events.h"
short game_engine_get_multiplayer_weapon_selection_absolute_index(long name);
long game_engine_get_multiplayer_weapon_selection_name(short absolute_index);
long game_engine_get_game_object_definition_index(long type);
long game_engine_get_carried_multiplayer_weapon_index(long player_index, long weapon_type);
long player_get_composite_score(long player_index);
long game_engine_weapon_item_definition_index_from_absolute_weapons_selection_block_index(short absolute_index, e_weapon_set weapon_set);
long parse_multiplayer_string_token(long player_index, const wchar_t *token, long token_length,
    const s_game_engine_event_data *event, wchar_t *buffer, long buffer_size);

struct s_lookup { int key; int value; int bit; };
extern "C" s_lookup g_game_engine_time_message_lookup[];
extern "C" unsigned short game_engine_get_active_team_mask(void);
#endif // GAME_GAME_ENGINE_UTIL_H
