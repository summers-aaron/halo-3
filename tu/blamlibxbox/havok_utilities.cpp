#include "physics/havok_collision_filter.h"
#include "physics/runtime_node_to_havok_component_rigid_body_index_mapping.h"
// blamlibxbox/havok_utilities.obj: from the records 0xecea8 0xef648 0xef7d8 0x103258 0x252b80 0x255820 0x259e78 0x259f50 0x259fd0 0x25a160 0x25a2c8 0x25a3e8 0x25a478 0x25a5e8 0x25a7d0 0x25a988 0x25ac50 0x25ad90 0x25b3c8 0x25b608 0x25bbd0 0x25be68 0x25bf50 0x25bfb0 0x25c140 0x25c1d8 0x25cb30 0x25cda0 0x25ce40 0x25d0d8 0x25d210 0x25d8f8 0x25dac0 0x25dc48 0x25dd88
// @compiler xdk6021
// @flags /TP /O2 /Oc
#include "physics/collisions.h"
#include "memory/datum_index.h"
#include "memory/tls_data_arrays.h"
#include "memory/tls_globals.h"
#include "multithreading/thread_tls.h"
#include "memory/data_types.h"
#include "objects/objects.h"
#include "math/real_math_globals.h"
#include "math/real_constant_pool_access.h"
#include "cseries/cseries_types.h"
#include "math/matrix_math_types.h"
#include "objects/object_reference_types.h"
#include "objects/object_datum.h"
// @address 0x252b80 ??_Gc_point_collector@?BD@??havok_calculate_features_in_simple_shape_phantom@@9@UAAPAXI@Z
// @address 0x25bfb0 ?havok_ray_cast_in_aabb_phantom@@YA_NPAVphys_box_volume@@PAUs_havok_ray_test@@JJ@Z
// @address 0x63d0e8 ?trace_ray@phys_world@@QBAXABUphys_world_ray_input@@AAVphys_ray_hit_collector@@@Z
#include <xtl.h>
#include <xboxmath.h>
#include <float.h>
#include <ppcintrinsics.h>
#include "cseries/cseries.h"
#include "math/real_math.h"
#include "math/real_math_normalize_inline.h"
#include "game/game_time_inline.h"
#include "tag_files/tag_types.h"
#include "scenario/scenario.h"
#include "physics/havok_component.h"
#include "physics/havok_utilities.h"
#include "physics/physics_models.h"
#include "physics/havok.h"
#include "motor/motor_programs.h"
#include "physics/collision_bsp_types.h"
#include "physics/collision_bsp_cache_types.h"
#include "physics/collision_bsp_types.h"
#include "physics/runtime_node_rigid_body_indices_types.h"
#include "physics/memory_allocator_types.h"
#include "cache/cache_files.h"
#include "physics/havok_memory.h"
#include "game/materials.h"
#include "physics/collision_result_types.h"
#include "physics/havok_ray_test_types.h"
#include "physics/havok_shape_key.h"
#include "physics/havok_group_types.h"
#include "scenario/scenario_tag.h"
#include "physics/havok_component_class.h"
#include "physics/havok_math.h"
#include "physics/havok_constants_types.h"
#include "physics/havok_memory_pool.h"
#include "motor/motor_program_definition.h"
#include "math/real_math_types.h"
#include "phys/phys_thread_memory.h"
#include "phys/phys_memory_class.h"
#include "phys/phys_math.h"
#include "phys/phys_simplex_status.h"
#include "phys/phys_dynamics.h"
#include "phys/phys_collide.h"
#include "phys/phys_collision_proxy.h"
#include "physics/havok_memory_class.h"
#include "physics/havok_collision_surface_shape.h"
#include "phys/phys_simplex_types.h"
#include "phys/phys_triangle_shape.h"
#include "phys/phys_transform_shape.h"
#include "phys/phys_multi_sphere_shape.h"
#include "phys/phys_convex_hull_shape.h"
#include "phys/phys_typed_broadphase_handle_pair.h"
#include "phys/phys_broadphase.h"
#include "phys/phys_dispatcher.h"
#include "scenario/scenario_structure_bsp_reference.h"
#include "structures/structure_bsp_tag.h"
#include "phys/phys_group_filter.h"
#include "phys/phys_keyframe_util.h"
#include "objects/object_header.h"
#include "units/unit_types.h"
#include "game/game_time_types.h"
#include "tag_files/tag_groups.h"
#include "physics/physics_model_instance.h"
#include "physics/havok_component_access.h"
#include "objects/object_material_types.h"
#include "game/materials_definitions.h"
#include "units/biped_definitions.h"
#include "creatures/creature_definitions.h"
#include "math/matrix_math_functions.h"
#include <string.h>
#include <vectorintrinsics.h>
#include "phys/phys_base_types.h"
#include "physics/collision_test_flags.h"
#include <math.h>
class c_havok_component;
const float k_real_min = -FLT_MAX;

// @address 0x405dc ??_7c_collector@@6B@
// @address 0x25be10 ??_Ec_collector@@UAAPAXI@Z
#include "physics/havok_ray_collector.h"
#include "physics/runtime_node_to_physics_model_rigid_body_index_mapping.h"
#include "physics/havok_memory_globals.h"
#include "physics/character_physics_shapes.h"
#include "physics/havok_penetration_collector.h"
#include "math/real_constant_pool.h"
#include "physics/havok_vehicle_physics_internals.h"
#include "physics/havok_vehicle_physics.h"
struct s_havok_sphere_query : s_friction_vehicle_physics_point_datum {};
struct s_havok_sphere_test_cache {
    real_point3d center;
    real_plane3d plane;
    c_global_material_type material_type;
    unsigned char flags;
    bool valid;
    long rigid_body_index;
    long object_index;
    long surface_index;
};
// match: 0x25c1d8: retain the POD squared value and scalar wrapper lifetimes for matching stack packing.
struct hu_simd_real {
    __vector4 m_simd_scalar;
    __forceinline hu_simd_real(const float &value) { m_simd_scalar = __vspltw(__lvlx(&value, 0), 0); }
    __forceinline operator float() const { float value; __stvewx(m_simd_scalar, &value, 0); return value; }
};
struct hu_squared { union { __vector4 m_simd_scalar; float m_components[4]; }; };
#include "items/item_definitions.h"
// The item prefix and equipment duration fields are shared with equipment_definitions.h.
// Its +0x1b6 flags are still opaque in that header.
struct havok_utilities_group_definition : s_item_definition {
    float duration;
    float phantom_duration;
    short max_charges;
    short equipment_flags;
};
#include "units/vehicle_definitions.h"
#include "physics/collision_materials.h"
#include "structures/structure_instanced_geometry.h"
#include "structures/structure_instanced_geometry_definition.h"
#include "physics/collision_bsp_cache_types.h"

class phys_simple_shape_volume;
namespace {
static __forceinline void object_get_region_information(long object_index, long *region_count, signed char **permutations) {
    object_datum *object = (object_datum *)object_get_and_verify_type(object_index, _object_mask_all);
    *region_count = object->region_information.size;
    char *storage = (char *)object + object->region_information.offset;
    if ((bool)(_rotl(object->flags, 21) & 1)) *region_count -= 16;
    *region_count /= 10;
    *permutations = (signed char *)(storage + *region_count * 8);
}
}
static __forceinline void havok_utilities_deallocate_collector(void *block, int size)
{
    phys_thread_memory *memory = &phys_thread_memory::get_singleton_instance();
    if (g_havok_memory_globals.active_allocator == _memory_allocator_havok)
    {
        phys_memory *allocator = &g_havok_memory;
        allocator->free_chunk(block, size, PHYS_MEM_AGENT);
    }
    else
        memory->free_chunk_to_list(block, size);
}
__forceinline void phys_all_hits_collector::operator delete(void *p)
{
    havok_utilities_deallocate_collector(p, sizeof(phys_all_hits_collector));
}

// match: 0x259f50: the build inlines the inplace array destructor into its owning collector.
template phys_inline_array<phys_root_hit_point, 8>::~phys_inline_array();
// @address 0x259e78 ??_Gphys_all_hits_collector@@UAAPAXI@Z
// @address 0x259f50 ??1phys_all_hits_collector@@UAA@XZ
phys_all_hits_collector::~phys_all_hits_collector() {}

