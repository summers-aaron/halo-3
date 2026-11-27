// hkdynamics/hkSimpleShapePhantom.obj: from the records 0x64a6b8 0x64a6d0 0x64a6d8 0x64a720 0x64a7e8 0x64a9c8 0x64aa68 0x64ab18 0x64aba0 0x64ac58 0x64ac70 0x64aca0 0x64ad00 0x64ad78 0x64af40 0x64afc0 0x64b038 0x64b0a8 0x64b110 0x64b178
// @compiler xdk6130
// @flags /TP /O2 /Oc /GF /EHs-c- /GR-
// @address 0x64a6b8 ?destroy_loaded_simple_shape_volume@@YAXPAX@Z
// @address 0x64a6d0 ?get_type_id@phys_simple_shape_volume@@UBA?AW4phys_volume_type@@XZ
// @address 0x64a6d8 ?has_overlapping_collider@phys_simple_shape_volume@@UAA?AVphys_bool@@PAVphys_collider@@@Z
// @address 0x64a720 ?erase_overlapping_collider@phys_simple_shape_volume@@UAAXPAVphys_collider@@@Z
// @address 0x64a7e8 ?set_position_and_sweep_cast@phys_simple_shape_volume@@UAAXABVphys_vec4@@ABUphys_linear_cast_input@@AAVphys_hit_collector@@PAV4@@Z
// @address 0x64ab18 ?insert_overlapping_collider@phys_simple_shape_volume@@UAAXPAVphys_collider@@@Z
// @address 0x64aba0 ?gather_memory_stats@phys_simple_shape_volume@@UBAXPAVphys_statistics_collector@@@Z
// @address 0x64ac58 ?fixup_loaded_simple_shape_volume@@YAXPAX@Z
// @address 0x64ac70 ?get_simple_shape_volume_vtable@@YAPBXXZ
// @address 0x64aca0 ??0phys_simple_shape_volume@@QAA@PBVphys_shape@@ABVphys_transform@@I@Z
// @address 0x64ad00 ??1phys_simple_shape_volume@@UAA@XZ
// @address 0x64ad78 ?clone@phys_simple_shape_volume@@UBAPAVphys_volume@@XZ
// @address 0x64af40 ?free_internal_arrays@phys_simple_shape_volume@@UAAXXZ
// @address 0x64afc0 ?notify_collider_added@phys_volume@@IAA?AW4phys_collider_accept@@PBVphys_collider@@@Z
// @address 0x64b038 ?notify_collider_removed@phys_volume@@IAAXPBVphys_collider@@Vphys_bool@@@Z
// @address ??_Gphys_volume@@UAAPAXI@Z
// @address 0x64b110 ??0phys_simple_shape_volume@@QAA@Vphys_fixup_tag@@@Z
// @address 0x64b178 ??_Gphys_simple_shape_volume@@UAAPAXI@Z
// @address 0x64a9c8 ?get_closest_points@phys_simple_shape_volume@@UAAXAAVphys_hit_collector@@@Z
// @address 0x64aa68 ?get_penetrations@phys_simple_shape_volume@@UAAXAAVphys_proxy_pair_collector@@@Z
// @address 0x64b0a8 ??_Gphys_shape_volume@@UAAPAXI@Z
#include "phys/phys_dynamics.h"
#include "phys/phys_aabb.h"
#include "phys/phys_dispatcher.h"
#include "phys/phys_statistics_collector.h"
#include "phys/phys_collectors.h"
#include "phys/phys_volume_overlap_listener.h"
#include "phys/phys_type_desc.h"
phys_collider_accept phys_volume::notify_collider_added( const phys_collider* collidable )
{
	phys_collider_added_event event;
	event.m_collider = collidable;
	event.m_overlap_volume = this;
	event.m_collider_accept = PHYS_COLLIDER_ACCEPT;

	for ( int i = m_overlap_listeners.getSize()-1; i >= 0; i-- )
	{
		if (m_overlap_listeners[i] != 0)
		{
			m_overlap_listeners[i]->collider_added_callback( event );
		}
	}
	
	return event.m_collider_accept;
}

void phys_volume::notify_collider_removed( const phys_collider* collidable, phys_bool collider_was_added )
{
	phys_collider_removed_event event;
	event.m_collider = collidable;
	event.m_overlap_volume = this;
	event.m_collider_was_added = collider_was_added;

	for ( int i = m_overlap_listeners.getSize()-1; i >= 0; i-- )
	{
		if (m_overlap_listeners[i] != 0)
		{
			m_overlap_listeners[i]->collider_removed_callback( event );
		}
	}
	
}


	phys_simple_shape_volume::phys_simple_shape_volume( const phys_shape* shape, const phys_transform& transform, unsigned int collision_filter_info )
	: phys_shape_volume( shape, transform )
{
	m_collider.set_collision_filter_info( collision_filter_info );
}

