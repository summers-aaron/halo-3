// blamlibxbox/looping_sound_manager.obj: from the records 0x1289e8 0x129770 0x129858 0x13a0e0 0x13aab8 0x13aba8 0x13d7f0 0x1f7c20 0x1f7df8 0x1f7e70 0x1f7f60 0x1f8030 0x1f80e0 0x1f81f8 0x1f8378 0x1f85c0 0x1f8620 0x1f86f8 0x1f8bd0 0x1f8d30 0x1f8e30 0x1f8f90 0x1f9290 0x1f92f0 0x1f9590 0x1f9618 0x1f96c8 0x1f9748 0x1f9aa0 0x1f9c00 0x1f9ca0
// @flags /TP /O2 /Oc
// @address 0x2251c k_real_zero_storage
#include "render/render_objects_static_lighting.h"
#include "sound/sound_source_types.h"
#include "sound/looping_sound_definitions.h"
#include "game/player_mapping_globals.h"
#include "math/real_math_constants.h"
#include "sound/sound_manager_runtime_types.h"
#include "sound/sound_cache_file_gestalt_globals.h"
#include "sound/sound_cache_file_gestalt.h"
#include "game/game_globals.h"
#include "sound/sound_xaudio_xenon.h"
#define NOD3D
#include "sound/sound_xaudio_xenon_source_manager_types.h"
#undef NOD3D
#include "game/players.h"
#include "game/player_mapping_types.h"
#include "sound/sound_event_fade_reason_types.h"
#include "math/random_math.h"
#include "sound/looping_sound_manager.h"
#include "sound/sound_cache_file_definitions.h"
#include "sound/sound_definition_inline.h"
#include "sound/sound_classes.h"
#include "sound/sound_definitions.h"
#include "sound/sound_manager.h"
#include "sound/sound_playback_controller.h"
#include "sound/sound_playback_controller_types.h"
#include "memory/data.h"
#include "memory/datum_index.h"
#include "math/random_math_range.h"
#include "sound/game_sound_definitions.h"
#include "sound/sound_channel_summary_types.h"
#include "scenario/scenario.h"
struct looping_sound_track_definition;
struct looping_sound_definition;
#include "memory/data_types.h"
#include "tag_files/tag_groups.h"
#include "sound/sound_manager_types.h"
#include "math/real_math.h"
#include <ppcintrinsics.h>
#include <math.h>
#include <float.h>
// @rdata 0x82021694
static const float k_looping_sound_maximum_audible_distance = FLT_MAX;
#include <stdlib.h>
#include <string.h>
#include "sound/sound_xaudio_xenon_3d_voice_types.h"
#include "sound/sound_class_definitions.h"
#include "sound/sound_classes_types.h"
#include "sound/sound_globals_reference_types.h"
// @address 0x1f8bd0 ?loop_track_fade_out@@YAXW4sound_event_fade_reason@@MJ@Z
// @address 0x1f8d30 ?loop_track_fade_in@@YAXFW4sound_event_fade_reason@@MJ@Z
// @address 0x1f7f60 looping_sound_iterate_scripted
// @address 0x1f8030 looping_sound_increment_pending_count
// @address 0x1f81f8 ?looping_sound_stop@@YAXJJW4e_sound_event_stop_reason@@@Z
// @address 0x1f7df8 ?next@c_track_sound_iterator@@QAA_NXZ
// @address 0x1f7e70 ?looping_sound_manager_reset@@YAXXZ
// @address 0x1f85c0 ?looping_sound_delete@@YAXJ@Z
// @address 0x1f8620 ?looping_sound_find_sound@@YAJJJJF@Z
// @address 0x1f8f90 ?track_loop_detail_impulse_sound@@YA_NJJPBUloop_impulse_sound_tracking_data@@PAUs_sound_source@@@Z
// @address 0x1f9290 ?are_detail_sounds_equivalent@@YA_NPBUloop_impulse_sound_tracking_data@@0@Z
// @address 0x1f9590 ?looping_sound_track_get_range_permutation_flags@@YAKPAUs_cache_file_sound_definition@@PAUlooping_sound_definition@@PAUlooping_sound_datum@@JF_N@Z
// @address 0x1f9618 ?looping_sound_track_set_range_permutation_flags@@YAXPAUs_cache_file_sound_definition@@PAUlooping_sound_definition@@PAUlooping_sound_datum@@JFFK@Z
// @address 0x1f96c8 ?looping_sound_track_get_last_playing_permutation_index@@YAJPAUs_cache_file_sound_definition@@PAUlooping_sound_definition@@J@Z
// @address 0x1f9c00 ?looping_sound_find@@YAJJJ@Z
// @address 0x1f9ca0 ?looping_sound_delete_all_of_sound_class@@YAXJ@Z
// @address 0x129858 ?looping_sound_try_to_get_mouth_apeture@@YAMJJJPBUs_sound_source@@@Z
// @address 0x13aba8 get_priority_cutoff_for_looping_sound_definition_index
// @address 0x1f9aa0 ?detail_sound_random_offset@@YAXPAUlooping_sound_detail@@PATreal_vector3d@@@Z
// @address 0x1f9748 ?detail_sound_build_impulse_definition@@YA_NPAUlooping_sound_detail@@JPAUs_sound_impulse_definition@@@Z

struct looping_sound_datum;
struct looping_sound_track_datum;


#include "sound/looping_sound_datum.h"
struct looping_sound_component_flags
{
    unsigned char value;
    __forceinline bool test(long bit) const { return (value & (1UL << bit)) != 0; }
    __forceinline void clear() { value = 0; }
    __forceinline void set(long bit, bool enabled)
    {
        unsigned char mask = (unsigned char)(1UL << bit);
        if (enabled)
            value = (unsigned char)(value | mask);
        else
            value = (unsigned char)(value & ~mask);
    }
};
struct track_context
{
    long controller;
    union
    {
        signed char track;
        c_enum_no_init<long, char, 0, 4> track_index;
    };
    looping_sound_component_flags component_flags;
    signed char synchronized;
    char pad7;
    unsigned long random_seed, permutation_flags;
    long next, prev;
};


typedef char looping_sound_datum_size_check[(sizeof(void *) != 4 || ((sizeof(sound_datum) == 0xc0))) ? 1 : -1];


#include "sound/sound_data.h"
#include "sound/game_sound_restore.h"
#include "memory/data_iterator_noalias.h"
#include "sound/looping_sound_manager_dependencies.h"
#include "math/real_math_functions.h"
#include "sound/sound_math.h"
#include "sound/sound_xaudio_xenon_mix_master.h"

static __forceinline sound_datum *looping_sound_get_sound(long index);

struct loop_impulse_sound_tracking_data
{
    // Stored as a position, passed to the detail randomizer as a vector.
    union
    {
        real_point3d offset;
        real_vector3d offset_vector;
    };
    unsigned long flags;
};

enum e_sound_gain_scale_type
{
    _sound_gain_scale_linear,
    _sound_gain_scale_power,
    _sound_gain_scale_inverse_power,
};

struct looping_sound_tracker_constants
{
    unsigned long looping_callbacks[8];
};

struct looping_sound_minimum_gain_constants
{
    real_decibel gain_db;
    float default_seconds;
    unsigned long reserved;
    float ratio;
};

struct looping_sound_reference_offset_table
{
    s_tag_reference looping_sound_track_definition::*references[7];
    unsigned long next_object_constants[2];
};

struct looping_sound_detail
{
    long name;
    s_tag_reference sound;
    float delay_lower, delay_upper;
    real_decibel gain;
    unsigned long flags;
    float yaw_lower, yaw_upper;
    float pitch_lower, pitch_upper;
    float distance_lower, distance_upper;
};


struct looping_sound_tag { unsigned short flags; signed char sound_class, unused; short kind_index; };

struct looping_sound_channel_quad { float n[4]; };

struct looping_sound_channel_mix { long values[6]; };

namespace {
class c_track_sound
{
public:
    c_track_sound(long sound_index)
    {
        m_sound_index = sound_index;
        m_context = (track_context *)looping_sound_get_sound(m_sound_index)->event_parameters;
    }
    long get_next_index() const { return m_context->next; }
private:
    long m_sound_index;
    track_context *m_context;
};
}

class c_track_sound_iterator
{
public:
    c_track_sound_iterator(long first_index) { m_first_index = first_index; m_current_index = first_index; m_count = 0; }
    bool next();
    long get_index() const { return m_current_index; }
private:
    long m_first_index;
    long m_current_index;
    long m_count;
};

template<class t_datum> class c_looping_sound_data_iterator
{
public:
    __forceinline void begin(s_data_array *data)
    {
        m_iterator.data = data;
        m_iterator.absolute_index = -1;
        m_iterator.index = -1;
    }
    __forceinline bool next()
    {
        m_datum = (t_datum *)data_iterator_next(&m_iterator);
        return m_datum != NULL;
    }
    __forceinline long get_index() const { return m_iterator.index; }

private:
    t_datum *m_datum;
    s_data_iterator m_iterator;
};

template<typename T, long N> class process_static_array {
public:
    __forceinline T &operator[](long index) { return m_storage[index]; }
    __forceinline void set_all(const T &value) { for (long i = 0; i < N; i++) m_storage[i] = value; }
    T m_storage[N];
};

// match: preserve the original unsigned-byte flag test and its shift extraction.
static __forceinline unsigned char looping_sound_source_test(const s_sound_source *source, long bit)
{
    return (source->flags >> bit) & 1;
}
static __forceinline void detail_sound_initialize_orientation(s_sound_source *source);


// @address 0x7e3650 ?g_looping_sound_data@@3V?$c_smart_data_array@Ulooping_sound_datum@@@@A
c_smart_data_array<looping_sound_datum> g_looping_sound_data;


static __forceinline sound_datum *looping_sound_get_sound(long index)
{
    // match: 0x13a0e0: direct storage access preserves the original pointer-load boundary.
    s_data_array *array = g_sound_data.m_data;
    return &((sound_datum *)array->data)[DATUM_INDEX_TO_ABSOLUTE_INDEX(index)];
}
static __forceinline looping_sound_datum *looping_sound_get_loop_unchecked(long index)
{
    s_data_array *array = g_looping_sound_data;
    return datum_get<looping_sound_datum>(array, index);
}
static __forceinline void *looping_sound_datum_try_and_get(const s_data_array *array, long index)
{
    void *result = 0;
    if (index != -1)
    {
        long absolute = index & 65535;
        if (absolute < array->first_unallocated)
        {
            void *datum = array->data + array->size * absolute;
            short identifier = *(short *)datum;
            if (identifier != 0 && identifier == (index >> 16))
                result = datum;
        }
    }
    return result;
}
static __forceinline unsigned char looping_sound_location_get(unsigned char location) { return (location >> 4) & 15; }

static __forceinline const s_cache_file_sound_pitch_range *looping_sound_definition_get_pitch_range(const s_cache_file_sound_definition *definition, long index)
{
    return &g_sound_cache_file_gestalt.data->pitch_ranges[(short)definition->pitch_range_index + index];
}


static __forceinline long looping_sound_data_next_index(s_data_array *array, long index)
{
    long absolute_index = ::data_next_absolute_index((const ::s_data_array *)(array), (index + 1) & 0xffff);
    long result = -1;
    if (absolute_index != -1)
        result = (*(unsigned short *)(array->data + absolute_index * array->size) << 16) | absolute_index;
    return result;
}


inline bool c_track_sound_iterator::next()
{
    if (m_count > 0 && m_current_index != -1)
    {
        c_track_sound track_sound(m_current_index);
        m_current_index = track_sound.get_next_index();
        if (m_current_index == m_first_index)
            m_current_index = -1;
    }
    m_count++;
    return m_current_index != -1;
}

// 0x1f7f60 (no record)
// match: 0x1f7f60: __restrict on state keeps the definition_index store below the volume and alternate loads; without it c2 hoists it above them.
extern "C" bool looping_sound_iterate_scripted(game_sound_restore_iterator *iterator, game_sound_restore_state *__restrict state)
{
    bool result = false;
    while (true)
    {
        iterator->index = looping_sound_data_next_index(g_looping_sound_data, iterator->index);
        if (iterator->index == -1)
            break;
        looping_sound_datum *loop = looping_sound_get_loop_unchecked(iterator->index);
        if (loop->state_flags.test(5))
        {
            state->definition_index = loop->definition_index;
            state->mode = (signed char)loop->state;
            state->volume = loop->volume;
            state->sound_identifier = loop->sound_identifier;
            state->alternate = loop->state_flags.test(0);
            result = true;
            break;
        }
    }
    return result;
}

static __forceinline bool looping_sound_is_fading_out(const sound_datum *sound)
{
    bool result = sound->flags.test(7) && sound->fade_start_time > sound->fade_end_time;
    return result;
}

// 0x1f8030 (no record)
extern "C" void looping_sound_increment_pending_count(long loop_index, long unused, long sound_index, track_context *context)
{
    sound_datum *sound = looping_sound_get_sound(sound_index);
    if (!looping_sound_is_fading_out(sound))
    {
        looping_sound_datum *loop = (looping_sound_datum *)datum_try_and_get(g_looping_sound_data, loop_index);
        if (loop)
            loop->tracks[context->track].pending_count++;
    }
}

