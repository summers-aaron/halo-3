// networkinglibxbox/network_http_buffer_downloader.obj: from the records 0x8f7e8 0x8fe00 0x93218 0x932e8 0x93358 0x933d8 0x93568 0x210cb8 0x3c3100 0x3c31d8 0x3c3578 0x3c3690 0x3c3738
// @flags /TP /O2 /Oc
#define NOD3D
#include <xtl.h>
#include "memory/static_arrays.h"
#include "math/random_math.h"
// @address 0x8f7e8 ?get_data@c_http_buffer_downloader@@QAAXXZ
// @address 0x932e8 ?get_data@c_http_buffer_downloader@@QAAJPAPBDPAJ@Z
// @address 0x3c3690 ?stop_cache_submit@c_http_buffer_downloader@@AAAXXZ
// @address 0x3c3690 ?stop_cache_retrieve@c_http_buffer_downloader@@AAAXXZ
#include "cseries/crt_xdk.h"
#include <ppcintrinsics.h>
#include "multithreading/synchronization.h"
#include "simulation/simulation.h"
#include "cache/security_functions.h"
#include "text/font_cache.h"
#include "networking/logic/storage/network_storage_manifest.h"
#include "networking/online/online_url.h"
#include "networking/session/network_session_parameters.h"
#include "networking/logic/storage/network_storage_cache_key_types.h"
#include "networking/logic/storage/network_storage_cache.h"
#include "networking/http_request_types.h"
#include "networking/logic/storage/network_http_buffer_downloader.h"
#include "networking/tools/network_blf.h"
#include "networking/logic/storage/network_http_buffer_url.h"
#include "networking/logic/storage/network_http_request_hash.h"
#include "multithreading/synchronized_value.h"
#include "networking/tools/blf_authentication_types.h"

enum e_network_http_request_result;
enum e_network_http_request_queue_failure_reason;
// @address 0x7b0ecc g_network_storage_cache

// @address 0x80618 ?simulation_starting_up@@YA_NXZ

// @address 0x846a8 ?internal_critical_section_enter@@YAXJ@Z
// @address 0x847d0 ?internal_critical_section_leave@@YAXJ@Z

// @address 0x3ada60 ?network_blf_find_chunk_in_file@@YAPAXPBDKJFW4e_blf_file_authentication_type@@PAK@Z

#include "networking/logic/storage/network_http_buffer_variant_queries.h"


struct s_network_http_buffer_blf_downloader_b;

struct s_network_http_buffer_blf_downloader_a
{
    c_http_buffer_downloader m_downloader;
    char m_buffer[0x1194 - 0x690];
    long m_last_retry_count;
    void *m_data;
    unsigned long m_data_size;
};

#include "networking/logic/storage/network_http_request_queue.h"

struct s_network_http_buffer_blf_downloader_b
{
    c_http_buffer_downloader m_downloader;
    char m_buffer[0xefc0 - 0x690];
    long m_last_retry_count;
    void *m_data;
    unsigned long m_data_size;
};


// @address 0x406198 ??0c_network_http_request_description@@QAA@PBDW4e_http_request_type@@@Z
// @address 0x4064b8 ?cancel_request@c_network_http_request_queue@@QAAXJ@Z

static __forceinline long network_http_buffer_downloader_synchronized_get(c_synchronized_long *value)
{
    long result = value->value;
    __lwsync();
    return result;
}

static __forceinline float network_http_buffer_downloader_random_scale()
{
    random_seed_next(get_local_random_seed_address());
    unsigned short random = random_seed_high_half(get_local_random_seed_address());
    float result = (float)(double)(__int64)(unsigned int)random * 1.5259022e-05f + 1.0f;
    return result;
}

