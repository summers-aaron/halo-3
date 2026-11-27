// blamlibxbox/character_physics_definitions.obj: from the records 0x2df710
// @flags /TP /O2 /Oc
#include "tag_files/tag_groups.h"
#include <stddef.h>
#include "physics/character_physics_definitions.h"
#include "physics/character_physics_shapes.h"
#include "phys/phys_base.h"
#include "physics/character_physics_definitions_internals.h"
#ifdef HALO3_64BIT
#include "physics/havok_tag_unpack_64.h"

static void unpack_character_sphere_block_64(s_tag_block *block)
{
    if (block->count <= 0 || tag_block_is_unpacked_64(block))
        return;

    sphere_definition *dst = (sphere_definition *)havok_unpack_alloc_64(
        (size_t)block->count * sizeof(sphere_definition));
    const char *src_base = (const char *)block->address;
    for (long i = 0; i < block->count; i++)
    {
        const char *src = src_base + (size_t)i * 0x70;
        memcpy(&dst[i].properties, src + 0x00, sizeof(s_character_physics_shape_properties));
        float sphere_radius;
        memcpy(&sphere_radius, src + 0x30, sizeof(float));
        dst[i].sphere.set_shape_radius(sphere_radius);
        float transform_radius;
        memcpy(&transform_radius, src + 0x50, sizeof(float));
        dst[i].transform.set_shape_radius(transform_radius);
        memcpy(&dst[i].transform.get_translation(), src + 0x60, sizeof(phys_vec4));
    }
    block->address = dst;
    tag_block_mark_unpacked_64(block);
}

static void unpack_character_pill_block_64(s_tag_block *block)
{
    if (block->count <= 0 || tag_block_is_unpacked_64(block))
        return;

    pill_definition *dst = (pill_definition *)havok_unpack_alloc_64(
        (size_t)block->count * sizeof(pill_definition));
    const char *src_base = (const char *)block->address;
    for (long i = 0; i < block->count; i++)
    {
        const char *src = src_base + (size_t)i * 0x60;
        memcpy(&dst[i].properties, src + 0x00, sizeof(s_character_physics_shape_properties));
        float pill_radius;
        memcpy(&pill_radius, src + 0x30, sizeof(float));
        dst[i].pill.set_shape_radius(pill_radius);
        memcpy(const_cast<phys_vec4 *>(dst[i].pill.get_vertices()), src + 0x40, sizeof(phys_vec4) * 2);
    }
    block->address = dst;
    tag_block_mark_unpacked_64(block);
}
#endif

// @address 0x1c00d8 ??0c_transform_shape@@QAA@Vphys_fixup_tag@@JPAVphys_shape@@@Z
// @address 0x2df710 ?character_physics_definition_fixup@@YAXPAUs_character_physics_definition@@@Z
void character_physics_definition_fixup(s_character_physics_definition *definition)
{
#ifdef HALO3_64BIT
    unpack_character_sphere_block_64(&definition->dead_spheres);
    unpack_character_pill_block_64(&definition->pills);
    unpack_character_sphere_block_64(&definition->spheres);
#endif
    for (long i = 0; i < definition->dead_spheres.count; i++)
    {
        sphere_definition *sphere = (sphere_definition *)character_physics_definitions_get_element(
            &definition->dead_spheres, i, sizeof(sphere_definition));
        c_sphere_shape *shape = new (&sphere->sphere) c_sphere_shape(phys_fixup_tag());
        c_transform_shape *transform = new (&sphere->transform) c_transform_shape(phys_fixup_tag(), 0, &sphere->sphere);
    }
    for (long i = 0; i < definition->pills.count; i++)
    {
        pill_definition *pill = (pill_definition *)character_physics_definitions_get_element(&definition->pills, i, sizeof(pill_definition));
        c_capsule_shape *shape = new (&pill->pill) c_capsule_shape(phys_fixup_tag());
    }
    for (long i = 0; i < definition->spheres.count; i++)
    {
        sphere_definition *sphere = (sphere_definition *)character_physics_definitions_get_element(&definition->spheres, i, sizeof(sphere_definition));
        c_sphere_shape *shape = new (&sphere->sphere) c_sphere_shape(phys_fixup_tag());
        c_transform_shape *transform = new (&sphere->transform) c_transform_shape(phys_fixup_tag(), 0, &sphere->sphere);
    }
}

const phys_shape *character_physics_definition_get_shape(const s_character_physics_definition *definition,
    s_character_physics_definition::e_shape_type shape_type)
{
    const phys_shape *shape = NULL;
    if (definition->pills.count)
        shape = &TAG_BLOCK_GET_ELEMENT(&definition->pills, shape_type, pill_definition)->pill;
    else if (!character_physics_definition_test_flag(definition, s_character_physics_definition::_use_spheres))
        shape = &TAG_BLOCK_GET_ELEMENT(&definition->spheres, shape_type, sphere_definition)->transform;
    else
        shape = &TAG_BLOCK_GET_ELEMENT(&definition->spheres, shape_type, sphere_definition)->sphere;
    return shape;
}
