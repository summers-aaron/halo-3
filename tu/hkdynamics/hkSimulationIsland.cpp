// hkdynamics/hkSimulationIsland.obj: from the records 0x659618 0x659620 0x659630 0x659640 0x659658 0x659660 0x6596c8 0x659728 0x659928 0x659978 0x6599f8 0x659d58 0x659db8 0x659e70 0x65a1b0 0x65a2b8
// @compiler xdk6130
// @flags /TP /O2 /Oc /GF /EHs-c- /GR-
#include <xtl.h>
// @address 0x659618 ?is_valid_aabb@phys_island@@QAAXXZ
// @address 0x659620 ?add_constraint_to_locked_island@phys_island@@MAAXPAVphys_constraint@@@Z
// @address 0x659630 ?remove_constraint_locked_island@phys_island@@MAAXPAVphys_constraint@@@Z
// @address 0x659640 ?add_callback_request@phys_island@@MAAXPAVphys_constraint@@H@Z
// @address 0x659658 ?mark_for_write@phys_island@@QAAXXZ
// @address 0x659660 ?remove_entity_internal@phys_island@@IAAXPAVphys_entity@@@Z
// @address 0x6596c8 ?remove_action@phys_island@@IAAXPAVphys_action@@@Z
// @address 0x659928 ?merge_constraint_info@phys_island@@QAAXAAV1@@Z
// @address 0x659978 ?add_entity_internal@phys_island@@IAAXPAVphys_entity@@@Z
// @address 0x6599f8 ?gather_memory_stats@phys_island@@UBAXPAVphys_statistics_collector@@@Z
// @address 0x659d58 ?add_world_action@phys_island@@IAAXPAVphys_action@@@Z
// @address 0x659db8 ??1phys_island@@MAA@XZ
// @address 0x659e70 ?is_fully_connected@phys_island@@IAA?AVphys_bool@@AAVphys_union_find@@@Z
// @address 0x65a1b0 ??0phys_island@@IAA@PAVphys_world@@@Z
// @address 0x65a2b8 ??_Gphys_island@@MAAPAXI@Z
#include "phys/phys_island.h"
#include "phys/phys_constraint.h"
#include "phys/phys_constraint_data.h"
#include "phys/phys_statistics_collector.h"
#include "phys/phys_union_find.h"
#include "phys/phys_world_op_util.h"
#include "phys/phys_agent_machine_util.h"

#include "phys/phys_action.h"
#include "phys/phys_null_action.h"

phys_island::phys_island( phys_world* world ) 
:	m_world( world ),
	m_dirty_list_idx(0xffff),
	m_hf_sleep_counter(0),
	m_low_freq_sleep_counter(0),
	m_split_check_requested(false),
	m_action_cleanup_needed (false),
	m_in_active_islands(true),
	m_active(true),
	m_in_integrate_job(false),
	m_since_high_freq_check(0),
	m_since_low_freq_check(0),
	m_sleep_time(-10.0f) 
{
	m_allow_island_locking = false;
	m_constraint_info.clear();
	m_constraint_count = 0;
	m_sparse_enabled = false;
}

phys_island::~phys_island()
{
	for (int i = 0; i < m_actions.getSize(); i++)
	{
	}
}

void phys_island::add_entity_internal(phys_entity* entity)
{

	entity->m_owner_island = this;
	entity->m_storage_idx = (unsigned short)m_island_entities.getSize();
	m_island_entities.array_push_back(entity);
}

void phys_island::remove_entity_internal(phys_entity* entity)
{

	{

		m_island_entities[entity->m_storage_idx] = m_island_entities[m_island_entities.getSize() - 1];
		m_island_entities[entity->m_storage_idx]->m_storage_idx = entity->m_storage_idx;
		m_island_entities.drop_back();
	}

	entity->m_owner_island = 0;
	entity->m_storage_idx = 0xffff;

	m_split_check_requested = true;
}

