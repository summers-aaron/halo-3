// port/sdk/kernel (both hosts): the Xbox's asynchronous file calls the game reaches under its own names.
//
//   xdk_read_file_ex (ReadFileEx), cache_files_windows_write_file_ex (WriteFileEx): an overlapped transfer at
//     OVERLAPPED::Offset / OffsetHigh whose completion routine runs as an asynchronous procedure call (APC) on the
//     thread that issued it, the next time that thread waits alertably;
//   xdk_sleep_ex (SleepEx): the alertable wait, which runs those completion routines;
//   xdk_set_file_information_by_handle (SetFileInformationByHandle).
//
// The transfer itself is done before the call returns (port_file_transfer_at, xapi_io.h); what stays asynchronous
// is what a caller can observe: the completion routine never runs inside the call, only from an alertable wait of
// the issuing thread, in issue order, with (error, bytes transferred, OVERLAPPED), and OVERLAPPED::Internal /
// InternalHigh hold the status and byte count, as the Xbox kernel leaves them. Every alertable wait the game makes
// is a SleepEx (its WaitForSingleObjectEx calls are not alertable), so SleepEx is where completions are delivered.
// Reads and writes of a cache-file header pass through the map swap hooks (map_swap.h) inside the host transfer.
#include "xapi_io.h"

typedef unsigned int port_dword;

// The kernel calls the game also sees (kernel_linux.c, or win32_windows.c and the forwarded thunks).
port_dword PORT_MS_ABI GetCurrentThreadId(void);
void PORT_MS_ABI SetLastError(port_dword error);
void PORT_MS_ABI Sleep(port_dword milliseconds);
int PORT_MS_ABI SetFileInformationByHandle(void *handle, int info_class, void *info, port_dword bytes);

#define PORT_ERROR_NOT_ENOUGH_MEMORY 8ul
#define PORT_ERROR_INVALID_PARAMETER 87ul
#define PORT_WAIT_IO_COMPLETION 0xc0ul
#define PORT_STATUS_SUCCESS 0ul
#define PORT_STATUS_END_OF_FILE 0xc0000011ul

// The game's completion routines: void (DWORD error, DWORD bytes, LPOVERLAPPED), ms_abi on x86_64.
typedef void (PORT_MS_ABI *port_completion_routine)(port_dword error, port_dword bytes, void *overlapped);

// OVERLAPPED: Internal, InternalHigh, Offset, OffsetHigh, hEvent (the game keeps its own pointer in hEvent).
typedef struct port_overlapped {
    port_dword internal, internal_high, offset, offset_high;
    void *event;
} port_overlapped;

typedef struct port_apc {
    port_dword thread;
    port_completion_routine routine;
    port_dword error, bytes;
    void *overlapped;
} port_apc;

// Completions queued and not yet delivered, oldest first. The game keeps a handful in flight per thread (the sound
// cache reader's request slots, one per async task), so a fixed table is ample; a full table fails the call with
// ERROR_NOT_ENOUGH_MEMORY before anything is transferred.
#define PORT_APC_CAPACITY 1024
static port_apc port_apcs[PORT_APC_CAPACITY];
static int port_apc_count;
static volatile char port_apc_busy;

static void port_apc_lock(void)
{
    while (__atomic_test_and_set(&port_apc_busy, __ATOMIC_ACQUIRE))
        __builtin_ia32_pause();
}
static void port_apc_unlock(void) { __atomic_clear(&port_apc_busy, __ATOMIC_RELEASE); }

static int port_apc_reserve(void)
{
    port_apc_lock();
    int ok = port_apc_count < PORT_APC_CAPACITY;
    if (ok)
        port_apcs[port_apc_count++] = (port_apc){0, 0, 0, 0, 0}; // a placeholder: no thread matches it until filled
    port_apc_unlock();
    return ok;
}

// Fills in the placeholder port_apc_reserve made (the oldest one), or drops it (routine 0).
static void port_apc_commit(port_completion_routine routine, port_dword error, port_dword bytes, void *overlapped)
{
    port_dword thread = GetCurrentThreadId();
    port_apc_lock();
    for (int i = 0; i < port_apc_count; i++) {
        if (port_apcs[i].thread != 0 || port_apcs[i].routine != 0)
            continue;
        if (routine == 0) {
            for (int j = i + 1; j < port_apc_count; j++)
                port_apcs[j - 1] = port_apcs[j];
            port_apc_count--;
        } else {
            port_apcs[i] = (port_apc){thread, routine, error, bytes, overlapped};
        }
        break;
    }
    port_apc_unlock();
}

