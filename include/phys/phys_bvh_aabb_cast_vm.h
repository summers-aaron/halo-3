#ifndef INCLUDE_PHYS_BVH_AABB_CAST_VM_H
#define INCLUDE_PHYS_BVH_AABB_CAST_VM_H

#include "phys/phys_bvh_vm.h"
#include "phys/phys_collision_input.h"

extern "C" { extern char g_havok_flyingcolors_mopp_aabb_cast; }

struct phys_bvh_aabb_cast_vm_query_int {
    phys_vec4 m_float_to_byte_offset;
    phys_vec4 m_cast_extents;
    float m_extents_sum3;
    int m_bit_shift;
    float m_float_to_byte_scale;
    unsigned int m_primitive_offset;
    unsigned int m_properties[1];
};

struct phys_bvh_aabb_cast_vm_query_float {
    phys_vec4 m_ray_ends[2];
};

class phys_bvh_aabb_cast_vm : public phys_bvh_vm {
public:
    struct phys_bvh_aabb_cast_input {
        phys_vec4 m_ray_from;
        phys_vec4 m_ray_to;
        phys_vec4 m_cast_extents;
        const phys_linear_cast_collision_input *m_collision_input;
        const phys_collision_proxy *m_cast_proxy;
        const phys_collision_proxy *m_bvh_proxy;
    };

    void trace_aabb_cast(const phys_bvh_aabb_cast_input &input, phys_hit_collector &cast_hit_collector, phys_hit_collector *start_collector);

protected:
    const phys_bvh_bytecode *m_bvh_code;
    float m_int_to_float_scale;
    phys_shape_type m_cast_object_type;
    float m_early_hit_frac;
    float m_ref_early_hit_frac;
    phys_hit_collector *m_cast_collector;
    phys_hit_collector *m_start_hit_collector;
    const phys_bvh_aabb_cast_input *m_input;

    void query_ray_on_tree(const phys_bvh_aabb_cast_vm_query_int *query, const unsigned char *bvh_commands,
        phys_bvh_aabb_cast_vm_query_float *const float_query, int bvh_chunk_offset);
    __forceinline void add_cast_hit(unsigned int id, const unsigned int properties[1], unsigned long chunk_id = 0);
};

// The machine occupies 48 bytes.
typedef char havok_size_hkMoppAabbCastVirtualMachine[(sizeof(void *) != 4 || ((sizeof(phys_bvh_aabb_cast_vm) == 48) )) ? 1 : -1];

#endif // HAVOK_HK_MOPP_AABB_CAST_VIRTUAL_MACHINE_H
