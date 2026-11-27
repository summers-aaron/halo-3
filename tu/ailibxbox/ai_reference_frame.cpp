// ailibxbox/ai_reference_frame.obj: from the records 0xb24b8 0xb7fd8 0xb8188 0xc1300 0xc4e80 0xc51d0 0xcd828 0xcd8d0 0xcebd8 0x50c9e0 0x50ca68 0x50cb50 0x50cc18 0x50ccc8 0x50ce58
// @flags /TP /O2
#include "objects/object_reference_types.h"
#include "math/real_math.h"
#include "math/matrix_math_types.h"
#include "tag_files/tag_types.h"
#include "objects/object_types.h"
#include "objects/object_datum.h"
#include "objects/object_header.h"
#include "tag_files/tag_groups.h"
#include "ai/ai_point3d.h"
// @address 0x50cb50 ?ai_refresh_reference_frames@@YAXXZ
// @address 0x50cc18 ?ai_reference_frame_get_by_node_index@@YAFJF@Z
// @address 0x50ccc8 ?ai_reference_frame_transform_point@@YA_NFPBTreal_point3d@@PAT1@@Z
// @address 0xb24b8 ai_point_set_from_world_position
#include "ai/ai_reference_frame.h"
#include "ai/sector_environment_objects.h"
#include "math/matrix_math.h"
#include "math/projection_math.h"
#include "physics/collision_models.h"
#include "scenario/scenario.h"
#include "scenario/scenario_tag.h"

#include "physics/collision_model_types.h"
#include "scenario/scenario_object_reference_frame.h"






#include "devices/device_definitions.h"


// @address 0x50c9e0 object_can_have_reference_frames
extern "C" bool object_can_have_reference_frames(long object_index)
{
    bool result = false;
    s_object_header *header = object_header_get(object_index);
    int object_type = header->object_type;
    if (object_type != 7) {
        if (object_type == 10 || object_type == 12)
            result = true;
    } else {
        const s_machine_definition *machine_definition = TAG_GET('mach', const s_machine_definition, header->datum->definition_index);
        if ((machine_definition->flags & 16) == 0)
            result = machine_definition->type != 0;
    }
    return result;
}

bool object_refresh_reference_frames(long object_index)
{
    bool result = false;
    if (object_can_have_reference_frames(object_index)) {
        scenario *globals = global_scenario;
        object_datum *object = object_get(object_index);
        bool set_flag = object->identifier.m_type == 12;
        for (short index = 0; index < globals->object_reference_frames.count; index++) {
            s_scenario_object_reference_frame *attachment = &((s_scenario_object_reference_frame *)globals->object_reference_frames.address)[index];
            if (attachment->identifier.is_equal(&object->identifier)) {
                s_attachment_state *state = &g_ai_reference_frame_data[index];
                state->object_index = object_index;
                if (set_flag)
                    state->flags |= 1;
                else
                    state->flags &= -2;
                result = true;
            }
        }
    }
    return result;
}

void ai_refresh_reference_frames(void)
{
    c_pathfinding_object objects[4000];
    long count = find_pathfinding_objects(global_scenario, 0, objects, 4000, true);

    short index = 0;
    s_attachment_state *frames = g_ai_reference_frame_data;
    do {
        frames[index].object_index = -1;
        frames[index].flags = 0;
        frames[index].reserved = 0;
        index = (short)(index + 1);
    } while (index < 150);

    if (count > 0) {
        index = 0;
        do {
            c_pathfinding_object *const &entry = &objects[index];
            int handle = entry->object_index;
            if (handle != -1)
                object_refresh_reference_frames(handle);
            index = (short)(index + 1);
        } while (index < count);
    }
}

static __forceinline short ai_reference_frame_find_by_node_index(const c_object_identifier *identifier, short node_index)
{
    const scenario *globals = global_scenario;
    short result = -1;
    for (short index = 0; index < globals->object_reference_frames.count; index++) {
        const s_scenario_object_reference_frame *attachment = &((s_scenario_object_reference_frame *)globals->object_reference_frames.address)[index];
        if (attachment->identifier.is_equal(identifier) && attachment->node_index == node_index) {
            result = index;
            break;
        }
    }
    return result;
}

short ai_reference_frame_get_by_node_index(long object_index, short node_index)
{
    object_datum *object = object_get(object_index);
    return ai_reference_frame_find_by_node_index(&object->identifier, node_index);
}

