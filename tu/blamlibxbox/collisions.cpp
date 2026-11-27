// blamlibxbox/collisions.obj: from the records 0x9d590 0x9d600 0x9e200 0x9e7e0 0x9eb08 0x9efa0 0x9f170 0x9f390 0xb0d20 0xba9b8 0xe3938 0xf3db0 0x1066c8 0x1067c0 0x112cc8 0x122d90 0x28a790 0x28a8f0 0x28ac38 0x28ad38 0x28aea8 0x28aed8 0x28b060 0x28b218 0x28b3e8 0x28ba40 0x28c3c0 0x28c500 0x28c640 0x28cd58 0x28cf50 0x28d490
// @flags /TP /O2 /Oc
#define REAL_MATH_PLANE_DISTANCE_LINKAGE static inline
#include "math/real_math.h"
#undef REAL_MATH_PLANE_DISTANCE_LINKAGE
#include "cache/cache_files.h"
#include "tag_files/tag_types.h"
#include "physics/breakable_surface_globals.h"
#include "structures/structure_seam_globals.h"
#include "objects/object_reference_types.h"
#include "math/real_math_bounds_storage.h"
#include "objects/object_cluster_globals.h"
#include "memory/tls_data_arrays.h"
#include "multithreading/thread_tls.h"
#include "physics/collision_types.h"
#include "physics/collision_materials.h"
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "physics/collision_bsp_types.h"
#include "models/model_types.h"
#include "render_methods/render_method_definitions.h"
#include "math/real_math_types.h"
#include "ai/pathfinding_data.h"
#include "objects/objects.h"
#include "scenario/scenario.h"
#include "ai/sector.h"
#include "scenario/scenario_access.h"
#include "scenario/cluster_reference_helpers.h"
#include "physics/breakable_surfaces.h"
#include "physics/bsp3d.h"
#include "physics/collision_features.h"
#include "physics/collision_feature_types.h"
#include "physics/collision_models.h"
#include "physics/collisions.h"
#if !defined(_XBOX) || defined(__clang__)
#include "physics/collision_test_vector.h"
#endif
#include "physics/collision_vector_query.h"
#include "structures/structures.h"
#include "scenario/scenario_soft_ceilings.h"
#include "objects/object_datum.h"
#include "cseries/cseries_wrapped.h"
#include "objects/object_broadphase.h"
#include "structures/cluster_partitions.h"
#include "scenario/scenario_structure_bsp_reference.h"
#include "scenario/scenario_tag.h"
#include "objects/object_marker_globals_types.h"
#include "physics/collision_model_bsp_iterator_types.h"
#include "cache/cache_file_globals.h"
#include "physics/collision_result_types.h"
#include "math/matrix_math_functions.h"
#include "math/real_math_functions.h"
#include "math/real_math_globals.h"
#include "math/matrix_math_constants.h"
#include "math/real_constant_pool.h"
#include "items/projectiles.h"
#include "physics/collision_globals.h"
#include "math/matrix_math_types.h"
#include "physics/collision_bsp_query_types.h"
#include "math/point_math.h"
#include "units/unit_types.h"
#include "objects/scenery.h"
#include "objects/crates.h"
#include "memory/data_types.h"
#include "memory/datum_index.h"
#include "structures/structure_instanced_geometry.h"
#include "structures/structure_instanced_geometry_definition.h"
#include "structures/structure_bsp_tag.h"
#include "geometry/render_geometry_mesh_types.h"
#include "physics/collision_bsp_test_context.h"
#include "physics/havok_runtime_pool_types.h"
#include "physics/collision_model_definition_types.h"
#include "physics/collision_model_types.h"
#include "structures/cluster_partition_types.h"
#include "physics/collision_bsp.h"
#include "physics/collision_instanced_geometry.h"
#include <ppcintrinsics.h>
#include <vectorintrinsics.h>
#include "physics/havok_math.h"
#include "phys/phys_collide.h"
#include "units/unit_definitions.h"
#include "math/real_math_fraction_constants.h"
#include "geometry/geometry_sampling.h"
#include "render_methods/render_method.h"
extern "C" {
#include "math/plane_intersections.h"
}
#include "structures/structure_seams.h"
#include "structures/structure_seam_cluster_stack.h"
#include "physics/breakable_surface_types.h"
#include "physics/collisions_breakable_surface_types.h"
#include "structures/structure_globals.h"
#include "structures/structure_runtime_resources.h"
#include "objects/object_broadphase_types.h"
#include "objects/object_header.h"
#include "tag_files/tag_groups.h"
#include "memory/static_arrays.h"
extern "C" {
#include "objects/object_globals.h"
}


typedef struct __regfix_ret_collisions_subtract_vectors3d { real_vector3d *v; } __regfix_ret_collisions_subtract_vectors3d;

struct s_zxy_vector3d { float z, x, y; };

struct s_breakable_surface_set;

struct collision_model_instance;

struct bsp3d;
struct bsp3d;

typedef char collisions_materials_offset[(offsetof(structure_bsp, collision_materials) == 0x24) ? 1 : -1];

typedef char collisions_breakable_surfaces_offset[(offsetof(structure_bsp, breakable_surfaces) == 0xe4) ? 1 : -1];

typedef char collisions_bounds_offset[(offsetof(structure_bsp, structure_physics) + offsetof(s_structure_physics,
    mopp_bounds_min) == 0x298) ? 1 : -1];

typedef char collisions_meshes_offset[(offsetof(structure_bsp, render_geometry) + offsetof(s_render_geometry, meshes) == 0x2e4) ? 1 : -1];

struct s_breakable_surface_set_table;


struct s_breakable_surface_set;

typedef char collisions_cluster_shape_offset[(offsetof(structure_cluster, collision_instanced_geometry)
    + offsetof(s_collision_instanced_geometry_definition, m_mopp_shape) == 0x74) ? 1 : -1];

typedef char collisions_cluster_mopp_codes_offset[(offsetof(structure_cluster, collision_instanced_geometry)
    + offsetof(s_collision_instanced_geometry_definition, m_mopp_codes) == 0x90) ? 1 : -1];


struct s_rmtr_definition
{
    c_render_method shader;
    long prediction_atom_index;
    long material_names[4];
    unsigned short materials[4];
    bool single_material;
};

struct s_structure_bsp_entry_fraction {
    float fraction;
    long structure_bsp_index;
};

// @address 0x1f98 ??_7c_instanced_geometry_ray_hit_collector@@6B@
// @address 0x25be10 ??_Ec_instanced_geometry_ray_hit_collector@@UAAPAXI@Z
// @address 0xb0ab0 ?add_ray_hit@c_instanced_geometry_ray_hit_collector@@MAAXABVphys_collision_proxy@@ABUphys_ray_collector_output@@@Z
// @address 0x7c5d34 k_null_rectangle3d_storage
// @address 0x9e7e0 ?collision_test_point@@YA_NUs_collision_test_flags@@PBTreal_point3d@@JJPAW4e_collision_result_type@@@Z
// @address 0xb0d20 ?instanced_geometry_test_vector_internal@@YA_NPBUs_collision_bsp_test_context@@JKKPBTreal_point3d@@PBTreal_vector3d@@PAUcollision_result@@@Z
// @address 0x28a790 ?instanced_geometry_test_point@@YA_NPBUs_collision_bsp_test_context@@JKPBTreal_point3d@@@Z
// @address 0x9d590 ?collision_test_line@@YA_NUs_collision_test_flags@@PBTreal_point3d@@1JJPAUcollision_result@@@Z
// @address 0x9efa0 ?collision_compute_cull_flags@@YAGUs_collision_test_flags@@@Z
// @address 0x9eb08 collision_test_cluster_instanced_geometry
// @address 0x9f390 collision_test_object
// @address 0xba9b8 ?build_collision_result_from_bsp_result@@YAXPAUcollision_result@@PBUcollision_bsp_test_vector_result@@PBUreal_matrix4x3@@@Z
// @address 0xe3938 ?collision_compute_object_cull_flags@@YAGJ@Z
// @address 0x28aea8 ?collisions_compare_floats@@YAHPBM0@Z
// @address 0x28cd58 ?collision_structure_ground_plane_process_surfaces@@YAJPBUcollision_bsp_test_sphere_result@@PBUreal_matrix4x3@@PBUcollision_bsp@@PBTreal_point3d@@PBTreal_vector3d@@JJPAUreal_plane3d@@@Z
// @address 0x28a8f0 object_test_point_internal
// @address 0x28aed8 ?collision_test_cluster@@YA_NPBUs_collision_bsp_test_context@@Us_cluster_reference@@Us_collision_test_flags@@KPBTreal_point3d@@PBTreal_vector3d@@JJJPAUcollision_result@@@Z
// @address 0xf3db0 ?get_bsp_from_collision_result@@YA_NPAUcollision_result@@PAPBUcollision_bsp@@PAPBUreal_matrix4x3@@@Z
// @address 0x28c500 ?collision_find_good_projectile_point@@YA_NPBTreal_point3d@@0PAT1@JJ@Z
// @address 0x9b308 ?normalize3d@@YAMPATreal_vector3d@@@Z
// @address 0x9bc18 ?bsp3d_test_point@@YAJPBUbsp3d@@JPBTreal_point3d@@@Z
// @address 0x9f8f0 ?collision_model_bsp_iterator_new@@YAXPAUcollision_model_bsp_iterator@@PBUs_model_definition@@PBUcollision_model_definition@@PBD@Z
// @address 0x9f9d0 ?collision_model_bsp_iterator_next@@YA_NPAUcollision_model_bsp_iterator@@@Z
// @address 0x9fb10 ?matrix4x3_inverse_transform_point@@YAPATreal_point3d@@PBUreal_matrix4x3@@PBT1@PAT1@@Z
// @address 0x9fbf0 ?matrix4x3_inverse_transform_vector@@YAPATreal_vector3d@@PBUreal_matrix4x3@@PBT1@PAT1@@Z
// @address 0xbaae8 ?matrix4x3_transform_plane@@YAPAUreal_plane3d@@PBUreal_matrix4x3@@PBU1@PAU1@@Z
// @address 0x9e5d0 ?collision_bsp_test_vector@@YA_NKPBUcollision_bsp@@PBUs_collision_materials_extant_flags@@FPBUs_breakable_surface_set@@PBTreal_point3d@@PBTreal_vector3d@@MPAUcollision_bsp_test_vector_result@@@Z
// @address 0x9c4f0 ?cluster_partition_get_first_datum_and_payload@@YAJPBUcluster_partition@@PAJUs_cluster_reference@@JPAPBX@Z
// @address 0x9e520 ?breakable_surface_get_surface_set_count@@YAJJJ@Z
// @address 0x7cff08 ?global_scenario@@3PAUscenario@@A
// @address 0x7cff0c ?global_tag_instances@@3PBUcache_file_tag_instance@@B
// @address 0x8a1ecc ?g_active_structure_bsp_mask@@3KA
// @address 0x9f098 ?fast_vector_intersects_sphere@@YA_NPBTreal_point3d@@PBTreal_vector3d@@0M@Z
// @address 0x3aac8 k_real_math_identity_matrix_storage


static __forceinline bool collision_flag_test(unsigned long flags, long bit)
{
    long shift = 32 - bit;
    unsigned long rotated_flags = _rotl(flags, shift);
    return (rotated_flags & 1) != 0;
}


inline bool collision_test_line(s_collision_test_flags flags, bool test_exit, const real_point3d *point0, const real_point3d *point1,
    long first_ignore_object_index, long second_ignore_object_index, collision_result *collision)
{
    real_vector3d vector;
    vector_from_points3d(point0, point1, &vector);
    return collision_test_vector(flags, test_exit, point0, &vector, first_ignore_object_index, second_ignore_object_index, -1, collision);
}

bool collision_test_line(s_collision_test_flags flags, const real_point3d *point0, const real_point3d *point1,
    long first_ignore_object_index, long second_ignore_object_index, collision_result *collision)
{
    return collision_test_line(flags, false, point0, point1, first_ignore_object_index, second_ignore_object_index, collision);
}

inline unsigned long build_bsp_flags_from_collision_flags(unsigned long collision_flags)
{
    unsigned long bsp_flags = 0;
    if (collision_flag_test(collision_flags, 11)) bsp_flags |= 1; else bsp_flags &= ~1ul;
    if (collision_flag_test(collision_flags, 12)) bsp_flags |= 2; else bsp_flags &= ~2ul;
    if (collision_flag_test(collision_flags, 13)) bsp_flags |= 4; else bsp_flags &= ~4ul;
    if (collision_flag_test(collision_flags, 14)) bsp_flags |= 8; else bsp_flags &= ~8ul;
    if (collision_flag_test(collision_flags, 15)) bsp_flags |= 16; else bsp_flags &= ~16ul;
    if (collision_flag_test(collision_flags, 16)) bsp_flags |= 32; else bsp_flags &= ~32ul;
    return bsp_flags;
}


static inline real_plane3d *collisions_bsp3d_get_plane_from_designator(const collision_bsp *bsp, long designator, real_plane3d *plane)
{
    const real_plane3d *bsp_plane = &((const real_plane3d *)bsp->planes.address)[designator & 0x7fff];
    if (designator & 0x8000)
        plane3d_negate(bsp_plane, plane);
    else
        *plane = *bsp_plane;
    return plane;
}

static inline real_vector3d *collisions_matrix4x3_rotate_vector3d(const real_matrix4x3 *matrix, const real_vector3d *vector,
    real_vector3d *result)
{
    float i = vector->i;
    float j = vector->j;
    float k = vector->k;
    result->i = matrix->forward.i * i + matrix->left.i * j + matrix->up.i * k;
    result->j = matrix->forward.j * i + matrix->left.j * j + matrix->up.j * k;
    result->k = matrix->forward.k * i + matrix->left.k * j + matrix->up.k * k;
    return result;
}

static inline real_plane3d *transform_surface_plane(const real_matrix4x3 *matrix, const real_plane3d *plane,
    real_plane3d *result)
{
    collisions_matrix4x3_rotate_vector3d(matrix, &plane->normal, &result->normal);
    result->distance = matrix->scale * plane->distance + dot_product3d(&matrix->position, &result->normal);
    return result;
}





