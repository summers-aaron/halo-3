// blamlibxbox/sound_manager.obj: from the records 0xf87d8 0xf8908 0xf8c88 0xf8e38 0xf9050 0xf9188 0xf9318 0xf9390 0xf9810 0xf9a98 0xf9b38 0xfa7e8 0xfa848 0xfa928 0xfab50 0x129a18 0x129b10 0x129e48 0x137470 0x13adb8 0x13ae18 0x13afb8 0x13b140 0x13b1a8 0x13b410 0x13b830 0x13b968 0x13bab0 0x13d038 0x13d348 0x13d510 0x13d580 0x13e2d8 0x1a2078 0x1a23a8 0x1a2540 0x1a2640 0x1a2820 0x1a2a70 0x1a2c18 0x1a2e30 0x1a2f38 0x1a2fe0 0x1a35d0 0x1a36b8 0x1a39e8 0x1a3cd0 0x1a3fd0 0x1a41c0 0x1a47f0 0x1a4888 0x1a4c08 0x1a4ea8 0x1a5210 0x1a53c8 0x1a5498 0x1a55c0 0x1a5b28 0x1a5b98 0x1a5fc0 0x1a6348 0x1a65f8 0x1a6698 0x1a6c60 0x1a7070 0x1a7170 0x1a73a8 0x1a7410 0x1a76d8 0x1a7848 0x1a7940 0x1a7bd0 0x1a7cb8
// @address 0x8a1ecc ?g_active_structure_bsp_mask@@3KA
#include "sound/game_sound_globals.h"
// @address 0x74b898 g_geometry_globals
// @address 0x94e3ac g_saved_film_playback_tick
// @address 0x221fc k_real_constant_pool
// @address 0x2251c k_real_zero_storage
// @address 0x3aa90 k_real_math_zero_vector_storage
// @address 0x11df8cc g_observer_globals
// @address 0x11df70c g_director_globals
// @flags /TP /O2
// @compiler xdk6021
#define NOD3D
#include "main/main_game.h"
#include "scenario/scenario_tag.h"
#include "scenario/scenario_structure_bsp_reference.h"
#include "game/player_mapping_types.h"
#include "game/game_time_types.h"
#include "sound/sound_manager_types.h"
#include "sound/sound_manager_runtime.h"
// @address 0x70dd64 XAudioGetSpeakerConfig
#include "sound/sound_manager_globals.h"
#include "sound/sound_source_types.h"
#include "sound/sound_manager_runtime_types.h"
#include "sound/game_sound_types.h"
#include "sound/game_sound_transmission_types.h"
#include "sound/sound_event_callbacks.h"
#include "multithreading/synchronization_types.h"
#include "sound/sound_xaudio_xenon_source_manager_types.h"
#include "cseries/crt_xdk.h"
#include "sound/sound_cache_file_gestalt_globals.h"
#include "sound/sound_cache_file_gestalt.h"
#include "sound/sound_permutation_chunk.h"
#include "sound/sound_cache_inline.h"
#include "sound/sound_event_fade_reason_types.h"
#include "math/matrix_math_types.h"
#include "game/game_globals_storage.h"
#include "memory/data_types.h"
#include "memory/datum_index_inlines.h"
#include "tag_files/tag_types.h"
// @address 0x129a18 ?sound_definition_is_valid@@YA_NJ@Z
// @address 0xf8908 ?sound_stop_all@@YAXJ@Z

#include <math.h>
#include "game/game_globals.h"
#include "game/game.h"
#include "game/players.h"
#include "objects/objects.h"
#include "scenario/scenario.h"
#include "units/units.h"
#include "units/unit_types.h"
#include "sound/sound_playback_controller.h"
#include "cache/xbox_sound_cache.h"
#include "camera/first_person_camera.h"
#include "cseries/runtime_state.h"
#include "memory/lruv_cache.h"
#include "physics/water_physics.h"
#include "physics/water_physics_types.h"
#include "sound/looping_sound_manager.h"
#include "sound/sound_classes.h"
#include "sound/sound_definitions.h"
#include "sound/sound_manager.h"
#include "sound/sound_xaudio_xenon.h"
#include "bink/bink_playback.h"
#include "game/game_time.h"
#include "main/main.h"
#include "saved_games/saved_film.h"
#include "sound/sound_effect_manager.h"
#include "sound/sound_manager_platform_playback.h"
#include "structures/structure_audibility.h"
#include "cache/cache_file_tag_resource_runtime_manager_storage.h"
#include "cache/cache_files.h"
#include "sound/sound_cache_file_definitions.h"
#include "sound/sound_definition_inline.h"
#include "sound/sound_manager_lruv_cache_types.h"
#include "sound/sound_channel_summary_types.h"
#include "sound/sound_pause_state_types.h"
#include "sound/sound_classes_types.h"
#include "multithreading/threads.h"
#include "sound/synchronization_event_types.h"
#include "multithreading/synchronization.h"
#include "sound/sound_globals_reference_types.h"
#include "camera/camera_types.h"
#include "camera/observer_globals_types.h"
#include "objects/object_datum.h"
#include "units/unit_definitions.h"
#include "sound/sound_effect_collection.h"
#include "math/matrix_math_functions.h"
#include "math/real_math_functions.h"
#include "math/real_math_globals.h"
#include "sound/sound_math.h"
#include "sound/game_sound_definitions.h"
#include "game/game_globals_inline.h"
#include "game/game_state_accessors.h"
#include "scenario/scenario_access.h"
#include "memory/datum_storage_access.h"
#include "objects/object_header.h"
#include "memory/data.h"
#include "sound/sound_xaudio_xenon_3d_voice_types.h"
#include "sound/sound_xaudio_xenon_voice_manager.h"
#include "sound/sound_xaudio_xenon_reverb_manager.h"
#include "sound/sound_playback_controller_types.h"
#include <string.h>
#include <ppcintrinsics.h>
#include "cseries/cseries.h"
#include "sound/sound_tracker.h"
#include "sound/looping_sound_datum.h"
#ifdef HALO3_64BIT
#include "sound/sound_effect_manager_internals.h"
#endif
#include "sound/sound_mix_definition.h"
#include "sound/sound_xaudio_xenon_source_state.h"
#include "sound/sound_xaudio_xenon_mix_master.h"
#include "memory/tls_globals.h"
#include <stdlib.h>
#include "saved_games/saved_film_tick.h"
#include "memory/data_array_functions.h"
#include "sound/sound_effect_manager_externs.h"
#include "sound/sound_xaudio_xenon_3d_voice.h"
#include "cache/geometry_globals.h"
#include "sound/sound_xaudio_sync.h"
#include "ai/event_handling_sound_types.h"
#include "camera/director_storage_types.h"
#include "camera/saved_film_director_storage.h"
#include "units/unit_seat.h"
#include "sound/platform_sound_listener_reverb_types.h"
#include "sound/sound_preferences.h"
#include "tag_files/tag_groups.h"
#include "tag_files/string_ids.h"
#include "math/random_math.h"
#include "math/random_math_range.h"
#include "game/player_mapping_inline.h"
#include "objects/object_reference_types.h"
struct s_cache_file_sound_permutation;
struct s_data_array;
struct sound_chunk;

enum e_sound_promotion_result {
    _sound_promotion_result_promoted = 0,
    _sound_promotion_result_suppressed = 1,
};

// 0x1a53c8: how much a sound's class keeps its priority at a distance
#include "sound/sound_class_definitions.h"

enum e_sound_manager_gain_scale_type
{
    _sound_manager_gain_scale_linear,
    _sound_manager_gain_scale_power,
    _sound_manager_gain_scale_inverse_power,
};
enum e_sound_fade_mode
{
    _sound_fade_mode_linear = 0,
    _sound_fade_mode_crossfade = 2,
};
enum e_sound_cache_miss_mode { _sound_cache_miss_mode_discard, _sound_cache_miss_mode_postpone };
enum e_sound_manager_impulse_flag_bits {
    _sound_manager_impulse_next_chunk_queued_bit = 3,
    _sound_manager_impulse_finished_bit = 6,
    _sound_manager_impulse_started_bit = 9,
};


static __forceinline bool kind_matches(const s_sound_codec *kind)
{
  return kind->encoding != 4 && ((1 << (signed char)kind->compression) & 0xc0) != 0;
}

namespace {
}


static __forceinline bool sound_manager_source_test(const s_sound_source *source, long bit)
{
    bool result = (source->flags & (1 << bit)) != 0;
    return result;
}


// @address 0xf9318 ?normalize_and_clamp_linear@@YAMMMM_N@Z
// match: 0x13d038: inline lets cl expand this into sound_calculate_fade_db and
// sound_source_calculate_location_rolloff_factor; plain, both keep the call and lose saved registers
#if defined(_XBOX) && !defined(__clang__)
inline float normalize_and_clamp_linear(float lower, float upper, float value, bool reverse)
#else
float normalize_and_clamp_linear(float lower, float upper, float value, bool reverse)
#endif
{
    float normalized;
    if (fabs(lower - upper) >= 0.0001f) normalized = (value - lower) / (upper - lower);
    else normalized = ((value < lower) == reverse) ? 0.0f : 1.0f;
    return (normalized > 0.0f ? normalized : 0.0f) < 1.0f ? (normalized > 0.0f ? normalized : 0.0f) : 1.0f;
}


// 0xf9a98: every sound's promotion starts over
// @address 0xf9a98 sound_reset_promotions
extern "C" void sound_reset_promotions(void)
{
    tag_iterator iterator;
    iterator.next_index = 0;
    iterator.group_tag = 'snd!';
    for (long tag_index = tag_iterator_next(&iterator); tag_index != -1; tag_index = tag_iterator_next(&iterator)) {
        const s_cache_file_sound_definition *definition =
            (const s_cache_file_sound_definition *)global_tag_instances[DATUM_INDEX_TO_ABSOLUTE_INDEX(tag_index)].definition;
        if (definition->promotion_index != -1) {
            s_sound_promotion *promotion = &g_sound_cache_file_gestalt.data->promotions[definition->promotion_index];
            if (promotion) {
                promotion->current_rule = -1;
                promotion->last_update_time = 0;
                promotion->suppress_until = 0;
            }
        }
    }
}

// A sound is valid when its flags and kind allow playback.
bool sound_definition_is_valid(long tag_index)
{
  s_cache_file_sound_definition *definition = TAG_GET('snd!', s_cache_file_sound_definition, tag_index);
  return !(definition->flags & 0x1000) && kind_matches(&g_sound_cache_file_gestalt.data->codecs[definition->platform_index]);
}

namespace {
__forceinline real_vector3d *sound_vector_from_points3d(const real_point3d *origin, const real_point3d *point, real_vector3d *result)
{
    result->i = point->x - origin->x;
    result->j = point->y - origin->y;
    result->k = point->z - origin->z;
    return result;
}
__forceinline float sound_magnitude_squared3d(const real_vector3d *vector)
{
    return vector->i * vector->i + vector->j * vector->j + vector->k * vector->k;
}
__forceinline float sound_magnitude_squared3d(const real_point3d *point)
{
    return point->x * point->x + point->y * point->y + point->z * point->z;
}
__forceinline float sound_distance_squared3d(const real_point3d *origin, const real_point3d *point)
{
    real_vector3d delta;
    sound_vector_from_points3d(origin, point, &delta);
    return sound_magnitude_squared3d(&delta);
}
__forceinline real_vector3d *sound_scale_vector3d(const real_vector3d *vector, float scale, real_vector3d *result)
{
    result->i = vector->i * scale;
    result->j = vector->j * scale;
    result->k = vector->k * scale;
    return result;
}
}

#if defined(_XBOX) && !defined(__clang__)
// match: 0xf8e38: 0.0f (+0x320), 1.0f and FLT_MAX (-0xb68) load off one k_real_constant_pool base in r11; spelled literals add a lis per load
#define k_sound_real_zero k_real_constant_pool[200]
#define k_sound_real_one k_real_constant_pool[0]
#define k_sound_real_max k_real_constant_pool[-730]
#else
#define k_sound_real_zero 0.0f
#define k_sound_real_one 1.0f
#define k_sound_real_max FLT_MAX
#endif

// @address 0xf8e38 ?source_distance_squared@@YAMJPBUs_sound_source@@@Z
float source_distance_squared(long listener_index, const s_sound_source *source)
{
    switch (source->current_spatialization) {
    case 0:
        return k_sound_real_zero;
    case 1:
        {
            const s_sound_listener *listener = &g_sound_manager_globals->listeners[listener_index];
            switch ((signed char)source->volume_type) {
            case 0:
                return sound_distance_squared3d(&source->position, &listener->matrix.position);
            case 1:
                {
                    real_matrix3x3 rotation;
                    matrix3x3_rotation_from_quaternion(&rotation, &source->orientation);
                    float length = source->length;
                    const real_point3d *point = &listener->matrix.position;
                    const real_point3d *origin = &source->position;
                    real_vector3d delta;
                    float fraction;
                    if ((length < 0.001f ||
                         (sound_vector_from_points3d(origin, point, &delta),
                          fraction = (delta.i * rotation.up.i + delta.j * rotation.up.j + delta.k * rotation.up.k) / length,
                          fraction >= k_sound_real_zero && fraction <= k_sound_real_one)) && source->length >= 0.001f) {
                        real_vector3d axis;
                        sound_scale_vector3d(&rotation.up, length, &axis);
                        return point_to_line_distance_squared3d(point, origin, &axis);
                    }
                    return k_sound_real_max;
                }
            default:
                {
                    real_matrix3x3 rotation;
                    matrix3x3_rotation_from_quaternion(&rotation, &source->orientation);
                    real_vector3d axis = rotation.up;
                    sound_scale_vector3d(&axis, source->length, &axis);
                    return point_to_line_distance_squared3d(&listener->matrix.position, &source->position, &axis);
                }
            }
        }
    default:
        return sqrtf(sound_magnitude_squared3d(&source->position));
    }
}

// @address 0x13d510 ?sound_source_has_directional_attenuation@@YA_NPBUs_sound_source@@J@Z
inline bool sound_source_has_directional_attenuation(const s_sound_source *source, long sound_index)
{
    return sound_manager_source_test(source, 4) || sound_manager_source_test(source, 5) ||
        !sound_definition_get_playback_parameters(
            TAG_GET('snd!', const s_cache_file_sound_definition, sound_index))
             ->test(4);
}

struct scenario;
struct s_game_globals;
struct s_platform_sound_effect_input_parameters;
namespace {
__forceinline long game_sound_get_platform_playback_label(long playback_index)
{
    long label = -1;
    long type = (unsigned long)playback_index >> 30;
    if (type != 1) {
        if (type == 2) {
            const scenario *scenario_data = global_scenario;
            if (scenario_data->sound_effect_collection.index != -1) {
                long element = playback_index & 0x3fffffff;
                const sound_effect_collection_definition *collection =
                    TAG_GET('sfx+', const sound_effect_collection_definition, scenario_data->sound_effect_collection.index);
                label = collection->sound_effects.begin()[element].name;
            }
        }
    } else {
        const s_game_globals *globals = global_game_globals;
        const sound_globals_reference *sounds = globals->sound_globals.count() ? globals->sound_globals.begin() : 0;
        if (sounds && sounds->sound_effects.index != -1) {
            long element = playback_index & 0x3fffffff;
            const sound_effect_collection_definition *collection =
                TAG_GET('sfx+', const sound_effect_collection_definition, sounds->sound_effects.index);
            label = collection->sound_effects.begin()[element].name;
        }
    }
    return label;
}
}
// @address 0x13d580 ?platform_sound_build_playback_submit_parameters@@YAXJJPBUs_platform_sound_effect_input_parameters@@PAUs_platform_sound_playback_submit_parameters@@@Z
void platform_sound_build_playback_submit_parameters(long sound_index, long playback_index,
    const s_platform_sound_effect_input_parameters *input, s_platform_sound_playback_submit_parameters *parameters)
{
    long label = game_sound_get_platform_playback_label(playback_index);
    switch (label) {
    case _string_id_controller_0_headset:
        parameters->output = 0;
        break;
    case _string_id_controller_1_headset:
        parameters->output = 1;
        break;
    case _string_id_controller_2_headset:
        parameters->output = 2;
        break;
    case _string_id_controller_3_headset:
        parameters->output = 3;
        break;
    default:
        parameters->output = -1;
        break;
    }
    const s_platform_sound_playback_definition *playback = game_sound_get_platform_playback_definition(-1, playback_index);
    const s_platform_sound_playback_definition *properties = playback;
    if (playback && properties->channel > 0 && (unsigned long)properties->channel <= 1) {
        parameters->channel = properties->channel - 1;
        parameters->special = (properties->flags & 1) != 0;
    } else {
        parameters->channel = -1;
    }
}

// @address 0x1a3fd0 ?sound_definition_promote@@YA?AW4e_sound_promotion_result@@JPAJ@Z
e_sound_promotion_result sound_definition_promote(long definition_index, long *promotion_definition_index)
{
    e_sound_promotion_result result = _sound_promotion_result_promoted;
    const s_cache_file_sound_definition *definition = TAG_GET('snd!', const s_cache_file_sound_definition, definition_index);
    s_sound_promotion *promotion = definition->promotion_index != -1 ? &g_sound_cache_file_gestalt.data->promotions[definition->promotion_index] : 0;
    if (promotion) {
        if (promotion->suppress_until <= g_sound_manager_globals->time) {
            promotion->suppress_until = 0;
            promotion->current_rule = -1;
        }
        long elapsed = g_sound_manager_globals->time - promotion->last_update_time;
        for (long timer_index = 0; timer_index < promotion->runtime_timers.count(); timer_index++) {
            long *timer = &promotion->runtime_timers[timer_index];
            *timer -= elapsed;
            *timer = *timer < 0 ? 0 : *timer;
        }
        long rule_index = 0;
        while (rule_index < promotion->rules.count()) {
            long *timer = &promotion->runtime_timers[rule_index];
            const s_sound_promotion_rule *rule = &promotion->rules[rule_index];
            *timer += rule->increment;
            if (rule->count <= 0 || *timer < rule->threshold || rule_index >= promotion->rules.count() - 1)
                break;
            *timer = 0;
            rule_index++;
        }
        long current_rule = promotion->current_rule;
        *promotion_definition_index = rule_index;
        if (rule_index <= current_rule && g_sound_manager_globals->time < promotion->suppress_until)
            result = _sound_promotion_result_suppressed;
        if (rule_index >= current_rule && result == _sound_promotion_result_promoted) {
            promotion->current_rule = rule_index;
            promotion->suppress_until = (long)(promotion->rules[rule_index].timeout_seconds * 1000.0f + (float)g_sound_manager_globals->time);
        }
        promotion->last_update_time = g_sound_manager_globals->time;
    } else {
        *promotion_definition_index = -1;
    }
    return result;
}

static __forceinline bool sound_manager_sound_class_uses_distance(const sound_class_definition *definition) {
    return !((definition->flags >> 13) & 1);
}
static __forceinline bool sound_manager_sound_class_is_exclusive_per_object(const sound_class_definition *definition) {
    return (definition->internal_flags >> 1) & 1;
}
// @address 0x1a53c8 sound_compute_priority
extern "C" int sound_compute_priority(int distance, int sound_class)
{
    int priority = inl_sound_class_get(sound_class)->priority;
    const sound_class_definition *definition = inl_sound_class_get(sound_class);
    if (priority < 8 && sound_manager_sound_class_uses_distance(definition)) {
        if (distance < 4)
            priority += 2;
        else if (distance < 100)
            priority += 1;
        if (priority > 8)
            priority = 8;
    }
    return priority;
}

// @address 0x13bab0 ?sound_manager_sound_class_get_gain_db@@YAPBUreal_decibel@@F_N@Z
const real_decibel *sound_manager_sound_class_get_gain_db(short sound_class, bool alternate)
{
    const sound_class_definition *definition = inl_sound_class_get(sound_class);
    return alternate ? &definition->alternate_gain_db : &definition->gain_db;
}

struct s_sound_permutation_chunk;


static __forceinline sound_channel_datum *channel_get(short channel_index) {
    return &g_sound_channels[channel_index];
}
static __forceinline void platform_sound_source_signal_complete(long source_index) {
    g_xaudio_source_manager.source_signal_complete(source_index);
}
// @address 0x1a73a8 ?channel_signal_complete@@YAXF@Z
void channel_signal_complete(short channel_index) {
    sound_channel_datum *channel = channel_get(channel_index);
    platform_sound_source_signal_complete(channel->source_index);
}

struct s_sound_event_callbacks;


// @address 0x7d1f0c ?g_sound_data@@3V?$c_smart_data_array@Usound_datum@@@@A
c_smart_data_array<sound_datum> g_sound_data;
// @address 0x7d1f08 ?g_sound_channels@@3PAUsound_channel_datum@@A
sound_channel_datum *g_sound_channels;
// @address 0x7d1f10 ?g_sound_manager_globals@@3PAUs_sound_manager_globals@@A
s_sound_manager_globals *g_sound_manager_globals;

static __forceinline void platform_sound_source_stop(long source_index) {
    g_xaudio_source_manager.source_stop(source_index);
}
// @address 0x1a7070 ?channel_pause@@YAXF@Z
void channel_pause(short channel_index) {
    sound_channel_datum *channel = channel_get(channel_index);
    short source_index = channel->source_index;
    if (source_index != -1 && !channel->paused) {
        bool pausable = true;
        if (channel->sound_index != -1) {
            const sound_datum *sound = datum_get<sound_datum>(g_sound_data, channel->sound_index);
            const s_cache_file_sound_definition *definition = 
                TAG_GET('snd!', const s_cache_file_sound_definition, sound->definition_index);
            if (definition->sound_class >= 0 && definition->sound_class < 54)
                pausable = !(sound_class_get(definition->sound_class)->flags & 1);
        }
        if (pausable) {
            platform_sound_source_stop(source_index);
            channel->paused = true;
        }
    }
}

// the function below: unlock6
static __forceinline float sound_manager_sound_definition_get_attack_distance(long sound_index)
{
    const s_cache_file_sound_definition *definition = TAG_GET('snd!', const s_cache_file_sound_definition, sound_index);
    const s_sound_playback_parameters *playback = sound_definition_get_playback_parameters(definition);
    if (playback->test(1))
        return sound_class_get(definition->sound_class)->attack_distance;
    return sound_definition_get_playback_parameters(definition)->attack_distance;
}
static __forceinline float sound_manager_sound_source_get_minimum_distance(const s_sound_source *source, long sound_index)
{
    if (sound_manager_source_test(source, 2))
        return source->minimum_distance;
    return sound_definition_get_minimum_distance(sound_index);
}
static __forceinline float sound_manager_sound_source_get_maximum_distance(const s_sound_source *source, long sound_index)
{
    if (sound_manager_source_test(source, 3))
        return source->maximum_distance;
    return sound_definition_get_maximum_distance(sound_index);
}

