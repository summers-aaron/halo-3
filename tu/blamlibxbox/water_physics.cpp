// blamlibxbox/water_physics.obj: from the records 0xadfb8 0xde760 0xef198 0xfbbd0 0x122710 0x122788 0x1b9f00 0x1ba530
// @flags /TP /O2 /Oc
#include "physics/physics_constants_tls.h"
#include "physics/water_physics_queries.h"
#include "memory/tls_globals.h"
#include "cache/restricted_memory_regions_inline.h"
#include "effects/material_effects.h"
#include <stddef.h>
// @compiler xdk6021
#include "memory/data.h"
#include "memory/datum_index.h"
#include <vectorintrinsics.h>
#include "cseries/cseries.h"
#include "phys/phys_base.h"
#include "math/real_math.h"
#include "tag_files/tag_groups.h"
#include "phys/phys_collide.h"
#include "phys/phys_dynamics.h"
#include <ppcintrinsics.h>
#include "objects/object_datum.h"
#include "math/matrix_math_types.h"
#include "game/game_time_types.h"
#include <math.h>
#include "objects/objects.h"
#include "physics/water_physics.h"
#include "objects/object_type_access.h"
#include "physics/impact_array_types.h"
#include "game/material_constants.h"
#include "physics/water_physics_globals.h"
#include "sound/game_sound.h"
#include "physics/havok_utilities.h"
#include "scenario/scenario.h"
#include "physics/havok_component.h"
#include "camera/observer_proximity.h"
#include "physics/water_physics_types.h"
#include "effects/material_effect_types.h"
#include "physics/havok_component_class.h"
#include "memory/data_typed_access.h"
struct s_water_physics_instance {
    short material_index;
    short pad;
    real_vector3d flow_direction;
    float flow_velocity;
    s_tag_block water_planes;
    s_tag_block water_debug_triangles;
};
struct s_water_physics_material { long name; };
#include "structures/structure_design_types.h"
#include "scenario/scenario_structure_bsp_reference.h"
#include "scenario/scenario_tag.h"
struct s_water_physics_interaction { real_vector3d velocity; real_vector3d direction; };
#include "render/water_renderer.h"

#include "phys/phys_bvh_ray_vm.h"


#include "game/game_globals.h"
#include "game/materials_definitions.h"

#include "objects/object_definitions.h"
#include "models/model_types.h"
#include "physics/physics_model_types.h"


template <typename T, long k_count> class c_water_static_stack {
    long m_count;
    T m_elements[k_count];
public:
    __forceinline c_water_static_stack() : m_count(0) {}
    __forceinline long count() const { return m_count; }
    __forceinline const long &count_reference() const { return m_count; }
    __forceinline T *get(long index) { return &m_elements[index]; }
    __forceinline void resize(long count) { m_count = count; }
};
struct s_water_physics_model {
    struct s_sphere { real_point3d center; float radius; };
    struct s_rigid_body { long alternate; c_water_static_stack<s_sphere, 8> spheres; };
    c_water_static_stack<s_rigid_body, 10> rigid_bodies;
    c_global_material_type material_type;
    short pad;
    volatile long sphere_count;
    __forceinline s_water_physics_model() {}
};


// @address 0xadfb8 water_physics_try_and_get_material
extern "C" bool water_physics_try_and_get_material(const real_point3d *point, c_global_material_type *material_type, long *material_name)
{
    s_water_physics_test_sphere_result result;
    result.material_type.m_index = -1;
    bool found = water_physics_test_sphere(true, point, 0.0f, &result);
    if (material_type)
        material_type->m_index = result.material_type.m_index;
    if (found) {
        if (material_name) {
            long design_index =
                ((scenario_structure_bsp_reference *)global_scenario->structure_bsps.address)[result.structure_bsp_index].structure_design.index;
            const s_structure_design *design = design_index != -1 ?
                (const s_structure_design *)global_tag_instances[datum_index_to_absolute_index(design_index)].definition : 0;
            *material_name = TAG_BLOCK_GET_ELEMENT(&design->water_groups, TAG_BLOCK_GET_ELEMENT(&design->water_instances,
                result.water_instance_index, const s_water_physics_instance)->material_index, const s_water_physics_material)->name;
        }
    }
    return found;
}
// @address 0x122710 water_physics_get_depth_at_point
extern "C" float water_physics_get_depth_at_point(const real_point3d *point)
{
    float depth = -3.4028234663852886e+38f;
    s_water_physics_test_sphere_result result;
    result.material_type.m_index = -1;
    if (water_physics_test_sphere(true, point, 0.0f, &result))
        depth = result.depth;
    return depth;
}

