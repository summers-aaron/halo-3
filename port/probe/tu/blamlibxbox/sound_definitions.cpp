// blamlibxbox/sound_definitions.obj: from the records 0xf8b88 0xf8c08 0xf9298 0xfa068 0xfa280 0xfa480 0xfa4f8 0x129aa8 0x1f6488 0x1f6600 0x1f66e0 0x1f6820 0x1f6888 0x1f6900 0x1f6a88
// @flags /TP /O2
#include "tag_files/tag_groups.h"
#include "sound/sound_classes.h"
#include "math/random_math.h"
#include "math/random_math_range.h"
#include "memory/datum_index.h"
// @address 0xfa068 ?sound_definition_find_pitch_range_by_pitch@@YAFPAUs_cache_file_sound_definition@@MF@Z
// @address 0xfa280 ?sound_definition_next_permutation@@YAFPAUs_cache_file_sound_definition@@F@Z
// @address 0x1f6820 ?set_discarded_permutation_index@s_cache_file_sound_pitch_range@@QAAXPAUs_cache_file_sound_definition@@F@Z
#include "game/game_globals.h"
#include "sound/sound_cache_file_definitions.h"
#include "sound/sound_definitions.h"
#include <math.h>
#include "sound/sound_cache_file_gestalt.h"
#include "sound/sound_classes_types.h"
#include "scenario/scenario.h"
#include "sound/sound_globals_reference_types.h"
#include "sound/sound_class_definitions.h"

#include "sound/sound_cache_file_gestalt_globals.h"
#include "sound/sound_definition_inline.h"

// @address 0x39274 ?sound_sample_rate_samples_per_second@@3QBJB
// @rdata 0x82039274..0x82039280
const long sound_sample_rate_samples_per_second[] = { 22050, 44100, 32000 };
// @rdata-end
static __forceinline long entry_value(const s_cache_file_sound_pitch_range *e, unsigned char flag)
{
    if (flag)
        return e->permutation_info >> 26;
    return (e->permutation_info >> 20) & 63;
}

// @address 0xf8c08 ?sound_definition_get_silence_distance@@YAMJ@Z
// @address 0xf9298 ?sound_definition_get_minimum_distance@@YAMJ@Z
static __forceinline s_cache_file_sound_definition *sound_tag_get(unsigned long group, long index)
{
    return (s_cache_file_sound_definition *)global_tag_instances[datum_index_to_absolute_index(index)].definition;
}
// @address 0xf8b88 ?sound_definition_get_maximum_distance@@YAMJ@Z
float sound_definition_get_maximum_distance(long sound_index)
{
    s_cache_file_sound_definition *definition = sound_tag_get('snd!', sound_index);
    const s_sound_playback_parameters *playback = sound_definition_get_playback_parameters(definition);
    float result;
    if (playback->test(3))
        result = sound_class_get(definition->sound_class)->maximum_distance;
    else
        result = sound_definition_get_playback_parameters(definition)->maximum_distance;
    return result;
}

float sound_definition_get_silence_distance(long sound_index)
{
    s_cache_file_sound_definition *definition = sound_tag_get('snd!', sound_index);
    const s_sound_playback_parameters *playback = sound_definition_get_playback_parameters(definition);
    float result;
    if (playback->test(0))
        result = sound_class_get(definition->sound_class)->silence_distance;
    else
        result = sound_definition_get_playback_parameters(definition)->silence_distance;
    return result;
}
float sound_definition_get_minimum_distance(long sound_index)
{
    s_cache_file_sound_definition *definition = sound_tag_get('snd!', sound_index);
    const s_sound_playback_parameters *playback = sound_definition_get_playback_parameters(definition);
    float result;
    if (playback->test(2))
        result = sound_class_get(definition->sound_class)->minimum_distance;
    else
        result = sound_definition_get_playback_parameters(definition)->minimum_distance;
    return result;
}
namespace sound_definition_detail {
static __forceinline const sound_class_definition *sound_class_get(short index)
{
    sound_globals_reference *reference = global_game_globals->sound_globals.count() ? global_game_globals->sound_globals.begin() : 0;
    const sound_classes *classes = (const sound_classes *)global_tag_instances[datum_index_to_absolute_index(reference->sound_classes.index)].definition;
    return &classes->classes[index];
}
}