// @address 0xf9188 ?sound_source_get_transmission_transform@@YAXPBUs_sound_source@@JPAUs_sound_transmission_transform@@@Z
void sound_source_get_transmission_transform(const s_sound_source *source, long sound_index, s_sound_transmission_transform *transform)
{
    if (!sound_manager_source_test(source, 3)) {
        transform->silence_distance = sound_definition_get_silence_distance(sound_index);
        transform->attack_distance = sound_manager_sound_definition_get_attack_distance(sound_index);
    } else {
        transform->silence_distance = 0.0f;
        transform->attack_distance = 0.0f;
    }
    transform->minimum_distance = sound_manager_sound_source_get_minimum_distance(source, sound_index);
    transform->maximum_distance = sound_manager_sound_source_get_maximum_distance(source, sound_index);
}

// sound_dispose: unlock6

typedef char sound_manager_datum_size[(sizeof(void *) != 4 || ((sizeof(sound_datum) == 0xc0))) ? 1 : -1];
typedef char sound_manager_datum_source_offset[(sizeof(void *) != 4 || ((offsetof(sound_datum, source) == 0x18))) ? 1 : -1];
typedef char sound_manager_datum_pitch_range_offset[(sizeof(void *) != 4 || ((offsetof(sound_datum, pitch_range_index) == 0xa4))) ? 1 : -1];

static const real_decibel k_sound_manager_zero_gain_db = { 0.0f };
static const real_decibel k_sound_manager_minimum_gain_db = { -64.0f };
struct s_sound_effect_datum;
struct sound_effect_manager_controller;
struct looping_sound_datum;
struct IXAudioSubmixVoice;


class c_xenon_audio_reverb_manager;
extern "C" {
class c_sound_playback_controller;
}
class c_xenon_audio_voice_manager;
namespace {
static __forceinline void sound_manager_sound_cache_delete()
{
    data_dispose(g_sound_cache_globals.sounds);
    s_lruv_cache *cache = g_sound_cache_globals.lruv;
    c_allocation_base *allocator = cache->allocator;
    data_dispose((s_data_array *)cache->header.block_data);
    allocator->deallocate(cache);
    g_sound_cache_globals.buffer_address = 0;
}
static __forceinline void sound_manager_platform_sound_dispose()
{
    g_xaudio_voice_manager.dispose();
    g_xaudio_source_manager.dispose();
    g_xaudio_reverb_manager.dispose();
    XAudioShutDown();
}
static __forceinline void sound_manager_sound_effect_manager_dispose()
{
    if (g_sound_effect_data)
        g_sound_effect_data = 0;
}
static __forceinline void sound_manager_looping_sound_dispose()
{
    if (g_looping_sound_data)
        g_looping_sound_data = 0;
}
static __forceinline void sound_manager_sound_playback_controllers_dispose()
{
    if (g_sound_playback_controller_data)
        g_sound_playback_controller_data = 0;
    if (g_default_playback_controller)
        g_default_playback_controller = 0;
}
}

// @address 0x1a23a8 ?sound_dispose@@YAXXZ
void sound_dispose()
{
    if (--g_sound_initialization_count == 0) {
        sound_manager_sound_cache_delete();
        sound_manager_platform_sound_dispose();
        sound_manager_sound_effect_manager_dispose();
        sound_manager_looping_sound_dispose();
        sound_manager_sound_playback_controllers_dispose();
        if (g_sound_manager_globals)
            g_sound_manager_globals = 0;
        if (g_sound_data) {
            g_sound_data->valid = false;
            g_sound_data = 0;
        }
        if (g_sound_channels)
            g_sound_channels = 0;
    }
}

// sound_stop: unlock9


static __forceinline sound_datum *sound_manager_sound_get(long sound_index)
{
    return datum_get<sound_datum>(g_sound_data, sound_index);
}
static __forceinline c_sound_playback_controller *sound_manager_playback_controller_get(long controller_index)
{
    c_sound_playback_controller *controller = (c_sound_playback_controller *)g_default_playback_controller;
    if (controller_index != -1)
    {
        s_data_array *data = g_sound_playback_controller_data;
        controller = datum_get<c_sound_playback_controller>(data, controller_index);
    }
    return controller;
}
static __forceinline bool sound_manager_can_delete_sounds()
{
    return !g_game_state_lock.value && !g_sound_manager_globals->sounds_locked;
}
static __forceinline void sound_manager_sound_delete(long sound_index)
{
    sound_datum *sound = sound_manager_sound_get(sound_index);
    if (sound->playback_controller_index != -1)
    {
        c_sound_playback_controller *controller = sound_manager_playback_controller_get(sound->playback_controller_index);
        if (sound->synchronized_playback == 0xff)
            controller->synchronized_sound_ready();
        sound_playback_controller_release_reference(sound->playback_controller_index);
    }
    datum_delete(g_sound_data, sound_index);
}


// @address 0x13b1a8 ?sound_stop@@YAXJW4e_sound_event_stop_reason@@@Z
void sound_stop(long sound_index, e_sound_event_stop_reason stop_reason)
{
    sound_datum *sound = sound_manager_sound_get(sound_index);
    if (sound_manager_can_delete_sounds())
    {
        if (sound->callbacks && sound->callbacks->stop_proc)
            sound->callbacks->stop_proc(sound->object_index, sound_index, stop_reason);
        sound_manager_sound_delete(sound_index);
    }
    else
    {
        sound->flags.set(11, true);
        sound->stop_reason = stop_reason;
    }
}

// @address 0x13b968 ?sound_find_like_channel@@YAFJPAFF@Z
short sound_find_like_channel(long sound_index, short *channel_indices, short channel_count)
{
    sound_datum *sound = sound_manager_sound_get(sound_index);
    const s_cache_file_sound_definition *definition = 
                TAG_GET('snd!', const s_cache_file_sound_definition, sound->definition_index);
    long best_age = 0;
    short best_channel_index = -1;
    for (short index = 0; index < channel_count; index++)
    {
        unsigned short channel_index = channel_indices[index];
        sound_channel_datum *channel = channel_get(channel_index);
        sound_datum *channel_sound = sound_manager_sound_get(channel->sound_index);
        long age = g_sound_manager_globals->time - channel_sound->start_time;
        if (age >= inl_sound_class_get(definition->sound_class)->preemption_time && age > best_age
            || sound->priority > channel_sound->priority
            || channel_sound->object_index == -1 && sound->object_index != -1)
        {
            best_channel_index = channel_index;
            best_age = age;
        }
    }
    return best_channel_index;
}

static __forceinline bool sound_manager_sound_valid_for_channel(const s_cache_file_sound_definition *definition, unsigned char channel_type)
{
    bool valid = true;
    if (g_sound_cache_file_gestalt.data->codecs[definition->platform_index].encoding != channel_type)
        valid = false;
    return valid;
}

// @address 0x13b410 ?sound_channel_summary_build@@YAXPAUsound_channel_summary@@J@Z
void sound_channel_summary_build(sound_channel_summary *out_summary, long sound_index)
{
    // match: 0x13b410: pointer __restrict qualifier prevents alias reload
    sound_channel_summary *__restrict summary = out_summary;
    sound_datum *sound = sound_manager_sound_get(sound_index);
    const s_cache_file_sound_definition *definition = 
                TAG_GET('snd!', const s_cache_file_sound_definition, sound->definition_index);
    summary->like_sound_started_this_tick = false;
    summary->like_channel_count = 0;
    summary->object_channel_count = 0;
    summary->maximum_like_channels = inl_sound_class_get(definition->sound_class)->maximum_sounds_per_tag;
    summary->maximum_object_channels = inl_sound_class_get(definition->sound_class)->maximum_sounds_per_object;
    for (short channel_index = 0; channel_index < g_sound_manager_globals->channel_count; channel_index++)
    {
        sound_channel_datum *channel = channel_get(channel_index);
        if (channel->sound_index != -1 && channel->sound_index != sound_index)
        {
            sound_datum *channel_sound = sound_manager_sound_get(channel->sound_index);
            if (sound_manager_sound_valid_for_channel(definition, channel->channel_type) &&
                sound->definition_index == channel_sound->definition_index)
            {
                summary->like_channels[summary->like_channel_count] = channel_index;
                summary->like_channel_count++;
                if (sound->object_index != -1 && channel_sound->object_index != -1 && sound->callbacks == channel_sound->callbacks
                    && (sound->object_index == channel_sound->object_index
                        || sound->callbacks->compare_proc && sound->callbacks->compare_proc(sound->object_index, sound->event_parameters,
                            channel_sound->object_index, channel_sound->event_parameters)))
                {
                    summary->object_channels[summary->object_channel_count] = channel_index;
                    summary->object_channel_count++;
                    if (g_sound_manager_globals->time == channel_sound->start_time)
                        summary->like_sound_started_this_tick = true;
                }
            }
        }
    }
}
static inline float sound_manager_real_decibels_to_gain(real_decibel decibels)
{
    return (float)exp((double)(decibels.to_real() * 0.115129255f));
}
static __forceinline real_decibel sound_manager_real_decibel_interpolate(
    e_sound_manager_gain_scale_type scale_type, real_decibel lower, real_decibel upper, float fraction)
{
    float lower_gain = sound_manager_real_decibels_to_gain(lower);
    float upper_gain = sound_manager_real_decibels_to_gain(upper);
    float curve;
    switch (scale_type)
    {
    case _sound_manager_gain_scale_linear:
        curve = fraction;
        break;
    case _sound_manager_gain_scale_power:
        curve = lower_gain < upper_gain ? sqrtf(fraction) : sqrtf(1.0f - fraction);
        break;
    case _sound_manager_gain_scale_inverse_power:
        curve = lower_gain < upper_gain ? 1.0f - sqrtf(1.0f - fraction) : 1.0f - sqrtf(fraction);
        break;
    }
    return real_gain_to_decibels((upper_gain - lower_gain) * curve + lower_gain);
}

// @address 0x13d038 ?sound_calculate_fade_db@@YA?AUreal_decibel@@J@Z
real_decibel sound_calculate_fade_db(long sound_index)
{
    sound_datum *sound = sound_manager_sound_get(sound_index);
    real_decibel result = k_sound_manager_zero_gain_db;
    if (sound->flags.test(7))
    {
        if (sound->fade_start_time == -1 || sound->fade_end_time == -1)
        {
            sound->fade_start_time += g_sound_manager_globals->time + 1;
            sound->fade_end_time += g_sound_manager_globals->time + 1;
        }
        long start_time = sound->fade_start_time;
        long end_time = sound->fade_end_time;
        long base_time = MAX(start_time, end_time);
        float fraction = normalize_and_clamp_linear((float)(start_time - base_time), (float)(end_time - base_time),
            (float)(g_sound_manager_globals->time - base_time), start_time < end_time);
        real_decibel lower = start_time < end_time ? sound->fade_gain_db : k_sound_manager_minimum_gain_db;
        real_decibel upper = start_time >= end_time ? sound->fade_gain_db : k_sound_manager_zero_gain_db;
        switch (sound->fade_type)
        {
        case 0:
            result = sound_manager_real_decibel_interpolate(_sound_manager_gain_scale_linear, lower, upper, fraction);
            break;
        case 1:
            result = sound_manager_real_decibel_interpolate(_sound_manager_gain_scale_power, lower, upper, fraction);
            break;
        case 2:
            result = sound_manager_real_decibel_interpolate(_sound_manager_gain_scale_inverse_power, lower, upper, fraction);
            break;
        }
        result = pin_gain_db(result);
    }
    return result;
}


// @address 0x129b10 ?sound_start_fade@@YAXFW4sound_event_fade_reason@@MJJ@Z
void sound_start_fade(short mode, sound_event_fade_reason reason, float seconds, long fade_in_sound_index, long fade_out_sound_index)
{
    if (fade_in_sound_index != -1)
    {
        sound_datum *sound = sound_manager_sound_get(fade_in_sound_index);
        if (sound->flags.test(7))
            sound->fade_gain_db = sound_calculate_fade_db(fade_in_sound_index);
        else
            sound->fade_gain_db = k_sound_manager_minimum_gain_db;
        sound->fade_type = mode;
        sound->fade_start_time = -1;
        sound->fade_end_time = (long)(seconds * 1000.0f);
        sound->flags.set(7, true);
    }
    if (fade_out_sound_index != -1)
    {
        sound_datum *sound = sound_manager_sound_get(fade_out_sound_index);
        sound->fade_gain_db = sound_calculate_fade_db(fade_out_sound_index);
        sound->fade_type = mode;
        sound->fade_end_time = -1;
        sound->fade_start_time = (long)(seconds * 1000.0f);
        sound->flags.set(7, true);
    }
}

// @address 0x1a7848 ?sound_manager_fade_ratio_gain@@YA?AUreal_decibel@@W4e_sound_manager_gain_scale_type@@MM@Z
real_decibel sound_manager_fade_ratio_gain(e_sound_manager_gain_scale_type, float numerator, float denominator)
{
    float fv1 = fabs(numerator / denominator);
    fv1 = PIN(fv1, 0.0f, 1.0f);
    if (denominator < 0.0f)
        fv1 = 1.0f - fv1;
    float fv2 = fv1;
    fv2 = PIN(fv2, 0.0f, 1.0f);
    union { long bits; real_decibel decibels; } low_db, high_db;
    low_db.bits = -0x3d800000;
    high_db.bits = 0;
    float low_gain = sound_manager_real_decibels_to_gain(low_db.decibels);
    float high_gain = sound_manager_real_decibels_to_gain(high_db.decibels);
    float curve = low_gain < high_gain ? sqrtf(fv2) : sqrtf(1.0f - fv2);
    return real_gain_to_decibels((high_gain - low_gain) * curve + low_gain);
}



enum e_sound_datum_flag_bits;




enum e_sound_impulse_new_failure_reason;

struct s_xaudio_source_state;
struct s_location;
/* ---- TLS and globals ---- */
// @address 0x7d1d9c g_sound_initialization_count
#if !defined(_XBOX) || defined(__clang__)
long g_sound_initialization_count;
#endif

// @address 0x74b6b8 g_sound_master_gain
float g_sound_master_gain[2] = { 1.0f, 1.0f };
// @address 0x72fb77 g_sound_transmission_enabled
bool g_sound_transmission_enabled = true;
// @address 0x8a1ef1 ?debug_sound_force_first_person_listener@@3_NA
// @address 0x8a1ef2 g_sound_always_update
#if !defined(_XBOX) || defined(__clang__)
bool debug_sound_force_first_person_listener = false;
bool g_sound_always_update = false;
#endif

/* ---- our own targets (defined below) ---- */

/* ---- inline views and helpers ---- */
static __forceinline s_sound_manager_globals *sound_manager_globals_view_get(void)
{
    return g_sound_manager_globals;
}

static __forceinline s_sound_cache_file_gestalt_definition *sound_manager_gestalt_get(void)
{
    return (s_sound_cache_file_gestalt_definition *)g_sound_cache_file_gestalt.data;
}

static __forceinline c_xenon_audio_source_manager *sound_manager_platform_sources_get(void)
{
    return (c_xenon_audio_source_manager *)&g_xaudio_source_manager;
}

static __forceinline sound_datum *sound_manager_sound_get_view(long sound_index)
{
    return datum_get<sound_datum>(g_sound_data, sound_index);
}

static __forceinline sound_channel_datum *sound_manager_channel_get_view(short channel_index)
{
    return &g_sound_channels[channel_index];
}

static __forceinline bool sound_manager_sound_is_active(void)
{
    s_sound_manager_globals *globals = sound_manager_globals_view_get();
    return globals->initialized && globals->enabled && globals->active;
}




static __forceinline bool sound_manager_is_main_thread(void)
{
    return g_thread_system_initialized.peek() == 0 || g_registered_thread_index == 1;
}

static __forceinline bool sound_manager_flag_test16(unsigned short flags, long bit)
{
    unsigned char result = (flags >> bit) & 1;
    return result;
}

static __forceinline bool sound_manager_sound_unknown_type(long sound_index)
{
    return ((1 << sound_manager_sound_get_view(sound_index)->type) & 0x1e) != 0;
}

static __forceinline bool sound_manager_sound_class_stops_when_attached_to_object(const sound_class_definition *definition)
{
    return sound_manager_flag_test16(definition->internal_flags, 1) || sound_manager_flag_test16(definition->internal_flags, 3) ||
        sound_manager_flag_test16(definition->flags, 11);
}

static __forceinline bool sound_manager_sound_tracking_allowed(void)
{
    return !g_game_state_lock.value && !sound_manager_globals_view_get()->sounds_locked;
}

static __forceinline long sound_manager_sound_samples_per_second(short codec_index)
{
    s_sound_cache_file_gestalt_definition *gestalt = sound_manager_gestalt_get();
    return sound_sample_rate_samples_per_second[gestalt->codecs[codec_index].sample_rate];
}

static __forceinline void sound_manager_platform_source_initialize_properties(
    long source_index, const s_sound_channel_properties *properties, const s_platform_sound_playback_submit_parameters *submit_parameters)
{
    ((c_platform_source_datum *)&sound_manager_platform_sources_get()->sources[source_index])->initialize_channel(properties->source_properties.channel);
    g_xaudio_mix_master.source_update_properties(source_index, &properties->source_properties, submit_parameters, false);
}

static __forceinline void sound_manager_flag_set16(unsigned short *flags, long bit, bool value)
{
    if (value)
        *flags |= (1 << bit);
    else
        *flags &= ~(1 << bit);
}

static __forceinline s_cache_file_sound_pitch_range *sound_manager_definition_get_pitch_range(
    const s_cache_file_sound_definition *definition, long pitch_range_index)
{
    return &sound_manager_gestalt_get()->pitch_ranges[definition->pitch_range_index + pitch_range_index];
}

static __forceinline long sound_manager_pitch_range_get_permutation_count(const s_cache_file_sound_pitch_range *pitch_range)
{
    unsigned long info = pitch_range->permutation_info;
    return g_sound_cache_file_gestalt.use_uncompressed_permutation_counts ? (info >> 26) : ((info >> 20) & 63);
}

static __forceinline s_cache_file_sound_permutation *sound_manager_pitch_range_get_permutation(const s_cache_file_sound_pitch_range *pitch_range,
    long permutation_index)
{
    return &sound_manager_gestalt_get()->permutations[((long)(pitch_range->permutation_info << 12) >> 12) + permutation_index];
}

static __forceinline s_sound_permutation_chunk *sound_manager_permutation_get_chunk(const s_cache_file_sound_permutation *permutation,
    long chunk_index)
{
    return (s_sound_permutation_chunk *)&sound_manager_gestalt_get()->chunks[permutation->first_chunk_index + chunk_index];
}


static __forceinline s_data_array *sound_manager_sound_data_array(void)
{
    c_smart_data_array<sound_datum> &data = g_sound_data;
    return data;
}

static __forceinline s_data_array *sound_manager_looping_sound_data_array(void)
{
    return g_looping_sound_data;
}

static __forceinline void sound_manager_channel_flush_for_restore(short channel_index)
{
    sound_channel_datum *channel = sound_manager_channel_get_view(channel_index);
    if (channel->source_index != -1 && channel->paused)
    {
        c_platform_source_datum *source = &sound_manager_platform_sources_get()->sources[channel->source_index];
        sound_manager_flag_set16(&source->flags, 2, true);
        if (source->state == 3)
            source->state = 1;
        sound_manager_flag_set16(&source->flags, 3, true);
        ((c_platform_source_datum *)source)->stop_immediately();
    }
}

static __forceinline long sound_manager_definition_get_encoding(const s_cache_file_sound_definition *definition)
{
    return sound_manager_gestalt_get()->codecs[definition->platform_index].encoding;
}

static __forceinline long sound_manager_definition_get_compression(const s_cache_file_sound_definition *definition)
{
    return sound_manager_gestalt_get()->codecs[definition->platform_index].compression;
}


static __forceinline void sound_manager_looping_initialize_for_structure_bsp()
{
    for (long sound_index = inl_data_next_index(sound_manager_looping_sound_data_array(), -1);
         sound_index != -1;
         sound_index = inl_data_next_index(sound_manager_looping_sound_data_array(), sound_index))
    {
        s_data_array *array = sound_manager_looping_sound_data_array();
        looping_sound_datum *looping_sound = datum_get<looping_sound_datum>(array, sound_index);
        if ((looping_sound->location & 0xf0) == 0x10)
            scenario_location_from_point((s_location *)&looping_sound->bsp_location, &looping_sound->position);
    }
}

// @address 0xf87d8 ?sound_impulse_new@@YAJJPBUs_sound_impulse_definition@@PAW4e_sound_impulse_new_failure_reason@@@Z
long sound_impulse_new(long definition_index, const s_sound_impulse_definition *impulse,
    e_sound_impulse_new_failure_reason *out_failure_reason)
{
    long sound_index = -1;
    long failure_reason = 5;
    if (sound_manager_is_main_thread() && sound_manager_sound_is_active())
    {
        long sound_priority;
        if (sound_definition_can_start_new_impulse_sound(definition_index, impulse, &sound_priority,
            (e_sound_impulse_new_failure_reason *)&failure_reason))
        {
            long promotion_definition_index;
            if (sound_definition_promote(definition_index, &promotion_definition_index) != 0)
            {
                sound_index = -1;
                failure_reason = 2;
            }
            else
            {
                sound_index = sound_impulse_new_internal(definition_index, sound_priority, impulse, promotion_definition_index);
                if (failure_reason != 0 && sound_index == -1)
                    failure_reason = 3;
            }
        }
    }
    if (out_failure_reason)
        *out_failure_reason = (e_sound_impulse_new_failure_reason)failure_reason;
    return sound_index;
}


static __forceinline bool sound_manager_cluster_references_equal(const s_cluster_reference *a, const s_cluster_reference *b)
{
    return *(const unsigned short *)a == *(const unsigned short *)b;
}

static __forceinline bool sound_manager_location_valid(const s_location *location)
{
    return (unsigned char)location->cluster_reference.bsp_index != 255;
}

static __forceinline const s_sound_listener *sound_manager_get_listener(long listener_index)
{
    return &sound_manager_globals_view_get()->listeners[listener_index];
}

static __forceinline float sound_manager_game_sound_get_cluster_distance(s_cluster_reference a, s_cluster_reference b)
{
    float distance = k_real_zero_storage;
    if (a.bsp_index == b.bsp_index)
    {
        distance = structure_audibility_get_cluster_distance(
            TAG_GET('sbsp', const structure_bsp,
                ((const scenario_structure_bsp_reference *)global_scenario->structure_bsps.address)[a.bsp_index].structure_bsp.index),
            a.cluster_index, b.cluster_index);
    }
    return distance;
}

