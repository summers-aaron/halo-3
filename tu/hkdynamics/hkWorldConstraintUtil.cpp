// hkdynamics/hkWorldConstraintUtil.obj: from the records 0x65ac18 0x65ac88 0x65af00 0x65afa0 0x65b0b0 0x65b0f0 0x65b118 0x65b258 0x65b6d0
// @compiler xdk6130
// @flags /TP /O2 /Oc /GF /EHs-c- /GR-
// @address 0x65ac18 ?update_moved_atom_parent@phys_world_constraint_util@@SAXPAVphys_constraint@@PBUphys_atom@@1H@Z
// @address 0x65ac88 ?remove_constraint@phys_world_constraint_util@@SAXPAVphys_constraint@@@Z
// @address 0x65af00 ?get_entity_constraint@phys_world_constraint_util@@SAPAVphys_constraint@@PBVphys_entity@@0@Z
// @address 0x65afa0 ?remove_constraint_modifier@phys_world_constraint_util@@SAXPAVphys_constraint@@AAVphys_constraint_owner@@W4atom_kind@phys_atom@@@Z
// @address 0x65b0b0 ?find_modifier@phys_world_constraint_util@@SAPAUphys_modifier_atom@@PAVphys_constraint@@W4atom_kind@phys_atom@@@Z
// @address 0x65b0f0 ?find_last_modifier@phys_world_constraint_util@@SAPAUphys_modifier_atom@@PAVphys_constraint@@@Z
// @address 0x65b118 ?add_modifier@phys_world_constraint_util@@SAXPAVphys_constraint@@AAVphys_constraint_owner@@PAUphys_modifier_atom@@@Z
// @address 0x65b258 ?add_world_constraint@phys_world_constraint_util@@SAXPAVphys_world@@PAVphys_constraint@@@Z
// @address 0x65b6d0 ?insert_at@?$phys_small_array@Uphys_constraint_internal@@@@QAAXHPBUphys_constraint_internal@@H@Z
#include "phys/phys_world_constraint_util.h"
#include "phys/phys_dynamics.h"
#include "phys/phys_constraint_owner.h"
#include "phys/phys_island.h"
#include "phys/phys_world_op_util.h"
void phys_world_constraint_util::add_world_constraint( phys_world* world, phys_constraint* constraint )
{


	constraint->add_object_reference();

	phys_constraint_data::constraint_info info;	constraint->get_chain_data()->get_constraint_info(info);

	phys_constraint_internal internal_storage;
	{
		internal_storage.m_contact_constraint  = constraint;
		internal_storage.m_island_entities[0] = constraint->m_island_entities[0];
		internal_storage.m_island_entities[1] = constraint->m_island_entities[1];
		phys_modifier_atom* modifier_atom = constraint->m_constraint_modifiers;
		internal_storage.m_data_atoms       = (modifier_atom) ? modifier_atom             : info.m_data_atoms;
		internal_storage.m_atoms_size   = (modifier_atom) ? modifier_atom->m_modifier_atom_size : static_cast<unsigned short>(info.m_all_atoms_size);
		internal_storage.m_priority    = constraint->m_priority;
	}

	phys_constraint_internal* internal_ref = &internal_storage;
	phys_world_op_util::merge_islands_if_needed(internal_ref->m_island_entities[0], internal_ref->m_island_entities[1]);

	{
		
		{
			
			internal_ref->m_master_body = internal_ref->m_island_entities[0]->body_is_fixed();
		}
	}
	phys_entity* master_entity = internal_ref->get_master_entity();

	phys_island* island = master_entity->get_island();

	constraint->set_collider_owner( island );
	island->m_constraint_count++;

	{
		phys_atom::atom_kind constraint_atom_type = internal_ref->get_atoms()->get_type_id();

		internal_ref->m_normal_type    = true;
		internal_ref->m_callback_request = phys_atom::ATOM_CALLBACK_NONE;

		if ( constraint_atom_type == phys_atom::ATOM_BRIDGE )
		{
			internal_ref->m_callback_request |= phys_atom::ATOM_CALLBACK_SETUP_HOST_ONLY;

			if (constraint->get_chain_data()->needs_jacobian_callback())
			{
				internal_ref->m_callback_request |= phys_atom::ATOM_CALLBACK_SETUP;
			}

			internal_ref->m_normal_type = constraint->get_type_id() == phys_constraint::CONSTRAINT_KIND_NORMAL;
		}

		if ( constraint_atom_type == phys_atom::ATOM_PULLEY )
		{
			internal_ref->m_callback_request |= phys_atom::ATOM_CALLBACK_SETUP_HOST_ONLY;
		}

		if ( constraint->m_constraint_modifiers)
		{
			internal_ref->m_callback_request |= phys_modifier_atom::add_all_modifier_info( constraint->m_constraint_modifiers, info );
		}
		internal_ref->clear_constraint_info();
		constraint->m_constraint_internal = internal_ref;	
		constraint->get_collider_owner()->add_constraint_info(constraint, info);
	}

	int insert_slot_idx;
	{
		phys_small_array<phys_constraint_internal>& master_constraints = master_entity->m_master_constraints;
		int master_cnt = master_constraints.getSize();
		for (insert_slot_idx = 0; insert_slot_idx < master_cnt; insert_slot_idx++)
		{
			phys_constraint_internal& master_slot = master_constraints[insert_slot_idx];
			
			if (master_slot.m_priority > internal_ref->m_priority)
			{
				break;
			}

			else if (master_slot.m_priority == internal_ref->m_priority)
			{
				
				const unsigned int master_uid = master_slot.m_island_entities[0]->get_uid();
				const unsigned int    new_uid = internal_ref->m_island_entities[0]->get_uid();

				if ( master_uid > new_uid  || ( master_uid == new_uid  &&
					master_slot.m_island_entities[1]->get_uid() > internal_ref->m_island_entities[1]->get_uid() ) )
				{
					break;
				}
			}
		}

		int relink_from_idx = ( master_constraints.getSize() < master_constraints.get_sector_capacity() ) ? insert_slot_idx : 0;

		master_constraints.insert_at( insert_slot_idx, *internal_ref );

		for ( int m = relink_from_idx; m < master_constraints.getSize(); m++ )
		{
			phys_constraint_internal* ci = &master_constraints[m];
			ci->m_contact_constraint->m_constraint_internal = ci;
		}

		internal_ref = constraint->m_constraint_internal;
	}

	{
		phys_small_array<phys_constraint_internal>& master_constraints = master_entity->m_master_constraints;
		phys_constraint_data::runtime_info constraint_runtime_info;
		internal_ref->m_contact_constraint->get_chain_data()->get_runtime_info( constraint->m_want_runtime, constraint_runtime_info );
		constraint_runtime_info.m_external_runtime_size = (((constraint_runtime_info.m_external_runtime_size) + 15) & ~15);
		internal_ref->m_runtime_size = (unsigned short)( constraint_runtime_info.m_external_runtime_size );
		
		if ( constraint_runtime_info.m_external_runtime_size )
		{
			phys_array<unsigned char>& runtime_bytes = master_entity->m_constraint_runtime;
			int array_old_size = runtime_bytes.getSize();
			unsigned char* old_runtime_begin = runtime_bytes.begin();
			runtime_bytes.reserve_exactly( array_old_size + constraint_runtime_info.m_external_runtime_size );
			runtime_bytes.set_size_unchecked( array_old_size + constraint_runtime_info.m_external_runtime_size );
			unsigned char* new_runtime_begin = runtime_bytes.begin();
			unsigned char* insert_ptr = new_runtime_begin;
			phys_ulong offset = phys_get_byte_offset( old_runtime_begin, new_runtime_begin );

			int p;
			for (p = 0; p < insert_slot_idx; p++)
			{
				phys_constraint_internal* ci = &master_constraints[p];
				if ( ci->m_solver_runtime )
				{
					ci->m_solver_runtime = phys_add_byte_offset( ci->m_solver_runtime, offset );
					insert_ptr = (unsigned char*)phys_add_byte_offset( ci->m_solver_runtime, ci->m_runtime_size );
				}
			}
			
			{
				unsigned char* rest_end = runtime_bytes.end() - internal_ref->m_runtime_size;
				phys_ulong rest_byte_size = phys_get_byte_offset( insert_ptr, rest_end );
				phys_string::mem_move( insert_ptr + internal_ref->m_runtime_size, insert_ptr, int(rest_byte_size) );
				internal_ref->m_solver_runtime = reinterpret_cast<phys_constraint_runtime*>(insert_ptr);
				p++;
			}

			offset += internal_ref->m_runtime_size;
			for (; p < master_constraints.getSize(); p++)
			{
				phys_constraint_internal* ci = &master_constraints[p];
				ci->m_solver_runtime = (ci->m_solver_runtime) ? phys_add_byte_offset( ci->m_solver_runtime, offset ): 0;
			}
		}
		else
		{
			internal_ref->m_solver_runtime = 0;
		}
		internal_ref->m_contact_constraint->m_data->add_constraint_instance( constraint, internal_ref->m_solver_runtime, internal_ref->m_runtime_size );
	}

	phys_entity* slave_entity = internal_ref->get_slave_entity();
	{
		internal_ref->m_slave_idx = static_cast<unsigned short>(slave_entity->m_slave_constraints.getSize() );
		slave_entity->m_slave_constraints.array_push_back( constraint );
	}
}

