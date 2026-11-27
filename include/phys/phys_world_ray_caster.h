#ifndef INCLUDE_PHYS_WORLD_RAY_CASTER_H
#define INCLUDE_PHYS_WORLD_RAY_CASTER_H

#include "phys/phys_collide.h"
#include "phys/phys_broadphase_cast_collector.h"

class phys_broadphase;
typedef char phys_broadphase_aabb_cache;

class phys_world_ray_caster : public phys_broadphase_cast_collector
{
	public:
	
		phys_world_ray_caster(){}

		~phys_world_ray_caster(){}

		void trace_ray( const phys_broadphase& broadphase, const phys_world_ray_input& input, const phys_collision_filter* filter,
			phys_ray_hit_collector& collector );

		void trace_ray(const phys_broadphase &broadphase, const phys_world_ray_input &input, const phys_collision_filter *filter,
			const phys_broadphase_aabb_cache *cache, phys_ray_hit_collector &collector);

		void trace_ray(const phys_broadphase &broadphase, const phys_world_ray_input &input, const phys_collision_filter *filter,
			phys_broadphase_aabb_cache *cache, phys_ray_hit_collector &collector);

		void trace_ray_group(phys_broadphase &broadphase, const phys_world_ray_input *ray_inputs, int ray_group_count, const phys_collision_filter *filter,
			phys_ray_hit_collector *cast_collector_base, int collector_stride);

		void trace_rays_from_point(phys_broadphase &broadphase, const phys_world_ray_input *ray_inputs, int ray_group_count,
			const phys_collision_filter *filter, phys_broadphase_aabb_cache *cache, phys_ray_hit_collector *cast_collector_base, int collector_stride);

	protected:
		virtual	phys_real add_broadphase_handle( const phys_broadphase_handle* broadphase_handle, int broadphase_cast_index );

	protected:
		const phys_world_ray_input*   m_input;
		const phys_ray_collider_filter* m_compound_filter;
		phys_ray_hit_collector*			 m_collector_base;
		int							 m_collector_stride;

		phys_ray_input	m_shape_ray_input;
};

#endif
