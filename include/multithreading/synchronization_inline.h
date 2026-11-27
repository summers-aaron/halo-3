#ifndef MULTITHREADING_SYNCHRONIZATION_INLINE_H
#define MULTITHREADING_SYNCHRONIZATION_INLINE_H

#include <ppcintrinsics.h>
#include "cseries/crt_xdk.h"
#include "multithreading/synchronization.h"
#include "multithreading/synchronization_types.h"

namespace {

static __forceinline void internal_critical_section_leave(long index)
{
    if (index != -1)
    {
        _InterlockedDecrement(&g_synch_globals.critical_sections[index].lock_count.value);
        RtlLeaveCriticalSection(&g_synch_globals.critical_sections[index].critical_section);
    }
}
}


#endif
