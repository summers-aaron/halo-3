// objectslibxbox/antenna.obj: from the records 0x126c18 0x3674a8 0x3674c0 0x367508 0x367520 0x367538 0x367558 0x367690 0x3676a8 0x3676f0 0x367740 0x367b00 0x367c28 0x367d40 0x367e58 0x368030 0x3680d0
// @flags /TP /O2 /Oc
#include "objects/object_reference_types.h"
#include "game/materials.h"
#include "math/real_math.h"
#include "memory/data.h"
#include "memory/datum_index.h"
#include "memory/datum_index_inlines.h"
#include "objects/object_placement.h"
// @address 0x3674c0 ?antennas_initialize@@YAXXZ
// @address 0x367508 ?antennas_initialize_for_new_map@@YAXXZ
// @address 0x367520 ?antennas_dispose_from_old_map@@YAXXZ
// @address 0x367538 ?antennas_dispose@@YAXXZ
// @address 0x367690 ?antenna_delete@@YAXJ@Z
// @address 0x1ec580 ?data_delete_all@@YAXPAUs_data_array@@@Z
// @address 0xa94d0 ?object_get_location@@YAPAUs_location@@JPAU1@@Z
#include <ppcintrinsics.h>
#include <math.h>
#include <float.h>
#include "math/matrix_math.h"
#include "memory/tls_globals.h"
#include "game/game_globals_storage.h"
#include "cache/restricted_memory_regions.h"
#include "objects/objects.h"
#include "scenario/scenario.h"
#include "objects/widgets/antenna.h"
#include "objects/widgets/widget_debug.h"
#include "physics/point_physics.h"
#include "render/render_sprite.h"
#include "cache/restricted_memory.h"
#include "bitmaps/bitmap_data.h"
#include "bitmaps/bitmap_group_definitions.h"
#include "tag_files/tag_types.h"
#include <stddef.h>
#include "tag_files/tag_groups.h"
#include "cache/restricted_memory_registration.h"
#include "cache/restricted_memory_regions_inline.h"
#include "multithreading/thread_tls.h"
#include "game/game_state_accessors.h"


// @address 0x11df7fc g_antenna_data
extern "C" {
__declspec(thread) s_data_array *g_antenna_data;
}

struct antenna_vertex_definition {
    real_euler_angles2d angles;
    float length;
    short sequence_index;
    real_argb_color color;
    real_argb_color lod_color;
    float spline_position;
    real_vector3d vector_to_next;
};
struct antenna_definition {
    long marker_name;
    s_tag_reference bitmap;
    s_tag_reference physics;
    float spring_strength_coefficient;
    float falloff_pixels;
    float cutoff_pixels;
    float point_of_bend;
    float starting_bend;
    float ending_bend;
    float length;
    long vertex_count;
    TAG_POINTER(antenna_vertex_definition) vertices;
    TAG_POINTER(void) vertex_definition;
};

typedef char antenna_vertex_definition_size[(sizeof(antenna_vertex_definition) == 0x40) ? 1 : -1];
typedef char antenna_definition_size[(sizeof(antenna_definition) == 0x4c) ? 1 : -1];
typedef char antenna_bitmap_index_offset[(offsetof(antenna_definition, bitmap) + offsetof(s_tag_reference, index) == 0x10) ? 1 : -1];
typedef char antenna_physics_index_offset[(offsetof(antenna_definition, physics) + offsetof(s_tag_reference, index) == 0x20) ? 1 : -1];

static __forceinline void antenna_data_make_invalid(s_data_array *array)
{
    array->valid = false;
}

void antennas_initialize(void)
{
    g_antenna_data = data_new("antenna", 12, sizeof(antenna_datum), 0, (c_allocation_base *)&g_antenna_data_allocator);
}

void antennas_initialize_for_new_map(void)
{
    data_make_valid(g_antenna_data);
}

void antennas_dispose_from_old_map(void)
{
    antenna_data_make_invalid(g_antenna_data);
}

void antennas_dispose(void)
{
    if (g_antenna_data != 0)
        g_antenna_data = 0;
}

