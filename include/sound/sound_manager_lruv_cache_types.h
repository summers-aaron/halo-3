#ifndef SOUND_SOUND_MANAGER_LRUV_CACHE_TYPES_H
#define SOUND_SOUND_MANAGER_LRUV_CACHE_TYPES_H

#include "memory/lruv_cache_types.h"

// The sound cache is a plain s_lruv_cache: tu/blamlibxbox/sound_manager.cpp
// assigns g_sound_cache_globals.lruv to s_lruv_cache * with no cast.
typedef s_lruv_cache s_sound_manager_lruv_cache;

#endif // SOUND_SOUND_MANAGER_LRUV_CACHE_TYPES_H
