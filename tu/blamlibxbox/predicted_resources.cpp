// blamlibxbox/predicted_resources.obj: from the records 0x21ee60 0x21eee0 0x21efe8 0x21f088 0x21f3b8 0x21f4a0 0x21f580 0x21f718 0x21f888 0x21f9c0 0x21fb88 0x21fcc0
// @flags /TP /O2 /Oc
#include "tag_files/tag_types.h"
#include "objects/object_header.h"
#include "objects/object_material_types.h"
#include "tag_files/tag_groups.h"
#include "game/game_globals.h"
#include "game/materials_definitions.h"
// @address 0x21efe8 predicted_resources_add_resources_from_render_geometry_parts
// @address 0x21f088 predicted_resources_add_resources_from_render_geometry_instances
#include "cseries/cseries.h"
#include <string.h>
#include "models/models.h"
#include "render/render_objects.h"
#include "cache/predicted_resources.h"
#include "scenario/scenario_geometry.h"
#include "objects/object_datum.h"
#include "cache/cache_file_globals.h"
#include "game/materials.h"
#include "cache/cache_file_allocation_range_types.h"
#include "effects/material_effects_definition_types.h"
#include "cache/render_model_region_types.h"
#include "effects/material_effect_search_types.h"
#include "objects/cluster_reference.h"
#include "effects/material_effect_types.h"
#include "effects/material_effects.h"
#include "geometry/render_geometry_types.h"
#include "scenario/scenario.h"
#include "main/main_render.h"
#include "models/render_model_definitions.h"
#include "structures/structure_bsp_tag.h"
#include "scenario/scenario_tag.h"
#include "scenario/scenario_structure_bsp_reference.h"

#include "cache/predicted_resource_visitor.h"

struct s_index_array {
    long maximum;
    short *elements;
    template<typename T, long N> __forceinline s_index_array(c_static_array<T, N> &array) : maximum(N), elements((short *)array.get_elements()) {}
};

// Render-method header followed by its material IDs.
#include "render_methods/render_method.h"
struct s_render_method_definition {
    c_render_method render_method;
    long prediction_atom_index;
    long material_ids[4];
};


#include "geometry/render_geometry_mesh_types.h"
#include "geometry/render_geometry_part_types.h"
#include "geometry/geometry_material_types.h"
#include "models/model_variant_types.h"
#include "models/model_types.h"

typedef char predicted_resources_material_offset[(offsetof(s_render_method_definition, material_ids) == 0x40) ? 1 : -1];

// Records each visited global material index as a bit (256 materials).
// @address 0x3e4a4 ??_7c_predicted_resources_index_visitor@@6B@
// @address 0x21f6e0 ?visit@c_predicted_resources_index_visitor@@UAAXF@Z
class c_predicted_resources_index_visitor
{
public:
    c_predicted_resources_index_visitor()
    {
        unsigned long *bit = bits;
        long remaining = 8;
        do {
            *bit = 0;
            bit++;
        } while (--remaining != 0);
    }
    virtual void visit(short index)
    {
        if (index != -1)
        {
            // match: 0x21f6e0: words keeps this+4 as the lwzx/stwx base; bits[] folds +4 into the index
            unsigned long *words = bits;
            // match: 0x21f6e0: long bit puts the mask in r8, the 1 in r9; using index swaps them
            long bit = index;
            words[bit >> 5] |= 1 << (bit & 31);
        }
    }
    unsigned long bits[8];
};



static __forceinline s_global_material_definition *material_effects_global_material(c_global_material_type material_type)
{
    s_global_material_definition *result = 0;
    if (material_type.m_index != -1)
    {
        if (material_type.m_index >= 0 && material_type.m_index < global_game_globals->materials.count())
            result = &((s_global_material_definition *)global_game_globals->materials.address)[material_type.m_index];
    }
    return result;
}

