// networkinglibxbox/network_debug_dump.obj: from the records 0x3a80f8 0x3a8400 0x3a8980 0x3a8b70 0x3a9300 0x3a9368
// @flags /TP /O2 /Oc
#define NOD3D
#include "multithreading/synchronization_types.h"
#include "multithreading/synchronized_value.h"
#include "multithreading/threads.h"
// @address 0x949aa8 g_thread_should_exit
#include "multithreading/thread_tls.h"
#include "cseries/crt_xdk.h"
#include "networking/transport/transport_globals.h"
#include "memory/static_arrays.h"
#include "networking/network_debug_dump.h"
#include <stdio.h>
// @address 0x3a8400 network_debug_dump_initialize_system_name
// @address 0x3a8b70 ?compute_task_file_upload_signatures@@YAXPAUs_netdebug_upload_task@@@Z
// @address 0x3a9300 ?print@?$c_static_string@$0CB@@@QAAPBDPBDZZ
// @address 0x3a9368 ?append@?$c_static_string@$0EB@@@QAAPBDPBD@Z
// @address 0x183ef0 ?crc_checksum_buffer@@YAKKPBXJ@Z
// @address 0x1858b8 ?csnzprintf@@YAPADPADIPBDZZ
// @address 0x1adbf8 ?file_open@@YA_NPAUs_file_reference@@KPAK@Z
// @address 0x1ae008 ?file_close@@YA_NPAUs_file_reference@@@Z
// @address 0x1ae220 ?file_read@@YA_NPAUs_file_reference@@K_NPAX@Z
// @address 0x1ae380 ?file_get_size@@YA_NPBUs_file_reference@@PAK@Z
// @address 0x82370 _vsnprintf
// @address 0x8f688 XNetGetTitleXnAddr
// @address 0x5a01a8 XUserGetName
#include <string.h>
#include <stdarg.h>
#include "multithreading/synchronization.h"
#include "tag_files/files_windows.h"
#include "memory/crc.h"
#include "networking/online/online_files.h"
#include "networking/tools/network_debug_dump.h"
#include "cseries/cseries.h"
#include "networking/tools/http_stream_types.h"
#include "networking/tools/http_client.h"
#include "networking/online/online_lsp_manager.h"
#include "networking/online/online_lsp_service_type.h"

#include "tag_files/file_reference.h"
#include <ppcintrinsics.h>

extern "C" {
// @address 0xf24a50 g_netdebug_globals
s_netdebug_globals g_netdebug_globals;
}

extern "C" void network_debug_dump_initialize_system_name(void)
{
    if (strncmp(g_netdebug_globals.system_name, "unknown_system", 160) == 0)
    {
        int found = 0;
        int controller_index = 0;
        g_netdebug_globals.system_name[0] = 'p';
        g_netdebug_globals.system_name[1] = 'n';
        g_netdebug_globals.system_name[2] = '@';
        do {
            if (XUserGetName(controller_index, &g_netdebug_globals.system_name[3], 157) == 0)
            {
                found = 1;
                break;
            }
            controller_index++;
        } while (controller_index < 4);

        if (!((unsigned char)found))
        {
            unsigned char xnaddr[36];
            xnaddr[0] = (unsigned char)found;
            memset(&xnaddr[1], 0, sizeof(xnaddr) - 1);
            XNetGetTitleXnAddr((XNADDR *)xnaddr);
            csnzprintf(g_netdebug_globals.system_name, 160, "%02X%02X%02X%02X%02X%02X",
                xnaddr[10],
                xnaddr[11],
                xnaddr[12],
                xnaddr[13],
                xnaddr[14],
                xnaddr[15]);
        }
    }
}

void compute_task_file_upload_signatures(s_netdebug_upload_task *task)
{
    unsigned long file_size = 0;
    bool success = true;
    if (file_open(&task->file, 1, &file_size))
    {
        file_size = 0;
        file_get_size(&task->file, &file_size);
        unsigned long bytes_remaining = file_size;
        task->file_crc = 0xFFFFFFFF;
        char buffer[1024];
        while (success && bytes_remaining != 0)
        {
            unsigned long bytes_to_read = bytes_remaining > sizeof(buffer) ? sizeof(buffer) : bytes_remaining;
            success = file_read(&task->file, bytes_to_read, true, buffer);
            if (success)
            {
                task->file_crc = crc_checksum_buffer(task->file_crc, buffer, bytes_to_read);
                bytes_remaining -= bytes_to_read;
            }
        }
        if (!success)
            task->file_crc = 0;
        file_close(&task->file);
    }
}

template<long k_maximum_count>
const char *c_static_string<k_maximum_count>::print(const char *format, ...)
{
    va_list args;
    va_start(args, format);
    _vsnprintf(m_string, k_maximum_count - 1, format, args);
    m_string[k_maximum_count - 1] = 0;
    return m_string;
}
template const char *c_static_string<33>::print(const char *format, ...);

