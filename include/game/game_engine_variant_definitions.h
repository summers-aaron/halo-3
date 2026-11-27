#ifndef GAME_GAME_ENGINE_VARIANT_DEFINITIONS_H
#define GAME_GAME_ENGINE_VARIANT_DEFINITIONS_H

#include "cseries/cseries_wrapped.h"
#include "tag_files/tag_types.h"

struct s_game_engine_social_options_definition {
    c_flags_no_init<long, short, 16> flags;
    short chat_setting;
};

struct s_game_engine_miscellaneous_options_definition {
    unsigned long flags;
    unsigned char round_time_limit_minutes;
    unsigned char round_limit;
    unsigned char early_victory_win_count;
    signed char round_reset_setting;
};
struct s_game_engine_respawn_options_definition {
    unsigned short flags;
    unsigned char lives_per_round;
    unsigned char team_lives_per_round;
    unsigned char respawn_time_seconds;
    unsigned char suicide_penalty_seconds;
    unsigned char betrayal_penalty_seconds;
    unsigned char respawn_growth_seconds;
    long respawn_player_traits;
    unsigned char respawn_player_traits_duration_seconds;
};


struct s_game_engine_map_override_options_definition {
    long flags;
    long base_player_traits;
    long weapon_set;
    long vehicle_set;
    long red_powerup_traits;
    long blue_powerup_traits;
    long yellow_powerup_traits;
    unsigned char red_powerup_duration_seconds;
    unsigned char blue_powerup_duration_seconds;
    unsigned char yellow_powerup_duration_seconds;
};

struct s_game_engine_base_variant_definition {
    long name;
    long description;
    c_typed_tag_block<s_game_engine_miscellaneous_options_definition> miscellaneous_options;
    c_typed_tag_block<s_game_engine_respawn_options_definition> respawn_options;
    c_typed_tag_block<long> social_options;
    c_typed_tag_block<s_game_engine_map_override_options_definition> map_override_options;
};

#if defined(_M_PPCBE) || defined(_XBOX)
typedef char game_engine_social_definition_size[(sizeof(s_game_engine_social_options_definition) == 4) ? 1 : -1];
typedef char game_engine_miscellaneous_definition_size[(sizeof(s_game_engine_miscellaneous_options_definition) == 8) ? 1 : -1];
typedef char game_engine_respawn_definition_size[(sizeof(s_game_engine_respawn_options_definition) == 0x10) ? 1 : -1];
typedef char game_engine_map_override_definition_size[(sizeof(s_game_engine_map_override_options_definition) == 0x20) ? 1 : -1];
typedef char game_engine_base_variant_definition_size[(sizeof(s_game_engine_base_variant_definition) == 0x38) ? 1 : -1];
#endif

#endif // GAME_GAME_ENGINE_VARIANT_DEFINITIONS_H