// @address 0x259fd0 ??0c_runtime_node_to_physics_model_rigid_body_index_mapping@@QAA@J_N@Z
c_runtime_node_to_physics_model_rigid_body_index_mapping::c_runtime_node_to_physics_model_rigid_body_index_mapping(long object_index, bool include_all)
{
    object_datum *object = (object_datum *)object_get_and_verify_type(object_index, _object_mask_all);
    s_object_material_definition *definition = (s_object_material_definition *)tag_get('obje', object->definition_index);
    long model_index = definition->model.index;
    m_node_count = 0;
    if (model_index != -1)
    {
        s_model_definition *model = (s_model_definition *)tag_get('hlmt', model_index);
        if (model->physics_model_index != -1)
        {
            s_physics_model_definition *physics = (s_physics_model_definition *)tag_get('phmo', model->physics_model_index);
            m_node_count = physics->nodes.count;
            for (long node_index = 0; node_index < m_node_count; ++node_index)
                entries[node_index].count = 0;
            entries[256].count = 0;
            long model_region_index;
            long model_permutation_index;
            for (long body_index = 0; body_index < physics->rigid_bodies.count(); ++body_index)
            {
                s_physics_model_rigid_body *body = (s_physics_model_rigid_body *)tag_block_get_element_with_size(
                    (const s_tag_block *)&physics->rigid_bodies, body_index, sizeof(s_physics_model_rigid_body));
                long node_index = body->node;
                s_runtime_node_rigid_body_indices *mapping = node_index == -1 ? &entries[256] : &entries[node_index];
                bool include = true;
                if (!include_all)
                {
                    calculate_model_region_and_permutation_from_physics_model_region_and_permutation(
                        object_index, model_index, body->region, body->permutation, &model_region_index, &model_permutation_index);
                    include = model_region_index != -1 && model_permutation_index != -1;
                }
                if (include)
                {
                    mapping->indices[mapping->count] = (signed char)body_index;
                    ++mapping->count;
                }
            }
        }
    }
}
// @address 0x25a160 ??0c_runtime_node_to_havok_component_rigid_body_index_mapping@@QAA@J@Z
c_runtime_node_to_havok_component_rigid_body_index_mapping::c_runtime_node_to_havok_component_rigid_body_index_mapping(long object_index)
{
    object_datum *object = (object_datum *)object_get_and_verify_type(object_index, _object_mask_all);
    s_object_material_definition *definition = (s_object_material_definition *)tag_get('obje', object->definition_index);
    long model_index = definition->model.index;
    m_node_count = 0;
    m_unmapped_rigid_body = -1;
    if (model_index != -1) {
        s_model_definition *model = (s_model_definition *)tag_get('hlmt', model_index);
        if (model->physics_model_index != -1) {
            s_physics_model_definition *physics = (s_physics_model_definition *)tag_get('phmo', model->physics_model_index);
            c_havok_component *component = havok_component_get(object->havok_component_index);
            m_node_count = physics->nodes.count;
            memset(m_node_to_rigid_body_mapping, -1, sizeof(m_node_to_rigid_body_mapping));
            for (long body = 0; body < component->get_rigid_body_count(); body++) {
                for (long child = 0; child < component->get_physics_model_rigid_body_index_count(body); child++) {
                    long physics_body = component->get_physics_model_rigid_body_index(body, child);
                    if (physics_body != -1) {
                        long node = physics->rigid_bodies.begin()[physics_body].node;
                        if (node != -1) m_node_to_rigid_body_mapping[node] = (signed char)body;
                        else m_unmapped_rigid_body = (signed char)body;
                    }
                }
            }
        }
    }
}
// @address 0x25a2c8 ?calculate_model_region_and_permutation_from_physics_model_region_and_permutation@@YAXJJJJPAJ0@Z
void calculate_model_region_and_permutation_from_physics_model_region_and_permutation(
    long object_index, long model_index, long physics_region, long physics_permutation, long *model_region, long *model_permutation)
{
    // match: 0x25a2c8: pointer __restrict qualifier prevents alias reload
    s_model_definition *__restrict model = (s_model_definition *)tag_get('hlmt', model_index);
    long region_count;
    signed char *permutations;
    object_get_region_information(object_index, &region_count, &permutations);
    *model_region = -1;
    *model_permutation = -1;
    for (long region_index = 0; region_index < model->regions.count; region_index++) {
        s_model_region *region = &((s_model_region *)model->regions.address)[region_index];
        if (region->physics_region_index == physics_region) {
            *model_region = region_index;
            if (region_index >= 0 && region_index < region_count && permutations[region_index] != -1) {
                s_model_region_permutation *permutation = (s_model_region_permutation *)tag_block_get_element_with_size(
                    (const s_tag_block *)&region->permutations, permutations[region_index], sizeof(s_model_region_permutation));
                if (permutation->physics_permutation_index == physics_permutation) {
                    *model_permutation = permutations[region_index];
                    break;
                } else break;
            } else break;
        } else continue;
    }
}
static __forceinline void havok_utilities_vector4_from_real_vector3d(phys_vec4 *v, const real_vector3d *direction)
{
    phys_simd_vec_union components;
    components.r[0] = direction->i;
    components.r[1] = direction->j;
    components.r[2] = direction->k;
    components.r[3] = 0.0f;
    v->m_simd_quad = components.q;
}
// @address 0x25a3e8 ?phys_mat3_from_matrix3x3@@YAXPAVphys_mat3@@PBUreal_matrix3x3@@@Z
void phys_mat3_from_matrix3x3(class phys_mat3 * _sym_p0, struct real_matrix3x3 const * matrix)
{
    havok_utilities_vector4_from_real_vector3d(&_sym_p0->m_column0, &matrix->forward);
    havok_utilities_vector4_from_real_vector3d(&_sym_p0->m_column1, &matrix->left);
    havok_utilities_vector4_from_real_vector3d(&_sym_p0->m_column2, &matrix->up);
}
namespace {
static __forceinline const s_character_physics_shape_properties *phys_shape_get_primitive(const phys_shape *shape) {
    return (const s_character_physics_shape_properties *)shape->get_user_data();
}
}
static __forceinline float havok_utilities_ray_dot_product3d(const real_vector3d *a, const real_vector3d *b) {
    return a->i * b->i + (a->j * b->j + a->k * b->k);
}
static __forceinline void havok_utilities_ray_normal_from_hkVector4(real_vector3d *normal, const phys_vec4 *vector) {
    normal->i = (*vector)(0); normal->j = (*vector)(1); normal->k = (*vector)(2);
}
// match: 0x25bbd0: retain the original entity-test branch boundary in this collector.
static __forceinline phys_rigid_body *havok_utilities_get_rigid_body(const phys_collider *collidable)
{
    if (collidable->get_type_id() == 1) return (phys_rigid_body *)collidable->get_collider_owner();
    else return 0;
}
// @address 0x25bbd0 ?add_ray_hit@c_collector@@UAAXABVphys_collision_proxy@@ABUphys_ray_collector_output@@@Z
void c_collector::add_ray_hit(const phys_collision_proxy &body, const phys_ray_collector_output &output)
{
    if (output.has_hit()) {
        float fraction = output.m_hit_frac;
        if (fraction < m_ray_test->fraction && havok_utilities_get_rigid_body(body.get_root_collider())) {
            phys_rigid_body *ignored_body = m_context != -1 ? havok_utilities_get_rigid_body(body.get_root_collider()) : 0;
            long ignored_component =
                ignored_body ? ::havok_entity_get_havok_component_index(ignored_body) : -1;
            if (ignored_component == -1 ||
                (::havok_entity_get_havok_component_index(ignored_body) != m_context &&
                    ::havok_entity_get_havok_component_index(ignored_body) != m_hit_index))
            {
                real_vector3d normal;
                havok_utilities_ray_normal_from_hkVector4(&normal, &output.m_normal);
                if (havok_utilities_ray_dot_product3d(&m_ray_test->vector, &normal) < 0.0f) {
                    phys_rigid_body *rigid_body = havok_utilities_get_rigid_body(body.get_root_collider());
                    m_hit_found = true;
                    m_ray_test->fraction = fraction;
                    m_ray_test->normal = normal;
                    m_ray_test->flags = 0;
                    m_ray_test->object_index = -1;
                    m_early_out_hit_frac = fraction;
                    if (rigid_body) {
                        long component_index = ::havok_entity_get_havok_component_index(rigid_body);
                        if (component_index != -1) m_ray_test->object_index = havok_component_get(component_index)->m_object_index;
                    }
                    if (body.get_shape()->get_type_id() == 30)
                        m_ray_test->flags = ((const c_collision_surface_shape *)body.get_shape())->m_surface_flags;
                }
            }
        }
    }
}
// @address 0x25be68 ?havok_ray_cast_in_world@@YA_NPAUs_havok_ray_test@@JJJ@Z
bool havok_ray_cast_in_world(s_havok_ray_test *ray_test, long filter_info, long context, long hit_index)
{
    phys_world_ray_input input;
    real_point3d ray_end;
    input.m_compound_filter_enabled = true;
    input.m_filter_info = filter_info;
    phys_vec4_from_real_point3d(&input.m_ray_from, &ray_test->start);
    point_from_line3d(&ray_test->start, &ray_test->vector, 1.0f, &ray_end);
    phys_vec4_from_real_point3d(&input.m_ray_to, &ray_end);
    c_collector collector(ray_test, context, hit_index);
    g_havok_world->trace_ray(input, collector);
    return collector.m_hit_found;
}
// @address 0x25bf50 ?havok_ray_cast_in_aabb_phantom@@YA_NJPAUs_havok_ray_test@@@Z
bool havok_ray_cast_in_aabb_phantom(long component_index, s_havok_ray_test *ray_test)
{
    bool result = false;
    if (component_index != -1) {
        c_havok_component *component = havok_component_get(component_index);
        phys_box_volume *phantom = (bool)(_rotl(component->m_flags, 29) & 1) ? component->m_aabb_phantom : 0;
        if (phantom) result = havok_ray_cast_in_aabb_phantom(phantom, ray_test, -1, -1);
    }
    return result;
}
bool havok_ray_cast_in_aabb_phantom(phys_box_volume *aabb_phantom, s_havok_ray_test *ray_test, long context, long unused_context)
{
    const phys_aabb &aabb = aabb_phantom->get_handle_aabb();
    phys_world_ray_input input;
    real_point3d ray_end;
    point_from_line3d(&ray_test->start, &ray_test->vector, 1.0f, &ray_end);
    c_collector collector(ray_test, context, -1);
    phys_vec4_from_real_point3d(&input.m_ray_from, &ray_test->start);
    phys_vec4_from_real_point3d(&input.m_ray_to, &ray_end);
    input.m_compound_filter_enabled = true;
    input.m_filter_info = aabb_phantom->get_collider()->get_collision_filter_info();
    for (long axis = 0; axis < 3; axis++)
    {
        input.m_ray_from(axis) = PIN(input.m_ray_from(axis), aabb.m_min_limit(axis), aabb.m_max_limit(axis));
        input.m_ray_to(axis) = PIN(input.m_ray_to(axis), aabb.m_min_limit(axis), aabb.m_max_limit(axis));
    }
    aabb_phantom->trace_ray(input, collector);
    return collector.m_hit_found;
}
// @address 0x25cda0 ?add_proxy_pair@c_penetration_collector@@UAAXABVphys_collision_proxy@@0@Z
void c_penetration_collector::add_proxy_pair(const phys_collision_proxy &body_a, const phys_collision_proxy &body_b)
{
    phys_rigid_body *rigid_body = phys_get_rigid_body(body_b.get_root_collider());
    if (rigid_body) {
        bool ignored = false;
        if (body_b.get_shape()->get_type_id() != 30 && body_b.get_shape()->get_user_data() != 0 &&
            (body_b.get_shape()->get_user_data() < 1 || body_b.get_shape()->get_user_data() > 16)) {
            const s_character_physics_shape_properties *primitive = phys_shape_get_primitive(body_b.get_shape());
            ignored = primitive->phantom_index + 1 != 0;
        }
        if (!ignored) m_early_out = true;
    }
}
static __forceinline void havok_utilities_transform_copy(phys_transform *out, const phys_transform &in)
{
    __vector4 col0 = in.m_rotation.m_column0.m_simd_quad;
    __vector4 col1 = in.m_rotation.m_column1.m_simd_quad;
    __vector4 col2 = in.m_rotation.m_column2.m_simd_quad;
    __vector4 translation = in.m_translation.m_simd_quad;
    out->m_rotation.m_column0.m_simd_quad = col0;
    out->m_rotation.m_column1.m_simd_quad = col1;
    out->m_rotation.m_column2.m_simd_quad = col2;
    out->m_translation.m_simd_quad = translation;
}

