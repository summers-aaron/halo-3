// hkvisualize/hkDisplayCapsule.obj: from the records 0x6b8830 0x6b8838 0x6b88e0
// @compiler xdk6130
// @flags /TP /O2 /Oc /GF /EHs-c- /GR-
// @address 0x6b8830 ?get_wireframe_geometry@phys_display_capsule@@UAAXAAV?$phys_array@Vphys_vec4@@@@@Z
// @address 0x6b8838 ??0phys_display_capsule@@QAA@ABVphys_vec4@@0MHH@Z
// @address 0x6b88e0 ?build_display_geometry@phys_display_capsule@@UAAXXZ
#include "physics/havok_memory.h"
#include "phys/phys_display_capsule.h"

// @address 0x14d50 ?phys_simd_vec_0010@@3U__vector4@@B
// @rdata 0x82014d50..0x82014d60
const __vector4 phys_simd_vec_0010 = { 0.0f, 0.0f, 1.0f, 0.0f };
// @rdata-end

phys_display_capsule::phys_display_capsule( const phys_vec4& top, const phys_vec4& bottom, phys_real radius, int side_count , int height_segment_count )
:	phys_display_geometry(PHYS_DISPLAY_CAPSULE), 
	m_side_count(side_count), m_height_segment_count(height_segment_count)
{
	m_capsule_top = top;
	m_capsule_bottom = bottom;
	m_radius = radius; 
}


void phys_display_capsule::get_wireframe_geometry(phys_array<phys_vec4>& lines)
{
	
}


