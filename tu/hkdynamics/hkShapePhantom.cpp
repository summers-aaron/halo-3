// hkdynamics/hkShapePhantom.obj: from the records 0x64b1e0 0x64b1e8 0x64b1f0 0x64b350 0x64b3f0 0x64b470 0x64b4a8
// @compiler xdk6130
// @flags /TP /O2 /Oc /GF /EHs-c- /GR-
#include <xtl.h>
// @address 0x64b1e0 ?get_motion_state@phys_shape_volume@@MAAPAVphys_motion_state@@XZ
// @address 0x64b1e8 ?free_internal_arrays@phys_shape_volume@@UAAXXZ
// @address 0x64b1f0 ?set_proxy_shape@phys_shape_volume@@UAA?AW4Result@phys_world_op@@PBVphys_shape@@@Z
// @address 0x64b350 ?set_transform@phys_shape_volume@@QAAXABVphys_transform@@@Z
// @address 0x64b3f0 ?contact_set_position@phys_shape_volume@@QAAXABVphys_vec4@@M@Z
// @address 0x64b470 ?compute_bounds@phys_shape_volume@@UAAXAAVphys_aabb@@@Z
// @address 0x64b4a8 ??0phys_shape_volume@@QAA@PBVphys_shape@@ABVphys_transform@@@Z
#include "physics/havok_memory.h"
#include "phys/phys_dynamics.h"
#include "phys/phys_aabb.h"
#include "phys/phys_collision_input.h"
#include "phys/phys_world_op_util.h"
#include "phys/phys_world_op_queue.h"

#include "phys/phys_world_callback_util.h"
phys_shape_volume::phys_shape_volume( const phys_shape* shape, const phys_transform& transform )
	: phys_volume( shape )
{
	
	phys_collider* collidable = get_collider_rw();
	
	collidable->set_motion_state( &m_motion_state );

	m_motion_state.get_transform() = transform;
}

phys_motion_state* phys_shape_volume::get_motion_state()
{
	return &m_motion_state;
}

void phys_shape_volume::set_transform( const phys_transform& transform )
{
	
	m_motion_state.get_transform() = transform;

	if ( m_world != 0 )
	{
		phys_aabb aabb;
		phys_real half_tolerance = 0.5f * m_world->get_collision_input()->get_tolerance();
		m_collider.get_shape()->get_handle_aabb( transform, half_tolerance, aabb );
		update_broadphase( aabb );
	}
}

void phys_shape_volume::contact_set_position( const phys_vec4& position, phys_real extra_tolerance )
{
	m_motion_state.get_transform().set_translation( position );

	if (m_world != 0)
	{
		phys_aabb aabb;
		phys_real half_tolerance = 0.5f * m_world->get_collision_input()->get_tolerance();
		m_collider.get_shape()->get_handle_aabb( m_motion_state.get_transform(), half_tolerance + extra_tolerance, aabb );
		update_broadphase( aabb );
	}
}

void phys_shape_volume::compute_bounds( phys_aabb& aabb )
{
	phys_real half_tolerance = 0.5f * m_world->get_collision_input()->get_tolerance();
	m_collider.get_shape()->get_handle_aabb( m_motion_state.get_transform(), half_tolerance, aabb );
}

void phys_shape_volume::free_internal_arrays()
{
	
	phys_volume::free_internal_arrays();
}

phys_world_op::Result phys_shape_volume::set_proxy_shape( const phys_shape* shape )
{

	if (m_world && m_world->critical_ops_locked())
	{
		phys_world_op::set_object_shape_op op;
		op.m_world_object = this;
		op.m_proxy_shape = shape;

		m_world->queue_world_op(op);
		return phys_world_op::OP_POSTPONED;
	}


	if (m_world)
	{
		m_world->lock_critical_ops();
		phys_world_op_util::remove_volume_from_broadphase(m_world, this);
	}

	{
		
		shape->add_object_reference();
		if (get_collider()->get_shape())
		{
			get_collider()->get_shape()->object_remove_ref();
		}
		get_collider_rw()->set_proxy_shape(shape);
		
	}

	if (m_world)
	{
		phys_world_callback_util::notify_volume_shape_set( m_world, this );
	}
	this->notify_volume_shape_set();

	if (m_world)
	{
		phys_world_op_util::add_volume_broadphase(m_world, this);
		m_world->unlock_and_flush_ops();
	}

	return phys_world_op::STEP_RESULT_DONE;
}