struct s_render_list;
// @address 0xecea8 ?havok_object_halt_keyframed_rigid_bodies@@YAXJ@Z
void havok_object_halt_keyframed_rigid_bodies(long object_index)
{
    long component_index = ((object_datum *)object_get_and_verify_type(object_index, -1))->havok_component_index;
    if (component_index != -1) {
        c_havok_component *component = havok_component_get(component_index);
        long body_index = 0;
        if (0 < component->m_rigid_bodies.m_size) {
            do {
                c_havok_component::c_rigid_body *body = &component->m_rigid_bodies.m_data[body_index];
                if (body->m_rigid_body->get_motion_type() == 6) {
                    if (component->m_rigid_bodies.m_data[body_index].m_rigid_body->is_body_active()) {
                        component->set_rigid_body_linear_velocity(body_index,
                            global_zero_vector3d);
                        component->set_rigid_body_angular_velocity(body_index,
                            global_zero_vector3d);
                    }
                }
                body_index = body_index + 1;
            } while (body_index < component->m_rigid_bodies.m_size);
        }
    }
}
// @address 0x103258 ?phys_transform_from_matrix4x3@@YAXPAVphys_transform@@PBUreal_matrix4x3@@@Z
void phys_transform_from_matrix4x3(phys_transform *transform, const real_matrix4x3 *matrix)
{
    ::phys_mat3_from_matrix3x3((phys_mat3 *)(phys_rotation *)transform, (real_matrix3x3 const *)&matrix->rotation);
    phys_simd_vec_union translation;
    translation.r[0] = matrix->position.x;
    translation.r[1] = matrix->position.y;
    translation.r[2] = matrix->position.z;
    translation.r[3] = 0.0f;
    transform->m_translation.m_simd_quad = translation.q;
}
extern "C" {
// @address 0x40450 k_havok_penetration_collector_vtable
}
static __forceinline void havok_utilities_cast_vector_zero(phys_vec4 *v, const real_vector3d *p, float zero)
{
    phys_simd_vec_union u;
    u.r[0] = p->i;
    u.r[1] = p->j;
    u.r[2] = p->k;
    u.r[3] = zero;
    v->m_simd_quad = u.q;
}
static __forceinline void havok_utilities_cast_copy(real_vector3d *dest, const phys_vec4 *src) {
    float x=(*src)(0), y=(*src)(1), z=(*src)(2);
    dest->i=x; dest->j=y; dest->k=z;
}
static __forceinline void havok_utilities_cast_copy_index(real_vector3d *dest, const phys_vec4 *src, long *index, long i) {
    float x=(*src)(0);
    *index=i;
    float y=(*src)(1), z=(*src)(2);
    dest->i=x; dest->j=y; dest->k=z;
}
// @address 0x255820 havok_ray_cast
bool havok_ray_cast(c_havok_component *object, long, long, const real_matrix4x3 *matrix, const real_vector3d *offset,
    // match: 0x255820: pointer __restrict qualifier prevents alias reload
    long, long *__restrict index, real_matrix4x3 *result)
{
    bool found = false;
    phys_rigid_body *body = object->m_rigid_bodies.m_data->m_rigid_body;
    phys_transform initial;
    phys_transform_from_matrix4x3(&initial, matrix);
    void *thread_memory = &phys_thread_memory::get_singleton_instance();
    void *allocation = ((phys_thread_memory *)thread_memory)->allocate_chunk(sizeof(phys_simple_shape_volume), (phys_memory_class)47);
    ((phys_ref_object *)allocation)->m_mem_size_and_flags = sizeof(phys_simple_shape_volume);
    int rfT0 = object->m_object_index + 1;
    phys_simple_shape_volume *phantom = new (allocation) phys_simple_shape_volume(body->get_collider()->get_shape(), initial, ((rfT0) << 16) | 2);
    const float zero = real_constant_pool_zero();
    phys_vec4 direction;
    havok_utilities_cast_vector_zero(&direction, offset, zero);
    g_havok_world->add_volume((phys_volume *)phantom);
    c_penetration_collector collector;
    const real_matrix3x3 *rotation = &matrix->rotation;
    const float step = 0.33333334f;
    for (long i = 0; i < 3; ++i) {
        phys_transform transform;
        havok_utilities_cast_vector_zero(&transform.m_rotation.m_column0, &rotation->forward, zero);
        havok_utilities_cast_vector_zero(&transform.m_rotation.m_column1, &rotation->left, zero);
        havok_utilities_cast_vector_zero(&transform.m_rotation.m_column2, &rotation->up, zero);
        {
            phys_simd_vec_union u;
            u.r[0] = matrix->position.x; u.r[1] = matrix->position.y;
            u.r[2] = matrix->position.z; u.r[3] = zero;
            transform.m_translation.m_simd_quad = u.q;
        }
        if (i > 0) {
            float fraction = (float)i * step;
            transform.m_translation.m_simd_quad = __vmaddfp(direction.m_simd_quad, __vspltw(__lvlx(&fraction, 0), 0), transform.m_translation.m_simd_quad);
        }
        collector.reset();
        phantom->set_transform(transform);
        phantom->get_penetrations(collector);
        if (!collector.get_early_out() &&
            !collision_test_point(k_placement_test_collision_flags, real_point3d_from_hkVector4_in_place(&transform.m_translation),
                object->m_object_index, -1, 0))
        {
            result->scale = real_constant_pool_one();
            found = true;
            real_matrix3x3 *out_rotation = &result->rotation;
            havok_utilities_cast_copy_index(&out_rotation->forward, &transform.m_rotation.m_column0, index, i);
            havok_utilities_cast_copy(&out_rotation->left, &transform.m_rotation.m_column1);
            havok_utilities_cast_copy(&out_rotation->up, &transform.m_rotation.m_column2);
            result->position.x = transform.m_translation(0);
            result->position.y = transform.m_translation(1);
            result->position.z = transform.m_translation(2);
            break;
        }
    }
    ((phys_world *)g_havok_world)->remove_volume((phys_volume *)phantom);
    ((phys_world_object *)phantom)->object_remove_ref();
    return found;
}
// @address 0x25dac0 ?havok_convex_shapes_collide@@YA_NPBVphys_convex_shape@@0MPATreal_vector3d@@PAM@Z
bool havok_convex_shapes_collide(const phys_convex_shape *shape_a, const phys_convex_shape *shape_b, float tolerance, real_vector3d *normal, float *distance)
{
    bool result = false;
    phys_transform transform;
    phys_extended_simplex_out output;
    transform.set_to_identity();
    phys_simplex_cache cache;
    cache.init(shape_a, shape_b, transform);
    phys_simplex gsk;
    phys_vec4 separating_normal;
    gsk.init(shape_a, shape_b, cache);
    gsk.get_closest_feature(shape_a, shape_b,
        (transform), (separating_normal));
    gsk.exit_and_export_cache(cache);
    gsk.feature_to_closest_distance(separating_normal, output);
    float separation = output.m_distance - shape_a->get_convex_radius() - shape_b->get_convex_radius();
    result = separation < tolerance;
    if (distance) *distance = separation;
    if (normal) real_vector3d_from_hkVector4(normal, &output.m_normal_in_a);
    return result;
}
// @address 0x25dc48 ?havok_object_compute_rigid_body_index_of_node@@YAJJJ@Z
long havok_object_compute_rigid_body_index_of_node(long object_index, long node_index)
{
    long best_node = -1;
    long result = -1;
    if (node_index != -1) {
        object_datum *object = (object_datum *)object_get_and_verify_type(object_index, _object_mask_all);
        if (object->definition_index != -1) {
            s_object_material_definition *definition = (s_object_material_definition *)tag_get('obje', object->definition_index);
            if (definition->model.index != -1) {
                s_model_definition *model = (s_model_definition *)tag_get('hlmt', definition->model.index);
                if (model->physics_model_index != -1) {
                    s_physics_model_definition *physics = (s_physics_model_definition *)tag_get('phmo', model->physics_model_index);
                    if (object->havok_component_index != -1) {
                        c_havok_component *component = havok_component_get(object->havok_component_index);
                        for (long body = 0; body < component->get_rigid_body_count(); body++) {
                            for (long child = 0; child < component->get_physics_model_rigid_body_index_count(body); child++) {
                                long physics_body = component->get_physics_model_rigid_body_index(body, child);
                                s_physics_model_rigid_body *rigid_body = &physics->rigid_bodies.begin()[physics_body];
                                if (rigid_body->node != -1 && (best_node == -1 || (rigid_body->node <= node_index && rigid_body->node > best_node))) {
                                    best_node = rigid_body->node;
                                    result = body;
                                }
                            }
                        }
                    }
                }
            }
        }
    }
    return result;
}

struct s_model;
struct s_body_world;
// @address 0x25a478 ?calculate_localized_physics_space_linear_velocity@@YA_NJJPBTreal_point3d@@PBUreal_matrix4x3@@PAU2@PATreal_vector3d@@PIAT3@3PAUreal_matrix3x3@@@Z
bool calculate_localized_physics_space_linear_velocity(long havok_component_index, long rigid_body_index, const real_point3d *point,
    // match: 0x25a478: pointer __restrict qualifier prevents alias reload
    const real_matrix4x3 *matrix, real_matrix4x3 *out_matrix, real_vector3d *origin, real_vector3d *__restrict linear_velocity,
    real_vector3d *linear_velocity_delta, real_matrix3x3 *rotation)
{
    bool result = false;
    if (havok_component_index != -1) {
        c_havok_component *model = havok_component_get(havok_component_index);
        long selected = model->m_object_postion_controlling_rigid_body_index;
        if (selected < 0 || selected >= model->m_rigid_bodies.m_size) selected = -1;
        short node_index = selected;
        if (node_index >= 0 && node_index < model->m_rigid_bodies.m_size) {
            real_matrix4x3 node_matrix;
            real_matrix4x3 inverse;
            real_matrix4x3 relative;
            real_vector3d velocity;
            model->get_rigid_body_transform(node_index, (real_matrix4x3 *)&node_matrix);
            matrix4x3_inverse(&node_matrix, &inverse);
            matrix4x3_compute_product(matrix, &inverse, &relative);
            matrix4x3_inverse(&relative, &relative);
            *rotation = relative.rotation;
            model->get_rigid_body_linear_velocity_at_point(node_index, &velocity, point);
            linear_velocity_delta->i = velocity.i - origin->i;
            linear_velocity_delta->j = velocity.j - origin->j;
            linear_velocity_delta->k = velocity.k - origin->k;
            *linear_velocity = velocity;
            *origin = velocity;
            *out_matrix = node_matrix;
            result = true;
        }
    }
    return result;
}

#define HAVOK_UTILITIES_SIGN(x) ((x) != 0.0f ? ((x) < 0.0f ? -1 : 1) : 0)

static __forceinline float havok_utilities_power(float x, float y)
{
    return (float)pow((double)x, (double)y);
}
static __forceinline float havok_utilities_braking_velocity(float distance, float acceleration)
{
    return sqrtf(2.0f * distance * acceleration);
}

// @address 0x25dd88 ?calculate_acceleration_magnitude@@YAM_N0MMMMMMM0PAM@Z
float calculate_acceleration_magnitude(bool clamp_to_infinity, bool scale_by_time, float time, float target, float acceleration,
    float velocity, float max_velocity, float friction, float exponent, bool instant, float *scale)
{
    float target_magnitude = (float)fabs(target);
    float acceleration_scaled = exponent > 0.001f ? acceleration * havok_utilities_power(target_magnitude, exponent) : acceleration;
    float acceleration_tick = scale_by_time && time > 0.001f ? (acceleration_scaled / time) * game_tick_length()
                                                             : game_tick_length() * acceleration_scaled;
    float acceleration_magnitude = acceleration_tick >= 0.0f ? acceleration_tick : -acceleration_tick;
    float velocity_tick = game_tick_length() * velocity;
    float result = velocity;
    target -= HAVOK_UTILITIES_SIGN(target) * friction;
    if (acceleration_tick > 0.001f) {
        float ticks = (float)fabs(velocity) / acceleration_magnitude + 1.0f;
        float stopping_distance = (float)fabs(0.5f * ticks * velocity_tick);
        if (instant) {
            result += acceleration_tick;
        } else if (stopping_distance < target_magnitude) {
            result += (acceleration_magnitude > target_magnitude ? target_magnitude : acceleration_magnitude) * HAVOK_UTILITIES_SIGN(target);
        } else {
            result = havok_utilities_braking_velocity(target_magnitude, acceleration_magnitude) * HAVOK_UTILITIES_SIGN(velocity);
        }
        result = PIN(result, clamp_to_infinity ? k_real_min : velocity, max_velocity);
    } else {
        result = MIN(MAX(result + acceleration_tick, -max_velocity), clamp_to_infinity ? k_real_max : velocity);
    }
    float abs_max = max_velocity >= 0.0f ? max_velocity : -max_velocity;
    *scale = abs_max > 0.001f ? (velocity / max_velocity >= 0.0f ? velocity / max_velocity : -(velocity / max_velocity)) : 0.0f;
    return PIN(result - velocity, -acceleration_magnitude, acceleration_magnitude);
}