static __forceinline bool collisions_test_instanced_geometry(
    const s_collision_bsp_test_context *context, long instance_index,
    const structure_instanced_geometry_instance *instance,
    const structure_instanced_geometry_definition *definition, unsigned long flags)
{
    bool result = true;
    if (definition->bsp.surfaces.count > 0) {
        if ((bool)((flags >> 6) & 1)) {
            const pathfinding_data *runtime = context->runtime;
            if (runtime) {
                if (instance_index >= 0 && instance_index < runtime->instanced_geometry_refs.count) {
                    const s_pathfinding_instanced_geometry_ref *runtime_instance =
                        &((const s_pathfinding_instanced_geometry_ref *)runtime->instanced_geometry_refs.address)[instance_index];
                    if (runtime_instance->index == 0xffff)
                        result = false;
                } else {
                    result = false;
                }
            }
        }
    } else {
        result = false;
    }
    return result;
}






// 0x9e7e0
bool collision_test_point(s_collision_test_flags flags, const real_point3d *point, long first_ignore_object_index,
    long second_ignore_object_index, e_collision_result_type *result_type)
{
    bool hit = false;
    e_collision_result_type type = _collision_result_none;
    for (long structure_bsp_index = 0; structure_bsp_index < 16; structure_bsp_index++) {
        if (global_structure_bsp_is_active(structure_bsp_index)) {
            long leaf_index = bsp3d_test_point(::global_bsp3d_get(structure_bsp_index), 0, point);
            if (leaf_index == -1) {
                hit = true;
                type = _collision_result_structure;
                continue;
            } else {
                hit = false;
                if (!(bool)(_rotl(flags.collision_flags, 31) & 1) && !(bool)(_rotl(flags.collision_flags, 29) & 1)
                    && !(flags.object_flags & 1)) {
                    break;
                } else {
                    const structure_bsp *structure_bsp = global_structure_bsp_get(structure_bsp_index);
                    s_cluster_reference cluster;
                    cluster.bsp_index = (signed char)structure_bsp_index;
                    cluster.cluster_index = ((const unsigned char *)structure_bsp->leaves.address)[leaf_index];
                    s_collision_bsp_test_context context;
                    collision_bsp_test_context_initialize(structure_bsp_index, &context);
                    if ((bool)(_rotl(flags.collision_flags, 29) & 1) && !hit) {
                        const structure_cluster *structure_cluster =
                            &((const struct structure_cluster *)context.structure_bsp->clusters.address)[cluster.cluster_index];
                        const s_mesh *group =
                            &((const s_mesh *)context.structure_bsp->render_geometry.meshes.address)[structure_cluster->mesh_index];
                        for (long group_index = 0; !hit && group_index < group->instance_buckets.count; group_index++) {
                            const s_mesh_instance_bucket *supernode =
                                &((const s_mesh_instance_bucket *)group->instance_buckets.address)[group_index];
                            for (long index = 0; !hit && index < supernode->instances.count(); index++) {
                                unsigned short instance_index = ((const unsigned short *)supernode->instances.address)[index];
                                if (instanced_geometry_test_point(&context, instance_index, flags.collision_flags, point)) {
                                    hit = true;
                                    type = _collision_result_instanced_geometry;
                                }
                            }
                        }
                    }
                    if ((flags.object_flags & 1) && !hit) {
                        const void *payload_data = 0;
                        unsigned short cull_flags = collision_compute_cull_flags(flags);
                        unsigned short object_types = (unsigned short)((flags.object_flags >> 1) & 0x3fff);
                        long iterator;
                        for (long object_index = cluster_partition_get_first_datum_and_payload(
                            &tls_collideable_object_cluster_partition(), &iterator, cluster, 20, &payload_data);
                            object_index != -1; object_index = cluster_partition_get_next_datum_and_payload(&iterator, &payload_data)) {
                            const s_object_broadphase_handle_data *payload = (const s_object_broadphase_handle_data *)payload_data;
                            if ((object_types & (1 << payload->object_type)) && !(cull_flags & payload->cull_flags) &&
                                point_in_sphere(point, &payload->center, payload->radius) &&
                                object_index != first_ignore_object_index && object_index != second_ignore_object_index &&
                                object_test_point_internal(object_index, true, flags, point, first_ignore_object_index,
                                    second_ignore_object_index)) {
                                hit = true;
                                type = _collision_result_object;
                                break;
                            }
                        }
                    }
                }
                break;
            }
        }
    }
    if (result_type)
        *result_type = type;
    return hit;
}

// 0x9eb08
extern "C" bool collision_test_cluster_instanced_geometry(const s_collision_bsp_test_context *context,
    s_cluster_reference cluster_reference,
    unsigned long flags, unsigned long bsp_flags, const real_point3d *point, const real_vector3d *vector,
    collision_result *result)
{
    bool hit = false;
    const structure_cluster *cluster =
        &((const struct structure_cluster *)context->structure_bsp->clusters.address)[cluster_reference.cluster_index];
    if (cluster->collision_instanced_geometry.m_mopp_codes.count > 0) {
        phys_ray_input input;
        phys_motion_state motion;
        const phys_shape *mopp_shape = collision_instanced_geometry_get_mopp_shape(&cluster->collision_instanced_geometry);
        phys_collider collidable(mopp_shape, &motion, 0);
        c_instanced_geometry_ray_hit_collector collector(context, flags, bsp_flags, point, vector, result);
        real_point3d end;
        point_from_line3d(point, vector, 1.0f, &end);
        phys_vec4_from_real_point3d(&input.m_ray_from, point);
        phys_vec4_from_real_point3d(&input.m_ray_to, &end);
        motion.m_body_transform.set_to_identity();
        mopp_shape->trace_ray_with_collector(input, collidable, collector);
        hit = collector.m_hit;
    }
    return hit;
}


// 0x9efa0
unsigned short collision_compute_cull_flags(s_collision_test_flags test_flags)
{
    unsigned long collision_flags = test_flags.collision_flags;
    unsigned short flags = 0;
    flags |= 1;
    if (collision_flag_test(collision_flags, 8)) flags |= 2; else flags &= ~2;
    if (collision_flag_test(collision_flags, 6)) flags |= 4; else flags &= ~4;
    if (collision_flag_test(collision_flags, 7)) flags |= 8; else flags &= ~8;
    if (collision_flag_test(collision_flags, 18)) flags |= 16; else flags &= ~16;
    if (collision_flag_test(collision_flags, 9)) flags |= 32; else flags &= ~32;
    if (collision_flag_test(collision_flags, 10)) flags |= 64; else flags &= ~64;
    if (collision_flag_test(collision_flags, 19)) flags |= 128; else flags &= ~128;
    if (collision_flag_test(collision_flags, 20)) flags |= 0x100; else flags &= ~0x100;
    if (collision_flag_test(collision_flags, 21)) flags |= 0x400; else flags &= ~0x400;
    return flags;
}


static __forceinline bool object_excluded_by_visibility(const s_object_header *header, const object_datum *object)
{
    return (header->flags & 16) || (object->flags & 1);
}


static __forceinline bool collisions_object_is_boarding(long index)
{
    bool result = false;
    if ((1 << object_header_get(index)->object_type) & 0x1003) {
        const object_datum *unit = object_get(index);
        if (unit->parent_object_index != -1 && ((const unit_datum *)unit)->unit.parent_seat_index != -1) {
            const object_datum *parent = object_get(unit->parent_object_index);
            const unit_definition *definition = TAG_GET('unit', unit_definition, parent->definition_index);
            const unit_seat *seat = &((const unit_seat *)definition->seats.address)[((const unit_datum *)unit)->unit.parent_seat_index];
            result = seat->index != -1;
        }
    }
    return result;
}

extern "C" bool collision_test_object(long index, const s_object_header *header, const object_datum *object,
    s_collision_test_flags flags, long ignore_a, long ignore_b, long ignore_c)
{
    bool result = false;
    if (index != ignore_a && index != ignore_b && index != ignore_c && !object_excluded_by_visibility(header, object)) {
        bool parent_a = false, parent_b = false, parent_c = false;
        for (long parent = index; parent != -1; parent = object_get(parent)->parent_object_index) {
            parent_a |= parent == ignore_a;
            parent_b |= parent == ignore_b;
            parent_c |= parent == ignore_c;
        }
        if ((!parent_a && !parent_b && !parent_c) || collisions_object_is_boarding(index)) {
            long type = header->object_type;
            if (flags.object_flags & (1UL << (type + 1))) {
                if (!(collision_flag_test(flags.collision_flags, 8) && collision_flag_test(object->flags, 20)) &&
                    !(collision_flag_test(flags.collision_flags, 6) && !collision_flag_test(object->flags, 22)) &&
                    !(collision_flag_test(flags.collision_flags, 7) && (object->ai_flags & 4)) &&
                    !(collision_flag_test(flags.collision_flags, 18) && !collision_flag_test(object->flags, 31)) &&
                    !(collision_flag_test(flags.collision_flags, 20) && collision_flag_test(object->physics_flags, 21)) &&
                    !(collision_flag_test(flags.collision_flags, 21) && collision_flag_test(object->physics_flags, 22))) {
                    switch (type) {
                    case 0: if (!(collision_flag_test(flags.collision_flags, 9) && collision_flag_test(object->damage_flags,
                        2))) result = true; break;
                    case 6: if (!(collision_flag_test(flags.collision_flags, 19)
                        && ((long)((const s_scenery_object *)object)->flags & 8))) result = true; break;
                    case _object_type_crate:
                    if (!(collision_flag_test(flags.collision_flags, 10) && collision_flag_test(((const crate_datum *)object)->flags, 1)))
                        result = true;
                    break;
                    default: result = true; break;
                    }
                }
            }
        }
    }
    return result;
}

// 0xb0d20
bool instanced_geometry_test_vector_internal(const s_collision_bsp_test_context *context, long instance_index,
    unsigned long flags, unsigned long bsp_flags, const real_point3d *point, const real_vector3d *vector,
    collision_result *result)
{
    bool success = false;
    const structure_bsp *structure_bsp = context->structure_bsp;
    const structure_instanced_geometry_instance *instance =
        &((const structure_instanced_geometry_instance *)structure_bsp->instanced_geometry_instances.address)[instance_index];
    const structure_instanced_geometry_definition *definition =
        &((const structure_instanced_geometry_definition *)context->resources->instanced_geometry_definitions.address)[instance->definition_index];
    if (!(instance->flags & 2) &&
        (!(bool)((flags >> 10) & 1) || !(instance->flags & 4)) &&
        collisions_test_instanced_geometry(context, instance_index, instance, definition, flags) &&
        fast_vector_intersects_sphere(point, vector, &instance->world_bounding_sphere_center, instance->world_bounding_sphere_radius)) {
        const collision_bsp *bsp = &definition->bsp;
        real_point3d local_point;
        real_vector3d local_vector;
        matrix4x3_inverse_transform_point(&instance->transform, point, &local_point);
        matrix4x3_inverse_transform_vector(&instance->transform, vector, &local_vector);
        if ((bool)((flags >> 4) & 1) && definition->render_bsp.count != 0)
            bsp = (const collision_bsp *)definition->render_bsp.address;
        collision_bsp_test_vector_result bsp_result;
        if (collision_bsp_test_vector(bsp_flags, bsp, 0,
            (short)breakable_surface_get_surface_set_count(context->structure_bsp_index, instance_index),
            ::breakable_surface_get_first_set(context->structure_bsp_index, instance_index),
            &local_point, &local_vector, result->fraction, &bsp_result)) {
            result->type = 3;
            result->bsp = bsp;
            const c_global_material_type *material = (short)bsp_result.material != -1
                ? &TAG_BLOCK_GET_ELEMENT(&context->structure_bsp->collision_materials, (short)bsp_result.material,
                      const s_collision_material)->global_material
                : &k_global_material_type_invalid;
            result->global_material = *material;
            result->geometry_index = -1;
            result->instance_index = instance_index;
            result->object_index = -1;
            result->structure_bsp_index = context->structure_bsp_index;
            build_collision_result_from_bsp_result(result, &bsp_result, &instance->transform);
            success = true;
        }
    }
    return success;
}

inline void build_collision_result_from_bsp_result(collision_result *result, const collision_bsp_test_vector_result *bsp,
    const real_matrix4x3 *matrix) {
    result->fraction = bsp->fraction;
    if (matrix) {
        matrix4x3_transform_plane(matrix, bsp->plane, &result->plane);
        if ((int)bsp->plane_index & 0x8000)
            plane3d_negate(&result->plane, &result->plane);
    } else {
        if ((int)bsp->plane_index & 0x8000)
            plane3d_negate(bsp->plane, &result->plane);
        else
            result->plane = *bsp->plane;
    }
    result->leaf_index = bsp->leaf_index;
    result->surface_index = bsp->surface_index;
    result->plane_index = bsp->plane_index;
    result->flags = bsp->flags;
    result->breakable_surface = bsp->breakable_surface;
    result->breakable_surface_bit = bsp->breakable_surface_bit;
    result->material_index = bsp->material;
}

unsigned short collision_compute_object_cull_flags(long object_index)
{
    s_object_header *header = object_header_get(object_index);
    object_datum *detail = (object_datum *)header->datum;
    int kind = detail->object_type;
    unsigned short flags = 0;
    bool excluded;
    if ((header->flags & 0x10) != 0 || (detail->flags & 1) != 0) excluded = true; else excluded = false;
    if (excluded) flags = 1;
    if ((unsigned char)((detail->flags >> 20) & 1) != 0) flags |= 2; else flags &= ~2;
    if ((unsigned char)((detail->flags >> 22) & 1) == 0) flags |= 4; else flags &= ~4;
    if ((detail->ai_flags & 4) != 0) flags |= 8; else flags &= ~8;
    if ((unsigned char)((detail->physics_flags >> 21) & 1) != 0) flags |= 0x100; else flags &= ~0x100;
    if ((unsigned char)((detail->physics_flags >> 22) & 1) != 0) flags |= 0x400; else flags &= ~0x400;
    if ((unsigned char)(detail->flags >> 31) == 0) flags |= 16;
    if (kind == 0) {
        if ((unsigned char)((detail->damage_flags >> 2) & 1) != 0) flags |= 32; else flags &= ~32;
    } else if (kind == 10) {
        if ((unsigned char)((((const crate_datum *)detail)->flags >> 1) & 1) != 0) flags |= 64; else flags &= ~64;
    } else if (kind == 6) {
        if (((int)((const s_scenery_object *)detail)->flags & 8) != 0) flags |= 128; else flags &= ~128;
    }
    if (((1 << kind) & 0x1c8b) != 0) return flags | 0x200;
    return flags & ~0x200;
}


