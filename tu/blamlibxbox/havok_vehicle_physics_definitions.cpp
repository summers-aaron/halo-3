// blamlibxbox/havok_vehicle_physics_definitions.obj: from the records 0xdc560 0x2de828
// @flags /TP /O2 /Oc
#include "math/real_math_types.h"
#include <stddef.h>
#include "tag_files/tag_types.h"
#include "tag_files/tag_groups.h"
#include "physics/havok_vehicle_physics_definitions.h"
// @address 0x2de828 ?havok_vehicle_physics_definition_fixup@@YAXPAUs_havok_vehicle_physics_definition@@@Z
// @address 0x6203c8 ??0phys_list_shape@@QAA@Vphys_fixup_tag@@@Z
// @address 0x61fe00 ?recompute_bounds_extents@phys_list_shape@@QAAXXZ


#include "physics/physics_model_shape_internal.h"
#include "physics/havok_vehicle_physics.h"
#ifdef HALO3_64BIT
#include "physics/havok_tag_unpack_64.h"

static void unpack_havok_vehicle_physics_shapes_64(s_tag_block *block)
{
    if (block->count <= 0 || tag_block_is_unpacked_64(block))
        return;

    s_havok_vehicle_physics_shapes *dst = (s_havok_vehicle_physics_shapes *)havok_unpack_alloc_64(
        (size_t)block->count * sizeof(s_havok_vehicle_physics_shapes));
    const char *src_base = (const char *)block->address;
    for (long i = 0; i < block->count; i++)
    {
        const char *src = src_base + (size_t)i * 0x330;
        dst[i].list_shape.m_edge_weld_disabled = (*(const char *)(src + 0x14)) != 0;
        memcpy(&dst[i].list_shape.m_child_info.m_size, src + 0x1c, sizeof(int));
        memcpy(&dst[i].list_shape.m_child_info.m_capacity_and_flags, src + 0x20, sizeof(int));
        memcpy(&dst[i].list_shape.m_aabb_half_extents, src + 0x30, sizeof(phys_vec4));
        memcpy(&dst[i].list_shape.m_aabb_center, src + 0x40, sizeof(phys_vec4));
        memcpy(&dst[i].shape_count, src + 0x50, sizeof(long));
        memcpy(&dst[i].flags, src + 0x54, sizeof(long));
        memcpy(&dst[i].phantom_aabb, src + 0x58, sizeof(real_rectangle3d));

        for (long s = 0; s < 4; s++)
        {
            const char *sphere_src = src + 0x70 + (size_t)s * 0xa0;
            memcpy(&dst[i].spheres[s].m_sphere_count, sphere_src + 0x10, sizeof(int));
            memcpy(dst[i].spheres[s].m_sphere_array, sphere_src + 0x20,
                sizeof(phys_vec4) * phys_multi_sphere_shape::MULTI_SPHERE_MAX);
        }

        for (long c = 0; c < 4; c++)
        {
            const char *child_src = src + 0x2f0 + (size_t)c * 0x10;
            memcpy(&dst[i].child_infos[c].m_collision_filter_info, child_src + 0x04, sizeof(unsigned int));
            memcpy(&dst[i].child_infos[c].m_shape_size, child_src + 0x08, sizeof(int));
            memcpy(&dst[i].child_infos[c].m_child_shape_count, child_src + 0x0c, sizeof(int));
        }
    }
    block->address = dst;
    tag_block_mark_unpacked_64(block);
}
#endif

//  0xdc560 ?havok_vehicle_physics_get_phantom_aabb@@@@
bool havok_vehicle_physics_get_phantom_aabb(const s_havok_vehicle_physics_definition *definition, real_rectangle3d *aabb)
{
    bool result = false;
    if (definition->shapes.count != 0) {
        s_havok_vehicle_physics_shapes *shapes = (s_havok_vehicle_physics_shapes *)tag_block_get_element_with_size(
            &definition->shapes, 0, sizeof(s_havok_vehicle_physics_shapes));
        if ((shapes->flags & 1) != 0) {
            *aabb = shapes->phantom_aabb;
            result = true;
        }
    }
    return result;
}

void havok_vehicle_physics_definition_fixup(s_havok_vehicle_physics_definition *definition)
{
    if (definition->shapes.count != 0)
    {
#ifdef HALO3_64BIT
        unpack_havok_vehicle_physics_shapes_64(&definition->shapes);
#endif
        s_havok_vehicle_physics_shapes *shapes = (s_havok_vehicle_physics_shapes *)tag_block_get_element_with_size(
            &definition->shapes, 0, sizeof(s_havok_vehicle_physics_shapes));

        for (long shape_index = 0; shape_index < shapes->shape_count; shape_index++)
        {
            c_multi_sphere_shape *multi_sphere_shape = new (&shapes->spheres[shape_index]) c_multi_sphere_shape(phys_fixup_tag());
        }

        if (shapes->shape_count > 0)
        {
            phys_list_shape::child_shape_entry *child_infos = shapes->child_infos;

            for (long shape_index = 0; shape_index < shapes->shape_count; shape_index++)
            {
                child_infos[shape_index].m_proxy_shape = &shapes->spheres[shape_index];
                child_infos[shape_index].m_collision_filter_info = 0;
                const_cast<phys_shape *>(child_infos[shape_index].m_proxy_shape)->set_user_data(shape_index + 1);
            }

            c_list_shape *list_shape = new (&shapes->list_shape) c_list_shape(phys_fixup_tag());
            shapes->list_shape.m_child_info.m_data = shapes->child_infos;
            shapes->list_shape.recompute_bounds_extents();
        }
    }
}
