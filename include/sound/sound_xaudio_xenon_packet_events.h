#ifndef SOUND_SOUND_XAUDIO_XENON_PACKET_EVENTS_H
#define SOUND_SOUND_XAUDIO_XENON_PACKET_EVENTS_H

#include <xtl.h>
#include <xaudio.h>
#include <ppcintrinsics.h>
#include <stddef.h>

struct __declspec(align(16)) s_sound_xaudio_packet_completion_entry
{
    SLIST_ENTRY link;
    XAUDIOVOICEPACKETCALLBACK callback;
    void *context;
};

struct s_sound_xaudio_packet_completion_queue
{
    s_sound_xaudio_packet_completion_entry entries[392];
    SLIST_HEADER pending;
    SLIST_HEADER free;
    bool initialized;
    long entry_limit;
    const char *name;
};

extern "C" { extern s_sound_xaudio_packet_completion_queue g_xaudio_packet_completion_queue; }

// March 82aba2d0 writes the caller context at entry +0x10.
static __forceinline void *sound_xaudio_xenon_allocate_packet_completion_event(void *context)
{
    s_sound_xaudio_packet_completion_entry *event =
        (s_sound_xaudio_packet_completion_entry *)InterlockedPopEntrySList(&g_xaudio_packet_completion_queue.free);
    __lwsync();
    if (event)
        event->context = context;
    return event;
}

#if defined(_M_PPCBE) || defined(_XBOX)
typedef char sound_xaudio_packet_completion_entry_size[(sizeof(void *) != 4 || (sizeof(s_sound_xaudio_packet_completion_entry) == 32)) ? 1 : -1];
typedef char sound_xaudio_packet_completion_context_offset[(sizeof(void *) != 4 || (offsetof(s_sound_xaudio_packet_completion_entry, context) == 16)) ? 1 : -1];
typedef char sound_xaudio_packet_completion_free_offset[(sizeof(void *) != 4 || (offsetof(s_sound_xaudio_packet_completion_queue, free) == 0x3108)) ? 1 : -1];
typedef char sound_xaudio_packet_completion_name_offset[(sizeof(void *) != 4 || (offsetof(s_sound_xaudio_packet_completion_queue, name) == 0x3118)) ? 1 : -1];
#endif

#endif // SOUND_SOUND_XAUDIO_XENON_PACKET_EVENTS_H