void phys_world_constraint_util::remove_constraint( phys_constraint* constraint )
{


	phys_constraint_internal* internal_ref = constraint->m_constraint_internal;

	phys_island* island = static_cast<phys_island*>(constraint->get_collider_owner());
	{
		island->m_split_check_requested = true;
	}

	{
		phys_constraint_info info;
		internal_ref->get_constraint_info(info);
		constraint->get_collider_owner()->sub_constraint_info(constraint, info);
	}
	island->m_constraint_count--;
	constraint->set_collider_owner( 0 );

	{
		phys_entity* slave_entity = internal_ref->get_slave_entity();
		unsigned short slave_idx = internal_ref->m_slave_idx;

		phys_array<phys_constraint*>& slave_constraints = slave_entity->m_slave_constraints;
		phys_constraint* last_constraint = slave_constraints.back();
			
		slave_constraints[slave_idx] = last_constraint;
		slave_constraints.drop_back();
		last_constraint->m_constraint_internal->m_slave_idx = slave_idx;
	}

	phys_entity* master_entity = internal_ref->get_master_entity();

	internal_ref->m_contact_constraint->get_chain_data()->remove_constraint_instance( constraint, internal_ref->m_solver_runtime,
		internal_ref->m_runtime_size );
	phys_ulong runtime_byte_offset = 0;
	if ( internal_ref->m_solver_runtime )
	{
		phys_constraint_internal* ci = internal_ref;
		runtime_byte_offset = (phys_ulong)(-(ptrdiff_t)ci->m_runtime_size);
		phys_constraint_runtime* delete_begin = ci->m_solver_runtime;
		phys_constraint_runtime* delete_end   = phys_add_byte_offset( ci->m_solver_runtime, ci->m_runtime_size );
		unsigned char* runtime_end      = master_entity->m_constraint_runtime.end();
		phys_ulong rest_byte_len = phys_get_byte_offset( delete_end, runtime_end );
		phys_string::mem_move( delete_begin, delete_end,  int(rest_byte_len) );
		master_entity->m_constraint_runtime.drop_back( ci->m_runtime_size );
		ci->m_solver_runtime = 0; 
	}

	{
		phys_atom* atom = internal_ref->m_data_atoms;
		while ( 1 )
		{
			if ( !atom->is_modifier_type() )
			{
				break;
			}
			phys_modifier_atom* modifier = reinterpret_cast<phys_modifier_atom*>(atom);
			atom = modifier->m_child_shape;
			phys_thread_memory::get_singleton_instance().free_chunk(modifier, modifier->m_modifier_atom_size, PHYS_MEM_CONSTRAINT );
		}
		internal_ref->m_contact_constraint->m_constraint_modifiers = 0;
		internal_ref->m_data_atoms = 0;
	}

	{
		phys_constraint_internal* ci = internal_ref;

		constraint->m_constraint_internal = 0;

		phys_constraint_internal* back = &master_entity->m_master_constraints.back();

		while( ci < back )
		{
			ci[0] = ci[1];
			ci->m_contact_constraint->m_constraint_internal = ci;
			ci->m_solver_runtime = (ci->m_solver_runtime) ? phys_add_byte_offset(ci->m_solver_runtime, runtime_byte_offset): 0;
			ci++;
		}
		master_entity->m_master_constraints.drop_back();
	}

	constraint->object_remove_ref();
}

