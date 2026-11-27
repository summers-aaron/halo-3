// blamlibxbox/particle_emitter.obj: from the records 0x1432e0 0x143780 0x143e98 0x1440c8 0x145358 0x14a410 0x14a9f8 0x180d90 0x295b08 0x3004c0 0x3004d8 0x709028 0x70c128
// @compiler xdk6130
// @flags /TP /O2 /Oc
#include "math/real_math_constant_pool.h"
#include "cseries/global_real_argb_colors.h"
#include "effects/particle_emitter_allocator.h"
#include "effects/effect_lightprobe_memory.h"
#include "effects/effect_lightprobe_slot_data.h"
#include "cache/restricted_memory_registration.h"
#include "effects/particle_movement_internals.h"
#include "effects/particle_location_runtime_types.h"
#include "cutscene/cinematic_globals_types.h"
#include "effects/particle_array_globals.h"
#include "cache/restricted_memory_regions.h"
#include "effects/effects.h"
#include "physics/collision_test_vector.h"
#include "math/matrix_math_types.h"
#include "physics/collision_types.h"
#include <string.h>
#include "math/real_math.h"
#include <math.h>
#include <new>
#include "cseries/cseries.h"
#include "cseries/crt_xdk.h"
#include "objects/objects.h"
#include "units/units.h"
#include "camera/following_camera.h"
#include "gpu_particle/particle_block.h"
#include "effects/particle_emitter.h"
#include "effects/particle_system.h"
#include "memory/data.h"
#include "objects/object_datum.h"
#include "math/matrix_math.h"
#include "effects/particle_emitter_gpu_array_types.h"
#include "cache/restricted_memory.h"
#include "math/real_math_types.h"
#include "units/unit_camera.h"
#include "memory/data_types.h"
#include "effects/effect_data.h"
#include "memory/tls_data_arrays.h"
#include "tag_files/tag_types.h"
#include "tag_files/tag_groups.h"
#include "cache/restricted_memory_layout.h"
#include "effects/particle_system_definitions.h"
#include "geometry/geometry_sampling_types.h"
#include "geometry/geometry_sampling.h"
#include "effects/effect_budget_limits.h"
#include "math/random_math.h"
#include "math/real_math_constants.h"
#include "effects/editable_property.h"
#include "effects/particle.h"
#include "units/unit_types.h"
#include "cache/restricted_memory_regions_inline.h"
#include "units/unit_definitions.h"
#include "render/views/player_view_types.h"
#include "effects/particle_random.h"
#include "effects/particle_definitions.h"
#include "effects/effect_datum.h"
#include "effects/particle_states.h"
#include "effects/particle_emitter_definitions.h"
#include "effects/particle_gpu_queue.h"
#include "math/real_math_interpolate.h"
#include "memory/datum_index.h"
#include "math/random_math_range.h"
#include "objects/object_header_access.h"
// @address 0x32938 k_real_constants_minus_one
// @address 0x221fc k_real_constant_pool
// @address 0x8c46dc ?x_current_player_view@c_player_view@@1PAV1@A
// @address 0x143e98 ?interpolate@?$c_editable_property_type_base@Treal_vector3d@@@@IBA?BTreal_vector3d@@M@Z
// @address 0xaae70 ?datum_new@@YAJPAUs_data_array@@@Z
// @address 0x1ec830 ?data_set_new_base_address@@YAXPAPAUs_data_array@@PAU1@@Z
// @address 0xac748 ?object_try_and_get_and_verify_type@@YAPAXJK@Z

// @address 0x8c43a8 g_effect_budget_limits
// @address 0x2251c k_real_zero_storage

// @address 0x119a460 g_particle_emitter_type_created
bool g_particle_emitter_type_created[4];

// @address 0x11df7ac g_particle_emitter_data
__declspec(thread) s_data_array *g_particle_emitter_data;





// @address 0x81358 sin






const real_vector3d c_editable_property_type_base<real_vector3d>::interpolate(float t) const
{
    float magnitudes = magnitude3d(&m_start) * magnitude3d(&m_end);
    if (magnitudes == 0.0f)
        magnitudes = 1.0f;
    float cosine = dot_product3d(&m_start, &m_end) / magnitudes;
    if (cosine >= 0.9999f)
        cosine = cosine * 0.9999f;
    float sine = sqrtf(1.0f - cosine * cosine);
    float angle = (float)acos(cosine);
    float scale_start = (float)(sin((1.0 - t) * angle) / sine);
    float scale_end = (float)sin(t * angle) / sine;
    real_vector3d result;
    result.i = m_start.i * scale_start + m_end.i * scale_end;
    result.j = m_start.j * scale_start + m_end.j * scale_end;
    result.k = m_start.k * scale_start + m_end.k * scale_end;
    return result;
}



