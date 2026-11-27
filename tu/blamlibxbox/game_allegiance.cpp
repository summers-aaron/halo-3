// blamlibxbox/game_allegiance.obj: from the records 0x9c990 0x9c9f8 0x9ca90 0x223168 0x2231c0 0x223240 0x2232f0 0x2233a0 0x2234f0 0x223638 0x223838 0x223918 0x2239f0 0x223ba0 0x223c40 0x223c58 0x223d50
// @compiler xdk6130
// @flags /TP /O2 /Oc
#include "game/game_types.h"
#include "game/game_allegiance_data.h"
#include "game/game_globals_storage.h"
// @address 0x9c990 ?game_team_is_friendly@@YA_NW4e_game_team@@0@Z
// @address 0x9c9f8 ?game_team_is_enemy@@YA_NW4e_game_team@@0@Z
// @address 0x9ca90 ?game_team_is_ally@@YA_NW4e_game_team@@0@Z
// @address 0x223168 ?game_allegiance_initialize@@YAXXZ
// @address 0x2231c0 ?game_allegiance_initialize_for_new_map@@YAXXZ
// @address 0x223240 game_allegiance_get_player_team
// @address 0x2232f0 ?game_team_is_traitor@@YA_NW4e_game_team@@0@Z
// @address 0x2233a0 ?game_allegiance_create@@YAXW4e_game_team@@_N01FF@Z
// @address 0x2234f0 ?game_allegiance_remove@@YA_NW4e_game_team@@0@Z
// @address 0x223638 ?game_allegiance_incident@@YA_NW4e_game_team@@0F@Z
// @address 0x223838 game_allegiance_update_allied_teams
// @address 0x223918 ?game_allegiance_provoke@@YAXW4e_game_team@@0@Z
// @address 0x2239f0 ?game_allegiance_broken@@YAXPAUgame_allegiance@@_N11@Z
// @address 0x223d50 ?__tls_set_g_player_training_globals_allocator@@YAXPAX@Z

#include <string.h>
#include "cache/restricted_memory_regions.h"
#include "game/game_allegiance.h"
#include "game/game_allegiance_types.h"
#include "game/game_engine_team.h"
#include "memory/tls_globals.h"
#include "multithreading/thread_tls.h"
#include "objects/object_header.h"
#include "units/unit_types.h"
#include "physics/impacts.h"
#include "memory/data.h"
#include "objects/object_datum.h"
#include "game/game_time_types.h"
#include "game/player_types.h"
#include "ai/ai.h"
#include "cache/restricted_memory_registration.h"
#include "game/game_simulation_inline.h"
#include "cache/restricted_memory_layout.h"
#include "cache/restricted_memory.h"
#include "game/player_training_globals.h"
#include "game/game_allegiance_allocator.h"
#include "memory/data_types.h"
#include "memory/bit_vectors.h"






static __forceinline void allegiance_flag_set(unsigned long *flags, long bit, bool value)
{
    if (value) flags[bit >> 5] |= 1 << (bit & 31);
    else flags[bit >> 5] &= ~(1 << (bit & 31));
}

bool game_team_is_friendly(e_game_team team, e_game_team other_team)
{
    bool result = !game_team_is_enemy(team, other_team) || game_team_is_ally(team, other_team);
    return result;
}

bool game_team_is_enemy(e_game_team team_a, e_game_team team_b)
{
  bool result = true;
  int mode = tls_game_globals()->options.game_mode;

  if (mode == 2) {
    result = game_engine_team_is_enemy(team_a, team_b);
  } else if (team_a == team_b) {
    result = false;
  } else if (team_a != -1 && team_b != -1 && mode == 1 &&
      (unsigned int)team_a <= 15 && (unsigned int)team_b <= 15) {
    int bit = (int)team_a * 16 + (int)team_b;
    result = (tls_game_allegiance_globals()->enemy_bits[bit >> 5] & (1 << (bit & 31))) == 0;
  }
  return result;
}

