#ifndef NETWORKING_LOGIC_STORAGE_NETWORK_STORAGE_FILES_H
#define NETWORKING_LOGIC_STORAGE_NETWORK_STORAGE_FILES_H

// Functions defined in tu/networkinglibxbox/network_storage_files.cpp that other units call.

class c_bitstream;

void matchmaking_hopper_statistics_encode_function(c_bitstream *packet, const void *data, long data_size);
bool matchmaking_hopper_statistics_decode_function(c_bitstream *packet, void *data, long data_size);

#endif // NETWORKING_LOGIC_STORAGE_NETWORK_STORAGE_FILES_H
