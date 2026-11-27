#ifndef NETWORKING_NETWORK_GAME_DEFINITIONS_H
#define NETWORKING_NETWORK_GAME_DEFINITIONS_H

#include "networking/player_configuration_encoding_types.h"

// Functions defined in tu/networkinglibxbox/network_game_definitions.cpp.

class c_bitstream;
struct s_network_squad_status_data;
struct s_queried_player_hopper_statistics;

bool network_session_qos_status_decode_from_block(long block_size, void const * block, struct s_network_squad_status_data * status_data_out);
bool player_configuration_client_hopper_stats_decode(c_bitstream *packet, s_queried_player_hopper_statistics *statistics);
void squad_status_encode(c_bitstream *packet, const s_network_squad_status_data *status_data);
bool squad_status_decode(c_bitstream *packet, s_network_squad_status_data *status_data);

struct s_game_results_player_data;
struct s_network_session_qos_status_header;

extern "C" bool network_session_qos_status_header_verify(const s_network_session_qos_status_header *header);
extern "C" bool network_session_qos_status_header_decode(c_bitstream *packet, s_network_session_qos_status_header *header);
extern "C" void network_game_definition_player_decode(c_bitstream *packet, s_game_results_player_data *player);

struct s_network_game_definitions_client_configuration;
struct s_player_configuration_from_client;
struct s_player_configuration_from_host;

void player_configuration_host_encode(c_bitstream *packet, const s_player_configuration_from_host *player_configuration_from_host,
    e_player_configuration_encoding_type encoding_type);
bool player_configuration_host_decode(c_bitstream *packet, s_player_configuration_from_host *from_host, e_player_configuration_encoding_type encoding_type);
bool player_configuration_client_decode( c_bitstream *packet, s_player_configuration_from_client *from_client,
    e_player_configuration_encoding_type encoding_type);
extern "C" void player_configuration_client_encode_packet(c_bitstream *packet,
    const s_network_game_definitions_client_configuration *player_configuration_from_client,
    e_player_configuration_encoding_type encoding_type);

struct s_player_appearance;

extern "C" void player_appearance_encode(c_bitstream *packet, const s_player_appearance *appearance, e_player_configuration_encoding_type encoding_type);
bool player_appearance_decode(c_bitstream *packet, s_player_appearance *appearance, e_player_configuration_encoding_type encoding_type);

class c_network_session_membership;

extern "C" void matchmaking_calculate_best_possible_host(const c_network_session_membership *, long *, long *, long *, long *, long *);
extern "C" const wchar_t k_fake_host_name_digits[];

#endif // NETWORKING_NETWORK_GAME_DEFINITIONS_H
