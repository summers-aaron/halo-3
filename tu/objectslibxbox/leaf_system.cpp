// objectslibxbox/leaf_system.obj: from the records 0x126db8 0x175230 0x36a190 0x36a350 0x36a398 0x36a3b0 0x36a3c8 0x36a418 0x36a5d8 0x36ac38 0x36ac90 0x36b100 0x36b208 0x36bb88 0x36bd10 0x36bdf8 0x36be98 0x54a090
#include "ai/ai_tls_data.h"
#include "objects/object_render_state_tls.h"
// @flags /TP /O2 /Oc
#include "objects/leaf_system_tls.h"
#include "objects/object_datum.h"
#include "objects/object_placement.h"
#include "tag_files/tag_groups.h"
#include "objects/object_reference_types.h"
#include "math/matrix_math_types.h"
#include <xtl.h>
#include "math/real_math.h"
#include "math/matrix_math.h"
#include "game/game_time_types.h"
#include "memory/data_types.h"
#include "memory/tls_data_arrays.h"
#include "cache/restricted_memory_regions.h"
#include "objects/objects.h"
#include "scenario/scenario.h"
#include "effects/decals.h"
#include "objects/widgets/cloth.h"
#include "objects/widgets/leaf_system.h"
#include "objects/widgets/widget_debug.h"
#include "render/render_objects_static_lighting.h"
#include "render_methods/render_method_submit.h"
#include "render_methods/render_method_data.h"
#include "rasterizer/rasterizer_resource_definitions.h"
#include "memory/data.h"
#include "memory/datum_storage_access.h"
#include "memory/datum_index_inlines.h"
#include "objects/object_header_access.h"
#include "cache/cache_files.h"
#include "math/unit_vector_quantization.h"
#include "math/word_quantized_position.h"
#include "geometry/geometry_sampling_types.h"
#include "geometry/geometry_sampling.h"
#include "math/random_math.h"
#include "math/random_math_range.h"
#include "bitmaps/bitmap_group.h"
#include "bitmaps/bitmap_group_definitions.h"
#include "rasterizer/rasterizer_primitives.h"
#include "render/render_objects.h"

#include "rasterizer/rasterizer_textures_cache.h"
#include "rasterizer/rasterizer.h"
#include "render_methods/render_method.h"
#include "tag_files/tag_types.h"
#include "render/shader_extern_info_types.h"
#include "cache/restricted_memory_layout.h"
#include "rasterizer/rasterizer_vertex_definitions.h"
#include "render_methods/render_method_definitions.h"

#include "math/short_quaternion.h"
#include "cache/restricted_memory_regions_inline.h"
#include "cache/restricted_memory_registration.h"
#include "rasterizer/rasterizer_main_globals.h"
#include "objects/object_render_state_tls.h"
#include "memory/tls_globals.h"
#include "cutscene/cinematic_globals_types.h"
#include "effects/decals_globals.h"
#include "visibility/visibility_collection.h"
#include "visibility/visibility_globals.h"
#include "visibility/visibility_region_types.h"
#include "effects/decal_types.h"
#include "render/render_visibility_cached_state_types.h"
#include "rasterizer/rasterizer_state_globals.h"
#include "rasterizer/rasterizer_device_states.h"
#include "multithreading/thread_tls.h"
#include "animations/animation_definition_storage.h"
#include "visibility/visibility_projections_and_volumes.h"
struct s_leaf {
    c_word_quantized_position position;
    unsigned char flags;
    unsigned char type;
    unsigned long direction;
    unsigned long axis;
    c_short_quaternion rotation;
    float speed;
    unsigned long velocity_direction;
    unsigned short age;
    unsigned short rotation_age;

    __forceinline float get_age_in_seconds() const;
    __forceinline void get_velocity(real_vector3d *velocity) const;
    __forceinline void set_velocity(const real_vector3d *velocity);
};
struct s_leaf_system_datum {
    s_datum_header header;
    long object_index;
    long tag_index;
    s_leaf leaves[64];
    real_rectangle3d bounds;
    real_point3d position;
    s_location location;
    float age;
    float lifetime;
    unsigned char lighting_sampled;
    real_vector3d lighting;
};



struct leaf_system_new_leaf_definition
{
    unsigned short sprite_index;
    float lifetime;
    float velocity_bias;
    real_bounds scale;
    float swing_scale;
    float swing_length;
    float swing_magnitude;
    float swing_frequency;
    float swing_phase;
    float tumble_scale;
    float spin_scale;
    float initial_speed;
    float horizontal_drag;
    float vertical_drag;
};
struct leaf_system_new_definition
{
    long flags;
    long marker_name;
    s_tag_reference bitmap;
    real_bounds emission_interval;
    float lifetime;
    float wind_minimum;
    float wind_maximum;
    float wind_scale;
    float time_scale;
    float fade_distance_minimum;
    float render_distance;
    float emission_radius;
    float radius;
    float fade_in_time;
    float fade_out_time;
    s_tag_block leaves;
};

typedef char leaf_datum_size[(sizeof(s_leaf_system_datum) == 0x94c) ? 1 : -1];
typedef char leaf_lighting_offset[(offsetof(s_leaf_system_datum, lighting_sampled) == 0x93c) ? 1 : -1];
typedef char leaf_definition_lifetime_offset[(offsetof(leaf_system_new_definition, lifetime) == 0x20) ? 1 : -1];
typedef char leaf_definition_distance_offset[(offsetof(leaf_system_new_definition, render_distance) == 0x38) ? 1 : -1];