void c_http_buffer_downloader::get_data()
{
    if (m_url.m_string.m_string[0] != 0 && (simulation_starting_up() || m_internal_status != 0))
    {
        long now = (long)GetTickCount();
        s_network_storage_cache_key key;
        online_url_make_cache_key(m_url.m_string.m_string, (char *)&key);
        if (http_buffer_downloader_is_cache_invalid(this, &key))
            reset();
        if (m_internal_status == 0)
        {
            bool cache_unavailable = false;
            bool have_hash = false;
            if (m_url.m_cache_mode != 0)
            {
                c_network_storage_manifest *manifest = (c_network_storage_manifest *)network_storage_manifest_get();
                long state = 4;
                if (manifest != 0)
                    state = manifest->get_entry_state(&key);
                if (state == 4)
                {
                    cache_unavailable = true;
                }
                else if (m_hash_valid != 0)
                {
                    have_hash = true;
                }
                else if (state == 1 || state == 2)
                {
                    if (network_storage_manifest_copy_hash((c_network_storage_manifest *)manifest, &key, &m_hash))
                        have_hash = true;
                }
                else if (state == 3)
                {
                    if (m_next_retry_time == -1 || now > m_next_retry_time)
                    {
                        m_internal_status = _internal_status_retry_wait;
                        m_retry_count = m_retry_count + 1;
                    }
                    else
                    {
                        cache_unavailable = true;
                    }
                }
            }
            if (!cache_unavailable && m_internal_status == 0)
            {
                if (have_hash && g_network_storage_cache.contains(&key, &m_hash))
                {
                    if (g_network_storage_cache.begin_get_buffer(&key, &m_hash, (char *)m_buffer, m_buffer_size, &m_bytes_received, &m_cache_index))
                        m_internal_status = _internal_status_cache_retrieving;
                }
                else
                {
                    if (g_network_storage_cache.contains(&key, 0))
                        g_network_storage_cache.remove(&key);
                    if (m_next_retry_time == -1 || now > m_next_retry_time)
                        m_internal_status = _internal_status_download_pending;
                }
            }
        }
        if (m_internal_status == _internal_status_cache_retrieving)
            http_buffer_downloader_update_cache_retrieve(this);
        else if (m_internal_status == _internal_status_cache_submitting)
            http_buffer_downloader_update_cache_submit(this);
        else if (m_internal_status == _internal_status_downloading || m_internal_status == _internal_status_download_pending)
            update_download();

        if (m_internal_status == _internal_status_cache_ready || m_internal_status == _internal_status_cache_submitting)
        {
            m_next_retry_time = -1;
            m_retry_delay_msec = (int)(network_http_buffer_downloader_random_scale() * 2e+03f);
        }
        else if (m_automatic_retry_mode == 1 && m_internal_status == _internal_status_backoff)
        {
            m_next_retry_time = (int)(network_http_buffer_downloader_random_scale() * (float)m_retry_delay_msec + (float)now);
            long next = m_retry_delay_msec * 2;
            m_retry_delay_msec = next > 300000 ? 300000 : next;
            m_internal_status = 0;
            return;
        }
        else if (m_automatic_retry_mode == 2 && (m_internal_status == _internal_status_backoff || m_internal_status == _internal_status_retry_wait))
        {
            m_next_retry_time = (int)(network_http_buffer_downloader_random_scale() * (float)m_retry_delay_msec + (float)now);
            long next = m_retry_delay_msec * 2;
            m_retry_delay_msec = next > 300000 ? 300000 : next;
            m_internal_status = 0;
            return;
        }
    }
}

// @address 0x93218 network_http_buffer_downloader_get_game_variant
extern "C" long network_http_buffer_downloader_get_game_variant(
    s_network_http_buffer_blf_downloader_a *downloader, void **out_data, unsigned long *out_data_size)
{
    long size;
    const char *data;
    long result;
    size = 0;
    data = 0;
    result = downloader->m_downloader.get_data(&data, &size);
    if (result == 2 && data != 0)
    {
        if (downloader->m_last_retry_count != downloader->m_downloader.m_retry_count)
        {
            downloader->m_data = network_blf_find_chunk_in_file(data, size, 0x67766172, 10, _blf_file_authentication_none, &downloader->m_data_size);
        }
    }
    else
    {
        downloader->m_data = 0;
        downloader->m_data_size = 0;
    }
    if (out_data != 0)
    {
        *out_data = downloader->m_data;
    }
    if (out_data_size != 0)
    {
        *out_data_size = downloader->m_data != 0 ? downloader->m_data_size : 0;
    }
    return result;
}


