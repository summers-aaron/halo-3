#include "memory/tls_globals.h"
#include "multithreading/thread_tls.h"
// ailibxbox/actor_dynamic_firing_position.obj: from the records 0xb4a98 0x5581f8 0x559778 0x5598b8 0x559cc8 0x559e90 0x559f28 0x559fa0 0x55a020 0x55a1b8 0x55a230 0x55a350 0x55a490 0x55a690 0x55abc0 0x55ac60
// @flags /TP /O2
#include "math/real_math_globals.h"
#define REAL_MATH_INLINE_ROTATION
#include "math/real_math_functions.h"
#undef REAL_MATH_INLINE_ROTATION
#include "memory/data_types.h"
#include "memory/tls_data_arrays.h"
#include "memory/static_arrays.h"
#include "math/real_math_types.h"
#include "math/matrix_math_types.h"
#include "ai/ai_point3d.h"
#include "ai/sector_ref.h"
#include "ai/actor_datum.h"
#include "ai/actor_firing_position.h"
#include "objects/object_definitions.h"
#include "ai/firing_position_definition.h"
#include "objects/object_reference_types.h"
#include "objects/object_datum.h"
#include "objects/object_placement.h"
#include "tag_files/tag_groups.h"

// @address 0x5581f8 ?__tls_set_g_dynamic_firing_set_allocator@@YAXPAX@Z
#include <math.h>
#include "ai/actors.h"
#include "cache/restricted_memory_regions.h"
#include "objects/objects.h"
#include "scenario/scenario.h"
#include "ai/actor_dynamic_firing_position.h"
#include "gpu_particle/particle_block.h"
#include "ai/pathfinding_utilities.h"
#include "memory/data.h"
#include "ai/sector.h"

#include "ai/ai_reference_frame.h"
#include "physics/collision_types.h"
#include "scenario/scenario_structure_bsp_reference.h"
#include "game/game_time_conversion.h"
#include "ai/path_types.h"
#include "cache/restricted_memory.h"
#include "scenario/scenario_tag.h"
#include "cache/restricted_memory_layout.h"
#include "structures/structure_bsp_tag.h"
#include "ai/pathfinding_collision.h"
#include "ai/sector_geometry.h"
#include "cache/restricted_memory_registration.h"
#include "cache/restricted_memory_region_members.h"
#include "objects/object_ai_properties.h"
#include "objects/object_header_access.h"
#include "math/real_math.h"


typedef char dynamic_firing_set_size[(sizeof(dynamic_firing_set_datum) == 0x584) ? 1 : -1];
typedef char dynamic_firing_set_points_offset[(offsetof(dynamic_firing_set_datum, points) == 0x44) ? 1 : -1];


namespace
{
struct s_dynamic_firing_set_data_publisher
{
    static __forceinline void publish(void *address)
    {
        data_set_new_base_address(&dynamic_firing_set_data, (s_data_array *)address);
    }
};


    struct dynamic_firing_set_iterator
    {
        c_data_iterator<dynamic_firing_set_datum> data_iterator;
        long index;
    };

    static __forceinline void dynamic_firing_set_iterator_new(dynamic_firing_set_iterator *iterator)
    {
        iterator->data_iterator.begin(dynamic_firing_set_data);
    }

    static __forceinline dynamic_firing_set_datum *dynamic_firing_set_iterator_next(dynamic_firing_set_iterator *iterator)
    {
        dynamic_firing_set_datum *datum = 0;
        if (iterator->data_iterator.next())
            datum = iterator->data_iterator.get_datum();
        iterator->index = iterator->data_iterator.get_index();
        return datum;
    }

}

void __tls_set_g_dynamic_firing_set_allocator(void *object)
{
    data_set_new_base_address(&joint_state_data(), (s_data_array *)object);
}

// @address 0x559e90 ?dynamic_firing_set_find@@YAJJF@Z
long dynamic_firing_set_find(long object_index, short firing_set_index)
{
    long result = -1;
    dynamic_firing_set_iterator iterator;
    dynamic_firing_set_iterator_new(&iterator);
    dynamic_firing_set_datum *datum;
    while ((datum = dynamic_firing_set_iterator_next(&iterator)) != 0)
    {
        if (datum->object_index == object_index && datum->firing_set_index == firing_set_index)
        {
            result = iterator.index;
            break;
        }
    }
    return result;
}