// @address 0x122788 water_physics_handle_impact_effects
extern "C" void water_physics_handle_impact_effects(long material_effects_index, long user_index, e_effect_type effect_type,
    e_sweetener_size sweetener_size, c_global_material_type material_type, bool underwater,
    const real_point3d *position, const real_vector3d *normal, const real_vector3d *direction,
    const long *location, float scale, const s_water_physics_interaction *interaction, long flags)
{
    long effects[7];
    material_effect_find_effects(false, position, material_effects_index, k_global_material_type_invalid, material_type, underwater,
        effect_type, sweetener_size, effects);
    if (effects[6] != NONE && interaction &&
        active_user_close_to_point_not_deterministic(position, 40.0f, _adjust_for_fov_scale_none) && g_restricted_address[1])
        c_water_renderer::game_interation_event_add(effects[6], position, &interaction->direction, &interaction->velocity);
#ifdef HALO3_64BIT
    effect_spawn_impact_effects(effects, position, normal, direction, location, (const void *)(uintptr_t)(unsigned long)flags);
#else
    effect_spawn_impact_effects(effects, position, normal, direction, location, flags);
#endif
    game_sound_new_material_effect_sounds(effects, position, normal, direction, location, user_index, scale);
}
static __forceinline void water_physics_vector4_from_point3d(phys_vec4 *vector, const real_point3d *point)
{
    union { float values[4]; __vector4 quad; } components;
    components.values[0] = point->x;
    components.values[1] = point->y;
    components.values[2] = point->z;
    components.values[3] = 0.0f;
    vector->m_simd_quad = components.quad;
}

// @address 0x74b548 g_water_physics_flow_minimum
// @address 0x74b54c g_water_physics_flow_maximum
extern "C" {
float g_water_physics_flow_minimum = 0.5f;
float g_water_physics_flow_maximum = 3.0f;
}
struct s_game_globals;
const float k_water_physics_maximum_surface_depth = 2.0f;

static __forceinline long water_physics_first_active_bsp()
{
    return 31 - _CountLeadingZeros(-g_active_structure_bsp_mask & g_active_structure_bsp_mask);
}
static __forceinline long water_physics_next_active_bsp(long index)
{
    unsigned long mask = g_active_structure_bsp_mask & ~(unsigned long)((1i64 << (index + 1)) - 1);
    return 31 - _CountLeadingZeros(-mask & mask);
}
static __forceinline const s_structure_design *water_physics_structure_design_get(long structure_bsp_index)
{
    long design_index =
                ((scenario_structure_bsp_reference *)global_scenario->structure_bsps.address)[structure_bsp_index].structure_design.index;
    return design_index != -1 ? (const s_structure_design *)global_tag_instances[datum_index_to_absolute_index(design_index)].definition : 0;
}
static __forceinline float water_physics_plane_distance(const real_plane3d *plane, const real_point3d *point)
{
    return dot_product3d(point, &plane->n) - plane->d;
}

// @address 0x1b9f00 ?water_physics_test_sphere@@YA_N_NPBTreal_point3d@@MPAUs_water_physics_test_sphere_result@@@Z
bool water_physics_test_sphere(bool point_test, const real_point3d *point, float radius, s_water_physics_test_sphere_result *result)
{
    bool found = false;
    result->structure_bsp_index = -1;
    result->water_instance_index = -1;
    result->flow = *global_zero_vector3d;
    result->fraction = 0.0f;
    result->plane.n = *global_up3d;
    result->plane.d = 0.0f;
    for (long structure_bsp_index = water_physics_first_active_bsp(); structure_bsp_index != -1;
        structure_bsp_index = water_physics_next_active_bsp(structure_bsp_index))
    {
        const s_structure_design *design = water_physics_structure_design_get(structure_bsp_index);
        if (design && design->water_mopp_codes.count)
        {
            const phys_bvh_bytecode *mopp_code = TAG_BLOCK_GET_ELEMENT(&design->water_mopp_codes, 0, const phys_bvh_bytecode);
            phys_inline_array<phys_bvh_primitive_info, 1024> hits;
            phys_array<phys_bvh_primitive_info> *hit_array = &hits;
            phys_vec4 center;
            water_physics_vector4_from_point3d(&center, point);
            phys_sphere sphere(center, radius);
            phys_bvh_sphere_vm_query_sphere(mopp_code, sphere, hit_array);
            if (radius != 0.0f || point_test)
            {
                for (long hit_index = 0; hit_index < hits.getSize(); hit_index++)
                {
                    const phys_bvh_primitive_info *hit = &hits[hit_index];
                    long instance_index = hit->ID;
                    const s_water_physics_instance *instances = TAG_BLOCK_GET_ELEMENT(&design->water_instances, 0,
                        const s_water_physics_instance);
                    const s_water_physics_instance *instance = &instances[instance_index];
                    bool intersects = false;
                    float best_fraction = 1.0f;
                    float surface_depth = k_water_physics_maximum_surface_depth;
                    long best_plane_index = -1;
                    bool surface_valid = false;
                    real_plane3d surface_plane;
                    for (long plane_index = 0; plane_index < instance->water_planes.count; plane_index++)
                    {
                        const real_plane3d *plane = TAG_BLOCK_GET_ELEMENT(&instance->water_planes, plane_index, const real_plane3d);
                        float distance = water_physics_plane_distance(plane, point);
                        if (distance > radius)
                        {
                            intersects = false;
                            break;
                        }
                        if (plane->n.k > 0.001f)
                        {
                            float denominator = dot_product3d(&plane->n, global_up3d);
                            float depth = denominator != 0.0f ? -(distance / denominator) : 0.0f;
                            float fraction;
                            if (radius > 0.001f)
                            {
                                fraction = (depth / radius + 1.0f) * 0.5f;
                                fraction = PIN(fraction, 0.0f, 1.0f);
                            }
                            else
                            {
                                fraction = 1.0f;
                            }
                            if (fraction <= best_fraction)
                            {
                                best_fraction = fraction;
                                best_plane_index = plane_index;
                            }
                            intersects = true;
                            if (depth >= 0.0f && depth < surface_depth)
                            {
                                surface_depth = depth;
                                surface_plane = *plane;
                                surface_valid = true;
                            }
                        }
                    }
                    if (!surface_valid)
                    {
                        surface_plane.n = *global_up3d;
                        surface_plane.d = 0.0f;
                        surface_depth = 0.0f;
                    }
                    if (intersects && best_plane_index != -1 &&
                        (result->water_instance_index == -1 || best_fraction > result->fraction))
                    {
                        const real_plane3d *best_plane = TAG_BLOCK_GET_ELEMENT(&instance->water_planes, best_plane_index,
                            const real_plane3d);
                        result->water_instance_index = instance_index;
                        result->fraction = best_fraction;
                        result->depth = surface_depth - radius;
                        result->structure_bsp_index = structure_bsp_index;
                        found = true;
                        result->surface_plane = surface_plane;
                        result->plane = *best_plane;
                        result->material_type.m_index = global_game_globals->water_material_type.m_index;
                    }
                }
            }
            else
            {
                for (long hit_index = 0; hit_index < hits.getSize(); hit_index++)
                {
                    const s_water_physics_instance *instance = TAG_BLOCK_GET_ELEMENT(
                        &design->water_instances, hits[hit_index].ID, const s_water_physics_instance);
                    bool inside = true;
                    for (long plane_index = 0; plane_index < instance->water_planes.count; plane_index++)
                    {
                        if (water_physics_plane_distance(TAG_BLOCK_GET_ELEMENT(&instance->water_planes, plane_index,
                            const real_plane3d), point) > 0.0f)
                        {
                            inside = false;
                            break;
                        }
                    }
                    if (inside)
                    {
                        found = true;
                        result->material_type.m_index = global_game_globals->water_material_type.m_index;
                        break;
                    }
                }
            }
        }
    }
    if (result->water_instance_index != -1)
    {
        const s_structure_design *design = water_physics_structure_design_get(result->structure_bsp_index);
        const s_water_physics_instance *instance = TAG_BLOCK_GET_ELEMENT(
            &design->water_instances, result->water_instance_index, const s_water_physics_instance);
        float flow_speed = instance->flow_velocity * 0.63661975f;
        float velocity = flow_speed * g_water_physics_flow_maximum + g_water_physics_flow_minimum;
        scale_vector3d(&instance->flow_direction, velocity, &result->flow);
    }
    return found;
}