long c_http_buffer_downloader::get_data(const char **buffer, long *buffer_size)
{
    get_data();
    if (buffer)
        *buffer = 0;
    if (m_internal_status == _internal_status_cache_ready || m_internal_status == _internal_status_cache_submitting)
    {
        if (buffer)
            *buffer = m_buffer;
        if (buffer_size)
            *buffer_size = m_bytes_received;
    }
    return network_http_buffer_downloader_get_public_status(m_internal_status);
}
// @address 0x210cb8 http_buffer_downloader_set_extra_headers
extern "C" void http_buffer_downloader_set_extra_headers(c_http_buffer_downloader *downloader, const char *headers)
{
    if (headers != 0)
    {
        downloader->m_extra_headers.set(headers);
    }
    else
    {
        downloader->m_extra_headers.clear();
    }
}

static __forceinline c_network_http_request_queue *network_http_request_queue_get(long queue_type)
{
    return &g_network_http_request_queues[queue_type];
}
__forceinline void c_http_buffer_downloader::stop_download()
{
    if (m_internal_status == _internal_status_downloading)
    {
        network_http_request_queue_get(m_url.m_queue_type)->cancel_request(m_request_cookie);
        m_internal_status = _internal_status_none;
    }
    else if (m_internal_status == _internal_status_download_pending)
    {
        m_internal_status = _internal_status_none;
    }
}
// @address 0x3c3100 ?reset@c_http_buffer_downloader@@QAAXXZ
void c_http_buffer_downloader::reset()
{
    switch (m_internal_status)
    {
    case _internal_status_cache_retrieving:
        stop_cache_retrieve();
        break;
    case _internal_status_cache_submitting:
        stop_cache_submit();
        break;
    case _internal_status_download_pending:
    case _internal_status_downloading:
        stop_download();
        break;
    }
    m_internal_status = _internal_status_none;
    m_retry_delay_msec = 2000;
    m_next_retry_time = -1;
}
static __forceinline s_network_storage_cache_entry *network_http_buffer_downloader_cache_entries()
{
    return g_network_storage_cache.m_storage.entries;
}
static __forceinline long network_http_buffer_downloader_find_cache_entry(long request_cookie)
{
    s_network_storage_cache_entry *entries = network_http_buffer_downloader_cache_entries();
    long index = 0;
    unsigned long offset = 0;
    do
    {
        if (entries[index].request_cookie == request_cookie)
            return index;
        index++;
        offset += sizeof(s_network_storage_cache_entry);
    } while (offset < 0x5a00);
    return -1;
}
void c_http_buffer_downloader::stop_cache_retrieve()
{
    long request_cookie = m_cache_index;
    internal_critical_section_enter(50);
    if (g_network_storage_cache.m_initialized)
    {
        long index = network_http_buffer_downloader_find_cache_entry(request_cookie);
        if (index != -1)
            g_network_storage_cache.cancel_io_internal(index);
    }
    internal_critical_section_leave(50);
    m_cache_index = -1;
    m_internal_status = _internal_status_none;
}
void c_http_buffer_downloader::stop_cache_submit()
{
    long request_cookie = m_cache_index;
    internal_critical_section_enter(50);
    if (g_network_storage_cache.m_initialized)
    {
        long index = network_http_buffer_downloader_find_cache_entry(request_cookie);
        if (index != -1)
            g_network_storage_cache.cancel_io_internal(index);
    }
    internal_critical_section_leave(50);
    m_cache_index = -1;
    m_internal_status = _internal_status_none;
}
// @address 0x3c3578 http_buffer_downloader_update_cache_retrieve
extern "C" void http_buffer_downloader_update_cache_retrieve(c_http_buffer_downloader *downloader)
{
    bool data_ready = false;
    bool handled = false;
    long request_cookie = downloader->m_cache_index;
    internal_critical_section_enter(50);
    if (g_network_storage_cache.m_initialized)
    {
        long index = network_http_buffer_downloader_find_cache_entry(request_cookie);
        if (index != -1)
        {
            long state = network_http_buffer_downloader_cache_entries()[index].state;
            if (state != 0)
            {
                if (state == 1 || state == 3)
                    data_ready = network_http_buffer_downloader_synchronized_get(&network_http_buffer_downloader_cache_entries()[index].async_success) != 0;
                handled = true;
            }
        }
    }
    internal_critical_section_leave(50);
    if (handled)
    {
        if (!data_ready)
            return;
        long retry_count = downloader->m_retry_count;
        downloader->m_cache_index = -1;
        downloader->m_internal_status = c_http_buffer_downloader::_internal_status_cache_ready;
        downloader->m_retry_count = retry_count + 1;
        return;
    }
    g_network_storage_cache.cancel_io_by_cookie(downloader->m_cache_index);
    downloader->m_cache_index = -1;
    downloader->m_internal_status = c_http_buffer_downloader::_internal_status_download_pending;
}
// @address 0x3c3738 http_buffer_downloader_update_cache_submit
extern "C" void http_buffer_downloader_update_cache_submit(c_http_buffer_downloader *downloader)
{
    bool in_flight = false;
    bool handled = false;
    long request_cookie = downloader->m_cache_index;
    internal_critical_section_enter(50);
    if (g_network_storage_cache.m_initialized)
    {
        long index = network_http_buffer_downloader_find_cache_entry(request_cookie);
        if (index != -1)
        {
            long state = network_http_buffer_downloader_cache_entries()[index].state;
            if (state != 0)
            {
                if (state == 1 || state == 2)
                    in_flight = true;
                handled = true;
            }
        }
    }
    internal_critical_section_leave(50);
    if (handled)
    {
        if (in_flight)
        {
            downloader->m_internal_status = c_http_buffer_downloader::_internal_status_cache_ready;
            downloader->m_cache_index = -1;
        }
    }
    else
    {
        downloader->stop_cache_submit();
    }
}
// @address 0x93358 network_http_buffer_downloader_get_public_status
extern "C" long network_http_buffer_downloader_get_public_status(unsigned long value)
{
    long result = 0;
    switch (value)
    {
    case 0:
        result = 0;
        break;
    case 1:
    case 2:
    case 3:
        result = 1;
        break;
    case 4:
    case 5:
        result = 2;
        break;
    case 6:
        result = 3;
        break;
    case 7:
        result = 4;
        break;
    }
    return result;
}
static __forceinline bool network_http_buffer_downloader_url_equal(const s_network_http_buffer_url *a, const s_network_http_buffer_url *b)
{
    return a->service_type == b->service_type && a->cache_mode == b->cache_mode && a->lifetime_seconds == b->lifetime_seconds &&
        strncmp(a->name, b->name, 256) == 0 && a->queue_type == b->queue_type;
}
// @address 0x933d8 http_buffer_downloader_set_url
extern "C" void http_buffer_downloader_set_url(c_http_buffer_downloader *downloader, const s_network_http_buffer_url *url)
{
    s_network_http_buffer_url *current = (s_network_http_buffer_url *)&downloader->m_url;
    if (!network_http_buffer_downloader_url_equal(url, current))
    {
        current->cache_mode = url->cache_mode;
        strncpy(current->name, url->name, 256);
        current->name[255] = 0;
        current->lifetime_seconds = url->lifetime_seconds;
        current->service_type = url->service_type;
        current->queue_type = url->queue_type;
        downloader->reset();
    }
}

