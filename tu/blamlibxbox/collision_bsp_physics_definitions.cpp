// blamlibxbox/collision_bsp_physics_definitions.obj: from the records 0x262d90
// @flags /TP /O2 /Oc
#include "tag_files/tag_groups.h"
#include "physics/collision_bsp_physics_definitions.h"
#include "physics/havok_mopp_code_definitions.h"
#include "physics/collision_bsp_types.h"
#include "physics/havok_collision_geometry_shape.h"
#include "physics/physics_model_shape_internal.h"
#include <new>

// @address 0x262d90 ?collision_bsp_physics_definition_fixup@@YA_NPAUs_collision_bsp_physics_definition@@JPAUs_tag_block@@PBUcollision_bsp@@@Z
// @address 0x405e4 k_collision_geometry_shape_vtable
// @address 0x40604 k_collision_geometry_container_vtable
// @address 0x139c ??_7c_mopp_bv_tree_shape@@6B@
bool collision_bsp_physics_definition_fixup(s_collision_bsp_physics_definition *definition,
    long index, s_tag_block *mopp_codes, const collision_bsp *bsp)
{
    bool result = false;
    if (bsp->surfaces.count > 0) {
        ::havok_mopp_code_definitions_fixup(mopp_codes);
        phys_bvh_bytecode *code = (phys_bvh_bytecode *)tag_block_get_element_with_size(mopp_codes, index, sizeof(s_havok_mopp_code_definition));
        c_collision_geometry_shape *geometry = new (definition->m_geometry_shape)
            c_collision_geometry_shape(phys_fixup_tag(), bsp);
        c_mopp_bv_tree_shape *mopp = new (definition->m_mopp_shape)
            c_mopp_bv_tree_shape(phys_fixup_tag(), 0, (phys_compound_shape *)definition, code);
        result = true;
    }
    return result;
}
