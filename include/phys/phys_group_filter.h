#ifndef INCLUDE_PHYS_GROUP_FILTER_H
#define INCLUDE_PHYS_GROUP_FILTER_H

#include "phys/phys_collide.h"

// Collision filter by layer, system group and subsystem (tu/hkcollide/hkGroupFilter.cpp). A filter info word holds the
// layer in bits 0-4, the subsystem id in bits 5-9, the subsystem it does not collide with in bits 10-14 and the
// system group in bits 16-31.
class phys_group_filter : public phys_collision_filter {
public:
    phys_group_filter();
    __forceinline phys_group_filter(phys_fixup_tag) { m_filter_kind = filter_kind_group; }
    ~phys_group_filter();

    void enable_collisions_between(int layer_a, int layer_b);
    void disable_layer_pair(int layer_a, int layer_b);
    void enable_collisions_by_mask(unsigned int layer_bits_a, unsigned int layer_bits_b);
    void disable_layer_bits(unsigned int layer_bits_a, unsigned int layer_bits_b);

    virtual phys_bool allows_collision(const phys_collider &a, const phys_collider &b) const;
    virtual phys_bool allows_collision(const phys_collision_input &input, const phys_collision_proxy &a, const phys_collision_proxy &b,
        const phys_shape_container &filter_container_b, phys_shape_key filter_key_b) const;
    virtual phys_bool allows_collision(const phys_ray_input &ray_input_a, const phys_shape &filter_shape_b, const phys_shape_container &filter_container_b,
        phys_shape_key filter_key_b) const;
    virtual phys_bool allows_collision(const phys_world_ray_input &body_input_a, const phys_collider &pair_collider_b) const;
    phys_bool allows_collision(unsigned int group_filter_info_a, unsigned int group_filter_info_b) const;

    int m_next_free_sys_group;
    unsigned int m_collision_lookup[32];
};

void fixup_loaded_group_filter(void *object);
void destroy_loaded_group_filter(void *object);
const void *get_group_filter_vtable();

#endif // HAVOK_HK_GROUP_FILTER_H
