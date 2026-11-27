// blamlibxbox/walker_physics.obj: from the records 0x579af8 0x579d18 0x579db8 0x57a408 0x57a590
// @flags /TP /O2 /Oc
#include "cseries/cseries.h"
#include "physics/physics_model_types.h"
#include "tag_files/tag_types.h"
#include "math/real_math.h"
#include "objects/object_datum.h"
#include "objects/object_header.h"
#include "tag_files/tag_groups.h"
#include "math/matrix_math.h"
#include <math.h>
#include <stddef.h>
#include "physics/physics_models.h"
#include "physics/physics_model_definitions.h"
#include "physics/walker_physics.h"
#include "physics/havok_component.h"
#include "physics/physics_model_instance.h"
#include "physics/runtime_node_to_havok_component_rigid_body_index_mapping.h"
#include "physics/walker_leg_definitions.h"
#include "physics/walker_physics_definitions.h"
#include "physics/havok_contact_point_iterator.h"
#include "physics/physics_model_constraint_reference.h"
#include "objects/object_definitions.h"
#include "models/model_variant_types.h"
#include "models/model_types.h"
#include "physics/havok_component_class.h"
#include "memory/data.h"
#include "objects/object_node_matrices_inline.h"
#include "math/real_math_trigonometry.h"
#include "math/point_math.h"
// @address 0x579d18 ?calculate_law_off_cosines_angle@@YAMMMM@Z
// @address 0x57a408?change_leg_group_stuck_state@c_walker_physics@@SAJPBUs_walker_physics_definition@@JJ_N@Z

float calculate_law_off_cosines_angle(float a, float b, float c)
{
    float denominator = 2.0f * b * c;
    float numerator = b * b + c * c - a * a;
    float cosine = fabs(denominator - 0.0f) < 0.0001f ? 0.0f : numerator / denominator;
    float clamped = PIN(cosine, -1.0f, 1.0f);
    return (float)acos(PIN(clamped, -1.0f, 1.0f));
}

long c_walker_physics::change_leg_group_stuck_state(const s_walker_physics_definition *definition, long object_index, long group, bool stuck)
{
    long result = 0;
    s_physics_model_instance instance;
    if (physics_model_instance_new(&instance, object_index)) {
        c_runtime_node_to_havok_component_rigid_body_index_mapping mapping(object_index);
        for (long leg_index = 0; leg_index < definition->leg_count; leg_index++) {
            const s_walker_leg_definition *leg = &definition->legs[leg_index];
            if (leg->enabled && leg->group == group && leg->node != -1) {
                long body_index = mapping.get_rigid_body_index(leg->node);
                if (body_index != -1) {
                    c_havok_contact_point_iterator iterator(object_index, false);
                    for (; iterator.valid(); iterator.next()) {
                        if (iterator.get_contact_point()->m_rigid_body_index == body_index)
                            result |= 1 << leg_index;
                    }
                }
            }
        }
    }
    return result;
}

