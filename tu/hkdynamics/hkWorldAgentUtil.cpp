// hkdynamics/hkWorldAgentUtil.obj: from the records 0x65a320 0x65a340 0x65a468 0x65a518 0x65a538 0x65a5f8 0x65a650 0x65a700 0x65a7f8
// @compiler xdk6130
// @flags /TP /O2 /Oc /GF /EHs-c- /GR-
#include <xtl.h>
// @address 0x65a320 ?get_any_non_fixed_island@@YAPAVphys_island@@PAV1@0@Z
// @address 0x65a468 ?island_from_agent_entry@phys_world_agent_util@@SAPAVphys_island@@PAUphys_pair_agent_entry@@PAV2@1@Z
// @address 0x65a518 ?get_agent_data@@YAPAXPAUphys_pair_agent_entry@@@Z
// @address 0x65a538 ?refilter_entity_compound@phys_world_agent_util@@SAXPAVphys_entity@@AAUphys_collision_input@@@Z
// @address 0x65a5f8 ?invalidate_sweep_info@phys_world_agent_util@@SAXPAVphys_entity@@AAUphys_collision_input@@@Z
// @address 0x65a650 ?warp_time@phys_world_agent_util@@SAXPAVphys_island@@MMAAUphys_collision_input@@@Z
// @address 0x65a700 ?remove_agent@phys_world_agent_util@@SAXPAUphys_pair_agent_entry@@@Z
// @address 0x65a7f8 ?remove_agent_and_toi_events@phys_world_agent_util@@SAXPAUphys_pair_agent_entry@@@Z
#include "phys/phys_dynamics.h"
#include "phys/phys_pair_agent_entry.h"
#include "phys/phys_pair_agent_sector.h"
#include "phys/phys_pair_agent_machine.h"
#include "phys/phys_collision_input.h"
#include "phys/phys_dispatcher.h"
#include "phys/phys_contact_manager_factory.h"
#include "phys/phys_island.h"
#include "phys/phys_simulation.h"
#include "phys/phys_world_agent_util.h"
#include "phys/phys_world_op_util.h"
#include "phys/phys_algorithm.h"

phys_island* get_any_non_fixed_island( phys_island* island_a, phys_island* island_b)
{
	if (!island_a->body_is_fixed())
	{
		return island_a;
	}
	if (!island_b->body_is_fixed())
	{
		return island_b;
	}
	return island_a;
}

// @address 0x65a340 ?add_pair_agent@phys_world_agent_util@@SAPAUphys_pair_agent_entry@@PAVphys_linked_collider@@0ABUphys_process_collision_input@@@Z
phys_pair_agent_entry* phys_world_agent_util::add_pair_agent( phys_linked_collider* collider_a, phys_linked_collider* collider_b,
	const phys_process_collision_input& input )
{

	phys_entity* listener_entity_a = static_cast<phys_entity*>( collider_a->get_collider_owner() );
	phys_entity* listener_entity_b = static_cast<phys_entity*>( collider_b->get_collider_owner() );

	phys_world_op_util::merge_islands_if_needed( listener_entity_a, listener_entity_b );

	phys_island* chosen_island = get_any_non_fixed_island(listener_entity_a->get_island(), listener_entity_b->get_island());

	int agent_type_id;
	int is_flipped;
	phys_pair_agent_get_type( collider_a, collider_b, input, agent_type_id, is_flipped );
	if ( is_flipped )
	{
		phys_algorithm::swap( collider_a, collider_b );
	}

	phys_contact_manager* pair_contact_manager;
	{
		phys_contact_manager_factory *factory = input.m_agent_dispatcher->get_contact_manager_factory(
			listener_entity_a->get_body_material().get_response_type(), listener_entity_b->get_body_material().get_response_type());
		pair_contact_manager = factory->create_contact_manager( *collider_a, *collider_b, input );
	}

	phys_pair_agent_track& track = chosen_island->m_agent_track;
	phys_pair_agent_entry* new_agent = phys_pair_agent_create( track, collider_a, collider_b, agent_type_id, input, pair_contact_manager );

	return new_agent;

}

void phys_world_agent_util::remove_agent( phys_pair_agent_entry* pair_agent_entry )
{

	phys_island* chosen_island;
	phys_entity* listener_entity_a = static_cast<phys_entity*>( pair_agent_entry->m_collider[0]->get_collider_owner() );
	phys_entity* listener_entity_b = static_cast<phys_entity*>( pair_agent_entry->m_collider[1]->get_collider_owner() );
	phys_island* island_a = listener_entity_a->get_island();
	phys_island* island_b = listener_entity_b->get_island();

	if (island_a == island_b)
	{
		chosen_island = island_a;
		chosen_island->m_split_check_requested = true;
	}
	else if (listener_entity_a->body_is_fixed())
	{
		
		chosen_island = island_b;
	}
	else if (listener_entity_b->body_is_fixed())
	{
		chosen_island = island_a;
	}
	else
	{
		
		chosen_island = island_from_agent_entry(pair_agent_entry, island_a, island_b);

		listener_entity_a->get_island()->m_split_check_requested = true;
		listener_entity_b->get_island()->m_split_check_requested = true;
	}

	phys_pair_agent_track& track = chosen_island->m_agent_track;
	phys_dispatcher* dispatch = chosen_island->get_action_world()->get_dispatcher();

	phys_contact_manager* pair_contact_manager = pair_agent_entry->m_contact_manager;
	phys_pair_agent_destroy( track, pair_agent_entry, dispatch, *chosen_island );
	pair_contact_manager->cleanup();

}

