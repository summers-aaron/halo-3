#ifndef INCLUDE_PHYS_PACKED_HULL_SHAPE_H
#define INCLUDE_PHYS_PACKED_HULL_SHAPE_H

#include "phys/phys_collide.h"
#include "phys/phys_strided_vertices.h"

class phys_packed_hull_shape : public phys_convex_shape
{
	public:

		PHYS_DECLARE_REFLECTION();
		
		phys_packed_hull_shape(phys_strided_vertices verts_in, const phys_array<phys_vec4>& hull_plane_equations,
			phys_real radius = phys_convex_shape_default_radius);

		virtual void get_support_vertex_impl(phys_vec4_param direction, phys_hull_vertex& supporting_vertex_out) const;

		virtual void vertex_ids_to_vertices_impl(const phys_vertex_id* ids, int id_count, phys_hull_vertex* vertices_out) const;

		virtual void get_shape_centre_impl(phys_vec4& centre_out) const;

		virtual int get_collision_sphere_count_impl() const;

		virtual const phys_sphere*	get_collision_spheres_impl(phys_sphere* sphere_buffer) const;

		virtual void get_aabb_impl(const phys_transform& local_to_world, phys_real tolerance, phys_aabb& out) const;

		virtual phys_bool trace_ray_impl(const phys_ray_input& input, phys_ray_output& results) const;

		void get_first_vertex(phys_vec4& v) const;




		virtual void gather_memory_stats( phys_statistics_collector* collector) const;

		virtual int compute_dma_size(int buffer_size_left) const;

		void compress_vertex_data(const float* vertex_in, int byte_stride, int hull_vertex_count );

		struct four_vecs
		{
			PHYS_DECLARE_NONVIRTUAL_ALLOCATOR( PHYS_MEM_COLLIDE, phys_packed_hull_shape::four_vecs );
			PHYS_DECLARE_REFLECTION();
			unsigned char m_x[4];
			unsigned char m_y[4];
			unsigned char m_soa_z[4];
		};

		class vec4_int_w 
		{
		public:
			PHYS_DECLARE_NONVIRTUAL_ALLOCATOR( PHYS_MEM_COLLIDE, phys_packed_hull_shape::vec4_int_w );
			PHYS_DECLARE_REFLECTION();

			PHYS_ALIGN16(phys_real m_x); 
			phys_real m_y; 
			phys_real m_soa_z; 
			unsigned int m_packed_w; 
		}; 

	public:   

		enum { PACKED_HULL_INLINE_QUADS=16 };   

		phys_array<phys_vec4> m_plane_equations; 
		
		phys_vec4 m_aabb_min; 
		phys_vec4 m_aabb_extents; 
		struct four_vecs m_vertices[phys_packed_hull_shape::PACKED_HULL_INLINE_QUADS]; 
	
	public:

		phys_packed_hull_shape( phys_fixup_tag flag ) : phys_convex_shape(flag), m_plane_equations(flag) { m_type = PHYS_SHAPE_PACKED_HULL; }
};


typedef char phys_packed_hull_shape_size[(sizeof(void *) != 4 || (sizeof(phys_packed_hull_shape) == 0x100)) ? 1 : -1];

#endif