static __forceinline void looping_sound_controller_synchronized_sound_ready(c_sound_playback_controller *controller)
{
    if (controller->owner_identifier != -1)
    {
        controller->synchronized_sound_count--;
        controller->synchronized_sounds_ready = false;
    }
}

enum e_sound_event_stop_reason;


static __forceinline sound_datum *looping_sound_stop_sound_get(long index)
{
    return datum_get<sound_datum>(g_sound_data, index);
}


static __forceinline bool looping_sound_manager_is_active()
{
    bool result = ((s_sound_manager_globals *)g_sound_manager_globals)->initialized &&
        ((s_sound_manager_globals *)g_sound_manager_globals)->enabled && ((s_sound_manager_globals *)g_sound_manager_globals)->active;
    return result;
}
static __forceinline float looping_sound_real_random_range(unsigned long *seed, float lower, float upper)
{
    random_seed_next(seed);
    float random = *seed >> 16;
    return lower + (upper - lower) * (1.5259022e-05f * (float)random);
}

// 0x1f80e0
struct s_sound_output_parameters;

// 0x1f7c20

// @rdata 0x820360e0..0x82036190
static const real_decibel k_looping_sound_zero_gain_db = { 0.0f };
static const looping_sound_minimum_gain_constants k_looping_sound_minimum_gain_db = { { -64.0f }, 0.5f, 0x42df18dc, 0.75f };
#if defined(_XBOX) && !defined(__clang__)
static const looping_sound_tracker_constants k_looping_sound_trackers = {
    { 0x821f8e30, 0x821f8030, 0, 0x821f80e0, 0x821f81f8, 0, 0x8242d380, 0 }
};
// Seven words: the eighth callback (compare_proc) is the zero of the unity gain that follows.
static const unsigned long k_looping_sound_detail_callbacks[7] = { 0x821f8f90, 0, 0, 0, 0, 0, 0x821f9290 };
#else
// The tables hold the real functions; three of them type their parameter block, so they are stored through the slot types.
// Slot six of the loop tracker table is the shared ICF body at 0x42d380 that only returns true.
static bool looping_sound_parameters_equivalent(const void *, const void *)
{
    return true;
}
static const s_sound_event_callbacks k_looping_sound_trackers = {
    track_loop_track_sound, (void (*)(long, long, long, void *))looping_sound_increment_pending_count, 0,
    looping_sound_update_output_properties, looping_sound_stop, 0, looping_sound_parameters_equivalent, 0
};
static const s_sound_event_callbacks k_looping_sound_detail_callbacks = {
    (bool (*)(long, long, const void *, s_sound_source *))track_loop_detail_impulse_sound, 0, 0, 0, 0, 0,
    (bool (*)(const void *, const void *))are_detail_sounds_equivalent, 0
};
#endif
static const real_decibel k_looping_sound_unity_gain_db = { 0.0f };
static const long k_looping_sound_normal_transitions[7] = { 1, 1, 2, 5, 5, 1, 6 };
static const long k_looping_sound_alternate_transitions[7] = { 3, 3, 2, 4, 4, 3, 6 };
static const looping_sound_reference_offset_table k_looping_sound_reference_offsets = { { &looping_sound_track_definition::start,
    &looping_sound_track_definition::loop, &looping_sound_track_definition::end, &looping_sound_track_definition::alternate_start,
    &looping_sound_track_definition::alternate_loop, &looping_sound_track_definition::alternate_transition,
    &looping_sound_track_definition::alternate_end }, { 0x7f7fffff, 0x3c8efa35 } };
// @rdata-end

static __forceinline const s_sound_gestalt_pitch_parameters *looping_sound_pitch_range_get_parameters(const s_cache_file_sound_pitch_range *pitch_range)
{
    return TAG_BLOCK_GET_ELEMENT(&g_sound_cache_file_gestalt.data->pitch_parameters, pitch_range->parameter_index, s_sound_gestalt_pitch_parameters);
}
static __forceinline const s_sound_pitch_bounds *sound_pitch_range_get_bend_bounds(const s_cache_file_sound_pitch_range *pitch_range)
{
    return &looping_sound_pitch_range_get_parameters(pitch_range)->bend_bounds;
}
static __forceinline const s_sound_pitch_bounds *sound_pitch_range_get_runtime_bend_bounds(const s_cache_file_sound_pitch_range *pitch_range)
{
    return &looping_sound_pitch_range_get_parameters(pitch_range)->runtime_bend_bounds;
}
static __forceinline float looping_sound_real_decibels_to_gain(real_decibel decibels)
{
    return (float)exp(decibels.to_real() * 0.115129255f);
}
static __forceinline real_decibel looping_sound_real_decibel_interpolate(
    e_sound_gain_scale_type scale_type, real_decibel lower, real_decibel upper, float fraction)
{
    float lower_gain = looping_sound_real_decibels_to_gain(lower);
    float upper_gain = looping_sound_real_decibels_to_gain(upper);
    float curve;
    switch (scale_type)
    {
    case _sound_gain_scale_linear:
        curve = fraction;
        break;
    case _sound_gain_scale_power:
        curve = lower_gain < upper_gain ? sqrtf(fraction) : sqrtf(1.0f - fraction);
        break;
    case _sound_gain_scale_inverse_power:
        curve = lower_gain < upper_gain ? 1.0f - sqrtf(1.0f - fraction) : 1.0f - sqrtf(fraction);
        break;
    }
    return real_gain_to_decibels((upper_gain - lower_gain) * curve + lower_gain);
}
static __forceinline real_decibel scale_real_decibel(e_sound_gain_scale_type scale_type, real_decibel decibel, float scale)
{
    scale = PIN(scale, 0.0f, 1.0f);
    return looping_sound_real_decibel_interpolate(scale_type, k_looping_sound_minimum_gain_db.gain_db, decibel, scale);
}

// @address 0x1f7c20 ?sound_pitch_range_map_gain_db@@YA?AUreal_decibel@@PBUs_cache_file_sound_definition@@PBUs_cache_file_sound_pitch_range@@M@Z
real_decibel sound_pitch_range_map_gain_db(const s_cache_file_sound_definition *cache_file_sound_definition,
    const s_cache_file_sound_pitch_range *cache_file_sound_pitch_range, float pitch)
{
    real_decibel result = k_looping_sound_zero_gain_db;
    if (TEST_BIT(cache_file_sound_definition->flags, 11))
    {
        const s_sound_pitch_bounds *bend_bounds = sound_pitch_range_get_bend_bounds(cache_file_sound_pitch_range);
        const s_sound_pitch_bounds *runtime_bend_bounds = sound_pitch_range_get_runtime_bend_bounds(cache_file_sound_pitch_range);
        if (!(fabs(pitch - (float)runtime_bend_bounds->lower) < 0.0001f) && pitch < (float)runtime_bend_bounds->lower)
        {
            result = scale_real_decibel(_sound_gain_scale_power, k_looping_sound_unity_gain_db,
                normalize_and_clamp_linear((float)bend_bounds->lower, (float)runtime_bend_bounds->lower, pitch, true));
        }
        else if (!(fabs(pitch - (float)runtime_bend_bounds->upper) < 0.0001f) && pitch > (float)runtime_bend_bounds->upper)
        {
            result = scale_real_decibel(_sound_gain_scale_power, k_looping_sound_unity_gain_db,
                normalize_and_clamp_linear((float)bend_bounds->upper, (float)runtime_bend_bounds->upper, pitch, false));
        }
    }
    return result;
}

static __forceinline real_decibel looping_sound_add_decibels(real_decibel a, real_decibel b)
{
    real_decibel result;
    result.value = a.value + b.value;
    return result;
}

// @address 0x1f80e0 ?looping_sound_update_output_properties@@YA_NJJPBXPAUs_sound_output_parameters@@@Z
bool looping_sound_update_output_properties(long loop_index, long definition_index, const void *source, s_sound_output_parameters *parameters)
{
    bool result = false;
    looping_sound_datum *loop = (looping_sound_datum *)looping_sound_datum_try_and_get(g_looping_sound_data, loop_index);
    if (loop && loop->sound_identifier == (loop->definition_index | 0x6000))
    {
        s_sound_output_parameters *output = (s_sound_output_parameters *)parameters;
        const track_context *gain_source = (const track_context *)source;
        const looping_sound_track *track = &loop->tracks[gain_source->track];
        const s_cache_file_sound_definition *definition = (const s_cache_file_sound_definition *)tag_get(0, definition_index);
        for (long i = 0; i < 4; i++)
        {
            real_decibel gain = real_gain_to_decibels(track->gains[i]);
            real_decibel mapped = sound_definition_map_gain_db(definition, gain, loop->volume);
            real_decibel offset = loop->gain_offset;
            output->output_gains.v[i] = looping_sound_add_decibels(offset, mapped);
        }
        output->flags |= 16;
        result = true;
    }
    return result;
}

void looping_sound_stop(long loop_index, long sound_index, e_sound_event_stop_reason stop_reason)
{
    looping_sound_datum *looping = (looping_sound_datum *)looping_sound_datum_try_and_get(g_looping_sound_data, loop_index);
    sound_datum *playing = looping_sound_stop_sound_get(sound_index);
    track_context *track = (track_context *)playing->event_parameters;
    long track_index = track->track;
    long next_sound = -1;
    if (track->next != sound_index)
    {
        sound_datum *previous = looping_sound_stop_sound_get(track->prev);
        track_context *prev = (track_context *)previous->event_parameters;
        sound_datum *next = looping_sound_stop_sound_get(track->next);
        track_context *next_track = (track_context *)next->event_parameters;
        prev->next = track->next;
        next_track->prev = track->prev;
        next_sound = track->next;
    }
    if (looping)
    {
        looping->components--;
        if (looping->tracks[track_index].sound == sound_index)
            looping->tracks[track_index].sound = next_sound;
    }
    if (track->controller != -1)
    {
        c_sound_playback_controller *controller = (c_sound_playback_controller *)datum_try_and_get(g_sound_playback_controller_data, track->controller);
        if (controller && track->synchronized == -1)
            looping_sound_controller_synchronized_sound_ready(controller);
    }
}

// 0x1f8378

static __forceinline c_sound_playback_controller *looping_sound_get_playback_controller(long controller)
{
    c_sound_playback_controller *result = g_default_playback_controller;
    if (controller != -1)
        result = datum_get<c_sound_playback_controller>((s_data_array *)g_sound_playback_controller_data, controller);
    return result;
}

static __forceinline float looping_sound_new_random_delay(unsigned long *seed, float lower, float upper)
{
    random_seed_next(seed);
    float fraction = (float)(*seed >> 16) * 1.5259022e-05f;
    return (upper - lower) * fraction + lower;
}

// @address 0x1f8378 ?looping_sound_new@@YAJJKJJJPBUs_sound_source@@@Z
long looping_sound_new(long definition_index, unsigned long salt, long sound_identifier, long owner_identifier, long flags, const s_sound_source *source)
{
    long index = -1;
    if (looping_sound_manager_is_active())
    {
        index = datum_new(g_looping_sound_data);
        if (index != -1)
        {
            looping_sound_datum *loop = looping_sound_get_loop_unchecked(index);
            loop->definition_index = definition_index;
            loop->update_flags = (unsigned short)salt;
            loop->sound_identifier = sound_identifier;
            loop->owner = flags;
            loop->controller = sound_playback_controller_find_or_create(owner_identifier);
            loop->components = 0;
            loop->state_flags.value = 0;
            looping_sound_definition *definition = (looping_sound_definition *)tag_get(0, definition_index);
            for (long i = 0; i < definition->details.count; i++)
            {
                looping_sound_detail *details = (looping_sound_detail *)definition->details.address;
                looping_sound_detail *detail = &details[i];
                float delay;
                if (detail->flags & 4)
                    delay = 0.0f;
                else
                    delay = looping_sound_new_random_delay(get_local_random_seed_address(), detail->delay_lower, detail->delay_upper);
                loop->detail_times[i] = (long)(delay * 1000.0f + (float)((s_sound_manager_globals *)g_sound_manager_globals)->time);
            }
            if ((definition->flags & 32) || (definition->flags & 16))
            {
                c_sound_playback_controller *controller = looping_sound_get_playback_controller(loop->controller);
                for (long i = 0; i < definition->tracks.count; i++)
                {
                    if (controller->owner_identifier == -1)
                        random_seed_next(&controller->random_seed);
                    loop->tracks[i].random_seed = controller->random_seed;
                }
            }
        }
    }
    return index;
}

void looping_sound_delete(long index)
{
    sound_playback_controller_release_reference(looping_sound_get_loop_unchecked(index)->controller);
    ::datum_delete(g_looping_sound_data, index);
}

