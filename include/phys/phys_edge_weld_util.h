#ifndef INCLUDE_PHYS_EDGE_WELD_UTIL_H
#define INCLUDE_PHYS_EDGE_WELD_UTIL_H

#include "phys/phys_base_types.h"

class phys_vec4;

class phys_edge_weld_util {
public:
    enum edge_weld_kind {
        EDGE_WELD_ANTICLOCKWISE = 0,
        EDGE_WELD_CLOCKWISE = 4,
        EDGE_WELD_TWO_SIDED = 5,
        EDGE_WELD_NONE = 6
    };
    enum weld_sector_kind { WELD_ACCEPT_A = 1, WELD_SNAP_A = 0, WELD_REJECT = 2, WELD_SNAP_B = 4, WELD_ACCEPT_B = 3 };
    enum weld_angle_count { EDGE_WELD_ANGLE_COUNT = 31 };
    struct trig_table_entry {
        float m_cos_accept0;
        float m_sin_accept_0;
        float m_cos_snap0;
        float m_cos_snap1;
        float m_cos_accept1;
        float m_sin_accept_1;
    };
    static trig_table_entry m_sin_cos_table[EDGE_WELD_ANGLE_COUNT + 1];
    static void init_edge_weld_table(float edge_normal_snap_angle, float tri_normal_snap_angle);
    static __forceinline weld_sector_kind get_sector(const phys_vec4 &edge_weld_triangle_normal, const phys_vec4 &collision_normal, int weld_edge_bitcode);
    static __forceinline void compute_snap_vector(const phys_vec4 &edge_weld_triangle_normal, const phys_vec4 &edge, int weld_edge_bitcode,
        weld_sector_kind sector, phys_vec4 &snap_vec_out);
    static __forceinline void snap_collision_normal(const phys_vec4 &edge_weld_triangle_normal, const phys_vec4 &edge, int weld_edge_bitcode,
        weld_sector_kind sector, phys_vec4 &collision_normal_in_out);
    static __forceinline phys_bool should_snap_one_sided(edge_weld_kind edge_weld_type, const phys_vec4 &edge_weld_triangle_normal,
        const phys_vec4 &collision_normal, int weld_edge_bitcode);
    static __forceinline void compute_one_sided_snap_vector(const phys_vec4 &edge_weld_triangle_normal, const phys_vec4 &edge, int weld_edge_bitcode,
        edge_weld_kind edge_weld_type, phys_vec4 &snap_vec_out);
};

#endif
