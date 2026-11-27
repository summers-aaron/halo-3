// networkinglibxbox/network_http_request_queue.obj: from the records 0x406198 0x4061f8 0x406300 0x4063f0 0x4064b8 0x406590 0x4066e0 0x406838 0x406938 0x406b90 0x406c10
// @flags /TP /O2 /Oc
#define NOD3D
#include "multithreading/synchronization_types.h"
#include "memory/static_arrays.h"
#include "networking/online/online_url.h"
// @address 0x406198 ??0c_network_http_request_description@@QAA@PBDW4e_http_request_type@@@Z
#include <string.h>
#include "multithreading/synchronization.h"
#include "networking/logic/storage/network_http_request_cache.h"
#include "text/font_cache.h"
#include "interface/user_interface_networking.h"
#include "networking/logic/storage/network_http_request_queue.h"
#include "networking/logic/storage/network_http_request_queue_internals.h"
#include "networking/online_lsp_server_acquire_result_types.h"
#include "networking/http_request_types.h"
#include "networking/online/online_lsp.h"
#include "networking/tools/http_stream.h"
#include "networking/logic/storage/network_http_request_hash.h"
#include "networking/transport/transport_endpoint_winsock.h"


// @address 0x451b0 ??_7c_http_get_stream@@6B@
// @rdata 0x82046db8..0x82046dd0
// @address 0x46db8 k_http_request_queue_name_format
extern "C" {
const char k_http_request_queue_name_format[] = "http request queue - %s";
}
// @rdata-end

c_network_http_request_description::c_network_http_request_description(const char *url, e_http_request_type request_type) :
    m_url(url),
    m_request_type(request_type),
    m_hash_valid(false),
    m_extra_headers(),
    m_post_source()
{
}

// @address 0x4061f8 ??0c_network_http_request_queue@@QAA@W4e_network_http_request_queue_type@@@Z
c_network_http_request_queue::c_network_http_request_queue(e_network_http_request_queue_type queue_type) :
    m_queue_type(queue_type),
    m_request_count(0),
    m_server_token(-1)
{
}

// @address 0x4066e0 ?acquire_server@c_network_http_request_queue@@AAA?AW4e_network_http_request_result@@PAVc_network_http_request_item@@@Z
e_network_http_request_result c_network_http_request_queue::acquire_server(c_network_http_request_item *item)
{
    e_network_http_request_result result = _network_http_request_result_failed;
    c_static_string<256> service;
    service.clear();
    const nhrq_http_description *description = item->get_description();
    service.print(k_http_request_queue_name_format, description->get_url());
    e_online_lsp_server_acquire_result acquire_result = g_online_lsp_manager.acquire_server(
        item->get_service_type(), &m_server_token, &m_server_address, &m_server_port, service.m_string);
    if (acquire_result == acquire_success) {
        e_http_request_type request_type = description->get_request_type();
        c_http_stream *stream = 0;
        c_static_string<256> url;
        url.clear();
        if (request_type == _http_request_type_get) {
            stream = &m_get_stream;
        } else if (request_type == _http_request_type_post) {
            c_http_post_stream *post_stream = &m_post_stream;
            post_stream->set_source((const c_http_post_source *)description->get_post_source());
            stream = post_stream;
        }
        stream->reset();
        stream->set_headers(description->get_headers());
        make_url(description->get_url(), url.m_string);
        if (m_http_client.start((c_http_stream *)stream, m_server_address, m_server_port, url.m_string, false)) {
            item->set_request_state(_network_http_request_state_started);
            result = _network_http_request_result_pending;
        }
    } else if (acquire_result == acquire_pending) {
        result = _network_http_request_result_pending;
    }
    return result;
}

// @address 0x4064b8 ?cancel_request@c_network_http_request_queue@@QAAXJ@Z
void c_network_http_request_queue::cancel_request(long cookie)
{
    internal_critical_section_enter(27);
    for (long request_index = 0; request_index < m_request_count; ++request_index) {
        if (m_requests[request_index].get_request_cookie() == cookie) {
            if (m_requests[request_index].get_request_state() == 1)
                request_failed(&m_requests[request_index], false, false);
            --m_request_count;
            memmove(&m_requests[request_index], &m_requests[request_index + 1],
                    (m_request_count - request_index) * sizeof(c_network_http_request_item));
            break;
        }
    }
    _InterlockedDecrement(&g_synch_globals.critical_sections[27].lock_count.value);
    RtlLeaveCriticalSection(&g_synch_globals.critical_sections[27].critical_section);
}

// @address 0x406b90 ?request_failed@c_network_http_request_queue@@AAAXPAVc_network_http_request_item@@_N1@Z
void c_network_http_request_queue::request_failed(c_network_http_request_item *item, bool retry, bool server_was_reached)
{
    if (m_server_token != -1) {
        c_online_lsp_manager::get()->disconnect_from_server(m_server_token, !server_was_reached);
        m_server_token = -1;
    }
    item->failed(retry);
    m_http_client.stop();
}