static __forceinline void water_physics_ray_point_from_line(const real_point3d *point, const real_vector3d *vector, float t,
    real_point3d *result)
{
    float x = point->x;
    float y = point->y;
    float z = point->z;
    result->x = vector->i * t + x;
    result->y = vector->j * t + y;
    result->z = vector->k * t + z;
}

static __forceinline void water_physics_ray_vector4(phys_vec4 *vector, const real_point3d *point)
{
    union { float values[4]; __vector4 quad; } components;
    components.values[2] = point->z;
    components.values[0] = point->x;
    components.values[1] = point->y;
    components.values[3] = 0.0f;
    vector->m_simd_quad = components.quad;
}

static __forceinline float water_physics_ray_plane_distance(float x, float y, float z, const real_plane3d *plane)
{
    const real_vector3d *normal = &plane->n;
    float z_product = z * normal->k;
    return ((y * plane->n.j + z_product) + x * normal->i) - plane->d;
}

static __forceinline float water_physics_vector_intersect_plane(const real_point3d *point, const real_vector3d *vector,
    const real_plane3d *plane)
{
    float denominator = dot_product3d(&plane->n, vector);
    if (denominator != 0.0f)
    {
        return -(water_physics_ray_plane_distance(point->x, point->y, point->z, plane) / denominator);
    }
    return 0.0f;
}
static __forceinline void water_physics_instance_get_flow(s_water_physics_instance *instance, real_vector3d *flow);

