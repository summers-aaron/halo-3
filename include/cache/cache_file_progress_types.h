#ifndef CACHE_CACHE_FILE_PROGRESS_TYPES_H
#define CACHE_CACHE_FILE_PROGRESS_TYPES_H

enum e_scenario_type
{
    _scenario_type_campaign = 0,
    _scenario_type_multiplayer = 1,
    _scenario_type_mainmenu = 2,
    _scenario_type_shared = 3,
    _scenario_type_single_player_shared = 4
};

enum e_cache_file_progress_type
{
    _cache_file_progress_fraction = 0,
    _cache_file_progress_remaining = 1
};

float cache_file_map_progress_helper(e_scenario_type type, const char *path, e_cache_file_progress_type progress_type);

#endif // CACHE_CACHE_FILE_PROGRESS_TYPES_H