// @address 0x559f28 dynamic_firing_set_attach
extern "C" void dynamic_firing_set_attach(long actor_index, long firing_set_index)
{
    actor_datum *actor =
        datum_get<actor_datum>(tls_actor_data(), actor_index);
    dynamic_firing_set_datum *datum = datum_get<dynamic_firing_set_datum>(dynamic_firing_set_data, firing_set_index);
    if (actor->dynamic_firing_set_index != firing_set_index)
    {
        if (actor->dynamic_firing_set_index != -1)
        {
            dynamic_firing_set_discard(actor_index);
        }
        actor->dynamic_firing_set_index = firing_set_index;
        datum->reference_count++;
    }
}

// @address 0x559fa0 ?dynamic_firing_set_discard@@YAXJ@Z
void dynamic_firing_set_discard(long actor_index)
{
    actor_datum *actor = datum_get<actor_datum>(tls_actor_data(), actor_index);
    long dynamic_firing_set_index = actor->dynamic_firing_set_index;
    actor_flush_position_indices(actor_index, true);
    actor->dynamic_firing_set_index = -1;
    dynamic_firing_set_datum *datum = datum_get<dynamic_firing_set_datum>(dynamic_firing_set_data,
        dynamic_firing_set_index);
    if (--datum->reference_count == 0)
    {
        ::datum_delete(dynamic_firing_set_data, dynamic_firing_set_index);
    }
}


// @address 0x559778 ?dynamic_firing_set_new_internal@@YAJJF@Z
long dynamic_firing_set_new_internal(long object_index, short firing_set_index)
{
    long result = -1;
    c_sector_ref sector;
    sector.clear();
    c_ai_point3d point;
    object_get_pathfinding_location(object_index, &sector, &point);
    if (sector.valid())
    {
        result = datum_new(dynamic_firing_set_data);
        if (result != -1)
        {
            dynamic_firing_set_datum *datum = datum_get<dynamic_firing_set_datum>(dynamic_firing_set_data, result);
            datum->firing_set_index = firing_set_index;
            datum->object_index = object_index;
            datum->position_count = 0;
            datum->reference_count = 0;
            datum->initialized_points.clear();
            real_vector3d forward, up;
            object_get_orientation(object_index, &forward, &up);
            datum->position = point;
            datum->sector = sector;
            datum->up = up;
            datum->forward = forward;
            datum->valid = true;
            datum->update_time = game_time_get();
        }
    }
    return result;
}


// @address 0x559cc8 ?dynamic_firing_point_initialize@@YA_NJFFPAVc_ai_point3d@@Vc_sector_ref@@PATreal_vector3d@@@Z
bool dynamic_firing_point_initialize(long firing_set_index, short point_index, short type, c_ai_point3d *point,
    c_sector_ref sector, real_vector3d *facing)
{
    dynamic_firing_set_datum *firing_set = datum_get<dynamic_firing_set_datum>(dynamic_firing_set_data,
        firing_set_index);
    bool attached = false;
    bool result = false;
    if (point_index >= 0 && point_index < 32) {
        if (!firing_set->initialized_points.test(point_index)) {
            firing_position_definition *firing_point = &firing_set->points[point_index];
            real_point3d position;
            ai_point_get_position(point, &position);
            point_from_line3d(&position, global_up3d, 0.05f, &position);
            s_cluster_reference cluster = scenario_cluster_reference_from_point(sector.structure_bsp_index, &position);
            if (cluster.bsp_index != -1) {
                firing_point->cluster_index = cluster.cluster_index;
                if (firing_point->cluster_index != -1) {
                    firing_set->initialized_points.set(point_index, true);
                    firing_point->sector = sector;
                    firing_point->point = *point;
                    firing_point->flags = 0;
                    firing_point->cornering_flags = 0;
                    firing_point->area = -1;
                    if (facing) {
                        euler_angles2d_from_vector3d(&firing_point->normal, facing);
                    } else {
                        firing_point->normal.yaw = 0.0f;
                        firing_point->normal.pitch = 0.0f;
                    }
                    firing_point->flags |= 0x40;
                    long point_type = type;
                    if (point_type > 0 && point_type <= 3)
                        firing_point->flags |= 0x90;
                    firing_set->point_types[point_index] = type;
                    result = true;
                }
            }
        } else {
            result = true;
        }
    }
    return result;
}


