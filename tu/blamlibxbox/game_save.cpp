// blamlibxbox/game_save.obj: from the records 0x1195b0 0x243300 0x243348 0x243388 0x2436a8 0x243748 0x243760
// @flags /TP /O2 /Oc
// @address 0x11df718 g_game_save_globals
#include "game/game_save_globals.h"
#include "game/game_save_allocator.h"
#include "memory/tls_data_arrays.h"
#include "game/game_time_inline.h"
#include "game/game_types.h"
#include "game/game_globals_storage.h"
#include <string.h>
#include "cache/restricted_memory_regions.h"
#include "game/players.h"
#include "objects/objects.h"
#include "units/units.h"
#include "ai/ai.h"
#include "effects/effects.h"
#include "items/items.h"
#include "items/projectiles.h"
#include "main/main_time.h"
#include "units/vehicles.h"
#include "interface/chud/chud_messaging.h"
#include "game/game_save.h"
#include "camera/director.h"
#include "game/game_allegiance.h"
#include "game/game_allegiance_types.h"
#include "multithreading/thread_tls.h"
#include "cache/restricted_memory_layout.h"
#include "memory/data.h"
#include "objects/object_datum.h"
#include "game/player_types.h"
#include "game/players_globals.h"
#include "memory/tls_globals.h"
#include "cutscene/cinematic_globals_types.h"
#include "cache/restricted_memory_registration.h"
#include "main/main_flag_state.h"
#include "main/main_runtime_globals.h"

__declspec(thread) s_game_save_globals *g_game_save_globals;

void game_save_initialize(void)
{
    g_game_save_globals =
        (s_game_save_globals *)((t_restricted_allocation_manager<1, static_cast<void (*)(void *)>(0), static_cast<void (*)(void *)>(0),
            &__tls_set_g_game_save_globals_allocator> *)&g_game_save_globals_allocator)
            ->reserve_memory("game save globals", 0, 24, 0);
}

// @address 0x5bec98 ?game_save_dispose@@YAXXZ
void game_save_dispose(void)
{
}

// @address 0x243348 ?game_save_initialize_for_new_map@@YAXXZ
void game_save_initialize_for_new_map(void)
{
    s_game_save_globals *globals = g_game_save_globals;
    memset(globals, 0, sizeof(*globals));
    // match: 0x243348: volatile keeps stw r11,14h(r10) first; without it stw r8,0(r10) hoists above both -1 stores
    *(volatile long *)&globals->dangerous_object_index = -1;
    globals->last_save_time = -1;
    // match: 0x243348: volatile keeps stw r8,0(r10) last; without it the state=0 store hoists above both -1 stores
    *(volatile long *)&globals->state = 0;
}

// @address 0x5bec98 ?game_save_dispose_from_old_map@@YAXXZ
void game_save_dispose_from_old_map(void)
{
}

// @address 0x2436a8 ?reserve_memory@?$t_restricted_allocation_manager@$00$0A@$0A@$1?__tls_set_g_game_save_globals_allocator@@YAXPAX@Z@@QAAPAXPBD0IJ@Z
template<>
void *t_restricted_allocation_manager<1, static_cast<void (*)(void *)>(0), static_cast<void (*)(void *)>(0),
    &__tls_set_g_game_save_globals_allocator>::reserve_memory(
    const char *name, const char *type, unsigned int size, long alignment_bits)
{
  int member_index = restricted_memory_register_member(&g_restricted_regions[1], name, type, size, 0, __tls_set_g_main_time_globals_allocator, 0, 0);
  unsigned int offset = restricted_region_get_member_offset(1, member_index);
  void *address = restricted_region_get_base_address(1) + offset;
  int owner = get_current_thread_index();
  m_thread_id = owner;
  m_member_index = member_index;
  g_game_save_globals = (s_game_save_globals *)address;
  return restricted_region_get_base_address(1) + restricted_region_get_member_offset(1, member_index);
}


// 0x243748
template <>
void *t_restricted_allocation_manager<1,static_cast<void (*)(void *)>(0),static_cast<void (*)(void *)>(0),
    __tls_set_g_game_save_globals_allocator>::allocate(unsigned int size, const char *name)
{
    return reserve_memory(name, "", size, 0);
}

// 0x243760
template <>
void t_restricted_allocation_manager<1,static_cast<void (*)(void *)>(0),static_cast<void (*)(void *)>(0),
    __tls_set_g_game_save_globals_allocator>::deallocate(void *memory)
{
    long value = m_member_index;
    g_game_save_globals = 0;
    ::restricted_region_free_member(1, value);
}


