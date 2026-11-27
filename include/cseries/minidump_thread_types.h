#ifndef CSERIES_MINIDUMP_THREAD_TYPES_H
#define CSERIES_MINIDUMP_THREAD_TYPES_H

#include <stddef.h>

struct s_minidump_memory_range
{
    unsigned long base;
    unsigned long size;
};

// The thread context record the dump stores: the XDK CONTEXT (winnt.h), 0xa40 bytes.
struct s_minidump_writer_context
{
    unsigned long context_flags;
    unsigned long msr;
    unsigned long iar;
    unsigned long lr;
    unsigned __int64 ctr;
    unsigned __int64 gpr[32];
    unsigned long cr;
    unsigned long xer;
    double fpscr;
    double fpr[32];
    unsigned long user_mode_control;
    unsigned long fill;
    float vscr[4];
    float vr[128][4];
};
typedef char s_minidump_writer_context_size[(sizeof(s_minidump_writer_context) == 0xa40) ? 1 : -1];

struct s_thread_information
{
    unsigned long thread_id;
    unsigned long suspend_count;
    unsigned long priority_class;
    unsigned long priority;
    unsigned __int64 teb;
    unsigned __int64 stack_start;
    unsigned long stack_data_size;
    unsigned long stack_rva;
    unsigned long context_data_size;
    unsigned long context_rva;
    s_minidump_writer_context context;
    // The producer computes signed 32-bit stack bounds before serializing them.
    struct s_stack_range { int base; int size; } stack;
};

typedef char minidump_thread_context_offset[(offsetof(s_thread_information, context) == 0x30) ? 1 : -1];
typedef char minidump_thread_stack_offset[(offsetof(s_thread_information, stack) == 0xa70) ? 1 : -1];
typedef char minidump_thread_stack_pointer_offset[(offsetof(s_minidump_writer_context, gpr) + sizeof(unsigned __int64) == 0x20) ? 1 : -1];

#endif
