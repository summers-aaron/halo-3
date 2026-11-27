// hkcollide/hkShape.obj: from the records 0x621bf8 0x621c00
// @compiler xdk6130
// @flags /TP /O2 /Oc
#include <vectorintrinsics.h>
#include "phys/phys_math.h"
#include "phys/phys_collide.h"
#include "phys/phys_vec4_util.h"
#include "phys/phys_quad_constants.h"
#include "phys/phys_axis_sweep_constants.h"
#include "physics/havok_component_globals.h"
// @address 0x621bf8 ?reset_shape_functions@phys_shape@@SAXXZ
void phys_shape::reset_shape_functions()
{
}
// @address 0x621c00 ?get_max_projection@phys_shape@@UBAMABVphys_vec4@@@Z
float phys_shape::get_max_projection(const phys_vec4 &direction) const
{
    phys_transform local_to_world;
    local_to_world.set_to_identity();
    phys_aabb aabb;
    get_handle_aabb(local_to_world, 0.0f, aabb);
    phys_vec4 box_half_extents;
    box_half_extents.set_sub4(aabb.m_max_limit, aabb.m_min_limit);
    box_half_extents.mul4(phys_simd_scalar(0.5f));
    phys_vec4 center;
    center.set_add4(aabb.m_max_limit, aabb.m_min_limit);
    center.mul4(phys_simd_scalar(0.5f));
    phys_vec4_util::mul_signs4(box_half_extents, direction);
    box_half_extents.add4(center);
    return static_cast<float>(box_half_extents.dot3(direction));
}

// Port link (P5): the float sign mask at 0x82000f00, owned by
// hkcollide:hkShape.obj (data-owners.tsv). Three separately-named users alias
// the same 16 bytes; -0.0f is exactly 0x80000000 per lane.
// @address 0xf00 g_phys_quad_sign_mask
__vector4 g_phys_quad_sign_mask = { -0.0f, -0.0f, -0.0f, -0.0f };
// @address 0xf00 k_havok_component_vector_sign_mask
__vector4 k_havok_component_vector_sign_mask = { -0.0f, -0.0f, -0.0f, -0.0f };
// @address 0xf00 k_phys_axis_sweep_negate_mask
__vector4 k_phys_axis_sweep_negate_mask = { -0.0f, -0.0f, -0.0f, -0.0f };