void phys_world_agent_util::remove_agent_and_toi_events ( phys_pair_agent_entry* pair_agent_entry )
{
	phys_simulation* simulation =
		static_cast<phys_entity*>( pair_agent_entry->m_collider[0]->get_collider_owner() )->get_island()->get_action_world()->m_world_simulation;
	simulation->remove_agent_collision_info( pair_agent_entry );

	phys_world_agent_util::remove_agent( pair_agent_entry );
}

phys_island* phys_world_agent_util::island_from_agent_entry( phys_pair_agent_entry* entry, phys_island* candidate_island_a, phys_island* candidate_island_b)
{
	
	phys_bool search_island_a = candidate_island_a->m_agent_track.m_track_sectors.getSize() <= candidate_island_b->m_agent_track.m_track_sectors.getSize();
	phys_island* island_to_search = search_island_a ? candidate_island_a : candidate_island_b;

	phys_bool sector_found = false;
	phys_array<phys_pair_agent_sector*>& sectors = island_to_search->m_agent_track.m_track_sectors;
	for (int i = 0; i < sectors.getSize(); i++)
	{
		phys_pair_agent_sector* sector = sectors[i];
		if (sector->get_track_begin() <= entry && entry < sector->get_track_end() )
		{
			sector_found = true;
			break;
		}
	}

	return (search_island_a ^ sector_found) ? candidate_island_b : candidate_island_a;
}

void* get_agent_data( phys_pair_agent_entry* entry)
{
	phys_narrow_agent::agent_stream_command command = phys_narrow_agent::agent_stream_command(entry->m_stream_command);
	if ( command == phys_narrow_agent::AGENT_STREAM_CALL_SWEEP)
	{
		return phys_add_byte_offset( entry, 48 );
	}
	else
	{
		return phys_add_byte_offset( entry, 32 );
	}
}

void phys_world_agent_util::refilter_entity_compound( phys_entity* entity, phys_collision_input& collision_input )
{
	phys_linked_collider* collidable = entity->get_linked_collider();
	for (int i = 0; i < collidable->m_collision_entries.getSize(); i++)
	{
		phys_pair_agent_entry* entry = collidable->m_collision_entries[i].m_agent_entry;

		phys_narrow_agent::agent_update_filter_fn func = collision_input.m_agent_dispatcher->get_narrow_agent_update_filter_fn(entry->m_agent_type);
		if (func)
		{
				
			phys_entity* listener_entity_a = static_cast<phys_entity*>(entry->m_collider[0]->get_collider_owner());
			phys_entity* listener_entity_b = static_cast<phys_entity*>(entry->m_collider[1]->get_collider_owner());
			phys_island* island = (listener_entity_a->body_is_fixed() )? listener_entity_b->get_island(): listener_entity_a->get_island();

			void* agent_data = get_agent_data(entry);
			func(entry, agent_data, *entry->m_collider[0], *entry->m_collider[1], collision_input, entry->m_contact_manager, *island);
		}
	}
}

void phys_world_agent_util::invalidate_sweep_info( phys_entity* entity, phys_collision_input& collision_input )
{
	phys_linked_collider* collidable = entity->get_linked_collider();
	for (int i = 0; i < collidable->m_collision_entries.getSize(); i++)
	{
		phys_pair_agent_entry* entry = collidable->m_collision_entries[i].m_agent_entry;
		phys_pair_agent_invalidate_sweep( entry, &collision_input );
	}
}

void phys_world_agent_util::warp_time(phys_island *island, float agent_old_time, float agent_new_time, phys_collision_input &collision_input)
{
	for (int track_sector_index = 0; track_sector_index < island->m_agent_track.m_track_sectors.getSize();) {
		phys_pair_agent_sector *sector = island->m_agent_track.m_track_sectors[track_sector_index];
		phys_pair_agent_entry *entry = sector->get_track_begin();
		track_sector_index++;
		phys_pair_agent_entry *end = track_sector_index == island->m_agent_track.m_track_sectors.getSize()
			? phys_add_byte_offset(entry, island->m_agent_track.m_last_sector_bytes) : sector->get_track_end();
		for (; entry < end; entry = phys_add_byte_offset(entry, entry->m_size)) {
			phys_pair_agent_warp_time(entry, agent_old_time, agent_new_time, &collision_input);
		}
	}
}