namespace {
static void hu_transform_set_identity(phys_transform *transform);
static void hu_vector4_from_point(phys_vec4 *v, const real_point3d *origin);
}
static __forceinline bool hu_component_get_rigid_body_is_fixed(const c_havok_component *component, long index)
{
    return component->m_rigid_bodies.m_data[index].m_rigid_body->get_motion_type() == 7;
}
static __forceinline const phys_collider *hu_component_get_rigid_body_collidable(const c_havok_component *component, long index)
{
    return component->m_rigid_bodies.m_data[index].m_rigid_body->get_collider();
}
class phys_broadphase;
// @address 0x25cb30 ?calculate_closest_point_and_normal_on_havok_component@@YA_NJPBTreal_point3d@@PAT1@PATreal_vector3d@@@Z
bool calculate_closest_point_and_normal_on_havok_component(long component_index, const real_point3d *origin, real_point3d *closest_point, real_vector3d *normal)
{
    phys_collision_input input = *g_havok_world->get_collision_input();
    bool found = false;
    c_havok_component *component = havok_component_get(component_index);
    if ((bool)(_rotl(component->m_flags, 29) & 1)) {
        phys_sphere_shape sphere(0.05f);
        phys_transform transform;
        hu_transform_set_identity(&transform);
        hu_vector4_from_point(&transform.m_translation, origin);
        phys_simple_shape_volume phantom(&sphere, transform, 0);
        const phys_collider *phantom_collidable = phantom.get_collider();
        const phys_shape *phantom_shape = phantom_collidable->get_shape();
        float closest_distance = k_real_max;
        input.m_box_tolerance = closest_distance;
        for (long index = 0; index < component->get_rigid_body_count(); index++) {
            if (!hu_component_get_rigid_body_is_fixed(component, index)) {
                phys_all_hits_collector collector;
                const phys_collider *collidable = hu_component_get_rigid_body_collidable(component, index);
                phys_dispatcher::closest_points_fn func = input.m_agent_dispatcher->get_closest_points_fn(phantom_shape->get_type_id(),
                    collidable->get_shape()->get_type_id());
                func(*phantom_collidable, *collidable, input, collector);
                for (long hit = 0; hit < collector.get_hits().m_size; hit++) {
                    const phys_root_hit_point *point = &collector.get_hits().m_data[hit];
                    if (point->m_hit_contact.m_separating_normal(3) < closest_distance) {
                        closest_distance = point->m_hit_contact.m_separating_normal(3);
                        found = true;
                        float px = point->m_hit_contact.m_position(0);
                        float pz = point->m_hit_contact.m_position(2);
                        float py = point->m_hit_contact.m_position(1);
                        closest_point->x = px;
                        closest_point->y = py;
                        closest_point->z = pz;
                        float nx = point->m_hit_contact.m_separating_normal(0);
                        float nz = point->m_hit_contact.m_separating_normal(2);
                        float ny = point->m_hit_contact.m_separating_normal(1);
                        normal->i = nx;
                        normal->j = ny;
                        normal->k = nz;
                    }
                }
            }
        }
    }
    return found;
}
namespace {
static void hu_transform_set_identity(phys_transform *transform)
{
    phys_vec4 zero;
#if defined(_XBOX) && !defined(__clang__)
    zero.m_simd_quad = __vxor(zero.m_simd_quad, zero.m_simd_quad);
    transform->m_translation.m_simd_quad = __vxor(transform->m_translation.m_simd_quad, transform->m_translation.m_simd_quad);
#else
    zero.m_simd_quad = __vzero();
    transform->m_translation.m_simd_quad = __vzero();
#endif
    transform->m_rotation.m_column0 = zero;
    transform->m_rotation.m_column0(0) = 1.0f;
    transform->m_rotation.m_column1 = zero;
    transform->m_rotation.m_column1(1) = 1.0f;
    transform->m_rotation.m_column2 = zero;
    transform->m_rotation.m_column2(2) = 1.0f;
}
static void hu_vector4_from_point(phys_vec4 *v, const real_point3d *origin)
{
    phys_simd_vec_union components;
    components.r[0] = origin->x;
    components.r[1] = origin->y;
    components.r[3] = 0.0f;
    components.r[2] = origin->z;
    v->m_simd_quad = components.q;
}
}

static __forceinline void havok_utilities_vector4_set(phys_vec4 *v, float x, float y, float z, float w)
{
    phys_simd_vec_union components;
    components.r[0] = x;
    components.r[1] = y;
    components.r[2] = z;
    components.r[3] = w;
    v->m_simd_quad = components.q;
}
// @address 0x25a7d0 havok_update_bodies_in_bounds
extern "C" void havok_update_bodies_in_bounds(unsigned long filter_info, const real_rectangle3d *bounds)
{
    const phys_group_filter *filter = static_cast<const phys_group_filter *>(g_havok_world->get_collision_filter());
    phys_aabb aabb;
    phys_inline_array<phys_broadphase_handle_pair, 2048> pairs;
    havok_utilities_vector4_set(&aabb.m_min_limit, bounds->x0, bounds->y0, bounds->z0, 0.0f);
    havok_utilities_vector4_set(&aabb.m_max_limit, bounds->x1, bounds->y1, bounds->z1, 0.0f);
    phys_broadphase *broad_phase = g_havok_world->get_broadphase();
    broad_phase->query_single_aabb(aabb, pairs);
    for (long index = 0; index < pairs.m_size; index++) {
        phys_collider *collidable = (phys_collider *)((const phys_typed_broadphase_handle *)pairs.m_data[index].m_b)->get_collider_owner();
        phys_rigid_body *rigid_body = phys_get_rigid_body(collidable);
        if (rigid_body && filter->allows_collision(rigid_body->get_collider()->get_collision_filter_info(), filter_info)) {
            long component_index = ::havok_entity_get_havok_component_index(rigid_body);
            if (component_index != -1)
                havok_component_activate_rigid_bodies(havok_component_get(component_index), 1);
        }
    }
    havok_memory_garbage_collect_locked();
}

typedef char sphere_query_marker_offset[(offsetof(s_havok_sphere_query, marker_position) == 52) ? 1 : -1];
typedef char sphere_query_size[(sizeof(s_havok_sphere_query) == 236) ? 1 : -1];


static __forceinline void havok_utilities_point_from_line(const real_point3d *origin, const real_vector3d *direction, float scale, real_point3d *result)
{
    float i = direction->i;
    float k = direction->k;
    float j = direction->j;
    result->x = origin->x + i * scale;
    result->y = origin->y + j * scale;
    result->z = origin->z + k * scale;
}

namespace {
static __forceinline hu_simd_real hu_length3(const phys_vec4 &v)
{
    phys_simd_vec_union length_squared;
    length_squared.q = __vmsum3fp(v.m_simd_quad, v.m_simd_quad);
    return hu_simd_real(sqrtf(length_squared.r[0]));
}
static __forceinline hu_squared hu_length_squared3(const phys_vec4 &v) { hu_squared r; r.m_simd_scalar = __vmsum3fp(v.m_simd_quad, v.m_simd_quad); return r; }
static __forceinline hu_simd_real hu_sqrt(const hu_squared &r) { return hu_simd_real(sqrtf(r.m_components[0])); }
#define HU_MIN(a, b) ((a) > (b) ? (b) : (a))
static __forceinline bool hu_is_shape_collection(const phys_shape *shape)
{
    bool result = false;
    if (shape->get_type_id() == 2 || shape->get_type_id() == 10 || shape->get_type_id() == 18)
        result = true;
    return result;
}
}

// @address 0x25c1d8 ?calculate_bounding_sphere_recursive@@YAMPBVphys_shape@@MABVphys_transform@@PAM2PAJ3@Z
float calculate_bounding_sphere_recursive(const phys_shape *shape, float radius, const phys_transform &transform, float *minimum_size,
    float *maximum_size, long *shape_count, long *empty_count)
{
    phys_transform shape_transform;
    havok_utilities_transform_copy(&shape_transform, transform);
    phys_transform child_transform;
    phys_transform convex_child_transform;
    phys_transform translated_transform;
    __declspec(align(16)) char buffer[512];
    if (shape->get_type_id() == 28) {
        const phys_transform_shape *transform_shape = (const phys_transform_shape *)shape;
        child_transform.set_mul(shape_transform, transform_shape->get_transform());
        radius = calculate_bounding_sphere_recursive(
            transform_shape->get_child_shape(), radius, child_transform, minimum_size, maximum_size, shape_count, empty_count);
    } else if (shape->get_type_id() == 13) {
        const phys_convex_xform_shape *transform_shape = (const phys_convex_xform_shape *)shape;
        convex_child_transform.set_mul(shape_transform, transform_shape->get_transform());
        radius = calculate_bounding_sphere_recursive(
            transform_shape->get_child_shape(), radius, convex_child_transform, minimum_size, maximum_size, shape_count, empty_count);
    } else if (shape->get_type_id() == 12) {
        const phys_convex_offset_shape *translate_shape = (const phys_convex_offset_shape *)shape;
        translated_transform.m_rotation.m_column0 = shape_transform.m_rotation.m_column0;
        translated_transform.m_rotation.m_column1 = shape_transform.m_rotation.m_column1;
        translated_transform.m_rotation.m_column2 = shape_transform.m_rotation.m_column2;
        translated_transform.m_translation = shape_transform.m_translation;
        translated_transform.m_translation.m_simd_quad = __vaddfp(translated_transform.m_translation.m_simd_quad,
            translate_shape->get_translation().m_simd_quad);
        radius = calculate_bounding_sphere_recursive(
            translate_shape->get_child_shape(), radius, translated_transform, minimum_size, maximum_size, shape_count, empty_count);
    } else if (shape->get_type_id() == 16) {
        const phys_multi_sphere_shape *multi_sphere = (const phys_multi_sphere_shape *)shape;
        for (long sphere_index = 0; sphere_index < multi_sphere->m_sphere_count; sphere_index++) {
            const phys_vec4 *sphere = &multi_sphere->m_sphere_array[sphere_index];
            float sphere_radius = (float)hu_length3(*sphere) + (*sphere)(3);
            radius = sphere_radius > radius ? sphere_radius : radius;
            *minimum_size = *minimum_size > 2.0f * sphere_radius ? 2.0f * sphere_radius : *minimum_size;
            *maximum_size = *maximum_size > 2.0f * sphere_radius ? *maximum_size : 2.0f * sphere_radius;
            (*shape_count)++;
        }
    } else if (hu_is_shape_collection(shape)) {
        const phys_shape_container *container = (const phys_compound_shape *)shape;
        const phys_compound_shape *collection = (const phys_compound_shape *)shape;
        for (long key = collection->get_first_key(); key != -1; key = collection->get_next_key(key)) {
            const phys_shape *child = collection->get_child_shape(key, buffer);
            float child_radius = calculate_bounding_sphere_recursive(child, radius, shape_transform, minimum_size, maximum_size, shape_count, empty_count);
            radius = child_radius > radius ? child_radius : radius;
        }
    } else if (shape->get_type_id() == 27) {
        radius = calculate_bounding_sphere_recursive(
            ((const phys_bvh_tree_shape *)shape)->get_single_child(), radius, shape_transform, minimum_size, maximum_size, shape_count, empty_count);
    } else if (shape->get_type_id() == 25) {
        radius = 0.0f;
        (*empty_count)++;
    } else {
        (*shape_count)++;
        const phys_convex_shape *convex = (const phys_convex_shape *)shape;
        switch (shape->get_type_id() - 3) {
        case 0:
            radius = convex->get_convex_radius() + (float)hu_length3(shape_transform.m_translation);
            *minimum_size = *minimum_size > convex->get_convex_radius() ? convex->get_convex_radius() : *minimum_size;
            *maximum_size = *maximum_size > 2.0f * convex->get_convex_radius() ? *maximum_size : 2.0f * convex->get_convex_radius();
            break;
        case 2: {
            float vertex_radius = 0.0f;
            float triangle_radius = convex->get_convex_radius();
            for (long vertex_index = 0; vertex_index < 3; vertex_index++) {
                float distance = (float)hu_length3(((const phys_triangle_shape *)shape)->get_vertex(vertex_index)) + triangle_radius;
                vertex_radius = vertex_radius > distance ? vertex_radius : distance;
            }
            radius = vertex_radius + (float)hu_length3(shape_transform.m_translation);
            float size = 2.0f * convex->get_convex_radius();
            *minimum_size = *minimum_size > size ? size : *minimum_size;
            *maximum_size = *maximum_size > size ? *maximum_size : size;
            break;
        }
        case 3: {
            const phys_vec4 &half_extents = ((const phys_box_shape *)shape)->get_half_extents();
            hu_squared extents_length_squared = hu_length_squared3(half_extents);
            hu_squared translation_length_squared = hu_length_squared3(shape_transform.m_translation);
            float extents_length = (float)hu_sqrt(extents_length_squared);
            float translation_length = (float)hu_sqrt(translation_length_squared);
            float size = 2.0f * (convex->get_convex_radius() + HU_MIN(((const phys_box_shape *)shape)->get_half_extents()(0),
                HU_MIN(((const phys_box_shape *)shape)->get_half_extents()(1), ((const phys_box_shape *)shape)->get_half_extents()(2))));
            radius = extents_length + translation_length;
            *minimum_size = *minimum_size > size ? size : *minimum_size;
            *maximum_size = *maximum_size > size ? *maximum_size : size;
            break;
        }
        case 4: {
            phys_vec4 axis;
            axis.m_simd_quad = __vsubfp(((const phys_capsule_shape *)shape)->get_vertex(0).m_simd_quad,
                ((const phys_capsule_shape *)shape)->get_vertex(1).m_simd_quad);
            float capsule_radius = convex->get_convex_radius();
            radius = (float)hu_sqrt(hu_length_squared3(axis)) * 0.5f + (float)hu_sqrt(hu_length_squared3(shape_transform.m_translation)) + capsule_radius;
            *minimum_size = *minimum_size > 2.0f * capsule_radius ? 2.0f * capsule_radius : *minimum_size;
            *maximum_size = *maximum_size > 2.0f * capsule_radius ? *maximum_size : 2.0f * capsule_radius;
            break;
        }
        case 5: {
            phys_vec4 center;
            center.m_simd_quad = ((const phys_convex_hull_shape *)shape)->m_hull_aabb_center.m_simd_quad;
            float size = 2.0f * (convex->get_convex_radius() + HU_MIN(((const phys_convex_hull_shape *)shape)->m_aabb_half_extents(0),
                HU_MIN(((const phys_convex_hull_shape *)shape)->m_aabb_half_extents(1), ((const phys_convex_hull_shape *)shape)->m_aabb_half_extents(2))));
            center.m_simd_quad = __vaddfp(center.m_simd_quad, shape_transform.m_translation.m_simd_quad);
            float center_length;
            float extents_length;
            float *center_output = &center_length;
            hu_squared extents_squared = hu_length_squared3(((const phys_convex_hull_shape *)shape)->m_aabb_half_extents);
            hu_squared center_squared = hu_length_squared3(center);
            __stvewx(hu_sqrt(extents_squared).m_simd_scalar, &extents_length, 0);
            __stvewx(hu_sqrt(center_squared).m_simd_scalar, center_output, 0);
            radius = extents_length + *center_output;
            *minimum_size = *minimum_size > size ? size : *minimum_size;
            *maximum_size = *maximum_size > size ? *maximum_size : size;
            break;
        }
        case PHYS_SHAPE_BVH_TREE: {
            phys_transform identity;
            phys_aabb aabb;
            phys_vec4 extents;
            identity.set_to_identity();
            shape->get_aabb_impl(identity, 0.0f, aabb);
            extents.m_simd_quad = __vsubfp(aabb.m_max_limit.m_simd_quad, aabb.m_min_limit.m_simd_quad);
            float size = MAX(extents(0), MAX(extents(1), extents(2)));
            *minimum_size = *minimum_size > size ? size : *minimum_size;
            *maximum_size = *maximum_size > size ? *maximum_size : size;
            radius = size + (float)hu_length3(shape_transform.m_translation);
            break;
        }
        default:
            // match: 0x25c1d8: unreachable hint omits dead branch check
            __assume(0);
        }
    }
    return radius;
}