bool game_team_is_ally(e_game_team a, e_game_team b)
{
    bool result = false;
    long mode = tls_game_globals()->options.game_mode;
    if (mode == 2) {
        result = !game_engine_team_is_enemy(a, b);
    } else if (a == -1 || b == -1) {
        result = false;
    } else {
        if (mode == 1) {
            if ((unsigned long)a <= 15 && (unsigned long)b <= 15) {
                long bit = (long)a * 16 + (long)b;
                result = (tls_game_allegiance_globals()->relation_bits[bit >> 5] & (mode << (bit & 31))) != 0;
            }
        } else {
            result = (long)b - (long)a == 0;
        }
    }
    return result;
}

void game_allegiance_initialize()
{
    s_game_allegiance_globals *globals = (s_game_allegiance_globals *)g_game_allegiance_globals_allocator.reserve_memory(
        "game allegiance globals", 0, sizeof(s_game_allegiance_globals), 0);
    tls_game_allegiance_globals_set(globals);
    memset(globals, 0, sizeof(s_game_allegiance_globals));
}

// @address 0x5bec98 ?game_allegiance_dispose@@YAXXZ
void game_allegiance_dispose(void)
{
}

void game_allegiance_initialize_for_new_map()
{
    memset(tls_game_allegiance_globals(), 0, sizeof(s_game_allegiance_globals));
    for (short team_index = 0; team_index < 16; team_index++) {
        long bit = team_index * 16 + team_index;
        allegiance_flag_set(tls_game_allegiance_globals()->enemy_bits, bit, true);
    }
}

// @address 0x5bec98 ?game_allegiance_dispose_from_old_map@@YAXXZ
void game_allegiance_dispose_from_old_map(void)
{
}


extern "C" long game_allegiance_get_player_team(long player_index)
{
    long result = -1;
    if (player_index != -1) {
        player_datum *player = (player_datum *)datum_try_and_get(tls_player_data(), player_index);
        if (player) {
            long mode = tls_game_globals()->options.game_mode;
            if (mode == 1) {
                if (player->unit_index != -1)
                    result = ((unit_datum *)object_get_and_verify_type(player->unit_index, _object_mask_unit))->unit.team;
            } else if (mode == 2) {
                result = player->configuration.host.team_index;
            }
        }
    }
    return result;
}

bool game_team_is_traitor(e_game_team team_a, e_game_team team_b)
{
  bool result = false;
  if (team_a != -1 && team_b != -1 && team_a != team_b &&
      tls_game_globals()->options.game_mode == 1 && (unsigned int)team_a <= 15 && (unsigned int)team_b <= 15) {
    result = bit_vector_test(tls_game_allegiance_globals()->relation_bits, team_a * 16 + team_b) &&
      !bit_vector_test(tls_game_allegiance_globals()->enemy_bits, team_a * 16 + team_b);
  }
  return result;
}

