// hkdynamics/hkBroadPhaseBorder.obj: from the records 0x64f578 0x64f5c0 0x64f608 0x64f698 0x64f6a0 0x64f710 0x64f7c0 0x64f848 0x64f888
// @compiler xdk6130
// @flags /TP /O2 /Oc /GF /EHs-c- /GR-
// @address 0x64f608 ??1phys_broadphase_border@@UAA@XZ
// @address 0x64f698 ?collider_removed_callback@phys_broadphase_border@@UAAXABUphys_collider_removed_event@@@Z
// @address 0x64f6a0 ?on_max_position_exceeded@phys_broadphase_border@@UAAXPAVphys_entity@@@Z
// @address 0x64f710 ?world_deleted_callback@phys_broadphase_border@@UAAXPAVphys_world@@@Z
// @address 0x64f7c0 ?deactivate@phys_broadphase_border@@UAAXXZ
// @address 0x64f848 ?collider_added_callback@phys_broadphase_border@@UAAXABUphys_collider_added_event@@@Z
// @address 0x64f888 ??0phys_broadphase_border@@QAA@PAVphys_world@@W4broadphase_border_mode@phys_world_desc@@@Z
// @address ??_Ephys_broadphase_border@@WM@AAPAXI@Z
// @address 0x64f578 ??_Gphys_volume_overlap_listener@@UAAPAXI@Z
// @address 0x64f5c0 ??_Gphys_world_deletion_listener@@UAAPAXI@Z
#include "physics/havok_memory.h"
#include "phys/phys_base.h"
#include "phys/phys_collide.h"
#include "phys/phys_dynamics.h"
#include "phys/phys_volume_overlap_listener.h"
#include "phys/phys_world_deletion_listener.h"


class phys_world;
class phys_entity;

#include "phys/phys_broadphase_border.h"


phys_broadphase_border::phys_broadphase_border( phys_world* world, phys_world_desc::broadphase_border_mode type )
{
	m_world = world;
	m_type = type;

	add_object_reference();
	m_world->add_world_delete_listener( this );

	phys_aabb info_aabb;

	const phys_vec4 min = m_world->m_broadphase_extents[0];
	const phys_vec4 max = m_world->m_broadphase_extents[1];
	{
		info_aabb.m_min_limit.set( max(0), min(1), min(2));
		info_aabb.m_max_limit.set( max(0), max(1), max(2));
		phys_box_volume* ph = new phys_box_volume( info_aabb );
		ph->get_collider_rw()->get_broadphase_handle()->m_type = phys_world_object::BROADPHASE_KIND_BORDER;
		ph->add_volume_overlap_listener( this );
		m_border_volumes[0] = ph;
		world->add_volume( ph );
	}
	{
		info_aabb.m_min_limit.set( min(0), min(1), min(2));
		info_aabb.m_max_limit.set( min(0), max(1), max(2));
		phys_box_volume* ph = new phys_box_volume( info_aabb );
		ph->get_collider_rw()->get_broadphase_handle()->m_type = phys_world_object::BROADPHASE_KIND_BORDER;
		ph->add_volume_overlap_listener( this );
		m_border_volumes[1] = ph;
		world->add_volume( ph );
	}

	{
		info_aabb.m_min_limit.set( min(0), max(1), min(2));
		info_aabb.m_max_limit.set( max(0), max(1), max(2));
		phys_box_volume* ph = new phys_box_volume( info_aabb );
		ph->get_collider_rw()->get_broadphase_handle()->m_type = phys_world_object::BROADPHASE_KIND_BORDER;
		ph->add_volume_overlap_listener( this );
		m_border_volumes[2] = ph;
		world->add_volume( ph );
	}
	{
		info_aabb.m_min_limit.set( min(0), min(1), min(2));
		info_aabb.m_max_limit.set( max(0), min(1), max(2));
		phys_box_volume* ph = new phys_box_volume( info_aabb );
		ph->get_collider_rw()->get_broadphase_handle()->m_type = phys_world_object::BROADPHASE_KIND_BORDER;
		ph->add_volume_overlap_listener( this );
		m_border_volumes[3] = ph;
		world->add_volume( ph );
	}

	{
		info_aabb.m_min_limit.set( min(0), min(1), max(2));
		info_aabb.m_max_limit.set( max(0), max(1), max(2));
		phys_box_volume* ph = new phys_box_volume( info_aabb );
		ph->get_collider_rw()->get_broadphase_handle()->m_type = phys_world_object::BROADPHASE_KIND_BORDER;
		ph->add_volume_overlap_listener( this );
		m_border_volumes[4] = ph;
		world->add_volume( ph );
	}

	{
		info_aabb.m_min_limit.set( min(0), min(1), min(2));
		info_aabb.m_max_limit.set( max(0), max(1), min(2));
		phys_box_volume* ph = new phys_box_volume( info_aabb );
		ph->get_collider_rw()->get_broadphase_handle()->m_type = phys_world_object::BROADPHASE_KIND_BORDER;
		ph->add_volume_overlap_listener( this );
		m_border_volumes[5] = ph;
		world->add_volume( ph );
	}
}

phys_broadphase_border::~phys_broadphase_border()
{
	for (int i = 0; i < 6; i++ )
	{
		if (m_border_volumes[i] != 0)
		{
			m_border_volumes[i]->object_remove_ref();
			m_border_volumes[i] = 0;
		}
	}
}

void phys_broadphase_border::collider_added_callback( const phys_collider_added_event& event )
{
	phys_rigid_body* body = phys_get_rigid_body( event.m_collider );
	if ( body )
	{
		on_max_position_exceeded( body );
	}
}

void phys_broadphase_border::collider_removed_callback( const phys_collider_removed_event& event )
{
}


void phys_broadphase_border::on_max_position_exceeded( phys_entity* entity )
{
	phys_rigid_body* body = static_cast<phys_rigid_body*>(entity);
	switch ( m_type )
	{
		case phys_world_desc::BROADPHASE_BORDER_ASSERT:
			body->set_motion_type( phys_motion::MOTION_FIXED, PHYS_ENTITY_ACTIVATION_DO_ACTIVATE, PHYS_UPDATE_FILTER_ON_ENTITY_FULL_CHECK );
			break;

		case phys_world_desc::BORDER_EVICT_ENTITY:
			m_world->remove_entity( entity );
			break;

		case phys_world_desc::BORDER_CLAMP_ENTITY:
			body->set_motion_type( phys_motion::MOTION_FIXED, PHYS_ENTITY_ACTIVATION_DO_ACTIVATE, PHYS_UPDATE_FILTER_ON_ENTITY_FULL_CHECK );
			break;

		default:
			break;
	}
}

void phys_broadphase_border::world_deleted_callback( phys_world* world )
{
	m_world->remove_world_deletion_listener( this );
	object_remove_ref();
}

void phys_broadphase_border::deactivate()
{
	if ( m_world )
	{
		m_world->remove_volume_batch( reinterpret_cast<phys_volume**>(&m_border_volumes[0]), 6 );
		for (int i =0; i < 6; i++ )
		{
			m_border_volumes[i]->remove_volume_overlap_listener( this );
			m_border_volumes[i]->object_remove_ref();
			m_border_volumes[i] = 0;
		}
		world_deleted_callback( m_world );
		m_world = 0;
	}
}

