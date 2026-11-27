#ifndef NETWORKING_LOGIC_STORAGE_NETWORK_HTTP_REQUEST_QUEUE_INTERNALS_H
#define NETWORKING_LOGIC_STORAGE_NETWORK_HTTP_REQUEST_QUEUE_INTERNALS_H

#include "multithreading/synchronization.h"
#include "multithreading/synchronization_types.h"

struct s_start_request_queue;

struct s_start_request_critical_section_scope
{
    s_start_request_critical_section_scope(long critical_section_id) : m_critical_section_id(critical_section_id)
    {
        internal_critical_section_enter(m_critical_section_id);
    }
    ~s_start_request_critical_section_scope()
    {
        internal_critical_section_leave(m_critical_section_id);
    }
    long m_critical_section_id;
};

struct s_start_request_url
{
    s_start_request_url() { m_string[0] = 0; }
    char m_string[256];
};

struct s_start_request_description
{
    char m_url[256];
#ifdef HALO3_64BIT
    char m_payload_bytes[sizeof(nhrq_http_description) - 256];
#else
    char m_payload_bytes[1620 - 256];
#endif
};

struct s_start_request_item
{
    s_start_request_item(long request_id, s_start_request_queue *queue, long service_type, const s_start_request_description *description) :
        m_request_id(request_id),
        m_attempt_count(0),
        m_queue(queue),
        m_description(*description),
        m_service_type(service_type),
        m_fill_buffer_length(0),
        m_fill_buffer_progress(0)
    {
        set_state(2);
    }
    void set_state(long state)
    {
        {
            s_start_request_critical_section_scope lock(28);
        }
        m_state = state;
    }
    void set_client_usage_type(long usage_type) { m_client_usage_type = usage_type; }
    long m_request_id;
    long m_attempt_count;
    s_start_request_queue *m_queue;
    s_start_request_description m_description;
    long m_client_usage_type;
    long m_service_type;
    char *m_fill_buffer;
    long m_fill_buffer_length;
    long m_fill_buffer_progress;
    long m_state;
};

struct s_start_request_queue
{
    long m_queue_type;
    long m_next_request_id;
    unsigned long m_request_count;
    s_start_request_item m_requests[32];
};

struct s_http_client_request_queue
{
    char client_prefix_bytes[0x105c];
    int state;
    bool is_connected() const { return state == 2 || state == 3 || state == 4; }
};

struct s_queue_item
{
    long id;
    long phase;
#ifdef HALO3_64BIT
    struct s_network_http_request_queue *m_queue;
    char url[256];
    char description_rest[sizeof(nhrq_http_description) - 256];
    long client_usage_type;
    long service_type;
    char *buffer;
    long buffer_size;
    long buffer_position;
    long retry_count;
#else
    long attempt_count;
    char url[256];
    long value;
    char has_hash;
    char hash_bytes[20];
    char has_headers;
    char header_bytes[1026];
    long post_source_type;
    char content_type[32];
    char blob[260];
    long post_source_length;
    long post_source_offset;
    long service_type;
    long service_flags;
    char item_prefix_bytes[8];
    char *buffer;
    long buffer_size;
    long buffer_position;
    long retry_count;
#endif
};

struct s_network_http_request_queue
{
    long queue_type;
    long next_request_id;
    long request_count;
    s_queue_item items[32];
    long server_id;
    char server_endpoint_bytes[8];
    s_http_client_request_queue http_client;
};

#endif // NETWORKING_LOGIC_STORAGE_NETWORK_HTTP_REQUEST_QUEUE_INTERNALS_H