// the listener is occluded from the source when both are in
// clusters and the audibility of the path between them is too low for the distance ratio
// @address 0xf9050 ?sound_occluded_simply@@YA_NJPBUs_sound_source@@M@Z
bool sound_occluded_simply(long listener_index, const s_sound_source *source, float distance_ratio)
{
    bool result = listener_index + 1 == 0;
    if (!result && !sound_manager_source_test(source, 0) && source->current_spatialization == 1)
    {
        result = true;
        const s_sound_listener *listener = sound_manager_get_listener(listener_index);
        const s_location *source_location = &source->location;
        if (sound_manager_location_valid(&source->location) && sound_manager_location_valid(&listener->location))
        {
            if (sound_manager_cluster_references_equal(&listener->location.cluster_reference, &source_location->cluster_reference))
                result = false;
            else if (sound_manager_game_sound_get_cluster_distance(listener->location.cluster_reference,
                source_location->cluster_reference) * distance_ratio < 256.0f)
                result = false;
        }
    }
    return result;
}


static __forceinline float sound_manager_point_segment_fraction(const real_point3d *point, const real_point3d *origin, real_vector3d axis,
    float length)
{
    float fraction;
    if (length < 0.001f)
        fraction = 0.0f;
    else
    {
        real_vector3d delta;
        sound_vector_from_points3d(origin, point, &delta);
        fraction = dot_product3d(&axis, &delta) / length;
    }
    return PIN(fraction, 0.0f, 1.0f);
}

// @address 0x13b830 ?sound_source_get_world_position@@YAXPBUs_sound_source@@JPATreal_point3d@@@Z
void sound_source_get_world_position(const s_sound_source *source, long listener_index, real_point3d *position)
{
    switch ((signed char)source->volume_type)
    {
    case 0:
        *position = source->position;
        break;
    case 1:
    case 2:
        {
            real_matrix3x3 rotation;
            matrix3x3_rotation_from_quaternion(&rotation, &source->orientation);
            real_vector3d axis = rotation.up;
            const s_sound_listener *listener = sound_manager_get_listener(listener_index);
            float length = source->length;
            float scale = source->length * sound_manager_point_segment_fraction(&listener->matrix.position, &source->position, axis,
                length);
            position->x = axis.i * scale + source->position.x;
            position->y = axis.j * scale + source->position.y;
            position->z = axis.k * scale + source->position.z;
        }
        break;
    }
}

// @address 0xf8c88 ?source_audible_internal@@YAJPBUs_sound_source@@MMPAM@Z
long source_audible_internal(const s_sound_source *source, float minimum_distance, float maximum_distance, float *out_distance)
{
    long best_listener_index = -1;
    float best_distance = 3.4028234663852886e+38f;
    if (source->current_spatialization != 1)
    {
        if (!(minimum_distance > 0.001f))
        {
            best_listener_index = inl_players_first_active_user();
            best_distance = 0.0f;
        }
    }
    else
    {
        float nearest_distance_squared = 3.402823466e+38F;
        float minimum_distance_squared = minimum_distance * minimum_distance;
        for (long listener_index = 0; listener_index < 4; listener_index++)
        {
            s_sound_listener *listener = &sound_manager_globals_view_get()->listeners[listener_index];
            if (listener->valid)
            {
                float distance_squared = source_distance_squared(listener_index, source);
                if (distance_squared >= minimum_distance_squared && distance_squared < nearest_distance_squared)
                {
                    best_listener_index = listener_index;
                    nearest_distance_squared = distance_squared;
                }
            }
        }
        best_distance = __fsqrts(nearest_distance_squared);
        if (sound_occluded_simply(best_listener_index, source, best_distance / maximum_distance))
            best_listener_index = -1;
        if (!sound_manager_flag_test16(source->flags, 8) && !sound_manager_flag_test16(source->flags, 11))
        {
            if (nearest_distance_squared > maximum_distance * maximum_distance)
            {
                best_listener_index = -1;
                best_distance = 3.402823466e+38F;
            }
        }
        else if (best_listener_index == -1)
        {
            best_listener_index = inl_players_first_active_user();
        }
    }
    if (out_distance)
        *out_distance = best_distance;
    return best_listener_index;
}

// @address 0xfa7e8 sound_set_permutation
extern "C" void sound_set_permutation(long sound_index, long definition_index, short pitch_range_index, short permutation_index,
    short chunk_index)
{
    sound_datum *sound = sound_manager_sound_get_view(sound_index);
    sound_datum_unlock(sound);
    sound->definition_index = definition_index;
    sound->pitch_range_index = pitch_range_index;
    sound->permutation_index = permutation_index;
    sound->chunk_index = chunk_index;
}

// @address 0xfa848 sound_datum_unlock
extern "C" void sound_datum_unlock(void *sound)
{
    sound_datum *datum = (sound_datum *)sound;
    if ((bool)((datum->flags.m_flags >> 1) & 1)) {
        const s_cache_file_sound_definition *definition =
            TAG_GET('snd!', const s_cache_file_sound_definition, datum->definition_index);
        const s_cache_file_sound_pitch_range *pitch_range =
            &sound_manager_gestalt_get()->pitch_ranges[definition->pitch_range_index + datum->pitch_range_index];
        const s_cache_file_sound_permutation *permutation =
            sound_manager_pitch_range_get_permutation(pitch_range, datum->permutation_index);
        sound_cache_sound_release(sound_manager_permutation_get_chunk(permutation, datum->chunk_index));
        datum->flags.m_flags &= 0xfffd;
    }
}

// @address 0xfa928 ?sound_make_permutation_ready@@YA_NPAUsound_datum@@PA_N@Z
bool sound_make_permutation_ready(sound_datum *datum, bool *out_ready)
{
    sound_datum *sound = (sound_datum *)datum;
    const s_cache_file_sound_definition *definition =
        TAG_GET('snd!', const s_cache_file_sound_definition, sound->definition_index);
    s_cache_file_sound_pitch_range *pitch_range = sound_manager_definition_get_pitch_range(definition, sound->pitch_range_index);
    s_cache_file_sound_permutation *permutation = sound_manager_pitch_range_get_permutation(pitch_range, sound->permutation_index);
    unsigned long result = _sound_cache_sound_request(
        sound->definition_index, (s_sound_permutation_chunk *)sound_manager_permutation_get_chunk(permutation, sound->chunk_index), 1);
    if (((result & 2) != 0) || ((result & 1) != 0))
        sound_datum_lock((sound_datum *)sound);
    if (out_ready)
        *out_ready = (unsigned char)((result >> 3) & 1);
    return (unsigned char)((result >> 1) & 1);
}

// @address 0xfab50 ?sound_datum_lock@@YAXPAUsound_datum@@@Z
void sound_datum_lock(sound_datum *datum)
{
    sound_datum *sound = (sound_datum *)datum;
    if (!sound_manager_flag_test16(sound->flags.m_flags, 1))
    {
        sound_manager_flag_set16(&sound->flags.m_flags, 1, true);
        long chunk_index = sound->chunk_index;
        long permutation_index = sound->permutation_index;
        long pitch_range_index = sound->pitch_range_index;
        const s_cache_file_sound_definition *definition =
            TAG_GET('snd!', const s_cache_file_sound_definition, sound->definition_index);
        _sound_cache_sound_request(-1,
            (s_sound_permutation_chunk *)sound_manager_permutation_get_chunk(
                sound_manager_pitch_range_get_permutation(
                    sound_manager_definition_get_pitch_range(definition, pitch_range_index), permutation_index),
                chunk_index),
            2);
    }
}

// @address 0x13adb8 sound_make_permutation_ready_by_index
extern "C" unsigned char sound_make_permutation_ready_by_index(unsigned short sound_index, bool *out_ready)
{
    unsigned short index = sound_index;
    bool result = false;
    if (sound_make_permutation_ready(&((sound_datum *)g_sound_data->data)[index], out_ready))
        result = true;
    return result;
}

// @address 0x13afb8 ?sound_track_and_update@@YA_NJPBUs_sound_tracker@@PAV?$c_flags@W4e_sound_datum_flag_bits@@G$0L@@@@Z
bool sound_track_and_update(long sound_index, const s_sound_tracker *tracker, c_flags<e_sound_datum_flag_bits, unsigned short, 11> *flags)
{
    sound_datum *sound = sound_manager_sound_get_view(sound_index);
    bool result = true;
    const s_cache_file_sound_definition *definition =
        TAG_GET('snd!', const s_cache_file_sound_definition, sound->definition_index);
    if (tracker && sound_manager_sound_tracking_allowed())
    {
        if (tracker->update(sound->object_index, sound->definition_index, sound->event_parameters, &sound->source.flags))
        {
            result = !sound_manager_flag_test16(sound->source.flags, 14);
        }
        else
        {
            result = !sound_manager_sound_unknown_type(sound_index)
                && !sound_manager_sound_class_stops_when_attached_to_object(sound_class_get(definition->sound_class))
                && !sound_manager_flag_test16(sound->source.flags, 14);
            flags->m_storage |= 0x20;
        }
    }
    return result;
}

// @address 0x13b140 sound_stop_and_unlock
extern "C" void sound_stop_and_unlock(unsigned long sound_index, e_sound_event_stop_reason reason)
{
    sound_datum *sound = sound_manager_sound_get_view(sound_index);
    if (sound->channel_index != -1)
    {
        channel_stop(sound->channel_index);
        sound->channel_index = -1;
    }
    sound_datum_unlock(sound);
    sound_stop(sound_index, reason);
}

// @address 0x13e2d8 ?channel_initialize@@YAXFPBUs_sound_channel_properties@@@Z
void channel_initialize(short channel_index, const s_sound_channel_properties *properties)
{
    sound_channel_datum *channel = sound_manager_channel_get_view(channel_index);
    sound_manager_platform_source_initialize_properties(channel->source_index, properties, &properties->submit_parameters);
}

// @address 0x1a2540 ?sound_initialize_for_new_map@@YAXXZ
void sound_initialize_for_new_map()
{
    cache_file_sound_definitions_initialize_for_new_map();
    sound_reset_promotions();
    sound_manager_globals_view_get()->master_gain = 0.0f;
    sound_manager_globals_view_get()->fade_in_delay_milliseconds = (long)(game_is_campaign() ? 2000.0f : 300.0f);
    sound_manager_globals_view_get()->active = true;
    unsigned long time = GetTickCount();
    s_sound_manager_globals *globals = sound_manager_globals_view_get();
    globals->ducker = -1;
    globals->time = 0;
    globals->started = false;
    globals->previous_ducker = -1;
    globals->reset_time = time;
    globals->environment.reverb_count = 0;
    globals->runtime_reverbs[0].scale = 0.0f;
    globals->runtime_reverbs[0].definition_index = -1;
    globals->runtime_reverbs[0].image.azimuth.lower = 0.0f;
    globals->runtime_reverbs[0].image.azimuth.upper = 6.2831855f;
    globals->runtime_reverbs[1].scale = 0.0f;
    globals->runtime_reverbs[1].definition_index = -1;
    globals->runtime_reverbs[1].image.azimuth.lower = 0.0f;
    globals->runtime_reverbs[1].image.azimuth.upper = 0.0f;
    globals->timeline_duck = 0.0f;
    for (long listener_index = 0; listener_index < 4; listener_index++)
        sound_manager_globals_view_get()->listeners[listener_index].valid = false;
    reset_platform_sound_mix();
}

static __forceinline void sound_manager_sound_start_fade_out(short mode, float seconds, long sound_index);
// @address 0x1a2640 ?sound_dispose_from_old_map@@YAXXZ
void sound_dispose_from_old_map(void)
{
    s_sound_manager_globals *globals = sound_manager_globals_view_get();
    if (!globals->paused && sound_manager_sound_is_active())
    {
        long start = GetTickCount();
        bool found = false;
        for (long sound_index = data_next_index(sound_manager_sound_data_array(), -1);
             sound_index != -1;
             sound_index = inl_data_next_index(sound_manager_sound_data_array(), sound_index))
        {
            sound_manager_sound_start_fade_out(0, 0.3f, sound_index);
            found = true;
        }
        while (found)
        {
            if ((float)GetTickCount() >= (float)start + 0.3f * 1000.0f)
                break;
            main_update_idle_tasks();
            sound_idle();
        }
    }
    sound_stop_all(0);
    globals = sound_manager_globals_view_get();
    globals->active = false;
    if (g_looping_sound_data)
    {
        data_delete_all(g_looping_sound_data);
        globals = sound_manager_globals_view_get();
    }
    if (g_sound_playback_controller_data)
    {
        data_delete_all(g_sound_playback_controller_data);
        globals = sound_manager_globals_view_get();
    }
    if (!main_game_globals.reset_in_progress)
        g_sound_cache_file_gestalt.data = 0;
    globals->environment.reverb_count = 0;
}

// @address 0x5bec98 ?sound_dispose_from_old_structure_bsp@@YAXK@Z
void sound_dispose_from_old_structure_bsp(unsigned long)
{
}

// @address 0x1a2820 ?sound_initialize_for_new_structure_bsp@@YAXK@Z
void sound_initialize_for_new_structure_bsp(unsigned long structure_bsp_mask)
{
    if (game_in_progress())
    {
        for (long listener_index = 0; listener_index < 4; listener_index++)
        {
            s_sound_listener *listener = &sound_manager_globals_view_get()->listeners[listener_index];
            if (listener->valid)
                scenario_location_from_point((s_location *)&listener->location, &listener->matrix.position);
        }
    }
    if (sound_manager_sound_is_active())
    {
        for (long sound_index = inl_data_next_index(sound_manager_sound_data_array(), -1);
             sound_index != -1;
             sound_index = inl_data_next_index(sound_manager_sound_data_array(), sound_index))
        {
            sound_datum *sound = sound_manager_sound_get_view(sound_index);
            if ((sound->source.spatialization & 0xf0) == 0x10)
            {
                s_location location;
                scenario_location_from_point((s_location *)&location, &sound->source.position);
                sound->source.location = location;
            }
        }
        sound_manager_looping_initialize_for_structure_bsp();
    }
}

// @address 0x1a2a70 ?sound_pause@@YAXW4e_sound_pause_state@@_N@Z
void sound_pause(e_sound_pause_state mode, bool paused)
{
    s_sound_manager_globals *globals = sound_manager_globals_view_get();
    globals->updating = true;
    globals->paused = paused;
    switch (mode)
    {
    case _sound_pause_paused:
        for (long channel_index = 0; channel_index < sound_manager_globals_view_get()->channel_count; channel_index++)
            channel_pause((short)channel_index);
        break;
    case _sound_pause_unpaused:
        for (long channel_index = 0; channel_index < sound_manager_globals_view_get()->channel_count; channel_index++)
            sound_manager_channel_flush_for_restore((short)channel_index);
        ::sound_xaudio_xenon_process_events();
        for (long channel_index = 0; channel_index < sound_manager_globals_view_get()->channel_count; channel_index++)
            channel_restore_from_pause((short)channel_index);
        sound_playback_controllers_synch_playback(true);
        unsigned long time = GetTickCount();
        sound_manager_globals_view_get()->reset_time = time;
        break;
    }
    sound_manager_globals_view_get()->updating = false;
}

// @address 0x1a2e30 ?reset_platform_sound_mix@@YAXXZ
void reset_platform_sound_mix()
{
    s_game_globals *game_globals = global_game_globals;
    sound_globals_reference *sounds = game_globals->sound_globals.count() ? game_globals->sound_globals.begin() : 0;
    if (sounds->sound_mix.index != -1)
    {
        const sound_manager_mix_definition *definition = 
                TAG_GET('snmx', const sound_manager_mix_definition, sounds->sound_mix.index);
        const s_sound_global_mix_configuration *global_mix = &definition->global;
        unsigned long speaker_config;
        XAudioGetSpeakerConfig(&speaker_config);
        g_xaudio_mix_master.set_global_mixes(&definition->first, &definition->ambient, global_mix,
            ((speaker_config >> 16) & 1) != 0);
        sound_xaudio_xenon_set_global_gain(global_mix->gain);
        s_sound_manager_globals *globals = sound_manager_globals_view_get();
        globals->solo_player_fade_out_delay = definition->global.solo_player_fade_out_delay;
        globals->solo_player_fade_out_time = definition->global.solo_player_fade_out_time;
        globals->solo_player_fade_in_time = definition->global.solo_player_fade_in_time;
    }
}

// @address 0x1a2f38 ?scripted_sound_predict@@YAXJ@Z
void scripted_sound_predict(long definition_index)
{
    if (definition_index != -1)
    {
        const s_cache_file_sound_definition *definition =
            TAG_GET('snd!', const s_cache_file_sound_definition, definition_index);
        if (definition->pitch_range_count != 0)
        {
            s_cache_file_sound_pitch_range *pitch_range = sound_manager_definition_get_pitch_range(definition, 0);
            if (sound_manager_pitch_range_get_permutation_count(pitch_range) != 0)
            {
                s_cache_file_sound_permutation *permutation = sound_manager_pitch_range_get_permutation(pitch_range, 0);
                _sound_cache_sound_request(definition_index, (s_sound_permutation_chunk *)sound_manager_permutation_get_chunk(permutation,
                    0), 1);
            }
        }
    }
}

// @address 0x1a47f0 sound_update_tracking
extern "C" unsigned char sound_update_tracking(long sound_index)
{
    bool result = true;
    sound_datum *sound = sound_manager_sound_get_view(sound_index);
    if (!sound_manager_flag_test16(sound->flags.m_flags, 0) && !sound_manager_flag_test16(sound->flags.m_flags, 5)
        && sound->callbacks && ((const s_sound_tracker *)sound->callbacks)->update
        && !(sound->start_time > sound_manager_globals_view_get()->time && !sound_manager_globals_view_get()->paused))
        result = sound_track_and_update(
            sound_index, (const s_sound_tracker *)sound->callbacks, (c_flags<e_sound_datum_flag_bits, unsigned short,
                11> *)&sound->flags.m_flags);
    return result;
}

static __forceinline void sound_manager_start_crossfade_fade_out(long fade_out_sound_index)
{
    sound_datum *sound = sound_manager_sound_get(fade_out_sound_index);
    sound->fade_gain_db = sound_calculate_fade_db(fade_out_sound_index);
    sound->fade_type = _sound_fade_mode_crossfade;
    sound->fade_start_time = 300;
    sound->fade_end_time = -1;
    sound->flags.set(7, true);
}

// @address 0x1a35d0 ?sound_stop_impulse@@YAXJ@Z
void sound_stop_impulse(long sound_index)
{
    if (sound_index == -1)
        return;
    s_data_array *data = sound_manager_sound_data_array();
    long index = DATUM_INDEX_TO_ABSOLUTE_INDEX(sound_index);
    if (index >= data->first_unallocated)
        return;
    sound_datum *datum = (sound_datum *)(data->data + index * data->size);
    if (datum->identifier == 0 || datum->identifier != sound_index >> 16)
        return;
    sound_datum *sound = sound_manager_sound_get_view(sound_index);
    if (sound->type != 0)
        return;
    const s_sound_event_callbacks *callbacks = (const s_sound_event_callbacks *)sound->callbacks;
    if (callbacks && callbacks->impulse_stop_proc)
        callbacks->impulse_stop_proc(sound->object_index, sound_index);
    sound_manager_start_crossfade_fade_out(sound_index);
}

// @address 0x1a5498 sound_has_higher_priority
// true when the sound outranks the other sound: higher class priority at its distance, then (same definition) the higher
// priority, then the nearer one
extern "C" bool sound_has_higher_priority(long sound_index, long other_sound_index, float distance)
{
    sound_datum *sound = sound_manager_sound_get(sound_index);
    sound_datum *other_sound = sound_manager_sound_get(other_sound_index);
    long definition_index = sound->definition_index;
    int priority = sound_compute_priority((int)distance,
        (TAG_GET('snd!', const s_cache_file_sound_definition, definition_index))->sound_class);
    const s_sound_source *other_source = &other_sound->source;
    long other_definition_index = other_sound->definition_index;
    signed char other_listener = other_sound->listener_index;
    int other_priority = sound_compute_priority((int)source_distance_squared(other_listener, other_source),
        ((const s_cache_file_sound_definition *)global_tag_instances[
            DATUM_INDEX_TO_ABSOLUTE_INDEX(other_definition_index)].definition)->sound_class);
    return priority > other_priority ||
        priority == other_priority &&
        (definition_index != other_definition_index ? distance < source_distance_squared(other_listener, other_source)
                                                    : sound->priority > other_sound->priority ||
                    sound->priority == other_sound->priority && distance < source_distance_squared(other_listener, other_source));
}

// @address 0x1a5b28 ?refresh_channels@@YAXXZ
void refresh_channels(void)
{
    short index = 0;
    if (sound_manager_globals_view_get()->channel_count > 0)
    {
        do {
            channel_refresh_state(index);
            index = index + 1;
        } while (index < sound_manager_globals_view_get()->channel_count);
    }
}


static __forceinline short sound_manager_permutation_next_chunk(const s_cache_file_sound_permutation *permutation, short chunk_index)
{
    return permutation && chunk_index < permutation->chunk_count - 1 ? chunk_index + 1 : -1;
}

static __forceinline bool sound_manager_platform_source_is_playing(short source_index)
{
    return sound_manager_platform_sources_get()->sources[source_index].state > 0;
}

static __forceinline bool sound_manager_cache_request_test(unsigned long result, long bit)
{
    return ((result >> bit) & 1) != 0;
}

static __forceinline float sound_manager_log2(double value)
{
    return (float)(log(value) * 1.4426950408889634);
}

static __forceinline float sound_manager_exp2(float value)
{
    return (float)exp((double)(value * 0.6931472f));
}

// @address 0x1a7410 ?channel_refresh_state@@YAXF@Z
void channel_refresh_state(short channel_index)
{
    sound_channel_datum *channel = sound_manager_channel_get_view(channel_index);
    if (channel->source_index != -1 && !channel->paused && channel->queued)
    {
        short state = sound_manager_platform_sources_get()->sources[channel->source_index].state;
        if (channel->next && state < 5)
        {
            sound_cache_sound_release((s_sound_permutation_chunk *)sound_manager_permutation_get_chunk(channel->queued,
                channel->queued_chunk));
            if (sound_manager_permutation_next_chunk(channel->queued, channel->queued_chunk) == -1)
                channel->position = 0.0f;
            channel->queued = channel->next;
            channel->queued_chunk = channel->next_chunk;
            channel->next = 0;
            channel->next_chunk = -1;
            if (!sound_manager_cache_request_test(
                    _sound_cache_sound_request(
                        -1, (s_sound_permutation_chunk *)sound_manager_permutation_get_chunk(channel->queued, channel->queued_chunk), 0),
                    1))
                state = 0;
        }
        if (channel->queued && state < 4)
        {
            sound_cache_sound_release((s_sound_permutation_chunk *)sound_manager_permutation_get_chunk(channel->queued,
                channel->queued_chunk));
            channel->queued = 0;
            channel->queued_chunk = -1;
        }
        if (sound_manager_platform_source_is_playing(channel->source_index))
        {
            const sound_datum *sound = sound_manager_sound_get_view(channel->sound_index);
            const s_cache_file_sound_definition *definition =
                TAG_GET('snd!', const s_cache_file_sound_definition, sound->definition_index);
            long sample_rate = sound_manager_gestalt_get()->codecs[definition->platform_index].sample_rate;
            float pitch;
            XAudioSourceVoice_GetPitch(sound_manager_platform_sources_get()->sources[channel->source_index].voice, &pitch);
            unsigned long samples_per_second = sound_sample_rate_samples_per_second[sample_rate];
            float scale = sound_manager_exp2(pitch - sound_manager_log2(44100.0 / (double)samples_per_second));
            channel->position += sound_manager_globals_view_get()->reverb_rate * scale;
        }
    }
}