void antenna_delete(long antenna_index)
{
    ::datum_delete(g_antenna_data, antenna_index);
}

// @address 0x3676f0 ?antenna_get_sorting_position@@YAXJPATreal_point3d@@PAUs_location@@PAJ@Z
void antenna_get_sorting_position(long antenna_index, real_point3d *position, s_location *location, long *object_index)
{
    antenna_datum *antenna = datum_get<antenna_datum>(g_antenna_data, antenna_index);
    *position = antenna->position;
    *object_index = antenna->object_index;
    object_get_location(antenna->object_index, location);
}


static __forceinline bitmap_group *antenna_get_bitmap_definition(long index)
{
    return (bitmap_group *)tag_get(NONE, index);
}
static __forceinline bitmap_data *antenna_try_get_bitmap(long bitmap_index, short index)
{
    bitmap_group *definition = antenna_get_bitmap_definition(bitmap_index);
    bitmap_data *bitmap = 0;
    if (definition != 0 && index >= 0 && index < definition->bitmaps.count)
        bitmap = &((bitmap_data *)definition->bitmaps.address)[index];
    return bitmap;
}

static __forceinline void antenna_initialize_vertex(antenna_vertex *vertex, const real_point3d *position)
{
    vertex->flags = 0;
    set_real_vector3d(&vertex->velocity, 0.0f, 0.0f, 0.0f);
    vertex->position = *position;
}

// @address 0x367558 ?antenna_new@@YAJJJPAUreal_matrix4x3@@@Z
long antenna_new(long tag_index, long object_index, real_matrix4x3 *matrix)
{
    long index = -1;
    if (tag_index != -1 && object_index != -1)
    {
        antenna_definition *definition = (antenna_definition *)tag_get(NONE, tag_index);
        index = datum_new(g_antenna_data);
        if (index != -1)
        {
            s_data_array *data = g_antenna_data;
            antenna_datum *antenna = datum_get<antenna_datum>(data, index);
            antenna->initialized = true;
            antenna->active = definition->vertex_count < 2;
            antenna->tag_index = tag_index;
            antenna->object_index = object_index;
            antenna->position.y = 0.0f;
            antenna->position.x = 0.0f;
            antenna->position.z = 0.0f;
            antenna->last_update_time = -1;
            antenna_vertex *vertex = antenna->vertices;
            antenna_initialize_vertex(vertex, &antenna->position);
            real_point3d base = antenna->position;
            vertex = &antenna->vertices[1];
            vertex->flags = 0;
            set_real_vector3d(&vertex->velocity, 0.0f, 0.0f, 0.0f);
            float length = definition->length;
            float *tip_z = &vertex->position.z;
            vertex->position.x = base.x + length * 0.0f;
            vertex->position.y = base.y + length * 0.0f;
            *tip_z = base.z + length;
        }
    }
    return index;
}


// @address 0x367b00 ?antenna_update_attachment@@YAXPAUantenna_datum@@PAUantenna_definition@@PAUs_location@@PATreal_point3d@@PATreal_vector3d@@@Z
void antenna_update_attachment(antenna_datum *datum, antenna_definition *definition,
    s_location *location, real_point3d *point, real_vector3d *vector)
{
    object_marker marker;
    internal_object_get_markers_by_string_id(datum->object_index, definition->marker_name, &marker, 1, false);
    *point = marker.matrix.position;
    *vector = marker.matrix.forward;
    scenario_location_from_point(location, &marker.matrix.position);
    real_vector3d delta;
    delta.i = point->x - datum->position.x;
    delta.j = point->y - datum->position.y;
    delta.k = point->z - datum->position.z;
    if (fabsf(delta.i) > 1.0f || fabsf(delta.j) > 1.0f || fabsf(delta.k) > 1.0f) {
        for (short i = 0; i < 2; i++) {
            antenna_vertex *vertex = &datum->vertices[i];
            vertex->position.x += delta.i;
            vertex->position.y += delta.j;
            vertex->position.z += delta.k;
        }
    }
    // match: 0x367b00: pointer __restrict qualifier prevents alias reload
    real_point3d *__restrict position = &datum->position;
    *position = *point;
}

