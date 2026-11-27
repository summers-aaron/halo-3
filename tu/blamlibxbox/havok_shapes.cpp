// blamlibxbox/havok_shapes.obj: from the records 0x1045f8 0x1048c8 0x1058a0 0x106610 0x1b65a8 0x1c1270 0x25ef50 0x25efa8 0x25f028 0x25f038 0x25f450 0x25f460 0x25f698 0x25f6a0 0x25f6e0 0x25f738 0x25f7f8 0x25f860 0x25f908 0x25fad0 0x25fb08 0x25fce8 0x25fe38 0x25feb0 0x25fec8 0x25ff40 0x25ffd0 0x260138 0x260348 0x4fd610 0x5058f0 0x505910
// @address 0x25f6a0 ?get_first_key@c_collision_geometry_shape@@UBAIXZ
// @address 0x25f6e0 ?get_next_key@c_collision_geometry_shape@@UBAII@Z
// @address 0x25feb0 ?get_aabb_impl@c_environment_surface_shape@@UBAXABVphys_transform@@MAAVphys_aabb@@@Z
// @address 0x25fec8 ?get_child_shape_count@c_environment_surface_shape@@UBAHXZ
#include "scenario/scenario_tag.h"
#include "math/real_math_bounds_storage.h"
// @address 0x7c5d34 k_null_rectangle3d_storage
#include "scenario/scenario_structure_bsp_reference.h"
#include "physics/havok_collision_surface_shape.h"
#include "math/matrix_math_types.h"
#include "structures/structure_runtime_resources.h"
#include "structures/structure_bsp_tag.h"
#include "structures/structure_instanced_geometry.h"
#include "physics/collision_bsp_physics_definitions.h"
#include "math/real_math_globals.h"
#include "ai/ai_reference_frame.h"
#include "tag_files/tag_groups.h"
#include "scenario/scenario_access.h"
// @flags /TP /O2
// @address 0x1c1270 ??_Gphys_shape_container@@UAAPAXI@Z
// @address 0x25f7f8 ??_Gc_environment_mopp_bv_tree_shape@@UAAPAXI@Z
// @address 0x25fe38 ??_Gc_environment_surface_shape@@UAAPAXI@Z
// @address 0x260348 ??_Ec_environment_surface_shape@@WBA@AAPAXI@Z
#include "math/real_math_types.h"
#include "math/point_math.h"
#include "multithreading/thread_tls.h"
#include <stddef.h>
#include "tag_files/tag_types.h"
#include "phys/phys_base_types.h"
// @address 0x106610 ?get_global_material_type@c_collision_surface_shape@@QBA?AVc_global_material_type@@XZ
// @address 0x1b65a8 ?get_collision_spheres_impl@c_collision_surface_shape@@UBAPBVphys_sphere@@PAV2@@Z
// @address 0x25ef50 get_bsp_surface_count
#include <ppcintrinsics.h>
#include <float.h>
#include <math.h>
#include <stdlib.h>
#include "ai/actors.h"
#include "cache/restricted_memory_regions.h"
#include "cache/restricted_memory_regions_inline.h"
#include "physics/havok_halo_external.h"
#include "physics/havok_shapes.h"
#include "physics/havok_shape_key.h"
#include "physics/havok_utilities.h"
#include "physics/havok_runtime_pool.h"
#include "physics/havok_runtime_pool_types.h"
#include "physics/havok_listeners.h"
#include "memory/hashtable.h"
#include "scenario/scenario.h"
#include "cache/restricted_memory_layout.h"
#include "cache/restricted_memory.h"
#include "cache/cache_files.h"


class phys_ref_object;
#include "models/model_material.h"
#include "physics/collision_materials.h"
#include "models/model_types.h"
struct scenario;
#include "game/materials.h"
extern "C" {
struct s_attachment_state;
}


#include <vectorintrinsics.h>
class phys_transform;
#include "phys/phys_collide.h"
class phys_aabb;
#include "phys/phys_aabb_util.h"
class phys_statistics_collector;
class phys_shape;
class phys_shape_container;
struct phys_ray_input;
struct phys_ray_output;
class phys_collision_proxy;
class phys_ray_hit_collector;
#include "phys/phys_thread_memory.h"
class phys_ref_object;
struct collision_bsp;
#include "physics/havok_collision_geometry_shape.h"
#include "physics/havok_environment_surface_shape.h"
class phys_hull_vertex;
struct real_matrix4x3;
#include "physics/havok_collision_surface_shape.h"
#include "physics/havok_environment_mopp_bv_tree_shape.h"
#include "structures/structure_instanced_geometry_definition.h"
#include "math/real_math.h"
#include "physics/havok_shapes_math_queries.h"
typedef void (*field_proc_t)(void *field);
class c_restricted_memory;
struct s_table;
#include "ai/actor_firing_position_hash.h"
long get_bsp_surface_count(const collision_bsp *list)
{
  long result = list->surfaces.count;
  for (long i = list->surfaces.count - 1; i >= 0; i--) {
    if (!(((const collision_surface *)list->surfaces.address)[i].flags & 0x10))
      break;
    result--;
  }
  return result;
}

// @address 0x25f028 havok_shape_get_source_count
extern "C" int havok_shape_get_source_count(const c_collision_surface_shape *shape)
{
    return shape->m_vertex_count;
}

// @address 0x620a98 ?get_collision_filter_info@phys_shape_container@@UBAII@Z
// @address 0x103d80 ?get_child_shape@c_environment_surface_shape@@UBAPBVphys_shape@@IAAY0CAA@D@Z
// @address 0x25f698 ?get_child_shape_count@c_collision_geometry_shape@@UBAHXZ
int c_collision_geometry_shape::get_child_shape_count() const
{
    return get_bsp_surface_count(m_bsp);
}