// 0x28a790
bool instanced_geometry_test_point(const s_collision_bsp_test_context *context, long instance_index,
    unsigned long flags, const real_point3d *point)
{
    bool result = false;
    const structure_bsp *structure_bsp = context->structure_bsp;
    const structure_instanced_geometry_instance *instance =
        &((const structure_instanced_geometry_instance *)structure_bsp->instanced_geometry_instances.address)[instance_index];
    const structure_instanced_geometry_definition *definition =
        &((const structure_instanced_geometry_definition *)context->resources->instanced_geometry_definitions.address)[instance->definition_index];
    if (!(instance->flags & 2) &&
        (!(bool)((flags >> 10) & 1) || !(instance->flags & 4)) &&
        collisions_test_instanced_geometry(context, instance_index, instance, definition, flags) &&
        point_in_sphere(point, &instance->world_bounding_sphere_center, instance->world_bounding_sphere_radius)) {
        real_point3d local_point;
        matrix4x3_inverse_transform_point(&instance->transform, point, &local_point);
        if (bsp3d_test_point((const bsp3d *)&definition->bsp, 0, &local_point) == -1)
            result = true;
    }
    return result;
}

// 0x28a8f0


static __forceinline s_zxy_vector3d *collisions_distance_vector_from_points(const real_point3d *p0,
    const real_point3d *p1, s_zxy_vector3d *v)
{
    v->x = p1->x - p0->x;
    v->y = p1->y - p0->y;
    v->z = p1->z - p0->z;
    return v;
}

static __forceinline float collisions_distance_vector_magnitude_squared(const s_zxy_vector3d *v)
{
    return v->z * v->z + (v->x * v->x + v->y * v->y);
}

extern "C" bool object_test_point_internal(long object_index, bool pretested, s_collision_test_flags flags, const real_point3d *point,
    long first_ignore_object_index, long second_ignore_object_index)
{
    long index = object_index;
    bool result = false;
    bool already_pretested = pretested;
    s_object_header *header;
    object_datum *object;
    collision_model_instance instance;
    s_zxy_vector3d vector;
    float radius;
    bool within_attached_bounds;
    bool child_contains_point;
    long child_object_index;
    do {
        header = object_header_get(index);
        object = (object_datum *)header->datum;
        if (!already_pretested) {
            if (!collision_test_object(index, header, object, flags, first_ignore_object_index, second_ignore_object_index, -1))
                continue;
            collisions_distance_vector_from_points(point, &object->bounding_sphere_center, &vector);
            radius = object->bounding_sphere_radius;
            within_attached_bounds = collisions_distance_vector_magnitude_squared(&vector) <= radius * radius;
            if (!within_attached_bounds)
                continue;
        }
        if (::collision_model_instance_new(&instance, index)) {
            if (collision_model_test_point(&instance, point)) {
                result = true;
                break;
            }
        }
        if ((bool)(_rotl(flags.collision_flags, 27) & 1))
            continue;
        child_object_index = object->first_child_object_index;
        if (child_object_index == -1)
            continue;
        child_contains_point = false;
        if (!::object_is_hidden(child_object_index)
            && object_test_point_internal(child_object_index, false, flags, point, first_ignore_object_index, second_ignore_object_index))
            child_contains_point = true;
        if (child_contains_point) {
            result = true;
            break;
        }
    } while ((index = object->next_object_index) != -1);
    return result;
}

// 0x28aea8
int collisions_compare_floats(const float *a, const float *b)
{
    int result = 0;
    if (*a > *b)
        result = 1;
    else if (*a < *b)
        result = -1;
    return result;
}


// 0x28aed8
bool collision_test_cluster(const s_collision_bsp_test_context *context, s_cluster_reference cluster_reference,
    s_collision_test_flags flags, unsigned long bsp_flags, const real_point3d *point, const real_vector3d *vector,
    long ignore_object_index, long ignore_object_index2, long ignore_index, collision_result *result)
{
    s_collision_test_flags test_flags = flags;
    bool hit = false;
    // match: 0x28aed8: without this identity rotation the bucket address add becomes add r29,r11,r19 instead of add r29,r19,r11
    if (collision_flag_test(_rotl(test_flags.collision_flags, 0), 3))
    {
        if (context->structure_bsp->import_version >= 3)
        {
            if (collision_test_cluster_instanced_geometry(context, cluster_reference, flags.collision_flags, bsp_flags, point,
                vector, result))
                return true;
        }
        else
        {
            const structure_cluster *cluster =
                &((const struct structure_cluster *)context->structure_bsp->clusters.address)[cluster_reference.cluster_index];
            const s_mesh *cluster_data =
                &((const s_mesh *)context->structure_bsp->render_geometry.meshes.address)[cluster->mesh_index];
            for (long list_index = 0; list_index < cluster_data->instance_buckets.count; list_index++)
            {
                const s_mesh_instance_bucket *lists = (const s_mesh_instance_bucket *)cluster_data->instance_buckets.address;
                const s_mesh_instance_bucket *list = &lists[list_index];
                // match: 0x28aed8: with the rotation above alone, the instance load becomes lhzx r31,r11,r30; this second pair restores lhzx r31,r30,r11
                for (long instance_index = 0; instance_index < (long)_rotl((long)_rotl(list->instances.count(), 0), 0); instance_index++)
                {
                    unsigned short instance = ((const unsigned short *)list->instances.address)[instance_index];
                    if (::structure_instanced_geometry_mark(cluster_reference.bsp_index, instance))
                    {
                        if (instanced_geometry_test_vector_internal(context, instance, flags.collision_flags, bsp_flags, point,
                            vector, result))
                            hit = true;
                        if (hit && collision_flag_test(flags.collision_flags, 16))
                            break;
                    }
                }
                if (hit && collision_flag_test(flags.collision_flags, 16))
                    break;
            }
        }
    }
    return hit;
}

// 0x28cd58
long collision_structure_ground_plane_process_surfaces(
    const collision_bsp_test_sphere_result *sphere_result,
    const real_matrix4x3 *matrix,
    const collision_bsp *bsp,
    const real_point3d *point,
    const real_vector3d *direction,
    long maximum_plane_count,
    long plane_count,
    real_plane3d *planes)
{
    for (long index = 0; index < sphere_result->surface_count; index++) {
        long surface_index = sphere_result->surface_indices[index];
        const collision_surface *surface = &((const collision_surface *)bsp->surfaces.address)[surface_index];
        real_plane3d plane;
        collisions_bsp3d_get_plane_from_designator(bsp, surface->plane, &plane);
        if (matrix)
            transform_surface_plane(matrix, &plane, &plane);
        float distance = plane3d_distance_to_point(&plane, point);
        if (distance > 0.0f && dot_product3d(direction, &plane.normal) < 0.0f && plane_count < 256) {
            planes[plane_count] = plane;
            plane_count++;
        }
    }
    return plane_count;
}


// @address 0xf03b24 k_projectiles_collision_test_flags

const float k_real_max = 3.4028234663852886e+38f;


// match: 0x28cf50: aggregate-return subtraction preserves the accepted register assignment.
static __forceinline __regfix_ret_collisions_subtract_vectors3d collisions_subtract_vectors3d_rfret(
    const real_vector3d *const &a, const real_vector3d *b, real_vector3d *result)
{
    result->i = a->i - b->i;
    result->j = a->j - b->j;
    result->k = a->k - b->k;
    { __regfix_ret_collisions_subtract_vectors3d rf_ret; rf_ret.v = (result); return rf_ret; }
}




static __forceinline float normalize_ground_plane_vector(real_vector3d *vector)
{
    float length = sqrtf(vector->i * vector->i + vector->j * vector->j + vector->k * vector->k);
    if (!(fabsf(length) < k_real_epsilon)) {
        scale_vector3d(vector, 1.0f / length, vector);
        return length;
    }
    return 0.0f;
}

static __forceinline void normalize_ground_plane_vector_with_default(real_vector3d *vector, const real_vector3d *default_vector)
{
    float length = sqrtf(vector->i * vector->i + vector->j * vector->j + vector->k * vector->k);
    if (!(fabsf(length) < k_real_epsilon))
        scale_vector3d(vector, 1.0f / length, vector);
    else
        *vector = *default_vector;
}


// 0x28cf50
// @address 0x28cf50 ?collision_structure_ground_plane_calculate@@YAPBTreal_vector3d@@PBTreal_point3d@@PBT1@PAT1@JPBUreal_plane3d@@@Z
const real_vector3d *collision_structure_ground_plane_calculate(const real_point3d *point, const real_vector3d *vector,
    real_vector3d *normal, long plane_count, const real_plane3d *planes)
{
    const real_vector3d *result = 0;
    c_static_stack<long, 256> contacts;
    real_vector3d movement = *vector;
    for (long iteration = 0; iteration < 3; iteration++) {
        float best_distance = k_real_max;
        long best_index = -1;
        for (long index = 0; index < plane_count; index++) {
            float distance = dot_product3d(&movement, &planes[index].normal);
            if (distance < -0.025f && distance < best_distance) {
                best_distance = distance;
                best_index = index;
            }
        }
        if (best_index == -1)
            break;
        contacts.push_back(best_index);
        switch (contacts.count()) {
        case 1: {
            const real_vector3d *plane_normal = &planes[contacts[0]].normal;
            real_vector3d offset;
            scale_vector3d(plane_normal, best_distance, &offset);
            collisions_subtract_vectors3d_rfret(vector, &offset, &movement).v;
            break;
        }
        case 2: {
            const real_vector3d *normal0 = &planes[contacts[0]].normal;
            const real_vector3d *normal1 = &planes[contacts[1]].normal;
            real_vector3d clip_line_vector;
            cross_product3d(normal0, normal1, &clip_line_vector);
            normalize_ground_plane_vector(&clip_line_vector);
            float clip_distance = dot_product3d(vector, &clip_line_vector);
            scale_vector3d(&clip_line_vector, clip_distance, &movement);
            break;
        }
        }
    }
    if (contacts.count() != 0) {
        long nearest = 0;
        float nearest_distance = k_real_max;
        for (long index = 0; index < contacts.count(); index++) {
            float distance = dot_product3d(vector, &planes[contacts[index]].normal);
            if (distance < nearest_distance) {
                nearest_distance = distance;
                nearest = index;
            }
        }
        if (contacts.count() != 1) {
            const real_vector3d *normal0 = &planes[contacts[0]].normal;
            const real_vector3d *normal1 = &planes[contacts[1]].normal;
            real_vector3d a, b;
            scale_vector3d(normal0, -dot_product3d(normal0, vector), &a);
            scale_vector3d(normal1, -dot_product3d(normal1, vector), &b);
            add_vectors3d(&a, &b, normal);
            normalize_ground_plane_vector_with_default(normal, &planes[contacts[nearest]].normal);
            if (contacts.count() == 3) {
                const real_vector3d *normal2 = &planes[contacts[2]].normal;
                real_vector3d c;
                scale_vector3d(normal2, -dot_product3d(normal2, vector), &c);
                add_vectors3d(normal, &c, normal);
                normalize_ground_plane_vector_with_default(normal, &planes[contacts[nearest]].normal);
            }
        } else {
            *normal = planes[contacts[0]].normal;
        }
        if (normalize_ground_plane_vector(normal) != 0.0f)
            result = normal;
    }
    return result;
}


// 0x28c500
bool collision_find_good_projectile_point(const real_point3d *known_point, const real_point3d *desired_point,
    real_point3d *good_point, long ignore_object_index, long unused)
{
    collision_result collision;
    bool success = false;
    s_collision_test_flags test_flags = k_projectiles_collision_test_flags;
    test_flags.collision_flags |= 512;
    if (collision_test_line(test_flags, known_point, desired_point, ignore_object_index, -1, &collision)) {
        if ((unsigned char)collision.location.cluster_reference.bsp_index != 255) {
            real_vector3d direction;
            vector_from_points3d(&collision.point, known_point, &direction);
            normalize3d(&direction);
            float distance = k_real_hundredth_storage;
            point_from_line3d(&collision.point, &direction, distance, good_point);
            success = true;
        }
    } else {
        success = true;
        *good_point = *desired_point;
    }
    return success;
}


static __forceinline structure_instanced_geometry_definition *collisions_cached_definitions_get(long structure_bsp_index)
{
    return (structure_instanced_geometry_definition *)g_collision_bsp_cache.users[get_current_thread_index()].entries[structure_bsp_index].data;
}

static __forceinline const collision_bsp *collisions_global_bsp_get(long structure_bsp_index)
{
#ifdef HALO3_64BIT
    return g_collision_bsp_cache.thread_users[get_current_thread_index()].collision_bsps[structure_bsp_index];
#else
    return ((const collision_bsp **)(&g_collision_bsp_cache))[get_current_thread_index() * 64 + structure_bsp_index];
#endif
}

static __forceinline const collision_bsp *collisions_model_bsp_get(const collision_model_instance *instance, unsigned long reference)
{
    const s_collision_model_region *region =
        &((const s_collision_model_region *)instance->collision_model->regions.address)[(reference >> 24) & 255];
    const s_collision_model_permutation *permutation =
        &((const s_collision_model_permutation *)region->permutations.address)[(reference >> 16) & 255];
    const s_collision_model_bsp *bsp =
        &((const s_collision_model_bsp *)permutation->bsps.address)[(reference >> 8) & 255];
    return &bsp->bsp;
}

