// blamlibxbox/collision_instanced_geometry.obj: from the records 0xb0ab0
// @flags /TP /O2 /Oc
#include "objects/object_reference_types.h"
#include "physics/collision_types.h"
// @address 0xb0ab0 ?add_ray_hit@c_instanced_geometry_ray_hit_collector@@MAAXABVphys_collision_proxy@@ABUphys_ray_collector_output@@@Z

#include "math/real_math.h"
#include "structures/structures.h"
#include "physics/collision_bsp_test_context.h"

#include "physics/collision_instanced_geometry.h"

#include "physics/collisions.h"
#include "physics/collision_bsp_types.h"

static __forceinline long instance_index_from_key(phys_shape_key key)
{
    return key;
}

// 0xb0ab0
void c_instanced_geometry_ray_hit_collector::add_ray_hit(const phys_collision_proxy &body, const phys_ray_collector_output &hit_info)
{
    long instance_index = instance_index_from_key(body.get_shape_key());
    if (structure_instanced_geometry_mark(m_context->structure_bsp_index, instance_index)) {
        if (instanced_geometry_test_vector_internal(m_context, instance_index, m_flags, m_bsp_flags, m_point, m_vector,
                m_result)) {
            float fraction;
            if ((m_flags >> 16) & 1) {
                fraction = 0.0f;
            } else {
                fraction = m_early_out_hit_frac;
                if (!(fraction <= m_result->fraction))
                    fraction = m_result->fraction;
            }
            m_early_out_hit_frac = fraction;
            m_hit = true;
        }
    }
}