phys_constraint* phys_world_constraint_util::get_entity_constraint( const phys_entity* listener_entity_a, const phys_entity* listener_entity_b)
{

	const phys_entity* entities[] = { listener_entity_a, listener_entity_b };

	for (int i = 0; i < 2; i++)
	{
		const phys_entity* master_entity = entities[i];
		const phys_entity* other_entity  = entities[1-i];

		for (int c = 0; c < master_entity->m_master_constraints.getSize(); c++)
		{
			if (master_entity->m_master_constraints[c].get_other_entity(master_entity) == other_entity)
			{
				
				return master_entity->m_master_constraints[c].m_contact_constraint;
			}
		}
	}
	return 0;
}

void phys_world_constraint_util::add_modifier( phys_constraint* instance, phys_constraint_owner& constraint_owner_ref, phys_modifier_atom* new_modifier )
{
	constraint_owner_ref.check_access_rw();

	phys_modifier_atom* last_modifier = phys_world_constraint_util::find_last_modifier( instance );
	phys_constraint_internal* constraint_internal = instance->m_constraint_internal;

	if ( new_modifier->get_type_id() == phys_atom::ATOM_MOD_MOVING_SURFACE && last_modifier != 0 )
	{
		
		new_modifier->m_child_shape     = last_modifier->m_child_shape;
		new_modifier->m_child_size = last_modifier->m_child_size;

		last_modifier->m_child_shape     =  new_modifier;
		last_modifier->m_child_size =  new_modifier->m_modifier_atom_size;
	}
	else
	{
		
		instance->m_constraint_modifiers = new_modifier;

		if ( constraint_internal )
		{
			new_modifier->m_child_shape     = constraint_internal->m_data_atoms;
			new_modifier->m_child_size = constraint_internal->m_atoms_size;
			constraint_internal->m_data_atoms       =  new_modifier;
			constraint_internal->m_atoms_size   =  new_modifier->m_modifier_atom_size;
		}
		else
		{
			phys_constraint_data::constraint_info cinfo; instance->get_chain_data()->get_constraint_info( cinfo );
			new_modifier->m_child_shape     = cinfo.m_data_atoms;
			new_modifier->m_child_size = static_cast<unsigned short>(cinfo.m_all_atoms_size);
		}
	}

	if ( constraint_internal )
	{
		phys_constraint_info cinfo;		cinfo.clear();

		int constraint_callback_request = new_modifier->add_modifier_info( cinfo );
		constraint_owner_ref.add_constraint_info( instance, cinfo );

		if ( instance->m_constraint_internal )
		{
			instance->m_constraint_internal->m_callback_request |= constraint_callback_request;
		}
	}
}

