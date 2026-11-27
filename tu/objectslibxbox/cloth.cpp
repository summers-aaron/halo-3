// objectslibxbox/cloth.obj: from the records 0x126ce0 0x358398 0x358518 0x3680f0 0x368108 0x368208 0x368220 0x368268 0x368280 0x368298 0x3682e8 0x3684d0 0x368540 0x368550 0x3685a0 0x368838 0x368900 0x368bf8 0x368e98 0x369298 0x369468 0x3697d8 0x369938 0x369cc0 0x369fc0 0x36a0b8 0x36a158
// @flags /TP /O2 /Oc
#include "objects/object_render_state_tls.h"
#include "devices/devices.h"
#include "multithreading/thread_tls.h"
#include "objects/cloth_tuning.h"
#include "render/render.h"
#include "memory/data_types.h"
#include "math/real_math.h"
#include "game/game_globals_storage.h"
#include "objects/object_reference_types.h"
#include "objects/object_placement.h"
#include "math/matrix_math.h"
#include "tag_files/tag_groups.h"
#include <ppcintrinsics.h>
#include <math.h>
#include <string.h>
#include "cache/restricted_memory_regions.h"
#include "objects/objects.h"
#include "objects/object_header_access.h"
#include "objects/object_markers_inline.h"
#include "objects/widgets/widgets.h"
#include "render/render_mesh.h"
#include "render/render_objects_static_lighting.h"
#include "rasterizer/rasterizer_resource_definitions.h"
#include "memory/data.h"
#include "cubemaps/cubemap_sample.h"
#include "math/unit_vector_quantization.h"
#include "geometry/geometry_sampling_types.h"
#include "math/random_math.h"
#include "render/render_objects_globals.h"
#include "render_methods/render_method.h"
#include "objects/object_datum.h"
#include "cutscene/cinematic_globals_types.h"
#include "game/player_mapping_types.h"
#include "cache/restricted_memory_regions_inline.h"
#include "objects/cloth_definition.h"
#include "render/shader_extern_info_types.h"
#include "render/render_visibility_cached_state_types.h"
#include "game/game_time_types.h"
#include "camera/observer_globals_types.h"
#include "camera/director_globals_types.h"
#include "objects/widgets/cloth.h"
#include "objects/widgets/widget_debug.h"
#include "animations/animation_definition_storage.h"
#include "math/real_math_constants.h"
#include "cache/restricted_memory_region_members.h"
#include "memory/gameplay_tls_globals.h"
#include "memory/data_typed_access.h"
#include "memory/datum_index.h"
#include "math/random_math_range.h"
#include "game/game_time_access.h"
#include "effects/decal_types.h"
// @address 0xa94d0 ?object_get_location@@YAPAUs_location@@JPAU1@@Z
// @address 0x1ec288 ?data_dispose@@YAXPAUs_data_array@@@Z
// @address 0x1ec580 ?data_delete_all@@YAXPAUs_data_array@@@Z
// @address 0x1ec830 ?data_set_new_base_address@@YAXPAPAUs_data_array@@PAU1@@Z
// @address 0x1d6bb0 ?restricted_region_free_member@@YAXJJ@Z
// @address 0x3684d0 ?cloth_get_is_transparent@@YA_NJ@Z
// @address 0x335a70 ?widget_delete_type_datum@@YAXJJ_N@Z

struct s_cloth_sim_vertex { real_point3d position; real_point3d previous_position; unsigned long normal; };
struct s_cloth_attachment {
    short vertex_index;
    short marker_index;
};
struct s_cloth_datum {
    s_datum_header header;
    long tag_index;
    long object_index;
    float field_c;
    real_point3d sorting_position;
    float speed;
    s_cloth_sim_vertex vertices[121];
    c_static_flags<120> attached_vertices;
    s_cloth_attachment attachments[6];
    short attachment_count;
    short attached_edge_indices[1210];
    unsigned char attached_edges_initialized;
    short attached_edge_count;
    long last_update;
};
typedef char cloth_layout_assert_1[(sizeof(s_cloth_datum) == 0x1704) ? 1 : -1];
typedef char cloth_layout_assert_2[(offsetof(s_cloth_datum, vertices) == 0x20) ? 1 : -1];
typedef char cloth_layout_assert_3[(offsetof(s_cloth_datum, attached_vertices) == 0xd5c) ? 1 : -1];
typedef char cloth_layout_assert_4[(offsetof(s_cloth_datum, attached_edge_indices) == 0xd86) ? 1 : -1];
typedef char cloth_layout_assert_5[(offsetof(s_cloth_datum, attached_edge_count) == 0x16fc) ? 1 : -1];
typedef char cloth_layout_assert_6[(offsetof(s_cloth_definition, collision_spheres) == 0x28) ? 1 : -1];
typedef char cloth_layout_assert_7[(offsetof(s_cloth_definition, iteration_base) == 0x36) ? 1 : -1];
typedef char cloth_layout_assert_8[(offsetof(s_cloth_definition, vertices) == 0x64) ? 1 : -1];
typedef char cloth_layout_assert_9[(offsetof(s_cloth_definition, attachment_edges) == 0x88) ? 1 : -1];

struct s_cloth_collision_sphere { long marker_name; float radius; };

namespace {
struct s_cloth_data_publisher
{
    static __forceinline void publish(void *address)
    {
        data_set_new_base_address((s_data_array **)&g_cloth_data, (s_data_array *)address);
    }
};
}

enum { k_maximum_cloth_attachments = 6 };

// @address 0x358518 ?cloth_dispose_from_old_map@@YAXXZ
void cloth_dispose_from_old_map()
{
    g_device_groups_data->valid = false;
}

// @address 0x11df740 g_cloth_data
extern "C" {
__declspec(thread) s_data_array *g_cloth_data;
}

