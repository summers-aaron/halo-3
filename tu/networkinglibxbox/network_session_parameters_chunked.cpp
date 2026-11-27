// networkinglibxbox/network_session_parameters_chunked.obj: from the records 0x444520 0x444590
// @compiler xdk6130
// @flags /TP /O2
#include "memory/bitstream.h"

#include "networking/session/network_session_parameters_chunked_types.h"

// @address 0x444520 network_session_encode_chunked
extern "C" void network_session_encode_chunked(c_bitstream *packet, const s_network_session_parameter_chunked_change_request *request)
{
    packet->write_integer("chunk-index", request->chunk_index, 9);
    packet->write_raw_data("chunk-data", request->chunk_data, 1024);
    if (request->chunk_index == 0)
    {
        packet->write_integer("chunk-count", request->chunk_count, 9);
        packet->write_integer("transmit-size", request->transmit_size, 16);
        packet->write_integer("transmit-checksum", request->transmit_checksum, 32);
    }
}

// @address 0x444590 network_session_decode_chunked
extern "C" bool network_session_decode_chunked(c_bitstream *packet, s_network_session_parameter_chunked_change_request *request)
{
    request->chunk_index = packet->read_integer("chunk-index", 9);
    packet->read_raw_data("chunk-data", request->chunk_data, 1024);
    if (request->chunk_index == 0)
    {
        request->chunk_count = packet->read_integer("chunk-count", 9);
        request->transmit_size = packet->read_integer("transmit-size", 16);
        request->transmit_checksum = packet->read_integer("transmit-checksum", 32);
    }
    bool valid = true;
    valid = valid && request->chunk_index >= 0;
    if (request->chunk_index == 0)
    {
        valid = valid && request->chunk_count > 0;
        valid = valid && request->transmit_size > 0;
    }
    return valid && !packet->error_occurred();
}