void phys_world_constraint_util::remove_constraint_modifier( phys_constraint* instance, phys_constraint_owner& constraint_owner_ref, phys_atom::atom_kind type )
{
	constraint_owner_ref.check_access_rw();

	phys_modifier_atom* modifier = instance->m_constraint_modifiers;

	if ( !modifier )
	{
		return;
	}
	phys_constraint_internal* constraint_internal = instance->m_constraint_internal;

	phys_atom::atom_kind modifier_type = modifier->get_type_id();
	if ( modifier_type == type)
	{
		phys_atom* child = modifier->m_child_shape;
		if ( constraint_internal )
		{
			constraint_internal->m_data_atoms     = modifier->m_child_shape;
			constraint_internal->m_atoms_size = modifier->m_child_size;
		}

		if ( child->is_modifier_type() )
		{
			instance->m_constraint_modifiers = static_cast<phys_modifier_atom*>(child);
		}
		else
		{
			instance->m_constraint_modifiers = 0;
		}
	}
	else
	{
		phys_modifier_atom* parent_modifier = modifier;
		phys_atom* atom = modifier->m_child_shape;
	
		while ( 1 )
		{
			
			if ( !atom->is_modifier_type() )
			{
				return;
			}
			modifier = static_cast<phys_modifier_atom*>(atom);
	
			phys_atom::atom_kind modifier_type = modifier->get_type_id();
	
			if ( modifier_type == type )
			{
				parent_modifier->m_child_shape     = modifier->m_child_shape;
				parent_modifier->m_child_size = modifier->m_child_size;
				break;
			}
			parent_modifier  = modifier;
			atom   = modifier->m_child_shape;
		}
	}

	
	if (constraint_internal)
	{
		phys_constraint_info cinfo;	cinfo.clear();
		modifier->add_modifier_info( cinfo );
		constraint_owner_ref.sub_constraint_info( constraint_internal->m_contact_constraint, cinfo );
	}
	phys_thread_memory::get_singleton_instance().free_chunk(modifier, modifier->m_modifier_atom_size, PHYS_MEM_CONSTRAINT );
}

