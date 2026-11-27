// graphicslibxbox/rasterizer_globals.obj: from the records 0x4489e0 0x448b28 0x448d70 0x70c8e8
// @flags /TP /O2 /Oc
// @address 0x80608 GetTickCount

#include <xtl.h>
#include <ppcintrinsics.h>
#include <string.h>
#include "rasterizer/rasterizer_globals.h"
#include "rasterizer/xenon/rasterizer_xenon_main.h"
#include "multithreading/synchronization.h"
#include "multithreading/synchronization_types.h"

// @address 0x5a9a58 D3DDevice_SetVerticalBlankCallback

#include "rasterizer/rasterizer_timing_types.h"

#if defined(_M_PPCBE) || defined(_XBOX)
typedef char rasterizer_timing_record_size[(sizeof(void *) != 4 || (sizeof(s_rasterizer_timing_record) == 0x60)) ? 1 : -1];
typedef char rasterizer_timing_globals_free_records_offset[(sizeof(void *) != 4 || (offsetof(s_rasterizer_timing_globals, free_records) == 0x1148)) ? 1 : -1];
typedef char rasterizer_timing_globals_present_free_offset[
    (sizeof(void *) != 4 || (offsetof(s_rasterizer_timing_globals, present_free_records) == 0x12f0)) ? 1 : -1];
typedef char rasterizer_timing_globals_size[(sizeof(void *) != 4 || (sizeof(s_rasterizer_timing_globals) == 0x1360)) ? 1 : -1];
#endif

#include "rasterizer/rasterizer_render_globals_types.h"
#include "rasterizer/rasterizer.h"

// @address 0x73a240 ?global_z_near@@3MA
float global_z_near = 0.0078125f;

// @address 0x73a248 ?render_globals@c_rasterizer@@2Us_rasterizer_render_globals@@A
s_rasterizer_render_globals c_rasterizer::render_globals = { 1152, 640, 0, 0, 512 };

// @address 0x10ea1a0 ?g_rasterizer_timing_globals@@3Us_rasterizer_timing_globals@@A
s_rasterizer_timing_globals g_rasterizer_timing_globals;

// @address 0x4489e0 ?rasterizer_globals_initialize@@YAXXZ
void rasterizer_globals_initialize(void)
{
    memset(&rasterizer_globals, 0, sizeof(rasterizer_globals));
    rasterizer_globals.vblank_index.set(0);

    memset(&g_rasterizer_timing_globals, 0, sizeof(g_rasterizer_timing_globals));
    InitializeSListHead(&g_rasterizer_timing_globals.presented_records);
    InitializeSListHead(&g_rasterizer_timing_globals.free_records);
    s_rasterizer_timing_record *record = g_rasterizer_timing_globals.records;
    for (long count = 0; count < k_timing_record_count; count++, record++)
        record->link.Next = 0;
    g_rasterizer_timing_globals.free_records_initialized = true;
    g_rasterizer_timing_globals.free_record_count = k_timing_record_count;
    for (long index = k_timing_record_count - 1; index >= 0; index--) {
        __lwsync();
        InterlockedPushEntrySList(&g_rasterizer_timing_globals.free_records, &g_rasterizer_timing_globals.records[index].link);
    }

    InitializeSListHead(&g_rasterizer_timing_globals.present_pending_records);
    InitializeSListHead(&g_rasterizer_timing_globals.present_pending_overflow_records);
    InitializeSListHead(&g_rasterizer_timing_globals.present_free_records);
    record = g_rasterizer_timing_globals.present_records;
    for (long count = 0; count < k_present_record_count; count++, record++)
        record->link.Next = 0;
    __lwsync();
    record = g_rasterizer_timing_globals.present_records;
    for (long count = 0; count < k_present_record_count; count++, record++)
        InterlockedPushEntrySList(&g_rasterizer_timing_globals.present_free_records, &record->link);
    g_rasterizer_timing_globals.present_free_initialized = true;

    rasterizer_globals.presentation_interval = 5;
    rasterizer_globals.vblank_callback_installed = true;
    D3DDevice_SetVerticalBlankCallback(c_rasterizer::get_device(), (D3DVBLANKCALLBACK)rasterizer_vblank_callback);
}