unsigned int c_collision_geometry_shape::get_first_key() const
{
    if (get_child_shape_count() > 0) {
        return 0;
    } else {
        return (unsigned int)-1;
    }
}

unsigned int c_collision_geometry_shape::get_next_key(unsigned int old_key) const
{
    if (old_key != (unsigned int)-1 && old_key < (unsigned int)(get_child_shape_count() - 1)) {
        return old_key + 1;
    } else {
        return (unsigned int)-1;
    }
}



// Collision surfaces do not provide collision spheres.
const phys_sphere *c_collision_surface_shape::get_collision_spheres_impl(phys_sphere *spheres) const
{
    __debugbreak();
    // match: 0x1b65a8: __assume(0) omits the return epilog after __debugbreak
    __assume(0);
}


static __forceinline scenario_structure_bsp_reference *surface_shape_bsp_get(long index)
{
    return &((scenario_structure_bsp_reference *)global_scenario->structure_bsps.address)[index];
}

c_global_material_type c_collision_surface_shape::get_global_material_type(void) const
{
    c_global_material_type result = k_global_material_type_default;
    if (m_instance_index != -1) {
        s_model_definition *model = (s_model_definition *)tag_get('hlmt', m_instance_index);
        s_damage_model_material *material = &((s_damage_model_material *)model->materials.address)[m_material_index];
        result = material->material;
    } else if (m_material_index != -1) {
        structure_bsp *bsp = (structure_bsp *)tag_get('sbsp', (unsigned short)surface_shape_bsp_get(m_structure_bsp_index)->structure_bsp.index);
        const s_collision_material *material =
            TAG_BLOCK_GET_ELEMENT(&bsp->collision_materials, m_material_index, const s_collision_material);
        result = material->global_material;
    }
    return result;
}

// @address 0x25efa8 ?get_aabb_impl@c_collision_surface_shape@@UBAXABVphys_transform@@MAAVphys_aabb@@@Z
void c_collision_surface_shape::get_aabb_impl(const phys_transform &transform, float tolerance, phys_aabb &out) const
{
    phys_vec4 blocks[8];
    for (int i = 0; i < m_vertex_count; i++)
        blocks[i].vec_set_transformed_pos(transform, m_vertices[i]);
    phys_aabb_util::compute_bounds((const float *)blocks, m_vertex_count, 16, out);
}

// @address 0x25f738 ?get_child_shape@c_collision_geometry_shape@@UBAPBVphys_shape@@IAAY0CAA@D@Z
const phys_shape *c_collision_geometry_shape::get_child_shape(unsigned int key, char (&buffer)[512]) const
{
    bool enabled = true;
    if (m_type == 2 && key < 8192) {
        long surface_key = (key << 16) | 0x60000000;
        surface_key |= m_instance_index;
        enabled = havok_runtime_pool_is_surface_enabled(m_structure_bsp_index, -1, 0, surface_key);
    }
    return new (buffer) c_collision_surface_shape(
        -1, m_structure_bsp_index, m_bsp, m_radius, false, !enabled, m_geometry_index, key, 0, _soft_surface_sink_class_default, 0.0f);
}


c_environment_mopp_bv_tree_shape::~c_environment_mopp_bv_tree_shape()
{
    m_environment_surface_shape->object_remove_ref();
    m_environment_surface_shape = 0;
}
namespace {
static __forceinline structure_instanced_geometry_definition *havok_shapes_collision_definitions_get(long index)
{
    return (structure_instanced_geometry_definition *)g_collision_bsp_cache.users[get_current_thread_index()].entries[index].data;
}
static __forceinline s_structure_bsp_resources *havok_shapes25ffd0_resources_get(s_structure_bsp_resource *resource)
{
    if (resource->use_resource_items)
        return resource->collision_resource_interface.get_resources();
    return (s_structure_bsp_resources *)resource->raw_resources.address;
}
static __forceinline collision_bsp *havok_shapes25ffd0_collision_bsp_get(s_structure_bsp_resource *resource)
{
    return havok_shapes25ffd0_resources_get(resource)->collision_bsp;
}
}
// @address 0x25ffd0 ?get_first_key@c_environment_surface_shape@@UBAIXZ
unsigned int c_environment_surface_shape::get_first_key() const
{
    unsigned int result = (unsigned int)-1;
    long bsp_index = m_structure_bsp_index;
    struct structure_bsp *structure_bsp = global_structure_bsp_get(bsp_index);
    s_structure_bsp_resource *resource = &structure_bsp->resource;
    if (get_bsp_surface_count(havok_shapes25ffd0_collision_bsp_get(resource)) > 0) {
        collision_bsp *bsp = havok_shapes25ffd0_collision_bsp_get(&structure_bsp->resource);
        bool sink = surface_has_sink(bsp, 0);
        result = shape_key_from_values(0, sink ? 0 : 5, 1);
    }
    if (result == (unsigned int)-1) {
        long count = structure_bsp->instanced_geometry_instances.count;
        long i = 0;
        if (count > 0) {
            structure_instanced_geometry_definition *definitions = havok_shapes_collision_definitions_get(m_structure_bsp_index);
            do {
                structure_instanced_geometry_instance *instance =
                    &((structure_instanced_geometry_instance *)structure_bsp->instanced_geometry_instances.address)[i];
                structure_instanced_geometry_definition *definition = &definitions[instance->definition_index];
                if (definition->bsp.surfaces.count > 0)
                    return shape_key_from_values(i, 0, 2);
                i++;
            } while (i < count);
        }
    }
    return result;
}
// @address 0x25f908 ?query_sphere@c_environment_mopp_bv_tree_shape@@QBAXABVphys_sphere@@AAV?$phys_array@I@@@Z
void c_environment_mopp_bv_tree_shape::query_sphere(const phys_sphere &sphere, phys_array<unsigned int> &keys) const
{
    for (int i = 0; i < keys.getSize(); i++) {
        if (keys[i] == (unsigned int)-1)
            continue;
        long index, surface_index, type;
        values_from_shape_key(&index, &surface_index, &type, keys[i]);
        if (type != 2)
            continue;

        structure_instanced_geometry_instance *instance =
            &((structure_instanced_geometry_instance *)global_structure_bsp_get(
                m_environment_surface_shape->m_structure_bsp_index)->instanced_geometry_instances.address)[index];
        structure_instanced_geometry_definition *definition =
            &havok_shapes_collision_definitions_get(m_environment_surface_shape->m_structure_bsp_index)[instance->definition_index];
        if (definition->bsp.surfaces.count > 8192)
            continue;
        s_collision_bsp_physics_definition *physics = instance->physics.begin();
        phys_transform transform;
        phys_vec4 position;
        position.m_simd_quad = sphere.get_contact_position().m_simd_quad;
        phys_transform_from_matrix4x3(&transform, &instance->transform);
        position.vec_set_transformed_inverse_pos(transform, position);
        keys.remove_at(i);
        i--;
        int old_count = keys.getSize();
        ((const phys_bvh_tree_shape *)physics->m_mopp_shape)->shape_query_sphere(phys_sphere(position, sphere.get_convex_radius()), keys);
        for (int j = old_count; j < keys.getSize(); j++)
            keys[j] = shape_key_from_values(index, keys[j], 3);
    }
}

