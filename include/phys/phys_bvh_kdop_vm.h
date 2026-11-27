#ifndef INCLUDE_PHYS_BVH_KDOP_VM_H
#define INCLUDE_PHYS_BVH_KDOP_VM_H

#include "phys/phys_collide.h"
#include "phys/phys_bvh_vm.h"
#include "phys/phys_kdop26.h"

extern "C" { extern const char k_phys_bvh_kdop_vm_source_file[]; }

struct phys_bvh_info {
    phys_kdop26 m_query_kdop;
    unsigned int m_shape_key;
    char m_kdop_level;
    phys_bool m_has_shape_key;
};

struct phys_bvh_kdop_query {
    phys_bool m_has_hit;
    int m_kdop_depth;
    phys_bool m_use_shape_key;
    unsigned int m_shape_key;
    __forceinline phys_bvh_kdop_query() {
        m_has_hit = false;
        m_kdop_depth = -1;
        m_use_shape_key = false;
        m_shape_key = 0;
    }
};

class __declspec(align(16)) phys_bvh_kdop_vm : public phys_bvh_vm {
public:
    typedef phys_bvh_info kdop_geometry;
    struct phys_bvh_kdop_vm_query {
        int m_obb_offset_x;
        int m_obb_offset_y;
        int m_obb_offset_z;
        int m_bit_shift;
        unsigned int m_primitive_offset;
        unsigned int m_properties[phys_bvh_bytecode::PRIMITIVE_PROP_MAX];
    };
    phys_bvh_kdop_vm();
    ~phys_bvh_kdop_vm();
    void query_bvh(const phys_bvh_bytecode *code, const phys_bvh_kdop_query &kdop_query_input, phys_bvh_info *kdop_geometries);
protected:
    void query_bvh_kdop_geoms_rec(const phys_bvh_kdop_vm_query *query, const unsigned char *PC);
    void push_kdop(phys_bool has_shape_key = false, unsigned int kdop_shape_key = (unsigned int)-1);
    __forceinline void pop_kdop() { m_next_geometry--; }
    float m_int_to_float_scale;
    phys_vec4 m_offset;
    unsigned int m_terminal_offset;
    phys_array<unsigned int> m_keys;
    kdop_geometry *m_next_geometry;
    phys_kdop26 m_current_kdop;
    int m_kdop_level;
    phys_bvh_kdop_query m_kdop_query;
    phys_bool m_has_hit;
    phys_bool m_early_exit;
protected:
  void add_cast_hit(unsigned int obj, unsigned int const *const B);
};

typedef char havok_size_hkMoppInfo[(sizeof(phys_bvh_info) == 112) ? 1 : -1];
typedef char havok_size_hkMoppKDopQuery[(sizeof(phys_bvh_kdop_query) == 16) ? 1 : -1];

#endif // HAVOK_HK_MOPP_KDOP_GEOMETRIES_VIRTUAL_MACHINE_H
