#ifndef GAME_MULTIPLAYER_HOPPER_SERVICES_H
#define GAME_MULTIPLAYER_HOPPER_SERVICES_H

#include "main/levels.h"
#include "networking/network_configuration_globals.h"

struct s_multiplayer_game_hopper_manifest_file;

extern "C" void online_url_set_base_path(void *url);

extern "C" long network_http_buffer_downloader_get_game_variant(void *downloader, void **out_data, unsigned long *out_data_size);
extern "C" long network_http_buffer_downloader_get_map_variant(void *downloader, void **out_data, unsigned long *out_data_size);
extern "C" const s_multiplayer_game_hopper_manifest_file *network_storage_manifest_get(void);


#endif // GAME_MULTIPLAYER_HOPPER_SERVICES_H
