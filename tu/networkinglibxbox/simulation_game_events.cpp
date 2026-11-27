// networkinglibxbox/simulation_game_events.obj: from the records 0x40a4d8 0x40a638 0x40a6c8 0x40a800 0x40a970
// @compiler xdk6130
// @flags /TP /O2 /Oc
#include "simulation/simulation_distributed_world_types.h"
#include "memory/data_types.h"
#include "memory/tls_globals.h"
#include "simulation/simulation_globals.h"
#include "game/player_types.h"
#include "game/player_access.h"
#include "simulation/game_interface/simulation_game_objects.h"
#include "simulation/simulation_queue_events.h"
#include "simulation/game_interface/simulation_game_events.h"
#include "simulation/simulation_event_handler_types.h"
// @address 0x40a4d8 ?simulation_event_generate@@YAXW4e_simulation_event_type@@JPBJJJPBXJ@Z
// @address 0x40a6c8 ?simulation_event_generate_exclusive@@YAXW4e_simulation_event_type@@JPBJJJPBXJ@Z
// @address 0x40a800 ?simulation_event_generate_exclusive_list@@YAXW4e_simulation_event_type@@JPBJPAJJJPBXJ@Z
// @address 0x40a638 ?simulation_event_generate_for_simulation_queue@@YAXW4e_simulation_event_type@@JPBJJPAX@Z

class c_simulation_watcher;
class c_simulation_distributed_world;
#include "simulation/simulation_world_types.h"

#include "simulation/simulation_watcher_types.h"
#include "game/players_globals.h"

// @address 0x808560 g_simulation_globals
// @address 0x11df67c players_globals

static __forceinline c_simulation_world *simulation_game_events_get_world()
{
    return g_simulation_globals.world;
}







static __forceinline const s_machine_identifier *simulation_game_events_machine_identifier(long machine_index)
{
    return &players_globals->machines[machine_index];
}

// 0x40a4d8: queue a simulation event with its entity reference indices and, when the
// player's machine is a simulation participant, a machine mask excluding the sender.
void simulation_event_generate(e_simulation_event_type event_type, long reference_count, const long *reference_indices, long player_index,
    long payload_size, const void *payload, long time)
{
    c_simulation_world *world = simulation_game_events_get_world();
    long world_type = world->m_world_type;
    bool distributed = world_type == 7 || world_type == 8;
    if (distributed)
    {
        c_simulation_event_handler *handler = &world->m_distributed_world->event_handler;
        long indices[2];
        simulation_event_build_entity_reference_indices(reference_count, reference_indices, indices, 2);
        if (reference_count > 0 && indices[0] == -1)
            return;
        unsigned long machine_mask = 0xffffffff;
        bool authority = world_type != 4 && world_type != 6 && world_type != 8;
        if (authority && player_index != -1)
        {
            short machine_index = player_get(player_index)->machine_index;
            if (machine_index != -1)
            {
                long sender_index = world->m_watcher->get_machine_index_by_identifier(simulation_game_events_machine_identifier(machine_index));
                if (sender_index != -1)
                    machine_mask = ~(1 << sender_index);
            }
        }
        long *references = reference_count > 0 ? indices : 0;
        handler->send_event(event_type, reference_count, references, machine_mask, payload_size, payload, time);
    }
}

void simulation_event_generate_for_simulation_queue(e_simulation_event_type type, long count, const long *references, long size, void *payload)
{
    c_simulation_world *world = simulation_game_events_get_world();
    if (world->is_distributed())
    {
        long indices[2];
        simulation_event_build_entity_reference_indices(count, references, indices, 2);
        if (indices[0] != -1)
            simulation_queue_event_insert(type, count, indices, size, payload);
    }
}

// 0x40a6c8: queue an exclusive event to the given player's machine only.
void simulation_event_generate_exclusive(e_simulation_event_type event_type, long reference_count, const long *reference_indices,
    long player_index, long payload_size, const void *payload, long time)
{
    c_simulation_world *world = simulation_game_events_get_world();
    long world_type = world->m_world_type;
    bool distributed = world_type == 7 || world_type == 8;
    if (distributed)
    {
        c_simulation_event_handler *handler = &world->m_distributed_world->event_handler;
        long indices[2];
        simulation_event_build_entity_reference_indices(1, reference_indices, indices, 2);
        if (indices[0] == -1)
            return;
        bool authority = world_type != 4 && world_type != 6 && world_type != 8;
        if (authority && player_index != -1)
        {
            short machine_index = player_get(player_index)->machine_index;
            if (machine_index != -1)
            {
                long sender_index = world->m_watcher->get_machine_index_by_identifier(simulation_game_events_machine_identifier(machine_index));
                if (sender_index != -1)
                    handler->send_event(_simulation_event_type_unit_pickup, 1, indices, 1 << sender_index, 12, payload, time);
            }
        }
    }
}

// 0x40a800: queue an exclusive event to every machine in a player list.
void simulation_event_generate_exclusive_list(e_simulation_event_type event_type, long reference_count, const long *reference_indices,
    long *player_list, long player_count, long payload_size, const void *payload, long time)
{
    c_simulation_world *world = simulation_game_events_get_world();
    long world_type = world->m_world_type;
    bool distributed = world_type == 7 || world_type == 8;
    if (distributed)
    {
        c_simulation_event_handler *handler = &world->m_distributed_world->event_handler;
        long indices[2];
        simulation_event_build_entity_reference_indices(0, 0, indices, 2);
        bool authority = world_type != 4 && world_type != 6 && world_type != 8;
        if (authority && player_list != 0 && player_count >= 1 && player_count <= 16)
        {
            unsigned long machine_mask = 0;
            for (long player_entry_index = 0; player_entry_index < player_count; player_entry_index++)
            {
                long player_index = player_list[player_entry_index];
                if (player_index != -1)
                {
                    short machine_index = player_get(player_index)->machine_index;
                    if (machine_index != -1)
                    {
                        long sender_index = world->m_watcher->get_machine_index_by_identifier(simulation_game_events_machine_identifier(machine_index));
                        if (sender_index != -1)
                            machine_mask |= 1 << sender_index;
                    }
                }
            }
            if (machine_mask != 0)
                handler->send_event(_simulation_event_type_game_engine, 0, 0, machine_mask, 40, payload, time);
        }
    }
}

// 0x40a970: resolve each reference to an entity index, the authoritative entity on the authority and the predictive one elsewhere.
// @address 0x40a970 ?simulation_event_build_entity_reference_indices@@YAXJPBJPAJJ@Z
void simulation_event_build_entity_reference_indices(long count, const long *references, long *indices, long index_count)
{
    if (count > 0)
    {
        long *index = indices;
#ifdef HALO3_64BIT
        ptrdiff_t reference_offset = (const char *)references - (const char *)indices;
#else
        long reference_offset = (char *)references - (char *)indices;
#endif
        c_simulation_world *world = simulation_game_events_get_world();
        unsigned long reference_count = count;
        do
        {
            if (world->is_authority())
                *index = simulation_object_get_authoritative_entity_internal(*(long *)((char *)index + reference_offset), false);
            else
                *index = simulation_object_get_predictive_entity_internal(*(long *)((char *)index + reference_offset), false);
            index++;
            reference_count--;
        } while (reference_count != 0);
    }
}
