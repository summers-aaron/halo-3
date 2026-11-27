#ifndef SOUND_SOUND_CACHE_FILE_GESTALT_H
#define SOUND_SOUND_CACHE_FILE_GESTALT_H

// Shared blocks indexed by cached sound definitions.

#include "tag_files/tag_types.h"
#include "sound/sound_effect_collection.h"
#include "sound/game_sound_definitions.h"
#include <stddef.h>

struct s_sound_permutation_dialogue_info
{
    long mouth_aperture_offset;
    long mouth_aperture_count;
    long lipsync_data_offset;
    long lipsync_data_count;
};
struct s_lipsync_keyframe {
    short time;
    signed char phoneme_index[3];
    signed char phoneme_value[3];
};

struct s_lipsync_data {
    float start_time;
    float end_time;
    float blend_in;
    float blend_out;
    // Reserved by the serialized facial-animation block; not read by this runtime path.
    unsigned long reserved_10;
    unsigned long reserved_14;
    unsigned long reserved_18;
    long keyframe_count;
    TAG_POINTER(s_lipsync_keyframe) keyframes;
    TAG_POINTER(void) keyframe_definition;
};

struct s_sound_extra_info {
    long facial_animation_count;
    TAG_POINTER(s_lipsync_data) lipsync;
    TAG_POINTER(void) facial_animation_definition;
};

struct s_sound_encoded_dialogue_section
{
    s_tag_data mouth_aperture;
    c_typed_tag_block<s_sound_permutation_dialogue_info> dialogue_info;
    c_typed_tag_block<s_sound_extra_info> facial_animation;
};
struct s_cache_file_sound_definition_extra_info
{
    c_typed_tag_block<s_sound_encoded_dialogue_section> encoded_dialogue;
};
struct s_sound_language_permutation_sample_index
{
    short permutation_start_index;
    short permutation_count;
};
struct s_sound_language_permutation_info
{
    long language;
    c_typed_tag_block<unsigned short> sample_counts;
    c_typed_tag_block<s_sound_language_permutation_sample_index> sample_indices;
};

typedef char sound_language_block_size[
    (sizeof(s_sound_language_permutation_info) == 28) ? 1 : -1];
typedef char sound_language_counts_offset[
    (offsetof(s_sound_language_permutation_info, sample_counts) + offsetof(s_tag_block, address) == 8) ? 1 : -1];
typedef char sound_dialogue_info_offset[
    (offsetof(s_sound_encoded_dialogue_section, dialogue_info) == 20) ? 1 : -1];

struct s_cache_file_sound_definition;

struct s_sound_promotion_rule
{
    short pitch_range_index;
    short count;
    float timeout_seconds;
    long threshold;
    long increment;
};
struct s_sound_promotion
{
    c_typed_tag_block<s_sound_promotion_rule> rules;
    c_typed_tag_block<long> runtime_timers;
    long current_rule;
    long last_update_time;
    long suppress_until;
};
struct s_sound_codec
{
    signed char sample_rate;
    unsigned char encoding;
    unsigned char compression;
};

struct s_sound_override_playback_location
{
    unsigned long flags;
    float azimuth;
    union { real_decibel gain; long gain_bits; };
    union { float distance; long distance_bits; };
};

struct s_sound_playback_parameters
{
    unsigned long flags;
    float silence_distance;
    float attack_distance;
    float minimum_distance;
    float maximum_distance;
    float gain;
    float skip_fraction;
    real_decibel gain_base;
    real_decibel gain_variance;
    short pitch_lower;
    short pitch_upper;
    float inner_cone_angle;
    float outer_cone_angle;
    real_decibel outer_cone_gain;
    s_sound_override_playback_location override_location;

    __forceinline bool test(long bit) const { bool result = (flags & (1 << bit)) != 0; return result; }
};

typedef char sound_playback_override_offset[
    (offsetof(s_sound_playback_parameters, override_location) == 0x34) ? 1 : -1];
typedef char sound_playback_parameters_size[
    (sizeof(s_sound_playback_parameters) == 0x44) ? 1 : -1];