// @address 0x14a9f8 particle_emitter_clear_particles
extern "C" unsigned short particle_emitter_clear_particles(c_particle_emitter *emitter)
{
    long particle_index = emitter->m_particle_index;
    while (particle_index != -1)
    {
        long next_index = ((c_particle *)g_particle_data->data)[datum_index_to_absolute_index(particle_index)].m_next_index;
        ::datum_delete(g_particle_data, particle_index);
        particle_index = next_index;
    }
    emitter->m_count = 0;
    emitter->m_particle_index = -1;
    s_particle_emitter_gpu_block *gpu =
        &((s_particle_emitter_gpu_block *)g_particle_gpu_block_data->data)[datum_index_to_absolute_index(emitter->m_gpu_index)];
    unsigned short result = gpu->count;
    particle_block_delete_element(gpu->block_index, true);
    gpu->block_index = -1;
    gpu->count = 0;
    return result;
}






static __forceinline bool flag_mask_test(unsigned long flags, long bit)
{
    bool result = (flags & (1 << bit)) != 0;
    return result;
}

static __forceinline const s_unit_camera *particle_emitter_unit_camera(long index)
{
    s_unit_datum *unit = (s_unit_datum *)object_get(index);
    const s_unit_camera *result = 0;
    if (unit->object.parent_object_index != -1)
    {
        s_object_header *parent_header = &((s_object_header *)object_header_data->data)[datum_index_to_absolute_index(unit->object.parent_object_index)];
        if ((1 << parent_header->object_type) & 0x1003)
        {
            s_unit_datum *parent = (s_unit_datum *)parent_header->datum;
            unit_definition *definition = (unit_definition *)tag_get(0, parent->object.definition_index);
            unit_seat *seat = &definition->seats[unit->unit.parent_seat_index];
            if (flag_mask_test(seat->flags, 2) || flag_mask_test(seat->flags, 0) || flag_mask_test(seat->flags, 4))
                result = &seat->camera;
        }
    }
    if (!result)
        result = &((unit_definition *)tag_get(0, unit->object.definition_index))->camera;
    return result;
}

static __forceinline bool particle_emitter_tight_camera_track(long index)
{
    bool result = false;
    s_unit_datum *unit = (s_unit_datum *)object_try_and_get_and_verify_type(index, 0x1003);
    if (unit)
        result = (unit->unit.unit_flags_words[BIG_ENDIAN_VIEW_INDEX(0, 2)] & 1) != 0;
    return result;
}

static __forceinline float particle_emitter_arcsine(float value, float upper)
{
    float lower = k_real_constants_minus_one[0];
    float clamped = lower;
    if (value > lower)
    {
        if (value < upper)
            clamped = value;
        else
            clamped = upper;
    }
    return (float)asin((double)clamped);
}

static __forceinline float normalize2d_unless_degenerate(real_vector2d *v, float one)
{
    float length = sqrtf(v->i * v->i + v->j * v->j);
#if defined(_XBOX) && !defined(__clang__)
    // match: 0x295b08: 1.0f and 0.0001f load off one 0x820221fc GPR base (literal: r29/r31 swap, a second lis)
    if (fabsf(length) < k_real_constant_pool[2500])
#else
    if (fabsf(length) < 0.0001f)
#endif
        length = 0.0f;
    else
    {
        float scale = one / length;
        v->i *= scale;
        v->j *= scale;
    }
    return length;
}

// @address 0x295b08 ?following_camera_deterministic@@YAXJPATreal_point3d@@PATreal_vector3d@@@Z
void following_camera_deterministic(long index, real_point3d *point, real_vector3d *direction)
{
    real_point3d *position = point;
    s_unit_datum *unit = (s_unit_datum *)object_get(index);
    const s_unit_camera *camera = particle_emitter_unit_camera(index);
    ::unit_get_camera_position(index, position);
    *direction = unit->unit.aiming_vector;
    bool tight = particle_emitter_tight_camera_track(index);
    float one = k_real_constant_pool[0];
    float t = compute_t_from_pitch(camera, particle_emitter_arcsine(direction->k, one), tight);
    real_vector3d track_direction, track_offset;
    camera_track_splut(camera, t, &track_direction, &track_offset, tight);
    real_vector2d horizontal = { direction->i, direction->j };
    normalize2d_unless_degenerate(&horizontal, one);
    position->i += track_offset.i * horizontal.i - track_offset.j * horizontal.j;
    position->j += track_offset.i * horizontal.j + track_offset.j * horizontal.i;
    position->k += track_offset.k;
}




