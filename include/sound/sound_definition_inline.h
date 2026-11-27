#ifndef SOUND_SOUND_DEFINITION_INLINE_H
#define SOUND_SOUND_DEFINITION_INLINE_H
#include "sound/sound_cache_file_definitions.h"
#include "sound/sound_cache_file_gestalt.h"
#include "sound/sound_cache_file_gestalt_globals.h"

static __forceinline const s_sound_playback_parameters *sound_definition_get_playback_parameters(const s_cache_file_sound_definition *definition) {
    return &g_sound_cache_file_gestalt.data->playback_parameters[definition->playback_index];
}

static __forceinline float sound_definition_map_pitch(const s_cache_file_sound_definition *definition, float pitch, float scale)
{
    const s_sound_scale_modifiers *modifiers = &g_sound_cache_file_gestalt.data->scale_modifiers[definition->scale_index];
    float lower = (float)modifiers->pitch.lower;
    float upper = (float)modifiers->pitch.upper;
    return pitch + (lower + (upper - lower) * scale);
}
#endif
