#ifndef RASTERIZER_RASTERIZER_TIMING_TYPES_H
#define RASTERIZER_RASTERIZER_TIMING_TYPES_H

#include <stddef.h>

// The CRT shim deliberately omits Windows headers. Keep the SDK's list ABI
// available to timing consumers which use that shim.
#ifndef _WINNT_
#if defined(_WIN64)
struct __declspec(align(16)) _SLIST_ENTRY
{
    _SLIST_ENTRY *Next;
};
typedef _SLIST_ENTRY SLIST_ENTRY;
struct __declspec(align(16)) _SLIST_HEADER
{
    unsigned __int64 Alignment;
    unsigned __int64 Region;
};
typedef _SLIST_HEADER SLIST_HEADER;
#else
struct _SINGLE_LIST_ENTRY
{
    _SINGLE_LIST_ENTRY *Next;
};
typedef _SINGLE_LIST_ENTRY SLIST_ENTRY;
union _SLIST_HEADER
{
    unsigned __int64 Alignment;
    struct
    {
        SLIST_ENTRY Next;
        unsigned short Depth;
        unsigned short Sequence;
    };
};
typedef _SLIST_HEADER SLIST_HEADER;
#endif
#endif

enum {
    k_timing_record_count = 45,
    k_present_record_count = 4
};

// One frame-timing record: the lock-free list link first (records move between the free, pending and presented
// SLists), copied whole (96 bytes) into the presented list by the vblank callback.
struct s_rasterizer_timing_record {
    SLIST_ENTRY link;
    __int64 input_collection_milliseconds;
    __int64 input_adjustment_time;
    __int64 synchronization_milliseconds;
    __int64 last_system_milliseconds;
    __int64 render_start_milliseconds;
    __int64 render_end_milliseconds;
    __int64 render_present_milliseconds;
    __int64 milliseconds;
    __int64 vblank_index;
    __int64 render_present_vblank_delta;
    __int64 vblank_delta;
};

struct s_rasterizer_timing_globals {
    s_rasterizer_timing_record last_presented_record;
    s_rasterizer_timing_record records[k_timing_record_count];
    SLIST_HEADER presented_records;
    SLIST_HEADER free_records;
    bool free_records_initialized;
    long free_record_count;
    __int64 pad; // unreferenced 8 bytes between free_record_count and present_records; March has no field records
    s_rasterizer_timing_record present_records[k_present_record_count];
    SLIST_HEADER present_pending_records;
    SLIST_HEADER present_pending_overflow_records;
    SLIST_HEADER present_free_records;
    bool present_free_initialized;
    s_rasterizer_timing_record current_record;
};

#if defined(_M_PPCBE) || defined(_XBOX)
typedef char timing_record_size[(sizeof(void *) != 4 || ((sizeof(s_rasterizer_timing_record) == 0x60))) ? 1 : -1];
typedef char timing_record_input_offset[(sizeof(void *) != 4 || ((offsetof(s_rasterizer_timing_record, input_collection_milliseconds) == 8))) ? 1 : -1];
typedef char timing_record_present_offset[(sizeof(void *) != 4 || ((offsetof(s_rasterizer_timing_record, render_present_milliseconds) == 0x38))) ? 1 : -1];
typedef char timing_globals_current_offset[(sizeof(void *) != 4 || ((offsetof(s_rasterizer_timing_globals, current_record) == 0x1300))) ? 1 : -1];
typedef char timing_globals_size[(sizeof(void *) != 4 || ((sizeof(s_rasterizer_timing_globals) == 0x1360))) ? 1 : -1];
#endif

#endif