static __forceinline void sound_manager_static_flag_clear(unsigned long *words, long index)
{
    unsigned long mask = 1UL << (index & 31);
    words[index >> 5] &= ~mask;
}
static __forceinline void sound_manager_voice_release(long index)
{
    short voice_index = (short)index;
    if (voice_index != -1)
        sound_manager_static_flag_clear(sound_manager_globals_view_get()->used_voices, voice_index);
}
// @address 0x1a76d8 ?channel_stop@@YAXF@Z
void channel_stop(short channel_index)
{
    sound_channel_datum *channel = sound_manager_channel_get_view(channel_index);
    channel->sound_index = -1;
    if (channel->next)
    {
        sound_cache_sound_release((s_sound_permutation_chunk *)sound_manager_permutation_get_chunk(channel->next, channel->next_chunk));
        channel->next = 0;
    }
    if (channel->queued)
    {
        sound_cache_sound_release((s_sound_permutation_chunk *)sound_manager_permutation_get_chunk(channel->queued, channel->queued_chunk));
        channel->queued = 0;
    }
    if (channel->source_index != -1)
    {
        platform_sound_source_stop(channel->source_index);
        sound_manager_static_flag_clear(sound_manager_globals_view_get()->used_sources, channel->source_index);
    }
    if (channel->voice_index != -1)
        sound_manager_voice_release(channel->voice_index);
    channel->playback_flags.clear();
    channel->previous_flags.clear();
    channel->source_index = -1;
    channel->hardware_voice_index = -1;
    channel->voice_index = -1;
    channel->paused = false;
}

static __forceinline bool sound_manager_static_flag_test(const unsigned long *words, long index)
{
    return (words[index >> 5] & (1UL << (index & 31))) != 0;
}
static __forceinline void sound_manager_static_flag_set(unsigned long *words, long index)
{
    words[index >> 5] |= 1UL << (index & 31);
}

/* sound_voice_manager_find_or_create_voice: the hardware voice of a source is the voice with the source's index */
static __forceinline long sound_manager_find_or_create_voice(const sound_datum *sound, short source_index)
{
    s_sound_manager_globals *globals = sound_manager_globals_view_get();
    if (source_index >= 0 && source_index < globals->voice_count
        && sound_manager_static_flag_test(globals->free_voices, source_index) && !sound_manager_static_flag_test(globals->used_voices,
            source_index))
        sound_manager_static_flag_set(globals->used_voices, source_index);
    return source_index;
}

/* sound_stop_and_unlock inlined: detach the sound from its channel and stop it */
static __forceinline void sound_manager_sound_stop_detached(unsigned long sound_index, e_sound_event_stop_reason reason)
{
    sound_datum *sound = sound_manager_sound_get_view(sound_index);
    if (sound->channel_index != -1)
    {
        channel_stop(sound->channel_index);
        sound->channel_index = -1;
    }
    sound_datum_unlock(sound);
    sound_stop(sound_index, reason);
}

// @address 0x1a6348 ?channel_touch@@YA_NF@Z
bool channel_touch(short channel_index)
{
    bool result = true;
    sound_channel_datum *channel = sound_manager_channel_get_view(channel_index);
    sound_datum *sound = sound_manager_sound_get_view(channel->sound_index);
    if (channel->source_index == -1 || (sound->playback_flags & 2) && channel->hardware_voice_index == -1)
    {
        short source_index = -1;
        long first_source = sound_manager_globals_view_get()->channel_type_first_source[(signed char)channel->channel_type];
        long last_source = sound_manager_globals_view_get()->channel_type_last_source[(signed char)channel->channel_type];
        s_sound_manager_globals *globals = sound_manager_globals_view_get();
        for (long index = first_source; index < last_source; index++)
        {
            if (sound_manager_static_flag_test(globals->free_sources, index) &&
                !sound_manager_static_flag_test(sound_manager_globals_view_get()->used_sources, index))
            {
                source_index = (short)index;
                break;
            }
        }
        if (source_index == -1)
            result = false;
        else
        {
            short hardware_voice_index = -1;
            long voice_index = -1;
            if (sound->hardware_voice_index != -1)
                hardware_voice_index = sound->hardware_voice_index;
            else if (sound->playback_flags & 2)
            {
                voice_index = sound_manager_find_or_create_voice(sound, source_index);
                if (voice_index != -1)
                    hardware_voice_index = (short)voice_index;
            }
            if (hardware_voice_index == -1 && ((sound->playback_flags & 2) || sound->hardware_voice_index != -1))
                result = false;
            else
            {
                channel->source_index = source_index;
                channel->hardware_voice_index = hardware_voice_index;
                channel->voice_index = voice_index;
                sound_manager_static_flag_set(sound_manager_globals_view_get()->used_sources, channel->source_index);
                sound_manager_static_flag_clear(sound_manager_globals_view_get()->free_sources, channel->source_index);
                result = true;
            }
        }
    }
    if (!result)
    {
        sound_manager_sound_stop_detached(channel->sound_index, _sound_event_stop_reason_no_free_channel);
        channel_stop(channel_index);
    }
    return result;
}

// @address 0x1a65f8 sound_compute_environment_gain
// combines the two gains of the pair (1 - (1-a)(1-b)), lifts the result above the floor and shapes it by the exponent
extern "C" float sound_compute_environment_gain(const s_sound_environment_parameters *pair, float exponent, float floor)
{
    float gain = (1.0f - (1.0f - pair->transmission) * (1.0f - pair->occlusion)) * (1.0f - floor) + floor;
    if (exponent > 0.001f && gain > 0.001f)
        return (float)pow(PIN(gain, 0.0f, 1.0f), 1.0f / exponent);
    return 0.0f;
}

extern "C" {

}
#define k_sound_class_channel_count 5
static __forceinline sound_class_datum *sound_manager_class_datum_get(short sound_class_index)
{
    return &((sound_class_datum *)g_sound_class_data)[sound_class_index];
}
/* real_decibel::operator+= */
static __forceinline real_decibel &sound_manager_real_decibel_add(real_decibel &gain_db, const real_decibel &other)
{
    gain_db.value += other.value;
    return gain_db;
}
static __forceinline real_decibel sound_manager_class_get_gain_db(short sound_class_index)
{
    sound_class_datum *datum = sound_manager_class_datum_get(sound_class_index);
    real_decibel gain_db;
    gain_db.value = 0.0f;
    long channel = 0;
    do
    {
        gain_db += datum->target_gain_db[channel];
        channel++;
    } while (channel < k_sound_class_channel_count);
    return gain_db;
}
static __forceinline bool sound_manager_class_get_ducker_enabled(short sound_class_index)
{
    return (sound_manager_class_datum_get(sound_class_index)->flags & 1) == 0;
}
static __forceinline real_decibel sound_manager_real_decibel_interpolate_linear(real_decibel lower, real_decibel upper, float fraction)
{
    real_decibel result;
    float lower_value = lower.to_real();
    result.value = lower_value + (upper.to_real() - lower_value) * fraction;
    return result;
}

// @address 0xf9810 ?sound_manager_master_gain_db@@YA?AUreal_decibel@@F@Z
real_decibel sound_manager_master_gain_db(short sound_class_index)
{
    real_decibel gain_db = sound_manager_class_get_gain_db(sound_class_index);
    if (!sound_manager_globals_view_get()->ducking_suppressed && sound_manager_class_get_ducker_enabled(sound_class_index))
    {
        const sound_class_definition *definition = sound_class_get(sound_class_index);
        s_sound_manager_globals *globals = sound_manager_globals_view_get();
        if (globals->previous_ducker != -1 && globals->ducker == globals->previous_ducker)
        {
            const sound_class_ducker *ducker = &definition->duckers[globals->ducker];
            float elapsed = globals->ducker_time - globals->previous_ducker_time;
            if (elapsed > ducker->hold_time)
            {
                if (ducker->fade_in_time >= 0.001f)
                {
                    if (ducker->fade_out_time >= 0.001f && ducker->fade_out_time + ducker->hold_time > elapsed)
                    {
                        float fade_out_offset = (elapsed - ducker->hold_time) / ducker->fade_in_time * ducker->fade_out_time;
                        float fraction = MIN((globals->ducker_time - elapsed + fade_out_offset) / ducker->fade_in_time, 1.0f);
                        gain_db += sound_manager_real_decibel_interpolate_linear(k_sound_manager_zero_gain_db, ducker->gain_db, fraction);
                    }
                    else
                    {
                        float fraction = MIN((globals->ducker_time - elapsed) / ducker->fade_in_time, 1.0f);
                        gain_db += sound_manager_real_decibel_interpolate_linear(k_sound_manager_zero_gain_db, ducker->gain_db, fraction);
                    }
                }
            }
            else
            {
                real_decibel ducker_gain_db = ducker->gain_db;
                gain_db += ducker_gain_db;
            }
        }
        else
        {
            if (globals->previous_ducker != -1)
            {
                const sound_class_ducker *ducker = &definition->duckers[globals->previous_ducker];
                if (ducker->fade_out_time >= 0.001f)
                {
                    float fraction = MIN(MAX(globals->previous_ducker_time - ducker->hold_time / ducker->fade_out_time, 0.0f), 1.0f);
                    gain_db += sound_manager_real_decibel_interpolate_linear(ducker->gain_db, k_sound_manager_zero_gain_db, fraction);
                }
            }
            if (globals->ducker != -1)
            {
                const sound_class_ducker *ducker = &definition->duckers[globals->ducker];
                if (ducker->fade_in_time >= 0.001f)
                {
                    float fraction = MIN(globals->ducker_time / ducker->fade_in_time, 1.0f);
                    gain_db += sound_manager_real_decibel_interpolate_linear(k_sound_manager_zero_gain_db, ducker->gain_db, fraction);
                }
            }
        }
    }
    return gain_db;
}




// @address 0x1a36b8 ?sound_manager_set_sound_environment@@YAXPBUs_sound_manager_environment@@@Z
void sound_manager_set_sound_environment(const s_sound_manager_environment *new_environment)
{
    unsigned long used_reverbs[1];
    memset(used_reverbs, 0, sizeof(used_reverbs));
    
    const s_sound_manager_environment *environment = &sound_manager_globals_view_get()->environment;
    for (long new_index = 0; new_index < environment->reverb_count; new_index++)
    {
        const s_sound_manager_reverb *new_reverb = &environment->reverbs[new_index];
        if (new_reverb->definition_index != -1)
        {
            long reverb_index = -1;
            for (long index = 0; index < 2; index++)
            {
                s_sound_manager_reverb *reverb = &sound_manager_globals_view_get()->runtime_reverbs[index];
                if (!sound_manager_static_flag_test(used_reverbs, index))
                {
                    if (reverb->definition_index == new_reverb->definition_index)
                    {
                        reverb_index = index;
                        break;
                    }
                    if (reverb->scale <= 0.001f)
                        reverb_index = index;
                }
            }
            if (reverb_index != -1)
            {
                float rate = sound_manager_globals_view_get()->reverb_rate * new_reverb->inverse_cutoff;
                s_sound_manager_reverb *reverb = &sound_manager_globals_view_get()->runtime_reverbs[reverb_index];
                if (reverb->scale <= 0.001f)
                {
                    reverb->image.azimuth.lower = new_reverb->image.azimuth.lower;
                    reverb->image.azimuth.upper = new_reverb->image.azimuth.upper;
                }
                else
                {
                    float angle_rate = rate * 3.1415927f;
                    interpolate_scalar(&reverb->image.azimuth.lower, new_reverb->image.azimuth.lower, angle_rate);
                    interpolate_scalar(&reverb->image.azimuth.upper, new_reverb->image.azimuth.upper, angle_rate);
                }
                interpolate_scalar(&reverb->image.base_volume, new_reverb->image.base_volume, rate);
                interpolate_scalar(&reverb->image.spread, new_reverb->image.spread, rate);
                interpolate_scalar(&reverb->scale, 1.0f, rate);
                reverb->inverse_cutoff = new_reverb->inverse_cutoff;
                reverb->definition_index = new_reverb->definition_index;
                sound_manager_static_flag_set(used_reverbs, reverb_index);
            }
        }
    }
    unsigned long index = 0;
    do
    {
        if (!sound_manager_static_flag_test(used_reverbs, index))
        {
            s_sound_manager_reverb *reverb = &sound_manager_globals_view_get()->runtime_reverbs[index];
            float rate = sound_manager_globals_view_get()->reverb_rate * reverb->inverse_cutoff;
            interpolate_scalar(&reverb->scale, 0.0f, rate);
        }
        index++;
    } while (index < 2);
}


// Fade out the previous sound when a crossfade has no incoming sound.
static __forceinline void sound_manager_sound_start_fade_out(short mode, float seconds, long sound_index)
{
    if (sound_index != -1)
    {
        sound_datum *sound = sound_manager_sound_get(sound_index);
        sound->fade_gain_db = sound_calculate_fade_db(sound_index);
        sound->fade_type = mode;
        sound->fade_start_time = (long)(seconds * 1000.0f);
        sound->fade_end_time = -1;
        sound->flags.set(7, true);
    }
}
/* channel_get_state */
static __forceinline short sound_manager_channel_get_state(short channel_index)
{
    return sound_manager_platform_sources_get()->sources[sound_manager_channel_get_view(channel_index)->source_index].state;
}

const float k_sound_manager_audible_fade_in_seconds = 0.5f;

// @address 0x1a4888 ?refresh_sounds@@YAXXZ
void refresh_sounds(void)
{
    for (long sound_index = data_next_index(sound_manager_sound_data_array(), -1);
         sound_index != -1;
         sound_index = inl_data_next_index(sound_manager_sound_data_array(), sound_index))
    {
        sound_datum *sound = sound_manager_sound_get(sound_index);
        const s_cache_file_sound_definition *definition =
            (const s_cache_file_sound_definition *)global_tag_instances[DATUM_INDEX_TO_ABSOLUTE_INDEX(sound->definition_index)].definition;
        s_sound_promotion *promotion = definition->promotion_index == -1 ? 0
            : &g_sound_cache_file_gestalt.data->promotions[definition->promotion_index];
        if (sound->channel_index != -1 && sound_manager_channel_get_view(sound->channel_index)->paused)
            continue;
        if (sound->flags.test(11))
            sound_stop(sound_index, (e_sound_event_stop_reason)sound->stop_reason);
        else if (sound->flags.test(9) && sound->flags.test(6)
            && sound_manager_channel_get_state(sound->channel_index) <= 1)
            sound_stop_and_unlock(sound_index, _sound_event_stop_reason_ended);
        else if (promotion && sound->priority != -1 && sound->priority < promotion->current_rule
            && sound->start_time == promotion->last_update_time && sound->start_time < promotion->suppress_until)
            sound_stop_and_unlock(sound_index, _sound_event_stop_reason_promotion);
        else if (!sound_update_tracking(sound_index))
        {
            if (sound_manager_source_test(&sound_manager_sound_get(sound_index)->source, 15))
                sound_manager_sound_start_fade_out(0, 3.0f, sound_index);
            else
                sound_stop_and_unlock(sound_index, _sound_event_stop_reason_refresh_stopped);
        }
        else
        {
            if (*(const unsigned short *)definition & 4)
                sound->source.flags |= 1;
            long listener_index = source_audible_internal(
                &sound->source, 0.0f, sound_manager_sound_source_get_maximum_distance(&sound->source, sound->definition_index), 0);
            if (listener_index == -1)
            {
                if (!sound->flags.test(2))
                {
                    sound_manager_sound_start_fade_out(0, 2.0f, sound_index);
                    sound->flags.set(2, true);
                }
            }
            else
            {
                sound->listener_index = (signed char)listener_index;
                if (sound->flags.test(2))
                {
                    sound_start_fade(0, (sound_event_fade_reason)0, k_sound_manager_audible_fade_in_seconds, sound_index, -1);
                    sound->flags.set(2, false);
                }
            }
        }
    }
}

static __forceinline void sound_manager_platform_source_flag_set(unsigned long *flags, long bit, bool value)
{
    if (value)
        *flags |= 1 << bit;
    else
        *flags &= ~(1 << bit);
}
static __forceinline bool sound_manager_class_flag_test(const sound_class_definition *definition, long bit)
{
    bool result = (definition->flags & (1 << bit)) != 0;
    return result;
}
static __forceinline bool sound_manager_sound_class_is_dialog(short sound_class)
{
    return sound_class == 32 || sound_class == 38;
}
static __forceinline bool sound_manager_sound_class_is_music(short sound_class)
{
    return sound_class == 19 || sound_class == 45 || sound_class == 46 || sound_class == 47 || sound_class == 48;
}
static __forceinline real_decibel sound_manager_real_decibel_add_values(real_decibel a, real_decibel b)
{
    real_decibel result = a;
    result += b;
    return result;
}

static float sound_direct_path_gain = 1.0f;

// @address 0xf9b38 ?channel_properties_initialize_from_sound@@YAXFPBUsound_datum@@PBUs_sound_output_parameters@@PAUs_sound_channel_properties@@@Z
void channel_properties_initialize_from_sound(
    short channel_index, const sound_datum *sound, const s_sound_output_parameters *output, s_sound_channel_properties *properties)
{
    sound_channel_datum *channel = sound_manager_channel_get_view(channel_index);
    long definition_index = sound->definition_index;
    const s_cache_file_sound_definition *definition =
        TAG_GET('snd!', const s_cache_file_sound_definition, definition_index);
    const sound_class_definition *class_definition = inl_sound_class_get(definition->sound_class);
    const real_decibel *class_gain = sound_manager_get_listener(sound->listener_index)->default_acoustics
        ? &class_definition->alternate_gain_db
                                                                                                  : &class_definition->gain_db;
    properties->source_properties.samples_per_second = sound_manager_sound_samples_per_second(definition->platform_index);
    properties->source_properties.sound_class = definition->sound_class;
    properties->source_properties.voice_state = sound->voice_state;
    properties->source_properties.hardware_voice_index = channel->hardware_voice_index;
    properties->source_properties.gain = class_gain[0] + real_gain_to_decibels(g_sound_master_gain[0]);
    properties->source_properties.reverb_gain = class_gain[1] + real_gain_to_decibels(sound_direct_path_gain);
    properties->source_properties.distance = output->distance;
    properties->source_properties.cone_attenuation = output->cone_attenuation;
    sound_source_get_transmission_transform(&sound->source, sound->definition_index, &properties->source_properties.transform);
    properties->source_properties.output_gain = output->gain;
    properties->source_properties.lower = output->lower;
    properties->source_properties.upper = output->upper;
    bool bypass = (sound->playback_flags & 8) != 0;
    sound_manager_platform_source_flag_set(&properties->source_properties.flags.m_storage, 1, sound_manager_class_flag_test(class_definition, 3));
    sound_manager_platform_source_flag_set(&properties->source_properties.flags.m_storage, 3, sound_manager_class_flag_test(class_definition, 1));
    sound_manager_platform_source_flag_set(&properties->source_properties.flags.m_storage, 4, class_definition->stereo_playback_type == 1);
    sound_manager_platform_source_flag_set(&properties->source_properties.flags.m_storage, 2, sound_manager_class_flag_test(class_definition, 1) || bypass);
    sound_manager_platform_source_flag_set(&properties->source_properties.flags.m_storage, 6, sound_manager_class_flag_test(class_definition, 9));
    sound_manager_platform_source_flag_set(&properties->source_properties.flags.m_storage, 7, sound->source.current_spatialization == 0);
    sound_manager_platform_source_flag_set(&properties->source_properties.flags.m_storage, 13,
        sound_manager_class_flag_test(class_definition, 7) && sound_manager_class_flag_test(class_definition, 3));
    sound_manager_platform_source_flag_set(&properties->source_properties.flags.m_storage, 9, (sound->playback_flags & 0x20) != 0);
    sound_manager_platform_source_flag_set(&properties->source_properties.flags.m_storage, 10, (output->flags & 0x40) != 0);
    sound_manager_platform_source_flag_set(&properties->source_properties.flags.m_storage, 12,
        sound_manager_sound_class_is_dialog(definition->sound_class) || sound_manager_sound_class_is_music(definition->sound_class));
    if (output->flags & 0x10)
    {
        sound_manager_platform_source_flag_set(&properties->source_properties.flags.m_storage, 5, true);
        properties->source_properties.output_gains = output->output_gains;
    }
    if (properties->source_properties.voice_state != -1)
        sound_manager_platform_source_flag_set(&properties->source_properties.flags.m_storage, 0, true);
    s_platform_sound_effect_input_parameters input;
    input.values[1] = (float)((sound_manager_globals_view_get()->time - sound->start_time) / 1000);
    input.values[0] = 0.0f;
    input.values[2] = sound->source.volume;
    input.values[3] = output->effect_fraction;
    input.position = output->position;
    platform_sound_build_playback_submit_parameters(definition_index, sound->playback_index, &input, &properties->submit_parameters);
    properties->submit_parameters.random_seed = sound->random_seed;
    const s_sound_source *source = &sound->source;
    if (sound_manager_source_test(source, 8))
        properties->submit_parameters.effect_scale = sqrtf(1.0f - output->effect_fraction) * source->volume;
    else
        properties->submit_parameters.effect_scale = source->volume;
}

// @address 0x1a7170 ?channel_restore_from_pause@@YAXF@Z
void channel_restore_from_pause(short channel_index)
{
    sound_channel_datum *channel = sound_manager_channel_get_view(channel_index);
    if (channel->source_index != -1 && channel->paused)
    {
        g_xaudio_source_manager.source_unpause(channel->source_index);
        if (channel->queued)
        {
            s_sound_permutation_chunk *current = sound_manager_permutation_get_chunk(channel->queued, channel->queued_chunk);
            sound_datum *sound = sound_manager_sound_get_view(channel->sound_index);
            const s_cache_file_sound_definition *definition =
                TAG_GET('snd!', const s_cache_file_sound_definition, sound->definition_index);
            long sample_offset = (long)(sound_manager_sound_samples_per_second(definition->platform_index) * channel->position);
            long previous_samples = 0;
            for (long chunk_index = 0; chunk_index < channel->queued_chunk; chunk_index++)
            {
                s_sound_permutation_chunk *previous = sound_manager_permutation_get_chunk(channel->queued, chunk_index);
                previous_samples += previous->last_sample - previous->first_sample;
            }
            sound_manager_platform_sources_get()->sources[channel->source_index].sample_offset = sample_offset - previous_samples;
            ((c_platform_source_datum *)&sound_manager_platform_sources_get()->sources[channel->source_index])
                ->queue_sound((s_sound_permutation_chunk *)current);
            if (channel->next)
            {
                s_sound_permutation_chunk *next = sound_manager_permutation_get_chunk(channel->next, channel->next_chunk);
                ((c_platform_source_datum *)&sound_manager_platform_sources_get()->sources[channel->source_index])
                    ->queue_sound((s_sound_permutation_chunk *)next);
            }
            sound = sound_manager_sound_get_view(channel->sound_index);
            if (sound_manager_flag_test16(sound->flags.m_flags, 6))
                channel_signal_complete(channel_index);
        }
        channel->paused = false;
    }
}

