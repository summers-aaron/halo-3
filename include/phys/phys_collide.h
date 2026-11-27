#ifndef INCLUDE_PHYS_COLLIDE_H
#define INCLUDE_PHYS_COLLIDE_H

#include "phys/phys_base.h"
#include "phys/phys_collectors.h"
#include "phys/phys_aabb.h"

#include "phys/phys_shape_key.h"
typedef unsigned short phys_vertex_id;
typedef unsigned short phys_contact_point_id;


struct phys_aabb_u32 {
    unsigned int m_min_limit[3];
    unsigned char m_expand_min[3];
    unsigned char m_expand_shift;
    unsigned int m_max_limit[3];
    unsigned char m_expand_max[3];
    unsigned char m_shape_key_byte;
};

#include "phys/phys_sphere.h"

#include "phys/phys_contact_point.h"

class phys_contact_point_material {
public:
    __forceinline void set_user_data(phys_ulong data) { m_user_data = data; }
    enum bvh_flag {
        CONTACT_FLAG_FRESH = 1,
        CONTACT_FLAG_SOLVER_PATH2 = 2,
        CONTACT_FLAG_BREAKOFF_ID = 4,
        CONTACT_FLAG_DISABLED = 8
    };

    __forceinline float get_friction() const { return static_cast<float>(m_contact_friction) * (1.0f / 256.0f); }
    __forceinline int get_friction_8_8() const { return m_contact_friction; }
    __forceinline int is_potential_contact() { return m_flags & CONTACT_FLAG_FRESH; }
    __forceinline void set_friction_coeff(float r) {
        m_contact_friction = static_cast<unsigned short>(static_cast<int>(r * 256.0f));
    }
    __forceinline float get_restitution() const { return static_cast<float>(m_contact_restitution) * (1.0f / 128.0f); }
    __forceinline void set_restitution(float r) { m_contact_restitution = static_cast<unsigned char>(static_cast<int>(r * 128.0f)); }
    __forceinline void reset() {
        m_contact_friction = 0;
        m_contact_restitution = 0;
        m_flags = CONTACT_FLAG_FRESH;
    }

    phys_ulong m_user_data;
    unsigned short m_contact_friction;
    unsigned char m_contact_restitution;
    unsigned char m_flags;
};

class __declspec(align(16)) phys_contact_point_material16 : public phys_contact_point_material {
public:
    __forceinline void set(const phys_contact_point_material16 &other) { *this = other; }
};

class phys_hull_vertex : public phys_vec4 {
public:
    __forceinline int get_packed_id() const { return get_int24_w(); }
};

#include "phys/phys_shape_type_enum.h"

class phys_shape;
class phys_shape_container;
class phys_collision_proxy;
class phys_collider;
class phys_ray_hit_collector;
class phys_hit_collector;
class phys_proxy_pair_collector;
class phys_ray_compound_shape_filter;

#include "phys/phys_shape_container.h"

struct phys_ray_input {
    __forceinline phys_ray_input() : m_filter_info(0), m_ray_compound_filter(0) {}
    phys_vec4 m_ray_from;
    phys_vec4 m_ray_to;
    unsigned int m_filter_info;
    const phys_ray_compound_shape_filter *m_ray_compound_filter;
};

struct phys_ray_collector_output {
    __forceinline phys_ray_collector_output() : m_hit_frac(1.0f), m_extra_info(-1) {}
    __forceinline phys_bool has_hit() const { return phys_bool(m_hit_frac < 1.0f); }
    __forceinline void reset() {
        m_hit_frac = 1.0f;
        m_extra_info = -1;
    }

    phys_vec4 m_normal;
    float m_hit_frac;
    int m_extra_info;
    int m_pad[2];
};

struct phys_ray_output : public phys_ray_collector_output {
    enum { HIERARCHY_DEPTH_LIMIT = 8 };

    __forceinline phys_ray_output() { reset_ray_output(); }
    __forceinline void reset() {
        phys_ray_collector_output::reset();
        reset_ray_output();
    }
    __forceinline void change_key_level(int delta) { m_shape_key_idx += delta; }
    __forceinline void set_cached_key(phys_shape_key key) { m_shape_keys[m_shape_key_idx] = key; }

    phys_shape_key m_shape_keys[HIERARCHY_DEPTH_LIMIT];
    int m_shape_key_idx;

private:
    __forceinline void reset_ray_output() {
        m_shape_key_idx = 0;
        m_shape_keys[0] = PHYS_INVALID_SHAPE_KEY;
    }
};

struct phys_world_ray_input {
    __forceinline phys_world_ray_input() : m_compound_filter_enabled(false), m_filter_info(0) {}
    phys_vec4 m_ray_from;
    phys_vec4 m_ray_to;
    phys_bool m_compound_filter_enabled;
    unsigned int m_filter_info;
};

struct phys_world_ray_output : public phys_ray_output {
    __forceinline phys_world_ray_output() : m_root_collider(0) {}
    __forceinline phys_bool has_hit() const { return phys_bool(m_root_collider != 0); }
    __forceinline void reset() {
        m_root_collider = 0;
        phys_ray_output::reset();
    }

    const phys_collider *m_root_collider;
};

struct phys_linear_cast_input {
    __forceinline phys_linear_cast_input() : m_max_extra_penetration(1.1920929e-7f), m_start_tolerance(1.1920929e-7f) {}
    phys_vec4 m_ray_to;
    float m_max_extra_penetration;
    float m_start_tolerance;
};

class phys_shape : public phys_ref_object {
public:
    PHYS_DECLARE_ALLOCATOR(PHYS_MEM_COLLISION_INFO);
    static void reset_shape_functions();

    __forceinline phys_shape(phys_shape_type type) {
        m_user_data = 0;
        m_type = type;
    }
    __forceinline phys_shape(phys_fixup_tag) { m_type = PHYS_SHAPE_INVALID; }

    __forceinline phys_shape_type get_type_id() const { return m_type; }
    __forceinline phys_ulong get_user_data() const { return m_user_data; }
    __forceinline void set_user_data(phys_ulong data) { m_user_data = data; }
    __forceinline void get_handle_aabb(const phys_transform &local_to_world, float tolerance, phys_aabb &out) const {
        get_aabb_impl(local_to_world, tolerance, out);
    }
    __forceinline phys_bool trace_ray(const phys_ray_input &input, phys_ray_output &output) const {
        return trace_ray_impl(input, output);
    }