bool ai_reference_frame_transform_point(short index, const real_point3d *point, real_point3d *result)
{
    bool transformed = false;
    scenario *globals = global_scenario;

    if (index >= 0 && index < globals->object_reference_frames.count) {
        s_attachment_state *reference_frame = &g_ai_reference_frame_data[index];
        s_scenario_object_reference_frame *attachment = &((s_scenario_object_reference_frame *)globals->object_reference_frames.address)[index];
        if (reference_frame->object_index != -1) {
            int node_index = attachment->node_index;
            object_datum *object = object_get(reference_frame->object_index);
            if (node_index >= 0 && node_index < (int)(object->node_matrices.size / sizeof(real_matrix4x3))) {
                short axis = attachment->axis;
                bool flip = (attachment->flags & 1) != 0;
                real_point3d permuted;
                for (int i = 0; i < 3; i++)
                    permuted.n[i] = point->n[global_projection3d_inverse_mappings[axis][flip][i]];
                const real_matrix4x3 *nodes = (const real_matrix4x3 *)((char *)object + object->node_matrices.offset);
                matrix4x3_transform_point(&nodes[node_index], &permuted, result);
                return true;
            }
        }
        *result = *point;
    } else {
        *result = *point;
        transformed = true;
    }
    return transformed;
}


static __forceinline real_vector3d *ai_reference_frame_vector_from_points(const real_point3d *a, const real_point3d *b, real_vector3d *result)
{
    result->i = b->x - a->x;
    result->j = b->y - a->y;
    result->k = b->z - a->z;
    return result;
}

static __forceinline float ai_reference_frame_magnitude_squared(const real_vector3d *v)
{
    return v->i * v->i + v->j * v->j + v->k * v->k;
}

static __forceinline float ai_reference_frame_distance_squared(const real_point3d *a, const real_point3d *b)
{
    real_vector3d vector;
    ai_reference_frame_vector_from_points(a, b, &vector);
    return ai_reference_frame_magnitude_squared(&vector);
}

// @address 0xb7fd8 ?ai_distance_squared3d@@YAMPBVc_ai_point3d@@PBTreal_point3d@@@Z
float ai_distance_squared3d(const c_ai_point3d *point, const real_point3d *other)
{
    if (point->reference_frame() == -1)
        return ai_reference_frame_distance_squared(&point->point, other);
    real_point3d position;
    ai_point_get_position(point, &position);
    return ai_reference_frame_distance_squared(&position, other);
}

void ai_vector_from_points3d(const real_point3d *a, const c_ai_point3d *b, real_vector3d *result)
{
    if (b->location[0] == -1) {
        ai_reference_frame_vector_from_points(a, &b->point, result);
        return;
    }
    real_point3d position;
    ai_point_get_position(b, &position);
    ai_reference_frame_vector_from_points(a, &position, result);
}

void ai_vector_from_points3d(const c_ai_point3d *a, const c_ai_point3d *b, real_vector3d *result)
{
    if (a->location[0] == b->location[0]) {
        ai_reference_frame_vector_from_points(&a->point, &b->point, result);
        if (a->location[0] != -1)
            ai_reference_frame_transform_vector(a->location[0], result, result);
        return;
    }
    real_point3d a_position, b_position;
    ai_point_get_position(a, &a_position);
    ai_point_get_position(b, &b_position);
    ai_reference_frame_vector_from_points(&a_position, &b_position, result);
}

// @address 0xc51d0 ?ai_point_get_position@@YAPATreal_point3d@@PBVc_ai_point3d@@PAT1@@Z
real_point3d *ai_point_get_position(const c_ai_point3d *point, real_point3d *result)
{
    if (point->reference_frame() != -1) {
        if (!ai_reference_frame_transform_point(point->location[0], &point->point, result))
            *result = point->point;
    } else {
        *result = point->point;
    }
    return result;
}

// @address 0xcd828 ?ai_distance_squared3d@@YAMPBVc_ai_point3d@@0@Z
float ai_distance_squared3d(const c_ai_point3d *a, const c_ai_point3d *b)
{
    if (a->location[0] == b->location[0])
        return ai_reference_frame_distance_squared(&a->point, &b->point);
    real_point3d pa;
    real_point3d pb;
    ai_point_get_position(a, &pa);
    ai_point_get_position(b, &pb);
    return ai_reference_frame_distance_squared(&pa, &pb);
}

// @address 0xcd8d0 ?ai_reference_frames_are_incompatible@@YA_NFF@Z
bool ai_reference_frames_are_incompatible(short a, short b)
{
    bool result = false;
    if (a != b) {
        s_attachment_state *state_a = a != -1 ? &g_ai_reference_frame_data[a] : 0;
        s_attachment_state *state_b = b != -1 ? &g_ai_reference_frame_data[b] : 0;
        if (state_a && (state_a->flags & 1))
            result = true;
        else if (state_b && (state_b->flags & 1))
            result = true;
    }
    return result;
}