// match: noalias declaration preserves caller live registers

static __forceinline float sound_manager_calculate_fancy_distance_attenuation(const s_sound_transmission_transform *transform,
    float distance)
{
    if (distance >= transform->attack_distance)
        return calculate_simple_distance_attenuation(transform->minimum_distance, transform->maximum_distance, distance);
    return calculate_simple_distance_attenuation(transform->maximum_distance - transform->attack_distance,
        transform->maximum_distance - transform->silence_distance, transform->maximum_distance - distance);
}
static __forceinline float sound_manager_sound_source_distance_attenuation(
    const s_sound_source *source, long sound_index, const real_point3d *point, float *x_squared)
{
    s_sound_transmission_transform transform;
    sound_source_get_transmission_transform(source, sound_index, &transform);
    *x_squared = point->x * point->x;
    float distance = sqrtf(point->y * point->y + point->z * point->z + *x_squared);
    return sound_manager_calculate_fancy_distance_attenuation(&transform, distance);
}
static __forceinline float sound_manager_sound_source_dequantize_cone_angle(unsigned short quantized)
{
    return dequantize_real(quantized, 0.0f, 3.1415927f, 16, false);
}
static __forceinline const s_sound_playback_parameters *sound_manager_sound_playback_parameters(long sound_index)
{
    return sound_definition_get_playback_parameters(TAG_GET('snd!', const s_cache_file_sound_definition, sound_index));
}
static __forceinline float sound_manager_sound_source_get_inner_cone_angle(const s_sound_source *source, long sound_index)
{
    if (sound_manager_source_test(source, 4))
        return sound_manager_sound_source_dequantize_cone_angle(source->inner_cone_angle);
    return sound_manager_sound_playback_parameters(sound_index)->inner_cone_angle;
}
static __forceinline float sound_manager_sound_source_get_outer_cone_angle(const s_sound_source *source, long sound_index)
{
    if (sound_manager_source_test(source, 5))
        return sound_manager_sound_source_dequantize_cone_angle(source->outer_cone_angle);
    return sound_manager_sound_playback_parameters(sound_index)->outer_cone_angle;
}
static __forceinline real_decibel sound_manager_sound_source_get_outer_cone_gain_db(const s_sound_source *source, long sound_index)
{
    if (sound_manager_source_test(source, 6))
        return source->outer_cone_gain;
    return sound_manager_sound_playback_parameters(sound_index)->outer_cone_gain;
}

// @address 0x1a7940 ?sound_source_calculate_location_rolloff_factor@@YAMPBUs_sound_source@@JPBTreal_point3d@@PBTreal_vector3d@@PAM@Z
float sound_source_calculate_location_rolloff_factor(
    const s_sound_source *source, long sound_index, const real_point3d *point, const real_vector3d *forward, float *out_orientation_factor)
{
    float x_squared;
    float distance_factor = sound_manager_sound_source_distance_attenuation(source, sound_index, point, &x_squared);
    float orientation_factor = 1.0f;
    if (sound_source_has_directional_attenuation(source, sound_index) &&
        point->y * point->y + point->z * point->z + x_squared >= 1.0000001e-6f)
    {
        real_vector3d direction;
        memcpy(&direction, point, sizeof(direction));
        direction.i = -direction.i;
        direction.j = -direction.j;
        direction.k = -direction.k;
        if (normalize3d(&direction) > 0.0f)
        {
            float angle = angle_between_normals3d(forward, &direction);
            float inner_angle = sound_manager_sound_source_get_inner_cone_angle(source, sound_index);
            float outer_angle = sound_manager_sound_source_get_outer_cone_angle(source, sound_index);
            float fraction = normalize_and_clamp_linear(inner_angle, outer_angle, angle, true);
            real_decibel outer_gain_db = sound_manager_sound_source_get_outer_cone_gain_db(source, sound_index);
            float outer_gain = sound_manager_real_decibels_to_gain(outer_gain_db);
            orientation_factor = 1.0f + (outer_gain - 1.0f) * fraction;
        }
    }
    float factor = distance_factor * orientation_factor;
    if (sound_manager_source_test(source, 9))
    {
        factor = 1.0f - factor;
        orientation_factor = 1.0f - orientation_factor;
    }
    *out_orientation_factor = orientation_factor;
    return factor;
}


// @address 0x1a7bd0 sound_stop_all_of_sound_class
// stops every sound whose definition belongs to the sound class
extern "C" void sound_stop_all_of_sound_class(long sound_class)
{
    for (long sound_index = data_next_index(sound_manager_sound_data_array(), -1);
         sound_index != -1;
         sound_index = inl_data_next_index(sound_manager_sound_data_array(), sound_index))
    {
        sound_datum *sound = sound_manager_sound_get_view(sound_index);
        if (sound_class == (TAG_GET('snd!', const s_cache_file_sound_definition, sound->definition_index))->sound_class)
        {
            if (sound->channel_index != -1)
            {
                channel_stop(sound->channel_index);
                sound->channel_index = -1;
            }
            sound_datum_unlock(sound);
            sound_stop(sound_index, _sound_event_stop_reason_cinematic_skipped);
        }
    }
}

// @address 0x1a7cb8 ?sound_definition_hardware_formats_match@@YA_NJJ@Z
bool sound_definition_hardware_formats_match(long definition_index_a, long definition_index_b)
{
    const s_cache_file_sound_definition *definition_b =
        TAG_GET('snd!', const s_cache_file_sound_definition, definition_index_b);
    const s_cache_file_sound_definition *definition_a =
        TAG_GET('snd!', const s_cache_file_sound_definition, definition_index_a);
    return sound_manager_definition_get_encoding(definition_a) == sound_manager_definition_get_encoding(definition_b)
        && sound_manager_definition_get_compression(definition_a) == sound_manager_definition_get_compression(definition_b);
}

// match: noalias declaration preserves caller live registers
#define k_sound_manager_speed_of_sound 111.548553f
static __forceinline const s_sound_listener *sound_manager_listener_get(long listener_index)
{
    return &g_sound_manager_globals->listeners[listener_index];
}

// Clamp Doppler shifts between one eighth and four times the source frequency.
static __forceinline double sound_manager_doppler_ratio_pin(float ratio)
{
    return MAX(ratio, 0.125f) < 4.0f ? (ratio > 0.125f ? (double)ratio : 0.125) : 4.0;
}

// @address 0x129e48 ?calculate_doppler_shift_cents@@YAMJPBUs_sound_location@@@Z
float calculate_doppler_shift_cents(long listener_index, const s_sound_location *sound_location)
{
    const s_sound_location *location = sound_location;
    const s_sound_listener *listener = sound_manager_listener_get(listener_index);
    real_vector3d direction;
    sound_vector_from_points3d(&listener->matrix.position, &location->position, &direction);
    normalize3d(&direction);
    real_vector3d listener_velocity;
    matrix4x3_transform_vector(&listener->matrix, &listener->velocity, &listener_velocity);
    float listener_speed = -dot_product3d(&listener_velocity, &direction);
    float source_speed = dot_product3d(&location->velocity, &direction);
    float denominator = MAX(k_sound_manager_speed_of_sound + source_speed, 0.001f);
    float ratio = (k_sound_manager_speed_of_sound - listener_speed) / denominator;
    return logf((float)sound_manager_doppler_ratio_pin(ratio)) * 1200.0f / 0.6931472f;
}

/* ==== sound_manager: sound_definition_map_gain_db (0xf9390) ==== */
static __forceinline real_decibel sound_manager_sound_definition_get_scale_gain_upper_bound(const s_cache_file_sound_definition *definition)
{
    const sound_class_definition *class_scale =
        inl_sound_class_get(definition->sound_class);
    const s_sound_scale_modifiers *modifier =
        &g_sound_cache_file_gestalt.data->scale_modifiers[definition->scale_index];
    real_decibel scale_gain = modifier->gain_upper;
    real_decibel class_gain = class_scale->scale_gain_upper;
    class_gain = class_gain + scale_gain;
    return class_gain;
}
static __forceinline real_decibel sound_manager_sound_definition_get_scale_gain_lower_bound(const s_cache_file_sound_definition *definition)
{
    const sound_class_definition *class_scale =
        (const sound_class_definition *)inl_sound_class_get(definition->sound_class);
    const s_sound_scale_modifiers *modifier =
        &g_sound_cache_file_gestalt.data->scale_modifiers[definition->scale_index];
    real_decibel scale_gain = modifier->gain_lower;
    real_decibel class_gain = class_scale->scale_gain_lower;
    class_gain = class_gain + scale_gain;
    return class_gain;
}

// @address 0xf9390 ?sound_definition_map_gain_db@@YA?AUreal_decibel@@PBUs_cache_file_sound_definition@@U1@M@Z
inline real_decibel sound_definition_map_gain_db(const s_cache_file_sound_definition *cache_file_sound_definition, real_decibel decibel,
    float fraction)
{
    real_decibel upper = sound_manager_sound_definition_get_scale_gain_upper_bound(cache_file_sound_definition);
    real_decibel lower = sound_manager_sound_definition_get_scale_gain_lower_bound(cache_file_sound_definition);
    return decibel + sound_manager_master_gain_db(cache_file_sound_definition->sound_class)
        + sound_manager_real_decibel_interpolate(_sound_manager_gain_scale_linear, lower, upper, fraction);
}

// @address 0x13ae18 ?channel_queue_sound@@YAXFPAUs_cache_file_sound_permutation@@F@Z
void channel_queue_sound(short channel_index, struct s_cache_file_sound_permutation * _sym_p1, short chunk_index)
{
    sound_channel_datum *channel = sound_manager_channel_get_view(channel_index);
    s_sound_permutation_chunk *chunk = sound_manager_permutation_get_chunk(((s_cache_file_sound_permutation *)_sym_p1), chunk_index);
    if (channel->next)
    {
        sound_cache_sound_release((s_sound_permutation_chunk *)sound_manager_permutation_get_chunk(channel->next, channel->next_chunk));
        channel->next_chunk = -1;
    }
    ((c_platform_source_datum *)&sound_manager_platform_sources_get()->sources[channel->source_index])->queue_sound((s_sound_permutation_chunk *)chunk);
    _sound_cache_sound_request(-1, (s_sound_permutation_chunk *)chunk, 2);
    if (channel->queued)
    {
        channel->next = ((s_cache_file_sound_permutation *)_sym_p1);
        channel->next_chunk = chunk_index;
    }
    else
    {
        channel->queued = ((s_cache_file_sound_permutation *)_sym_p1);
        channel->queued_chunk = chunk_index;
        channel->position = 0.0f;
    }
}

static __forceinline void sound_manager_inverse_transform_normal(const real_matrix4x3 *matrix, const real_vector3d *normal,
    real_vector3d *result)
{
    float i = normal->i;
    float j = normal->j;
    float k = normal->k;
    result->i = matrix->forward.i * i + (matrix->forward.k * k + matrix->forward.j * j);
    result->j = matrix->left.i * i + (matrix->left.k * k + matrix->left.j * j);
    result->k = matrix->up.i * i + (matrix->up.k * k + matrix->up.j * j);
}
// @address 0x13d348 ?sound_calculate_listener_relative_position_and_attenuation@@YAXJPBUs_sound_source@@JPAM1PATreal_point3d@@@Z
void sound_calculate_listener_relative_position_and_attenuation(long listener_index, const s_sound_source *source, long sound_index,
    float *out_attenuation, float *out_cone_attenuation, real_point3d *out_position)
{
    real_vector3d forward = k_real_math_zero_vector_storage;
    real_point3d position;
    const s_sound_listener *listener = sound_manager_get_listener(listener_index);
    if ((unsigned long)(unsigned char)(source->current_spatialization << 4) == 16)
    {
        sound_source_get_world_position(source, listener_index, &position);
        matrix4x3_inverse_transform_point(&listener->matrix, &position, &position);
        if (sound_source_has_directional_attenuation(source, sound_index))
        {
            real_matrix3x3 rotation;
            matrix3x3_rotation_from_quaternion(&rotation, &source->orientation);
            forward = rotation.forward;
            sound_manager_inverse_transform_normal(&listener->matrix, &forward, &forward);
        }
    }
    else
    {
        position = source->position;
        if (sound_source_has_directional_attenuation(source, sound_index))
        {
            real_matrix3x3 rotation;
            matrix3x3_rotation_from_quaternion(&rotation, &source->orientation);
            forward = rotation.forward;
        }
    }
    *out_attenuation = sound_source_calculate_location_rolloff_factor(source, sound_index, &position, &forward, out_cone_attenuation);
    *out_position = position;
}
static __forceinline bool sound_manager_channel_encoding_matches(long encoding, const sound_channel_datum *channel)
{
    bool matches = true;
    if (encoding != (signed char)channel->channel_type)
        matches = false;
    return matches;
}
// @address 0x1a5210 sound_find_preemption_channel
// Keep the observed two-argument ABI while naming the preemption search.
extern "C" short sound_find_preemption_channel(long sound_index, e_sound_event_stop_reason *reason)
{
    short best_channel = -1;
    long best_sound = -1;
    sound_datum *sounds = ((sound_datum *)g_sound_data->data);
    const sound_datum *sound = &sounds[DATUM_INDEX_TO_ABSOLUTE_INDEX(sound_index)];
    const s_cache_file_sound_definition *definition =
        TAG_GET('snd!', const s_cache_file_sound_definition, sound->definition_index);
    float distance = source_distance_squared(sound->listener_index, &sound->source);
    short channel_index = 0;
    long channel_count = sound_manager_globals_view_get()->channel_count;
    *reason = _sound_event_stop_reason_unknown;
    if (channel_count > 0)
    {
        float best_distance;
        long encoding = (signed char)sound_manager_gestalt_get()->codecs[definition->platform_index].encoding;
        do
        {
            sound_channel_datum *channel = channel_get(channel_index);
            if (sound_manager_channel_encoding_matches(encoding, channel))
            {
                long other_index = channel->sound_index;
                if (other_index == -1)
                {
                    return channel_index;
                }
                if (sound_has_higher_priority(sound_index, other_index, distance))
                {
                    if (best_channel == -1 || sound_has_higher_priority(best_sound, other_index, best_distance))
                    {
                        *reason = _sound_event_stop_reason_preempted_less_important;
                        best_channel = channel_index;
                        best_sound = other_index;
                        const sound_datum *other = datum_get(sounds, other_index);
                        best_distance = source_distance_squared(other->listener_index, &other->source);
                    }
                }
            }
            channel_index++;
        } while (channel_index < channel_count);
    }
    return best_channel;
}

// @address 0x74b830 k_sound_distance_delay_factor
float k_sound_distance_delay_factor = 8.9647064f;

static __forceinline float sound_manager_real_random(unsigned long *seed)
{
    random_seed_next(seed);
    return 0.000015259022f * ((float)(*seed >> 16));
}
static __forceinline const s_sound_playback_parameters *sound_manager_definition_get_impulse_playback(const s_cache_file_sound_definition *definition)
{
    return &g_sound_cache_file_gestalt.data->playback_parameters[definition->playback_index];
}
static __forceinline real_decibel sound_definition_random_gain_db(const s_cache_file_sound_definition *definition)
{
    const s_sound_playback_parameters *playback = sound_manager_definition_get_impulse_playback(definition);
    float fraction = sound_manager_real_random(get_local_random_seed_address());
    return playback->gain_base + sound_manager_real_decibel_interpolate_linear(k_sound_manager_zero_gain_db, playback->gain_variance,
        fraction);
}
static __forceinline float sound_definition_random_pitch(const s_cache_file_sound_definition *definition)
{
    const s_sound_playback_parameters *playback = sound_manager_definition_get_impulse_playback(definition);
    return _real_random_range(get_local_random_seed_address(), (float)playback->pitch_lower,
        (float)playback->pitch_upper);
}

static __forceinline short sound_definition_get_pitch_range_from_promotion_index(const s_cache_file_sound_definition *definition,
    long promotion_index)
{
    short pitch_range_index = 0;
    signed char index = definition->promotion_index;
    const s_sound_promotion *promotion = index != -1 ? &g_sound_cache_file_gestalt.data->promotions[index] : 0;
    if (promotion) {
        short rule_pitch_range = promotion->rules[promotion_index].pitch_range_index;
        if (rule_pitch_range != -1)
            pitch_range_index = rule_pitch_range;
    }
    return pitch_range_index;
}
static __forceinline unsigned char sound_manager_source_flag_test(const s_sound_source *source, long bit)
{
    unsigned char result = (source->flags >> bit) & 1;
    return result;
}
static __forceinline bool sound_manager_definition_ignores_delay(long definition_index)
{
    const s_cache_file_sound_definition *definition = TAG_GET('snd!', const s_cache_file_sound_definition, definition_index);
    bool result = (definition->flags >> 9) & 1;
    return result;
}
static __forceinline long sound_manager_impulse_get_delay(long definition_index, long listener_index, const s_sound_source *source)
{
    float delay = sound_manager_definition_ignores_delay(definition_index)
        ? 0.0f
        : sqrtf(source_distance_squared(listener_index, source)) * k_sound_distance_delay_factor;
    return (long)delay;
}
// @address 0x1a2fe0 ?sound_impulse_new_internal@@YAJJJPBUs_sound_impulse_definition@@J@Z
long sound_impulse_new_internal(long definition_index, long listener_index, const s_sound_impulse_definition *impulse, long promotion_index)
{
    s_cache_file_sound_definition *definition = TAG_GET('snd!', s_cache_file_sound_definition, definition_index);
    real_decibel gain = sound_definition_random_gain_db(definition) + ((impulse->flags & (1 << 7)) ? impulse->gain
        : k_sound_manager_zero_gain_db);
    real_decibel output_gain = gain + sound_definition_map_gain_db(definition, impulse->source.gain, impulse->source.volume);
    long sound_index = -1;
    if (output_gain > k_sound_manager_minimum_gain_db) {
        sound_index = datum_new(sound_manager_sound_data_array());
        if (sound_index != -1) {
            sound_datum *sound = datum_get<sound_datum>(g_sound_data, sound_index);
            long delay;
            if (!(impulse->flags & (1 << 11)) && sound_manager_globals_view_get()->listeners[listener_index].valid) {
                delay = sound_manager_impulse_get_delay(definition_index, listener_index, &impulse->source);
            } else {
                delay = 0;
            }
            sound->type = 0;
            sound->channel_index = -1;
            sound->playback_flags = (unsigned short)impulse->playback_flags;
            sound->listener_index = (signed char)listener_index;
            sound->pitch = sound_definition_random_pitch(definition);
            sound->gain = gain;
            sound->random_seed = *get_local_random_seed_address();
            sound->flags.set_unsafe((unsigned short)((impulse->flags & (1 << 1)) ? impulse->datum_flags : 0));
            sound->object_index = (impulse->flags & (1 << 3)) ? impulse->object_index : -1;
            sound->source = impulse->source;
            if (definition->flags & (1 << 2))
                sound->source.flags |= 1;
            bool ignore_object_obstruction = (sound_class_get(definition->sound_class)->flags >> 2) & 1;
            if (ignore_object_obstruction)
                sound->source.flags |= 128;
            sound->hardware_voice_index = (signed char)((impulse->flags & (1 << 6)) ? impulse->hardware_voice : -1);
            sound->voice_state = (signed char)((impulse->flags & (1 << 9)) ? impulse->voice_state : -1);
            sound->playback_index = (impulse->flags & (1 << 8)) ? impulse->playback_index : -1;
            if (impulse->flags & (1 << 4)) {
                sound->playback_controller_index = sound_playback_controller_find_add_reference(impulse->controller_index);
                if (sound->playback_controller_index != -1) {
                    c_sound_playback_controller *controller = sound_manager_playback_controller_get(sound->playback_controller_index);
                    controller->add_synch_count(1);
                }
            } else {
                sound->playback_controller_index = -1;
            }
            ((signed char &)sound->synchronized_playback) = -1;
            if (impulse->flags & (1 << 5)) {
                sound->callbacks = (const s_sound_event_callbacks *)impulse->callbacks;
                long size = (unsigned long)impulse->event_size > sizeof(sound->event_parameters) ? sizeof(sound->event_parameters)
                    : impulse->event_size;
                memcpy(sound->event_parameters, impulse->event_parameters, size);
            } else {
                sound->callbacks = 0;
            }
            if (impulse->flags & (1 << 10)) {
                sound_set_permutation(sound_index, definition_index, impulse->pitch_range, impulse->permutation, 0);
            } else {
                short pitch_range_index;
                if (promotion_index != -1)
                    pitch_range_index = sound_definition_get_pitch_range_from_promotion_index(definition, promotion_index);
                else
                    pitch_range_index = sound_definition_find_pitch_range_by_pitch(
                        definition, sound_definition_map_pitch(definition, sound->pitch, impulse->source.volume), -1);
                short permutation_index = sound_definition_next_permutation(definition, pitch_range_index);
                sound_set_permutation(sound_index, definition_index, pitch_range_index, permutation_index, 0);
            }
            sound->priority = (signed char)promotion_index;
            sound->fade_end_time = 0;
            sound->fade_start_time = 0;
            if (delay > 100 && !sound_manager_source_flag_test(&sound->source, 8) && !sound_manager_source_flag_test(&sound->source, 11)) {
                sound->start_time = sound_manager_globals_view_get()->time + delay;
                sound->flags.set(0, true);
            } else {
                sound->start_time = sound_manager_globals_view_get()->time;
            }
            sound_make_permutation_ready((sound_datum *)sound, 0);
        }
    }
    return sound_index;
}