    virtual float get_max_projection(const phys_vec4 &direction) const;
    virtual void trace_ray_with_collector(const phys_ray_input &input, const phys_collision_proxy &collision_proxy,
        phys_ray_hit_collector &collector) const = 0;
    virtual const phys_shape_container *get_shape_container() const { return 0; }
    virtual int compute_dma_size(int buffer_size_left) const { return -1; }
    virtual void get_aabb_impl(const phys_transform &local_to_world, float tolerance, phys_aabb &out) const = 0;
    virtual phys_bool trace_ray_impl(const phys_ray_input &input, phys_ray_output &output) const = 0;

    phys_ulong m_user_data;
    phys_enum<phys_shape_type, unsigned int> m_type;
};

inline phys_single_shape_container::phys_single_shape_container(const phys_shape *s) : m_single_child_shape(s) {
    m_single_child_shape->add_object_reference();
}

class phys_sphere_rep_shape : public phys_shape {
public:
    __forceinline phys_sphere_rep_shape(phys_shape_type type) : phys_shape(type) {}
    __forceinline phys_sphere_rep_shape(phys_fixup_tag f) : phys_shape(f) { m_type = PHYS_SHAPE_SPHERE_REP; }
    __forceinline int get_collision_sphere_count() const { return get_collision_sphere_count_impl(); }
    __forceinline const phys_sphere *get_collision_spheres(phys_sphere *sphere_buffer) const { return get_collision_spheres_impl(sphere_buffer); }
    virtual int get_collision_sphere_count_impl() const = 0;
    virtual const phys_sphere *get_collision_spheres_impl(phys_sphere *sphere_buffer) const = 0;
};

extern float phys_convex_shape_default_radius;

class phys_convex_shape : public phys_sphere_rep_shape {
public:
    enum weld_result {
        WELD_REJECT_POINT,
        WELD_ACCEPT_MODIFIED,
        WELD_ACCEPT_UNMODIFIED
    };
    __forceinline phys_convex_shape(phys_shape_type type, float radius) : phys_sphere_rep_shape(type) { m_radius = radius; }
    __forceinline phys_convex_shape(phys_fixup_tag f) : phys_sphere_rep_shape(f) { m_type = PHYS_SHAPE_CONVEX; }

    __forceinline float get_convex_radius() const { return m_radius; }
    __forceinline void set_shape_radius(float radius) { m_radius = radius; }
    __forceinline void get_support_vertex(const phys_vec4 &direction, phys_hull_vertex &supporting_vertex_out) const {
        get_support_vertex_impl(direction, supporting_vertex_out);
    }
    __forceinline void vertex_ids_to_vertices(const phys_vertex_id *ids, int id_count, phys_hull_vertex *vertex_array_out) const {
        vertex_ids_to_vertices_impl(ids, id_count, vertex_array_out);
    }
    __forceinline void get_shape_centre(phys_vec4 &centre_out) const { get_shape_centre_impl(centre_out); }
    __forceinline weld_result weld_contact_point(phys_vertex_id *feature_points, unsigned char &feature_point_count,
        phys_vec4 &contact_point_world, const phys_transform *this_obj_transform, const phys_convex_shape *colliding_shape,
        const phys_transform *colliding_transform, phys_vec4 &separating_normal_in_out) const {
        return static_cast<weld_result>(weld_contact_point_impl(feature_points, feature_point_count, contact_point_world,
            this_obj_transform, colliding_shape, colliding_transform, separating_normal_in_out));
    }

    virtual float get_max_projection(const phys_vec4 &direction) const;
    virtual void trace_ray_with_collector(const phys_ray_input &input, const phys_collision_proxy &collision_proxy, phys_ray_hit_collector &collector) const;
    virtual void get_first_vertex(phys_vec4 &v) const = 0;
    virtual void get_support_vertex_impl(const phys_vec4 &direction, phys_hull_vertex &supporting_vertex_out) const = 0;
    virtual void vertex_ids_to_vertices_impl(const phys_vertex_id *ids, int id_count, phys_hull_vertex *vertex_array_out) const = 0;

protected:
    virtual int weld_contact_point_impl(phys_vertex_id *feature_points, unsigned char &feature_point_count,
        phys_vec4 &contact_point_world, const phys_transform *this_obj_transform, const phys_convex_shape *colliding_shape,
        const phys_transform *colliding_transform, phys_vec4 &separating_normal_in_out) const;
    virtual void get_shape_centre_impl(phys_vec4 &centre_out) const;

    float m_radius;
};

class phys_sphere_shape : public phys_convex_shape {
public:
    phys_sphere_shape(float radius);
    __forceinline phys_sphere_shape(phys_fixup_tag f) : phys_convex_shape(f) { m_type = PHYS_SHAPE_SPHERE; }
    virtual void get_support_vertex_impl(const phys_vec4 &direction, phys_hull_vertex &supporting_vertex_out) const;
    virtual void vertex_ids_to_vertices_impl(const phys_vertex_id *ids, int id_count, phys_hull_vertex *vertices_out) const;
    virtual void get_shape_centre_impl(phys_vec4 &centre_out) const;
    virtual int get_collision_sphere_count_impl() const;
    virtual const phys_sphere *get_collision_spheres_impl(phys_sphere *sphere_buffer) const;
    virtual void get_aabb_impl(const phys_transform &local_to_world, float tolerance, phys_aabb &out) const;
    virtual phys_bool trace_ray_impl(const phys_ray_input &input, phys_ray_output &results) const;
    virtual void get_first_vertex(phys_vec4 &v) const;
    virtual void gather_memory_stats(phys_statistics_collector *collector) const;
    virtual int compute_dma_size(int buffer_size_left) const { return sizeof(phys_sphere_shape); }
};

class phys_box_shape : public phys_convex_shape {
public:
    phys_box_shape(const phys_vec4 &box_half_extents, float radius = 0.05f);
    __forceinline phys_box_shape(phys_fixup_tag f) : phys_convex_shape(f) { m_type = PHYS_SHAPE_BOX; }
    virtual ~phys_box_shape();
    __forceinline const phys_vec4 &get_half_extents() const { return m_half_extents; }
    void set_half_extents(const phys_vec4 &box_half_extents);