static __forceinline long game_allegiance_seconds_to_ticks_round(float seconds)
{
    return (long)((float)tls_game_time_globals()->ticks_per_second * seconds + 0.5f);
}
static __forceinline void game_allegiance_copy(game_allegiance *destination, const game_allegiance *source)
{
    *destination = *source;
}
void game_allegiance_create(e_game_team team, bool reverse, e_game_team other_team, bool forward, short threshold, short delay)
{
    if (game_is_authoritative()) {
        s_game_allegiance_globals *globals = tls_game_allegiance_globals();
        short count = globals->count;
        short index = 0;
        game_allegiance *entry = &globals->allegiances[0];
        for (; index < count; index++, entry++) {
            if ((entry->team == team && entry->other_team == other_team) ||
                (entry->other_team == team && entry->team == other_team))
                break;
        }
        if (index >= count && count < 16) {
            index = globals->count;
            globals->count = count + 1;
        }
        if (index < globals->count) {
            entry = &globals->allegiances[index];
            entry->team = team;
            entry->reverse = reverse;
            entry->other_team = other_team;
            entry->forward = forward;
            entry->threshold = threshold;
            entry->delay = delay;
            entry->incidents = 0;
            entry->timer = 0;
            entry->broken = true;
            ::game_allegiance_broken(entry, false, false, false);
            entry->changed = false;
        }
    }
}
bool game_allegiance_remove(e_game_team team, e_game_team other_team)
{
    bool result = false;
    if (game_is_authoritative()) {
        short index = 0;
        game_allegiance *entry = &tls_game_allegiance_globals()->allegiances[0];
        for (; index < tls_game_allegiance_globals()->count; index++, entry++) {
            if ((entry->team == team && entry->other_team == other_team) ||
                (entry->other_team == team && entry->team == other_team)) {
                ::game_allegiance_broken(entry, true, true, false);
                if (--tls_game_allegiance_globals()->count > index)
                    game_allegiance_copy(&tls_game_allegiance_globals()->allegiances[index],
                        &tls_game_allegiance_globals()->allegiances[tls_game_allegiance_globals()->count]);
                result = true;
                break;
            }
        }
    }
    return result;
}
bool game_allegiance_incident(e_game_team team, e_game_team other_team, short incident)
{
    bool result = false;
    if (game_is_authoritative()) {
        short index = 0;
        game_allegiance *entry = &tls_game_allegiance_globals()->allegiances[0];
        for (; index < tls_game_allegiance_globals()->count; index++, entry++) {
            if ((entry->team == team && entry->other_team == other_team && entry->forward) ||
                (entry->other_team == team && entry->team == other_team && entry->reverse)) {
                short delta = 0;
                long now = tls_game_time_globals()->time;
                if (now - entry->time > game_allegiance_seconds_to_ticks_round(0.2f)) {
                    entry->time = now;
                    switch (incident) {
                    case 0: delta = 1; break;
                    case 1: delta = 3; break;
                    case 2: delta = -1; break;
                    case 3: delta = entry->threshold - entry->incidents < 1 ? 1 : entry->threshold - entry->incidents; break;
                    }
                    entry->incidents += delta;
                    if (entry->delay != -1) entry->timer = entry->delay;
                    if (entry->threshold != -1 && entry->incidents >= entry->threshold) {
                        ::game_allegiance_broken(entry, true, false, true);
                        result = true;
                    }
                    break;
                }
            }
        }
    }
    return result;
}

extern "C" void game_allegiance_update_allied_teams(long team_a, long team_b, bool broken, bool permanently)
{
    if (team_a != 1 && team_b != 1) return;
    long other = team_a == 1 ? team_b : team_a;
    short index = 0;
    game_allegiance *entry = &tls_game_allegiance_globals()->allegiances[0];
    for (; index < tls_game_allegiance_globals()->count; index++, entry++) {
        if (entry->team == 1 && game_team_is_ally((e_game_team)entry->other_team, (e_game_team)other))
            ::game_allegiance_broken(entry, broken, permanently, false);
        else if (entry->other_team == 1 && game_team_is_ally((e_game_team)entry->team, (e_game_team)other))
            ::game_allegiance_broken(entry, broken, permanently, false);
    }
}

void game_allegiance_provoke(e_game_team team, e_game_team other_team)
{
    if (game_is_authoritative()) {
        short index = 0;
        game_allegiance *entry = &tls_game_allegiance_globals()->allegiances[0];
        for (; index < tls_game_allegiance_globals()->count; index++, entry++) {
            if ((entry->team == team && entry->other_team == other_team && entry->forward) ||
                (entry->other_team == team && entry->team == other_team && entry->reverse)) {
                if (entry->incidents > 0 && entry->delay != -1)
                    entry->timer = entry->delay;
                break;
            }
        }
    }
}