// The first entry in the owner's circular list for the slot whose key matches (and whose type does,
// unless type is -1), or -1. Advance after the first pass and stop on
// wrapping back to the first entry.
long looping_sound_find_sound(long owner_index, long key, long type, short slot)
{
    long first = looping_sound_get_loop_unchecked(owner_index)->tracks[slot].sound;
    long result = -1;
    long current = first;
    long count = 0;
    while (true)
    {
        if (count > 0 && current != -1)
        {
            track_context *current_track = (track_context *)looping_sound_get_sound(current)->event_parameters;
            current = current_track->next;
            if (current == first)
                current = -1;
        }
        count++;
        if (current == -1)
            break;
        sound_datum *entry = looping_sound_get_sound(current);
        if (entry->definition_index == key && (type == -1 || entry->pitch_range_index == type))
        {
            result = current;
            break;
        }
    }
    return result;
}


static __forceinline void looping_sound_manager_start_fade(
    short fade_type, sound_event_fade_reason reason, float fade_time, long sound_index, long secondary_sound_index)
{
    if (sound_index != -1)
    {
        sound_datum *sound = looping_sound_get_sound(sound_index);
        if (sound->flags.test(7))
        {
            sound->fade_gain_db = sound_calculate_fade_db(sound_index);
        }
        else
        {
            union { long bits; real_decibel decibels; } inaudible_db;
            inaudible_db.bits = -0x3d800000;
            sound->fade_gain_db = inaudible_db.decibels;
        }
        sound->fade_type = (signed char)fade_type;
        sound->fade_start_time = -1;
        sound->fade_end_time = (long)(fade_time * 1000.0f);
        sound->flags.set(7, true);
    }
    if (secondary_sound_index != -1)
    {
        sound_datum *sound = looping_sound_get_sound(secondary_sound_index);
        sound->fade_gain_db = sound_calculate_fade_db(secondary_sound_index);
        sound->fade_type = (signed char)fade_type;
        sound->fade_start_time = (long)(fade_time * 1000.0f);
        sound->fade_end_time = -1;
        sound->flags.set(7, true);
    }
}

void loop_track_fade_in(short fade_type, sound_event_fade_reason reason, float fade_time, long sound_index)
{
    if (sound_index != -1)
    {
        c_track_sound_iterator iterator(sound_index);
        while (iterator.next())
        {
            long index = iterator.get_index();
            sound_datum *sound = looping_sound_get_sound(index);
            if (sound->type == 2)
                looping_sound_manager_start_fade(1, reason, fade_time, index, -1);
        }
    }
}

bool track_loop_detail_impulse_sound(long loop_index, long sound_index, const loop_impulse_sound_tracking_data *tracking, s_sound_source *source)
{
    looping_sound_datum *loop = (looping_sound_datum *)looping_sound_datum_try_and_get(g_looping_sound_data, loop_index);
    bool result = false;
    if ((!loop || loop->state == 2) && (tracking->flags & 16))
    {
        source->source_flags.set(14, true);
        source->source_flags.set(15, true);
    }
    else if (loop)
    {
        if (looping_sound_location_get(loop->location) != 0)
        {
            source->velocity = loop->velocity;
            source->orientation = loop->orientation;
            source->location = loop->bsp_location;
            source->source_flags.set(2, loop->flags.test(2));
            source->minimum_distance = loop->minimum;
            source->source_flags.set(3, loop->flags.test(3));
            source->maximum_distance = loop->maximum;
            source->source_flags.set(4, loop->flags.test(4));
            source->inner_cone_angle = loop->inner;
            source->source_flags.set(5, loop->flags.test(5));
            source->outer_cone_angle = loop->outer;
            source->source_flags.set(6, loop->flags.test(6));
            source->outer_cone_gain = loop->outer_cone_gain;
        }
        else
        {
            detail_sound_initialize_orientation(source);
        }
        source->position = tracking->offset;
        if (looping_sound_location_get(source->spatialization) == 1 && looping_sound_location_get(loop->location) == 1)
        {
            source->position.x += loop->position.x;
            source->position.y += loop->position.y;
            source->position.z += loop->position.z;
        }
        if (tracking->flags & 8)
            source->volume = loop->volume;
        result = true;
    }
    return result;
}

bool are_detail_sounds_equivalent(const loop_impulse_sound_tracking_data *a, const loop_impulse_sound_tracking_data *b)
{
    return fabs(a->offset.x - b->offset.x) < 0.0001f
        && fabs(a->offset.y - b->offset.y) < 0.0001f
        && fabs(a->offset.z - b->offset.z) < 0.0001f;
}

unsigned long looping_sound_track_get_range_permutation_flags(s_cache_file_sound_definition *sound_definition,
    looping_sound_definition *definition, looping_sound_datum *loop, long track_index, short pitch_range_index, bool reset)
{
    unsigned long result;
    if (((definition->flags & 32) != 0 || (definition->flags & 16) != 0) && loop->controller != -1)
    {
        result = loop->tracks[track_index].range_permutation_flags;
        if (reset)
            loop->tracks[track_index].range_permutation_flags = 0;
    }
    else
    {
        const s_cache_file_sound_pitch_range *range = looping_sound_definition_get_pitch_range(sound_definition, pitch_range_index);
        result = sound_pitch_range_get_runtime_permutation_flags(sound_definition, range);
    }
    return result;
}

void looping_sound_track_set_range_permutation_flags(s_cache_file_sound_definition *sound_definition, looping_sound_definition *definition,
    looping_sound_datum *loop, long track_index, short pitch_range_index, short last_permutation_index, unsigned long flags)
{
    if (((definition->flags & 32) != 0 || (definition->flags & 16) != 0) && loop->controller != -1)
    {
        loop->tracks[track_index].range_permutation_flags = flags;
    }
    else
    {
        s_cache_file_sound_pitch_range *range = (s_cache_file_sound_pitch_range *)looping_sound_definition_get_pitch_range(sound_definition, pitch_range_index);
        sound_pitch_range_set_runtime_permutation_flags(sound_definition, range, flags);
        range->set_last_permutation_index(sound_definition, last_permutation_index);
    }
}

long looping_sound_track_get_last_playing_permutation_index(
    s_cache_file_sound_definition *sound_definition, looping_sound_definition *definition, long pitch_range_index)
{
    long result;
    if ((definition->flags & 32) == 0 && (definition->flags & 16) == 0)
    {
        const s_cache_file_sound_pitch_range *range = looping_sound_definition_get_pitch_range(sound_definition, pitch_range_index);
        result = range->get_last_permutation_index(sound_definition);
    }
    else
    {
        result = -1;
    }
    return result;
}

long looping_sound_find(long definition_index, long sound_identifier)
{
    long index;
    for (index = ::data_next_index(g_looping_sound_data, -1); index != -1;
        index = looping_sound_data_next_index(g_looping_sound_data, index))
    {
        looping_sound_datum *loop = looping_sound_get_loop_unchecked(index);
        if (loop->definition_index == definition_index && loop->sound_identifier == sound_identifier)
            break;
    }
    return index;
}


static __forceinline void looping_sound_manager_release_controller(long index)
{
    if (index == -1) return;
    s_data_array *data = g_sound_playback_controller_data;
    c_sound_playback_controller *controller = datum_get<c_sound_playback_controller>(data, index);
    controller->reference_count--;
    if (controller->reference_count == 0)
        controller->flags &= 0xfd;
    if (controller->reference_count == 0)
        ::datum_delete(g_sound_playback_controller_data, index);
}

void looping_sound_delete_all_of_sound_class(long sound_class)
{
    long index = ::data_next_index(g_looping_sound_data, -1);
    if (index != -1)
    {
        const cache_file_tag_instance *tag_instances = global_tag_instances;
        do
        {
            looping_sound_datum *loop = looping_sound_get_loop_unchecked(index);
            bool found = false;
            looping_sound_definition *definition =
                (looping_sound_definition *)tag_instances[datum_index_to_absolute_index(loop->definition_index)].definition;
            for (long track_index = 0; track_index < definition->tracks.count; track_index++)
            {
                long sound_index = loop->tracks[track_index].sound;
                if (sound_index != -1)
                {
                    sound_datum *sound = looping_sound_get_sound(sound_index);
                    if (((s_cache_file_sound_definition *)tag_instances[datum_index_to_absolute_index(sound->definition_index)].definition)
                            ->sound_class == sound_class)
                    {
                        found = true;
                        break;
                    }
                }
            }
            if (found)
            {
                looping_sound_manager_release_controller(loop->controller);
                ::datum_delete(g_looping_sound_data, index);
            }
            index = looping_sound_data_next_index(g_looping_sound_data, index);
        } while (index != -1);
    }
}


static __forceinline void looping_sound_manager_delete(long index)
{
    looping_sound_manager_release_controller(looping_sound_get_loop_unchecked(index)->controller);
    ::datum_delete(g_looping_sound_data, index);
}

void looping_sound_manager_reset()
{
    if (g_looping_sound_data)
    {
        c_looping_sound_data_iterator<looping_sound_datum> iterator;
        iterator.begin(g_looping_sound_data);
        while (iterator.next())
            looping_sound_manager_delete(iterator.get_index());
    }
}

struct sound_channel_datum;

static __forceinline long looping_sound_create_identifier(long index, long type) { return index | (type << 13); }
static __forceinline long looping_sound_seconds_to_ticks(float seconds) { return (long)(seconds * 30.0f + 0.5f); }

float looping_sound_try_to_get_mouth_apeture(long tag_index, long identifier, long unused, const s_sound_source *source)
{
    float result = 0.0f;
    if (looping_sound_manager_is_active())
    {
        looping_sound_definition *definition = (looping_sound_definition *)tag_get(0, tag_index);
        bool global = (source->spatialization & 0xf0) != 0 && (definition->flags & 128) != 0;
        long sound_identifier = global ? looping_sound_create_identifier(tag_index, 3) : identifier;
        long index = looping_sound_find(tag_index, sound_identifier);
        if (index != -1)
        {
            looping_sound_datum *loop = datum_get<looping_sound_datum>(g_looping_sound_data, index);
            long sound_index = -1;
            for (long i = 0; i < definition->tracks.count; i++)
            {
                if (loop->tracks[i].sound != -1)
                {
                    sound_index = loop->tracks[i].sound;
                    break;
                }
            }
            if (sound_index != -1)
            {
                sound_datum *sound = datum_get<sound_datum>(g_sound_data, sound_index);
                if (sound->channel_index != -1)
                {
                    sound_channel_datum *channel = g_sound_channels + sound->channel_index;
                    result = sound_permutation_get_real_mouth_aperture(
                        sound->definition_index, sound->pitch_range_index, sound->permutation_index, (short)looping_sound_seconds_to_ticks(channel->position));
                }
            }
        }
    }
    return result;
}

// @address 0x13aab8 looping_sound_get_active_sound_index
static __forceinline long looping_sound_manager_track_sound_index(const looping_sound_track_definition *track, signed char kind)
{
    const s_tag_reference *reference = &(track->*k_looping_sound_reference_offsets.references[kind]);
    return reference->index;
}
extern "C" long looping_sound_get_active_sound_index(long looping_sound_index)
{
    long result = -1;
    looping_sound_datum *loop = looping_sound_get_loop_unchecked(looping_sound_index);
    looping_sound_definition *definition = (looping_sound_definition *)tag_get(0, loop->definition_index);
    for (long track_index = 0; track_index < definition->tracks.count; track_index++)
    {
        looping_sound_track *track = &loop->tracks[track_index];
        if (track->sound != -1)
        {
            looping_sound_track_definition *track_definition = &((looping_sound_track_definition *)definition->tracks.address)[track_index];
            if (sound_definition_is_valid(looping_sound_manager_track_sound_index(track_definition, track->check_kind)))
                result = looping_sound_manager_track_sound_index(track_definition, track->test_kind);
            break;
        }
    }
    return result;
}

// @address 0xf8c88 ?source_audible_internal@@YAJPBUs_sound_source@@MMPAM@Z
// @address 0x1f8e30 ?track_loop_track_sound@@YA_NJJPBXPAUs_sound_source@@@Z


// @address 0x129770 looping_sound_mark_distant_duplicates
extern "C" void looping_sound_mark_distant_duplicates(long index, long definition_index, float minimum_distance)
{
    long current_index = index;
    if (current_index != -1)
    {
        float minimum = 0.0f;
        float maximum = k_looping_sound_maximum_audible_distance;
        do
        {
            s_data_array *data = g_looping_sound_data;
            looping_sound_datum *loop = datum_get<looping_sound_datum>(data, current_index);
            if (definition_index == looping_sound_get_active_sound_index(current_index))
            {
                float distance;
                source_audible_internal((const s_sound_source *)&loop->flags, minimum, maximum, &distance);
                if (distance >= minimum_distance)
                {
                    loop->state_flags.value |= 16;
                    data = g_looping_sound_data;
                }
            }
            current_index = looping_sound_data_next_index(data, current_index);
        } while (current_index != -1);
    }
}

extern "C" {
// @address 0xfa7e8 sound_set_permutation
// @address 0x13b140 sound_stop_and_unlock
}
// @address 0x13b410 ?sound_channel_summary_build@@YAXPAUsound_channel_summary@@J@Z
// @address 0x13b968 ?sound_find_like_channel@@YAFJPAFF@Z
// @address 0x1a7cb8 ?sound_definition_hardware_formats_match@@YA_NJJ@Z
static __forceinline sound_datum *sound_commit_get_sound(long index)
{
    return datum_get<sound_datum>(g_sound_data, index);
}
static __forceinline long sound_commit_track_definition(const looping_sound_track_definition *track, long index)
{
    return (track->*k_looping_sound_reference_offsets.references[index]).index;
}
static __forceinline void sound_commit_flag_set(unsigned short *flags, long bit, bool value)
{
    if (value) *flags |= (1UL << bit);
    else *flags &= ~(1UL << bit);
}