//  0x6205e0 ?querySphere@hkMoppBvTreeShape@@UBAXABVhkSphere@@AAV?$hkArray@I@@@Z
// @address 0x25fad0 ?shape_query_sphere@c_environment_mopp_bv_tree_shape@@UBAXABVphys_sphere@@AAV?$phys_array@I@@@Z
void c_environment_mopp_bv_tree_shape::shape_query_sphere(const phys_sphere &sphere, phys_array<unsigned int> &keys) const
{
    phys_bvh_tree_shape::shape_query_sphere(sphere, keys);
    query_sphere(sphere, keys);
}

// @address 0x25fb08 ?query_obb@c_environment_mopp_bv_tree_shape@@UBAXABVphys_transform@@ABVphys_vec4@@MAAV?$phys_array@I@@@Z
void c_environment_mopp_bv_tree_shape::query_obb(const phys_transform &transform, const phys_vec4 &half_extents, float tolerance,
    phys_array<unsigned int> &keys) const
{
    phys_bvh_tree_shape::query_obb(transform, half_extents, tolerance, keys);
    for (int i = 0; i < keys.getSize(); i++) {
        if (keys[i] == (unsigned int)-1)
            continue;
        long index, surface_index, type;
        values_from_shape_key(&index, &surface_index, &type, keys[i]);
        if (type != 2)
            continue;

        structure_instanced_geometry_instance *instance =
            &((structure_instanced_geometry_instance *)global_structure_bsp_get(
                m_environment_surface_shape->m_structure_bsp_index)->instanced_geometry_instances.address)[index];
        structure_instanced_geometry_definition *definition =
            &havok_shapes_collision_definitions_get(m_environment_surface_shape->m_structure_bsp_index)[instance->definition_index];
        if (definition->bsp.surfaces.count > 8192)
            continue;
        s_collision_bsp_physics_definition *physics = instance->physics.begin();
        phys_transform instance_transform;
        phys_transform relative_transform;
        phys_transform_from_matrix4x3(&instance_transform, &instance->transform);
        relative_transform.set_mul_inverse_mul(instance_transform, transform);
        keys.remove_at_and_copy(i);
        i--;
        int old_count = keys.getSize();
        ((const phys_bvh_tree_shape *)physics->m_mopp_shape)->query_obb(relative_transform, half_extents, tolerance, keys);
        for (int j = old_count; j < keys.getSize(); j++)
            keys[j] = shape_key_from_values(index, keys[j], 3);
    }
}

// @address 0x25fce8 ??0c_environment_surface_shape@@QAA@JJ@Z
c_environment_surface_shape::c_environment_surface_shape(long structure_bsp_index, long) :
    phys_compound_shape(PHYS_SHAPE_COMPOUND)
{
    m_structure_bsp_index = structure_bsp_index;
    m_runtime_pool_index = -1;
    set_user_data(123456789);
    m_edge_weld_disabled = false;

    struct structure_bsp *structure_bsp = global_structure_bsp_get(m_structure_bsp_index);
    phys_vec4 lower;
    phys_vec4 upper;
    lower.set(structure_bsp->structure_physics.mopp_bounds_min.x, structure_bsp->structure_physics.mopp_bounds_min.y,
        structure_bsp->structure_physics.mopp_bounds_min.z, 0.0f);
    upper.set(structure_bsp->structure_physics.mopp_bounds_max.x, structure_bsp->structure_physics.mopp_bounds_max.y,
        structure_bsp->structure_physics.mopp_bounds_max.z, 0.0f);
    m_aabb_center.set_add4(lower, upper);
    m_aabb_center.mul4(phys_simd_scalar(0.5f));
    m_aabb_half_extents.set_sub4(upper, lower);
    m_aabb_half_extents.mul4(phys_simd_scalar(0.5f));
}