// @address 0x175230 ?leaf_system_unattached_visible_render@@YAXJ@Z
void leaf_system_unattached_visible_render(long view)
{
    for (long index = ::data_next_index(g_leaf_system_data, -1); index != -1;
        index = inl_data_next_index(g_leaf_system_data, index))
    {
        s_leaf_system_datum *datum = datum_get<s_leaf_system_datum>(g_leaf_system_data, index);
        if (datum->object_index == -1 && leaf_system_visible(index))
            leaf_system_render_opaque(index, view);
    }
}

// @address 0x36a350 ?leaf_system_initialize@@YAXXZ
void leaf_system_initialize(void)
{
    g_leaf_system_data = data_new("leaf system", 32, sizeof(s_leaf_system_datum), 0, (c_allocation_base *)&g_leaf_system_data_allocator);
}

// @address 0x36a398 ?leaf_system_initialize_for_new_map@@YAXXZ
void leaf_system_initialize_for_new_map(void)
{
    data_make_valid(g_leaf_system_data);
}

// @address 0x36a3b0 ?leaf_system_dispose_from_old_map@@YAXXZ
void leaf_system_dispose_from_old_map(void)
{
    g_leaf_system_data->valid = false;
}

// @address 0x36a3c8 ?leaf_system_dispose@@YAXXZ
void leaf_system_dispose(void)
{
    s_data_array *data = g_leaf_system_data;
    if (data != 0)
    {
        data_dispose(data);
        g_leaf_system_data = 0;
    }
}

// @address 0x5bec98 ?leaf_system_delete@@YAXJ@Z
void leaf_system_delete(long)
{
}

// Empty: leaf render-debug drawing is stripped from this build.
// @address 0x5bec98 ?leaf_system_render_debug@@YAXJJ@Z
void leaf_system_render_debug(long, long)
{
}

// @address 0x42d298 ?leaf_system_get_is_transparent@@YA_NJ@Z
bool leaf_system_get_is_transparent(long)
{
    return false;
}

static __forceinline bool lsv_game_invisible()
{
    bool result = false;
    if (cinematic_globals)
        result = cinematic_globals->in_progress;
    return result;
}
static __forceinline c_visibility_collection *lsv_get_global_camera_collection()
{
    return (c_visibility_collection *)&g_visibility_globals.collection_input;
}
static __forceinline bool lsv_get_cluster_visible(c_visibility_collection *c, const s_cluster_reference *ref)
{
    unsigned char cluster = ref->cluster_index;
    const s_visibility_region_build *region = (const s_visibility_region_build *)c->get_region();
    unsigned long bit = 1UL << (cluster & 31);
    return (region->visible_cluster_bits[ref->bsp_index][cluster >> 5] & bit) != 0;
}
// @address 0x36a418 ?leaf_system_visible@@YA_NJ@Z
bool leaf_system_visible(long index)
{
    bool visible = false;
    s_leaf_system_datum *s = datum_get<s_leaf_system_datum>(g_leaf_system_data, index);
    if (!lsv_game_invisible() && s->object_index == -1 && s->location.cluster_reference.bsp_index != (signed char)-1 &&
        s->location.cluster_reference.cluster_index != -1)
    {
        c_visibility_collection *c = lsv_get_global_camera_collection();
        visible = lsv_get_cluster_visible(c, &s->location.cluster_reference);
        if (visible)
        {
            float radius = MAX(s->bounds.x1 - s->bounds.x0, MAX(s->bounds.y1 - s->bounds.y0, s->bounds.z1 - s->bounds.z0));
            leaf_system_new_definition *d = TAG_GET('lswd', leaf_system_new_definition, s->tag_index);
            float distance = distance3d(&s->position, &((const s_visibility_region_build *)c->get_region())->sphere_center) - radius;
            visible = false;
            if (distance < d->render_distance)
                visible = visibility_volume_test_sphere_planes(c->get_input()->projection.volume.vector_planes, &s->position, radius) != 0;
        }
    }
    return visible;
}

/* leaf_system_get_location: copy position, location and object index out of leaf system datum */

// @address 0x36ac38 ?leaf_system_get_location@@YAXJPATreal_point3d@@PAUs_location@@PAJ@Z
void leaf_system_get_location(long leaf_system_index, real_point3d *position, s_location *location, long *object_index)
{
    s_leaf_system_datum *datum = datum_get<s_leaf_system_datum>(g_leaf_system_data, leaf_system_index);
    *position = datum->position;
    *location = datum->location;
    *object_index = datum->object_index;
}

