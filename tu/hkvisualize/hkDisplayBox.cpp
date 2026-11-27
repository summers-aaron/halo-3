// hkvisualize/hkDisplayBox.obj: from the records 0x6b7330 0x6b7398 0x6b7448 0x6b74d8 0x6b7518 0x6b7678
// @compiler xdk6130
// @flags /TP /O2 /Oc /GF /EHs-c- /GR-
// @address ??_Gphys_display_geometry@@UAAPAXI@Z
// @address 0x6b7398 ??0phys_display_box@@QAA@ABVphys_vec4@@@Z
// @address 0x6b7448 ??0phys_display_box@@QAA@XZ
// @address 0x6b74d8 ?set_box_params@phys_display_box@@QAAXABVphys_vec4@@ABVphys_transform@@@Z
// @address 0x6b7518 ?get_wireframe_geometry@phys_display_box@@UAAXAAV?$phys_array@Vphys_vec4@@@@@Z
// @address 0x6b7678 ?build_display_geometry@phys_display_box@@UAAXXZ
// @address 0x6b7330 ??_Gphys_display_box@@UAAPAXI@Z
#define PHYS_CONFIG_SIMD 1
#include "physics/havok_memory.h"

#include "phys/phys_display_box.h"
phys_display_box::phys_display_box(const phys_vec4& box_half_extents)
:	phys_display_geometry(PHYS_DISPLAY_BOX),
    m_half_extents(box_half_extents)
{
    m_half_extents = box_half_extents;
}

phys_display_box::phys_display_box()
:	phys_display_geometry(PHYS_DISPLAY_BOX)
{
    m_half_extents.set_zero4();
}

void phys_display_box::set_box_params(const phys_vec4& box_half_extents, const phys_transform& t)
{
    m_half_extents = box_half_extents;
    m_body_transform = t;
}

void phys_display_box::build_display_geometry()
{
    m_geometry = new phys_geometry;

    m_geometry->m_vertices.expand_by(1)->set(-m_half_extents(0), m_half_extents(1), m_half_extents(2));
    m_geometry->m_vertices.expand_by(1)->set(m_half_extents(0), m_half_extents(1), m_half_extents(2));
    m_geometry->m_vertices.expand_by(1)->set(m_half_extents(0), -m_half_extents(1), m_half_extents(2));
    m_geometry->m_vertices.expand_by(1)->set(-m_half_extents(0), -m_half_extents(1), m_half_extents(2));
    m_geometry->m_vertices.expand_by(1)->set(-m_half_extents(0), m_half_extents(1), -m_half_extents(2));
    m_geometry->m_vertices.expand_by(1)->set(m_half_extents(0), m_half_extents(1), -m_half_extents(2));
    m_geometry->m_vertices.expand_by(1)->set(m_half_extents(0), -m_half_extents(1), -m_half_extents(2));
    m_geometry->m_vertices.expand_by(1)->set(-m_half_extents(0), -m_half_extents(1), -m_half_extents(2));

    for (int i=0; i < m_geometry->m_vertices.getSize(); i++)
    {
        m_geometry->m_vertices[i].vec_set_transformed_pos( m_body_transform, m_geometry->m_vertices[i] );
    }

    m_geometry->m_triangle_list.expand_by(1)->set(3, 2, 1);
    m_geometry->m_triangle_list.expand_by(1)->set(3, 1, 0);
    m_geometry->m_triangle_list.expand_by(1)->set(6, 7, 4);
    m_geometry->m_triangle_list.expand_by(1)->set(6, 4, 5);
    m_geometry->m_triangle_list.expand_by(1)->set(4, 7, 3);
    m_geometry->m_triangle_list.expand_by(1)->set(4, 3, 0);
    m_geometry->m_triangle_list.expand_by(1)->set(2, 6, 5);
    m_geometry->m_triangle_list.expand_by(1)->set(2, 5, 1);
    m_geometry->m_triangle_list.expand_by(1)->set(7, 6, 2);
    m_geometry->m_triangle_list.expand_by(1)->set(7, 2, 3);
    m_geometry->m_triangle_list.expand_by(1)->set(1, 5, 4);
    m_geometry->m_triangle_list.expand_by(1)->set(1, 4, 0);

}

void phys_display_box::get_wireframe_geometry(phys_array<phys_vec4>& lines)
{
    lines.set_array_size(24);

    phys_vec4 points[8];

    {
        for (int i = 0; i < 8; i++)
        {
            phys_vec4 v = m_half_extents;
            if ( i & 1 )	v(0) *= -1.0f;
            if ( i & 2 )	v(1) *= -1.0f;
            if ( i & 4 )	v(2) *= -1.0f;
            points[i].vec_set_transformed_pos(m_body_transform, v);
        }
    }

    {
        int k = 0;
        for (int i = 0; i < 8; i++)
        {
            for ( int bit = 1; bit < 8; bit <<= 1 )
            {
                int j = i ^ bit;

                if ( i < j )
                {
                    lines[k++] = points[i];
                    lines[k++] = points[j];
                }
            }
        }
    }

}

