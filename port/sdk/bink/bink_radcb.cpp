// port/sdk/bink (both hosts): the rest of radcb.obj, RAD's callback threads (the decompiled half is
// tu/binkxenon/radcb.cpp: the thread loop, registration, suspend / resume and freeing). Built like the game, against
// include/bink/radcb.h.
//
// A handler is one of eight s_callback_thread slots: a suspended thread running RAD_callback_thread every
// 1000 / rate ms (RADCB_resume_handler starts it), with its wake event and its mutex. Bink's sound tracks are
// serviced on one (bink_read.cpp: BinkOpen allocates it at -13, as the retail BinkOpen does: a negative rate is
// the same rate; it is 13 Hz, a 76 ms period, against Bink's 600 ms sound rings).
#include <xtl.h>

#include "bink/radcb.h"

namespace {

const int k_handler_count = 8;
s_callback_thread g_radcb_handlers[k_handler_count]; // retail 0x827ceea0
int g_radcb_handler_used[k_handler_count];          // retail 0x831df2c0

// RAD_callback_thread as a Win32 thread routine (WINAPI on the Windows build).
DWORD WINAPI radcb_thread_main(LPVOID parameter)
{
    return (DWORD)RAD_callback_thread((s_callback_thread *)parameter);
}

} // namespace

extern "C" {

// @address 0x712630 RADCB_allocate_handler
s_callback_thread *RADCB_allocate_handler(int rate)
{
    int slot = 0;
    while (slot < k_handler_count && g_radcb_handler_used[slot])
        slot++;
    if (slot == k_handler_count)
        return 0;
    g_radcb_handler_used[slot] = 1;
    s_callback_thread *thread = &g_radcb_handlers[slot];
    if (rate < 0)
        rate = -rate;
    if (rate == 0)
        rate = 1;
    thread->state = 0;
    thread->callbacks = 0;
    thread->iteration = 1;
    thread->callback_count = 0;
    thread->timeout = 1000 / rate;
    thread->thread_handle = CreateThread(0, 0x8000, radcb_thread_main, thread, CREATE_SUSPENDED, &thread->thread_id);
    if (thread->thread_handle == 0) {
        g_radcb_handler_used[slot] = 0;
        return 0;
    }
    XSetThreadProcessor(thread->thread_handle, 5);
    thread->mutex = CreateMutexA(0, 0, 0);
    thread->event = CreateEventA(0, 0, 0, 0);
    return thread;
}

// @address 0x712868 RADCB_release_handler_resources
// The kernel objects of a handler whose thread has exited (RADCB_free_2_handlers and do_unregister call this after
// waiting for it), and its slot.
void RADCB_release_handler_resources(s_callback_thread *thread)
{
    if (thread == 0)
        return;
    if (thread->mutex != 0)
        CloseHandle(thread->mutex);
    if (thread->thread_handle != 0)
        CloseHandle(thread->thread_handle);
    if (thread->event != 0)
        CloseHandle(thread->event);
    thread->mutex = 0;
    thread->thread_handle = 0;
    thread->event = 0;
    const int slot = (int)(thread - g_radcb_handlers);
    if (slot >= 0 && slot < k_handler_count)
        g_radcb_handler_used[slot] = 0;
}

} // extern "C"
