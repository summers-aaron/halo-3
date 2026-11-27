// hkcollide/hkWorldRayCaster.obj: from the records 0x7040a8 0x704160 0x704218 0x7043e0 0x704488
// @compiler xdk6130
// @flags /TP /O2 /Oc /GF /EHs-c- /GR-
// @address 0x7040a8 ?trace_ray@phys_world_ray_caster@@QAAXABVphys_broadphase@@ABUphys_world_ray_input@@PBVphys_collision_filter@@AAVphys_ray_hit_collector@@@Z
// @address 0x704160 ?trace_ray@phys_world_ray_caster@@QAAXABVphys_broadphase@@ABUphys_world_ray_input@@PBVphys_collision_filter@@PADAAVphys_ray_hit_collector@@@Z
// @address 0x704488 ?add_broadphase_handle@phys_world_ray_caster@@MAAMPBVphys_broadphase_handle@@H@Z
// @address 0x7043e0 ?trace_rays_from_point@phys_world_ray_caster@@QAAXAAVphys_broadphase@@PBUphys_world_ray_input@@HPBVphys_collision_filter@@PADPAVphys_ray_hit_collector@@H@Z
// @address 0x42d298 ?havok_override_thread_memory_stack_allocations@@YA_NXZ
#define PHYS_CONFIG_SIMD 1
#include "physics/havok_memory.h"
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
#include "phys/phys_statistics_collector.h"
#include "phys/phys_aabb.h"
#include "phys/phys_broadphase_cast_collector.h"
#include "phys/phys_world_ray_caster.h"
#include "phys/phys_broadphase.h"

 class phys_bool;

#define PHYS_BASE_CONFIG_MONITORS_H 

#define PHYS_CONFIG_MONITORS_DISABLED 0
#define PHYS_CONFIG_MONITORS_ENABLED 1


#define PHYS_CONFIG_MONITORS PHYS_CONFIG_MONITORS_DISABLED







#define PHYS_PERF_BOOKMARK(BOOKMARK_VALUE) 


    #pragma intrinsic(__mftb)




#define PHYS_MONITOR_KIND_TIMER "t"
#define PHYS_MONITOR_KIND_FLOAT "f"
#define PHYS_MONITOR_KIND_INT "i"

#define PHYS_MONITOR_CMD_PUSH_DIR "Pd"
#define PHYS_MONITOR_CMD_POP_DIR "pd"
#define PHYS_MONITOR_CMD_BEGIN_TIMER "T"

#define PHYS_MONITOR_CMD_END_TIMER "E"
#define PHYS_MONITOR_CMD_BEGIN_LIST "L"
#define PHYS_MONITOR_CMD_SPLIT_LIST "S"
#define PHYS_MONITOR_CMD_END_LIST "l"

#define PHYS_MONITOR_CMD_NOP "N"


#define PHYS_MONITOR_PUSH_DIR(MONITOR_PATH) 
#define PHYS_MONITOR_POP_DIR() 
#define PHYS_MONITOR_ADD_VALUE(STAT_NAME,VALUE,MONITOR_KIND) 
#define PHYS_MONITOR_NOP() 

#define PHYS_TIMER_BEGIN(STAT_NAME,TIMER_OBJECT) 
#define PHYS_TIMER_END() 
#define PHYS_TIMER_NAMED_END(STAT_NAME) 
#define PHYS_TIMER_BEGIN_LIST(STAT_NAME,TIMER_OBJECT) 
#define PHYS_TIMER_SPLIT_LIST(STAT_NAME) 
#define PHYS_TIMER_END_LIST() 

#define PHYS_TIME_CODE_BLOCK(STAT_NAME,TIMER_OBJECT) 



#define PHYS_INTERNAL_TIMER_BEGIN(STAT_NAME,TIMER_OBJECT) {}
#define PHYS_INTERNAL_TIMER_END() {}
#define PHYS_INTERNAL_TIMER_BEGIN_LIST(a,b) {}
#define PHYS_INTERNAL_TIMER_SPLIT_LIST(a) {}
#define PHYS_INTERNAL_TIMER_END_LIST() {}
#define PHYS_INTERNAL_MONITOR_ADD_VALUE(STAT_NAME,VALUE,MONITOR_KIND) {}

