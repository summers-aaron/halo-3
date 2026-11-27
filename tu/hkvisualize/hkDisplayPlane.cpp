// hkvisualize/hkDisplayPlane.obj: from the records 0x6bb688 0x6bb690 0x6bb698 0x6bb6a8 0x6bb768 0x6bb7a0 0x6bb8b8 0x6bb980 0x6bba88
// @compiler xdk6130
// @flags /TP /O2 /Oc
// @address 0x6bb688 ?get_contact_normal@phys_display_plane@@QAAAAVphys_vec4@@XZ
// @address 0x6bb690 ?get_plane_center@phys_display_plane@@QAAAAVphys_vec4@@XZ
// @address 0x6bb698 ?get_perp_to_normal@phys_display_plane@@QAAAAVphys_vec4@@XZ
#include <vectorintrinsics.h>
#include "physics/havok_memory.h"
// @address 0x6bb768 ?set_box_params@phys_display_plane@@QAAXABVphys_vec4@@000@Z
// @address 0x6bba88 ?build_display_geometry@phys_display_plane@@UAAXXZ

#include "phys/phys_base.h"
#include "phys/phys_display_geometry.h"
#include "phys/phys_display_plane.h"
#include "phys/phys_statistics_collector.h"
#include "phys/phys_thread_memory.h"

phys_vec4 &phys_display_plane::get_contact_normal()
{
    return m_normal;
}

phys_vec4 &phys_display_plane::get_plane_center()
{
    return m_plane_center;
}

phys_vec4 &phys_display_plane::get_perp_to_normal()
{
    return m_perp_to_normal;
}

void phys_display_plane::set_box_params(const phys_vec4 &normal, const phys_vec4 &perp_to_normal, const phys_vec4 &center, const phys_vec4 &extent)
{
    m_normal = normal;
    m_plane_center = center;
    m_perp_to_normal = perp_to_normal;
    m_plane_extent = extent;
}

void phys_display_plane::build_display_geometry()
{
    m_geometry = new phys_geometry;
    m_geometry->m_vertices.set_array_size(5);
    m_geometry->m_vertices[4] = m_plane_center;
    make_plane_points(&m_geometry->m_vertices[0]);
    m_geometry->m_triangle_list.expand_by(1)->set(2, 4, 3);
    m_geometry->m_triangle_list.expand_by(1)->set(0, 4, 2);
    m_geometry->m_triangle_list.expand_by(1)->set(1, 4, 0);
    m_geometry->m_triangle_list.expand_by(1)->set(3, 4, 1);
}

// @address 0x6bb6a8 ?make_plane_points@phys_display_plane@@IAAXPAVphys_vec4@@@Z
void phys_display_plane::make_plane_points(phys_vec4 *points)
{
    phys_vec4 other_dir;
    other_dir.set_cross(m_normal, m_perp_to_normal);

    for (int i = 0; i < 4; i++)
    {
        points[i] = m_plane_center;
    }

    phys_vec4 offset1;
    phys_vec4 plane_offset2;

    offset1.set_mul4(m_plane_extent, other_dir);
    plane_offset2.set_mul4(m_plane_extent, m_perp_to_normal);

    points[0].add4(offset1);
    points[0].add4(plane_offset2);

    points[1].add4(offset1);
    points[1].sub4(plane_offset2);

    points[2].sub4(offset1);
    points[2].add4(plane_offset2);

    points[3].sub4(offset1);
    points[3].sub4(plane_offset2);
}

// @address 0x6bb980 ?get_wireframe_geometry@phys_display_plane@@UAAXAAV?$phys_array@Vphys_vec4@@@@@Z
void phys_display_plane::get_wireframe_geometry(phys_array<phys_vec4> &lines)
{
  lines.set_array_size(12);

  phys_vec4 points[4];

  make_plane_points(points);

  lines[0] = points[0];
  lines[1] = points[1];

  lines[2] = points[1];
  lines[3] = points[2];

  lines[4] = points[2];
  lines[5] = points[3];

  lines[6] = points[3];
  lines[7] = points[0];

  lines[8] = points[0];
  lines[9] = points[2];

  lines[10] = points[1];
  lines[11] = points[3];
}

// @address 0x6bb7a0 ??0phys_display_plane@@QAA@ABVphys_vec4@@000@Z
phys_display_plane::phys_display_plane(const phys_vec4 &normal, const phys_vec4 &perp_to_normal, const phys_vec4 &center, const phys_vec4 &extent)
    : phys_display_geometry(PHYS_DISPLAY_PLANE),
      m_normal(normal),
      m_plane_center(center),
      m_perp_to_normal(perp_to_normal),
      m_plane_extent(extent)
{
}

// @address 0x6bb8b8 ??0phys_display_plane@@QAA@XZ
phys_display_plane::phys_display_plane()
    : phys_display_geometry(PHYS_DISPLAY_PLANE)
{
    m_plane_extent.set_zero4();
    m_normal.set_zero4();
    m_plane_center.set_zero4();
    m_perp_to_normal.set_zero4();
}