// Runs the calling thread's queued completions in order; returns how many ran. A routine may issue more transfers
// (and so queue more completions); those run too, as Windows delivers APCs queued during an alertable wait.
static int port_apc_deliver(void)
{
    port_dword thread = GetCurrentThreadId();
    int delivered = 0;
    for (;;) {
        port_apc apc;
        int found = 0;
        port_apc_lock();
        for (int i = 0; i < port_apc_count; i++) {
            if (port_apcs[i].thread == thread) {
                apc = port_apcs[i];
                for (int j = i + 1; j < port_apc_count; j++)
                    port_apcs[j - 1] = port_apcs[j];
                port_apc_count--;
                found = 1;
                break;
            }
        }
        port_apc_unlock();
        if (!found)
            return delivered;
        apc.routine(apc.error, apc.bytes, apc.overlapped);
        delivered++;
    }
}

static int port_transfer_ex(void *handle, void *buffer, port_dword bytes, void *overlapped, void *routine, int write)
{
    port_overlapped *request = (port_overlapped *)overlapped;
    if (request == 0 || routine == 0) {
        SetLastError(PORT_ERROR_INVALID_PARAMETER);
        return 0;
    }
    if (!port_apc_reserve()) {
        SetLastError(PORT_ERROR_NOT_ENOUGH_MEMORY);
        return 0;
    }
    unsigned long long offset = (unsigned long long)request->offset_high << 32 | request->offset;
    port_dword done = 0;
    port_dword error = port_file_transfer_at(handle, buffer, bytes, offset, write, &done);
    if (error != 0) {
        // the transfer did not start (a read at or past the end of the file, a bad handle): the call fails and no
        // completion is queued, as ReadFileEx / WriteFileEx report errors they detect at once
        port_apc_commit(0, 0, 0, 0);
        request->internal = error == PORT_ERROR_HANDLE_EOF ? PORT_STATUS_END_OF_FILE : error;
        request->internal_high = 0;
        SetLastError(error);
        return 0;
    }
    request->internal = PORT_STATUS_SUCCESS;
    request->internal_high = done;
    port_apc_commit((port_completion_routine)routine, 0, done, overlapped);
    SetLastError(0);
    return 1;
}

int PORT_MS_ABI ReadFileEx(void *handle, void *buffer, port_dword size, void *context, void *completion_routine)
{
    return port_transfer_ex(handle, buffer, size, context, completion_routine, 0);
}

int PORT_MS_ABI WriteFileEx(void *handle, const void *buffer, port_dword bytes, void *overlapped, void *completion)
{
    return port_transfer_ex(handle, (void *)buffer, bytes, overlapped, completion, 1);
}

port_dword PORT_MS_ABI SleepEx(port_dword milliseconds, int alertable)
{
    if (alertable && port_apc_deliver() != 0)
        return PORT_WAIT_IO_COMPLETION;
    Sleep(milliseconds);
    if (alertable && port_apc_deliver() != 0)
        return PORT_WAIT_IO_COMPLETION;
    return 0;
}

int PORT_MS_ABI xdk_read_file_ex(void *handle, void *buffer, port_dword size, void *context, void *completion_routine)
{
    return ReadFileEx(handle, buffer, size, context, completion_routine);
}

int PORT_MS_ABI cache_files_windows_write_file_ex(void *handle, void *buffer, port_dword bytes, void *overlapped, void *completion)
{
    return WriteFileEx(handle, buffer, bytes, overlapped, completion);
}

port_dword PORT_MS_ABI xdk_sleep_ex(port_dword milliseconds, int alertable)
{
    return SleepEx(milliseconds, alertable);
}

int PORT_MS_ABI xdk_set_file_information_by_handle(void *handle, long info_class, void *info, port_dword bytes)
{
    return SetFileInformationByHandle(handle, (int)info_class, info, bytes);
}

int port_apc_queue(void *routine, port_dword error, port_dword bytes, void *context)
{
    return port_apc_queue_thread(GetCurrentThreadId(), routine, error, bytes, context);
}

int PORT_MS_ABI port_apc_queue_thread(port_dword thread, void *routine, port_dword error, port_dword bytes, void *context)
{
    if (routine == 0 || thread == 0)
        return 0;
    port_apc_lock();
    const int queued = port_apc_count < PORT_APC_CAPACITY;
    if (queued)
        port_apcs[port_apc_count++] = (port_apc){thread, (port_completion_routine)routine, error, bytes, context};
    port_apc_unlock();
    return queued;
}