// @address 0x40480 k_fixed_position_search_directions
// @rdata 0x82040480..0x820405c4
const real_vector3d k_fixed_position_search_directions[27] = {
    { 0.0f, 0.0f, 0.0f },
    { 1.0f, 0.0f, 0.0f },
    { 0.0f, 0.0f, 1.0f },
    { 0.70710677f, 0.0f, 0.70710677f },
    { 0.57735026f, 0.57735026f, 0.57735026f },
    { 0.57735026f, -0.57735026f, 0.57735026f },
    { 0.70710677f, 0.70710677f, 0.0f },
    { 0.70710677f, -0.70710677f, 0.0f },
    { 0.0f, 0.70710677f, 0.70710677f },
    { 0.0f, -0.70710677f, 0.70710677f },
    { -1.0f, 0.0f, 0.0f },
    { 0.0f, 1.0f, 0.0f },
    { 0.0f, -1.0f, 0.0f },
    { -0.70710677f, -0.70710677f, 0.0f },
    { -0.70710677f, 0.70710677f, 0.0f },
    { -0.70710677f, 0.0f, 0.70710677f },
    { -0.57735026f, -0.57735026f, 0.57735026f },
    { -0.57735026f, 0.57735026f, 0.57735026f },
    { 0.0f, 0.0f, -1.0f },
    { 0.70710677f, 0.0f, -0.70710677f },
    { -0.70710677f, 0.0f, -0.70710677f },
    { 0.0f, 0.70710677f, -0.70710677f },
    { 0.0f, -0.70710677f, -0.70710677f },
    { 0.57735026f, 0.57735026f, -0.57735026f },
    { 0.57735026f, -0.57735026f, -0.57735026f },
    { -0.57735026f, -0.57735026f, -0.57735026f },
    { -0.57735026f, 0.57735026f, -0.57735026f },
};
// @rdata-end
static __forceinline bool havok_utilities_location_valid(const s_location *location)
{
    return location->cluster_reference.bsp_index != -1;
}


static __forceinline unsigned int havok_utilities_filter_info(e_havok_group group, long object_index)
{
    return ((object_index == -1 ? 0 : (unsigned short)object_index + 1) << 16) | group;
}
static __forceinline void havok_utilities_rotation_from_matrix3x3(phys_rotation *out, const real_matrix3x3 *matrix)
{
    havok_utilities_vector4_from_real_vector3d(&out->m_column0, &matrix->forward);
    havok_utilities_vector4_from_real_vector3d(&out->m_column1, &matrix->left);
    havok_utilities_vector4_from_real_vector3d(&out->m_column2, &matrix->up);
}
static __forceinline void havok_utilities_transform_from_matrix4x3(phys_transform *transform, const real_matrix4x3 *matrix)
{
    havok_utilities_rotation_from_matrix3x3((phys_rotation *)transform, &matrix->rotation);
    phys_simd_vec_union translation;
    translation.r[0] = matrix->position.x;
    translation.r[1] = matrix->position.y;
    translation.r[2] = matrix->position.z;
    translation.r[3] = 0.0f;
    transform->m_translation.m_simd_quad = translation.q;
}
// @address 0x25d210 ?calculate_shape_fixed_position@@YA_NPBVphys_shape@@JW4e_havok_group@@PBUreal_matrix4x3@@PBTreal_vector3d@@PATreal_point3d@@PAUs_location@@MM_NPBT5@J@Z
bool calculate_shape_fixed_position(const phys_shape *shape, long object_index, e_havok_group group, const real_matrix4x3 *matrix,
    const real_vector3d *velocity, real_point3d *position, s_location *location, float minimum_search_radius, float maximum_search_radius,
    bool search_everywhere, const real_point3d *raycast_origin, long raycast_object_index)
{
    phys_box_volume *aabb_phantom = 0;
    long raycast_component_index = -1;
    if (raycast_origin) {
        phys_aabb aabb;
        float extent = maximum_search_radius + 0.001f;
        havok_utilities_vector4_set(&aabb.m_min_limit, matrix->position.x - extent, matrix->position.y - extent, matrix->position.z - extent, 0.0f);
        havok_utilities_vector4_set(&aabb.m_max_limit, matrix->position.x + extent, matrix->position.y + extent, matrix->position.z + extent, 0.0f);
        for (long axis = 0; axis < 3; axis++) {
            if ((&raycast_origin->x)[axis] < aabb.m_min_limit(axis) + 0.001f)
                aabb.m_min_limit(axis) = (&raycast_origin->x)[axis] - 0.001f;
            if ((&raycast_origin->x)[axis] > aabb.m_max_limit(axis) - 0.001f)
                aabb.m_max_limit(axis) = (&raycast_origin->x)[axis] + 0.001f;
            if (velocity) {
                aabb.m_min_limit(axis) -= (float)fabs((&velocity->i)[axis]);
                aabb.m_max_limit(axis) += (float)fabs((&velocity->i)[axis]);
            }
        }
        void *thread_memory = &phys_thread_memory::get_singleton_instance();
        void *allocation = ((phys_thread_memory *)thread_memory)->allocate_chunk(sizeof(phys_box_volume), (phys_memory_class)47);
        ((phys_ref_object *)allocation)->m_mem_size_and_flags = sizeof(phys_box_volume);
        aabb_phantom = new (allocation) phys_box_volume(aabb, havok_utilities_filter_info(group, object_index));
        g_havok_world->add_volume((phys_volume *)aabb_phantom);
        if (raycast_object_index != -1)
            raycast_component_index = ((object_datum *)object_get_and_verify_type(raycast_object_index, -1))->havok_component_index;
    }
    bool found = false;
    long direction_count = 27 - (search_everywhere ? 0 : 9);
    phys_transform transform;
    havok_utilities_transform_from_matrix4x3(&transform, matrix);
    void *thread_memory = &phys_thread_memory::get_singleton_instance();
    void *allocation = ((phys_thread_memory *)thread_memory)->allocate_chunk(sizeof(phys_simple_shape_volume), (phys_memory_class)47);
    ((phys_ref_object *)allocation)->m_mem_size_and_flags = sizeof(phys_simple_shape_volume);
    phys_simple_shape_volume *shape_phantom = new (allocation) phys_simple_shape_volume(
        shape, transform, havok_utilities_filter_info(group, object_index));
    g_havok_world->add_volume((phys_volume *)shape_phantom);
    c_penetration_collector collector;
    collector.reset();
    real_vector3d horizontal;
    real_point3d sink_point;
    real_point3d test_point;
    s_havok_ray_test ray_test;
    for (long direction_index = 0; direction_index < direction_count; direction_index++) {
        real_vector3d direction = k_fixed_position_search_directions[direction_index];
        if (minimum_search_radius > 0.0f) {
            horizontal = direction;
            horizontal.k = 0.0f;
            if (normalize3d(&horizontal) == 0.0f)
                continue;
            scale_vector3d(&horizontal, minimum_search_radius, &horizontal);
            scale_vector3d(&direction, (maximum_search_radius - minimum_search_radius) / maximum_search_radius, &direction);
            add_vectors3d(&direction, &horizontal, &direction);
        }
        point_from_line3d(&matrix->position, &direction, maximum_search_radius, position);
        phys_vec4_from_real_point3d(&transform.m_translation, position);
        collector.reset();
        ((phys_shape_volume *)shape_phantom)->set_transform(transform);
        shape_phantom->get_penetrations(collector);
        if (collector.get_early_out())
            continue;
        s_collision_test_flags flags = k_placement_test_collision_flags;
        test_point = *position;
        point_from_line3d(&test_point, velocity, 1.0f, &test_point);
        sink_point = test_point;
        sink_point.z += ((s_scenario_soft_surface_sink_depths *)global_scenario->soft_surfaces.address)
            ->maximum_sink_depth[havok_group_soft_surface_sink_class_get((e_havok_group)group)];
        if (collision_test_point(flags, &test_point, object_index, -1, 0) && collision_test_point(flags, &sink_point, object_index, -1, 0))
            continue;
        if (aabb_phantom) {
            ray_test.start = *raycast_origin;
            ray_test.vector.i = test_point.x - ray_test.start.x;
            ray_test.vector.j = test_point.y - ray_test.start.y;
            ray_test.vector.k = test_point.z - ray_test.start.z;
            if (!havok_ray_cast_in_aabb_phantom((phys_box_volume *)aabb_phantom, &ray_test, raycast_component_index, -1))
                found = true;
        } else {
            found = true;
        }
        if (found) {
            position->x = transform.m_translation(0);
            position->y = transform.m_translation(1);
            position->z = transform.m_translation(2);
            scenario_location_from_point(location, &test_point);
            found = havok_utilities_location_valid(location);
            if (found)
                break;
        }
    }
    ((phys_world *)g_havok_world)->remove_volume((phys_volume *)shape_phantom);
    ((phys_world_object *)shape_phantom)->object_remove_ref();
    if (aabb_phantom) {
        ((phys_world *)g_havok_world)->remove_volume((phys_volume *)aabb_phantom);
        ((phys_world_object *)aabb_phantom)->object_remove_ref();
    }
    return found;
}

