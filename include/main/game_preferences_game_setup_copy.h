#ifndef MAIN_GAME_PREFERENCES_GAME_SETUP_COPY_H
#define MAIN_GAME_PREFERENCES_GAME_SETUP_COPY_H

#include "main/game_preferences_game_setup.h"
#include "interface/gui_screen_pregame_lobby_mapeditor_types.h"
#include <string.h>

static __forceinline void global_preferences_get_last_game_setup(s_gui_game_setup_storage *last_game_setup)
{
    const s_gui_game_setup_storage *source = global_preferences_get_game_setup_storage();
    memcpy(last_game_setup, source, sizeof(s_gui_game_setup_storage));
}

#endif