#define PHYS_USER_TIMER_BEGIN(STAT_NAME,TIMER_OBJECT) PHYS_TIMER_BEGIN(STAT_NAME,0)
#define PHYS_USER_TIMER_END PHYS_TIMER_END
#define PHYS_USER_TIMER_BEGIN_LIST(a,b) PHYS_TIMER_BEGIN_LIST(a,b)
#define PHYS_USER_TIMER_SPLIT_LIST(a) PHYS_TIMER_SPLIT_LIST(a)
#define PHYS_USER_TIMER_END_LIST PHYS_TIMER_END_LIST




#define PHYS_MONITOR_STATS_COLLECTOR_H 







#define PHYS_MATH_AABB_H 















#define PHYS_MATH_SPHERE_H 














#define PHYS_MATH_CONTACT_POINT_H 

 typedef unsigned short phys_contact_point_id;


#define PHYS_INVALID_CONTACT_POINT (0xffff)




















#define PHYS_WORLD_RAY_CASTER_H 


#define PHYS_BROADPHASE_CAST_COLLECTOR_H 

 class phys_broadphase_handle;




#define PHYS_SHAPE_RAY_INPUT 

 class phys_ray_compound_shape_filter;






#define PHYS_WORLD_RAY_INPUT 



 struct phys_world_ray_input;
 struct phys_world_ray_output;
 class phys_collision_filter;
 class phys_ray_collider_filter;
 class phys_broadphase;
 typedef char phys_broadphase_aabb_cache;




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






 phys_real 	phys_world_ray_caster::add_broadphase_handle( const phys_broadphase_handle* broadphase_handle, int broadphase_cast_index )
 {
    const phys_collider* hit_collider =
        static_cast<phys_collider*>( static_cast<const phys_typed_broadphase_handle*>(broadphase_handle)->get_collider_owner() );
    const phys_shape* shape = hit_collider->get_shape();

    phys_ray_hit_collector* collector = phys_add_byte_offset( m_collector_base, m_collector_stride * broadphase_cast_index );
    if (shape)
    {
        const phys_world_ray_input& input = m_input[broadphase_cast_index];
        if ( m_compound_filter->allows_collision( input, *hit_collider ) )
        {
            const phys_transform& trans = hit_collider->get_transform();

            m_shape_ray_input.m_ray_from.set_transformed_inverse_pos( trans, m_input->m_ray_from);
            m_shape_ray_input.m_ray_to.  set_transformed_inverse_pos( trans, input.m_ray_to);
            m_shape_ray_input.m_filter_info = input.m_filter_info;

            shape->trace_ray_with_collector( m_shape_ray_input, *hit_collider, *collector);
        }
    }
    return collector->m_early_out_hit_frac;
 }

 void phys_world_ray_caster::trace_ray(
    const phys_broadphase &broadphase, const phys_world_ray_input &input, const phys_collision_filter *filter, phys_ray_hit_collector &collector)
 {

    PHYS_TIMER_BEGIN("RayCast", 0);
    m_input = &input;
    m_collector_base = &collector;
    m_collector_stride = 0;
    m_compound_filter = filter;

    if ( input.m_compound_filter_enabled )
    {
        m_shape_ray_input.m_ray_compound_filter = filter;
    }
    else
    {
        m_shape_ray_input.m_ray_compound_filter = 0;
    }

    phys_broadphase::phys_ray_cast_input ray_input;
    ray_input.m_ray_from = input.m_ray_from;
    ray_input.m_to_base = &input.m_ray_to;

    broadphase.trace_ray( ray_input, this, 0 );
    PHYS_TIMER_END();
 }

 void phys_world_ray_caster::trace_ray(const phys_broadphase &broadphase, const phys_world_ray_input &input, const phys_collision_filter *filter,
    const phys_broadphase_aabb_cache *cache, phys_ray_hit_collector &collector)
 {

    PHYS_TIMER_BEGIN("RayCstCached", 0);
    m_input = &input;
    m_collector_base = &collector;
    m_collector_stride = 0;
    m_compound_filter = filter;

    if ( input.m_compound_filter_enabled )
    {
        m_shape_ray_input.m_ray_compound_filter = filter;
    }
    else
    {
        m_shape_ray_input.m_ray_compound_filter = 0;
    }

    phys_broadphase::phys_ray_cast_input ray_input;
    ray_input.m_ray_from = input.m_ray_from;
    ray_input.m_to_base = &input.m_ray_to;
    ray_input.m_aabb_cache_info = cache;

    broadphase.trace_ray( ray_input, this, 0 );
    PHYS_TIMER_END();
 }

