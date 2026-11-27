// OBJ: from the records 0x61f0f8
// @compiler xdk3424
// @flags /TP /O1 /Oc
// @address 0x61f0f8 ?register_all_agents@phys_agent_register_util@@SAXPAVphys_dispatcher@@@Z
#include "phys/phys_base_types.h"
#include "phys/phys_collide.h"
#include "phys/phys_shape_type.h"
#include "phys/phys_dispatcher.h"
#include "phys/phys_bounding_agent.h"
#include "phys/phys_multi_sphere_agent.h"
#include "phys/phys_bvh_agent.h"
#include "phys/phys_bvh_tree_agent.h"
#include "phys/phys_bvh_stream_agent.h"
#include "phys/phys_bvh_tree_stream_agent.h"
#include "phys/phys_compound_shape_agent.h"
#include "phys/phys_transform_agent.h"
#include "phys/phys_volume_agent.h"
#include "phys/phys_list_agent.h"
#include "phys/phys_convex_list_agent.h"
#include "phys/phys_heightfield_agent.h"
#include "phys/phys_pred_simplex_agent.h"
#include "phys/phys_box_box_agent.h"
#include "phys/phys_sphere_sphere_agent.h"
#include "phys/phys_sphere_capsule_agent.h"
#include "phys/phys_sphere_triangle_agent.h"
#include "phys/phys_sphere_box_agent.h"
#include "phys/phys_capsule_capsule_agent.h"
#include "phys/phys_capsule_triangle_agent.h"
#include "phys/phys_multi_sphere_triangle_agent.h"
#include "phys/phys_multi_ray_convex_agent.h"
#include "phys/phys_bvh_embedded_stream_narrow_agent.h"
#include "phys/phys_list_narrow_agent.h"
#include "phys/phys_pred_simplex_narrow_agent.h"
#include "phys/phys_pred_simplex_cylinder_narrow_agent.h"
#include "phys/phys_box_box_narrow_agent.h"
#include "phys/phys_capsule_triangle_narrow_agent.h"
#include "phys/phys_agent_register_util.h"
void phys_agent_register_util::register_all_agents(phys_dispatcher *dispatcher)
{
    phys_register_alt_shape_types(dispatcher);
    phys_bounding_agent::register_agent(dispatcher);
    phys_multi_sphere_agent::register_agent(dispatcher);
    dispatcher->set_enable_checks(false);
    phys_bvh_agent::register_agent(dispatcher);
    phys_bvh_tree_agent::register_agent(dispatcher);
    phys_bvh_stream_agent::register_agent(dispatcher);
    phys_bvh_tree_stream_agent::register_agent(dispatcher);
    phys_bvh_embedded_stream_narrow_agent::register_narrow_agent(dispatcher);
    dispatcher->set_enable_checks(true);
    dispatcher->set_enable_checks(false);
    phys_compound_shape_agent::register_agent(dispatcher);
    dispatcher->set_enable_checks(true);
    phys_transform_agent::register_agent(dispatcher);
    phys_volume_agent::register_agent(dispatcher);
    phys_list_agent::register_agent(dispatcher);
    phys_convex_list_agent::register_agent(dispatcher);
    phys_bvh_stream_agent::register_convex_list_agent(dispatcher);
    phys_heightfield_agent::register_agent(dispatcher);
    phys_list_narrow_agent::register_narrow_agent(dispatcher);
    phys_pred_simplex_agent::register_agent(dispatcher);
    phys_pred_simplex_narrow_agent::register_narrow_agent(dispatcher, PHYS_SHAPE_CONVEX, PHYS_SHAPE_CONVEX);
    phys_pred_simplex_cylinder_narrow_agent::register_narrow_agent(dispatcher);
    phys_box_box_narrow_agent::register_narrow_agent(dispatcher);
    phys_box_box_agent::register_agent(dispatcher);
    phys_sphere_sphere_agent::register_agent(dispatcher);
    phys_sphere_capsule_agent::register_agent(dispatcher);
    phys_sphere_triangle_agent::register_agent(dispatcher);
    phys_pred_simplex_narrow_agent::register_narrow_agent(dispatcher, PHYS_SHAPE_TRIANGLE, PHYS_SHAPE_SPHERE);
    phys_pred_simplex_narrow_agent::register_narrow_agent(dispatcher, PHYS_SHAPE_SPHERE, PHYS_SHAPE_TRIANGLE);
    phys_sphere_box_agent::register_agent(dispatcher);
    phys_capsule_capsule_agent::register_agent(dispatcher);
    phys_capsule_triangle_agent::register_agent(dispatcher);
    phys_capsule_triangle_narrow_agent::register_narrow_agent(dispatcher);
    phys_multi_sphere_triangle_agent::register_agent(dispatcher);
    phys_multi_ray_convex_agent::register_agent(dispatcher);
    phys_bvh_stream_agent::register_multi_ray_agent(dispatcher);
}