// @address 0x1f92f0 ?sound_commit_looping_definition@@YA_NPAUlooping_sound_datum@@PAUlooping_sound_track_datum@@JPAK@Z
bool sound_commit_looping_definition(struct looping_sound_datum * loop, struct looping_sound_track_datum * track, long index, unsigned long * random)
{
    bool result = false;
    sound_datum *sound = sound_commit_get_sound(index);
    looping_sound_definition *definition = (looping_sound_definition *)tag_get(0, loop->definition_index);
    track_context *state = (track_context *)sound->event_parameters;
    looping_sound_track_definition *track_definition_ptr = &((looping_sound_track_definition *)definition->tracks.address)[state->track_index];
    long queued = sound_commit_track_definition(track_definition_ptr, track->check_kind.m_storage);
    if (sound_definition_is_valid(queued))
    {
        if (sound_definition_hardware_formats_match(sound_commit_track_definition(track_definition_ptr, track->test_kind.m_storage), queued))
        {
            s_cache_file_sound_definition *queued_definition = (s_cache_file_sound_definition *)tag_get(0, queued);
            looping_sound_definition *loop_definition = (looping_sound_definition *)definition;
            long stop_index = -1;
            long reason = 0;
            sound_commit_flag_set(&sound->flags.m_flags, 3, true);
            short range = ::sound_definition_find_pitch_range_by_pitch(queued_definition, sound->pitch, sound->pitch_range_index);
            unsigned long flags = looping_sound_track_get_range_permutation_flags(queued_definition, loop_definition, loop, state->track_index, range, true);
            long last = looping_sound_track_get_last_playing_permutation_index(queued_definition, loop_definition, range);
            short permutation = ::sound_definition_next_permutation_deterministic(queued_definition,
                range, (short)last, (unsigned long *)((int *)&flags), (unsigned long *)(random), (bool *)(0), false);
            looping_sound_track_set_range_permutation_flags(queued_definition, loop_definition, loop, state->track_index, range, permutation, flags);
            sound_set_permutation(index, queued, range, permutation, 0);
            if ((unsigned short)sound->channel_index != 65535)
            {
                short count = 0;
                short *channels = 0;
                sound_channel_summary summary;
                sound_channel_summary_build(&summary, index);
                if (summary.object_channel_count >= summary.maximum_object_channels)
                {
                    reason = 5;
                    count = summary.object_channel_count;
                    channels = summary.object_channels;
                }
                else if (summary.like_channel_count >= summary.maximum_like_channels)
                {
                    reason = 6;
                    count = summary.like_channel_count;
                    channels = summary.like_channels;
                }
                if (count > 0)
                {
                    short chosen = sound_find_like_channel(index, channels, count);
                    if (chosen == -1) stop_index = index;
                    else stop_index = g_sound_channels[chosen].sound_index;
                }
            }
            if (stop_index != -1)
            {
                sound_stop_and_unlock(stop_index, reason);
                result = stop_index == index;
            }
            track->test_kind.m_storage = track->check_kind.m_storage;
        }
        else
        {
            sound_stop_and_unlock(index, 16);
            result = true;
        }
    }
    else
    {
        sound_stop_and_unlock(index, 16);
        result = true;
    }
    return result;
}

static __forceinline sound_globals_reference *looping_sound_get_sound_globals()
{
    return ((s_game_globals *)global_game_globals)->sound_globals.count() ? ((s_game_globals *)global_game_globals)->sound_globals.begin() : 0;
}

static __forceinline sound_class_definition *looping_sound_get_sound_class(short index)
{
    return &((sound_classes *)tag_get(0, looping_sound_get_sound_globals()->sound_classes.index))
                ->classes[index];
}

extern "C" float get_priority_cutoff_for_looping_sound_definition_index(
    long first_looping_sound_index, c_static_flags<128> &visited, long definition_index, long matching_sound_definition_index)
{
    float distances[16];
    for (long i = 0; i < 16; i++)
        distances[i] = FLT_MAX;
    long entry_index =
        ((looping_sound_definition *)tag_get(0, definition_index))
            ->entry_index;
    long maximum_count = looping_sound_get_sound_class(entry_index)->maximum_sounds_per_tag;
    float threshold = FLT_MAX;
    long replace_index = NONE;
    for (long looping_sound_index = first_looping_sound_index; looping_sound_index != NONE;
        looping_sound_index = looping_sound_data_next_index(g_looping_sound_data, looping_sound_index))
    {
        looping_sound_datum *looping_sound = looping_sound_get_loop_unchecked(looping_sound_index);
        if (matching_sound_definition_index == looping_sound_get_active_sound_index(looping_sound_index))
        {
            long absolute_index = looping_sound_index & 0xffff;
            if (!visited.test(absolute_index))
            {
                visited.set(absolute_index, true);
                float distance;
                source_audible_internal((const s_sound_source *)&looping_sound->flags, 0.0f, FLT_MAX, &distance);
                if (distance <= threshold)
                {
                    if (replace_index == NONE)
                    {
                        float maximum = -FLT_MAX;
                        long i = 0;
                        if (maximum_count > 0)
                        {
                            do
                            {
                                if (distances[i] > maximum)
                                {
                                    replace_index = i;
                                    maximum = distances[i];
                                }
                            } while (++i < maximum_count);
                        }
                    }
                    if (replace_index != NONE)
                    {
                        if (distances[replace_index] > distance)
                        {
                            threshold = distances[replace_index];
                            distances[replace_index] = distance;
                            replace_index = NONE;
                        }
                        else if (threshold > distance)
                        {
                            threshold = distance;
                        }
                    }
                }
            }
        }
    }
    return threshold;
}

bool track_loop_track_sound(long looping_sound_index, long definition_index, const void *unknown, s_sound_source *out_source)
{
    bool result = false;
    if (looping_sound_index != -1)
    {
        looping_sound_datum *looping_sound = (looping_sound_datum *)looping_sound_datum_try_and_get(g_looping_sound_data, looping_sound_index);
        if (looping_sound)
        {
            c_sound_playback_controller *unknown_element = g_default_playback_controller;
            if (looping_sound->controller != -1)
                unknown_element = datum_get<c_sound_playback_controller>((s_data_array *)g_sound_playback_controller_data, looping_sound->controller);
            if (!looping_sound->state_flags.test(4))
            {
                if (looping_sound->controller == -1 || (unknown_element->flags & 1) || !(unknown_element->flags & 2))
                {
                    s_sound_source *source = (s_sound_source *)&looping_sound->flags;
                    float maximum_distance = looping_sound_source_test(source,
                        3) ? source->maximum_distance : sound_definition_get_maximum_distance(definition_index);
                    float minimum_distance = sound_definition_get_silence_distance(definition_index);
                    if (source_audible_internal(source, minimum_distance, maximum_distance, 0) != -1)
                    {
                        memcpy(out_source, source, sizeof(s_sound_source));
                        result = true;
                        if (looping_sound->sound_identifier == (looping_sound->definition_index | 0x6000))
                            out_source->spatialization &= 15;
                    }
                }
            }
            else
            {
                unsigned char *state_flags = &looping_sound->state_flags.value;
                *state_flags &= 239;
            }
        }
    }
    return result;
}



static __forceinline const sound_class_definition *looping_sound_fadeout_class_get(short class_index)
{
    sound_globals_reference *sound_globals = ((s_game_globals *)global_game_globals)->sound_globals.count()
        ? (sound_globals_reference *)((s_game_globals *)global_game_globals)->sound_globals.begin()
        : 0;
    sound_classes *classes = (sound_classes *)tag_get(0, 
        sound_globals->sound_classes.index);
    return (const sound_class_definition *)classes->classes + class_index;
}

static __forceinline void looping_sound_fadeout_start_fade(
    short mode, sound_event_fade_reason reason, float seconds, long fade_in_sound_index, long fade_out_sound_index)
{
    if (fade_in_sound_index != -1)
    {
        sound_datum *sound = looping_sound_get_sound(fade_in_sound_index);
        if (sound->flags.test(7))
            sound->fade_gain_db = sound_calculate_fade_db(fade_in_sound_index);
        else
            *(long *)&sound->fade_gain_db = (long)0xc2800000;
        sound->fade_type = (signed char)mode;
        sound->fade_start_time = -1;
        sound->fade_end_time = (long)(seconds * 1000.0f);
        sound->flags.set(7, true);
    }
    if (fade_out_sound_index != -1)
    {
        sound_datum *sound = looping_sound_get_sound(fade_out_sound_index);
        sound->fade_gain_db = sound_calculate_fade_db(fade_out_sound_index);
        sound->fade_type = (signed char)mode;
        sound->fade_end_time = -1;
        sound->fade_start_time = (long)(seconds * 1000.0f);
        sound->flags.set(7, true);
    }
}

// match: noalias declaration preserves caller live registers

void loop_track_fade_out(sound_event_fade_reason reason, float fade_seconds, long looping_sound_index)
{
    if (looping_sound_index != -1)
    {
        c_track_sound_iterator iterator(looping_sound_index);
        while (iterator.next())
        {
            long sound_index = iterator.get_index();
            sound_datum *sound = looping_sound_get_sound(sound_index);
            s_cache_file_sound_definition *definition = (s_cache_file_sound_definition *)tag_get(0, sound->definition_index);
            long sound_class = definition->sound_class;
            short mode = !looping_sound_fadeout_class_get(sound_class)->loop_fade_out_linear();
            if (reason == 5 || reason == 6)
                mode = 2;
            looping_sound_fadeout_start_fade(mode, reason, fade_seconds, -1, sound_index);
        }
    }
}


// 0x1f9aa0 detail_sound_random_offset
void detail_sound_random_offset(looping_sound_detail *detail, real_vector3d *offset)
{
    float distance = looping_sound_real_random_range(get_local_random_seed_address(), detail->distance_lower, detail->distance_upper);
    if (distance != 0.0f)
    {
        real_euler_angles2d angles;
        angles.pitch = looping_sound_real_random_range(get_local_random_seed_address(), detail->pitch_lower, detail->pitch_upper);
        angles.yaw = looping_sound_real_random_range(get_local_random_seed_address(), detail->yaw_lower, detail->yaw_upper);
        vector3d_from_euler_angles2d(offset, &angles);
        offset->i = offset->i * distance;
        offset->j = offset->j * distance;
        offset->k = offset->k * distance;
    }
    else
    {
        *offset = *global_zero_vector3d;
    }
}

static __forceinline unsigned char looping_sound_new_set_component_flag(unsigned char flags)
{
    return flags | 1;
}
static __forceinline sound_datum *looping_sound_new_sound_get(long index)
{
    return datum_get<sound_datum>(g_sound_data, index);
}
static __forceinline looping_sound_datum *looping_sound_new_loop_get(long index)
{
    return datum_get<looping_sound_datum>(g_looping_sound_data, index);
}
static __forceinline void looping_sound_new_link(long loop_index, short track_index, long sound_index)
{
    looping_sound_track *track = &looping_sound_new_loop_get(loop_index)->tracks[track_index];
    track_context *context = (track_context *)looping_sound_new_sound_get(sound_index)->event_parameters;
    if (track->sound != -1)
    {
        track_context *previous = (track_context *)looping_sound_new_sound_get(track->sound)->event_parameters;
        context->next = previous->next;
        context->prev = track->sound;
        previous->next = sound_index;
        track_context *next_context = (track_context *)looping_sound_new_sound_get(context->next)->event_parameters;
        next_context->prev = sound_index;
    }
}

