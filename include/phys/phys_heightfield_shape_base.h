#ifndef INCLUDE_PHYS_HEIGHTFIELD_SHAPE_BASE_H
#define INCLUDE_PHYS_HEIGHTFIELD_SHAPE_BASE_H

#include "phys/phys_collide.h"

class phys_heightfield_shape_base : public phys_shape
{
public:
    struct collide_spheres_input
    {
        PHYS_DECLARE_NONVIRTUAL_ALLOCATOR(PHYS_MEM_COLLIDE, phys_heightfield_shape_base::collide_spheres_input);

        class phys_sphere *m_sphere_array;
        int m_sphere_count;
        phys_real m_box_tolerance;
    };

    typedef phys_vec4 sphere_hit_output;

    virtual void collide_spheres(const collide_spheres_input &input, sphere_hit_output *output_array) const = 0;

    struct phys_sphere_cast_input : public phys_ray_input
    {
        PHYS_DECLARE_NONVIRTUAL_ALLOCATOR(PHYS_MEM_COLLIDE, phys_heightfield_shape_base::phys_sphere_cast_input);

        phys_real m_radius;
        phys_real m_max_extra_penetration;
    };

    virtual void trace_sphere(const phys_sphere_cast_input &input, const phys_collision_proxy &collision_proxy, phys_ray_hit_collector &collector) const = 0;

public:
    phys_heightfield_shape_base(phys_shape_type type) : phys_shape(type) {}
    phys_heightfield_shape_base(phys_fixup_tag flag) : phys_shape(flag) { m_type = PHYS_SHAPE_HEIGHTFIELD; }
};

#endif // HAVOK_HK_HEIGHT_FIELD_SHAPE_H
