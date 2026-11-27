#ifndef NETWORKING_ONLINE_VOICE_LIFECYCLE_H
#define NETWORKING_ONLINE_VOICE_LIFECYCLE_H

#include "networking/online/voice_interface_types.h"
#include "networking/online/voice_xenon.h"

static __forceinline e_voice_lifecycle voice_get_lifecycle()
{
    e_voice_lifecycle lifecycle = _voice_lifecycle_none;
    if (g_voice_interface_globals.initialized)
        lifecycle = g_voice_interface_globals.lifecycle;
    return lifecycle;
}

static __forceinline bool voice_lifecycle_valid()
{
    bool valid = false;
    if (g_voice_interface_globals.initialized)
        valid = voice_get_lifecycle() != _voice_lifecycle_none;
    return valid;
}

#endif // NETWORKING_ONLINE_VOICE_LIFECYCLE_H