// @address 0x1f86f8 ?looping_sound_new_sound@@YAJJJJFFPAK@Z
long looping_sound_new_sound(long loop_index, long sound_definition_index, long pitch_range_index, short track_index, short type, unsigned long *random_seed)
{
    long result = -1;
    looping_sound_datum *loop = looping_sound_new_loop_get(loop_index);
    looping_sound_definition *definition = (looping_sound_definition *)tag_get(0, loop->definition_index);
    looping_sound_track_definition *track_definition = &((looping_sound_track_definition *)definition->tracks.address)[track_index];
    if (sound_definition_is_valid(sound_definition_index))
    {
        s_sound_source *source = (s_sound_source *)&loop->flags;
        s_cache_file_sound_definition *sound_definition = (s_cache_file_sound_definition *)tag_get(0, sound_definition_index);
        float maximum_distance = looping_sound_source_test(source,
            3) ? source->maximum_distance : sound_definition_get_maximum_distance(sound_definition_index);
        float silence_distance = sound_definition_get_silence_distance(sound_definition_index);
        long listener = source_audible_internal(source, silence_distance, maximum_distance, 0);
        if (listener != -1)
        {
            result = datum_new(g_sound_data);
            if (result != -1)
            {
                s_cache_file_sound_definition *cache = (s_cache_file_sound_definition *)sound_definition;
                s_sound_cache_file_gestalt_definition *gestalt = (s_sound_cache_file_gestalt_definition *)g_sound_cache_file_gestalt.data;
                s_sound_playback_parameters *playback =
                    &((s_sound_cache_file_gestalt_definition *)g_sound_cache_file_gestalt.data)->playback_parameters[cache->playback_index];
                looping_sound_track *track = &loop->tracks[track_index];
                sound_datum *sound = looping_sound_new_sound_get(result);
                sound->channel_index = -1;
                sound->listener_index = (signed char)listener;
                sound->flags.m_flags = 0;
                sound->pitch = looping_sound_new_random_delay(random_seed, (float)playback->pitch_lower, (float)playback->pitch_lower);
                sound->gain = track_definition->gain;
                sound->random_seed = *random_seed;
                sound->object_index = loop_index;
                memcpy(&sound->source, source, sizeof(s_sound_source));
                sound->type = (signed char)type;
                sound->playback_flags = loop->update_flags;
                sound->start_time = ((s_sound_manager_globals *)g_sound_manager_globals)->time;
                sound->callbacks = (const s_sound_event_callbacks *)&k_looping_sound_trackers;
                sound->fade_end_time = 0;
                sound->fade_start_time = 0;
                sound->hardware_voice_index = -1;
                sound->voice_state = -1;
                sound->playback_index = loop->owner;
                sound->playback_controller_index = -1;
                track_context *context = (track_context *)sound->event_parameters;
                context->track = (signed char)track_index;
                context->component_flags.clear();
                context->random_seed = track->random_seed;
                context->permutation_flags = track->range_permutation_flags;
                context->next = result;
                context->prev = result;
                if (sound->type == 2 && (cache->flags & 2048))
                    context->component_flags.set(0, true);
                if (((1 << (signed char)sound->type) & 30) && (definition->flags & 32))
                {
                    c_sound_playback_controller *controller = looping_sound_get_playback_controller(loop->controller);
                    if (controller->owner_identifier != -1)
                        controller->synchronized_sound_count++;
                    context->controller = loop->controller;
                    context->synchronized = -1;
                }
                else
                    context->controller = -1;
                long selected_range = pitch_range_index;
                if (selected_range == -1)
                {
                    s_sound_scale_modifiers *scale =
                        &((s_sound_cache_file_gestalt_definition *)g_sound_cache_file_gestalt.data)->scale_modifiers[cache->scale_index];
                    float minimum_pitch = (float)scale->pitch.lower;
                    float maximum_pitch = (float)scale->pitch.upper;
                    selected_range = ::sound_definition_find_pitch_range_by_pitch(sound_definition,
                        ((maximum_pitch - minimum_pitch) * track->pitch + minimum_pitch) + sound->pitch, -1);
                }
                short range = (short)selected_range;
                unsigned long flags = looping_sound_track_get_range_permutation_flags(
                    sound_definition, (looping_sound_definition *)definition, loop, track_index, range, type == 2);
                long last = looping_sound_track_get_last_playing_permutation_index(sound_definition, (looping_sound_definition *)definition, range);
                short permutation = ::sound_definition_next_permutation_deterministic(sound_definition,
                    range, (short)last, (unsigned long *)((int *)&flags), (unsigned long *)(random_seed), (bool *)(0), false);
                looping_sound_track_set_range_permutation_flags(
                    sound_definition, (looping_sound_definition *)definition, loop, track_index, range, permutation, flags);
                sound_set_permutation(result, sound_definition_index, range, permutation, 0);
                sound_make_permutation_ready((sound_datum *)sound, 0);
                looping_sound_new_link(loop_index, track_index, result);
                loop->components++;
            }
        }
    }
    return result;
}

// @rdata 0x82022dc0
static const float k_looping_sound_default_fade_seconds = 0.5f;


static __forceinline void looping_sound_refresh_start_fade(
    short mode, sound_event_fade_reason reason, float seconds, long fade_in_sound_index, long fade_out_sound_index)
{
    if (fade_in_sound_index != -1)
    {
        sound_datum *sound = looping_sound_get_sound(fade_in_sound_index);
        if (sound->flags.test(7))
            sound->fade_gain_db = sound_calculate_fade_db(fade_in_sound_index);
        else
            *(long *)&sound->fade_gain_db = (long)0xc2800000;
        sound->fade_type = (signed char)mode;
        sound->fade_start_time = -1;
        sound->fade_end_time = (long)(seconds * 1000.0f);
        sound->flags.set(7, true);
    }
    if (fade_out_sound_index != -1)
    {
        sound_datum *sound = looping_sound_get_sound(fade_out_sound_index);
        sound->fade_gain_db = sound_calculate_fade_db(fade_out_sound_index);
        sound->fade_type = (signed char)mode;
        sound->fade_end_time = -1;
        sound->fade_start_time = (long)(seconds * 1000.0f);
        sound->flags.set(7, true);
    }
}

static __forceinline bool looping_sound_refresh_test_flag(unsigned long flags, long bit) { return (flags & (1UL << bit)) != 0; }
static __forceinline bool looping_sound_refresh_alternate(const looping_sound_track *track)
{
    return ((1L << track->test_kind) & 0x58) != 0;
}
static __forceinline const s_tag_reference *looping_sound_refresh_reference(const looping_sound_track_definition *definition, long state)
{
    const s_tag_reference *result = 0;
    if (k_looping_sound_reference_offsets.references[state] != 0)
        result = &(definition->*k_looping_sound_reference_offsets.references[state]);
    return result;
}
static __forceinline long looping_sound_refresh_find_or_new(long index, long definition, long range, short track, unsigned long *seed)
{
    long sound = looping_sound_find_sound(index, definition, range, track);
    if (sound == -1)
        sound = looping_sound_new_sound(index, definition, range, track, 2, seed);
    return sound;
}
static __forceinline void looping_sound_refresh_set_flag(looping_sound_flags8 *flags, long bit, bool enabled)
{
    unsigned char mask = (unsigned char)(1UL << bit);
    if (enabled) flags->value |= mask;
    else flags->value &= ~mask;
}