short sound_definition_find_pitch_range_by_pitch(struct s_cache_file_sound_definition * _sym_p0, float x, short hint)
{
    short result = -1;
    long count = _sym_p0->pitch_range_count;
    unsigned char flag = g_sound_cache_file_gestalt.use_uncompressed_permutation_counts;
    s_sound_cache_file_gestalt_definition *tables = g_sound_cache_file_gestalt.data;
    if (hint != -1 && hint < count) {
        s_cache_file_sound_pitch_range *e = &tables->pitch_ranges[_sym_p0->pitch_range_index + hint];
        s_sound_gestalt_pitch_parameters *r = &tables->pitch_parameters[e->parameter_index];
        if (!(x < (float)r->runtime_bend_bounds.lower) && !(x > (float)r->runtime_bend_bounds.upper) && entry_value(e, flag) > 0)
            result = hint;
    }
    if (result == -1) {
        float best = 3.4028234e+38f;
        short i = 0;
        if (count > 0) {

            short base = _sym_p0->pitch_range_index;
            do {
                s_cache_file_sound_pitch_range *e = &tables->pitch_ranges[i + base];
                if (entry_value(e, flag) > 0) {
                    s_sound_gestalt_pitch_parameters *r = &tables->pitch_parameters[e->parameter_index];
                    if (!(x < (float)r->runtime_bend_bounds.lower) && !(x > (float)r->runtime_bend_bounds.upper))
                    { result = i; break; }
                    float d;
                    float lo = (float)r->bend_bounds.upper;
                    if (lo < x)
                        d = x - lo;
                    else
                        d = (float)r->bend_bounds.lower - x;
                    if (d < best) { result = i; best = d; }
                }
                i++;

            } while (i < count);
        }
    }
    return result;
}

short sound_definition_next_permutation(struct s_cache_file_sound_definition * definition, short index)
{
    s_cache_file_sound_pitch_range *range =
        &g_sound_cache_file_gestalt.data->pitch_ranges[definition->pitch_range_index + index];
    if ((short)range->get_discarded_permutation_index(definition) != -1)
    {
        short permutation = range->get_discarded_permutation_index(definition);
        range->set_discarded_permutation_index(definition, -1);
        range->set_last_permutation_index(definition, permutation);
        return permutation;
    }
    int flags = ::sound_pitch_range_get_runtime_permutation_flags(
        definition, range);
    short permutation = ::sound_definition_next_permutation_deterministic(definition, index,
        range->get_last_permutation_index(definition), (unsigned long *)(&flags), (unsigned long *)(get_local_random_seed_address()), (bool *)(0), 0);
    ::sound_pitch_range_set_runtime_permutation_flags(
        definition, range, flags);
    range->set_last_permutation_index(definition, permutation);
    return permutation;
}

// @address 0xfa480 ?get_last_permutation_index@s_cache_file_sound_pitch_range@@QBAFPAUs_cache_file_sound_definition@@@Z
short s_cache_file_sound_pitch_range::get_last_permutation_index(s_cache_file_sound_definition *owning_sound) const
{
    bool deterministic = sound_definition_detail::sound_class_get(owning_sound->sound_class)->is_deterministic();
    long result;
    if (!deterministic)
        result = last_permutation_index;
    else
        result = -1;
    return (short)result;
}

static __forceinline s_cache_file_sound_pitch_range *sound_definitions_get_pitch_range(s_cache_file_sound_definition *definition, long index)
{
    return &g_sound_cache_file_gestalt.data->pitch_ranges[definition->pitch_range_index + index];
}
static __forceinline long sound_definitions_get_permutation_count(const s_cache_file_sound_pitch_range *range)
{
    return g_sound_cache_file_gestalt.use_uncompressed_permutation_counts ? range->uncompressed_permutation_count : range->permutation_count;
}
static __forceinline s_cache_file_sound_permutation *sound_definitions_get_permutation(s_cache_file_sound_pitch_range *range, long index)
{
    return &g_sound_cache_file_gestalt.data->permutations[range->first_permutation_index + index];
}

// @address 0x129aa8 ?set_last_permutation_index@s_cache_file_sound_pitch_range@@QAAXPAUs_cache_file_sound_definition@@F@Z
void s_cache_file_sound_pitch_range::set_last_permutation_index(s_cache_file_sound_definition *sound_definition, short new_permutation_index)

