#ifndef INCLUDE_PHYS_BVH_LONG_RAY_VM_H
#define INCLUDE_PHYS_BVH_LONG_RAY_VM_H

#include "phys/phys_bvh_vm.h"
#include "phys/phys_collide.h"
extern "C" {
    extern char g_phys_bvh_long_ray_flying_colors_flag;
    extern const float k_phys_u8_to_real[];
}

class phys_bvh_long_ray_vm : public phys_bvh_vm {
public:
    phys_bool query_long_ray(const phys_shape_container *collection, const phys_bvh_bytecode *code, const phys_ray_input &input, phys_ray_output &ray_result);
    void query_long_ray(const phys_shape_container *collection, const phys_bvh_bytecode *code, const phys_ray_input &input,
        const phys_collision_proxy &body, phys_ray_hit_collector &collector);

protected:
    const phys_bvh_bytecode *m_bvh_code;
    float m_int_to_float_scale;
    phys_ray_input m_ray_input;
    phys_bool m_obb_hit_found;
    float m_early_out_hit_frac;
    phys_ray_output *m_ray_output_ptr;
    phys_ray_hit_collector *m_collector;
    const phys_collision_proxy *m_ray_proxy;
    const phys_shape_container *m_shape_container;

    struct query_int {
        phys_vec4 m_offset;
        int m_bit_shift;
        float m_scale;
        unsigned int m_primitive_offset;
        unsigned int m_properties[1];
    };
    struct query_float {
        phys_vec4 m_ray_from;
        phys_vec4 m_ray_to;
    };

    void query_ray_on_tree(const query_int *query, const unsigned char *bvh_commands, query_float *const float_query, int bvh_chunk_offset);
    __forceinline void add_cast_hit(unsigned int id, const unsigned int properties[1]);
    __forceinline void query_long_ray_sub(const phys_bvh_bytecode *code, const phys_ray_input &input);
};

#endif // HAVOK_HK_MOPP_LONG_RAY_VIRTUAL_MACHINE_H
