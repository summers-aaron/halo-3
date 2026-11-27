#ifndef NETWORKING_SESSION_NETWORK_SESSION_PARAMETERS_SESSION_H
#define NETWORKING_SESSION_NETWORK_SESSION_PARAMETERS_SESSION_H

// Functions defined in tu/networkinglibxbox/network_session_parameters_session.cpp that other units call.

#include "networking/session/network_session_parameters_base.h"

class c_bitstream;

bool remote_join_data_decode_function(c_bitstream *stream, void *update_message, long size);
void remote_join_data_encode_function(c_bitstream *stream, const void *update_message, long size);
void session_size_encode_function(c_bitstream *packet, const void *data, long size);
bool session_size_decode_function(c_bitstream *stream,void *data,long size);
void session_mode_encode_function(c_bitstream *packet, const void *data, long size);
bool session_mode_decode_function(c_bitstream *stream, void *data, long size);
bool host_frame_quality_decode(c_bitstream *stream, unsigned long *out);

#endif // NETWORKING_SESSION_NETWORK_SESSION_PARAMETERS_SESSION_H