{
  unsigned char flag;

  // match: preserve the original volatile reads and their memory access order.
  flag = ((const sound_classes *)global_tag_instances[
      // match: 0x129aa8: volatile tag-reference read preserves access order.
      DATUM_INDEX_TO_ABSOLUTE_INDEX(((const volatile sound_globals_reference *)(
          global_game_globals->sound_globals.count() ? global_game_globals->sound_globals.begin() : 0))->sound_classes.index)].definition)
      ->classes[sound_definition->sound_class].flags >> 5 & 1;
  if (flag != 0) {
    return;
  }
  last_permutation_index = new_permutation_index;
  return;
}

static __forceinline s_sound_encoded_dialogue_section *sound_definitions_get_encoded_dialogue(long sound_index)
{
    s_cache_file_sound_definition *definition = sound_tag_get('snd!', sound_index);
    return TAG_BLOCK_GET_ELEMENT(
        &g_sound_cache_file_gestalt.data->extra_infos[definition->extra_info_index].encoded_dialogue, 0,
        s_sound_encoded_dialogue_section);
}
static __forceinline const s_sound_permutation_dialogue_info *sound_definitions_get_dialogue_info(long sound_index, long index)
{
    s_sound_encoded_dialogue_section *section = sound_definitions_get_encoded_dialogue(sound_index);
    return &section->dialogue_info[index];
}
static __forceinline const unsigned char *sound_definitions_get_mouth_aperture(long sound_index, const s_sound_permutation_dialogue_info *info, short index)
{
    s_sound_encoded_dialogue_section *section = sound_definitions_get_encoded_dialogue(sound_index);
    return &((const unsigned char *)section->mouth_aperture.address)[info->mouth_aperture_offset + index];
}
static __forceinline float sound_definitions_pitch_from_cents(float cents)
{
    return (float)exp((double)(cents * 0.0005776227f));
}
// @address 0x1f6488 ?sound_pitch_range_get_default_sample_time@@YAMPAUs_cache_file_sound_definition@@PAUs_cache_file_sound_pitch_range@@K@Z
static __forceinline const s_sound_cache_file_gestalt_definition *sound_definitions_gestalt_get()
{
    return (const s_sound_cache_file_gestalt_definition *)g_sound_cache_file_gestalt.data;
}
float sound_pitch_range_get_default_sample_time(s_cache_file_sound_definition *definition, s_cache_file_sound_pitch_range *range, unsigned long sample_count)
{
    const s_sound_playback_parameters *playback = &sound_definitions_gestalt_get()->playback_parameters[definition->playback_index];
    const s_sound_scale_modifiers *scale = &sound_definitions_gestalt_get()->scale_modifiers[definition->scale_index];
    long natural_pitch = (short)sound_definitions_gestalt_get()->pitch_parameters[range->parameter_index].natural_pitch;
    short playback_pitch = (short)playback->pitch_lower <= (short)playback->pitch_upper ? (short)playback->pitch_lower : (short)playback->pitch_upper;
    short scale_pitch = (short)scale->pitch.lower <= (short)scale->pitch.upper ? (short)scale->pitch.lower : (short)scale->pitch.upper;
    float minimum_pitch = sound_definitions_pitch_from_cents((float)(playback_pitch + scale_pitch));
    float natural_pitch_scale = sound_definitions_pitch_from_cents((float)natural_pitch);
    return ((float)sample_count * natural_pitch_scale * 1000.0f) /
        ((float)sound_sample_rate_samples_per_second[sound_definitions_gestalt_get()->codecs[definition->platform_index].sample_rate] * minimum_pitch);
}

// @address 0x1f6600 ?sound_definition_get_permutation_play_time@@YAMJJJ@Z
float sound_definition_get_permutation_play_time(long sound_index, long pitch_range_index, long permutation_index)
{
    s_cache_file_sound_definition *definition = sound_tag_get('snd!', sound_index);
    s_cache_file_sound_pitch_range *range = sound_definitions_get_pitch_range(definition, pitch_range_index);
    s_cache_file_sound_permutation *permutation = sound_definitions_get_permutation(range, permutation_index);
    float sample_time = sound_pitch_range_get_default_sample_time(definition, range, permutation->sample_count);
    long natural_pitch = (short)sound_definitions_gestalt_get()->pitch_parameters[range->parameter_index].natural_pitch;
    return sample_time * (float)exp((double)((float)natural_pitch * -0.0005776227f));
}

