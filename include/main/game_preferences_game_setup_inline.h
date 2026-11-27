#ifndef MAIN_GAME_PREFERENCES_GAME_SETUP_INLINE_H
#define MAIN_GAME_PREFERENCES_GAME_SETUP_INLINE_H

#include "main/game_preferences_game_setup.h"
#include <string.h>

// Copy the caller's complete setup layout without imposing constructor visibility.
template<typename t_game_setup>
static __forceinline void global_preferences_get_last_game_setup(t_game_setup *last_game_setup)
{
    const void *source = global_preferences_get_game_setup_storage();
    memcpy(last_game_setup, source, sizeof(t_game_setup));
}

#endif // MAIN_GAME_PREFERENCES_GAME_SETUP_INLINE_H