void phys_island::gather_memory_stats( phys_statistics_collector* collector ) const
{
	collector->begin_stats_object( 0, collector->stat_category_runtime,  this );
	
	{
		collector->add_array_stats( "EntityPtrs", collector->stat_category_runtime, m_island_entities );
		for ( int i = 0; i < m_island_entities.getSize(); ++i )
		{
			phys_entity* entity = m_island_entities[i];
			collector->add_child_object( "Entity", collector->stat_category_instance, entity);

			{
				collector->push_stats_dir( "Constraints" );
				collector->add_small_array_stats("ConMstPtr", collector->stat_category_runtime, entity->m_master_constraints);
				collector->add_array_stats("ConSlvPtr", collector->stat_category_runtime, entity->m_slave_constraints);
				for ( int j = 0; j < entity->m_master_constraints.getSize(); ++j )
				{
					const phys_constraint* instance = entity->m_master_constraints[j].m_contact_constraint;
					collector->add_child_object( "ConInstance", collector->stat_category_instance, instance );	

					const phys_constraint_data* data = entity->m_master_constraints[j].m_contact_constraint->get_chain_data();
					collector->add_child_object( "ConData", collector->stat_category_shared, data );
				}
				collector->add_array_stats( "Runtime",	 collector->stat_category_engine, entity->m_constraint_runtime );
				collector->pop_dir(  );
			}
		}
	}

	{
		collector->add_array_stats( "ActionPtrs", collector->stat_category_runtime, m_actions );
		for ( int i = 0; i < m_actions.getSize(); ++i )
		{
			collector->add_child_object( "Actions", collector->stat_category_instance, m_actions[i] );
		}
	}

	phys_agent_machine_util::gather_contact_manager_stats(m_agent_track, collector);

	collector->push_stats_dir( "CollAgents" );
		phys_agent_machine_util::gather_pair_agent_stats(m_agent_track, collector);
	collector->pop_dir();

	collector->end_object();
}

static inline phys_real closest_dist_of( phys_process_collision_output& result )
{
	phys_real closest_dist = 3.40282e+38f;

	for (int i = 0; i < result.get_process_contact_count(); ++i )
	{
		if ( result.m_process_contact_points[i].m_hit_contact.get_distance() < closest_dist )
		{
			closest_dist = result.m_process_contact_points[i].m_hit_contact.get_distance();
		}
	}

	return closest_dist;

}

void phys_island::add_world_action( phys_action* act )
{

	m_actions.array_push_back( act );
	act->set_island(this);
}

void phys_island::remove_action( phys_action* act )
{
	int action_idx = m_actions.array_index_of( act );
	

	m_actions[action_idx] = phys_null_action::get_null_action();
	
	act->set_island(0);

	m_split_check_requested = true;	
	m_action_cleanup_needed = true;
}

phys_bool phys_island::is_fully_connected( phys_union_find& connectivity_out )
{	

	{
		for (int e = 0; e < m_island_entities.getSize(); e++)
		{
			phys_linked_collider* collidable = &m_island_entities[e]->m_collider;
			for (int i = 0; i < collidable->m_collision_entries.getSize(); i++)
			{
				phys_linked_collider* partner_collider = collidable->m_collision_entries[i].m_linked_partner;

				phys_entity* partner_entity = static_cast<phys_entity*>(partner_collider->get_collider_owner());
				if (!partner_entity->body_is_fixed())
				{
					int idx = partner_entity->m_storage_idx;

					connectivity_out.add_union_edge( e, idx );

					if ( connectivity_out.is_one_group() )
					{
						return true;
					}

				}
			}
		}
	}
	
	{
		for ( int e = 0; e < m_island_entities.getSize(); e++)
		{
			phys_entity* entity = m_island_entities[e];
			for ( int i = 0; i < entity->m_master_constraints.getSize(); ++i )
			{
				phys_constraint_internal* constraint_internal = &entity->m_master_constraints[i];
				if ( !constraint_internal->m_island_entities[0]->body_is_fixed() && !constraint_internal->m_island_entities[1]->body_is_fixed())
				{
					int a = constraint_internal->m_island_entities[0]->m_storage_idx;
					int b = constraint_internal->m_island_entities[1]->m_storage_idx;
					connectivity_out.add_union_edge( a, b );
					if ( connectivity_out.is_one_group() )
					{
						return true;
					}
				}
			}
		}
	}	

	{
		phys_inline_array<phys_entity*,10> action_entities;
		for ( int i = 0; i < m_actions.getSize(); ++i )
		{
			if (m_actions[i] != phys_null_action::get_null_action())
			{
				action_entities.clear();
				m_actions[i]->get_entities( action_entities );

				int j = 0;
				int first_unfixed_idx = -1;
				while ( (first_unfixed_idx == -1) && (j < action_entities.getSize()) )
				{
					if (!action_entities[j]->body_is_fixed())
					{
						first_unfixed_idx = j;
					}
					j++;
				}
	
				for ( ; j < action_entities.getSize(); ++j )
				{
					if (!action_entities[j]->body_is_fixed())
					{
						int a = action_entities[first_unfixed_idx]->m_storage_idx;
						int b = action_entities[j]->m_storage_idx;
	
						connectivity_out.add_union_edge( a, b );
						if ( connectivity_out.is_one_group() )
						{
							return true;
						}
					}
				}
			}
		}
	}
	
	return connectivity_out.is_one_group();
}