// @address 0x367d40 antenna_compute_sprite_width_scale
extern "C" float antenna_compute_sprite_width_scale(antenna_definition *definition, long vertex_index, float width)
{
    float result = 0.0f;
    if (definition->bitmap.index != -1) {
        const short &sequence_index = definition->vertices[vertex_index].sequence_index;
        bitmap_group *bitmap_definition = antenna_get_bitmap_definition(definition->bitmap.index);
        if (sequence_index >= 0 && sequence_index < bitmap_definition->sequences.count) {
            bitmap_sequence *sequence = &((bitmap_sequence *)bitmap_definition->sequences.address)[sequence_index];
            if (sequence->sprites.count != 0) {
                bitmap_sprite *sprite = (bitmap_sprite *)sequence->sprites.address;
                bitmap_data *bitmap = antenna_try_get_bitmap(definition->bitmap.index, sprite->bitmap_index);
                if (bitmap != 0) {
                    float sprite_width = (sprite->bounds.x1 - sprite->bounds.x0) * bitmap->width
                        - bitmap_definition->sprite_spacing * 2.0f - 1.0f;
                    if (sprite_width > 0.0f)
                        result = width / sprite_width;
                }
            }
        }
    }
    return result;
}


// @address 0x3674a8 ?__tls_set_g_antenna_data_allocator@@YAXPAX@Z
void __tls_set_g_antenna_data_allocator(void *address)
{
    data_set_new_base_address(&g_antenna_data, (s_data_array *)address);
}

// @address 0x368030 ?allocate@?$t_restricted_allocation_manager@$02$0A@$0A@$1?__tls_set_g_antenna_data_allocator@@YAXPAX@Z@@UAAPAXIPBD@Z
template <>
void *t_restricted_allocation_manager<3, static_cast<void (*)(void *)>(0),
    static_cast<void (*)(void *)>(0), &__tls_set_g_antenna_data_allocator>::allocate(
    unsigned int size, const char *name)
{
    long member_index = restricted_memory_register_member(&g_restricted_regions[3], name, "", size, 0, __tls_set_g_antenna_data_allocator, 0, 0);
    data_set_new_base_address(&g_antenna_data, (s_data_array *)restricted_region_get_member_address(3, member_index));
    m_member_index = member_index;
    m_thread_id = get_current_thread_index();
    return restricted_region_get_member_address(3, m_member_index);
}


// @address 0x3680d0 ?deallocate@?$t_restricted_allocation_manager@$02$0A@$0A@$1?__tls_set_g_antenna_data_allocator@@YAXPAX@Z@@UAAXPAX@Z
template <>
void t_restricted_allocation_manager<3, static_cast<void (*)(void *)>(0),
    static_cast<void (*)(void *)>(0), &__tls_set_g_antenna_data_allocator>::deallocate(
    void *allocation)
{
    g_antenna_data = 0;
    restricted_region_free_member(3, m_member_index);
}

// match: 0x126c18: external storage preserves antennas_update code; owner: __real@3d086595
// @address 0x7c6058 antenna_update_time_step

// @address 0x126c18 antennas_update
extern "C" void antennas_update(void)
{
    long antenna_index = data_next_index(g_antenna_data, -1);
    while (antenna_index != -1)
    {
        antenna_datum *antenna = datum_get<antenna_datum>(g_antenna_data, antenna_index);
        if (!antenna->active && antenna->object_index != 0)
        {
            antenna_definition *definition = (antenna_definition *)global_tag_instances[datum_index_to_absolute_index(antenna->tag_index)].definition;
            antenna_update(antenna, definition, antenna_update_time_step);
        }
        antenna_index = inl_data_next_index(g_antenna_data, antenna_index);
    }
}


// @address 0x5bec98 ?antenna_render_debug@@YAXJJ@Z
void antenna_render_debug(long object_index, long antenna_index)
{
}