phys_simple_shape_volume::~phys_simple_shape_volume()
{

}

phys_volume_type phys_simple_shape_volume::get_type_id() const
{
	return PHYS_VOLUME_SIMPLE_SHAPE;
}

phys_volume* phys_simple_shape_volume::clone() const
{
	const phys_collider* c = get_collider();
	phys_simple_shape_volume* volume_clone = new phys_simple_shape_volume(c->get_shape(), m_motion_state.get_transform(), c->get_collision_filter_info() );
	
	volume_clone->m_overlap_listeners = m_overlap_listeners;
	volume_clone->m_volume_listeners = m_volume_listeners;

	volume_clone->copy_object_props( this );
	
	return volume_clone;
}

void phys_simple_shape_volume::set_position_and_sweep_cast(
	const phys_vec4 &position, const phys_linear_cast_input &input, phys_hit_collector &cast_hit_collector, phys_hit_collector *start_collector)
{


	m_motion_state.get_transform().set_translation( position );

	phys_aabb aabb;
	phys_vec4 path;

	{
		phys_real half_tolerance = 0.5f * m_world->get_collision_input()->get_tolerance();
		m_collider.get_shape()->get_handle_aabb( m_motion_state.get_transform(), half_tolerance + input.m_start_tolerance, aabb );

		path.set_sub4( input.m_ray_to, position );
		phys_vec4 zero; zero.set_zero4();
		phys_vec4 path_min; path_min.set_min4( zero, path );
		phys_vec4 path_max; path_max.set_max4( zero, path );

		aabb.m_min_limit.add4( path_min );
		aabb.m_max_limit.add4( path_max );
		update_broadphase( aabb );
	}

	phys_linear_cast_collision_input linear_cast_input;
	{
		linear_cast_input.set( *m_world->get_collision_input() );
		linear_cast_input.set_path_and_tolerance( path, input.m_start_tolerance );
		linear_cast_input.m_max_extra_penetration = input.m_max_extra_penetration;
	}

	{
		phys_dispatcher* dispatcher = m_world->get_dispatcher();
		for ( int i = m_collision_details.getSize() - 1; i >= 0; i-- )
		{
			collision_detail& capsule_determinant = m_collision_details[i];
			phys_shape_type shape_type_b = capsule_determinant.m_collider->get_shape()->get_type_id();

			phys_dispatcher::agent_funcs::sweep_cast_fn sweep_cast = dispatcher->get_linear_cast_fn( m_collider.get_shape()->get_type_id(), shape_type_b );
			sweep_cast( m_collider, *capsule_determinant.m_collider, linear_cast_input, cast_hit_collector, start_collector );
		}
	}
}

void phys_simple_shape_volume::get_closest_points( phys_hit_collector& collector, const phys_collision_input* input )
{
	if (!input)
	{
		input = m_world->get_collision_input();
	}
	phys_dispatcher* dispatcher = input->m_agent_dispatcher;
	for ( int i = m_collision_details.getSize() - 1; i >= 0; i-- )
	{
		collision_detail& capsule_determinant = m_collision_details[i];
		phys_shape_type shape_type_b = capsule_determinant.m_collider->get_shape()->get_type_id();

		phys_dispatcher::agent_funcs::closest_points_fn closest_pts_agent_fn = dispatcher->get_closest_points_fn(
			m_collider.get_shape()->get_type_id(), shape_type_b);
		(*closest_pts_agent_fn)( m_collider, *capsule_determinant.m_collider, *input, collector );
	}
}
	
void phys_simple_shape_volume::get_penetrations( phys_proxy_pair_collector& collector, const phys_collision_input* input )
{
	if (!input)
	{
		input = m_world->get_collision_input();
	}
	phys_dispatcher* dispatcher = input->m_agent_dispatcher;
	for ( int i = m_collision_details.getSize() - 1; i >= 0; i-- )
	{
		collision_detail& capsule_determinant = m_collision_details[i];
		phys_shape_type shape_type_b = capsule_determinant.m_collider->get_shape()->get_type_id();

		phys_dispatcher::agent_funcs::penetrations_fn agent_get_penetrations_fn = dispatcher->get_penetrations_fn(
			m_collider.get_shape()->get_type_id(), shape_type_b);
		(*agent_get_penetrations_fn)( m_collider, *capsule_determinant.m_collider, *input, collector );
		if ( collector.get_early_out() )
		{
			break;
		}
	}
}

