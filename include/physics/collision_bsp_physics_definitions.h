#ifndef PHYSICS_COLLISION_BSP_PHYSICS_DEFINITIONS_H
#define PHYSICS_COLLISION_BSP_PHYSICS_DEFINITIONS_H

// Functions defined in tu/blamlibxbox/collision_bsp_physics_definitions.cpp that other units call.

struct collision_bsp;
struct s_collision_bsp_physics_definition {
#ifdef HALO3_64BIT
    char m_geometry_shape[0x70];
    char m_mopp_shape[0x40];
#else
    char m_geometry_shape[80];
    char m_mopp_shape[32];
#endif
};
struct s_tag_block;

bool collision_bsp_physics_definition_fixup(s_collision_bsp_physics_definition *definition, long index, s_tag_block *mopp_codes, const collision_bsp *bsp);


#endif // PHYSICS_COLLISION_BSP_PHYSICS_DEFINITIONS_H
