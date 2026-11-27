// blamlibxbox/collision_models.obj: from the records 0x9f668 0x9f7f8 0x9f8f0 0x9f9d0 0x2fa6a0
// @flags /TP /O2 /Oc
#include "tag_files/tag_groups.h"
#include "objects/object_header.h"
#include "cseries/cseries_wrapped.h"
#include "objects/object_reference_types.h"
#include "math/matrix_math_types.h"
#include "math/real_math_types.h"
#include "tag_files/tag_types.h"
#include "objects/object_datum.h"
// @address 0x9f668 ?collision_model_instance_new@@YA_NPAUcollision_model_instance@@J@Z
// @address 0x9f8f0 ?collision_model_bsp_iterator_new@@YAXPAUcollision_model_bsp_iterator@@PBUs_model_definition@@PBUcollision_model_definition@@PBD@Z
// @address 0x9f9d0 ?collision_model_bsp_iterator_next@@YA_NPAUcollision_model_bsp_iterator@@@Z
// @address 0x9fb10 ?matrix4x3_inverse_transform_point@@YAPATreal_point3d@@PBUreal_matrix4x3@@PBT1@PAT1@@Z
// @address 0x9fbf0 ?matrix4x3_inverse_transform_vector@@YAPATreal_vector3d@@PBUreal_matrix4x3@@PBT1@PAT1@@Z
// @address 0x9e5d0 ?collision_bsp_test_vector@@YA_NKPBUcollision_bsp@@PBUs_collision_materials_extant_flags@@FPBUs_breakable_surface_set@@PBTreal_point3d@@PBTreal_vector3d@@MPAUcollision_bsp_test_vector_result@@@Z
// @address 0xf25a0 ?matrix4x3_inverse@@YAXPBUreal_matrix4x3@@PAU1@@Z
// @address 0x2b24c8 ?collision_bsp_test_sphere@@YA_NPBUcollision_bsp@@V?$c_flags@W4e_collision_bsp_test_sphere_flag@@K$02@@JPBUs_breakable_surface_set@@PBTreal_point3d@@MPAUcollision_bsp_test_sphere_result@@@Z
#include <stddef.h>
#include <string.h>
#include "physics/collision_features.h"
#include "physics/collision_models.h"
#include "math/matrix_math.h"
#include "physics/collision_model_bsp_iterator_types.h"
#include "physics/collision_model_bsp_iterator_internals.h"
#include "physics/collision_bsp_types.h"
#include "physics/collision_feature_types.h"
#include "models/model_types.h"
#include "physics/collision_bsp_query_types.h"
#include "physics/collision_bsp.h"
#include "objects/object_definitions.h"
#include "physics/collision_model_types.h"
#include "physics/collision_model_definition_types.h"

struct collision_model_instance;
struct collision_model_bsp_iterator;
struct s_breakable_surface_set;

static __forceinline void collision_models_get_region_information(
    long object_index, long *region_count, char **permutations, char **extra_out, object_region_information **info)
{
    object_datum *object = object_get(object_index);
    long size = object->regions.size;
    char *base = (char *)object + object->regions.offset;
    if ((bool)((object->flags >> 11) & 1))
        size -= 16;
    *region_count = size / 10;
    if (permutations)
        *permutations = (char *)((object_region_information *)base + *region_count);
}
static __forceinline real_matrix4x3 *collision_models_get_node_matrices(long object_index, long *count)
{
    object_datum *object = object_get(object_index);
    *count = object->node_matrices.size / (long)sizeof(real_matrix4x3);
    return (real_matrix4x3 *)((char *)object + object->node_matrices.offset);
}

// match: 0x9f668: __restrict on instance issues the collision_model store and node_matrices load ahead of the region_count divide
bool collision_model_instance_new(struct collision_model_instance * __restrict instance, long object_index)
{
    object_datum *object = object_get(object_index);
    s_object_definition *definition = (s_object_definition *)tag_get('obje', object->definition_index);
    bool result = false;
    if (definition->model.index != -1)
    {
        s_model_definition *model = (s_model_definition *)tag_get('hlmt', definition->model.index);
        if (model->collision_model_index != -1)
        {
            collision_model_definition *collision_model = (collision_model_definition *)tag_get('coll', model->collision_model_index);
            long region_count;
            char *permutations;
            long node_count;
            collision_models_get_region_information(object_index, &region_count, &permutations, 0, 0);
            real_matrix4x3 *node_matrices = collision_models_get_node_matrices(object_index, &node_count);
            instance->object_index = object_index;
            instance->model = model;
            instance->collision_model = collision_model;
            instance->node_matrices = node_matrices;
            instance->region_permutations = permutations;
            result = true;
        }
    }
    return result;
}