// @address 0x1a4c08 ?prioritize_sounds@@YAXXZ
void prioritize_sounds(void)
{
    for (long sound_index = data_next_index(sound_manager_sound_data_array(), -1);
         sound_index != -1;
         sound_index = inl_data_next_index(sound_manager_sound_data_array(), sound_index))
    {
        sound_datum *sound = sound_manager_sound_get(sound_index);
        if (sound->start_time > g_sound_manager_globals->time)
            continue;
        bool permutation_pending = false;
        bool permutation_ready = false;
        if (sound_make_permutation_ready(sound, &permutation_pending))
            permutation_ready = true;
        if (permutation_ready)
        {
            e_sound_event_stop_reason channel_stop_reason;
            short channel_index = sound_find_channel(sound_index, &channel_stop_reason);
            if (channel_index != -1)
            {
                sound_channel_datum *channel = channel_get(channel_index);
                if (channel->sound_index != sound_index)
                {
                    if (channel->sound_index != -1)
                    {
                        sound_stop_and_unlock(channel->sound_index, channel_stop_reason);
                        channel_stop(channel_index);
                    }
                    channel->sound_index = sound_index;
                    sound->start_time = g_sound_manager_globals->time;
                    sound->channel_index = channel_index;
                }
                if (!sound->flags.test(5) && sound->callbacks && sound->callbacks->start_proc)
                    sound->callbacks->start_proc(sound->object_index, sound->definition_index, sound_index, sound->event_parameters);
                continue;
            }
            sound_stop_and_unlock(sound_index, _sound_event_stop_reason_no_free_channel);
        }
        else if (permutation_pending)
        {
            sound_stop_and_unlock(sound_index, _sound_event_stop_reason_io_failure);
        }
        else
        {
            if (sound->channel_index != -1 || g_sound_manager_globals->time - sound->start_time <= 49)
                continue;
            s_cache_file_sound_definition *definition =
                (s_cache_file_sound_definition *)global_tag_instances[DATUM_INDEX_TO_ABSOLUTE_INDEX(sound->definition_index)].definition;
            short cache_miss_mode = sound->flags.test(4) ? 1 : sound_class_get(definition->sound_class)->cache_miss_mode;
            switch (cache_miss_mode)
            {
            case _sound_cache_miss_mode_discard:
            {
                sound_datum *view = (sound_datum *)sound;
                s_cache_file_sound_pitch_range *pitch_range = (s_cache_file_sound_pitch_range *)&sound_manager_gestalt_get()
                                                                  ->pitch_ranges[definition->pitch_range_index + view->pitch_range_index];
                if (!sound_class_get(definition->sound_class)->is_deterministic() &&
                    pitch_range->get_discarded_permutation_index(definition) == -1)
                    pitch_range->set_discarded_permutation_index(definition, view->permutation_index);
                sound_stop_and_unlock(sound_index, _sound_event_stop_reason_not_cached);
                break;
            }
            case _sound_cache_miss_mode_postpone:
                continue;
            }
        }
    }
}

// @address 0x1a4ea8 ?sound_find_channel@@YAFJPAW4e_sound_event_stop_reason@@@Z
// match: 0x1a4ea8: pointer __restrict qualifier prevents alias reload
short sound_find_channel(long sound_index, e_sound_event_stop_reason *__restrict reason)
{
    sound_datum *sound = sound_manager_sound_get(sound_index);
    *reason = _sound_event_stop_reason_unknown;
    short channel_index = sound->channel_index;
    if (channel_index == -1) {
        const s_cache_file_sound_definition *definition = TAG_GET('snd!', const s_cache_file_sound_definition, sound->definition_index);
        const sound_class_definition *sound_class = sound_class_get(definition->sound_class);
        if (sound_manager_sound_class_is_exclusive_per_object(sound_class) && sound->object_index != -1) {
            long other_sound_index = -1;
            short other_channel_index = -1;
            short matching_channel = -1;
            for (short index = 0; index < g_sound_manager_globals->channel_count; ++index) {
                sound_channel_datum *channel = channel_get(index);
                if (channel->sound_index != -1) {
                    sound_datum *other = sound_manager_sound_get(channel->sound_index);
                    if (other->object_index == sound->object_index) {
                        const s_cache_file_sound_definition *other_definition = TAG_GET('snd!', const s_cache_file_sound_definition,
                            other->definition_index);
                        if (sound_manager_sound_class_is_exclusive_per_object(sound_class_get(other_definition->sound_class))) {
                            bool same_type = true;
                            if (g_sound_cache_file_gestalt.data->codecs[definition->platform_index].encoding != channel->channel_type)
                                same_type = false;
                            if (same_type) {
                                matching_channel = index;
                                *reason = _sound_event_stop_reason_preempted_speech;
                                sound->source.current_spatialization = other->source.current_spatialization;
                                break;
                            }
                            other_channel_index = index;
                            other_sound_index = channel->sound_index;
                        }
                    }
                }
            }
            if (matching_channel != -1) {
                long replacement_sound = -1;
                short count = 0;
                short *channels = 0;
                sound_channel_summary summary;
                sound_channel_summary_build(&summary, sound_index);
                e_sound_event_stop_reason stop_reason = _sound_event_stop_reason_unknown;
                if (summary.object_channel_count >= summary.maximum_object_channels) {
                    stop_reason = _sound_event_stop_reason_too_many_same_attached_to_object;
                    channels = summary.object_channels;
                    count = summary.object_channel_count;
                } else if (summary.like_channel_count >= summary.maximum_like_channels) {
                    stop_reason = _sound_event_stop_reason_too_many_same_playing;
                    channels = summary.like_channels;
                    count = summary.like_channel_count;
                }
                if (count > 0) {
                    short replacement = sound_find_like_channel(sound_index, channels, count);
                    if (replacement == -1) matching_channel = -1;
                    else replacement_sound = channel_get(replacement)->sound_index;
                }
                if (matching_channel != -1) {
                    if (replacement_sound != -1) sound_stop_and_unlock(replacement_sound, stop_reason);
                    if (other_sound_index != -1 && replacement_sound != -1) sound_stop_and_unlock(other_sound_index,
                        _sound_event_stop_reason_preempted_speech);
                    if (other_channel_index != -1) channel_stop(other_channel_index);
                }
                return matching_channel;
            }
        }
        sound_channel_summary summary;
        sound_channel_summary_build(&summary, sound_index);
        if (summary.like_sound_started_this_tick) {
            *reason = _sound_event_stop_reason_too_many_same_attached_to_object;
            return -1;
        }
        if (summary.object_channel_count >= summary.maximum_object_channels) {
            *reason = _sound_event_stop_reason_too_many_same_attached_to_object;
            return sound_find_like_channel(sound_index, summary.object_channels, summary.object_channel_count);
        }
        if (summary.like_channel_count >= summary.maximum_like_channels) {
            *reason = _sound_event_stop_reason_too_many_same_playing;
            return sound_find_like_channel(sound_index, summary.like_channels, summary.like_channel_count);
        }
        channel_index = sound_find_preemption_channel(sound_index, reason);
    }
    return channel_index;
}


static __forceinline unsigned char sound_manager_transmission_class_flag(const sound_class_definition *definition)
{
    unsigned char result = (definition->flags >> 6) & 1;
    return result;
}
static __forceinline void sound_manager_environment_clear(s_sound_environment_parameters *environment)
{
    environment->obstruction =  0.0f;
    environment->transmission =  0.0f;
    environment->occlusion = 0.0f;
    environment->low_frequency_transmission = 0.0f;
    environment->material_index = -1;
}
/* the datum's s_sound_source begins at tracker_flags (+0x18) */
static __forceinline const s_sound_source *sound_manager_sound_get_source(const sound_datum *sound)
{
    return &sound->source;
}
// @address 0x1a6698 ?channel_refresh_location@@YAXFPBUs_sound_environment_parameters@@@Z
void channel_refresh_location(short channel_index, const s_sound_environment_parameters *environment)
{
    sound_channel_datum *channel = sound_manager_channel_get_view(channel_index);
    sound_datum *sound = sound_manager_sound_get_view(channel->sound_index);
    channel->previous_flags = channel->playback_flags;
    channel->playback_flags.clear();
    long type = sound_manager_sound_get_source(sound)->current_spatialization;
    if ((unsigned long)type >= 1) {
        if ((unsigned long)type >= 3 || !(sound->playback_flags & 2) || channel->hardware_voice_index == -1) return;
        s_sound_listener *listener = &sound_manager_globals_view_get()->listeners[sound->listener_index];
        s_platform_sound_voice_update_definition update;
        real_point3d position;
        real_vector3d velocity;
        if (type == 1) {
            sound_source_get_world_position(sound_manager_sound_get_source(sound), sound->listener_index, &position);
            matrix4x3_inverse_transform_point(&listener->matrix, &position, &position);
            matrix4x3_inverse_transform_vector(&listener->matrix, &sound->source.velocity, &velocity);
        } else {
            position = sound->source.position;
            velocity = sound->source.velocity;
        }
        const s_cache_file_sound_definition *definition = TAG_GET('snd!', const s_cache_file_sound_definition, sound->definition_index);
    float zz0 = 0.0f;
    float zz1 = 0.0f;
    float zz2 = 0.0f;
    float zz3 = 0.0f;
    float zz4 = 0.0f;
    float zz5 = 0.0f;
    float zz6 = 0.0f;
    float zz7 = 0.0f;
    float zz8 = 0.0f;
    float zz9 = 0.0f;
    float zz10 = 0.0f;
    float zz11 = 0.0f;
    float zz12 = 0.0f;
    float zz13 = 0.0f;
    float zz14 = 0.0f;
    float zz15 = 0.0f;
    float zz16 = 0.0f;
    float zz17 = 0.0f;
    float zz18 = 0.0f;
    float zz19 = 0.0f;
    float zz20 = 0.0f;
    float zz21 = 0.0f;
    float zz22 = 0.0f;
    float zz23 = 0.0f;
    float zz24 = 0.0f;
    float zz25 = 0.0f;
    float zz26 = 0.0f;
    float zz27 = 0.0f;
    float zz28 = 0.0f;
    float zz29 = 0.0f;
    float zz30 = 0.0f;
    float zz31 = 0.0f;
    float zz32 = 0.0f;
    float zz33 = 0.0f;
    float zz34 = 0.0f;
    float zz35 = 0.0f;
    float zz36 = 0.0f;
    float zz37 = 0.0f;
    float zz38 = 0.0f;
    float zz39 = 0.0f;
    float zz40 = 0.0f;
    float zz41 = 0.0f;
    float zz42 = 0.0f;
    float zz43 = 0.0f;
    float zz44 = 0.0f;
    float zz45 = 0.0f;
    float zz46 = 0.0f;
    float zz47 = 0.0f;
    float zz48 = 0.0f;
    float zz49 = 0.0f;
    float zz50 = 0.0f;
    float zz51 = 0.0f;
    float zz52 = 0.0f;
    float zz53 = 0.0f;
    float zz54 = 0.0f;
    float zz55 = 0.0f;
    float zz56 = 0.0f;
    float zz57 = 0.0f;
    float zz58 = 0.0f;
    float zz59 = 0.0f;
    float zz60 = 0.0f;
    float zz61 = 0.0f;
    float zz62 = 0.0f;
    float zz63 = 0.0f;
    float zz64 = 0.0f;
    float zz65 = 0.0f;
    float zz66 = 0.0f;
    float zz67 = 0.0f;
    float zz68 = 0.0f;
    float zz69 = 0.0f;
        const sound_class_definition *sound_class = (const sound_class_definition *)sound_class_get(definition->sound_class);
        update.listener = sound->listener_index;
        update.position = position;
        update.velocity = velocity;
        update.forward = *global_forward3d;
        update.up = *global_up3d;
        if (environment) {
            const float *coefficients = listener->default_acoustics ? sound_class->alternate : sound_class->normal;
            float floor = sound_class->environment_floor * sound_manager_globals_view_get()->timeline_duck;
            float direct = (1.0f - (1.0f - environment->transmission) * (1.0f - environment->obstruction)) * (1.0f - floor) + floor;
            float exponent = sound_class->exponent;
            if (exponent > 0.001f && direct > 0.001f) direct = (float)pow(PIN(direct, 0.0f, 1.0f), 1.0f / exponent);
            else direct = 0.0f;
            float reverb = sound_compute_environment_gain((const s_sound_environment_parameters *)environment, exponent, floor);
            float direct_lpf = coefficients[2] + (1.0f - coefficients[2]) * direct;
            float reverb_lpf = coefficients[3] + (1.0f - coefficients[3]) * reverb;
            update.direct_lpf = PIN(direct_lpf, 0.0f, 1.0f);
            update.reverb_lpf = PIN(reverb_lpf, 0.0f, 1.0f);
            real_decibel gain_db = real_gain_to_decibels(g_sound_master_gain[0]);
            real_decibel class_gain_db = *sound_manager_sound_class_get_gain_db(definition->sound_class, listener->default_acoustics);
            update.gain = class_gain_db.value + gain_db.value;
            const sound_class_definition *rate_class = (const sound_class_definition *)sound_class_get(definition->sound_class);
            update.reverb_rate = sound_manager_globals_view_get()->reverb_rate * (rate_class->direct_rate == 0.0f ? 2.0f
                : rate_class->direct_rate);
            update.direct_rate = sound_manager_globals_view_get()->reverb_rate * (rate_class->reverb_rate != 0.0f ? rate_class->reverb_rate
                : 2.0f);
        } else {
            update.direct_lpf = 0.0f;
            update.reverb_lpf = 0.0f;
            real_decibel gain_db = real_gain_to_decibels(g_sound_master_gain[0]);
            real_decibel class_gain_db = *sound_manager_sound_class_get_gain_db(definition->sound_class, listener->default_acoustics);
            update.gain = class_gain_db.value + gain_db.value;
            update.reverb_rate = 0.0f;
            update.direct_rate = 0.0f;
        }
        update.flags = 0;
        if (listener->default_acoustics) update.flags |= 1; else update.flags &= ~1;
        update.flags |= 2;
        if (sound->start_time == sound_manager_globals_view_get()->time) update.flags |= 4; else update.flags &= ~4;
        c_xenon_audio_voice_manager *manager = &g_xaudio_voice_manager;
        long voice = channel->hardware_voice_index;
        if (manager->listener_flags.test(update.listener)) {
            long channels = manager->source_manager->sources[voice].sound_channel_count;
            manager->speakers[voice].calculate(manager->x3d_instance, channels, &manager->listeners[update.listener], &update,
                0.033333335f);
        }
    } else if (sound_manager_source_test(sound_manager_sound_get_source(sound), 12)) {
        if (environment) {
            const s_cache_file_sound_definition *definition = TAG_GET('snd!', const s_cache_file_sound_definition, sound->definition_index);
            const sound_class_definition *sound_class = (const sound_class_definition *)sound_class_get(definition->sound_class);
            s_sound_listener *listener = &sound_manager_globals_view_get()->listeners[sound->listener_index];
            const float *coefficients = listener->default_acoustics ? sound_class->alternate : sound_class->normal;
            float attenuation = sound_compute_environment_gain((const s_sound_environment_parameters *)environment, sound_class->exponent,
                sound_manager_globals_view_get()->timeline_duck * sound_class->environment_floor);
            float transmission = coefficients[3] + (1.0f - coefficients[3]) * attenuation;
            sound->source.volume *= 1.0f - PIN(transmission, 0.0f, 1.0f);
        } else {
            long bsp = sound->source.location.cluster_reference.bsp_index;
            if (!structure_bsp_is_active(bsp)) sound->source.volume = 0.0f;
        }
    }
}
// @address 0x1a5b98 ?touch_channels@@YAXPAUs_game_sound_obstruction_parameters@@PAV?$c_static_array@PBUs_sound_environment_parameters@@$0ME@@@1@Z
void touch_channels(s_game_sound_obstruction_parameters *out_obstruction_parameters,
    c_static_array<const s_sound_environment_parameters *, 196> *out_voice_map,
    c_static_array<const s_sound_environment_parameters *, 196> *out_source_map)
{
    c_static_flags<256> touched_voices;
    touched_voices.clear();
    for (long channel_index = 0; channel_index < sound_manager_globals_view_get()->channel_count; ++channel_index)
    {
        if (out_obstruction_parameters->full())
            return;
        sound_channel_datum *channel = sound_manager_channel_get_view((short)channel_index);
        if (channel->sound_index != -1 && !channel->paused && channel_touch((short)channel_index))
        {
            sound_datum *sound = datum_get<sound_datum>(g_sound_data, channel->sound_index);
            bool spatial = sound->source.current_spatialization == 1 && !sound->flags.test(2)
                && !sound_manager_source_test(&sound->source, 7) && !sound_manager_source_test(&sound->source, 0);
            bool ambient = sound->source.current_spatialization == 0 && !sound->flags.test(2)
                && sound_manager_source_test(&sound->source, 12) &&
                    structure_bsp_is_active(sound->source.location.cluster_reference.bsp_index);
            if (spatial)
            {
                short voice_index = channel->hardware_voice_index;
                if (voice_index != -1 && !touched_voices.test(voice_index))
                {
                    touched_voices.set(voice_index, true);
                    out_obstruction_parameters->push();
                    const s_cache_file_sound_definition *definition =
                (const s_cache_file_sound_definition *)global_tag_instances[DATUM_INDEX_TO_ABSOLUTE_INDEX(sound->definition_index)].definition;
                    s_game_sound_transmission_source *source = out_obstruction_parameters->get_top();
                    source->listener_index = sound->listener_index;
                    source->position = sound->source.position;
                    source->location = sound->source.location;
                    source->portals_ignored = false;
                    source->no_obstruction = sound_manager_source_test(&sound->source, 7);
                    source->has_obstruction_points = false;
                    source->use_audibility = sound_manager_source_test(&sound->source, 13)
                        || sound_manager_transmission_class_flag(sound_class_get(definition->sound_class))
                        || (definition->flags & 16) != 0;
                    sound_manager_environment_clear(&source->environment);
                    source->environment.transmission_multiplier = sound_class_get(definition->sound_class)->transmission_multiplier;
                    (*out_voice_map)[channel->hardware_voice_index] = &source->environment;
                }
            }
            else if (ambient && channel->source_index != -1)
            {
                out_obstruction_parameters->push();
                s_game_sound_transmission_source *source = out_obstruction_parameters->get_top();
                source->listener_index = sound->listener_index;
                source->position = *global_origin3d;
                source->location = sound->source.location;
                source->portals_ignored = true;
                source->no_obstruction = true;
                source->has_obstruction_points = false;
                source->use_audibility = true;
                source->location.leaf_index = -1;
                sound_manager_environment_clear(&source->environment);
                source->environment.transmission_multiplier =  1.0f;
                (*out_source_map)[channel->source_index] = &source->environment;
            }
        }
    }
}


// @address 0xf8908 ?sound_stop_all@@YAXJ@Z
void sound_stop_all(long flags)
{
    bool stop_sounds = TEST_BIT(flags, 4);
    bool keep_attached_sounds = TEST_BIT(flags, 5);
    s_sound_manager_globals *globals = g_sound_manager_globals;
    if (!stop_sounds) {
        if (globals->paused) {
            sound_pause((e_sound_pause_state)2, false);
            globals = g_sound_manager_globals;
        }
    }
    if (stop_sounds && !keep_attached_sounds)
        globals->master_gain = 0.0f;
    if (globals->initialized) {
        sound_effect_manager_stop_all();
        for (long sound_index = data_next_index(sound_manager_sound_data_array(), -1); sound_index != -1;
             sound_index = inl_data_next_index(sound_manager_sound_data_array(), sound_index)) {
            sound_datum *sound = datum_get<sound_datum>(g_sound_data, sound_index);
            bool should_stop = true;
            if (stop_sounds) {
                const s_cache_file_sound_definition *definition = TAG_GET('snd!', const s_cache_file_sound_definition,
                    sound->definition_index);
                long sound_class = definition->sound_class;
                if (!sound->flags.test(10) || !keep_attached_sounds) {
                    if (sound_manager_sound_class_is_dialog(sound_class)) {
                        if (sound_class != 38)
                            should_stop = false;
                        else
                            should_stop = !keep_attached_sounds;
                    } else if (sound_manager_sound_unknown_type(sound_index) && sound_class_get(sound_class)->stereo_playback_type == 1) {
                        should_stop = false;
                    }
                } else {
                    should_stop = false;
                }
            }
            if (should_stop)
                sound_stop_and_unlock((unsigned long)sound_index, _sound_event_stop_reason_all);
            else
                sound->start_time = -1;
        }
        if (!stop_sounds) {
            long source_count = g_xaudio_source_manager.source_count;
            for (long source_index = 0; source_index < source_count; source_index++)
                g_xaudio_source_manager.source_stop(source_index);
            do {
                sound_xaudio_xenon_process_events();
                g_xaudio_source_manager.begin_scene();
                g_xaudio_source_manager.end_scene();
            } while (g_xaudio_source_manager.any_active);
        } else {
            sound_idle();
        }
    }
    unsigned long tick_count = GetTickCount();
    globals = g_sound_manager_globals;
    globals->reset_time = (long)tick_count;
    if (!stop_sounds)
        globals->time = 0;
    globals->ducker = -1;
    globals->previous_ducker = -1;
    sound_reset_promotions();
    g_game_sound_globals->scripted_dialog_timer = 0;
    if (!stop_sounds)
        looping_sound_manager_reset();
}

// ---- sound_idle (0x1a2c18) supporting views ----
struct s_synchronization_globals;
struct s_lruv_cache;
extern "C" {

// match: 0x13efb0: SDK header adoption is blocked by this unit's current types/API arguments: 'DWORD' : redefinition; different basic types.


}

// @address 0x1a2c18 ?sound_idle@@YAXXZ
void sound_idle(void)
{
    unsigned long now = GetTickCount();
    s_sound_manager_globals *globals = sound_manager_globals_view_get();
    long elapsed = (long)now - (long)globals->reset_time;
    if (game_in_progress()) {
        float elapsed_seconds = (float)(double)elapsed;
        if (elapsed_seconds > game_time_globals->tick_length * 1000.0f || g_sound_always_update) {
            bool latched = globals->updating;
            globals->sounds_locked = true;
            if (!latched) {
                globals->updating = true;
                if (sound_manager_sound_is_active()) {
                    void *wait_handle = g_synch_globals.events[6].handle;
                    if (wait_handle)
                        WaitForSingleObjectEx(wait_handle, (unsigned long)-1, 0);
                    get_geometry_runtime_manager()->update_bulk_reader();
                    sound_xaudio_xenon_process_events();
                    g_xaudio_source_manager.begin_scene();
                    sound_update_time();
                    long channel_index = 0;
                    if (sound_manager_globals_view_get()->channel_count > 0) {
                        do {
                            sound_channel_datum *channel = channel_get((short)channel_index);
                            if (channel->sound_index != -1 && !channel->paused && !sound_update_tracking(channel->sound_index))
                                sound_stop_and_unlock((unsigned long)channel->sound_index, _sound_event_stop_reason_refresh_stopped);
                            channel_index++;
                        } while (channel_index < sound_manager_globals_view_get()->channel_count);
                    }
                    refresh_channels();
                    update_channels();
                    sound_xaudio_xenon_process_events();
                    g_xaudio_source_manager.end_scene();
                    XAudioSynchStart();
                    SetEvent(g_synch_globals.events[5].handle);
                }
                lruv_idle((s_lruv_cache *)g_sound_cache_globals.lruv);
                globals = sound_manager_globals_view_get();
                globals->updating = false;
            }
            globals->sounds_locked = false;
        }
    }
}