// @address 0x93568 network_http_buffer_downloader_get_map_variant
extern "C" long network_http_buffer_downloader_get_map_variant(
    s_network_http_buffer_blf_downloader_b *downloader, void **out_data, unsigned long *out_data_size)
{
    long size;
    const char *data;
    long result;
    size = 0;
    data = 0;
    result = downloader->m_downloader.get_data(&data, &size);
    if (result == 2 && data != 0)
    {
        if (downloader->m_last_retry_count != downloader->m_downloader.m_retry_count)
        {
            downloader->m_data = network_blf_find_chunk_in_file(data, size, 0x6d766172, 12, _blf_file_authentication_none, &downloader->m_data_size);
        }
    }
    else
    {
        downloader->m_data = 0;
        downloader->m_data_size = 0;
    }
    if (out_data != 0)
    {
        *out_data = downloader->m_data;
    }
    if (out_data_size != 0)
    {
#if defined(__clang__)
        *out_data_size = downloader->m_data ? downloader->m_data_size : 0;
#else
        // match: 0x93568: int-typed null test keeps the two *out_data_size store/return tails apart (a pointer test merges them, function shrinks 232B -> 212B)
        unsigned long cached_data = (unsigned long)downloader->m_data;
        *out_data_size = cached_data ? downloader->m_data_size : cached_data;
#endif
    }
    return result;
}