static __forceinline s_sound_cache_file_gestalt_definition *looping_sound_refresh_get_gestalt()
{
    return (s_sound_cache_file_gestalt_definition *)g_sound_cache_file_gestalt.data;
}
static __forceinline float looping_sound_refresh_map_pitch(const s_cache_file_sound_definition *cache, float base_pitch, float fraction)
{
 const s_sound_cache_file_gestalt_definition *gestalt = looping_sound_refresh_get_gestalt();
 const s_sound_scale_modifiers *scale = &gestalt->scale_modifiers[cache->scale_index];
 float minimum = (float)scale->pitch.lower;
 float maximum = (float)scale->pitch.upper;
 return ((maximum - minimum) * fraction + base_pitch) + minimum;
}
static __forceinline float looping_sound_refresh_source_maximum_distance(const s_sound_source *source, long definition_index)
{
    
    return looping_sound_source_test(source, 3)
        ? source->maximum_distance
        : ((looping_sound_definition *)tag_get(0, definition_index))->maximum_distance;
}
static __forceinline unsigned long *looping_sound_refresh_get_random_seed(const looping_sound_definition *definition, looping_sound_track *track)
{
    unsigned long *seed = get_local_random_seed_address();
    if (TEST_BIT(definition->flags, 5) || TEST_BIT(definition->flags, 4))
        seed = &track->random_seed;
    return seed;
}
// @address 0x1289e8 ?sound_refresh_looping@@YA_NJKJJJPBUs_sound_source@@W4e_looping_sound_refresh_state@@KM@Z
bool sound_refresh_looping(long definition_index, unsigned long playback_flags, long identifier, long controller, long owner,
    const s_sound_source *in_source, e_looping_sound_refresh_state refresh_state, unsigned long flags, float fade_seconds)
{
    bool result = refresh_state == _looping_sound_refresh_stop;
    if (definition_index != -1)
    {
        bool alternate_requested = (flags & 1) != 0;
        if (looping_sound_manager_is_active())
        {
            looping_sound_definition *definition = (looping_sound_definition *)tag_get(0, definition_index);
            bool global = (in_source->spatialization & 0xf0) != 0 && (definition->flags & 128) != 0;
            long sound_identifier = global ? looping_sound_create_identifier(definition_index, 3) : identifier;
            bool accumulate_gains = global;
            long index = looping_sound_find(definition_index, sound_identifier);
            bool created = false;
            result = true;
            if (index != -1)
            {
                if ((flags & 16) && refresh_state == _looping_sound_refresh_start)
                    refresh_state = _looping_sound_refresh_update;
                if (looping_sound_get_loop_unchecked(index)->state == 2 && refresh_state != _looping_sound_refresh_stop)
                {
                    looping_sound_delete(index);
                    index = -1;
                }
            }
            if (index == -1 && ((1L << refresh_state) & 3) && !looping_sound_source_test(in_source, 1))
            {
                index = looping_sound_new(definition_index, playback_flags, sound_identifier, controller, owner, in_source);
                created = true;
            }
            if (index != -1)
            {
                looping_sound_datum *loop = looping_sound_get_loop_unchecked(index);
                loop->update_flags = (unsigned short)playback_flags;
                if (!looping_sound_source_test(in_source, 1))
                    *(s_sound_source *)&loop->flags = *in_source;
                if (((refresh_state == _looping_sound_refresh_stop && loop->state == 2) || loop->state_flags.test(3)) && loop->components == 0)
                {
                    looping_sound_delete(index);
                }
                else
                {
                    result = false;
                    for (long track_index = 0; track_index < definition->tracks.count; track_index++)
                    {
                            looping_sound_track_definition *track_definition = &(
                                (looping_sound_track_definition *)definition->tracks.address)[track_index];
                            looping_sound_track *track = &loop->tracks[track_index];
                            unsigned long *seed = looping_sound_refresh_get_random_seed(definition, track);
                            bool alternate = alternate_requested;
                            if (track_definition->alternate_start.index == -1 && track_definition->alternate_loop.index == -1 &&
                                track_definition->alternate_end.index == -1 && track_definition->alternate_transition.index == -1 &&
                                !TEST_BIT(track_definition->flags, 2) && !TEST_BIT(track_definition->flags, 4))
                                alternate = false;
                            if (created)
                            {
                                track->pitch = loop->volume;
                                track->pending_count = 0;
                            }
                            if (created || track->sound == -1)
                            {
                                track->sound = -1;
                                track->test_kind = alternate ? _looping_sound_playback_alternate_start : _looping_sound_playback_start;
                                track->check_kind = track->test_kind;
                            }
                            if (refresh_state == _looping_sound_refresh_start && track_definition->start.index != -1)
                            {
                                long previous = looping_sound_find_sound(index, track_definition->end.index, -1, (short)track_index);
                                if (!(track_definition->flags & 1))
                                    track->sound = looping_sound_new_sound(index, track_definition->start.index, -1, (short)track_index, 1, seed);
                                else
                                {
                                    looping_sound_new_sound(index, track_definition->start.index, -1, (short)track_index, 0, seed);
                                    if (previous != -1)
                                        looping_sound_refresh_start_fade(1, (sound_event_fade_reason)1, track_definition->fade_in_seconds, -1, previous);
                                    loop_track_fade_in(1, (sound_event_fade_reason)1, track_definition->fade_in_seconds, track->sound);
                                }
                                track->test_kind = _looping_sound_playback_start;
                                track->check_kind = track->test_kind;
                            }
                            if (refresh_state == _looping_sound_refresh_stop || loop->state_flags.test(3))
                            {
                                if (loop->state != 2)
                                {
                                    if (fade_seconds != 0.0f)
                                    {
                                        if (track->sound != -1)
                                        {
                                            track->fading = true;
                                            loop_track_fade_out((sound_event_fade_reason)5, fade_seconds, track->sound);
                                        }
                                    }
                                    else
                                    {
                                        bool alternate_end = looping_sound_refresh_alternate(track) &&
                                            ((track_definition->flags & 16) || track_definition->alternate_end.index != -1);
                                        bool impulse = alternate_end ? TEST_BIT(track_definition->flags, 4) : TEST_BIT(track_definition->flags, 1);
                                        long end_sound = alternate_end ? track_definition->alternate_end.index : track_definition->end.index;
                                        if (track->sound != -1 && (impulse || (end_sound == -1 && !(definition->flags & 2))))
                                        {
                                            float fade = looping_sound_refresh_alternate(track) && (track_definition->flags & 16)
                                                ? track_definition->alternate_fade_out_seconds
                                                : track_definition->fade_out_seconds;
                                            loop_track_fade_out((sound_event_fade_reason)6, fade, track->sound);
                                        }
                                        if (end_sound != -1)
                                        {
                                            if (impulse)
                                                looping_sound_new_sound(index, end_sound, -1, (short)track_index, 0, seed);
                                            else if (track->sound == -1)
                                            {
                                                track->sound = looping_sound_new_sound(index, end_sound, -1, (short)track_index, 3, seed);
                                                track->test_kind = track->check_kind = (e_looping_sound_playback_state)((alternate_end ? 4 : 0) + 2);
                                            }
                                            else
                                            {
                                                sound_datum *sound = looping_sound_get_sound(track->sound);
                                                if (sound->channel_index != -1)
                                                {
                                                    track->check_kind = (e_looping_sound_playback_state)((alternate_end ? 4 : 0) + 2);
                                                    sound->type = 3;
                                                }
                                            }
                                        }
                                    }
                                }
                                else
                                {
                                    if (fade_seconds != 0.0f)
                                    {
                                        c_track_sound_iterator iterator(track->sound);
                                        while (iterator.next())
                                        {
                                            if (iterator.get_index() != -1 && !track->fading && track->sound != -1)
                                            {
                                                track->fading = true;
                                                loop_track_fade_out((sound_event_fade_reason)5, fade_seconds, track->sound);
                                            }
                                        }
                                    }
                                }
                            }
                            else
                            {
                                long next_sound = -1;
                                long impulse = -1;
                                c_flags<long, unsigned long, 7> impulse_states;
                                impulse_states.set(0, (track_definition->flags & 1) != 0);
                                impulse_states.set(2, (track_definition->flags & 2) != 0);
                                impulse_states.set(6, (track_definition->flags & 2) != 0);
                                impulse_states.set(3, (track_definition->flags & 4) != 0);
                                impulse_states.set(5, (track_definition->flags & 4) != 0);
                                if (track->test_kind == track->check_kind || alternate != looping_sound_refresh_alternate(track))
                                {
                                    long next;
                                    long current = track->test_kind;
                                    next = alternate ? k_looping_sound_alternate_transitions[current] : k_looping_sound_normal_transitions[current];
                                    while (next != current)
                                    {
                                        const s_tag_reference *reference = looping_sound_refresh_reference(track_definition, next);
                                        if (reference && reference->index != -1)
                                        {
                                            if (!impulse_states.test(next))
                                            {
                                                next_sound = reference->index;
                                                break;
                                            }
                                            impulse = reference->index;
                                        }
                                        current = next;
                                        next = alternate ? k_looping_sound_alternate_transitions[next] : k_looping_sound_normal_transitions[next];
                                    }
                                    track->check_kind = (e_looping_sound_playback_state)next;
                                }
                                if (impulse != -1)
                                    looping_sound_new_sound(index, impulse, -1, (short)track_index, 0, seed);
                                if (next_sound != -1 || alternate != looping_sound_refresh_alternate(track))
                                {
                                    bool replace = refresh_state == _looping_sound_refresh_start && (track_definition->flags & 1);
                                    if (track->sound == -1 || replace)
                                    {
                                        if (next_sound != -1)
                                        {
                                            s_cache_file_sound_definition *sound_definition =
                                                (s_cache_file_sound_definition *)tag_get(0, next_sound);
                                            s_cache_file_sound_definition *cache = (s_cache_file_sound_definition *)sound_definition;
                                            s_sound_cache_file_gestalt_definition *gestalt = looping_sound_refresh_get_gestalt();
                                            s_sound_playback_parameters *playback = &gestalt->playback_parameters[cache->playback_index];
                                            float pitch = looping_sound_refresh_map_pitch(cache, (float)playback->pitch_lower, track->pitch);
                                            short range = sound_definition_find_pitch_range_by_pitch(sound_definition, pitch, -1);
                                            long sound = looping_sound_refresh_find_or_new(index, next_sound, range, (short)track_index, seed);
                                            if (sound != -1)
                                            {
                                                if (refresh_state == _looping_sound_refresh_start)
                                                {
                                                    if (track_definition->flags & 1)
                                                        sound_start_fade(1, (sound_event_fade_reason)1, track_definition->fade_in_seconds, sound, -1);
                                                }
                                                else
                                                    sound_start_fade(1, (sound_event_fade_reason)2, k_looping_sound_default_fade_seconds, sound, -1);
                                                track->sound = sound;
                                                track->test_kind = track->check_kind;
                                            }
                                        }
                                    }
                                    else if (looping_sound_get_sound(track->sound)->definition_index != next_sound &&
                                        alternate != looping_sound_refresh_alternate(track) && (track_definition->flags & 4))
                                    {
                                        long sound = next_sound != -1 ? looping_sound_refresh_find_or_new(index, next_sound, -1, (short)track_index, seed) : -1;
                                        if (sound != -1 || next_sound == -1)
                                        {
                                            sound_start_fade(1, (sound_event_fade_reason)3, track_definition->crossfade_seconds, sound, track->sound);
                                            track->sound = sound;
                                            track->test_kind = (e_looping_sound_playback_state)((alternate ? 3 : 0) + 1);
                                            track->check_kind = track->test_kind;
                                        }
                                    }
                                }
                            }
                            if (accumulate_gains)
                            {
                                if (loop->flip_flop != ((s_sound_manager_globals *)g_sound_manager_globals)->flip_flop)
                                    for (long i = 0; i < 4; i++)
                                        track->gains[i] = 0.0f;
                                long sound_index = (track_definition->*k_looping_sound_reference_offsets.references[track->test_kind]).index;
                                if (sound_index != -1)
                                {
                                    s_sound_source *source = (s_sound_source *)&loop->flags;
                                    float maximum = looping_sound_source_test(source,
                                        3) ? source->maximum_distance : sound_definition_get_maximum_distance(sound_index);
                                    struct
                                    {
                                        float attenuation, distance;
                                    } listener_data;
                                    long listener = source_audible_internal(source, 0.0f, maximum, &listener_data.distance);
                                    if (listener != -1)
                                    {
                                        union
                                        {
                                            real_point3d point;
                                            real_vector3d vector;
                                        } position;
                                        sound_calculate_listener_relative_position_and_attenuation(
                                            listener, source, sound_index, &listener_data.attenuation, &listener_data.distance, &position.point);
                                        normalize3d(&position.vector);
                                        c_static_array<float, 5> speaker_gains;
                                        calculate_positional_speaker_gain_internal(&position.vector, &speaker_gains, true, 4);
                                        c_static_array<float, 4> gains;
                                        gains[0] = speaker_gains[0];
                                        gains[1] = speaker_gains[1];
                                        gains[2] = speaker_gains[2];
                                        gains[3] = speaker_gains[3];
                                        float scale = 1.0f;
                                        float magnitude = 0.0f;
                                        for (long i = 0; i < 4; i++)
                                        {
                                            float gain = gains[i];
                                            gain *= listener_data.attenuation;
                                            magnitude += gain * gain;
                                        }
                                        if (magnitude > 1.0f)
                                            scale = 1.0f / magnitude;
                                        for (long i = 0; i < 4; i++)
                                            track->gains[i] += (gains[i] * scale) * listener_data.attenuation;
                                    }
                                }
                                loop->update_flags &= ~2;
                            }
                        }
                    if (loop->components == 0 &&
                        source_audible_internal((const s_sound_source *)&loop->flags, 0.0f,
                            looping_sound_refresh_source_maximum_distance((const s_sound_source *)&loop->flags, loop->definition_index),
                            0) == -1)
                        looping_sound_delete(index);
                    else
                    {
                        looping_sound_refresh_set_flag(&loop->state_flags, 0, alternate_requested);
                        looping_sound_refresh_set_flag(&loop->state_flags, 1, (flags & 2) != 0);
                        looping_sound_refresh_set_flag(&loop->state_flags, 2, (flags & 4) != 0);
                        looping_sound_refresh_set_flag(&loop->state_flags, 5, (flags & 16) != 0);
                        loop->state = (unsigned char)refresh_state;
                        loop->flip_flop = ((s_sound_manager_globals *)g_sound_manager_globals)->flip_flop;
                    }
                }
            }
            else if (created)
                result = false;
        }
    }
    else
        result = true;
    return result;
}

// The detail callback table shares the identity quaternion anchor.
static __forceinline const void *detail_sound_tracking_callbacks()
{
    return &k_looping_sound_detail_callbacks;
}
static __forceinline void detail_sound_initialize_orientation(s_sound_source *source)
{
    const real_vector3d *velocity = global_zero_vector3d;
    source->orientation = *global_identity_quaternion;
    source->velocity = *velocity;
}

static __forceinline void detail_sound_copy_tracking(loop_impulse_sound_tracking_data *, const loop_impulse_sound_tracking_data *);
// match: 0x1f9748: __restrict on destination expands the 16-byte copy as grouped loads then stores; without it c2 interleaves each load with its store.
static __forceinline void detail_sound_copy_tracking(loop_impulse_sound_tracking_data *__restrict destination, const loop_impulse_sound_tracking_data *source)
{
    memcpy(destination, source, sizeof(*destination));
}
static __forceinline short detail_sound_random_range(unsigned long *seed, short lower, short upper)
{
    short range = upper - lower;
    random_seed_next(seed);
    unsigned long random = *seed >> 16;
    return lower + (short)((random * range) >> 16);
}
static __forceinline real_point3d *detail_sound_point_from_offset(const real_point3d *origin, float distance, const real_point3d *offset, real_point3d *result)
{
    result->x = origin->x + distance * offset->x;
    result->y = origin->y + distance * offset->y;
    result->z = origin->z + distance * offset->z;
    return result;
}
static __forceinline void detail_sound_add_gain(real_decibel *destination, const real_decibel &amount)
{
    destination->value = amount.value + destination->value;
}
bool detail_sound_build_impulse_definition(looping_sound_detail *detail, long loop_index, s_sound_impulse_definition *impulse)
{
    bool result = false;
    s_sound_source *source = &impulse->source;
    looping_sound_datum *loop = datum_get<looping_sound_datum>(g_looping_sound_data, loop_index);
    const looping_sound_tag *tag = (const looping_sound_tag *)tag_get(0, detail->sound.index);
    memcpy(source, &loop->flags, sizeof(s_sound_source));
    source->volume_type = 0;
    detail_sound_add_gain(&source->gain, detail->gain);
    signed char encoding = ((s_sound_codec *)g_sound_cache_file_gestalt.data->codecs.address)[tag->kind_index].encoding;
    impulse->datum_flags = 16;
    impulse->playback_flags = encoding == 0 ? 2 : 0;
    if ((loop->location & 0xf0) == 0x10) {
        result = true;
        source->current_spatialization = 1;
        loop_impulse_sound_tracking_data tracking;
        detail_sound_random_offset(detail, &tracking.offset_vector);
        tracking.flags = detail->flags;
        track_loop_detail_impulse_sound(loop_index, detail->sound.index, &tracking, source);
        impulse->object_index = loop_index;
        impulse->flags = 47;
        impulse->callbacks = detail_sound_tracking_callbacks();
        detail_sound_copy_tracking((loop_impulse_sound_tracking_data *)impulse->event_parameters, &tracking);
        impulse->event_size = 16;
    } else {
        long count = player_mapping_globals->local_player_count;
        if (count > 0) {
        long selected = detail_sound_random_range(get_local_random_seed_address(), 0, count);
        long user = -1;
        for (long i = 0; i < 4; ++i) if (player_mapping_globals->player_indices[i] != -1) { user = i; break; }
        for (; selected > 0; --selected) user = players_next_active_user(user);
        s_sound_listener *listener = &((s_sound_manager_globals *)g_sound_manager_globals)->listeners[user];
        if (listener->valid) {
            result = true;
            source->current_spatialization = 1;
            loop_impulse_sound_tracking_data tracking;
            detail_sound_random_offset(detail, &tracking.offset_vector);
            detail_sound_point_from_offset(&listener->matrix.position, 1.0f, &tracking.offset, &source->position);
            tracking.offset = source->position;
            tracking.flags = detail->flags;
            track_loop_detail_impulse_sound(loop_index, detail->sound.index, &tracking, source);
            source->location = listener->location;
            detail_sound_initialize_orientation(source);
            impulse->flags = 0x82f;
            impulse->object_index = loop_index;
            impulse->callbacks = detail_sound_tracking_callbacks();
            detail_sound_copy_tracking((loop_impulse_sound_tracking_data *)impulse->event_parameters, &tracking);
        impulse->event_size = 16;
            }
    }
    }
    source->source_flags.set(7, true);
    source->initial_spatialization = source->current_spatialization;
    return result;
}

static __forceinline unsigned char looping_sound_manager_get_flip_flop()
{
    return ((s_sound_manager_globals *)g_sound_manager_globals)->flip_flop;
}
enum e_sound_impulse_new_failure_reason;
static __forceinline long sound_definition_get_pitch_range_count(const s_cache_file_sound_definition *definition)
{
    return definition->pitch_range_count;
}
static __forceinline float looping_sound_process_map_pitch(const s_cache_file_sound_definition *definition, float pitch, float scale)
{
    const s_sound_scale_modifiers *range = &g_sound_cache_file_gestalt.data->scale_modifiers[definition->scale_index];
    float lower = (float)range->pitch.lower;
    float upper = (float)range->pitch.upper;
    return ((upper - lower) * scale + lower) + pitch;
}


// @rdata 0x82022b14
static const float k_looping_sound_process_crossfade_seconds = 0.75f;
// @rdata 0x8202490c
static const float k_looping_sound_process_pitch_epsilon = 0.0001f;
static __forceinline void process_looping_sounds_interpolate_scalar(float *value, float target, float rate)
{
    float current = *value;
    float difference = target - current;
    *value = current + PIN(difference, -rate, rate);
}