// @address 0xfbbd0 ?water_physics_test_vector@@YA_NPBTreal_point3d@@PBTreal_vector3d@@PAUs_water_physics_test_vector_result@@@Z
bool water_physics_test_vector(const real_point3d *point, const real_vector3d *vector, s_water_physics_test_vector_result *result)
{
    bool found = false;
    bool inside = false;
    result->fraction = 1.0f;
    for (long structure_bsp_index = water_physics_first_active_bsp(); !inside && structure_bsp_index != -1;
        structure_bsp_index = water_physics_next_active_bsp(structure_bsp_index))
    {
        s_structure_design *design = (s_structure_design *)water_physics_structure_design_get(structure_bsp_index);
        if (design && design->water_mopp_codes.count)
        {
            phys_bvh_ray_vm machine;
            const phys_bvh_bytecode *code = TAG_BLOCK_GET_ELEMENT(&design->water_mopp_codes, 0, const phys_bvh_bytecode);
            phys_inline_array<phys_bvh_primitive_info, 1024> hits;
            phys_vec4 from, to;
            real_point3d endpoint;
            water_physics_ray_point_from_line(point, vector, 1.0f, &endpoint);
            water_physics_ray_vector4(&to, &endpoint);
            water_physics_ray_vector4(&from, point);
            machine.query_ray(code, from, to, 0.0f, &hits);
            for (long hit_index = 0; !inside && hit_index < hits.getSize(); ++hit_index)
            {
                long instance_index = hits[hit_index].ID;
                s_water_physics_instance *instance = TAG_BLOCK_GET_ELEMENT(&design->water_instances, instance_index,
                    s_water_physics_instance);
                long inside_plane_count = 0;
                bool intersects = false;
                float best_fraction = 0.0f;
                real_vector3d normal;
                real_point3d intersection;
                float end_fraction = 1.0f;
                for (long plane_index = 0; plane_index < instance->water_planes.count; ++plane_index)
                {
                    const real_plane3d *plane = TAG_BLOCK_GET_ELEMENT(&instance->water_planes, plane_index, const real_plane3d);
                    float distance = water_physics_plane_distance(plane, point);
                    float fraction = water_physics_vector_intersect_plane(point, vector, plane);
                    if (distance > -0.001f)
                    {
                        if (fraction > best_fraction)
                        {
                            best_fraction = fraction;
                            normal = plane->n;
                            water_physics_ray_point_from_line(point, vector, fraction, &intersection);
                            intersects = true;
                        }
                        else if (fraction <= 0.0f)
                        {
                            intersects = false;
                            break;
                        }
                    }
                    else
                    {
                        if (fraction > 0.0f && fraction < end_fraction)
                            end_fraction = MIN(fraction, end_fraction);
                        ++inside_plane_count;
                    }
                }
                if (inside_plane_count == instance->water_planes.count)
                {
                    inside = true;
                    found = false;
                }
                else if (intersects && best_fraction <= 1.0f && best_fraction < result->fraction && end_fraction >= best_fraction)
                {
                    result->structure_bsp_index = structure_bsp_index;
                    result->water_instance_index = instance_index;
                    result->fraction = best_fraction;
                    result->normal = normal;
                    result->point = intersection;
                    result->material_type.m_index = global_game_globals->water_material_type.m_index;
                    result->velocity = *global_zero_vector3d;
                    real_vector3d *flow = &result->flow;
                    water_physics_instance_get_flow(instance, flow);
                    found = true;
                }
            }
        }
    }
    return found;
}

static __forceinline void water_physics_instance_get_flow(s_water_physics_instance *instance, real_vector3d *flow)
{
    const float *minimum = &g_water_physics_flow_minimum;
    const float *maximum = &g_water_physics_flow_maximum;
    scale_vector3d(&instance->flow_direction, *minimum + *maximum * (instance->flow_velocity / 1.5707964f), flow);
}

#define real_rectangle3d real_rectangle3d_legacy_union
#undef real_rectangle3d
class phys_shape;
class phys_rigid_body;
#if !defined(_WIN64) && !defined(__LP64__)
typedef char water_physics_havok_body_size[(sizeof(c_havok_component::c_rigid_body) == 72) ? 1 : -1];
#endif