// @address 0x55a020 ?firing_set_update@@YA_NJ@Z
bool firing_set_update(long firing_set_index)
{
    dynamic_firing_set_datum *set = datum_get<dynamic_firing_set_datum>(dynamic_firing_set_data, firing_set_index);
    bool result = true;
    if (game_time_get() > set->update_time)
    {
        set->valid = false;
        if (set->object_index != -1)
        {
            real_vector3d forward, up;
            object_get_orientation(set->object_index, &forward, &up);
            if (set->type == 1)
            {
                if (dot_product3d(&up, &set->up) < 0.9f)
                    result = false;
                else if (dot_product3d(&forward, &set->forward) < 0.99f)
                    result = false;
            }
            if (result)
            {
                float distance_squared = set->type == 1 ? 0.0025000002f : 0.040000003f;
                c_sector_ref sector;
                c_ai_point3d position;
                object_get_pathfinding_location(set->object_index, &sector, &position);
                if (sector.valid())
                {
                    if (ai_distance_squared3d(&position, &set->position) <= distance_squared)
                    {
                        set->valid = true;
                        result = true;
                    }
                    else
                    {
                        result = false;
                    }
                }
                else
                {
                    result = false;
                }
            }
        }
        else
        {
            result = false;
        }
        set->update_time = game_time_get();
    }
    else
    {
        result = set->valid;
    }
    return result;
}

// @address 0x55a1b8 ?actor_dynamic_firing_set_support_object@@YAXJJ@Z
void actor_dynamic_firing_set_support_object(long actor_index, long object_index)
{
    actor_datum *actor =
        datum_get<actor_datum>(tls_actor_data(), actor_index);
    if (actor->support_object_index != object_index)
    {
        if (actor->dynamic_firing_set_index != -1)
        {
            dynamic_firing_set_discard(actor_index);
        }
        actor->support_object_index = object_index;
        actor->support_index = 0;
    }
}


static __forceinline long actor_dynamic_firing_position_ultimate_parent(long object_index)
{
    long parent;
    do
    {
        parent = object_index;
        object_index = object_get(object_index)->parent_object_index;
    } while (object_index != -1);
    return parent;
}


// @rdata 0x82022dc0
static const float actor_dynamic_firing_position_point_offset = 0.5f;


static __forceinline s_cluster_reference actor_dynamic_firing_position_cluster_reference_from_leaf_index(long structure_bsp_index, long leaf_index)
{
    s_cluster_reference cluster_reference;
    scenario_structure_bsp_reference *bsp_reference = TAG_BLOCK_GET_ELEMENT(
        &global_scenario->structure_bsps, structure_bsp_index, scenario_structure_bsp_reference);
    structure_bsp *bsp = (structure_bsp *)tag_get('sbsp', bsp_reference->structure_bsp.index);
    cluster_reference.bsp_index = (signed char)structure_bsp_index;
    cluster_reference.cluster_index = ((unsigned char *)bsp->leaves.address)[leaf_index];
    return cluster_reference;
}

static __forceinline s_cluster_reference actor_dynamic_firing_position_cluster_reference_from_point(const real_point3d *point)
{
    long structure_bsp_index;
    s_cluster_reference cluster_reference;
    long leaf_index = scenario_leaf_index_from_point(point, &structure_bsp_index);
    if (leaf_index != -1)
        cluster_reference = actor_dynamic_firing_position_cluster_reference_from_leaf_index(structure_bsp_index,
            leaf_index);
    else
        *(short *)&cluster_reference = -1;
    return cluster_reference;
}