phys_bool phys_simple_shape_volume::has_overlapping_collider( phys_collider* collidable )
{
	for( int i = 0; i<m_collision_details.getSize(); i++ )
	{
		if( m_collision_details[i].m_collider == collidable )
		{
			return true;
		}
	}
	return false;
}

void phys_simple_shape_volume::insert_overlapping_collider( phys_collider* collidable )
{

	if ( collidable->get_shape() != 0 )
	{
		phys_collider_accept accept = notify_collider_added( collidable );

		if ( accept == PHYS_COLLIDER_ACCEPT  )
		{
			collision_detail& capsule_determinant = m_collision_details.expand_one();
			capsule_determinant.m_collider = collidable;
		}
	}
}

void phys_simple_shape_volume::erase_overlapping_collider( phys_collider* collidable )
{

	if ( collidable->get_shape() != 0 )
	{
		
		for ( int i = m_collision_details.getSize() - 1; i >= 0; i-- )
		{
			if ( m_collision_details[i].m_collider == collidable )
			{
				notify_collider_removed( collidable, true );
				m_collision_details.remove_at( i );
				return;
			}
		}
		notify_collider_removed( collidable, false );
	}
}

void phys_simple_shape_volume::gather_memory_stats( phys_statistics_collector* collector ) const
{
	collector->begin_stats_object("SimplePhantm", collector->stat_category_instance, this);
	phys_volume::gather_memory_stats(collector);
	collector->add_array_stats("OverlapPtr", collector->stat_category_runtime, m_collision_details);
	collector->end_object();
}

void phys_simple_shape_volume::free_internal_arrays()
{
	
	if (m_collision_details.getSize() == 0)
	{
		m_collision_details.clear_and_deallocate();
	}

	phys_shape_volume::free_internal_arrays();
}


void phys_simple_shape_volume::get_closest_points( phys_hit_collector& collector )
{
	phys_dispatcher* dispatcher = m_world->get_collision_input()->m_agent_dispatcher;
	const phys_collision_input* input = m_world->get_collision_input();
	for ( int i = m_collision_details.getSize() - 1; i >= 0; i-- )
	{
		collision_detail& capsule_determinant = m_collision_details[i];
		phys_shape_type shape_type_b = capsule_determinant.m_collider->get_shape()->get_type_id();

		phys_dispatcher::agent_funcs::closest_points_fn closest_pts_agent_fn = dispatcher->get_closest_points_fn(
			m_collider.get_shape()->get_type_id(), shape_type_b);
		(*closest_pts_agent_fn)( m_collider, *capsule_determinant.m_collider, *input, collector );
	}
}

void phys_simple_shape_volume::get_penetrations( phys_proxy_pair_collector& collector )
{
	phys_dispatcher* dispatcher = m_world->get_collision_input()->m_agent_dispatcher;
	const phys_collision_input* input = m_world->get_collision_input();
	for ( int i = m_collision_details.getSize() - 1; i >= 0; i-- )
	{
		collision_detail& capsule_determinant = m_collision_details[i];
		phys_shape_type shape_type_b = capsule_determinant.m_collider->get_shape()->get_type_id();

		phys_dispatcher::agent_funcs::penetrations_fn agent_get_penetrations_fn = dispatcher->get_penetrations_fn(
			m_collider.get_shape()->get_type_id(), shape_type_b);
		(*agent_get_penetrations_fn)( m_collider, *capsule_determinant.m_collider, *input, collector );
		if ( collector.get_early_out() )
		{
			break;
		}
	}
}

void fixup_loaded_simple_shape_volume(void *object)
{
	phys_fixup_tag flag;
	flag.m_fixup_finishing = 1;
	new (object) phys_simple_shape_volume(flag);
}
void destroy_loaded_simple_shape_volume(void *object)
{
	static_cast<phys_simple_shape_volume *>(object)->~phys_simple_shape_volume();
}
const void *get_simple_shape_volume_vtable()
{
	union { __declspec(align(16)) void *ptr; char buffer[sizeof(phys_simple_shape_volume)]; } object;
	phys_fixup_tag flag;
	new (object.buffer) phys_simple_shape_volume(flag);
	return object.ptr;
}

const phys_type_desc phys_simple_shape_volume_type_desc = phys_type_desc("hkSimpleShapePhantom", fixup_loaded_simple_shape_volume,
	destroy_loaded_simple_shape_volume, get_simple_shape_volume_vtable());