// 0xf3db0
inline bool get_bsp_from_collision_result(
    // match: 0xf3db0: without __restrict on bsp and matrix the case-1 *bsp store sinks below the instance matrix loads and the null-store branch collapses
    struct collision_result *collision, struct collision_bsp const **__restrict bsp, struct real_matrix4x3 const **__restrict matrix)
{
    switch (collision->type) {
    case 1:
        *bsp = collisions_global_bsp_get(collision->structure_bsp_index);
        *matrix = 0;
        break;
    case 3: {
        const structure_bsp *structure = global_structure_bsp_get(collision->structure_bsp_index);
        const structure_instanced_geometry_instance *instance =
            &((const structure_instanced_geometry_instance *)structure->instanced_geometry_instances.address)[collision->instance_index];
        const structure_instanced_geometry_definition *definition =
            &collisions_cached_definitions_get(collision->structure_bsp_index)[instance->definition_index];
        *bsp = &definition->bsp;
        *matrix = &instance->transform;
        break;
    }
    case 4: {
        collision_model_instance instance;
        if (::collision_model_instance_new(&instance, collision->object_index)) {
            *bsp = collisions_model_bsp_get(&instance, collision->bsp_reference);
            *matrix = &instance.node_matrices[collision->node_index];
        } else {
            *bsp = 0;
            *matrix = 0;
        }
        break;
    }
    default:
        *bsp = 0;
        *matrix = 0;
        break;
    }
    return *bsp != 0;
}



static __forceinline const s_collision_material *collisions_structure_material_get(long structure_bsp_index, short material_index)
{
    return TAG_BLOCK_GET_ELEMENT(&global_structure_bsp_get(structure_bsp_index)->collision_materials, material_index,
        const s_collision_material);
}

// @address 0x1067c0 collision_get_surface_material
extern "C" c_global_material_type collision_get_surface_material(long structure_bsp_index,
    const collision_bsp *structure_bsp_tag, const real_point3d *position, long surface_index,
    long instance_index)
{
    c_global_material_type material_type = k_global_material_type_invalid;
    const collision_surface *surface = &((const collision_surface *)structure_bsp_tag->surfaces.address)[surface_index];
    if ((unsigned short)surface->material != 0xffff) {
        const s_collision_material *material = collisions_structure_material_get(structure_bsp_index,
            (unsigned short)surface->material);
        material_type = collisions_structure_material_get(structure_bsp_index, (unsigned short)surface->material)->global_material;
        if (material->render_method.index != -1 && tag_get_group_tag(material->render_method.index) == 'rmtr') {
            const s_rmtr_definition *definition =
                (const s_rmtr_definition *)tag_get('rmtr', material->render_method.index);
            if (definition->single_material) {
                material_type.m_index = definition->materials[0];
                return material_type;
            }
            bool on_surface;
            s_geometry_test_result test_result;
            memset(&test_result, 0, sizeof(test_result));
            if (c_geometry_sampler::geometry_test_collision_result(structure_bsp_index, -1, instance_index,
                    surface_index, position, true, false, on_surface, test_result)) {
                real_point2d texcoord;
                texcoord.x = test_result.texcoord.x;
                texcoord.y = test_result.texcoord.y;
                real_argb_color color;
                float best_weight = 0.0f;
                short best_material = k_global_material_type_invalid.m_index;
                if (((const s_render_method_postprocess_definition *)definition->shader.postprocess.address)->sample_texture(
                    s_render_method_postprocess_definition::_runtime_queryable_property_diffuse_weights, texcoord, 1, &color)) {
                    short invalid_material = k_global_material_type_invalid.m_index;
                    short default_material = k_global_material_type_default.m_index;
                    if (color.red > best_weight) {
                        short material_index = definition->materials[0];
                        if (material_index != invalid_material && material_index != default_material) {
                            best_weight = color.red;
                            best_material = material_index;
                        }
                    }
                    if (color.green > best_weight) {
                        short material_index = definition->materials[1];
                        if (material_index != invalid_material && material_index != default_material) {
                            best_weight = color.green;
                            best_material = material_index;
                        }
                    }
                    if (color.blue > best_weight) {
                        short material_index = definition->materials[2];
                        if (material_index != invalid_material && material_index != default_material) {
                            best_weight = color.blue;
                            best_material = material_index;
                        }
                    }
                    if (color.alpha > best_weight) {
                        short material_index = definition->materials[3];
                        if (material_index != invalid_material && material_index != default_material) {
                            best_weight = color.alpha;
                            best_material = material_index;
                        }
                    }
                    material_type.m_index = best_material;
                }
            }
        }
    }
    return material_type;
}


// @address 0x1066c8 collision_get_bsp_surface_material
extern "C" c_global_material_type collision_get_bsp_surface_material(long structure_bsp_index, const real_point3d *position,
    long surface_index, long instance_index)
{
    const collision_bsp *bsp;
    if (instance_index != -1) {
        const structure_bsp *structure = global_structure_bsp_get(structure_bsp_index);
        const structure_instanced_geometry_instance *instance =
            &((const structure_instanced_geometry_instance *)structure->instanced_geometry_instances.address)[instance_index];
        bsp = &collisions_cached_definitions_get(structure_bsp_index)[instance->definition_index].bsp;
    } else {
        bsp = collisions_global_bsp_get(structure_bsp_index);
    }
    return collision_get_surface_material(structure_bsp_index, bsp, position, surface_index,
        instance_index);
}


static __forceinline bool collisions_features_point_in_sphere(const real_point3d *point, const real_point3d *center, float radius)
{
    real_vector3d vector;
    vector_from_points3d(point, center, &vector);
    return vector.j * vector.j + vector.k * vector.k + vector.i * vector.i <= radius * radius;
}

// @address 0x28b060 ?instanced_geometry_get_features_in_sphere@@YAXPBUs_collision_bsp_test_context@@JKPBTreal_point3d@@MMMPAUcollision_feature_list@@@Z
void instanced_geometry_get_features_in_sphere(const s_collision_bsp_test_context *context, long instance_index,
    unsigned long flags, const real_point3d *point, float radius, float feature_radius, float height,
    collision_feature_list *features)
{
    const long *structure_bsp_index = &context->structure_bsp_index;
    const structure_instanced_geometry_instance *instance =
        &((const structure_instanced_geometry_instance *)context->structure_bsp->instanced_geometry_instances.address)[instance_index];
    const structure_instanced_geometry_definition *definition =
        &((const structure_instanced_geometry_definition *)context->resources->instanced_geometry_definitions.address)[instance->definition_index];
    if (collisions_test_instanced_geometry(context, instance_index, instance, definition, flags)) {
        if (collisions_features_point_in_sphere(point, &instance->world_bounding_sphere_center, radius
            + instance->world_bounding_sphere_radius)) {
            real_matrix4x3 inverse;
            real_point3d local_point;
            collision_bsp_test_sphere_result sphere_result;
            matrix4x3_inverse(&instance->transform, &inverse);
            matrix4x3_transform_point(&inverse, point, &local_point);
            const s_breakable_surface_set *sets =
                ::breakable_surface_get_first_set(*structure_bsp_index, instance_index);
            long set_count = breakable_surface_get_surface_set_count(*structure_bsp_index, instance_index);
            if (collision_bsp_test_sphere(&definition->bsp, c_collision_bsp_test_sphere_flags(6),
                set_count, sets, &local_point, inverse.scale * radius, &sphere_result)) {
                ::collision_bsp_get_features_in_sphere(&definition->bsp,
                    &sphere_result, &instance->transform,
                    feature_radius, height, -1, -1, instance_index, *structure_bsp_index, features);
            }
        }
    }
}


// @address 0x28b218 ?object_get_features_in_sphere@@YAXJUs_collision_test_flags@@PBTreal_point3d@@MMMJJPAUcollision_feature_list@@@Z
// match: 0x28b218: component store order preserves the matched load schedule.
static __forceinline real_vector3d *collisions_object_features_vector_from_points3d(const real_point3d *p0, const real_point3d *p1, real_vector3d *v)
{
    v->i = p1->x - p0->x;
    v->k = p1->z - p0->z;
    v->j = p1->y - p0->y;
    return v;
}

void object_get_features_in_sphere(long object_index, s_collision_test_flags flags, const real_point3d *point, float radius,
    float feature_radius, float height, long first_ignore_object_index, long second_ignore_object_index, collision_feature_list *features)
{
    do {
        const s_object_header *header = object_header_get(object_index);
        const object_datum *object = (const object_datum *)header->datum;
        if (collision_test_object(object_index, header, object, flags, first_ignore_object_index, second_ignore_object_index, -1)) {
            real_vector3d vec;
            collisions_object_features_vector_from_points3d(point, &object->bounding_sphere_center, &vec);
            float combined_radius = object->bounding_sphere_radius + radius;
            bool in_sphere = (magnitude_squared3d(&vec) <= combined_radius * combined_radius);
            if (in_sphere) {
                switch (object->object_type) {
                case 0:
                case 1:
                case 4:
                case 6:
                case 7:
                case 8:
                case _object_type_crate:
                case _object_type_giant: {
                    collision_model_instance instance;
                    if (::collision_model_instance_new(&instance, object_index))
                        collision_model_get_features_in_sphere(&instance, point, radius, feature_radius, height, features);
                    break;
                }
                case 2:
                case 3:
                case 5:
                case 9:
                case _object_type_creature:
                default:
                    break;
                }
                if (!collision_flag_test(flags.collision_flags, 5)) {
                    long child_object_index = object->first_child_object_index;
                    if (child_object_index != -1)
                        object_get_features_in_sphere(child_object_index, flags, point, radius, feature_radius, height,
                            first_ignore_object_index, second_ignore_object_index, features);
                }
            }
        }
        object_index = object->next_object_index;
    } while (object_index != -1);
}



static __forceinline real_vector3d *collisions_vector_from_line3d(const real_vector3d *point, const real_vector3d *vector, float t,
    real_vector3d *result)
{
    result->i = point->i + vector->i * t;
    result->j = point->j + vector->j * t;
    result->k = point->k + vector->k * t;
    return result;
}

static __forceinline real_vector3d *collisions_cross_product3d_safe(const real_vector3d *a, const real_vector3d *b, real_vector3d *result)
{
    float i = a->j * b->k - a->k * b->j;
    float k = a->i * b->j - a->j * b->i;
    float j = a->k * b->i - a->i * b->k;
    result->i = i;
    result->j = j;
    result->k = k;
    return result;
}

static __forceinline real_vector3d *collisions_move_cross_product3d(const real_vector3d *a, const real_vector3d *b, real_vector3d *result)
{
    float k = a->i * b->j - a->j * b->i;
    float j = a->k * b->i - a->i * b->k;
    result->i = a->j * b->k - a->k * b->j;
    result->j = j;
    result->k = k;
    return result;
}

static __forceinline void collisions_project_vector_onto_plane(const real_vector3d *vector, const real_plane3d *plane,
    real_vector3d *result)
{
    collisions_vector_from_line3d(vector, &plane->normal, -dot_product3d(vector, &plane->normal), result);
}

static __forceinline void collisions_project_point_onto_plane(const real_point3d *point, const real_plane3d *plane, real_point3d *result)
{
    point_from_line3d(point, &plane->normal, -(dot_product3d(point, &plane->normal) - plane->distance), result);
}

static __forceinline void collisions_project_point_onto_line(
    const real_point3d *point, const real_point3d *line_point, const real_vector3d *line_vector, real_point3d *result)
{
    real_vector3d offset;
    vector_from_points3d(line_point, point, &offset);
    point_from_line3d(
        line_point, line_vector, dot_product3d(&offset, line_vector) / magnitude_squared3d(line_vector), result);
}

static __forceinline void collisions_project_vector_onto_line(
    const real_vector3d *vector, const real_point3d *line_point, const real_vector3d *line_vector, real_vector3d *result)
{
    scale_vector3d(line_vector, dot_product3d(vector, line_vector) / magnitude_squared3d(line_vector), result);
}