// @address 0x21ee60 predicted_resources_find_material_index
void predicted_resources_find_material_index(long material_id, s_index_array array, long *count)
{
    long n = *count;
    if (n >= array.maximum) return;
    short found = -1;
    s_game_globals *globals = global_game_globals;
    long total = globals->materials.count();
    long i = 0;
    if (total > 0) {
        s_global_material_definition *entry = (s_global_material_definition *)globals->materials.address;
        do {
            if (material_id == entry->material_id) {
                found = (short)i;
                break;
            }
            i++;
            entry++;
        } while (i < total);
    }
    if (found == -1) return;
    array.elements[n] = found;
    *count = n + 1;
}

// @address 0x21eee0 predicted_resources_get_shader_materials
long predicted_resources_get_shader_materials(long tag_index, s_index_array array)
{
    long count = 0;
    if (tag_index != -1) {
        unsigned long group_tag = tag_get_group_tag(tag_index);
        switch (group_tag) {
        case 'rmcs':
            predicted_resources_find_material_index(
                TAG_GET('rmcs', s_render_method_definition, tag_index)->material_ids[0], array, &count);
            break;
        case 'rmct':
            predicted_resources_find_material_index(
                TAG_GET('rmct', s_render_method_definition, tag_index)->material_ids[0], array, &count);
            break;
        case 'rmfl':
            predicted_resources_find_material_index(
                TAG_GET('rmfl', s_render_method_definition, tag_index)->material_ids[0], array, &count);
            break;
        case 'rmhg':
            predicted_resources_find_material_index(
                TAG_GET('rmhg', s_render_method_definition, tag_index)->material_ids[0], array, &count);
            break;
        case 'rmsh':
            predicted_resources_find_material_index(
                TAG_GET('rmsh', s_render_method_definition, tag_index)->material_ids[0], array, &count);
            break;
        case 'rmsk':
            predicted_resources_find_material_index(
                TAG_GET('rmsk', s_render_method_definition, tag_index)->material_ids[0], array, &count);
            break;
        case 'rmtr':
        {
            s_render_method_definition *definition = TAG_GET('rmtr', s_render_method_definition, tag_index);
            predicted_resources_find_material_index(definition->material_ids[0], array, &count);
            predicted_resources_find_material_index(definition->material_ids[1], array, &count);
            predicted_resources_find_material_index(definition->material_ids[2], array, &count);
            predicted_resources_find_material_index(definition->material_ids[3], array, &count);
            break;
        }
        }
    }
    return count;
}

void predicted_resources_add_resources_from_render_geometry_parts(c_visitor *visitor,
    const s_render_geometry *definition, s_tag_block *instance, long group_index)
{
    if (group_index != -1) {
        s_mesh *group = &((s_mesh *)definition->meshes.address)[group_index];
        for (int i = 0; i < group->parts.count; i++) {
            short element = ((s_render_part *)group->parts.address)[i].render_method_index;
            if (element != -1) {
                int value = ((geometry_material *)instance->address)[element].render_method.index;
                if (value != -1) {
                    visitor->visit(value);
                }
            }
        }
    }
}

void predicted_resources_add_resources_from_render_geometry_instances(c_visitor *visitor,
    const s_render_geometry *render_geometry, s_tag_block *tag_block, long geometry_mesh_index)
{
    if (geometry_mesh_index != -1) {
        const s_mesh *mesh = &((s_mesh *)render_geometry->meshes.address)[geometry_mesh_index];
        for (long bucket_index = 0; bucket_index < mesh->instance_buckets.count; bucket_index++) {
            predicted_resources_add_resources_from_render_geometry_parts(visitor, render_geometry, tag_block,
                TAG_BLOCK_GET_ELEMENT(&mesh->instance_buckets, bucket_index, s_mesh_instance_bucket)->mesh_index);
        }
    }
}