// @address 0x9f7f8 ?collision_model_test_vector@@YA_NPBUcollision_model_instance@@KPBTreal_point3d@@PBTreal_vector3d@@PAUcollision_model_test_vector_result@@@Z
// match: 0x9f7f8: pointer __restrict qualifier prevents alias reload
bool collision_model_test_vector(struct collision_model_instance const *__restrict instance, unsigned long p1,
    union real_point3d const *point, union real_vector3d const *vector, struct collision_model_test_vector_result *result)
{
    bool hit = false;
    collision_model_bsp_iterator iterator;
    result->bsp_result.fraction = 3.4028234663852886e+38f;
    collision_model_bsp_iterator_new(&iterator, instance->model, instance->collision_model, instance->region_permutations);
    if (collision_model_bsp_iterator_next(&iterator)) {
        do {
            unsigned long reference = iterator.index;
            unsigned long node_index = reference & 255;
            real_point3d local_point;
            real_vector3d local_vector;
            matrix4x3_inverse_transform_point(&instance->node_matrices[node_index], point, &local_point);
            matrix4x3_inverse_transform_vector(&instance->node_matrices[node_index], vector, &local_vector);
            if (collision_bsp_test_vector(p1, (const collision_bsp *)iterator.bsp, 0, 0, 0, &local_point, &local_vector,
                    result->bsp_result.fraction, &result->bsp_result))
            {
                result->bsp_reference = reference;
                hit = true;
                result->node_index = node_index;
                result->region_index = (short)(reference >> 24);
            }
        } while (collision_model_bsp_iterator_next(&iterator));
    }
    return hit;
}

// match: 0x9f8f0: integer region cursor induction preserves add lowering.
void collision_model_bsp_iterator_new(collision_model_bsp_iterator *iterator, const s_model_definition *model,
    const collision_model_definition *collision_model, const char *region_permutations)
{
    ptrdiff_t iterator_address = (ptrdiff_t)iterator;
    ptrdiff_t model_address = (ptrdiff_t)model;
    size_t region_permutation_address = (size_t)region_permutations;
    iterator->model = (collision_model_definition *)collision_model;
    iterator->index = -1;
    if (region_permutation_address == 0) {
        memset(iterator->permutations, 0, sizeof(iterator->permutations));
        return;
    } else if ((size_t)model_address == 0) {
        memcpy(iterator->permutations, region_permutations, sizeof(iterator->permutations));
        return;
    } else {
        int region_value;
        region_value = ((const s_model_definition *)model_address)->regions.count;
        int region_count;
        region_count = 16;
        if (region_value <= 16) {
            region_count = region_value;
        }
        int region_index;
        region_index = 0;
        memset(iterator->permutations, -1, sizeof(iterator->permutations));
        if (region_count > 0) {
            ptrdiff_t region_cursor =
                (ptrdiff_t)((const s_model_definition *)model_address)->regions.address +
                (ptrdiff_t)offsetof(s_model_region, collision_region_index);
            do {
                const s_collision_model_region_cursor *cursor =
                    (const s_collision_model_region_cursor *)region_cursor;
                const signed char &collision_region_index = cursor->collision_region_index;
                int permutation_index;
                permutation_index =
                    (signed char)*(const unsigned char *)((size_t)region_index + region_permutation_address);
                if (collision_region_index != -1 && permutation_index >= 0 && permutation_index < cursor->permutation_count) {
                    const s_model_region_permutation *permutation_entry =
                        (const s_model_region_permutation *)((size_t)(permutation_index << 3) + (size_t)cursor->permutations);
                    unsigned char collision_permutation = (unsigned char)permutation_entry->collision_permutation_index;
                    if (collision_permutation != 255) {
                        ((collision_model_bsp_iterator *)((ptrdiff_t)collision_region_index + iterator_address))->permutations[0] =
                            (signed char)collision_permutation;
                    }
                }
                region_index = region_index + 1;
                region_cursor = region_cursor + (ptrdiff_t)sizeof(s_model_region);
            } while (region_index < region_count);
            return;
        } else {
            return;
        }
    }
}