// @address 0x25d0d8 havok_try_and_get_sphere_fixed_position
extern "C" bool havok_try_and_get_sphere_fixed_position(float radius, float height, e_havok_group group, long attempt_count, long ignore_object_index,
    const real_point3d *point, const real_point3d *raycast_origin, long raycast_object_index, real_point3d *fixed_point, bool search_everywhere)
{
    phys_sphere_shape sphere(radius);
    float search_radius = height;
    if (search_radius <= 0.001f)
        search_radius = 0.001f;
    bool result = false;
    real_matrix4x3 matrix;
    memcpy(&matrix, global_identity4x3, sizeof(matrix));
    matrix.position = *point;
    for (long attempt = 0; attempt < attempt_count; attempt++)
    {
        real_point3d position;
        s_location location;
        if (calculate_shape_fixed_position((const phys_shape *)&sphere, -1, group, &matrix, global_zero_vector3d, &position, &location, 0.0f,
                search_radius, search_everywhere, raycast_origin, raycast_object_index))
        {
            *fixed_point = position;
            result = true;
            break;
        }
        search_radius *= 2.0f;
    }
    return result;
}

// @address 0x25ac50 havok_get_object_bounds_and_group
extern "C" int havok_get_object_bounds_and_group(long object_index, real_rectangle3d *rectangle)
{
    long header_index = object_index & 0xffff;
    long rigid_body_size = 0;
    int result = 3;
    bool bounds_valid = false;
    bool filtered = false;
    s_object_header *header = &((s_object_header *)tls_object_header_data()->data)[header_index];
    long component_index = header->datum->havok_component_index;
    if (component_index != -1) {
        c_havok_component *component = havok_component_get(component_index);
        bounds_valid = component->calculate_physics_bounding_box(rectangle);
        rigid_body_size = component->m_maximum_rigid_body_size;
        if (component->m_rigid_bodies.m_size > 0) {
            filtered = true;
            result = component->m_rigid_bodies.m_data[0].m_rigid_body->get_collider()->get_collision_filter_info() & 31;
        }
    }
    if (!filtered)
        result = havok_get_object_collision_group(object_index, rigid_body_size);
    if (!bounds_valid) {
        s_object_header *bounds_header = &((s_object_header *)tls_object_header_data()->data)[header_index];
        object_datum *object = bounds_header->datum;
        float radius = object->bounding_radius;
        real_point3d center = object->bounding_center;
        rectangle->x0 = center.x - radius;
        rectangle->x1 = center.x + radius;
        rectangle->y0 = center.y - radius;
        rectangle->y1 = center.y + radius;
        rectangle->z0 = center.z - radius;
        rectangle->z1 = center.z + radius;
    }
    return result;
}


// match: 0x25ce40: the build inlines these validations before the later out-of-line accessor call.
static __forceinline short havok_utilities_position_body_index(const c_havok_component *component)
{
    long index = component->m_object_postion_controlling_rigid_body_index;
    return (short)((index >= 0 && index < component->m_rigid_bodies.m_size) ? index : -1);
}

// @address 0x25ce40 ?simple_fix_havok_object_position@@YA_NJJMW4e_havok_group@@@Z
bool simple_fix_havok_object_position(long object_index, long raycast_object_index, float minimum_distance, e_havok_group group)
{
    bool fixed = false;
    object_datum *object = (object_datum *)object_get_and_verify_type(object_index, _object_mask_all);
    object_activate(object_index);
    if (object->havok_component_index != -1) {
        c_havok_component *component = (c_havok_component *)havok_component_get(object->havok_component_index);
        if (havok_utilities_position_body_index(component) != -1) {
            const real_point3d *raycast_origin = 0;
            float step = object->bounding_radius > k_real_three_fortieths_storage ? object->bounding_radius : k_real_three_fortieths_storage;
            short body_index = havok_utilities_position_body_index(component);
            const phys_vec4 *body = &component->m_rigid_bodies.m_data[body_index].m_rigid_body->get_contact_position();
            real_point3d position;
            float px = (*body)(0);
            float py = (*body)(1);
            float pz = (*body)(2);
            position.x = px;
            position.y = py;
            position.z = pz;
            real_point3d center;
            component->get_rigid_body_center_of_mass(havok_utilities_position_body_index(component), &center);
            real_vector3d velocity;
            velocity.i = center.x - px;
            velocity.j = center.y - py;
            velocity.k = center.z - pz;
            real_point3d raycast_center;
            if (raycast_object_index != -1)
                raycast_origin = object_get_center_of_mass(raycast_object_index, &raycast_center);
            real_point3d fixed_position;
            s_location location;
            for (long pass = 0; !fixed && pass < 2; ++pass) {
                bool use_raycast = pass == 0;
                for (long attempt = 0; !fixed && attempt < 4; ++attempt) {
                    long origin_object = use_raycast ? raycast_object_index : -1;
                    const real_point3d *origin = use_raycast ? raycast_origin : 0;
                    fixed = component->rigid_body_calculate_fixed_position(component->get_object_postion_controlling_rigid_body_index(),
                                    _havok_group_vehicle, &position, &velocity,
                                    &fixed_position, &location, 0.0f, (float)(__int64)(attempt + 1) * step, true, origin, origin_object);
                }
            }
            if (fixed)
                object_set_position_direct(object_index, &fixed_position, 0, 0, &location, false);
        }
    }
    return fixed;
}

static __forceinline void havok_utilities_vector4_from_point3d(phys_vec4 *v, const real_point3d *p)
{
    phys_simd_vec_union components;
    components.r[0] = p->x;
    components.r[1] = p->y;
    components.r[2] = p->z;
    components.r[3] = real_constant_pool_zero();
    v->m_simd_quad = components.q;
}
// @address 0x25d8f8 ?havok_calculate_sphere_vs_triangle_collision@@YA_NPBTreal_point3d@@M000PATreal_vector3d@@PAM@Z
bool havok_calculate_sphere_vs_triangle_collision(const real_point3d *center, float radius, const real_point3d *a, const real_point3d *b,
    const real_point3d *c, real_vector3d *normal, float *distance)
{
    phys_vec4 vertices[3];
    phys_transform transform;
    havok_utilities_vector4_from_point3d(&vertices[0], a);
    havok_utilities_vector4_from_point3d(&vertices[1], b);
    havok_utilities_vector4_from_point3d(&vertices[2], c);
    phys_vec4 identity;
#if defined(_XBOX) && !defined(__clang__)
    identity.m_simd_quad = __vxor(identity.m_simd_quad, identity.m_simd_quad);
#else
    identity.m_simd_quad = __vzero();
#endif
    transform.m_rotation.m_column0 = identity;
    transform.m_rotation.m_column0(0) = real_constant_pool_one();
    transform.m_rotation.m_column1 = identity;
    transform.m_rotation.m_column1(1) = real_constant_pool_one();
    transform.m_rotation.m_column2 = identity;
    transform.m_rotation.m_column2(2) = real_constant_pool_one();
    havok_utilities_vector4_from_point3d(&transform.m_translation, center);
    phys_sphere_shape sphere(radius);
    phys_convex_xform_shape translated(&sphere, transform);
    bool result;
    {
        phys_triangle_shape triangle(g_havok_constants.collision_tolerance);
        triangle.set_triangle_vertex(0, vertices[0]);
        triangle.set_triangle_vertex(1, vertices[1]);
        triangle.set_triangle_vertex(2, vertices[2]);
        result = havok_convex_shapes_collide(&translated,
            &triangle, g_havok_constants.collision_tolerance, normal, distance);
    }
    return result;
}
// @address 0x25c140 ?calculate_havok_shape_bounding_sphere_radius@@YAMPBVphys_shape@@PAM1PAJ2@Z
float calculate_havok_shape_bounding_sphere_radius(
    const phys_shape *shape, float *minimum_dimension, float *largest_shape_dimension, long *convex_shape_count, long *phantom_shape_count)
{
    phys_transform transform;
    phys_transform_from_matrix4x3(&transform, global_identity4x3);
    *minimum_dimension = FLT_MAX;
    *largest_shape_dimension = -FLT_MAX;
    *convex_shape_count = 0;
    *phantom_shape_count = 0;
    float radius = calculate_bounding_sphere_recursive(
        shape, 0.0f, transform, minimum_dimension, largest_shape_dimension, convex_shape_count, phantom_shape_count);
    if (radius == 0.0f) {
        *minimum_dimension = 0.0f;
        *largest_shape_dimension = 0.0f;
    }
    return radius;
}
// @address 0x25a5e8 ?havok_object_keyframed_rigid_bodies_to_node_transforms@@YAXJ_N@Z
void havok_object_keyframed_rigid_bodies_to_node_transforms(long object_index, bool reset)
{
    object_datum *object = ((s_object_header *)tls_object_header_data()->data)[(unsigned short)object_index].datum;
    phys_transform reset_transform;
    phys_transform hard_transform;
    s_physics_model_instance instance;
    phys_quat orientation;
    if (object->havok_component_index != -1 && ::physics_model_instance_new(&instance, object_index)) {
        c_havok_component *component = &((c_havok_component *)((s_data_array *)g_havok_component_data)
                ->data)[(unsigned short)object->havok_component_index];
        long body_index = 0;
        if (0 < component->get_rigid_body_count()) {
            do {
                if (component->get_rigid_body_is_keyframed(body_index) && component->get_physics_model_rigid_body_index_count(body_index) > 0) {
                    long physics_body_index = component->get_physics_model_rigid_body_index(body_index, 0);
                    const s_physics_model_rigid_body *physics_body = &instance.physics_model->rigid_bodies.begin()[physics_body_index];
                    if (reset) {
                        component->set_rigid_body_linear_velocity(body_index,
                            global_zero_vector3d);
                        component->set_rigid_body_angular_velocity(body_index,
                            global_zero_vector3d);
                        phys_transform_from_matrix4x3(&reset_transform, instance.get_node_matrix(physics_body->node));
                        component->set_rigid_body_transform_internal(body_index, &reset_transform);
                    } else {
                        const real_matrix4x3 *matrix = instance.get_node_matrix(physics_body->node);
                        float ticks_per_second = (float)tls_game_time_globals()->ticks_per_second;
                        phys_transform_from_matrix4x3(&hard_transform, matrix);
                        orientation.set(hard_transform.m_rotation);
                        ::phys_keyframe_util::apply_hard_keyframe((*&hard_transform.m_translation),
                            orientation, ticks_per_second,
                            component->m_rigid_bodies.m_data[body_index].m_rigid_body);
                    }
                }
                body_index = body_index + 1;
            } while (body_index < component->get_rigid_body_count());
        }
        if (reset)
            component->align_constraint_positions();
    }
}

typedef char havok_group_equipment_flags_offset[(offsetof(havok_utilities_group_definition, equipment_flags) == 0x1b6) ? 1 : -1];

