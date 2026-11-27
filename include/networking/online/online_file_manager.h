#ifndef NETWORKING_ONLINE_ONLINE_FILE_MANAGER_H
#define NETWORKING_ONLINE_ONLINE_FILE_MANAGER_H

#include <string.h>
#include <stddef.h>
class c_online_file_manager;
extern c_online_file_manager g_online_file_manager;
#include "cseries/async.h"
#include "networking/online/online_file_work_item_types.h"
#include "networking/tools/http_client.h"
#include "saved_games/buffered_file_writer_types.h"
#include "tag_files/file_reference.h"
#include "interface/c_signed_in_users_watcher.h"
#include "interface/queued_work_item_status_state_types.h"
#include "multithreading/synchronized_value.h"
#include "cseries/async_block.h"
#include "cseries/async_helpers.h"
#include "zlib/blam_zlib_memory_types.h"
#include "zlib/zlib.h"

enum e_map_memory_configuration;
struct s_work_item_transfer_task;

struct s_queued_work_item {
    e_queued_work_item_type type;
    s_online_file_description description;
    long priority;
    char hash[33];
    unsigned __int64 owner_xuid;
    unsigned __int64 requester_xuid;
    long slot;
    bool user_initiated;
    char name[256];
};

struct s_online_file_buffered_writer : s_buffered_file_writer {
    char write_buffer[4096];
};

class c_online_files_enqueuer {
private:
    static e_async_completion async_do_enqueue_for_transfer(s_async_task *task);
public:
    __forceinline c_online_files_enqueuer() : m_task_id(-1), m_done(0), m_success(0)
    {
        memset(m_items, 0, sizeof(m_items));
    }
    static __forceinline e_async_completion (*enqueue_callback())(s_async_task *) { return async_do_enqueue_for_transfer; }
    __forceinline void enqueue(const s_queued_work_item *item)
    {
        if (m_task_id != -1)
            async_block_until_done(&m_done, simple_yield_function, true, false, 0, 0);
        for (long index = 0; index < sizeof(m_items) / sizeof(m_items[0]); index++)
        {
            if (m_items[index].type == _queued_work_item_type_none)
            {
                m_items[index] = *item;
                break;
            }
        }
    }
    void update(bool *changed);
    s_queued_work_item m_items[8];
    long m_task_id;
    c_synchronized_long m_done;
    c_synchronized_long m_success;
    s_queued_work_item m_current_item;
};

union u_online_file_transfer_data {
    struct {
        s_queued_work_item_status_tracker transfers[16];
        long transfer_count;
        long upstream_quota;
        long transfer_priority;
    } enumeration;
    struct {
        s_file_reference queued_file;
        s_file_reference transfer_file;
        s_queued_work_item work_item;
        char url[256];
    } upload;
    struct {
        s_file_reference queued_file;
        s_file_reference transfer_file;
        s_file_reference decompressed_file;
        s_blam_zlib_memory zlib_memory;
        z_stream_s stream;
        s_queued_work_item work_item;
    } compression;
    struct {
        s_file_reference queued_file;
        s_file_reference transfer_file;
        s_queued_work_item work_item;
        char input[256];
        char output[256];
    } download;
};

class c_online_files_active_transfer_tracker {
public:
    void set_transfers(s_queued_work_item_status_tracker *new_transfers, long new_count, bool *changed);
    void update_transfer_status(const char *filename, e_queued_work_item_status_state state, float progress);
    bool get_transfer_status(long transfer_index, s_queued_work_item_status *status) const;
    bool get_transfer_status_by_slot(unsigned __int64 owner_xuid, long slot, s_queued_work_item_status *status) const;
    __forceinline void mark_dirty()
    {
        m_status_dirty = true;
    }
    __forceinline void refresh()
    {
        m_refresh_count++;
        m_status_dirty = true;
    }
    __forceinline void update();
    __forceinline long get_transfer_count() const { return m_transfer_count; }
    s_queued_work_item_status_tracker m_transfers[16];
    long m_transfer_count;
    long m_refresh_count;
    bool m_status_dirty;
    long m_state;
    long m_task_id;
    volatile long m_task_done;
    volatile long m_enumeration_done;
    bool m_enumeration_succeeded;
    c_signed_in_users_watcher m_signed_in_users_watcher;
private:
    void start_enumerate_files();
    static e_async_completion async_do_enumerate_files(s_async_task *task);
    __forceinline void sort_transfers();
};

