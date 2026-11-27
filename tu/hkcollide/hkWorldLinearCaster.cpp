// hkcollide/hkWorldLinearCaster.obj: from the records 0x7053e0 0x705498
// @compiler xdk6130
// @flags /TP /O2 /Oc /GF /EHs-c- /GR-
// @address 0x7053e0 ?add_broadphase_handle@phys_world_linear_caster@@MAAMPBVphys_broadphase_handle@@H@Z
// @address 0x705498 ?sweep_cast@phys_world_linear_caster@@QAAXABVphys_broadphase@@PBVphys_collider@@ABUphys_linear_cast_input@@PBVphys_collider_pair_filter@@ABUphys_collision_input@@PAUphys_agent_config@@AAVphys_hit_collector@@PAV8@@Z
#define PHYS_CONFIG_SIMD 1
#include "phys/phys_pair_agent_machine.h"
#include "phys/phys_solver.h"


#define PHYS_COLLIDE_H 


#include "phys/phys_base.h"
#include <float.h>


#define PHYS_MONITOR_STREAM_H 
#include "phys/phys_shape_type.h"
#include "phys/phys_solver_export.h"
#include "phys/phys_collide.h"
#include "phys/phys_agent.h"
#include "phys/phys_symmetric_agent_linear_cast.h"
#include "phys/phys_atoms.h"
#include "phys/phys_dynamics.h"
#include "phys/phys_monitor_stream.h"
#include "phys/phys_solver_elem_temp.h"
#include "phys/phys_agent_entry.h"

 class phys_bool;

#define PHYS_MONITOR_STATS_COLLECTOR_H 

#include "phys/phys_statistics_collector.h"
#include "phys/phys_aabb.h"
#include "phys/phys_dispatcher.h"
#include "phys/phys_broadphase_cast_collector.h"
#include "phys/phys_world_linear_caster.h"
#include "phys/phys_broadphase.h"






#define PHYS_MATH_AABB_H 















#define PHYS_MATH_SPHERE_H 














#define PHYS_MATH_CONTACT_POINT_H 

 typedef unsigned short phys_contact_point_id;


#define PHYS_INVALID_CONTACT_POINT (0xffff)




















#define PHYS_WORLD_LINEAR_CASTER 


#define PHYS_BROADPHASE_CAST_COLLECTOR_H 

 class phys_broadphase_handle;



 struct phys_collision_input;
 struct phys_linear_cast_input;
 struct phys_agent_config;
 class phys_hit_collector;
 class phys_collision_filter;
 class phys_collider_pair_filter;
 class phys_broadphase;
 typedef char phys_broadphase_aabb_cache;




#define PHYS_LINEAR_CAST_INPUT 




#define PHYS_SHAPE_RAY_INPUT 

 class phys_ray_compound_shape_filter;






#define PHYS_RAY_HIT_COLLECTOR 

 struct phys_ray_input;
 struct phys_ray_collector_output;
 class phys_collision_proxy;
 typedef unsigned int phys_shape_key;







#define PHYS_COLLISION_FILTER_H 


#define PHYS_COLLIDER_COLLIDER_FILTER_H 

 class phys_collider;




#define PHYS_RAY_COLLIDER_FILTER_H 

 struct phys_world_ray_input;





#define PHYS_COMPOUND_SHAPE_FILTER_H 

 struct phys_collision_input;
 class phys_bvh_shape;




#define PHYS_RAY_COMPOUND_SHAPE_FILTER 

 struct phys_ray_input;
 class phys_compound_shape;
 class phys_shape;





 class phys_bvh_shape;






 phys_real phys_world_linear_caster::add_broadphase_handle( const phys_broadphase_handle* broadphase_handle, int broadphase_cast_index )
 {
    const phys_collider* hit_collider =
        static_cast<phys_collider*>( static_cast<const phys_typed_broadphase_handle*>(broadphase_handle)->get_collider_owner() );
    
    const phys_typed_broadphase_handle* tp = static_cast<const phys_typed_broadphase_handle*>( broadphase_handle );
    const phys_collider* collider_b = reinterpret_cast<const phys_collider*>( tp->get_collider_owner() );

    const phys_shape* penetration_shape_b = collider_b->get_shape();

    if ( (!penetration_shape_b) || (m_collider_a == collider_b) )
    {
        return m_cast_collector->get_early_out_distance();
    }

    if ( m_compound_filter->allows_collision( *m_collider_a, *hit_collider ) )
    {
        phys_shape_type shape_type_b = penetration_shape_b->get_type_id();
        phys_dispatcher::sweep_cast_fn dispatch_linear_cast_fn = m_shape_ray_input.m_agent_dispatcher->get_linear_cast_fn( m_cast_type_a, shape_type_b );
        dispatch_linear_cast_fn( *m_collider_a, *collider_b, m_shape_ray_input, *m_cast_collector, m_start_hit_collector ); 
    }
    return m_cast_collector->get_early_out_distance();
 }

 void phys_world_linear_caster::sweep_cast( const phys_broadphase& broadphase, const phys_collider* collider_a,
                                      const phys_linear_cast_input& input, const phys_collider_pair_filter* filter,
                                      const phys_collision_input& cast_collision_input, phys_agent_config* config, 
                                      phys_hit_collector& cast_hit_collector, phys_hit_collector* start_point_collector )
 {

    m_cast_collector = &cast_hit_collector;
    m_start_hit_collector = start_point_collector;
    m_input = &input;
    m_collider_a = collider_a;
    m_compound_filter = filter;
    m_cast_type_a = collider_a->get_shape()->get_type_id();

    phys_collision_input& ip = m_shape_ray_input;
    ip = cast_collision_input;
    m_shape_ray_input.m_agent_config = config;

    phys_broadphase::phys_aabb_cast_input ci;
    {
        phys_vec4 path; path.set_sub4( input.m_ray_to, collider_a->get_transform().get_translation() );
        m_shape_ray_input.set_path_and_tolerance( path, input.m_start_tolerance );
        phys_aabb aabb;
        collider_a->get_shape()->get_handle_aabb( collider_a->get_transform(), input.m_start_tolerance, aabb );
        
        ci.m_ray_from.set_lerp4( aabb.m_min_limit, aabb.m_max_limit, 0.5f );
        ci.m_ray_to.set_add4( ci.m_ray_from, path );
        ci.m_half_extents.set_sub4( aabb.m_max_limit, aabb.m_min_limit );
        ci.m_half_extents.mul4( 0.5f );
        m_shape_ray_input.m_max_extra_penetration = input.m_max_extra_penetration;
    }
    broadphase.cast_bounds( ci, *this );
 }