// @address 0x5598b8 ?dynamic_firing_set_new_automatic@@YAJJ@Z
long dynamic_firing_set_new_automatic(long object_index)
{
    long firing_set_index = dynamic_firing_set_new_internal(object_index, 0);
    if (firing_set_index != -1)
    {
        dynamic_firing_set_datum *firing_set = datum_get<dynamic_firing_set_datum>(dynamic_firing_set_data,
            firing_set_index);
        object_datum *object = object_get(object_index);
        float radius = object->bounding_radius;
        c_sector_ref sector = firing_set->sector;
        s_object_definition *definition = (s_object_definition *)tag_get(
            'obje', object->definition_index);
        if (definition->ai_properties.count > 0 &&
            TEST_BIT(TAG_BLOCK_GET_ELEMENT(&definition->ai_properties, 0, object_ai_properties)->flags, 4))
            radius = 0.5f;
        for (short ring_index = 0; ring_index < 2; ring_index++)
        {
            float ring = (float)ring_index;
            float distance = (float)(ring * radius + radius * 1.2);
            float angle_offset = ring * 0.3926991f;
            for (short point_index = 0; point_index < 8; point_index++)
            {
                real_vector3d direction = *global_forward3d;
                float angle = (float)point_index * 0.7853982f + angle_offset;
                float angle_cosine = cos(angle);
                float angle_sine = sin(angle);
                rotate_vector_about_axis(&direction, global_up3d, angle_sine, angle_cosine);
                path_collision_result collision;
                if (!pathfinding_test_vector(&firing_set->position.position, sector, g_sector_none, &direction,
                    distance, 0, &collision) &&
                    !collision.collision)
                {
                    s_location location = object_get(object_index)->location;
                    collision_result collision_info;
                    real_point3d point;
                    point_from_line3d(&firing_set->position.position, &direction, collision.t, &point);
                    point_from_line3d(&point, global_up3d, actor_dynamic_firing_position_point_offset, &point);
                    firing_position_definition *firing_point = &firing_set->points[firing_set->position_count];
                    real_point3d transformed_point;
                    if (!ai_reference_frame_transform_point(firing_set->position.attachment_index, &point,
                        &transformed_point))
                        transformed_point = point;
                    firing_point->sector = pathfinding_find_ground_sector(&transformed_point, global_up3d,
                        &firing_point->point,
                        &collision_info, actor_dynamic_firing_position_point_offset);
                    firing_point->cluster_index = -1;
                    point_from_line3d(&collision_info.point, global_up3d, 0.1f, &point);
                    s_cluster_reference cluster_reference = actor_dynamic_firing_position_cluster_reference_from_point(&point);
                    if (cluster_reference.bsp_index != -1
                        && location.cluster_reference.bsp_index == cluster_reference.bsp_index
                        && cluster_reference.cluster_index != -1)
                    {
                        firing_point->cluster_index = cluster_reference.cluster_index;
                        firing_point->area = -1;
                        firing_point->flags |= 0x40;
                        firing_point->normal.yaw = 0.0f;
                        firing_point->normal.pitch = 1.5707964f;
                        firing_set->point_types[firing_set->position_count] = 0;
                        firing_set->initialized_points.set(firing_set->position_count, true);
                        firing_set->position_count++;
                    }
                }
            }
        }
    }
    return firing_set_index;
}

// @address 0x55a230 ?support_object_accessible@@YA_NJJ_N@Z
bool support_object_accessible(long actor_index, long support_object_index, bool ignore_velocity)
{
    bool attached = false;
    bool result = false;
    actor_datum *actor = datum_get<actor_datum>(tls_actor_data(), actor_index);
    object_datum *object = object_get(actor_dynamic_firing_position_ultimate_parent(support_object_index));
    real_vector3d velocity;
    object_get_velocities(support_object_index, &velocity, 0);
    if (ignore_velocity || magnitude_squared3d(&velocity) < 0.1f * 0.1f)
    {
        real_point3d position = object->bounding_center;
        if (distance3d(&position, &actor->position.body) < 15.0f)
        {
            result = true;
        }
    }
    return result;
}


// @address 0x55a350 ?object_get_dynamic_cover_points@@YAFJPAUobject_marker@@PAFF@Z
short object_get_dynamic_cover_points(long object_index, object_marker *markers, short *marker_types,
    short maximum_marker_count)
{
    short marker_count = internal_object_get_markers_by_string_id(object_index, 723, markers, 32, false);
    if (marker_types)
    {
        for (short marker_index = 0; marker_index < marker_count; marker_index++)
            marker_types[marker_index] = 2;
    }
    if (marker_count < 32)
    {
        short added_count = internal_object_get_markers_by_string_id(object_index, 724, &markers[marker_count],
            (short)(32 - marker_count), false);
        if (marker_types)
        {
            for (short marker_index = marker_count; marker_index < marker_count + added_count; marker_index++)
                marker_types[marker_index] = 1;
        }
        marker_count = (short)(marker_count + added_count);
    }
    if (marker_count < 32)
    {
        short added_count = internal_object_get_markers_by_string_id(object_index, 725, &markers[marker_count],
            (short)(32 - marker_count), false);
        if (marker_types)
        {
            for (short marker_index = marker_count; marker_index < marker_count + added_count; marker_index++)
                marker_types[marker_index] = 3;
        }
        marker_count = (short)(marker_count + added_count);
    }
    return marker_count;
}


