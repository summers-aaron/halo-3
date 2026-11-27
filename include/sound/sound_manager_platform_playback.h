#ifndef SOUND_SOUND_MANAGER_PLATFORM_PLAYBACK_H
#define SOUND_SOUND_MANAGER_PLATFORM_PLAYBACK_H

// Functions defined in tu/blamlibxbox/sound_manager_platform_playback.cpp.

struct s_platform_sound_playback_definition;

const s_platform_sound_playback_definition *game_sound_get_platform_playback_definition(long sound_index, long playback_index);
long game_sound_find_platform_playback_by_label(long name);
long sound_effect_collection_find_playback(long collection_index, long name);

#endif // SOUND_SOUND_MANAGER_PLATFORM_PLAYBACK_H
