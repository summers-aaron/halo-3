#ifndef NETWORKING_ONLINE_ONLINE_FILES_GLOBALS_H
#define NETWORKING_ONLINE_ONLINE_FILES_GLOBALS_H

extern "C" {
extern const char k_custom_map_image_filename_format[];
extern unsigned char k_online_files_rsa_public_key[];
extern const long k_online_files_incompressible_blf_types[];
}

class c_network_http_request_queue;
#include "networking/online/online_globals_types.h"

#include "interface/c_player_marketplace.h"

#endif // NETWORKING_ONLINE_ONLINE_FILES_GLOBALS_H