// @address 0x55abc0 ?allocate@?$t_restricted_allocation_manager@$00$0A@$0A@$1?__tls_set_g_dynamic_firing_set_allocator@@YAXPAX@Z@@UAAPAXIPBD@Z
template<> void *t_restricted_allocation_manager<1,
    static_cast<void (*)(void *)>(0), static_cast<void (*)(void *)>(0),
    &__tls_set_g_dynamic_firing_set_allocator>::allocate(unsigned int size, const char *name)
{
    m_member_index = restricted_region_add_member<s_dynamic_firing_set_data_publisher>(1, name, "", size, 0,
        __tls_set_g_particle_row_data_array_allocator);
    m_thread_id = get_current_thread_index();
    return restricted_region_get_member_address(1, m_member_index);
}

// @address 0x55ac60 ?deallocate@?$t_restricted_allocation_manager@$00$0A@$0A@$1?__tls_set_g_dynamic_firing_set_allocator@@YAXPAX@Z@@UAAXPAX@Z
template<> void t_restricted_allocation_manager<1,
    static_cast<void (*)(void *)>(0), static_cast<void (*)(void *)>(0),
    &__tls_set_g_dynamic_firing_set_allocator>::deallocate(void *address)
{
    dynamic_firing_set_data = 0;
    restricted_region_free_member(1, m_member_index);
}

// match: noalias declaration preserves caller live registers

template<class P, class O> static __forceinline O *firing_line_v2(const P *p, const real_vector3d *v, float d, O *out)
{
    out->i = p->i + d*v->i;
    out->j = p->j + d*v->j;
    out->k = p->k + d*v->k;
    return out;
}

#define firing_ai_point_from_world(frame, world, out, success) do { \
    short firing_frame = (frame); \
    (success) = false; \
    if (::ai_reference_frame_inverse_transform_point(firing_frame, \
        (::real_point3d const *)((const real_point3d *)(world)), (::real_point3d *)(&(out)->position))) { \
        (out)->attachment_index = firing_frame; \
        (out)->pad = scenario_structure_index_from_point((const real_point3d *)(world)); \
        (success) = true; \
    } \
} while (0)


// @address 0x55a490 dynamic_firing_position_find_test_point
extern "C" bool dynamic_firing_position_find_test_point(long actor_index, long set_index, short marker_index,
    short type,
    object_marker *marker, real_point3d *world_position, c_ai_point3d *position, c_sector_ref *sector)
{
    bool attached = false;
    bool result = false;
    dynamic_firing_set_datum *set = datum_get<dynamic_firing_set_datum>(dynamic_firing_set_data, set_index);
    if ((unsigned short)set->type == 1) {
        real_vector3d test_point = marker->matrix.left;
        test_point.k = 0.0f;
        if (normalize3d(&test_point) > 0.0f) {
            switch (type) {
            case 1:
                firing_line_v2(&marker->matrix.position, &test_point, 0.25f, &test_point);
                break;
            case 2:
                firing_line_v2(&marker->matrix.position, &test_point, -0.25f, &test_point);
                break;
            case 3:
                test_point = *(const real_vector3d *)&marker->matrix.position;
                break;
            default:
                type = -1;
                break;
            }
            if (type != -1) {
                real_vector3d up = marker->matrix.up;
                up.k = 0.0f;
                firing_line_v2(&test_point, &up, 0.15f, &test_point);
                bool converted;
                firing_ai_point_from_world(set->position.attachment_index, &test_point, position, converted);
                if (converted) {
                    if (sector_from_point_and_sector(&set->position.position, set->sector, &position->position,
                        sector)) {
                        result = true;
                    }
                }
            }
        }
    }
    return result;
}