    virtual void get_support_vertex_impl(const phys_vec4 &direction, phys_hull_vertex &supporting_vertex_out) const;
    virtual void vertex_ids_to_vertices_impl(const phys_vertex_id *ids, int id_count, phys_hull_vertex *vertices_out) const;
    virtual void get_shape_centre_impl(phys_vec4 &centre_out) const;
    virtual int get_collision_sphere_count_impl() const;
    virtual const phys_sphere *get_collision_spheres_impl(phys_sphere *sphere_buffer) const;
    virtual void get_aabb_impl(const phys_transform &local_to_world, float tolerance, phys_aabb &out) const;
    virtual phys_bool trace_ray_impl(const phys_ray_input &input, phys_ray_output &results) const;
    virtual void get_first_vertex(phys_vec4 &v) const;
    virtual void gather_memory_stats(phys_statistics_collector *collector) const;
    virtual int compute_dma_size(int buffer_size_left) const;

protected:
    phys_vec4 m_half_extents;
};

class phys_capsule_shape : public phys_convex_shape {
public:
    static void closest_inf_segment_pair(const phys_vec4 &, const phys_vec4 &, const phys_vec4 &,
        const phys_vec4 &, float &, float &, float &, phys_vec4 &, phys_vec4 &);
    static void closest_point_on_segment(const phys_vec4 &, const phys_vec4 &, const phys_vec4 &, phys_vec4 &);
    phys_capsule_shape(const phys_vec4 &capsule_vertex_a, const phys_vec4 &capsule_vertex_b, float radius);
    __forceinline phys_capsule_shape(phys_fixup_tag f) : phys_convex_shape(f) { m_type = PHYS_SHAPE_CAPSULE; }

    __forceinline const phys_vec4 *get_vertices() const { return &m_vertex_a; }
    __forceinline const phys_vec4 &get_vertex(int i) const { return (&m_vertex_a)[i]; }

    virtual void get_support_vertex_impl(const phys_vec4 &direction, phys_hull_vertex &supporting_vertex_out) const;
    virtual void vertex_ids_to_vertices_impl(const phys_vertex_id *ids, int id_count, phys_hull_vertex *vertices_out) const;
    virtual void get_shape_centre_impl(phys_vec4 &centre_out) const;
    virtual int get_collision_sphere_count_impl() const;
    virtual const phys_sphere *get_collision_spheres_impl(phys_sphere *sphere_buffer) const;
    virtual void get_aabb_impl(const phys_transform &local_to_world, float tolerance, phys_aabb &out) const;
    virtual phys_bool trace_ray_impl(const phys_ray_input &input, phys_ray_output &results) const;
    virtual void get_first_vertex(phys_vec4 &v) const;
    virtual void gather_memory_stats(phys_statistics_collector *collector) const;
    virtual int compute_dma_size(int buffer_size_left) const { return sizeof(phys_capsule_shape); }

protected:
    phys_vec4 m_vertex_a;
    phys_vec4 m_vertex_b;
};

class phys_convex_offset_shape : public phys_convex_shape {
public:
    phys_convex_offset_shape(const phys_convex_shape *bounding_child_shape, const phys_vec4 &translation);
    __forceinline phys_convex_offset_shape(phys_fixup_tag f) : phys_convex_shape(f), m_single_child_shape(f) { m_type = PHYS_SHAPE_CONVEX_OFFSET; }

    __forceinline const phys_convex_shape *get_child_shape() const { return static_cast<const phys_convex_shape *>(m_single_child_shape.get_single_child()); }
    __forceinline const phys_vec4 &get_translation() const { return m_translation; }
    __forceinline phys_vec4 &get_translation() { return m_translation; }

    virtual void get_support_vertex_impl(const phys_vec4 &direction, phys_hull_vertex &supporting_vertex_out) const;
    virtual void vertex_ids_to_vertices_impl(const phys_vertex_id *ids, int id_count, phys_hull_vertex *vertices_out) const;
    virtual void get_shape_centre_impl(phys_vec4 &centre_out) const;
    virtual int get_collision_sphere_count_impl() const;
    virtual const phys_sphere *get_collision_spheres_impl(phys_sphere *sphere_buffer) const;
    virtual void get_aabb_impl(const phys_transform &local_to_world, float tolerance, phys_aabb &out) const;
    virtual phys_bool trace_ray_impl(const phys_ray_input &input, phys_ray_output &results) const;
    virtual void get_first_vertex(phys_vec4 &v) const;
    virtual float get_max_projection(const phys_vec4 &direction) const;
    virtual void trace_ray_with_collector(const phys_ray_input &input, const phys_collision_proxy &collision_proxy, phys_ray_hit_collector &collector) const;
    virtual void gather_memory_stats(phys_statistics_collector *collector) const;
    virtual const phys_shape_container *get_shape_container() const;
    virtual int compute_dma_size(int buffer_size_left) const;

protected:
    phys_single_shape_container m_single_child_shape;
    mutable int m_child_shape_size;
    phys_vec4 m_translation;
};

class phys_convex_xform_shape : public phys_convex_shape {
public:
    phys_convex_xform_shape(const phys_convex_shape *bounding_child_shape, const phys_transform &transform);
    __forceinline phys_convex_xform_shape(phys_fixup_tag f) : phys_convex_shape(f), m_single_child_shape(f) { m_type = PHYS_SHAPE_CONVEX_XFORM; }

    __forceinline const phys_convex_shape *get_child_shape() const { return static_cast<const phys_convex_shape *>(m_single_child_shape.get_single_child()); }
    __forceinline const phys_transform &get_transform() const { return m_body_transform; }
    __forceinline phys_transform &get_transform() { return m_body_transform; }
    void set_transform(const phys_transform &transform);

