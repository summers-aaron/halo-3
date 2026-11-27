// blamlibxbox/sound_effect_manager.obj: from the records 0xf8550 0x12a0b8 0x1f9e48 0x1f9e68 0x1f9f28 0x1fa018 0x1fa140 0x1fa1a8 0x1fa4f8 0x1fa5c0 0x1fa630 0x1fa658 0x1fa680 0x1fa6b0 0x1fa770 0x1fa7d8 0x1fa918 0x1fa9f0 0x1faa70 0x1faad0
// @compiler xdk6130
#include "cseries/cseries_wrapped.h"
#include "sound/sound_source_types.h"
#include "sound/sound_cache_file_gestalt_globals.h"
#include "sound/sound_manager_runtime.h"
#include "sound/sound_manager.h"
#include "sound/sound_event_callbacks.h"
// @flags /TP /O2 /Oc
#include "memory/data_types.h"
#include <string.h>
#include "sound/game_sound.h"
#include "sound/sound_definitions.h"
#include "sound/sound_effect_manager.h"
#include "sound/sound_manager_platform_playback.h"
#include "memory/data.h"
#include "memory/data_handle_access.h"
#include "memory/datum_index.h"
#include "sound/game_sound_definitions.h"
#include "cache/cache_files.h"
#include "sound/sound_playback_controller.h"
#include "sound/sound_playback_controller_types.h"
#include "sound/sound_effect_collection.h"
#include "sound/game_sound_types.h"
#include "sound/game_looping_sound_data.h"
#include "sound/looping_sound_manager.h"
#include "sound/sound_cache_file_gestalt.h"
#include "sound/sound_cache_file_definitions.h"
#include "cache/cache_file_globals.h"
#include "sound/sound_data.h"
#include "sound/sound_effect_manager_externs.h"
#include "sound/sound_effect_manager_internals.h"
#include "tag_files/tag_groups.h"


void sound_effect_notify_origin_in_channel(long sound_index, long playback_index, long tag_index, void *origin)
{
    sound_effect_manager_origin *o = (sound_effect_manager_origin *)origin;
    if (o->handler == 0) return;
    if (o->handler->start_proc == 0) return;
    o->handler->start_proc(sound_index, playback_index, tag_index, origin);
}

void sound_effect_notify_origin_sound(long sound_index, const void *origin, long definition_index, long pitch_range_index,
    const s_cache_file_sound_permutation *permutation, float time)
{
    sound_effect_manager_origin *o = (sound_effect_manager_origin *)origin;
    if (o->handler == 0) return;
    if (o->handler->update_proc == 0) return;
    o->handler->update_proc(sound_index, origin, definition_index, pitch_range_index, permutation, time);
}

bool sound_effect_update_origin_sound_output_properties(long sound_index, long playback_index, const void *origin,
    s_sound_output_parameters *output)
{
    bool result = false;
    const sound_effect_manager_origin *o = (const sound_effect_manager_origin *)origin;
    if (o->handler != 0 && o->handler->spatialization_proc != 0)
        result = o->handler->spatialization_proc(sound_index, playback_index, origin, output);
    return result;
}

c_smart_data_array<s_sound_effect_datum> g_sound_effect_data;

// @address 0x1fa9f0 sound_effect_new
long sound_effect_new(const sound_effect_manager_definition *definition)
{
    long effect_index = -1;
    if (definition->components.count != 0 && (effect_index = ::datum_new(g_sound_effect_data)) != -1)
    {
        s_sound_effect_datum *effect = &((s_sound_effect_datum *)((s_data_array *)g_sound_effect_data)->data)[datum_index_to_absolute_index(effect_index)];
        effect->definition = definition;
        effect->controller_index = -1;
    }
    return effect_index;
}

void sound_effect_delete(long index)
{
    s_sound_effect_datum *datum = datum_get<s_sound_effect_datum>(((s_data_array *)g_sound_effect_data), index);
    if (datum->controller_index != -1)
    {
        sound_playback_controller_release_reference(datum->controller_index);
        datum->controller_index = -1;
    }
    ::datum_delete((s_data_array *)g_sound_effect_data, index);
}