__forceinline phys_bool phys_island_is_same( phys_island*island_a, phys_island*island_b )
{
	if ( island_a == island_b )
	{
		return true;
	}

	if ( island_a->body_is_fixed())
	{
		return true;
	}
	if ( island_b->body_is_fixed())
	{
		return true;
	}

	return false;
}

void phys_island::is_valid_aabb()
{
}

void phys_island::add_constraint_to_locked_island( phys_constraint* constraint )
{
	phys_world_op_util::add_constraint_to_locked_island( constraint->get_entity_a()->get_action_world(), constraint );
}

void phys_island::remove_constraint_locked_island( phys_constraint* constraint )
{
	phys_world_op_util::remove_constraint_locked_island( constraint->get_entity_a()->get_action_world(), constraint );
}

void phys_island::add_callback_request( phys_constraint* constraint, int request )
{
	constraint->m_constraint_internal->m_callback_request |= request;
}

void phys_island::merge_constraint_info( phys_island& other )
{
	m_constraint_info.merge( other.m_constraint_info );
}

void phys_island::mark_for_write( )
{
}

// @address 0x659728 ?should_sleep_legacy@phys_island@@QAA?AVphys_bool@@ABVphys_step_info@@@Z
phys_bool phys_island::should_sleep_legacy(const phys_step_info &step_info)
{
	m_since_high_freq_check += step_info.m_delta_time;
	m_since_low_freq_check += step_info.m_delta_time;
	const phys_bool high_freq_check = m_since_high_freq_check > m_world->get_high_freq_sleep_period();
	const phys_bool low_freq_check = m_since_low_freq_check > m_world->get_low_freq_sleep_period();
	if (high_freq_check) {
		m_since_high_freq_check -= m_world->get_high_freq_sleep_period();
		phys_bool deactivate = true;
		for (int i = 0; i < m_island_entities.getSize(); ++i) {
			phys_entity *entity = m_island_entities[i];
			if (entity->get_deactivator() == 0 || !entity->get_deactivator()->should_sleep_high_freq(entity)) {
				deactivate = false;
				continue;
			}
		}
		if (deactivate) {
			if (++m_hf_sleep_counter >= 5) {
				m_hf_sleep_counter = 0;
				return true;
			}
		} else {
			m_hf_sleep_counter = 0;
		}
	} else if (low_freq_check) {
		m_since_low_freq_check -= m_world->get_low_freq_sleep_period();
		phys_bool deactivate = true;
		for (int i = 0; i < m_island_entities.getSize(); ++i) {
			if (m_island_entities[i]->get_deactivator() == 0 || !m_island_entities[i]->get_deactivator()->should_sleep_low_freq(m_island_entities[i])) {
				deactivate = false;
				continue;
			}
		}
		if (deactivate) {
			if (++m_low_freq_sleep_counter >= 5) {
				m_low_freq_sleep_counter = 0;
				return true;
			}
		} else {
			m_low_freq_sleep_counter = 0;
		}
	}
	return false;
}
