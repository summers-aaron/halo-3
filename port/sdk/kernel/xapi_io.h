// port/sdk/kernel: what the shared Xbox file API (xapi_io.c) needs from each host's kernel (kernel_linux.c,
// win32_windows.c). Host code: no XDK headers.
#ifndef PORT_KERNEL_XAPI_IO_H
#define PORT_KERNEL_XAPI_IO_H

#ifdef __cplusplus
extern "C" {
#endif

#define PORT_ERROR_HANDLE_EOF 38ul

// Moves `bytes` at file offset `offset` to (write 0) or from (write 1) `buffer` and returns when the transfer is done,
// whichever way the handle was opened; the cache-file header hooks (map_swap.h) see it as they see ReadFile and
// WriteFile. Returns 0 or a Win32 error code (PORT_ERROR_HANDLE_EOF for a read that starts at or past the end of the
// file); *done receives the bytes moved. The file pointer is not used.
unsigned int port_file_transfer_at(void *handle, void *buffer, unsigned int bytes, unsigned long long offset,
    int write, unsigned int *done);

// Queues `routine(error, bytes, context)` as an asynchronous procedure call of the calling thread: it runs from that
// thread's next alertable wait (xdk_sleep_ex), after the calls queued before it. Returns 0 when the queue is full.
int port_apc_queue(void *routine, unsigned int error, unsigned int bytes, void *context);
// The same for the thread `thread` (a GetCurrentThreadId value), from any thread: a call that completes later on
// another thread (the keyboard UI, port/sdk/online/keyboard_ui.cpp) queues the routine to the thread that issued it.
int PORT_MS_ABI port_apc_queue_thread(unsigned int thread, void *routine, unsigned int error, unsigned int bytes, void *context);

#ifdef __cplusplus
}
#endif

#endif