// @address 0x28ba40 ?collision_move_point@@YAFPBTreal_point3d@@PBTreal_vector3d@@PBUcollision_feature_list@@PAT1@PAT2@FPAUcollision_plane@@@Z
short collision_move_point(const real_point3d *old_position, const real_vector3d *old_velocity, const collision_feature_list *features,
    real_point3d *new_position, real_vector3d *new_velocity, short maximum_collision_count, collision_plane *collisions)
{
    short collision_count = 0;
    real_point3d position = *old_position;
    real_vector3d velocity = *old_velocity;
    real_point3d clipped_position = position;
    real_vector3d clipped_velocity = velocity;
    short clip_count = 0;
    short clips[3];
    real_plane3d clip_plane;
    real_point3d clip_line_point;
    real_vector3d clip_line_vector;
    real_point3d clip_point;

    while (!(fabsf(clipped_velocity.i - 0.0f) < k_real_epsilon && fabsf(clipped_velocity.j - 0.0f) < k_real_epsilon &&
        fabsf(clipped_velocity.k - 0.0f) < k_real_epsilon))
    {
        collision_plane *collision = &collisions[collision_count];
        if (collision_features_test_vector(features, &clipped_position, &clipped_velocity, collision)) {
            collision_count++;
            position = collision->point;
            scale_vector3d(&velocity, 1.0f - collision->t, &velocity);

            short new_clip_count = 1;
            short new_clips[3];
            new_clips[0] = collision_count - 1;
            clip_plane = collisions[new_clips[0]].plane;
            collisions_project_vector_onto_plane(&velocity, &clip_plane, &clipped_velocity);
            collisions_project_point_onto_plane(&position, &clip_plane, &clipped_position);
            if (clip_count > 0) {
                if (dot_product3d(&clipped_velocity, &collisions[clips[0]].plane.normal) < rasterizer_states_mode_7_depth_bias &&
                    planes_try_and_get_intersection_line(&collisions[new_clips[0]].plane, &collisions[clips[0]].plane,
                        &clip_line_point, &clip_line_vector)) {
                    new_clip_count = 2;
                    new_clips[1] = clips[0];
                    collisions_project_vector_onto_line(&velocity, &clip_line_point, &clip_line_vector, &clipped_velocity);
                    collisions_project_point_onto_line(&position, &clip_line_point, &clip_line_vector, &clipped_position);
                    if (clip_count > 1 &&
                        dot_product3d(&clipped_velocity, &collisions[clips[1]].plane.normal) < rasterizer_states_mode_7_depth_bias &&
                        planes_try_and_get_intersection_point(&collisions[new_clips[0]].plane, &collisions[new_clips[1]].plane,
                            &collisions[clips[1]].plane, &clip_point))
                        {
                        new_clip_count = 3;
                        new_clips[2] = clips[1];
                        set_real_vector3d(&clipped_velocity, 0.0f, 0.0f, 0.0f);
                        clipped_position = clip_point;
                    }
                } else if (clip_count > 1 &&
                    dot_product3d(&clipped_velocity, &collisions[clips[1]].plane.normal) < rasterizer_states_mode_7_depth_bias &&
                    planes_try_and_get_intersection_line(&collisions[new_clips[0]].plane, &collisions[clips[1]].plane,
                        &clip_line_point, &clip_line_vector)) {
                    new_clip_count = 2;
                    new_clips[1] = clips[1];
                    collisions_project_vector_onto_line(&velocity, &clip_line_point, &clip_line_vector, &clipped_velocity);
                    collisions_project_point_onto_line(&position, &clip_line_point, &clip_line_vector, &clipped_position);
                }
            }
            clip_count = new_clip_count;
            memcpy(clips, new_clips, new_clip_count * sizeof(short));
            if (collision_count >= maximum_collision_count)
                break;
        } else {
            clipped_position = collision->point;
            break;
        }
    }

    *new_position = clipped_position;
    switch (clip_count) {
    case 0:
        *new_velocity = *old_velocity;
        break;
    case 1:
        collisions_project_vector_onto_plane(old_velocity, &clip_plane, new_velocity);
        break;
    case 2:
        collisions_project_vector_onto_line(old_velocity, &clip_line_point, &clip_line_vector, new_velocity);
        break;
    case 3:
        set_real_vector3d(new_velocity, 0.0f, 0.0f, 0.0f);
        break;
    default:
        // match: 0x28ba40: without __assume(0) the clip-count switch gains a cmplwi r6,3 / bgt range check before the jump table
        __assume(0);
    }

    if (clip_count > 1 && collision_count < maximum_collision_count) {
        collision_plane *collision = &collisions[collision_count++];
        collision->t = collisions[clips[clip_count - 1]].t;
        collision->point = collisions[clips[clip_count - 1]].point;
        collision->user0 = -1;
        collision->user2 = -1;
        collision->user1 = -1;
        collision->user4 = 0;
        collision->user5 = 0;
        collision->user6 = -1;

        float lowest_k = 0.0f;
        short lowest_clip_index = -1;
        for (long clip_index = 0; clip_index < clip_count; clip_index++) {
            const collision_plane *clip = &collisions[clips[clip_index]];
            if (lowest_k > clip->plane.normal.k) {
                lowest_k = clip->plane.normal.k;
                lowest_clip_index = clip_index;
            }
        }

        if (clip_count == 2) {
            if (lowest_clip_index != -1) {
                const collision_plane *clip = &collisions[clips[lowest_clip_index]];
                if (lowest_clip_index == 0)
                    collisions_move_cross_product3d(&clip_line_vector, &clip->plane.normal, &collision->plane.normal);
                else
                    collisions_move_cross_product3d(&clip->plane.normal, &clip_line_vector, &collision->plane.normal);
            } else {
                collisions_vector_from_line3d(global_up3d, &clip_line_vector,
                    -clip_line_vector.k / magnitude_squared3d(&clip_line_vector), &collision->plane.normal);
            }
            if (normalize3d(&collision->plane.normal) != 0.0f)
                collision->plane.distance = dot_product3d(&clip_line_point, &collision->plane.normal);
            else
                collision_count--;
        } else {
            if (lowest_clip_index != -1) {
                const collision_plane *clip = &collisions[clips[lowest_clip_index]];
                collisions_vector_from_line3d(global_up3d, &clip->plane.normal, -clip->plane.normal.k, &collision->plane.normal);
                if (normalize3d(&collision->plane.normal) != 0.0f)
                    collision->plane.distance = dot_product3d(&clip_point, &collision->plane.normal);
                else
                    collision_count--;
            } else {
                collision->plane.normal = *global_up3d;
                collision->plane.distance = dot_product3d(&clip_point, &collision->plane.normal);
            }
        }
    }
    return collision_count;
}

// @address 0x28c3c0 collision_move_point_with_features
#if !defined(_XBOX) || defined(__clang__)
extern "C" short collision_move_point_with_features(s_collision_test_flags flags, const real_point3d *point, const real_vector3d *delta,
    float radius, float margin, long first_ignore, long second_ignore, real_point3d *out_point, real_vector3d *out_delta,
    short maximum_planes, collision_plane *planes)
#else
extern "C" short collision_move_point_with_features(unsigned __int64 packed_flags, const real_point3d *point, const real_vector3d *delta,
    float radius, float margin, long first_ignore, long second_ignore, real_point3d *out_point, real_vector3d *out_delta,
    unsigned short maximum_planes, collision_plane *planes)
#endif
{
    real_point3d center;
    collision_feature_list features;
    short result = 0;

    center.x = delta->i * 0.5f + point->x;
    center.y = delta->j * 0.5f + point->y;
    center.z = delta->k * 0.5f + point->z;

#if !defined(_XBOX) || defined(__clang__)
    unsigned __int64 packed_flags;
    memcpy(&packed_flags, &flags, sizeof(packed_flags));
#endif
    if (collision_get_features_in_sphere(packed_flags, &center,
            sqrtf(delta->i * delta->i + (delta->k * delta->k + delta->j * delta->j)) * 0.5f + margin, 0.0f, margin, -1, -1, &features))
        result = collision_move_point(
            point, delta, &features, out_point, out_delta, maximum_planes, planes);
    else {
        out_point->x = point->x + delta->i;
        out_point->y = point->y + delta->j;
        out_point->z = point->z + delta->k;
        *out_delta = *delta;
    }
    return result;
}
// 0x9d600
// @address 0x112cc8 ?real_rectangle3d_enclose_point@@YAPATreal_rectangle3d@@PAT1@PBTreal_point3d@@@Z
// match: 0x9d600: inline lets collision_test_vector inline the body; out of line it keeps bounds in r30/r31
#if defined(_XBOX) && !defined(__clang__)
inline real_rectangle3d *real_rectangle3d_enclose_point(real_rectangle3d *rectangle, const real_point3d *point)
#else
real_rectangle3d *real_rectangle3d_enclose_point(real_rectangle3d *rectangle, const real_point3d *point)
#endif
{
    if (rectangle->x0 > point->x)
        rectangle->x0 = point->x;
    if (rectangle->x1 < point->x)
        rectangle->x1 = point->x;
    if (rectangle->y0 > point->y)
        rectangle->y0 = point->y;
    if (rectangle->y1 < point->y)
        rectangle->y1 = point->y;
    if (rectangle->z0 > point->z)
        rectangle->z0 = point->z;
    if (rectangle->z1 < point->z)
        rectangle->z1 = point->z;
    return rectangle;
}


static __forceinline s_structure_globals *collisions_structure_globals_get()
{
    long current_thread = get_current_thread_index();
    s_structure_globals *globals = thread_structure_globals + current_thread;
    return globals - 1;
}

static __forceinline void collisions_cluster_marker_begin()
{
    s_structure_globals *globals = collisions_structure_globals_get();
    globals->cluster_marker_sequence++;
    globals->cluster_marker_initialized = true;
}

static __forceinline void collisions_cluster_marker_end()
{
    collisions_structure_globals_get()->cluster_marker_initialized = false;
}

static __forceinline bool collisions_cluster_mark(s_cluster_reference reference)
{
    s_structure_globals *globals = collisions_structure_globals_get();
    long sequence = globals->cluster_marker_sequence;
    long previous = globals->cluster_markers[reference.bsp_index][reference.cluster_index];
    globals->cluster_markers[reference.bsp_index][reference.cluster_index] = sequence;
    return previous != sequence;
}

static __forceinline void collisions_instanced_geometry_marker_begin()
{
    long current_thread = get_current_thread_index();
    s_structure_globals *globals = &thread_structure_globals[current_thread - 1];
    globals->instanced_geometry_marker_sequence++;
    globals->instanced_geometry_marker_initialized = true;
    globals->marker.clear();
}

static __forceinline void collisions_instanced_geometry_marker_end()
{
    long current_thread = get_current_thread_index();
    s_structure_globals *globals = &thread_structure_globals[current_thread - 1];
    globals->instanced_geometry_marker_initialized = false;
}

static __forceinline const s_breakable_surface_set *breakable_surface_get_default_set(long structure_bsp_index)
{
    const s_breakable_surface_set *result = 0;
    const collisions_breakable_surface_globals *globals = (const collisions_breakable_surface_globals *)g_breakable_surface_globals;
    if ((globals->slot_mask & (1 << structure_bsp_index)) != 0
        && global_structure_bsp_get(structure_bsp_index)->breakable_surfaces.count > 0) {
        short entry_index = globals->default_entry_indices[structure_bsp_index];
        if (entry_index != -1)
            result = &globals->entries[entry_index];
    }
    return result;
}


static __forceinline s_cluster_reference *collisions_cluster_reference_set_none(s_cluster_reference *cluster_reference)
{
    *(short *)cluster_reference = -1;
    return cluster_reference;
}

static __forceinline s_cluster_reference structure_bsp_leaf_cluster_reference(long structure_bsp_index, long leaf_index)
{
    s_cluster_reference cluster_reference;
    if (leaf_index != -1) {
        const unsigned char *cluster_index = (const unsigned char *)tag_block_get_element_with_size(
            &global_structure_bsp_get(structure_bsp_index)->leaves, leaf_index, sizeof(unsigned char));
        cluster_reference_set(&cluster_reference, structure_bsp_index, *cluster_index);
    } else {
        collisions_cluster_reference_set_none(&cluster_reference);
    }
    return cluster_reference;
}

static __forceinline void collisions_location_from_leaf(s_location *location, long structure_bsp_index, long leaf_index)
{
    location->leaf_index = (short)leaf_index;
    s_cluster_reference reference = structure_bsp_leaf_cluster_reference(structure_bsp_index, leaf_index);
    location->cluster_reference = reference;
}

static __forceinline void collisions_structure_bsp_bounds_get(long structure_bsp_index, real_rectangle3d *bounds,
    const real_rectangle3d &empty_bounds)
{
    const s_structure_physics *structure_bounds = &global_structure_bsp_get(structure_bsp_index)->structure_physics;
    *bounds = empty_bounds;
    real_rectangle3d_enclose_point(bounds, &structure_bounds->mopp_bounds_min);
    real_rectangle3d_enclose_point(bounds, &structure_bounds->mopp_bounds_max);
}


static __forceinline void collisions_initialize_vector_result(collision_result *collision)
{
    collision->type = 0;
    collision->fraction = 1.0f;
    collision->bsp = 0;
    location_invalidate(&collision->start_location);
    location_invalidate(&collision->location);
    collision->geometry_index = -1;
    collision->instance_index = -1;
    collision->object_index = -1;
    collision->bsp_reference = -1;
    collision->surface_index = -1;
    collision->leaf_index = -1;
    collision->structure_bsp_index = -1;
    collision->breakable_surface = 255;
}

// @address 0x28ac38 collision_test_instanced_geometry_instance
extern "C" bool collision_test_instanced_geometry_instance(long structure_bsp_index, long instance_index, unsigned long flags,
    const real_point3d *point, const real_vector3d *vector, collision_result *result)
{
    bool success = false;
    collisions_initialize_vector_result(result);
    s_collision_bsp_test_context context;
    collision_bsp_test_context_initialize(structure_bsp_index, &context);
    const unsigned long &bsp_flags = build_bsp_flags_from_collision_flags(flags);
#if defined(__clang__)
    if (instanced_geometry_test_vector_internal(&context, instance_index, flags, bsp_flags, point, vector, result))
        success = true;
#else
    // match: 0x28ac38: without this copy the point argument move (mr r7,r30) is scheduled last, after mr r9,r31
    const unsigned long &query_point = _rotl((unsigned long)point, 0);
    // match: 0x28ac38: without this copy the flags argument move (mr r5,r28) is scheduled last, after mr r9,r31
    const unsigned long &query_flags = _rotl(flags, 0);
    // match: 0x28ac38: without this copy the vector argument move (mr r8,r29) is scheduled last, after mr r9,r31
    const unsigned long &query_vector = _rotl((unsigned long)vector, 0);
    // match: 0x28ac38: without this copy the instance argument move (mr r4,r27) is scheduled last, after mr r9,r31
    const unsigned long &query_instance_index = _rotl((unsigned long)instance_index, 0);
    if (instanced_geometry_test_vector_internal(&context, (long)query_instance_index, query_flags, bsp_flags,
        (const real_point3d *)query_point, (const real_vector3d *)query_vector, result))
        success = true;
#endif
    point_from_line3d(point, vector, result->fraction, &result->point);
    scenario_location_from_point(&result->location, &result->point);
    return success;
}


// @address 0x122d90 ?collision_bsp_test_context_initialize@@YAXJPAUs_collision_bsp_test_context@@@Z
inline void collision_bsp_test_context_initialize(long structure_bsp_index, s_collision_bsp_test_context *context)
{
    context->structure_bsp_index = structure_bsp_index;
    const scenario_structure_bsp_reference *reference =
        &((const scenario_structure_bsp_reference *)global_scenario->structure_bsps.address)[structure_bsp_index];
    const s_structure_bsp_resource *first = &((structure_bsp *)
        tag_get('sbsp', reference->structure_bsp.index))->resource;
    context->structure_bsp = (structure_bsp *)tag_get('sbsp', reference->structure_bsp.index);
    s_structure_bsp_resources *resources;
    if (first->use_resource_items != 0) {
        resources = first->collision_resource_interface.get_resources();
    } else {
        resources = (s_structure_bsp_resources *)first->raw_resources.address;
    }
    context->resources = resources;
    const s_structure_bsp_resource *second = &context->structure_bsp->resource;
    if (second->use_resource_items != 0) {
        resources = second->collision_resource_interface.get_resources();
    } else {
        resources = (s_structure_bsp_resources *)second->raw_resources.address;
    }
    context->unknown = resources->collision_bsp;
    context->runtime = (pathfinding_data *)pathfinding_data_get((short)structure_bsp_index);
}


static __forceinline long collisions_lowest_bit_index(unsigned long mask)
{
    return 31 - _CountLeadingZeros(mask & -(long)mask);
}

