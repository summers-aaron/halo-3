#ifndef NETWORKING_NETWORK_CONFIGURATION_H
#define NETWORKING_NETWORK_CONFIGURATION_H

// Functions defined in tu/networkinglibxbox/network_configuration.cpp.

struct s_network_configuration;
extern s_network_configuration g_network_configuration;

struct s_network_configuration_state;
extern "C" s_network_configuration_state g_network_configuration_state;

void network_configuration_initialize_private(bool unused);
extern "C" void network_configuration_update(void);

struct s_network_configuration_downloader;
struct s_player_profile_summary;

extern "C" long network_configuration_try_and_get_data(s_network_configuration_downloader *downloader, void **out_data, unsigned long *out_data_size);
extern "C" bool network_configuration_parse_lobby_film_max_players(void *self, wchar_t *buffer, int size);
extern "C" bool network_configuration_parse_lobby_forge_max_players(void *self, wchar_t *buffer, int size);
extern "C" bool network_configuration_parse_lobby_coop_max_players(void *self, wchar_t *buffer, int size);
extern "C" int network_configuration_get_player_class(s_player_profile_summary *profile);

extern "C" long g_network_configuration_update_version;

#endif // NETWORKING_NETWORK_CONFIGURATION_H
