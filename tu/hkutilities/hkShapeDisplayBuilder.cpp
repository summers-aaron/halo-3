// hkutilities/hkShapeDisplayBuilder.obj: from the records 0x6181f0 0x618200 0x618238 0x618248 0x6182f0 0x618418 0x618480 0x619cf0 0x619d70 0x619e98 0x619ef8 0x70b228
// @compiler xdk6130
// @flags /TP /O2 /Oc /GF /EHs-c- /GR-
// @address 0x70b228 ??__Ephys_singleton_reg_user_shape_display@@YAXXZ
#include "phys/phys_base.h"
#include "phys/phys_collide.h"
#include "phys/phys_heightfield_shape_base.h"
#include "phys/phys_shape_display_builder.h"
#include "phys/phys_local_array.h"
#include "phys/phys_multi_sphere_shape.h"
#include "phys/phys_cylinder_shape.h"
#include "phys/phys_transform_shape.h"
#include "phys/phys_bounding_shape.h"
#include "phys/phys_triangle_shape.h"
#include "phys/phys_convex_hull_shape.h"
#include "phys/phys_packed_hull_shape.h"
#include "phys/phys_multi_ray_shape.h"
#include "phys/phys_geometry_util.h"
#include "phys/phys_hull_mode.h"
#include "phys/phys_display_plane.h"
#include "phys/phys_display_sphere.h"
#include "phys/phys_display_cylinder.h"
#include "phys/phys_display_capsule.h"
#include "phys/phys_display_box.h"
#include "phys/phys_display_convex.h"
#include "phys/phys_debug_display.h"
// @address 0x6181f0 ??0phys_shape_display_env@phys_shape_display_builder@@QAA@XZ
// @address 0x618200 ??0phys_shape_display_builder@@QAA@ABUphys_shape_display_env@0@@Z
// @address 0x618238 ?reset_current_raw_geometry@phys_shape_display_builder@@QAAXXZ
// @address 0x618248 ?register_user_shape_display@phys_user_shape_display_builder@@QAAXP6AXPBVphys_shape@@ABVphys_transform@@AAV?$phys_array@PAVphys_display_geometry@@@@PAVphys_shape_display_builder@@@ZW4phys_shape_type@@@Z
// @address 0x6182f0 ?get_current_raw_geometry@phys_shape_display_builder@@IAAPAVphys_display_geometry@@AAV?$phys_array@PAVphys_display_geometry@@@@@Z
// @address 0x618418 ?phys_user_shape_display_create@@YAPAVphys_ref_object@@XZ
// @address 0x618480 ?build_shape_display@phys_shape_display_builder@@QAAXPBVphys_shape@@ABVphys_transform@@AAV?$phys_array@PAVphys_display_geometry@@@@@Z
// @address 0x619cf0 ?build_display_geometries@phys_shape_display_builder@@QAAXPBVphys_shape@@AAV?$phys_array@PAVphys_display_geometry@@@@@Z
// @address 0x619d70 ?add_object_to_debug_display@phys_shape_display_builder@@SAXPBVphys_shape@@AAVphys_transform@@K@Z
// @address 0x619e98 ??_Gphys_shape_display_builder@@UAAPAXI@Z
// @address 0x619ef8 ??_Gphys_user_shape_display_builder@@UAAPAXI@Z

typedef int phys_i32;

class phys_heightfield_base_desc
{
	public:
		PHYS_DECLARE_NONVIRTUAL_ALLOCATOR( PHYS_MEM_COLLIDE, phys_heightfield_base_desc );
	
		phys_heightfield_base_desc()
		{
			m_scale.set(1.0f, 1.0f, 1.0f);
			m_x_res = 2;
			m_z_res = 2;
			m_min_height = 0.0f;
			m_max_height = -1.0f;
			m_use_projection_height = false;
		}			
		
	public:
		
		phys_vec4 m_scale;
		
		phys_i32 m_x_res;
		
		phys_i32 m_z_res;
		
		phys_real m_min_height;
		
		phys_real m_max_height;

		phys_bool m_use_projection_height;
};

class phys_heightfield_shape : public phys_heightfield_shape_base
{
	public:

