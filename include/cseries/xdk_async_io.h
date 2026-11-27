#ifndef CSERIES_XDK_ASYNC_IO_H
#define CSERIES_XDK_ASYNC_IO_H

// XDK winbase.h completion-port and worker-thread APIs.
struct _OVERLAPPED;
extern "C" {
#if defined(_WIN64)
int __stdcall GetQueuedCompletionStatus(void *completion_port, unsigned long *bytes_transferred,
    unsigned __int64 *completion_key, _OVERLAPPED **overlapped, unsigned long milliseconds);
#else
int __stdcall GetQueuedCompletionStatus(void *completion_port, unsigned long *bytes_transferred,
    unsigned long *completion_key, _OVERLAPPED **overlapped, unsigned long milliseconds);
#endif
int __stdcall ReadFile(void *file, void *buffer, unsigned long bytes_to_read,
    unsigned long *bytes_read, _OVERLAPPED *overlapped);
unsigned long __stdcall ResumeThread(void *thread);
}

#endif
