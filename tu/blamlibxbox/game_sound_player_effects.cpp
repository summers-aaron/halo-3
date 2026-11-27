// OBJ: from the records 0x1281d0 0x23e288 0x23e2d0 0x23e2f8 0x23e398 0x23e3b0
// @compiler xdk6130
// @flags /TP /O2 /Oc
#include "memory/tls_data_arrays.h"
#include "multithreading/thread_tls.h"
#include "memory/data_types.h"
#include "memory/data.h"
#include "tag_files/tag_types.h"
#include "tag_files/tag_groups.h"
#include <string.h>
#include "cache/restricted_memory_regions.h"
#include "camera/observer.h"
#include "sound/game_sound_player_effects.h"
#include "sound/game_sound_player_effects_types.h"
#include "sound/sound_effect_manager.h"
#include "sound/sound_effect_manager_internals.h"
#include "cache/cache_files.h"
#include "math/function_definitions.h"


#include "cache/restricted_memory_registration.h"
#include "cache/restricted_memory.h"






#include "objects/damage_effect_definitions.h"
// @address 0x11df868 g_player_effects_globals
extern "C" {
__declspec(thread) s_player_effects_globals *g_player_effects_globals;
}

// @address 0x23e2f8 ?reserve_memory@?$t_restricted_allocation_manager@$00$0A@$0A@$1?__tls_set_g_player_effects_globals_allocator@@YAXPAX@Z@@QAAPAXPBD0IJ@Z
template<>
void *t_restricted_allocation_manager<1,
    static_cast<void (*)(void *)>(0), static_cast<void (*)(void *)>(0),
    &__tls_set_g_player_effects_globals_allocator>::reserve_memory(
    const char *name, const char *type, unsigned int size, long alignment_bits)
{
  int member_index = restricted_memory_register_member(&g_restricted_regions[1],
      name, type, size, 0, __tls_set_g_observer_globals_allocator, 0, 0);
  unsigned int offset = g_restricted_regions[1].get_member_offset(member_index);
  void *address = g_restricted_address[1] + offset;
  int owner = get_current_thread_index();
  m_thread_id = owner;
  m_member_index = member_index;
  g_player_effects_globals = (s_player_effects_globals *)address;
  return g_restricted_address[1] + g_restricted_regions[1].get_member_offset(member_index);
}

void game_sound_player_effects_initialize(void)
{
    g_player_effects_globals = (s_player_effects_globals *)g_player_effects_globals_allocator.reserve_memory(
        "game sound player effects globals", 0, sizeof(s_player_effects_globals), 0);
}

// The March release map folds this empty game-system callback into the shared blr at 0x5bec98.
// @address 0x5bec98 ?game_sound_player_effects_dispose@@YAXXZ
void game_sound_player_effects_dispose(void)
{
}

void game_sound_player_effects_initialize_for_new_map(void)
{
    memset(g_player_effects_globals, 0, sizeof(s_player_effects_globals));
}

// @address 0x5bec98 ?game_sound_player_effects_dispose_from_old_map@@YAXXZ
void game_sound_player_effects_dispose_from_old_map(void)
{
}

template<>
void *t_restricted_allocation_manager<1,
    static_cast<void (*)(void *)>(0), static_cast<void (*)(void *)>(0),
    &__tls_set_g_player_effects_globals_allocator>::allocate(unsigned int size, const char *name)
{
    return reserve_memory(name, "", size, 0);
}

// Clears the thread's slot 0x120, then restricted_region_free_member(1) with the object's word 4.
// @address 0x23e3b0 ?deallocate@?$t_restricted_allocation_manager@$00$0A@$0A@$1?__tls_set_g_player_effects_globals_allocator@@YAXPAX@Z@@UAAXPAX@Z
template<>
void t_restricted_allocation_manager<1,
    static_cast<void (*)(void *)>(0), static_cast<void (*)(void *)>(0),
    &__tls_set_g_player_effects_globals_allocator>::deallocate(void *memory)
{
    g_player_effects_globals = 0;
    restricted_region_free_member(1, m_member_index);
}




static __forceinline void game_sound_player_effects_set_scale(int index, float scale)
{
    s_sound_effect_datum *datum =
        datum_get<s_sound_effect_datum>(g_sound_effect_data, index);
    datum->manual_volume = scale;
}

// @address 0x1281d0 ?game_sound_player_effects_update@@YAXM@Z
void game_sound_player_effects_update(float elapsed)
{
    float scale;
    float minimum_duration = 0.001f;
    float zero = 0.0f;
    for (int index = 0; index < 2; ++index)
    {
        s_player_effect *effect = &g_player_effects_globals->effects[index];
        if (effect->active)
        {
            bool stop = false;
            switch (index)
            {
            case 0:
                {
                    const s_damage_effect_definition *tag =
                        TAG_GET('jpt!', const s_damage_effect_definition, effect->definition_index);
                    const s_damage_effect_player_response *response =
                        &tag->player_responses[effect->value.effect_index];
                    float duration = response->sound_effect.duration > minimum_duration ? response->sound_effect.duration : minimum_duration;
                    float progress = effect->time / duration;
                    scale = response->sound_effect.function.evaluate_scalar(progress, zero);
                    stop = effect->time > response->sound_effect.duration;
                }
                break;
            case 1:
                scale = effect->value.scale;
                break;
            }
            if (effect->sound_effect_index != -1)
                game_sound_player_effects_set_scale(effect->sound_effect_index, scale);
            effect->time += elapsed;
            if (stop)
            {
                if (effect->sound_effect_index != -1)
                    ::sound_effect_stop_internal(effect->sound_effect_index);
                effect->active = false;
            }
        }
    }
}


t_player_effects_globals_allocator g_player_effects_globals_allocator;