// @address 0x704218 ?trace_ray_group@phys_world_ray_caster@@QAAXAAVphys_broadphase@@PBUphys_world_ray_input@@HPBVphys_collision_filter@@PAVphys_ray_hit_collector@@H@Z
 void phys_world_ray_caster::trace_ray_group(phys_broadphase &broadphase, const phys_world_ray_input *ray_inputs, int ray_group_count,
    const phys_collision_filter *filter, phys_ray_hit_collector *cast_collector_base, int collector_stride)
 {
    PHYS_TIMER_BEGIN("RayCastGroup", 0);
    phys_aabb aabb;
    {
        aabb.m_min_limit.set_min4( ray_inputs->m_ray_from, ray_inputs->m_ray_to );
        aabb.m_max_limit.set_max4( ray_inputs->m_ray_from, ray_inputs->m_ray_to );

        const phys_world_ray_input* in = ray_inputs;
        in++;
        for ( int x = ray_group_count-2; x>=0; x--)
        {
            aabb.m_min_limit.set_min4( aabb.m_min_limit, in->m_ray_to );
            aabb.m_min_limit.set_min4( aabb.m_min_limit, in->m_ray_from );

            aabb.m_max_limit.set_max4( aabb.m_max_limit, in->m_ray_to );
            aabb.m_max_limit.set_max4( aabb.m_max_limit, in->m_ray_from );
            in++;
        }
    }
    int aabb_cache_size = broadphase.get_aabb_cache_size();
    phys_broadphase_aabb_cache* cache =
        static_cast<phys_broadphase_aabb_cache*>(phys_thread_memory::get_singleton_instance().allocate_stack( aabb_cache_size ));

    broadphase.compute_bounds_cache( aabb, cache );
    
    {
        const phys_world_ray_input* in = ray_inputs;
        phys_ray_hit_collector* collector = cast_collector_base;
        for ( int x = ray_group_count-1; x>=0; x--)
        {
            trace_ray( broadphase, *in, filter, cache, *collector);
            in++;
            collector = phys_add_byte_offset( collector, collector_stride );
        }
    }
    phys_thread_memory::get_singleton_instance().free_stack( cache );
    PHYS_TIMER_END();
 }

 void phys_world_ray_caster::trace_rays_from_point(phys_broadphase &broadphase, const phys_world_ray_input *ray_inputs, int ray_group_count,
    const phys_collision_filter *filter, phys_broadphase_aabb_cache *cache, phys_ray_hit_collector *cast_collector_base, int collector_stride)
 {

    PHYS_TIMER_BEGIN("RayCastFSP", 0);
    m_input = ray_inputs;
    m_compound_filter = filter;
    m_collector_base = cast_collector_base;
    m_collector_stride = collector_stride;

    if ( ray_inputs->m_compound_filter_enabled )
    {
        m_shape_ray_input.m_ray_compound_filter = filter;
    }
    else
    {
        m_shape_ray_input.m_ray_compound_filter = 0;
    }

    phys_broadphase::phys_ray_cast_input ray_input;
    ray_input.m_ray_from = ray_inputs->m_ray_from;
    ray_input.m_to_base = &ray_inputs->m_ray_to;
    ray_input.m_to_stride = int(sizeof( phys_world_ray_input ));
    ray_input.m_aabb_cache_info = cache;
    ray_input.m_cast_count = ray_group_count;

    broadphase.trace_ray( ray_input, this, 0 );
    PHYS_TIMER_END();
 }

 void phys_world_ray_caster::trace_ray(const phys_broadphase &broadphase, const phys_world_ray_input &input, const phys_collision_filter *filter,
    phys_broadphase_aabb_cache *cache, phys_ray_hit_collector &collector)
 {
    m_input = &input;
    m_collector_base = &collector;
    m_collector_stride = 0;
    m_compound_filter = filter;

    if ( input.m_compound_filter_enabled )
    {
        m_shape_ray_input.m_ray_compound_filter = filter;
    }
    else
    {
        m_shape_ray_input.m_ray_compound_filter = 0;
    }

    phys_broadphase::phys_ray_cast_input ray_input;
    ray_input.m_ray_from = input.m_ray_from;
    ray_input.m_to_base = &input.m_ray_to;
    ray_input.m_aabb_cache_info = cache;

    broadphase.trace_ray( ray_input, this, 0 );
 }
