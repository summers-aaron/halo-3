#ifndef NETWORKING_LOGIC_STORAGE_NETWORK_HTTP_BUFFER_DOWNLOADER_H
#define NETWORKING_LOGIC_STORAGE_NETWORK_HTTP_BUFFER_DOWNLOADER_H

#include "networking/logic/storage/network_http_request_hash.h"
#include "networking/tools/http_stream.h"
#include <stddef.h>

struct s_network_storage_cache_key;
extern "C" long network_http_buffer_downloader_get_public_status(unsigned long internal_status);
enum e_online_lsp_service_type;
enum e_download_status;
class c_http_buffer_downloader;
extern "C" void http_buffer_downloader_update_cache_submit(c_http_buffer_downloader *downloader);

// c_url_string has no March assert naming its header. Kept beside its by-value user
// alongside the downloader that stores it by value.
class c_url_string
{
public:
    __forceinline c_url_string() : m_service_type(0), m_cache_mode(0), m_lifetime_seconds(600), m_queue_type(0) {}
    c_url_string(const char *url, bool cachable);
    void set(const c_url_string *url);
    const char *get_string() const;
    c_static_string<256> *get_url_string();
    bool get_cachable() const;
    void set_cachable(bool cachable);
    e_online_lsp_service_type get_service_type() const;
    void set_service_type(e_online_lsp_service_type service_type);
    long get_untracked_cache_lifetime_seconds() const;
    c_static_string<256> m_string;
    long m_service_type;
    long m_cache_mode;
    long m_lifetime_seconds;
    long m_queue_type;
};

// March's methods/asserts: networking\logic\storage\network_http_buffer_downloader.cpp.
// The buffer constructor initializes the URL, request tokens, and retry state.
// The URL and download status are queried independently.
class c_http_buffer_downloader
{
    friend void http_buffer_downloader_update_cache_submit(c_http_buffer_downloader *downloader);
public:
    enum e_internal_status
    {
        _internal_status_none,
        _internal_status_cache_retrieving,
        _internal_status_download_pending,
        _internal_status_downloading,
        _internal_status_cache_submitting,
        _internal_status_cache_ready,
        _internal_status_backoff,
        _internal_status_retry_wait
    };
    // The buffer constructor supplies the download destination.
    // March initializes the URL/post source, request/cache tokens, buffer, status and retry fields.
    c_http_buffer_downloader();
    c_http_buffer_downloader(char *buffer, long buffer_size);
    __forceinline virtual ~c_http_buffer_downloader() { reset(); }
    void reset();
    void get_data();
    long get_data(const char **buffer, long *buffer_size);
    void set_hash(const s_network_http_request_hash *hash);
    // March: m_buffer = buffer; m_buffer_size = buffer_size.
    void set_download_buffer(char *buffer, long buffer_size);
    // March: if (source) { m_post_source = *source; m_has_extra_headers = true; }
    //        else m_has_extra_headers = false. No staged caller instantiates this setter.
    void set_http_post_source(const c_http_post_source *source);
    // March: headers ? m_extra_headers.set(headers) : m_extra_headers.clear().
    void set_extra_headers(const char *headers);
    __forceinline long get_download_status() const { return network_http_buffer_downloader_get_public_status(m_internal_status); }
    // Retry policy and progress are available to GUI consumers.
    __forceinline long get_retry_count() const { return m_retry_count; }
    __forceinline void set_automatic_retries_enabled(bool enabled) { m_automatic_retry_mode = enabled ? 2 : 0; }
    __forceinline void set_fixed_retries_enabled(bool enabled) { m_automatic_retry_mode = enabled ? 1 : 0; }
private:
    void start_download();
    __forceinline void stop_download();
    void update_download();
    void start_cache_retrieve(const s_network_storage_cache_key *key, const s_network_http_request_hash *hash);
    void stop_cache_retrieve();
    void update_cache_retrieve();
    __forceinline void start_cache_submit(const s_network_storage_cache_key *key, const s_network_http_request_hash *hash);
    void stop_cache_submit();
    void update_cache_submit();
    // March maps 0 -> 0, 1/2/3 -> 1, 4/5 -> 2, 6 -> 3, 7 -> 4;
    // network_http_buffer_downloader_get_public_status implements this status mapping.
    static e_download_status get_download_status_from_internal_status(e_internal_status internal_status);
public:
    // Fields remain accessible to the existing free owner helpers while those are adopted.
    c_url_string m_url;
    bool m_hash_valid;
    s_network_http_request_hash m_hash;
    c_http_post_source m_post_source;
    bool m_has_extra_headers;                 // (post source is set)
    c_static_string<1024> m_extra_headers;
    long m_internal_status;
    long m_request_cookie;
    long m_cache_index;
    char *m_buffer;
    long m_buffer_size;                       // (capacity)
    long m_bytes_received;
    long m_automatic_retry_mode;
    long m_retry_delay_msec;
    long m_next_retry_time;
    long m_retry_count;
};

template<long k_buffer_size>
class c_http_stored_buffer_downloader : public c_http_buffer_downloader
{
public:
    // March 8259bf10: unadjusted base this, buffer immediately follows the base.
    __forceinline c_http_stored_buffer_downloader() : c_http_buffer_downloader(m_buffer, k_buffer_size) {}
    __forceinline virtual ~c_http_stored_buffer_downloader() {}
#if defined(_XBOX) && !defined(__clang__)
    // match: 0x958c8: declared, the compiler emits no second implicit copy past the end of the image.
    c_http_stored_buffer_downloader &operator=(const c_http_stored_buffer_downloader &other);
#endif
private:
    char m_buffer[k_buffer_size];
};

typedef char http_buffer_downloader_size_check[(sizeof(void *) != 4 || ((sizeof(c_http_buffer_downloader) == 0x690) )) ? 1 : -1];
typedef char http_buffer_downloader_hash_offset_check[(sizeof(void *) != 4 || ((offsetof(c_http_buffer_downloader, m_hash) == 0x115) )) ? 1 : -1];
typedef char http_buffer_downloader_post_offset_check[(sizeof(void *) != 4 || ((offsetof(c_http_buffer_downloader, m_post_source) == 0x12c) )) ? 1 : -1];
typedef char http_buffer_downloader_headers_offset_check[(sizeof(void *) != 4 || ((offsetof(c_http_buffer_downloader, m_extra_headers) == 0x265) )) ? 1 : -1];
typedef char http_buffer_downloader_status_offset_check[(sizeof(void *) != 4 || ((offsetof(c_http_buffer_downloader, m_internal_status) == 0x668) )) ? 1 : -1];
typedef char http_buffer_downloader_capacity_offset_check[(sizeof(void *) != 4 || ((offsetof(c_http_buffer_downloader, m_buffer_size) == 0x678) )) ? 1 : -1];
typedef char http_buffer_downloader_retry_offset_check[(sizeof(void *) != 4 || ((offsetof(c_http_buffer_downloader, m_retry_count) == 0x68c) )) ? 1 : -1];

// Functions defined in tu/networkinglibxbox/network_http_buffer_downloader.cpp.

extern "C" void http_buffer_downloader_set_extra_headers(c_http_buffer_downloader *downloader, const char *headers);
extern "C" void http_buffer_downloader_update_cache_retrieve(c_http_buffer_downloader *downloader);
extern "C" unsigned char http_buffer_downloader_is_cache_invalid(
    c_http_buffer_downloader *downloader, const s_network_storage_cache_key *key);

#endif