struct s_sound_pitch_bounds { short lower, upper; };
struct s_sound_scale_modifiers
{
    real_decibel gain_lower;
    real_decibel gain_upper;
    s_sound_pitch_bounds pitch;
    real_decibel skip_fraction_lower;
    real_decibel skip_fraction_upper;
};

struct s_sound_gestalt_pitch_parameters
{
    short natural_pitch;
    s_sound_pitch_bounds bend_bounds;
    s_sound_pitch_bounds runtime_bend_bounds;
    s_sound_pitch_bounds maximum_gain_pitch_bounds;
};


// Permutation counts occupy the high bits of permutation_info.
struct s_cache_file_sound_pitch_range
{
    short name_index;
    short parameter_index;
    signed char last_permutation_index;
    signed char discarded_permutation_index;
    unsigned short internal_first_flag_index;   // Bit index into runtime_permutation_flags.
    union
    {
        unsigned long permutation_info;
#if defined(_XBOX) && !defined(__clang__)
        struct
        {
            unsigned long uncompressed_permutation_count : 6;
            unsigned long permutation_count : 6;
            long first_permutation_index : 20;
        };
#else
        // Other compilers on little-endian hosts allocate bit-fields from the low bit: the fields are listed low to
        // high so each keeps its Xbox bit position in the swapped word (GAME-CHANGES F38).
        struct
        {
            long first_permutation_index : 20;
            unsigned long permutation_count : 6;
            unsigned long uncompressed_permutation_count : 6;
        };
#endif
    };

    void set_discarded_permutation_index(s_cache_file_sound_definition *owning_sound, short discarded_index);
    void set_last_permutation_index(s_cache_file_sound_definition *owning_sound, short new_permutation_index);
    short get_last_permutation_index(s_cache_file_sound_definition *owning_sound) const;
    short get_discarded_permutation_index(s_cache_file_sound_definition *owning_sound) const;
    long get_first_deterministic_flag_index(s_cache_file_sound_definition *owning_sound) const;
};

// The flags and size word of a sound chunk: the flags in the top 6 bits, the size in the low 26.
struct sound_chunk_flags_and_size
{
    unsigned long value;

    __forceinline long get_flags() const { return value >> 26; }
    __forceinline long get_size() const { return value & 0x03ffffff; }
    __forceinline void set_flags(long flags) { value = (get_size() & 0x03ffffff) | ((flags << 26) & 0xfc000000); }
};

// One chunk of sample data (gestalt chunks element, 20 bytes). runtime_offset is -1 until the chunk is loaded.
struct sound_chunk
{
    long file_offset;
    sound_chunk_flags_and_size flags;
    long runtime_offset;
    long unknown_offset;
    long unknown_size;

    __forceinline bool test_flag(long bit) const { return (flags.get_flags() & (1 << bit)) != 0; }
    __forceinline void set_flag(long bit, bool set)
    {
        long value = flags.get_flags();
        if (set)
            value |= 1 << bit;
        else
            value &= ~(1 << bit);
        flags.set_flags(value);
    }
};

struct s_cache_file_sound_permutation
{
    unsigned short name_index;
    unsigned short skip_fraction;
    unsigned long sample_count;
    long first_chunk_index;
    unsigned short chunk_count;
    signed char gain;
    signed char dialogue_info_index;
};

struct s_sound_cache_file_gestalt_definition
{
    c_typed_tag_block<s_sound_codec> codecs;
    c_typed_tag_block<s_sound_playback_parameters> playback_parameters;
    c_typed_tag_block<s_sound_scale_modifiers> scale_modifiers;
    s_tag_block import_names;
    c_typed_tag_block<s_sound_gestalt_pitch_parameters> pitch_parameters;
    c_typed_tag_block<s_cache_file_sound_pitch_range> pitch_ranges;
    c_typed_tag_block<s_cache_file_sound_permutation> permutations;
    c_typed_tag_block<s_platform_sound_playback_definition> platform_playbacks;
    c_typed_tag_block<s_sound_language_permutation_info> language_permutation_info;
    s_tag_block runtime_permutation_flags;                      // one bit per permutation, bytes big-endian
    s_tag_data sample_data;                                     // cache_file_sound_get_sample_pointer
    unsigned long sample_data_offset;                           // cache_file_sound_get_sample_offset base
    unsigned long : 32;
    c_typed_tag_block<sound_chunk> chunks;
    c_typed_tag_block<s_sound_promotion> promotions;
    c_typed_tag_block<s_cache_file_sound_definition_extra_info> extra_infos;
};