static __forceinline const s_platform_sound_playback_definition *sound_effect_manager_game_sound_get_default_platform_playback_definition(long sound_index)
{
    const s_platform_sound_playback_definition *result = 0;
    if (sound_index != -1 && tag_get_group_tag(sound_index) == 'snd!') {
        s_cache_file_sound_definition *definition = (s_cache_file_sound_definition *)tag_get(0, sound_index);
        result = sound_definition_try_and_get_platform_playback(definition);
    }
    return result;
}
// @address 0x1faad0 sound_playback_has_sound_effect_components
bool sound_playback_has_sound_effect_components(long sound_index, long playback_index, const sound_effect_manager_definition **out_definition)
{
    bool result = false;
    const s_platform_sound_playback_definition *platform = game_sound_get_platform_playback_definition(sound_index, playback_index);
    const s_platform_sound_playback_definition *default_platform = sound_effect_manager_game_sound_get_default_platform_playback_definition(sound_index);
    const s_platform_sound_playback_definition *selected = 0;
    if (platform && platform->effects.count())
        selected = platform;
    else if (playback_index == -1 && default_platform && default_platform->effects.count())
        selected = default_platform;
    if (selected) {
        const sound_effect_manager_definition *effect = platform->effects.count() > 0 ? (sound_effect_manager_definition *)platform->effects.address : 0;
        if (effect) {
            result = effect->components.count > 0;
            *out_definition = effect;
        }
    }
    return result;
}

// @address 0x1f9f28 sound_effect_manager_stop_all
extern "C" void sound_effect_manager_stop_all(void)
{
    sound_effect_manager_iterator iterator;
    iterator.next_index = -1;
    iterator.index = -1;
    iterator.array = g_sound_effect_data;
    iterator.datum = ::data_iterator_next((s_data_iterator *)&iterator.array);
    if (iterator.datum)
    {
        do
        {
            int index = iterator.index;
            s_data_array *data = g_sound_effect_data;
            s_sound_effect_datum *effect = datum_get<s_sound_effect_datum>(data, index);
            int controller_index = effect->controller_index;
            if (controller_index != -1)
            {
                s_data_array *controllers = g_sound_playback_controller_data;
                c_sound_playback_controller *controller_data = (c_sound_playback_controller *)controllers->data;
                unsigned short datum_index = (unsigned short)controller_index;
                c_sound_playback_controller *controller = &controller_data[datum_index];
                controller->reference_count--;
                if (controller->reference_count == 0)
                    controller->flags &= 253;
                if (controller->reference_count == 0)
                    ::datum_delete(g_sound_playback_controller_data, controller_index);
                effect->controller_index = -1;
                data = g_sound_effect_data;
            }
            ::datum_delete(data, index);
            iterator.datum = ::data_iterator_next((s_data_iterator *)&iterator.array);
        } while (iterator.datum);
    }
}

// 0x1fa4f8
bool sound_effect_track_component_sound(long sound_index, long playback_index, const void *origin, s_sound_source *sound_source)
{
    long effect_index = ((const sound_effect_manager_origin *)origin)->effect_index;
    s_sound_effect_datum *effect = (s_sound_effect_datum *)datum_try_and_get_inline(g_sound_effect_data, effect_index);
    bool result = effect != 0 && (effect->flags & 1) == 0;
    if (result && effect != 0)
    {
        bool stopped = (effect->flags >> 4) & 1;
        if (!stopped)
            sound_effect_get_source(effect_index, sound_source);
    }
    return result;
}

// 0x1fa5c0
bool sound_effect_track_origin_sound(long sound_index, long playback_index, const void *origin, s_sound_source *sound_source)
{
    long effect_index = ((const sound_effect_manager_origin *)origin)->effect_index;
    const s_sound_effect_datum *effect = datum_get<s_sound_effect_datum>(((s_data_array *)g_sound_effect_data), effect_index);
    bool result = (effect->flags & 16) == 0;
    if (result)
        sound_effect_get_source(effect_index, sound_source);
    return result;
}