/* ---- sound_render (0x137470) ---- */

extern "C" {

}

// @address 0x137470 ?sound_render@@YAXXZ
void sound_render(void)
{
    if (!sound_manager_sound_is_active())
        return;

    s_synchronization_globals *synchronization = &g_synch_globals;
    if (synchronization->events[6].handle != 0)
        WaitForSingleObjectEx(synchronization->events[6].handle, -1, 0);
    sound_manager_globals_view_get()->updating = true;
    sound_xaudio_xenon_process_events();
    c_xenon_audio_source_manager *source_manager = &g_xaudio_source_manager;
    source_manager->begin_scene();

    s_sound_manager_globals *globals = sound_manager_globals_view_get();
    if (globals->active)
    {
            if (globals->paused && globals->update_frame && !game_time_is_paused())
            {
                sound_pause(_sound_pause_unpaused, false);
                globals = sound_manager_globals_view_get();
            }
            platform_sound_get_status(globals);
            if (!globals->paused)
            {
                if (globals->started)
                    sound_update_time();
                else
                {
                    unsigned long tick_count = GetTickCount();
                    sound_manager_globals_view_get()->reset_time = tick_count;
                }
                ((void (*)())sound_manager_set_sound_environment)();
            }

            bool bink_playing = bink_is_idle_and_drive_empty();
            globals = sound_manager_globals_view_get();
            float reverb_rate = globals->reverb_rate;
            globals->ducking_suppressed = bink_playing;
            sound_xaudio_xenon_update_reverb_levels(reverb_rate);
            refresh_listener(true);

            for (long controller_index = data_next_index((s_data_array *)g_sound_playback_controller_data, -1);
                 controller_index != -1;
                 controller_index = inl_data_next_index((s_data_array *)g_sound_playback_controller_data, controller_index))
            {
                c_sound_playback_controller *controller = sound_manager_playback_controller_get(controller_index);
                unsigned long *seed = &controller->random_seed;
                random_seed_next(seed);
            }

            process_looping_sounds(sound_manager_globals_view_get()->update_frame);
            refresh_channels();
            refresh_sounds();
            get_geometry_runtime_manager()->update_bulk_reader();
            prioritize_sounds();

            c_static_array<const s_sound_environment_parameters *, 196> voice_map;
            c_static_array<const s_sound_environment_parameters *, 196> source_map;
            s_game_sound_obstruction_parameters obstruction_parameters;
            obstruction_parameters.count = 0;
            for (long index = 0; index < 196; index++)
                voice_map[index] = 0;
            for (long index = 0; index < 196; index++)
                source_map[index] = 0;
            touch_channels(&obstruction_parameters, &voice_map, &source_map);

            if (g_sound_transmission_enabled)
            {
                audibility_table audibility;
                actor_perception_initialize_audibility_table(&audibility, global_scenario_index, global_zone_set_index);
                if (audibility.audibility != 0)
                {
                    long user_index = inl_players_first_active_user();
                    while (user_index != -1)
                    {
                        const s_sound_listener *listener = sound_manager_get_listener(user_index);
                        if (listener && sound_manager_location_valid(&listener->location))
                        {
                            s_game_sound_transmission_listener source;
                            source.listener_index = user_index;
                            source.location = listener->location;
                            source.position = listener->matrix.position;
                            game_sound_transmission_update_sources(&audibility, &source, &obstruction_parameters);
                        }
                        user_index = players_next_active_user(user_index);
                    }
                }
            }

            for (long channel_index = 0; channel_index < sound_manager_globals_view_get()->channel_count; channel_index++)
            {
                sound_channel_datum *channel = sound_manager_channel_get_view((short)channel_index);
                if (channel->sound_index == -1 || channel->paused)
                    continue;
                const s_sound_environment_parameters *environment;
                if (channel->hardware_voice_index != -1)
                    environment = voice_map[channel->hardware_voice_index];
                else if (channel->source_index != -1)
                    environment = source_map[channel->source_index];
                else
                    continue;
                channel_refresh_location((short)channel_index, environment);
            }

            update_channels();
            if (sound_manager_globals_view_get()->update_frame)
                sound_manager_globals_view_get()->update_flags++;
            sound_xaudio_xenon_process_events();
            source_manager->end_scene();
            XAudioSynchStart();
            sound_playback_controllers_synch_playback(false);
            sound_manager_globals_view_get()->update_frame = false;
            sound_manager_globals_view_get()->updating = false;
        }
    lruv_idle((s_lruv_cache *)g_sound_cache_globals.lruv);
    SetEvent(synchronization->events[5].handle);
}

static __forceinline bool sound_manager_impulse_flag_test(const sound_datum *sound, e_sound_manager_impulse_flag_bits bit)
{
    bool result = sound->flags.test(bit) != 0;
    return result;
}


static __forceinline s_sound_cache_file_gestalt_definition *sound_manager_gestalt_blocks_get(void)
{
    return (s_sound_cache_file_gestalt_definition *)g_sound_cache_file_gestalt.data;
}
static __forceinline const s_sound_scale_modifiers *sound_manager_definition_get_scale_modifiers(const s_cache_file_sound_definition *definition)
{
    const s_sound_scale_modifiers *element = 
        (const s_sound_scale_modifiers *)tag_block_get_range_with_size(&sound_manager_gestalt_blocks_get()->scale_modifiers,
            definition->scale_index, 1,
            sizeof(s_sound_scale_modifiers));
    return element;
}
static __forceinline float sound_manager_scale_value(float value, float fraction, const s_sound_pitch_bounds *range)
{
    float lower = (float)range->lower;
    float upper = (float)range->upper;
    return ((upper - lower) * fraction + value) + lower;
}
static __forceinline float sound_manager_definition_map_pitch(const s_cache_file_sound_definition *definition, float pitch, float fraction)
{
    return sound_manager_scale_value(pitch, fraction, &sound_manager_definition_get_scale_modifiers(definition)->pitch);
}
static __forceinline s_cache_file_sound_pitch_range *sound_manager_definition_get_pitch_range_block(const s_cache_file_sound_definition *definition, long index)
{
    s_cache_file_sound_pitch_range *element = 
        (s_cache_file_sound_pitch_range *)tag_block_get_range_with_size(&sound_manager_gestalt_blocks_get()->pitch_ranges,
            definition->pitch_range_index + index, 1,
            sizeof(s_cache_file_sound_pitch_range));
    return element;
}
static __forceinline s_cache_file_sound_permutation *sound_manager_pitch_range_get_permutation_block(const s_cache_file_sound_pitch_range *pitch_range,
    long index)
{
    s_cache_file_sound_permutation *element = 
        (s_cache_file_sound_permutation *)tag_block_get_range_with_size(&sound_manager_gestalt_blocks_get()->permutations,
            pitch_range->first_permutation_index + index, 1,
            sizeof(s_cache_file_sound_permutation));
    return element;
}
static __forceinline s_sound_permutation_chunk *sound_manager_permutation_get_chunk_block(const s_cache_file_sound_permutation *permutation, long index)
{
    s_sound_permutation_chunk *element = 
        (s_sound_permutation_chunk *)tag_block_get_range_with_size(&sound_manager_gestalt_blocks_get()->chunks,
            permutation->first_chunk_index + index, 1,
            sizeof(s_sound_permutation_chunk));
    return element;
}
static __forceinline long sound_manager_pitch_range_get_natural_pitch(const s_cache_file_sound_pitch_range *pitch_range)
{
    const s_sound_gestalt_pitch_parameters *parameters =
        (const s_sound_gestalt_pitch_parameters *)tag_block_get_range_with_size(&sound_manager_gestalt_blocks_get()->pitch_parameters,
            pitch_range->parameter_index, 1,
            sizeof(s_sound_gestalt_pitch_parameters));
    return parameters->natural_pitch;
}
static __forceinline float sound_manager_pitch_range_get_relative_pitch(const s_cache_file_sound_pitch_range *pitch_range, float pitch)
{
    long natural_pitch = sound_manager_pitch_range_get_natural_pitch(pitch_range);
    float relative_pitch = pitch - (float)natural_pitch;
    return relative_pitch;
}
static __forceinline long sound_manager_permutation_get_chunk_count(const s_cache_file_sound_permutation *permutation)
{
    return permutation->chunk_count;
}
static __forceinline short sound_manager_permutation_get_next_chunk_index(const s_cache_file_sound_permutation *permutation,
    short chunk_index)
{
    long next_chunk_index = permutation && chunk_index < sound_manager_permutation_get_chunk_count(permutation) - 1 ? chunk_index + 1 : -1;
    return (short)next_chunk_index;
}


static __forceinline const s_cache_file_sound_definition *sound_manager_sound_definition_get(long definition_index)
{
    const s_cache_file_sound_definition *definition = TAG_GET('snd!', const s_cache_file_sound_definition, definition_index);
    return definition;
}

static __forceinline float sound_manager_playback_controller_get_doppler_shift(const c_sound_playback_controller *controller,
    long listener_index)
{
    if (controller->owner_identifier != -1)
        return controller->doppler_shifts[listener_index];
    return k_real_zero_storage;
}

static __forceinline bool sound_manager_sound_class_prefetches_next_chunk(short sound_class)
{
    bool result = sound_class == 38 || sound_class == 49 || sound_class == 48;
    return result;
}

static __forceinline void sound_manager_platform_source_update_properties(long source_index, const s_sound_channel_properties *properties,
    const s_platform_sound_playback_submit_parameters *submit_parameters, bool force)
{
    g_xaudio_mix_master.source_update_properties(source_index, &properties->source_properties, submit_parameters, force);
}

static __forceinline real_decibel sound_manager_convert_real_to_decibels(float value)
{
    real_decibel gain_db;
    gain_db.value = value;
    return gain_db;
}

static __forceinline real_decibel sound_manager_permutation_get_gain_db(const s_cache_file_sound_permutation *permutation)
{
    return sound_manager_convert_real_to_decibels((float)permutation->gain);
}

static __forceinline void sound_manager_impulse_queue_permutation(
    short channel_index, const sound_datum *sound, s_cache_file_sound_permutation *permutation)
{
    short chunk_index = sound->chunk_index;
    channel_queue_sound(channel_index, (s_cache_file_sound_permutation *)permutation, chunk_index);
}
static __forceinline real_decibel sound_manager_impulse_get_permutation_gain_db(real_decibel gain_db,
    const s_cache_file_sound_permutation *permutation)
{
    real_decibel permutation_gain_db = sound_manager_permutation_get_gain_db(permutation);
    real_decibel pinned_gain_db = pin_gain_db(gain_db + permutation_gain_db);
    return pinned_gain_db;
}
static __forceinline short sound_manager_impulse_get_next_chunk_index(const sound_datum *sound,
    const s_cache_file_sound_permutation *permutation)
{
    short chunk_index = sound->chunk_index;
    short next_chunk_index = sound_manager_permutation_get_next_chunk_index(permutation, chunk_index);
    return next_chunk_index;
}

static __forceinline bool sound_manager_impulse_make_permutation_ready(sound_datum *sound)
{
    bool ready = sound_make_permutation_ready((sound_datum *)sound, 0);
    return ready;
}
static __forceinline c_sound_playback_controller *sound_manager_impulse_get_controller(const sound_datum *sound)
{
    long controller_index = sound->playback_controller_index;
    c_sound_playback_controller *controller = sound_manager_playback_controller_get(controller_index);
    return controller;
}
static __forceinline float sound_manager_impulse_get_doppler_shift(const sound_datum *sound)
{
    float doppler = sound->playback_controller_index != -1
        ? sound_manager_playback_controller_get_doppler_shift(sound_manager_impulse_get_controller(sound), sound->listener_index)
        : calculate_doppler_shift_cents(sound->listener_index, (const s_sound_location *)&sound->source.position);
    return doppler;
}

static __forceinline void sound_manager_impulse_prefetch_next_chunk(const sound_datum *sound,
    const s_cache_file_sound_permutation *permutation)
{
    short next_chunk = sound_manager_impulse_get_next_chunk_index(sound, permutation);
    if (next_chunk != -1)
    {
        s_sound_permutation_chunk *chunk = sound_manager_permutation_get_chunk_block(permutation, next_chunk);
        _sound_cache_sound_request(sound->definition_index, (s_sound_permutation_chunk *)chunk, 1);
    }
}
static __forceinline void sound_manager_impulse_advance_chunk(short channel_index, const sound_channel_datum *channel,
    sound_datum *sound, const s_cache_file_sound_permutation *permutation)
{
    short next_chunk = sound_manager_impulse_get_next_chunk_index(sound, permutation);
    if (next_chunk != -1)
    {
        sound_set_permutation(channel->sound_index, sound->definition_index, sound->pitch_range_index, sound->permutation_index,
            next_chunk);
        sound_manager_impulse_make_permutation_ready(sound);
        sound->flags.set(_sound_manager_impulse_next_chunk_queued_bit, true);
    }
    else
    {
        sound->flags.set(_sound_manager_impulse_finished_bit, true);
        channel_signal_complete(channel_index);
    }
}

static __forceinline void sound_manager_channel_update_properties(short channel_index, const s_sound_channel_properties *properties,
    bool force)
{
    sound_channel_datum *channel = sound_manager_channel_get_view(channel_index);
    sound_manager_platform_source_update_properties(channel->source_index, properties, &properties->submit_parameters, force);
}

static __forceinline void sound_manager_impulse_start(short channel_index, sound_datum *sound,
    s_cache_file_sound_permutation *permutation, real_decibel gain_db, s_sound_channel_properties *properties)
{
    properties->source_properties.gain_db = sound_manager_impulse_get_permutation_gain_db(gain_db, permutation);
    channel_initialize(channel_index, properties);
    sound_manager_impulse_queue_permutation(channel_index, sound, permutation);
    if (sound->playback_controller_index != -1)
    {
        c_sound_playback_controller *controller = sound_manager_impulse_get_controller(sound);
        sound->synchronized_playback = controller->synchronized_sound_ready((signed char)-1);
    }
    sound->flags.set(_sound_manager_impulse_started_bit, true);
}

static __forceinline void sound_manager_impulse_update_gain(
    short channel_index, real_decibel gain_db, const s_cache_file_sound_permutation *permutation, s_sound_channel_properties *properties)
{
    properties->source_properties.gain_db = sound_manager_impulse_get_permutation_gain_db(gain_db, permutation);
    sound_manager_channel_update_properties(channel_index, properties, true);
}
static __forceinline void sound_manager_impulse_apply_doppler(const s_cache_file_sound_definition *definition,
    const sound_datum *sound, const s_sound_output_parameters *output, s_sound_channel_properties *properties)
{
    const sound_class_definition *sound_class = sound_class_get(definition->sound_class);
    float doppler = sound_manager_impulse_get_doppler_shift(sound);
    properties->source_properties.pitch += sound_class->doppler * output->doppler * doppler;
}

// @address 0x1a55c0 ?update_channel_for_impulse_sound@@YAXFUreal_decibel@@PBUs_sound_output_parameters@@@Z
void update_channel_for_impulse_sound(short channel_index, real_decibel decibel, const s_sound_output_parameters *channel_output_parameters)
{
    sound_channel_datum *channel = sound_manager_channel_get_view(channel_index);
    sound_datum *sound = (sound_datum *)sound_manager_sound_get(channel->sound_index);
    const s_cache_file_sound_definition *definition = sound_manager_sound_definition_get(sound->definition_index);
    s_sound_channel_properties properties = {0};
    if (sound_manager_source_test(&sound->source, 8))
        properties.submit_parameters.special = true;
    float fraction = sound->source.volume;
    float pitch = sound_manager_definition_map_pitch(definition, sound->pitch, fraction);
    real_decibel gain_db = sound_definition_map_gain_db(definition, decibel + sound->gain + sound->source.gain, fraction);
    channel_properties_initialize_from_sound(channel_index, (const sound_datum *)sound, channel_output_parameters, &properties);
    s_cache_file_sound_pitch_range *pitch_range = sound_manager_definition_get_pitch_range_block(definition, sound->pitch_range_index);
    s_cache_file_sound_permutation *permutation = sound_manager_pitch_range_get_permutation_block(pitch_range, sound->permutation_index);
    s_sound_permutation_chunk *chunk = sound_manager_permutation_get_chunk_block(permutation, sound->chunk_index);
    properties.source_properties.effect = 0;
    properties.source_properties.channel = sound->playback_controller_index;
    properties.source_properties.pitch = sound_manager_pitch_range_get_relative_pitch(pitch_range, pitch);
    if (sound->playback_flags & 2)
    {
        sound_manager_impulse_apply_doppler(definition, sound, channel_output_parameters, &properties);
    }
    if (!sound_manager_impulse_flag_test(sound, _sound_manager_impulse_started_bit))
    {
        sound_manager_impulse_start(channel_index, sound, permutation, gain_db, &properties);
    }
    else if (channel->queued != 0)
    {
        sound_manager_impulse_update_gain(channel_index, gain_db, channel->queued, &properties);
    }
    if (!sound_manager_impulse_flag_test(sound, _sound_manager_impulse_finished_bit))
    {
        if (sound_manager_impulse_flag_test(sound, _sound_manager_impulse_next_chunk_queued_bit) && channel->next == 0 &&
            sound_manager_impulse_make_permutation_ready(sound))
        {
            sound_manager_impulse_queue_permutation(channel_index, sound, permutation);
            sound->flags.set(_sound_manager_impulse_next_chunk_queued_bit, false);
        }
        if (!sound_manager_impulse_flag_test(sound, _sound_manager_impulse_next_chunk_queued_bit))
        {
            sound_manager_impulse_advance_chunk(channel_index, channel, sound, permutation);
        }
        bool prefetch = sound_manager_sound_class_prefetches_next_chunk(definition->sound_class);
        if (prefetch && sound_manager_impulse_flag_test(sound, _sound_manager_impulse_next_chunk_queued_bit))
        {
            sound_manager_impulse_prefetch_next_chunk(sound, permutation);
        }
    }
}


// ==== refresh_listener (0x1a41c0) ====

/* the globals' reverb entries are six 32-bit words plus the trailing float gain */

// @address 0x35d40 k_default_reverb_definition
#if !defined(_XBOX) || defined(__clang__)
const sound_environment k_default_reverb_definition =
{
    -32768,
    -100.0f, -100.0f, 0.0f, 1.0f, 1.0f,
    -100.0f, 0.0f, -100.0f, 0.0f, 1.0f, 1.0f, 5000.0f
};
#endif

static __forceinline bool sound_manager_point_in_water(const real_point3d *point, short *material_type)
{
    s_water_physics_test_sphere_result water_result;
    water_result.material_type.m_index = -1;
    bool in_water = water_physics_test_sphere(true, point, 0.0f, &water_result);
    *material_type = water_result.material_type.m_index;
    return in_water;
}

static __forceinline const s_observer_result_fields *sound_manager_observer_try_get_camera(long user_index)
{
    const s_observer_result_fields *result = 0;
    if (user_index != -1 && g_active_structure_bsp_mask != 0)
    {
        bool player_valid = players_user_is_active(user_index);
        if (player_valid)
            result = &g_observer_globals->blocks[user_index].result;
    }
    return result;
}

#if !defined(_XBOX) || defined(__clang__)
extern "C" int __cdecl port_config_splitscreen_full_quality(void);
#endif
// @address 0x1a41c0 ?refresh_listener@@YAX_N@Z
void refresh_listener(bool update_platform)
{
    if (game_in_progress())
    {
        bool split_screen = player_mapping_globals->local_player_count > 1;
        for (long user_index = 0; user_index < 4; user_index++)
        {
            s_sound_listener *listener = &sound_manager_globals_view_get()->listeners[user_index];
            const s_observer_result_fields *camera = sound_manager_observer_try_get_camera(user_index);
            if (camera)
            {
                long unit_index = player_mapping_globals->unit_indices[user_index];
                short perspective = debug_sound_force_first_person_listener ? 0 : g_director_globals->slots[user_index].perspective;

                real_point3d position;
                s_location location;
                union { real_vector3d forward; real_point3d forward_as_point; };
                union { real_vector3d up; real_point3d up_as_point; };
                real_vector3d velocity;

                if (unit_index != -1 && debug_sound_force_first_person_listener)
                {
                    s_observer_command command;
                    first_person_camera_for_unit_and_vector(unit_index,
                        &OBJECT_GET(unit_index, unit_datum, _object_mask_unit)->unit.aiming_vector,
                        &command);
                    forward = command.forward;
                    up = command.up;
                    position = command.position;
                    object_get_velocities(unit_index, &velocity, 0);
                    scenario_location_from_point((s_location *)&location, &position);
                }
                else if (unit_index != -1 && ((1 << g_director_globals->slots[user_index].perspective) & 3) != 0)
                {
                    unit_datum *unit = OBJECT_GET(unit_index, unit_datum, _object_mask_unit);
                    if (unit->object.parent_object_index != -1 && unit->unit.parent_seat_index != -1 && perspective != 0)
                    {
                        unit_definition *parent_definition = (unit_definition *)global_tag_instances[
                            DATUM_INDEX_TO_ABSOLUTE_INDEX(
                                ((s_object_header *)object_header_data->data)[
                                    DATUM_INDEX_TO_ABSOLUTE_INDEX(unit->object.parent_object_index)].datum->definition_index)]
                            .definition;
                        unit_seat *seat = &((unit_seat *)parent_definition->seats.begin())[unit->unit.parent_seat_index];
                        real_point3d head;
                        unit_get_head_position(unit_index, &head);
                        position.x = (head.x - camera->position.x) * seat->listener_interpolation + camera->position.x;
                        position.y = (head.y - camera->position.y) * seat->listener_interpolation + camera->position.y;
                        position.z = (head.z - camera->position.z) * seat->listener_interpolation + camera->position.z;
                        real_quaternion rotation;
                        quaternion_from_unit_vectors(&rotation, &camera->forward,
                            &unit->unit.desired_looking_vector);
                        quaternions_interpolate(global_identity_quaternion, &rotation, seat->listener_interpolation, &rotation);
                        quaternion_normalize(&rotation);
                        quaternion_transform_point(&rotation, &camera->forward_as_point, &forward_as_point);
                        quaternion_transform_point(&rotation, &camera->up_as_point, &up_as_point);
                        scenario_location_from_point((s_location *)&location, &position);
                        if ((unsigned char)location.cluster_reference.bsp_index == 0xff)
                            location = camera->bsp_location;
                    }
                    else
                    {
                        position = camera->position;
                        location = camera->bsp_location;
                        forward = camera->forward;
                        up = camera->up;
                    }
                    object_get_velocities(unit_index, &velocity, 0);
                }
                else
                {
                    const s_observer_result_fields *default_camera = &g_observer_globals->blocks[user_index].result;
                    position = default_camera->position;
                    location = default_camera->bsp_location;
                    forward = default_camera->forward;
                    up = default_camera->up;
                    velocity = default_camera->velocity;
                }

                listener->valid = true;
                short water_material_type = -1;
#if !defined(_XBOX) || defined(__clang__)
                bool underwater = (!split_screen || port_config_splitscreen_full_quality()) && sound_manager_point_in_water(&position, &water_material_type);
#else
                bool underwater = !split_screen && sound_manager_point_in_water(&position, &water_material_type);
#endif
                listener->default_acoustics = underwater;
                listener->water_material_type = water_material_type;
                real_matrix4x3 *matrix = &listener->matrix;
                matrix4x3_rotation_from_vectors(matrix, &forward, &up);
                matrix->position = position;
                matrix4x3_inverse_transform_vector(matrix, &velocity, &listener->velocity);
                listener->location = location;
            }
            else
            {
                location_invalidate(&listener->location);
            }
        }

        if (update_platform)
        {
            c_static_array<s_platform_sound_listener_reverb, 2> entries;
            long reverb_index = 0;
            do
            {
                const s_sound_manager_reverb *reverb = &sound_manager_globals_view_get()->runtime_reverbs[reverb_index];
                const sound_environment *definition;
                if (reverb->definition_index == -1)
                    definition = &k_default_reverb_definition;
                else
                    definition = TAG_GET('snde', const sound_environment, reverb->definition_index);
                entries[reverb_index].environment = definition;
                entries[reverb_index].azimuth_image = reverb->image;
                entries[reverb_index].wet_gain = reverb->scale;
#if !defined(_XBOX) || defined(__clang__)
                if (split_screen && !port_config_splitscreen_full_quality())
                    entries[reverb_index].wet_gain *= 0.5f;
#else
                if (split_screen)
                    entries[reverb_index].wet_gain *= 0.5f;
#endif
                reverb_index++;
            } while (reverb_index < 2);
            platform_sound_listener_properties properties;
            properties.forward = global_identity4x3->forward;
            properties.up = global_identity4x3->up;
            properties.position = *global_origin3d;
            properties.velocity = *global_zero_vector3d;
            properties.reverb_count = 2;
            properties.reverbs = entries.begin();
            platform_sound_listener_set_properties(&properties);
        }
    }
}