// @address 0x368220 ?cloth_initialize@@YAXXZ
void cloth_initialize(void)
{
    g_cloth_data = (s_data_array *)data_new("cloth", 8, sizeof(s_cloth_datum), 0, (c_allocation_base *)&g_cloth_data_allocator);
}

// @address 0x368268 ?cloth_initialize_for_new_map@@YAXXZ
void cloth_initialize_for_new_map()
{
    data_make_valid((s_data_array *)g_cloth_data);
}
// @address 0x368280 ?cloth_invalidate_all@@YAXXZ
void cloth_invalidate_all()
{
    g_cloth_data->valid = false;
}

// @address 0x368298 ?cloth_dispose@@YAXXZ
void cloth_dispose()
{
    if (g_cloth_data != 0)
    {
        data_dispose((s_data_array *)g_cloth_data);
        g_cloth_data = 0;
    }
}


// @address 0x3aa90 k_real_math_origin3d

// @address 0x5bec98 ?cloth_render_debug@@YAXJJ@Z
void cloth_render_debug(long object_index, long cloth_index)
{
}

// @address 0x3682e8 ?cloth_render_opaque@@YAXJJ@Z
void cloth_render_opaque(long cloth_index, long p1)
{
    s_cloth_datum *cloth = datum_get<s_cloth_datum>(g_cloth_data, cloth_index);
    s_cloth_definition *definition = TAG_GET('clwd', s_cloth_definition, cloth->tag_index);
    long render_state = object_get_cached_render_state(cloth->object_index);
    if (render_state != -1 && definition->render_method.index != -1)
    {
        s_shader_extern_info *extern_info = &datum_get<s_visibility_cached_object_render_state>(g_cached_object_render_states,
            render_state)->shader_info;
        rasterizer_vertex_world vertices[726];
        for (long pass = 0; pass < 2; pass++)
        {
            float sign = (float)(pass == 0 ? 1 : -1);
            real_vector3d axis;
            scale_vector3d(global_left3d, sign, &axis);
            long vertex_count = definition->indices.count;
            if (vertex_count > 726)
                vertex_count = 726;
            for (long i = 0; i < vertex_count; i++)
            {
                long vertex_index = *TAG_BLOCK_GET_ELEMENT(&definition->indices, i, short);
                s_cloth_sim_vertex *sim_vertex = &cloth->vertices[vertex_index];
                vertices[i].position = sim_vertex->position;
                vertices[i].texcoord = TAG_BLOCK_GET_ELEMENT(&definition->vertices, vertex_index, s_cloth_definition_vertex)->texcoord;
                vertices[i].normal = sim_vertex->normal;
                vertices[i].tangent = rasterizer_pack_vector_dhenn3(&global_forward3d->i);
                vertices[i].binormal = rasterizer_pack_vector_dhenn3(&axis.i);
            }
            render_dynamic_mesh(vertices, (definition->grid_y_dimension * 2 - 2) * (definition->grid_x_dimension - 1),
                definition->render_method.index, pass == 0 ? 6 : 2, p1, extern_info, false);
        }
    }
}

bool cloth_get_is_transparent(long cloth_index)
{
    bool result = false;
    s_cloth_datum *cloth = datum_get<s_cloth_datum>(g_cloth_data, cloth_index);
    s_cloth_definition *definition = TAG_GET('clwd', s_cloth_definition, cloth->tag_index);
    if (definition->render_method.index != -1)
        result = c_render_method::get_is_transparent(definition->render_method.index);
    return result;
}

// @address 0x368540 ?cloth_render_transparent@@YAXPBXJ@Z
void cloth_render_transparent(const void *user_data, long cloth_index)
{
    cloth_render_opaque(cloth_index, 5);
}

// @address 0x368550 ?cloth_get_sorting_position@@YAXJPATreal_point3d@@PAUs_location@@PAJ@Z
void cloth_get_sorting_position(long cloth_index, real_point3d *position, s_location *location, long *object_index)
{
    s_cloth_datum *cloth = datum_get<s_cloth_datum>(g_cloth_data, cloth_index);
    *position = cloth->sorting_position;
    *object_index = cloth->object_index;
    object_get_location(cloth->object_index, location);
}

static __forceinline bool cloth_object_flag_test(unsigned long flags, long bit) { return (flags & (1 << bit)) != 0; }
// @address 0x368838 ?cloth_check_for_delete@@YAXJ@Z
void cloth_check_for_delete(long cloth_index)
{
    long object_index = datum_get<s_cloth_datum>(g_cloth_data, cloth_index)->object_index;
    if (object_index != -1)
    {
        s_object_header *header = datum_get<s_object_header>(object_header_data, object_index);
        object_datum *object = header->datum;
        if (header->object_type == 0 && cloth_object_flag_test(object->damage_flags, 2))
        {
            if (game_ticks_to_seconds(game_time_globals->time - object->last_motion_time) > 10.0f)
            {
                widget_delete_type_datum(object_index, cloth_index, true);
            }
        }
    }
}