void c_environment_surface_shape::get_aabb_impl(const phys_transform &transform, float tolerance, phys_aabb &out) const
{
    phys_aabb_util::compute_bounds(transform, m_aabb_half_extents, m_aabb_center, tolerance, out);
}

#if !defined(_XBOX) || defined(__clang__)
// GAME-CHANGES F120: Xbox vtable 0x820405e4 slot 6 is 0x25feb0, the body above (see havok_collision_geometry_shape.h).
void c_collision_geometry_shape::get_aabb_impl(const phys_transform &transform, float tolerance, phys_aabb &out) const
{
    phys_aabb_util::compute_bounds(transform, m_aabb_half_extents, m_aabb_center, tolerance, out);
}
#endif

int c_environment_surface_shape::get_child_shape_count() const
{
    int count = 0;
    for (unsigned int key = get_first_key(); key != (unsigned int)-1; key = get_next_key(key)) {
        count++;
    }
    return count;
}


// @address 0x25ff40 ?surface_has_sink@@YA_NPAUcollision_bsp@@J@Z
bool surface_has_sink(collision_bsp *bsp, long surface_index)
{
    collision_surface *surface = &((collision_surface *)bsp->surfaces.address)[surface_index];
    bool result = false;
    long first_edge = surface->first_edge;
    long edge_index = first_edge;
    do {
        collision_edge *edge = &((collision_edge *)bsp->edges.address)[edge_index];
        bool side = surface_index - edge->surfaces[1] == 0;
        collision_vertex *vertex = &((collision_vertex *)bsp->vertices.address)[edge->vertices[side]];
        if (vertex->sink != 0) {
            result = true;
            break;
        } else {
            edge_index = edge->edges[side];
        }
    } while (edge_index != first_edge);
    return result;
}


// @address 0x25f460 ??0c_collision_geometry_shape@@QAA@JPBUcollision_bsp@@JJJMJPBVphys_vec4@@1@Z
c_collision_geometry_shape::c_collision_geometry_shape(long, const collision_bsp *bsp, long, long, long geometry_index,
    float radius, long, const phys_vec4 *aabb_center, const phys_vec4 *aabb_half_extents) :
    phys_compound_shape(PHYS_SHAPE_COMPOUND)
{
    typedef char aabb_center_offset[(sizeof(void *) != 4 || offsetof(c_collision_geometry_shape, m_aabb_center) == 0x20) ? 1 : -1];
    typedef char geometry_index_offset[(sizeof(void *) != 4 || offsetof(c_collision_geometry_shape, m_geometry_index) == 0x40) ? 1 : -1];
    typedef char radius_offset[(sizeof(void *) != 4 || offsetof(c_collision_geometry_shape, m_radius) == 0x4c) ? 1 : -1];
    m_radius = radius;
    m_geometry_index = geometry_index;
    m_bsp = bsp;
    m_structure_bsp_index = -1;
    m_type = 1;
    m_instance_index = -1;
    if (radius == 0.0f)
        radius = 1.0f;
    if (aabb_center == 0) {
        real_rectangle3d rectangle = k_null_rectangle3d_storage;
        for (long vertex_index = 0; vertex_index < m_bsp->vertices.count; vertex_index++)
            real_rectangle3d_enclose_point(&rectangle, &((collision_vertex *)bsp->vertices.address)[vertex_index].point);
        phys_vec4 center;
        phys_vec4 half_extents;
        center.m_simd_quad.w = 0.0f;
        half_extents.m_simd_quad.w = 0.0f;
        center.m_simd_quad.x = (rectangle.x0 + rectangle.x1) * radius * 0.5f;
        center.m_simd_quad.y = (rectangle.y0 + rectangle.y1) * radius * 0.5f;
        center.m_simd_quad.z = (rectangle.z0 + rectangle.z1) * radius * 0.5f;
        half_extents.m_simd_quad.x = (rectangle.x1 - rectangle.x0) * radius * 0.5f;
        half_extents.m_simd_quad.y = (rectangle.y1 - rectangle.y0) * radius * 0.5f;
        half_extents.m_simd_quad.z = (rectangle.z1 - rectangle.z0) * radius * 0.5f;
        m_aabb_center.m_simd_quad = center.m_simd_quad;
        m_aabb_half_extents.m_simd_quad = half_extents.m_simd_quad;
    } else {
        m_aabb_center.m_simd_quad = aabb_center->m_simd_quad;
        m_aabb_half_extents.m_simd_quad = aabb_half_extents->m_simd_quad;
    }
}

namespace {
static __forceinline real_point3d *havok_shapes_matrix4x3_transform_point(const real_matrix4x3 *matrix, const real_point3d *point,
    real_point3d *result)
{
    float x = point->x * matrix->scale;
    float y = point->y * matrix->scale;
    float z = point->z * matrix->scale;
    result->x = x * matrix->matrix[0][0] + y * matrix->matrix[1][0] + z * matrix->matrix[2][0] + matrix->position.x;
    result->y = x * matrix->matrix[0][1] + y * matrix->matrix[1][1] + z * matrix->matrix[2][1] + matrix->position.y;
    result->z = x * matrix->matrix[0][2] + y * matrix->matrix[1][2] + z * matrix->matrix[2][2] + matrix->position.z;
    return result;
}
}
// @address 0x61ada8 ?trace_ray_with_collector@phys_convex_shape@@UBAXABUphys_ray_input@@ABVphys_collision_proxy@@AAVphys_ray_hit_collector@@@Z
// @address 0x61acb0 ?weld_contact_point_impl@phys_convex_shape@@MBAHPAGAAEAAVphys_vec4@@PBVphys_transform@@PBV1@32@Z
// @address 0x25f450 ?get_first_vertex@c_collision_surface_shape@@UBAXAAVphys_vec4@@@Z
// @address 0x1045f8 ?get_support_vertex_impl@c_collision_surface_shape@@UBAXABVphys_vec4@@AAVphys_hull_vertex@@@Z
// @address 0x1043d8 ?vertex_ids_to_vertices_impl@c_collision_surface_shape@@UBAXPBGHPAVphys_hull_vertex@@@Z

