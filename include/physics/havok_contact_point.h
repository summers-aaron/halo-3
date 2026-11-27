#ifndef PHYSICS_HAVOK_CONTACT_POINT_H
#define PHYSICS_HAVOK_CONTACT_POINT_H

#include "game/materials.h"
#include "math/real_math_types.h"
#include <stddef.h>
#include "cseries/crt_xdk.h"

class phys_rigid_body;
class phys_vec4;
class phys_dynamics_contact_manager;
class phys_shape;
struct phys_contact_point_props;

// Contact point shared by the havok listeners and collision damage code.
class c_havok_contact_point
{
public:
    c_havok_contact_point() {}
    c_havok_contact_point(unsigned short contact_point_id, long structure_bsp_index, unsigned int environment_shape_key, long surface_index,
        long other_object_index,
        unsigned char flags, phys_rigid_body *rigid_body, phys_rigid_body *other_rigid_body, float friction, float restitution,
        c_global_material_type material_a, c_global_material_type material_b);
    bool matches_contact_description(unsigned short contact_point_id, long bsp_index, long other_havok_component_index,
        long rigid_body_index, long other_rigid_body_index) const
    {
        return m_contact_point_id == contact_point_id
            && m_structure_bsp_index == bsp_index
            && m_other_havok_component_index == other_havok_component_index
            && rigid_body_index == m_rigid_body_index
            && other_rigid_body_index == m_other_rigid_body_index;
    }
    __forceinline bool is_physics_only_surface() const { return (m_flags & (1 << 1)) != 0; }
    __forceinline bool is_climbable_surface() const { return (m_flags & (1 << 2)) != 0; }
    void get_surface_normal(real_vector3d *surface_normal) const;
    void update(bool unused, const phys_vec4 *position, const phys_vec4 *normal, phys_dynamics_contact_manager *contact_manager,
        const phys_shape *shape, bool reversed, long component_index, long rigid_body_index);
    bool get_life_cycle_flag_is_deleted() const { return (bool)(_rotl(m_life_cycle_flags, 31) & 1); }
    void notify_is_deleted() { m_life_cycle_flags |= 2; }
    void update_contact_point_properties(phys_contact_point_props *properties, long component_index);
    // Inline accessors for the contacted component, object, and surface normal.
    __forceinline long get_contact_havok_component_index() const { return m_other_havok_component_index; }
    __forceinline long get_contact_point_object_index() const { return m_other_object_index; }
    __forceinline const real_vector3d *get_normal() const { return &m_normal; }
    // March accessors: 82a63930, 82a63968, 82a63c50, 82a63c90, 82b798b8.
    __forceinline long get_impact_datum_index() const { return m_impact_datum_index; }
    __forceinline void set_impact_datum_index(long index) { m_impact_datum_index = index; }
    __forceinline long get_rigid_body_index_a() const { return m_rigid_body_index; }
    __forceinline long get_rigid_body_index_b() const { return m_other_rigid_body_index; }
    __forceinline long get_structure_bsp_index() const { return m_structure_bsp_index; }
    __forceinline unsigned long get_environment_shape_key() const { return m_environment_shape_key; }
    __forceinline c_global_material_type get_global_material_type_b() const { return m_global_material_type_b; }
    __forceinline const real_point3d *get_position() const { return &m_position; }
    __forceinline c_global_material_type calculate_global_material_type_a() const
    {
        return global_material_resolve_underwater_material_proxy(m_global_material_type_a, &m_position);
    }
    c_global_material_type calculate_global_material_type_b() const;

    unsigned long m_environment_shape_key;
    long m_impact_datum_index;
    long m_surface_index;
    c_global_material_type m_global_material_type_a;
    c_global_material_type m_global_material_type_b;
    long m_other_havok_component_index;
    long m_other_object_index;
    real_point3d m_position;
    real_vector3d m_normal;
    float m_distance;
    float m_contact_friction;
    float m_contact_restitution;
    unsigned short m_contact_point_id;
    short m_life_cycle_flags;
    unsigned char m_flags;
    signed char m_rigid_body_index;
    signed char m_other_rigid_body_index;
    signed char m_structure_bsp_index;
};

typedef char havok_contact_point_size[(sizeof(c_havok_contact_point) == 68) ? 1 : -1];
typedef char havok_contact_point_life_cycle_flags_offset[(offsetof(c_havok_contact_point, m_life_cycle_flags) == 0x3e) ? 1 : -1];

#endif