static __forceinline real_point3d *cloth_vector_from_points(const real_point3d *a, const real_point3d *b, real_point3d *v)
{
    v->x = b->x - a->x;
    v->y = b->y - a->y;
    v->z = b->z - a->z;
    return v;
}
static __forceinline real_point3d *cloth_cross_product(const real_point3d *a, const real_point3d *b, real_point3d *v)
{
    float i = a->y * b->z - a->z * b->y;
    float k = a->x * b->y - a->y * b->x;
    float j = a->z * b->x - a->x * b->z;
    v->x = i;
    v->y = j;
    v->z = k;
    return v;
}
static __forceinline real_point3d *cloth_add_vectors(const real_point3d *a, const real_point3d *b, real_point3d *v)
{
    v->x = a->x + b->x;
    v->y = a->y + b->y;
    v->z = a->z + b->z;
    return v;
}
static __forceinline real_point3d *cloth_scale_vector(const real_point3d *vector, float scale, real_point3d *r)
{
    r->x = scale * vector->x;
    r->y = scale * vector->y;
    r->z = scale * vector->z;
    return r;
}
static __forceinline float cloth_normalize(real_point3d *v)
{
    float length = sqrtf(v->x * v->x + v->y * v->y + v->z * v->z);
    if (!((float)fabs(length - 0.0f) < 0.0001f))
    {
        cloth_scale_vector(v, 1.0f / length, v);
    }
    else
    {
        length = 0.0f;
    }
    return length;
}
// @address 0x368bf8 ?cloth_calculate_normals@@YAXJ@Z
void cloth_calculate_normals(long cloth_index)
{
    s_cloth_datum *cloth = datum_get<s_cloth_datum>(g_cloth_data, cloth_index);
    s_cloth_definition *definition = TAG_GET('clwd', s_cloth_definition, cloth->tag_index);
    real_point3d normals[122];
    memset(normals, 0, definition->vertices.count * sizeof(real_point3d));
    for (long i = 0; i < definition->indices.count; i += 3)
    {
        long triangle_indices[3] = {
            *TAG_BLOCK_GET_ELEMENT(&definition->indices, i, short),
            *TAG_BLOCK_GET_ELEMENT(&definition->indices, i + 1, short),
            *TAG_BLOCK_GET_ELEMENT(&definition->indices, i + 2, short)
        };
        const real_point3d *p0 = &cloth->vertices[triangle_indices[0]].position;
        const real_point3d *p1 = &cloth->vertices[triangle_indices[1]].position;
        const real_point3d *p2 = &cloth->vertices[triangle_indices[2]].position;
        real_point3d a, b, normal;
        cloth_vector_from_points(p0, p1, &a);
        cloth_vector_from_points(p0, p2, &b);
        cloth_cross_product(&a, &b, &normal);
        cloth_normalize(&normal);
        // match: 0x368bf8: dead normal.x scales hold the accumulate-loop fadds emission.
        float d1 = normal.x * (float)1;
        float d2 = normal.x * (float)2;
        float d3 = normal.x * (float)3;
        cloth_add_vectors(&normal, &normals[triangle_indices[0]], &normals[triangle_indices[0]]);
        cloth_add_vectors(&normal, &normals[triangle_indices[1]], &normals[triangle_indices[1]]);
        cloth_add_vectors(&normal, &normals[triangle_indices[2]], &normals[triangle_indices[2]]);
    }
    for (long i = 0; i < definition->vertices.count; i++)
    {
        if (cloth_normalize(&normals[i]) == 0.0f)
        {
            cloth->vertices[i].normal = rasterizer_pack_vector_dhenn3(&k_real_math_identity_matrix_storage.forward.i);
        }
        else
        {
            cloth->vertices[i].normal = rasterizer_pack_vector_dhenn3(&normals[i].x);
        }
    }
}

// @address 0x358398 ?device_groups_data_set_new_base_address@@YAXPAX@Z
void device_groups_data_set_new_base_address(void *object)
{
    data_set_new_base_address(&g_device_groups_data, (s_data_array *)object);
}
// @address 0x368208 ?cloth_delete@@YAXJ@Z
void cloth_delete(long cloth_index)
{
    datum_delete(g_cloth_data, cloth_index);
}

// @address 0x3680f0 ?__tls_set_g_cloth_data_allocator@@YAXPAX@Z
void __tls_set_g_cloth_data_allocator(void *address)
{
    data_set_new_base_address((s_data_array **)&g_cloth_data, (s_data_array *)address);
}

// @address 0x36a0b8 ?allocate@?$t_restricted_allocation_manager@$02$0A@$0A@$1?__tls_set_g_cloth_data_allocator@@YAXPAX@Z@@UAAPAXIPBD@Z
template <>
void *t_restricted_allocation_manager<3,
    static_cast<void (*)(void *)>(0),
    static_cast<void (*)(void *)>(0),
    &__tls_set_g_cloth_data_allocator>::allocate(unsigned int size, const char *name)
{
    m_member_index = restricted_region_add_member<s_cloth_data_publisher>(3, name, "", size, 0, __tls_set_g_cloth_data_allocator);
    m_thread_id = get_current_thread_index();
    return restricted_region_get_member_address(3, m_member_index);
}

// @address 0x36a158 ?deallocate@?$t_restricted_allocation_manager@$02$0A@$0A@$1?__tls_set_g_cloth_data_allocator@@YAXPAX@Z@@UAAXPAX@Z
template <>
void t_restricted_allocation_manager<3,
    static_cast<void (*)(void *)>(0),
    static_cast<void (*)(void *)>(0),
    &__tls_set_g_cloth_data_allocator>::deallocate(void *allocation)
{
    g_cloth_data = 0;
    restricted_region_free_member(3, m_member_index);
}