    virtual void get_support_vertex_impl(const phys_vec4 &direction, phys_hull_vertex &supporting_vertex_out) const;
    virtual void vertex_ids_to_vertices_impl(const phys_vertex_id *ids, int id_count, phys_hull_vertex *vertices_out) const;
    virtual void get_shape_centre_impl(phys_vec4 &centre_out) const;
    virtual int get_collision_sphere_count_impl() const;
    virtual const phys_sphere *get_collision_spheres_impl(phys_sphere *sphere_buffer) const;
    virtual void get_aabb_impl(const phys_transform &local_to_world, float tolerance, phys_aabb &out) const;
    virtual phys_bool trace_ray_impl(const phys_ray_input &input, phys_ray_output &results) const;
    virtual void get_first_vertex(phys_vec4 &v) const;
    virtual float get_max_projection(const phys_vec4 &direction) const;
    virtual void trace_ray_with_collector(const phys_ray_input &input, const phys_collision_proxy &collision_proxy, phys_ray_hit_collector &collector) const;
    virtual void gather_memory_stats(phys_statistics_collector *collector) const;
    virtual const phys_shape_container *get_shape_container() const;
    virtual int compute_dma_size(int buffer_size_left) const;

protected:
    phys_single_shape_container m_single_child_shape;
    mutable int m_child_shape_size;
    phys_transform m_body_transform;
};

class phys_compound_shape : public phys_shape, public phys_shape_container {
public:
    phys_compound_shape(phys_shape_type type);
    __forceinline phys_compound_shape(phys_fixup_tag f) : phys_shape(f) { m_type = PHYS_SHAPE_COMPOUND; }

    virtual phys_bool trace_ray_impl(const phys_ray_input &input, phys_ray_output &results) const;
    virtual void trace_ray_with_collector(const phys_ray_input &input, const phys_collision_proxy &collision_proxy, phys_ray_hit_collector &collector) const;
    virtual void get_aabb_impl(const phys_transform &local_to_world, float tolerance, phys_aabb &out) const;
    virtual float get_max_projection(const phys_vec4 &direction) const;
    virtual void gather_memory_stats(phys_statistics_collector *collector) const;
    virtual const phys_shape_container *get_shape_container() const;

    __forceinline bool edge_weld_enabled() const { return !m_edge_weld_disabled; }

    phys_bool m_edge_weld_disabled;
};

class phys_list_shape : public phys_compound_shape {
public:
    struct child_shape_entry {
        __declspec(align(16)) const phys_shape *m_proxy_shape;
        unsigned int m_collision_filter_info;
        mutable int m_shape_size;
        mutable int m_child_shape_count;
    };

    phys_list_shape(const phys_shape *const *child_shape_array, int list_shape_count);
    phys_list_shape(phys_fixup_tag f);
    virtual ~phys_list_shape();
    void set_collision_filter_info(phys_shape_key key, unsigned int info);
    void recompute_bounds_extents();

protected:
    void set_child_shapes(const phys_shape *const *child_shape_array, int list_shape_count, const unsigned int *list_filter_info);

public:

    __forceinline const phys_shape *get_child_shape_inl(int i) const { return m_child_info[i].m_proxy_shape; }

    virtual int get_child_shape_count() const;
    virtual phys_shape_key get_first_key() const;
    virtual phys_shape_key get_next_key(phys_shape_key old_key) const;
    virtual unsigned int get_collision_filter_info(phys_shape_key key) const;
    virtual const phys_shape *get_child_shape(phys_shape_key key, phys_shape_buffer &buffer) const;
    virtual void get_aabb_impl(const phys_transform &local_to_world, float tolerance, phys_aabb &out) const;
    virtual phys_bool trace_ray_impl(const phys_ray_input &input, phys_ray_output &results) const;
    virtual void gather_memory_stats(phys_statistics_collector *collector) const;
    virtual int compute_dma_size(int buffer_size_left) const;

    phys_array<child_shape_entry> m_child_info;
    phys_vec4 m_aabb_half_extents;
    phys_vec4 m_aabb_center;
};

class phys_bvh_shape : public phys_shape {
public:
    __forceinline phys_bvh_shape(phys_shape_type type) : phys_shape(type) {}
    __forceinline phys_bvh_shape(phys_fixup_tag f) : phys_shape(f) { m_type = PHYS_SHAPE_BVH; }

    virtual void shape_query_sphere(const phys_sphere &sphere, phys_array<phys_shape_key> &hits) const = 0;
    virtual void query_obb(const phys_transform &obb_transform, const phys_vec4 &obb_extent, float tolerance, phys_array<phys_shape_key> &hits) const = 0;
    virtual void query_aabb(const phys_aabb &aabb, phys_array<phys_shape_key> &hits) const = 0;
    virtual const phys_shape_container *get_shape_container() const = 0;
};

class phys_bvh_bytecode : public phys_ref_object {
public:
    PHYS_DECLARE_ALLOCATOR(PHYS_MEM_BVH);

    enum { PRIMITIVE_PROP_MAX = 1 };
    enum bvh_build_kind {
        BVH_BUILD_CHUNKED = 0,
        BVH_BUILD_UNCHUNKED = 1,
        BVH_BUILD_UNSET = 2
    };
    struct bytecode_info {
        phys_vec4 m_offset;
        __forceinline bytecode_info() {}
        __forceinline bytecode_info(const bytecode_info &info) { m_offset = info.m_offset; }
        __forceinline float get_offset_scale() const { return m_offset(3); }
    };

    __forceinline phys_bvh_bytecode(const bytecode_info &info, const unsigned char *bytecode_data, int bytecode_size)
        : m_bvh_info(info), m_data(const_cast<unsigned char *>(bytecode_data), bytecode_size, bytecode_size) {}
    virtual ~phys_bvh_bytecode();

    __forceinline int get_code_size() const { return m_data.getSize(); }

    bytecode_info m_bvh_info;
    phys_array<unsigned char> m_data;
    phys_enum<bvh_build_kind, signed char> m_build_type;
};

class phys_bvh_tree_shape : public phys_bvh_shape {
public:
    phys_bvh_tree_shape(const phys_compound_shape *collection, const phys_bvh_bytecode *code);
    __forceinline phys_bvh_tree_shape(phys_fixup_tag f) : phys_bvh_shape(f), m_child_shape(f) {
        m_type = PHYS_SHAPE_BVH_TREE;
    }
    virtual ~phys_bvh_tree_shape();

    virtual void shape_query_sphere(const phys_sphere &sphere, phys_array<phys_shape_key> &hits) const;
    virtual void query_obb(const phys_transform &obb_to_bvh, const phys_vec4 &extent, float tolerance, phys_array<phys_shape_key> &hits) const;
    virtual void query_aabb(const phys_aabb &aabb, phys_array<phys_shape_key> &hits) const;
    unsigned int query_aabb(const phys_aabb &aabb, phys_shape_key *hits, int max_num_keys) const;