// @address 0x3004c0 ?particle_emitter_set_array_address@@YAXPAX@Z
void particle_emitter_set_array_address(void *object)
{
    data_set_new_base_address((s_data_array **)&g_particle_emitter_data, (s_data_array *)object);
}




// @address 0x3004d8 ?allocate@?$t_restricted_allocation_manager@$01$0A@$0A@$1?particle_emitter_set_array_address@@YAXPAX@Z@@UAAPAXIPBD@Z
template<> void *t_restricted_allocation_manager<2,
    static_cast<void (*)(void *)>(0), static_cast<void (*)(void *)>(0), &particle_emitter_set_array_address>::allocate(
    unsigned int size, const char *name)
{
    int index = restricted_memory_register_member(&g_restricted_regions[2], name, "", size, 0, particle_emitter_set_array_address, 0, 0);
    data_set_new_base_address((s_data_array **)&g_particle_emitter_data, (s_data_array *)restricted_region_get_member_address(2, index));
    m_member_index = index;
    m_thread_id = g_registered_thread_index;
    return restricted_region_get_member_address(2, index);
}
static __forceinline c_particle_emitter *particle_emitter_get(long index)
{
    return &((c_particle_emitter *)g_particle_emitter_data->data)[datum_index_to_absolute_index(index)];
}

static __forceinline s_particle_emitter_gpu_block *particle_emitter_gpu_get(long index)
{
    return &((s_particle_emitter_gpu_block *)g_particle_gpu_block_data->data)[datum_index_to_absolute_index(index)];
}



// @address 0x1432e0 particle_emitter_new
extern "C" long particle_emitter_new(unsigned char type, long location_index, long unit_index)
{
    long result = -1;
    if (g_particle_emitter_data->actual_count + 1 <= g_effect_budget_limits->table->blocks[23].limits[type].maximum_count)
    {
        long index = datum_new((s_data_array *)g_particle_emitter_data);
        c_particle_emitter *emitter = particle_emitter_get(index);
        emitter->m_accumulated_count = k_real_zero_storage;
        emitter->m_flags = 0;
        emitter->m_next_emitter = -1;
        emitter->m_count = 0;
        emitter->m_particle_index = -1;
        emitter->m_location_index = location_index;
        emitter->m_definition_index = unit_index;
        long source = ((c_particle_location *)g_particle_location_data_array->data)[datum_index_to_absolute_index(location_index)].system_index;
        long gpu_index = datum_new(g_particle_gpu_block_data);
        s_particle_emitter_gpu_block *gpu = particle_emitter_gpu_get(gpu_index);
        gpu->source = source;
        gpu->unit_index = unit_index;
        gpu->emitter_index = index;
        gpu->block_index = -1;
        gpu->count = 0;
        emitter->m_gpu_index = gpu_index;
        memset(&emitter->m_property_values, 255, sizeof(emitter->m_property_values));
        g_particle_emitter_type_created[type] = true;
        result = index;
    }
    else
    {
        g_particle_emitter_type_created[type] = false;
    }
    return result;
}




class c_particle_state_list;
class c_particle_emitter_definition;
static __forceinline real_point3d *sum_points3d(const real_point3d *point, const real_point3d *offset, real_point3d *result)
{
    result->x = point->x + offset->x;
    result->y = point->y + offset->y;
    result->z = point->z + offset->z;
    return result;
}

// @address 0x180d90 ?get_position_world@c_particle_emitter@@QBAPBTreal_point3d@@PAT2@PBVc_particle_system@@PBVc_particle_system_definition@@@Z
const real_point3d *c_particle_emitter::get_position_world(
    real_point3d *point, const c_particle_system *particle_system, const c_particle_system_definition *particle_system_definition) const
{
    const real_point3d *emitter_position = &m_matrix.position;
    long location_index = m_location_index & 0xffff;
    if ((((c_particle_location *)g_particle_location_data_array->data)[location_index].state & 1) != 0
        && particle_system_definition->m_coordinate_system != 0) {
        *point = *emitter_position;
        real_matrix4x3 camera;
        c_player_view::get_player_render_camera_orientation(&camera);
        matrix4x3_transform_point(&camera, point, point);
        return point;
    } else if ((bool)((particle_system->m_flags >> 7) & 1)) {
        *point = m_matrix.position;
        sum_points3d(point, &c_player_view::get_current()->rasterizer_camera.position, point);
        return point;
    } else {
        return emitter_position;
    }
}