static __forceinline bool cloth_user_is_active(long user)
{
    bool result = false;
    if (user != -1) result = player_mapping_globals->player_indices[user] != -1;
    return result;
}
static __forceinline float cloth_distance(const real_point3d *a, const real_point3d *b)
{
    real_vector3d delta;
    vector_from_points3d(b, a, &delta);
    return sqrtf(delta.i * delta.i + delta.j * delta.j + delta.k * delta.k);
}
// @address 0x3685a0 cloth_is_update_due
extern "C" bool cloth_is_update_due(long index)
{
    long cloth_index = datum_index_to_absolute_index(index);
    s_cloth_datum *cloth = datum_get<s_cloth_datum>(g_cloth_data, index);
    s_cloth_datum *state = cloth;
    bool result = false;
    bool game_active = false;
    game_globals_storage *game = game_globals;
    if (game && game->in_progress && !game->initializing && game->active)
        game_active = true;
    if (game_active) {
        long last_update = state->last_update;
        long time = game_time_globals->time;
        if (time > last_update) {
            result = true;
            bool cinematic = false;
            if (cinematic_globals)
                cinematic = cinematic_globals->in_progress;
            if (!cinematic && state->speed == 0.0f) {
                float distance = 3.402823466e+38f;
                for (long user = 0; user < 4; ++user) {
                    if (cloth_user_is_active(user)) {
                        const s_director_storage *director = &tls_director_globals()->directors[user];
                        real_point3d camera;
                        float offset = -tls_director_globals()->directors[user].get_director()->stored_command.focus_distance;
                        camera.x = director->get_director()->stored_command.position.x + offset * director->get_director()->stored_command.forward.i;
                        camera.y = director->get_director()->stored_command.position.y + offset * director->get_director()->stored_command.forward.j;
                        camera.z = director->get_director()->stored_command.position.z + offset * director->get_director()->stored_command.forward.k;
                        float scale = observer_get_globals()->blocks[user].camera_result.field_of_view_scale;
                        if (cloth_distance(&cloth->sorting_position, &camera) * scale < distance)
                            distance = cloth_distance(&cloth->sorting_position, &camera) * scale;
                    }
                }
                if (distance > 20.0f)
                    result = cloth_index % 8 == time % 8 || time - 7 > last_update;
                else if (distance > 15.0f)
                    result = (cloth_index / 2) % 4 == time % 4 || time - 3 > last_update;
                else if (distance > 10.0f)
                    result = ((time ^ cloth_index) & 1) == 0 || time - 1 > last_update;
            }
        }
    }
    return result;
}

// @rdata 0x8202251c
static const float cloth_collision_zero = 0.0f;
// @rdata 0x8202490c
static const float cloth_collision_epsilon = 0.0001f;
static __forceinline float cloth_collision_minimum_distance_squared() { return cloth_collision_epsilon; }
// @address 0x369298 ?cloth_collide@@YAXJ@Z
void cloth_collide(long cloth_index)
{
    s_cloth_datum *cloth = datum_get<s_cloth_datum>(g_cloth_data, cloth_index);
    s_cloth_definition *definition = TAG_GET('clwd', s_cloth_definition, cloth->tag_index);
    if (definition->collision_spheres.count > 0) {
        bool valid[16];
        object_marker markers[8];
        object_marker *marker = markers;
        for (long i = 0; i < definition->collision_spheres.count; ++i, ++marker) {
            valid[i] = false;
            if (internal_object_get_markers_by_string_id(cloth->object_index,
            (*TAG_BLOCK_GET_ELEMENT(&definition->collision_spheres,
            i,
            s_cloth_collision_sphere)).marker_name,
            marker,
            1,
            false) > 0)
                valid[i] = true;
        }
        s_cloth_datum *mesh = cloth;
        for (long i = 0; i < definition->vertices.count; ++i) {
            real_point3d *position = &mesh->vertices[i].position;
            for (long j = 0; j < definition->collision_spheres.count; ++j) {
                if (valid[j]) {
                    s_cloth_collision_sphere *sphere = TAG_BLOCK_GET_ELEMENT(&definition->collision_spheres, j, s_cloth_collision_sphere);
                    const real_point3d *center = &markers[j].matrix.position;
                    float y = center->y - position->y;
                    float x = center->x - position->x;
                    float z = center->z - position->z;
                    float distance_squared = x * x + z * z + y * y;
                    if (distance_squared > cloth_collision_minimum_distance_squared()) {
                        if (distance_squared < sphere->radius * sphere->radius) {
                            float scale = sphere->radius / sqrtf(distance_squared);
                            real_vector3d delta;
                            delta.i = position->x - center->x;
                            delta.j = position->y - center->y;
                            delta.k = position->z - center->z;
                            position->x = delta.i * scale + center->x;
                            position->y = delta.j * scale + center->y;
                            position->z = delta.k * scale + center->z;
                        }
                    } else {
                        float radius = sphere->radius;
                        float offset = radius * cloth_collision_zero;
                        position->x = center->x + offset;
                        position->y = center->y + radius;
                        position->z = center->z + offset;
                    }
                }
            }
        }
    }
}