// 0x1fa6b0
void sound_effect_on_origin_sound_stop(long sound_index, long sound_datum_index, e_sound_event_stop_reason stop_reason)
{
    if (stop_reason != _sound_event_stop_reason_all)
    {
        sound_effect_manager_sound_datum *sound = datum_get<sound_effect_manager_sound_datum>(g_sound_data, sound_datum_index);
        s_sound_effect_datum *effect = datum_get<s_sound_effect_datum>(((s_data_array *)g_sound_effect_data), sound->effect_index);
        if (sound->tracker != 0 && sound->tracker->stop != 0)
            sound->tracker->stop(sound_index, sound_datum_index, stop_reason);
        if (stop_reason != _sound_event_stop_reason_ended)
        {
            effect->flags |= 2;
            ::sound_effect_stop_internal(sound->effect_index);
            return;
        }
        effect->flags &= 0xfffd;
        ::sound_effect_stop_internal(sound->effect_index);
    }
}

// 0x1fa770
void sound_effect_on_origin_sound_scripted_stop(long sound_index, long sound_datum_index)
{
    sound_effect_on_origin_sound_stop(sound_index, sound_datum_index, _sound_event_stop_reason_scripting);
    sound_effect_manager_sound_datum *sound = datum_get<sound_effect_manager_sound_datum>(g_sound_data, sound_datum_index);
    sound->callbacks = sound->tracker;
    memset(&sound->effect_index, 0, sizeof(sound->effect_index) + sizeof(sound->tracker));
}

// 0x1fa018
bool sound_effect_track_and_update_internal(long sound_effect_index)
{
    bool result = false;
    s_sound_effect_datum *sound_effect = datum_get<s_sound_effect_datum>(((s_data_array *)g_sound_effect_data), sound_effect_index);
    switch (sound_effect->source_type.m_value)
    {
    case _sound_effect_source_sound:
        if (!(sound_effect->flags & 8) && !(sound_effect->flags & 16))
        {
            long sound_index = sound_effect->sound_index;
            sound_effect_manager_sound_datum *sound = datum_get<sound_effect_manager_sound_datum>(g_sound_data, sound_index);
            c_flags<e_sound_datum_flag_bits, unsigned short, 11> track_flags;
            result = sound_track_and_update(sound_index, sound->tracker, &track_flags);
            if (track_flags.test(_sound_datum_flag_bit5))
                sound_effect->flags |= 8;
        }
        else
            result = true;
        break;
    case _sound_effect_source_impulse:
        return false;
    case _sound_effect_source_manual:
        result = !(sound_effect->flags & 16);
        break;
    default:
        break;
    }
    return result;
}

// 0x1fa7d8
static __forceinline float sound_effect_manager_sound_source_get_minimum_distance(const s_sound_source *sound_source, long sound_index)
{
    float result;
    if (sound_source->source_flags.test(_sound_source_minimum_distance_valid_bit))
        result = sound_source->minimum_distance;
    else
        result = sound_definition_get_minimum_distance(sound_index);
    return result;
}
static __forceinline float sound_effect_manager_sound_source_get_maximum_distance(const s_sound_source *sound_source, long sound_index)
{
    float result;
    if (sound_source->source_flags.test(_sound_source_maximum_distance_valid_bit))
        result = sound_source->maximum_distance;
    else
        result = sound_definition_get_maximum_distance(sound_index);
    return result;
}

