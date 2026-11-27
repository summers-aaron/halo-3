// hkcollide/hkSimpleWorldRayCaster.obj: from the records 0x704728 0x7047d8 0x704880 0x704ab8 0x704b58
// @compiler xdk6130
// @flags /TP /O2 /Oc /GF /EHs-c- /GR-
// @address 0x704728 ?trace_ray@phys_simple_world_ray_caster@@QAAXABVphys_broadphase@@ABUphys_world_ray_input@@PBVphys_collision_filter@@AAUphys_world_ray_output@@@Z
// @address 0x704880 ?trace_ray_group@phys_simple_world_ray_caster@@QAAXABVphys_broadphase@@PBUphys_world_ray_input@@HPBVphys_collision_filter@@PAUphys_world_ray_output@@@Z
// @address 0x704b58 ?add_broadphase_handle@phys_simple_world_ray_caster@@MAAMPBVphys_broadphase_handle@@H@Z
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
#include "phys/phys_simple_world_ray_caster.h"
#include "phys/phys_broadphase.h"

 class phys_bool;

#define PHYS_BASE_CONFIG_MONITORS_H 

#define PHYS_CONFIG_MONITORS_DISABLED 0
#define PHYS_CONFIG_MONITORS_ENABLED 1


#define PHYS_CONFIG_MONITORS PHYS_CONFIG_MONITORS_DISABLED







#define PHYS_PERF_BOOKMARK(BOOKMARK_VALUE) 






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




















#define PHYS_SIMPLE_WORLD_RAY_CASTER 


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




#define PHYS_WORLD_RAY_OUTPUT 


 class phys_collider;


    typedef phys_world_ray_output phys_world_ray_output_ppu;








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






 phys_real 	phys_simple_world_ray_caster::add_broadphase_handle( const phys_broadphase_handle* broadphase_handle, int broadphase_cast_index )
 {
    const phys_collider* hit_collider =
        static_cast<phys_collider*>( static_cast<const phys_typed_broadphase_handle*>(broadphase_handle)->get_collider_owner() );
    const phys_shape* shape = hit_collider->get_shape();

    if (shape)
    {
        if ( m_compound_filter->allows_collision( m_input[broadphase_cast_index], *hit_collider ) )
        {
            const phys_transform& trans = hit_collider->get_transform();
            m_shape_ray_input.m_ray_from.set_transformed_inverse_pos( trans, m_input[broadphase_cast_index].m_ray_from);
            m_shape_ray_input.m_ray_to.  set_transformed_inverse_pos( trans, m_input[broadphase_cast_index].m_ray_to);
            m_shape_ray_input.m_filter_info = m_input[broadphase_cast_index].m_filter_info;

            if ( shape->trace_ray( m_shape_ray_input, *m_result) )
            {
                m_result->m_root_collider = hit_collider;
                m_result->m_normal.set_rotated_dir( trans.get_rotation(), m_result->m_normal );
            }
        }
    }
    return m_result->m_hit_frac;
 }

 void phys_simple_world_ray_caster::trace_ray(
    const phys_broadphase &broadphase, const phys_world_ray_input &input, const phys_collision_filter *filter, phys_world_ray_output &output)
 {

    PHYS_TIMER_BEGIN("RayCastSimpl", 0);
    m_input = &input;
    m_result = &output;
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

// @address 0x7047d8 ?trace_ray@phys_simple_world_ray_caster@@QAAXABVphys_broadphase@@ABUphys_world_ray_input@@PBVphys_collision_filter@@PADAAUphys_world_ray_output@@@Z
 void phys_simple_world_ray_caster::trace_ray(const phys_broadphase &broadphase, const phys_world_ray_input &input, const phys_collision_filter *filter,
    phys_broadphase_aabb_cache *cache, phys_world_ray_output &output)
 {

    PHYS_TIMER_BEGIN("RayCstCchSim", 0);
    m_input = &input;
    m_result = &output;
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

 void phys_simple_world_ray_caster::trace_ray_group(const phys_broadphase &broadphase, const phys_world_ray_input *ray_inputs, int ray_group_count,
    const phys_collision_filter *filter, phys_world_ray_output *outputs)
 {
    PHYS_TIMER_BEGIN("RayCstGrpSim", 0);
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
        phys_world_ray_output* out = outputs;
        for ( int x = ray_group_count-1; x>=0; x--)
        {
            trace_ray( broadphase, *in, filter, cache, *out);
            in++;
            out++;
        }
    }
    phys_thread_memory::get_singleton_instance().free_stack( cache );
    PHYS_TIMER_END();
 }

// @address 0x704ab8 ?trace_rays_from_point@phys_simple_world_ray_caster@@QAAXABVphys_broadphase@@PBUphys_world_ray_input@@HPBVphys_collision_filter@@PADPAUphys_world_ray_output@@@Z
 void phys_simple_world_ray_caster::trace_rays_from_point(const phys_broadphase &broadphase, const phys_world_ray_input *ray_inputs, int ray_group_count,
    const phys_collision_filter *filter, phys_broadphase_aabb_cache *cache, phys_world_ray_output *outputs)
 {

    PHYS_TIMER_BEGIN("RayCstFSPSim", 0);
    m_input = ray_inputs;
    m_result = outputs;
    m_compound_filter = filter;

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