		phys_heightfield_shape( const phys_heightfield_base_desc& ci );

		void get_desc( phys_heightfield_base_desc& cinfo ) const;

		virtual void get_aabb_impl( const phys_transform& local_to_world, phys_real tolerance, phys_aabb& out  ) const;

		virtual void collide_spheres( const collide_spheres_input& input, sphere_hit_output* output_array) const = 0;

		virtual void trace_sphere( const phys_sphere_cast_input& input, const phys_collision_proxy& collision_proxy, phys_ray_hit_collector& collector ) const;

		virtual phys_real get_height_at( int x, int z ) const = 0;

		virtual phys_bool get_triangle_flip() const = 0;

		phys_bool trace_ray_impl( const phys_ray_input& input, phys_ray_output& output ) const;

		void trace_ray_with_collector( const phys_ray_input& input, const phys_collision_proxy& collision_proxy, phys_ray_hit_collector& collector ) const;

		void get_height_and_normal_at( int grid_x, int z_pos, phys_real sub_x, phys_real sub_z, phys_vec4& normal_out,
			phys_real& height_out, int& triangle_index_out ) const;

	private:
		
		__forceinline void get_height_and_normal_inline(
			int grid_x, int z_pos, phys_real sub_x, phys_real sub_z, phys_vec4 &normal_out, phys_real &height_out, int &triangle_index_out) const;
		void trace_ray_internal(const phys_ray_input &input, const phys_collision_proxy &collision_proxy, phys_bool report_penetrating_start,
			phys_real max_extra_penetration, phys_ray_hit_collector &collector) const;

	public:
		
		int m_x_res;
		int m_z_res;

		phys_real m_height_center;

		phys_bool m_use_projection_height;

		phys_vec4 m_quant_scale;
		phys_vec4 m_float_to_int_scale;
		phys_vec4 m_float_to_int_offset_floor;
		phys_vec4 m_cast_extents;

	public:

		phys_heightfield_shape( ) : phys_heightfield_shape_base( PHYS_SHAPE_SAMPLED_HEIGHTFIELD ) {}
		phys_heightfield_shape( phys_fixup_tag flag ) : phys_heightfield_shape_base(flag) { m_type = PHYS_SHAPE_SAMPLED_HEIGHTFIELD; }
};

class phys_plane_shape : public phys_heightfield_shape_base
{
	public:

		phys_plane_shape(const phys_vec4& plane, const phys_aabb& aabb);

		phys_plane_shape( const phys_vec4& direction, const phys_vec4& center, const phys_vec4& box_half_extents );

		void get_aabb_impl( const phys_transform& local_to_world, phys_real tolerance, phys_aabb& out  ) const;

		virtual void gather_memory_stats( phys_statistics_collector* collector) const;

		virtual phys_bool trace_ray_impl(const phys_ray_input& input, phys_ray_output& results) const;

		virtual void trace_ray_with_collector( const phys_ray_input& input, const phys_collision_proxy& collision_proxy,
			phys_ray_hit_collector& collector ) const;

		virtual void trace_sphere( const phys_sphere_cast_input& input, const phys_collision_proxy& collision_proxy, phys_ray_hit_collector& collector ) const;

		virtual void collide_spheres( const collide_spheres_input& input, sphere_hit_output* output_array) const;

		inline const phys_vec4& get_face_plane() const { return m_hull_plane; }

	protected:
		phys_vec4 m_hull_plane;
		phys_vec4 m_aabb_center;
		phys_vec4 m_aabb_half_extents;

	public:

		phys_plane_shape( phys_fixup_tag flag ) : phys_heightfield_shape_base(flag) { m_type = PHYS_SHAPE_PLANE; }

};

class phys_convex_piece_shape : public phys_convex_shape, protected phys_shape_container
{
	public:

		phys_convex_piece_shape( phys_real radius = phys_convex_shape_default_radius );

		void get_support_vertex_impl( phys_vec4_param direction, phys_hull_vertex& supporting_vertex ) const;

		void vertex_ids_to_vertices_impl( const phys_vertex_id* ids, int id_count, phys_hull_vertex* vertices_out) const;

		virtual void get_first_vertex(phys_vec4& v) const;

		void compute_bounds();