static __forceinline long dynamic_firing_set_new_cover_markers(long object_index)
{
    long firing_set = -1;
    s_object_definition *definition = TAG_GET(
        'obje', s_object_definition, object_get(object_index)->definition_index);
    if (definition->ai_properties.count > 0
        && TEST_BIT(TAG_BLOCK_GET_ELEMENT(&definition->ai_properties, 0, object_ai_properties)->flags, 2))
    {
        firing_set = dynamic_firing_set_new_internal(object_index, 1);
        if (firing_set != -1)
            datum_get<dynamic_firing_set_datum>(dynamic_firing_set_data, firing_set)->position_count = 32;
    }
    return firing_set;
}
// @address 0x55a690 dynamic_firing_set_find_and_attach
extern "C" bool dynamic_firing_set_find_and_attach(
    long actor_index, firing_position_evaluation_context *context, bool use_primary, bool use_secondary)
{
    bool attached = false;
    bool result = false;
    actor_datum *actor = datum_get<actor_datum>(tls_actor_data(), actor_index);
    object_marker *markers = context->cornering_markers;
    short *marker_types = context->cornering_marker_types;
    if (actor->unit_index != -1)
    {
        object_datum *unit_object = object_get(actor->unit_index);
        s_location location = unit_object->location;
        real_point3d center;
        object_get_origin(actor->unit_index, &center);
        long object_indices[20];
        short object_count = objects_in_sphere(1, 0x14cb, &location, &center, 4.0f, object_indices, 20);
        if (object_count > 0)
        {
            long best_object = -1;
            bool use_cover = false;
            float best_distance = 3.4028234663852886e+38f;
            for (short i = 0; i < object_count; i++)
            {
                long candidate = object_indices[i];
                if (candidate != actor->unit_index)
                {
                    object_datum *object = object_get(candidate);
                    s_object_definition *definition = TAG_GET(
                        'obje', s_object_definition, object->definition_index);
                    if (definition->ai_properties.count > 0)
                    {
                        object_ai_properties *properties = TAG_BLOCK_GET_ELEMENT(
                            &definition->ai_properties, 0, object_ai_properties);
                        if (TEST_BIT(properties->flags, 2)
                            && (candidate != -1 ? support_object_accessible(actor_index, candidate, false) : false))
                        {
                            long flags = properties->flags;
                            bool consider = TEST_BIT(flags, 5) ? use_primary : use_secondary;
                            if (consider)
                            {
                                real_vector3d offset;
                                vector_from_points3d(&unit_object->bounding_center, &object->bounding_center, &offset);
                                // match: 0x55a690: the j term leads the sum; distance_squared3d sums i first.
                                float distance = offset.j * offset.j + offset.i * offset.i + offset.k * offset.k;
                                if (distance < best_distance)
                                {
                                    best_distance = distance;
                                    best_object = candidate;
                                    use_cover = TEST_BIT(flags, 5);
                                }
                            }
                        }
                    }
                }
            }
            if (best_object != -1)
            {
                if (use_cover)
                {
                    long firing_set = -1;
                    dynamic_firing_set_datum *datum = 0;
                    short marker_count = object_get_dynamic_cover_points(best_object, markers, marker_types, 32);
                    c_sector_ref sector;
                    real_vector3d direction;
                    c_ai_point3d point;
                    for (short point_index = 0; point_index < marker_count; point_index++)
                    {
                        object_marker *marker = &markers[point_index];
                        long marker_index = point_index;
                        if (context->evaluate_danger)
                            vector_from_points3d(&marker->matrix.position, &context->target_position, &direction);
                        else if (context->guard)
                            direction = context->guard_plane;
                        else
                            scale_vector3d(&marker->matrix.up, -1.0f, &direction);
                        bool valid = false;
                        if ((marker->matrix.forward.j + marker->matrix.forward.i) * 0.0f + marker->matrix.forward.k > 0.95f)
                            valid = true;
                        if (valid && dot_product3d(&marker->matrix.up, &direction) < 0.0f)
                        {
                            if (firing_set == -1)
                            {
                                firing_set = dynamic_firing_set_find(best_object, 1);
                                if (firing_set == -1)
                                {
                                    firing_set = dynamic_firing_set_new_cover_markers(best_object);
                                    if (firing_set == -1)
                                        break;
                                }
                                actor_dynamic_firing_set_support_object(actor_index, best_object);
                                dynamic_firing_set_attach(actor_index, firing_set);
                                result = true;
                                attached = true;
                                datum = datum_get<dynamic_firing_set_datum>(dynamic_firing_set_data, firing_set);
                                if (firing_set == -1)
                                    continue;
                            }
                            if (!datum->initialized_points.test(marker_index))
                            {
                                sector.clear();
                                if (dynamic_firing_position_find_test_point(actor_index, firing_set, point_index,
                                        marker_types[marker_index], (object_marker *)marker, 0, &point, &sector)
                                    && sector.valid())
                                {
                                    dynamic_firing_point_initialize(firing_set, point_index, marker_types[marker_index], &point, sector, &marker->matrix.up);
                                }
                            }
                        }
                    }
                }
                else
                {
                    long auto_set = dynamic_firing_set_find(best_object, 0);
                    if (auto_set == -1)
                        auto_set = dynamic_firing_set_new_automatic(best_object);
                    if (auto_set != -1)
                    {
                        actor_dynamic_firing_set_support_object(actor_index, best_object);
                        dynamic_firing_set_attach(actor_index, auto_set);
                        attached = true;
                    }
                }
            }
        }
    }
    if (!attached)
        actor_dynamic_firing_set_support_object(actor_index, -1);
    return result;
}