// @address 0x223ba0 ?reserve_memory@?$t_restricted_allocation_manager@$02$0A@$0A@$1?__tls_set_g_game_allegiance_globals_allocator@@YAXPAX@Z@@QAAPAXPBD0IJ@Z
// @address 0x223c40 ?allocate@?$t_restricted_allocation_manager@$02$0A@$0A@$1?__tls_set_g_game_allegiance_globals_allocator@@YAXPAX@Z@@UAAPAXIPBD@Z
// @address 0x223c58 ?deallocate@?$t_restricted_allocation_manager@$02$0A@$0A@$1?__tls_set_g_game_allegiance_globals_allocator@@YAXPAX@Z@@UAAXPAX@Z

// Registers a member of field layout 1 (see c_restricted_memory::add_member) and publishes its
// per-thread address. One of a family of identical allocators that differ
// only in the layout index, the update callback and the thread-local pointer.





void game_allegiance_broken(game_allegiance *allegiance, bool broken, bool force, bool notify)
{
    if (force || allegiance->broken != broken) {
        allegiance->broken = broken;
        if (allegiance->team < 16 && allegiance->other_team < 16) {
            s_game_allegiance_globals *globals = tls_game_allegiance_globals();
            allegiance_flag_set(globals->relation_bits, allegiance->team * 16 + allegiance->other_team, !force);
            allegiance_flag_set(globals->relation_bits, allegiance->other_team * 16 + allegiance->team, !force);
            allegiance_flag_set(globals->enemy_bits, allegiance->team * 16 + allegiance->other_team, !broken);
            allegiance_flag_set(globals->enemy_bits, allegiance->other_team * 16 + allegiance->team, !broken);
        }
        allegiance->changed = true;
        ::ai_handle_allegiance_status_changed((e_game_team)allegiance->team,
            (e_game_team)allegiance->other_team, broken, force, notify);
    }
}



template<>
void *t_restricted_allocation_manager<3, static_cast<void (*)(void *)>(0), static_cast<void (*)(void *)>(0),
    &__tls_set_g_game_allegiance_globals_allocator>::reserve_memory(
    const char *name, const char *type, unsigned int size, long alignment_bits)
{
  int member_index = restricted_memory_register_member(&g_restricted_regions[3], name, type, size, 0, __tls_set_g_impact_globals_allocator,
      0, 0);
  unsigned int offset = g_restricted_regions[3].get_member_offset(member_index);
  void *address = g_restricted_address[3] + offset;
  int owner = get_current_thread_index();
  m_thread_id = owner;
  m_member_index = member_index;
  tls_game_allegiance_globals_set((s_game_allegiance_globals *)address);
  return g_restricted_address[3] + g_restricted_regions[3].get_member_offset(member_index);
}

template<> void *t_restricted_allocation_manager<3, static_cast<void (*)(void *)>(0), static_cast<void (*)(void *)>(0),
    &__tls_set_g_game_allegiance_globals_allocator>::allocate(unsigned int size, const char *name)
{
    return reserve_memory(name, "", size, 0);
}

template<> void t_restricted_allocation_manager<3, static_cast<void (*)(void *)>(0), static_cast<void (*)(void *)>(0),
    &__tls_set_g_game_allegiance_globals_allocator>::deallocate(void *memory)
{
    int value = m_member_index;
    tls_game_allegiance_globals_set(0);
    restricted_region_free_member(3, value);
}

// @address 0x11df714 g_game_allegiance_globals
__declspec(thread) s_game_allegiance_globals *g_game_allegiance_globals;

void __tls_set_g_player_training_globals_allocator(void *value)
{
    player_training_globals_set((s_player_training_entry_data *)value);
}



t_game_allegiance_allocation_manager g_game_allegiance_globals_allocator;

// @address 0x25888 k_observer_default_aspect_ratio
// @rdata 0x82025888..0x8202588c
const float k_observer_default_aspect_ratio = 1.3333334f;
// @rdata-end