    virtual void get_aabb_impl(const phys_transform &local_to_world, float tolerance, phys_aabb &out) const;
    virtual phys_bool trace_ray_impl(const phys_ray_input &input, phys_ray_output &results) const;
    virtual void trace_ray_with_collector(const phys_ray_input &input, const phys_collision_proxy &collision_proxy, phys_ray_hit_collector &collector) const;

    __forceinline const phys_bvh_bytecode *get_bvh_bytecode() const { return m_bvh_code; }
    __forceinline void set_bvh_bytecode(const phys_bvh_bytecode *code) {
        code->add_object_reference();
        m_bvh_code->object_remove_ref();
        m_bvh_code = code;
    }

    virtual void gather_memory_stats(phys_statistics_collector *collector) const;
    __forceinline const phys_compound_shape *get_compound_shape() const {
        return static_cast<const phys_compound_shape *>(m_child_shape.get_single_child());
    }
    virtual const phys_shape_container *get_shape_container() const;
    __forceinline const phys_shape *get_single_child() const { return m_child_shape.get_single_child(); }

    phys_single_shape_container m_child_shape;
    const phys_bvh_bytecode *m_bvh_code;
};

#include "phys/phys_broadphase_handle.h"

class phys_typed_broadphase_handle : public phys_broadphase_handle {
public:
    enum { COLLIDE_OFFSET_INVALID = 127 };

    __forceinline phys_typed_broadphase_handle() {}
    __forceinline phys_typed_broadphase_handle(int type) {
        m_type = static_cast<signed char>(type);
        m_collision_filter_info = 0;
        m_owner_offset = COLLIDE_OFFSET_INVALID;
    }
    inline phys_typed_broadphase_handle(phys_fixup_tag flag) : phys_broadphase_handle(flag) {}
    __forceinline void set_collider_owner(void *owner) {
        int offset = phys_get_byte_offset_int(this, owner);
        m_owner_offset = static_cast<signed char>(offset);
    }
    __forceinline int get_type_id() const { return m_type; }
    __forceinline void *get_collider_owner() const { return const_cast<char *>(reinterpret_cast<const char *>(this) + m_owner_offset); }
    __forceinline unsigned int get_collision_filter_info() const { return m_collision_filter_info; }
    __forceinline void set_collision_filter_info(unsigned int info) { m_collision_filter_info = info; }
    __forceinline int get_object_quality_type() const { return m_quality_type; }
    __forceinline void set_object_quality_type(unsigned short t) { m_quality_type = t; }

    signed char m_type;
    signed char m_owner_offset;
    unsigned short m_quality_type;
    unsigned int m_collision_filter_info;
};

#include "phys/phys_collision_proxy.h"

enum phys_collider_quality {
    PHYS_COLLIDER_QUALITY_INVALID = 0,
    PHYS_COLLIDER_QUALITY_FIXED = 1,
    PHYS_COLLIDER_QUALITY_KEYFRAMED = 2,
    PHYS_COLLIDER_QUALITY_DEBRIS = 3,
    PHYS_COLLIDER_QUALITY_MOVING = 4,
    PHYS_COLLIDER_QUALITY_CRITICAL = 5,
    PHYS_COLLIDER_QUALITY_BULLET = 6,
    PHYS_COLLIDER_QUALITY_CHARACTER = 7,
    PHYS_COLLIDER_QUALITY_KEYFRAMED_REPORTING = 8,
    PHYS_COLLIDER_QUALITY_MAX = 9
};

class phys_collider : public phys_collision_proxy {
public:
    inline phys_collider(const phys_shape *shape, const phys_motion_state *motion_state, int type)
        : phys_collision_proxy(shape, motion_state), m_owner_offset(0), m_broadphase_handle(type) {
        m_broadphase_handle.set_collider_owner(this);
        m_force_collide_ppu = 0;
        m_shape_size_dma = 0;
    }
    inline phys_collider(phys_fixup_tag flag) : m_broadphase_handle(flag) {
        if (flag.m_fixup_finishing) {
            m_broadphase_handle.set_collider_owner(this);
        }
    }
    __forceinline void set_proxy_shape(const phys_shape *shape) { m_proxy_shape = shape; }
    __forceinline void *get_collider_owner() const { return const_cast<char *>(reinterpret_cast<const char *>(this) + m_owner_offset); }
    __forceinline void set_collider_owner(void *owner) {
        int owner_offset = phys_get_byte_offset_int(this, owner);
        m_owner_offset = static_cast<signed char>(owner_offset);
    }
    __forceinline const phys_typed_broadphase_handle *get_broadphase_handle() const { return &m_broadphase_handle; }
    __forceinline phys_typed_broadphase_handle *get_broadphase_handle() { return &m_broadphase_handle; }
    __forceinline unsigned int get_collision_filter_info() const { return m_broadphase_handle.get_collision_filter_info(); }
    __forceinline void set_collision_filter_info(unsigned int info) { m_broadphase_handle.set_collision_filter_info(info); }
    __forceinline phys_collider_quality get_quality_type() const { return static_cast<phys_collider_quality>(m_broadphase_handle.m_quality_type); }
    __forceinline void set_quality_type(phys_collider_quality t) { m_broadphase_handle.m_quality_type = static_cast<unsigned short>(t); }
    __forceinline int get_type_id() const { return m_broadphase_handle.get_type_id(); }
    __forceinline float get_allowed_penetration_depth() const { return m_allowed_penetration_depth; }
    __forceinline void set_allowed_penetration_depth(float v) { m_allowed_penetration_depth = v; }

protected:
    signed char m_owner_offset;

public:
    unsigned char m_force_collide_ppu;
    unsigned short m_shape_size_dma;
    phys_typed_broadphase_handle m_broadphase_handle;
    float m_allowed_penetration_depth;
};

struct phys_pair_agent_entry;

class phys_linked_collider : public phys_collider {
public:
    inline phys_linked_collider(const phys_shape *shape, const phys_motion_state *motion_state, int type)
        : phys_collider(shape, motion_state, type) {}
    inline phys_linked_collider(phys_fixup_tag flag) : phys_collider(flag) {}

    struct collision_entry {
        phys_pair_agent_entry *m_agent_entry;
        phys_linked_collider *m_linked_partner;
    };

    phys_array<collision_entry> m_collision_entries;
};

