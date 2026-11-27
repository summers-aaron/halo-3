#ifndef SOUND_SCRIPTED_SOUND_TIME_INLINE_H
#define SOUND_SCRIPTED_SOUND_TIME_INLINE_H
#include "game/game_time_inline.h"
#include "sound/game_sound.h"

static __forceinline long scripted_sound_time_explicit_hs_ticks(long definition_index, long hs_ticks)
{
    long game_ticks = scripted_sound_time_explicit_game_ticks(definition_index, hs_ticks);
    float seconds = (float)game_ticks * game_tick_length();
    return (long)(seconds * 30.0f + 0.5f);
}
#endif