extern "C" {
// @address 0x7c5ef0 k_negative_flt_max_vector
__vector4 k_negative_flt_max_vector = { -FLT_MAX, -FLT_MAX, -FLT_MAX, -FLT_MAX };
}
// @address 0x8c425a g_havok_shapes_environment_null_shapes_locked
bool g_havok_shapes_environment_null_shapes_locked;
static __forceinline __vector4 havok_shapes_dot3(__vector4 a, __vector4 b)
{
    return __vmsum3fp(a, b);
}
static __forceinline bool havok_shapes_all_greater_or_equal3(__vector4 a, __vector4 b)
{
    unsigned int comparison = 0;
    __vcmpgefpR(__vpermwi(a, 0x18), __vpermwi(b, 0x18), &comparison);
    return (comparison & 0x80) > 0;
}
void c_collision_surface_shape::get_first_vertex(phys_vec4 &vertex) const
{
    vertex.m_simd_quad = m_vertices[0].m_simd_quad;
}
// Reached through the vtable; callers pass 3 to 8 vertices.
void c_collision_surface_shape::get_support_vertex_impl(const phys_vec4 &direction, phys_hull_vertex &vertex) const
{
    __vector4 d = direction.m_simd_quad;
    __vector4 best_dot = k_negative_flt_max_vector;
    __vector4 best;
    __vector4 dot;
    switch (m_vertex_count) {
    case 8:
        best = m_vertices[7].m_simd_quad;
        best_dot = havok_shapes_dot3(best, d);
    case 7:
        dot = havok_shapes_dot3(m_vertices[6].m_simd_quad, d);
        if (havok_shapes_all_greater_or_equal3(dot, best_dot)) {
            best = m_vertices[6].m_simd_quad;
            best_dot = dot;
        }
    case 6:
        dot = havok_shapes_dot3(m_vertices[5].m_simd_quad, d);
        if (havok_shapes_all_greater_or_equal3(dot, best_dot)) {
            best = m_vertices[5].m_simd_quad;
            best_dot = dot;
        }
    case 5:
        dot = havok_shapes_dot3(m_vertices[4].m_simd_quad, d);
        if (havok_shapes_all_greater_or_equal3(dot, best_dot)) {
            best = m_vertices[4].m_simd_quad;
            best_dot = dot;
        }
    case 4:
        dot = havok_shapes_dot3(m_vertices[3].m_simd_quad, d);
        if (havok_shapes_all_greater_or_equal3(dot, best_dot)) {
            best = m_vertices[3].m_simd_quad;
            best_dot = dot;
        }
    case 3:
        dot = havok_shapes_dot3(m_vertices[2].m_simd_quad, d);
        if (havok_shapes_all_greater_or_equal3(dot, best_dot)) {
            best = m_vertices[2].m_simd_quad;
            best_dot = dot;
        }
        dot = havok_shapes_dot3(m_vertices[1].m_simd_quad, d);
        if (havok_shapes_all_greater_or_equal3(dot, best_dot)) {
            best = m_vertices[1].m_simd_quad;
            best_dot = dot;
        }
        dot = havok_shapes_dot3(m_vertices[0].m_simd_quad, d);
        if (havok_shapes_all_greater_or_equal3(dot, best_dot)) {
            best = m_vertices[0].m_simd_quad;
            best_dot = dot;
        }
    }
    vertex.m_simd_quad = best;
}
// @address 0x1058a0 ??0c_collision_surface_shape@@QAA@KJPBUcollision_bsp@@M_N1JJPBUreal_matrix4x3@@W4e_soft_surface_sink_class@@M@Z
c_collision_surface_shape::c_collision_surface_shape(unsigned long user_data, long structure_bsp_index, const collision_bsp *bsp, float scale,
    bool collidable, bool unpooled, long instance_index, long surface_index, const real_matrix4x3 *matrix,
    e_soft_surface_sink_class sink_class, float sink_scale) :
    phys_convex_shape(PHYS_SHAPE_USER0, phys_convex_shape_default_radius * 0.328f)
{
    m_surface_index = surface_index;
    m_structure_bsp_index = (short)structure_bsp_index;
    m_vertex_count = 0;
    m_flags.set_unsafe(collidable | (unpooled << 1));
    m_instance_index = instance_index;
    m_scale = matrix ? 0.0f : scale;

    const collision_surface *surface = &((collision_surface *)bsp->surfaces.address)[surface_index];
    const s_scenario_soft_surface_sink_depths *sink_depths = &global_scenario->soft_surfaces[0];
    long first_edge = surface->first_edge;
    m_best_plane_calculation_vertex_index = surface->best_plane_calculation_vertex_index;
    long edge_index = first_edge;
    do {
        const collision_edge *edge = &((collision_edge *)bsp->edges.address)[edge_index];
        bool side = surface_index - edge->surfaces[1] == 0;
        const collision_vertex *vertex = &((collision_vertex *)bsp->vertices.address)[edge->vertices[side]];
        const real_point3d *point = &vertex->point;
        real_point3d transformed_point;
        if (matrix) {
            point = havok_shapes_matrix4x3_transform_point(matrix, point, &transformed_point);
        } else if (m_scale != 0.0f) {
            point = scale_point3d(point, m_scale, &transformed_point);
        }
        float sink_depth = 0.2f * (1.0f / 32767.0f) * (short)vertex->sink;
        if (sink_depth > sink_depths->maximum_sink_depth[sink_class]) {
            sink_depth = sink_depths->maximum_sink_depth[sink_class];
        }
        m_vertices[m_vertex_count].set(point->x, point->y, point->z - sink_depth, 0.0f);
        m_vertices[m_vertex_count].set_int24_w(m_vertex_count);
        m_vertex_count++;
        edge_index = edge->edges[side];
    } while (edge_index != first_edge);
    m_material_index = surface->material;
    m_surface_flags = surface->flags;
    set_user_data(user_data);
}