template<> const char *c_static_string<65>::append(const char *source)
{
    unsigned int length = csstrnlen(m_string, 65);
    strncpy(m_string + length, source, 65 - length);
    m_string[64] = '\0';
    return m_string;
}

// @address 0x3a8980 network_debug_dump_handle_upload_task
extern "C" int network_debug_dump_handle_upload_task(c_http_client *client, c_http_stream *stream, long unused, s_netdebug_upload_task *task)
{
    int acquire_result;
    unsigned short server_port;
    long server_address;
    unsigned long deadline;
    c_http_stream *upload_stream;
    long stream_length;
    unsigned char completed;
    unsigned char available;
    int success;
    long connection_token;
    union { unsigned short port; bool completed; } status;
    long ip_address;
    long acquired_token;
    success = 0;
    available = transport_startup_state.initialized && transport_startup_state.started;
    if (available)
    {
        ip_address = success;
        status.port = (unsigned short)success;
        do
        {
            acquire_result = c_online_lsp_manager::get()->acquire_server(
                online_lsp_service_netdebug, &acquired_token, &ip_address, &status.port, "crash upload");
            Sleep(0);
        } while (acquire_result == 0);
        if (acquire_result == 1)
        {
            server_port = status.port;
            server_address = ip_address;
            if (client->start(stream, server_address, server_port, "/upload_server/upload.ashx", true)) {
                status.completed = false;
                deadline = GetTickCount() + 1800000;
                connection_token = acquired_token;
                do
                {
                    if (GetTickCount() >= deadline)
                        break;
                    if (!client->do_work(&status.completed, 0, 0, 0))
                    {
                        client->stop();
                        break;
                    }
                    if (task->progress_callback)
                    {
                        stream_length = client->get_upload_length();
                        task->progress_callback(client->get_upload_position(), stream_length);
                    }
                    completed = status.completed;
                    if (completed)
                    {
                        c_online_lsp_manager::get()->disconnect_from_server(connection_token, true);
                        connection_token = -1;
                        success = 1;
                    }
                    if (client->is_connected())
                        c_online_lsp_manager::get()->server_connected(connection_token);
                    Sleep(0);
                } while (!completed);
                if (GetTickCount() >= deadline)
                    client->stop();
            } else {
                connection_token = acquired_token;
            }
            if (!(unsigned char)success)
                c_online_lsp_manager::get()->disconnect_from_server(connection_token, false);
        }
    }
    return success;
}

extern "C" {
// @address 0xf283c0 g_network_debug_dump_thread_done
c_synchronized_long g_network_debug_dump_thread_done = 1;

}

// @address 0x3a80f8 network_debug_dump_update_upload_queue
extern "C" int network_debug_dump_update_upload_queue()
{
    g_network_debug_dump_thread_done.set(0);
    while (g_thread_should_exit[get_current_thread_index()] != 1)
    {
        if (!g_netdebug_globals.active_task.active)
        {
            internal_critical_section_enter(4);
            if (!g_netdebug_globals.active_task.active)
            {
                for (long task_index = 0; task_index < 10; ++task_index)
                {
                    if (g_netdebug_globals.requests[task_index].active)
                    {
                        s_netdebug_upload_task *task = &g_netdebug_globals.requests[task_index];
                        g_netdebug_globals.active_task = *task;
                        g_netdebug_globals.retry_count = 0;
                        memset(task, 0, sizeof(*task));
                        task->active = false;
                        break;
                    }
                }
                if (g_netdebug_globals.active_task.active)
                    compute_task_file_upload_signatures(&g_netdebug_globals.active_task);
            }
            s_critical_section_handle *lock = &g_synch_globals.critical_sections[4];
            _InterlockedDecrement(&lock->lock_count.value);
            RtlLeaveCriticalSection(&lock->critical_section);
        }
        else
        {
            if ((unsigned char)netdebug_upload_file(&g_netdebug_globals.active_task))
            {
                if (g_netdebug_globals.active_task.completion_callback)
                    g_netdebug_globals.active_task.completion_callback(true, g_netdebug_globals.active_task.callback_data);
            }
            else
            {
                if (++g_netdebug_globals.retry_count < 3)
                {
                    Sleep(1000);
                    continue;
                }
                if (g_netdebug_globals.active_task.completion_callback)
                    g_netdebug_globals.active_task.completion_callback(false, g_netdebug_globals.active_task.callback_data);
            }
            memset(&g_netdebug_globals.active_task, 0, sizeof(g_netdebug_globals.active_task));
        }
        Sleep(1000);
    }
    g_network_debug_dump_thread_done.set(1);
    return 0;
}
