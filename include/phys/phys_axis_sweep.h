#ifndef INCLUDE_PHYS_AXIS_SWEEP_H
#define INCLUDE_PHYS_AXIS_SWEEP_H

#include "phys/phys_broadphase.h"
#include "phys/phys_typed_broadphase_handle_pair.h"

class phys_axis_sweep : public phys_broadphase {
public:
    struct phys_sweep_aabb
    {
        __declspec(align(16)) unsigned int m_min_limit[3];
        __declspec(align(16)) unsigned int m_max_limit[3];
    };

    class phys_sweep_marker {
    public:
        __forceinline static void *operator new(size_t, void *);
        __forceinline phys_sweep_marker();
        unsigned short m_node_idx;
        unsigned short m_value;
        phys_array<unsigned short> m_overlapping_objects;
    };
    class phys_sweep_node {
    public:
        static const int s_memberOffsets[6];
        unsigned short min_y, lower_z, max_y, max_z_val, lower_x, max_x_val;
        phys_broadphase_handle *m_handle;
        __forceinline void set_elem(int axis, int min_or_max, int value) {
            unsigned short *p = (unsigned short *)((char *)&min_y + s_memberOffsets[axis * 2 + min_or_max]);
            p[0] = (unsigned short)value;
        }
        __forceinline unsigned int yz_disjoint(const phys_sweep_node &other) const {
            unsigned int min_a_bits = *(const unsigned int *)&min_y;
            unsigned int min_b_bits = *(const unsigned int *)&other.min_y;
            unsigned int max_bits_a = *(const unsigned int *)&max_y;
            unsigned int max_bits_b = *(const unsigned int *)&other.max_y;
            max_bits_a -= min_b_bits;
            max_bits_b -= min_a_bits;
            max_bits_a |= max_bits_b;
            max_bits_a &= 0x80008000;
            return max_bits_a;
        }
        __forceinline int xz_disjoint(const phys_sweep_node &other) const {
            short a = max_x_val - other.lower_x;
            short b = max_z_val - other.lower_z;
            short c = other.max_x_val - lower_x;
            short d = other.max_z_val - lower_z;
            a |= b;
            c |= d;
            c |= a;
            return c & 0x8000;
        }
        __forceinline int xy_disjoint(const phys_sweep_node &other) const {
            unsigned short a = max_y - other.min_y;
            unsigned short b = max_x_val - other.lower_x;
            unsigned short c = other.max_y - min_y;
            unsigned short d = other.max_x_val - lower_x;
            a |= b;
            c |= d;
            c |= a;
            return c & 0x8000;
        }
        __forceinline int is_marker() const { return static_cast<int>(reinterpret_cast<intptr_t>(m_handle) & 1); }
        __forceinline phys_sweep_marker *get_marker(phys_sweep_marker *markers) const {
            return reinterpret_cast<phys_sweep_marker *>(reinterpret_cast<char *>(markers) +
                (reinterpret_cast<intptr_t>(m_handle) & ~static_cast<intptr_t>(1)));
        }
        __forceinline unsigned short &get_min_endpoint(int index) {
            if (index == 0) return lower_x;
            if (index == 1) return min_y;
            return lower_z;
        }
        __forceinline unsigned short &get_max_endpoint(int index) {
            if (index == 0) return max_x_val;
            if (index == 1) return max_y;
            return max_z_val;
        }
    };
    class phys_sweep_endpoint {
    public:
        unsigned short m_value;
        unsigned short m_node_idx;
        __forceinline int is_max_point() const { return m_value & 1; }
        static __forceinline int is_max_point(int value) { return value & 1; }
    };
    class phys_sweep_axis {
    public:
        static __forceinline void *operator new(size_t, void *storage) { return storage; }
        phys_array<phys_sweep_endpoint> m_sweep_endpoints;
        phys_sweep_axis() {}
        phys_sweep_axis(phys_sweep_endpoint *ep, int initial_array_size) {
            m_sweep_endpoints.m_data = ep;
            m_sweep_endpoints.m_size = 0;
            m_sweep_endpoints.m_capacity_and_flags = initial_array_size | PHYS_ARRAY_DONT_DEALLOCATE_FLAG;
        }
        const phys_sweep_endpoint *find(const phys_sweep_endpoint *first, const phys_sweep_endpoint *last, unsigned short key) const;
        void merge_endpoint_batch(phys_sweep_node *nodes, int new_endpoints_idx, int sweep_new_count, int axis, phys_sweep_endpoint *scratch);
        void remove(int sweep_min_index, int sweep_max_index);
        void remove_node_batch(phys_sweep_node *nodes, int axis, const phys_fixed_array<int> &node_relocations);
        void insert(phys_sweep_node *nodes, int sweep_node_idx, unsigned short sweep_min_value, unsigned short sweep_max_value,
            unsigned short &sweep_min_index, unsigned short &sweep_max_index);
    };
    phys_aabb m_original_aabb;
    phys_vec4 m_offset_low;
    phys_vec4 m_offset_high;
    phys_vec4 m_scale;
    phys_array<phys_sweep_node> m_nodes;
    phys_sweep_axis m_axis[3];
    int m_sweep_marker_count;
    int m_log2_marker_count;
    phys_sweep_marker *m_markers;
    float m_int_to_float_floor_fix;
    phys_axis_sweep(const phys_vec4 &min, const phys_vec4 &max, int sweep_marker_count);
    virtual ~phys_axis_sweep();
    virtual void gather_memory_stats(phys_statistics_collector *collector) const;
    const phys_aabb &get_original_aabb() const;
    virtual void add_broadphase_object(phys_broadphase_handle *object, const phys_aabb &aabb, phys_array<phys_broadphase_handle_pair> &pairs);
    virtual void add_object_batch(phys_array<phys_broadphase_handle*> &objects_to_add, phys_array<phys_aabb> &aabbs_to_add,
        phys_array<phys_broadphase_handle_pair> &new_pairs);
    virtual void remove_broadphase_object(phys_broadphase_handle *object, phys_array<phys_broadphase_handle_pair> &pairs);
    virtual void remove_broadphase_object_batch(phys_array<phys_broadphase_handle*> &handles_to_remove,
        phys_array<phys_broadphase_handle_pair> &sweep_removed_pairs);
    virtual int get_object_count();
    virtual void update_aabbs(phys_broadphase_handle *objects[], const phys_aabb *aabb_list, int sweep_object_count,
        phys_array<phys_broadphase_handle_pair> &sweep_added_pairs, phys_array<phys_broadphase_handle_pair> &erased_sweep_pairs);
    virtual void defragment_sweep();
    virtual void get_all_aabbs(phys_array<phys_aabb> &aabb_list);
    virtual void get_handle_aabb(const phys_broadphase_handle *handle, phys_aabb &aabb) const;
    virtual void get_extents(phys_vec4 &world_min, phys_vec4 &world_max) const;
    virtual void query_single_aabb(const phys_aabb &aabb, phys_array<phys_broadphase_handle_pair> &pairs) const;
    virtual void requery_single_object(const phys_broadphase_handle *handle, phys_array<phys_broadphase_handle_pair> &pairs) const;
    virtual void shift_all_objects(const phys_vec4 &shift_distance, phys_vec4 &effective_shift_out, phys_array<phys_broadphase_handle*> &new_collision_pairs);
    virtual void shift_broadphase(const phys_vec4 &shift_distance, phys_vec4 &effective_shift_out, phys_array<phys_broadphase_handle*> &new_collision_pairs);
    virtual void trace_ray(const phys_ray_cast_input &input, phys_broadphase_cast_collector *cast_collector_base, int collector_stride) const;
    virtual int get_aabb_cache_size() const;
    static void __fastcall begin_overlap(phys_sweep_node &sweep_node_a, phys_sweep_node &sweep_node_b, phys_array<phys_broadphase_handle_pair> &pairs);
    static void __fastcall end_overlap(phys_sweep_node &sweep_node_a, phys_sweep_node &sweep_node_b, phys_array<phys_broadphase_handle_pair> &pairs);
    static void __fastcall begin_overlap_check_marker(
        phys_sweep_marker *markers, phys_sweep_node &sweep_node_a, int sweep_node_idx, phys_sweep_node &sweep_node_b,
            phys_array<phys_broadphase_handle_pair> &pairs);
    struct value_int_pair {
        unsigned short m_value;
        unsigned short m_old_idx;
        __forceinline phys_bool operator<(const value_int_pair &other) const { return m_value < other.m_value; }
    };
    __forceinline int get_marker_count() const { return m_sweep_marker_count; }
    void convert_aabb_to_int(const phys_aabb &aabb, phys_sweep_aabb &aabb_out) const;
    void convert_vector_to_int(const phys_vec4 &vec, unsigned int *out_ints) const;
    static float s_axis_max_value[4];
    enum phys_sweep_marker_use { PHYS_SWEEP_NO_MARKER, PHYS_SWEEP_USE_MARKER };
protected:
    void fix_nodes_after_insert(phys_sweep_node *nodes, int sweep_node_count, phys_sweep_node &new_sweep_node);
    void fix_nodes_after_delete(phys_sweep_node *nodes, int sweep_node_count, phys_sweep_node &old_node);
    void set_bits_from_x_interval(int sweep_node_count, int sweep_x_value, const phys_sweep_node &node, unsigned short sweep_node_idx,
        unsigned int *bits) const;
    __forceinline void query_single_aabb_add(phys_broadphase_handle *object, int new_node_index, const unsigned int *bit_field,
        phys_sweep_node &ref_node, phys_array<phys_broadphase_handle_pair> &node_pairs_out) const;
    __forceinline void query_single_aabb_remove(phys_broadphase_handle *object, int new_node_index, const unsigned int *bit_field,
        phys_sweep_node &ref_node, phys_array<phys_broadphase_handle_pair> &node_pairs_out) const;
    enum phys_sweep_query_kind { PHYS_SWEEP_REPORT_HANDLES, PHYS_SWEEP_REPORT_NODES };
    __forceinline void query_single_aabb_sub(phys_broadphase_handle *object, const unsigned int *bit_field, phys_sweep_node &ref_node,
        phys_sweep_query_kind type, phys_array<phys_broadphase_handle_pair> *node_pairs_out, phys_array<const phys_sweep_node *> *nodes_out) const;
    __forceinline void query_single_bounds(const phys_aabb &aabb_in, phys_sweep_query_kind type, phys_array<phys_broadphase_handle_pair> *node_pairs_out,
        phys_array<const phys_sweep_node *> *nodes_out) const;
    void get_aabb_from_node(const phys_sweep_node &node, phys_aabb &aabb) const;
    void compute_bounds_cache_internal(const phys_array<const phys_sweep_node *> &overlap_nodes, char *bounds_cache_out) const;
    void query_batch_aabb_sub(unsigned int *bit_field, phys_array<phys_broadphase_handle_pair> &batch_pairs_out, phys_bool add_or_remove_overlaps) const;
    __forceinline void add_node_pair(const phys_sweep_node *n0, const phys_sweep_node *n1,
        phys_array<phys_broadphase_handle_pair> &batch_pairs_out, phys_bool pair_add_mode) const;
public:
  void check_sweep_consistency();
  virtual void compute_bounds_cache(const phys_aabb &aabb, char *bounds_cache_out) const;
  virtual void compute_bounds_cache(phys_array<phys_collider *> const &p0, char *p1) const;
  virtual void cast_bounds(const phys_aabb_cast_input &input, phys_broadphase_cast_collector &collector) const;
public:
  static void __fastcall end_overlap_check_marker(class phys_axis_sweep::phys_sweep_marker *p0, class phys_axis_sweep::phys_sweep_node &p1, int p2,
      class phys_axis_sweep::phys_sweep_node &p3, class phys_array<class phys_broadphase_handle_pair> &p4);
};

#endif
