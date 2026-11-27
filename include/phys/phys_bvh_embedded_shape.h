#ifndef INCLUDE_PHYS_BVH_EMBEDDED_SHAPE_H
#define INCLUDE_PHYS_BVH_EMBEDDED_SHAPE_H

#include "phys/phys_collide.h"
#include "phys/phys_edge_weld_util.h"

struct phys_bvh_reindexed_terminal {
    unsigned int m_original_key;
    unsigned int m_reindexed_key;
};

class phys_type_desc;
extern const phys_type_desc phys_bvh_embedded_shape_type_desc;

class phys_bvh_embedded_shape : public phys_bvh_shape, public phys_shape_container {
public:
    phys_bvh_bytecode *m_bvh_code;
    const unsigned char *m_code_data;
    unsigned int m_code_size;
    phys_vec4 m_aabb_half_extents;
    phys_vec4 m_aabb_center;
    phys_bvh_bytecode::bytecode_info m_code_info;
    phys_vec4 m_triangle_extrusion;
    float m_child_radius;
    unsigned char m_edge_weld_type;
    const void *m_material_base;
    int m_material_stride;
    int m_material_count;
    phys_array<phys_bvh_reindexed_terminal> m_reindexed_terminals;
    phys_bvh_embedded_shape(phys_fixup_tag flag);
    phys_bvh_embedded_shape(const phys_bvh_bytecode *code, phys_array<phys_bvh_reindexed_terminal> *reindexed_terminals);
    virtual ~phys_bvh_embedded_shape();
    virtual void gather_memory_stats(phys_statistics_collector *collector) const;
    virtual void shape_query_sphere(const phys_sphere &sphere, phys_array<unsigned int> &hits) const;
    virtual void query_obb(const phys_transform &transform, const phys_vec4 &box_half_extents, float tolerance, phys_array<unsigned int> &hits) const;
    virtual int get_child_shape_count() const;
    void compute_edge_weld_info(phys_edge_weld_util::edge_weld_kind edge_weld_type);
    void recompute_bounds_extents();
    unsigned int query_aabb(const phys_aabb &aabb, unsigned int *hits, int max_num_keys) const;
    unsigned int get_original_key(unsigned int key) const;
    void initialize(const phys_bvh_bytecode::bytecode_info &info, const unsigned char *code, unsigned int size);
    virtual void get_aabb_impl(const phys_transform &transform, float tolerance, phys_aabb &out) const;
    virtual int compute_dma_size(int) const;
    virtual const phys_shape_container *get_shape_container() const;
    virtual unsigned int get_first_key() const;
    virtual unsigned int get_next_key(unsigned int) const;
    virtual unsigned int get_collision_filter_info(unsigned int) const;
    virtual const phys_shape *get_child_shape(unsigned int, char (&)[512]) const;
    virtual phys_bool trace_ray_impl(const phys_ray_input &, phys_ray_output &) const;
    virtual void trace_ray_with_collector(const phys_ray_input &, const phys_collision_proxy &, phys_ray_hit_collector &) const;
    virtual void query_aabb(const phys_aabb &, phys_array<unsigned int> &) const;
};

#if defined(_M_PPCBE) || defined(_XBOX)
typedef char phys_bvh_embedded_shape_size[(sizeof(void *) != 4 || sizeof(phys_bvh_embedded_shape) == 128) ? 1 : -1];
typedef char phys_bvh_embedded_shape_code_offset[(sizeof(void *) != 4 || offsetof(phys_bvh_embedded_shape, m_bvh_code) == 20) ? 1 : -1];
typedef char phys_bvh_embedded_shape_material_offset[(sizeof(void *) != 4 || offsetof(phys_bvh_embedded_shape, m_material_base) == 104) ? 1 : -1];
#endif

#endif
