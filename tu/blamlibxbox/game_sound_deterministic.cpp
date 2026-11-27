// blamlibxbox/game_sound_deterministic.obj: from the records 0x23dc08 0x23dc18 0x23dc88 0x23dcb0 0x23dcf8 0x23dff8 0x23e0f0 0x23e1a0 0x23e240 0x23e258
#include "sound/game_sound_deterministic_globals.h"
// @flags /TP /O2
#include "memory/tls_globals.h"
#include "sound/sound_cache_file_gestalt_globals.h"
#include "sound/sound_cache_file_gestalt.h"
#include "tag_files/tag_groups.h"
#include "multithreading/thread_tls.h"
#include <string.h>
#include "memory/bit_vectors.h"
#include "math/random_math_range.h"
#include "game/game_globals_storage.h"
#include "game/game_skulls.h"
#include "cache/restricted_memory_regions.h"
#include "cseries/runtime_state.h"
#include "sound/game_sound_deterministic.h"
#include "sound/game_sound_deterministic_permutation.h"
#include "sound/sound_classes.h"
#include "sound/sound_definitions.h"
#include "sound/sound_cache_file_definitions.h"
#include "cache/cache_files.h"
#include "sound/sound_class_definitions.h"
#include "cache/restricted_memory_registration.h"
#include "cache/restricted_memory_regions_inline.h"
// @address 0x23dc08 game_sound_deterministic_set_region_address
// @address 0x23dcf8 ?game_sound_deterministic_pick_permutation@@YAXJPAUs_game_sound_deterministic_permutation@@PA_N@Z
// @address 0x23dcb0 ?game_sound_deterministic_initialize_for_new_map@@YAXXZ
// @address 0x23e0f0 ?game_sound_deterministic_permutation_valid@@YA_NJPBUs_game_sound_deterministic_permutation@@@Z
// @address 0x23dff8 ?game_sound_deterministic_get_permutation_playing_time@@YAMJPBUs_game_sound_deterministic_permutation@@@Z
// @address 0x1d6bb0 ?restricted_region_free_member@@YAXJJ@Z
// @address 0xf8500 ?sound_class_get@@YAPBUsound_class_definition@@F@Z
// @address 0x1f6600 ?sound_definition_get_permutation_play_time@@YAMJJJ@Z
// @address 0x1f6a88 sound_definition_get_permutation_play_time_for_language
// @address 0x7e3640 g_sound_cache_file_gestalt


// Registers a field in restricted region 1 and publishes its thread-local address.
#include "cache/restricted_memory_layout.h"
#include "cache/restricted_memory.h"
#include "tag_files/tag_types.h"

extern "C" {
// @address 0x3e9b4 k_game_sound_deterministic_allocation_name
// @rdata 0x8203e9b4..0x8203e9d8
const char k_game_sound_deterministic_allocation_name[36] = "deterministic game sound globals";
}
// @rdata-end

extern "C" {
// @address 0x8c403c g_game_sound_deterministic_globals
s_game_sound_deterministic_globals *g_game_sound_deterministic_globals;
// @address 0x8c4040 g_game_sound_deterministic_non_multiplayer_globals
s_game_sound_deterministic_globals *g_game_sound_deterministic_non_multiplayer_globals;
// @address 0x11df8d0 g_game_sound_deterministic_multiplayer_globals
__declspec(thread) s_game_sound_deterministic_globals *g_game_sound_deterministic_multiplayer_globals;
}


extern "C" void game_sound_deterministic_set_region_address(void *address)
{
  g_game_sound_deterministic_multiplayer_globals = (s_game_sound_deterministic_globals *)address;
}


void game_sound_deterministic_initialize(void)
{
  g_game_sound_deterministic_multiplayer_globals = (s_game_sound_deterministic_globals *)g_future_development_allocator.reserve_memory(
      k_game_sound_deterministic_allocation_name, 0, sizeof(s_game_sound_deterministic_globals), 0);
  g_game_sound_deterministic_non_multiplayer_globals =
      (s_game_sound_deterministic_globals *)::runtime_state_malloc("", "", sizeof(s_game_sound_deterministic_globals));
  g_game_sound_deterministic_globals = g_game_sound_deterministic_non_multiplayer_globals;
}

// @address 0x23dc88 game_sound_deterministic_dispose
extern "C" void game_sound_deterministic_dispose(void)
{
  g_game_sound_deterministic_non_multiplayer_globals = 0;
  g_game_sound_deterministic_multiplayer_globals = 0;
  g_game_sound_deterministic_globals = 0;
}