// @address 0x8fe00 http_buffer_downloader_is_cache_invalid
static __forceinline bool network_http_buffer_downloader_hash_equal(const s_network_http_request_hash *a, const s_network_http_request_hash *b)
{
    return memcmp(a, b, sizeof(*a)) == 0;
}

extern "C" unsigned char http_buffer_downloader_is_cache_invalid(
    c_http_buffer_downloader *downloader, const s_network_storage_cache_key *key)
{
    c_network_storage_manifest *manifest = (c_network_storage_manifest *)network_storage_manifest_get();
    bool result = false;
    if (manifest != 0 && downloader->m_hash_valid == 0 && downloader->m_url.m_cache_mode != 0)
    {
        long state = manifest->get_entry_state(key);
        if (state == 1)
        {
            if (downloader->m_internal_status == c_http_buffer_downloader::_internal_status_cache_ready)
            {
                long now = (long)GetTickCount();
                long expiration_time = 0;
                if (manifest->m_manifest != 0)
                {
                    c_static_string<80> key_string(key->path);
                    for (long entry_index = 0;
                        entry_index < network_blf_payload_long(manifest->m_manifest->entry_count);
                        entry_index++)
                    {
                        if (key_string.is_equal(manifest->m_manifest->entries[entry_index].key.path))
                        {
                            expiration_time = manifest->m_times[entry_index];
                            break;
                        }
                    }
                }
                if (now > expiration_time + 300000)
                {
                    s_network_http_request_hash hash;
                    result = !network_storage_manifest_copy_hash(manifest, key, &hash)
                        || !network_http_buffer_downloader_hash_equal(&hash, &downloader->m_hash);
                }
            }
        }
        else if (state == 0)
        {
            if (downloader->m_url.m_cache_mode == 1)
                result = downloader->m_internal_status == c_http_buffer_downloader::_internal_status_cache_ready;
            else if (downloader->m_url.m_cache_mode == 2)
                result = downloader->m_internal_status == c_http_buffer_downloader::_internal_status_cache_ready
                    || downloader->m_internal_status == c_http_buffer_downloader::_internal_status_retry_wait;
        }
    }
    return result;
}


static __forceinline void network_http_buffer_downloader_touch_manifest(c_network_storage_manifest *manifest, const s_network_storage_cache_key *key)
{
    if (manifest->m_manifest != 0)
    {
        c_static_string<80> key_string(key->path);
        for (long index = 0; index < network_blf_payload_long(manifest->m_manifest->entry_count); ++index)
        {
            if (key_string.is_equal(manifest->m_manifest->entries[index].key.path))
            {
                manifest->m_times[index] = GetTickCount();
                break;
            }
        }
    }
}

__forceinline void c_http_buffer_downloader::start_cache_submit(const s_network_storage_cache_key *key, const s_network_http_request_hash *hash)
{
    m_hash = *hash;
    if (g_network_storage_cache.add_buffer(key, m_buffer, m_bytes_received, &m_hash, &m_cache_index))
        m_internal_status = _internal_status_cache_submitting;
}

