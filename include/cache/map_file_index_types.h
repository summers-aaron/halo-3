#ifndef CACHE_MAP_FILE_INDEX_TYPES_H
#define CACHE_MAP_FILE_INDEX_TYPES_H

enum e_map_file_index
{
    _map_file_index_none = -1,
    _map_file_index_mainmenu = 0,
    _map_file_index_shared = 1,
    _map_file_index_single_player_shared = 2,
    _map_file_index_first_multiplayer = 3,
    _map_file_index_last_multiplayer = 12,
    _map_file_index_first_campaign = 13,
    _map_file_index_last_campaign = 14,
    k_number_of_map_files = 15,
    k_number_of_legacy_map_files = 20
};

#endif // CACHE_MAP_FILE_INDEX_TYPES_H