// @address 0x21f4a0 predicted_resources_iterate_bsp_permutation
extern "C" void predicted_resources_iterate_bsp_permutation(c_predicted_resources_index_visitor *visitor, long tag_index, s_cluster_reference key)
{
    signed char bsp_index = key.bsp_index;
    unsigned char cluster_index = key.cluster_index;
    struct scenario *scenario = TAG_GET('scnr', struct scenario, tag_index);
    scenario_structure_bsp_reference *reference = TAG_BLOCK_GET_ELEMENT(&scenario->structure_bsps, bsp_index, scenario_structure_bsp_reference);
    structure_bsp *bsp = TAG_GET('sbsp', structure_bsp, reference->structure_bsp.index);
    structure_cluster *cluster = TAG_BLOCK_GET_ELEMENT(&bsp->clusters, cluster_index, structure_cluster);
    const s_render_geometry *geometry = (const s_render_geometry *)scenario_structure_bsp_get_render_geometry(tag_index, bsp_index);
    void *auxiliary = &bsp->materials;
    predicted_resources_iterate_geometry_group(visitor, geometry, auxiliary, cluster->mesh_index);
    const s_mesh *mesh = TAG_BLOCK_GET_ELEMENT(&geometry->meshes, cluster->mesh_index, s_mesh);
    long bucket_index = 0;
    if (mesh->instance_buckets.count > 0) {
        long bucket_offset = 0;
        do {
            const s_mesh_instance_bucket *bucket =
                (const s_mesh_instance_bucket *)((char *)mesh->instance_buckets.address + bucket_offset);
            predicted_resources_iterate_geometry_group(visitor, geometry, auxiliary, bucket->mesh_index);
            bucket_index++;
            bucket_offset += sizeof(s_mesh_instance_bucket);
        } while (bucket_index < mesh->instance_buckets.count);
    }
}

// @address 0x21f580 predicted_resources_iterate_material_resources
void predicted_resources_iterate_material_resources(c_visitor *visitor, long unused, c_global_material_type material_type)
{
    if (material_type.m_index != -1)
    {
        s_global_material_definition *material = material_effects_global_material(material_type);
        for (long i = 0; i < 17; i++)
        {
            long sound = material->sounds[i].index;
            if (sound != -1)
            {
                visitor->visit(sound);
            }
        }

        tag_iterator iterator;
        iterator.next_index = 0;
        iterator.group_tag = 'foot';
        for (long tag_index = tag_iterator_next(&iterator); tag_index != -1; tag_index = tag_iterator_next(&iterator))
        {
            for (long effect_type = 0; effect_type < 23; effect_type++)
            {
                long resolved_effect_type = material_effect_resolve_effect_type(tag_index, _material_effect_search_second, effect_type);
                if (resolved_effect_type != -1)
                {
                    s_material_effects_definition *effects_def = TAG_GET('foot', s_material_effects_definition, tag_index);
                    s_material_effect *effect = TAG_BLOCK_GET_ELEMENT(&effects_def->material_effects, resolved_effect_type, s_material_effect);
                    s_material_effect_material *found_material = material_effect_find_material_internal(effect, _material_effect_search_second, material_type);
                    if (found_material)
                    {
                        visitor->visit(found_material->effect.index);
                        visitor->visit(found_material->second_effect.index);
                    }
                }
            }
        }
    }
}
// @address 0x21f718 ?predicted_resources_iterate_bsp_materials@@YAXPAUc_visitor@@JJUs_cluster_reference@@@Z
void predicted_resources_iterate_bsp_materials(c_visitor *visitor, long tag_index, long tag_datum, s_cluster_reference key)
{
    if (key.bsp_index != -1) {
        long handle = visitor->begin(tag_index, 0, (key.bsp_index << 8) | key.cluster_index);
        if (!visitor->has_handle(handle)) {
            visitor->enter(handle);
            c_predicted_resources_index_visitor visited;
            predicted_resources_iterate_bsp_permutation(&visited, tag_index, key);
            s_game_globals *definition = TAG_GET('matg', s_game_globals, tag_datum);
            for (long i = 0; i < 256; i++) {
                if ((visited.bits[i >> 5] & (1 << (i & 31))) != 0) {
                    short material = (short)i;
                    short clamped = -1;
                    if (material != -1) {
                        if (material >= 0 && material < definition->materials.count())
                            clamped = material;
                    }
                    else
                        clamped = -1;
                    c_global_material_type material_type;
                    material_type.m_index = clamped;
                    predicted_resources_iterate_material_resources(visitor, tag_datum, material_type);
                }
            }
            visitor->leave();
        }
    }
}
// @address 0x21f888 predicted_resources_iterate_model_permutation
void predicted_resources_iterate_model_permutation(c_visitor *visitor, long tag_index, long region_index, long permutation_index)
{
    if (permutation_index != -1)
    {
        long handle = visitor->begin(tag_index, region_index, permutation_index);
        if (!visitor->has_handle(handle))
        {
            visitor->enter(handle);
            if (tag_index != -1)
            {
                const s_object_material_definition *object_definition = TAG_GET('obje', const s_object_material_definition, tag_index);
                if (object_definition->model.index != -1)
                {
                    const s_model_definition *model_definition = TAG_GET('hlmt', const s_model_definition, object_definition->model.index);
                    if (model_definition->render_model_index != -1)
                    {
                        long section_index = 0;
                        render_model_definition *render_model = TAG_GET('mode', render_model_definition, model_definition->render_model_index);
                        const s_render_model_permutation *permutation =
                            &((s_render_model_region *)render_model->regions.address)[region_index].permutations[permutation_index];
                        if (permutation->count != 0)
                        {
                            do
                            {
                                predicted_resources_add_resources_from_render_geometry_parts(visitor, &render_model->render_geometry,
                                    &render_model->materials, permutation->start_index + section_index);
                                section_index++;
                            } while (section_index < permutation->count);
                        }
                    }
                }
            }
            visitor->leave();
        }
    }
}

