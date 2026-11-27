#ifndef NETWORKING_TOOLS_HTTP_CLIENT_H
#define NETWORKING_TOOLS_HTTP_CLIENT_H

#include "networking/tools/http_stream_types.h"
#include "networking/transport/transport_address.h"
#include "networking/transport/transport_endpoint_winsock.h"

// The owner and request queue
// agree on the 0x1060-byte layout, including the embedded transport endpoint.
class c_http_client
{
public:
    c_http_client();
    bool is_started() { return m_current_state != 0; }
    bool is_connected();
    // match: 0x3b90b0, 0x3b9b00: state comparison inlines in both async routines instead of calling the placed method.
    __forceinline bool online_files_is_connected() const { return m_current_state == 2 || m_current_state == 3 || m_current_state == 4; }
    bool start(c_http_stream *stream, long ip_address, unsigned short port, const char *url, bool use_ssl);
    bool stop();
    bool do_work(bool *out_completed_successfully, char *out_response_content_buffer,
        long *out_response_content_buffer_count, long *out_response_code);
    long get_upload_position() { return m_upload_stream->get_position(); }
    long get_upload_length() { return m_upload_stream->get_length(); }
    unsigned long get_ip_address() { unsigned long address = 0; if (is_started()) address = m_address.address.ipv4_address; return address; }
    long get_upstream_quota() { return m_rate; }
    void set_upstream_quota(long quota) { m_rate = quota; }
    static void get_ip_address_string(long ip_address, c_static_string<16> *string);
private:
    bool start_connect();
    bool send_data();
    bool receive_data(bool *out_completed_successfully, char *out_response_content_buffer,
        long *out_response_content_buffer_count, long *out_response_code);
    bool parse_http_response(const char *buffer, long buffer_size, bool *out_completed_successfully,
        long *http_header_size, long *http_response_code, long *content_length);
    static void transport_shutdown(void *context);
    transport_address m_address;
    c_static_string<16> m_ip_address_string;
    transport_endpoint m_endpoint_storage;
    transport_endpoint *m_endpoint;
    c_http_stream *m_upload_stream;
    char m_response_buffer[4096];
    long m_response_position;
    long m_unknown;
    long m_socket_count;
    __int64 m_current_time;
    __int64 m_previous_time;
    long m_rate;
    long m_current_state;
};

typedef char http_client_size_check[(sizeof(void *) != 4 || ((sizeof(c_http_client) == 0x1060) )) ? 1 : -1];

#endif