// @address 0x36b100 ?leaf_system_cleanup@@YAXJM@Z
void leaf_system_cleanup(long leaf_system_index, float delta)
{
    s_data_array *data = g_leaf_system_data;
    s_leaf_system_datum *datum = datum_get<s_leaf_system_datum>(data, leaf_system_index);
    leaf_system_new_definition *definition = TAG_GET('lswd', leaf_system_new_definition, datum->tag_index);
    for (long leaf_index = 0; leaf_index < 64; leaf_index++)
    {
        s_leaf *leaf = &datum->leaves[leaf_index];
        if (leaf->flags & 1)
        {
            leaf->age = (unsigned short)((float)leaf->age + (float)TLS_GAME_TIME_GLOBALS->ticks_per_second * delta * 4.0f);
            if ((float)leaf->age / 4.0f / (float)TLS_GAME_TIME_GLOBALS->ticks_per_second >= definition->lifetime)
                leaf->flags = (unsigned char)(leaf->flags & 0xfe);
        }
    }
}

// @rdata 0x8202251c 4
static const float k_leaf_lighting_zero = 0.0f;

// @address 0x36bb88 ?leaf_system_sample_lighting@@YAXJ@Z
void leaf_system_sample_lighting(long leaf_system_index)
{
    s_data_array *data = g_leaf_system_data;
    s_leaf_system_datum *datum = datum_get<s_leaf_system_datum>(data, leaf_system_index);
    if (datum->lighting_sampled)
        return;
    if (datum->object_index != -1)
    {
        long render_state_index = object_get_cached_render_state(datum->object_index);
        if (render_state_index != -1)
        {
            s_visibility_cached_object_render_state *state =
                datum_get<s_visibility_cached_object_render_state>((s_data_array *)g_cached_object_render_states, render_state_index);
            datum->lighting.i = state->shader_info.lightprobe_sample.red[0] * 0.282095f;
            datum->lighting.j = state->shader_info.lightprobe_sample.green[0] * 0.282095f;
            datum->lighting.k = state->shader_info.lightprobe_sample.blue[0] * 0.282095f;
            datum->lighting_sampled = 1;
        }
        else
        {
            datum->lighting.i = 0.5f;
            datum->lighting.j = 0.5f;
            datum->lighting.k = 0.5f;
        }
    }
    else
    {
        real_vector3d direction = *global_down3d;
        direction.i = k_leaf_lighting_zero;
        direction.j *= 20.0f;
        direction.k *= 20.0f;
        s_geometry_sample samples;
        if (geometry_sampling_sample_point_lighting(datum->position, direction, true, -1, true, false, samples) == 0)
        {
            datum->lighting.i = samples.red[0] * 0.282095f;
            datum->lighting.j = samples.green[0] * 0.282095f;
            datum->lighting.k = samples.blue[0] * 0.282095f;
            datum->lighting_sampled = 1;
        }
        else
        {
            datum->lighting.i = 0.5f;
            datum->lighting.j = 0.5f;
            datum->lighting.k = 0.5f;
        }
    }
}

// @address 0x36bd10 ?leaf_system_setup_shader@@YA_NJ@Z
bool leaf_system_setup_shader(long index)
{
    bool result = c_rasterizer::set_explicit_shaders(55, _vertex_type_world, _transfer_vector_vertex_none, _entry_point_default);
    ::c_rasterizer::set_z_buffer_mode((c_rasterizer::e_z_buffer_mode)0);
    s_leaf_system_datum *datum = datum_get<s_leaf_system_datum>(g_leaf_system_data, index);
    struct { float x, y, z, w; } color;
    color.x = 0.0f;
    color.y = 0.0f;
    color.z = 0.0f;
    color.w = 1.0f;
    if (datum->lighting_sampled != 0)
    {
        color.x = datum->lighting.i;
        color.y = datum->lighting.j;
        color.z = datum->lighting.k;
    }
    g_rasterizer_device->SetVertexShaderConstantF(229, &color.x, 1);
    return result;
}

namespace {
static __forceinline long restricted_region_add_leaf_member(const char *name, unsigned int size)
{
    long member_index = restricted_memory_register_member(&g_restricted_regions[3], name, "", size, 0,
        __tls_set_g_decal_system_allocator, 0, 0);
    void *address = restricted_region_get_member_address(3, member_index);
    data_set_new_base_address(&g_leaf_system_data, (s_data_array *)address);
    return member_index;
}
}

// @address 0x36bdf8 ?allocate@?$t_restricted_allocation_manager@$02$0A@$0A@$1?__tls_set_g_leaf_system_data_allocator@@YAXPAX@Z@@UAAPAXIPBD@Z
template<> void *t_restricted_allocation_manager<3, static_cast<void (*)(void *)>(0), static_cast<void (*)(void *)>(0),
    &__tls_set_g_leaf_system_data_allocator>::allocate(unsigned int size, const char *name)
{
    long member_index = restricted_region_add_leaf_member(name, size);
    m_member_index = member_index;
    m_thread_id = get_current_thread_index();
    return restricted_region_get_member_address(3, member_index);
}

// @address 0x36be98 ?deallocate@?$t_restricted_allocation_manager@$02$0A@$0A@$1?__tls_set_g_leaf_system_data_allocator@@YAXPAX@Z@@UAAXPAX@Z
template<> void t_restricted_allocation_manager<3, static_cast<void (*)(void *)>(0), static_cast<void (*)(void *)>(0),
    &__tls_set_g_leaf_system_data_allocator>::deallocate(void *address)
{
    g_leaf_system_data = 0;
    restricted_region_free_member(3, m_member_index);
}