// @address 0x3676a8 antenna_render_transparent
extern "C" void antenna_render_transparent(void *user_data, unsigned short antenna_index)
{
    antenna_datum *antenna = datum_get<antenna_datum>(g_antenna_data, antenna_index);
    if (!antenna->active)
        antenna_render_proper(antenna, (antenna_definition *)tag_get(NONE, antenna->tag_index));
}

static __forceinline float antenna_spring_reach(const antenna_definition *definition)
{ return definition->length * definition->point_of_bend; }


static __forceinline real_point3d *compute_resting_tip(const real_point3d *point,
    const real_vector3d *direction, float distance, real_point3d *result)
{
    result->x = point->x + direction->i * distance;
    // match: 0x367c28: assumption anchors fmul operand assignment
    __assume(distance == distance);
    // match: 0x367c28: assumption anchors fmul operand assignment
    __assume(distance <= FLT_MAX);
    // match: 0x367c28: assumption anchors fmul operand assignment
    __assume(distance >= -FLT_MAX);
    result->y = point->y + direction->j * distance;
    // match: 0x367c28: assumption anchors fmul operand assignment
    __assume(point != 0);
    result->z = point->z + direction->k * distance;
    return result;
}
static __forceinline real_vector3d *compute_tip_displacement(const real_point3d *first, const real_point3d *second, real_vector3d *result)
{
    // match: 0x367c28: assumption anchors fmul operand assignment
    __assume(second != 0);
    // match: 0x367c28: assumption anchors fmul operand assignment
    __assume(result != 0);
    result->i = second->x - first->x;
    result->j = second->y - first->y;
    result->k = second->z - first->z;
    return result;
}

static __forceinline void antenna_copy_endpoint(const real_point3d *source, real_point3d *destination)
{
    // match: 0x367c28: long pun keeps both endpoint copies in integer regs (lwz/stw); float copy emits lfs/stfs and struct copy reschedules the prologue
    *(long *)&destination->x = *(const long *)&source->x;
    *(long *)&destination->y = *(const long *)&source->y;
    *(long *)&destination->z = *(const long *)&source->z;
}

// @address 0x367c28 antenna_compute_render_vectors
// match: 0x367c28: pointer __restrict qualifier prevents alias reload
extern "C" void antenna_compute_render_vectors(const antenna_datum *datum, real_point3d *__restrict first, real_point3d *__restrict second,
    real_vector3d *acceleration, real_vector3d *displacement)
{
    antenna_copy_endpoint(&datum->vertices[0].position, first);
    const antenna_definition *definition = (const antenna_definition *)tag_get(NONE, (datum->tag_index));
    antenna_copy_endpoint(&datum->vertices[1].position, second);
    *acceleration = datum->acceleration;
    float length = antenna_spring_reach(definition);
    real_point3d resting_tip;
    compute_resting_tip(first, acceleration, length, &resting_tip);
    compute_tip_displacement(&resting_tip, second, displacement);
    scale_vector3d(acceleration, definition->starting_bend, acceleration);
    scale_vector3d(displacement, definition->ending_bend, displacement);
}


// @address 0x367e58 ?antenna_render_proper@@YAXPAUantenna_datum@@PAUantenna_definition@@@Z
void antenna_render_proper(antenna_datum *antenna, antenna_definition *definition)
{
    if (definition->vertex_count != 0) {
        real_point3d base, tip;
        real_vector3d base_tangent, tip_tangent;
        antenna_compute_render_vectors(antenna, &base, &tip, &base_tangent, &tip_tangent);
        real_point3d previous = base;
        for (short vertex_index = 0; vertex_index < definition->vertex_count; vertex_index++) {
            antenna_vertex_definition *vertex = &definition->vertices[vertex_index];
            float t = vertex->spline_position;
            float t2 = t * t;
            float t3 = t2 * t;
            float h01 = (3.0f - 2.0f * t) * t * t;
            float h00 = 2.0f * t3 - 3.0f * t2 + 1.0f;
            float h10 = t3 - 2.0f * t2 + t;
            float h11 = t3 - t2;
            real_point3d point;
            point.x = tip.x * h01 + base.x * h00 + base_tangent.i * h10 + tip_tangent.i * h11;
            point.y = tip.y * h01 + base.y * h00 + base_tangent.j * h10 + tip_tangent.j * h11;
            point.z = tip.z * h01 + base.z * h00 + base_tangent.k * h10;
            point.z += tip_tangent.k * h11;
            real_vector3d direction;
            vector_from_points3d(&previous, &point, &direction);
            float width = antenna_compute_sprite_width_scale(definition, vertex_index, sqrtf(magnitude_squared3d(&direction)));
            if (width > 0.0f)
                build_sprite(definition->bitmap.index, 1, vertex->sequence_index, 0, &previous, &direction, 0.0f, width, 0, &vertex->color);
            previous = point;
        }
    }
}