// @address 0x448b28 ?rasterizer_globals_vblank_callback@@YAXK@Z
void rasterizer_globals_vblank_callback(unsigned long swap_index)
{
    long previous_swap_index = rasterizer_globals.swap_index.set(swap_index);
    rasterizer_globals.vblank_index.increment();
    SetEvent(g_synch_globals.events[3].handle);
    SetEvent(g_synch_globals.events[4].handle);
    if (swap_index == previous_swap_index)
        return;

    long delta = (long)(__int64)rasterizer_globals.vblank_index - (long)(__int64)rasterizer_globals.last_vblank_index;
    rasterizer_globals.last_vblank_index = rasterizer_globals.vblank_index;

    PSLIST_ENTRY entry = InterlockedFlushSList(&g_rasterizer_timing_globals.present_pending_records);
    __lwsync();
    if (!entry) {
        entry = InterlockedFlushSList(&g_rasterizer_timing_globals.present_pending_overflow_records);
        __lwsync();
    }
    if (entry) {
        // the flushed list is newest first: reverse it, keep the oldest record and push the others back
        PSLIST_ENTRY cursor = entry;
        PSLIST_ENTRY reversed = 0;
        while (cursor) {
            PSLIST_ENTRY next = cursor->Next;
            cursor->Next = reversed;
            reversed = cursor;
            cursor = next;
        }
        s_rasterizer_timing_record *record = CONTAINING_RECORD(reversed, s_rasterizer_timing_record, link);

        PSLIST_ENTRY rest = record->link.Next;
        while (rest) {
            PSLIST_ENTRY next = rest->Next;
            __lwsync();
            InterlockedPushEntrySList(&g_rasterizer_timing_globals.present_pending_records, rest);
            rest = next;
        }
        record->link.Next = 0;
        record->milliseconds = GetTickCount();
        record->vblank_delta = (long)(__int64)rasterizer_globals.vblank_index - (long)record->vblank_index;

        PSLIST_ENTRY free_entry = InterlockedPopEntrySList(&g_rasterizer_timing_globals.free_records);
        __lwsync();
        if (free_entry) {
            s_rasterizer_timing_record *free_record = CONTAINING_RECORD(free_entry, s_rasterizer_timing_record, link);
#ifdef HALO3_64BIT
{ __int64 * s = (__int64 *)record; __int64 *__restrict d = (__int64 *)free_record; for (long n = (long)(sizeof(*record) / sizeof(__int64)); n > 0; n--) { *d++ = *s++; } }
#else
// match: 0x448b28: pointer __restrict qualifier prevents alias reload
{ __int64 * s = (__int64 *)record; __int64 *__restrict d = (__int64 *)free_record; for (long n = 12; n > 0; n--) { *d++ = *s++; } }
#endif
            __lwsync();
            InterlockedPushEntrySList(&g_rasterizer_timing_globals.presented_records, &free_record->link);
        }
        __lwsync();
        InterlockedPushEntrySList(&g_rasterizer_timing_globals.present_free_records, &record->link);
    }

    if (delta > 0) {
        rasterizer_globals.frame_delta.set(delta);
        rasterizer_globals.frame_deltas[rasterizer_globals.frame_delta_index] = delta;
        rasterizer_globals.frame_delta_index = (rasterizer_globals.frame_delta_index + 1) % k_frame_delta_count;
    }
}

// @address 0x448d70 ?rasterizer_get_average_frame_delta@@YAMXZ
float rasterizer_get_average_frame_delta(void)
{
    float total = 0.0f;
    float count = 0.0f;
    long current = rasterizer_globals.frame_delta_index;
    short index = (short)((current + k_frame_delta_count - 1) % k_frame_delta_count);
    while (index != current) {
        total += (float)rasterizer_globals.frame_deltas[index];
        count += 1.0f;
        index = (short)((index + k_frame_delta_count - 1) % k_frame_delta_count);
    }
    if (count > 0.5f)
        total /= count;
    return total;
}

s_rasterizer_globals rasterizer_globals;