void sound_effect_get_source(long sound_effect_index, s_sound_source *sound_source)
{
    s_sound_effect_datum *sound_effect = datum_get<s_sound_effect_datum>(((s_data_array *)g_sound_effect_data), sound_effect_index);
    switch (sound_effect->source_type)
    {
    case _sound_effect_source_sound:
    {
        sound_effect_manager_sound_datum *sound = datum_get<sound_effect_manager_sound_datum>(g_sound_data, sound_effect->get_identifier(sound_effect_index));
        if (sound_source != &sound->source)
        {
            memcpy(sound_source, &sound->source, sizeof(s_sound_source));
            if (sound_source->source_flags.test(_sound_source_bit8))
            {
                sound_source->minimum_distance = sound_effect_manager_sound_source_get_minimum_distance(&sound->source, sound->definition_index);
                sound_source->source_flags.set(_sound_source_minimum_distance_valid_bit, true);
                sound_source->maximum_distance = sound_effect_manager_sound_source_get_maximum_distance(&sound->source, sound->definition_index);
                sound_source->source_flags.set(_sound_source_maximum_distance_valid_bit, true);
                sound_source->source_flags.set(_sound_source_bit9, true);
            }
        }
        break;
    }
    case _sound_effect_source_impulse:
        game_looping_sound_get_source(sound_effect->get_identifier(sound_effect_index), sound_source);
        break;
    case _sound_effect_source_manual:
        sound_source->spatialization &= 0xf0;
        sound_source->spatialization &= 0x0f;
        sound_source->volume = sound_effect->manual_volume;
        sound_source->name = 0;
        break;
    default:
        // match: 0x1fa7d8: assume(0) omits cmplwi-3/bge upper range guard on the source_type dispatch
        __assume(0);
    }
}

// 0x1fa918
void sound_effect_set_track_data(long sound_effect_index, s_sound_impulse_definition *impulse_definition)
{
    s_sound_effect_datum *sound_effect = datum_get<s_sound_effect_datum>(((s_data_array *)g_sound_effect_data), sound_effect_index);
    impulse_definition->event_size = 48;
    s_sound_effect_track_data *track_data = &impulse_definition->track_data;
    if (impulse_definition->flags & (1 << _sound_impulse_tracking_valid_bit))
        track_data->callback = impulse_definition->callbacks;
    else
    {
        sound_effect->flags |= 8;
        track_data->callback = 0;
    }
    track_data->effect_index = sound_effect_index;
    impulse_definition->callbacks = &k_sound_effect_impulse_callbacks;
    impulse_definition->flags |= 1 << _sound_impulse_tracking_valid_bit;
    if (impulse_definition->flags & (1 << _sound_impulse_playback_valid_bit))
        sound_effect->controller_index = sound_playback_controller_find_add_reference(impulse_definition->controller_index);
    else
    {
        long identifier = create_sound_identifier(sound_effect_index, _sound_identifier_type_effect);
        sound_effect->controller_index = sound_playback_controller_find_or_create(identifier);
        if (sound_effect->controller_index != -1)
        {
            impulse_definition->controller_index = identifier;
            impulse_definition->flags |= 1 << _sound_impulse_playback_valid_bit;
        }
    }
}

// 0xf8550
long sound_effect_manager_create_impulse(long sound_index, s_sound_impulse_definition *impulse)
{
    long playback_index = (impulse->flags & 0x100) ? impulse->playback_index : -1;
    const sound_effect_manager_definition *effect_definition = 0;
    long effect_index = -1;
    long result;
    if (sound_playback_has_sound_effect_components(sound_index, playback_index, &effect_definition))
        effect_index = sound_effect_new(effect_definition);
    if (effect_index != -1)
    {
        const s_platform_sound_playback_definition *platform = game_sound_get_platform_playback_definition(sound_index, playback_index);
        if (platform && (platform->flags & 1))
            impulse->source.flags |= 0x100;
        sound_effect_set_track_data(effect_index, impulse);
        result = sound_impulse_new(sound_index, impulse, 0);
        if (result != -1)
        {
            s_sound_effect_datum *effect = datum_get<s_sound_effect_datum>(((s_data_array *)g_sound_effect_data), effect_index);
            effect->source_type = _sound_effect_source_sound;
            effect->sound_index = result;
            effect->source_data = (short)impulse->playback_flags;
            sound_effect_update_components_internal(effect_index, _sound_effect_refresh_start);
        }
        else
            sound_effect_delete(effect_index);
    }
    else
        result = sound_impulse_new(sound_index, impulse, 0);
    return result;
}

