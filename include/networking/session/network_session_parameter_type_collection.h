#ifndef NETWORKING_SESSION_NETWORK_SESSION_PARAMETER_TYPE_COLLECTION_H
#define NETWORKING_SESSION_NETWORK_SESSION_PARAMETER_TYPE_COLLECTION_H

#include "networking/session/network_session_parameters_base.h"
class c_bitstream;

// Shared 32-byte parameter record; offsets and callbacks confirmed by
// March network_session_parameter_type_collection.cpp, 825e1d78..825e27f8.

typedef void (*t_encode_network_session_parameter)(c_bitstream *, const void *, long);
typedef bool (*t_decode_network_session_parameter)(c_bitstream *, void *, long);


struct s_network_session_parameter_type
{
    bool initialized;
    const char *session_parameter_name;
    t_encode_network_session_parameter update_encode;
    t_decode_network_session_parameter update_decode;
    long update_size;
    t_encode_network_session_parameter change_request_encode;
    t_decode_network_session_parameter change_request_decode;
    long change_request_size;
};

class c_network_session_parameter_type_collection
{
public:
    void register_session_parameter_type(
        e_network_session_parameter_type session_parameter_type,
        const char *session_parameter_name,
        t_encode_network_session_parameter update_encode,
        t_decode_network_session_parameter update_decode,
        long update_size,
        t_encode_network_session_parameter change_request_encode,
        t_decode_network_session_parameter change_request_decode,
        long change_request_size)
    {
        s_network_session_parameter_type *parameter = &m_session_parameter_types[session_parameter_type];
        if (session_parameter_type == _network_session_parameter_type_session_size)
            m_session_parameter_types[0].session_parameter_name = session_parameter_name;
        else
            parameter->session_parameter_name = session_parameter_name;
        parameter->update_encode = update_encode;
        parameter->update_decode = update_decode;
        parameter->update_size = update_size;
        parameter->change_request_encode = change_request_encode;
        parameter->change_request_decode = change_request_decode;
        parameter->change_request_size = change_request_size;
        parameter->initialized = true;
    }

    __forceinline long get_update_size(long type)
    {
        long result = 0;
        s_network_session_parameter_type *parameter = &m_session_parameter_types[type];
        result = parameter->update_size;
        return result;
    }
    __forceinline void encode_update(long type, c_bitstream *stream, const void *data, long size)
    {
        m_session_parameter_types[type].update_encode(stream, data, size);
    }
    __forceinline void encode_change_request(long type, c_bitstream *stream, const void *data, long size)
    {
        m_session_parameter_types[type].change_request_encode(stream, data, size);
    }
    __forceinline bool decode_update(long type, c_bitstream *stream, void *data, long size)
    {
        bool result = false;
        s_network_session_parameter_type *parameter = &m_session_parameter_types[type];
        result = parameter->update_decode(stream, data, size);
        return result;
    }
    __forceinline long get_change_request_size(long type)
    {
        return m_session_parameter_types[type].change_request_size;
    }
    __forceinline bool decode_change_request(long type, c_bitstream *stream, void *data, long size)
    {
        return m_session_parameter_types[type].change_request_decode(stream, data, size);
    }

private:
    s_network_session_parameter_type m_session_parameter_types[k_network_session_parameter_type_count];
};




extern "C" {
extern c_network_session_parameter_type_collection *g_network_session_parameter_types;
// Decoder registry slot at 0x828c4488; lifecycle storage uses 0x828c4464.
extern c_network_session_parameter_type_collection *g_network_session_parameter_message_types;

bool synchronous_out_of_sync_decode_function(c_bitstream *, void *, long);


void network_session_encode_chunked(c_bitstream *, const void *, long);
bool network_session_decode_chunked(c_bitstream *, void *, long);
void saved_film_encode(c_bitstream *, const void *, long);
bool saved_film_decode(c_bitstream *, void *, long);
}

#endif