static __forceinline float cloth_enforce_distance_squared3d(const real_point3d *a, const real_point3d *b)
{
    real_vector3d vector;
    vector.j = b->y - a->y;
    vector.i = b->x - a->x;
    vector.k = b->z - a->z;
    return vector.i * vector.i + vector.j * vector.j + vector.k * vector.k;
}
static __forceinline void cloth_enforce_attach_vertex(s_cloth_datum *cloth, long vertex_index)
{
    cloth->attached_vertices.m_storage[vertex_index >> 5] |= 1 << (vertex_index & 31);
}
// @address 0x369468 ?cloth_enforce_attachments@@YAXJ@Z
void __cdecl cloth_enforce_attachments(long cloth_index)
{
    // match: 0x369468: 45 dead locals keep the attachment-edge add as add r11,r11,r8; 44 or fewer emit add r11,r8,r11.
    long il_slot_00 = cloth_index;
    long il_slot_01 = cloth_index;
    long il_slot_02 = cloth_index;
    long il_slot_03 = cloth_index;
    long il_slot_04 = cloth_index;
    long il_slot_05 = cloth_index;
    long il_slot_06 = cloth_index;
    long il_slot_07 = cloth_index;
    long il_slot_08 = cloth_index;
    long il_slot_09 = cloth_index;
    long il_slot_10 = cloth_index;
    long il_slot_11 = cloth_index;
    long il_slot_12 = cloth_index;
    long il_slot_13 = cloth_index;
    long il_slot_14 = cloth_index;
    long il_slot_15 = cloth_index;
    long il_slot_16 = cloth_index;
    long il_slot_17 = cloth_index;
    long il_slot_18 = cloth_index;
    long il_slot_19 = cloth_index;
    long il_slot_20 = cloth_index;
    long il_slot_21 = cloth_index;
    long il_slot_22 = cloth_index;
    long il_slot_23 = cloth_index;
    long il_slot_24 = cloth_index;
    long il_slot_25 = cloth_index;
    long il_slot_26 = cloth_index;
    long il_slot_27 = cloth_index;
    long il_slot_28 = cloth_index;
    long il_slot_29 = cloth_index;
    long il_slot_30 = cloth_index;
    long il_slot_31 = cloth_index;
    long il_slot_32 = cloth_index;
    long il_slot_33 = cloth_index;
    long il_slot_34 = cloth_index;
    long il_slot_35 = cloth_index;
    long il_slot_36 = cloth_index;
    long il_slot_37 = cloth_index;
    long il_slot_38 = cloth_index;
    long il_slot_39 = cloth_index;
    long il_slot_40 = cloth_index;
    long il_slot_41 = cloth_index;
    long il_slot_42 = cloth_index;
    long il_slot_43 = cloth_index;
    long il_slot_44 = cloth_index;
    s_cloth_datum *cloth = datum_get<s_cloth_datum>(g_cloth_data, cloth_index);
    s_cloth_definition *definition = TAG_GET('clwd', s_cloth_definition, cloth->tag_index);
    real_point3d center;
    float radius;
    object_get_bounding_sphere(cloth->object_index, &center, &radius);
    if (cloth_enforce_distance_squared3d(&center, &cloth->sorting_position) > 0.4f * 0.4f)
    {
        real_vector3d delta;
        vector_from_points3d(&cloth->sorting_position, &center, &delta);
        for (long i = 0; i < definition->vertices.count; i++)
        {
            point_from_line3d(&cloth->vertices[i].position, &delta, 1.0f, &cloth->vertices[i].position);
            cloth->vertices[i].previous_position = cloth->vertices[i].position;
        }
    }
    real_vector3d velocity;
    object_get_velocities(cloth->object_index, &velocity, 0);
    float speed = magnitude3d(&velocity);
    cloth->speed = speed;
    cloth->sorting_position = center;
    for (long i = 0; i < cloth->attachment_count; i++)
    {
        s_cloth_attachment *attachment = &cloth->attachments[i];
        object_marker markers[6];
        if (definition->attachment_end_marker_name && i == cloth->attachment_count - 1 &&
            object_get_markers_by_string_id(cloth->object_index, definition->attachment_end_marker_name, markers, 6) > 0)
        {
            cloth->vertices[attachment->vertex_index].position = markers[0].matrix.position;
            cloth_enforce_attach_vertex(cloth, attachment->vertex_index);
        }
        else if (object_get_markers_by_string_id(cloth->object_index, definition->attachment_marker_name, markers, 6) > 0)
        {
            cloth->vertices[attachment->vertex_index].position = markers[attachment->marker_index].matrix.position;
            cloth_enforce_attach_vertex(cloth, attachment->vertex_index);
        }
    }
    if (!cloth->attached_edges_initialized)
    {
        cloth->attached_edge_count = 0;
        for (long i = 0; i < definition->attachment_edges.count; i++)
        {
            s_cloth_attachment_edge *edge = TAG_BLOCK_GET_ELEMENT(&definition->attachment_edges, i, s_cloth_attachment_edge);
            short vertex_a = edge->vertex_index_a;
            short vertex_b = edge->vertex_index_b;
            if (cloth->attached_edge_count < 1210 &&
                (cloth->attached_vertices.test(vertex_a) || cloth->attached_vertices.test(vertex_b)))
            {
                long edge_index = cloth->attached_edge_count;
                cloth->attached_edge_count++;
                cloth->attached_edge_indices[edge_index] = (short)i;
            }
        }
        cloth->attached_edges_initialized = true;
    }
}



// @address 0x369fc0 ?cloth_reset_flag_to_default_positions@@YAXJ@Z
void __cdecl cloth_reset_flag_to_default_positions(long cloth_index)
{
    s_cloth_datum *cloth = datum_get<s_cloth_datum>(g_cloth_data, cloth_index);
    s_cloth_definition *definition = TAG_GET('clwd', s_cloth_definition, cloth->tag_index);
    for (long i = 0; i < definition->vertices.count; i++) {
        s_cloth_sim_vertex *vertex = &cloth->vertices[i];
        s_cloth_definition_vertex *vertex_definition = TAG_BLOCK_GET_ELEMENT(&definition->vertices, i, s_cloth_definition_vertex);
        vertex->position = vertex_definition->position;
        vertex->previous_position = vertex->position;
        vertex->normal = rasterizer_pack_vector_dhenn3(&global_left3d->i);
    }
    cloth->sorting_position = *global_origin3d;
    cloth_enforce_attachments(cloth_index);
}

// @address 0x368108 ?cloth_new@@YAJJJPAUreal_matrix4x3@@@Z
long __cdecl cloth_new(long tag_index, long object_index, real_matrix4x3 *)
{
    long cloth_index = -1;
    if (tag_index != -1 && object_index != -1)
    {
        cloth_index = datum_new((s_data_array *)g_cloth_data);
        if (cloth_index != -1)
        {
            s_cloth_datum *cloth = datum_get<s_cloth_datum>(g_cloth_data, cloth_index);
            cloth->tag_index = tag_index;
            cloth->field_c = 0.0f;
            cloth->object_index = object_index;
            cloth->attached_edges_initialized = 0;
            cloth->last_update = -1;
            s_cloth_datum *setup_cloth = datum_get<s_cloth_datum>(g_cloth_data, cloth_index);
            bool success = cloth_setup_attachments(cloth_index);
            if (success)
            {
                cloth_reset_flag_to_default_positions(cloth_index);
                setup_cloth->attached_vertices.clear();
                cloth_enforce_attachments(cloth_index);
                unsigned long count = 5;
                do
                {
                    cloth_constrain_positions_relaxation(cloth_index);
                } while (--count != 0);
            }
            if (!success)
            {
                datum_delete((s_data_array *)g_cloth_data, cloth_index);
                cloth_index = -1;
            }
        }
    }
    return cloth_index;
}