static __forceinline const s_sound_event_callbacks *sound_manager_sound_get_update_callback(const sound_datum *sound)
{
    return (const s_sound_event_callbacks *)sound->callbacks;
}

static __forceinline bool sound_manager_sound_class_is_unscaled(short sound_class)
{
    return sound_class == 38 || sound_class == 49 || sound_class == 48;
}

static __forceinline bool sound_manager_sound_fade_finished(long sound_index)
{
    sound_datum *sound = sound_manager_sound_get_view(sound_index);
    return sound_manager_flag_test16(sound->flags.m_flags, 7) && sound->fade_start_time > sound->fade_end_time;
}

// @address 0x1a5fc0 ?update_channels@@YAXXZ
void update_channels(void)
{
    real_decibel master_gain = real_gain_to_decibels(g_sound_manager_globals->master_gain);
    s_sound_output_parameters output;
    platform_sound_get_status(g_sound_manager_globals);
    for (long index = 0; index < g_sound_manager_globals->channel_count; index++) {
        short channel_index = (short)index;
        bool updated = false;
        sound_channel_datum *channel = sound_manager_channel_get_view(channel_index);
        if (channel->sound_index != -1 && !channel->paused) {
            sound_datum *sound = datum_get<sound_datum>(g_sound_data, channel->sound_index);
            real_decibel current;
            {
                current = sound_calculate_fade_db(channel->sound_index);
            }
            bool faded_out = (current + sound->source.gain) <= k_sound_manager_minimum_gain_db;
            if (faded_out && sound_manager_sound_fade_finished(channel->sound_index)) {
                sound_stop_and_unlock((unsigned long)channel->sound_index, _sound_event_stop_reason_fade_out);
            } else {
                sound_datum *fade_sound = sound_manager_sound_get(channel->sound_index);
                if (fade_sound->flags.test(7)
                    && fade_sound->fade_end_time > fade_sound->fade_start_time
                    && g_sound_manager_globals->time >= fade_sound->fade_end_time)
                    fade_sound->flags.set(7, false);
                if (channel->source_index != -1) {
                    long sound_class =
                        ((const s_cache_file_sound_definition *)global_tag_instances[
                            DATUM_INDEX_TO_ABSOLUTE_INDEX(sound->definition_index)].definition)->sound_class;
                    channel_update_spatialization(channel_index, &current, &output);
                    unsigned long ignores_master_gain;
                    if (game_globals->game_lost != 0)
                        ignores_master_gain = sound_manager_flag_test16(sound_class_get(sound_class)->flags, 12);
                    else
                        ignores_master_gain = sound_manager_sound_class_is_unscaled(sound_class);
                    if (!ignores_master_gain)
                        current += master_gain;
                    if (sound_manager_sound_unknown_type(channel->sound_index))
                        update_channel_for_looping_sound(channel_index, current, &output);
                    else
                        update_channel_for_impulse_sound(channel_index, current, &output);
                    updated = true;
                }
            }
        }
        if (updated && channel->sound_index != -1 && sound_manager_can_delete_sounds()) {
            sound_datum *sound = sound_manager_sound_get_view(channel->sound_index);
            if (!sound_manager_flag_test16(sound->flags.m_flags, 5)
                && sound->callbacks
                && sound_manager_sound_get_update_callback(sound)->update_proc)
                sound_manager_sound_get_update_callback(sound)->update_proc(sound->object_index, sound->event_parameters,
                    sound->definition_index,
                    sound->pitch_range_index, channel->queued, channel->position);
        }
    }
}


static __forceinline void sound_manager_copy_output_parameters(s_sound_output_parameters *destination,
    const s_sound_output_parameters *source)
{
    memcpy(destination, source, sizeof(*destination));
}
// @address 0x1a6c60 ?channel_update_spatialization@@YAXFPAUreal_decibel@@PAUs_sound_output_parameters@@@Z
void channel_update_spatialization(short channel_index, real_decibel *gain, s_sound_output_parameters *output)
{
    sound_datum *sounds = ((sound_datum *)g_sound_data->data);
    sound_channel_datum *channel = sound_manager_channel_get_view(channel_index);
    channel->previous_flags = channel->playback_flags;
    channel->playback_flags.clear();
    sound_datum *sound = &sounds[DATUM_INDEX_TO_ABSOLUTE_INDEX(channel->sound_index)];
    switch (sound->source.current_spatialization) {
    case 0:
        channel->playback_flags.set(_sound_manager_channel_primary_flag, true);
        if (sound_manager_flag_test16(sound->source.flags, 8))
            output->effect_fraction = 0.0f;
        else
            output->effect_fraction = 1.0f;
        output->position = *global_origin3d;
        break;
    case 1:
    case 2:
        sound_calculate_listener_relative_position_and_attenuation(sound->listener_index, &sound->source,
            sound->definition_index, &output->effect_fraction, &output->cone_attenuation,
            &output->position);
        if ((sound->playback_flags & 2) != 0)
            channel->playback_flags.set(_sound_manager_channel_secondary_flag, true);
        else {
            real_decibel attenuation = real_gain_to_decibels(output->effect_fraction);
            *gain += attenuation;
            channel->playback_flags.set(_sound_manager_channel_primary_flag, true);
        }
        if (sound_manager_flag_test16(sound->source.flags, 11))
            channel->playback_flags.set(_sound_manager_channel_primary_flag, true);
        break;
    default:
        break;
    }
    output->doppler = channel->playback_flags.test(_sound_manager_channel_primary_flag) ? 0.0f : 1.0f;
    long now = g_sound_manager_globals->time;
    if (channel->previous_flags.is_clear()) {
        channel->transition_start = 0;
        const real_decibel *distance = channel->playback_flags.test(_sound_manager_channel_primary_flag)
            ? &k_sound_manager_zero_gain_db : &k_sound_manager_minimum_gain_db;
        output->distance = *distance;
        const real_decibel &level = channel->playback_flags.test(_sound_manager_channel_secondary_flag)
            ? k_sound_manager_zero_gain_db : k_sound_manager_minimum_gain_db;
        output->gain = level;
    } else {
        if (channel->previous_flags != channel->playback_flags) {
            long time_remaining = PIN(1000 - channel->transition_start - now, 0, 1000);
            channel->transition_start = now - time_remaining;
        }
        real_decibel forward = k_sound_manager_zero_gain_db;
        real_decibel reverse = k_sound_manager_minimum_gain_db;
        if (channel->transition_start > 0) {
            long elapsed = now - channel->transition_start;
            if (elapsed > 1000)
                channel->transition_start = 0;
            else {
                forward = sound_manager_fade_ratio_gain(_sound_manager_gain_scale_power, (float)elapsed, 1000.0f);
                reverse = sound_manager_fade_ratio_gain(_sound_manager_gain_scale_power,
                    (float)(now - channel->transition_start), -1000.0f);
            }
        }
        real_decibel blend;
        if (channel->playback_flags.test(_sound_manager_channel_secondary_flag))
            blend = forward;
        else
            blend = reverse;
        output->doppler = sound_manager_real_decibels_to_gain(blend);
        channel->previous_flags = channel->playback_flags;
        const real_decibel *distance = channel->playback_flags.test(_sound_manager_channel_primary_flag) ? &forward : &reverse;
        output->distance = *distance;
        const real_decibel *level = channel->playback_flags.test(_sound_manager_channel_secondary_flag) ? &forward : &reverse;
        output->gain = *level;
    }
    output->flags = channel->playback_flags.get_unsafe();
    output->lower = k_sound_manager_zero_gain_db;
    output->upper = k_sound_manager_zero_gain_db;
    if (sound_manager_flag_test16(sound->source.flags, 11)) {
        const s_cache_file_sound_definition *definition = TAG_GET('snd!', const s_cache_file_sound_definition, sound->definition_index);
        output->distance += sound_class_get(definition->sound_class)->distance_offset;
    }
    if (!sound->flags.test(5)) {
        const s_sound_event_callbacks *tracker = (const s_sound_event_callbacks *)sound->callbacks;
        if (tracker && tracker->spatialization_proc) {
            s_sound_output_parameters temporary;
            sound_manager_copy_output_parameters(&temporary, output);
            bool updated = tracker->spatialization_proc(sound->object_index, sound->definition_index, sound->event_parameters, &temporary);
            if (updated)
                sound_manager_copy_output_parameters(output, &temporary);
        }
    }
}

// @address 0x7c5d60 g_sound_preferences
s_sound_preferences g_sound_preferences = { { 42, 42, 21, 14, 0 }, { 36, 36, 18, 12, 0 }, 128, 111.548553f };
static __forceinline void sound_manager_initialize_read_preferences(const s_sound_preferences **preferences)
{
    *preferences = &g_sound_preferences;
}
static __forceinline bool sound_manager_looping_sound_initialize()
{
#ifdef HALO3_64BIT
    g_looping_sound_data = runtime_state_data_new("looping sounds", 0x80, (long)sizeof(looping_sound_datum));
#else
    g_looping_sound_data = runtime_state_data_new("looping sounds", 0x80, 0x124);
#endif
    if (g_looping_sound_data)
        data_make_valid(g_looping_sound_data);
    return g_looping_sound_data != 0;
}
static __forceinline bool sound_manager_sound_effect_manager_initialize()
{
#ifdef HALO3_64BIT
    g_sound_effect_data = runtime_state_data_new("sounds effects", 0x10, (long)sizeof(s_sound_effect_datum));
#else
    g_sound_effect_data = runtime_state_data_new("sounds effects", 0x10, 0x14);
#endif
    if (g_sound_effect_data)
        data_make_valid(g_sound_effect_data);
    return g_sound_effect_data != 0;
}
static __forceinline bool sound_manager_sound_voice_manager_initialize(const s_sound_preferences *preferences)
{
    return true;
}
// @address 0x1a2078 ?sound_initialize@@YAXXZ
void sound_initialize()
{
    if (!g_sound_initialization_count) {
#ifdef HALO3_64BIT
        s_sound_manager_globals *manager = (s_sound_manager_globals *)runtime_state_malloc("", "", (long)sizeof(s_sound_manager_globals));
#else
        s_sound_manager_globals *manager = (s_sound_manager_globals *)runtime_state_malloc("", "", 0x298);
#endif
        g_sound_manager_globals = manager;
        memset(manager, 0, sizeof(*manager));

        s_sound_manager_globals *globals = (s_sound_manager_globals *)manager;
        globals->enabled = true;
        globals->updating = false;
        const s_sound_preferences *preferences;
        sound_manager_initialize_read_preferences(&preferences);
        sound_cache_new();

        globals = sound_manager_globals_view_get();
        globals->environment.reverb_count = 0;
        globals->runtime_reverbs[0].definition_index = -1;
        globals->runtime_reverbs[1].definition_index = -1;
        globals->runtime_reverbs[0].image.azimuth.lower = 0.0f;
        globals->runtime_reverbs[0].image.azimuth.upper = 6.2831855f;
        globals->runtime_reverbs[1].image.azimuth.lower = 0.0f;
        globals->runtime_reverbs[1].image.azimuth.upper = 0.0f;
        globals->timeline_duck = 0.0f;

        c_static_array<s_sound_listener, 4> &listener_array = *(
            c_static_array<s_sound_listener, 4> *)globals->listeners;
        listener_array.m_storage[0].header_signature = 'lshd';
        listener_array.m_storage[1].header_signature = 'lshd';
        listener_array[0].footer_signature = 'lsft';
        listener_array[1].footer_signature = 'lsft';
        listener_array.m_storage[2].header_signature = 'lshd';
        listener_array[2].footer_signature = 'lsft';
        listener_array.m_storage[3].header_signature = 'lshd';
        listener_array[3].footer_signature = 'lsft';

        main_set_status_value("sound", "init start");

        g_sound_data = (s_data_array *)runtime_state_data_new("sound sources", 0x180, sizeof(sound_datum));
        g_sound_channels = (sound_channel_datum *)runtime_state_malloc("", "", 0x1b90);
        main_set_status_value("sound", "init globals");

        s_data_array *sound_data = g_sound_data;
        if (sound_data && g_sound_channels) {
            sound_data->valid = true;
            data_delete_all((s_data_array *)sound_data);
            memset(g_sound_channels, 0, 196 * sizeof(*g_sound_channels));

            if (sound_playback_controllers_initialize() &&
                sound_manager_looping_sound_initialize() &&
                sound_manager_sound_effect_manager_initialize() &&
                sound_manager_sound_voice_manager_initialize(preferences) &&
                sound_xaudio_xenon_initialize(preferences)) {
                main_set_status_value("sound", "init other crap");

                short channel_index = 0;
                for (long source_index = 0; source_index < 7; source_index++)
                    sound_manager_globals_view_get()->used_sources[source_index] = 0;
                for (long voice_index = 0; voice_index < 7; voice_index++)
                    sound_manager_globals_view_get()->used_voices[voice_index] = 0;
                globals = sound_manager_globals_view_get();

                short channel_type_value = 0;
                long channel_type = 0;
                do {
                    globals->channel_count = globals->channel_count + preferences->channel_counts[channel_type];
                    if (preferences->channel_counts[channel_type] > 0) {
                        signed char channel_type_narrow = (signed char)channel_type_value;
                        for (short type_index = 0; type_index < preferences->channel_counts[channel_type]; type_index++) {
                            sound_channel_datum *channel = (sound_channel_datum *)channel_get(channel_index++);
                            channel->sound_index = -1;
                            channel->channel_type = channel_type_narrow;
                            channel->playback_flags.clear();
                            channel->previous_flags.clear();
                            channel->source_index = -1;
                            channel->hardware_voice_index = -1;
                            channel->voice_index = -1;
                            channel->queued = 0;
                            channel->next = 0;
                            channel->paused = false;
                        }
                    }
                    channel_type = channel_type_value = (short)(channel_type + 1);
                } while (channel_type < 5);

                short voice_count = preferences->voice_count;
                globals->initialized = true;
                globals->voice_count = voice_count;
            }
        }
    }

    g_sound_initialization_count++;
    main_set_status_value("sound", "init done");
}
/* ==== sound impulse gating ==== */
static __forceinline s_sound_cache_file_gestalt_definition *sound_manager_impulse_gate_gestalt_get(void)
{
    return (s_sound_cache_file_gestalt_definition *)g_sound_cache_file_gestalt.data;
}
static __forceinline bool sound_manager_impulse_gain_audible(const real_decibel &gain_db);

// @address 0x1a3cd0 ?sound_definition_can_start_new_impulse_sound@@YA_NJPBUs_sound_impulse_definition@@PAJPAW4e_sound_impulse_new_failure_reason@@@Z
bool sound_definition_can_start_new_impulse_sound(long definition_index, const s_sound_impulse_definition *impulse, long *sound_priority,
    e_sound_impulse_new_failure_reason *failure_reason)
{
    bool result = false;
    const s_sound_source *source = &impulse->source;
    long reason = 5;
    if (sound_manager_sound_is_active())
    {
        if (sound_definition_is_valid(definition_index))
        {
            reason = 1;
            const s_cache_file_sound_definition *definition = TAG_GET('snd!', const s_cache_file_sound_definition, definition_index);
            random_seed_next(get_local_random_seed_address());
            float random = (float)random_seed_high_half(get_local_random_seed_address()) * 1.5259022e-05f;
            float volume = source->volume;
            const s_sound_scale_modifiers *scale = &sound_manager_impulse_gate_gestalt_get()->scale_modifiers[definition->scale_index];
            float threshold = sound_manager_impulse_gate_gestalt_get()->playback_parameters[definition->playback_index].gain
                * ((scale->skip_fraction_upper.to_real() - scale->skip_fraction_lower.to_real()) * volume + scale->skip_fraction_lower.to_real());
            if (random > threshold)
            {
                reason = 4;
                const s_cache_file_sound_definition *cache_definition =
                    TAG_GET('snd!', const s_cache_file_sound_definition, definition_index);
                s_cache_file_sound_pitch_range *pitch_range = &sound_manager_impulse_gate_gestalt_get()->pitch_ranges[cache_definition->pitch_range_index];
                bool has_permutations = sound_definition_is_valid(definition_index)
                    && cache_definition->pitch_range_count
                    && sound_manager_pitch_range_get_permutation_count(pitch_range) != 0;
                if (has_permutations)
                {
                    float maximum_distance = sound_manager_source_test(source, 3) ? source->maximum_distance
                        : sound_definition_get_maximum_distance(definition_index);
                    float minimum_distance = sound_definition_get_silence_distance(definition_index);
                    float distance;
                    *sound_priority = source_audible_internal(source, minimum_distance, maximum_distance, &distance);
                    if (*sound_priority != -1)
                    {
                        s_sound_transmission_transform transform;
                        sound_source_get_transmission_transform(source, definition_index, &transform);
                        float attenuation;
                        if (distance >= transform.attack_distance)
                        {
                            attenuation = calculate_simple_distance_attenuation(
                                transform.minimum_distance,
                                transform.maximum_distance,
                                distance);
                        }
                        else
                        {
                            float maximum = transform.maximum_distance;
                            attenuation = calculate_simple_distance_attenuation(
                                maximum - transform.attack_distance,
                                maximum - transform.silence_distance,
                                maximum - distance);
                        }
                        const real_decibel &gain_quantity = sound_definition_map_gain_db(definition, source->gain, volume);
                        const real_decibel &attenuation_quantity = real_gain_to_decibels(attenuation);
                        const float &attenuation_db = attenuation_quantity.value;
                        const float &gain = gain_quantity.value;
                        real_decibel attenuated_gain = { gain + attenuation_db };
                        real_decibel gain_db = attenuated_gain
                            + sound_manager_impulse_gate_gestalt_get()->playback_parameters[definition->playback_index].gain_base;
                        if (sound_manager_source_test(source, 11) || sound_manager_source_test(source, 8) ||
                            sound_manager_impulse_gain_audible(gain_db))
                        {
                            reason = 0;
                            result = true;
                        }
                    }
                }
            }
        }
    }
    if (failure_reason)
        *failure_reason = (e_sound_impulse_new_failure_reason)reason;
    return result;
}

static __forceinline bool sound_manager_impulse_gain_audible(const real_decibel &gain_db)
{
    bool audible = gain_db > k_sound_manager_minimum_gain_db;
    return audible;
}


extern "C" {

}

static __forceinline void sound_manager_update_time(s_sound_manager_globals *globals, unsigned long now)
{
    long elapsed = (long)(now - globals->reset_time);
    globals->reset_time = now;
    float ducker_delta = (float)elapsed * 0.001f;
    float elapsed_seconds = (float)elapsed / 1000.0f;
    globals->reverb_rate = elapsed_seconds;
    globals->ducker_time += ducker_delta;
    globals->previous_ducker_time += ducker_delta;
    globals->time += elapsed;

    if (game_in_progress() && game_is_campaign())
    {
        if (game_globals->game_lost)
        {
            if (globals->fade_out_delay_milliseconds == -1)
                globals->fade_out_delay_milliseconds = (long)(globals->solo_player_fade_out_delay * 1000.0f);
            if (globals->fade_out_delay_milliseconds > 0)
            {
                globals->fade_out_delay_milliseconds = MAX(globals->fade_out_delay_milliseconds - elapsed, 0);
            }
            if (globals->fade_out_delay_milliseconds == 0)
                interpolate_scalar(&globals->master_gain, 0.0f, elapsed_seconds / MAX(0.001f, globals->solo_player_fade_out_time));
        }
        else if (!main_has_pending_state_changes())
        {
            float delay = 0.0f;
            if (globals->fade_in_delay_milliseconds > 0)
            {
                delay = (float)globals->fade_in_delay_milliseconds * 0.001f;
                globals->fade_in_delay_milliseconds = MAX(globals->fade_in_delay_milliseconds - elapsed, 0);
            }
            if (globals->fade_in_delay_milliseconds == 0)
            {
                interpolate_scalar(&globals->master_gain, 1.0f, (sound_manager_globals_view_get()->reverb_rate - delay) / MAX(0.001f,
                    globals->solo_player_fade_in_time));
                globals->fade_out_delay_milliseconds = -1;
            }
        }
    }
    else if (game_in_progress() && game_globals->options.game_mode == 2 && game_is_cooperative())
    {
        int film_tick = saved_film_get_target_tick();
        bool behind = false;
        if (film_tick != -1 && const_cast<const long &>(g_saved_film_playback_tick.value) < film_tick)
            behind = true;
        if (behind)
            globals->master_gain = 0.0f;
        else
            interpolate_scalar(&globals->master_gain, 1.0f, sound_manager_globals_view_get()->reverb_rate / MAX(0.001f,
                globals->solo_player_fade_in_time));
    }
    else
    {
        globals->master_gain = 1.0f;
    }
}

// @address 0x1a39e8 sound_update_time
extern "C" void sound_update_time(void)
{
    unsigned long now = GetTickCount();
    sound_manager_update_time(sound_manager_globals_view_get(), now);
}