// @address 0x54a090 ?__tls_set_g_leaf_system_data_allocator@@YAXPAX@Z
void __tls_set_g_leaf_system_data_allocator(void *address)
{
    data_set_new_base_address((s_data_array **)&g_ai_squad_group_data, (s_data_array *)address);
}


// @address 0x126db8 ?leaf_system_update@@YAXM@Z
void leaf_system_update(float delta)
{
    delta = MIN(MAX(delta, 0.0001f), TLS_GAME_TIME_GLOBALS->tick_length);
    for (long index = ::data_next_index(g_leaf_system_data, -1); index != -1;
        index = inl_data_next_index(g_leaf_system_data, index))
    {
        leaf_system_emit(index, delta);
        leaf_system_update_physics(index, delta);
        leaf_system_cleanup(index, delta);
    }
}

static __forceinline void leaf_system_new_get_bounding_sphere(long object_index, real_point3d *center, float *radius)
{
    object_datum *object = object_get(object_index);
    *center = object->bounding_center;
    *radius = object->bounding_radius;
}
// @address 0x36a190 ?leaf_system_new@@YAJJJPAUreal_matrix4x3@@@Z
long leaf_system_new(long definition_index, long object_index, real_matrix4x3 *matrix)
{
    long result = -1;
    if (definition_index != -1)
    {
        result = datum_new(g_leaf_system_data);
        if (result != -1)
        {
            s_leaf_system_datum *datum = datum_get<s_leaf_system_datum>(g_leaf_system_data, result);
            leaf_system_new_definition *definition = TAG_GET('lswd', leaf_system_new_definition, definition_index);
            memset(datum->leaves, 0, sizeof(datum->leaves));
            for (long index = 0; index < definition->leaves.count; ++index)
            {
                leaf_system_new_leaf_definition *leaf = TAG_BLOCK_GET_ELEMENT(&definition->leaves, index, leaf_system_new_leaf_definition);
                datum->lifetime += leaf->lifetime;
            }
            if (object_index != -1)
            {
                object_marker marker;
                if (internal_object_get_markers_by_string_id(object_index, definition->marker_name, &marker, 1, false) > 0)
                    datum->position = marker.matrix.position;
                else
                {
                    float radius;
                    leaf_system_new_get_bounding_sphere(object_index, &datum->position, &radius);
                }
            }
            else
                datum->position = matrix->position;
            float radius = definition->radius;
            datum->bounds.x0 = datum->position.x - radius;
            datum->bounds.x1 = datum->position.x + radius;
            datum->bounds.y0 = datum->position.y - radius;
            datum->bounds.y1 = datum->position.y + radius;
            datum->bounds.z0 = datum->position.z - radius;
            datum->bounds.z1 = datum->position.z + radius;
            datum->lighting_sampled = false;
            set_real_vector3d(&datum->lighting, 0.0f, 0.0f, 0.0f);
            datum->object_index = object_index;
            datum->age = 0.0f;
            datum->tag_index = definition_index;
            scenario_location_from_point(&datum->location, &datum->position);
        }
    }
    return result;
}

static __forceinline real_vector3d *leaf_system_random_direction3d(real_vector3d *direction)
{
    random_seed_next(get_local_random_seed_address());
    short index = (short)(((*get_local_random_seed_address() >> 16) * 1026) >> 16);
    *direction = global_random_directions[index];
    return direction;
}

// match: 0x36ac90: float_control pragma preserves single-precision rounding
#pragma float_control(precise, on, push)
static __forceinline float leaf_system_random_fraction()
{
    random_seed_next(get_local_random_seed_address());
    return (float)(*get_local_random_seed_address() >> 16) * (1.0f / 65535.0f);
}
// match: 0x36ac90: float_control pragma preserves single-precision rounding
#pragma float_control(pop)

static __forceinline void leaf_system_quaternion_from_half_angle_and_vector(real_quaternion *quaternion, float half_angle, const real_vector3d *vector)
{
    float sine = (float)sin(half_angle);
    quaternion->w = (float)cos(half_angle);
    quaternion->v.i = sine * vector->i;
    quaternion->v.j = sine * vector->j;
    quaternion->v.k = sine * vector->k;
}

static __forceinline float normalize3d_inline_leaf_system(real_vector3d *vector)
{
    float length = sqrtf(vector->i * vector->i + vector->j * vector->j + vector->k * vector->k);
    if (!(fabsf(length) < k_real_epsilon))
    {
        scale_vector3d(vector, k_real_one / length, vector);
        return length;
    }
    return 0.0f;
}

