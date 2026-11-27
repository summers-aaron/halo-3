// hkvisualize/hkDisplayCylinder.obj: from the records 0x6b8000 0x6b8008 0x6b80b0
// @compiler xdk6130
// @flags /TP /O2 /Oc /GF /EHs-c- /GR-
// @address 0x6b8000 ?get_wireframe_geometry@phys_display_cylinder@@UAAXAAV?$phys_array@Vphys_vec4@@@@@Z
// @address 0x6b8008 ??0phys_display_cylinder@@QAA@ABVphys_vec4@@0MHH@Z
// @address 0x6b80b0 ?build_display_geometry@phys_display_cylinder@@UAAXXZ
// @address 0x6f4ca4 __restvmx_123
// @address 0x6f4a0c __savevmx_123
#include "physics/havok_memory.h"
#include "phys/phys_display_cylinder.h"

// @address 0x82014d30
const __vector4 phys_simd_vec_1000 = { 1.0f, 0.0f, 0.0f, 0.0f };

// @address 0x82014d40
const __vector4 phys_simd_vec_0100 = { 0.0f, 1.0f, 0.0f, 0.0f };

phys_display_cylinder::phys_display_cylinder( const phys_vec4& top, const phys_vec4& bottom, phys_real radius, int side_count , int height_segment_count )
:	phys_display_geometry(PHYS_DISPLAY_CYLINDER), 
	m_side_count(side_count), m_height_segment_count(height_segment_count)
{
	m_capsule_top = top;
	m_capsule_bottom = bottom;
	m_radius = radius; 
}

void phys_display_cylinder::build_display_geometry()
{
	
	int& height_segments = m_height_segment_count;
	int& side_segments   = m_side_count;

	{
		
		phys_real height;
		{
			phys_vec4 segment;
			segment.set_sub4(m_capsule_top, m_capsule_bottom);
			height = segment.length_3d();
		}

		const phys_vec4& base = m_capsule_bottom;
		
		phys_vec4 axis;
		axis.set_sub4(m_capsule_top, m_capsule_bottom);
		axis.mul4( 1.0f / height_segments );

		phys_vec4 unit_axis = axis;
		unit_axis.normalize_xyz();
		
		phys_vec4 radius;
		{
			phys_vec4 tmp;
			if ( phys_math::fabs(unit_axis(0)) > 0.5f )
			{
				tmp = phys_simd_vec_0100;
			}
			else if ( phys_math::fabs(unit_axis(1)) > 0.5f )
			{
				tmp = phys_simd_vec_0010;
			}
			else if ( phys_math::fabs(unit_axis(2)) > 0.5f )
			{
				tmp = phys_simd_vec_1000;
			}
			radius.set_cross(unit_axis, tmp);
			radius.normalize_xyz();
			radius.mul4(m_radius);
		}
		
		phys_quat step_quat; step_quat.set_axis_angle( unit_axis, 3.14159265358979f * 2.0f / side_segments );
		phys_quat curr_quat; curr_quat.set_axis_angle( unit_axis, 0 );

		m_geometry = new phys_geometry;

		*(m_geometry->m_vertices.expand_by(1)) = base;
		{
			phys_vec4 axis_end;
			axis_end.set_mul4((phys_real)height_segments, axis);
			axis_end.add4(base);
			*(m_geometry->m_vertices.expand_by(1)) = axis_end;
		}

		for (int s = 0; s < side_segments; ++s)
		{
			phys_vec4 point;
			{
				
				phys_vec4 mod_radius;
				mod_radius.vec_set_rotated_dir(curr_quat, radius);
				point.set_add4(base, mod_radius);
			}

			for (int h = 0; h < height_segments; ++h)
			{
				*(m_geometry->m_vertices.expand_by(1)) = point;
				point.add4(axis);
			}
			*(m_geometry->m_vertices.expand_by(1)) = point;

			phys_quat tmp = curr_quat;
			curr_quat.set_mul(tmp, step_quat);
		}
		
	}

	{
		
		const int vert_column_len = height_segments + 1;
		const int all_verts_count = 2 + side_segments * (1 + height_segments);
		
		int indices[4] = { 2+0,
		                   2+vert_column_len,
						   2+1,
		                   2+vert_column_len+1
		                 };

		for (int s = 0; s < side_segments; ++s)
		{
			for (int h = 0; h < height_segments; ++h)
			{
				m_geometry->m_triangle_list.expand_by(1)->set(indices[0], indices[1], indices[2]);
				m_geometry->m_triangle_list.expand_by(1)->set(indices[2], indices[1], indices[3]);
				++indices[0];
				++indices[1];
				++indices[2];
				++indices[3];
			}
			
			++indices[0];
			++indices[1];
			++indices[2];
			++indices[3];
			if (indices[3] >= all_verts_count)
			{
				indices[1] += 2 - all_verts_count;
				indices[3] += 2 - all_verts_count;
			}
		}

		for (int base = 0; base < 2; ++base)
		{
			int start = 2 + (base ? height_segments : 0);
			for (int s = 0; s < side_segments; ++s)
			{
				int next = start + vert_column_len;
				if (next >= all_verts_count)
				{
					next += 2 - all_verts_count;
				}

				if (base)
				{
					m_geometry->m_triangle_list.expand_by(1)->set(base, start, next );
				}
				else
				{
					m_geometry->m_triangle_list.expand_by(1)->set(base, next, start );
				}
				start = next;
			}
		}
	}

}

void phys_display_cylinder::get_wireframe_geometry(phys_array<phys_vec4>& lines)
{
	
}

