// hkcollide/hkCapsuleCapsuleAgent.obj: from the records 0x62c1c8 0x62c240 0x62c2b8 0x62c4f0 0x62c508 0x62c6d8 0x62c898 0x62ca88 0x62cc78 0x62cce0
// @compiler xdk6130
// @flags /TP /O2 /Oc /GF /EHs-c- /GR-
// @address 0x62c1c8 ?create_capsule_capsule_agent@phys_capsule_capsule_agent@@KAPAVphys_agent@@ABVphys_collision_proxy@@0ABUphys_collision_input@@PAVphys_contact_manager@@@Z
// @address 0x62c240 ?cleanup@phys_capsule_capsule_agent@@UAAXAAVphys_constraint_owner@@@Z
// @address 0x62c4f0 ?get_penetrations@phys_capsule_capsule_agent@@UAAXABVphys_collision_proxy@@0ABUphys_collision_input@@AAVphys_proxy_pair_collector@@@Z
// @address 0x62c508 ?get_three_closest_points_inl@@YAXABVphys_collision_proxy@@0ABUphys_collision_input@@PAVphys_contact_point@@@Z
// @address 0x62cc78 ?register_agent@phys_capsule_capsule_agent@@SAXPAVphys_dispatcher@@@Z
#define PHYS_CONFIG_SIMD 1
#define PHYS_LEGACY_COMPAT 1
#include "physics/havok_memory.h"
#include "phys/phys_pair_agent_machine.h"
#include "phys/phys_solver.h"


#define PHYS_COLLIDE_H 


#include "phys/phys_base.h"
#include <float.h>
#include "phys/phys_collide.h"


#define PHYS_MONITOR_STREAM_H 
#include "phys/phys_collide_capsule_util.h"
#include "phys/phys_shape_type.h"
#include "phys/phys_solver_export.h"
#include "phys/phys_agent.h"
#include "phys/phys_symmetric_agent_linear_cast.h"
#include "phys/phys_atoms.h"
#include "phys/phys_dynamics.h"
#include "phys/phys_monitor_stream.h"
#include "phys/phys_vec4_util.h"
#include "phys/phys_solver_elem_temp.h"
#include "phys/phys_agent_entry.h"
#include "phys/phys_collide_triangle_util.h"
#include "phys/phys_statistics_collector.h"
#include "phys/phys_aabb.h"
#include "phys/phys_dispatcher.h"
#include "phys/phys_capsule_capsule_agent.h"


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




















#define PHYS_CAPSULE_SHAPE_H 


#define PHYS_CONVEX_SHAPE_H 


#define PHYS_COLLISION_SPHERES_SHAPE_H 

 class phys_sphere;

#define PHYS_COLLISION_SPHERE_BUFFER_SIZE 64






 class phys_hull_vertex;


#define PHYS_W_NOT_SET_TO_VERTEX_ID "The function getSupportingVertex did not set the .w component of the supporting vertex to the vertex id using set3asUin16"
    


















#define PHYS_CAPSULE_CAPSULE_AGENT_H 


#define PHYS_LEAF_AGENT_H 




#define PHYS_COLLIDE_TRIANGLE_UTIL_H 



 class phys_dispatcher;




#define PHYS_COLLIDE_CAPSULE_UTIL_H 




#define PHYS_LINE_SEGMENT_UTIL_H 

 class phys_contact_point;







 phys_agent *PHYS_CALL phys_capsule_capsule_agent::create_capsule_capsule_agent(
    const phys_collision_proxy &agent_body_a, const phys_collision_proxy &agent_body_b, const phys_collision_input &input,
        phys_contact_manager *pair_contact_manager)
 {
    return new phys_capsule_capsule_agent( agent_body_a, agent_body_b, input, pair_contact_manager );
 }

 void PHYS_CALL phys_capsule_capsule_agent::register_agent(phys_dispatcher* dispatcher)
 {
    {
        phys_dispatcher::agent_funcs af;
        af.m_create_fn          = create_capsule_capsule_agent;
        af.m_penetrations_fn = static_penetrations;
        af.m_closest_points_fn = static_closest_points;
        af.m_linear_cast_func      = static_sweep_cast;
        af.m_flipped           = false;
        af.m_predictive		 = false;
        dispatcher->register_collision_agent(af, PHYS_SHAPE_CAPSULE, PHYS_SHAPE_CAPSULE);	
    }
 }

 void phys_capsule_capsule_agent::cleanup(phys_constraint_owner& constraint_owner_ref)
 {
    for (int i = 0; i < 3; i++ )
    {
        if(m_process_contact_id[i] != PHYS_INVALID_CONTACT_POINT)
        {
            m_contact_manager->contact_remove_point(m_process_contact_id[i], constraint_owner_ref );
        }
    }
    delete this;
 }

 void PHYS_CALL get_three_closest_points_inl( const  phys_collision_proxy& agent_body_a,
     const phys_collision_proxy& agent_body_b,	const phys_collision_input& input, phys_contact_point* points )
 {
    const phys_real max_dist_tol = input.get_tolerance();
    points[0].contact_set_distance( max_dist_tol );
    points[1].contact_set_distance( max_dist_tol );
    points[2].contact_set_distance( max_dist_tol );

    const phys_capsule_shape* capsule_shape_a = static_cast<const phys_capsule_shape*>(agent_body_a.get_shape());
    const phys_capsule_shape* capsule_shape_b = static_cast<const phys_capsule_shape*>(agent_body_b.get_shape());
    phys_vec4 pA[2]; phys_vec4_util::transform_points( agent_body_a.get_transform(), capsule_shape_a->get_vertices(), 2, pA );
    phys_vec4 pB[2]; phys_vec4_util::transform_points( agent_body_b.get_transform(), capsule_shape_b->get_vertices(), 2, pB );

    phys_capsule_util_manifold_caps_vs_caps( pA, capsule_shape_a->get_convex_radius(), pB, capsule_shape_b->get_convex_radius(), points );
 }

 void phys_capsule_capsule_agent::get_penetrations(const  phys_collision_proxy& agent_body_a, const phys_collision_proxy& agent_body_b,
     const phys_collision_input& input, phys_proxy_pair_collector& collector )
 {
    static_penetrations( agent_body_a, agent_body_b, input, collector );
 }