// @address 0x1fa140 ?sound_effect_stop_internal@@YAXJ@Z
void sound_effect_stop_internal(long effect_index)
{
    s_sound_effect_datum *effect = datum_get<s_sound_effect_datum>(((s_data_array *)g_sound_effect_data), effect_index);
    effect->flags |= 8;
    effect->flags |= 16;
    sound_effect_update_components_internal(effect_index, _sound_effect_refresh_stop);
    effect->flags |= 1;
}

static __forceinline void sound_effect_manager_set_scale_manual_internal(long effect_index, float scale)
{
    s_sound_effect_datum *effect = datum_get<s_sound_effect_datum>(((s_data_array *)g_sound_effect_data), effect_index);
    effect->manual_volume = scale;
}
// @address 0x1f9e68 ?sound_effect_manager_create_manual@@YAJJM@Z
long sound_effect_manager_create_manual(long playback_index, float scale)
{
    long effect_index = -1;
    const sound_effect_manager_definition *definition = 0;
    if (sound_playback_has_sound_effect_components(-1, playback_index, &definition))
    {
        effect_index = sound_effect_new(definition);
        if (effect_index != -1)
        {
            s_sound_effect_datum *effect = datum_get<s_sound_effect_datum>(((s_data_array *)g_sound_effect_data), effect_index);
            effect->source_type = _sound_effect_source_manual;
            sound_effect_manager_set_scale_manual_internal(effect_index, scale);
            effect->source_data = 8;
            sound_effect_update_components_internal(effect_index, _sound_effect_refresh_start);
        }
    }
    return effect_index;
}

// match: 0x12a0b8: the update loop requires deletion to inline; the public function remains out of line.
static __forceinline void sound_effect_manager_delete_internal(long index)
{
    s_sound_effect_datum *datum = datum_get<s_sound_effect_datum>(((s_data_array *)g_sound_effect_data), index);
    if (datum->controller_index != -1)
    {
        sound_playback_controller_release_reference(datum->controller_index);
        datum->controller_index = -1;
    }
    ::datum_delete((s_data_array *)g_sound_effect_data, index);
}
// @address 0x12a0b8 ?sound_effects_update@@YAXXZ
void sound_effects_update()
{
    sound_effect_manager_iterator iterator;
    iterator.next_index = -1;
    iterator.index = -1;
    iterator.array = g_sound_effect_data;
    iterator.datum = ::data_iterator_next((s_data_iterator *)&iterator.array);
    while (iterator.datum)
    {
        long effect_index = iterator.index;
        s_sound_effect_datum *effect = (s_sound_effect_datum *)iterator.datum;
        if (effect->flags & 1)
            sound_effect_manager_delete_internal(effect_index);
        else if (sound_effect_track_and_update_internal(effect_index))
            sound_effect_update_components_internal(effect_index, _sound_effect_refresh_update);
        iterator.datum = ::data_iterator_next((s_data_iterator *)&iterator.array);
    }
}

// @address 0x36084 k_sound_effect_impulse_callbacks
// @rdata 0x82036084..0x820360e0
const s_sound_event_callbacks k_sound_effect_impulse_callbacks =
{
    sound_effect_track_origin_sound,
    sound_effect_notify_origin_in_channel,
    sound_effect_notify_origin_sound,
    sound_effect_update_origin_sound_output_properties,
    sound_effect_on_origin_sound_stop,
    sound_effect_on_origin_sound_scripted_stop,
    0,
    0
};
static const sound_effect_manager_component_callbacks k_sound_effect_manager_component_callbacks =
{
    sound_effect_track_component_sound, { 0, 0, 0, 0, 0, 0, 0 }
};
static const long k_sound_effect_manager_effect_refresh_states[3] = { 0, 1, 2 };
static const long k_sound_effect_manager_game_refresh_states[4] = { 0, 1, 2, 2 };
// @rdata-end