// @address 0x21f3b8 predicted_resources_iterate_geometry_group
extern "C" void predicted_resources_iterate_geometry_group(c_predicted_resources_index_visitor *visitor,
    const void *geometry, const void *auxiliary, long group_index)
{
    const s_render_geometry *resources = (const s_render_geometry *)geometry;
    const s_tag_block *sections = (const s_tag_block *)auxiliary;
    const s_mesh *group = &((const s_mesh *)resources->meshes.address)[group_index];
    for (long instance_index = 0; instance_index < group->parts.count; ++instance_index)
    {
        const geometry_material *section =
            &((const geometry_material *)sections->address)[((s_render_part *)group->parts.address)[instance_index].render_method_index];
        if (section->render_method.index != -1)
        {
            c_static_array<c_global_material_type, 4> materials;
            s_index_array view(materials);
            const s_index_array *view_pointer = &view;
            long count = predicted_resources_get_shader_materials(section->render_method.index, *view_pointer);
            for (long index = 0; index < count; ++index)
                visitor->visit(view_pointer->elements[index]);
        }
    }
}

// @address 0x21f9c0 predicted_resources_iterate_model_variant
extern "C" void predicted_resources_iterate_model_variant(c_visitor *visitor, long tag_index, long variant_index)
{
    long handle = visitor->begin(tag_index, 16, variant_index);
    if (!visitor->has_handle(handle))
    {
        visitor->enter(handle);
        s_object_material_definition *object = TAG_GET('obje', s_object_material_definition, tag_index);
        if (object->model.index != -1)
        {
            s_model_definition *model = TAG_GET('hlmt', s_model_definition, object->model.index);
            if (variant_index != -1)
            {
                s_model_variant *variant = &((s_model_variant *)model->variants.address)[variant_index];
                if (variant->instance_group_index != -1)
                {
                    if (model->render_model_index != -1)
                    {
                        render_model_definition *render_model = TAG_GET('mode', render_model_definition, model->render_model_index);
                        if (render_model && render_model->instance_starting_mesh_index != -1)
                        {
                            c_static_flags_no_init<64> instances;
                            instances.clear();
                            model_instance_group_calculate_all_possible_instances(object->model.index, variant->instance_group_index,
                                instances.get_writeable_bits_direct());
                            if (render_model->instance_starting_mesh_index != -1)
                            {
                                const s_mesh *group = &((s_mesh *)render_model->render_geometry.meshes.address)[render_model->instance_starting_mesh_index];
                                for (long instance_index = 0; instance_index < group->parts.count; ++instance_index)
                                {
                                    if (instances.test(instance_index))
                                    {
                                        short section_index = ((s_render_part *)group->parts.address)[instance_index].render_method_index;
                                        long shader_index = ((geometry_material *)render_model->materials.address)[section_index].render_method.index;
                                        if (shader_index != -1)
                                            visitor->visit(shader_index);
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
        visitor->leave();
    }
}


// @address 0x21fb88 predicted_resources_iterate_object_instance
extern "C" void predicted_resources_iterate_object_instance(c_visitor *visitor, long object_index)
{
    if (object_index == -1) return;
    object_datum *object = (object_datum *)object_get_and_verify_type(object_index, NONE);
    long tag_index = object->definition_index;
    if (tag_index == -1) return;
    long permutation_table_size = object->block_size;
    char *permutation_table = (char *)object + object->block_offset;
    if ((bool)((object->flags >> 11) & 1))
        permutation_table_size -= 16;
    long region_count = permutation_table_size / 10;
    signed char *permutation_indices = (signed char *)(permutation_table + region_count * 8);
    for (long region_index = 0; region_index < region_count; region_index++)
        predicted_resources_iterate_model_permutation(visitor, tag_index, region_index, permutation_indices[region_index]);

    void *address;
    if (!object_get_render_model_instance_bitfield_visible(object_index, &address)) return;
    const long *word = (const long *)address;
    bool found = false;
    unsigned long word_index = 0;
    do
    {
        if (word_index >= 2) break;
        found = *word != 0;
        word++;
        word_index++;
    } while (!found);
    if (found)
        predicted_resources_iterate_model_variant(visitor, tag_index, object->variant_index);
}

// @address 0x21fcc0 predicted_resources_iterate_object_definition
extern "C" void predicted_resources_iterate_object_definition(c_visitor *visitor, long object_index)
{
    if (object_index == -1) return;
    s_object_material_definition *object = TAG_GET('obje', s_object_material_definition, object_index);
    if (object->model.index != -1)
    {
        s_model_definition *model = TAG_GET('hlmt', s_model_definition, object->model.index);
        for (long region_index = 0; region_index < model->regions.count; region_index++)
        {
            const s_model_region *region = &((s_model_region *)model->regions.address)[region_index];
            for (long permutation_index = 0; permutation_index < region->permutations.count; permutation_index++)
                predicted_resources_iterate_model_permutation(visitor, object_index, region_index, permutation_index);
        }
        for (long variant_index = 0; variant_index < model->variants.count; variant_index++)
            predicted_resources_iterate_model_variant(visitor, object_index, variant_index);
    }
    main_render_predict_tag_resources(visitor, object_index);
}
// @address 0x7cf604 ?global_game_globals@@3PAUs_game_globals@@A
// @address 0x945f50 ?g_cache_file_globals@@3Us_cache_file_globals@@A
// @address 0x7cff0c ?global_tag_instances@@3PBUcache_file_tag_instance@@B
// @address 0x25e2d8 ?material_effect_find_material_internal@@YAPAUs_material_effect_material@@PBUs_material_effect@@W4e_material_effect_search@@Vc_global_material_type@@@Z
// @address 0x263118 ?model_instance_group_calculate_all_possible_instances@@YAXJJPAK@Z
// @address 0x1a1b68 ?tag_iterator_next@@YAJPAUtag_iterator@@@Z
