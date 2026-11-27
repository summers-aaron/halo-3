// blamlibxbox/scenario_kill_trigger_volumes.obj: from the records 0xe0cc8 0x2263e8 0x226438 0x226450 0x226498 0x2264b0 0x226550
// @flags /TP /O2 /Oc
#include "memory/tls_globals.h"
#include "multithreading/thread_tls.h"
#include <string.h>
#include "ai/swarms.h"
#include "scenario/scenario_kill_trigger_volumes.h"
#include "scenario/scenario_kill_trigger_volumes_state.h"
#include "scenario/scenario_trigger_volumes.h"
#include "cache/restricted_memory_regions.h"
#include "scenario/scenario_tag.h"
#include "scenario/scenario.h"
#include "cache/restricted_memory.h"
#include "cache/restricted_memory_layout.h"
#include "cache/restricted_memory_regions_inline.h"
#include "cache/restricted_memory_registration.h"

// @address 0x11df82c g_scenario_kill_trigger_volumes_state
extern "C" { __declspec(thread) s_kill_trigger_volume_runtime_state *g_scenario_kill_trigger_volumes_state; }

// @address 0xe0cc8 ?scenario_kill_trigger_volumes_test_object@@YA_NJPA_N@Z
bool scenario_kill_trigger_volumes_test_object(long object_index, bool *always_kill)
{
    scenario *scenario = global_scenario;
    bool result = false;

    if (scenario->kill_trigger_volume_indices.count > 0)
    {
        long index = (tls_game_time_globals()->time + (unsigned short)object_index) % scenario->kill_trigger_volume_indices.count;
        if (!(g_scenario_kill_trigger_volumes_state->disabled_volumes[index >> 5] & (1 << (index & 31))))
        {
            const short *indices = (const short *)scenario->kill_trigger_volume_indices.address;
            short trigger_volume_index = indices[index];
            if (trigger_volume_index != -1)
                result = scenario_trigger_volume_test_object(trigger_volume_index, object_index);
        }
    }

    if (always_kill)
    {
        *always_kill = true;
        if (scenario->trigger_volume_indices.count > 0
            && (tls_game_time_globals()->time + (unsigned short)object_index) % scenario->trigger_volume_indices.count == 0)
        {
            *always_kill = false;
            for (long i = 0; i < scenario->trigger_volume_indices.count; i++)
            {
                const short *indices = (const short *)scenario->trigger_volume_indices.address;
                short trigger_volume_index = indices[i];
                if (trigger_volume_index != -1 && scenario_trigger_volume_test_object(trigger_volume_index, object_index))
                {
                    *always_kill = true;
                    break;
                }
            }
        }
    }
    return result;
}

// @address 0x2263e8 ?scenario_kill_trigger_volumes_initialize@@YAXXZ
void scenario_kill_trigger_volumes_initialize(void)
{
    s_kill_trigger_volume_runtime_state *state = (s_kill_trigger_volume_runtime_state *)
        g_scenario_kill_trigger_volumes_state_allocator.reserve_memory("kill trigger volume state", 0,
            sizeof(s_kill_trigger_volume_runtime_state), 0);
    g_scenario_kill_trigger_volumes_state = state;
    state->map_initialized = false;
}

// @address 0x226438 ?scenario_kill_trigger_volumes_dispose@@YAXXZ
void scenario_kill_trigger_volumes_dispose(void)
{
    g_scenario_kill_trigger_volumes_state = 0;
}

// @address 0x226450 ?scenario_kill_trigger_volumes_initialize_for_new_map@@YAXXZ
void scenario_kill_trigger_volumes_initialize_for_new_map()
{
    s_kill_trigger_volume_runtime_state *state = g_scenario_kill_trigger_volumes_state;
    memset(state->disabled_volumes, 0, sizeof(state->disabled_volumes));
    state->map_initialized = true;
}

// @address 0x226498 ?scenario_kill_trigger_volumes_dispose_from_old_map@@YAXXZ
void scenario_kill_trigger_volumes_dispose_from_old_map()
{
    g_scenario_kill_trigger_volumes_state->map_initialized = false;
}

// @address 0x2264b0 ?reserve_memory@?$t_restricted_allocation_manager@$00$0A@$0A@$1?__tls_set_g_scenario_kill_trigger_volumes_state_allocator@@YAXPAX@Z@@QAAPAXPBD0IJ@Z
template<>
void *t_restricted_allocation_manager<1, static_cast<void (*)(void *)>(0), static_cast<void (*)(void *)>(0),
    &__tls_set_g_scenario_kill_trigger_volumes_state_allocator>::reserve_memory(
    const char *name_str, const char *type_str, unsigned int size, long alignment_bits)
{
  c_restricted_memory *region = &g_restricted_regions[1];
  int member_index = restricted_memory_register_member(region, name_str, type_str, size, 0, __tls_set_g_spawner_globals_allocator, 0, 0);
  unsigned int offset = g_restricted_regions[1].get_member_offset(member_index);
  void *address = restricted_region_get_base_address(1) + offset;
  int owner = get_current_thread_index();
  m_thread_id = owner;
  m_member_index = member_index;
  g_scenario_kill_trigger_volumes_state = (s_kill_trigger_volume_runtime_state *)address;
  return restricted_region_get_base_address(1) + g_restricted_regions[1].get_member_offset(member_index);
}

// @address 0x226550 ?allocate@?$t_restricted_allocation_manager@$00$0A@$0A@$1?__tls_set_g_scenario_kill_trigger_volumes_state_allocator@@YAXPAX@Z@@UAAPAXIPBD@Z
template<>
void *t_restricted_allocation_manager<1, static_cast<void (*)(void *)>(0), static_cast<void (*)(void *)>(0),
    &__tls_set_g_scenario_kill_trigger_volumes_state_allocator>::allocate(unsigned int size, const char *name)
{
    return reserve_memory(name, "", size, 0);
}

c_kill_trigger_volumes_allocation_manager g_scenario_kill_trigger_volumes_state_allocator;