		void get_aabb_impl( const phys_transform& local_to_world, phys_real tolerance, phys_aabb& out  ) const;

		virtual phys_bool trace_ray_impl(const phys_ray_input& input,phys_ray_output& results) const;

		virtual void trace_ray_with_collector( const phys_ray_input& input, const phys_collision_proxy& collision_proxy,
			phys_ray_hit_collector& collector ) const;

		virtual int get_collision_sphere_count_impl( )  const;

		virtual const phys_sphere* get_collision_spheres_impl( phys_sphere* sphere_buffer ) const;

		virtual const phys_shape_container* get_shape_container() const;

		virtual phys_shape_key get_first_key() const;

		virtual phys_shape_key get_next_key( phys_shape_key old_key ) const;

		virtual const phys_shape* get_child_shape( phys_shape_key key, phys_shape_buffer& buffer ) const;

	public:

		phys_vec4* m_vertices;

		int m_hull_vertex_count;

		const phys_compound_shape* m_display_mesh;

		const phys_shape_key* m_display_shape_keys;

		int m_display_shape_key_count;
}; 


phys_shape_display_builder::phys_shape_display_env::phys_shape_display_env()
:	m_sphere_phi_res(8),
	m_sphere_theta_res(8)
{
}

phys_shape_display_builder::phys_shape_display_builder(const phys_shape_display_env& collision_env)
:	m_display_env(collision_env),
	m_current_geometry(0)
{
}

template<> phys_user_shape_display_builder* phys_singleton<phys_user_shape_display_builder>::s_instance = 0;

// match: 0x618480: The accessor boundary preserves both convex-vertex array-add operand ranks.
static __forceinline phys_user_shape_display_builder& get_user_shape_display_builder()
{
	return phys_user_shape_display_builder::get_singleton_instance();
}