// @address 0x57a590 ?calculate_foot_group_center_matrix@c_walker_physics@@SA_NPBUs_walker_physics_definition@@JJPAUreal_matrix4x3@@@Z
bool c_walker_physics::calculate_foot_group_center_matrix(const s_walker_physics_definition *definition, long object_index, long group, real_matrix4x3 *matrix)
{
    long front_min_height = -1;
    long front_min_leg = -1;
    long front_max_height = -1;
    long front_max_leg = -1;
    long rear_min_height = -1;
    long rear_min_leg = -1;
    long rear_max_height = -1;
    long rear_max_leg = -1;
    bool result = false;
    for (long leg_index = 0; leg_index < definition->leg_count; leg_index++)
    {
        const s_walker_leg_definition *leg = &definition->legs[leg_index];
        if (leg->enabled && leg->group == group)
        {
            if (leg->leg_side == 0)
            {
                if (front_min_leg == -1 || leg->height < front_min_height)
                {
                    front_min_height = leg->height;
                    front_min_leg = leg_index;
                }
                if (front_max_leg == -1 || leg->height > front_max_height)
                {
                    front_max_height = leg->height;
                    front_max_leg = leg_index;
                }
            }
            else
            {
                if (rear_min_leg == -1 || leg->height < rear_min_height)
                {
                    rear_min_height = leg->height;
                    rear_min_leg = leg_index;
                }
                if (rear_max_leg == -1 || leg->height > rear_max_height)
                {
                    rear_max_height = leg->height;
                    rear_max_leg = leg_index;
                }
            }
        }
    }
    if (front_min_leg != -1 && rear_min_leg != -1 && front_max_leg != -1 && rear_max_leg != -1)
    {
        real_matrix4x3 front_min_foot;
        real_matrix4x3 front_max_foot;
        real_matrix4x3 rear_min_foot;
        real_matrix4x3 rear_max_foot;
        walker_physics_calculate_foot_matrix(definition, object_index, front_min_leg, false, &front_min_foot);
        walker_physics_calculate_foot_matrix(definition, object_index, front_max_leg, false, &front_max_foot);
        walker_physics_calculate_foot_matrix(definition, object_index, rear_min_leg, false, &rear_min_foot);
        walker_physics_calculate_foot_matrix(definition, object_index, rear_max_leg, false, &rear_max_foot);
        real_point3d front_center, rear_center;
        add_points3d(&front_min_foot.position, &front_max_foot.position, &front_center);
        add_points3d(&rear_min_foot.position, &rear_max_foot.position, &rear_center);
        scale_point3d(&rear_center, 0.5f, &rear_center);
        scale_point3d(&front_center, 0.5f, &front_center);
        if (front_min_height != front_max_height)
        {
            vector_from_points3d(&front_max_foot.position, &front_min_foot.position, &matrix->forward);
            result = true;
        }
        if (rear_min_height != rear_max_height)
        {
            vector_from_points3d(&rear_max_foot.position, &rear_min_foot.position, &matrix->forward);
            result = true;
        }
        if (result)
        {
            vector_from_points3d(&rear_center, &front_center, &matrix->left);
            normalize3d_with_default(&matrix->forward, global_forward3d);
            normalize3d_with_default(&matrix->left, global_left3d);
            real_vector3d projection;
            scale_vector3d(&matrix->forward, dot_product3d(&matrix->left, &matrix->forward), &projection);
            subtract_vectors3d(&matrix->left, &projection, &matrix->left);
            normalize3d_with_default(&matrix->left, global_left3d);
            matrix->scale = 1.0f;
            cross_product3d(&matrix->forward, &matrix->left, &matrix->up);
            add_points3d(&front_center, &rear_center, &matrix->position);
            scale_point3d(&matrix->position, 0.5f, &matrix->position);
        }
    }
    return result;
}