static __forceinline bool collisions_ground_plane_test_instanced_geometry(const pathfinding_data *runtime,
    long instance_index, const structure_instanced_geometry_definition *definition, unsigned long flags)
{
    bool result = true;
    if (definition->bsp.surfaces.count > 0) {
        if ((bool)((flags >> 6) & 1)) {
            if (runtime) {
                if (instance_index >= 0 && instance_index < runtime->instanced_geometry_refs.count) {
                    const s_pathfinding_instanced_geometry_ref *runtime_instance =
                        &((const s_pathfinding_instanced_geometry_ref *)runtime->instanced_geometry_refs.address)[instance_index];
                    if (runtime_instance->index == 0xffff)
                        result = false;
                } else {
                    result = false;
                }
            }
        }
    } else {
        result = false;
    }
    return result;
}

static __forceinline real_point3d *transform_point_at_height(const real_matrix4x3 *matrix,
    const real_point3d *point, float point_z, real_point3d *result)
{
    float x = point->x;
    float y = point->y;
    float z = point_z;
    x *= matrix->scale;
    y *= matrix->scale;
    z *= matrix->scale;
    result->x = x * matrix->forward.i + y * matrix->left.i + z * matrix->up.i + matrix->position.x;
    result->y = x * matrix->forward.j + y * matrix->left.j + z * matrix->up.j + matrix->position.y;
    result->z = x * matrix->forward.k + y * matrix->left.k + z * matrix->up.k + matrix->position.z;
    return result;
}

static __forceinline bool collisions_ground_plane_point_in_sphere(const real_point3d *point,
    float *point_z, const real_point3d *center, float radius)
{
    real_vector3d delta;
    delta.i = center->x - point->x;
    delta.j = center->y - point->y;
    *point_z = point->z;
    delta.k = center->z - *point_z;
    return magnitude_squared3d(&delta) <= radius * radius;
}

static __forceinline bool collisions_ground_plane_cluster_mark(s_cluster_reference reference)
{
    s_structure_globals *globals = collisions_structure_globals_get();
    long *marker = &globals->cluster_markers[reference.bsp_index][reference.cluster_index];
    long sequence = globals->cluster_marker_sequence;
    long previous = *marker;
    *marker = sequence;
    return previous != sequence;
}

static __forceinline const structure_bsp *collisions_ground_plane_structure_bsp_get(long structure_bsp_index)
{
    const scenario_structure_bsp_reference *reference =
        &((const scenario_structure_bsp_reference *)global_scenario->structure_bsps.address)[structure_bsp_index];
    return (const structure_bsp *)global_tag_instances[(unsigned short)reference->structure_bsp.index].definition;
}

static __forceinline long collisions_ground_plane_surface_set_count(long structure_bsp_index)
{
    return collisions_ground_plane_structure_bsp_get(structure_bsp_index)->breakable_surfaces.count;
}

static __forceinline const collisions_breakable_surface_globals *collisions_ground_plane_breakable_globals_get()
{
    return (const collisions_breakable_surface_globals *)g_breakable_surface_globals;
}

static __forceinline const s_breakable_surface_set *collisions_ground_plane_first_set(
    long structure_bsp_index)
{
    const s_breakable_surface_set *result = 0;
    if ((collisions_ground_plane_breakable_globals_get()->slot_mask & (1 << structure_bsp_index)) != 0 &&
        collisions_ground_plane_surface_set_count(structure_bsp_index) > 0)
    {
        short entry_index = collisions_ground_plane_breakable_globals_get()->default_entry_indices[structure_bsp_index];
        if (entry_index != -1)
            result = &collisions_ground_plane_breakable_globals_get()->entries[entry_index];
    }
    return result;
}

static __forceinline s_cluster_reference collisions_ground_plane_cluster_reference_from_leaf(long structure_bsp_index, long leaf_index)
{
    s_cluster_reference cluster_reference;
    if (leaf_index != -1) {
        const unsigned char *cluster_index = (const unsigned char *)tag_block_get_element_with_size(
            &collisions_ground_plane_structure_bsp_get(structure_bsp_index)->leaves, leaf_index, sizeof(unsigned char));
        cluster_reference_set(&cluster_reference, structure_bsp_index, *cluster_index);
    } else {
        collisions_cluster_reference_set_none(&cluster_reference);
    }
    return cluster_reference;
}

// 0x28d490
// @address 0x28d490 ?collision_structure_ground_plane_calculate@@YAPBTreal_vector3d@@PBTreal_point3d@@PBT1@MPAT1@@Z
const real_vector3d *collision_structure_ground_plane_calculate(const real_point3d *point, const real_vector3d *vector,
    float radius, real_vector3d *normal)
{
    *normal = k_real_math_identity_matrix_storage.up;
    long plane_count = 0;
    long runtime_instance_index = 0;
    real_plane3d planes[256];
    collision_bsp_test_sphere_result sphere_result;
    long bsp_index = collisions_lowest_bit_index(g_active_structure_bsp_mask);
    const unsigned long active_mask = g_active_structure_bsp_mask;
    if (bsp_index != -1) {
        do {
            s_collision_bsp_test_context context;
            collision_bsp_test_context_initialize(bsp_index, &context);
            radius += 0.0625f;
            const s_breakable_surface_set *sets = collisions_ground_plane_first_set(bsp_index);
            if (collision_bsp_test_sphere((const collision_bsp *)context.unknown, c_collision_bsp_test_sphere_flags(6),
                collisions_ground_plane_surface_set_count(bsp_index),
                sets, point, radius, &sphere_result))
                plane_count = collision_structure_ground_plane_process_surfaces(&sphere_result, 0,
                    (const collision_bsp *)context.unknown, point, vector,
                    256, plane_count, planes);
            if (sphere_result.leaf_count != 0) {
                collisions_cluster_marker_begin();
                collisions_instanced_geometry_marker_begin();
                long leaf_number = 0;
                if (plane_count < 256) {
                    const long *leaf_indices = sphere_result.leaf_indices;
                    const pathfinding_data *runtime = context.runtime;
                    const s_structure_bsp_resources *resources = context.resources;
                    const structure_bsp *structure_bsp = context.structure_bsp;
                    do {
                        if (leaf_number >= sphere_result.leaf_count)
                            break;
                        s_cluster_reference cluster_reference = collisions_ground_plane_cluster_reference_from_leaf(bsp_index,
                            *leaf_indices);
                        if (collisions_ground_plane_cluster_mark(cluster_reference)) {
                            const structure_cluster *cluster =
                                &((const struct structure_cluster *)structure_bsp->clusters.address)[cluster_reference.cluster_index];
                            const s_mesh *group =
                                &((const s_mesh *)structure_bsp->render_geometry.meshes.address)[cluster->mesh_index];
                            long supernode_index = 0;
                            if (plane_count < 256) {
                                for (; plane_count < 256 && supernode_index < group->instance_buckets.count;
                                    supernode_index++) {
                                    const s_mesh_instance_bucket *supernode =
                                        &((const s_mesh_instance_bucket *)group->instance_buckets.address)[supernode_index];
                                    {
                                        for (long index = 0; plane_count < 256 && index < supernode->instances.count(); index++) {
                                            unsigned short instance_index =
                                                ((const unsigned short *)supernode->instances.address)[index];
                                            if (!structure_instanced_geometry_mark(bsp_index, instance_index))
                                                continue;
                                            const structure_instanced_geometry_instance *instance =
                                                &((const structure_instanced_geometry_instance *)
                                                        structure_bsp->instanced_geometry_instances.address)[instance_index];
                                            const structure_instanced_geometry_definition *definition =
                                                &((const structure_instanced_geometry_definition *)
                                                        resources->instanced_geometry_definitions.address)[instance->definition_index];
                                            runtime_instance_index = instance_index;
                                            if (!collisions_ground_plane_test_instanced_geometry(runtime, runtime_instance_index,
                                                definition,
                                                k_collision_test_none.collision_flags))
                                                continue;
                                            float point_z;
                                            if (!collisions_ground_plane_point_in_sphere(point, &point_z,
                                                &instance->world_bounding_sphere_center,
                                                instance->world_bounding_sphere_radius + radius))
                                                continue;
                                            real_matrix4x3 inverse;
                                            real_point3d local_point;
                                            collision_bsp_test_sphere_result instanced_sphere_result;
                                            matrix4x3_inverse(&instance->transform, &inverse);
                                            transform_point_at_height(&inverse, point, point_z, &local_point);
                                            const s_breakable_surface_set *instance_sets =
                                                ::breakable_surface_get_first_set(bsp_index, instance_index);
                                            long set_count = breakable_surface_get_surface_set_count(bsp_index, instance_index);
                                            const collision_bsp *instance_bsp = &definition->bsp;
                                            if (collision_bsp_test_sphere(instance_bsp, c_collision_bsp_test_sphere_flags(6),
                                                set_count, instance_sets, &local_point, inverse.scale * radius,
                                                &instanced_sphere_result))
                                                plane_count = collision_structure_ground_plane_process_surfaces(
                                                    &instanced_sphere_result, &instance->transform, instance_bsp, point, vector,
                                                    256, plane_count, planes);
                                        }
                                    }
                                }
                            }
                        }
                        leaf_number++;
                        leaf_indices++;
                    } while (plane_count < 256);
                }
                collisions_instanced_geometry_marker_end();
                collisions_cluster_marker_end();
            }
            bsp_index = collisions_lowest_bit_index(active_mask & ~((unsigned long)((1i64 << (bsp_index + 1)) - 1)));
        } while (bsp_index != -1);
    }
    return collision_structure_ground_plane_calculate(point, vector, normal, plane_count, planes);
}

// @address 0x7f4310 g_object_marker_globals

static __forceinline void collisions_features_new(collision_feature_list *features)
{
    memset(features, 0, sizeof(features->counts));
}

static __forceinline s_object_marker_globals *collisions_object_marker_globals_get()
{
    return &g_object_marker_globals[get_current_thread_index() - 1];
}

static __forceinline void collisions_object_marker_begin()
{
    s_object_marker_globals *markers = collisions_object_marker_globals_get();
    markers->mark++;
    markers->active = true;
}

static __forceinline void collisions_object_marker_end()
{
    collisions_object_marker_globals_get()->active = false;
}

static __forceinline bool collisions_object_mark(long object_index)
{
    s_object_marker_globals *markers = collisions_object_marker_globals_get();
    long mark = markers->mark;
    long previous = markers->marks[datum_index_to_absolute_index(object_index)];
    markers->marks[datum_index_to_absolute_index(object_index)] = mark;
    return previous != mark;
}

static __forceinline long collisions_cluster_partition_get_next_datum(const cluster_partition *partition, long *iterator)
{
    const s_data_array *array = partition->data;
    long datum_index;
    if (*iterator != -1) {
        long datum_size = array->size;
        char *data = array->data;
        const s_cluster_partition_element *element =
            (const s_cluster_partition_element *)(data + datum_index_to_absolute_index(*iterator) * datum_size);
        long next = element->next;
        if (next != -1)
            __dcbt(0, data + datum_index_to_absolute_index(next) * datum_size);
        datum_index = element->datum_index;
        *iterator = next;
    } else {
        datum_index = -1;
    }
    return datum_index;
}

static __forceinline long collisions_cluster_get_first_collideable_object(long *iterator, s_cluster_reference cluster)
{
    const cluster_partition *partition = &tls_collideable_object_cluster_partition();
    *iterator = partition->cluster_heads[cluster.bsp_index * 256 + cluster.cluster_index];
    return collisions_cluster_partition_get_next_datum(partition, iterator);
}

static __forceinline long collisions_cluster_get_next_collideable_object(long *iterator)
{
    return collisions_cluster_partition_get_next_datum(&tls_collideable_object_cluster_partition(), iterator);
}