void phys_shape_display_builder::build_shape_display(		const phys_shape* shape,
													const phys_transform& transform,
													phys_array<phys_display_geometry*>& display_geometries)
{
	switch (shape->get_type_id())
	{
		case PHYS_SHAPE_SPHERE:
		{
			const phys_sphere_shape* sphere_shape = static_cast<const phys_sphere_shape*>(shape);
			phys_vec4 zero_vec4;
			zero_vec4.set_zero4();
			phys_sphere sphere( zero_vec4, sphere_shape->get_convex_radius());

			phys_display_sphere* display_sphere = new phys_display_sphere(sphere, m_display_env.m_sphere_theta_res, m_display_env.m_sphere_phi_res);

			display_sphere->get_transform() = transform;
			display_geometries.array_push_back(display_sphere);

			break;
		}
		case PHYS_SHAPE_MULTI_SPHERE:
		{
			const phys_multi_sphere_shape* s = static_cast<const phys_multi_sphere_shape*>(shape);
			const phys_vec4* v = s->get_spheres();
			for(int i = 0; i < s->get_sphere_count(); ++i)
			{
				phys_sphere sphere( phys_vec4::get_zero(), v[i](3) );
				phys_display_sphere* display_sphere = new phys_display_sphere(sphere, m_display_env.m_sphere_theta_res, m_display_env.m_sphere_phi_res);

				display_sphere->get_transform().set_to_identity();
				display_sphere->get_transform().set_translation(v[i]);
				display_geometries.array_push_back(display_sphere);
			}

			break;
		}
		case PHYS_SHAPE_PLANE:
		{
			const phys_plane_shape* plane_shape = static_cast<const phys_plane_shape*>(shape);
			const phys_vec4& plane_eq = plane_shape->get_face_plane();
			phys_aabb plane_local_bounds;
			plane_shape->get_handle_aabb(phys_transform::get_identity(), 0.001f, plane_local_bounds);
			
			phys_vec4 c; c.set_sub4(plane_local_bounds.m_max_limit, plane_local_bounds.m_min_limit);
			phys_vec4 axis_perp_normal; axis_perp_normal.set_zero4(); axis_perp_normal(c.get_major_axis()) = 1;
			c.mul4(0.5f);
			phys_vec4 box_extent; box_extent.m_simd_quad = c.m_simd_quad;
			c.add4(plane_local_bounds.m_min_limit);

			phys_display_plane* display_plane = new phys_display_plane(plane_eq, axis_perp_normal, c, box_extent);
			display_plane->get_transform() = transform;
			display_geometries.array_push_back(display_plane);
			break;
		}
		case PHYS_SHAPE_CAPSULE:
		{
			const phys_capsule_shape* s = static_cast<const phys_capsule_shape*>(shape);

			phys_display_capsule* display_capsule = new phys_display_capsule( s->get_vertex(1), s->get_vertex(0), s->get_convex_radius() );
			display_capsule->get_transform() = transform;
			display_geometries.array_push_back( display_capsule );
	
			break;
		}
		case PHYS_SHAPE_CYLINDER:
		{
			const phys_cylinder_shape* s = static_cast<const phys_cylinder_shape*>(shape);

			phys_vec4 top;		top.vec_set_transformed_pos( transform, s->get_vertex(1) );
			phys_vec4 bottom;	bottom.vec_set_transformed_pos( transform, s->get_vertex(0) );
			phys_display_cylinder* display_cylinder = new phys_display_cylinder( top, bottom, s->get_cylinder_radius() );
			display_cylinder->get_transform() = transform;
			display_geometries.array_push_back( display_cylinder );
	
			break;
		}

		case PHYS_SHAPE_MULTI_RAY:
		{
			
			const phys_multi_ray_shape* s = static_cast<const phys_multi_ray_shape*>(shape);

			phys_display_geometry* display_geom = get_current_raw_geometry(display_geometries);
			phys_geometry* wire_body_cap = display_geom->get_geometry();

			int vert_base = 0;

			for(int j = 0; j < s->get_rays().getSize(); j++)
			{
				phys_multi_ray_shape::multi_ray_entry ray_segment; ray_segment = s->get_rays()[j];

				phys_vec4& start = *wire_body_cap->m_vertices.expand_by(1);
				start = ray_segment.m_start;
				start.vec_set_transformed_pos( transform, start );

				phys_vec4& joggle_vertex = *wire_body_cap->m_vertices.expand_by(1);
				joggle_vertex = ray_segment.m_start;
				phys_vec4 offset; offset.set( 0.01f, 0.f, 0.f );
				joggle_vertex.add4( offset );

				phys_vec4& end = *wire_body_cap->m_vertices.expand_by(1);
				end = ray_segment.m_end;
				end.vec_set_transformed_pos( transform, end );

				phys_geometry::geometry_triangle& tri_corners = *wire_body_cap->m_triangle_list.expand_by(1);
				tri_corners.set(vert_base, vert_base + 1, vert_base + 2);

				vert_base += 3;
			}
			break;
		}
		case PHYS_SHAPE_BOX:
		{
			const phys_box_shape* box_shape = static_cast<const phys_box_shape*>(shape);
			phys_display_box* display_box = new phys_display_box(box_shape->get_half_extents());
			display_box->get_transform() = transform;
			display_geometries.array_push_back(display_box);
			break;
		}
		
		case PHYS_SHAPE_CONVEX_OFFSET:
		{
			const phys_convex_offset_shape* ts = static_cast<const phys_convex_offset_shape*>( shape );
			phys_transform test_xform; test_xform.set_to_identity();	test_xform.set_translation( ts->get_translation());

			phys_transform T; T.set_mul( transform, test_xform );

			build_shape_display( ts->get_child_shape(), T, display_geometries);

			break;
		}

		case PHYS_SHAPE_CONVEX_XFORM:
		{
			const phys_convex_xform_shape* ts = static_cast<const phys_convex_xform_shape*>( shape );
			phys_transform T; T.set_mul( transform, ts->get_transform() );

			build_shape_display( ts->get_child_shape(), T, display_geometries);

			break;
		}	
		case PHYS_SHAPE_TRANSFORM:
		{
			const phys_transform_shape* ts = static_cast<const phys_transform_shape*>( shape );
			phys_transform T; T.set_mul( transform, ts->get_transform() );

			build_shape_display( ts->get_child_shape(), T, display_geometries);

			break;
		}
		case PHYS_SHAPE_BOUNDING:
		{
			const phys_bounding_shape* agent_bounding_shape = static_cast<const phys_bounding_shape*>(shape);
			build_shape_display( agent_bounding_shape->get_bounding_volume_shape(), transform, display_geometries);
			break;
		}
		





	

		case PHYS_SHAPE_BVH:
		case PHYS_SHAPE_BVH_TREE:
		{
			const phys_bvh_shape* agent_bounding_shape = static_cast<const phys_bvh_shape*>(shape);
			const phys_shape_container* shape_container = agent_bounding_shape->get_shape_container();

			phys_shape_buffer buffer;

			for (phys_shape_key key = shape_container->get_first_key(); key != PHYS_INVALID_SHAPE_KEY; key = shape_container->get_next_key( key ) )
			{
				const phys_shape* child = shape_container->get_child_shape(key, buffer );
				build_shape_display(child, transform, display_geometries);
			}
			break;
		}

		case PHYS_SHAPE_CONVEX_LIST:
		case PHYS_SHAPE_LIST:
		case PHYS_SHAPE_COMPOUND:
		case PHYS_SHAPE_TRIANGLE_COMPOUND:
		case PHYS_SHAPE_TRI_PATCH:
		case PHYS_SHAPE_BVH_EMBEDDED:
		{
			const phys_shape_container* shape_container = shape->get_shape_container();
			
			phys_shape_buffer buffer;

			for (phys_shape_key key = shape_container->get_first_key(); key != PHYS_INVALID_SHAPE_KEY; key = shape_container->get_next_key( key ) )
			{
				const phys_shape* child = shape_container->get_child_shape(key, buffer );
				build_shape_display(child, transform, display_geometries);
			}
		
			break;
		}
		case PHYS_SHAPE_TRIANGLE:
		{
			const phys_triangle_shape* tri_shape_out = static_cast<const phys_triangle_shape*>(shape);

			phys_display_geometry* display_geom = get_current_raw_geometry(display_geometries);
			phys_geometry* wire_body_cap = display_geom->get_geometry();

				
			int vert_base = wire_body_cap->m_vertices.getSize();

			wire_body_cap->m_vertices.expand_by(1)->vec_set_transformed_pos(transform, tri_shape_out->get_vertex(0));
			wire_body_cap->m_vertices.expand_by(1)->vec_set_transformed_pos(transform, tri_shape_out->get_vertex(1));
			wire_body_cap->m_vertices.expand_by(1)->vec_set_transformed_pos(transform, tri_shape_out->get_vertex(2));
			phys_geometry::geometry_triangle& tri_corners = *wire_body_cap->m_triangle_list.expand_by(1);
			tri_corners.set(vert_base, vert_base + 1, vert_base + 2);

			if ( tri_shape_out->get_extrusion().length_3d() > 0 )
			{
				phys_vec4 extruded_v0; extruded_v0.set_add4(tri_shape_out->get_vertex(0), tri_shape_out->get_extrusion());
				phys_vec4 extruded_v1; extruded_v1.set_add4(tri_shape_out->get_vertex(1), tri_shape_out->get_extrusion());
				phys_vec4 extruded_v2; extruded_v2.set_add4(tri_shape_out->get_vertex(2), tri_shape_out->get_extrusion());
				wire_body_cap->m_vertices.expand_by(1)->vec_set_transformed_pos(transform, extruded_v0);
				wire_body_cap->m_vertices.expand_by(1)->vec_set_transformed_pos(transform, extruded_v1);
				wire_body_cap->m_vertices.expand_by(1)->vec_set_transformed_pos(transform, extruded_v2);

				phys_geometry::geometry_triangle* extruded_tris = wire_body_cap->m_triangle_list.expand_by(7);
				extruded_tris[0].set(vert_base + 3, vert_base + 1, vert_base + 0);
				extruded_tris[1].set(vert_base + 3, vert_base + 4, vert_base + 1);
				extruded_tris[2].set(vert_base + 4, vert_base + 5, vert_base + 1);
				extruded_tris[3].set(vert_base + 5, vert_base + 2, vert_base + 1);
				extruded_tris[4].set(vert_base + 5, vert_base + 0, vert_base + 2);
				extruded_tris[5].set(vert_base + 5, vert_base + 3, vert_base + 0);
				extruded_tris[6].set(vert_base + 5, vert_base + 4, vert_base + 3);
			}

			break;
		}

		case PHYS_SHAPE_CONVEX_HULL:
		case PHYS_SHAPE_PACKED_HULL:
		{
			int child_sphere_count = shape->get_type_id() == PHYS_SHAPE_CONVEX_HULL
				? static_cast<const phys_convex_hull_shape*>(shape)->get_collision_sphere_count()
				: static_cast<const phys_packed_hull_shape*>(shape)->get_collision_sphere_count();

			phys_local_array<phys_sphere> vertices(child_sphere_count); vertices.set_array_size( child_sphere_count );
			
			const phys_sphere* spheres = shape->get_type_id() == PHYS_SHAPE_CONVEX_HULL
				? static_cast<const phys_convex_hull_shape*>(shape)->get_collision_spheres(vertices.begin())
				: static_cast<const phys_packed_hull_shape*>(shape)->get_collision_spheres(vertices.begin());

				
			phys_array<phys_vec4> transformed_vertices;
			transformed_vertices.set_array_size( child_sphere_count );
			for(int i = 0; i < child_sphere_count; i++)
			{
				transformed_vertices[i].vec_set_transformed_pos(transform, spheres[i].get_contact_position());
			}

			phys_geometry* output_geom = new phys_geometry;
			phys_array<phys_vec4> used_vertices;
			phys_strided_vertices strided_verts;
			{
				strided_verts.m_hull_vertex_count = transformed_vertices.getSize();
				strided_verts.m_vertex_stride_bytes = sizeof(phys_vec4);
				strided_verts.m_vertices = &(transformed_vertices[0](0));
			}
			phys_geometry_util::build_convex_geometry( strided_verts, *output_geom, PHYS_HULL_MODE_FAST );

			phys_display_convex* display_geom = new phys_display_convex(output_geom);

			display_geometries.array_push_back(display_geom);
			break;
		}
		case PHYS_SHAPE_CONVEX_PIECE:
		{
			const phys_convex_piece_shape* convex_piece_shape = static_cast<const phys_convex_piece_shape*>(shape);

			
			phys_geometry* output_geom = new phys_geometry;

			phys_shape_buffer buffer2;

			for ( int i = 0 ; i < convex_piece_shape->m_display_shape_key_count ; i++ )
			{
				const phys_triangle_shape& tri_shape_out = *( static_cast< const phys_triangle_shape* >( 
					convex_piece_shape->m_display_mesh->get_child_shape( convex_piece_shape->m_display_shape_keys[i], buffer2 ) ));

				
				phys_geometry::geometry_triangle& tri_corners = *output_geom->m_triangle_list.expand_by(1);

				int base_vertex_cnt = output_geom->m_vertices.getSize();
				tri_corners.set( base_vertex_cnt, base_vertex_cnt+1, base_vertex_cnt+2	);

				for ( int j = 0 ; j < 3 ; j++ )
				{
					phys_vec4& transformed_vertex = *output_geom->m_vertices.expand_by(1);
					transformed_vertex.vec_set_transformed_pos(transform, tri_shape_out.get_vertex( j ));
				}
			}
			
			phys_display_convex* display_geom = new phys_display_convex(output_geom);
			display_geometries.array_push_back(display_geom);
			
			break;
		}

		case PHYS_SHAPE_SAMPLED_HEIGHTFIELD:
		{
			const phys_heightfield_shape* height_field = static_cast<const phys_heightfield_shape*>(shape);

			phys_display_geometry* display_geom = get_current_raw_geometry(display_geometries);
			phys_geometry* wire_body_cap = display_geom->get_geometry();

				
			phys_vec4 scale; scale.m_simd_quad = height_field->m_quant_scale.m_simd_quad;

			for (int i = 0; i < height_field->m_x_res-1; i++)
			{
				for (int j = 0; j < height_field->m_z_res-1; j++)
				{
					phys_vec4 grid_p00; grid_p00.set( i+0.f, height_field->get_height_at( i+0, j+0 ), j+0.f ); grid_p00.mul4( scale );
					phys_vec4 grid_p01; grid_p01.set( i+0.f, height_field->get_height_at( i+0, j+1 ), j+1.f ); grid_p01.mul4( scale );
					phys_vec4 p10; p10.set( i+1.f, height_field->get_height_at( i+1, j+0 ), j+0.f ); p10.mul4( scale );
					phys_vec4 p11; p11.set( i+1.f, height_field->get_height_at( i+1, j+1 ), j+1.f ); p11.mul4( scale );

					{
						int vert_base = wire_body_cap->m_vertices.getSize();

						wire_body_cap->m_vertices.expand_by(1)->vec_set_transformed_pos(transform, grid_p00 );
						wire_body_cap->m_vertices.expand_by(1)->vec_set_transformed_pos(transform, grid_p01 );
						wire_body_cap->m_vertices.expand_by(1)->vec_set_transformed_pos(transform, p10 );
						wire_body_cap->m_vertices.expand_by(1)->vec_set_transformed_pos(transform, p11 );

						if ( height_field->get_triangle_flip())
						{
							wire_body_cap->m_triangle_list.expand_by(1)->set(vert_base + 0, vert_base + 1, vert_base + 3);
							wire_body_cap->m_triangle_list.expand_by(1)->set(vert_base + 0, vert_base + 3, vert_base + 2);
						}
						else
						{
							wire_body_cap->m_triangle_list.expand_by(1)->set(vert_base + 0, vert_base + 1, vert_base + 2);
							wire_body_cap->m_triangle_list.expand_by(1)->set(vert_base + 3, vert_base + 2, vert_base + 1);
						}
					}
				}
			}

			break;			
		}

		default:
		{
			for (int i = 0; i < get_user_shape_display_builder().m_user_shape_builders.getSize(); ++i )
			{
				if ( get_user_shape_display_builder().m_user_shape_builders[i].type == shape->get_type_id() )
				{
					get_user_shape_display_builder().m_user_shape_builders[i].f( shape, transform, display_geometries, this );
					continue;
				}
			}
		}
	}
}
void phys_shape_display_builder::build_display_geometries(		const phys_shape* shape,
														phys_array<phys_display_geometry*>& display_geometries)
{

	phys_transform transform;
	transform.set_to_identity();

	reset_current_raw_geometry();
	display_geometries.clear();

	build_shape_display(shape, transform, display_geometries);
}

