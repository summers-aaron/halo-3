#ifndef NETWORKING_TOOLS_NETWORK_DEBUG_DUMP_H
#define NETWORKING_TOOLS_NETWORK_DEBUG_DUMP_H

#include "multithreading/synchronized_value.h"

// Functions defined in tu/networkinglibxbox/network_debug_dump.cpp that other units call.

class c_http_client;
class c_http_stream;
#include "memory/static_arrays.h"
#include "tag_files/file_reference.h"
#include "networking/tools/http_stream_types.h"
#include "networking/tools/http_client.h"

struct s_netdebug_upload_task
{
    bool active;
    c_static_string<256> stream_name;
    void (*progress_callback)(long, long);
    void (*completion_callback)(bool, void *);
    void *callback_data;
    s_file_reference file;
    unsigned long file_crc;
};

struct s_netdebug_globals {
    char project[64];
    char build_version[64];
    char system_name[160];
    char session_id[128];
    char flash_version[256];
    char xdk_version[260];
    c_http_post_stream post_stream;
    c_http_client http_client;
    short retry_count;
    s_netdebug_upload_task active_task;
    s_netdebug_upload_task requests[10];
    bool enabled;
};

typedef char netdebug_upload_file_offset[(sizeof(void *) != 4 || ((offsetof(s_netdebug_upload_task, file) == 0x110))) ? 1 : -1];
typedef char netdebug_upload_crc_offset[(sizeof(void *) != 4 || ((offsetof(s_netdebug_upload_task, file_crc) == 0x220))) ? 1 : -1];
typedef char netdebug_post_stream_offset[(sizeof(void *) != 4 || ((offsetof(s_netdebug_globals, post_stream) == 932))) ? 1 : -1];
typedef char netdebug_http_client_offset[(sizeof(void *) != 4 || ((offsetof(s_netdebug_globals, http_client) == 4472))) ? 1 : -1];
typedef char netdebug_retry_offset[(sizeof(void *) != 4 || ((offsetof(s_netdebug_globals, retry_count) == 0x21d8))) ? 1 : -1];
extern "C" s_netdebug_globals g_netdebug_globals;
void compute_task_file_upload_signatures(s_netdebug_upload_task *task);

extern "C" int network_debug_dump_handle_upload_task(c_http_client *client, c_http_stream *stream, long unused, s_netdebug_upload_task *task);

extern "C" {
extern c_synchronized_long g_network_debug_dump_thread_done;
}

#endif // NETWORKING_TOOLS_NETWORK_DEBUG_DUMP_H