// match: 0x6b88e0: buildGeometry matches only unoptimized (11684B; /O2 shrinks it to 3168B)
#pragma optimize("",off)
void phys_display_capsule::build_display_geometry()
{
	const phys_vec4& start = m_capsule_top;
	const phys_vec4& end = m_capsule_bottom;
	const phys_real radius = m_radius;

	const int slice_samples_steps	= m_height_segment_count;
	const int sweep_samples_val	= m_side_count;
	const int lat_segs_dome	= sweep_samples_val >> 1;

	phys_array<phys_vec4> verts;

	phys_transform body_to_tm;
	
	phys_vec4 axis;
	axis.set_sub4(end, start);
	phys_real height = axis.length_3d();
	if(height > 0.0f)
	{
		axis.normalize_xyz();

		phys_vec4 canonical_up_z; canonical_up_z = phys_simd_vec_0010;
		if(phys_math::fabs(axis.dot3(canonical_up_z)) < 1 - 1e-5f)
		{
			phys_rotation rotation;
			phys_vec4& col0 = rotation.get_column(0);
			phys_vec4& col1 = rotation.get_column(1);
			phys_vec4& col2 = rotation.get_column(2);

			col2 = axis;
			col1.set_cross( col2, canonical_up_z); 
			col1.normalize_xyz();
			col0.set_cross( col1, col2 );
			body_to_tm.set_rotation(rotation);
		}
		else
		{
			body_to_tm.set_to_identity();	
		}

	}
	else
	{
		body_to_tm.set_to_identity();
	}

	{
		phys_vec4 half_shift_loc;
		half_shift_loc.set_add4(start, end);
		half_shift_loc.mul4(0.5f);
		body_to_tm.set_translation(half_shift_loc);
	}
	
	int i,j;

	phys_vec4 dome_point_vtx;

	phys_vec4 floor_loc_cap; floor_loc_cap.set(0,0,-height/2);
	phys_vec4 head_vec_origin; head_vec_origin.set(0,0,height/2);
	phys_vec4 mesh_span_frame;
	mesh_span_frame.set_sub4(head_vec_origin, floor_loc_cap);
	phys_vec4 stem_geom_ref; stem_geom_ref.set(0,0,1);
	phys_vec4 rad_tan_local; rad_tan_local.set(1,0,0);
	phys_vec4 ortho_dir_ref; ortho_dir_ref.set(0,-1,0);

	verts.reserve_exactly(2 * lat_segs_dome * sweep_samples_val + 2 + (slice_samples_steps-1) * sweep_samples_val);

	dome_point_vtx.set(0, 0 ,height*0.5f + radius);
	dome_point_vtx.vec_set_transformed_pos(body_to_tm, dome_point_vtx);
	verts.array_push_back(dome_point_vtx);

	for (i = lat_segs_dome-1 ; i >= 0; i--)
	{
		phys_quat up_upper_step(ortho_dir_ref, phys_real(i) / lat_segs_dome * 3.14159265358979f * .5f);
		phys_vec4 tip_ring_lat;
		tip_ring_lat.vec_set_rotated_dir(up_upper_step, rad_tan_local);

		for (j = 0; j < sweep_samples_val; j++)
		{
			phys_quat quat_north_rot(stem_geom_ref, phys_real(j) / sweep_samples_val * 3.14159265358979f * 2);			
			phys_vec4 tip_unit;
			tip_unit.vec_set_rotated_dir(quat_north_rot, tip_ring_lat);

			phys_real dist = radius;
			dome_point_vtx.set_add_mul4(head_vec_origin, tip_unit, dist);

			dome_point_vtx.vec_set_transformed_pos(body_to_tm, dome_point_vtx);

			verts.array_push_back(dome_point_vtx);

		}
	}

	for (j = slice_samples_steps-1; j > 0; j--)
	{
	
		for (i = 0; i < sweep_samples_val; i++)
		{	
		
			phys_quat q(stem_geom_ref, phys_real(i) / sweep_samples_val * 3.14159265358979f * 2);
			phys_vec4 direction;
			direction.vec_set_rotated_dir(q, rad_tan_local);
			
			phys_vec4 s;
			s.set_add_mul4(floor_loc_cap, mesh_span_frame, phys_real(j) / phys_real(slice_samples_steps));

			phys_real dist = radius;

			dome_point_vtx.set_add_mul4(s, direction, dist);

			dome_point_vtx.vec_set_transformed_pos(body_to_tm, dome_point_vtx);

			verts.array_push_back(dome_point_vtx);

		}
	}

	for (i = 0; i < lat_segs_dome; i++)
	{
		phys_quat q_base_dome(ortho_dir_ref, -phys_real(i) / lat_segs_dome * 3.14159265358979f * .5f);
		phys_vec4 bottom_dip_out;
		bottom_dip_out.vec_set_rotated_dir(q_base_dome, rad_tan_local);

		for (j = 0; j < sweep_samples_val; j++)
		{
			phys_quat yaw_twist_quat(stem_geom_ref, phys_real(j) / sweep_samples_val * 3.14159265358979f * 2);			
			phys_vec4 low_spoke_dir;
			low_spoke_dir.vec_set_rotated_dir(yaw_twist_quat, bottom_dip_out);

			phys_real dist = radius;

			dome_point_vtx.set_add_mul4(floor_loc_cap, low_spoke_dir, dist);
			dome_point_vtx.vec_set_transformed_pos(body_to_tm, dome_point_vtx);
			verts.array_push_back(dome_point_vtx);
		}
	}

	dome_point_vtx.set(0, 0 , -(height*0.5f + radius));
	dome_point_vtx.vec_set_transformed_pos(body_to_tm, dome_point_vtx);

	verts.array_push_back(dome_point_vtx);

	phys_geometry* wire_body_cap = new phys_geometry;
	wire_body_cap->m_vertices = verts;
	
	{
		for ( int vi = 0; vi < wire_body_cap->m_vertices.getSize(); ++vi )
		{
			wire_body_cap->m_vertices[vi].vec_set_transformed_pos(m_body_transform, wire_body_cap->m_vertices[vi]);
		}
	}

	phys_geometry::geometry_triangle tr;

	int strip_ring_start = 1;
	for (i = 0; i < sweep_samples_val; i++)
	{
		tr.m_a = 0;
		tr.m_b = strip_ring_start + i;
		tr.m_c = strip_ring_start + (i+1)%(sweep_samples_val);

		wire_body_cap->m_triangle_list.array_push_back(tr);
	}

	for(j = 0; j < 2*(lat_segs_dome-1) + slice_samples_steps; j++)
	{
		for (i = 0; i < sweep_samples_val; i++)
		{
			tr.m_a = strip_ring_start + i;
			tr.m_b = strip_ring_start + sweep_samples_val + i;
			tr.m_c = strip_ring_start + sweep_samples_val + (i+1)%(sweep_samples_val);

			wire_body_cap->m_triangle_list.array_push_back(tr);

			tr.m_b = strip_ring_start + i;
			tr.m_a = strip_ring_start + (i+1)%(sweep_samples_val);
			tr.m_c = strip_ring_start + sweep_samples_val + (i+1)%(sweep_samples_val);
		
			wire_body_cap->m_triangle_list.array_push_back(tr);

		}
		strip_ring_start += sweep_samples_val;
	}
	
	for (i = 0; i < sweep_samples_val; i++)
	{
		tr.m_b = strip_ring_start + i;
		tr.m_a = strip_ring_start + (i+1)%(sweep_samples_val);
		tr.m_c = strip_ring_start + sweep_samples_val;

		wire_body_cap->m_triangle_list.array_push_back(tr);
	}
	
	m_geometry = wire_body_cap;
}

// match: 0x6b88e0: re-enable optimization after unoptimized buildGeometry
#pragma optimize("",on)

