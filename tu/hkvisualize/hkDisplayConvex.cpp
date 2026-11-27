// hkvisualize/hkDisplayConvex.obj: from the records 0x6b7060 0x6b7068 0x6b70f0
// @compiler xdk6130
// @flags /TP /O2 /Oc /GF /EHs-c- /GR-
// @address 0x6b7060 ?build_display_geometry@phys_display_convex@@UAAXXZ
// @address 0x6b7068 ??0phys_display_convex@@QAA@PAUphys_geometry@@@Z
// @address 0x6b70f0 ?get_wireframe_geometry@phys_display_convex@@UAAXAAV?$phys_array@Vphys_vec4@@@@@Z
#define PHYS_CONFIG_SIMD 1

#include "phys/phys_display_convex.h"
phys_display_convex::phys_display_convex(phys_geometry* wire_body_cap)
:	phys_display_geometry(PHYS_DISPLAY_CONVEX)
{
	m_geometry = wire_body_cap;
}

void phys_display_convex::build_display_geometry()
{
  
}

void phys_display_convex::get_wireframe_geometry(phys_array<phys_vec4>& lines)
{
	
	if(m_geometry != 0)
	{
		long v[3];
	
		for(int i = m_geometry->m_triangle_list.getSize()-1; i >= 0;  i--)
		{
			v[0] = m_geometry->m_triangle_list[i].m_a;
			v[1] = m_geometry->m_triangle_list[i].m_b;
			v[2] = m_geometry->m_triangle_list[i].m_c;
			
			lines.array_push_back(m_geometry->m_vertices[ v[0] ]);
			lines.array_push_back(m_geometry->m_vertices[ v[1] ]);

			lines.array_push_back(m_geometry->m_vertices[ v[0] ]);
			lines.array_push_back(m_geometry->m_vertices[ v[2] ]);			

			lines.array_push_back(m_geometry->m_vertices[ v[1] ]);
			lines.array_push_back(m_geometry->m_vertices[ v[2] ]);
		}
	}
}

