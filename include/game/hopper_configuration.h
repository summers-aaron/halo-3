#ifndef GAME_HOPPER_CONFIGURATION_H
#define GAME_HOPPER_CONFIGURATION_H

#include "game/multiplayer_game_hopper_download_storage.h"

// c_hopper_configuration is defined in multiplayer_game_hopper_download_storage.h,
// next to the table entry it wraps; the table holds configurations directly.

typedef char hopper_configuration_type_offset[
    (offsetof(s_hopper_configuration_table_entry, hopper_type) == 0x3c) ? 1 : -1];
typedef char hopper_configuration_image_index_offset[
    (offsetof(s_hopper_configuration_table_entry, image_index) == 0x44) ? 1 : -1];
typedef char hopper_configuration_matchmaking_seconds_offset[
    (offsetof(s_hopper_configuration_table_entry, maximum_total_matchmaking_seconds) == 0xc4) ? 1 : -1];
typedef char hopper_configuration_skill_match_delta_offset[
    (offsetof(s_hopper_configuration_table_entry, maximum_skill_level_match_delta) == 0x1ac) ? 1 : -1];

#endif // GAME_HOPPER_CONFIGURATION_H