static __forceinline void looping_sound_process_stop_detached(unsigned long sound_index, e_sound_event_stop_reason reason)
{
    sound_datum *sound = datum_get<sound_datum>(g_sound_data, sound_index);
    if (sound->channel_index != -1)
    {
        channel_stop(sound->channel_index);
        sound->channel_index = -1;
    }
    sound_datum_unlock(sound);
    sound_stop(sound_index, reason);
}
static __forceinline sound_datum *looping_sound_process_datum_get(const s_data_array *array, long index)
{
    unsigned short absolute_index = (unsigned short)index;
    return &((sound_datum *)array->data)[absolute_index];
}
static __forceinline float looping_sound_process_interpolate_random(float lower, float upper, float fraction)
{
    float range = upper - lower;
    return lower + range * fraction;
}
static __forceinline float looping_sound_process_random_range(unsigned long *seed, float lower, float upper)
{
    random_seed_next(seed);
    float fraction = (float)(*seed >> 16);
    fraction *= 1.5259022e-05f;
    return looping_sound_process_interpolate_random(lower, upper, fraction);
}
// @address 0x13a0e0 ?process_looping_sounds@@YAX_N@Z
void process_looping_sounds(bool p0)
{
    long index;
    for (index = data_next_index((s_data_array *)g_looping_sound_data, -1); index != -1;
        index = looping_sound_data_next_index((s_data_array *)g_looping_sound_data, index))
    {
        looping_sound_datum *loop = looping_sound_get_loop_unchecked(index);
        if (p0 && loop->flip_flop != looping_sound_manager_get_flip_flop())
        {
            if (!loop->wait_until_components_finish() || loop->components == 0)
            {
                looping_sound_delete(index);
                continue;
            }
        }
        looping_sound_definition *definition = (looping_sound_definition *)tag_get(0, loop->definition_index);
        for (long i = 0; i < definition->tracks.count; i++)
        {
            looping_sound_track_definition *track_definition = (looping_sound_track_definition *)tag_block_get_element_with_size(&definition->tracks, i,
                sizeof(looping_sound_track_definition));
            looping_sound_track *track = &loop->tracks[i];
            track->pending_count = 0;
            if (loop->state == 2)
                continue;
            if (track->sound != -1)
            {
                sound_datum *previous = looping_sound_get_sound(track->sound);
                s_cache_file_sound_definition *previous_definition = (s_cache_file_sound_definition *)tag_get(0, previous->definition_index);
                const s_sound_playback_parameters *playback = sound_definition_get_playback_parameters(previous_definition);
                if ((fabs(playback->skip_fraction) < k_looping_sound_process_pitch_epsilon) == false)
                    process_looping_sounds_interpolate_scalar(&track->pitch, loop->volume,
                        playback->skip_fraction *((s_sound_manager_globals *)g_sound_manager_globals)->reverb_rate);
                else
                    track->pitch = loop->volume;
            }
            if (!p0)
                continue;
            if (track->sound == -1)
                continue;
            sound_datum *sound = looping_sound_get_sound(track->sound);
            long sound_definition_index = sound->definition_index;
            s_cache_file_sound_definition *sound_definition = (s_cache_file_sound_definition *)tag_get(0, sound_definition_index);
            float pitch = looping_sound_process_map_pitch(sound_definition, sound->pitch, track->pitch);
            c_sound_playback_controller *controller = looping_sound_get_playback_controller(loop->controller);
            if (controller->owner_identifier == -1)
                random_seed_next(&controller->random_seed);
            unsigned long random_seed = controller->random_seed;
            signed char type = sound->pitch_range_index;
            short range = sound_definition_find_pitch_range_by_pitch(sound_definition, pitch, type);
            if ((sound_definition->flags & 0x800) != 0)
            {
                unsigned long available = 0;
                process_static_array<real_decibel, 12> gain_map;
                process_static_array<long, 12> range_sounds;
                gain_map.set_all(k_looping_sound_minimum_gain_db.gain_db);
                range_sounds.set_all(-1);
                c_track_sound_iterator iterator(track->sound);
                while (iterator.next())
                {
                    long sound_index = iterator.get_index();
                    sound_datum *range_sound = looping_sound_get_sound(sound_index);
                    if (range_sound->definition_index != sound_definition_index)
                        continue;
                    if (range_sound->type != 2)
                        continue;
                    range_sounds[range_sound->pitch_range_index] = sound_index;
                }
                long bend_range_count = sound_definition_get_pitch_range_count(sound_definition);
                for (long r = 0; r < bend_range_count; r++)
                {
                    const s_cache_file_sound_pitch_range *pitch_range = looping_sound_definition_get_pitch_range(sound_definition, r);
                    const s_sound_pitch_bounds *bend_bounds = sound_pitch_range_get_bend_bounds(pitch_range);
                    gain_map[r] = sound_pitch_range_map_gain_db(sound_definition, pitch_range, pitch);
                    if (pitch >= (float)bend_bounds->lower && pitch <= (float)bend_bounds->upper)
                        available |= 1 << r;
                }
                long gain_range_count = sound_definition_get_pitch_range_count(sound_definition);
                for (long r = 0; r < gain_range_count; r++)
                {
                    if ((available & (1 << r)) == 0)
                    {
                        if (range_sounds[r] != -1)
                        {
                            looping_sound_process_stop_detached(range_sounds[r], _sound_event_stop_reason_killed_by_loop);
                            range_sounds[r] = -1;
                        }
                    }
                    if ((available & (1 << r)) != 0 && range_sounds[r] == -1)
                    {
                        long new_sound = looping_sound_new_sound(index, sound_definition_index, r, (short)i, 2, &random_seed);
                        if (new_sound != -1)
                        {
                            if (range == r)
                                track->sound = new_sound;
                            range_sounds[r] = new_sound;
                            looping_sound_get_sound(new_sound)->gain = k_looping_sound_minimum_gain_db.gain_db;
                        }
                    }
                    if (range_sounds[r] != -1)
                    {
                        sound_datum *range_sound = looping_sound_get_sound(range_sounds[r]);
                        range_sound->gain = gain_map[r] + track_definition->gain;
                        track_context *range_track = (track_context *)range_sound->event_parameters;
                        range_track->pad7 = -1;
                    }
                }
            }
            else
            {
                if (range == type)
                    continue;
                long found = looping_sound_find_sound(sound->object_index, sound->definition_index, range, (short)i);
                if (found == -1)
                    found = looping_sound_new_sound(sound->object_index, sound->definition_index, range, (short)i, 2, &random_seed);
                if (found == -1)
                    continue;
                sound_datum *new_sound = looping_sound_get_sound(found);
                new_sound->pitch = sound->pitch;
                sound_start_fade(1, (sound_event_fade_reason)10, k_looping_sound_process_crossfade_seconds, found, track->sound);
                track->sound = found;
            }
        }
        if (loop->state == 2)
            continue;
        if (!loop->can_play_details())
            continue;
        for (long d = 0; d < definition->details.count; d++)
        {
            long detail_time = loop->detail_times[d];
            looping_sound_detail *detail = (looping_sound_detail *)tag_block_get_element_with_size(&definition->details, d, sizeof(looping_sound_detail));
            if (detail->sound.index == -1)
                continue;
            if (detail_time >= ((s_sound_manager_globals *)g_sound_manager_globals)->time)
                continue;
            if ((detail->flags & 1) != 0 && loop->alternate())
                continue;
            if ((detail->flags & 2) != 0 && !loop->alternate())
                continue;
            s_sound_impulse_definition impulse;
            impulse.source.flags = 0;
            if (!detail_sound_build_impulse_definition(detail, index, &impulse))
                continue;
            e_sound_impulse_new_failure_reason failure;
            if (sound_impulse_new(detail->sound.index, &impulse, &failure) == -1 && failure != 1)
                continue;
            float lower = detail->delay_lower;
            float upper = detail->delay_upper;
            unsigned long *seed = get_local_random_seed_address();
            random_seed_next(seed);
            float fraction = (float)(*seed >> 16) * 1.5259022e-05f;
            float random = lower + (upper - lower) * fraction;
            loop->detail_times[d] = (long)(random * 1000.0f + (float)((s_sound_manager_globals *)g_sound_manager_globals)->time);
        }
    }
    c_static_flags<128> visited;
    visited.clear();
    for (index = looping_sound_data_next_index((s_data_array *)g_looping_sound_data, -1); index != -1;
        index = looping_sound_data_next_index((s_data_array *)g_looping_sound_data, index))
    {
        long absolute_index = index & 0xffff;
        if (!visited.test(absolute_index))
        {
            long datum_offset = absolute_index * sizeof(looping_sound_datum);
            looping_sound_datum *loop = (looping_sound_datum *)(((s_data_array *)g_looping_sound_data)->data + datum_offset);
            looping_sound_definition *definition = (looping_sound_definition *)tag_get(0, loop->definition_index);
            if ((definition->flags & 0x80) == 0)
            {
                long sound_definition_index = looping_sound_get_active_sound_index(index);
                if (sound_definition_index != -1)
                {
                    float cutoff = get_priority_cutoff_for_looping_sound_definition_index(
                        index, visited, loop->definition_index, sound_definition_index);
                    looping_sound_mark_distant_duplicates(index, sound_definition_index, cutoff);
                }
            }
        }
    }
}

struct s_sound_location;
struct s_platform_sound_source_properties;
class c_xenon_audio_source_manager;

