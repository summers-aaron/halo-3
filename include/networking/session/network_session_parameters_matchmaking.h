#ifndef NETWORKING_SESSION_NETWORK_SESSION_PARAMETERS_MATCHMAKING_H
#define NETWORKING_SESSION_NETWORK_SESSION_PARAMETERS_MATCHMAKING_H

// Functions defined in tu/networkinglibxbox/network_session_parameters_matchmaking.cpp.

class c_bitstream;

void matchmaking_game_list_encode_function(c_bitstream *stream, const void *data, long size);
bool matchmaking_game_list_decode_function(c_bitstream *stream, void *data, long size);
void matchmaking_hopper_list_encode_function(c_bitstream *stream,const void *data,long size);
bool matchmaking_hopper_list_decode_function(c_bitstream *stream, void *data, long size);
void matchmaking_requested_hopper_encode_function(c_bitstream *stream, const void *data, long size);
bool matchmaking_requested_hopper_decode_function(c_bitstream *stream, void *data, long size);
void matchmaking_rematch_encode_function(c_bitstream *stream, const void *data, long size);
bool matchmaking_rematch_decode_function(c_bitstream *stream, void *data, long size);

struct s_matchmaking_criteria_basic;
struct s_matchmaking_criteria_ranked;

extern "C" bool matchmaking_progress_decode_basic_criteria(c_bitstream *stream, s_matchmaking_criteria_basic *criteria);
extern "C" bool matchmaking_progress_decode_ranked_criteria(c_bitstream *stream, s_matchmaking_criteria_ranked *criteria);
extern "C" bool matchmaking_progress_search_criteria_decode(c_bitstream *stream, void *data, long size);

extern "C" void matchmaking_hopper_encode_function(c_bitstream *stream, const void *data, long size);
extern "C" bool matchmaking_hopper_decode(c_bitstream *stream, void *data, long size);
extern "C" bool matchmaking_search_preference_decode(c_bitstream *stream, void *data, long size);
extern "C" void matchmaking_progress_encode(c_bitstream *stream, const void *data, long size);
extern "C" bool matchmaking_progress_decode_function(c_bitstream *stream, void *data, long size);
extern "C" void matchmaking_progress_search_criteria_encode(c_bitstream *stream, const void *data, long size);
extern "C" void matchmaking_assembly_peer_evict_mask_encode(c_bitstream *stream, const void *data, long size);
extern "C" bool matchmaking_assembly_peer_evict_mask_decode(c_bitstream *stream, void *data, long size);
extern "C" void matchmaking_messaging_encode(c_bitstream *stream, const void *data, long size);
extern "C" bool matchmaking_messaging_decode(c_bitstream *stream, void *data, long size);

#endif // NETWORKING_SESSION_NETWORK_SESSION_PARAMETERS_MATCHMAKING_H