// @address 0x406300 ?start_request@c_network_http_request_queue@@QAAJW4e_online_lsp_service_type@@PBVc_network_http_request_description@@@Z
long c_network_http_request_queue::start_request(e_online_lsp_service_type service_type, const c_network_http_request_description *request_description)
{
    s_start_request_queue *queue = (s_start_request_queue *)this;
    const s_start_request_description *description = (const s_start_request_description *)request_description;
    long request_id = 0;
    s_start_request_url url;
    s_start_request_critical_section_scope lock(27);
    make_url(description->m_url, url.m_string);
    if (queue->m_request_count < 32)
    {
        if (++queue->m_next_request_id == 0)
            queue->m_next_request_id = 1;
        queue->m_requests[queue->m_request_count] = s_start_request_item(queue->m_next_request_id, queue, service_type, description);
        queue->m_requests[queue->m_request_count].set_client_usage_type(0);
        queue->m_request_count++;
        request_id = queue->m_next_request_id;
    }
    return request_id;
}

static __forceinline s_start_request_item *network_http_request_queue_find_start_item(s_start_request_queue *queue, long request_id)
{
    s_start_request_item *item = 0;
    for (long request_index = 0; request_index < (long)queue->m_request_count; request_index++)
    {
        if (queue->m_requests[request_index].m_request_id == request_id)
        {
            item = &queue->m_requests[request_index];
            break;
        }
    }
    return item;
}

// @address 0x4063f0 ?start_request_into_buffer@c_network_http_request_queue@@QAAJW4e_online_lsp_service_type@@PBVc_network_http_request_description@@PADJ@Z
long c_network_http_request_queue::start_request_into_buffer(
    e_online_lsp_service_type service_type, const c_network_http_request_description *description, char *buffer, long length)
{
    s_start_request_critical_section_scope lock(27);
    s_start_request_queue *queue = (s_start_request_queue *)this;
    long request_id = start_request(service_type, description);
    if (request_id != 0)
    {
        s_start_request_item *item = network_http_request_queue_find_start_item(queue, request_id);
        if (item != 0)
        {
            item->m_fill_buffer = buffer;
            item->m_fill_buffer_length = length;
            item->m_fill_buffer_progress = 0;
            item->set_client_usage_type(1);
        }
    }
    return request_id;
}

// @address 0x406c10 make_url
void make_url(const char *in, char *out_url)
{
    // match: 0x406c10: prefix read through a pointer keeps both strlen calls as runtime loops; a direct literal folds them to 15
    const char *title_storage_prefix = "/storage/title/";
    c_static_string<256> str(in);
    if (str.starts_with(title_storage_prefix)) {
        c_static_string<256> sub;
        int prefix_length = (int)strlen(title_storage_prefix);
        int start = (int)strlen(title_storage_prefix);
        str.substring(start, str.length() - prefix_length, &sub);
        str.print("%s%s/%s", title_storage_prefix, g_storage_url_subdirectory.get_string(), sub.m_string);
    }
    strncpy(out_url, str.m_string, 256);
    out_url[255] = 0;
}

// @address 0x406590 ?is_fill_buffer_complete@c_network_http_request_queue@@QAA?AW4e_network_http_request_result@@JPAJPAUs_network_http_request_hash@@PAW4e_network_http_request_queue_failure_reason@@@Z
enum e_network_http_request_result c_network_http_request_queue::is_fill_buffer_complete(long request_id, long *out_position,
    struct s_network_http_request_hash *out_hash, enum e_network_http_request_queue_failure_reason *out_failure_reason)
{
    internal_critical_section_enter(27);
    s_network_http_request_queue *queue = (s_network_http_request_queue *)this;
    s_queue_item *item = 0;
    for (long i = 0; i < queue->request_count; ++i) {
        if (queue->items[i].id == request_id) {
            item = &queue->items[i];
            break;
        }
    }
    char *buffer = item->buffer;
    long buffer_size = item->buffer_size;
    long position = item->buffer_position;
    long bytes_read = 0;
#ifdef HALO3_64BIT
    typedef char nhrq_item_size_match1[(sizeof(s_start_request_item) == sizeof(c_network_http_request_item)) ? 1 : -1];
    typedef char nhrq_item_size_match2[(sizeof(s_queue_item) == sizeof(c_network_http_request_item)) ? 1 : -1];
    typedef char nhrq_desc_size_match[(sizeof(c_network_http_request_description) == sizeof(nhrq_http_description)) ? 1 : -1];
    char request_copy[sizeof(c_network_http_request_description)];
#else
    char request_copy[0x654];
#endif
    memcpy(request_copy, item->url, sizeof(request_copy));
    bool item_removed = false;
    long result = read_bytes_internal((c_network_http_request_item *)item, buffer + position, buffer_size - position, &bytes_read,
        &item_removed, out_failure_reason);
    if (result == 3) {
        item->buffer_position = 0;
        *out_position = 0;
        result = 0;
    } else {
        long new_position = bytes_read + position;
        *out_position = new_position;
        if (result == 0) {
            item->buffer_position = new_position;
            if (new_position == buffer_size) {
                cancel_request(item->id);
                result = 2;
            }
        } else if (result == 1) {
            network_http_request_cache_add_file_to_cache((const c_network_http_request_description *)request_copy,
                buffer, new_position, 0);
        }
    }
    internal_critical_section_leave(27);
    return (e_network_http_request_result)result;
}

