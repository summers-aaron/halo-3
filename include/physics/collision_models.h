#ifndef PHYSICS_COLLISION_MODELS_H
#define PHYSICS_COLLISION_MODELS_H

#include "math/matrix_math_functions.h"
#include "physics/bsp3d.h"
#include "physics/collision_bsp_query_types.h"
#include "physics/collision_model_bsp_iterator_types.h"
#include "physics/collision_model_types.h"

// Functions defined in tu/blamlibxbox/collision_models.cpp that other units call.

struct collision_feature_list;

void collision_model_bsp_iterator_new(collision_model_bsp_iterator *iterator, const s_model_definition *model,
    const collision_model_definition *collision_model, const char *region_permutations);
bool collision_model_bsp_iterator_next(collision_model_bsp_iterator *iterator);
bool collision_model_get_features_in_sphere(const collision_model_instance *instance, const real_point3d *point, float radius,
    float feature_radius, float height, collision_feature_list *features);
bool collision_model_instance_new(struct collision_model_instance * instance, long object_index);

union real_vector3d;

struct collision_model_test_vector_result
{
    unsigned long bsp_reference;
    short node_index;
    short region_index;
    collision_bsp_test_vector_result bsp_result;
};

#if !defined(_WIN64) && !defined(__LP64__)
typedef char collision_model_vector_result_size[(sizeof(collision_model_test_vector_result) == 0x428) ? 1 : -1];
#endif
typedef char collision_model_vector_node_offset[(offsetof(collision_model_test_vector_result, node_index) == 4) ? 1 : -1];
typedef char collision_model_vector_region_offset[(offsetof(collision_model_test_vector_result, region_index) == 6) ? 1 : -1];
typedef char collision_model_vector_bsp_offset[(offsetof(collision_model_test_vector_result, bsp_result) == 8) ? 1 : -1];

__forceinline bool collision_model_test_point(const collision_model_instance *instance, const real_point3d *point)
{
    collision_model_bsp_iterator iterator;
    collision_model_bsp_iterator_new(&iterator, instance->model, instance->collision_model, instance->region_permutations);
    if (collision_model_bsp_iterator_next(&iterator)) {
        const real_matrix4x3 *node_matrices = instance->node_matrices;
        do {
            unsigned long node_index = (unsigned char)iterator.index;
            real_point3d local_point;
            matrix4x3_inverse_transform_point(&node_matrices[node_index], point, &local_point);
            if (bsp3d_test_point((const bsp3d *)iterator.bsp, 0, &local_point) == -1)
                return true;
        } while (collision_model_bsp_iterator_next(&iterator));
    }
    return false;
}

bool collision_model_test_vector(const collision_model_instance *instance, unsigned long flags, const real_point3d *point,
    const real_vector3d *vector, collision_model_test_vector_result *result);

#endif // PHYSICS_COLLISION_MODELS_H