class c_particle_state_list;


// @address 0xb34 k_random_word_normalization

static const real_rgb_color *const global_real_rgb_white = &global_real_argb_colors[_real_argb_white].rgb;
static const bool debug_effects_lightprobe_sampling = false;

static __forceinline const s_geometry_sample *particle_lightprobe_geometry_sample(long lightprobe_index)
{
    const s_geometry_sample *sample = debug_effects_lightprobe_sampling ? 0 : &c_geometry_sampler::m_default_lightprobe_sample;
    if (lightprobe_index != -1) {
        const s_effect_lightprobe *lightprobe = &g_effect_lightprobe_memory[datum_index_to_absolute_index(lightprobe_index)];
        const s_geometry_sample *lightprobe_sample = debug_effects_lightprobe_sampling ? 0 : &lightprobe->sample;
        if (lightprobe->flags.test(0))
            sample = lightprobe->flags.test(1) ? &lightprobe->sample : lightprobe_sample;
    }
    return sample;
}
static __forceinline const real_rgb_color *effect_get_diffuse_tint(long lightprobe_index)
{
    return (const real_rgb_color *)&particle_lightprobe_geometry_sample(lightprobe_index)->diffuse;
}


__forceinline void c_particle_definition::evaluate_animation_rate_if_not_constant_over_time(const c_particle_state_list *state_list, float *value) const
{
    if (!c_editable_property_base::property_value_is_initialized(value) || !m_animation_rate.is_constant_over_time())
        *value = m_animation_rate.evaluate(state_list);
}

// @address 0x145358 ?initialize_particle@c_particle_definition@@QBAXPBVc_particle_state_list@@PAVc_particle@@J@Z
void c_particle_definition::initialize_particle(const c_particle_state_list *state_list, c_particle *particle, long lightprobe_index) const
{
    evaluate_animation_rate_if_not_constant_over_time(state_list, &particle->m_property_values.m_animation_rate);
    if ((flags2 & 4) != 0)
        particle->m_random_offset = particle_random_range(get_local_random_seed_address(), 0.0f, 1.0f);
    particle->m_color.alpha = 0.0f;
    particle->m_color.rgb = global_real_argb_colors[_real_argb_white].rgb;
    if (get_particle_tint_from_lightmap()) {
        real_argb_color tint = effect_get_lightmap_tint(lightprobe_index);
        particle->m_color.rgb.red *= tint.rgb.red;
        particle->m_color.rgb.green *= tint.rgb.green;
        particle->m_color.rgb.blue *= tint.rgb.blue;
        particle->m_color.alpha += tint.alpha;
    }
    if (get_particle_tint_from_diffuse_texture()) {
        const real_rgb_color *diffuse_tint = effect_get_diffuse_tint(lightprobe_index);
        particle->m_color.rgb.red *= diffuse_tint->red;
        particle->m_color.rgb.green *= diffuse_tint->green;
        particle->m_color.rgb.blue *= diffuse_tint->blue;
    }
}




// @address 0x11db308 g_particle_emitter_gpu_scratch_particle
c_particle g_particle_emitter_gpu_scratch_particle;
static __forceinline float particle_emitter_spawn_random_range(unsigned long *seed, float low, float high)
{
    random_seed_next(seed);
    return low + (float)(*seed >> 16) * k_random_word_normalization * (high - low);
}
static __forceinline real_vector3d *particle_emitter_spawn_set_vector(real_vector3d *v, float i, float j, float k)
{
    v->i = i;
    v->j = j;
    v->k = k;
    return v;
}
static __forceinline real_point3d *particle_emitter_spawn_point_from_line(const real_point3d *point, const real_vector3d *vector, float t, real_point3d *result)
{
    result->x = point->x + vector->i * t;
    result->y = point->y + vector->j * t;
    result->z = point->z + vector->k * t;
    return result;
}
static __forceinline void particle_emitter_initialize_particle_flags(unsigned short *flags, unsigned long *gpu_index)
{
    unsigned short old_flags = *flags;
    *gpu_index = -1;
    *flags = (old_flags & 7) | 0x20;
}
// match: 0x1440c8: __restrict issues both position loads before the stores (lwz r10/r9, stw/stw); plain pointers serialize the copy through r11
static __forceinline void particle_emitter_copy_spawn_position(const real_point3d *__restrict source, real_point3d *__restrict destination)
{
    *destination = *source;
}
// @address 0x1440c8 ?spawn_particle@c_particle_emitter@@AAAXPAVc_particle_state_list@@PAVc_particle_system@@PBVc_particle_emitter_definition@@ABTreal_point3d@@MMM@Z
void c_particle_emitter::spawn_particle(class c_particle_state_list *state_list, class c_particle_system *system,
    class c_particle_emitter_definition const *definition, union real_point3d const &point, float t, float scale, float velocity_scale)
{
    // match: 0x1440c8: value range assumption guides branch lowering
    __assume(state_list != 0);
    // match: 0x1440c8: value range assumption guides branch lowering
    __assume(system != 0);
    // match: 0x1440c8: value range assumption guides branch lowering
    __assume(definition != 0);
    c_particle_system *particle_system = system;
    c_particle_emitter *emitter = this;
    int type = particle_system->m_type;
    c_particle *particle;