// @address 0x36ac90
void leaf_system_emit(long leaf_system_index, float delta)
{
    s_leaf_system_datum *datum = datum_get<s_leaf_system_datum>(g_leaf_system_data, leaf_system_index);
    leaf_system_new_definition *definition = TAG_GET('lswd', leaf_system_new_definition, datum->tag_index);
    datum->age -= delta;
    if (!(datum->age > 0.0f) && datum->lifetime > 0.0f)
    {
        for (long leaf_index = 0; leaf_index < 64; ++leaf_index)
        {
            s_leaf *leaf = &datum->leaves[leaf_index];
            if (!TEST_BIT(leaf->flags, 0))
            {
                leaf->flags |= FLAG(0);
                {
                    real_point3d position = datum->position;
                    real_vector3d offset;
                    leaf_system_random_direction3d(&offset);
                    point_from_line3d(&position, &offset, leaf_system_random_fraction() * definition->emission_radius, &position);
                    leaf->position.set_real_point3d(&datum->bounds, &position);
                }
                {
                    real_vector3d direction;
                    real_vector3d axis;
                    leaf_system_random_direction3d(&direction);
                    real_vector3d_exclude_from_pole_by_angle(&direction, 0.09817477f, &direction);
                    cross_product3d(&direction, global_up3d, &axis);
                    normalize3d(&axis);
                    leaf->axis = rasterizer_pack_vector_dhenn3(&axis.i);
                    leaf->direction = rasterizer_pack_vector_dhenn3(&direction.i);
                    real_quaternion rotation;
                    leaf_system_quaternion_from_half_angle_and_vector(&rotation, 0.0f, &direction);
                    leaf->rotation.set_real_quaternion(&rotation);
                }
                leaf->type = 0;
                leaf->age = 0;
                leaf->rotation_age = 0;
                float weight = leaf_system_random_fraction() * datum->lifetime;
                for (long type_index = 0; type_index < definition->leaves.count; ++type_index)
                {
                    leaf_system_new_leaf_definition *type = TAG_BLOCK_GET_ELEMENT(&definition->leaves, type_index, leaf_system_new_leaf_definition);
                    if (weight < type->lifetime)
                    {
                        leaf->type = (unsigned char)type_index;
                        leaf->speed = type->initial_speed;
                        break;
                    }
                    weight -= type->lifetime;
                }
                {
                    real_vector3d velocity;
                    leaf_system_random_direction3d(&velocity);
                    if (leaf->type < definition->leaves.count)
                    {
                        leaf_system_new_leaf_definition *type = TAG_BLOCK_GET_ELEMENT(&definition->leaves, leaf->type, leaf_system_new_leaf_definition);
                        if (type->velocity_bias > 0.0f)
                            velocity.k -= type->velocity_bias;
                    }
                    if (velocity.k > 0.0f)
                        velocity.k *= -1.0f;
                    normalize3d_inline_leaf_system(&velocity);
                    leaf->velocity_direction = rasterizer_pack_vector_dhenn3(&velocity.i);
                }
                datum->age = leaf_system_random_fraction() * (definition->emission_interval.upper - definition->emission_interval.lower) +
                    definition->emission_interval.lower;
                break;
            }
        }
    }
}

// @address 0x119a480 g_leaf_system_time
extern "C" {
float g_leaf_system_time;
}


static __forceinline short leaf_system_game_ticks_per_second(void)
{
    return TLS_GAME_TIME_GLOBALS->ticks_per_second;
}

__forceinline float s_leaf::get_age_in_seconds() const
{
    float age_in_seconds = (float)age / 4.0f / (float)leaf_system_game_ticks_per_second();
    return age_in_seconds;
}

__forceinline void s_leaf::get_velocity(real_vector3d *velocity) const
{
    ((const c_dword_unit_vector *)&velocity_direction)->get_real_vector3d(velocity);
    scale_vector3d(velocity, speed, velocity);
}

__forceinline void s_leaf::set_velocity(const real_vector3d *velocity)
{
    real_vector3d direction = *velocity;
    speed = normalize3d(&direction);
    velocity_direction = rasterizer_pack_vector_dhenn3(&direction.i);
}

__forceinline void c_short_quaternion::set_real_matrix4x3(const real_matrix4x3 *matrix)
{
    real_quaternion quaternion;
    matrix3x3_rotation_to_quaternion(&matrix->rotation, &quaternion);
    set_real_quaternion(&quaternion);
}

