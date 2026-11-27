#ifndef SHELL_SHELL_XENON_H
#define SHELL_SHELL_XENON_H

// Functions and data of tu/blamlibxbox/shell_xenon.cpp.

#include <stdlib.h>

// The XDK's XALLOC_ATTRIBUTES word, as the game's XMemAlloc/XMemFree hooks read it.
// cl.exe allocates bitfields from the most significant bit on PowerPC; little-endian clang (the PC port, which also
// defines _M_PPCBE so the XDK headers parse) allocates from bit 0, so it lists the fields in XALLOC_ATTRIBUTES order.
struct s_alloc_attributes {
    union {
        unsigned long value;
        struct {
#if defined(_M_PPCBE) && !defined(__clang__)
            unsigned long physical_memory : 1;
            unsigned long zero_initialize : 1;
            unsigned long memory_protect : 2;
            unsigned long alignment : 4;
            unsigned long allocator_id : 8;
            unsigned long fixed_size : 1;
            unsigned long must_succeed : 1;
            unsigned long heap_tracks_attributes : 1;
            unsigned long object_type : 13;
#else
            unsigned long object_type : 13;
            unsigned long heap_tracks_attributes : 1;
            unsigned long must_succeed : 1;
            unsigned long fixed_size : 1;
            unsigned long allocator_id : 8;
            unsigned long alignment : 4;
            unsigned long memory_protect : 2;
            unsigned long zero_initialize : 1;
            unsigned long physical_memory : 1;
#endif
        };
    };
};

typedef char shell_alloc_attributes_size[(sizeof(s_alloc_attributes) == 4) ? 1 : -1];

// A linear allocator the XMemAlloc hook serves one allocator id from (gui_custom_bitmap_storage_xenon sets it up).
struct s_allocator_hook
{
    unsigned char enabled;
    long allocator_id;
    uintptr_t base;
    long limit;
    long used;
};

struct shell_xenon_timing_globals { __int64 frequency; bool initialized; };

extern "C" {
extern s_allocator_hook g_shell_allocator_hook;
extern shell_xenon_timing_globals g_shell_timing_globals;
}

extern "C" void *shell_alloc_memory(unsigned long size, unsigned long attributes);
extern "C" void shell_free_memory(void *address, unsigned long attributes);
extern "C" void *shell_alloc_physical_memory(unsigned long size, s_alloc_attributes attributes);
extern "C" int shell_start_main_thread(void);
extern "C" int main(void);
extern "C" void shell_test_dvd_presence(void);
extern "C" void shell_dispose(void);
extern "C" unsigned char shell_initialize(long parameter);

class c_shell_physical_memory_stage;

extern "C" c_shell_physical_memory_stage g_shell_physical_memory_stage;
void shell_halt_on_pure_virtual_call();

#endif // SHELL_SHELL_XENON_H