static __forceinline float cloth_vector_component(const real_vector3d *vector, long component)
{
    return vector->n[component];
}
static __forceinline real_vector3d *cross_product3d_indexed(const real_vector3d *a, const real_vector3d *b, real_vector3d *result)
{
    float k = a->i * b->j - a->j * b->i;
    float j = b->i * cloth_vector_component(a, 2) - a->i * cloth_vector_component(b, 2);
    result->i = a->j * cloth_vector_component(b, 2) - cloth_vector_component(a, 2) * b->j;
    result->j = j;
    result->k = k;
    return result;
}
// @address 0x3697d8 ?real_vector3d_exclude_from_pole_by_angle@@YA_NPBTreal_vector3d@@MPAT1@@Z
bool real_vector3d_exclude_from_pole_by_angle(const real_vector3d *pole, float angle, real_vector3d *vector)
{
    bool result = false;
    real_vector3d axis;
    real_vector3d direction;
    axis = *pole;
    direction = *vector;
    normalize3d(&axis);
    float length = normalize3d(&direction);
    float difference = angle_between_vectors3d(&axis, &direction);
    float minimum = (float)fabs(angle);
    if (difference < minimum)
    {
        real_vector3d rotation_axis;
        cross_product3d_indexed(&axis, &direction, &rotation_axis);
        normalize3d(&rotation_axis);
        float rotation = minimum - difference;
        float cosine = (float)cos(rotation);
        float sine = (float)sin(rotation);
        rotate_vector_about_axis(&direction, &rotation_axis, sine, cosine);
        normalize3d(&direction);
        scale_vector3d(&direction, length, vector);
        result = true;
    }
    return result;
}

// @rdata 0x82022dc0
static const float cloth_relax_half = 0.5f;
// @rdata 0x820242d8
static const float cloth_relax_two = 2.0f;
// @rdata 0x82024264
static const float cloth_relax_five = 5.0f;
// @address 0x368e98 ?cloth_constrain_positions_relaxation@@YAXJ@Z
void cloth_constrain_positions_relaxation(long cloth_index)
{
    s_cloth_datum *cloth = datum_get<s_cloth_datum>(g_cloth_data, cloth_index);
    s_cloth_definition *definition = TAG_GET('clwd', s_cloth_definition, cloth->tag_index);
    long iteration_count = (long)(cloth->speed * cloth_relax_five + (float)definition->iteration_base);
    long total_iterations = 0;
    iteration_count = PIN(iteration_count, 1, 15);
    long iteration = 0;
    if (iteration_count > 0)
    {
        do
        {
            float max_correction = cloth_collision_zero;
            cloth_enforce_attachments(cloth_index);
            long stretch_index = 0;
            if (cloth->attached_edge_count > 0)
            {
                short *pair = cloth->attached_edge_indices;
                for (; stretch_index < cloth->attached_edge_count; stretch_index++, pair++)
                {
                    s_cloth_attachment_edge *constraint = TAG_BLOCK_GET_ELEMENT(&definition->attachment_edges, *pair, s_cloth_attachment_edge);
                    s_cloth_sim_vertex *vertex_a = &cloth->vertices[constraint->vertex_index_a];
                    s_cloth_sim_vertex *vertex_b = &cloth->vertices[constraint->vertex_index_b];
                    float dy = vertex_b->position.y - vertex_a->position.y;
                    const float minimum_distance = cloth_collision_epsilon;
                    float scale = cloth_relax_half;
                    float dx = vertex_b->position.x - vertex_a->position.x;
                    float dz = vertex_b->position.z - vertex_a->position.z;
                    float distance = (float)sqrt((dx * dx + dz * dz) + dy * dy);
                    float difference = distance - constraint->rest_length;
                    if (distance <= minimum_distance)
                        distance = minimum_distance;
                    scale *= difference / distance;
                    dx *= scale;
                    dy *= scale;
                    dz *= scale;
                    if (((1 << (constraint->vertex_index_a & 31))
                            & cloth->attached_vertices.m_storage[constraint->vertex_index_a >> 5]) == 0)
                    {
                        vertex_a->position.x += dx;
                        vertex_a->position.y += dy;
                        vertex_a->position.z += dz;
                        if (((1 << (constraint->vertex_index_b & 31))
                            & cloth->attached_vertices.m_storage[constraint->vertex_index_b >> 5]) != 0)
                        {
                            vertex_a->position.x += dx;
                            vertex_a->position.y += dy;
                            vertex_a->position.z += dz;
                        }
                    }
                    if (((1 << (constraint->vertex_index_b & 31))
                            & cloth->attached_vertices.m_storage[constraint->vertex_index_b >> 5]) == 0)
                    {
                        vertex_b->position.x -= dx;
                        vertex_b->position.y -= dy;
                        vertex_b->position.z -= dz;
                        if (((1 << (constraint->vertex_index_a & 31))
                            & cloth->attached_vertices.m_storage[constraint->vertex_index_a >> 5]) != 0)
                        {
                            vertex_b->position.x -= dx;
                            vertex_b->position.y -= dy;
                            vertex_b->position.z -= dz;
                        }
                    }
                    if (fabsf(difference) > max_correction)
                        max_correction = fabsf(difference);
                }
            }
            long bending_index = 0;
            if (definition->attachment_edges.count > 0)
            {
                for (; bending_index < definition->attachment_edges.count; bending_index++)
                {
                    s_cloth_attachment_edge *constraint = TAG_BLOCK_GET_ELEMENT(&definition->attachment_edges, bending_index, s_cloth_attachment_edge);
                    short s0 = constraint->vertex_index_a;
                    float rest_length = constraint->rest_length;
                    short s1 = constraint->vertex_index_b;
                    s_cloth_sim_vertex *vertex_a = &cloth->vertices[s0];
                    s_cloth_sim_vertex *vertex_b = &cloth->vertices[s1];
                    real_vector3d delta;
                    vector_from_points3d(&vertex_a->position, &vertex_b->position, &delta);
                    float distance = magnitude3d(&delta);
                    float difference = distance - rest_length;
                    if (distance <= cloth_collision_epsilon)
                        distance = cloth_collision_epsilon;
                    float scale = difference / distance * cloth_relax_half;
                    scale_vector3d(&delta, scale, &delta);
                    vertex_a->position.x += delta.i;
                    vertex_a->position.y += delta.j;
                    vertex_a->position.z += delta.k;
                    vertex_b->position.x -= delta.i;
                    vertex_b->position.y -= delta.j;
                    vertex_b->position.z -= delta.k;
                }
            }
            if (fabsf(max_correction) > cloth_relax_two)
            {
                cloth_reset_flag_to_default_positions(cloth_index);
                iteration--;
            }
            if (total_iterations++ == 15)
                iteration = iteration_count;
            cloth_enforce_attachments(cloth_index);
            iteration++;
        } while (iteration < iteration_count);
    }
}
// @address 0x126ce0 ?cloth_update@@YAXM@Z
void cloth_update(float dt)
{
    long cloth_index = data_next_index((s_data_array *)g_cloth_data, -1);
    while (cloth_index != -1) {
        s_cloth_datum *cloth = datum_get<s_cloth_datum>(g_cloth_data, cloth_index);
        if (cloth_is_update_due(cloth_index)) {
            cloth_constrain_positions_relaxation(cloth_index);
            cloth_collide(cloth_index);
            cloth_update_positions_verlet(cloth_index);
            cloth_calculate_normals(cloth_index);
            cloth_check_for_delete(cloth_index);
            cloth->last_update = game_time_globals->time;
        }
        s_data_array *array = g_cloth_data;
        long absolute_index = data_next_absolute_index(array, datum_index_to_absolute_index(cloth_index + 1));
        long next_index = -1;
        if (absolute_index != -1)
        {
            s_datum_header *header = (s_datum_header *)datum_get_absolute(array, absolute_index);
            next_index = ((unsigned short)header->identifier << 16) | absolute_index;
        }
        cloth_index = next_index;
    }
}