// @rdata 0x82021694
static const float k_water_physics_extent_minimum_initial = 3.4028234663852886e+38f;
// @rdata 0x82022024
static const float k_water_physics_extent_maximum_initial = -3.4028234663852886e+38f;
// @rdata 0x8203385c
static const float k_water_physics_multiple_sphere_minimum_extent = 0.2f;
static __forceinline real_point3d *rectangle3d_center(const real_rectangle3d *rectangle, real_point3d *center)
{
    center->x = (rectangle->x0 + rectangle->x1) * 0.5f;
    center->y = (rectangle->y0 + rectangle->y1) * 0.5f;
    center->z = (rectangle->z0 + rectangle->z1) * 0.5f;
    return center;
}
static __forceinline void water_physics_rigid_body_material_type(
    const c_havok_component *component, long rigid_body_index, c_global_material_type *material_type)
{
    float mass;
    float volume;
    material_type->m_index = -1;
    const phys_rigid_body *rigid_body = component->m_rigid_bodies.m_data[rigid_body_index].m_rigid_body;
    if (!calculate_havok_shape_properties_recursive(rigid_body->get_collider()->get_shape(), rigid_body, 0, 0xFFFFFFFF, &mass,
        &volume, material_type))
        material_type->m_index = 0;
}
static __forceinline long water_physics_model_rigid_body_count(const c_havok_component *component, bool clamp_to_maximum)
{
    long count = component->m_rigid_bodies.m_size;
    if (clamp_to_maximum)
        count = MIN(count, 10);
    return count;
}
// @address 0xef198 ?water_physics_model_new@@YA_NJPAUs_water_physics_model@@@Z
bool water_physics_model_new(long object_index, s_water_physics_model *model)
{
    bool result = false;
    object_datum *object = (object_datum *)object_get_and_verify_type(object_index, -1);
    s_object_definition *definition =
        (s_object_definition *)tag_get('obje', object->definition_index);
    if (object->havok_component_index != -1)
    {
        if ((unsigned char)definition->water_density != 6)
        {
            c_havok_component *component = datum_get_typed<c_havok_component>(g_havok_component_data, object->havok_component_index);
            model->rigid_bodies.resize(water_physics_model_rigid_body_count(component, true));
            model->sphere_count = 0;
            for (long rigid_body_index = 0; rigid_body_index < model->rigid_bodies.count(); rigid_body_index++)
            {
                s_water_physics_model::s_rigid_body *water_rigid_body = model->rigid_bodies.get(rigid_body_index);
                if (rigid_body_index == 0)
                {
                    c_global_material_type material_type;
                    water_physics_rigid_body_material_type(component, rigid_body_index, &material_type);
                    model->material_type.m_index = material_type.m_index;
                }
                real_rectangle3d bounds;
                if (component->calculate_local_space_physics_bounding_box(rigid_body_index, &bounds))
                {
                    float min_extent = k_water_physics_extent_minimum_initial;
                    float max_extent = k_water_physics_extent_maximum_initial;
                    long min_axis = 0;
                    unsigned long is_physics_model = (component->m_flags >> 8) & 1;
                    if (is_physics_model != 0)
                    {
                        c_havok_component::c_rigid_body *havok_body = &component->m_rigid_bodies.m_data[rigid_body_index];
                        if (havok_body->m_physics_model_rigid_body_indices.m_size > 0)
                        {
                            s_model_definition *hlmt =
                                (s_model_definition *)tag_get('hlmt', definition->model.index);
                            s_physics_model_definition *physics_model =
                                (s_physics_model_definition *)tag_get('phmo', hlmt->physics_model_index);
                            s_physics_model_rigid_body *body_data = &physics_model->rigid_bodies[havok_body->m_physics_model_rigid_body_indices.m_data[0]];
                            for (long k = 0; k < 6; k++)
                                bounds.m[0][k] += body_data->water_physics_bounds.m[0][k];
                        }
                    }
                    for (long axis = 0; axis < 3; axis++)
                    {
                        float extent = bounds.n[axis].upper - bounds.n[axis].lower;
                        if (extent < min_extent)
                        {
                            min_axis = axis;
                            min_extent = extent;
                        }
                        max_extent = MAX(extent, max_extent);
                    }
                    float radius = 0.5f * min_extent;
                    long axis1 = (min_axis + 1) % 3;
                    long axis2 = (min_axis + 2) % 3;
                    float extent1 = bounds.n[axis1].upper - bounds.n[axis1].lower;
                    float extent2 = bounds.n[axis2].upper - bounds.n[axis2].lower;
                    real_point3d center;
                    rectangle3d_center(&bounds, &center);
                    long sphere_index = 0;
                    if (is_physics_model != 0 && ((component->m_rigid_bodies.m_data[rigid_body_index].m_flags >> 1) & 1) == 0 &&
                        max_extent > k_water_physics_multiple_sphere_minimum_extent)
                    {
                        water_rigid_body->spheres.resize(4);
                        model->sphere_count += 4;
                        for (long i = 0; i < 2; i++)
                        {
                            float sign1 = (i == 0) ? 1.0f : -1.0f;
                            for (long j = 0; j < 2; j++)
                            {
                                s_water_physics_model::s_sphere *sphere = water_rigid_body->spheres.get(sphere_index++);
                                float sign2 = (j == 0) ? 1.0f : -1.0f;
                                sphere->center.n[min_axis] = center.n[min_axis];
                                sphere->center.n[axis1] = center.n[axis1] + sign1 * (0.5f * extent1 - radius);
                                sphere->center.n[axis2] = center.n[axis2] + sign2 * (0.5f * extent2 - radius);
                                sphere->radius = radius;
                            }
                        }
                    }
                    else
                    {
                        water_rigid_body->spheres.resize(1);
                        model->sphere_count += 1;
                        s_water_physics_model::s_sphere *sphere = water_rigid_body->spheres.get(0);
                        sphere->center = center;
                        sphere->radius = max_extent;
                        if (((1 << object_get_type(object_index)) & 0x801) != 0)
                        {
                            sphere->radius = radius;
                            sphere->center.z = sphere->center.z - 0.5f * (bounds.z1 - bounds.z0) + radius;
                        }
                    }
                }
                else
                {
                    water_rigid_body->spheres.resize(0);
                }
                water_rigid_body->alternate = (unsigned char)definition->water_density;
                if (water_rigid_body->alternate == 0)
                    water_rigid_body->alternate = object_get_water_properties_index(object_index) != 1 ? 2 : 5;
                result = true;
            }
        }
    }
    return result;
}

// @rdata 0x820221fc
static const float k_water_physics_update_one = 1.0f;
// @rdata 0x82022850
static const float k_water_physics_update_density_delay = 10.0f;
// @rdata 0x82022dc0
static const float k_water_physics_update_effect_period = 0.5f;
// @rdata 0x8202a318
static const float k_water_physics_update_ripple_radius = 40.0f;