// @address 0x62ca88 ?static_closest_points@phys_capsule_capsule_agent@@SAXABVphys_collision_proxy@@0ABUphys_collision_input@@AAVphys_hit_collector@@@Z
void PHYS_CALL phys_capsule_capsule_agent::static_closest_points(
    const phys_collision_proxy &agent_body_a, const phys_collision_proxy &agent_body_b, const phys_collision_input &input, phys_hit_collector &collector)
{
    phys_hit_point event(agent_body_a, agent_body_b);
    const phys_capsule_shape *capsule_shape_a = static_cast<const phys_capsule_shape *>(agent_body_a.get_shape());
    const phys_capsule_shape *capsule_shape_b = static_cast<const phys_capsule_shape *>(agent_body_b.get_shape());
    phys_vec4 pA[2];
    phys_vec4_util::transform_points(agent_body_a.get_transform(), capsule_shape_a->get_vertices(), 2, pA);
    phys_vec4 pB[2];
    phys_vec4_util::transform_points(agent_body_b.get_transform(), capsule_shape_b->get_vertices(), 2, pB);
    if (phys_capsule_util_closest_caps_vs_caps(pA, capsule_shape_a->get_convex_radius(), pB, capsule_shape_b->get_convex_radius(),
        input.get_tolerance(), event.m_hit_contact) == 0)
        collector.add_hit(event);
}

// @address 0x62c6d8 ?get_closest_point_internal@@YA?AW4phys_result@@ABVphys_collision_proxy@@0ABUphys_collision_input@@AAVphys_contact_point@@@Z
phys_result PHYS_CALL get_closest_point_internal(const phys_collision_proxy &agent_body_a, const phys_collision_proxy &agent_body_b,
    const phys_collision_input &input, phys_contact_point &capsule_closest_point)
{
    const phys_capsule_shape *capsule_shape_a = static_cast<const phys_capsule_shape *>(agent_body_a.get_shape());
    const phys_capsule_shape *capsule_shape_b = static_cast<const phys_capsule_shape *>(agent_body_b.get_shape());
    phys_vec4 pA[2];
    phys_vec4_util::transform_points(agent_body_a.get_transform(), capsule_shape_a->get_vertices(), 2, pA);
    phys_vec4 pB[2];
    phys_vec4_util::transform_points(agent_body_b.get_transform(), capsule_shape_b->get_vertices(), 2, pB);
    return phys_capsule_util_closest_caps_vs_caps(pA, capsule_shape_a->get_convex_radius(), pB, capsule_shape_b->get_convex_radius(),
        input.get_tolerance(), capsule_closest_point);
}


// @address 0x62c898 ?get_closest_points@phys_capsule_capsule_agent@@UAAXABVphys_collision_proxy@@0ABUphys_collision_input@@AAVphys_hit_collector@@@Z
void phys_capsule_capsule_agent::get_closest_points(const phys_collision_proxy &agent_body_a, const phys_collision_proxy &agent_body_b,
    const phys_collision_input &input, phys_hit_collector &collector)
{
    phys_hit_point event(agent_body_a, agent_body_b);
    const phys_capsule_shape *capsule_shape_a = static_cast<const phys_capsule_shape *>(agent_body_a.get_shape());
    const phys_capsule_shape *capsule_shape_b = static_cast<const phys_capsule_shape *>(agent_body_b.get_shape());
    phys_vec4 pA[2];
    phys_vec4_util::transform_points(agent_body_a.get_transform(), capsule_shape_a->get_vertices(), 2, pA);
    phys_vec4 pB[2];
    phys_vec4_util::transform_points(agent_body_b.get_transform(), capsule_shape_b->get_vertices(), 2, pB);
    if (phys_capsule_util_closest_caps_vs_caps(pA, capsule_shape_a->get_convex_radius(), pB, capsule_shape_b->get_convex_radius(),
        input.get_tolerance(), event.m_hit_contact) == 0)
        collector.add_hit(event);
}