static __forceinline bool havok_utilities_group_test(unsigned long flags, long bit)
{
    return ((flags >> bit) & 1) != 0;
}
static __forceinline int havok_utilities_rigid_group(bool large, long state)
{
    int group;
    if (state <= 1)
        group = large ? 18 : 4;
    else if (state >= 5)
        group = large ? 17 : 5;
    else
        group = large ? 17 : 3;
    return group;
}
// @address 0x25a988 havok_get_object_collision_group
extern "C" int havok_get_object_collision_group(long object_index, long state)
{
    int group = 3;
    s_object_header *header = (s_object_header *)&((s_object_header *)tls_object_header_data()->data)[datum_index_to_absolute_index(object_index)];
    object_datum *object = header->datum;
    long type_mask = 1 << header->object_type;
    if (type_mask & 0x410) {
        if (state != 0) {
            bool large = false;
            long model_index = ((s_object_material_definition *)tag_get('obje', object->definition_index))->model.index;
            if (model_index != -1) {
                long physics_index = ((s_model_definition *)tag_get('hlmt', model_index))->physics_model_index;
                if (physics_index != -1)
                    large = havok_utilities_group_test(((s_physics_model_definition *)tag_get('phmo', physics_index))->flags, 1);
            }
            group = havok_utilities_rigid_group(large, state);
        }
    } else if (type_mask & 2) {
        vehicle_definition *definition = (vehicle_definition *)tag_get('vehi', object->definition_index);
        group = havok_utilities_group_test(definition->flags, 11) ? 16 : 15;
    } else if (type_mask & 1) {
        object = (object_datum *)object_get_and_verify_type(object_index, -1);
        if (havok_utilities_group_test(object->damage_flags, 2)) {
            long program_index = *(long *)((char *)object + ((unit_datum *)object)->unit.motor_program_offset);
            c_motor_program_definition *program = program_index != -1 ? g_program_definition[program_index] : 0;
            bool dynamic = false;
            // Ask the unit's motor program whether it moves dynamically.
            bool (*is_dynamic)(long) = reinterpret_cast<bool (*)(long)>(program->fix_transform_to_physics);
            if (is_dynamic)
                dynamic = is_dynamic(object_index);
            group = dynamic ? 13 : 14;
        } else {
            group = 11;
        }
    } else if ((type_mask & 0x80) || (type_mask & 0x1000)) {
        group = havok_utilities_group_test(object->physics_flags, 0) ? 10 : 9;
    } else if (type_mask & 12) {
        object = (object_datum *)object_get_and_verify_type(object_index, -1);
        havok_utilities_group_definition *definition = (havok_utilities_group_definition *)tag_get('obje', object->definition_index);
        if ((type_mask & 8) && ((long)definition->equipment_flags & 2))
            group = 8;
        else
            group = (definition->flags >> 3 & 1) | 6;
    }
    return group;
}


static __forceinline float havok_utilities_plane3d_distance_to_point(const real_plane3d *plane, const real_point3d *point)
{
    return dot_product3d(&plane->n, point) - plane->d;
}
static inline real_plane3d *havok_utilities_plane3d_from_point_and_normal(real_plane3d *plane, const real_point3d *point, const real_vector3d *normal)
{
    real_vector3d *plane_normal = &plane->n;
    *plane_normal = *normal;
    plane->d = dot_product3d(plane_normal, point);
    return plane;
}

static __forceinline real_point3d *havok_utilities_sphere_point_from_line(
    const real_point3d *origin, const real_vector3d *direction, float scale, real_point3d *result)
{
    result->x = origin->x + direction->i * scale;
    result->y = origin->y + direction->j * scale;
    result->z = origin->z + direction->k * scale;
    return result;
}
static __forceinline void havok_utilities_initialize_sphere_tests(long count, s_havok_sphere_test *tests)
{
    for (long i = 0; i < count; i++) {
        tests[i].hit_found = false;
        tests[i].distance = FLT_MAX;
        tests[i].object_index = -1;
        tests[i].rigid_body_index = -1;
        tests[i].material = k_global_material_type_default;
        tests[i].flag = 0;
        tests[i].surface_index = 0;
    }
}

// match: 0x25b3c8: preserve the scalarized point copy; a trivial aggregate copy emits a 48-byte loop.
static __forceinline phys_hit_point havok_utilities_copy_cd_point(const phys_hit_point &point)
{
    phys_hit_point result(point.m_proxy_a, point.m_proxy_b);
    result.m_hit_contact.m_position = point.m_hit_contact.m_position;
    result.m_hit_contact.m_separating_normal = point.m_hit_contact.m_separating_normal;
    return result;
}
// match: 0x25ad90: retain one component lookup before selecting its connected phantom.
static __forceinline phys_simple_shape_volume *havok_utilities_get_simple_shape_phantom(c_havok_component *component)
{
    return (bool)(_rotl(component->m_flags, 29) & 1) ? component->m_simple_shape_phantom : 0;
}
// @address 0x25ad90 havok_calculate_features_in_simple_shape_phantom
extern "C" void havok_calculate_features_in_simple_shape_phantom(long component_index, float radius,
    const s_havok_sphere_query *queries, s_havok_sphere_test *tests,
    long count, s_havok_sphere_test_cache *cache, bool replay)
{
    phys_simple_shape_volume *phantom = havok_utilities_get_simple_shape_phantom(havok_component_get(component_index));
    havok_utilities_initialize_sphere_tests(count, tests);
    if (replay) {
        for (long i = 0; i < count; i++) {
            const s_havok_sphere_test_cache *entry = &cache[i];
            const s_havok_sphere_query *query = &queries[i];
            s_havok_sphere_test *test = &tests[i];
            bool valid = entry->valid;
            if (valid && entry->object_index != -1) {
                object_datum *object = (object_datum *)object_try_and_get_and_verify_type(entry->object_index, -1);
                if (!object || object->havok_component_index == -1 || entry->rigid_body_index < 0 ||
                    entry->rigid_body_index >= havok_component_get((unsigned short)object->havok_component_index)->m_rigid_bodies.m_size)
                    valid = false;
            }
            if (valid) {
                test->hit_found = entry->valid;
                test->distance = havok_utilities_plane3d_distance_to_point(&entry->plane, &query->marker_position);
                test->object_index = entry->object_index;
                test->rigid_body_index = entry->rigid_body_index;
                test->material = entry->material_type;
                test->flag = entry->flags;
                test->surface_index = entry->surface_index;
                test->normal = entry->plane.n;
                test->contact_velocity = *global_zero_vector3d;
                havok_utilities_sphere_point_from_line(&query->marker_position, &entry->plane.n, -test->distance, &test->position);
            }
        }
    } else if (phantom) {
        class c_point_collector : public phys_hit_collector {
        public:
            static __forceinline void operator delete(void *p)
            {
                havok_utilities_deallocate_collector(p, sizeof(phys_hit_collector));
            }
            inline c_point_collector(long component, float minimum_k, s_havok_sphere_test *sphere_tests, long sphere_count)
                : m_tests(sphere_tests), m_minimum_normal_k(minimum_k), m_test_count(sphere_count), m_component_index(component) {}
            virtual ~c_point_collector() {}
            // @address 0x25b3c8 ?add_hit@c_point_collector@?BD@??havok_calculate_features_in_simple_shape_phantom@@9@UAAXABUphys_hit_point@@@Z
            virtual void add_hit(const phys_hit_point &point)
            {
                long index = calculate_sphere_index(havok_utilities_copy_cd_point(point));
                if (index != -1) {
                    float distance;
                    unsigned char flags;
                    real_vector3d velocity;
                    real_point3d position;
                    real_vector3d normal;
                    c_global_material_type material_type;
                    long hit_component_index;
                    long rigid_body_index;
                    long unknown;
                    long surface_index;
                    if (havok_try_and_get_cd_point_hit(false, true, true, m_component_index, point, m_tests[index].distance,
                            &distance, &flags, &velocity, &position,
                            &normal, &material_type, &hit_component_index, &rigid_body_index, &unknown, &surface_index) &&
                        normal.k > m_minimum_normal_k) {
                        m_tests[index].hit_found = true;
                        m_tests[index].position = position;
                        m_tests[index].normal = normal;
                        m_tests[index].material = material_type;
                        m_tests[index].distance = distance;
                        m_tests[index].object_index = hit_component_index != -1 ? havok_component_get(hit_component_index)->m_object_index : -1;
                        m_tests[index].rigid_body_index = rigid_body_index;
                        m_tests[index].flag = flags;
                        m_tests[index].contact_velocity = velocity;
                        m_tests[index].surface_index = surface_index;
                    }
                }
            }
            __forceinline long calculate_sphere_index(const phys_hit_point &point)
            {
                const phys_collision_proxy *body = &point.m_proxy_a;
                const phys_collision_proxy *other = &point.m_proxy_b;
                long index = -1;
                if (body->get_shape()->get_type_id() == 3) {
                    index = body->m_shape_key;
                    if (body->get_parent_proxy() != 0 && body->get_parent_proxy()->get_shape() != 0 &&
                        body->get_parent_proxy()->get_shape()->get_type_id() == 16 &&
                        other->get_shape()->get_user_data() >= 1 && other->get_shape()->get_user_data() <= 16)
                        index += (body->get_parent_proxy()->get_shape()->get_user_data() - 1) * 8;
                }
                return (index >= 0 && index < m_test_count) ? index : -1;
            }
            
            s_havok_sphere_test *m_tests;
            float m_minimum_normal_k;
            long m_test_count;
            long m_component_index;
        };
        c_point_collector collector(component_index, radius, tests, count);
        phantom->get_closest_points(collector);
        if (cache) {
            for (long i = 0; i < count; i++) {
                const s_havok_sphere_query *query = &queries[i];
                s_havok_sphere_test_cache *entry = &cache[i];
                const s_havok_sphere_test &test = tests[i];
                entry->valid = test.hit_found;
                entry->center = query->marker_position;
                if (test.hit_found) {
                    havok_utilities_plane3d_from_point_and_normal(&entry->plane, &test.position, &test.normal);
                    entry->object_index = tests[i].object_index;
                    entry->rigid_body_index = test.rigid_body_index;
                    entry->material_type = test.material;
                    entry->flags = test.flag;
                    cache[i].surface_index = test.surface_index;
                }
            }
        }
    }
}

struct s_collision_bsp_cache;
// match: the C-linked collision producer currently declares its BSP parameter through a private tag view.
static __forceinline const s_global_material_definition *havok_surface_material_get(short material_index)
{
    const s_global_material_definition *entry = 0;
    if (material_index != -1 && material_index >= 0) {
        const s_game_globals *globals = global_game_globals;
        if (material_index < globals->materials.count())
            entry = &globals->materials.begin()[material_index];
    }
    return entry;
}

// @address 0xef7d8 calculate_havok_shape_properties
extern "C" void calculate_havok_shape_properties(const phys_shape *shape, const phys_rigid_body *rigid_body, const real_point3d *position,
    unsigned long packed_surface, float *friction, float *restitution, c_global_material_type *material)
{
    const phys_rigid_body *entity = rigid_body;
    int type = shape->get_type_id();
    if (type == 13 || type == 12)
        shape = ((const phys_convex_xform_shape *)shape)->get_child_shape();
    if (shape->get_type_id() == 30) {
        const c_collision_surface_shape *collision_shape = (const c_collision_surface_shape *)shape;
        const s_global_material_definition *entry = havok_surface_material_get(collision_shape->get_global_material_type().m_index);
        *material = collision_shape->get_global_material_type();
        if (entry) {
            *friction = entry->friction;
            *restitution = entry->restitution;
        } else {
            *friction = 1.0f;
            *restitution = 0.0f;
        }
        if (position != 0 && packed_surface != -1) {
            long bsp_index = havok_entity_get_structure_bsp_index(entity);
            long kind = (long)((unsigned long)packed_surface >> 29);
            long surface = packed_surface & 0xffff;
            switch (kind) {
            case 1: {
                c_global_material_type collision_material = collision_get_surface_material(
                    bsp_index, g_collision_bsp_cache.thread_users[get_current_thread_index()].collision_bsps[bsp_index],
                    position, surface, -1);
                *material = collision_material;
                return;
            }
            case 3:
                *material = collision_get_bsp_surface_material(bsp_index, position, ((unsigned long)packed_surface >> 16) & 0x1fff, surface);
                return;
            }
        }
    } else {
        phys_ulong user_data = shape->get_user_data();
        if (user_data != 0 && (user_data < 1 || user_data > 16)) {
            const s_character_physics_shape_properties *surface_data = (const s_character_physics_shape_properties *)user_data;
            material->m_index = surface_data->runtime_material_type;
            *friction = surface_data->friction;
            *restitution = surface_data->restitution;
            return;
        }
        if (entity == 0) {
            material->m_index = k_global_material_type_default.m_index;
            *friction = 1.0f;
            *restitution = 0.0f;
            return;
        } else {
            long component_index = havok_entity_get_havok_component_index(entity);
            if (component_index != -1) {
                const c_havok_component *component = havok_component_get(component_index);
                const s_object_header *header = &((s_object_header *)tls_object_header_data()->data)[datum_index_to_absolute_index(component->m_object_index)];
                long object_type = header->object_type;
                if (object_type == 0) {
                    const object_datum *datum = (const object_datum *)header->datum;
                    bool damaged = (datum->damage_flags & (1UL << 2)) != 0;
                    const biped_definition *biped = (const biped_definition *)tag_get(0, datum->definition_index);
                    const phys_material *entity_surface = &entity->get_body_material();
                    *friction = entity_surface->get_friction();
                    *restitution = entity_surface->get_restitution();
                    const short &biped_material = damaged ? biped->physics.dead_global_material_index : biped->physics.living_global_material_index;
                    material->m_index = biped_material;
                    return;
                }
                if (object_type == 11) {
                    const object_datum *datum = (const object_datum *)header->datum;
                    bool damaged = (datum->damage_flags & (1UL << 2)) != 0;
                    const creature_definition *creature = (const creature_definition *)tag_get(0, datum->definition_index);
                    const phys_material *entity_surface = &entity->get_body_material();
                    *friction = entity_surface->get_friction();
                    *restitution = entity_surface->get_restitution();
                    const short &creature_material = damaged ? creature->physics.dead_global_material_index : creature->physics.living_global_material_index;
                    material->m_index = creature_material;
                    return;
                }
            }
        }
        material->m_index = k_global_material_type_default.m_index;
        *friction = 1.0f;
        *restitution = 0.0f;
    }
}

