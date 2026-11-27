// networkinglibxbox/network_webstats.obj: from the records 0x95f68 0x3a7798 0x3a7840 0x3a7850 0x3a7a58 0x3a7ba8 0x3a7ce0 0x3a7e28 0x3a7e88
// @flags /TP /O2 /Oc
// @address 0x95f68 ?network_webstats_update_upload_state@@YAXXZ
// @address 0x3a7798 network_webstats_initialize
// @address 0x3a7840 network_webstats_dispose
// @address 0x3a7ba8 ?do_uploading_work@@YAXXZ
// @address 0x3a7ce0 ?upload_failed@@YAX_N@Z
#include "cseries/cseries_types.h"
#include <string.h>
#include "memory/static_arrays.h"
#include "networking/tools/network_webstats.h"
#include "networking/tools/network_webstats_globals.h"
#include "tag_files/files_windows.h"
#include "zlib/blam_zlib_memory.h"
#include "memory/zlib_compression_level_types.h"
#include "zlib/zlib.h"
#include "networking/tools/http_stream.h"
#include "networking/tools/http_client.h"
#include "networking/online/online_lsp_manager.h"
// @address 0x408b50 ?stop@c_http_client@@QAA_NXZ
// @address 0x80608 GetTickCount
// @address 0x3975b8 compress3
// @address 0x3a7850 ?network_webstats_submit_buffer@@YA_NW4e_online_lsp_service_type@@PBXJPBDJ@Z
// @address 0x3c4130 ??0c_http_post_source@@QAA@XZ
// @address 0x286540 ?blam_zlib_malloc@@YAPAXPAXII@Z
// @address 0x286590 ?blam_zlib_free@@YAXPAX0@Z
// @address 0x1ae008 ?file_close@@YA_NPAUs_file_reference@@@Z
// @address 0x3a3530 ?disconnect_from_server@c_online_lsp_manager@@QAAXJ_N@Z

#include "cseries/crt_xdk.h"

#include "tag_files/file_reference.h"

// Untagged: 0x828c446c is inside the network_globals.cpp @shared_bss pool;
// wpl places this by name via data-owners.tsv (a tag conflicts with the pool).
extern "C" {
bool g_webstats_enabled;
}

// Other compilers only: the Xbox link takes this object's constant-initialized .data words and its initializer from the image.
#if !defined(_XBOX) || defined(__clang__)
s_webstats_globals g_webstats_globals;
#endif

struct s_upload_response
{
    bool complete;
    long length;
};

// 0x95f68
void network_webstats_update_upload_state(void)
{
  if (g_webstats_enabled && !update_lsp() && g_webstats_globals.upload_state != 0) {
    upload_failed(false);
  }
}

// 0x3a7798
void network_webstats_initialize(void)
{
    g_webstats_enabled = true;
    g_webstats_globals.upload_queued = false;
    g_webstats_globals.upload_queue_count = 0;
    memset(&g_webstats_globals.upload_file, 0, sizeof(g_webstats_globals.upload_file));
    g_webstats_globals.upload_state = 0;
    g_webstats_globals.server_token = -1;
    g_webstats_globals.failure_count = 0;
    g_webstats_globals.step_timeout_seconds = 0;
    g_webstats_globals.step_deadline_tick = 0;
}

// 0x3a7840
void network_webstats_dispose(void)
{
    g_webstats_enabled = false;
}

static __forceinline bool network_webstats_runtime_data_compress(const void *buffer, unsigned long buffer_size,
    s_webstats_upload_buffer *destination, unsigned long *destination_size, unsigned long capacity,
    e_zlib_compression_level compression_level, unsigned long scratch_size, void *scratch_buffer)
{
    bool success = false;
    if (capacity >= 4) {
        s_memory_pool *pool = fixed_memory_pool_new("zlib pool", scratch_buffer, scratch_size, _memory_pool_callback_list_none);
        if (pool) {
            s_blam_zlib_memory memory;
            blam_zlib_initialize(pool, &memory);
            destination->uncompressed_size = buffer_size;
            *destination_size = capacity - 4;
            z_stream_s stream;
            stream.next_in = (unsigned char *)buffer;
            stream.avail_in = buffer_size;
            stream.next_out = (unsigned char *)destination->compressed_data;
            stream.avail_out = *destination_size;
            if (compress3(destination_size, compression_level, &memory, &stream) == 0) {
                *destination_size += 4;
                success = true;
            }
            blam_zlib_dispose(&memory);
            fixed_memory_pool_free(pool);
        }
    }
    return success;
}