struct s_online_file_manager_self {
    // match: the constructor only ever runs on the one global instance g_online_file_manager
    __forceinline s_online_file_manager_self();
};

class c_online_file_manager : s_online_file_manager_self {
public:
    c_online_file_manager();
    void stop();
    __forceinline void set_upstream_quota(long quota)
    {
        if (m_upstream_quota_override == -1)
            m_http_client.set_upstream_quota(quota);
    }
    static __forceinline c_online_file_manager *get()
    {
        return (c_online_file_manager *)(&g_online_file_manager);
    }
    __forceinline void memory_initialize(e_map_memory_configuration configuration);
    void enqueue_for_transfer(e_queued_work_item_type type, const s_online_file_description *description, const char *name,
        const char *hash, unsigned __int64 owner_xuid, unsigned __int64 requester_xuid, long slot, bool user_initiated);
    void post_connection_error(bool forced, const s_queued_work_item_status_tracker *transfers);
    void update();
    __forceinline void start()
    {
        if (m_buffer && !m_active)
        {
            m_active = true;
            m_refresh_queue = true;
            m_active_transfer_tracker.refresh();
        }
    }
    __forceinline const c_online_files_active_transfer_tracker *get_active_transfers() const { return &m_active_transfer_tracker; }
    bool m_active;
    long m_state;
    bool m_refresh_queue;
    bool m_status_known;
    bool m_error_posted;
    c_synchronized_long m_upstream_quota;
    c_synchronized_long m_transfer_priority;
    long m_next_refresh_time;
    void *m_buffer;
    long m_buffer_size;
    c_http_client m_http_client;
    c_http_post_stream m_post_stream;
    c_http_get_stream m_get_stream;
    long m_upstream_quota_override;
    long m_task_id;
    c_synchronized_long m_task_done;
    struct s_sync_word {
        volatile long value;
        __forceinline s_sync_word() : value(0) {}
        __forceinline long *operator&() { return (long *)&value; }
    };
    s_sync_word m_task_succeeded;
    s_sync_word m_task_cancelled;
    bool m_task_error_forced;
    long m_upload_id;
    c_synchronized_long m_upload_status;
    long m_download_id;
    c_synchronized_long m_download_status;
    c_online_files_enqueuer m_enqueuer;
    c_signed_in_users_watcher m_signed_in_users_watcher;
    c_online_files_active_transfer_tracker m_active_transfer_tracker;
    s_online_file_buffered_writer m_writer;
    u_online_file_transfer_data m_transfer_data;
private:
    void start_enumerate_files();
    void end_enumerate_files();
    void setup_task(s_work_item_transfer_task *task);
    static void create_file_reference_with_extension(s_file_reference *reference, const char *name, const char *extension);
    void start_compressing_file(const s_queued_work_item_status_tracker *tracker);
    void start_uploading_file(const s_queued_work_item_status_tracker *tracker, bool compressed);
    void start_downloading_file(const s_queued_work_item_status_tracker *tracker, bool downloaded);
    void start_decompressing_file(const s_queued_work_item_status_tracker *tracker);
    static e_async_completion async_do_enumerate_and_or_cleanup_files(s_async_task *task);
    static e_async_completion async_do_upload(s_async_task *task);
    static e_async_completion async_do_compression(s_async_task *task);
    static e_async_completion async_do_download(s_async_task *task);
    static e_async_completion async_do_bump_transfer_priority(s_async_task *task);
    static e_async_completion async_do_decompression(s_async_task *task);
public:
  void bump_transfer_priority(char const *p0);
};

typedef char online_file_manager_size_check[(sizeof(void *) != 4 || sizeof(c_online_file_manager) == 0x76f8) ? 1 : -1];
typedef char online_file_manager_http_offset[(sizeof(void *) != 4 || ((offsetof(c_online_file_manager, m_http_client) == 0x20))) ? 1 : -1];
typedef char online_file_manager_quota_offset[(sizeof(void *) != 4 || ((offsetof(c_online_file_manager, m_upstream_quota_override) == 0x295c))) ? 1 : -1];
typedef char online_file_transfer_data_size[(sizeof(void *) != 4 || sizeof(u_online_file_transfer_data) == 0x1410) ? 1 : -1];
typedef char online_file_compression_work_item_offset[(sizeof(void *) != 4 || offsetof(u_online_file_transfer_data, compression.work_item) == 0x370) ? 1 : -1];
typedef char online_files_enqueuer_size[(sizeof(void *) != 4 || sizeof(c_online_files_enqueuer) == 0x14e0) ? 1 : -1];
#endif