// @address 0xef648 ?calculate_havok_shape_properties_recursive@@YA_NPBVphys_shape@@PBVphys_rigid_body@@PBTreal_point3d@@IPAM3PAVc_global_material_type@@@Z
bool calculate_havok_shape_properties_recursive(const phys_shape *shape, const phys_rigid_body *rigid_body, const real_point3d *point,
    unsigned int key, float *friction, float *restitution, c_global_material_type *material)
{
    bool result = false;
    if (shape->get_type_id() == 28)
    {
        const phys_transform_shape *transform_shape = (const phys_transform_shape *)shape;
        result = calculate_havok_shape_properties_recursive(transform_shape->get_child_shape(), rigid_body, point, key, friction, restitution, material);
    }
    else if (shape->get_type_id() == 13)
    {
        const phys_convex_xform_shape *transform_shape = (const phys_convex_xform_shape *)shape;
        result = calculate_havok_shape_properties_recursive(transform_shape->get_child_shape(), rigid_body, point, key, friction, restitution, material);
    }
    else if (shape->get_type_id() == 12)
    {
        const phys_convex_offset_shape *translate_shape = (const phys_convex_offset_shape *)shape;
        result = calculate_havok_shape_properties_recursive(translate_shape->get_child_shape(), rigid_body, point, key, friction, restitution, material);
    }
    else if (shape->get_type_id() == 16)
    {
        calculate_havok_shape_properties(shape, rigid_body, point, key, friction, restitution, material);
        result = true;
    }
    else if (hu_is_shape_collection(shape))
    {
        const phys_compound_shape *collection = (const phys_compound_shape *)shape;
        __declspec(align(16)) char buffer[512];
        for (long child_key = collection->get_first_key(); !result && child_key != -1; child_key = collection->get_next_key(child_key))
        {
            const phys_shape *child = collection->get_child_shape(child_key, buffer);
            result = calculate_havok_shape_properties_recursive(child, rigid_body, point, (unsigned int)child_key, friction, restitution, material);
        }
    }
    else if (shape->get_type_id() == 27)
    {
        const phys_bvh_tree_shape *tree_shape = (const phys_bvh_tree_shape *)shape;
        result = calculate_havok_shape_properties_recursive(tree_shape->get_single_child(), rigid_body, point, key, friction, restitution, material);
    }
    else if (shape->get_type_id() != 25)
    {
        calculate_havok_shape_properties(shape, rigid_body, point, key, friction, restitution, material);
        result = true;
    }
    return result;
}

static __forceinline const structure_bsp *havok_utilitiestructure_bsp_get(long structure_bsp_index)
{
    const scenario_structure_bsp_reference *reference = &(
        (const scenario_structure_bsp_reference *)global_scenario->structure_bsps.address)[structure_bsp_index];
    return (const structure_bsp *)tag_get('sbsp', reference->structure_bsp.index);
}
static __forceinline const collision_bsp *havok_utilities_collision_bsp_get(long structure_bsp_index)
{
    return g_collision_bsp_cache.thread_users[get_current_thread_index()].collision_bsps[structure_bsp_index];
}
static __forceinline const structure_instanced_geometry_definition *havok_utilities_instanced_geometry_definitions_get(long structure_bsp_index)
{
    return (const structure_instanced_geometry_definition *)g_collision_bsp_cache.thread_users[get_current_thread_index()]
        .instanced_geometry_definitions[structure_bsp_index]
        .address;
}
static __forceinline c_global_material_type havok_utilities_collision_calculate_structure_global_material_type(
    long structure_bsp_index, const real_point3d *position, long surface_index, long instance_index)
{
    const collision_bsp *bsp;
    if (instance_index != -1) {
        const structure_instanced_geometry_instance *instance =
            &((const structure_instanced_geometry_instance *)havok_utilitiestructure_bsp_get(structure_bsp_index)
                    ->instanced_geometry_instances.address)[instance_index];
        bsp = &havok_utilities_instanced_geometry_definitions_get(structure_bsp_index)[instance->definition_index].bsp;
    } else {
        bsp = havok_utilities_collision_bsp_get(structure_bsp_index);
    }
    return collision_get_surface_material(structure_bsp_index, bsp, position, surface_index, instance_index);
}
static __forceinline real_plane3d *havok_utilities_bsp3d_get_plane_from_designator(const collision_bsp *bsp, long designator, real_plane3d *plane)
{
    const real_plane3d *bsp_plane = &((const real_plane3d *)bsp->planes.address)[designator & 0x7fff];
    if (designator & 0x8000) {
        plane->n.i = -bsp_plane->n.i;
        plane->n.j = -bsp_plane->n.j;
        plane->n.k = -bsp_plane->n.k;
        plane->d = -bsp_plane->d;
    } else {
        *plane = *bsp_plane;
    }
    return plane;
}
// @address 0x25b608 havok_try_and_get_cd_point_hit
extern "C" bool havok_try_and_get_cd_point_hit(bool ignore_surface_side, bool calculate_material_type, bool collide_with_structure, long ignore_component_index,
    // match: 0x25b608: pointer __restrict qualifier prevents alias reload
    const phys_hit_point &point, float maximum_distance, float *out_distance, unsigned char *__restrict out_conveyor, real_vector3d *out_velocity,
    real_point3d *out_point, real_vector3d *out_normal, c_global_material_type *out_material_type, long *out_component_index,
    // match: 0x25b608: pointer __restrict qualifier prevents alias reload
    long *__restrict out_rigid_body_index, long *__restrict out_structure_bsp_index, long *__restrict out_surface_flags)
{
    const phys_collision_proxy *body_a = &point.m_proxy_a;
    const phys_collision_proxy *body_b = &point.m_proxy_b;
    const phys_shape *shape = body_a->get_shape();
    phys_rigid_body *rigid_body = phys_get_rigid_body(body_b->get_root_collider());
    long component_index = rigid_body ? havok_entity_get_havok_component_index(rigid_body) : -1;
    bool result = false;
    bool ignored = false;
    bool invalid = false;
    if (!body_b->get_root_collider()) {
        invalid = true;
    } else if (component_index != -1 && body_b->get_shape()->get_type_id() != 30 && body_b->get_shape()->get_user_data() != 0 &&
        (body_b->get_shape()->get_user_data() < 1 || body_b->get_shape()->get_user_data() > 16)) {
        const s_character_physics_shape_properties *primitive = phys_shape_get_primitive(body_b->get_shape());
        ignored = primitive->phantom_index != -1;
    }
    if (!invalid && !ignored && shape->get_type_id() == 3 && rigid_body) {
        long structure_bsp_index = havok_entity_get_structure_bsp_index(rigid_body);
        real_point3d origin;
        const phys_vec4 *translation = &((const phys_transform *)body_a->m_motion)->m_translation;
        set_real_point3d(&origin, (*translation)(0), (*translation)(1), (*translation)(2));
        const phys_contact_point &contact = point.m_hit_contact;
        float distance = contact.m_separating_normal(3) + ((const phys_convex_shape *)shape)->get_convex_radius();
        if ((ignore_component_index == -1 || ignore_component_index != component_index) &&
            (collide_with_structure || structure_bsp_index == -1) && distance < maximum_distance) {
            result = true;
            *out_distance = distance;
            *out_conveyor = false;
            *out_component_index = component_index;
            *out_velocity = *global_zero_vector3d;
            set_real_point3d(out_point, contact.m_position(0), contact.m_position(1), contact.m_position(2));
            set_real_vector3d(out_normal, contact.m_separating_normal(0), contact.m_separating_normal(1), contact.m_separating_normal(2));
            *out_material_type = k_global_material_type_default;
            *out_rigid_body_index = havok_entity_get_havok_component_rigid_body_index(rigid_body);
            *out_structure_bsp_index = structure_bsp_index;
            *out_surface_flags = 0;
            if (body_b->get_shape()->get_type_id() == 30) {
                const c_collision_surface_shape *surface_shape = (const c_collision_surface_shape *)body_b->get_shape();
                const collision_bsp *bsp = 0;
                unsigned long shape_key = surface_shape->get_user_data();
                *out_surface_flags = surface_shape->m_surface_flags;
                if (shape_key != -1) {
                    long index;
                    long surface_index;
                    long type;
                    values_from_shape_key(&index, &surface_index, &type, shape_key);
                    real_plane3d plane;
                    switch (type) {
                    case 1: {
                        bsp = havok_utilities_collision_bsp_get(structure_bsp_index);
                        const collision_surface *surface = &((const collision_surface *)bsp->surfaces.address)[index];
                        *out_surface_flags = surface->flags;
                        havok_utilities_bsp3d_get_plane_from_designator(bsp, surface->plane, &plane);
                        if (surface->flags & 0x20)
                            *out_conveyor = false;
                        if (ignore_surface_side || havok_utilities_ray_dot_product3d(&plane.n, out_normal) >= 0.0f)
                            *out_normal = plane.n;
                        else
                            result = false;
                        if (calculate_material_type)
                            *out_material_type = havok_utilities_collision_calculate_structure_global_material_type(structure_bsp_index, out_point, index, -1);
                        break;
                    }
                    case 3: {
                        const structure_instanced_geometry_instance *instance =
                            &((const structure_instanced_geometry_instance *)havok_utilitiestructure_bsp_get(structure_bsp_index)
                                    ->instanced_geometry_instances.address)[index];
                        const structure_instanced_geometry_definition *definition =
                            &havok_utilities_instanced_geometry_definitions_get(structure_bsp_index)[instance->definition_index];
                        const collision_surface *surface = &((const collision_surface *)definition->bsp.surfaces.address)[surface_index];
                        *out_surface_flags = surface->flags;
                        havok_utilities_bsp3d_get_plane_from_designator(&definition->bsp, surface->plane, &plane);
                        matrix4x3_transform_plane(&instance->transform, &plane, &plane);
                        if (ignore_surface_side || havok_utilities_plane3d_distance_to_point(&plane, &origin) > 0.0f)
                            *out_normal = plane.n;
                        else
                            result = false;
                        if (calculate_material_type)
                            *out_material_type = collision_get_bsp_surface_material(structure_bsp_index, out_point, surface_index, index);
                        break;
                    }
                    default:
                        UNREACHABLE();
                    }
                } else {
                    *out_material_type = surface_shape->get_global_material_type();
                }
            } else {
                float friction;
                float restitution;
                calculate_havok_shape_properties(
                    body_b->get_shape(), phys_get_rigid_body(body_b->get_root_collider()), 0, -1, &friction, &restitution, out_material_type);
            }
        }
    }
    return result;
}
