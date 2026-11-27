#ifndef NETWORKING_LOGIC_STORAGE_NETWORK_HTTP_BLF_SIMPLE_DOWNLOADER_H
#define NETWORKING_LOGIC_STORAGE_NETWORK_HTTP_BLF_SIMPLE_DOWNLOADER_H

#include "networking/logic/storage/network_http_buffer_downloader.h"

// The stored buffer holds the whole BLF file: for the banhammer files it is the
// chunk payload size plus 2205 bytes (6049 = 3844 + 2205, 10401 = 8196 + 2205);
// the storage manifest buffer is March c_http_stored_buffer_downloader<0x3aa1>.
template<typename t_blf_type>
struct s_http_blf_simple_downloader_storage
{
    typedef c_http_stored_buffer_downloader<sizeof(t_blf_type)> type;
};

struct s_files_user_recent_players;
struct s_files_machine_banhammer_messages;
struct s_files_user_banhammer_messages;
struct s_online_file_manifest;
struct s_dynamic_matchmaking_hopper_statistics;
template<> struct s_http_blf_simple_downloader_storage<s_files_user_recent_players>
{
    typedef c_http_stored_buffer_downloader<6049> type;
};
template<> struct s_http_blf_simple_downloader_storage<s_files_machine_banhammer_messages>
{
    typedef c_http_stored_buffer_downloader<34977> type;
};
template<> struct s_http_blf_simple_downloader_storage<s_files_user_banhammer_messages>
{
    typedef c_http_stored_buffer_downloader<10401> type;
};
template<> struct s_http_blf_simple_downloader_storage<s_online_file_manifest>
{
    typedef c_http_stored_buffer_downloader<15009> type;
};
// The hopper statistics file is downloaded into a 2465-byte stored buffer.
template<> struct s_http_blf_simple_downloader_storage<s_dynamic_matchmaking_hopper_statistics>
{
    typedef c_http_stored_buffer_downloader<2465> type;
};

template <typename t_blf_type>
class c_http_blf_simple_downloader
{
public:
    c_http_blf_simple_downloader();
    __forceinline ~c_http_blf_simple_downloader() {}
    __forceinline void reset()
    {
        m_downloader.reset();
        m_last_retry_count = m_downloader.get_retry_count();
    }
    __forceinline void set_automatic_retries_enabled(bool enabled) { m_downloader.set_automatic_retries_enabled(enabled); }
    __forceinline void set_fixed_retries_enabled(bool enabled) { m_downloader.set_fixed_retries_enabled(enabled); }
    __forceinline c_http_buffer_downloader *buffer_downloader() { return &m_downloader; }
    __forceinline long get_retry_count() const { return m_downloader.get_retry_count(); }
    __forceinline long get_data(const char **buffer, long *buffer_size) { return m_downloader.get_data(buffer, buffer_size); }
private:
    typename s_http_blf_simple_downloader_storage<t_blf_type>::type m_downloader;
public:
    long m_last_retry_count;
    const t_blf_type *m_data;
    long m_data_size;
};

template <typename t_blf_type>
__forceinline c_http_blf_simple_downloader<t_blf_type>::c_http_blf_simple_downloader()
{
    m_data = 0;
    m_data_size = 0;
    m_last_retry_count = m_downloader.get_retry_count();
}

// This instantiation has an out-of-line constructor; every lobby screen that holds one calls it.
template<> c_http_blf_simple_downloader<s_dynamic_matchmaking_hopper_statistics>::c_http_blf_simple_downloader();

#endif