// @address 0x1f66e0 ?sound_permutation_get_real_mouth_aperture@@YAMJFFF@Z
float sound_permutation_get_real_mouth_aperture(long sound_index, short pitch_range_index, short permutation_index, short mouth_aperture_index)
{
    s_cache_file_sound_definition *definition = sound_tag_get('snd!', sound_index);
    s_cache_file_sound_pitch_range *pitch_range = sound_definitions_get_pitch_range(definition, pitch_range_index);
    if (permutation_index >= 0 && permutation_index < sound_definitions_get_permutation_count(pitch_range))
    {
        s_cache_file_sound_permutation *permutation = sound_definitions_get_permutation(pitch_range, permutation_index);
        const s_sound_permutation_dialogue_info *dialogue_info = sound_definitions_get_dialogue_info(sound_index, permutation->dialogue_info_index);
        if (dialogue_info->mouth_aperture_count > 0)
        {
            mouth_aperture_index = (short)((mouth_aperture_index > 0 ? mouth_aperture_index : 0) < dialogue_info->mouth_aperture_count - 1
                    ? (mouth_aperture_index > 0 ? mouth_aperture_index : 0)
                    : dialogue_info->mouth_aperture_count - 1);
            return (float)*sound_definitions_get_mouth_aperture(sound_index, dialogue_info, mouth_aperture_index) * (1.0f / 255.0f);
        }
    }
    return 0.0f;
}

void s_cache_file_sound_pitch_range::set_discarded_permutation_index(s_cache_file_sound_definition *parent_sound,short discarded_index)

{
  sound_globals_reference *globals_reference;
  unsigned char flag;
  
  if (global_game_globals->sound_globals.count()) {
    globals_reference = global_game_globals->sound_globals.begin();
  }
  else {
    globals_reference = 0;
  }
  // match: preserve the original volatile reads and their memory access order.
  // match: 0x1f6820: volatile class flags read preserves access order.
  flag = ((const volatile sound_classes *)global_tag_instances[
      // match: 0x1f6820: volatile tag-reference read preserves access order.
      ((const volatile sound_globals_reference *)globals_reference)->sound_classes.index & 0xffffU].definition)
      ->classes[((const s_cache_file_sound_definition *)parent_sound)->sound_class].flags >> 5 & 1;
  if (flag != 0) {
    return;
  }
  discarded_permutation_index = discarded_index;
  return;
}

// @address 0x1f6888 ?get_discarded_permutation_index@s_cache_file_sound_pitch_range@@QBAFPAUs_cache_file_sound_definition@@@Z
short s_cache_file_sound_pitch_range::get_discarded_permutation_index(s_cache_file_sound_definition *owning_sound) const
{
    bool deterministic = sound_definition_detail::sound_class_get(owning_sound->sound_class)->is_deterministic();
    long result;
    if (!deterministic)
        result = discarded_permutation_index;
    else
        result = -1;
    return (short)result;
}

// @address 0x1f6900 ?get_first_deterministic_flag_index@s_cache_file_sound_pitch_range@@QBAJPAUs_cache_file_sound_definition@@@Z
long s_cache_file_sound_pitch_range::get_first_deterministic_flag_index(s_cache_file_sound_definition *owning_sound) const

{
  sound_globals_reference *sound_globals_entry;
  unsigned short flags;
  unsigned char b;
  
  if (global_game_globals->sound_globals.count()) {
    sound_globals_entry = global_game_globals->sound_globals.begin();
  }
  else {
    sound_globals_entry = 0;
  }
  // match: 0x1f6900: preserve the original volatile tag-reference read.
  flags = ((const sound_classes *)global_tag_instances[
      // match: 0x1f6900: volatile tag-reference read preserves access order.
      DATUM_INDEX_TO_ABSOLUTE_INDEX(((const volatile sound_globals_reference *)sound_globals_entry)->sound_classes.index)].definition)
      ->classes[owning_sound->sound_class].flags;
  b = (flags >> 5) & 1;
  if (b && (internal_first_flag_index != 0xffff)) {
    return (unsigned int)internal_first_flag_index;
  }
  return 0xffffffffffffffff;
}