// @address 0x36b208 ?leaf_system_update_physics@@YAXJM@Z
void leaf_system_update_physics(long leaf_system_index, float delta)
{
    s_leaf_system_datum *datum = datum_get((s_leaf_system_datum *)g_leaf_system_data->data, leaf_system_index);
    leaf_system_new_definition *definition = TAG_GET('lswd', leaf_system_new_definition, datum->tag_index);
    float time_scale = definition->time_scale * delta * (float)leaf_system_game_ticks_per_second();
    g_leaf_system_time += delta * time_scale;
    delta *= time_scale;
    if (delta > 0.0f)
    {
        float wind_scale = definition->wind_scale;
        real_vector3d wind;
        cloth_update_wind(&datum->position, &wind, true);
        float wind_speed = normalize3d(&wind) * wind_scale;
        if (wind.k > 0.0f)
            wind.k *= -1.0f;
        real_vector3d wind_direction = wind;
        if (definition->wind_minimum != 0.0f || definition->wind_maximum != 0.0f)
            wind_speed = PIN(wind_speed, definition->wind_minimum, definition->wind_maximum);
        scale_vector3d(&wind, wind_speed * delta * 30.0f, &wind);
        real_vector3d_exclude_from_pole_by_angle(global_up3d, 95.0f * k_degrees_to_radians, &wind);
        float wind_minimum_speed = MAX(wind_scale * 0.0087f, 0.0001f);
        float wind_factor = PIN(wind_speed / wind_minimum_speed, 0.0f, 1.0f);
        for (long leaf_index = 0; leaf_index < 64; leaf_index++)
        {
            s_leaf *leaf = &datum->leaves[leaf_index];
            if (TEST_BIT(leaf->flags, 0))
            {
                real_vector3d displacement = *global_zero_vector3d;
                real_vector3d direction;
                ((const c_dword_unit_vector *)&leaf->direction)->get_real_vector3d(&direction);
                real_matrix4x3 matrix;
                leaf->rotation.get_real_matrix4x3(&matrix);
                leaf->position.get_real_point3d(&datum->bounds, &matrix.position);
                real_vector3d velocity;
                leaf->get_velocity(&velocity);
                real_vector3d velocity_direction = velocity;
                normalize3d(&velocity_direction);
                normals_interpolate(&direction, &wind_direction, delta, &direction);
                normalize3d(&direction);
                leaf->direction = rasterizer_pack_vector_dhenn3(&direction.i);
                if (leaf->type < definition->leaves.count)
                {
                    const float k_drag_scale = 5.0f;
                    leaf_system_new_leaf_definition *type = TAG_BLOCK_GET_ELEMENT(&definition->leaves, leaf->type, leaf_system_new_leaf_definition);
                    velocity.i = velocity.i * (1.0f - type->horizontal_drag) + k_drag_scale * wind.i * type->horizontal_drag / delta;
                    velocity.j = velocity.j * (1.0f - type->horizontal_drag) + k_drag_scale * wind.j * type->horizontal_drag / delta;
                    velocity.k = velocity.k * (1.0f - type->vertical_drag) + k_drag_scale * wind.k * type->vertical_drag / delta;

                    float swing_magnitude = type->swing_magnitude * 10.0f;
                    float swing_frequency = type->swing_frequency;
                    float swing_phase = type->swing_phase;
                    float swing = swing_magnitude * (float)sin(((float)leaf_index * swing_phase + g_leaf_system_time) * swing_frequency);
                    float swing_angle = (1.0f - wind_factor) * type->swing_scale * swing * delta;
                    swing_angle *= PIN(leaf->get_age_in_seconds() / 2.0f, 0.0f, 1.0f);

                    const float k_tumble_minimum = 0.01f;
                    const float k_tumble_floor = 0.02f;
                    const float k_tumble_scale = 5.0f;
                    const float k_tumble_range = 1.0f - k_tumble_minimum;
                    float tumble = MAX(PIN(wind_factor - k_tumble_minimum, 0.0f, k_tumble_range) / k_tumble_range, k_tumble_floor);
                    float tumble_angle = type->tumble_scale * k_tumble_scale * tumble * delta * PIN(leaf->speed, 0.0f, 1.0f);

                    if (swing_angle != 0.0f)
                    {
                        real_vector3d axis;
                        ((const c_dword_unit_vector *)&leaf->axis)->get_real_vector3d(&axis);
                        if (fabsf((axis.i * direction.i + direction.j * axis.j) + direction.k * axis.k - 1.0f) < k_real_epsilon)
                        {
                            leaf->axis = rasterizer_pack_vector_dhenn3(&global_left3d->i);
                            if (fabsf(dot_product3d(&axis, global_left3d) - 1.0f) < k_real_epsilon)
                                leaf->axis = rasterizer_pack_vector_dhenn3(&global_forward3d->i);
                        }
                        real_vector3d offset = direction;
                        float length = type->swing_length;
                        const float k_flatten_height = 0.2f;
                        bool upright = true;
                        if (offset.k < 0.0f)
                        {
                            offset.k *= -1.0f;
                            upright = false;
                        }
                        scale_vector3d(&offset, length, &offset);
                        if (offset.k < k_flatten_height)
                        {
                            float flatten = (offset.k - 0.0f) / MAX(k_flatten_height - 0.0f, k_real_epsilon);
                            swing_angle *= flatten * flatten;
                        }
                        real_point3d pivot;
                        point_from_line3d(&matrix.position, &offset, 1.0f, &pivot);
                        rotate_vector_about_axis(&offset, &axis, (float)sin(swing_angle), (float)cos(swing_angle));
                        if (fabsf(dot_product3d(&offset, global_up3d) - 1.0f) < k_real_epsilon)
                            rotate_vector_about_axis(&offset, &axis, (float)sin(swing_angle), (float)cos(swing_angle));
                        real_point3d end;
                        point_from_line3d(&pivot, &offset, -1.0f, &end);
                        real_vector3d new_displacement;
                        vector_from_points3d(&matrix.position, &end, &new_displacement);
                        vector_from_points3d(&end, &pivot, &direction);
                        normalize3d(&direction);
                        direction.k *= upright ? 1.0f : -1.0f;
                        leaf->direction = rasterizer_pack_vector_dhenn3(&direction.i);
                        displacement = new_displacement;
                        real_vector3d forward;
                        normals_interpolate(&matrix.forward, &direction, (1.0f - wind_factor) * delta, &forward);
                        real_matrix4x3 rotation;
                        matrix4x3_rotation_between_vectors(&rotation, &matrix.forward, &forward);
                        matrix4x3_compute_product(&matrix, &rotation, &matrix);
                    }
                    if (tumble_angle > 0.0f)
                    {
                        real_vector3d tumble_axis;
                        cross_product3d(global_up3d, &velocity_direction, &tumble_axis);
                        if (normalize3d(&tumble_axis) == 0.0f)
                            tumble_axis = *global_left3d;
                        real_matrix4x3 rotation;
                        matrix4x3_rotation_from_axis_and_angle(&rotation, &tumble_axis, (float)sin(tumble_angle), (float)cos(tumble_angle));
                        matrix4x3_compute_product(&matrix, &rotation, &matrix);
                    }
                    if (type->spin_scale > 0.0f)
                    {
                        const float k_spin_rate = -0.003f;
                        leaf->rotation_age -= (unsigned short)(wind_factor * delta * k_spin_rate * 65535.0f * type->spin_scale);
                        float spin_fraction = (float)leaf->rotation_age / 65535.0f;
                        float spin_angle = spin_fraction * 6.2831855f;
                        rotate_vector_about_axis(&matrix.left, &matrix.forward, (float)sin(spin_angle), (float)cos(spin_angle));
                        rotate_vector_about_axis(&matrix.up, &matrix.forward, (float)sin(spin_angle), (float)cos(spin_angle));
                    }
                    leaf->rotation.set_real_matrix4x3(&matrix);
                    velocity.k -= type->velocity_bias * delta;
                }
                add_vectors3d(&velocity, &wind, &velocity);
                leaf->set_velocity(&velocity);
                point_from_line3d(&matrix.position, &velocity, delta, &matrix.position);
                point_from_line3d(&matrix.position, &displacement, 1.0f, &matrix.position);
                leaf->position.set_real_point3d(&datum->bounds, &matrix.position);
            }
        }
    }
}


