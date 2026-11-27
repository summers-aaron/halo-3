#ifndef GAME_GAME_TYPES_H
#define GAME_GAME_TYPES_H

enum e_game_team {
    _game_team_none = -1,
    _game_team_invalid = -1,
    _infection_team_none = -1,
    _game_team_default = 0,
    _game_team_dummy = 0,
    game_team_prop_dialogue_placeholder = 0,
    _game_team_player = 1,
    _ai_dialogue_team_player = 1,
    _game_team_human = 2,
    _game_team_covenant = 3,
    _game_team_flood = 4,
    _game_team_sentinel = 5,
    _game_team_heretic = 6,
    _game_team_prophet = 7,
    k_multiplayer_team_count = 8
};

enum e_multiplayer_team_designator {
    _multiplayer_team_designator_first = 0,
    _multiplayer_team_designator_defender = 0,
    _multiplayer_team_designator_attacker = 1,
    k_multiplayer_team_designator_count = 8,
    _multiplayer_team_designator_none = 8,
    _multiplayer_team_designator_invalid = 9
};

enum e_campaign_difficulty_level {
    _campaign_difficulty_level_easy = 0,
    _campaign_difficulty_easy = 0,
    campaign_difficulty_default = 0,
    _campaign_difficulty_level_normal = 1,
    _campaign_difficulty_normal = 1,
    _campaign_difficulty_level_heroic = 2,
    _campaign_difficulty_heroic = 2,
    _campaign_difficulty_level_legendary = 3,
    _campaign_difficulty_legendary = 3,
    k_number_of_campaign_difficulty_levels = 4,
    k_campaign_difficulty_levels_count = 4,
    campaign_difficulty_count = 5
};

enum e_campaign_id {
    _campaign_id_pre_game_fallback = 1,
    _campaign_id_none = -1,
    _campaign_none = -1,
    _main_render_campaign_none = -1
};

enum e_map_id {
    _map_id_mainmenu = 0x10231971,
    _map_id_pre_game_fallback = 0xbbd,
    _map_id_none = -1,
    _map_none = -1,
    _main_render_map_none = -1
};

enum e_game_engine_type {
    _game_engine_none = 0,
    _game_engine_type_none = 0,
    _game_engine_base = 0,
    _game_engine_base_variant = 0,
    _game_engine_ctf = 1,
    _game_engine_type_ctf = 1,
    _game_engine_slayer = 2,
    _game_engine_type_slayer = 2,
    _game_engine_oddball = 3,
    _game_engine_type_oddball = 3,
    _game_engine_king = 4,
    _game_engine_type_koth = 4,
    _game_engine_sandbox = 5,
    _game_engine_type_sandbox = 5,
    _game_engine_unused = 5,
    _game_engine_vip = 6,
    _game_engine_type_vip = 6,
    _game_engine_juggernaut = 7,
    _game_engine_type_jugg = 7,
    _game_engine_territories = 8,
    _game_engine_type_terries = 8,
    _game_engine_assault = 9,
    _game_engine_type_assault = 9,
    _game_engine_infection = 10,
    _game_engine_type_zombiez = 10,
    k_game_engine_type_count = 11
};

typedef char game_types_size_team[(sizeof(e_game_team) == 4) ? 1 : -1];
typedef char game_types_size_designator[(sizeof(e_multiplayer_team_designator) == 4) ? 1 : -1];
typedef char game_types_size_difficulty[(sizeof(e_campaign_difficulty_level) == 4) ? 1 : -1];
typedef char game_types_size_campaign_id[(sizeof(e_campaign_id) == 4) ? 1 : -1];
typedef char game_types_size_map_id[(sizeof(e_map_id) == 4) ? 1 : -1];
typedef char game_types_size_engine_type[(sizeof(e_game_engine_type) == 4) ? 1 : -1];

#endif // GAME_GAME_TYPES_H