static __forceinline long water_physics_impact_index(const c_havok_component *component, long index)
{
    return datum_get_typed<s_impact_array>(TLS_IMPACT_ARRAY_DATA, component->m_impact_array_index)->impact_indices[index];
}
static __forceinline void water_physics_transform_point(const real_matrix4x3 *matrix, const real_point3d *point, real_point3d *result)
{
    float y = point->y * matrix->scale;
    float z = point->z * matrix->scale;
    float x = point->x * matrix->scale;
    result->x = (matrix->forward.i * x + (matrix->up.i * z + matrix->left.i * y)) + matrix->position.x;
    result->y = (matrix->forward.j * x + (matrix->left.j * y + matrix->up.j * z)) + matrix->position.y;
    result->z = (matrix->forward.k * x + (matrix->up.k * z + matrix->left.k * y)) + matrix->position.z;
}
static __forceinline bool water_physics_rigid_body_is_dynamic(const c_havok_component *component, long index, bool exclude_keyframed = true)
{
    // match: 0xde760: without this the rigid-body m_data[index] address add swaps operands to add r11,r11,r14
    __assume(index >= 0);
    unsigned long type = component->m_rigid_bodies.m_data[index].m_rigid_body->get_motion_type();
    return type != phys_motion::MOTION_FIXED && (!exclude_keyframed || type != phys_motion::MOTION_KEYFRAMED);
}
static __forceinline void water_physics_update_set_flag(unsigned long *flags, long bit, bool set)
{
    if (set)
        *flags |= FLAG(bit);
    else
        *flags &= ~FLAG(bit);
}
static __forceinline float water_physics_update_magnitude3d(const real_vector3d *vector)
{
    const float &y_squared = vector->j * vector->j;
    float x = vector->i;
    float z = vector->k;
    return sqrtf((x * x + y_squared) + z * z);
}
static __forceinline c_global_material_type &water_physics_update_assign_material(const c_global_material_type &other,
    c_global_material_type &material)
{
    material.m_index = other.m_index;
    return material;
}
static __forceinline float water_physics_update_surface(const real_plane3d *plane, const real_point3d *point,
    const real_vector3d *velocity, real_point3d *surface_point, real_vector3d *surface_velocity)
{
    float normal_i = plane->n.i;
    float normal_k = plane->n.k;
    float normal_j = plane->n.j;
    float normal_speed = normal_i * velocity->i + (normal_k * velocity->k + normal_j * velocity->j);
    real_vector3d normal_velocity;
    normal_velocity.i = normal_i * normal_speed;
    normal_velocity.j = normal_j * normal_speed;
    normal_velocity.k = normal_k * normal_speed;
    float distance = -(normal_i * point->x + (normal_k * point->z + normal_j * point->y) - plane->d);
    surface_point->x = normal_i * distance + point->x;
    surface_point->y = normal_j * distance + point->y;
    surface_point->z = normal_k * distance + point->z;
    subtract_vectors3d(velocity, &normal_velocity, surface_velocity);
    return distance;
}