// @address 0x579db8 ?keyframe_leg@c_walker_physics@@CAXPBUs_walker_physics_definition@@JJ_NPBUreal_matrix4x3@@2PBTreal_vector3d@@M@Z
void c_walker_physics::keyframe_leg(const s_walker_physics_definition *definition, long object_index, long leg_index, bool unused,
    const real_matrix4x3 *matrix_a, const real_matrix4x3 *matrix_b, const real_vector3d *vector, float angle)
{
    const s_walker_leg_definition *leg = &definition->legs[leg_index];
    object_datum *object = (object_datum *)object_get_and_verify_type(object_index, 0xffffffff);
    const s_object_definition *object_definition = (const s_object_definition *)tag_get('obje', object->definition_index);
    const s_model_definition *model_definition = (const s_model_definition *)tag_get('hlmt', object_definition->model.index);
    s_physics_model_definition *physics_definition = (s_physics_model_definition *)tag_get('phmo', model_definition->physics_model_index);
    const s_model_node *element_a =
        (const s_model_node *)tag_block_get_element_with_size(
            &model_definition->nodes, leg->node_a, sizeof(s_model_node));
    const s_model_node *element_b =
        (const s_model_node *)tag_block_get_element_with_size(
            &model_definition->nodes, leg->node_b, sizeof(s_model_node));
    c_havok_component *component = datum_get<c_havok_component>(g_havok_component_data, object->havok_component_index);
    c_havok_component::s_powered_chain_link_reference reference_b = k_invalid_powered_chain_link_reference;
    c_havok_component::s_powered_chain_link_reference reference_a = k_invalid_powered_chain_link_reference;
    if (element_a->parent_node_index != -1 && leg->node_a != -1)
        reference_a = component->find_powered_chain_reference_connecting_nodes(element_a->parent_node_index, leg->node_a);
    if (element_b->parent_node_index != -1 && leg->node_b != -1)
        reference_b = component->find_powered_chain_reference_connecting_nodes(element_b->parent_node_index, leg->node_b);
    const real_vector3d leg_vector = leg->vector;
    const real_matrix4x3 *node_matrix = object_get_node_matrix(object_index, leg->node_a);
    real_matrix4x3 node_transform;
    matrix4x3_compute_product(matrix_b, node_matrix, &node_transform);
    real_vector3d offset = leg_vector;
    add_vectors3d(&offset, &node_transform.position, &offset);
    real_vector3d direction = offset;
    float length = normalize3d(&direction);
    if (length != 0.0f) {
        float cosine = (float)cos((double)angle);
        float sine = (float)sin((double)angle);
        rotate_vector_about_axis(&direction, global_up3d, sine, cosine);
        scale_vector3d(&direction, length, &offset);
    }
    subtract_vectors3d(&offset, &node_transform.position, &offset);
    add_vectors3d(&offset, vector, &offset);
    float offset_i2 = offset.i * offset.i;
    float offset_j2 = offset.j * offset.j;
    float leg_i2 = leg_vector.i * leg_vector.i;
    float leg_j2 = leg_vector.j * leg_vector.j;
    real_point2d offset_point;
    real_point2d leg_point;
    set_real_point2d(&offset_point, square_root(offset_i2 + offset_j2), offset.k);
    set_real_point2d(&leg_point, square_root(leg_i2 + leg_j2), leg_vector.k);
    float offset_length = distance2d(global_origin2d, &offset_point);
    float leg_length = distance2d(global_origin2d, &leg_point);
    float angle_a = calculate_law_off_cosines_angle(offset_length, leg->bone_length_a, leg->bone_length_b);
    float angle_b = calculate_law_off_cosines_angle(leg_length, leg->bone_length_a, leg->bone_length_b);
    float angle_difference = angle_a - angle_b;
    real_vector2d leg_vector2d;
    set_real_vector2d(&leg_vector2d, leg_vector.i, leg_vector.j);
    real_vector2d offset_vector2d;
    set_real_vector2d(&offset_vector2d, offset.i, offset.j);
    normalize2d(&leg_vector2d);
    normalize2d(&offset_vector2d);
    float signed_angle = signed_angle_between_vectors2d(&leg_vector2d, &offset_vector2d);
    float elevation_a = arctangent(offset_point.y, offset_point.x);
    float elevation_b = arctangent(leg_point.y, leg_point.x);
    float combined_a = calculate_law_off_cosines_angle(leg->bone_length_b, leg->bone_length_a, offset_length);
    float combined_b = calculate_law_off_cosines_angle(leg->bone_length_b, leg->bone_length_a, leg_length);
    float combined_angle = (combined_a + elevation_a) - (combined_b + elevation_b);
    if (reference_a.constraint_index != -1) {
        float sine_signed = (float)sin((double)signed_angle);
        float cosine_signed = (float)cos((double)signed_angle);
        float sine_combined = (float)sin((double)combined_angle);
        float cosine_combined = (float)cos((double)combined_angle);
        real_matrix3x3 rotation = global_identity4x3->rotation;
        if (combined_angle != 0.0f) {
            rotate_vector_about_axis(&rotation.up, &rotation.left, sine_signed, cosine_signed);
            rotate_vector_about_axis(&rotation.forward, &rotation.left, sine_signed, cosine_signed);
        } else {
            const real_vector3d *axis = &matrix_a->up;
            rotate_vector_about_axis(&rotation.forward, axis, sine_signed, cosine_signed);
            rotate_vector_about_axis(&rotation.left, axis, sine_signed, cosine_signed);
            rotate_vector_about_axis(&rotation.up, axis, sine_signed, cosine_signed);
        }
        rotate_vector_about_axis(&rotation.forward, &rotation.up, sine_combined, cosine_combined);
        rotate_vector_about_axis(&rotation.left, &rotation.up, sine_combined, cosine_combined);
        s_physics_model_constraint_reference constraint_reference = component->get_physics_model_constraint_reference(reference_a.constraint_index);
        s_physics_model_powered_chain *chain = (s_physics_model_powered_chain *)tag_block_get_element_with_size(
            &physics_definition->powered_chains, constraint_reference.index, sizeof(s_physics_model_powered_chain));
        const s_physics_model_powered_chain_link *link =
            (const s_physics_model_powered_chain_link *)tag_block_get_element_with_size(
                &chain->links, reference_a.link_index, sizeof(s_physics_model_powered_chain_link));
        s_walker_constraint_bodies *bodies =
            (s_walker_constraint_bodies *)(ptrdiff_t)physics_model_try_and_get_constraint(physics_definition, &link->constraint);
        const real_matrix4x3 *parent = bodies->rigid_body_b > bodies->rigid_body_a ? &bodies->bodies[0] : &bodies->bodies[1];
        const real_matrix4x3 *child = bodies->rigid_body_b > bodies->rigid_body_a ? &bodies->bodies[1] : &bodies->bodies[1];
        real_matrix3x3 temp;
        matrix3x3_multiply(&parent->rotation, &child->rotation, &temp);
        matrix3x3_multiply(&temp, &rotation, &rotation);
        component->keyframe_powered_chain_link(reference_a, &rotation);
    }
    if (reference_b.constraint_index != -1) {
        float sine_difference = (float)sin((double)angle_difference);
        float cosine_difference = (float)cos((double)angle_difference);
        real_matrix3x3 rotation = global_identity4x3->rotation;
        rotate_vector_about_axis(&rotation.forward, &rotation.up, sine_difference, cosine_difference);
        rotate_vector_about_axis(&rotation.left, &rotation.up, sine_difference, cosine_difference);
        s_physics_model_constraint_reference constraint_reference = component->get_physics_model_constraint_reference(reference_b.constraint_index);
        s_physics_model_powered_chain *chain = (s_physics_model_powered_chain *)tag_block_get_element_with_size(
            &physics_definition->powered_chains, constraint_reference.index, sizeof(s_physics_model_powered_chain));
        const s_physics_model_powered_chain_link *link =
            (const s_physics_model_powered_chain_link *)tag_block_get_element_with_size(
                &chain->links, reference_b.link_index, sizeof(s_physics_model_powered_chain_link));
        s_walker_constraint_bodies *bodies =
            (s_walker_constraint_bodies *)(ptrdiff_t)physics_model_try_and_get_constraint(physics_definition, &link->constraint);
        const real_matrix4x3 *parent = bodies->rigid_body_b > bodies->rigid_body_a ? &bodies->bodies[0] : &bodies->bodies[1];
        const real_matrix4x3 *child = bodies->rigid_body_b > bodies->rigid_body_a ? &bodies->bodies[1] : &bodies->bodies[1];
        real_matrix3x3 temp;
        matrix3x3_multiply(&parent->rotation, &child->rotation, &temp);
        matrix3x3_multiply(&temp, &rotation, &rotation);
        component->keyframe_powered_chain_link(reference_b, &rotation);
    }
}