// @address 0x4fd610 ?__tls_set_g_ai_reference_frame_data_allocator@@YAXPAX@Z
void __tls_set_g_ai_reference_frame_data_allocator(void *allocator)
{
    g_ai_reference_frame_data = (s_attachment_state *)allocator;
}

// @address 0x5058f0 ?deallocate@?$t_restricted_allocation_manager@$00$0A@$0A@$1?__tls_set_g_ai_reference_frame_data_allocator@@YAXPAX@Z@@UAAXPAX@Z
template<> void t_restricted_allocation_manager<1, static_cast<void (*)(void *)>(0), static_cast<void (*)(void *)>(0),
    &__tls_set_g_ai_reference_frame_data_allocator>::deallocate(void *address)
{
    __tls_set_g_ai_reference_frame_data_allocator(0);
    restricted_region_free_member(1, m_member_index);
}

// @address 0x505910 ?allocate@?$t_restricted_allocation_manager@$00$0A@$0A@$1?__tls_set_g_actor_firing_position_hash_allocator@@YAXPAX@Z@@UAAPAXIPBD@Z
template<> void *t_restricted_allocation_manager<1, static_cast<void (*)(void *)>(0), static_cast<void (*)(void *)>(0),
    &__tls_set_g_actor_firing_position_hash_allocator>::allocate(unsigned int size, const char *name)
{
    long member_index;
    member_index = g_restricted_regions[1].add_member(name, "", size, 0, __tls_set_g_actor_firing_position_hash_allocator, 0, 0);
    char *address_value;
    address_value = restricted_region_get_base_address(1) + g_restricted_regions[1].get_member_offset(member_index);
    g_actor_firing_position_hash = (s_hash_table *)address_value;
    if (restricted_region_get_base_address(1) + g_restricted_regions[1].get_member_offset(member_index) != 0) {
        hash_table_add((s_hash_table *)address_value);
    }
    m_member_index = member_index;
    m_thread_id = get_current_thread_index();
    return restricted_region_get_base_address(1) + g_restricted_regions[1].get_member_offset(member_index);
}


static __forceinline void havok_shapes_identity(phys_transform &transform, phys_vec4 &zero)
{
#if defined(_XBOX) && !defined(__clang__)
    zero.m_simd_quad = __vxor(zero.m_simd_quad, zero.m_simd_quad);
#else
    zero.m_simd_quad = __vzero();
#endif
    transform.get_column(0).m_simd_quad = zero.m_simd_quad;
    transform.get_column(0).m_simd_quad.x = 1.0f;
    transform.get_column(1).m_simd_quad = zero.m_simd_quad;
    transform.get_column(1).m_simd_quad.y = 1.0f;
    transform.get_column(2).m_simd_quad = zero.m_simd_quad;
    transform.get_column(2).m_simd_quad.z = 1.0f;
}
// @address 0x1048c8 ?query_aabb@c_environment_mopp_bv_tree_shape@@UBAXABVphys_aabb@@AAV?$phys_array@I@@@Z
void c_environment_mopp_bv_tree_shape::query_aabb(const phys_aabb &aabb, phys_array<unsigned int> &keys) const
{
    phys_vec4 zero;
    phys_bvh_tree_shape::query_aabb(aabb, keys);
    for (int i = 0; i < keys.getSize(); ++i) {
        if (keys[i] == (unsigned int)-1)
            continue;
        long index, surface_index, type;
        values_from_shape_key(&index, &surface_index, &type, keys[i]);
        if (type == 2) {
            structure_instanced_geometry_instance *instance =
                &((structure_instanced_geometry_instance *)global_structure_bsp_get(
                m_environment_surface_shape->m_structure_bsp_index)->instanced_geometry_instances.address)[index];
            structure_instanced_geometry_definition *definition = &havok_shapes_collision_definitions_get(
                m_environment_surface_shape->m_structure_bsp_index)[instance->definition_index];
            if (definition->bsp.surfaces.count <= 8192) {
                s_collision_bsp_physics_definition *physics = instance->physics.begin();
                phys_transform instance_transform;
                phys_transform box_transform;
                phys_transform relative_transform;
                phys_vec4 half_extents;
                phys_vec4 size;
                phys_transform_from_matrix4x3(&instance_transform, &instance->transform);
                size.set_sub4(aabb.m_max_limit, aabb.m_min_limit);
                half_extents.m_simd_quad = __vmulfp(size.m_simd_quad, phys_simd_scalar(0.5f).m_simd_scalar);
                havok_shapes_identity(box_transform, zero);
                box_transform.get_column(3).m_simd_quad = __vaddfp(aabb.m_min_limit.m_simd_quad, half_extents.m_simd_quad);
                relative_transform.set_mul_inverse_mul(instance_transform, box_transform);
                keys.remove_at_and_copy(i);
                --i;
                int old_count = keys.getSize();
                ((const phys_bvh_tree_shape *)physics->m_mopp_shape)->query_obb(relative_transform, half_extents, 0.0001f, keys);
                for (int j = old_count; j < keys.getSize(); ++j)
                    keys[j] = shape_key_from_values(index, keys[j], 3);
            }
        }
    }
}



