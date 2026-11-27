// ailibxbox/ai_mission_dialogue.obj: from the records 0x226898 0x567a88 0x567cc0
// @flags /TP /O2 /Oc
#include "memory/tls_globals.h"
#include "game/game_globals.h"
#include "tag_files/tag_types.h"
#include "ai/actor_datum.h"
// @address 0x226898 ?__tls_set_g_scenes_played_allocator@@YAXPAX@Z
// @address 0x567a88 mission_dialogue_play_line
// @address 0x567cc0 ai_mission_dialogue_try_and_get_variant
#include <stddef.h>
#include <stdlib.h>
#include "ai/ai_atom_scripting.h"
#include "ai/ai_mission_dialogue.h"
#include "cache/cache_files.h"
#include "scenario/scenario.h"


struct s_pvs;
struct s_zone_set;
struct s_scenario_soft_ceiling;
struct s_weapon_spawn_influence;
struct s_equipment_spawn_influence;

#include "scenario/scenario_tag.h"
#include "scenario/scenario_soft_ceiling_state.h"

#include "ai/mission_dialogue_variant.h"
#include "ai/mission_dialogue_types.h"
#include "ai/ai_globals_definition.h"
#include "ai/unit_dialogue_types.h"
#include "objects/object_header.h"
#include "units/unit_types.h"
#include "memory/data.h"
#include "objects/object_header_access.h"
#include "tag_files/tag_groups.h"

struct s_unit_dialogue_tag_definition
{
    s_tag_reference global_dialogue_info;
    unsigned long flags;
    s_tag_block vocalizations;
    long mission_dialogue_designator;
};


static __forceinline long ai_mission_dialogue_pick(long first, long second)
{
    return second != 0 && second != -1 ? second : first;
}

static __forceinline long ai_mission_dialogue_default_playback()
{
    long result = 0;
    s_game_globals *globals = global_game_globals;
    if (globals != 0 && globals->ai_globals.count() > 0)
    {
        s_ai_globals_definition *element = globals->ai_globals.begin();
        result = element->mission_dialogue_playback_label;
    }
    return result;
}

extern "C" bool mission_dialogue_play_line(long p0, long p1, bool p2, long p3, float *p4)
{
    bool result = false;
    scenario *scenario_data = global_scenario;
    if (p4 != 0)
        *p4 = 0.0f;
    if (scenario_data->ai_mission_dialogue.count > 0)
    {
        actor_datum *actor = datum_get<actor_datum>(tls_actor_data(), p0);
        if (actor->unit_index != -1)
        {
            long tag_index = ((s_tag_reference *)scenario_data->ai_mission_dialogue.address)->index;
            if (tag_index != -1)
            {
                ai_mission_dialogue *definition = (ai_mission_dialogue *)tag_get('mdlg', tag_index);
                long count = definition->lines.count();
                if (count > 0)
                {
                    mission_dialogue_line *lines = definition->lines.begin();
                    short i = 0;
                    do
                    {
                        mission_dialogue_line *line = &lines[i];
                        if (line->name == p3)
                        {
                            long playback = ai_mission_dialogue_pick(ai_mission_dialogue_default_playback(), line->playback_label);
                            unit_datum *object = (unit_datum *)object_get(actor->unit_index);
                            s_unit_dialogue *dialogue = (s_unit_dialogue *)object_header_block_get(object, &object->unit.dialogue_reference);
                            if (dialogue->dialogue_index != -1)
                            {
                                long fallback = dialogue->playback_label;
                                s_unit_dialogue_tag_definition *tag
                                    = (s_unit_dialogue_tag_definition *)tag_get('udlg', dialogue->dialogue_index);
                                playback = ai_mission_dialogue_pick(playback, fallback);
                                if (playback == -1 || playback == 0)
                                    playback = fallback;
                                long variant_count = line->variants.count();
                                for (short k = 0; k < variant_count; k++)
                                {
                                    s_ai_mission_dialogue_variant *variant = &line->variants.begin()[k];
                                    if (variant->id == tag->mission_dialogue_designator)
                                    {
                                        if (variant->sound.index != -1)
                                        {
                                            playback = ai_mission_dialogue_pick(playback, variant->playback_label);
                                            if (variant->playback_label != 0 && variant->playback_label != -1)
                                                playback = variant->playback_label;
                                            float duration = ai_scripting_play_sound_internal(p0, p1, p2, variant->sound.index, playback, 1.0f, 1.0f);
                                            if (p4 != 0)
                                                *p4 = duration;
                                            result = true;
                                            break;
                                        }
                                        break;
                                    }
                                }
                            }
                            break;
                        }
                        i = (short)(i + 1);
                    } while (i < count);
                }
            }
        }
    }
    return result;
}

// @address 0x11df884 g_scenes_played
__declspec(thread) unsigned long *g_scenes_played;

void __tls_set_g_scenes_played_allocator(void *allocator)
{
    TLS_SCENARIO_SOFT_CEILING_STATE = (unsigned long *)allocator;
}

s_ai_mission_dialogue_variant *ai_mission_dialogue_try_and_get_variant(long id, long sub_id, long *out)
{
    s_ai_mission_dialogue_variant *result = 0;
    scenario *scenario_data = global_scenario;
    if (scenario_data->ai_mission_dialogue.count > 0)
    {
        long tag_index = ((s_tag_reference *)scenario_data->ai_mission_dialogue.address)->index;
        if (tag_index != -1)
        {
            ai_mission_dialogue *definition = (ai_mission_dialogue *)tag_get('mdlg', tag_index);
            long count = definition->lines.count();
            if (count > 0)
            {
                mission_dialogue_line *lines = definition->lines.begin();
                short i = 0;
                do
                {
                    mission_dialogue_line *line = &lines[i];
                    if (line->name == id)
                    {
                        long variant_count = line->variants.count();
                        *out = line->playback_label;
                        if (variant_count > 0)
                        {
                            if (sub_id == -1 || sub_id == 0)
                            {
                                result = line->variants.begin();
                            }
                            else
                            {
                                for (short j = 0; j < variant_count; j++)
                                {
                                    s_ai_mission_dialogue_variant *variant = &line->variants.begin()[j];
                                    if (variant->id == sub_id)
                                    {
                                        result = variant;
                                        break;
                                    }
                                }
                            }
                            if (result != 0)
                            {
                                long value = result->playback_label;
                                if (value != 0 && value != -1)
                                    *out = value;
                            }
                        }
                        break;
                    }
                    i++;
                } while (i < count);
            }
        }
    }
    return result;
}