// @address 0x368900 ?cloth_setup_attachments@@YA_NJ@Z
bool cloth_setup_attachments(long cloth_index)
{
    s_cloth_datum *cloth = datum_get<s_cloth_datum>(g_cloth_data, cloth_index);
    s_cloth_definition *definition = TAG_GET('clwd', s_cloth_definition, cloth->tag_index);
    long marker_indices[k_maximum_cloth_attachments] = { 0, 1, 2, 3 };
    object_marker markers[k_maximum_cloth_attachments];
    cloth->attachment_count = object_get_markers_by_string_id(
        cloth->object_index, definition->attachment_marker_name, markers, k_maximum_cloth_attachments);
    long axis = 2;
    if (definition->flags & 2)
        axis = 0;
    for (long i = 0; i < cloth->attachment_count; i++)
    {
        for (long j = i + 1; j < cloth->attachment_count; j++)
        {
            if (markers[i].matrix.position.n[axis] > markers[j].matrix.position.n[axis])
            {
                object_marker marker = markers[i];
                markers[i] = markers[j];
                markers[j] = marker;
                long marker_index = marker_indices[i];
                marker_indices[i] = marker_indices[j];
                marker_indices[j] = marker_index;
            }
        }
    }
    if (cloth->attachment_count > 0)
    {
        float spacing = (float)definition->grid_y_dimension / (float)cloth->attachment_count;
        for (long i = 0; i < cloth->attachment_count; i++)
        {
            s_cloth_attachment *attachment = &cloth->attachments[i];
            long vertex_index;
            if (i == 0)
                vertex_index = 0;
            else if (i == cloth->attachment_count - 1)
                vertex_index = (definition->grid_y_dimension - 1) * definition->grid_x_dimension;
            else
                vertex_index = (long)(ceilf(spacing * (float)i) * (float)definition->grid_x_dimension);
            attachment->marker_index = marker_indices[i];
            attachment->vertex_index = vertex_index;
        }
    }
    if (cloth->attachment_count < k_maximum_cloth_attachments && definition->attachment_end_marker_name != 0)
    {
        s_cloth_attachment *attachment = &cloth->attachments[cloth->attachment_count];
        attachment->marker_index = marker_indices[cloth->attachment_count];
        attachment->vertex_index = definition->vertices.count - 1;
        cloth->attachment_count++;
    }
    return cloth->attachment_count > 0;
}
extern "C" {
// @address 0x11db068 g_cloth_wind_direction_guard
unsigned long g_cloth_wind_direction_guard;
// @address 0x11db05c g_cloth_wind_direction
real_vector3d g_cloth_wind_direction;
// @address 0x119a470 g_cloth_wind_time
float g_cloth_wind_time;
// @address 0x119a474 g_cloth_wind_direction_valid
bool g_cloth_wind_direction_valid;
// @address 0x119a47c g_cloth_wind_last_gust_frame
long g_cloth_wind_last_gust_frame;
// @address 0x119a478 g_cloth_wind_oscillation
float g_cloth_wind_oscillation;
// @address 0x7c882c g_cloth_wind_magnitude_bias
float g_cloth_wind_magnitude_bias = 0.0075f;
}
// @rdata 0x820334a8
static const float cloth_wind_oscillation_step = 0.01f;
// @address 0x369938 ?cloth_update_wind@@YAXPATreal_point3d@@PATreal_vector3d@@_N@Z
void cloth_update_wind(real_point3d *point, real_vector3d *vector, bool update)
{
    if ((g_cloth_wind_direction_guard & 1) == 0) {
        g_cloth_wind_direction_guard |= 1;
        g_cloth_wind_direction = *global_zero_vector3d;
    }
    float wind_time = (float)(double)game_time_globals->time * (float)(double)game_time_globals->ticks_per_second;
    g_cloth_wind_time = wind_time;
    if (!g_cloth_wind_direction_valid) {
        short index = _random_range(get_local_random_seed_address(), 0, 0, 0, 0, 1026);
        g_cloth_wind_direction = global_random_directions[index];
        if (fabsf(normalize3d(&g_cloth_wind_direction)) >= cloth_collision_epsilon)
            g_cloth_wind_direction_valid = true;
    }
    if (c_render_globals::get_frame_index() > g_cloth_wind_last_gust_frame) {
        float seed_fraction = _real_random_range(get_local_random_seed_address(), 0.0f, 1.0f);
        float time05 = wind_time * 0.005f;
        double cos_a = cos((double)time05);
        float cos_af = (float)cos_a;
        double cos_b = cos((double)time05);
        short index2 = _random_range(get_local_random_seed_address(), 0, 0, 0, 0, 1026);
        float gust = (float)cos_b * cos_af * seed_fraction * 0.15f;
        real_vector3d axis;
        axis = global_random_directions[index2];
        double cos_g = cos((double)gust);
        float cos_gf = (float)cos_g;
        double sin_g = sin((double)gust);
        rotate_vector_about_axis(&g_cloth_wind_direction, &axis, (float)sin_g, cos_gf);
        real_vector3d_exclude_from_pole_by_angle(global_up3d, 0.34906584f, &g_cloth_wind_direction);
        g_cloth_wind_last_gust_frame = c_render_globals::get_frame_index();
    }
    float s1 = (float)sin((double)(float)(wind_time * 0.005));
    float c1 = (float)cos((double)(wind_time * 0.0001f));
    float c2 = (float)cos((double)(wind_time * 0.0006f));
    float c3 = (float)cos((double)(float)(wind_time * 0.001));
    float c4 = (float)cos((double)(wind_time * 0.0001f));
    float osc_sum = (c4 * c3) * c2 + (c1 * s1);
    float osc_scaled = osc_sum * cloth_wind_oscillation_step;
    float osc_delta = osc_scaled * 0.4f;
    g_cloth_wind_oscillation += osc_delta;
    g_cloth_wind_oscillation = PIN(g_cloth_wind_oscillation, -0.1f, 0.1f);
    float scale = (fabsf(g_cloth_wind_oscillation) + g_cloth_wind_magnitude_bias) * 0.080000006f;
    scale_vector3d(&g_cloth_wind_direction, scale, vector);
}