bool network_webstats_submit_buffer(e_online_lsp_service_type service_type, const void *buffer,
    long buffer_size, const char *content_type, long timeout)
{
    bool success = false;
    if (g_webstats_enabled) {
        if (g_webstats_globals.upload_state == 0 && !g_webstats_globals.upload_queued) {
            long scratch_size = 307200;
            char scratch_buffer[307200];
            if (network_webstats_runtime_data_compress(buffer, buffer_size, &g_webstats_globals.upload_buffer,
                (unsigned long *)&g_webstats_globals.upload_queue_count, 40952, _zlib_compression_level_fast, scratch_size, scratch_buffer)) {
                c_http_post_source source;
                s_webstats_upload_buffer *upload = &g_webstats_globals.upload_buffer;
                upload->size = g_webstats_globals.upload_queue_count + 8;
                upload->version = 1;
                g_webstats_globals.upload_queue_count += 8;
                source.set_source_as_buffer((const char *)upload, g_webstats_globals.upload_queue_count);
                source.set_content_type(content_type);
                g_webstats_globals.http_stream.set_source(&source);
                g_webstats_globals.failure_callback = 0;
                g_webstats_globals.failure_count = 0;
                g_webstats_globals.step_timeout_seconds = -1;
                g_webstats_globals.step_deadline_tick = -1;
                g_webstats_globals.service_type = 3;
                g_webstats_globals.upload_state = 1;
                success = true;
            } else {
                success = true;
            }
        }
    } else {
        success = true;
    }
    return success;
}

// 0x3a7ba8
void do_uploading_work(void)
{
    s_upload_response response;
    char buffer[0x400];
    response.complete = false;
    response.length = sizeof(buffer);
    if (!g_webstats_globals.http_client.do_work(&response.complete, buffer, &response.length, 0))
    {
        upload_failed(false);
        return;
    }
    if (upload_step_is_taking_too_long()) {
        upload_failed(false);
        return;
    }
    if (!response.complete)
        return;
    if (response.length != 4 || strncmp(buffer, "DONE", 4) != 0) {
        upload_failed(false);
        return;
    }
    upload_complete();
    if (g_webstats_globals.server_token != -1) {
        g_online_lsp_manager.disconnect_from_server(g_webstats_globals.server_token, true);
        g_webstats_globals.server_token = -1;
    }
    if (g_webstats_globals.failure_callback) {
        g_webstats_globals.failure_callback(1);
        g_webstats_globals.failure_callback = 0;
    }
}

// 0x3a7ce0
void upload_failed(bool immediate)
{
  g_webstats_globals.failure_count++;
  g_webstats_globals.http_client.stop();
  if (g_webstats_globals.server_token != -1) {
    g_online_lsp_manager.disconnect_from_server(g_webstats_globals.server_token, false);
    g_webstats_globals.server_token = -1;
  }
  if (upload_step_is_taking_too_long()) {
    unsigned int deadline;
    if (g_webstats_globals.step_timeout_seconds == -1) {
      deadline = 0xffffffff;
    } else {
      unsigned int now = GetTickCount();
      deadline = g_webstats_globals.step_timeout_seconds * 1000 + now;
    }
    g_webstats_globals.step_deadline_tick = deadline;
  }
  if (immediate) {
    upload_complete();
  } else if (g_webstats_globals.failure_count >= 3) {
    upload_complete();
    if (g_webstats_globals.failure_callback) {
      g_webstats_globals.failure_callback(0);
      g_webstats_globals.failure_callback = 0;
    }
  }
}

void upload_complete(void)
{
  if (g_webstats_globals.upload_state == 4) {
    file_close(&g_webstats_globals.upload_file);
  }
  g_webstats_globals.upload_state = 0;
}

bool upload_step_is_taking_too_long(void)
{
  return (g_webstats_globals.step_deadline_tick != 0xffffffff) && (GetTickCount() >= g_webstats_globals.step_deadline_tick);
}

// @address 0x3a7a58 ?update_lsp@@YA_NXZ
bool update_lsp(void)
{
    bool success = false;
    if (g_webstats_globals.upload_state != 0) {
        if (g_webstats_globals.upload_state == 1 || g_webstats_globals.upload_state == 4) {
            if (upload_step_is_taking_too_long()) {
                upload_failed(false);
            } else if (g_webstats_globals.server_token == -1) {
                bool acquired = false;
                long ip_address;
                unsigned short port;
                e_online_lsp_server_acquire_result result = g_online_lsp_manager.acquire_server(
                    (e_online_lsp_service_type)g_webstats_globals.service_type,
                    &g_webstats_globals.server_token, &ip_address, &port, "webstats");
                if (result == acquire_pending) {
                    acquired = true;
                } else if (result == acquire_success) {
                    if (g_webstats_globals.http_client.start(&g_webstats_globals.http_stream,
                        ip_address, port, "/upload_server/stats.ashx", false)) {
                        acquired = true;
                    }
                }
                if (!acquired) {
                    upload_failed(false);
                }
            } else {
                bool connected = g_webstats_globals.http_client.online_files_is_connected();
                if (connected) {
                    g_online_lsp_manager.server_connected(g_webstats_globals.server_token);
                }
                do_uploading_work();
            }
        }
        success = true;
    }
    return success;
}
