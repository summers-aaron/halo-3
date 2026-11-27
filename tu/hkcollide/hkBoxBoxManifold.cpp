// hkcollide/hkBoxBoxManifold.obj: from the records 0x639bb8 0x639c18 0x639c50 0x639cd0
// @flags /TP /O2 /Oc
// @address 0x639bb8 ?find_in_manifold@phys_box_box_manifold@@QAA?AVphys_bool@@ABVphys_feature_contact_point@@@Z
// @address 0x639c18 ??0phys_box_box_manifold@@QAA@XZ
// @address 0x639cd0 ?bridge_remove_point@phys_box_box_manifold@@QAAXH@Z
#include "phys/phys_base_types.h"
// @address 0x639c50 ?add_manifold_point@phys_box_box_manifold@@QAAHABVphys_collision_proxy@@0AAVphys_feature_contact_point@@@Z
#include "phys/phys_box_box_manifold.h"
phys_box_box_manifold::phys_box_box_manifold()
{
    m_contact_point_count = 0;
    m_face_contact_count = 0;
    m_manifold_complete = false;
}
int phys_box_box_manifold::add_manifold_point(const phys_collision_proxy &agent_body_a, const phys_collision_proxy &agent_body_b,
    phys_feature_contact_point &point)
{
    unsigned int count = m_contact_point_count;
    if (count > 8)
        return -1;
    if (find_in_manifold(point))
        return -1;
    int index = count;
    if (index < 8) {
        m_process_contact_points[index] = point;
        ++m_contact_point_count;
        return index;
    }
    return -1;
}
void phys_box_box_manifold::bridge_remove_point(int index)
{
    m_manifold_complete = false;
    m_process_contact_points[index] = m_process_contact_points[m_contact_point_count - 1];
    --m_contact_point_count;
}