// @address 0xcebd8 ?ai_reference_frame_transform_vector@@YA_NFPBTreal_vector3d@@PAT1@@Z
bool ai_reference_frame_transform_vector(short index, const real_vector3d *vector, real_vector3d *result_vec)
{
    bool transformed = false;
    scenario *globals = global_scenario;

    if (index >= 0 && index < globals->object_reference_frames.count) {
        s_attachment_state *reference_frame = &g_ai_reference_frame_data[index];
        s_scenario_object_reference_frame *attachment = &((s_scenario_object_reference_frame *)globals->object_reference_frames.address)[index];
        if (reference_frame->object_index != -1) {
            int node_index = attachment->node_index;
            object_datum *object = object_get(reference_frame->object_index);
            if (node_index >= 0 && node_index < (int)(object->node_matrices.size / sizeof(real_matrix4x3))) {
                short axis = attachment->axis;
                bool flip = (attachment->flags & 1) != 0;
                real_vector3d permuted;
                for (int i = 0; i < 3; i++)
                    permuted.n[i] = vector->n[global_projection3d_inverse_mappings[axis][flip][i]];
                const real_matrix4x3 *nodes = (const real_matrix4x3 *)((char *)object + object->node_matrices.offset);
                matrix4x3_transform_vector(&nodes[node_index], &permuted, result_vec);
                return true;
            }
        }
        *result_vec = *vector;
    } else {
        *result_vec = *vector;
        transformed = true;
    }
    return transformed;
}


// @address 0x50ce58 ?ai_reference_frame_inverse_transform_vector@@YA_NFPBTreal_vector3d@@PAT1@@Z
bool ai_reference_frame_inverse_transform_vector(short index, const real_vector3d *vector, real_vector3d *result_vec)
{
    bool transformed = false;
    scenario *globals = global_scenario;

    if (index >= 0 && index < globals->object_reference_frames.count) {
        s_attachment_state *reference_frame = &g_ai_reference_frame_data[index];
        s_scenario_object_reference_frame *attachment = &((s_scenario_object_reference_frame *)globals->object_reference_frames.address)[index];
        if (reference_frame->object_index != -1) {
            int node_index = attachment->node_index;
            object_datum *object = object_get(reference_frame->object_index);
            if (node_index >= 0 && node_index < (int)(object->node_matrices.size / sizeof(real_matrix4x3))) {
                const real_matrix4x3 *nodes = (const real_matrix4x3 *)((char *)object + object->node_matrices.offset);
                real_vector3d temp;
                matrix4x3_inverse_transform_vector(&nodes[node_index], vector, &temp);
                bool flip = (attachment->flags & 1) != 0;
                short axis = attachment->axis;
                set_real_vector3d(result_vec, temp.n[global_projection3d_mappings[axis][flip][0]],
                    temp.n[global_projection3d_mappings[axis][flip][1]], temp.n[global_projection3d_mappings[axis][flip][2]]);
                return true;
            }
        }
        *result_vec = *vector;
    } else {
        *result_vec = *vector;
        transformed = true;
    }
    return transformed;
}

// 0xb24b8
extern "C" bool ai_point_set_from_world_position(const real_point3d *point, long object_index, long node_index, c_ai_point3d *result)
{
    if (object_index == -1) {
        result->point = *point;
        result->location[0] = -1;
    } else {
        short frame_index = -1;
        collision_model_instance instance;
        if (object_can_have_reference_frames(object_index) && node_index != -1 &&
            ::collision_model_instance_new(&instance, object_index))
            frame_index = ai_reference_frame_get_by_node_index(object_index, node_index & 255);
        if (frame_index == -1) {
            result->point = *point;
            result->location[0] = -1;
        } else if (ai_reference_frame_inverse_transform_point(frame_index, point, &result->point)) {
            result->location[0] = frame_index;
        } else {
            result->point = *point;
            result->location[0] = -1;
        }
    }
    result->location[1] = -1;
    return true;
}

// @address 0xc1300 ?ai_reference_frame_inverse_transform_point@@YA_NFPBTreal_point3d@@PAT1@@Z
bool ai_reference_frame_inverse_transform_point(short index, const real_point3d *point, real_point3d *result)
{
    bool transformed = false;
    scenario *globals = global_scenario;

    if (index >= 0 && index < globals->object_reference_frames.count) {
        s_attachment_state *reference_frame = &g_ai_reference_frame_data[index];
        s_scenario_object_reference_frame *attachment = &((s_scenario_object_reference_frame *)globals->object_reference_frames.address)[index];
        if (reference_frame->object_index != -1) {
            int node_index = attachment->node_index;
            object_datum *object = object_get(reference_frame->object_index);
            if (node_index >= 0 && node_index < (int)(object->node_matrices.size / sizeof(real_matrix4x3))) {
                const real_matrix4x3 *nodes = (const real_matrix4x3 *)((char *)object + object->node_matrices.offset);
                real_point3d temp;
                matrix4x3_inverse_transform_point(&nodes[node_index], point, &temp);
                project_point3d(&temp, attachment->axis, (attachment->flags & 1) != 0, result);
                return true;
            }
        }
        *result = *point;
    } else {
        *result = *point;
        transformed = true;
    }
    return transformed;
}
