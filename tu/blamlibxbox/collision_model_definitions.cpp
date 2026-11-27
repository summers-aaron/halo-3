// blamlibxbox/collision_model_definitions.obj: from the records 0x1c15c0 0x1c16b8
// @flags /TP /O2 /Oc
// @address 0x1c15c0 ?collision_model_fixup@@YAXJ@Z
// @address 0x262d90 ?collision_bsp_physics_definition_fixup@@YA_NPAUs_collision_bsp_physics_definition@@JPAUs_tag_block@@PBUcollision_bsp@@@Z
// @address 0x262eb0 ?collision_instanced_geometry_definition_fixup@@YAXPAUs_collision_instanced_geometry_definition@@JJ@Z
// @address 0x7cff0c ?global_tag_instances@@3PBUcache_file_tag_instance@@B
#include <stddef.h>
#include <ppcintrinsics.h>
#include <math.h>
#include <stdlib.h>
#include "math/matrix_math_types.h"
#include "tag_files/tag_types.h"
#include "physics/havok_mopp_code_definitions.h"
#include "physics/collision_model_definitions.h"
#include "physics/collision_bsp_physics_definitions.h"
#include "physics/collision_instanced_geometry_definitions.h"
#include "physics/collision_bsp_types.h"
#include "physics/collision_instanced_geometry_definition_types.h"
#include "tag_files/tag_groups.h"
#include "structures/structure_bsp_tag.h"
#include "physics/collision_model_definition_types.h"
#include "structures/structure_instanced_geometry_types.h"
#include "structures/structure_instanced_geometry.h"
#include "memory/datum_index.h"
#ifdef HALO3_64BIT
#include "physics/havok_tag_unpack_64.h"
#include "physics/havok_collision_geometry_shape.h"
#include "physics/physics_model_shape_internal.h"
#endif

typedef char instanced_geometry_resource_size[(sizeof(s_structure_instanced_geometry_resource) == 0xb8) ? 1 : -1];
typedef char instanced_geometry_resource_collision_offset[(offsetof(s_structure_instanced_geometry_resource, collision_info) == 0x14) ? 1 : -1];
typedef char instanced_geometry_resource_mopp_offset[(offsetof(s_structure_instanced_geometry_resource, collision_mopp_codes) == 0x80) ? 1 : -1];

#ifdef HALO3_64BIT
static void unpack_collision_bsp_physics_block_64(s_tag_block *block)
{
    if (block->count <= 0 || tag_block_is_unpacked_64(block))
        return;

    s_collision_bsp_physics_definition *dst = (s_collision_bsp_physics_definition *)havok_unpack_alloc_64(
        (size_t)block->count * sizeof(s_collision_bsp_physics_definition));
    const char *src_base = (const char *)block->address;
    for (long i = 0; i < block->count; i++)
    {
        const char *src = src_base + (size_t)i * 0x70;
        c_collision_geometry_shape *geom = (c_collision_geometry_shape *)dst[i].m_geometry_shape;
        memcpy(&geom->m_aabb_center, src + 0x20, sizeof(phys_vec4));
        memcpy(&geom->m_aabb_half_extents, src + 0x30, sizeof(phys_vec4));
        memcpy(&geom->m_geometry_index, src + 0x40, sizeof(long));
        geom->m_structure_bsp_index = *(const signed char *)(src + 0x48);
        geom->m_type = *(const unsigned char *)(src + 0x49);
        memcpy(&geom->m_instance_index, src + 0x4a, sizeof(short));
        memcpy(&geom->m_radius, src + 0x4c, sizeof(float));

        c_mopp_bv_tree_shape *mopp = (c_mopp_bv_tree_shape *)dst[i].m_mopp_shape;
        memcpy(&mopp->m_scale, src + 0x6c, sizeof(float));
    }
    block->address = dst;
    tag_block_mark_unpacked_64(block);
}

void collision_model_unpack_bsp_instances_64(structure_bsp *bsp)
{
    s_tag_block &instances = bsp->instanced_geometry_instances;
    structure_instanced_geometry_instance *entries = (structure_instanced_geometry_instance *)instances.address;
    for (long index = 0; index < instances.count; ++index)
        unpack_collision_bsp_physics_block_64(&entries[index].physics);
}
#endif

void collision_model_fixup(long index)
{
    const cache_file_tag_instance *instances = global_tag_instances;
    collision_model_definition *definition = (collision_model_definition *)instances[datum_index_to_absolute_index(index)].definition;
    if ((definition->flags & 1) == 0) {
        for (int region_index = 0; region_index < definition->regions.count; region_index = region_index + 1) {
            s_collision_model_region *region = &((s_collision_model_region *)definition->regions.address)[region_index];
            for (int permutation_index = 0; permutation_index < region->permutations.count; permutation_index = permutation_index + 1) {
                s_collision_model_permutation *permutation = &((s_collision_model_permutation *)region->permutations.address)[permutation_index];
#ifdef HALO3_64BIT
                unpack_collision_bsp_physics_block_64(&permutation->physics);
#endif
                for (int physics_index = 0; physics_index < permutation->physics.count; physics_index = physics_index + 1) {
                    s_collision_bsp_physics_definition *physics = (s_collision_bsp_physics_definition *)permutation->physics.address;
                    if (permutation->mopp_codes.count > 0) {
                        s_collision_model_bsp *bsps = (s_collision_model_bsp *)permutation->bsps.address;
                        collision_bsp_physics_definition_fixup((s_collision_bsp_physics_definition *)&physics[physics_index],
                            physics_index, &permutation->mopp_codes,
                            &bsps[physics_index].bsp);
                    }
                }
            }
        }
    }
}

// @address 0x1c16b8 collision_model_instance_list_fixup
bool collision_model_instance_list_fixup(long handle, void *list)
{
    const cache_file_tag_instance *instances = global_tag_instances;
    structure_bsp *definition = (structure_bsp *)instances[datum_index_to_absolute_index(handle)].definition;
    s_structure_bsp_resources *resources = (s_structure_bsp_resources *)list;
    s_structure_bsp_resources *resources_alias = resources;
    for (int i = 0; i < resources->instanced_geometry_definitions.count; i = i + 1) {
        s_structure_instanced_geometry_resource *entries = (s_structure_instanced_geometry_resource *)
            resources_alias->instanced_geometry_definitions.address;
        ::havok_mopp_code_definitions_fixup(&entries[i].collision_mopp_codes);
    }
    for (int attachment_index = 0; attachment_index < definition->instanced_geometry_instances.count; attachment_index = attachment_index + 1) {
        structure_instanced_geometry_instance *instance = &((structure_instanced_geometry_instance *)
            definition->instanced_geometry_instances.address)[attachment_index];
        s_structure_instanced_geometry_resource *entry = &((s_structure_instanced_geometry_resource *)
            resources->instanced_geometry_definitions.address)[instance->definition_index];
        if (instance->physics_count > 0) {
            collision_bsp_physics_definition_fixup((s_collision_bsp_physics_definition *)instance->physics.address, 0,
                &entry->collision_mopp_codes, &entry->collision_info);
        }
    }
    for (int marker_index = 0; marker_index < definition->clusters.count; marker_index = marker_index + 1) {
        collision_instanced_geometry_definition_fixup(&((structure_cluster *)definition->clusters.address)[marker_index].collision_instanced_geometry,
            handle, marker_index);
    }
    return true;
}