template<typename t_type> static __forceinline const t_type &firing_position_minimum(const t_type &a, const t_type &b) { return a < b ? a : b; }
template<typename t_type> static __forceinline const t_type &firing_position_maximum(const t_type &a, const t_type &b) { return a > b ? a : b; }


// @address 0xb4a98 ?actor_dynamic_firing_positions_update@@YAXJ@Z
void actor_dynamic_firing_positions_update(long actor_index)
{
    actor_datum *actor = datum_get<actor_datum>(tls_actor_data(), actor_index);
    if (actor->root_vehicle_index == -1
        && actor->attached_vehicle_index != -1
        && actor->vehicle_desire >= 2
        && actor->support_object_index != actor->attached_vehicle_index)
    {
        actor_dynamic_firing_set_support_object(actor_index, actor->attached_vehicle_index);
    }

    if (actor->support_object_index != -1)
    {
        if (actor->root_vehicle_index != -1)
        {
            actor_dynamic_firing_set_support_object(actor_index, -1);
        }
        else
        {
            bool valid = support_object_accessible(actor_index, actor->support_object_index,
                actor->dynamic_firing_set_index != -1);
            if (valid)
            {
                actor->support_index = 0;
                if (actor->dynamic_firing_set_index == -1)
                {
                    long firing_set_index = dynamic_firing_set_find(actor->support_object_index, 0);
                    if (firing_set_index != -1)
                    {
                        dynamic_firing_set_attach(actor_index, firing_set_index);
                    }
                    else
                    {
                        firing_set_index = dynamic_firing_set_new_automatic(actor->support_object_index);
                        if (firing_set_index != -1)
                            dynamic_firing_set_attach(actor_index, firing_set_index);
                    }

                    if (firing_set_index != -1)
                    {
                        long delay = actor->squad_seek_ticks + game_seconds_to_ticks_integer(5);
                        long zero = 0;
                        long maximum = game_seconds_to_ticks_integer(8);
                        actor->squad_seek_ticks = (short)firing_position_minimum(firing_position_maximum(delay,
                            zero), maximum);
                    }
                    else
                    {
                        valid = false;
                    }
                }
                else
                {
                    valid = firing_set_update(actor->dynamic_firing_set_index);
                }
            }

            if (!valid)
            {
                if (actor->dynamic_firing_set_index != -1)
                    dynamic_firing_set_discard(actor_index);

                if (actor->support_object_index != actor->attached_vehicle_index)
                {
                    actor->support_index++;
                    if (game_ticks_to_seconds(actor->support_index) > 5.0f)
                    {
                        actor_dynamic_firing_set_support_object(actor_index, -1);
                        actor->squad_seek_ticks = 0;
                    }
                }
            }
        }
    }
}

typedef t_restricted_allocation_manager<1, static_cast<void (*)(void *)>(0), static_cast<void (*)(void *)>(0),
    &__tls_set_g_dynamic_firing_set_allocator> dynamic_firing_set_allocation_manager;
dynamic_firing_set_allocation_manager g_ai_dynamic_firing_set_allocator;


// @address 0x11df798 dynamic_firing_set_data
__declspec(thread) s_data_array *dynamic_firing_set_data;
