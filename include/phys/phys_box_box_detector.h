#ifndef INCLUDE_PHYS_BOX_BOX_DETECTOR_H
#define INCLUDE_PHYS_BOX_BOX_DETECTOR_H

#include "phys/phys_collide.h"
#include "phys/phys_box_box_manifold.h"

class phys_box_box_detector
{
public:
    __forceinline phys_box_box_detector(const phys_collision_proxy &agent_body_a, const phys_collision_proxy &agent_body_b,
        const phys_process_collision_input *collision_env, phys_contact_manager *pair_contact_manager, phys_process_collision_output *result,
        const phys_transform &box_a_from_b, const phys_transform &world_from_a, const phys_vec4 &radius_a,
        const phys_transform &world_from_b, const phys_vec4 &radius_b, float tolerance);
    phys_bool get_penetrations();
    phys_result check_box_intersection(const phys_vec4 &tolerance) const;
    phys_bool compute_closest_point(phys_contact_point &contact) const;
    void compute_box_manifold(phys_box_box_manifold &box_manifold) const;

protected:
    enum { PHYS_BOX_BOX_FACE_A_Z = 2, PHYS_BOX_BOX_FACE_B_X = 4, PHYS_BOX_BOX_FACE_B_Z = 6 };
    static const float m_manifold_cos_tolerance;
    static const float m_coplanar_ang_cos_tolerance;
    static const float m_normal_ang_cos_tolerance;
    static const float m_edge_end_tolerance;
    static const int m_max_rejected_features;
    class phys_feature_point_cache
    {
    public:
        phys_vec4 m_vec_a;
        phys_vec4 m_vec_b;
        phys_vec4 m_normal_a;
        float m_normal_sign;
        float m_distance;
        int m_feature_idx_a;
        int m_feature_idx_b;
    };

    inline void init_work_vars() const
    {
        m_din_a = m_a_to_b.get_translation();
        m_din_b.set_rotated_inverse_dir(m_a_to_b.get_rotation(), m_din_a);
    }
    int find_closest_point(phys_box_box_manifold &box_manifold, phys_feature_contact_point &feature_contact,
        phys_feature_point_cache &feature_point_cache) const;
    void check_manifold_completeness(phys_box_box_manifold &box_manifold, int plane_mask_a, int plane_mask_b) const;
    phys_bool is_valid_edge_edge(phys_feature_point_cache &feature_point_cache) const;
    __forceinline void face_a_vertex_b_validate_idx(phys_feature_point_cache &feature_point_cache, int feature_idx) const;
    __forceinline void face_b_vertex_a_validate_idx(phys_feature_point_cache &feature_point_cache, int feature_idx) const;
    __forceinline void edge_edge_validate_from_idx(phys_feature_point_cache &feature_point_cache) const;
    __forceinline phys_bool is_valid_face_a_vertex_b(phys_feature_point_cache &feature_point_cache) const;
    __forceinline phys_bool is_valid_face_b_vertex_a(phys_feature_point_cache &feature_point_cache) const;
    void add_extra_edge_helper(phys_box_box_manifold &box_manifold, phys_feature_contact_point &feature_contact, float closest_point_dist) const;
    void try_add_edge_point(phys_box_box_manifold &box_manifold, int edge_a, int edge_b, int next_vert_a, int next_vert_b, const phys_vec4 &box_normal_a,
        const phys_vec4 &box_normal_b, float closest_point_dist) const;