// @address 0x1f6a88 sound_definition_get_permutation_play_time_for_language
extern "C" {
float sound_definition_get_permutation_play_time_for_language(long sound_index, long pitch_range_index, long permutation_index, long block_index)
{
    const s_sound_language_permutation_info *block = &g_sound_cache_file_gestalt.data->language_permutation_info[block_index];
    s_cache_file_sound_definition *definition = sound_tag_get('snd!', sound_index);
    long sample_offset = block->sample_indices[definition->first_sample_index + pitch_range_index].permutation_start_index;
    unsigned short sample_count = block->sample_counts[sample_offset + permutation_index];
    s_cache_file_sound_pitch_range *range = sound_definitions_get_pitch_range(definition, pitch_range_index);
    long natural_pitch = (short)sound_definitions_gestalt_get()->pitch_parameters[range->parameter_index].natural_pitch;
    float sample_time = (float)sample_count * (1.0f / 30.0f);
    float pitch = (float)exp((double)((float)natural_pitch * -0.0005776227f));
    return pitch * sample_time * 1000.0f;
}
}

extern "C" {
// @address 0xfa4f8 ?sound_definition_next_permutation_deterministic@@YAFPAUs_cache_file_sound_definition@@FFPAK1PA_N_N@Z

}
// PowerPC slw gives 0 for a shift count of 32..63; x86 shl takes the count mod 32, so 1 << 32 is 1 there. A pitch
// range has up to 63 permutations and its played flags are one 32-bit word: with 32 permutations "every permutation
// played" ((1 << 32) - 1, all ones on the Xbox) became 0 and the flags reset on every pick (GAME-CHANGES F111).
#if defined(__clang__)
#define SOUND_PERMUTATION_BIT(n) ((unsigned long)(n) < 32 ? 1UL << (n) : 0UL)
#else
#define SOUND_PERMUTATION_BIT(n) (1UL << (n))
#endif
short sound_definition_next_permutation_deterministic(struct s_cache_file_sound_definition *definition, short pitch_range_index,
    short last_permutation_index, unsigned long *permutation_flags, unsigned long *seed, bool *exhausted, bool invert_skip)
{
    s_cache_file_sound_pitch_range *range = sound_definitions_get_pitch_range(definition, pitch_range_index);
    int packed = g_sound_cache_file_gestalt.use_uncompressed_permutation_counts;
    s_sound_cache_file_gestalt_definition *gestalt = g_sound_cache_file_gestalt.data;
    long count = (((unsigned int)packed) ? (range->permutation_info >> 26) : ((range->permutation_info >> 20) & 63));
    long maximum_attempts;
    long attempts;
    short permutation_index;
    if (exhausted) *exhausted = 0;
    maximum_attempts = count * 2;
    attempts = 0;
    float half = 0.5f;
    float one = 1.0f;
    float scale = 1.5259022e-05f;
    for (;;) {
        long reset_count;
        unsigned char multiple;
        float skip_fraction;
        permutation_index = random_range(seed, 0, (short)count);
        reset_count = (((unsigned int)packed) ? (range->permutation_info >> 26) : ((range->permutation_info >> 20) & 63));
        if (((SOUND_PERMUTATION_BIT(reset_count) - 1) & ~*((int *)permutation_flags)) == 0) {
            *((int *)permutation_flags) = 0;
            multiple = reset_count > 1;
            if (multiple && last_permutation_index != -1)
                *((int *)permutation_flags) = SOUND_PERMUTATION_BIT(last_permutation_index);
            last_permutation_index = -1;
        }
        if ((*((int *)permutation_flags) & SOUND_PERMUTATION_BIT((short)permutation_index)) == 0) {
            *((int *)permutation_flags) |= SOUND_PERMUTATION_BIT((short)permutation_index);
            if (exhausted && ((SOUND_PERMUTATION_BIT(count) - 1) & ~*((int *)permutation_flags)) == 0)
                *exhausted = 1;
            skip_fraction =
                (float)gestalt->permutations[((long)(range->permutation_info << 12) >> 12) + (short)permutation_index]
                    .skip_fraction *
                scale;
            if (invert_skip && !(skip_fraction < half))
                skip_fraction = one - skip_fraction;
            {
                unsigned long random = random_seed_next(seed);
                if (skip_fraction <= (float)(random >> 16) * scale) break;
            }
            ++attempts;
            if (attempts == maximum_attempts) break;
        }
    }
    return permutation_index;
}