struct phys_hit_point {
    __forceinline phys_hit_point(const phys_collision_proxy &a, const phys_collision_proxy &b) : m_proxy_a(a), m_proxy_b(b) {}

    phys_contact_point m_hit_contact;
    const phys_collision_proxy &m_proxy_a;
    const phys_collision_proxy &m_proxy_b;
};

struct phys_root_hit_point {
    PHYS_DECLARE_NONVIRTUAL_ALLOCATOR(PHYS_MEM_COLLIDE, phys_root_hit_point);

    phys_contact_point m_hit_contact;
    const phys_collider *m_root_collider_a;
    phys_shape_key m_shape_key_a;
    const phys_collider *m_root_collider_b;
    phys_shape_key m_shape_key_b;

    inline phys_bool operator<(const phys_root_hit_point &other) const;
};

inline phys_bool phys_root_hit_point::operator<(const phys_root_hit_point &other) const {
    return m_hit_contact.get_distance() < other.m_hit_contact.get_distance();
}

struct phys_collision_input;

class phys_collider_pair_filter {
public:
    virtual ~phys_collider_pair_filter() {}
    virtual phys_bool allows_collision(const phys_collider &a, const phys_collider &b) const = 0;
};

class phys_compound_shape_filter {
public:
    virtual phys_bool allows_collision(const phys_collision_input &input, const phys_collision_proxy &agent_body_a,
        const phys_collision_proxy &compound_proxy_b,
        const phys_shape_container &collector_container_b, phys_shape_key container_key_b) const = 0;
    virtual ~phys_compound_shape_filter() {}
};

class phys_ray_compound_shape_filter {
public:
    virtual phys_bool allows_collision(const phys_ray_input &input, const phys_shape &shape, const phys_shape_container &container,
        phys_shape_key key) const = 0;
    virtual ~phys_ray_compound_shape_filter() {}
};

class phys_ray_collider_filter {
public:
    virtual ~phys_ray_collider_filter() {}
    virtual phys_bool allows_collision(const phys_world_ray_input &input, const phys_collider &collidable) const = 0;
};

class phys_collision_filter : public phys_ref_object, public phys_collider_pair_filter, public phys_compound_shape_filter,
                          public phys_ray_compound_shape_filter, public phys_ray_collider_filter {
public:
    enum phys_filter_type {
        filter_kind_unknown = 0,
        filter_kind_null = 1,
        filter_kind_group = 2,
        filter_kind_list = 3,
        filter_kind_custom = 4,
        filter_kind_pair = 5,
        filter_kind_constraint = 6
    };

    PHYS_DECLARE_ALLOCATOR(PHYS_MEM_COLLISION_INFO);

    __forceinline phys_collision_filter() { m_filter_kind = filter_kind_unknown; }
    virtual phys_bool allows_collision(const phys_collider &a, const phys_collider &b) const = 0;
    virtual phys_bool allows_collision(const phys_collision_input &input, const phys_collision_proxy &a, const phys_collision_proxy &b,
        const phys_shape_container &collector_container_b, phys_shape_key container_key_b) const = 0;
    virtual phys_bool allows_collision(const phys_ray_input &input, const phys_shape &shape, const phys_shape_container &container,
        phys_shape_key key) const = 0;
    virtual phys_bool allows_collision(const phys_world_ray_input &input, const phys_collider &collidable) const = 0;

    __declspec(align(16)) phys_enum<phys_filter_type, unsigned int> m_filter_kind;
    unsigned int m_padding[3];
};

class phys_null_collision_filter : public phys_collision_filter {
public:
    phys_null_collision_filter();
    __forceinline phys_null_collision_filter(phys_filter_type type) { m_filter_kind = type; }
    __forceinline phys_null_collision_filter(phys_fixup_tag) { m_filter_kind = filter_kind_null; }
    ~phys_null_collision_filter();
    virtual phys_bool allows_collision(const phys_collider &a, const phys_collider &b) const { return true; }
    virtual phys_bool allows_collision(const phys_collision_input &input, const phys_collision_proxy &a, const phys_collision_proxy &b,
        const phys_shape_container &collector_container_b, phys_shape_key container_key_b) const { return true; }
    virtual phys_bool allows_collision(const phys_ray_input &input, const phys_shape &shape, const phys_shape_container &container,
        phys_shape_key key) const { return true; }
    virtual phys_bool allows_collision(const phys_world_ray_input &input, const phys_collider &collidable) const { return true; }
};

class phys_ray_hit_collector {
public:
    __forceinline phys_ray_hit_collector() : m_early_out_hit_frac(1.0f) {}
    __forceinline void reset() { m_early_out_hit_frac = 1.0f; }
    virtual void add_ray_hit(const phys_collision_proxy &collision_proxy, const phys_ray_collector_output &ray_hit_info) = 0;
    virtual __forceinline ~phys_ray_hit_collector() {}

    static int shape_keys_from_proxy(phys_shape_key *buf, int max_keys, const phys_collision_proxy &body);

    float m_early_out_hit_frac;
};

class phys_all_hits_collector : public phys_hit_collector {
public:
    static __forceinline void *operator new(size_t nbytes) {
        return phys_thread_memory::get_singleton_instance().allocate_chunk_const_size(static_cast<int>(nbytes), PHYS_MEM_AGENT);
    }
    static __forceinline void operator delete(void *p);
    static __forceinline void *operator new(size_t, void *p) { return p; }
    static __forceinline void operator delete(void *, void *) {}
    __forceinline phys_all_hits_collector() { reset(); }
    virtual ~phys_all_hits_collector();
    __forceinline void reset() {
        m_hit_list.clear();
        phys_hit_collector::reset();
    }
    __forceinline const phys_array<phys_root_hit_point> &get_hits() const { return m_hit_list; }
    __forceinline phys_array<phys_root_hit_point> &get_hits() { return m_hit_list; }
    __forceinline phys_bool has_hit() const { return phys_bool(m_hit_list.getSize() > 0); }
    void sort_hits();

protected:
    virtual void add_hit(const phys_hit_point &event);
    phys_inline_array<phys_root_hit_point, 8> m_hit_list;
};