// @address 0x3c31d8 ?update_download@c_http_buffer_downloader@@AAAXXZ
void c_http_buffer_downloader::update_download()
{
    c_network_http_request_queue *queue = network_http_request_queue_get(this->m_url.m_queue_type);
    if (this->m_internal_status == c_http_buffer_downloader::_internal_status_download_pending)
    {
        c_network_http_request_description description(
            this->m_url.m_string.m_string, this->m_has_extra_headers ? _http_request_type_post : _http_request_type_get);
        if (this->m_has_extra_headers)
            description.m_post_source = this->m_post_source;
        description.m_extra_headers.set(this->m_extra_headers.get_string());
        e_online_lsp_service_type service_type = (e_online_lsp_service_type)this->m_url.m_service_type;
        this->m_request_cookie = queue->start_request_into_buffer(service_type, &description, (char *)this->m_buffer, this->m_buffer_size);
        this->m_internal_status = this->m_request_cookie != 0 ? c_http_buffer_downloader::_internal_status_downloading
                                                              : c_http_buffer_downloader::_internal_status_backoff;
    }
    else if (this->m_internal_status == c_http_buffer_downloader::_internal_status_downloading)
    {
        long failure_reason;
        long result = ((long)(queue)
                ->is_fill_buffer_complete(this->m_request_cookie, (long *)(&this->m_bytes_received), 0,
                    (e_network_http_request_queue_failure_reason *)&failure_reason));
        s_network_storage_cache_key key;
        online_url_make_cache_key(this->m_url.m_string.m_string, (char *)&key);
        bool invalid_hash = false;
        if (result == 1)
        {
            this->m_request_cookie = 0;
            this->m_internal_status = _internal_status_cache_ready;
            ++this->m_retry_count;
            if (this->m_url.m_cache_mode != 0)
            {
                s_network_http_request_hash hash;
                if (security_calculate_hash(this->m_buffer, this->m_bytes_received, true, &hash))
                {
                    if (this->m_hash_valid)
                    {
                        if (!network_http_buffer_downloader_hash_equal(&this->m_hash, &hash))
                            invalid_hash = true;
                        else if (g_network_storage_cache.add_buffer(&key, this->m_buffer, this->m_bytes_received, &this->m_hash, &this->m_cache_index))
                            this->m_internal_status = c_http_buffer_downloader::_internal_status_cache_submitting;
                    }
                    else
                    {
                        c_network_storage_manifest *manifest = (c_network_storage_manifest *)network_storage_manifest_get();
                        if (manifest != 0)
                        {
                            if (manifest->get_entry_state(&key) == 1)
                            {
                                s_network_http_request_hash expected_hash;
                                if (!network_storage_manifest_copy_hash(manifest, &key, &expected_hash) ||
                                    !network_http_buffer_downloader_hash_equal(&hash, &expected_hash))
                                    invalid_hash = true;
                                else
                                    network_http_buffer_downloader_touch_manifest(manifest, &key);
                            }
                            else
                                manifest->add_untracked_entry(&key, &hash, this->m_url.m_lifetime_seconds);
                        }
                        if (!invalid_hash)
                        {
                            this->start_cache_submit(&key, &hash);
                        }
                    }
                }
                else
                    invalid_hash = true;
            }
        }
        if (invalid_hash || result == 2)
        {
            this->m_request_cookie = 0;
            this->m_internal_status = !invalid_hash && failure_reason == 1 ? _internal_status_retry_wait : _internal_status_backoff;
            ++this->m_retry_count;
            if (failure_reason == 1 && this->m_url.m_cache_mode == 2)
            {
                c_network_storage_manifest *manifest = (c_network_storage_manifest *)network_storage_manifest_get();
                if (manifest != 0)
                    manifest->add_untracked_entry(&key, 0, this->m_url.m_lifetime_seconds);
            }
        }
    }
}
