#ifndef HS_HS_THREAD_TYPES_H
#define HS_HS_THREAD_TYPES_H

enum e_hs_thread_type
{
    _hs_thread_type_script,
    _hs_thread_type_global_initialize,
    _hs_thread_type_console,
    _hs_thread_type_runtime_evaluate,
    _hs_thread_type_command_script
};

// A script thread: the datum of the "det hs thread" and "non-det hs thread" arrays (0x524 bytes). Names follow
// hs_runtime.cpp; March hs_node_gc deletes the threads whose type is _hs_thread_type_console.
struct hs_thread
{
    unsigned short identifier;
    short script_index;
    short cleanup_script_index;
    short padding6;
    long sleep_until;
    long previous_sleep_until;
    short stack_offset;
    short padding12;
    long result;
    long thread_index;
    unsigned char type;
    unsigned char flags;
    unsigned char flags2;
    unsigned char abort_combat_status;
    long ai_index;
    char stack[0x500];
};

typedef char hs_thread_result_offset[(((unsigned long)&((hs_thread *)0)->result) == 0x14) ? 1 : -1];
typedef char hs_thread_index_offset[(((unsigned long)&((hs_thread *)0)->thread_index) == 0x18) ? 1 : -1];
typedef char hs_thread_abort_offset[(((unsigned long)&((hs_thread *)0)->abort_combat_status) == 0x1f) ? 1 : -1];
typedef char hs_thread_ai_index_offset[(((unsigned long)&((hs_thread *)0)->ai_index) == 0x20) ? 1 : -1];
typedef char hs_thread_size[(sizeof(hs_thread) == 0x524) ? 1 : -1];

#endif // HS_HS_THREAD_TYPES_H
