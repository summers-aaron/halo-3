#ifndef INCLUDE_PHYS_WORLD_LINEAR_CASTER_H
#define INCLUDE_PHYS_WORLD_LINEAR_CASTER_H

#include "phys/phys_base.h"
#include "phys/phys_broadphase_cast_collector.h"
#include "phys/phys_collision_input.h"
#include "phys/phys_shape_type.h"

class phys_broadphase;
struct phys_linear_cast_input;
class phys_collider;
class phys_collider_pair_filter;
struct phys_agent_config;
class phys_hit_collector;

class phys_world_linear_caster : public phys_broadphase_cast_collector
{
	public:
	
		phys_world_linear_caster(){}

		~phys_world_linear_caster(){}

		void sweep_cast( const phys_broadphase& broadphase, const phys_collider* collider_a, 
					     const phys_linear_cast_input& input, const phys_collider_pair_filter* filter,
						 const phys_collision_input& cast_collision_input, phys_agent_config* config,
						 phys_hit_collector& cast_hit_collector, phys_hit_collector* start_point_collector );

	protected:
		virtual	phys_real add_broadphase_handle( const phys_broadphase_handle* broadphase_handle, int broadphase_cast_index );

	protected:

		const phys_linear_cast_input* m_input;
		const phys_collider_pair_filter* m_compound_filter;
		phys_hit_collector*			 m_cast_collector;
		phys_hit_collector*			 m_start_hit_collector;
		const phys_collider*			 m_collider_a;
		phys_shape_type					 m_cast_type_a;

		phys_linear_cast_collision_input	m_shape_ray_input;
};

#endif