// @address 0x579af8 ?keyframe_leg_group@c_walker_physics@@SAXPBUs_walker_physics_definition@@PBUreal_matrix3x3@@JJJPBJPBTreal_vector3d@@M@Z
void c_walker_physics::keyframe_leg_group(const s_walker_physics_definition *definition, const real_matrix3x3 *orientation,
    long object_index, long group_a, long group_b, const long *flags, const real_vector3d *vector, float angle)
{
    real_point3d center = *global_origin3d;
    long count = 0;
    for (long leg_index = 0; leg_index < definition->leg_count; leg_index++) {
        const s_walker_leg_definition *leg = &definition->legs[leg_index];
        if (leg->enabled && leg->group == group_b) {
            const real_matrix4x3 *node_matrix = object_get_node_matrix(object_index, leg->node_a);
            add_points3d(&center, &node_matrix->position, &center);
            count++;
        }
    }
    if ((float)count > 0.0f)
        scale_point3d(&center, 1.0f / (float)count, &center);
    real_matrix4x3 matrix;
    matrix.rotation = *orientation;
    matrix.position = center;
    matrix.scale = 1.0f;
    real_matrix4x3 inverse;
    matrix4x3_inverse(&matrix, &inverse);
    for (long leg_index = 0; leg_index < definition->leg_count; leg_index++) {
        const s_walker_leg_definition *leg = &definition->legs[leg_index];
        if (leg->enabled && leg->group == group_a)
            keyframe_leg(definition, object_index, leg_index, ((*flags & (1 << leg_index)) != 0), &matrix, &inverse, vector, angle);
    }
    *(long *)flags = *flags;
}