    if (definition->is_cpu())
    {
        if ((unsigned long)(g_particle_data->actual_count + 1)
            > (unsigned long)g_effect_budget_limits->table->blocks[24].limits[type & 0xff].maximum_count)
            return;
        long index = datum_new(g_particle_data);
        if (index == -1)
            return;
        particle = &((c_particle *)g_particle_data->data)[datum_index_to_absolute_index(index)];
        s_data_array *emitters = (s_data_array *)g_particle_emitter_data;
        particle->m_emitter_index = (long)(emitter - ((c_particle_emitter *)emitters->data)) | ((long)emitter->m_identifier << 16);
        unsigned short count = emitter->m_count;
        long next_index = emitter->m_particle_index;
        emitter->m_count = count + 1;
        particle->m_next_index = next_index;
        emitter->m_particle_index = index;
    }
    else
    {
        if (!definition->is_gpu())
            return;
        particle = &g_particle_emitter_gpu_scratch_particle;
    }

    long tag_index = particle_system->m_tag_index;
    s_effect_definition *tag_definition = (s_effect_definition *)tag_get(0, tag_index);
    c_particle_system_definition *system_definition =
        &((c_particle_system_definition *)
            ((s_particle_movement_effect_event *)tag_definition->events.address)[particle_system->m_def_index].particle_systems.address)
            [particle_system->m_sub_index];

    memset(&particle->m_property_values, 255, sizeof(particle->m_property_values));
    particle_emitter_initialize_particle_flags(&particle->m_flags, &particle->m_gpu_index);
    unsigned long *global_seed = get_local_random_seed_address();
    random_seed_next(global_seed);
    particle->m_age = 0.0f;
    particle->m_flags &= 0xfff8;

    unsigned short *random_state = particle->m_random_seed;
    unsigned long random_count = 8;
    do
    {
        *random_state++ = (unsigned short)(long)(particle_emitter_spawn_random_range(&particle_system->m_random_seed, 0.0f, 1.0f) * 65535.0f);
    } while (--random_count != 0);

    particle->m_correlation_1 = particle_system->m_age;
    state_list->set_particle((c_particle *)particle);

    definition->initialize_particle(state_list, (c_particle *)particle, this, system);

    particle->m_position.x *= velocity_scale;
    particle->m_position.y *= velocity_scale;
    particle->m_position.z *= velocity_scale;
    particle->m_age = particle->m_inverse_lifespan * t * scale + particle->m_age;

    if (system_definition->m_coordinate_system == 0)
    {
        real_point3d interpolated_point;
        real_math_interpolate_points(&point, &m_matrix.position, t, &interpolated_point);

        matrix3x3_transform_vector(&emitter->m_matrix.rotation, &particle->m_position_vector, &particle->m_position_vector);
        particle->m_position.x = interpolated_point.x + particle->m_position.x;
        particle->m_position.y = interpolated_point.y + particle->m_position.y;
        particle->m_position.z = interpolated_point.z + particle->m_position.z;
        matrix3x3_transform_vector(&emitter->m_matrix.rotation, &particle->m_velocity, &particle->m_velocity);

        if (definition->get_emission_shape() == 8)
        {
            collision_result collision;
            real_point3d collision_point;
            particle_emitter_copy_spawn_position(&particle->m_position, &collision_point);
            real_vector3d collision_vector;
            particle_emitter_spawn_set_vector(&collision_vector, 0.0f, 0.0f, -1.0f);
            collision_point.z += 1.0f;
            if (collision_test_vector(k_collision_test_structure, false, &collision_point, &collision_vector, -1, -1, -1, &collision))
            {
                particle_emitter_spawn_point_from_line(&collision.point, &collision.plane.n, 0.005f, &particle->m_position);
                particle->m_velocity = collision.plane.n;
                particle->m_flags |= 0x50;
            }
            else
            {
                particle->m_flags |= 8;
            }
        }

        if ((bool)((system_definition->flags >> 6) & 1))
        {
            long effect_index = particle_system->m_effect_index;
            const real_vector3d *effect_velocity;
            if (effect_index != -1)
                effect_velocity = &((effect_datum *)(effect_data)->data)[datum_index_to_absolute_index(effect_index)].velocity;
            else
                effect_velocity = global_zero_vector3d;
            particle->m_velocity.i = particle->m_velocity.i + effect_velocity->i;
            particle->m_velocity.j = effect_velocity->j + particle->m_velocity.j;
            particle->m_velocity.k = effect_velocity->k + particle->m_velocity.k;
        }
    }
    else
    {
        float position_scale = (1.0f - t) * scale;
        particle->m_position.x = particle->m_velocity.i * position_scale + particle->m_position.x;
        particle->m_position.y = particle->m_velocity.j * position_scale + particle->m_position.y;
        particle->m_position.z = particle->m_velocity.k * position_scale + particle->m_position.z;
    }