phys_display_geometry* phys_shape_display_builder::get_current_raw_geometry(phys_array<phys_display_geometry*>& display_geometries)
{
	if (m_current_geometry == 0)
	{
		phys_geometry* wire_body_cap = new phys_geometry;
		m_current_geometry = new phys_display_convex(wire_body_cap);
		display_geometries.array_push_back(m_current_geometry);
	}
	return m_current_geometry;
}

void phys_shape_display_builder::reset_current_raw_geometry()
{
	m_current_geometry = 0;
}


void phys_user_shape_display_builder::register_user_shape_display( shape_display_builder_fn f, phys_shape_type type )
{
	for (int i = 0; i < m_user_shape_builders.getSize(); ++i )
	{
		if ( m_user_shape_builders[i].type == type )
		{
			return;
		}
	}
	user_shape_builder b;
	b.f = f;
	b.type = type;

	m_user_shape_builders.array_push_back(b);
}


phys_ref_object* PHYS_CALL phys_user_shape_display_create()
{
	return new phys_user_shape_display_builder();
}

phys_singleton_init_node phys_singleton_reg_user_shape_display = phys_singleton_init_node(phys_user_shape_display_create,
	reinterpret_cast<void **>(&phys_singleton<phys_user_shape_display_builder>::s_instance));

void PHYS_CALL phys_shape_display_builder::add_object_to_debug_display(const phys_shape* shape, phys_transform& t, phys_ulong id)
{
	phys_shape_display_builder::phys_shape_display_env collision_env;
	phys_shape_display_builder builder(collision_env);
	phys_array<phys_display_geometry*> display_geometries;
	builder.build_display_geometries(shape, display_geometries);
	phys_debug_display::get_singleton_instance().add_geometry(display_geometries, t, id, 0);
	while (display_geometries.getSize() != 0)
	{
		delete display_geometries[0];
		display_geometries.remove_at(0);
	}
}

