#ifndef NETWORKING_ONLINE_VOICE_FILTER_H
#define NETWORKING_ONLINE_VOICE_FILTER_H

typedef long (*voice_queue_get_effect_version_proc)(void *effect, unsigned long *version, unsigned long *platform);
// @address 0x8c43e8 g_voice_queue_get_effect_version
#if defined(_MSC_VER) && !defined(__clang__)
extern "C" { extern voice_queue_get_effect_version_proc g_voice_queue_get_effect_version; }
#endif
// Other compilers: the cell is the Waves import view's slot 3 (no unit defines the name on the Xbox), bound by name
// in voice_queue.cpp, its only user.

#endif // NETWORKING_ONLINE_VOICE_FILTER_H