    long effect_index = system_definition->get_particle_definition_index();
    char *effect_definition = effect_index != -1 ? (char *)tag_get(0, effect_index) : 0;
    unsigned long effect_flags = *(unsigned long *)effect_definition;
    c_particle_collision_event collision_event;
    if ((bool)((effect_flags >> 6) & 1))
    {
        collision_event.initialize(&particle->m_position, &particle->m_velocity, &global_identity4x3->up, c_global_material_type::_default);
        particle_handle_collision_event(particle, 0, emitter->m_particle_index, &collision_event, state_list);
    }

    if (definition->is_gpu())
    {
        unsigned short gpu_index = (unsigned short)emitter->m_gpu_index;
        particle->m_gpu_index = particle_block_queue_gpu_particle(
            &((s_particle_emitter_gpu_block *)g_particle_gpu_block_data->data)[gpu_index], type, particle, scale, false);
    }
}




static __forceinline const c_particle_location *particle_emitter_pulse_get_location(c_particle_location *locations, long index)
{
    return &locations[datum_index_to_absolute_index(index)];
}

static __forceinline bool particle_emitter_pulse_should_pulse(const c_particle_system *system)
{
    bool result = (bool)((system->m_flags >> 4) & 1);
    return result;
}

// @address 0x143780 particle_emitter_spawn_particles
extern "C" long particle_emitter_spawn_particles(c_particle_emitter *emitter, float delta, c_particle_state_list *state_list)
{
    long result = 0;
    const c_particle_location *location = particle_emitter_pulse_get_location(
        (c_particle_location *)g_particle_location_data_array->data, emitter->m_location_index);
    c_particle_system *system = &((c_particle_system *)g_particle_system_data_array->data)[datum_index_to_absolute_index(location->system_index)];
    // match: 0x143780: rotating the tag index keeps the emitter base in the first source operand of the add at 0x14383c.
    const s_effect_definition *effect = (const s_effect_definition *)tag_get(0, _rotl(system->m_tag_index, 0));
    const c_particle_system_definition *system_definition = &(
        (const c_particle_system_definition *)((s_particle_movement_effect_event *)effect->events.address)[system->m_def_index].particle_systems.address)
        [system->m_sub_index];
    const c_particle_emitter_definition *emitters = system_definition->get_emitter(0);
    // match: 0x143780: rotating the definition index schedules its multiply by 752 before the two datum-array loads.
    const c_particle_emitter_definition *definition = &emitters[_rotl(emitter->m_definition_index, 0)];
    emitter->m_accumulated_count = (float)fmod((double)emitter->m_accumulated_count, 1.0);
    bool matrix_initialized = (bool)((emitter->m_flags >> 2) & 1);
    real_point3d previous_position = emitter->m_matrix.position;
    float scale = particle_emitter_compute_matrix(emitter, definition, system, state_list);
    if (!(bool)((system->m_flags >> 1) & 1) || !matrix_initialized)
        previous_position = emitter->m_matrix.position;
    if (particle_emitter_pulse_should_pulse(system))
    {
        if (!(bool)((location->state >> 4) & 1))
        {
            emitter->m_property_values.starting_count = definition->m_starting_count.evaluate(state_list);
            const float &accumulated_count = +emitter->m_accumulated_count;
            emitter->m_accumulated_count = accumulated_count + emitter->m_property_values.starting_count;
        }
        emitter->m_property_values.emission_rate = definition->m_emission_rate.evaluate(state_list);
        emitter->m_accumulated_count += emitter->m_property_values.emission_rate * delta;
        emitter->m_property_values.max_count = definition->m_max_count.evaluate(state_list);
        if (emitter->m_property_values.max_count > 0.0f)
        {
            float available_count = emitter->m_property_values.max_count - (float)(long)particle_emitter_gpu_get(emitter->m_gpu_index)->count;
            float accumulated_count = emitter->m_accumulated_count;
            if (accumulated_count > available_count)
                accumulated_count = available_count;
            emitter->m_accumulated_count = accumulated_count;
        }
    }
    if (emitter->m_accumulated_count >= 1.0f)
    {
        float t = 0.0f;
        float step = 1.0f / emitter->m_accumulated_count;
        t = 0.0f;
        result = (long)(float)floor((double)emitter->m_accumulated_count);
        if (result >= 1)
        {
            unsigned long count = result;
            do
            {
                emitter->spawn_particle(state_list, system, definition, previous_position, t, delta, scale);
                --count;
                t += step;
            } while (count != 0);
        }
    }
    return result;
}



