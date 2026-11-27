#ifndef INCLUDE_PHYS_BOX_BOX_MANIFOLD_H
#define INCLUDE_PHYS_BOX_BOX_MANIFOLD_H

// hkFeatureContactPoint and hkBoxBoxManifold, defined in tu/hkcollide/hkBoxBoxManifold.cpp.

#include "phys/phys_base_types.h"
#include "phys/phys_thread_memory.h"
#include "phys/phys_math.h"

class phys_collision_proxy;

class phys_feature_contact_point {
public:
    unsigned char m_feature_a;
    unsigned char m_feature_b;
    unsigned short m_process_contact_id;
    __forceinline phys_feature_contact_point() : m_process_contact_id(0) {}
};

class phys_box_box_manifold {
public:
    PHYS_DECLARE_NONVIRTUAL_ALLOCATOR(PHYS_MEM_MANIFOLD, phys_box_box_manifold);
    // March 0x8349e970 reads the byte at 0x21.
    __forceinline int get_manifold_point_count() { return m_contact_point_count; }
    // March 0x835637b8 and 0x8349e988.
    __forceinline phys_bool has_no_points_left() { return m_contact_point_count >= 8; }
    __forceinline void set_complete(phys_bool complete) { m_manifold_complete = complete; }
    __forceinline phys_bool is_complete() { return m_manifold_complete; }
    __forceinline phys_feature_contact_point &operator[](int index) { return m_process_contact_points[index]; }
    phys_feature_contact_point m_process_contact_points[8];
    unsigned char m_face_contact_count;
    unsigned char m_contact_point_count;
    phys_bool m_manifold_complete;
    phys_bool m_manifold_normal_ready;
    // Cached manifold normals, checked for consistency when the manifold is reused.
    unsigned int m_manifold_normal_b;
    phys_vec4 m_manifold_normal_a;
    phys_bool find_in_manifold(const phys_feature_contact_point &point) {
        for (int i = m_contact_point_count - 1; i >= 0; --i) {
            if (m_process_contact_points[i].m_feature_a == point.m_feature_a &&
                m_process_contact_points[i].m_feature_b == point.m_feature_b) {
                return true;
            }
        }
        return false;
    }
    phys_box_box_manifold();
    int add_manifold_point(const phys_collision_proxy &agent_body_a, const phys_collision_proxy &agent_body_b, phys_feature_contact_point &point);
    void bridge_remove_point(int index);
};

typedef char phys_box_box_manifold_size[(sizeof(phys_box_box_manifold) == 0x40) ? 1 : -1];
typedef char phys_box_box_manifold_normal_b[(offsetof(phys_box_box_manifold, m_manifold_normal_b) == 0x24) ? 1 : -1];
typedef char phys_box_box_manifold_normal_a[(offsetof(phys_box_box_manifold, m_manifold_normal_a) == 0x30) ? 1 : -1];

#endif // HAVOK_HK_BOX_BOX_MANIFOLD_H