static __forceinline void havok_shapes_point_from_vector(const phys_vec4 &v, real_point3d *result)
{
    result->x = v.m_simd_quad.x; result->y = v.m_simd_quad.y; result->z = v.m_simd_quad.z;
}
static __forceinline void havok_shapes_vector_from_points(const real_point3d &a, const real_point3d &b, real_vector3d *result)
{
    result->i = b.x - a.x; result->j = b.y - a.y; result->k = b.z - a.z;
}
static __forceinline void havok_shapes_project_point(const real_point3d *point, short projection, bool sign, real_point2d *result)
{
    float x = point->n[global_projection3d_mappings[projection][sign][0]];
    float y = point->n[global_projection3d_mappings[projection][sign][1]];
    result->x = x;
    result->y = y;
}
static __forceinline void havok_shapes_project_point(const phys_vec4 *point, short projection, bool sign, real_point2d *result)
{
    float x = (*point)(global_projection3d_mappings[projection][sign][0]);
    float y = (*point)(global_projection3d_mappings[projection][sign][1]);
    result->x = x;
    result->y = y;
}
static __forceinline short havok_shapes_projection(const real_vector3d *normal)
{
    float x = (float)fabs(normal->i), y = (float)fabs(normal->j), z = (float)fabs(normal->k);
    if (z >= y && z >= x)
        return 2;
    else if (y >= x)
        return 1;
    else
        return 0;
}
static __forceinline const phys_vec4 *havok_shapes_get_vertex(const c_collision_surface_shape *shape, long index)
{
    return &shape->m_vertices[index];
}
static __forceinline void havok_shapes_cross_product(const real_vector3d *a, const real_vector3d *b, real_vector3d *result)
{
    result->i = a->j * b->k - a->k * b->j;
    result->j = a->k * b->i - a->i * b->k;
    result->k = a->i * b->j - a->j * b->i;
}
// match: 0x25f038: optimize pragma keeps the vertex-index modulo an unchecked divw sequence
#pragma optimize("c", on)
static __forceinline const phys_vec4 *havok_shapes_plane_vertex(const c_collision_surface_shape *shape, long index)
{
    unsigned char first = shape->m_best_plane_calculation_vertex_index;
    if (index != 0)
        return &shape->m_vertices[(first + index) % shape->m_vertex_count];
    return &shape->m_vertices[first];
}
// @address 0x25f038 ?trace_ray_impl@c_collision_surface_shape@@UBA?AVphys_bool@@ABUphys_ray_input@@AAUphys_ray_output@@@Z
phys_bool c_collision_surface_shape::trace_ray_impl(const phys_ray_input &input, phys_ray_output &output) const
{
    phys_bool result;
    result = false;
    real_point3d from; havok_shapes_point_from_vector(input.m_ray_from, &from);
    real_point3d to; havok_shapes_point_from_vector(input.m_ray_to, &to);
    real_point3d a; havok_shapes_point_from_vector(*havok_shapes_plane_vertex(this, 0), &a);
    real_point3d b; havok_shapes_point_from_vector(*havok_shapes_plane_vertex(this, 1), &b);
    real_point3d c; havok_shapes_point_from_vector(*havok_shapes_plane_vertex(this, 2), &c);
    real_vector3d ab; havok_shapes_vector_from_points(a, b, &ab);
    real_vector3d ac; havok_shapes_vector_from_points(a, c, &ac);
    if (m_scale != 0.0f) {
        scale_point3d(&from, m_scale, &from);
        scale_point3d(&to, m_scale, &to);
    }
    real_vector3d direction; havok_shapes_vector_from_points(from, to, &direction);
    __declspec(align(16)) real_plane3d plane;
    havok_shapes_cross_product(&ab, &ac, &plane.normal);
    if (normalize3d(&plane.normal) != 0.0f) {
        plane.distance = 0.0f;
        real_point3d relative = { from.x - a.x, from.y - a.y, from.z - a.z };
        float fraction = vector_intersect_plane3d(&relative, &direction, &plane);
        if (fraction < output.m_hit_frac && fraction >= 0.0f && fraction <= 1.0f) {
            long projection = havok_shapes_projection(&plane.normal);
            bool sign = plane.normal.n[(short)projection] > 0.0f;
            real_point3d point = { direction.i * fraction + from.x, direction.j * fraction + from.y, direction.k * fraction + from.z };
            real_point2d projected;
            havok_shapes_project_point(&point, (short)projection, sign, &projected);
            real_point2d vertices[8];
            for (long i = 0; i < m_vertex_count; ++i)
                havok_shapes_project_point(havok_shapes_get_vertex(this, i), (short)projection, sign, &vertices[i]);
            if (convex_hull2d_test_point(m_vertex_count, vertices, &projected, 0.0f)) {
                output.m_hit_frac = fraction;
                output.m_normal.set(plane.normal.i, plane.normal.j, plane.normal.k, 0.0f);
                result = true;
            }
        }
    }
    return result;
}
// match: 0x25f038: optimize pragma keeps the vertex-index modulo an unchecked divw sequence
#pragma optimize("c", off)