// @address 0x28b3e8 ?collision_get_features_in_sphere@@YA_N_KPBTreal_point3d@@MMMJJPAUcollision_feature_list@@@Z
bool collision_get_features_in_sphere(unsigned __int64 test_flags, const real_point3d *point, float radius,
    float feature_radius, float height, long first_ignore_object_index, long second_ignore_object_index,
    collision_feature_list *features)
{
    s_collision_test_flags &flags = *(s_collision_test_flags *)&test_flags;
    radius += 0.0625f;
    collisions_features_new(features);
    if (collision_flag_test(flags.collision_flags, 2))
        scenario_soft_ceilings_add_collision_features(features, point, radius, feature_radius, height);

    const unsigned long active_mask = g_active_structure_bsp_mask;
    for (long bsp_index = collisions_lowest_bit_index(active_mask); bsp_index != -1;
        bsp_index = collisions_lowest_bit_index(active_mask & ~((unsigned long)((1i64 << (bsp_index + 1)) - 1)))) {
        unsigned long test_structure = collision_flag_test(flags.collision_flags, 0);
        if (test_structure || collision_flag_test(flags.collision_flags, 3) || (flags.object_flags & 1)) {
            s_collision_bsp_test_context context;
            collision_bsp_test_sphere_result sphere_result;
            collision_bsp_test_context_initialize(bsp_index, &context);
            const s_breakable_surface_set *sets = collisions_ground_plane_first_set(bsp_index);
            if (collision_bsp_test_sphere((const collision_bsp *)context.unknown, c_collision_bsp_test_sphere_flags(6),
                collisions_ground_plane_surface_set_count(bsp_index), sets, point, radius, &sphere_result) && test_structure) {
                collision_bsp_get_features_in_sphere((const collision_bsp *)context.unknown, &sphere_result, 0,
                    feature_radius, height, -1, -1, -1, bsp_index, features);
            }

            const collision_bsp_test_sphere_result *result = &sphere_result;
            if ((result->leaf_count > 0 && collision_flag_test(flags.collision_flags, 3)) ||
                (flags.object_flags & 1)) {
                collisions_cluster_marker_begin();
                if (collision_flag_test(flags.collision_flags, 3))
                    collisions_instanced_geometry_marker_begin();
                if (collision_flag_test(flags.object_flags, 0)) {
                    collisions_object_marker_begin();
                    const s_collision_test_flags *default_flags = &k_collision_test_all_objects;
                    if (!(flags.object_flags & default_flags->object_flags)) {
                        s_collision_test_flags combined_flags;
                        combined_flags.collision_flags = flags.collision_flags | default_flags->collision_flags;
                        combined_flags.object_flags = flags.object_flags | default_flags->object_flags;
                        flags = combined_flags;
                    }
                }

                for (long leaf_number = 0; leaf_number < result->leaf_count; leaf_number++) {
                    s_cluster_reference cluster_reference =
                        collisions_ground_plane_cluster_reference_from_leaf(bsp_index, result->leaf_indices[leaf_number]);
                    if (collisions_ground_plane_cluster_mark(cluster_reference)) {
                        if (collision_flag_test(flags.collision_flags, 3)) {
                            const structure_bsp *structure_bsp = context.structure_bsp;
                            const structure_cluster *cluster =
                                &((const struct structure_cluster *)structure_bsp->clusters.address)[cluster_reference.cluster_index];
                            const s_mesh *group =
                                &((const s_mesh *)structure_bsp->render_geometry.meshes.address)[cluster->mesh_index];
                            for (long supernode_index = 0; supernode_index < group->instance_buckets.count; supernode_index++) {
                                const s_mesh_instance_bucket *supernode =
                                    &((const s_mesh_instance_bucket *)group->instance_buckets.address)[supernode_index];
                                for (long index = 0; index < supernode->instances.count(); index++) {
                                    unsigned short instance_index = ((const unsigned short *)supernode->instances.address)[index];
                                    if (structure_instanced_geometry_mark(bsp_index, instance_index))
                                        instanced_geometry_get_features_in_sphere(&context, instance_index, flags.collision_flags,
                                            point, radius, feature_radius, height, features);
                                }
                            }
                        }

                        if (collision_flag_test(flags.object_flags, 0)) {
                            long iterator;
                            for (long object_index = collisions_cluster_get_first_collideable_object(&iterator, cluster_reference);
                                object_index != -1; object_index = collisions_cluster_get_next_collideable_object(&iterator)) {
                                if (collisions_object_mark(object_index))
                                    object_get_features_in_sphere(object_index, *(s_collision_test_flags *)&test_flags, point,
                                        radius, feature_radius, height, first_ignore_object_index, -1, features);
                            }
                        }
                    }
                }

                if (collision_flag_test(flags.object_flags, 0))
                    collisions_object_marker_end();
                if (collision_flag_test(flags.collision_flags, 3))
                    collisions_instanced_geometry_marker_end();
                collisions_cluster_marker_end();
            }
        }
    }

    return features->sphere_count || features->cylinder_count || features->prism_count;
}


static __forceinline void collisions_normalize_test_flags(s_collision_test_flags &flags)
{
    if (!collision_flag_test(flags.collision_flags, 11) && !collision_flag_test(flags.collision_flags, 12))
        flags.collision_flags |= 0x1800;
}

// @address 0x9d600 collision_test_vector
extern "C" bool collision_test_vector(s_collision_test_flags flags, bool test_exit, const real_point3d *point,
    const real_vector3d *vector, long first_ignore_object_index, long second_ignore_object_index, long ignore_index,
    collision_result *collision)
{
    bool hit = false;
    long entry_count = 0;
    float fraction = 1.0f;
    c_static_array<s_structure_bsp_entry_fraction, 16> entries;
    c_static_array<collision_bsp_test_vector_result, 16> bsp_results;

    collisions_normalize_test_flags(flags);
    collisions_initialize_vector_result(collision);
    unsigned long bsp_flags = build_bsp_flags_from_collision_flags(flags.collision_flags);
    const real_rectangle3d &empty_bounds = *global_null_rectangle3d;

    for (long structure_bsp_index = 0; !(hit && collision_flag_test(flags.collision_flags, 16))
        && structure_bsp_index < 16; structure_bsp_index++) {
        if (global_structure_bsp_is_active(structure_bsp_index)) {
            real_rectangle3d bounds;
            float maximum;
            collisions_structure_bsp_bounds_get(structure_bsp_index, &bounds, empty_bounds);
            entries[entry_count].structure_bsp_index = structure_bsp_index;
            if (clip_vector_to_rectangle(&bounds, point, vector, fraction, &entries[entry_count].fraction, &maximum))
                entry_count++;
        }
    }
    qsort(&entries[0], entry_count, sizeof(s_structure_bsp_entry_fraction), (int (__cdecl *)(const void *, const void *))collisions_compare_floats);

    long entry_index;
    for (entry_index = 0; !(hit && collision_flag_test(flags.collision_flags, 16)) && entry_index < entry_count; entry_index++) {
        s_structure_bsp_entry_fraction *entry = &entries[entry_index];
        long structure_bsp_index = entry->structure_bsp_index;
        if (fraction >= entry->fraction) {
            collision_bsp_test_vector_result *bsp_result = &bsp_results[structure_bsp_index];
            const structure_bsp *structure_bsp = global_structure_bsp_get(structure_bsp_index);
            const collision_bsp *bsp = collisions_global_bsp_get(structure_bsp_index);
            const s_collision_materials_extant_flags *materials
                = (const s_collision_materials_extant_flags *)&g_structure_seam_globals->bsp_seam_flags[structure_bsp_index];
            const s_breakable_surface_set *sets = breakable_surface_get_default_set(structure_bsp_index);
            short set_count = (short)global_structure_bsp_get(structure_bsp_index)->breakable_surfaces.count;
            if (collision_bsp_test_vector(bsp_flags, bsp, materials, set_count, sets,
                point, vector, k_real_max, bsp_result)) {
                if (collision_flag_test(flags.collision_flags, 0) && collision->fraction > bsp_results[structure_bsp_index].fraction) {
                    collision->type = 1;
                    collision->bsp = bsp;
                    const c_global_material_type *material = (short)bsp_results[structure_bsp_index].material != -1
                        ? &TAG_BLOCK_GET_ELEMENT(&structure_bsp->collision_materials,
                              (short)bsp_results[structure_bsp_index].material, const s_collision_material)->global_material
                        : &k_global_material_type_invalid;
                    collision->global_material = *material;
                    collision->geometry_index = -1;
                    collision->instance_index = -1;
                    collision->object_index = -1;
                    collision->structure_bsp_index = structure_bsp_index;
                    build_collision_result_from_bsp_result(collision, &bsp_results[structure_bsp_index], 0);
                    hit = true;
                }
                if (fraction > bsp_result->fraction && bsp_result->leaf_count > 0) {
                    collisions_location_from_leaf(&collision->start_location, structure_bsp_index, bsp_result->leaf_indices[0]);
                    collisions_location_from_leaf(&collision->location, structure_bsp_index,
                        bsp_result->leaf_indices[bsp_result->leaf_count - 1]);
                    fraction = bsp_result->fraction;
                }
            }
        }
    }

    for (entry_index = 0; !(hit && collision_flag_test(flags.collision_flags, 16)) && entry_index < entry_count; entry_index++) {
        s_structure_bsp_entry_fraction *entry = &entries[entry_index];
        long structure_bsp_index = entry->structure_bsp_index;
        if (fraction >= entry->fraction) {
            const long leaf_count = bsp_results[structure_bsp_index].leaf_count;
            if (leaf_count > 0 &&
                (collision_flag_test(flags.collision_flags, 1) || collision_flag_test(flags.collision_flags, 3)
                    || (flags.object_flags & 1)) &&
                !(hit && collision_flag_test(flags.collision_flags, 16))) {
                s_collision_bsp_test_context context;
                collision_bsp_test_context_initialize(structure_bsp_index, &context);
                collisions_cluster_marker_begin();
                if (collision_flag_test(flags.collision_flags, 3))
                    collisions_instanced_geometry_marker_begin();
                if ((flags.object_flags & 1) && !(flags.object_flags & k_collision_test_all_objects.object_flags)) {
                    s_collision_test_flags combined_flags;
                    combined_flags.collision_flags = k_collision_test_all_objects.collision_flags | flags.collision_flags;
                    combined_flags.object_flags = flags.object_flags | k_collision_test_all_objects.object_flags;
                    flags = combined_flags;
                }
                long leaf_index = 0;
                if (leaf_index < leaf_count) {
                do {
                    s_cluster_reference cluster_reference = structure_bsp_leaf_cluster_reference(
                        structure_bsp_index, bsp_results[structure_bsp_index].leaf_indices[leaf_index]);
                    s_cluster_stack connected;
                    if (collisions_ground_plane_cluster_mark(cluster_reference)) {
                        if (collision_test_cluster(&context, cluster_reference, flags, bsp_flags, point, vector,
                            first_ignore_object_index, second_ignore_object_index, ignore_index, collision)) {
                            hit = true;
                            if (collision_flag_test(flags.collision_flags, 16))
                                break;
                        }
                        if (structure_seams_connected_cluster_references_get(global_scenario_index, cluster_reference, false, &connected)) {
                            for (long connected_index = 0; connected_index < connected.m_count; connected_index++) {
                                const s_cluster_reference *connected_cluster = &connected.m_elements[connected_index];
                                s_cluster_reference connected_reference = *connected_cluster;
                                s_collision_bsp_test_context connected_context;
                                collision_bsp_test_context_initialize((long)connected_reference.bsp_index, &connected_context);
                                if (collisions_cluster_mark(connected_reference) &&
                                    collision_test_cluster(&connected_context, *connected_cluster, flags, bsp_flags, point, vector,
                                        first_ignore_object_index, second_ignore_object_index, ignore_index, collision)) {
                                    hit = true;
                                    if (collision_flag_test(flags.collision_flags, 16))
                                        break;
                                }
                            }
                            if (hit && collision_flag_test(flags.collision_flags, 16))
                                break;
                        }
                    }
                }
                while (++leaf_index < bsp_results[structure_bsp_index].leaf_count);
                }
                if (collision_flag_test(flags.collision_flags, 3))
                    collisions_instanced_geometry_marker_end();
                collisions_cluster_marker_end();
            }
        }
    }

    const unsigned long result_flags = flags.collision_flags;
    if (flags.object_flags & 1)
        hit |= object_broadphase_ray_cast(*(unsigned __int64 *)&flags, bsp_flags, point, vector,
            first_ignore_object_index, second_ignore_object_index, ignore_index, collision);
    point_from_line3d(point, vector, collision->fraction, &collision->point);

    bool compute_location;
    if (hit) {
        c_global_material_type material;
        compute_location = collision->type != 1 && !collision_flag_test(result_flags, 16);
        if (test_exit && collision->structure_bsp_index != -1 && collision->object_index == -1) {
            if ((material = collision_get_surface_material(collision->structure_bsp_index,
                (const collision_bsp *)collision->bsp, &collision->point, collision->surface_index,
                    collision->instance_index)) != k_global_material_type_invalid)
                collision->global_material = material;
        }
        if (collision_flag_test(result_flags, 17)) {
            const collision_bsp *bsp = 0;
            const real_matrix4x3 *matrix = 0;
            if (collision->type != 1)
                get_bsp_from_collision_result(collision, &bsp, &matrix);
            real_point3d local_point;
            if ((collision->structure_bsp_index != -1
                && (unsigned char)collision->location.cluster_reference.bsp_index != 255 &&
                    bsp3d_test_point(global_bsp3d_get(collision->structure_bsp_index), 0,
                        &collision->point) != (unsigned short)collision->location.leaf_index) ||
                (bsp && collision->leaf_index != -1 &&
                    bsp3d_test_point((const bsp3d *)bsp, 0, matrix4x3_inverse_transform_point(matrix, &collision->point,
                        &local_point)) != collision->leaf_index))
                    {
                point_from_line3d(&collision->point, &collision->plane.normal, 0.00024414062f, &collision->point);
                scenario_location_from_point(&collision->location, &collision->point);
                compute_location = false;
                if ((unsigned char)collision->location.cluster_reference.bsp_index == 255 ||
                    (bsp && bsp3d_test_point((const bsp3d *)bsp, 0, matrix4x3_inverse_transform_point(matrix, &collision->point,
                        &local_point)) == -1)) {
                    float dot = dot_product3d(vector, &collision->plane.normal);
                    float step = dot != 0.0f ? 0.00024414062f / (float)fabs(dot) : 0.03125f;
                    do {
                        collision->fraction = collision->fraction - step > 0.0f
                            ? collision->fraction - step : 0.0f;
                        point_from_line3d(point, vector, collision->fraction, &collision->point);
                        scenario_location_from_point(&collision->location, &collision->point);
                    } while (collision->fraction > 0.0f
                        && ((unsigned char)collision->location.cluster_reference.bsp_index == 255 ||
                        (bsp && bsp3d_test_point((const bsp3d *)bsp, 0, matrix4x3_inverse_transform_point(matrix,
                            &collision->point, &local_point)) == -1)));
                }
            }
        }
        if (compute_location)
            scenario_location_from_point(&collision->location, &collision->point);
    } else if (collision_flag_test(result_flags, 17)) {
        scenario_location_from_point(&collision->location, &collision->point);
    }
    return hit;
}

// @address 0x9f170 ?object_test_vector_internal@@YA_NJ_NUs_collision_test_flags@@KPBTreal_point3d@@PBTreal_vector3d@@JJJPAUcollision_result@@@Z
bool object_test_vector_internal(long object_index, bool test_children, s_collision_test_flags flags,
    unsigned long collision_model_flags, const real_point3d *point, const real_vector3d *vector,
    long first_ignore_object_index, long second_ignore_object_index, long ignore_index, collision_result *collision)
{
    bool found = false;
    collision_model_instance instance;
    collision_model_test_vector_result model_result;
    do
    {
        const s_object_header *header = object_header_get(object_index);
        const object_datum *object = (const object_datum *)header->datum;
        if (collision_test_object(object_index, header, object, flags,
                first_ignore_object_index, second_ignore_object_index, ignore_index)
            && fast_vector_intersects_sphere(point, vector, &object->bounding_sphere_center, object->bounding_sphere_radius)
            && collision_model_instance_new(&instance, object_index)
            && collision_model_test_vector(&instance, collision_model_flags, point, vector, &model_result)
            && !(collision->fraction <= model_result.bsp_result.fraction))
        {
            collision->type = 4;
            collision->bsp = collisions_model_bsp_get(&instance, model_result.bsp_reference);
            collision->global_material = object_get_material(instance.object_index, (short)model_result.bsp_result.material);
            collision->geometry_index = -1;
            collision->instance_index = -1;
            collision->object_index = object_index;
            collision->region_index = model_result.region_index;
            collision->node_index = model_result.node_index;
            collision->bsp_reference = model_result.bsp_reference;
            collision->structure_bsp_index = -1;
            build_collision_result_from_bsp_result(collision, &model_result.bsp_result,
                &instance.node_matrices[model_result.node_index]);
            found = true;
            if (collision_flag_test(flags.collision_flags, 16))
                break;
        }
        if (!collision_flag_test(flags.collision_flags, 5))
        {
            long child_index = object->first_child_object_index;
            if (child_index != -1 && object_test_vector_internal(child_index, false, flags, collision_model_flags,
                    point, vector, first_ignore_object_index, second_ignore_object_index, ignore_index, collision))
            {
                found = true;
                if (collision_flag_test(flags.collision_flags, 16))
                    break;
            }
        }
        object_index = object->next_object_index;
    } while (object_index != -1);
    return found;
}

