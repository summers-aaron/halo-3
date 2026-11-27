// blamlibxbox/sound_cache_file_definitions.obj: from the records 0xfa378 0xfa6f0 0x1f6978
// @flags /TP /O2 /Oc
#include "tag_files/tag_groups.h"
#include "cseries/crt_xdk.h"
#include "game/game_globals.h"
#include "sound/sound_cache_file_definitions.h"
#include "sound/sound_classes_types.h"
#include "cache/cache_file_globals.h"
#include "sound/sound_globals_reference_types.h"
#include "sound/sound_class_definitions.h"
#include "sound/sound_cache_file_gestalt.h"
#include "main/main_game.h"
#include "cseries/byte_swapping.h"
#include "scenario/scenario.h"
#include "cache/cache_file_reset_state.h"


#include "sound/sound_cache_file_gestalt_globals.h"
// @address 0x7e3640 g_sound_cache_file_gestalt
gestalt_globals g_sound_cache_file_gestalt;

static __forceinline long sound_pitch_range_get_permutation_count(const s_cache_file_sound_pitch_range *range) {
    return g_sound_cache_file_gestalt.use_uncompressed_permutation_counts ? range->uncompressed_permutation_count : range->permutation_count;
}

// @address 0x8a1eaf g_cache_files_globals
// @address 0x8a1ead g_utility_drive_mounted

unsigned long sound_pitch_range_get_runtime_permutation_flags(const s_cache_file_sound_definition *definition,const s_cache_file_sound_pitch_range *range) {
    unsigned long flags = 0;
    sound_globals_reference *reference = global_game_globals->sound_globals.count()
        ? global_game_globals->sound_globals.begin() : 0;
    const sound_class_definition *sound_class =
        &TAG_GET('sncl', sound_classes, reference->sound_classes.index)->classes[definition->sound_class];
    if (!sound_class->is_deterministic() && sound_pitch_range_get_permutation_count(range) > 1) {
        unsigned long byte_count = (sound_pitch_range_get_permutation_count(range) + 7u) / 8;
        const void *flag_data = tag_block_get_range_with_size(
            &g_sound_cache_file_gestalt.data->runtime_permutation_flags, range->internal_first_flag_index / 8, byte_count, 1);
        memcpy(&flags, flag_data, byte_count);
#if defined(_XBOX) && !defined(__clang__)
        // The flag bytes hold permutations 0..7 first. A big-endian host copies them into the high bytes and swaps;
        // a little-endian host already has them in the low bytes.
        flags = byte_swap_long(flags);
#endif
    }
    return flags;
}

void sound_pitch_range_set_runtime_permutation_flags(
    const s_cache_file_sound_definition *definition, const s_cache_file_sound_pitch_range *range, unsigned long flags)
{
    sound_globals_reference *reference = global_game_globals->sound_globals.count()
        ? global_game_globals->sound_globals.begin() : 0;
    const sound_class_definition *sound_class =
        &TAG_GET('sncl', sound_classes, reference->sound_classes.index)->classes[definition->sound_class];
    if (!sound_class->is_deterministic() && sound_pitch_range_get_permutation_count(range)>1) {
        unsigned long byte_count=(sound_pitch_range_get_permutation_count(range)+7u)/8;
        void *flag_data=tag_block_get_range_with_size(
            &g_sound_cache_file_gestalt.data->runtime_permutation_flags, range->internal_first_flag_index / 8, byte_count, 1);
#if defined(_XBOX) && !defined(__clang__)
        flags = byte_swap_long(flags);
#endif
        memcpy(flag_data,&flags,byte_count);
    }
}

void cache_file_sound_definitions_initialize_for_new_map() {
 g_sound_cache_file_gestalt.use_uncompressed_permutation_counts=cache_files_reset_in_progress();
 if(!main_game_reset_in_progress()) {
  long index=cache_file_get_global_tag_index('ugh!');
  g_sound_cache_file_gestalt.data=TAG_GET('ugh!', s_sound_cache_file_gestalt_definition, index);
  for(long i=0;i<g_sound_cache_file_gestalt.data->chunks.count();i++) {
   sound_chunk *chunk=&g_sound_cache_file_gestalt.data->chunks[i];
   if(chunk->file_offset!=-1 && !chunk->test_flag(1)) {
    chunk->runtime_offset=-1;
    chunk->set_flag(1,true);
   }
  }
 }
}

const s_platform_sound_playback_definition *sound_definition_try_and_get_platform_playback(s_cache_file_sound_definition *definition)
{
    if (definition->platform_playback_index == -1)
        return 0;
    return &g_sound_cache_file_gestalt.data->platform_playbacks[definition->platform_playback_index];
}
