// hkutilities/hkInertiaTensorComputer.obj: from the records 0x6132f8 0x6133a0 0x613470 0x6134b8 0x613568 0x613618 0x613668 0x6136b8 0x613968 0x613bb0 0x613d08 0x613dc0 0x6140b8 0x614260 0x614468 0x6146c8 0x614b18 0x614d50 0x614f40 0x615238 0x6158c0 0x616188 0x616b20 0x616e80 0x617360 0x617a58 0x617ce8 0x617e70 0x617ff0 0x618170
// @compiler xdk6130
// @flags /TP /O2 /Oc /GF /EHs-c- /GR-
// @address 0x616188 ?capsule_volume_mass_props@phys_inertia_util@@SA?AW4phys_result@@ABVphys_vec4@@0MMAAUphys_mass_props@@@Z
// @address 0x616e80 ?geometry_surface_mass_props@phys_inertia_util@@SA?AW4phys_result@@PBUphys_geometry@@MVphys_bool@@MAAUphys_mass_props@@@Z
// @address 0x616b20 ?cylinder_volume_mass_props@phys_inertia_util@@SA?AW4phys_result@@ABVphys_vec4@@0MMAAUphys_mass_props@@@Z
// @address 0x6132f8 ?sphere_volume_mass_props@phys_inertia_util@@SA?AW4phys_result@@MMAAUphys_mass_props@@@Z
// @address 0x6133a0 ?box_volume_mass_props@phys_inertia_util@@SA?AW4phys_result@@ABVphys_vec4@@MAAUphys_mass_props@@@Z
// @address 0x613470 ?clip_inertia_ratio@phys_inertia_util@@SAXMAAVphys_rigid_body_desc@@@Z
// @address 0x6134b8 ?shift_inertia_to_com@phys_inertia_util@@SAXAAVphys_vec4@@MAAVphys_mat3@@@Z
// @address 0x613568 ?shift_inertia_from_com@phys_inertia_util@@SAXAAVphys_vec4@@MAAVphys_mat3@@@Z
// @address 0x613618 ?scale_mass_props@@YAXMAAUphys_mass_props@@@Z
// @address 0x613668 ?simplify_inertia_to_particle@phys_inertia_util@@SAXAAVphys_mat3@@@Z
// @address 0x6136b8 ?compute_projection_integrals@inertia_util_internal@@QAAXPBVphys_vec4@@@Z
// @address 0x613968 ?compute_face_integrals@inertia_util_internal@@QAAXPBVphys_vec4@@ABV2@@Z
// @address 0x613bb0 ?compute_inertia_tensor_internal@inertia_util_internal@@QAAXMMAAVphys_vec4@@AAVphys_mat3@@@Z
// @address 0x613d08 ?compute_geometry_aabb@inertia_util_internal@@SAXPBVphys_vec4@@HAAV2@1@Z
// @address 0x613dc0 ?inertia_to_principal_axes@phys_inertia_util@@SAXAAVphys_mat3@@AAVphys_rotation@@@Z
// @address 0x6140b8 ?sphere_surface_mass_props@phys_inertia_util@@SA?AW4phys_result@@MMMAAUphys_mass_props@@@Z
// @address 0x614260 ?box_surface_mass_props@phys_inertia_util@@SA?AW4phys_result@@ABVphys_vec4@@MMAAUphys_mass_props@@@Z
// @address 0x614468 ?combine_mass_props@phys_inertia_util@@SA?AW4phys_result@@ABV?$phys_array@Uphys_mass_element@@@@AAUphys_mass_props@@@Z
// @address 0x6146c8 ?compute_volume_integrals@inertia_util_internal@@QAAXPBUphys_geometry@@ABVphys_vec4@@@Z
// @address 0x614b18 ?geometry_volume_mass_props@phys_inertia_util@@SAXPBUphys_geometry@@MAAUphys_mass_props@@@Z
// @address 0x614d50 ?geometry_volume_mass_props_checked@phys_inertia_util@@SA?AW4phys_result@@PBUphys_geometry@@MAAUphys_mass_props@@@Z
// @address 0x614f40 ?vertex_hull_mass_props@phys_inertia_util@@SA?AW4phys_result@@PBMHHMAAUphys_mass_props@@@Z
// @address 0x615238 ?vertex_cloud_mass_props@phys_inertia_util@@SA?AW4phys_result@@PBMHHMAAUphys_mass_props@@@Z
// @address 0x6158c0 ?triangle_surface_mass_props@phys_inertia_util@@SA?AW4phys_result@@ABVphys_vec4@@00MMAAUphys_mass_props@@@Z
// @address 0x617a58 ?phys_inertia_util_optimize_tree_int@@YAMAAV?$phys_array@PBVphys_constraint@@@@PAVphys_rigid_body@@M@Z
// @address 0x617ce8 ?optimize_tree_inertias@phys_inertia_util@@SAXPBQAVphys_constraint@@HPAVphys_rigid_body@@M@Z
// @address 0x617e70 ?set_shape_volume_mass_props@phys_inertia_util@@SAXPBVphys_shape@@MAAVphys_rigid_body_desc@@@Z
// @address 0x617ff0 ?shape_volume_mass_props@phys_inertia_util@@SAXPBVphys_shape@@MAAUphys_mass_props@@@Z
// @address 0x618170 ??1phys_geometry@@QAA@XZ
// @address 0x42d298 ?havok_override_thread_memory_stack_allocations@@YA_NXZ
// @address 0x617360 ?recursive_shape_mass_props@@YAXPBVphys_shape@@ABVphys_transform@@AAUphys_mass_props@@@Z
#define PHYS_CONFIG_SIMD 1
#include <float.h>
#include "phys/phys_strided_vertices.h"
#include "physics/havok_memory.h"
#include "phys/phys_solver.h"
#include "phys/phys_math_types.h"
#include "phys/phys_mat3.h"
#include "phys/phys_shape_type.h"
#include "phys/phys_vec_constant_types.h"
#include "phys/phys_math_neg_sqrt_mode_types.h"
#include "phys/phys_math_accuracy_mode_types.h"
#include "phys/phys_math_io_mode_types.h"
#include "phys/phys_math_rounding_mode_types.h"
#include "phys/phys_vec_permute_types.h"
#include "phys/phys_math_div_by_zero_mode_types.h"

#include "phys/phys_dynamics.h"
#include "phys/phys_constraint.h"

#include "phys/phys_local_array.h"

#include "phys/phys_local_buffer.h"

#include "phys/phys_sphere.h"

#include "phys/phys_inertia_util.h"



#include "phys/phys_geometry.h"

#include "phys/phys_geometry_util.h"
#include "phys/phys_hull_mode.h" 

#include "phys/phys_collide.h"

#include "phys/phys_transform_shape.h"

#include "phys/phys_aabb_util.h"

#include "phys/phys_convex_hull_shape.h"

#include "phys/phys_packed_hull_shape.h"

#include "phys/phys_triangle_shape.h"

#include "phys/phys_cylinder_shape.h"

#include "phys/phys_bounding_shape.h"

#include "phys/phys_multi_sphere_shape.h"



struct inertia_util_internal
{
	void compute_inertia_tensor_internal(phys_real mass, phys_real density, phys_vec4 &r, phys_mat3 &J);
	void compute_projection_integrals(const phys_vec4* v);
	void compute_face_integrals(const phys_vec4* v,const phys_vec4& n);
	void compute_volume_integrals(const phys_geometry* wire_body_cap,const phys_vec4& shift);
	static void PHYS_CALL compute_geometry_aabb(const phys_vec4* vertices, int hull_vert_count, phys_vec4& center_out, phys_vec4& aabb_extents_out );

	int A;    
	int B;    
	int C;    

	phys_real P1, poly_int_a, poly_int_b, proj_aa, proj_ab, proj_bb, proj_aaa, proj_aab, proj_abb, proj_bbb;

	phys_real face_a, face_b, face_c, face_aa, face_bb, face_cc, face_aaa, face_bbb, face_ccc, face_aab, face_bbc, face_cca;

	phys_real T0, T1[3], T2[3], TP[3];
	
	inline phys_real square_real(phys_real x);
	inline phys_real cube_real(phys_real x);
};


phys_result PHYS_CALL phys_inertia_util::sphere_volume_mass_props( phys_real radius, phys_real mass, phys_mass_props& result)
{
	if(mass <= 0.0f)
	{
		return PHYS_FAILURE;
	}
	if(radius <= 0.0f)
	{
		return PHYS_FAILURE;
	}

	phys_real k = mass * radius * radius * 0.4f;

	result.m_inertia_tensor.set_to_identity();

	result.m_inertia_tensor( 0, 0 ) = k;
	result.m_inertia_tensor( 1, 1 ) = k;
	result.m_inertia_tensor( 2, 2 ) = k;

	result.m_center_of_mass.set_zero4();
	result.m_body_volume = 4.0f/3.0f * 3.14159265358979f * radius * radius * radius;
	result.m_body_mass = mass;

	return PHYS_SUCCESS;
}