namespace {
static __forceinline bool game_sound_deterministic_is_campaign()
{
  return tls_game_globals()->options.game_mode == 1;
}
static __forceinline bool game_sound_deterministic_is_multiplayer()
{
  return tls_game_globals()->options.game_mode == 2;
}
static __forceinline void game_sound_deterministic_clear_flags(s_game_sound_deterministic_globals *globals)
{
  memset(globals->permutation_flags, 0, sizeof(globals->permutation_flags));
}
}

// 0x23dcb0
void game_sound_deterministic_initialize_for_new_map()
{
  if (game_sound_deterministic_is_multiplayer())
    g_game_sound_deterministic_globals = g_game_sound_deterministic_multiplayer_globals;
  else
    g_game_sound_deterministic_globals = g_game_sound_deterministic_non_multiplayer_globals;
  game_sound_deterministic_clear_flags(g_game_sound_deterministic_globals);
}

// @address 0x5bec98 ?game_sound_deterministic_dispose_from_old_map@@YAXXZ
void game_sound_deterministic_dispose_from_old_map(void)
{
}

struct sound_chunk;
namespace {
static __forceinline const s_cache_file_sound_pitch_range *game_sound_deterministic_pitch_range_get(
    const s_cache_file_sound_definition *definition, long pitch_range_index)
{
  return &g_sound_cache_file_gestalt.data->pitch_ranges[
      definition->pitch_range_index + pitch_range_index];
}
static __forceinline long game_sound_deterministic_permutation_count(const s_cache_file_sound_pitch_range *range)
{
  return g_sound_cache_file_gestalt.use_uncompressed_permutation_counts
      ? range->permutation_info >> 26
      : (range->permutation_info >> 20) & 63;
}
}

namespace {
static __forceinline short game_sound_deterministic_ceiling_log2(unsigned long value)
{
  short log2 = 0;
  if (value > 1)
  {
    unsigned long remaining = value - 1;
    for (log2 = 0; remaining != 1; remaining >>= 1, log2 = (short)(log2 + 1))
    {
    }
  }
  return (short)(log2 + 1);
}
}

// 0x23dcf8
void game_sound_deterministic_pick_permutation(
    long sound_index, s_game_sound_deterministic_permutation *permutation, bool *out_exhausted)
{
  bool picked = false;
  if (out_exhausted)
    *out_exhausted = false;
  if (sound_index != -1)
  {
    const s_cache_file_sound_definition *definition = (const s_cache_file_sound_definition *)tag_get('snd!', sound_index);
    if (definition->pitch_range_count == 1)
    {
      const s_cache_file_sound_pitch_range *pitch_range = game_sound_deterministic_pitch_range_get(definition, 0);
      long permutation_count = game_sound_deterministic_permutation_count(pitch_range);
      if (permutation_count > 0)
      {
        long first_flag = pitch_range->get_first_deterministic_flag_index((s_cache_file_sound_definition *)definition);
        picked = true;
        if (first_flag != -1)
        {
          int available = 0;
          int last = 0;
          long last_bits = game_sound_deterministic_ceiling_log2(permutation_count);
          for (long i = 0; i < permutation_count; i++)
          {
            long flag_index = i + first_flag;
            if (bit_vector_test(g_game_sound_deterministic_globals->permutation_flags, flag_index))
              available |= GAME_SOUND_PERMUTATION_BIT(i);
            else
              available &= ~GAME_SOUND_PERMUTATION_BIT(i);
          }
          for (long i = 0; i < last_bits; i++)
          {
            long flag_index = i + permutation_count + first_flag;
            if (bit_vector_test(g_game_sound_deterministic_globals->permutation_flags, flag_index))
              last |= 1 << i;
            else
              last &= ~(1 << i);
          }
          bool flavor = first_flag != -1 ? game_secondary_skull_is_active(4) : false;
          permutation->permutation = (signed char)::sound_definition_next_permutation_deterministic(
              (s_cache_file_sound_definition *)definition, 0, (short)last,
              (unsigned long *)(&available), (unsigned long *)(tls_random_seed()), (bool *)(out_exhausted), flavor);
          for (long i = 0; i < last_bits; i++)
          {
            long flag_index = i + permutation_count + first_flag;
            if ((permutation->permutation & (1 << i)) != 0)
              g_game_sound_deterministic_globals->permutation_flags[flag_index >> 5] |= 1 << (flag_index & 31);
            else
              g_game_sound_deterministic_globals->permutation_flags[flag_index >> 5] &= ~(1 << (flag_index & 31));
          }
          for (long i = 0; i < permutation_count; i++)
          {
            long flag_index = i + first_flag;
            if ((unsigned long)(available & GAME_SOUND_PERMUTATION_BIT(i)) != 0)
              g_game_sound_deterministic_globals->permutation_flags[flag_index >> 5] |= 1 << (flag_index & 31);
            else
              g_game_sound_deterministic_globals->permutation_flags[flag_index >> 5] &= ~(1 << (flag_index & 31));
          }
        }
        else
        {
          permutation->permutation = (signed char)random_range(tls_random_seed(), 0, (short)permutation_count);
        }
        permutation->pitch_range = 0;
      }
    }
  }
  if (!picked)
  {
    permutation->pitch_range = -1;
    permutation->permutation = -1;
  }
}

