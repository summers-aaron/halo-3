#ifndef GAME_HOPPER_SKILL_MATCH_STATE_TYPES_H
#define GAME_HOPPER_SKILL_MATCH_STATE_TYPES_H

#include "game/multiplayer_game_hopper_download_storage.h"

// The current hopper's configuration entry (c_hopper_configuration::m_configuration). Retire this
// name once its last user reads multiplayer_game_hoppers_get_current_hopper_configuration().
typedef s_hopper_configuration_table_entry s_hopper_skill_match_state;

#include "game/multiplayer_game_hopper.h"

#endif // GAME_HOPPER_SKILL_MATCH_STATE_TYPES_H
