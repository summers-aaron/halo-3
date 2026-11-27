#ifndef NETWORKING_SESSION_NETWORK_SESSION_PARAMETERS_GAME_H
#define NETWORKING_SESSION_NETWORK_SESSION_PARAMETERS_GAME_H

// Functions defined in tu/networkinglibxbox/network_session_parameters_game.cpp that other units call.

class c_bitstream;

void game_start_status_encode_function(c_bitstream *stream, const void *data, long size);
bool game_start_status_decode_function(c_bitstream *stream, void *data, long size);
void countdown_timer_encode_function(c_bitstream *stream, const void *data, long size);
bool countdown_timer_decode_function(c_bitstream *stream, void *data, long size);
void map_encode_function(c_bitstream *stream, const void *data, long size);
bool map_decode_function(c_bitstream *stream, void *data, long size);
void initial_participants_encode_function(c_bitstream *stream, const void *data, long size);
bool random_seed_decode_function(c_bitstream *stream, void *data, long size);
bool simulation_protocol_decode_function(c_bitstream *stream, void *data, long size);
bool campaign_difficulty_decode_function(c_bitstream *stream, void *data, long size);
void random_seed_encode_function(c_bitstream *stream, const void *data, long size);
void end_game_encode_function(c_bitstream *stream, const void *data, long size);
bool privacy_mode_decode_function(c_bitstream *stream, void *data, long size);
void voice_repeater_encode_function(c_bitstream *stream, const void *data, long size);
bool voice_repeater_decode_function(c_bitstream *stream, void *data, long size);
void game_instance_encode_function(c_bitstream *stream, const void *data, long size);
bool game_instance_decode_function(c_bitstream *stream, void *data, long size);
bool initial_participants_decode_function(c_bitstream *stream, void *data, long size);

extern "C" bool request_campaign_quit_decode_function(c_bitstream *stream, void *data, long size);
extern "C" void leader_properties_encode_function(c_bitstream *stream, const void *data, long size);
extern "C" bool leader_properties_decode_function(c_bitstream *stream, void *data, long size);
extern "C" bool end_game_decode_function(c_bitstream *stream, void *data, long size);
extern "C" bool ui_game_mode_param_decode_function(c_bitstream *stream, void *data, long size);

extern "C" void matchmaking_tip_encode_function(c_bitstream *stream, const void *data, long size);
extern "C" bool matchmaking_tip_decode_function(c_bitstream *stream, void *data, long size);
extern "C" void language_encode_function(c_bitstream *stream, const void *data, long size);
extern "C" bool language_decode_function(c_bitstream *stream, void *data, long size);
extern "C" void determinism_version_encode_function(c_bitstream *stream, const void *data, long size);
extern "C" bool determinism_version_decode_function(c_bitstream *stream, void *data, long size);
extern "C" void simulation_protocol_encode_function(c_bitstream *stream, const void *data, long size);
extern "C" void campaign_difficulty_encode_function(c_bitstream *stream, const void *data, long size);
extern "C" void campaign_metagame_scoring_encode_function(c_bitstream *stream, const void *data, long size);
extern "C" bool campaign_metagame_scoring_decode_function(c_bitstream *stream, void *data, long size);
extern "C" void privacy_mode_encode_function(c_bitstream *stream, const void *data, long size);
extern "C" bool privacy_mode_extended_decode_function(c_bitstream *stream, void *data, long size);
extern "C" void synchronous_out_of_sync_encode_function(c_bitstream *stream, const void *data, long size);
extern "C" void request_campaign_quit_encode_function(c_bitstream *stream, const void *data, long size);

#endif // NETWORKING_SESSION_NETWORK_SESSION_PARAMETERS_GAME_H