static __forceinline c_flags_no_init<long, unsigned long, 19> water_physics_update_get_flags(const object_datum *object)
{
    return *(const c_flags_no_init<long, unsigned long, 19> *)&object->physics_flags;
}
// @address 0xde760 ?water_physics_update@@YA_NJ@Z
bool water_physics_update(long object_index)
{
    const s_game_globals *globals = global_game_globals;
    const s_object_header *header = object_header_get(object_index);
    object_datum *object = (object_datum *)header->datum;
    const s_object_definition *definition =
        (const s_object_definition *)tag_get('obje', object->definition_index);
    bool static_object = (object->physics_flags >> 9) & 1;
    bool destroyed = TEST_BIT(object->damage_flags, 2);
    bool in_water = false;
    if (globals && (unsigned char)definition->water_density != 6)
    {
        bool update = false;
        // match: 0xde760: zero-bit rotate preserves 32-bit GPR value
        const unsigned long &physics_flags = _rotl(water_physics_update_get_flags(object).get_unsafe(), 0);
        bool was_in_water = (bool)(_rotl(physics_flags, 22) & 1);
        // match: 0xde760: const reference binding preserves temporary lifetime
        const bool &was_static = (bool)(_rotl(physics_flags, 23) & 1);
        if ((!was_static || was_in_water) && (bool)(_rotl(physics_flags, 25) & 1) && object->havok_component_index != -1)
        {
            if (was_in_water ||
                (TLS_GAME_TIME_GLOBALS->time + TLS_GAME_TIME_GLOBALS->ticks_per_second / 5 + object_index) % (TLS_GAME_TIME_GLOBALS->ticks_per_second / 5) == 0)
                update = true;
        }
        if (update)
        {
            c_havok_component *component = datum_get_typed<c_havok_component>(g_havok_component_data, object->havok_component_index);
            bool apply_drag = ((1 << header->object_type) & 0x1003) != 0;
            long sweetener_size = PIN((unsigned char)definition->sweetener_size, 0, 2);
            bool completely_submerged = true;
            float zero = 0.0f;
            float maximum_velocity = 0.0f;
            c_global_material_type material_type;
            material_type.m_index = -1;
            s_water_physics_model model;
            long sphere_sequence = 0;
            real_vector3d effect_normal;
            s_water_physics_interaction interaction;
            long ripple_effect = -1;
            {
            real_point3d effect_position;
            water_physics_update_set_flag(&object->physics_flags, 10, false);
            if (water_physics_model_new(object_index, &model))
            {
                for (long rigid_body_index = 0; rigid_body_index < model.rigid_bodies.count(); rigid_body_index++)
                {
                    if (water_physics_rigid_body_is_dynamic(component, rigid_body_index))
                    {
                        s_water_physics_model::s_rigid_body *water_rigid_body = model.rigid_bodies.get(rigid_body_index);
                        const long &sphere_count = water_rigid_body->spheres.count_reference();
                        // match: 0xde760: keep the density-derived factor first in the surface-scale multiplication.
                        long density_index = (long)_rotl(water_rigid_body->alternate, 0);
                        float in_water_time = TLS_GAME_TIME_GLOBALS->tick_length * (float)MAX(object->physics_reconnect_time, 0);
                        if (in_water_time > k_water_physics_update_density_delay)
                            density_index = (long)((in_water_time - k_water_physics_update_density_delay) / 3.0f + (float)density_index +
                                k_water_physics_update_one);
                        if (sphere_count)
                        {
                            real_matrix4x3 transform;
                            float volume = zero;
                            bool buoyant = false;
                            // match: 0xde760: zero-bit rotate preserves the transform index value.
                        component->get_rigid_body_transform((long)_rotl(rigid_body_index, 0), &transform);
                            for (long sphere_index = 0; sphere_index < sphere_count; sphere_index++)
                            {
                                const s_water_physics_model::s_sphere *sphere = water_rigid_body->spheres.get(sphere_index);
                                volume += sphere->radius * sphere->radius * sphere->radius * 4.1887903f;
                            }
                            for (long sphere_index = 0; sphere_index < water_rigid_body->spheres.count(); sphere_index++, sphere_sequence++)
                            {
                                const s_water_physics_model::s_sphere *sphere = water_rigid_body->spheres.get(sphere_index);
                                s_water_physics_test_sphere_result result;
                                result.material_type.m_index = -1;
                                real_point3d center;
                                water_physics_transform_point(&transform, &sphere->center, &center);
                                if (water_physics_test_sphere(true, &center, sphere->radius, &result))
                                {
                                    long index = result.material_type.m_index;
                                    const s_global_material_definition *material =
                                        index != -1 && index >= 0 && index < globals->materials.count() ? &globals->materials[index] : 0;
                                    if (material)
                                    {
                                        long property_index = object_get_water_properties_index(object_index);
                                        property_index = MIN(property_index, material->water_properties.count() - 1);
                                        const water_physics_material_properties *properties = property_index >= 0 &&
                                                property_index < material->water_properties.count()
                                            ? &material->water_properties[property_index]
                                            : 0;
                                        if (properties)
                                        {
                                            float drag_q = properties->drag_q;
                                            float drag_k = properties->drag_k;
                                            float drag_e = properties->drag_e;
                                            float mass = component->get_rigid_body_mass(rigid_body_index);
                                            float speed = zero;
                                            float density = 1000.0f;
                                            if (density_index >= 1 && density_index <= 5)
                                            {
                                                buoyant = true;
                                                density = properties->densities[density_index-1];
                                            }
                                            real_vector3d velocity;
                                            component->get_rigid_body_linear_velocity_at_point(rigid_body_index, &velocity, &center);
                                            subtract_vectors3d(&velocity, &result.flow, &velocity);
                                            float fraction = result.fraction;
                                            // match: 0xde760: zero-bit rotate preserves 32-bit GPR value
                                            const unsigned long &apply_buoyancy = _rotl((unsigned long)buoyant, 0);
                                            if (apply_buoyancy || (apply_drag && !static_object && !destroyed))
                                            {
                                                speed = normalize3d(&velocity);
                                                maximum_velocity = MAX(maximum_velocity, speed);
                                                if (speed != zero && !static_object)
                                                {
                                                    real_vector3d drag;
                                                    float tick_length = TLS_GAME_TIME_GLOBALS->tick_length;
                                                    float power = (float)pow(speed * drag_k, drag_e);
                                                    float scale = -(power * tick_length * fraction * drag_q);
                                                    scale_vector3d(&velocity, scale, &drag);
                                                    component->rigid_body_apply_acceleration_at_point(rigid_body_index, &center, &drag);
                                                }
                                            }
                                            if (buoyant && mass > 0.001f && density > 0.001f && !static_object)
                                            {
                                                float sphere_volume = sphere->radius * sphere->radius * sphere->radius * 4.1887903f;
                                                float volume_fraction = sphere_volume / volume;
                                                float displaced_volume = volume_fraction / density * mass;
                                                float gravity_acceleration = g_physics_constants->gravity / mass;
                                                float scale = fraction * TLS_GAME_TIME_GLOBALS->tick_length * gravity_acceleration * displaced_volume;
                                                real_vector3d acceleration;
                                                acceleration.i = scale * zero;
                                                acceleration.j = scale * zero;
                                                acceleration.k = scale;
                                                component->rigid_body_apply_acceleration_at_point(rigid_body_index, &center, &acceleration);
                                            }
                                            real_vector3d surface_velocity;
                                            real_point3d surface_position;
                                            float distance = water_physics_update_surface(
                                                &result.plane, &center, &velocity, &surface_position, &surface_velocity);
                                            float depth = distance - sphere->radius;
                                            if (!in_water)
                                            {
                                                water_physics_update_assign_material(result.material_type, material_type);
                                                effect_normal = result.plane.n;
                                                effect_position = surface_position;
                                                interaction.direction = surface_velocity;
                                                interaction.velocity = result.flow;
                                                in_water = true;
                                                water_physics_update_set_flag(&object->physics_flags, 10, true);
                                                long effects[7];
                                                material_effect_find_effects(false, &surface_position, -1, k_global_material_type_invalid,
                                                    material_type, false, (e_effect_type)(sweetener_size + 10),
                                                    (e_sweetener_size)sweetener_size, effects);
                                                ripple_effect = effects[6];
                                            }
                                            if (ripple_effect != -1 &&
                                                water_physics_test_ripple_spawn(
                                                    &model, object_index, depth, speed, sphere_sequence, model.sphere_count) &&
                                                active_user_close_to_point_not_deterministic(
                                                    &surface_position, k_water_physics_update_ripple_radius, _adjust_for_fov_scale_none))
                                                c_water_renderer::game_interation_event_add(ripple_effect, &surface_position,
                                                    &surface_velocity, &result.flow);
                                            if (fraction < k_water_physics_update_one)
                                                completely_submerged = false;
                                            continue;
                                        }
                                    }
                                }
                                completely_submerged = false;
                            }
                        }
                    }
                }
            }
            if (in_water)
            {
                if (TLS_GAME_TIME_GLOBALS->tick_length * (float)-object->physics_reconnect_time > k_water_physics_update_effect_period)
                {
                    float scale = water_physics_update_magnitude3d(&object->linear_velocity) * k_water_physics_update_effect_period;
                    scale = PIN(scale, zero, k_water_physics_update_one);
                    water_physics_handle_impact_effects(definition->material_effects.index, object_index,
                        (e_effect_type)(sweetener_size + 10), (e_sweetener_size)PIN((unsigned char)definition->sweetener_size, 0,
                            2), material_type, false,
                        &effect_position, &effect_normal, &effect_normal, (const long *)&object->location, scale, &interaction, 0);
                }
                object->physics_reconnect_time = (short)(PIN(object->physics_reconnect_time, 0, 32766) + 1);
                c_impact::s_contact_description contact;
                real_point3d center_of_mass;
                object_get_center_of_mass(object_index, &center_of_mass);
                impact_initialize_contact_description(&contact, 3, false, object_index, 0, model.material_type, -1, -1, material_type,
                    &center_of_mass, &global_identity4x3->up, -1, 0);
                if (component->find_impact_which_includes_contact(&contact, false) == -1)
                {
                    float score = c_impact::calculate_score(&contact.position, c_impact::state1);
                    if (component->make_space_for_new_impact(score, contact.node_index_a))
                    {
                        long impact_index = impact_new_from_contact(&contact, 1);
                        component->add_impact(impact_index);
                        datum_get_typed<c_impact>(TLS_IMPACT_DATA, impact_index)->physics_reference_count++;
                    }
                }
                for (long index = 0; index < component->get_impact_count(); index++)
                {
                    c_impact *impact = datum_get_typed<c_impact>(TLS_IMPACT_DATA, water_physics_impact_index(component, index));
                    if (impact->type == 3 && impact->physics_reference_count != 0)
                    {
                        float scale = PIN(maximum_velocity / 3.0f, 0.001f, k_water_physics_update_one);
                        if (completely_submerged)
                        {
                            impact->volume_a = zero;
                            impact->volume_b = scale;
                        }
                        else
                        {
                            impact->volume_a = scale;
                            impact->volume_b = zero;
                        }
                    }
                }
            }
            else
            {
                for (long index = 0; index < component->get_impact_count(); index++)
                {
                    long impact_index = water_physics_impact_index(component, index);
                    c_impact *impact = datum_get_typed<c_impact>(TLS_IMPACT_DATA, impact_index);
                    if (impact->type == 3 && impact->physics_reference_count != 0)
                        impact->remove_reference(impact_index, false);
                }
                object->physics_reconnect_time = (short)(PIN(object->physics_reconnect_time, -32767, 0)-1);
            }
            }
        }
    }
    return in_water;
}

