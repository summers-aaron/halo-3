#ifndef SOUND_SOUND_EFFECT_MANAGER_EXTERNS_H
#define SOUND_SOUND_EFFECT_MANAGER_EXTERNS_H

// match: 0x1a2fe0: noalias keeps g_sound_cache_file_gestalt in r28 across the call; without it the compiler emits an lwz r28 reload (+4B, RA cascade).
__declspec(noalias) long sound_playback_controller_find_add_reference(long identifier);

#endif