// 0x23dff8
float game_sound_deterministic_get_permutation_playing_time(
    long sound_index, const s_game_sound_deterministic_permutation *permutation)
{
  float playing_time;
  float inner = 0.0f;
  do
  {
    if (sound_index != -1)
    {
      const s_cache_file_sound_definition *definition = (const s_cache_file_sound_definition *)tag_get('snd!', sound_index);
      if (game_sound_deterministic_permutation_valid(sound_index, permutation))
      {
        const sound_class_definition *sound_class = sound_class_get(definition->sound_class);
        if ((bool)((sound_class->internal_flags >> 7) & 1))
          inner = sound_definition_get_permutation_play_time_for_language(
              sound_index, permutation->pitch_range, permutation->permutation, tls_game_globals()->options.language);
        else
          inner = sound_definition_get_permutation_play_time(sound_index, permutation->pitch_range, permutation->permutation);
      }
      else
      {
        playing_time = (float)definition->maximum_play_time;
        break;
      }
    }
    playing_time = inner;
  } while (false);
  return playing_time / 1000.0f;
}

// 0x23e0f0
bool game_sound_deterministic_permutation_valid(long sound_index, const s_game_sound_deterministic_permutation *permutation)
{
  bool valid = false;
  if (sound_index != -1)
  {
    const s_cache_file_sound_definition *definition = (const s_cache_file_sound_definition *)tag_get('snd!', sound_index);
    long pitch_range_index = permutation->pitch_range;
    if (pitch_range_index >= 0 && pitch_range_index < definition->pitch_range_count)
    {
      const s_cache_file_sound_pitch_range *pitch_range = game_sound_deterministic_pitch_range_get(definition, pitch_range_index);
      long permutation_index = permutation->permutation;
      if (permutation_index >= 0 && permutation_index < game_sound_deterministic_permutation_count(pitch_range))
        valid = true;
    }
  }
  return valid;
}

template<>
void *t_restricted_allocation_manager<1, static_cast<void (*)(void *)>(0), static_cast<void (*)(void *)>(0),
    __tls_set_g_future_development_allocator>::reserve_memory(
    const char *name, const char *type, unsigned int size, long alignment_bits)
{
  int field_index = restricted_memory_register_member(
      &g_restricted_regions[1], name, type, size, 0,
      game_sound_deterministic_set_region_address, 0, 0);
  unsigned int offset = restricted_region_get_member_offset(1, field_index);
  void *address = restricted_region_get_base_address(1) + offset;
  int owner_thread = get_current_thread_index();
  m_thread_id = owner_thread;
  m_member_index = field_index;
  g_game_sound_deterministic_multiplayer_globals = (s_game_sound_deterministic_globals *)address;
  return restricted_region_get_member_address(1, field_index);
}

template<>
void *t_restricted_allocation_manager<1, static_cast<void (*)(void *)>(0), static_cast<void (*)(void *)>(0),
    __tls_set_g_future_development_allocator>::allocate(unsigned int size, const char *name)
{
  return reserve_memory(name, "", size, 0);
}


static __forceinline void game_sound_deterministic_set_future_allocator(void *address)
{
    g_game_sound_deterministic_multiplayer_globals = (s_game_sound_deterministic_globals *)address;
}

// @address 0x23e258 ?deallocate@?$t_restricted_allocation_manager@$00$0A@$0A@$1?__tls_set_g_future_development_allocator@@YAXPAX@Z@@UAAXPAX@Z
template<>
void t_restricted_allocation_manager<1, static_cast<void (*)(void *)>(0), static_cast<void (*)(void *)>(0),
    __tls_set_g_future_development_allocator>::deallocate(void *allocation)
{
    long allocation_member_index = m_member_index;
    game_sound_deterministic_set_future_allocator(0);
    restricted_region_free_member(1, allocation_member_index);
}


t_game_sound_deterministic_allocator g_future_development_allocator;