static __forceinline bool particle_emitter_frame_test_long_flag(unsigned long flags, long bit)
{
    return (_rotl(flags, 32 - bit) & 1) != 0;
}

static __forceinline bool particle_emitter_frame_test_flag(unsigned short flags, long bit)
{
    unsigned char result = (unsigned char)((flags >> bit) & 1);
    return result != 0;
}

static __forceinline bool particle_emitter_frame_can_update(const c_particle *particle)
{
    return (particle->m_flags & 8) == 0 && particle->m_age <= 1.0f;
}

static __forceinline bool particle_emitter_frame_run_physics(const c_particle *particle)
{
    return (particle->m_flags & 0x48) == 0 && particle->m_age <= 1.0f;
}

static __forceinline float particle_emitter_frame_floor(float value)
{
    return (float)floor((double)value);
}

static __forceinline const c_particle_system_definition *particle_emitter_frame_get_system_definition(const c_particle_system *system)
{
    const s_effect_definition *effect = (const s_effect_definition *)tag_get(0, system->m_tag_index);
    return &((s_particle_movement_effect_event *)effect->events.address)[system->m_def_index].particle_systems[system->m_sub_index];
}

static __forceinline const c_particle_definition *particle_emitter_frame_get_particle_definition(const c_particle_system *system)
{
    long index = particle_emitter_frame_get_system_definition(system)->get_particle_definition_index();
    return index != -1 ? (const c_particle_definition *)tag_get(0, index) : 0;
}

static __forceinline c_particle_system *particle_emitter_frame_get_system(long index)
{
    return &((c_particle_system *)((s_data_array *)g_particle_system_data_array)->data)[datum_index_to_absolute_index(index)];
}

