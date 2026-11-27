#ifndef PHYSICS_COLLISION_INSTANCED_GEOMETRY_DEFINITION_TYPES_H
#define PHYSICS_COLLISION_INSTANCED_GEOMETRY_DEFINITION_TYPES_H

struct phys_shape;

struct s_collision_instanced_geometry_definition {
    char m_cluster_shape[44];
    char m_mopp_shape[28];
    s_tag_block m_mopp_codes;
};

static __forceinline const phys_shape *collision_instanced_geometry_get_mopp_shape(
    const s_collision_instanced_geometry_definition *definition)
{
#ifdef HALO3_64BIT
    return *(const phys_shape *const *)definition->m_mopp_shape;
#else
    return (const phys_shape *)definition->m_mopp_shape;
#endif
}

#endif // PHYSICS_COLLISION_INSTANCED_GEOMETRY_DEFINITION_TYPES_H
