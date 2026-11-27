#ifndef INCLUDE_PHYS_INERTIA_UTIL_H
#define INCLUDE_PHYS_INERTIA_UTIL_H

#include "phys/phys_base.h"

struct phys_geometry;
class phys_shape;
class phys_rigid_body_desc;
class phys_rigid_body;
class phys_constraint;

struct phys_mass_props
{
    PHYS_DECLARE_NONVIRTUAL_ALLOCATOR(PHYS_MEM_INERTIA, phys_mass_props);

    phys_real    m_body_volume;

    phys_real    m_body_mass;

    phys_vec4 m_center_of_mass;

    phys_mat3 m_inertia_tensor;

    phys_mass_props() :
        m_body_volume(0.0f),
        m_body_mass(0.0f)
    {
        m_center_of_mass.set_zero4();
        m_inertia_tensor.set_zero();
    }

};

struct phys_mass_element
{
    PHYS_DECLARE_NONVIRTUAL_ALLOCATOR(PHYS_MEM_INERTIA, phys_mass_element);

    phys_mass_props m_properties;

    phys_transform m_body_transform;

    phys_mass_element()
    {
        m_body_transform.set_to_identity();
    }

    phys_mass_element(const phys_mass_props& properties,const phys_transform& transform) :
        m_properties(properties),
        m_body_transform(transform)
    {
    }

};


void PHYS_CALL recursive_shape_mass_props(const phys_shape* shape, const phys_transform& transform, phys_mass_props& mass_props_out);
void PHYS_CALL scale_mass_props(phys_real scale, phys_mass_props& mass_props);

class phys_inertia_util
{
    public:

        static phys_result PHYS_CALL sphere_volume_mass_props( phys_real radius, phys_real mass, phys_mass_props& result);

        static phys_result PHYS_CALL box_volume_mass_props(const phys_vec4& box_half_extents, phys_real mass, phys_mass_props& result);

        static phys_result PHYS_CALL capsule_volume_mass_props(
            const phys_vec4 &start_axis, const phys_vec4 &end_axis, phys_real radius, phys_real mass, phys_mass_props &result);

        static phys_result PHYS_CALL sphere_surface_mass_props( phys_real radius, phys_real mass, phys_real surface_thickness, phys_mass_props& result);

        static phys_result PHYS_CALL box_surface_mass_props(const phys_vec4& box_half_extents, phys_real mass,
            phys_real surface_thickness, phys_mass_props& result);

        static phys_result PHYS_CALL triangle_surface_mass_props(
            const phys_vec4 &v0, const phys_vec4 &v1, const phys_vec4 &v2, phys_real mass, phys_real surface_thickness, phys_mass_props &result);

        static phys_result PHYS_CALL cylinder_volume_mass_props(
            const phys_vec4 &start_axis, const phys_vec4 &end_axis, phys_real radius, phys_real mass, phys_mass_props &result);

        static phys_result PHYS_CALL vertex_hull_mass_props(
            const phys_real *vertex_in, int inertia_vertex_stride, int hull_vertex_count, phys_real mass, phys_mass_props &result);

        static phys_result PHYS_CALL vertex_cloud_mass_props(const phys_real* vertex_in, int inertia_vertex_stride,
            int hull_vertex_count, phys_real mass, phys_mass_props &result);

        static phys_result PHYS_CALL geometry_surface_mass_props(
            const phys_geometry *wire_body_cap, phys_real surface_thickness, phys_bool distribute_uniformly, phys_real mass, phys_mass_props &result);

        static void PHYS_CALL geometry_volume_mass_props(const phys_geometry* wire_body_cap, phys_real mass, phys_mass_props &result);

        static phys_result PHYS_CALL geometry_volume_mass_props_checked(const phys_geometry* wire_body_cap, phys_real mass, phys_mass_props &result);

        static phys_result PHYS_CALL combine_mass_props(const phys_array<phys_mass_element>& elements, phys_mass_props& result );

        static void PHYS_CALL shape_volume_mass_props(const phys_shape* shape, phys_real mass, phys_mass_props &result);

        static void PHYS_CALL set_shape_volume_mass_props(const phys_shape* shape, phys_real mass, phys_rigid_body_desc& body_desc);

        static void PHYS_CALL clip_inertia_ratio(phys_real max_inertia_ratio, phys_rigid_body_desc& body_desc);

        static void PHYS_CALL simplify_inertia_to_particle(phys_mat3 &inertia);

        static void PHYS_CALL inertia_to_principal_axes( phys_mat3& inertia, phys_rotation& principal_axis_out );

        static void PHYS_CALL optimize_tree_inertias(
            phys_constraint *const *constraints, int solver_constraint_count, phys_rigid_body *root_body, phys_real inertia_factor_hint = 1.5f);

        static void PHYS_CALL shift_inertia_to_com(phys_vec4& shift, phys_real mass, phys_mat3& inertia);

        static void PHYS_CALL shift_inertia_from_com(phys_vec4& shift, phys_real mass, phys_mat3& inertia);

};

typedef char phys_mass_props_size[(sizeof(phys_mass_props) == 0x50) ? 1 : -1];
typedef char phys_mass_props_center_offset[(offsetof(phys_mass_props, m_center_of_mass) == 0x10) ? 1 : -1];
typedef char phys_mass_props_inertia_offset[(offsetof(phys_mass_props, m_inertia_tensor) == 0x20) ? 1 : -1];
typedef char phys_mass_element_size[(sizeof(phys_mass_element) == 0x90) ? 1 : -1];

#endif