// @address 0x14a410 particle_emitter_update_particles
extern "C" int particle_emitter_update_particles(c_particle_emitter *emitter, c_particle_state_list *state_list, float delta)
{
    const c_particle_location *location = particle_emitter_pulse_get_location(
        (c_particle_location *)g_particle_location_data_array->data, emitter->m_location_index);
    s_particle_emitter_gpu_block *gpu = particle_emitter_gpu_get(emitter->m_gpu_index);
    c_particle_system *system = particle_emitter_frame_get_system(location->system_index);
    const c_particle_system_definition *emitter_def = particle_emitter_frame_get_system_definition(system);
    bool active = particle_emitter_frame_test_flag(emitter->m_flags, 0) || !particle_emitter_frame_test_flag(emitter->m_flags, 1);
    bool keep = active || !particle_emitter_frame_test_flag(emitter_def->flags, 0);
    if (keep)
    {
        const c_particle_emitter_definition *unit_array = emitter_def->get_emitter(0);
        const c_particle_emitter_definition *base = &unit_array[emitter->m_definition_index];
        long movement_tag = emitter_def->get_particle_definition_index();
        const c_particle_definition *movement_def =
            movement_tag != -1 ? (const c_particle_definition *)tag_get(0, movement_tag) : 0;
        const c_particle_movement_definition *movement = base->get_particle_movement();
        long type_byte = system->m_type;
        if (movement != 0) {
            long first = emitter->m_particle_index;
            if (first != -1) {
                movement->update_particles(system, (const c_particle_location *)location, first, delta);
            }
        }
        long particle_index = emitter->m_particle_index;
        c_particle *prev_particle = 0;
        while (particle_index != -1) {
            c_particle *particle = &((c_particle *)g_particle_data->data)[(unsigned short)particle_index];
            // match: 0x14a410: zero-bit rotate preserves 32-bit GPR value
            long next_index = (long)_rotl(particle->m_next_index, 0);
            float age = particle->m_inverse_lifespan * delta + particle->m_age;
            particle->m_age = age;
            if (!(age <= 1.0f))
                particle->m_flags = particle->m_flags | 8;
            if (particle_emitter_frame_can_update(particle)) {
                state_list->set_particle((const c_particle *)particle);
                if (!c_editable_property_base::property_value_is_initialized(&particle->m_property_values.m_self_acceleration) ||
                    !base->m_particle_self_acceleration.is_constant_over_time())
                {
                    float t = base->m_particle_self_acceleration.c_editable_property_base::evaluate(state_list);
                    particle->m_property_values.m_self_acceleration = base->m_particle_self_acceleration.interpolate_public(t);
                }
                particle->m_velocity.i = particle->m_property_values.m_self_acceleration.i * delta + particle->m_velocity.i;
                particle->m_velocity.j = particle->m_property_values.m_self_acceleration.j * delta + particle->m_velocity.j;
                particle->m_velocity.k = particle->m_property_values.m_self_acceleration.k * delta + particle->m_velocity.k;
            }
            unsigned short pflags = particle->m_flags;
            if (particle_emitter_frame_run_physics(particle)) {
                const real_vector3d *velocity = &particle->m_velocity;
                if ((pflags & 0x80) != 0 && !(dot_product3d(velocity, velocity) > 0.0625f)) {
                    particle->m_flags = pflags | 0x40;
                } else {
                    particle->m_flags = pflags & 0xff7f;
                    particle->m_position.x += particle->m_velocity.i * delta;
                    particle->m_position.y += particle->m_velocity.j * delta;
                    particle->m_position.z += particle->m_velocity.k * delta;
                    particle->m_random_offset += particle->m_rotational_velocity * delta;
                    particle->m_random_offset -= particle_emitter_frame_floor(particle->m_random_offset);
                }
            }
            else if (particle_emitter_frame_test_flag(pflags, 6) && particle_emitter_frame_test_long_flag(movement_def->flags, 0))
            {
                particle->m_flags = pflags | 8;
            }
            if (!particle_emitter_frame_test_flag(particle->m_flags, 3)) {
                bool use_gpu = true;
                if (particle_emitter_frame_test_flag(particle->m_flags, 6) && ((const c_particle_emitter_definition *)base)->is_gpu_when_at_rest()) {
                    real_vector3d *vel = &particle->m_velocity;
                    float len = normalize3d(vel);
                    const real_vector3d *src = len > 0.0f ? vel : global_down3d;
                    use_gpu = false;
                    vel->i = src->i * 0.001f;
                    vel->j = src->j * 0.001f;
                    vel->k = src->k * 0.001f;
                    particle->m_rotational_velocity = 0.0f;
                    particle->m_flags = particle->m_flags | 8;
                }
                particle->m_gpu_index = particle_block_queue_gpu_particle(gpu, type_byte, particle, delta, use_gpu);
            }
            if (!particle_emitter_frame_test_flag(particle->m_flags, 3)) {
                prev_particle = particle;
            } else {
                const c_particle_definition *def2 = particle_emitter_frame_get_particle_definition(system);
                if (particle_emitter_frame_test_long_flag(def2->flags, 8)) {
                    particle->m_age = 1.0f;
                    particle_state_list_set_particle(state_list, (const c_particle *)particle);
                    particle_handle_collision_event(particle, 2, -1, 0, state_list);
                }
                particle->m_next_index = -1;
                emitter->m_count = emitter->m_count + 0xffff;
                datum_delete(g_particle_data, particle_index);
                if (prev_particle != 0)
                    prev_particle->m_next_index = next_index;
                else
                    emitter->m_particle_index = next_index;
            }
            particle_index = next_index;
        }
    }
    {
        bool cinematic = false;
        s_cinematic_globals_definition *cine = cinematic_globals;
        if (cine != 0)
            cinematic = cine->in_progress;
        if (cinematic != 0)
            emitter->m_flags |= 1;
        else
            emitter->m_flags &= 0xfffe;
        emitter->m_flags &= 0xfffd;
        if (keep != 0)
            emitter->m_flags |= 8;
        else
            emitter->m_flags &= 0xfff7;
    }
    return gpu->count;
}

c_particle_emitter_allocation_manager g_particle_emitter_allocator;