// @address 0x406838 ?run_request@c_network_http_request_queue@@AAA_NPADJPAJPA_N1@Z
// match: 0x406838: pointer __restrict qualifier prevents alias reload
bool c_network_http_request_queue::run_request(char *buffer, long buffer_length, long *__restrict bytes_read, bool *request_complete, long *error)
{
    bool result = false;
    if (m_http_client.online_files_is_connected()) {
        c_online_lsp_manager::get()->server_connected(m_server_token);
    }
    *bytes_read = 0;
    *request_complete = false;
    if (buffer_length > 0) {
        long local_length = buffer_length;
        if (m_http_client.do_work(request_complete, buffer, &local_length, error))
        {
            *bytes_read = local_length;
            if (*request_complete) {
                c_online_lsp_manager::get()->disconnect_from_server(m_server_token, true);
                m_server_token = -1;
            }
            result = true;
        }
    } else {
        result = true;
    }
    return result;
}

// @address 0x406938 ?read_bytes_internal@c_network_http_request_queue@@AAA?AW4e_network_http_request_result@@PAVc_network_http_request_item@@PADJPAJPA_NPAW4e_network_http_request_queue_failure_reason@@@Z
e_network_http_request_result c_network_http_request_queue::read_bytes_internal(
    c_network_http_request_item *request_item, char *buffer, long buffer_length, long *out_bytes_read,
    bool *out_item_was_removed, e_network_http_request_queue_failure_reason *out_failure_reason)
{
    s_network_http_request_queue *queue = (s_network_http_request_queue *)this;
    s_queue_item *item = (s_queue_item *)request_item;
    char success_url[256];
    char failure_url[256];
    internal_critical_section_enter(27);
    int result = 2;
    int failure_reason = 0;
    internal_critical_section_enter(27);
    int item_index = -1;
    for (int i = 0; i < queue->request_count; ++i) {
        if (item->id == queue->items[i].id) {
            item_index = i;
            break;
        }
    }
    _InterlockedDecrement(&g_synch_globals.critical_sections[27].lock_count.value);
    RtlLeaveCriticalSection(&g_synch_globals.critical_sections[27].critical_section);
    *out_bytes_read = 0;
    *out_item_was_removed = false;
    if (item_index == 0) {
        if (item->phase == 0) {
            if (!user_interface_networking_is_throttled())
                result = acquire_server(request_item);
            else result = 0;
        } else if (item->phase == 1) {
            bool request_complete = false;
            long response_code = 0;
            if (run_request(buffer, buffer_length, out_bytes_read, &request_complete, &response_code)) {
                result = request_complete ? 1 : 0;
            } else if (response_code == 404) {
                failure_reason = 1;
            } else if (response_code == 400) {
                failure_reason = 2;
            } else if (response_code == 500) {
                failure_reason = 3;
            }
        }
    } else result = 0;
    if (out_failure_reason) *out_failure_reason = (e_network_http_request_queue_failure_reason)failure_reason;
    if (result == 1) {
        success_url[0] = 0;
        make_url(item->url, success_url);
        --queue->request_count;
        memmove(queue->items, queue->items + 1, queue->request_count * sizeof(s_queue_item));
        *out_item_was_removed = true;
    } else if (result == 2) {
        failure_url[0] = 0;
        make_url(item->url, failure_url);
        if (failure_reason == 1 || failure_reason == 2) {
            request_failed(request_item, false, false);
            item->phase = 0;
            --queue->request_count;
            memmove(queue->items, queue->items + 1, queue->request_count * sizeof(s_queue_item));
            *out_item_was_removed = true;
        } else {
            request_failed(request_item, true, true);
            item->phase = 0;
            if (item->retry_count > 0) result = 3;
            else {
                --queue->request_count;
                memmove(queue->items, queue->items + 1, queue->request_count * sizeof(s_queue_item));
                *out_item_was_removed = true;
            }
        }
    }
    internal_critical_section_leave(27);
    return (e_network_http_request_result)result;
}

c_static_string<256> g_storage_url_subdirectory = "";
c_network_http_request_queue g_network_http_request_queues[2] = {
    c_network_http_request_queue(_network_http_request_queue_type_default),
    c_network_http_request_queue(_network_http_request_queue_files),
};