typedef char sound_pitch_maximum_gain_offset[
    (offsetof(s_sound_gestalt_pitch_parameters, maximum_gain_pitch_bounds) == 0xa) ? 1 : -1];

#if defined(_M_PPCBE) || defined(_XBOX)
typedef char sound_promotion_size[(sizeof(s_sound_promotion) == 36) ? 1 : -1];
typedef char sound_promotion_runtime_timers_offset[
    (offsetof(s_sound_promotion, runtime_timers) == 12) ? 1 : -1];
typedef char sound_playback_parameters_size[
    (sizeof(s_sound_playback_parameters) == 0x44) ? 1 : -1];
typedef char sound_playback_gain_offset[
    (offsetof(s_sound_playback_parameters, gain_base) == 0x1c) ? 1 : -1];
typedef char sound_scale_modifiers_size[
    (sizeof(s_sound_scale_modifiers) == 0x14) ? 1 : -1];
typedef char sound_pitch_bend_offset[
    (offsetof(s_sound_gestalt_pitch_parameters, bend_bounds) == 2) ? 1 : -1];
typedef char sound_pitch_runtime_bend_offset[
    (offsetof(s_sound_gestalt_pitch_parameters, runtime_bend_bounds) == 6) ? 1 : -1];
typedef char sound_pitch_parameters_size[
    (sizeof(s_sound_gestalt_pitch_parameters) == 0xe) ? 1 : -1];
typedef char sound_codec_size[
    (sizeof(s_sound_codec) == 3) ? 1 : -1];
typedef char sound_gestalt_language_offset[
    (offsetof(s_sound_cache_file_gestalt_definition, language_permutation_info) == 0x60) ? 1 : -1];
typedef char sound_gestalt_scale_offset[
    (offsetof(s_sound_cache_file_gestalt_definition, scale_modifiers) == 0x18) ? 1 : -1];
typedef char sound_gestalt_pitch_parameters_offset[
    (offsetof(s_sound_cache_file_gestalt_definition, pitch_parameters) == 0x30) ? 1 : -1];
typedef char sound_gestalt_promotions_offset[
    (offsetof(s_sound_cache_file_gestalt_definition, promotions) == 0xa0) ? 1 : -1];
typedef char sound_cache_file_sound_pitch_range_size[
    (sizeof(s_cache_file_sound_pitch_range) == 12) ? 1 : -1];
typedef char sound_permutation_size[
    (sizeof(s_cache_file_sound_permutation) == 16) ? 1 : -1];
typedef char sound_gestalt_offset_permutations[
    (offsetof(s_sound_cache_file_gestalt_definition, permutations) == 0x48) ? 1 : -1];
typedef char sound_chunk_size[
    (sizeof(sound_chunk) == 20) ? 1 : -1];
typedef char sound_gestalt_offset_pitch_ranges[
    (offsetof(s_sound_cache_file_gestalt_definition, pitch_ranges) == 0x3c) ? 1 : -1];
typedef char sound_gestalt_offset_platform_playbacks[
    (offsetof(s_sound_cache_file_gestalt_definition, platform_playbacks) == 0x54) ? 1 : -1];
typedef char sound_gestalt_offset_runtime_permutation_flags[
    (offsetof(s_sound_cache_file_gestalt_definition, runtime_permutation_flags) == 0x6c) ? 1 : -1];
typedef char sound_gestalt_offset_sample_data[
    (offsetof(s_sound_cache_file_gestalt_definition, sample_data) == 0x78) ? 1 : -1];
typedef char sound_gestalt_offset_chunks[
    (offsetof(s_sound_cache_file_gestalt_definition, chunks) == 0x94) ? 1 : -1];
typedef char sound_gestalt_offset_extra_infos[
    (offsetof(s_sound_cache_file_gestalt_definition, extra_infos) == 0xac) ? 1 : -1];
#endif

#endif // SOUND_SOUND_CACHE_FILE_GESTALT_H