phys_modifier_atom* phys_world_constraint_util::find_modifier( phys_constraint* instance, phys_atom::atom_kind type )
{

	phys_modifier_atom* atom = instance->m_constraint_modifiers;
	if (!atom)
	{
		return 0;
	}

	while ( 1 )
	{
		
		phys_atom::atom_kind modifier_type = atom->get_type_id();

		if ( modifier_type == type )
		{
			return atom;
		}
		if ( !atom->m_child_shape->is_modifier_type() )
		{
			return 0;
		}

		atom = static_cast<phys_modifier_atom*>(atom->m_child_shape);
	}

}

phys_modifier_atom* phys_world_constraint_util::find_last_modifier( phys_constraint* instance )
{

	phys_modifier_atom* atom = instance->m_constraint_modifiers;

	if ( !atom )
	{
		return 0;
	}

	while ( 1 )
	{
		phys_atom* child_atom     = atom->m_child_shape;

		if ( !child_atom->is_modifier_type() )
		{
			return atom;
		}

		atom = static_cast<phys_modifier_atom*>(child_atom);
	}
}



void phys_world_constraint_util::update_moved_atom_parent(
	phys_constraint *constraint, const phys_atom *old_atom, const phys_atom *new_atom, int size)
{
	phys_modifier_atom* first = constraint->m_constraint_modifiers;
	phys_constraint_internal* parent_modifier = constraint->m_constraint_internal;
	if (first == 0) {
		if (parent_modifier != 0) {
			parent_modifier->m_data_atoms = const_cast<phys_atom*>(new_atom);
			parent_modifier->m_atoms_size = (unsigned short)size;
		}
		return;
	}
	if (first == (const phys_modifier_atom*)old_atom) {
		constraint->m_constraint_modifiers = (phys_modifier_atom*)const_cast<phys_atom*>(new_atom);
		if (parent_modifier != 0) {
			parent_modifier->m_data_atoms = const_cast<phys_atom*>(new_atom);
			parent_modifier->m_atoms_size = (unsigned short)size;
		}
		return;
	}
	phys_modifier_atom* prev = first;
	phys_atom* child = first->m_child_shape;
	while (child != old_atom) {
		prev = (phys_modifier_atom*)child;
		child = ((phys_modifier_atom*)child)->m_child_shape;
	}
	prev->m_child_shape = const_cast<phys_atom*>(new_atom);
	prev->m_child_size = (unsigned short)size;
}