phys_result PHYS_CALL phys_inertia_util::capsule_volume_mass_props(
	const phys_vec4 &start_axis, const phys_vec4 &end_axis, phys_real radius, phys_real mass, phys_mass_props &result)
{
	if(mass <= 0.0f)
	{
		return PHYS_FAILURE;
	}
	if(radius <= 0.0f)
	{
		return PHYS_FAILURE;
	}
	phys_vec4 axis;
	axis.set_sub4(end_axis, start_axis);
	phys_real height = axis.length_3d();
	phys_transform body_to_tm;
	if(height > 0.0f)
	{
		axis.normalize_xyz();
		phys_vec4 canonical_up_z; canonical_up_z.set(0,0,1);
		if(phys_math::fabs(axis.dot3(canonical_up_z)) < 1 - 1e-5f)
		{
			phys_vec4 rot_axis;
			rot_axis.set_cross(canonical_up_z, axis);
			rot_axis.normalize_xyz();
			phys_real rot_angle = phys_math::acos(axis.dot3(canonical_up_z));
			phys_quat q;
			q.set_axis_angle(rot_axis, rot_angle);
			body_to_tm.set_rotation(q);
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
		half_shift_loc.set_add4(start_axis, end_axis);
		half_shift_loc.mul4(0.5f);
		body_to_tm.set_translation(half_shift_loc);
	}
	const phys_real caps_volume = 4.0f / 3.0f * 3.14159265358979f * radius * radius * radius;
	const phys_real cylinder_volume = 3.14159265358979f * radius * radius * height;
	const phys_real total_volume = caps_volume + cylinder_volume;
	const phys_real caps_mass = mass * caps_volume / total_volume;
	const phys_real cylinder_mass = mass * cylinder_volume / total_volume;
	phys_inline_array<phys_mass_element,3> elements;
	{
		phys_mass_element cylinder_mass_elem;
		cylinder_mass_elem.m_body_transform = body_to_tm;
		cylinder_mass_elem.m_properties.m_center_of_mass.set(0,0,0);
		cylinder_mass_elem.m_properties.m_inertia_tensor.set_to_identity();
		cylinder_mass_elem.m_properties.m_inertia_tensor(0,0) = radius * radius * 0.25f	+ (height/2 * height/2) / 3.0f;
		cylinder_mass_elem.m_properties.m_inertia_tensor(1,1) = cylinder_mass_elem.m_properties.m_inertia_tensor(0,0);
		cylinder_mass_elem.m_properties.m_inertia_tensor(2,2) = radius * radius * 0.5f;
		cylinder_mass_elem.m_properties.m_inertia_tensor.mul(cylinder_mass);
		cylinder_mass_elem.m_properties.m_center_of_mass.set_zero4();
		cylinder_mass_elem.m_properties.m_body_volume = cylinder_volume;
		cylinder_mass_elem.m_properties.m_body_mass = cylinder_mass;
		elements.array_push_back(cylinder_mass_elem);
	}
	{
		phys_mass_element cap_element;
		cap_element.m_body_transform = body_to_tm;
		phys_vec4 pos; pos.set(0, 0, (height / 2.0f));
		pos.vec_set_rotated_dir(body_to_tm.get_rotation(), pos);
		pos.add4(cap_element.m_body_transform.get_translation());
		cap_element.m_body_transform.set_translation(pos);
		cap_element.m_properties.m_center_of_mass.set(0, 0, 3.0f / 8.0f * radius);
		{
			phys_mass_props sphere_mass_props;
			sphere_volume_mass_props(radius, caps_mass, sphere_mass_props);
			cap_element.m_properties.m_inertia_tensor = sphere_mass_props.m_inertia_tensor;
			cap_element.m_properties.m_inertia_tensor.mul(0.5f);
			shift_inertia_to_com(cap_element.m_properties.m_center_of_mass, caps_mass * 0.5f, cap_element.m_properties.m_inertia_tensor);
		}
		cap_element.m_properties.m_body_volume = caps_volume * 0.5f;
		cap_element.m_properties.m_body_mass = caps_mass * 0.5f;
		elements.array_push_back(cap_element);
	}
	{
		phys_mass_element cap_element;
		cap_element.m_body_transform = body_to_tm;
		phys_vec4 pos; pos.set(0, 0, -(height / 2.0f));
		pos.vec_set_rotated_dir(body_to_tm.get_rotation(), pos);
		pos.add4(cap_element.m_body_transform.get_translation());
		cap_element.m_body_transform.set_translation(pos);
		cap_element.m_properties.m_center_of_mass.set(0, 0, -3.0f / 8.0f * radius);
		{
			phys_mass_props sphere_mass_props;
			sphere_volume_mass_props(radius, caps_mass, sphere_mass_props);
			cap_element.m_properties.m_inertia_tensor = sphere_mass_props.m_inertia_tensor;
			cap_element.m_properties.m_inertia_tensor.mul(0.5f);
			shift_inertia_to_com(cap_element.m_properties.m_center_of_mass, caps_mass * 0.5f, cap_element.m_properties.m_inertia_tensor);
		}
		cap_element.m_properties.m_body_volume = caps_volume * 0.5f;
		cap_element.m_properties.m_body_mass = caps_mass * 0.5f;
		elements.array_push_back(cap_element);
	}
	combine_mass_props(elements, result);
	return PHYS_SUCCESS;
}
phys_result PHYS_CALL phys_inertia_util::geometry_surface_mass_props(
	const phys_geometry *wire_body_cap, phys_real surface_thickness, phys_bool distribute_uniformly, phys_real mass, phys_mass_props &result)
{
	if(mass <= 0.0f)
	{
		return PHYS_FAILURE;
	}
	if(surface_thickness <= 0.0f)
	{
		return PHYS_FAILURE;
	}
	result.m_inertia_tensor.set_zero();
	result.m_center_of_mass.set_zero4();
	int i;
	const phys_real min_twice_area = 1e-5f;
	phys_array<phys_real>	tri_areas;
	tri_areas.set_array_size(wire_body_cap->m_triangle_list.getSize());
	phys_real total_area = 0.0f;
	for(i = 0; i < wire_body_cap->m_triangle_list.getSize(); i++)
	{
		const phys_geometry::geometry_triangle& tri_corners = wire_body_cap->m_triangle_list[i]; 
		phys_vec4 normal;
		phys_vec4 v0; v0 = wire_body_cap->m_vertices[tri_corners.m_a];
		phys_vec4 v1; v1 = wire_body_cap->m_vertices[tri_corners.m_b];
		phys_vec4 v2; v2 = wire_body_cap->m_vertices[tri_corners.m_c];
		{
			phys_vec4 cb;
			phys_vec4 ab;
			cb.set_sub4(v2,v1);
			ab.set_sub4(v0,v1);
			normal.set_cross( cb , ab);
		}
		phys_real twice_area = normal.length_3d();
		if(distribute_uniformly)
		{
			if(twice_area < min_twice_area)
			{
				tri_areas[i] = 0.5f * min_twice_area;
			}
			else
			{
				tri_areas[i] = 0.5f * twice_area;
			}
		}
		else
		{
			tri_areas[i] = 1.0f;
		}
		total_area += tri_areas[i];
	}
	for(i = 0; i < wire_body_cap->m_triangle_list.getSize(); i++)
	{
		const phys_geometry::geometry_triangle& tri_corners = wire_body_cap->m_triangle_list[i]; 
		phys_vec4 v0; v0 = wire_body_cap->m_vertices[tri_corners.m_a];
		phys_vec4 v1; v1 = wire_body_cap->m_vertices[tri_corners.m_b];
		phys_vec4 v2; v2 = wire_body_cap->m_vertices[tri_corners.m_c];
		phys_real tri_mass = mass * tri_areas[i] / total_area;
		phys_mass_props tri_mass_props;
		phys_inertia_util::triangle_surface_mass_props(v0, v1, v2, tri_mass, surface_thickness, tri_mass_props);
		phys_vec4 schema_cursor; schema_cursor = tri_mass_props.m_center_of_mass;
		phys_mat3 it = tri_mass_props.m_inertia_tensor;
		result.m_center_of_mass.add_scaled4(tri_mass, schema_cursor);
		phys_vec4 shift; shift = schema_cursor;
		shift_inertia_from_com(shift, tri_mass, it);
		result.m_inertia_tensor.add(it);			
	}
	result.m_body_mass = mass;
	result.m_center_of_mass.set_mul4(1.0f/mass, result.m_center_of_mass);
	shift_inertia_to_com(result.m_center_of_mass, mass, result.m_inertia_tensor);
	return PHYS_SUCCESS;
}
phys_result PHYS_CALL phys_inertia_util::cylinder_volume_mass_props(
	const phys_vec4 &start_axis, const phys_vec4 &end_axis, phys_real radius, phys_real mass, phys_mass_props &result)
{
	if(mass <= 0.0f)
	{
		return PHYS_FAILURE;
	}
	if(radius <= 0.0f)
	{
		return PHYS_FAILURE;
	}
	phys_vec4 axis;
	axis.set_sub4(end_axis, start_axis);
	phys_real height = axis.length_3d();
	phys_transform body_to_tm;
	if(height > 0.0f)
	{
		axis.normalize_xyz();
		phys_vec4 canonical_up_z; canonical_up_z = phys_transform::get_identity().get_column(2);
		if(phys_math::fabs(axis.dot3(canonical_up_z)) < 1 - 1e-5f)
		{
			phys_vec4 rot_axis;
			rot_axis.set_cross(canonical_up_z, axis);
			rot_axis.normalize_xyz();
			phys_real rot_angle = phys_math::acos(axis.dot3(canonical_up_z));
			phys_quat q;
			q.set_axis_angle(rot_axis, rot_angle);
			body_to_tm.set_rotation(q);
		}
		else
		{
			body_to_tm.set_to_identity();	
		}
	}
	else
	{
		return PHYS_FAILURE;
	}
	{
		phys_vec4 half_shift_loc;
		half_shift_loc.set_add4(start_axis, end_axis);
		half_shift_loc.mul4(0.5f);
		body_to_tm.set_translation(half_shift_loc);
	}
	const phys_real cylinder_volume = 3.14159265358979f * radius * radius * height;
	const phys_real cylinder_mass = mass;
	phys_inline_array<phys_mass_element,1> elements;
	{
		phys_mass_element& cylinder_mass_elem = elements.expand_one();
		cylinder_mass_elem.m_body_transform = body_to_tm;
		cylinder_mass_elem.m_properties.m_center_of_mass.set(0,0,0);
		cylinder_mass_elem.m_properties.m_inertia_tensor.set_to_identity();
		cylinder_mass_elem.m_properties.m_inertia_tensor(0,0) = radius * radius * 0.25f	+ (height/2 * height/2) / 3.0f;
		cylinder_mass_elem.m_properties.m_inertia_tensor(1,1) = cylinder_mass_elem.m_properties.m_inertia_tensor(0,0);
		cylinder_mass_elem.m_properties.m_inertia_tensor(2,2) = radius * radius * 0.5f;
		cylinder_mass_elem.m_properties.m_inertia_tensor.mul(cylinder_mass);
		cylinder_mass_elem.m_properties.m_center_of_mass.set_zero4();
		cylinder_mass_elem.m_properties.m_body_volume = cylinder_volume;
		cylinder_mass_elem.m_properties.m_body_mass = cylinder_mass;
	}
	combine_mass_props(elements, result);
	return PHYS_SUCCESS;
}
phys_result PHYS_CALL phys_inertia_util::sphere_surface_mass_props( phys_real radius, phys_real mass, phys_real surface_thickness, phys_mass_props& result)
{
	if(mass <= 0.0f)
	{
		return PHYS_FAILURE;
	}
	if(radius <= surface_thickness)
	{
		return PHYS_FAILURE;
	}
	if(surface_thickness <= 0.0f)
	{
		return PHYS_FAILURE;
	}

	phys_real shell_outer_radius = radius;
	phys_real shell_core_radius = radius - surface_thickness;
	
	phys_real outer_volume = 4.0f/3.0f * 3.14159265358979f * shell_outer_radius * shell_outer_radius * shell_outer_radius;
	phys_real inner_volume = 4.0f/3.0f * 3.14159265358979f * shell_core_radius * shell_core_radius * shell_core_radius;

	phys_real outer_mass = 1.0f * outer_volume;
	phys_real inner_mass = 1.0f * inner_volume;

	phys_mass_props outer_result;
	phys_mass_props inner_result;

	sphere_volume_mass_props(shell_outer_radius, outer_mass, outer_result);
	sphere_volume_mass_props(shell_core_radius, inner_mass, inner_result);

	result.m_center_of_mass.set_zero4();
	result.m_inertia_tensor = outer_result.m_inertia_tensor;
	result.m_inertia_tensor.sub(inner_result.m_inertia_tensor);

	result.m_body_mass = outer_result.m_body_mass - inner_result.m_body_mass;
	result.m_body_volume = outer_result.m_body_volume - inner_result.m_body_volume;

	scale_mass_props(mass / (outer_volume - inner_volume), result);

	return PHYS_SUCCESS;
}

phys_result PHYS_CALL phys_inertia_util::box_surface_mass_props(
	const phys_vec4 &box_half_extents, phys_real mass, phys_real surface_thickness, phys_mass_props &result)
{
	if(mass <= 0.0f)
	{
		return PHYS_FAILURE;
	}
	if(!((box_half_extents(0) > surface_thickness) && (box_half_extents(1) > surface_thickness) && (box_half_extents(2) > surface_thickness)))
	{
		return PHYS_FAILURE;
	}
	if(surface_thickness <= 0.0f)
	{
		return PHYS_FAILURE;
	}

	phys_vec4 half_extents_small;
	half_extents_small.set((box_half_extents(0) - surface_thickness), (box_half_extents(1) - surface_thickness), (box_half_extents(2) - surface_thickness));

	phys_real outer_volume = 8.0f * box_half_extents(0) * box_half_extents(1) * box_half_extents(2);
	phys_real inner_volume = 8.0f * half_extents_small(0) * half_extents_small(1) * half_extents_small(2);

	phys_real outer_mass = 1.0f * outer_volume;
	phys_real inner_mass = 1.0f * inner_volume;

	phys_mass_props outer_result;
	phys_mass_props inner_result;

	box_volume_mass_props(box_half_extents, outer_mass, outer_result);
	box_volume_mass_props(half_extents_small, inner_mass, inner_result);

	result.m_center_of_mass.set_zero4();
	result.m_inertia_tensor = outer_result.m_inertia_tensor;
	result.m_inertia_tensor.sub(inner_result.m_inertia_tensor);

	result.m_body_mass = outer_result.m_body_mass - inner_result.m_body_mass;
	result.m_body_volume = outer_result.m_body_volume - inner_result.m_body_volume;

	scale_mass_props(mass / (outer_volume - inner_volume), result);

	return PHYS_SUCCESS;
}

phys_result PHYS_CALL phys_inertia_util::triangle_surface_mass_props(
	const phys_vec4 &v0, const phys_vec4 &v1, const phys_vec4 &v2, phys_real mass, phys_real surface_thickness, phys_mass_props &result)
{
	if(mass <= 0.0f)
	{
		return PHYS_FAILURE;
	}
	if(surface_thickness < 0.0f)
	{
		return PHYS_FAILURE;
	}

	const phys_real min_twice_area = 1e-5f;

	phys_vec4 schema_cursor;
	phys_mat3 it;
	phys_vec4 normal;
	{
		phys_vec4 cb;
		phys_vec4 ab;
		cb.set_sub4(v2,v1);
		ab.set_sub4(v0,v1);

		normal.set_cross( cb , ab);
	}

	phys_real twice_area = normal.length_3d();
	
	if(surface_thickness < 1e-5f)
	{
		schema_cursor = v0;
		schema_cursor.add4(v1);
		schema_cursor.add4(v2);
		schema_cursor.set_mul4(1.0f/3.0f, schema_cursor);

		phys_real inertia_xx = mass/12.0f * ( 9.0f*schema_cursor(0)*schema_cursor(0) + v0(0)*v0(0) + v1(0)*v1(0) + v2(0)*v2(0) );
		phys_real inertia_yy = mass/12.0f * ( 9.0f*schema_cursor(1)*schema_cursor(1) + v0(1)*v0(1) + v1(1)*v1(1) + v2(1)*v2(1) );
		phys_real inertia_zz = mass/12.0f * ( 9.0f*schema_cursor(2)*schema_cursor(2) + v0(2)*v0(2) + v1(2)*v1(2) + v2(2)*v2(2) );
		phys_real inertia_xy = mass/12.0f * ( 9.0f*schema_cursor(0)*schema_cursor(1) + v0(0)*v0(1) + v1(0)*v1(1) + v2(0)*v2(1) );
		phys_real inertia_xz = mass/12.0f * ( 9.0f*schema_cursor(0)*schema_cursor(2) + v0(0)*v0(2) + v1(0)*v1(2) + v2(0)*v2(2) );
		phys_real inertia_yz = mass/12.0f * ( 9.0f*schema_cursor(1)*schema_cursor(2) + v0(1)*v0(2) + v1(1)*v1(2) + v2(1)*v2(2) );

		it(0,0) = inertia_yy + inertia_zz;
		it(1,1) = inertia_xx + inertia_zz;
		it(2,2) = inertia_xx + inertia_yy;
		it(0,1) = it(1,0) = -inertia_xy;
		it(0,2) = it(2,0) = -inertia_xz;
		it(2,1) = it(1,2) = -inertia_yz;

		shift_inertia_to_com(schema_cursor, mass, it);
	}
	else
	{
			
		if(twice_area < min_twice_area)
		{
			schema_cursor = v0;
			schema_cursor.add4(v1);
			schema_cursor.add4(v2);
			schema_cursor.set_mul4(1.0f/3.0f, schema_cursor);

			phys_real rx = schema_cursor(0);
			phys_real ry = schema_cursor(1);
			phys_real rz = schema_cursor(2);

			it(0,0) = mass * (ry*ry+rz*rz);
			it(1,1) = mass * (rx*rx+rz*rz);
			it(2,2) = mass * (ry*ry+rx*rx);
			it(0,1) = -mass * (ry*rx);
			it(1,0) = -mass * (ry*rx);
			it(0,2) = -mass * (rz*rx);
			it(2,0) = -mass * (rz*rx);
			it(2,1) = -mass * (ry*rz);
			it(1,2) = -mass * (ry*rz);	
		}
		else
		{
			normal.normalize_xyz();

			phys_inline_array<phys_vec4, 6> verts;
			verts.set_array_size(6);

			verts[0] = v0;
			verts[1] = v0;
			verts[0].add_scaled4(surface_thickness * 0.5f,normal);
			verts[1].add_scaled4(-surface_thickness * 0.5f,normal);

			verts[2] = v1;
			verts[3] = v1;
			verts[2].add_scaled4(surface_thickness * 0.5f,normal);
			verts[3].add_scaled4(-surface_thickness * 0.5f,normal);

			verts[4] = v2;
			verts[5] = v2;
			verts[4].add_scaled4(surface_thickness * 0.5f,normal);
			verts[5].add_scaled4(-surface_thickness * 0.5f,normal);

			phys_mass_props properties;		
			{
				phys_geometry tri_geometry;
				tri_geometry.m_vertices = verts;

				tri_geometry.m_triangle_list.expand_by(1)->set(0, 2, 4);	
				tri_geometry.m_triangle_list.expand_by(1)->set(1, 5, 3);	

				tri_geometry.m_triangle_list.expand_by(1)->set(0, 3, 2);
				tri_geometry.m_triangle_list.expand_by(1)->set(0, 1, 3);

				tri_geometry.m_triangle_list.expand_by(1)->set(1, 0, 4);
				tri_geometry.m_triangle_list.expand_by(1)->set(1, 4, 5);

				tri_geometry.m_triangle_list.expand_by(1)->set(2, 5, 4);
				tri_geometry.m_triangle_list.expand_by(1)->set(2, 3, 5);

				phys_inertia_util::geometry_volume_mass_props(&tri_geometry, mass, properties);
			}
			
			schema_cursor = properties.m_center_of_mass;
			it = properties.m_inertia_tensor;
		}
	}

	result.m_body_mass = mass;
	result.m_inertia_tensor = it;
	result.m_center_of_mass = schema_cursor;
	result.m_body_volume = twice_area * 0.5f * surface_thickness;

	return PHYS_SUCCESS;

}

phys_result PHYS_CALL phys_inertia_util::box_volume_mass_props(const phys_vec4& box_half_extents, phys_real mass, phys_mass_props &result)
{
	if(mass <= 0.0f)
	{
		return PHYS_FAILURE;
	}

	phys_real x = box_half_extents(0);
	phys_real y = box_half_extents(1);
	phys_real z = box_half_extents(2);

	phys_real k = mass * ( 1.0f / 3.0f);

	result.m_inertia_tensor.set_to_identity();

	result.m_inertia_tensor( 0, 0 ) = (y*y + z*z) * k;
	result.m_inertia_tensor( 1, 1 ) = (x*x + z*z) * k;
	result.m_inertia_tensor( 2, 2 ) = (x*x + y*y) * k;

	result.m_center_of_mass.set_zero4();
	result.m_body_volume = x * y * z * 8.0f;
	result.m_body_mass = mass;

	return PHYS_SUCCESS;

}

phys_result PHYS_CALL phys_inertia_util::vertex_hull_mass_props(
	const phys_real *vertex_in, int inertia_vertex_stride, int hull_vertex_count, phys_real mass, phys_mass_props &result)
{
	if(mass <= 0.0f)
	{
		return PHYS_FAILURE;
	}

	phys_array<phys_vec4> verts;
	verts.set_array_size(hull_vertex_count);
	phys_real *ptr = const_cast<phys_real*> (vertex_in);
	for(int i = 0; i < hull_vertex_count; i++)
	{
		verts[i](0) = ptr[0];
		verts[i](1) = ptr[1];
		verts[i](2) = ptr[2];
		ptr = phys_add_byte_offset( ptr, inertia_vertex_stride );
	}

	phys_strided_vertices strided_verts;
	{
		strided_verts.m_vertex_stride_bytes = sizeof(phys_vec4);
		strided_verts.m_hull_vertex_count = hull_vertex_count;
		strided_verts.m_vertices = &(verts[0](0));
	}

	phys_geometry g;
	phys_array<phys_vec4> hull_plane_equations;

	phys_geometry_util::build_convex_geometry( strided_verts, g, hull_plane_equations, PHYS_HULL_MODE_FAST);

	phys_inertia_util::geometry_volume_mass_props(&g, mass, result);
	result.m_body_mass = mass;

	return PHYS_SUCCESS;
}

phys_result PHYS_CALL phys_inertia_util::vertex_cloud_mass_props(
	const phys_real *vertex_in, int inertia_vertex_stride, int hull_vertex_count, phys_real mass, phys_mass_props &result)
{

	if(hull_vertex_count <= 0)
	{
		return PHYS_FAILURE;
	}

	phys_array<phys_vec4> verts;
	verts.set_array_size(hull_vertex_count);
	phys_real *ptr = const_cast<phys_real*> (vertex_in);
	int i;
	for(i = 0; i < hull_vertex_count; i++)
	{
		verts[i](0) = ptr[0];
		verts[i](1) = ptr[1];
		verts[i](2) = ptr[2];
		ptr = phys_add_byte_offset( ptr, inertia_vertex_stride );
	}

	result.m_body_mass = mass;
	result.m_inertia_tensor.set_zero();
	result.m_center_of_mass.set_zero4();

	phys_real point_mass = mass / hull_vertex_count;

	for(i = 0; i < hull_vertex_count; i++)
	{
		result.m_center_of_mass.add4(verts[i]);	
	}
	result.m_center_of_mass.set_mul4(1.0f/hull_vertex_count, result.m_center_of_mass);

	for(i = 0; i < hull_vertex_count; i++)
	{
		phys_real rx, ry, rz;
		rx = verts[i](0) - result.m_center_of_mass(0);
		ry = verts[i](1) - result.m_center_of_mass(1);
		rz = verts[i](2) - result.m_center_of_mass(2);
		
		result.m_inertia_tensor(0,0) += point_mass * (ry*ry+rz*rz);
		result.m_inertia_tensor(1,1) += point_mass * (rx*rx+rz*rz);
		result.m_inertia_tensor(2,2) += point_mass * (ry*ry+rx*rx);
		result.m_inertia_tensor(0,1) -= point_mass * (ry*rx);
		result.m_inertia_tensor(1,0) -= point_mass * (ry*rx);
		result.m_inertia_tensor(0,2) -= point_mass * (rz*rx);
		result.m_inertia_tensor(2,0) -= point_mass * (rz*rx);
		result.m_inertia_tensor(2,1) -= point_mass * (ry*rz);
		result.m_inertia_tensor(1,2) -= point_mass * (ry*rz);
		
	}

	return PHYS_SUCCESS;	
}

phys_result PHYS_CALL phys_inertia_util::combine_mass_props(const phys_array<phys_mass_element>& elements, phys_mass_props& result )
{
	
	phys_vec4 group_center_of_mass;
	group_center_of_mass.set_zero4();

	phys_real group_mass = 0.0f;
	phys_real group_volume = 0.0f;

	int i;
	for(i = 0; i < elements.getSize(); i++)
	{
		phys_vec4 common_com;
		common_com.vec_set_transformed_pos(elements[i].m_body_transform, elements[i].m_properties.m_center_of_mass);
		group_center_of_mass.add_scaled4(elements[i].m_properties.m_body_mass, common_com);
		
		group_mass += elements[i].m_properties.m_body_mass;
		group_volume += elements[i].m_properties.m_body_volume;
	}
	if(group_mass <= 0.0f)
	{
		return PHYS_FAILURE;
	}

	group_center_of_mass.set_mul4(1.0f/group_mass, group_center_of_mass);

	result.m_center_of_mass = group_center_of_mass;
	result.m_body_mass = group_mass;
	result.m_body_volume = group_volume;

	result.m_inertia_tensor.set_zero();

	for(i = 0; i < elements.getSize(); i++)
	{

		phys_mat3 common_space_inertia;
		common_space_inertia = elements[i].m_properties.m_inertia_tensor;

		common_space_inertia.change_basis(elements[i].m_body_transform.get_rotation());
				
		phys_vec4 shift;
		shift.vec_set_transformed_pos(elements[i].m_body_transform, elements[i].m_properties.m_center_of_mass);
		shift.set_sub4(shift, group_center_of_mass);
		shift_inertia_from_com(shift, elements[i].m_properties.m_body_mass, common_space_inertia);

		result.m_inertia_tensor.add(common_space_inertia);
	
	}
	return PHYS_SUCCESS;

}

void PHYS_CALL phys_inertia_util::set_shape_volume_mass_props(const phys_shape* shape, phys_real mass, phys_rigid_body_desc& body_desc)
{

	phys_mass_element element;

	element.m_properties.m_center_of_mass.set_zero4();
	element.m_properties.m_inertia_tensor.set_zero();
	element.m_properties.m_body_mass = 0.0f;
	element.m_properties.m_body_volume = 0.0f;
	element.m_body_transform.set_to_identity();

	recursive_shape_mass_props(shape, element.m_body_transform, element.m_properties);
	if(element.m_properties.m_body_volume == 0.0f)
	{
		element.m_properties.m_center_of_mass.set_zero4();
		element.m_properties.m_inertia_tensor.set_zero();
		element.m_properties.m_body_mass = 0.0f;
		element.m_properties.m_body_volume = 0.0f;
		return;
	}

	scale_mass_props(mass/element.m_properties.m_body_volume, element.m_properties);
	
	body_desc.m_body_mass = element.m_properties.m_body_mass;
	body_desc.m_center_of_mass = element.m_properties.m_center_of_mass;
	body_desc.m_inertia_tensor = element.m_properties.m_inertia_tensor;
}

void PHYS_CALL phys_inertia_util::clip_inertia_ratio( phys_real max_inertia_ratio, phys_rigid_body_desc& body_desc)
{
	phys_real max_inertia = body_desc.m_inertia_tensor(0,0);
	max_inertia = phys_math::max_of_two( max_inertia, body_desc.m_inertia_tensor(1,1));
	max_inertia = phys_math::max_of_two( max_inertia, body_desc.m_inertia_tensor(2,2));
	max_inertia /= max_inertia_ratio;

	body_desc.m_inertia_tensor(0,0) = phys_math::max_of_two( body_desc.m_inertia_tensor(0,0), max_inertia );
	body_desc.m_inertia_tensor(1,1) = phys_math::max_of_two( body_desc.m_inertia_tensor(1,1), max_inertia );
	body_desc.m_inertia_tensor(2,2) = phys_math::max_of_two( body_desc.m_inertia_tensor(2,2), max_inertia );
}

void PHYS_CALL phys_inertia_util::shape_volume_mass_props(const phys_shape* shape, phys_real mass, phys_mass_props &result)
{
	
	phys_mass_element element;

	element.m_properties.m_center_of_mass.set_zero4();
	element.m_properties.m_inertia_tensor.set_zero();
	element.m_properties.m_body_mass = 0.0f;
	element.m_properties.m_body_volume = 0.0f;
	element.m_body_transform.set_to_identity();

	recursive_shape_mass_props(shape, element.m_body_transform, element.m_properties);
	if(element.m_properties.m_body_volume == 0.0f)
	{
		element.m_properties.m_center_of_mass.set_zero4();
		element.m_properties.m_inertia_tensor.set_zero();
		element.m_properties.m_body_mass = 0.0f;
		element.m_properties.m_body_volume = 0.0f;
		return;
	}

	scale_mass_props(mass/element.m_properties.m_body_volume, element.m_properties);

	result = element.m_properties;

}

void PHYS_CALL recursive_shape_mass_props(const phys_shape* shape, const phys_transform& transform, phys_mass_props& mass_props_out)
{
	phys_mass_props new_mass_props;
	
	switch (shape->get_type_id())
	{
		
		case PHYS_SHAPE_SPHERE:
		{
			const phys_sphere_shape* sphere_shape = static_cast<const phys_sphere_shape*>(shape);
			phys_inertia_util::sphere_volume_mass_props(sphere_shape->get_convex_radius(), 1.0f, new_mass_props);
			break;
		}

		case PHYS_SHAPE_BOX:
		{
			const phys_box_shape* box_shape = static_cast<const phys_box_shape*>(shape);
			phys_vec4 box_half_extents; box_half_extents = box_shape->get_half_extents();
			phys_inertia_util::box_volume_mass_props(box_half_extents, 1.0f, new_mass_props);
			break;
		}

		case PHYS_SHAPE_CONVEX_HULL:
		case PHYS_SHAPE_PACKED_HULL:
		{
			int child_sphere_count = shape->get_type_id() == PHYS_SHAPE_CONVEX_HULL
				? static_cast<const phys_convex_hull_shape*>(shape)->get_collision_sphere_count()
				: static_cast<const phys_packed_hull_shape*>(shape)->get_collision_sphere_count();

			phys_local_buffer<phys_sphere> buffer( child_sphere_count );

			const phys_sphere* spheres = shape->get_type_id() == PHYS_SHAPE_CONVEX_HULL
				? static_cast<const phys_convex_hull_shape*>(shape)->get_collision_spheres(buffer.begin())
				: static_cast<const phys_packed_hull_shape*>(shape)->get_collision_spheres(buffer.begin());

			phys_inertia_util::vertex_hull_mass_props(
				reinterpret_cast<const phys_real *>(spheres), sizeof(phys_sphere), child_sphere_count, 1.0f, new_mass_props);
			
			break;
		}
		
		case PHYS_SHAPE_TRIANGLE:
		{
			const phys_triangle_shape* tri_shape_out = static_cast<const phys_triangle_shape*>(shape);
			phys_vec4 v0; v0 = tri_shape_out->get_vertex(0);
			phys_vec4 v1; v1 = tri_shape_out->get_vertex(1);
			phys_vec4 v2; v2 = tri_shape_out->get_vertex(2);
			phys_inertia_util::triangle_surface_mass_props(v0, v1, v2, 1.0f, tri_shape_out->get_convex_radius(), new_mass_props);

			break;
		}

		case PHYS_SHAPE_MULTI_RAY:
		{
			return;
		}

		case PHYS_SHAPE_BOUNDING:
		{
			const phys_bounding_shape* agent_bounding_shape = static_cast<const phys_bounding_shape*>(shape);
		
			recursive_shape_mass_props(agent_bounding_shape->get_child_shape(), transform, mass_props_out);

			return;
		}		
		case PHYS_SHAPE_CONVEX_OFFSET:
		{
			const phys_convex_offset_shape* ts = static_cast<const phys_convex_offset_shape*>( shape );

			phys_transform test_xform; test_xform.set_to_identity();
			test_xform.set_translation( ts->get_translation() );

			phys_transform t;			t.set_mul(transform, test_xform);	
			const phys_shape* bounding_child_shape = ts->get_child_shape();
			recursive_shape_mass_props(bounding_child_shape, t, mass_props_out);

			return;
		}

		case PHYS_SHAPE_CONVEX_XFORM:
		{
			const phys_convex_xform_shape* ts = static_cast<const phys_convex_xform_shape*>( shape );

			phys_transform t;
			t.set_mul(transform, ts->get_transform());	
			const phys_shape* bounding_child_shape = ts->get_child_shape();
			recursive_shape_mass_props(bounding_child_shape, t, mass_props_out);

			return;
		}
		case PHYS_SHAPE_TRANSFORM:
		{
			const phys_transform_shape* ts = static_cast<const phys_transform_shape*>( shape );

			phys_transform t;
			t.set_mul(transform, ts->get_transform());	
			const phys_shape* bounding_child_shape = ts->get_child_shape();
			recursive_shape_mass_props(bounding_child_shape, t, mass_props_out);

			return;
		}

		case PHYS_SHAPE_MULTI_SPHERE:
		{
			const phys_multi_sphere_shape* multi_sphere_src = static_cast<const phys_multi_sphere_shape*>(shape);	

			for (int i = 0; i < multi_sphere_src->get_sphere_count(); i++ )
			{
				phys_transform t; t = transform;
				const phys_vec4& sphere = multi_sphere_src->get_spheres()[i];
				t.get_translation().vec_set_transformed_pos( t, sphere );
				
				phys_sphere_shape sp( sphere(3) );
				recursive_shape_mass_props( &sp, t, mass_props_out);
			}
			return;
		}

		case PHYS_SHAPE_TRIANGLE_COMPOUND: 
		case PHYS_SHAPE_CONVEX_LIST:
		case PHYS_SHAPE_LIST:
		case PHYS_SHAPE_COMPOUND:
		case PHYS_SHAPE_BVH:
		case PHYS_SHAPE_BVH_TREE:
		case PHYS_SHAPE_BVH_EMBEDDED:
		{

			const phys_shape_container* shape_container;			
			if(shape->get_type_id() == PHYS_SHAPE_BVH || shape->get_type_id() == PHYS_SHAPE_BVH_TREE)
			{
				const phys_bvh_shape* bvh_tree_shape = static_cast<const phys_bvh_shape*>(shape);	
				shape_container = bvh_tree_shape->get_shape_container();	
			}
			else
			{
				shape_container = shape->get_shape_container();
			}

			{
				phys_shape_buffer buffer;
				for (	phys_shape_key key = shape_container->get_first_key();
						key != PHYS_INVALID_SHAPE_KEY;
						key = shape_container->get_next_key( key ))
				{
					const phys_shape* bounding_child_shape = shape_container->get_child_shape( key, buffer );
					if (bounding_child_shape != 0)
					{
						recursive_shape_mass_props(bounding_child_shape, transform, mass_props_out);	
					}
					
				}
			}

			return;
		}

		case PHYS_SHAPE_PLANE:
		{
			break;
		}

		case PHYS_SHAPE_CAPSULE:
		{
			const phys_capsule_shape* capsule_shape = static_cast<const phys_capsule_shape*>(shape);
			phys_inertia_util::capsule_volume_mass_props(
				capsule_shape->get_vertex(0), capsule_shape->get_vertex(1), capsule_shape->get_convex_radius(), 1.0f, new_mass_props);
			break;
		}

		case PHYS_SHAPE_CYLINDER:
		{
			const phys_cylinder_shape* cylinder_shape = static_cast<const phys_cylinder_shape*>(shape);
			phys_inertia_util::cylinder_volume_mass_props(
				cylinder_shape->get_vertex(0), cylinder_shape->get_vertex(1), cylinder_shape->get_cylinder_radius(), 1.0f, new_mass_props);
			break;
		}
		
		default:
		{
			return;
		}

	}

	if(new_mass_props.m_body_volume == 0.0f)
	{
		return;
	}

	scale_mass_props(new_mass_props.m_body_volume/1.0f, new_mass_props);

	{
		phys_array<phys_mass_element> all_elements;

		phys_mass_element current_mass_elem;
		current_mass_elem.m_properties = mass_props_out;
		all_elements.array_push_back(current_mass_elem);

		phys_mass_element new_mass_elem;
		new_mass_elem.m_properties = new_mass_props;
		new_mass_elem.m_body_transform = transform;
		all_elements.array_push_back(new_mass_elem);

		phys_inertia_util::combine_mass_props(all_elements, mass_props_out);
	}	

}

inline void PHYS_CALL phys_inertia_util::shift_inertia_to_com(phys_vec4& shift, phys_real mass, phys_mat3& inertia)
{
	inertia(0,0) -= mass * (shift(1)*shift(1) + shift(2)*shift(2));
	inertia(1,1) -= mass * (shift(2)*shift(2) + shift(0)*shift(0));
	inertia(2,2) -= mass * (shift(0)*shift(0) + shift(1)*shift(1));
	inertia(0,1) = inertia(1,0) += mass * shift(0) * shift(1); 
	inertia(1,2) = inertia(2,1) += mass * shift(1) * shift(2); 
	inertia(2,0) = inertia(0,2) += mass * shift(2) * shift(0); 
}

void PHYS_CALL phys_inertia_util::shift_inertia_from_com(phys_vec4& shift, phys_real mass, phys_mat3& inertia)
{
	inertia(0,0) += mass * (shift(1)*shift(1) + shift(2)*shift(2));
	inertia(1,1) += mass * (shift(2)*shift(2) + shift(0)*shift(0));
	inertia(2,2) += mass * (shift(0)*shift(0) + shift(1)*shift(1));
	inertia(0,1) = inertia(1,0) -= mass * shift(0) * shift(1); 
	inertia(1,2) = inertia(2,1) -= mass * shift(1) * shift(2); 
	inertia(2,0) = inertia(0,2) -= mass * shift(2) * shift(0); 
}

inline void PHYS_CALL scale_mass_props(phys_real scale, phys_mass_props& mass_props)
{
	
	mass_props.m_body_mass = mass_props.m_body_mass * scale;
	mass_props.m_inertia_tensor.mul( scale );
}

void PHYS_CALL phys_inertia_util::simplify_inertia_to_particle(phys_mat3 &inertia)
{
		
	phys_real temp = phys_math::max_of_two( phys_math::max_of_two( inertia(0,0), inertia(1,1) ), inertia(2,2) );
	inertia.set_diagonal(temp, temp, temp);
}

inline phys_real inertia_util_internal::square_real(phys_real x)
{
	return x * x;
}

inline phys_real inertia_util_internal::cube_real(phys_real x)
{
	return x * x * x;
}

#define X 0
#define Y 1
#define Z 2

void inertia_util_internal::compute_projection_integrals(const phys_vec4* v)
{
	phys_real a0, a1, da;
	phys_real b0, b1, db;
	phys_real a0_pow2, a0_pow3, a0_pow4, b0_pow2, b0_pow3, b0_pow4;
	phys_real a1_pow2, a1_pow3, b1_pow2, b1_pow3;
	phys_real C1, c_a, c_aa, c_aaa, c_b, c_bb, c_bbb;
	phys_real c_ab, k_ab, c_aab, k_aab, c_abb, k_abb;
	int i;

	P1 = poly_int_a = poly_int_b = proj_aa = proj_ab = proj_bb = proj_aaa = proj_aab = proj_abb = proj_bbb = 0.0f;

	for (i = 0; i < 3; i++) 
	{
		a0 = v[i](A);
		b0 = v[i](B);

		a1 = v[(i+1) % 3](A);
		b1 = v[(i+1) % 3](B);
	
		da = a1 - a0;
		db = b1 - b0;
		a0_pow2 = a0 * a0; a0_pow3 = a0_pow2 * a0; a0_pow4 = a0_pow3 * a0;
		b0_pow2 = b0 * b0; b0_pow3 = b0_pow2 * b0; b0_pow4 = b0_pow3 * b0;
		a1_pow2 = a1 * a1; a1_pow3 = a1_pow2 * a1; 
		b1_pow2 = b1 * b1; b1_pow3 = b1_pow2 * b1;

		C1 = a1 + a0;
		c_a = a1*C1 + a0_pow2; c_aa = a1*c_a + a0_pow3; c_aaa = a1*c_aa + a0_pow4;
		c_b = b1*(b1 + b0) + b0_pow2; c_bb = b1*c_b + b0_pow3; c_bbb = b1*c_bb + b0_pow4;
		c_ab = 3*a1_pow2 + 2*a1*a0 + a0_pow2; k_ab = a1_pow2 + 2*a1*a0 + 3*a0_pow2;
		c_aab = a0*c_ab + 4*a1_pow3; k_aab = a1*k_ab + 4*a0_pow3;
		c_abb = 4*b1_pow3 + 3*b1_pow2*b0 + 2*b1*b0_pow2 + b0_pow3;
		k_abb = b1_pow3 + 2*b1_pow2*b0 + 3*b1*b0_pow2 + 4*b0_pow3;

		P1 += db*C1;
		poly_int_a += db*c_a;
		proj_aa += db*c_aa;
		proj_aaa += db*c_aaa;
		poly_int_b += da*c_b;
		proj_bb += da*c_bb;
		proj_bbb += da*c_bbb;
		proj_ab += db*(b1*c_ab + b0*k_ab);
		proj_aab += db*(b1*c_aab + b0*k_aab);
		proj_abb += da*(a1*c_abb + a0*k_abb);
	}

	P1 /= 2.0f;
	poly_int_a /= 6.0f;
	proj_aa /= 12.0f;
	proj_aaa /= 20.0f;
	poly_int_b /= -6.0f;
	proj_bb /= -12.0f;
	proj_bbb /= -20.0f;
	proj_ab /= 24.0f;
	proj_aab /= 60.0f;
	proj_abb /= -60.0f;
	
}

void inertia_util_internal::compute_face_integrals(const phys_vec4* v,const phys_vec4& n)
{
	phys_real w;
	phys_real coeff_k1, k2, coeff_k3, coeff_k4;

	compute_projection_integrals(v);

	w = - n(X) * v[0](X)
		- n(Y) * v[0](Y)
		- n(Z) * v[0](Z);
	
	coeff_k1 = 1 / n(C); k2 = coeff_k1 * coeff_k1; coeff_k3 = k2 * coeff_k1; coeff_k4 = coeff_k3 * coeff_k1;

	face_a = coeff_k1 * poly_int_a;
	face_b = coeff_k1 * poly_int_b;
	face_c = -k2 * (n(A)*poly_int_a + n(B)*poly_int_b + w*P1);

	face_aa = coeff_k1 * proj_aa;
	face_bb = coeff_k1 * proj_bb;
	face_cc = coeff_k3 * (square_real(n(A))*proj_aa + 2*n(A)*n(B)*proj_ab + square_real(n(B))*proj_bb
				+ w*(2*(n(A)*poly_int_a + n(B)*poly_int_b) + w*P1));

	face_aaa = coeff_k1 * proj_aaa;
	face_bbb = coeff_k1 * proj_bbb;
	face_ccc = -coeff_k4 * (cube_real(n(A))*proj_aaa + 3*square_real(n(A))*n(B)*proj_aab 
			+ 3*n(A)*square_real(n(B))*proj_abb + cube_real(n(B))*proj_bbb
			+ 3*w*(square_real(n(A))*proj_aa + 2*n(A)*n(B)*proj_ab + square_real(n(B))*proj_bb)
			+ w*w*(3*(n(A)*poly_int_a + n(B)*poly_int_b) + w*P1));
	
	face_aab = coeff_k1 * proj_aab;
	face_bbc = -k2 * (n(A)*proj_abb + n(B)*proj_bbb + w*proj_bb);
	face_cca = coeff_k3 * (square_real(n(A))*proj_aaa + 2*n(A)*n(B)*proj_aab + square_real(n(B))*proj_abb
			+ w*(2*(n(A)*proj_aa + n(B)*proj_ab) + w*poly_int_a));

}

void inertia_util_internal::compute_volume_integrals(const phys_geometry* wire_body_cap,const phys_vec4& shift)
{
	
	phys_real nx, ny, nz;
	int i;
	phys_vec4 normal;
	phys_vec4 verts[3];
	long hull_face_count;
	phys_geometry::geometry_triangle geom_tri_idx;
	phys_vec4 edge_sub1;
	phys_vec4 edge_sub2;	
	hull_face_count = wire_body_cap->m_triangle_list.getSize(); 
	phys_real area,inv_length;

	T0 = T1[X] = T1[Y] = T1[Z] 
		= T2[X] = T2[Y] = T2[Z] 
		= TP[X] = TP[Y] = TP[Z] = 0;

	for (i = 0; i < hull_face_count; i++) 
	{
		geom_tri_idx = wire_body_cap->m_triangle_list[i];
		verts[0].set_add4(wire_body_cap->m_vertices[geom_tri_idx.m_a],shift);
		verts[1].set_add4(wire_body_cap->m_vertices[geom_tri_idx.m_b],shift);
		verts[2].set_add4(wire_body_cap->m_vertices[geom_tri_idx.m_c],shift);
		edge_sub1.set_sub4(verts[1],verts[0]);
		edge_sub2.set_sub4(verts[2],verts[0]);
		normal.set_cross(edge_sub1,edge_sub2);
		
		area = normal.length_3d();

		if (area > 0)	
		{

			inv_length = 1.f/area;
			normal.mul4(inv_length);
			
			nx = phys_math::fabs(normal(X));
			ny = phys_math::fabs(normal(Y));
			nz = phys_math::fabs(normal(Z));
			if (nx > ny && nx > nz) C = X;
			else C = (ny > nz) ? Y : Z;
			A = (C + 1) % 3;
			B = (A + 1) % 3;
		
			compute_face_integrals(verts,normal);
			
			T0 += normal(X) * ((A == X) ? face_a : ((B == X) ? face_b : face_c));

			T1[A] += normal(A) * face_aa;
			T1[B] += normal(B) * face_bb;
			T1[C] += normal(C) * face_cc;
			T2[A] += normal(A) * face_aaa;
			T2[B] += normal(B) * face_bbb;
			T2[C] += normal(C) * face_ccc;
			TP[A] += normal(A) * face_aab;
			TP[B] += normal(B) * face_bbc;
			TP[C] += normal(C) * face_cca;
		}
	}

	T1[X] /= 2; T1[Y] /= 2; T1[Z] /= 2;
	T2[X] /= 3; T2[Y] /= 3; T2[Z] /= 3;
	TP[X] /= 2; TP[Y] /= 2; TP[Z] /= 2;

#if defined(__MWERKS__)
#pragma optimization_level 0
#endif

}

#if defined(__MWERKS__)
#pragma optimization_level 4
#endif

void inertia_util_internal::compute_inertia_tensor_internal(phys_real mass, phys_real density,
													phys_vec4 &r, phys_mat3 &J)
{
		
	r(X) = T1[X] / T0;
	r(Y) = T1[Y] / T0;
	r(Z) = T1[Z] / T0;

	J(X,X) = density * (T2[Y] + T2[Z]);
	J(Y,Y) = density * (T2[Z] + T2[X]);
	J(Z,Z) = density * (T2[X] + T2[Y]);
	J(X,Y) = J(Y,X) = - density * TP[X];
	J(Y,Z) = J(Z,Y) = - density * TP[Y];
	J(Z,X) = J(X,Z) = - density * TP[Z];

	J(X,X) -= mass * (r(Y)*r(Y) + r(Z)*r(Z));
	J(Y,Y) -= mass * (r(Z)*r(Z) + r(X)*r(X));
	J(Z,Z) -= mass * (r(X)*r(X) + r(Y)*r(Y));
	J(X,Y) = J(Y,X) += mass * r(X) * r(Y); 
	J(Y,Z) = J(Z,Y) += mass * r(Y) * r(Z); 
	J(Z,X) = J(X,Z) += mass * r(Z) * r(X); 
}

#undef X
#undef Y
#undef Z

void PHYS_CALL inertia_util_internal::compute_geometry_aabb(const phys_vec4* vertices, int hull_vert_count, phys_vec4& center_out, phys_vec4& aabb_extents_out )
{
	center_out.set_zero4();
	aabb_extents_out.set_zero4();

	phys_vec4 min_corner;	min_corner.flags_set_all(3.40282e+38f);
	phys_vec4 box_max_pt;	box_max_pt.set_neg4(min_corner);

	for(int i = 0; i< hull_vert_count; i++)
	{
		min_corner.set_min4(vertices[i], min_corner);
		box_max_pt.set_max4(vertices[i], box_max_pt);
	}

	if(hull_vert_count != 0)
	{		
		center_out.set_lerp4( min_corner, box_max_pt, 0.5f );
		aabb_extents_out.set_sub4( box_max_pt, min_corner );
	}
}

void PHYS_CALL phys_inertia_util::geometry_volume_mass_props(const phys_geometry* wire_body_cap, phys_real mass, phys_mass_props &result)
{

	if (mass <= 0)	{		mass = 1.0f;	}

	phys_vec4 hull_aabb_center;
	phys_vec4 aabb_extents;
	inertia_util_internal::compute_geometry_aabb(wire_body_cap->m_vertices.begin(), wire_body_cap->m_vertices.getSize(), hull_aabb_center, aabb_extents );

	phys_mass_props aabb_mass_props;
	{
		box_volume_mass_props( aabb_extents, mass, aabb_mass_props );
		aabb_mass_props.m_center_of_mass = hull_aabb_center;
	}

	inertia_util_internal inertia_calc;
	{
		phys_vec4 shift; shift.set_neg4( hull_aabb_center );
		inertia_calc.compute_volume_integrals(wire_body_cap, shift);
	}

	phys_real poly_volume = inertia_calc.T0;

	if (poly_volume <= 0.0f)
	{
		result = aabb_mass_props;
		return;
	}

	phys_real density = mass/poly_volume;

	result.m_body_volume = poly_volume;
	result.m_body_mass   = mass;

	phys_mat3& J = result.m_inertia_tensor;
	{
		inertia_calc.compute_inertia_tensor_internal(mass, density, result.m_center_of_mass, J);
		
		result.m_center_of_mass.add4(hull_aabb_center);
	}

	{
		
		for (int a = 0; a < 3; a++)
		{
			if ( J(a,a) < 0.1f * aabb_mass_props.m_inertia_tensor(a,a) )
			{
				J(a,a) = 0.1f * aabb_mass_props.m_inertia_tensor(a,a);
				result.m_center_of_mass(a) = hull_aabb_center(a);
				for (int b = 0; b < 3; b++)
				{
					if ( b != a){ J(b,a) = J(a,b) = phys_math::max_of_two( .0f, J(b,a)); }
				}
			}
		}
	}
}

phys_result PHYS_CALL phys_inertia_util::geometry_volume_mass_props_checked(const phys_geometry* wire_body_cap, phys_real mass, phys_mass_props &result)
{
	if (mass <= 0)
	{
		return PHYS_FAILURE;
	}

	phys_vec4 hull_aabb_center;
	phys_vec4 aabb_extents;
	inertia_util_internal::compute_geometry_aabb(wire_body_cap->m_vertices.begin(), wire_body_cap->m_vertices.getSize(), hull_aabb_center, aabb_extents );

	phys_mass_props aabb_mass_props;
	{
		box_volume_mass_props( aabb_extents, mass, aabb_mass_props );
		aabb_mass_props.m_center_of_mass = hull_aabb_center;
	}

	inertia_util_internal inertia_calc;
	{
		phys_vec4 shift; shift.set_neg4( hull_aabb_center );
		inertia_calc.compute_volume_integrals(wire_body_cap, shift);
	}

	phys_real poly_volume = inertia_calc.T0;

	if (poly_volume <= 0.0f)
	{
		return PHYS_FAILURE;
	}

	phys_real density = mass/poly_volume;

	result.m_body_volume = poly_volume;
	result.m_body_mass   = mass;

	phys_mat3& J = result.m_inertia_tensor;
	{
		inertia_calc.compute_inertia_tensor_internal(mass, density, result.m_center_of_mass, J);
		result.m_center_of_mass.add4(hull_aabb_center);
	}

	{
		
		for (int a = 0; a < 3; a++)
		{
			if ( J(a,a) < 0.1f * aabb_mass_props.m_inertia_tensor(a,a) )
			{
				J(a,a) = 0.1f * aabb_mass_props.m_inertia_tensor(a,a);
				result.m_center_of_mass(a) = hull_aabb_center(a);
				for (int b = 0; b < 3; b++)
				{
					if ( b != a){ J(b,a) = J(a,b) = phys_math::max_of_two( .0f, J(b,a)); }
				}
			}
		}
	}
	return PHYS_SUCCESS;
}

phys_real PHYS_CALL phys_inertia_util_optimize_tree_int(
			phys_array<const phys_constraint*>& constraints, 
			phys_rigid_body* body, phys_real inertia_factor_hint )
{

	phys_inline_array<const phys_constraint*,16> children;
	{
		for (int i = constraints.getSize()-1; i>=0;i-- )
		{
			const phys_constraint* ci = constraints[i];
			phys_rigid_body* other_proxy = 0;
			if (  ci->get_rigid_body_a() == body )
			{
				other_proxy = ci->get_rigid_body_b();
			}
			else if (  ci->get_rigid_body_b() == body )
			{
				other_proxy = ci->get_rigid_body_a();
			}
			if ( other_proxy )
			{
				constraints.remove_at(i);
				children.array_push_back( ci );
			}
		}
	}

	phys_real child_inertia_sum = 0.0f;
	phys_real max_child_inertia = 0.0f;
	{
		for (int i = children.getSize()-1; i>=0;i-- )
		{
			const phys_constraint* ci = children[i];
			phys_rigid_body* other_proxy = (  ci->get_rigid_body_a() == body ) ? ci->get_rigid_body_b() : ci->get_rigid_body_a();
			phys_real child_inertia = phys_inertia_util_optimize_tree_int( constraints, other_proxy, inertia_factor_hint );
			child_inertia_sum += child_inertia;
			max_child_inertia = phys_math::max_of_two( child_inertia, max_child_inertia );
		}
	}

	if ( body->is_fixed_or_keyframed() )
	{
		return 0.0f;
	}

	phys_mat3 inertia;
	body->get_inertia_local( inertia );
	phys_real max_dist_tol = phys_math::max_of_two( inertia(0,0), phys_math::max_of_two( inertia(1,1), inertia(2,2) ) );

	if ( child_inertia_sum == 0.0f )
	{
		return max_dist_tol;
	}

	phys_real min_inertia_bound = phys_math::min_of_two( child_inertia_sum, max_dist_tol * inertia_factor_hint );
	min_inertia_bound = phys_math::max_of_two( min_inertia_bound, max_child_inertia );
	{
		inertia(0,0) = phys_math::max_of_two( inertia(0,0), min_inertia_bound );
		inertia(1,1) = phys_math::max_of_two( inertia(1,1), min_inertia_bound );
		inertia(2,2) = phys_math::max_of_two( inertia(2,2), min_inertia_bound );

		body->set_local_inertia( inertia );
	}

	phys_real max_inertia_bound = phys_math::min_of_two( max_dist_tol * inertia_factor_hint, max_dist_tol + child_inertia_sum );
	return phys_math::max_of_two( max_inertia_bound, min_inertia_bound );
}

void phys_inertia_util::optimize_tree_inertias(
	phys_constraint *const *constraints, int solver_constraint_count, phys_rigid_body *root_body, phys_real inertia_factor_hint)
{
	phys_local_array<const phys_constraint*> constraint_cursor(solver_constraint_count);
	for (int i = 0; i < solver_constraint_count; i++ )
	{
		constraint_cursor.push_back_unchecked( constraints[i] );
	}
	phys_inertia_util_optimize_tree_int( constraint_cursor, root_body, inertia_factor_hint );
}

void PHYS_CALL phys_inertia_util::inertia_to_principal_axes( phys_mat3& inertia, phys_rotation& principal_axis_out )
{
	principal_axis_out.set_to_identity();

	for (int iterations = 5; iterations > 0; iterations--)
	{
		
		phys_real max_val_sqrd = inertia(1,0) * inertia(1,0);
		int max_reach = 1;
		int max_component = 0;
		for (int c = 0; c < 2; c++ )
		{
			phys_real in = inertia(2,c);
			in *= in;
			if ( in > max_val_sqrd )
			{
				max_val_sqrd = in;
				max_reach = 2;
				max_component = c;
			}
		}
		if ( max_val_sqrd < FLT_EPSILON * FLT_EPSILON)
		{
			break;
		}

		phys_real si;
		phys_real cos_term;
		{
			phys_real w = inertia( max_component, max_reach );
			phys_real x = inertia( max_component, max_component );
			phys_real y = inertia( max_reach, max_reach );
			phys_real a = (y-x)/ (2.0f * w);
			phys_real a_sign = (a>0.0f) ? 1.0f : -1.0f;
			phys_real r = a_sign / ( phys_math::fabs(a) + phys_math::sqrt( 1 + a*a ) );
			cos_term = phys_math::inv_sqrt( 1 + r * r );
			si = r * cos_term;
		}
			
		phys_rotation rot;	rot.set_to_identity();
		rot( max_component, max_component ) = cos_term;
		rot( max_reach, max_reach ) = cos_term;
		rot( max_component, max_reach ) = -si;
		rot( max_reach, max_component ) = si;

		phys_rotation tmp; tmp.set_mul( rot, inertia );

		rot( max_component, max_reach ) = si;
		rot( max_reach, max_component ) = -si;

		inertia.set_mul( tmp, rot );

		tmp.set_mul( principal_axis_out, rot );
		static_cast<phys_mat3&>(principal_axis_out) = tmp;
	}

	principal_axis_out.get_column(0).normalize_xyz();

	principal_axis_out.get_column(1).set_cross( principal_axis_out.get_column(2), principal_axis_out.get_column(0) );
	principal_axis_out.get_column(1).normalize_xyz();

	principal_axis_out.get_column(2).set_cross( principal_axis_out.get_column(0), principal_axis_out.get_column(1) );
	principal_axis_out.get_column(2).normalize_xyz();

}

