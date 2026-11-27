#ifndef INCLUDE_PHYS_MULTI_SPHERE_SHAPE_H
#define INCLUDE_PHYS_MULTI_SPHERE_SHAPE_H

#include "phys/phys_collide.h"

class phys_multi_sphere_shape : public phys_sphere_rep_shape {
public:
    PHYS_DECLARE_ALLOCATOR(PHYS_MEM_BASE);
    enum { MULTI_SPHERE_MAX = 8 };
    phys_multi_sphere_shape(const phys_vec4 *spheres, int count);
    __forceinline phys_multi_sphere_shape(phys_fixup_tag flag) : phys_sphere_rep_shape(flag) {
        m_type = PHYS_SHAPE_MULTI_SPHERE;
    }
    virtual __forceinline ~phys_multi_sphere_shape() {}
    __forceinline int get_sphere_count() const { return m_sphere_count; }
    __forceinline const phys_vec4 *get_spheres() const { return m_sphere_array; }
    virtual int get_collision_sphere_count_impl() const;
    virtual const phys_sphere *get_collision_spheres_impl(phys_sphere *spheres) const;
    virtual void gather_memory_stats(phys_statistics_collector *collector) const;
    virtual phys_bool trace_ray_impl(const phys_ray_input &input, phys_ray_output &output) const;
    virtual void get_aabb_impl(const phys_transform &local_to_world, float tolerance, phys_aabb &out) const;
    virtual void trace_ray_with_collector(const phys_ray_input &input, const phys_collision_proxy &body, phys_ray_hit_collector &collector) const;

    int m_sphere_count;
    phys_vec4 m_sphere_array[MULTI_SPHERE_MAX];
};

typedef char phys_multi_sphere_shape_spheres_offset[(sizeof(void *) != 4 || ((offsetof(phys_multi_sphere_shape, m_sphere_array) == 0x20))) ? 1 : -1];
typedef char phys_multi_sphere_shape_size[(sizeof(void *) != 4 || ((sizeof(phys_multi_sphere_shape) == 0xa0))) ? 1 : -1];

struct phys_ray_input;
struct phys_ray_output;
class phys_vec4;

int trace_ray_internal(const phys_ray_input &input, const phys_vec4 *spheres, int count, float *hit_fractions, int *indices);
int get_best_hit(const phys_vec4 *spheres, int count, float *hit_fractions, int *indices, int hit_count,
    const phys_ray_input &input, phys_ray_output &output);

#endif // HAVOK_HK_MULTI_SPHERE_SHAPE_H
