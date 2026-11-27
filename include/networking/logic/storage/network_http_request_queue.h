#ifndef NETWORKING_LOGIC_STORAGE_NETWORK_HTTP_REQUEST_QUEUE_H
#define NETWORKING_LOGIC_STORAGE_NETWORK_HTTP_REQUEST_QUEUE_H

// Functions defined in tu/networkinglibxbox/network_http_request_queue.cpp.

extern "C" void make_url(const char *in, char *out_url);

#include <string.h>
#include "memory/static_arrays.h"
#include "networking/logic/storage/network_http_request_hash.h"
#include "networking/http_request_types.h"
#include "networking/online_lsp_server_acquire_result_types.h"
#include "networking/tools/http_client.h"
#include "networking/online/online_lsp_manager.h"

class c_network_http_request_queue;
enum e_network_http_request_queue_failure_reason;
extern "C" const char k_http_request_queue_name_format[]; // "http request queue - %s"

class c_network_http_request_description {
    friend class c_http_buffer_downloader;
public:
    c_network_http_request_description(const char *url, e_http_request_type request_type);
private:
    c_static_string<256> m_url;
    e_http_request_type m_request_type;
    bool m_hash_valid;
    s_network_http_request_hash m_hash;
    c_static_string<1024> m_extra_headers;
    c_http_post_source m_post_source;
};

enum e_network_http_request_state {
    _network_http_request_state_none,
    _network_http_request_state_started
};

enum e_network_http_request_result {
    _network_http_request_result_pending,
    _network_http_request_result_succeeded,
    _network_http_request_result_failed,
    _network_http_request_result_retry
};

namespace {

class nhrq_http_post_source
{
public:
    __forceinline nhrq_http_post_source() :
        m_type(0),
        m_content_type("text/plain"),
        m_filename("blob"),
        m_contents(0),
        m_contents_file(0),
        m_contents_length(0),
        m_contents_offset(0)
    {
    }
private:
    long m_type;
    c_static_string<32> m_content_type;
    c_static_string<260> m_filename;
    const char *m_contents;
    s_file_reference *m_contents_file;
    long m_contents_length;
    long m_contents_offset;
};

class nhrq_http_description
{
public:
    __forceinline nhrq_http_description() :
        m_url(),
        m_request_type(_http_request_type_get),
        m_has_hash(false),
        m_extra_headers(),
        m_post_source()
    {
    }
    __forceinline const char *get_url() const { return m_url.get_string(); }
    __forceinline e_http_request_type get_request_type() const { return (e_http_request_type)m_request_type; }
    __forceinline const char *get_headers() const { return m_extra_headers.get_string(); }
    __forceinline const nhrq_http_post_source *get_post_source() const { return &m_post_source; }
private:
    c_static_string<256> m_url;
    e_http_request_type m_request_type;
    bool m_has_hash;
    unsigned char m_hash[20];
    c_static_string<1024> m_extra_headers;
    nhrq_http_post_source m_post_source;
};


}

enum e_network_http_request_queue_type { _network_http_request_queue_type_default, _network_http_request_queue_files };

class c_network_http_request_item {
public:
    __forceinline c_network_http_request_item() :
        m_state(0),
        m_queue(0),
        m_description(),
        m_fill_buffer_length(0),
        m_fill_buffer_progress(0),
        m_retry_count(0)
    {
    }
    __forceinline long get_request_cookie() const { return m_cookie; }
    __forceinline long get_request_state() const { return m_state; }
    __forceinline void set_request_state(e_network_http_request_state state) { m_state = state; }
    __forceinline const nhrq_http_description *get_description() const { return &m_description; }
    __forceinline e_online_lsp_service_type get_service_type() const { return m_service_type; }
    __forceinline void failed(bool retry) {
        m_retry_count = retry ? m_retry_count - 1 : 0;
    }
private:
    long m_cookie;
    long m_state;
    c_network_http_request_queue *m_queue;
    nhrq_http_description m_description;
    long m_client_usage_type;
    e_online_lsp_service_type m_service_type;
    char *m_fill_buffer;
    long m_fill_buffer_length;
    long m_fill_buffer_progress;
    long m_retry_count;
};

typedef char http_request_description_layout_check[(sizeof(void *) != 4 || (sizeof(nhrq_http_description) == 0x654)) ? 1 : -1];
typedef char http_request_item_layout_check[(sizeof(void *) != 4 || sizeof(c_network_http_request_item) == 0x678) ? 1 : -1];

class c_network_http_request_queue {
public:
    static __forceinline c_network_http_request_queue *get(e_network_http_request_queue_type type);
    c_network_http_request_queue(e_network_http_request_queue_type queue_type);
    void cancel_request(long cookie);
    long start_request(e_online_lsp_service_type service_type, const c_network_http_request_description *description);
    long start_request_into_buffer(e_online_lsp_service_type service_type,
        const c_network_http_request_description *description, char *buffer, long length);
    e_network_http_request_result is_fill_buffer_complete(long request_cookie, long *out_position,
        s_network_http_request_hash *out_hash, e_network_http_request_queue_failure_reason *out_failure_reason);
private:
    bool run_request(char *buffer, long buffer_length, long *bytes_read, bool *request_complete, long *error);
    e_network_http_request_result read_bytes_internal(c_network_http_request_item *item, char *buffer,
        long buffer_length, long *bytes_read, bool *item_was_removed,
        e_network_http_request_queue_failure_reason *failure_reason);
    e_network_http_request_result acquire_server(c_network_http_request_item *item);
    void request_failed(c_network_http_request_item *item, bool retry, bool server_was_reached);
    long m_queue_type;
    long m_next_cookie;
    long m_request_count;
    c_network_http_request_item m_requests[32];
    long m_server_token;
    long m_server_address;
    unsigned short m_server_port;
    c_http_client m_http_client;
    c_http_get_stream m_get_stream;
    c_http_post_stream m_post_stream;
};

extern c_network_http_request_queue g_network_http_request_queues[2];

__forceinline c_network_http_request_queue *c_network_http_request_queue::get(e_network_http_request_queue_type type)
{
    return &g_network_http_request_queues[type];
}

typedef char network_http_request_queue_size_check[(sizeof(void *) != 4 || ((sizeof(c_network_http_request_queue) == 0xf858) )) ? 1 : -1];

extern c_static_string<256> g_storage_url_subdirectory;
#endif // NETWORKING_LOGIC_STORAGE_NETWORK_HTTP_REQUEST_QUEUE_H