// @address 0x28ad38 ?object_test_vector@@YA_NJUs_collision_test_flags@@PBTreal_point3d@@PBTreal_vector3d@@JJPAUcollision_result@@@Z
bool object_test_vector(long object_index, s_collision_test_flags flags, const real_point3d *point,
    const real_vector3d *vector, long first_ignore_object_index, long second_ignore_object_index,
    collision_result *collision)
{
    first_ignore_object_index = -1;
    second_ignore_object_index = -1;
    bool success = false;
    collisions_initialize_vector_result(collision);
    if ((flags.object_flags & 1) && !(flags.object_flags & k_collision_test_all_objects.object_flags))
    {
        s_collision_test_flags combined_flags;
        combined_flags.collision_flags = k_collision_test_all_objects.collision_flags | flags.collision_flags;
        combined_flags.object_flags = flags.object_flags | k_collision_test_all_objects.object_flags;
        flags = combined_flags;
    }
    if (!object_is_hidden(object_index) &&
        object_test_vector_internal(object_index, false, flags,
            build_bsp_flags_from_collision_flags(flags.collision_flags), point, vector,
            first_ignore_object_index, second_ignore_object_index, -1, collision))
        success = true;
    point_from_line3d(point, vector, collision->fraction, &collision->point);
    scenario_location_from_point(&collision->location, &collision->point);
    return success;
}










static __forceinline float collisions_normalize_vector3d(real_vector3d *vector)
{
    float length = magnitude3d(vector);
    if (fabs(length - 0.0f) < k_real_epsilon)
        length = 0.0f;
    else
        scale_vector3d(vector, 1.0f / length, vector);
    return length;
}


// @address 0x28c640 ?collision_get_unobstructed_point@@YA_N_KPBTreal_point3d@@MJJPAT1@PAM@Z
bool collision_get_unobstructed_point(unsigned __int64 test_flags, const real_point3d *center, float radius,
    // match: 0x28c640: without __restrict on new_radius the face/edge radius store sinks below the three new_center stores
    long first_ignore_object_index, long second_ignore_object_index, real_point3d *new_center, float *__restrict new_radius)
{
    bool success = false;
    if (radius > 0.0f)
    {
        long edge_count = 0;
        float edge_radius = radius;
        real_point3d edge_center = *global_origin3d;
        long face_count = 0;
        float face_radius = radius;
        real_point3d face_center = *global_origin3d;
        collision_feature_list features;
        collision_get_features_in_sphere(test_flags, center, radius, 0.0f, 0.0f, first_ignore_object_index, -1, &features);
        for (long prism_index = 0; prism_index < features.prism_count; prism_index++)
        {
            const collision_prism *prism = &features.prisms[prism_index];
            bool inside = true;
            float plane_distance = plane3d_distance_to_point(&prism->plane, center);
            float closest_distance_squared = k_real_max;
            real_point3d plane_point;
            point_from_line3d(center, &prism->plane.normal, -plane_distance, &plane_point);
            long point_count = prism->count > 8 ? 8 : prism->count;
            real_point3d closest_point;
            for (long point_index = 0; point_index < point_count; point_index++)
            {
                long next_point_index = (point_index + 1) % prism->count;
                real_point3d point0;
                real_point3d point1;
                project_point2d(&prism->points[point_index], &prism->plane, prism->projection, prism->sign, &point0);
                project_point2d(&prism->points[next_point_index], &prism->plane, prism->projection, prism->sign, &point1);
                real_vector3d to_plane_point;
                real_vector3d edge;
                vector_from_points3d(&point0, &plane_point, &to_plane_point);
                vector_from_points3d(&point0, &point1, &edge);
                const real_vector3d *normal = &prism->plane.normal;
                const float &edge_side = triple_product3d(&edge, &to_plane_point, normal);
                if (edge_side < 0.0f)
                {
                    inside = false;
                }
                else
                {
                    real_vector3d edge_normal;
                    collisions_cross_product3d_safe(&edge, &prism->plane.normal, &edge_normal);
                    real_point3d edge_point;
                    point_from_line3d(&plane_point, &edge_normal, -dot_product3d(&to_plane_point, &edge_normal), &edge_point);
                    real_vector3d to_edge_point;
                    vector_from_points3d(&point0, &edge_point, &to_edge_point);
                    float t = dot_product3d(&to_edge_point, &edge);
                    float edge_length_squared = dot_product3d(&edge, &edge);
                    real_point3d closest;
                    if (t <= 0.0f)
                        closest = point0;
                    else if (t >= edge_length_squared)
                        closest = point1;
                    else
                        closest = edge_point;
                    float distance_squared = distance_squared3d(&plane_point, &closest);
                    if (distance_squared < closest_distance_squared)
                    {
                        closest_distance_squared = distance_squared;
                        closest_point = closest;
                    }
                }
            }
            if (inside)
            {
                closest_distance_squared = 0.0f;
                closest_point = plane_point;
            }
            if (closest_distance_squared < radius * radius)
            {
                float threshold = 0.01f;
                bool use_normal = true;
                real_vector3d direction;
                if (fabs(plane_distance) >= threshold || !inside)
                {
                    vector_from_points3d(center, &closest_point, &direction);
                    if (collisions_normalize_vector3d(&direction) > threshold)
                        use_normal = false;
                }
                real_point3d point0;
                real_point3d point1;
                if (use_normal)
                {
                    real_vector3d vector;
                    collision_plane plane;
                    scale_vector3d(&prism->plane.normal, radius, &vector);
                    bool hit = collision_features_test_vector(&features, center, &vector, &plane);
                    if (hit)
                        point0 = plane.point;
                    else
                        point_from_line3d(center, &vector, 1.0f, &point0);
                    point1 = *center;
                }
                else
                {
                    real_vector3d vector;
                    collision_plane plane;
                    scale_vector3d(&direction, radius, &vector);
                    bool hit = collision_features_test_vector(&features, center, &vector, &plane);
                    if (hit)
                        point0 = plane.point;
                    else
                        point_from_line3d(center, &vector, 1.0f, &point0);
                    scale_vector3d(&direction, -radius, &vector);
                    hit = collision_features_test_vector(&features, center, &vector, &plane);
                    if (hit)
                        point1 = plane.point;
                    else
                        point_from_line3d(center, &vector, 1.0f, &point1);
                }
                real_point3d midpoint;
                midpoint3d(&point0, &point1, &midpoint);
                const float &distance = distance3d(&point0, &midpoint);
                if (use_normal)
                {
                    add_points3d(&face_center, &midpoint, &face_center);
                    face_radius = face_radius > distance ? distance : face_radius;
                    face_count++;
                }
                else
                {
                    add_points3d(&edge_center, &midpoint, &edge_center);
                    edge_radius = edge_radius > distance ? distance : edge_radius;
                    edge_count++;
                }
            }
        }
        if (face_count > 0)
        {
            scale_point3d(&face_center, 1.0f / (float)face_count, new_center);
            *new_radius = face_radius;
            success = true;
        }
        else if (edge_count > 0)
        {
            scale_point3d(&edge_center, 1.0f / (float)edge_count, new_center);
            *new_radius = edge_radius;
            success = true;
        }
        else
        {
            *new_center = *center;
            *new_radius = radius;
        }
    }
    else
    {
        *new_center = *center;
        *new_radius = radius;
    }
    return success;
}

// @address 0xf03afc k_collision_test_none
// @address 0x708558 ??__Ek_collision_test_none@@YAXXZ
s_collision_test_flags k_collision_test_none = collision_test_flags_new(0, 0);
// @address 0x708578 ??__Ek_collision_test_structure@@YAXXZ
s_collision_test_flags k_collision_test_structure = collision_test_flags_new(0x809, 0);
// @address 0x708598 ??__Ek_collision_test_structure_and_water@@YAXXZ
s_collision_test_flags k_collision_test_structure_and_water
    = collision_test_flags_new(k_collision_test_structure.collision_flags | 2, k_collision_test_structure.object_flags);
// @address 0x7085c8 ??__Ek_collision_test_objects@@YAXXZ
s_collision_test_flags k_collision_test_objects = collision_test_flags_new(0, 0x2985);
// @address 0x7085e8 ??__Ek_collision_test_all_objects@@YAXXZ
s_collision_test_flags k_collision_test_all_objects = collision_test_flags_new(0, 0x7ffe);
// @address 0x708608 ??__Ek_projectiles_collision_test_flags@@YAXXZ
s_collision_test_flags k_projectiles_collision_test_flags
    = collision_test_flags_new(k_collision_test_structure_and_water.collision_flags | 0x24000,
    k_collision_test_structure_and_water.object_flags | 1);
// @address 0x708640 ??__Ek_projectiles_fat_collision_test_flags@@YAXXZ
s_collision_test_flags k_projectiles_fat_collision_test_flags = collision_test_flags_new(0x4800, 0x2007);
// @address 0x708660 ??__Ek_ai_melee_collision_flags@@YAXXZ
s_collision_test_flags k_ai_melee_collision_flags
    = collision_test_flags_new(k_projectiles_collision_test_flags.collision_flags | 0x10000,
    k_projectiles_collision_test_flags.object_flags);
// @address 0x708690 ??__Ek_collision_test_default@@YAXXZ
s_collision_test_flags k_collision_test_default
    = collision_test_flags_new(k_collision_test_objects.collision_flags | k_collision_test_structure.collision_flags | 0x6000,
    k_collision_test_objects.object_flags | k_collision_test_structure.object_flags | 1);
// @address 0x7086d8 ??__Ek_aim_assist_los_collision_flags@@YAXXZ
s_collision_test_flags k_aim_assist_los_collision_flags
    = collision_test_flags_new(k_collision_test_default.collision_flags | 0x10000, k_collision_test_default.object_flags);
// @address 0x708708 ??__Ek_collision_test_default_without_instances@@YAXXZ
s_collision_test_flags k_collision_test_default_without_instances
    = collision_test_flags_new(k_collision_test_objects.collision_flags | k_collision_test_structure.collision_flags | 0x4000,
    k_collision_test_objects.object_flags | k_collision_test_structure.object_flags | 1);
// @address 0x708750 ??__Ek_collision_test_default_with_phantoms@@YAXXZ
s_collision_test_flags k_collision_test_default_with_phantoms
    = collision_test_flags_new(k_collision_test_default.collision_flags | 0x20, k_collision_test_default.object_flags);
// @address 0x708780 ??__Ek_collision_test_visibility@@YAXXZ
s_collision_test_flags k_collision_test_visibility = collision_test_flags_new(k_collision_test_objects.collision_flags | 0x24c09,
    k_collision_test_objects.object_flags | 0x11);
// @address 0x7087b8 ??__Ek_collision_test_visibility_first_person@@YAXXZ
s_collision_test_flags k_collision_test_visibility_first_person
    = collision_test_flags_new((k_collision_test_visibility.collision_flags & ~0x30000ul) | 0x10000, 0x11);
// @address 0x7087e8 ??__Ek_collision_test_camera@@YAXXZ
s_collision_test_flags k_collision_test_camera = collision_test_flags_new(9, 0x9a5);
// @address 0x708808 ??__Ek_unit_access_collision_flags@@YAXXZ
s_collision_test_flags k_unit_access_collision_flags
    = collision_test_flags_new(k_collision_test_objects.collision_flags | k_collision_test_structure.collision_flags | 0x4000,
    k_collision_test_objects.object_flags | k_collision_test_structure.object_flags | 1);
// @address 0x708850 ??__Ek_biped_camera_collision_flags@@YAXXZ
s_collision_test_flags k_biped_camera_collision_flags
    = collision_test_flags_new(k_collision_test_structure.collision_flags | 0x4000, k_collision_test_structure.object_flags | 0x81);
// @address 0x708880 ??__Ek_collision_test_camera_excluding_objects@@YAXXZ
s_collision_test_flags k_collision_test_camera_excluding_objects
    = collision_test_flags_new(k_collision_test_structure.collision_flags | 0x80000, k_collision_test_structure.object_flags | 0x81);
// @address 0x7088b0 ??__Ek_unit_enter_seat_collision_flags@@YAXXZ
s_collision_test_flags k_unit_enter_seat_collision_flags = collision_test_flags_new(k_collision_test_structure.collision_flags,
    k_collision_test_structure.object_flags | 0x101);
// @address 0x708960 ??__Ek_flying_vehicle_collision_flags@@YAXXZ
s_collision_test_flags k_flying_vehicle_collision_flags = collision_test_flags_new(0x1889, 0x985);
// @address 0x708980 ??__Ek_ai_line_of_sight_default_flags@@YAXXZ
s_collision_test_flags k_ai_line_of_sight_default_flags
    = collision_test_flags_new(k_collision_test_objects.collision_flags | 0xd80b, k_collision_test_objects.object_flags | 0x11);
// @address 0x7089e0 ??__Ek_collision_test_structure_without_objects@@YAXXZ
s_collision_test_flags k_collision_test_structure_without_objects = collision_test_flags_new(0x13801, 0);
// @address 0x708a08 ??__Ek_collision_test_structure_first_person@@YAXXZ
s_collision_test_flags k_collision_test_structure_first_person = collision_test_flags_new(0x10801, 0);
