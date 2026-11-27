#ifndef NETWORKING_SESSION_NETWORK_SESSION_PARAMETERS_CHUNKED_H
#define NETWORKING_SESSION_NETWORK_SESSION_PARAMETERS_CHUNKED_H

// c_network_session_parameter_chunked<t_type>: a parameter too large for one update, sent in 128-byte chunks
// (game variant, map variant, saved film game options). The header March 2007's asserts name
// (networking\session\network_session_parameters_chunked.h:164..800). Vtable: the 16 base slots, then
// encode_update, decode_update, encode_change_request, decode_change_request.
//
// The chunk
// buffers are sized by the encoded size of each payload (6144, 28672, 34816 bytes). Every method kept out
// of line per instantiation (the owner unit defines them); prototypes here.

#include "networking/session/network_session_parameters_generic.h"
#include "networking/session/network_session_parameters_chunked_types.h"

class c_bitstream;

template<typename t_type> struct s_network_session_parameter_chunked_capacity;

template<typename t_type>
class c_network_session_parameter_chunked : public c_generic_network_session_parameter_data<t_type>
{
public:
    c_network_session_parameter_chunked();

private:
    virtual void notify_reset_peer_state(long peer_index);
    virtual bool can_set_peer_no_longer_requires_update(long peer_index);
    virtual bool can_clear_change_request_desired();
    virtual long get_update_payload_size() const;
    virtual bool handle_update_payload(const void *payload);
    virtual bool handle_change_request_payload(const void *payload);
    virtual void notify_set_update_required();
    virtual void notify_set_change_request();
    virtual void get_update_payload(long peer_index, void *payload) const;
    virtual void get_change_request_payload(long peer_index, void *payload) const;
    virtual long get_change_request_payload_size() const;
    virtual void notify_clear();
    virtual bool parameter_data_ready_for_consumption() const;
    virtual void notify_transition_state_to_become_host();
    virtual void notify_update_payload_sent_to_peer(long peer_index);
    virtual void notify_change_request_payload_sent();
    virtual void encode_update(c_bitstream *packet, const t_type *data) = 0;
    virtual bool decode_update(c_bitstream *packet, t_type *data) = 0;
    virtual void encode_change_request(c_bitstream *packet, const t_type *data) = 0;
    virtual bool decode_change_request(c_bitstream *packet, t_type *data) = 0;

    static long get_chunk_count_by_buffer_size(long buffer_size, long chunk_size);
    const char *get_chunk(long chunk_index, long *chunk_size) const;
    const char *get_update_chunk(long chunk_index, long *chunk_size) const;
    void reset_chunks();
    void clear_internal();

    long m_peer_chunk_indices[16];
    long m_change_request_chunk_index;
    long m_received_update_chunk_index;
    long m_received_change_request_chunk_index;
    char m_update_data[s_network_session_parameter_chunked_capacity<t_type>::size];
    long m_update_size;
    unsigned long m_update_checksum;
    long m_update_chunk_count;
    char m_transmit_data[s_network_session_parameter_chunked_capacity<t_type>::size];
    long m_transmit_size;
    unsigned long m_transmit_checksum;
    long m_transmit_chunk_count;
    long m_maximum_chunk_count;
};

#endif // NETWORKING_SESSION_NETWORK_SESSION_PARAMETERS_CHUNKED_H