static __forceinline void leaf_system_set_sampler_filter(long sampler, long filter_mode)
{
    if (g_rasterizer_sampler_filter_modes[sampler] != filter_mode)
    {
        g_rasterizer_sampler_filter_modes[sampler] = filter_mode;
        rasterizer_device_set_sampler_filter_modes(g_rasterizer_device, sampler, filter_mode, filter_mode);
    }
}

static __forceinline void leaf_system_set_cull_mode(long cull_mode)
{
    g_rasterizer_device->SetRenderState(D3DRS_CULLMODE, cull_mode);
}

static __forceinline void leaf_system_render_dynamic_mesh_inline(void *vertices, long triangle_count, long render_method_index, long cull_mode,
    long entry_point, s_shader_extern_info *shader_extern_info, bool)
{
    bool valid = true;
    if (shader_extern_info != 0)
    {
        render_method_submit_set_extern_info(
            31 - _CountLeadingZeros(-((long)g_active_structure_bsp_mask) & ((long)g_active_structure_bsp_mask)), shader_extern_info);
        if (entry_point >= 5)
            render_objects_set_lighting_constants(&shader_extern_info->lighting, 0, entry_point);
    }
    leaf_system_set_cull_mode(cull_mode);
    if (render_method_index != -1)
        valid = render_method_submit(render_method_index, (const c_render_method *)tag_get('rm  ', render_method_index), _vertex_type_world,
            _transfer_vector_vertex_none, (e_entry_point)entry_point, 0);
    if (valid != 0)
        g_rasterizer_device->DrawPrimitiveUP(g_primitive_types[c_rasterizer_index_buffer::_primitive_type_triangle_list], triangle_count, vertices, 32);
    if (shader_extern_info != 0)
    {
        c_render_method_data::m_valid = false;
        render_method_submit_valid();
    }
}

static __forceinline float leaf_system_seeded_real_random(unsigned long *seed)
{
    random_seed_next(seed);
    float fraction = (float)(*seed >> 16) * 1.5259022e-05f;
    return fraction;
}