typedef char phys_collide_size_aabb_u32[(sizeof(phys_aabb_u32) == 0x20) ? 1 : -1];
typedef char phys_collide_size_sphere[(sizeof(phys_sphere) == 0x10) ? 1 : -1];
typedef char phys_collide_size_contact[(sizeof(phys_contact_point) == 0x20) ? 1 : -1];
typedef char phys_collide_size_contact_material[(sizeof(void *) != 4 || ((sizeof(phys_contact_point_material) == 8))) ? 1 : -1];
typedef char phys_collide_size_shape[(sizeof(void *) != 4 || ((sizeof(phys_shape) == 0x10) )) ? 1 : -1];
typedef char phys_collide_size_convex[(sizeof(void *) != 4 || ((sizeof(phys_convex_shape) == 0x14) )) ? 1 : -1];
typedef char phys_collide_size_capsule[(sizeof(void *) != 4 || ((sizeof(phys_capsule_shape) == 0x40))) ? 1 : -1];
typedef char phys_collide_size_single_container[(sizeof(void *) != 4 || ((sizeof(phys_single_shape_container) == 8) )) ? 1 : -1];
typedef char phys_collide_size_collision_proxy[(sizeof(void *) != 4 || ((sizeof(phys_collision_proxy) == 0x10) )) ? 1 : -1];
typedef char phys_collide_size_collider[(sizeof(void *) != 4 || ((sizeof(phys_collider) == 0x24) )) ? 1 : -1];

#include "phys/phys_collision_quality_info.h"

class phys_contact_manager;
struct phys_process_collision_input;

struct phys_narrow_agent_input {
    phys_padded<const phys_collision_proxy *> m_body_a;
    phys_padded<const phys_collision_proxy *> m_body_b;
    phys_padded<const phys_process_collision_input *> m_input;
    phys_padded<phys_contact_manager *> m_contact_manager;
    phys_transform m_a_to_b;
};

struct phys_narrow_agent_process_input : public phys_narrow_agent_input {
    phys_padded<float> m_dist_at_t1;
    phys_padded<int> m_sector_offset_delta;
    phys_vec4 m_lin_sweep_info;
};
typedef char phys_collide_size_narrow_agent_input[(sizeof(void *) != 4 || ((sizeof(phys_narrow_agent_input) == 0x50) )) ? 1 : -1];
typedef char phys_collide_size_narrow_agent_process[(sizeof(void *) != 4 || ((sizeof(phys_narrow_agent_process_input) == 0x70) )) ? 1 : -1];

struct phys_process_hit_point {
    phys_contact_point m_hit_contact;
    phys_padded<unsigned int> m_process_contact_id;
};
typedef char phys_collide_size_process_point[(sizeof(phys_process_hit_point) == 0x30) ? 1 : -1];

class phys_triangle_shape;
class phys_simplex_cache {
public:
    unsigned short m_vertices[4];
    unsigned char m_dim_a;
    unsigned char m_dim_b;
    enum max_dim { DIM_LIMIT_MAX = 255 };
    unsigned char m_max_dim_a;
    unsigned char m_max_dim_b;
    void init(const phys_convex_shape *, const phys_convex_shape *, const phys_transform &);
    void init_triangle(const phys_convex_shape *, const phys_triangle_shape *, const phys_transform &);
    __forceinline void flip() {
        unsigned char h;
        h = m_max_dim_a; m_max_dim_a = m_max_dim_b; m_max_dim_b = h;
        h = m_dim_a; m_dim_a = m_dim_b; m_dim_b = h;
        int last = m_dim_a + m_dim_b - 1;
        unsigned short k;
        k = m_vertices[0]; m_vertices[0] = m_vertices[last]; m_vertices[last] = k;
        if ((m_dim_a & m_dim_b) == 2) {
            k = m_vertices[1]; m_vertices[1] = m_vertices[2]; m_vertices[2] = k;
        }
    }
};

class __declspec(align(16)) phys_simplex_cache16 : public phys_simplex_cache {
public:
    __forceinline void set(const phys_simplex_cache16 &other) { *this = other; }
};

class phys_constraint_owner;
struct phys_agent_entry;

struct phys_process_collision_data {
    struct toi_info {
        __forceinline toi_info() : m_time(3.40282e+38f) {}
        void flip();
        phys_contact_point m_contact_point;
        phys_padded<float> m_time;
        phys_padded<float> m_separating_speed;
        phys_simplex_cache16 m_simplex_cache;
        phys_contact_point_material16 m_properties;
    };
    __forceinline phys_process_collision_data(phys_constraint_owner *owner) { m_constraint_owner = owner; }
    __forceinline int get_process_contact_count() const { return int(m_next_free_contact - &m_process_contact_points[0]); }
    __forceinline phys_process_hit_point &get_process_contact_point(int index) { return m_process_contact_points[index]; }
    __forceinline phys_process_hit_point *get_first_contact_point() { return &m_process_contact_points[0]; }
    __forceinline phys_process_hit_point *get_track_end() { return m_next_free_contact; }
    __forceinline phys_bool is_array_empty() const { return m_next_free_contact == &m_process_contact_points[0]; }
    __forceinline phys_bool has_toi() const { return m_collision_toi.m_time != 3.40282e+38f; }
    __forceinline phys_contact_point &get_toi_contact_point() { return m_collision_toi.m_contact_point; }
    __forceinline float get_toi() const { return m_collision_toi.m_time; }
    __forceinline phys_contact_point_material &get_toi_properties() { return m_collision_toi.m_properties; }
    phys_padded<phys_process_hit_point *> m_next_free_contact;
    phys_padded<phys_constraint_owner *> m_constraint_owner;
    phys_process_hit_point m_process_contact_points[256];
    toi_info m_collision_toi;
};

