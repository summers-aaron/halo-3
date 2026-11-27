#ifndef INCLUDE_PHYS_MULTI_RAY_SHAPE_H
#define INCLUDE_PHYS_MULTI_RAY_SHAPE_H
#include "phys/phys_collide.h"
class phys_multi_ray_shape : public phys_shape {
public:
    struct multi_ray_entry {
        phys_vec4 m_start;
        phys_vec4 m_end;
    };
    phys_array<multi_ray_entry> m_ray_list;
    float m_ray_penetration;
    __forceinline const phys_array<multi_ray_entry> &get_rays() const { return m_ray_list; }
    __forceinline float get_ray_penetration_distance() const { return m_ray_penetration; }
};
#endif