    __forceinline void edge_edge_validate_from_id(phys_feature_point_cache &feature_point_cache, const phys_feature_contact_point &feature_contact) const
    {
        feature_point_cache.m_feature_idx_a = feature_contact.m_feature_a;
        feature_point_cache.m_feature_idx_b = feature_contact.m_feature_b;
    }
    __forceinline void face_a_vertex_b_contact(
        phys_process_hit_point &contact_hit_out, const phys_feature_contact_point &feature_contact, const phys_feature_point_cache &feature_point_cache) const;
    __forceinline void face_b_vertex_a_contact(
        phys_process_hit_point &contact_hit_out, const phys_feature_contact_point &feature_contact, const phys_feature_point_cache &feature_point_cache) const;
    __forceinline void edge_edge_contact_from_cache(
        phys_process_hit_point &contact_hit_out, const phys_feature_contact_point &feature_contact, const phys_feature_point_cache &feature_point_cache) const;
    __forceinline void contact_from_feature_cache(phys_process_hit_point &contact_hit_out,
        const phys_feature_contact_point &feature_contact, const phys_feature_point_cache &feature_point_cache) const;
    __forceinline phys_result add_manifold_point(phys_box_box_manifold &box_manifold,
        const phys_feature_point_cache &feature_point_cache, phys_feature_contact_point &feature_contact) const;
    __forceinline void bridge_remove_point(phys_box_box_manifold &box_manifold, int index) const;
    __forceinline void face_a_vertex_b_validate_id(phys_feature_point_cache &feature_point_cache, const phys_feature_contact_point &feature_contact) const;
    __forceinline void face_b_vertex_a_validate_id(phys_feature_point_cache &feature_point_cache, const phys_feature_contact_point &feature_contact) const;
    __forceinline void refresh_manifold(phys_box_box_manifold &box_manifold) const;
    __forceinline phys_bool check_manifold_normals(phys_box_box_manifold &box_manifold) const;
    __forceinline void check_manifold_normal_consistency(phys_box_box_manifold &box_manifold) const;
    __forceinline void compute_manifold_normal(phys_vec4 &manifold_normal, const phys_feature_contact_point &feature_contact,
        phys_feature_point_cache &feature_point_cache, phys_bool is_point_validated) const;
    __forceinline void try_add_point_face_a(
        phys_box_box_manifold &box_manifold, const phys_feature_contact_point feature_contact_template, unsigned short plane_mask,
            float closest_point_dist) const;
    __forceinline void try_add_point_face_b(
        phys_box_box_manifold &box_manifold, const phys_feature_contact_point feature_contact_template, unsigned short plane_mask,
            float closest_point_dist) const;
    __forceinline void find_extra_manifold_points(phys_box_box_manifold &box_manifold, phys_feature_contact_point feature_contact_a_template) const;
    __forceinline void set_vd_proj(const phys_rotation &b_rot_a, int i, int j, phys_vec4 &vd_proj) const;

    const phys_collision_proxy &m_body_a;
    const phys_collision_proxy &m_body_b;
    const phys_process_collision_input *m_collide_env;
    phys_contact_manager *m_contact_manager;
    phys_process_collision_output *m_result;
    const phys_transform &m_world_from_a;
    const phys_transform &m_world_from_b;
    phys_transform m_a_to_b;
    phys_vec4 m_radius_a;
    phys_vec4 m_radius_b;
    phys_vec4 m_box_tolerance4;
    phys_vec4 m_keep_radius_a;
    phys_vec4 m_keep_radius_b;
    const float m_box_tolerance;
    const float m_boundary_tolerance;
    mutable phys_vec4 m_din_a;
    mutable phys_vec4 m_din_b;
    mutable phys_vec4 m_sep_dist[5];
};
typedef char phys_box_box_detect_size[(sizeof(void *) != 4 || sizeof(phys_box_box_detector) == 304) ? 1 : -1];

inline phys_box_box_detector::phys_box_box_detector(const phys_collision_proxy &agent_body_a, const phys_collision_proxy &agent_body_b,
    const phys_process_collision_input *collision_env, phys_contact_manager *pair_contact_manager, phys_process_collision_output *result,
    const phys_transform &box_a_from_b, const phys_transform &world_from_a, const phys_vec4 &radius_a,
    const phys_transform &world_from_b, const phys_vec4 &radius_b, float tolerance)
    : m_body_a(agent_body_a), m_body_b(agent_body_b), m_collide_env(collision_env), m_contact_manager(pair_contact_manager), m_result(result),
      m_world_from_a(world_from_a), m_world_from_b(world_from_b), m_a_to_b(box_a_from_b), m_radius_a(radius_a), m_radius_b(radius_b),
      m_box_tolerance(tolerance), m_boundary_tolerance(0.01f)
{
    m_box_tolerance4.flags_set_all(tolerance);
    m_keep_radius_a.set_add4(m_box_tolerance4, m_radius_a);
    m_keep_radius_b.set_add4(m_box_tolerance4, m_radius_b);
}

#endif