static __forceinline sound_channel_datum *looping_sound_channel_get(short index)
{
    return &g_sound_channels[index];
}
static __forceinline sound_datum *looping_sound_channel_sound_get(long index)
{
    s_data_array *data = g_sound_data;
    long datum_size = data->size;
    // match: 0x1f8bd0: pins datum_size to 192: drops the size load, strength-reduces the multiply, keeps the data load below the index math.
    __assume(datum_size == sizeof(sound_datum));
    return (sound_datum *)(data->data + (unsigned short)index * datum_size);
}
static __forceinline looping_sound_track *looping_sound_channel_track_get(looping_sound_track (*const &tracks)[4], long index)
{
    return &(*tracks)[index];
}
static __forceinline void *looping_sound_channel_loop_try_get(const s_data_array *data, long index)
{
    void *result = 0;
    if (index != -1)
    {
        int absolute_index = index & 0xffff;
        if (absolute_index < data->first_unallocated)
        {
            short *datum = (short *)(data->data + data->size * absolute_index);
            int identifier = *datum;
            if (identifier != 0 && identifier == (index >> 16))
                result = datum;
        }
    }
    return result;
}
static __forceinline s_cache_file_sound_definition *looping_sound_channel_definition_get(long index)
{
    return (s_cache_file_sound_definition *)tag_get(0, index);
}
static __forceinline looping_sound_definition *looping_sound_channel_loop_definition_get(const looping_sound_datum *loop)
{
    return (looping_sound_definition *)tag_get(0, loop->definition_index);
}
static __forceinline looping_sound_track_definition *looping_sound_channel_track_definitions_get(const looping_sound_definition *definition)
{
    return (looping_sound_track_definition *)((const looping_sound_definition *)definition)->tracks.address;
}
static __forceinline s_cache_file_sound_pitch_range *looping_sound_channel_pitch_range_get(const s_cache_file_sound_definition *definition, long index)
{
    return (s_cache_file_sound_pitch_range *)tag_block_get_element_with_size((const s_tag_block *)&g_sound_cache_file_gestalt.data->pitch_ranges,
        definition->pitch_range_index + index, sizeof(s_cache_file_sound_pitch_range));
}
static __forceinline s_cache_file_sound_permutation *looping_sound_channel_permutation_get(const s_cache_file_sound_pitch_range *range, long index)
{
    s_sound_cache_file_gestalt_definition *gestalt = (s_sound_cache_file_gestalt_definition *)g_sound_cache_file_gestalt.data;
    return &gestalt->permutations[range->first_permutation_index + index];
}
static __forceinline s_cache_file_sound_permutation *looping_sound_channel_playing_permutation_get(const sound_datum *sound)
{
    return looping_sound_channel_permutation_get(
        looping_sound_channel_pitch_range_get(looping_sound_channel_definition_get(sound->definition_index), sound->pitch_range_index),
        sound->permutation_index);
}
static __forceinline sound_chunk *looping_sound_channel_chunk_get(const s_cache_file_sound_permutation *permutation, long index)
{
    s_sound_cache_file_gestalt_definition *gestalt = (s_sound_cache_file_gestalt_definition *)g_sound_cache_file_gestalt.data;
    return &gestalt->chunks[permutation->first_chunk_index + index];
}
static __forceinline short looping_sound_channel_next_chunk_get(const s_cache_file_sound_permutation *permutation, short index)
{
    long next = permutation && index < permutation->chunk_count - 1 ? index + 1 : -1;
    return (short)next;
}
static __forceinline real_decibel looping_sound_channel_permutation_gain_get(const s_cache_file_sound_permutation *permutation)
{
    real_decibel gain = { (float)permutation->gain };
    return gain;
}
static __forceinline const s_sound_pitch_bounds *looping_sound_channel_playback_bend_bounds_get(const s_cache_file_sound_pitch_range *range)
{
    return &looping_sound_pitch_range_get_parameters(range)->maximum_gain_pitch_bounds;
}
static __forceinline long looping_sound_channel_natural_pitch_get(const s_cache_file_sound_pitch_range *range)
{
    return looping_sound_pitch_range_get_parameters(range)->natural_pitch;
}
static __forceinline float looping_sound_channel_relative_pitch_get(const s_cache_file_sound_pitch_range *range, float pitch)
{
    const s_sound_pitch_bounds *bounds = looping_sound_channel_playback_bend_bounds_get(range);
    long natural_pitch = looping_sound_channel_natural_pitch_get(range);
    float bounded = PIN(pitch, (float)bounds->lower, (float)bounds->upper);
    return bounded - (float)natural_pitch;
}
static __forceinline float looping_sound_channel_scale_pitch(float pitch, float scale, const s_sound_scale_modifiers *range)
{
    float lower = (float)range->pitch.lower;
    float upper = (float)range->pitch.upper;
    return ((upper - lower) * scale + pitch) + lower;
}
static __forceinline float looping_sound_channel_map_pitch(const s_cache_file_sound_definition *definition, float pitch, float scale)
{
    const s_sound_scale_modifiers *range = &g_sound_cache_file_gestalt.data->scale_modifiers[definition->scale_index];
    return looping_sound_channel_scale_pitch(pitch, scale, range);
}
static __forceinline c_sound_playback_controller *looping_sound_channel_controller_get(long index)
{
    c_sound_playback_controller *controller = g_default_playback_controller;
    if (index != -1)
        controller = datum_get<c_sound_playback_controller>(((s_data_array *)g_sound_playback_controller_data), index);
    return controller;
}
static __forceinline float looping_sound_channel_doppler_get(const c_sound_playback_controller *controller, long listener)
{
    if (controller->owner_identifier != -1)
        return controller->doppler_shifts[listener];
    return 0.0f;
}
static __forceinline signed char looping_sound_channel_synchronized_ready(c_sound_playback_controller *controller)
{
    signed char marker = -1;
    if (controller->owner_identifier != -1)
    {
        marker = controller->synchronized_sound_count;
        controller->synchronized_sound_count--;
        controller->synchronized_sounds_ready = false;
    }
    return marker;
}
static __forceinline bool looping_sound_channel_state_change_pending(const looping_sound_track *track, const looping_sound_track_definition *definition)
{
    return (long)track->test_kind != (long)track->check_kind;
}
static __forceinline bool looping_sound_channel_can_use_marker(const looping_sound_track *track)
{
    bool result = ((1UL << (long)track->test_kind) & 41) != 0 &&
        (k_looping_sound_alternate_transitions[(long)track->test_kind] == (long)track->check_kind ||
            k_looping_sound_normal_transitions[(long)track->test_kind] == (long)track->check_kind);
    return !result;
}
static __forceinline bool looping_sound_channel_chunk_has_marker(const sound_chunk *chunk)
{
    unsigned char result = (chunk->flags.value >> 28) & 1;
    return result;
}
static __forceinline unsigned long *looping_sound_channel_random_seed_get(
    const looping_sound_definition *definition, looping_sound_track *track, track_context *context, bool main_sound)
{
    unsigned long *seed = get_local_random_seed_address();
    if ((definition->flags & (1 << 5)) || (definition->flags & (1 << 4)))
        seed = main_sound ? &track->random_seed : &context->random_seed;
    return seed;
}
static __forceinline long looping_sound_channel_track_definition_get(const looping_sound_track_definition *track, long state)
{
    const s_tag_reference *reference = &(track->*k_looping_sound_reference_offsets.references[state]);
    return reference->index;
}
static __forceinline bool looping_sound_channel_pending_valid(const looping_sound_track *track, const looping_sound_track_definition *definition)
{
    long next = looping_sound_channel_track_definition_get(definition, track->check_kind);
    return next != -1 && sound_definition_hardware_formats_match(looping_sound_channel_track_definition_get(definition, track->test_kind), next);
}
static __forceinline short looping_sound_channel_platform_state_get(long source_index)
{
    return g_xaudio_source_manager.sources[source_index].state;
}
static __forceinline short looping_sound_channel_state_get(short channel_index)
{
    sound_channel_datum *channel = looping_sound_channel_get(channel_index);
    return looping_sound_channel_platform_state_get(channel->source_index);
}
static __forceinline void looping_sound_channel_platform_update_properties(long source_index,
    const s_platform_sound_source_properties *properties, const s_platform_sound_playback_submit_parameters *submit, bool force)
{
    g_xaudio_mix_master.source_update_properties(source_index, properties, submit, force);
}
static __forceinline void looping_sound_channel_update_properties(short channel_index, const s_sound_channel_properties *properties, bool force)
{
    sound_channel_datum *channel = looping_sound_channel_get(channel_index);
    looping_sound_channel_platform_update_properties(
        channel->source_index, &properties->source_properties, &properties->submit_parameters, force);
}

// @address 0x13d7f0
void update_channel_for_looping_sound(short channel_index, real_decibel decibel, const s_sound_output_parameters *output)
{
    sound_channel_datum *channel = looping_sound_channel_get(channel_index);
    // match: 0x13d7f0: const reference binding preserves temporary lifetime
    const unsigned __int64 &lookup_offset = (unsigned long)(channel_index * sizeof(sound_channel_datum));
    // match: 0x13d7f0: const reference binding preserves temporary lifetime
    const unsigned long &lookup_offset_value = (unsigned long)lookup_offset;
    // match: 0x13d7f0: zero-bit rotate preserves 32-bit GPR value
    long sound_index = ((sound_channel_datum *)((char *)g_sound_channels + _rotl(lookup_offset_value, 0)))->sound_index;
    sound_datum *sound = looping_sound_channel_sound_get(sound_index);
    track_context *context = (track_context *)sound->event_parameters;
    s_cache_file_sound_definition *definition = looping_sound_channel_definition_get(sound->definition_index);
    s_cache_file_sound_pitch_range *pitch_range = looping_sound_channel_pitch_range_get(definition, sound->pitch_range_index);
    looping_sound_datum *loop = (looping_sound_datum *)looping_sound_channel_loop_try_get(g_looping_sound_data, sound->object_index);
    if (loop)
    {
        looping_sound_track *track = looping_sound_channel_track_get(&loop->tracks, context->track_index);
        looping_sound_definition *loop_definition = looping_sound_channel_loop_definition_get(loop);
        const long &track_byte_offset = context->track_index * sizeof(looping_sound_track_definition);
        looping_sound_track_definition *const &track_definitions = looping_sound_channel_track_definitions_get(loop_definition);
        looping_sound_track_definition *track_definition = (looping_sound_track_definition *)((char *)track_definitions + track_byte_offset);
        c_sound_playback_controller *controller = looping_sound_channel_controller_get(loop->controller);
        float scale = sound->source.volume;
        float pitch = looping_sound_channel_map_pitch(definition, sound->pitch, track->pitch);
        real_decibel gain = sound->gain;
        bool main_sound = track->sound == sound_index;
        s_sound_channel_properties properties = {0};
        properties.source_properties.gain_db = sound_definition_map_gain_db(definition, decibel + gain + sound->source.gain, scale);
        properties.source_properties.effect = track_definition->effect;
        if (properties.source_properties.effect)
            properties.source_properties.flags.set(_platform_sound_source_bit_first, true);
        if (loop_definition->flags & (1 << 5))
            properties.source_properties.channel = loop->controller;
        else
            properties.source_properties.channel = -1;
        properties.source_properties.pitch = looping_sound_channel_relative_pitch_get(pitch_range, pitch);
        if (sound->playback_flags & 2)
        {
            const sound_class_definition *sound_class = sound_class_get(definition->sound_class);
            float doppler = loop->controller != -1
                ? looping_sound_channel_doppler_get(looping_sound_channel_controller_get(loop->controller), sound->listener_index)
                : calculate_doppler_shift_cents(sound->listener_index, (const s_sound_location *)&sound->source.position);
            properties.source_properties.pitch += sound_class->doppler * output->doppler * doppler;
        }
        channel_properties_initialize_from_sound(channel_index, (const sound_datum *)sound, output, &properties);
        if (loop->flags.test_bool(8))
            properties.submit_parameters.special = true;
        if (!sound->flags.test(9))
        {
            s_cache_file_sound_permutation *permutation = looping_sound_channel_permutation_get(pitch_range, sound->permutation_index);
            properties.source_properties.gain_db += looping_sound_channel_permutation_gain_get(permutation);
            channel_initialize(channel_index, &properties);
            channel_queue_sound(channel_index, permutation, sound->chunk_index);
            if (loop_definition->flags & (1 << 5))
                context->synchronized = looping_sound_channel_synchronized_ready(controller);
            sound->flags.set(9, true);
        }
        else
        {
            if (!sound->flags.test(6))
            {
                if (sound->type == 2 || sound->type == 3 || sound->type == 1)
                {
                    short state = looping_sound_channel_state_get(sound->channel_index);
                    if (!sound->flags.test(3) && (state != 5 || looping_sound_channel_state_change_pending(track, track_definition)))
                    {
                        unsigned long *seed = looping_sound_channel_random_seed_get(loop_definition, track, context, main_sound);
                        short permutation_index = sound->permutation_index;
                        short chunk_index = looping_sound_channel_next_chunk_get(looping_sound_channel_playing_permutation_get(sound), sound->chunk_index);
                        bool end_of_permutation = chunk_index == -1;
                        bool use_marker = looping_sound_channel_can_use_marker(track);
                        if (!context->component_flags.test(0) && main_sound &&
                            looping_sound_channel_state_change_pending(track, track_definition) &&
                            (end_of_permutation ||
                                (use_marker &&
                                    looping_sound_channel_chunk_has_marker(looping_sound_channel_chunk_get(
                                        looping_sound_channel_playing_permutation_get(sound), sound->chunk_index)))))
                        {
                            if (looping_sound_channel_pending_valid(track, track_definition))
                            {
                                bool marker_transition = use_marker &&
                                    looping_sound_channel_chunk_has_marker(looping_sound_channel_chunk_get(
                                        looping_sound_channel_playing_permutation_get(sound), sound->chunk_index));
                                (void)marker_transition;
                                if (sound_commit_looping_definition(loop, (looping_sound_track_datum *)track, channel->sound_index, seed))
                                    return;
                                pitch_range = looping_sound_channel_pitch_range_get(
                                    looping_sound_channel_definition_get(sound->definition_index), sound->pitch_range_index);
                                sound->flags.set(3, true);
                            }
                            else
                            {
                                sound->flags.set(6, true);
                                channel_signal_complete(channel_index);
                            }
                        }
                        else
                        {
                            if (end_of_permutation)
                            {
                                if (!(loop_definition->flags & 2))
                                {
                                    unsigned long permutation_flags = main_sound
                                        ? looping_sound_track_get_range_permutation_flags(
                                              definition, loop_definition, loop, context->track_index, sound->pitch_range_index, false)
                                        : context->permutation_flags;
                                    permutation_index = sound_definition_next_permutation_deterministic(
                                        definition, sound->pitch_range_index, sound->permutation_index, &permutation_flags, seed, 0, false);
                                    if (main_sound)
                                        looping_sound_track_set_range_permutation_flags(definition, loop_definition, loop,
                                            context->track_index, sound->pitch_range_index, permutation_index, permutation_flags);
                                    else
                                        context->permutation_flags = permutation_flags;
                                    chunk_index = 0;
                                }
                                else
                                {
                                    sound->type = 4;
                                    if (main_sound)
                                        loop->state_flags.value |= 8;
                                    permutation_index = -1;
                                }
                            }
                            if (permutation_index != -1)
                            {
                                sound_set_permutation(channel->sound_index, sound->definition_index, sound->pitch_range_index, permutation_index, chunk_index);
                                sound->flags.set(3, true);
                            }
                            else
                            {
                                sound->flags.set(6, true);
                                channel_signal_complete(channel_index);
                            }
                        }
                    }
                    if (sound->flags.test(3))
                    {
                        s_cache_file_sound_permutation *permutation = looping_sound_channel_permutation_get(pitch_range, sound->permutation_index);
                        if (sound_make_permutation_ready_by_index(channel->sound_index, 0) && state != 5)
                        {
                            channel_queue_sound(channel_index, permutation, sound->chunk_index);
                            sound->flags.set(3, false);
                            if (main_sound && !looping_sound_channel_state_change_pending(track, track_definition) &&
                                looping_sound_channel_next_chunk_get(permutation, sound->chunk_index) == -1)
                            {
                                if (sound->type == 1)
                                    sound->type = 2;
                                else if (sound->type == 3)
                                    sound->type = 4;
                            }
                        }
                    }
                }
                else
                {
                    sound->flags.set(6, true);
                    channel_signal_complete(channel_index);
                }
            }
            s_cache_file_sound_permutation *permutation = looping_sound_channel_permutation_get(pitch_range, sound->permutation_index);
            properties.source_properties.gain_db += looping_sound_channel_permutation_gain_get(permutation);
            looping_sound_channel_update_properties(channel_index, &properties, false);
        }
        if (main_sound)
        {
            context->random_seed = track->random_seed;
            context->permutation_flags = track->range_permutation_flags;
        }
    }
}