// @address 0x62c2b8 ?static_penetrations@phys_capsule_capsule_agent@@SAXABVphys_collision_proxy@@0ABUphys_collision_input@@AAVphys_proxy_pair_collector@@@Z
void PHYS_CALL phys_capsule_capsule_agent::static_penetrations(
    const phys_collision_proxy &agent_body_a, const phys_collision_proxy &agent_body_b, const phys_collision_input &input, phys_proxy_pair_collector &collector)
{
    const phys_capsule_shape *capsule_shape_a = static_cast<const phys_capsule_shape *>(agent_body_a.get_shape());
    const phys_capsule_shape *capsule_shape_b = static_cast<const phys_capsule_shape *>(agent_body_b.get_shape());
    phys_vec4 pA[2];
    phys_vec4_util::transform_points(agent_body_a.get_transform(), capsule_shape_a->get_vertices(), 2, pA);
    phys_vec4 pB[2];
    phys_vec4_util::transform_points(agent_body_b.get_transform(), capsule_shape_b->get_vertices(), 2, pB);
    phys_vec4 dA;
    dA.set_sub4(pA[1], pA[0]);
    phys_vec4 dB;
    dB.set_sub4(pB[1], pB[0]);
    phys_collide_triangle_util::segment_segment_result result;
    phys_collide_triangle_util::closest_segment_pair(pA[0], dA, pB[0], dB, result);
    phys_real capsule_radius_sum = capsule_shape_a->get_convex_radius() + capsule_shape_b->get_convex_radius();
    if (result.m_dist_sq < capsule_radius_sum * capsule_radius_sum)
        collector.add_proxy_pair(agent_body_a, agent_body_b);
}

// @address 0x62cce0 ?process_collision@phys_capsule_capsule_agent@@UAAXABVphys_collision_proxy@@0ABUphys_process_collision_input@@AAUphys_process_collision_output@@@Z
void phys_capsule_capsule_agent::process_collision(
    const phys_collision_proxy &agent_body_a, const phys_collision_proxy &agent_body_b, const phys_process_collision_input &input,
        phys_process_collision_output &result)
{
    phys_contact_point points[3];
    {
        const phys_real max_dist_tol = input.get_tolerance();
        points[0].contact_set_distance( max_dist_tol );
        points[1].contact_set_distance( max_dist_tol );
        points[2].contact_set_distance( max_dist_tol );

        const phys_capsule_shape* capsule_shape_a = static_cast<const phys_capsule_shape*>(agent_body_a.get_shape());
        const phys_capsule_shape* capsule_shape_b = static_cast<const phys_capsule_shape*>(agent_body_b.get_shape());
        phys_vec4 pA[2]; phys_vec4_util::transform_points( agent_body_a.get_transform(), capsule_shape_a->get_vertices(), 2, pA );
        phys_vec4 pB[2]; phys_vec4_util::transform_points( agent_body_b.get_transform(), capsule_shape_b->get_vertices(), 2, pB );

        phys_capsule_util_manifold_caps_vs_caps( pA, capsule_shape_a->get_convex_radius(), pB, capsule_shape_b->get_convex_radius(), points );
    }
    {
        for (int p = 0; p < 3; p++ )
        {
            if ( points[p].get_distance() + 0.001 < input.get_tolerance() )
            {
                if(m_process_contact_id[p] == PHYS_INVALID_CONTACT_POINT)
                {
                    m_process_contact_id[p] = m_contact_manager->add_manifold_contact_point(agent_body_a, agent_body_b, input, result, 0, points[p] );
                }

                if ( m_process_contact_id[p] != PHYS_INVALID_CONTACT_POINT )
                {
                    phys_process_hit_point& point = *result.reserve_contact_points(1);
                    result.commit_contact_points(1);
                    point.m_hit_contact.get_contact_position() = points[p].get_contact_position();
                    point.m_hit_contact.get_separating_normal() = points[p].get_separating_normal();
                    point.m_process_contact_id = m_process_contact_id[p];
                }
            }
            else
            {
                if(m_process_contact_id[p] != PHYS_INVALID_CONTACT_POINT)
                {
                    m_contact_manager->contact_remove_point( m_process_contact_id[p], *result.m_constraint_owner.val() );
                    m_process_contact_id[p] = PHYS_INVALID_CONTACT_POINT;
                }
            }
        }
    }
}