// @address 0x369cc0 ?cloth_update_positions_verlet@@YAXJ@Z
void __cdecl cloth_update_positions_verlet(long cloth_index)
{
    s_cloth_datum *cloth = datum_get<s_cloth_datum>(g_cloth_data, cloth_index);
    s_cloth_definition *definition = TAG_GET('clwd', s_cloth_definition, cloth->tag_index);
    float maximum_speed = 1.5f;
    real_vector3d wind;
    cloth_update_wind(&cloth->vertices[0].position, &wind, true);
    float wind_scale = definition->wind_scale;
    scale_vector3d(&wind, wind_scale, &wind);
    if (cloth->speed < maximum_speed)
    {
        float drag = -definition->weight * 9.78f * 0.328084f / (float)game_time_globals->ticks_per_second / 32.0f;
        real_vector3d gravity;
        memcpy(&gravity, &k_real_math_origin3d, sizeof(gravity));
        float position_scale = 2.0f - definition->drag;
        float previous_scale = 1.0f - definition->drag;
        gravity.k = drag;
        for (long vertex_index = 0; vertex_index < definition->vertices.count; vertex_index++)
        {
            s_cloth_sim_vertex *vertex = &cloth->vertices[vertex_index];
            real_vector3d vertex_wind = wind;
            if (!cloth->attached_vertices.test(vertex_index))
            {
                real_vector3d position;
                memcpy(&position, &vertex->position, sizeof(position));
                real_vector3d previous;
                memcpy(&previous, &vertex->previous_position, sizeof(previous));
                scale_vector3d(&position, position_scale, &position);
                scale_vector3d(&previous, previous_scale, &previous);
                subtract_vectors3d(&position, &previous, &position);
                add_vectors3d(&position, &gravity, &position);
                real_vector3d normal;
                ((const c_dword_unit_vector *)&vertex->normal)->get_real_vector3d(&normal);
                real_vector3d parallel;
                real_vector3d perpendicular;
                component_vectors_from_normal3d(&vertex_wind, &normal, &parallel, &perpendicular);
                scale_vector3d(&perpendicular, definition->wind_flappiness_scale, &perpendicular);
                subtract_vectors3d(&vertex_wind, &perpendicular, &vertex_wind);
                add_vectors3d(&position, &vertex_wind, &position);
                vertex->previous_position = vertex->position;
                memcpy(&vertex->position, &position, sizeof(vertex->position));
            }
        }
    }
    else
    {
        for (long vertex_index = 0; vertex_index < definition->vertices.count; vertex_index++)
        {
            s_cloth_sim_vertex *vertex = &cloth->vertices[vertex_index];
            vertex->previous_position = vertex->position;
        }
    }
}

t_cloth_data_allocator g_cloth_data_allocator;