extern "C" {
}
// @rdata 0x82039798..0x820397c8
static const float k_water_ripple_depth_limits[3] = { 0.05f, 0.1f, 0.3f };
static const float k_water_ripple_min_rates[3] = { 6.2f, 8.5f, 12.5f };
static const float k_water_ripple_max_rates[3] = { 12.0f, 15.0f, 25.0f };
static const float k_water_ripple_speed_divisors[3] = { 2.0f, 3.0f, 5.0f };
// @rdata-end
static const float k_water_ripple_minimum_rate = 0.001f;
static __forceinline const s_object_definition *water_physics_object_definition_get(long object_index)
{
    long definition_index = object_get(object_index)->definition_index;
    return (const s_object_definition *)tag_get('obje', definition_index);
}
// @address 0x1ba530 water_physics_test_ripple_spawn
extern "C" unsigned char water_physics_test_ripple_spawn(
    const s_water_physics_model *model, long object_index, float depth, float speed, long sequence, long scale)
{
    const s_object_definition *definition = water_physics_object_definition_get(object_index);
    unsigned long type = PIN((unsigned char)definition->sweetener_size, 0, 2);
    bool result = false;
    if (depth < k_water_ripple_depth_limits[type])
    {
        long count = model->sphere_count;
        if (count > 0)
        {
            float factor = PIN(speed / k_water_ripple_speed_divisors[type], 0.0f, 1.0f);
            float complement = 1.0f - factor;
            float inverse_count = 1.0f / (float)model->sphere_count;
            float scaled_max = k_water_ripple_max_rates[type] * inverse_count;
            float scaled_min = k_water_ripple_min_rates[type] * inverse_count;
            float rate = (scaled_min * complement + scaled_max * factor) * TLS_GAME_TIME_GLOBALS->tick_length;
            if (rate > k_water_ripple_minimum_rate)
            {
                float interval = 1.0f / rate / (float)scale;
                long period = (long)(interval + 0.5f);
                long stride = (long)(interval / (float)scale + 0.5f);
                if (stride <= 1)
                    stride = 1;
                result = period == 0 || (stride * sequence + TLS_GAME_TIME_GLOBALS->time + object_index) % period == 0;
            }
        }
    }
    return result;
}

typedef char water_physics_instance_size[(sizeof(s_water_physics_instance) == 0x2c) ? 1 : -1];
typedef char water_physics_structure_design_size[(sizeof(s_structure_design) == 0x40) ? 1 : -1];
