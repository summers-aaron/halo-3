#ifndef SOUND_SOUND_CACHE_FILE_DEFINITIONS_H
#define SOUND_SOUND_CACHE_FILE_DEFINITIONS_H

// Functions defined in tu/blamlibxbox/sound_cache_file_definitions.cpp that other units call.

#include <stddef.h>

// The cached 'snd!' tag. Holds indexes to codecs, pitch ranges,
// language sample counts at +8, playbacks at +0xc and extra information at +0x12.
struct s_cache_file_sound_definition
{
    unsigned short flags;
    signed char sound_class;
    signed char pitch_range_count;
    short platform_index;
    short pitch_range_index;
    short first_sample_index;
    unsigned short : 16;
    short playback_index;
    short scale_index;
    signed char promotion_index;
    signed char platform_playback_index;
    short extra_info_index;
    long maximum_play_time;
};

typedef char cache_file_sound_definition_size[(sizeof(s_cache_file_sound_definition) == 0x18) ? 1 : -1];
typedef char cache_file_sound_definition_class_offset[(offsetof(s_cache_file_sound_definition, sound_class) == 2) ? 1 : -1];
typedef char cache_file_sound_definition_count_offset[(offsetof(s_cache_file_sound_definition, pitch_range_count) == 3) ? 1 : -1];
typedef char cache_file_sound_definition_platform_offset[(offsetof(s_cache_file_sound_definition, platform_index) == 4) ? 1 : -1];
typedef char cache_file_sound_definition_range_offset[(offsetof(s_cache_file_sound_definition, pitch_range_index) == 6) ? 1 : -1];
typedef char cache_file_sound_definition_samples_offset[(offsetof(s_cache_file_sound_definition, first_sample_index) == 8) ? 1 : -1];
typedef char cache_file_sound_definition_playback_offset[(offsetof(s_cache_file_sound_definition, playback_index) == 0xc) ? 1 : -1];
typedef char cache_file_sound_definition_scale_offset[(offsetof(s_cache_file_sound_definition, scale_index) == 0xe) ? 1 : -1];
typedef char cache_file_sound_definition_promotion_offset[(offsetof(s_cache_file_sound_definition, promotion_index) == 0x10) ? 1 : -1];
typedef char cache_file_sound_definition_platform_playback_offset[(offsetof(s_cache_file_sound_definition, platform_playback_index) == 0x11) ? 1 : -1];
typedef char cache_file_sound_definition_extra_info_offset[(offsetof(s_cache_file_sound_definition, extra_info_index) == 0x12) ? 1 : -1];
typedef char cache_file_sound_definition_maximum_time_offset[(offsetof(s_cache_file_sound_definition, maximum_play_time) == 0x14) ? 1 : -1];

struct s_cache_file_sound_pitch_range;

unsigned long sound_pitch_range_get_runtime_permutation_flags(const s_cache_file_sound_definition *definition,const s_cache_file_sound_pitch_range *range);
void sound_pitch_range_set_runtime_permutation_flags(const s_cache_file_sound_definition *definition,
    const s_cache_file_sound_pitch_range *range,
    unsigned long flags);
void cache_file_sound_definitions_initialize_for_new_map();

struct s_platform_sound_playback_definition;
// Gets the platform playback definition for a cached sound definition.
const s_platform_sound_playback_definition *sound_definition_try_and_get_platform_playback(s_cache_file_sound_definition *definition);

// One bit of a pitch range's 32-bit played-permutation flags. A pitch range has up to 63 permutations: PowerPC slw
// gives 0 for a shift count of 32..63, x86 shl takes the count mod 32 (1 << 32 is 1), so the PC build spells out the
// Xbox result (GAME-CHANGES F111).
#if defined(_XBOX) && !defined(__clang__)
#define SOUND_PERMUTATION_BIT(n) (1UL << (n))
#define GAME_SOUND_PERMUTATION_BIT(n) (1 << (n))
#else
#define SOUND_PERMUTATION_BIT(n) ((unsigned long)(n) < 32 ? (1UL << (n)) : 0UL)
#define GAME_SOUND_PERMUTATION_BIT(n) ((unsigned long)(n) < 32 ? (int)(1UL << (n)) : 0)
#endif

#endif // SOUND_SOUND_CACHE_FILE_DEFINITIONS_H
