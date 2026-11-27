#ifndef CSERIES_ASYNC_READ_CONTEXT_H
#define CSERIES_ASYNC_READ_CONTEXT_H

#include <stddef.h>
#include "multithreading/synchronized_value.h"

// ReadFileEx clients share the completion callback's first three words.
struct s_async_read_completion
{
    c_synchronized_long done;
    unsigned long error;
    unsigned long bytes_read;
    unsigned long requested_size;

    __forceinline s_async_read_completion() { done.set(0); }
};

// The OVERLAPPED prefix is followed by the client's completion record.
struct s_async_read_context
{
    unsigned long internal;
    unsigned long internal_high;
    unsigned long offset;
    unsigned long offset_high;
    s_async_read_completion *completion;
};

typedef char async_read_completion_size[(sizeof(s_async_read_completion) == 0x10) ? 1 : -1];
typedef char async_read_completion_requested_offset[(offsetof(s_async_read_completion, requested_size) == 0xc) ? 1 : -1];
typedef char async_read_context_size[(sizeof(void *) != 4 || ((sizeof(s_async_read_context) == 0x14))) ? 1 : -1];
typedef char async_read_context_completion_offset[(offsetof(s_async_read_context, completion) == 0x10) ? 1 : -1];

#endif