// @address 0x260138 ?get_next_key@c_environment_surface_shape@@UBAII@Z
unsigned int c_environment_surface_shape::get_next_key(unsigned int old_key) const
{
    structure_bsp *structure = global_structure_bsp_get(m_structure_bsp_index);

    unsigned int result = (unsigned int)-1;
    long count = get_bsp_surface_count(havok_shapes25ffd0_collision_bsp_get(&structure->resource));
    bool found = false;
    long index, sub_index, type;
    values_from_shape_key(&index, &sub_index, &type, old_key);
    if (type == 1) {
        collision_bsp *bsp = havok_shapes25ffd0_collision_bsp_get(&structure->resource);
        if (surface_has_sink(bsp, index) && sub_index < 4) {
            found = true;
            result = shape_key_from_values(index, sub_index + 1, 1);
        }
        if (!found && index < count - 1) {
            ++index;
            bool sink = surface_has_sink(bsp, index);
            result = shape_key_from_values(index, sink ? 0 : 5, 1);
            found = true;
        }
        if (found)
            return result;
        index = -1;
    }
    if (type == 1 || type == 2) {
        long instance_count = structure->instanced_geometry_instances.count;
        if (instance_count > 0 && index < instance_count - 1) {
            structure_instanced_geometry_definition *definitions = havok_shapes_collision_definitions_get(m_structure_bsp_index);
            for (; index < instance_count - 1; ++index) {
                const structure_instanced_geometry_instance *instance =
                    &((structure_instanced_geometry_instance *)structure->instanced_geometry_instances.address)[index + 1];
                const structure_instanced_geometry_definition *definition = &definitions[instance->definition_index];
                if (definition->bsp.surfaces.count != 0 && definition->collision_mopp_codes.count != 0 && instance->physics.count() != 0 &&
                    !(instance->flags & 2)) {
                    return shape_key_from_values(index + 1, 0, 2);
                }
            }
        }
    }
    return result;
}

// NOTE: the thread-hook twins (havok_thread_memory_allocate/deallocate_stack,
// havok_thread_user_data_get, havok_next_thread_paramerters_get,
// havok_notify_thread_started/stopped) are intentionally NOT defined here. Verified over three
// image checks: defining one drops its section as a fold while its live native-side callers
// cannot be rebound (folds only rebind inside the merged /GL object), so the final link fails
// LNK2001; and which twin fails flip-flops as HEAD churns native callers (allocate failed on
// two HEADs, deallocate on another). The hkAlgorithm::quickSortRecursive twins
// (hkEntity/hkConstraintInstance) fail the same way: folded, their live native caller in
// hkContinuousSimulation dangles; surviving, they displace the recorded twin past the end of
// .text and diff its call site. All of these need fold-aware native rebinding (or equivalent
// wpl support); until then the absolute-address fallback covers callers. March owners for that
// future work: havok_memory.obj 0x82bd6cc8/0x82bd6df0, threads.obj
// 0x8295c340/0x8295c0b8/0x8295c1e8/0x8295c230.
// A fourth check confirms explicit @address definitions fail the same way (LNK2001 on a native caller).

#if defined(__clang__)
// Port-only bodies for the thread-hook twins: March defines them (havok_memory.obj
// 0x82bd6cc8/0x82bd6df0, threads.obj 0x8295c340/0x8295c0b8/0x8295c1e8/0x8295c230); the linker
// folds all six into the shared trap at 0x1b65a8. Xbox callers (native Havok objects) bind the
// tag-absolute per the NOTE above. The PC link needs bodies from source; they trap, matching
// the folded bytes.
void *havok_thread_memory_allocate_stack(int size)
{
    (void)size;
    __builtin_trap();
}
void havok_thread_memory_deallocate_stack(void *block)
{
    (void)block;
    __builtin_trap();
}
void *havok_thread_user_data_get(long thread_index)
{
    (void)thread_index;
    __builtin_trap();
}
void havok_next_thread_paramerters_get(long thread_index, long *out_thread_index, unsigned long *out_stack_size)
{
    (void)thread_index;
    (void)out_thread_index;
    (void)out_stack_size;
    __builtin_trap();
}
void havok_notify_thread_started(void *thread_handle, long thread_index, unsigned long thread_id, void *argument)
{
    (void)thread_handle;
    (void)thread_index;
    (void)thread_id;
    (void)argument;
    __builtin_trap();
}
void havok_notify_thread_stopped(void *thread_handle)
{
    (void)thread_handle;
    __builtin_trap();
}
#endif

// March havok_listeners.obj (0x82cfed30): ASSERT(unreachable) at havok_listeners.cpp:568; release
// ASSERT compiles out, so the shared trap is explicit like getCollisionSpheresImpl above.
// @address 0x1b65a8 ?enable_all_reports@c_havok_error@@UAAXXZ
void c_havok_error::enable_all_reports()
{
    __debugbreak();
    // match: 0x1b65a8: __assume(0) omits the trailing blr after __debugbreak
    __assume(0);
}

// March havok_shapes.obj (0x82bbaf08): empty body returning 0; collision surfaces provide no spheres.
// Shares its bytes with the return-0 stubs at 0x42d298.
// @address 0x42d298 ?get_collision_sphere_count_impl@c_collision_surface_shape@@UBAHXZ
int c_collision_surface_shape::get_collision_sphere_count_impl() const
{
    return 0;
}