// @address 0x367740
void antenna_update(antenna_datum *antenna, antenna_definition *definition, float delta_time)
{
    bool update = game_in_progress() && tls_game_time_globals()->time > antenna->last_update_time;
    s_location location;
    real_point3d attachment_point;
    real_vector3d attachment_vector;
    antenna_update_attachment(antenna, definition, &location, &attachment_point, &attachment_vector);
    antenna->acceleration = attachment_vector;
    if (!antenna->active && update)
    {
        real_point3d previous_point;
        real_point3d previous_endpoint;
        antenna->last_update_time = tls_game_time_globals()->time;
        float inverse_time = 1.0f / 0.0333f;
        for (long vertex_index = 0; vertex_index < 2; vertex_index++)
        {
            antenna_vertex *vertex = &antenna->vertices[vertex_index];
            float spring = definition->spring_strength_coefficient;
            vertex->flags++;
            real_point3d point;
            real_vector3d direction;
            if (vertex_index == 0)
            {
                point = attachment_point;
                direction = attachment_vector;
            }
            else
            {
                point = vertex->position;
                if (definition->physics.index != NONE)
                {
                    const point_physics_definition *physics = (const point_physics_definition *)
                        tag_get(NONE, definition->physics.index);
                    point_physics_update(0, physics, &location, -1, &point, &vertex->velocity,
                        0, 0, 0, 0.02f, 0.0333f);
                }
                real_vector3d offset;
                offset.i = point.x - previous_point.x;
                offset.j = point.y - previous_point.y;
                offset.k = point.z - previous_point.z;
                float distance = sqrtf(magnitude_squared3d(&offset));
                float scale = definition->length / distance;
                real_point3d constrained;
                point_from_line3d(&previous_point, &offset, scale, &constrained);
                point.x = (1.0f - spring) * constrained.x + spring * previous_endpoint.x;
                point.y = (1.0f - spring) * constrained.y + spring * previous_endpoint.y;
                point.z = (1.0f - spring) * constrained.z + spring * previous_endpoint.z;
                vector_from_points3d(&previous_point, &point, &direction);
            }
            real_vector3d up = *global_up3d;
            real_vector3d axis;
            cross_product3d(&up, &direction, &axis);
            if (normalize3d(&axis) == 0.0f)
                axis = *global_left3d;
            real_vector3d rotated;
            scale_vector3d(global_up3d, definition->length, &rotated);
            float angle = angle_between_vectors3d(&up, &direction);
            float rotation_cosine = (float)cos(angle);
            float rotation_sine = (float)sin(angle);
// match: 0x367740: keeps the rotation call at 0x367a68 after sine and cosine have been evaluated.
#pragma inline_depth(0)
            rotate_vector_about_axis(&rotated, &axis, rotation_sine, rotation_cosine);
// match: 0x367740: without the reset, the antenna allocator loses its vtable pointer and both -1 fields at 0x79dc98.
#pragma inline_depth()
            point_from_line3d(&point, &rotated, 1.0f, &previous_endpoint);
            previous_point = point;
            vertex->velocity.i = (point.x - vertex->position.x) * inverse_time;
            vertex->velocity.j = (point.y - vertex->position.y) * inverse_time;
            vertex->velocity.k = (point.z - vertex->position.z) * inverse_time;
            vertex->position = point;
        }
    }
}

antenna_data_allocation_manager g_antenna_data_allocator;
