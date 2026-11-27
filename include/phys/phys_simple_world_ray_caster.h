#ifndef INCLUDE_PHYS_SIMPLE_WORLD_RAY_CASTER_H
#define INCLUDE_PHYS_SIMPLE_WORLD_RAY_CASTER_H

#include "phys/phys_collide.h"
#include "phys/phys_broadphase_cast_collector.h"

class phys_broadphase;
typedef char phys_broadphase_aabb_cache;

class phys_simple_world_ray_caster : public phys_broadphase_cast_collector
{
	public:
	
		phys_simple_world_ray_caster(){}

		~phys_simple_world_ray_caster(){}

		void trace_ray( const phys_broadphase& broadphase, const phys_world_ray_input& input, const phys_collision_filter* filter,
			phys_world_ray_output& output );

		void trace_ray_group(const phys_broadphase &broadphase, const phys_world_ray_input *ray_inputs, int ray_group_count,
			const phys_collision_filter *filter, phys_world_ray_output *outputs);

		void trace_ray(const phys_broadphase &broadphase, const phys_world_ray_input &input, const phys_collision_filter *filter,
			phys_broadphase_aabb_cache *cache, phys_world_ray_output &output);

		void trace_rays_from_point(const phys_broadphase &broadphase, const phys_world_ray_input *ray_inputs, int ray_group_count,
			const phys_collision_filter *filter, phys_broadphase_aabb_cache *cache, phys_world_ray_output *outputs);
	protected:
		virtual	phys_real add_broadphase_handle( const phys_broadphase_handle* broadphase_handle, int broadphase_cast_index );

	protected:
		const phys_world_ray_input*   m_input;
		const phys_ray_collider_filter* m_compound_filter;
		phys_world_ray_output*  m_result;

		phys_ray_input    m_shape_ray_input;
};


#endif