// @address 0x36a5d8 ?leaf_system_render_opaque@@YAXJJ@Z
void leaf_system_render_opaque(long leaf_system_index, long entry_point)
{
    if (entry_point == 5)
    {
        s_leaf_system_datum *datum = datum_get<s_leaf_system_datum>(g_leaf_system_data, leaf_system_index);
        leaf_system_new_definition *definition = TAG_GET('lswd', leaf_system_new_definition, datum->tag_index);
        long last_type = NONE;
        bool setup_shader = true;
        bool shader_valid = true;
        const float a = -1.0f;
        const float b = -0.33999997f;
        const float c = 0.32000005f;
        const float d = 1.0f;
        real_point3d points[12] = {
            { 0.0f, b, a }, { 0.0f, c, a },
            { 0.0f, a, b }, { 0.0f, b, b }, { 0.0f, c, b }, { 0.0f, d, b },
            { 0.0f, a, c }, { 0.0f, b, c }, { 0.0f, c, c }, { 0.0f, d, c },
            { 0.0f, b, d }, { 0.0f, c, d },
        };
        long triangles[14][3] = {
            { 0, 2, 3 }, { 3, 1, 0 }, { 3, 4, 1 }, { 1, 4, 5 },
            { 2, 6, 3 }, { 3, 6, 7 }, { 7, 8, 3 }, { 3, 8, 4 }, { 4, 8, 5 }, { 5, 8, 9 },
            { 6, 10, 7 }, { 7, 10, 8 }, { 8, 10, 11 }, { 11, 9, 8 },
        };
        leaf_system_sample_lighting(leaf_system_index);
        for (long leaf_index = 0; leaf_index < 64; leaf_index++)
        {
            s_leaf *leaf = &datum->leaves[leaf_index];
            if (TEST_BIT(leaf->flags, 0))
            {
                real_rectangle2d texcoord_bounds;
                texcoord_bounds.x0 = 0.0f;
                texcoord_bounds.x1 = 1.0f;
                texcoord_bounds.y0 = 0.0f;
                texcoord_bounds.y1 = 1.0f;
                unsigned long seed = leaf_index * 0x240b4b;
                float scale = 0.02f;
                real_matrix4x3 matrix;
                leaf->rotation.get_real_matrix4x3(&matrix);
                leaf->position.get_real_point3d(&datum->bounds, &matrix.position);
                if (setup_shader)
                {
                    setup_shader = false;
                    shader_valid = leaf_system_setup_shader(leaf_system_index);
                }
                if (shader_valid)
                {
                    long sprite_index = 0;
                    long bitmap_index = 0;
                    if (leaf->type < definition->leaves.count)
                    {
                        leaf_system_new_leaf_definition *type = TAG_BLOCK_GET_ELEMENT(&definition->leaves, leaf->type, leaf_system_new_leaf_definition);
                        sprite_index = type->sprite_index;
                        if (type->scale.lower != 0.0f && type->scale.upper != 0.0f)
                            scale = type->scale.lower + leaf_system_seeded_real_random(&seed) * (type->scale.upper - type->scale.lower);
                    }
                    if (definition->bitmap.index != NONE)
                    {
                        bitmap_group *bitmap = TAG_GET('bitm', bitmap_group, definition->bitmap.index);
                        if (bitmap->bitmaps.count > 0)
                        {
                            if (bitmap->sequences.count > 0)
                            {
                                bitmap_sequence *sequence = TAG_BLOCK_GET_ELEMENT(&bitmap->sequences, 0, bitmap_sequence);
                                if (VALID_INDEX(sprite_index, sequence->sprites.count))
                                {
                                    bitmap_sprite *sprite = TAG_BLOCK_GET_ELEMENT(&sequence->sprites, sprite_index, bitmap_sprite);
                                    bitmap_index = sprite->bitmap_index;
                                    texcoord_bounds = sprite->bounds;
                                }
                            }
                            if (last_type != leaf->type)
                            {
                                last_type = leaf->type;
                                c_rasterizer_texture_ref texture;
                                const bitmap_data *bitmap_entry;
                                bitmap_group_get_xenon_hardware_format(definition->bitmap.index,
                                    MIN(MAX(bitmap_index, 0), bitmap->bitmaps.count), &texture, &bitmap_entry, NULL);
                                c_rasterizer::set_sampler_texture(0, texture);
                                c_rasterizer::set_sampler_address_mode(0, c_rasterizer::_sampler_address_clamp,
                                    c_rasterizer::_sampler_address_clamp, c_rasterizer::_sampler_address_clamp);
                                leaf_system_set_sampler_filter(0, 2);
                            }
                        }
                    }
                }
                if (shader_valid)
                {
                    rasterizer_vertex_world vertices[14 * 3];
                    long vertex_count = 0;
                    for (long triangle_index = 0; triangle_index < 14; triangle_index++)
                    {
                        for (long vertex_index = 0; vertex_index < 3; vertex_index++)
                        {
                            long point_index = triangles[triangle_index][vertex_index];
                            vertices[vertex_count].texcoord.x =
                                (points[point_index].y + 1.0f) * (texcoord_bounds.x1 - texcoord_bounds.x0) * 0.5f + texcoord_bounds.x0;
                            vertices[vertex_count].texcoord.y =
                                (points[point_index].z + 1.0f) * (texcoord_bounds.y1 - texcoord_bounds.y0) * 0.5f + texcoord_bounds.y0;
                            vertex_count++;
                        }
                    }
                    real_point3d transformed_points[12];
                    for (long point_index = 0; point_index < 12; point_index++)
                    {
                        const real_point3d *point = &points[point_index];
                        real_point3d *transformed_point = &transformed_points[point_index];
                        transformed_point->x = scale * point->x;
                        transformed_point->y = scale * point->y;
                        transformed_point->z = scale * point->z;
                        transformed_points[point_index].x += (leaf_system_seeded_real_random(&seed) * 2.0f - 1.0f) * scale * 0.15f;
                    }
                    matrix4x3_transform_points(&matrix, 12, transformed_points, transformed_points);
                    vertex_count = 0;
                    for (long triangle_index = 0; triangle_index < 14; triangle_index++)
                    {
                        for (long vertex_index = 0; vertex_index < 3; vertex_index++)
                        {
                            vertices[vertex_count].position = transformed_points[triangles[triangle_index][vertex_index]];
                            vertex_count++;
                        }
                    }
                    leaf_system_render_dynamic_mesh_inline(vertices, 14, NONE, 0, entry_point, NULL, false);
                }
            }
        }
    }
}

typedef t_restricted_allocation_manager<3, static_cast<void (*)(void *)>(0), static_cast<void (*)(void *)>(0),
    &__tls_set_g_leaf_system_data_allocator> t_leaf_system_data_allocator;
t_leaf_system_data_allocator g_leaf_system_data_allocator;

// @address 0x11df77c g_leaf_system_data
extern "C" {
__declspec(thread) s_data_array *g_leaf_system_data;
}