// @address 0x1fa1a8
void sound_effect_update_components_internal(long effect_index, e_sound_effect_refresh_state effect_refresh_state)
{
    s_sound_effect_datum *sound_effect = datum_get<s_sound_effect_datum>(g_sound_effect_data, effect_index);
    c_sound_playback_controller *controller = ((c_sound_playback_controller *)g_default_playback_controller);
    if (sound_effect->controller_index != -1)
        controller = &((c_sound_playback_controller *)((s_data_array *)g_sound_playback_controller_data)->data)
            [datum_index_to_absolute_index(sound_effect->controller_index)];
    long controller_identifier = controller->owner_identifier;
    s_sound_source source;
    source.source_flags.clear();
    sound_effect_get_source(effect_index, &source);
    sound_effect_manager_looping_parameters loop_parameters;
    if (sound_effect->source_type == _sound_effect_source_impulse)
    {
        long looping_index = sound_effect->sound_index;
        game_looping_sound_datum *looping = &((game_looping_sound_datum *)game_looping_sound_data.m_data->data)[datum_index_to_absolute_index(looping_index)];
        loop_parameters.alternate = looping->flags.test(4);
        loop_parameters.loop_state = looping->state;
    }
    // match: 0x1fa1a8: value range assumption guides branch lowering
    __assume((unsigned long)effect_refresh_state < 3);
    const sound_effect_manager_definition *definition = sound_effect->definition;
    for (long component_index = 0; component_index < definition->components.count; ++component_index)
    {
        const sound_effect_manager_component *component = &definition->components.data[component_index];
        if (component->definition_index != -1)
        {
            switch (component->group_tag)
            {
            case 'lsnd':
            {
                const unsigned long *looping_definition = (const unsigned long *)tag_get(0, component->definition_index);
                bool use_loop_parameters = (component->flags & (1 << 5)) && sound_effect->source_type == _sound_effect_source_impulse;
                e_looping_sound_refresh_state refresh_state = (e_looping_sound_refresh_state)(use_loop_parameters
                    ? k_sound_effect_manager_game_refresh_states[loop_parameters.loop_state]
                    : k_sound_effect_manager_effect_refresh_states[(long)effect_refresh_state]);
                bool alternate = use_loop_parameters ? loop_parameters.alternate
                    : ((component->flags & (1 << 3)) || (sound_effect->source_stopped()
                        && sound_effect->stopping_abnormally() && (component->flags & (1 << 4))));
                unsigned long playback_flags = (unsigned short)sound_effect->source_data;
                if (*looping_definition & (1 << 3))
                    playback_flags &= ~1;
                if (*looping_definition & (1 << 6))
                    playback_flags &= ~2;
                unsigned long flags = 0;
                if (alternate) flags |= 1; else flags &= ~1;
                if ((long)effect_refresh_state == _sound_effect_refresh_stop) flags |= 2; else flags &= ~2;
                s_sound_source component_source;
                memcpy(&component_source, &source, sizeof(source));
                component_source.gain += component->gain;
                long identifier = sound_effect->get_identifier(effect_index);
                sound_refresh_looping(component->definition_index, playback_flags, identifier, controller_identifier, -1, &component_source,
                    refresh_state, flags, 0.0f);
                break;
            }
            case 'snd!':
                if (((long)effect_refresh_state == _sound_effect_refresh_start && !(component->flags & 1))
                    || ((long)effect_refresh_state == _sound_effect_refresh_stop && (component->flags & 2)))
                {
                    s_sound_impulse_definition impulse;
                    impulse.flags = 21;
                    impulse.playback_flags = (unsigned short)sound_effect->source_data;
                    memcpy(&impulse.source, &source, sizeof(source));
                    if ((long)effect_refresh_state != _sound_effect_refresh_stop)
                    {
                        impulse.controller_index = controller_identifier;
                        impulse.flags |= 32;
                        impulse.callbacks = &k_sound_effect_manager_component_callbacks;
                        memset(&impulse.tracking_data, 0, sizeof(impulse.tracking_data) + sizeof(impulse.track_data));
                        impulse.event_size = 48;
                        impulse.track_data.effect_index = effect_index;
                    }
                    impulse.gain = component->gain;
                    impulse.flags |= 128;
                    sound_impulse_new(component->definition_index, &impulse, 0);
                }
                break;
            }
        }
    }
}