struct phys_process_collision_output : public phys_process_collision_data {
    __forceinline phys_process_collision_output(phys_constraint_owner *owner) : phys_process_collision_data(owner) {}
    __forceinline void reset() {
        m_next_free_contact = &m_process_contact_points[0];
        m_collision_toi.m_time = 3.40282e+38f;
        m_potential_contacts = 0;
    }
    __forceinline phys_process_hit_point *reserve_contact_points(int count = 1) { return m_next_free_contact; }
    __forceinline void commit_contact_points(int count = 1) { m_next_free_contact = m_next_free_contact + count; }
    __forceinline void abort_contact_points(int count = 1) {}
    struct contact_ref {
        phys_process_hit_point *m_contact_point;
        phys_agent_entry *m_agent_entry;
        void *m_agent_data;
    };
    struct potential_info {
        __forceinline void reset() {
            m_next_free_potential = &m_potential_contacts[0];
            m_next_free_representative = &m_representative_contacts[0];
        }
        contact_ref *m_next_free_potential;
        phys_process_hit_point **m_next_free_representative;
        phys_process_hit_point *m_representative_contacts[256];
        contact_ref m_potential_contacts[256];
    };
    phys_padded<potential_info *> m_potential_contacts;
};
typedef char phys_collide_size_simplex_cache[(sizeof(phys_simplex_cache) == 0xc) ? 1 : -1];
typedef char phys_collide_size_toi_info[(sizeof(phys_process_collision_data::toi_info) == 0x50) ? 1 : -1];
typedef char phys_collide_size_process_data[(sizeof(phys_process_collision_data) == 0x3060) ? 1 : -1];
typedef char phys_collide_size_process_output[(sizeof(phys_process_collision_output) == 0x3070) ? 1 : -1];
class phys_constraint_owner;
class phys_simplex_cache;
struct phys_process_collision_data;
struct phys_process_collision_output;

class phys_contact_manager : public phys_ref_object {
public:
    enum Type {
        CONTACT_MANAGER_SIMPLE_CONSTRAINT,
        CONTACT_MANAGER_REPORT,
        CONTACT_MANAGER_CONVEX_LIST,
        CONTACT_MANAGER_NULL,
        CONTACT_MANAGER_USER,
        CONTACT_MANAGER_TYPE_COUNT
    };
    enum toi_verdict { TOI_EVENT_ACCEPT, TOI_EVENT_REJECT };
    PHYS_DECLARE_ALLOCATOR(PHYS_MEM_CONTACT);
    __forceinline phys_contact_manager(Type type) : m_type(type) {}
    virtual __forceinline ~phys_contact_manager() {}
    __forceinline phys_contact_point_id add_manifold_contact_point(const phys_collision_proxy &a, const phys_collision_proxy &b,
        const phys_process_collision_input &input, phys_process_collision_output &output,
        const phys_simplex_cache *cache, phys_contact_point &point) {
        return add_contact_point_impl(a, b, input, output, cache, point);
    }
    __forceinline phys_result reserve_contact_points(int count) { return reserve_contact_points_impl(count); }
    __forceinline void contact_remove_point(phys_contact_point_id id, phys_constraint_owner &owner) { remove_contact_point_impl(id, owner); }
    __forceinline void process_contact(const phys_collider &a, const phys_collider &b,
        const phys_process_collision_input &input, phys_process_collision_data &data) { process_contact_impl(a, b, input, data); }
    __forceinline toi_verdict add_toi(const phys_collision_proxy &a, const phys_collision_proxy &b,
        const phys_process_collision_input &input, phys_process_collision_output &output, float time,
        phys_contact_point &point, const phys_simplex_cache *cache, float &velocity, phys_contact_point_material &material) {
        return add_toi_impl(a, b, input, output, time, point, cache, velocity, material);
    }
    __forceinline void remove_toi(const phys_collider &a, const phys_collider &b,
        phys_constraint_owner &owner, phys_contact_point_material &material) { remove_toi_impl(a, b, owner, material); }
    virtual phys_contact_point_id add_contact_point_impl(const phys_collision_proxy &, const phys_collision_proxy &,
        const phys_process_collision_input &, phys_process_collision_output &, const phys_simplex_cache *, phys_contact_point &) = 0;
    virtual phys_result reserve_contact_points_impl(int count) = 0;
    virtual void remove_contact_point_impl(phys_contact_point_id id, phys_constraint_owner &owner) = 0;
    virtual void process_contact_impl(const phys_collider &, const phys_collider &,
        const phys_process_collision_input &, phys_process_collision_data &) = 0;
    virtual toi_verdict add_toi_impl(const phys_collision_proxy &, const phys_collision_proxy &,
        const phys_process_collision_input &, phys_process_collision_output &, float,
        phys_contact_point &, const phys_simplex_cache *, float &, phys_contact_point_material &) = 0;
    virtual void remove_toi_impl(const phys_collider &, const phys_collider &,
        phys_constraint_owner &, phys_contact_point_material &) = 0;
    virtual void cleanup();
    Type m_type;
};
typedef char phys_collide_size_contact_manager[(sizeof(void *) != 4 || ((sizeof(phys_contact_manager) == 0xc) )) ? 1 : -1];

typedef char phys_collide_offset_quality_continuous[(offsetof(phys_collision_quality_info, m_use_ccd) == 0x10) ? 1 : -1];
typedef char phys_collide_offset_quality_edge_weld[(offsetof(phys_collision_quality_info, m_toi_weld_reject) == 0x3a) ? 1 : -1];
typedef char phys_collide_offset_narrow_agent_xform[(sizeof(void *) != 4 || ((offsetof(phys_narrow_agent_input, m_a_to_b) == 0x10) )) ? 1 : -1];
typedef char phys_collide_offset_process_point_id[(offsetof(phys_process_hit_point, m_process_contact_id) == 0x20) ? 1 : -1];
typedef char phys_collide_offset_cache_max_a[(offsetof(phys_simplex_cache, m_max_dim_a) == 0xa) ? 1 : -1];
typedef char phys_collide_offset_cache_max_b[(offsetof(phys_simplex_cache, m_max_dim_b) == 0xb) ? 1 : -1];
typedef char phys_collide_offset_data_points[(offsetof(phys_process_collision_data, m_process_contact_points) == 0x10) ? 1 : -1];
typedef char phys_collide_offset_data_toi[(offsetof(phys_process_collision_data, m_collision_toi) == 0x3010) ? 1 : -1];
typedef char phys_collide_offset_toi_cache[(offsetof(phys_process_collision_data::toi_info, m_simplex_cache) == 0x30) ? 1 : -1];
typedef char phys_collide_offset_toi_properties[(offsetof(phys_process_collision_data::toi_info, m_properties) == 0x40) ? 1 : -1];
typedef char phys_collide_offset_output_potentials[(offsetof(phys_process_collision_output, m_potential_contacts) == 0x3060) ? 1 : -1];
typedef char phys_collide_offset_contact_manager_type[(sizeof(void *) != 4 || ((offsetof(phys_contact_manager, m_type) == 8) )) ? 1 : -1];

#endif