// @address 0x243388 ?game_safe_to_save_internal@@YA_N_N@Z
bool game_safe_to_save_internal(bool unused)
{
    bool result = true;
    long *dangerous_object_index = &g_game_save_globals->dangerous_object_index;
    g_game_save_globals->dangerous_object_index = -1;
    long last_save_time = g_game_save_globals->last_save_time;
    bool time_elapsed = last_save_time == -1 || game_time_get() > last_save_time + (long)((float)game_tick_rate() * 10.0f + 0.5f);
    if (!time_elapsed)
        result = false;
    if (result)
    {
        bool traitor = false;
        short count = tls_game_allegiance_globals()->count;
        game_allegiance *allegiance = tls_game_allegiance_globals()->allegiances;
        for (short i = 0; i < count && !traitor; i++, allegiance++)
        {
            e_game_team other_team = (e_game_team)allegiance->other_team;
            e_game_team team = (e_game_team)allegiance->team;
            traitor = game_team_is_traitor(team, other_team) || game_team_is_traitor(other_team, team);
        }
        if (traitor)
            result = false;
    }
    if (result && ai_enemies_can_see_player(dangerous_object_index))
        result = false;
    if (result && dangerous_projectiles_near_player(dangerous_object_index))
        result = false;
    if (result && dangerous_items_near_player(dangerous_object_index))
        result = false;
    if (result && dangerous_effects_near_player())
        result = false;
    if (result && any_unit_is_dangerous(dangerous_object_index))
        result = false;
    if (result && players_any_are_in_the_air(dangerous_object_index))
        result = false;
    if (result && players_any_are_near_death(&g_game_save_globals->dangerous_object_index))
        result = false;
    if (result)
    {
        unsigned char any_dead;
        unsigned char all_dead;
        players_death_status_calculate(&any_dead, &all_dead);
        if (any_dead)
            result = false;
    }
    if (result && tls_game_globals()->game_lost)
        result = false;
    if (result && vehicle_moving_near_any_player(&g_game_save_globals->dangerous_object_index))
        result = false;
    if (result && ::vehicle_about_to_detonate_near_any_player((long *)(&g_game_save_globals->dangerous_object_index)))
        result = false;
    if (result && tls_players_globals()->suppressed)
        return false;
    return result;
}





static __forceinline bool cinematic_in_progress()
{
    bool result = false;
    // match: 0x1195b0: an ordered pointer test against null keeps the unsigned compare.
    if (tls_cinematic_globals() > static_cast<s_cinematic_globals_definition *>(0))
        result = tls_cinematic_globals()->in_progress;
    return result;
}

static __forceinline bool game_is_lost()
{
    return tls_game_globals()->game_lost != 0;
}





static __forceinline void main_save_map()
{
    main_globals.reset_requested = true;
}

// @address 0x1195b0
void game_save_update(void)
{
    bool cancel = false;
    bool save = false;
    bool cinematic_skip = false;
    bool immediate = false;
    bool active = false;

    if (cinematic_in_progress())
    {
        active = g_game_save_globals->state == 4;
        immediate = active;
    }
    else if (g_game_save_globals->state > 0)
    {
        active = true;
        immediate = g_game_save_globals->state >= 3;
    }

    if (active)
    {
        if (immediate)
        {
            if (game_is_lost())
            {
                cancel = true;
            }
            else
            {
                save = true;
                cinematic_skip = g_game_save_globals->state == 4;
            }
        }
        else
        {
            if (g_game_save_globals->safe_check_ticks > 0)
            {
                g_game_save_globals->safe_check_ticks--;
            }
            else
            {
                if (game_safe_to_save_internal(false))
                {
                    if (++g_game_save_globals->safe_count >= 3)
                        save = true;
                }
                else
                {
                    g_game_save_globals->safe_count = 0;
                }
                g_game_save_globals->safe_check_ticks = game_seconds_to_ticks_round(0.33f);
            }
            if (!save && g_game_save_globals->state != 2 && game_ticks_to_seconds(game_time_get() - g_game_save_globals->start_time) >= 8.0f)
                cancel = true;
        }
    }

    if (save)
    {
        g_game_save_globals->state = 0;
        if (cinematic_skip)
        {
            director_reset();
        }
        else
        {
            c_data_iterator<player_datum> iterator;
            iterator.begin(tls_player_data());
            while (iterator.next())
            {
                long unit_index = iterator.get_datum()->unit_index;
                if (unit_index != NONE)
                {
                    object_datum *unit = (object_datum *)object_try_and_get_and_verify_type(unit_index, 0x1003);
                    if (unit && unit->body_vitality < 1.0f)
                    {
                        unit->body_vitality = 1.0f;
                        unit->body_stun_ticks = 0;
                    }
                }
            }
        }
        main_save_map();
        chud_messaging_special_save(true);
        g_game_save_globals->last_save_time = game_time_get();
    }
    else if (cancel)
    {
        g_game_save_globals->state = 0;
    }
}


t_game_save_globals_allocator g_game_save_globals_allocator;