bool collision_model_bsp_iterator_next(collision_model_bsp_iterator *iterator) {
 long region_index,permutation_index,bsp_index;
 s_collision_model_region *r;s_collision_model_permutation *p;
 if(iterator->index==-1) {region_index=-1;permutation_index=-1;bsp_index=-1;r=0;p=0;}
 else {
 region_index=(iterator->index>>24)&255;permutation_index=(iterator->index>>16)&255;bsp_index=(iterator->index>>8)&255;
 r=(s_collision_model_region *)tag_block_get_element_with_size(&iterator->model->regions,region_index,sizeof(s_collision_model_region));
 p=(s_collision_model_permutation *)tag_block_get_element_with_size(&r->permutations,permutation_index,sizeof(s_collision_model_permutation));
 }
 if(p && bsp_index+1<p->bsps.count) bsp_index++;
 else {
 while(true) {
 region_index=region_index==-1?0:region_index+1;
 if(region_index<iterator->model->regions.count) {
 r=(s_collision_model_region *)tag_block_get_element_with_size(&iterator->model->regions,region_index,sizeof(s_collision_model_region));
 permutation_index=iterator->permutations[region_index];
 if(permutation_index>=0 && permutation_index<r->permutations.count) {
 p=(s_collision_model_permutation *)tag_block_get_element_with_size(&r->permutations,permutation_index,sizeof(s_collision_model_permutation));
 if(p->bsps.count>0) {bsp_index=0;break;}
 }
 } else {region_index=-1;break;}
 }
 }
 if(region_index==-1) {iterator->index=-1;iterator->bsp=0;}
 else {
 s_collision_model_bsp *b=(s_collision_model_bsp *)tag_block_get_element_with_size(&p->bsps,bsp_index,sizeof(s_collision_model_bsp));
 iterator->index=((region_index&255)<<24)|((permutation_index&255)<<16)|((bsp_index&255)<<8)|(b->node_index&255);
 iterator->bsp=&b->bsp;
 }
 return iterator->bsp!=0;
}



static __forceinline real_point3d *collision_models_transform_point_by_matrix(const real_matrix4x3 *m, const real_point3d *p, real_point3d *result)
{
    float x = p->x * m->scale;
    float y = p->y * m->scale;
    float z = p->z * m->scale;
    float rx = x * m->forward.i + (y * m->left.i + z * m->up.i);
    float ry = x * m->forward.j + (z * m->up.j + y * m->left.j);
    float rz = x * m->forward.k + (z * m->up.k + y * m->left.k);
    result->x = rx + m->position.x;
    result->y = ry + m->position.y;
    result->z = rz + m->position.z;
    return result;
}

// @address 0x2fa6a0 ?collision_model_get_features_in_sphere@@YA_NPBUcollision_model_instance@@PBTreal_point3d@@MMMPAUcollision_feature_list@@@Z
bool collision_model_get_features_in_sphere(const collision_model_instance *instance, const real_point3d *point, float radius,
    float feature_radius, float height, collision_feature_list *features)
{
    // match: 0x2fa6a0: value range assumption guides branch lowering
    __assume(instance != 0);
    bool result = false;
    collision_model_bsp_iterator iterator;
    collision_model_bsp_iterator_new(&iterator, instance->model, instance->collision_model, instance->region_permutations);
    while (collision_model_bsp_iterator_next(&iterator))
    {
        unsigned long reference = iterator.index;
        const real_matrix4x3 *matrix = &instance->node_matrices[reference & 255];
        real_matrix4x3 inverse;
        real_point3d local_point;
        collision_bsp_test_sphere_result sphere_result;
        // match: 0x2fa6a0: the named flags local preserves transform instruction selection.
        c_collision_bsp_test_sphere_flags flags(6);
        matrix4x3_inverse(matrix, &inverse);
        collision_models_transform_point_by_matrix(&inverse, point, &local_point);
        if (collision_bsp_test_sphere((const collision_bsp *)iterator.bsp, flags, 0, 0, &local_point, inverse.scale * radius, &sphere_result))
        {
            ::collision_bsp_get_features_in_sphere((const collision_bsp *)iterator.bsp,
                &sphere_result, matrix, feature_radius, height,
                instance->object_index, reference, -1, -1, features);
            result = true;
        }
    }
    return result;
}
