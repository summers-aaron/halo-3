// blamlibxbox/materials.obj: from the records 0xf4290 0x25e100 0x25e1f8
// @flags /TP /O2 /Oc

#include "objects/object_header.h"
#include "tag_files/tag_groups.h"
#include "physics/water_physics.h"
#include "physics/water_physics_types.h"
#include "game/materials.h"
#include "game/materials_definitions.h"
#include "objects/object_material_types.h"
#include "models/model_types.h"
#include "models/model_damage_info.h"
#include "scenario/scenario.h"

// @address 0x7832dc ?k_global_material_type_invalid@@3Vc_global_material_type@@B
const c_global_material_type k_global_material_type_invalid = c_global_material_type::_invalid;

// @address 0xa430d4 ?k_global_material_type_default@@3Vc_global_material_type@@B
const c_global_material_type k_global_material_type_default = c_global_material_type::_default;

static __forceinline bool global_material_is_type(c_global_material_type material, c_global_material_type type)
{
    bool result = false;
    for (;;) {
        s_global_material_definition *definition = material.get_material(global_game_globals);
        if (material == type) {
            result = true;
            break;
        } else if (definition == 0) {
            break;
        } else {
            material = definition->parent_material_type;
            continue;
        }
    }
    return result;
}

// @address 0x25e100 ?global_material_resolve_underwater_material_proxy@@YA?AVc_global_material_type@@V1@0@Z
c_global_material_type global_material_resolve_underwater_material_proxy(c_global_material_type material, c_global_material_type underwater_material)
{
    s_global_material_definition *definition = material.get_material(global_game_globals);
    c_global_material_type result = material;
    if (definition) {
        for (long proxy_index = 0; proxy_index < definition->underwater_proxies.count; ++proxy_index) {
            s_underwater_material_proxy *proxy = TAG_BLOCK_GET_ELEMENT(&definition->underwater_proxies, proxy_index, s_underwater_material_proxy);
            if (global_material_is_type(underwater_material, proxy->underwater_material_type)) {
                result = proxy->proxy_material_type;
                break;
            } else {
                continue;
            }
        }
    }
    return result;
}

static __forceinline bool materials_water_physics_test_point(const real_point3d *point, c_global_material_type *material_type)
{
    s_water_physics_test_sphere_result result;
    result.material_type.m_index = -1;
    bool found = water_physics_test_sphere(false, point, 0.0f, &result);
    *material_type = result.material_type;
    return found;
}

// @address 0xf4290 ?global_material_resolve_underwater_material_proxy@@YA?AVc_global_material_type@@V1@PBTreal_point3d@@@Z
c_global_material_type global_material_resolve_underwater_material_proxy(c_global_material_type material, const real_point3d *position)
{
    c_global_material_type result = material;
    c_global_material_type underwater_material;
    if (materials_water_physics_test_point(position, &underwater_material))
        result = global_material_resolve_underwater_material_proxy(material, underwater_material);
    return result;
}

// @address 0x25e1f8 ?object_get_default_material@@YA?AVc_global_material_type@@J@Z
c_global_material_type object_get_default_material(long object_index)
{
    c_global_material_type material = k_global_material_type_invalid;
    object_datum *object = OBJECT_GET(object_index, object_datum, _object_mask_all);
    s_object_material_definition *definition = TAG_GET('obje', s_object_material_definition, object->definition_index);
    if (object_try_and_get_and_verify_type(object_index, _object_mask_biped)) {
        s_object_material_definition *biped = TAG_GET('bipd', s_object_material_definition, object->definition_index);
        bool damaged = (object->damage_flags >> 2) & 1;
        if (damaged)
            material = biped->dead_material_type;
        else
            material = biped->default_material_type;
    }
    if (material == k_global_material_type_invalid && definition->model.index != -1) {
        s_model_definition *model = TAG_GET('hlmt', s_model_definition, definition->model.index);
        if (model->damage_info.count != 0) {
            s_model_damage_info *damage_info = TAG_BLOCK_GET_ELEMENT(&model->damage_info, 0, s_model_damage_info);
            material = damage_info->indirect_material_type;
        }
    }
    return material;
}
