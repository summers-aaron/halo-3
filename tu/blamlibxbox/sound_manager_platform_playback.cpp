// blamlibxbox/sound_manager_platform_playback.obj: from the records 0xf8688 0x2879a8 0x287a70
// @flags /TP /O2 /Oc
#include "tag_files/tag_types.h"
#include "game/game_globals.h"
#include "sound/sound_manager_platform_playback.h"
#include "sound/sound_cache_file_definitions.h"
#include "cache/cache_file_globals.h"
#include "sound/sound_globals_reference_types.h"
#include "sound/sound_effect_collection.h"

// @address 0xf8688 ?game_sound_get_platform_playback_definition@@YAPBUs_platform_sound_playback_definition@@JJ@Z

#include "tag_files/tag_groups.h"
#include "scenario/scenario.h"
#include "scenario/scenario_tag.h"


__forceinline unsigned long playback_type(long index)
{
    return (unsigned long)index >> 30;
}

__forceinline long playback_element(long index)
{
    return index & 0x3fffffff;
}

const s_platform_sound_playback_definition *game_sound_get_platform_playback_definition(long sound_index, long playback_index)
{
    const s_platform_sound_playback_definition *result = 0;
    if (playback_index == -1)
    {
        if (sound_index != -1 && tag_get_group_tag(sound_index) == 'snd!')
        {
            s_cache_file_sound_definition *definition = TAG_GET('snd!', s_cache_file_sound_definition, sound_index);
            result = sound_definition_try_and_get_platform_playback(definition);
        }
    }
    else
    {
        unsigned long type = playback_type(playback_index);
        switch (type)
        {
        case 0:
            if (tag_get_group_tag(sound_index) == 'snd!')
            {
                s_cache_file_sound_definition *definition = TAG_GET('snd!', s_cache_file_sound_definition, sound_index);
                result = sound_definition_try_and_get_platform_playback(definition);
            }
            break;
        case 1:
        {
            s_game_globals *globals = global_game_globals;
            sound_globals_reference *sounds = globals->sound_globals.count() ? globals->sound_globals.begin() : 0;
            if (sounds && sounds->sound_effects.index != -1)
            {
                long element = playback_element(playback_index);
                sound_effect_collection_definition *definition = TAG_GET('sfx+', sound_effect_collection_definition, sounds->sound_effects.index);
                result = &definition->sound_effects[element].playback;
            }
            break;
        }
        case 2:
        {
            scenario *scenario_data = global_scenario;
            if (scenario_data->sound_effect_collection.index != -1)
            {
                long element = playback_element(playback_index);
                sound_effect_collection_definition *definition =
                    TAG_GET('sfx+', sound_effect_collection_definition, scenario_data->sound_effect_collection.index);
                result = &definition->sound_effects[element].playback;
            }
            break;
        }
        }
    }
    return result;
}

namespace {
static __forceinline long make_platform_playback_index(long type, long element)
{
    return (element & 0x3fffffff) | ((type << 30) & 0xc0000000);
}
}

long game_sound_find_platform_playback_by_label(long name)
{
    long result = -1;
    if (name != 0 && name != -1)
    {
        if (name == 243)
            result = make_platform_playback_index(3, 0);
        else
        {
            scenario *scenario_data = global_scenario;
            long element = sound_effect_collection_find_playback(scenario_data->sound_effect_collection.index, name);
            if (element != -1)
                result = make_platform_playback_index(2, element);
        }
        if (result == -1)
        {
            s_game_globals *globals = global_game_globals;
            sound_globals_reference *sounds = globals->sound_globals.count() ? globals->sound_globals.begin() : 0;
            if (sounds)
            {
                long element = sound_effect_collection_find_playback(sounds->sound_effects.index, name);
                if (element != -1)
                    result = make_platform_playback_index(1, element);
            }
        }
    }
    return result;
}

// @address 0x287a70 ?sound_effect_collection_find_playback@@YAJJJ@Z
long sound_effect_collection_find_playback(long collection_index, long name)
{
    long result = -1;
    if (collection_index != -1)
    {
        sound_effect_collection_definition *collection = TAG_GET('sfx+', sound_effect_collection_definition, collection_index);
        for (long index = 0; index < collection->sound_effects.count(); index++)
        {
            if (collection->sound_effects[index].name == name)
            {
                result = index;
                break;
            }
        }
    }
    return result;
}
