// blamlibxbox/physics_model_definitions.obj: from the records 0x1037b8 0x1038a8 0x104b40 0x1c0098 0x1c00a8 0x1c00b8 0x1c00c8 0x1c00d8 0x1c0180 0x1c0190 0x1c01a0 0x1c01b0 0x1c0238 0x1c0898 0x1c0a08 0x1c0be8 0x1c0c58 0x1c0cc8 0x1c0d78 0x1c0e50 0x1c0ef8 0x1c0fa0 0x1c1008 0x1c10c0 0x1c10c8 0x1c1190 0x1c1240 0x1c1248 0x1c1250 0x1c1258 0x1c1360 0x1c14b8
// @flags /TP /O2 /Oc
// @address 0x1038a8 ?get_shape_container@phys_bvh_tree_shape@@UBAPBVphys_shape_container@@XZ
// @address 0x104b40 ?query_obb@c_mopp_bv_tree_shape@@UBAXABVphys_transform@@ABVphys_vec4@@MAAV?$phys_array@I@@@Z
// @address 0x1c0fa0 ?shape_query_sphere@c_mopp_bv_tree_shape@@UBAXABVphys_sphere@@AAV?$phys_array@I@@@Z
// @address 0x1c10c0 ?get_aabb_impl@c_mopp_bv_tree_shape@@UBAXABVphys_transform@@MAAVphys_aabb@@@Z
// @address 0x1c1190 ?trace_ray_with_collector@c_mopp_bv_tree_shape@@UBAXABUphys_ray_input@@ABVphys_collision_proxy@@AAVphys_ray_hit_collector@@@Z
#include "physics/physics_model_types.h"
#include "physics/character_physics_shapes.h"
#include "physics/character_physics_shape_types.h"
#include "physics/character_physics_capsule_types.h"
#include "math/matrix_math_types.h"
#include "game/game_globals.h"
#include "memory/data_types.h"
#include "phys/phys_base_types.h"
#include "phys/phys_sphere.h"
#include <stddef.h>
#include "math/real_math.h"
#include "tag_files/tag_groups.h"
// @address 0x1037b8 physics_model_is_shape_static
// @address 0x1c0098 ??_Gc_multi_sphere_shape@@UAAPAXI@Z
// @address 0x1c00a8 ??_Gc_capsule_shape@@UAAPAXI@Z
// @address 0x1c00b8 ??_Gc_sphere_shape@@UAAPAXI@Z
// @address 0x1c00c8 ??_Gc_convex_transform_shape@@UAAPAXI@Z
// @address 0x1c0180 ??_Gc_box_shape@@UAAPAXI@Z
// @address 0x1c14b8 ??_Gc_bv_shape@@UAAPAXI@Z
// @address 0x1c0190 ??_Gc_triangle_shape@@UAAPAXI@Z
// @address 0x1c01a0 ??_Gc_convex_vertices_shape@@UAAPAXI@Z
// @address 0x1c01b0 ??_Gc_list_shape@@UAAPAXI@Z
// @address 0x3c344 ??_7c_phantom_shape@@6B@
// @address 0x1c0898 ?physics_model_get_shape_from_type_and_index@@YAPAVphys_shape@@PBUs_physics_model_definition@@FF@Z
// @address 0x1c0d78 ?physics_model_try_and_get_constraint@@YAPAUs_physics_model_constraint_bodies@@PAUs_physics_model_definition@@PBUs_physics_model_constraint_reference@@@Z
// @address 0x1c1008 ?query_aabb@c_mopp_bv_tree_shape@@UBAXABVphys_aabb@@AAV?$phys_array@I@@@Z
// @address 0x1c10c8 ?trace_ray_impl@c_mopp_bv_tree_shape@@UBA?AVphys_bool@@ABUphys_ray_input@@AAUphys_ray_output@@@Z
// @address 0x1c1240 ?physics_model_get_multi_sphere_shape_size@@YAHXZ
#include <ppcintrinsics.h>
#include <math.h>
#include <stdlib.h>
#include <vectorintrinsics.h>
#include "physics/havok_component.h"
#include "physics/physics_model_definitions.h"
#include "physics/physics_model_constraint_reference.h"
#include "game/game_engine_base_variant.h"
#include "game/materials.h"
#include "cache/cache_files.h"
#include "scenario/scenario.h"
#include "phys/phys_collision_proxy.h"

// @address 0x1620 ??_7c_convex_transform_shape@@6B@
// @address 0x16d4 k_c_transform_shape_vtable
class phys_world_object;

#include "math/real_constant_pool_access.h"
#include "phys/phys_collide.h"
template <class T> class phys_array;
class phys_ray_hit_collector;
class phys_shape_container;

#include "phys/phys_aabb_types.h"
struct phys_ray_output;

// match: 0x1c1360: suppress this destructor's vtable store without changing constructors in other units.
class __declspec(novtable) phys_convex_hull_shape;
#include "physics/physics_model_shape_internal.h"

#include "phys/phys_base.h"

#include "phys/phys_convex_hull_shape.h"

typedef char physics_convex_vertices_half_extents_offset[(sizeof(void *) != 4 || ((offsetof(phys_convex_hull_shape, m_aabb_half_extents) == 0x20))) ? 1 : -1];

#include "math/real_constant_pool.h"
/* the default constraint instance: two identity transforms and a null reference */
struct physics_model_default_constraint {
    long reserved0;
    long reserved1;
    real_matrix4x3 transform;
    real_matrix4x3 inverse_transform;
    s_physics_model_constraint_reference reference;
};
extern "C" {
// @address 0x119afec g_physics_model_default_constraint_guard
int g_physics_model_default_constraint_guard;
// @address 0x7c5f90 g_physics_model_default_constraint
physics_model_default_constraint g_physics_model_default_constraint = { -1, -1 };
}

typedef char physics_model_layout_1[(sizeof(void *) != 4 || ((sizeof(c_transform_shape) == 0x30))) ? 1 : -1];
#include "phys/phys_shape_container.h"
#include "physics/physics_model_constraint_bodies_iterator.h"

class phys_world_object;
class phys_collider;
struct phys_collision_input;
#include "physics/havok_component_class.h"
#include "physics/havok_component_datum_access.h"
#include "phys/phys_volume_callback_shape.h"
class c_phantom_shape : public phys_volume_callback_shape {
public:
    __forceinline c_phantom_shape(phys_fixup_tag flag, long material_index) : phys_volume_callback_shape(flag)
    {
        initialize_tag_shape_reference_count_and_user_data(this, material_index);
    }
    __forceinline long get_phantom_material_index() const { return (long)get_user_data(); }
    virtual void volume_enter_event(const phys_collider *, const phys_collider *, const phys_collision_input &);
    virtual void volume_leave_event(const phys_collider *, const phys_collider *);
};

// Serialized rigid bodies and material records used by the material query.
enum e_physics_model_shape_type {
    _physics_model_shape_type_sphere = 0,
    _physics_model_shape_type_capsule = 1,
    _physics_model_shape_type_box = 2,
    _physics_model_shape_type_triangle = 3,
    _physics_model_shape_type_convex_vertices = 4,
    _physics_model_shape_type_multi_sphere = 5,
    _physics_model_shape_type_phantom_shape = 6,
    _physics_model_shape_type_list = 14,
    _physics_model_shape_type_mopp = 15
};
struct physics_model_material { long fallback_name; long name; long flags; };
#include "phys/phys_bounding_shape.h"
#include "phys/phys_transform_shape.h"
#include "game/materials_definitions.h"
#include "cseries/crt_xdk.h"

#include "phys/phys_shape_constants.h"

class c_convex_transform_shape : public phys_convex_xform_shape
{
public:
    __forceinline c_convex_transform_shape(phys_fixup_tag flag, phys_ulong user_data, phys_shape *shape) : phys_convex_xform_shape(flag)
    {
        m_single_child_shape = phys_single_shape_container(shape);
        initialize_tag_shape_reference_count_and_user_data(this, user_data);
    }
    // match: 0x1c00c8: the tag-owned shape destructor is unreachable.
    virtual ~c_convex_transform_shape() { __assume(0); }
};
#include "phys/phys_bounding_shape.h"
class c_bv_shape : public phys_bounding_shape
{
public:
    __forceinline c_bv_shape(phys_fixup_tag, phys_ulong user_data, const phys_shape *bounding_shape, const phys_shape *child_shape) :
        phys_bounding_shape(bounding_shape, child_shape)
    {
        initialize_tag_shape_reference_count_and_user_data(this, user_data);
    }
};
struct physics_fixup_multi_sphere { s_character_physics_shape_properties properties; c_multi_sphere_shape shape; };
struct physics_fixup_box { s_character_physics_shape_properties properties; c_box_shape shape; c_convex_transform_shape transform; };
struct physics_fixup_triangle { s_character_physics_shape_properties properties; c_triangle_shape shape; };
struct physics_fixup_convex { s_character_physics_shape_properties properties; c_convex_vertices_shape shape; };
struct physics_fixup_phantom { c_bv_shape bounding; c_phantom_shape phantom; };

// Serialized extents agree with the corresponding canonical Havok layouts.
typedef char physics_model_layout_6[(sizeof(void *) != 4 || ((sizeof(physics_fixup_box) == 0xb0))) ? 1 : -1];
typedef char physics_model_layout_10[(sizeof(void *) != 4 || ((sizeof(phys_bvh_bytecode) == 0x30))) ? 1 : -1];
typedef char physics_model_layout_11[(offsetof(phys_bvh_bytecode, m_data) == 0x20) ? 1 : -1];

#ifdef HALO3_64BIT
#include "physics/havok_tag_unpack_64.h"

typedef char physics_model_64_sphere_size[(sizeof(s_character_physics_dead_sphere) == sizeof(physics_sphere_record) && sizeof(s_character_physics_dead_sphere) == 0xA0) ? 1 : -1];
typedef char physics_model_64_sphere_offset[(offsetof(s_character_physics_dead_sphere, transform) == offsetof(physics_sphere_record, shape)) ? 1 : -1];
typedef char physics_model_64_multi_sphere_size[(sizeof(physics_fixup_multi_sphere) == sizeof(physics_multi_sphere_record) && sizeof(physics_fixup_multi_sphere) == 0xD0) ? 1 : -1];
typedef char physics_model_64_multi_sphere_offset[(offsetof(physics_fixup_multi_sphere, shape) == offsetof(physics_multi_sphere_record, shape)) ? 1 : -1];
typedef char physics_model_64_capsule_size[(sizeof(s_character_physics_capsule) == sizeof(physics_capsule_record) && sizeof(s_character_physics_capsule) == 0x70) ? 1 : -1];
typedef char physics_model_64_capsule_offset[(offsetof(s_character_physics_capsule, shape) == offsetof(physics_capsule_record, shape)) ? 1 : -1];
typedef char physics_model_64_box_size[(sizeof(physics_fixup_box) == sizeof(physics_box_record) && sizeof(physics_fixup_box) == 0xE0) ? 1 : -1];
typedef char physics_model_64_box_offset[(offsetof(physics_fixup_box, transform) == offsetof(physics_box_record, shape)) ? 1 : -1];
typedef char physics_model_64_triangle_size[(sizeof(physics_fixup_triangle) == sizeof(physics_triangle_record) && sizeof(physics_fixup_triangle) == 0x90) ? 1 : -1];
typedef char physics_model_64_triangle_offset[(offsetof(physics_fixup_triangle, shape) == offsetof(physics_triangle_record, shape)) ? 1 : -1];
typedef char physics_model_64_convex_size[(sizeof(physics_fixup_convex) == sizeof(physics_convex_record) && sizeof(physics_fixup_convex) == 0xA0) ? 1 : -1];
typedef char physics_model_64_convex_offset[(offsetof(physics_fixup_convex, shape) == offsetof(physics_convex_record, shape)) ? 1 : -1];
typedef char physics_model_64_list_size[(sizeof(c_list_shape) == sizeof(physics_list_record) && sizeof(c_list_shape) == 0x60) ? 1 : -1];
typedef char physics_model_64_list_child_size[(sizeof(phys_list_shape::child_shape_entry) == 0x20) ? 1 : -1];
typedef char physics_model_64_mopp_size[(sizeof(c_mopp_bv_tree_shape) == sizeof(physics_mopp_record) && sizeof(c_mopp_bv_tree_shape) == 0x40) ? 1 : -1];
typedef char physics_model_64_phantom_size[(sizeof(physics_fixup_phantom) == sizeof(physics_phantom_record) && sizeof(physics_fixup_phantom) == 0x58) ? 1 : -1];
typedef char physics_model_64_mopp_code_size[(sizeof(phys_bvh_bytecode) == 0x40) ? 1 : -1];
#endif

union s_shape_reference
{
    __forceinline s_shape_reference(const phys_shape *shape) { pointer = shape; }
    const phys_shape *pointer;
    struct { short type; short index; };
};
struct physics_model_serialized_mopp_code
{
    phys_bvh_bytecode code;
    unsigned char bytecode[1];
};
typedef char physics_model_mopp_bytecode_offset[(sizeof(void *) != 4 || ((offsetof(physics_model_serialized_mopp_code, bytecode) == 0x30))) ? 1 : -1];

bool physics_model_is_shape_static(const phys_shape *target_shape)
{
    // The serialized static marker is the high byte of the Havok size/flags field.
#if defined(_M_PPCBE) && !defined(__clang__)
    return *reinterpret_cast<const char *>(&target_shape->m_mem_size_and_flags) == 0;
#else
    return (target_shape->m_mem_size_and_flags >> 8) == 0;
#endif
}

// 0x1038a8
const phys_shape_container *phys_bvh_tree_shape::get_shape_container() const
{
    const phys_compound_shape *collection = get_compound_shape();
    return collection->get_shape_container();
}

// 0x104b40
void c_mopp_bv_tree_shape::query_obb(const phys_transform &transform, const phys_vec4 &extent,
    float tolerance, phys_array<unsigned int> &hits) const
{
    if (m_scale != real_constant_pool_zero()) {
        phys_transform scaled_transform;
        scaled_transform.m_rotation.get_column(0).m_simd_quad = transform.m_rotation.get_column(0).m_simd_quad;
        scaled_transform.m_rotation.get_column(1).m_simd_quad = transform.m_rotation.get_column(1).m_simd_quad;
        scaled_transform.m_rotation.get_column(2).m_simd_quad = transform.m_rotation.get_column(2).m_simd_quad;
        scaled_transform.m_translation.m_simd_quad = transform.m_translation.m_simd_quad;
        phys_vec4 scaled_extent;
        scaled_extent = extent;
        scaled_transform.get_translation().mul4(phys_simd_scalar(real_constant_pool_one() / m_scale));
        scaled_extent.mul4(phys_simd_scalar(real_constant_pool_one() / m_scale));
        phys_bvh_tree_shape::query_obb(scaled_transform, scaled_extent,
            tolerance * (real_constant_pool_one() / m_scale), hits);
    } else {
        phys_bvh_tree_shape::query_obb(transform, extent, tolerance, hits);
    }
}

// @address 0x1c00d8 ??0c_transform_shape@@QAA@Vphys_fixup_tag@@JPAVphys_shape@@@Z
c_transform_shape::c_transform_shape(phys_fixup_tag flag, long data, phys_shape *shape) :
    phys_convex_offset_shape(flag)
{
    m_single_child_shape = phys_single_shape_container(shape);
    initialize_tag_shape_reference_count_and_user_data(this, 0);
}

phys_shape *physics_model_get_shape_from_type_and_index(const s_physics_model_definition *model, short shape_type, short shape_index)
{
    phys_shape *shape = 0;
    switch (shape_type) {
    case _physics_model_shape_type_sphere: shape = (phys_shape *)model->spheres[shape_index].shape; break;
    case _physics_model_shape_type_multi_sphere: shape = (phys_shape *)model->multi_spheres[shape_index].shape; break;
    case _physics_model_shape_type_capsule: shape = (phys_shape *)model->capsules[shape_index].shape; break;
    case _physics_model_shape_type_box: shape = (phys_shape *)model->boxes[shape_index].shape; break;
    case _physics_model_shape_type_triangle: shape = (phys_shape *)model->triangles[shape_index].shape; break;
    case _physics_model_shape_type_convex_vertices: shape = (phys_shape *)model->convex_vertices[shape_index].shape; break;
    case _physics_model_shape_type_list: shape = (phys_shape *)model->lists[shape_index].shape; break;
    case _physics_model_shape_type_mopp: shape = (phys_shape *)model->mopps[shape_index].shape; break;
    case _physics_model_shape_type_phantom_shape: shape = (phys_shape *)model->phantom_shapes[shape_index].shape; break;
    }
    return shape;
}

// 0x1c0cc8
s_tag_block *physics_model_constraint_reference_get_tag_block(s_physics_model_definition *model,
    const s_physics_model_constraint_reference *reference, long *element_size)
{
    s_tag_block *block = 0;
    switch (reference->type) {
    case 0: block = &model->hinge_constraints; *element_size = 120; break;
    case 1: block = &model->limited_hinge_constraints; *element_size = 132; break;
    case 2: block = &model->ragdoll_constraints; *element_size = 148; break;
    case 3: block = &model->stiff_spring_constraints; *element_size = 124; break;
    case 4: block = &model->ball_and_socket_constraints; *element_size = 120; break;
    case 5: block = &model->prismatic_constraints; *element_size = 132; break;
    case 6: block = &model->powered_chains; *element_size = 24; break;
    // match: 0x1c0cc8: __assume(0) omits the jump-table bounds check for the unreachable default
    default: __assume(0);
    }
    return block;
}

// @address 0x1c0d78 physics_model_try_and_get_constraint
s_physics_model_constraint_bodies *physics_model_try_and_get_constraint(s_physics_model_definition *model,
    const s_physics_model_constraint_reference *reference)
{
    int guard;
    s_tag_block *block;
    short index;
    long element_size;
    guard = g_physics_model_default_constraint_guard;
    if ((guard & 1u) == 0) {
        g_physics_model_default_constraint_guard = (guard | 1);
        g_physics_model_default_constraint.transform = k_real_math_identity_matrix_storage;
        g_physics_model_default_constraint.inverse_transform = k_real_math_identity_matrix_storage;
        g_physics_model_default_constraint.reference.type = -1;
        g_physics_model_default_constraint.reference.index = 0;
    }
    s_physics_model_constraint_bodies *result = 0;
    block = physics_model_constraint_reference_get_tag_block(model, reference, &element_size);
    index = reference->index;
    if (index < block->count) {
        if ((unsigned short)reference->type == 6) {
            result = (s_physics_model_constraint_bodies *)&g_physics_model_default_constraint;
        } else {
            result = (s_physics_model_constraint_bodies *)((char *)block->address + index * element_size);
        }
    }
    return result;
}

// @address 0x1c0be8 ?init@c_physics_model_constraint_bodies_iterator@@AAAXXZ
void c_physics_model_constraint_bodies_iterator::init()
{
    reference.type = 0;
    reference.index = 0;
    while (physics_model_try_and_get_constraint(model, &reference) == 0 && reference.type < 6) {
        reference.index = 0;
        reference.type++;
    }
}

// @address 0x1c0c58 ?next@c_physics_model_constraint_bodies_iterator@@QAAXXZ
void c_physics_model_constraint_bodies_iterator::next()
{
    reference.index++;
    while (physics_model_try_and_get_constraint(model, &reference) == 0 && reference.type < 5) {
        reference.index = 0;
        reference.type++;
    }
}

// @address 0x1c0e50 ?volume_enter_event@c_phantom_shape@@UAAXPBVphys_collider@@0ABUphys_collision_input@@@Z
void c_phantom_shape::volume_enter_event(const phys_collider *collidable,
    const phys_collider *other_collidable, const phys_collision_input &input)
{
    const phys_world_object *world_object = (const phys_world_object *)collidable->get_collider_owner();
    const phys_world_object *other_world_object = (const phys_world_object *)other_collidable->get_collider_owner();
    if (world_object != 0 && other_world_object != 0) {
        int component_index = ::havok_entity_get_havok_component_index(world_object);
        if (component_index != -1) {
            int rigid_body_index = ::havok_entity_get_havok_component_rigid_body_index(world_object);
            c_havok_component *component = havok_component_get(component_index);
            component->notify_entity_entered_phantom_material(component_index, rigid_body_index,
                world_object, other_world_object, (long)m_user_data);
        }
    }
}

// @address 0x1c0ef8 ?volume_leave_event@c_phantom_shape@@UAAXPBVphys_collider@@0@Z
void c_phantom_shape::volume_leave_event(const phys_collider *collidable,
    const phys_collider *other_collidable)
{
    const phys_world_object *world_object = (const phys_world_object *)collidable->get_collider_owner();
    const phys_world_object *other_world_object = (const phys_world_object *)other_collidable->get_collider_owner();
    if (world_object != 0 && other_world_object != 0) {
        int component_index = ::havok_entity_get_havok_component_index(world_object);
        if (component_index != -1) {
            int rigid_body_index = ::havok_entity_get_havok_component_rigid_body_index(world_object);
            c_havok_component *component = havok_component_get(component_index);
            component->notify_entity_left_phantom_material(component_index, rigid_body_index,
                world_object, other_world_object, (long)m_user_data);
        }
    }
}

// 0x1c0fa0
void c_mopp_bv_tree_shape::shape_query_sphere(const phys_sphere &sphere, phys_array<unsigned int> &hits) const
{
    if (m_scale != real_constant_pool_zero()) {
        phys_sphere scaled_sphere;
        scaled_sphere.set_position_and_radius(sphere.get_position_and_radius());
        scaled_sphere.get_position_and_radius().mul4(phys_simd_scalar(real_constant_pool_one() / m_scale));
        phys_bvh_tree_shape::shape_query_sphere(scaled_sphere, hits);
    } else {
        phys_bvh_tree_shape::shape_query_sphere(sphere, hits);
    }
}

// 0x1c1008
void c_mopp_bv_tree_shape::query_aabb(const phys_aabb &aabb, phys_array<unsigned int> &hits) const
{
    if (m_scale != real_constant_pool_zero()) {
        phys_aabb scaled_aabb;
        scaled_aabb.m_min_limit = aabb.m_min_limit;
        scaled_aabb.m_max_limit = aabb.m_max_limit;
        scaled_aabb.m_min_limit.mul4(phys_simd_scalar(real_constant_pool_one() / m_scale));
        scaled_aabb.m_max_limit.mul4(phys_simd_scalar(real_constant_pool_one() / m_scale));
        phys_bvh_tree_shape::query_aabb(scaled_aabb, hits);
    } else {
        phys_bvh_tree_shape::query_aabb(aabb, hits);
    }
}

// 0x1c10c0
void c_mopp_bv_tree_shape::get_aabb_impl(const phys_transform &local_to_world, float tolerance, phys_aabb &out) const
{
    phys_bvh_tree_shape::get_aabb_impl(local_to_world, tolerance, out);
}

phys_bool c_mopp_bv_tree_shape::trace_ray_impl(const phys_ray_input &input, phys_ray_output &output) const
{
    phys_bool result;
    if (m_scale != real_constant_pool_zero()) {
        phys_ray_input scaled;
        scaled = input;
        scaled.m_ray_from.mul4(phys_simd_scalar(real_constant_pool_one() / m_scale));
        scaled.m_ray_to.mul4(phys_simd_scalar(real_constant_pool_one() / m_scale));
        result = phys_bvh_tree_shape::trace_ray_impl(scaled, output);
    } else {
        result = phys_bvh_tree_shape::trace_ray_impl(input, output);
    }
    return result;
}

// 0x1c1190
void c_mopp_bv_tree_shape::trace_ray_with_collector(const phys_ray_input &input,
    const phys_collision_proxy &body, phys_ray_hit_collector &collector) const
{
    if (m_scale != real_constant_pool_zero()) {
        phys_ray_input scaled;
        scaled = input;
        scaled.m_ray_from.mul4(phys_simd_scalar(real_constant_pool_one() / m_scale));
        scaled.m_ray_to.mul4(phys_simd_scalar(real_constant_pool_one() / m_scale));
        phys_bvh_tree_shape::trace_ray_with_collector(scaled, body, collector);
    } else {
        phys_bvh_tree_shape::trace_ray_with_collector(input, body, collector);
    }
}

// @address 0x1c1240 physics_model_get_multi_sphere_shape_size
int physics_model_get_multi_sphere_shape_size()
{
    return 96;
}

// @address 0x1c1248 ?physics_model_get_sphere_shape_size@@YAHXZ
int physics_model_get_sphere_shape_size()
{
    return 48;
}

// @address 0x1c1250 ?physics_model_get_capsule_shape_size@@YAHXZ
int physics_model_get_capsule_shape_size()
{
    return 64;
}

// @address 0x1c1258 ?get_game_engine_name_string_id@c_game_engine_base_variant@@MBAJXZ
long c_game_engine_base_variant::get_game_engine_name_string_id() const
{
    return -1;
}

// @address 0x1c1360 ??1phys_convex_hull_shape@@UAA@XZ
phys_convex_hull_shape::~phys_convex_hull_shape()
{
}

static __forceinline bool physics_model_definitions_is_collection(phys_shape *shape)
{
    bool result = false;
    if (shape->m_type == 2 || shape->m_type == 10 || shape->m_type == 18)
        result = true;
    return result;
}
static __forceinline s_character_physics_shape_properties *physics_model_definitions_primitive(phys_shape *shape)
{
    if (shape->m_type == 25)
        shape = (phys_shape *)((phys_bounding_shape *)shape)->get_bounding_volume_shape();
    if (shape->m_type == 28)
        shape = (phys_shape *)((phys_transform_shape *)shape)->get_child_shape();
    else if (shape->m_type == 13 || shape->m_type == 12)
    {
        if (shape->m_type == 13)
            shape = (phys_shape *)((phys_convex_xform_shape *)shape)->get_child_shape();
        else
            shape = (phys_shape *)((phys_convex_offset_shape *)shape)->get_child_shape();
    }
    return (s_character_physics_shape_properties *)shape->m_user_data;
}
static __forceinline c_global_material_type physics_model_definitions_material_index(long name)
{
    c_global_material_type material;
    material.m_index = -1;
    for (long i = 0; i < global_game_globals->materials.count(); i++) {
        if (name == global_game_globals->materials[i].material_id) {
            material.m_index = (short)i;
            break;
        }
    }
    return material;
}
static __forceinline s_global_material_definition *physics_model_definitions_material_get(c_global_material_type type)
{
    s_global_material_definition *material = 0;
    short index = (short)type.m_index;
    if (index != -1 && index >= 0 && index < global_game_globals->materials.count())
        material = &global_game_globals->materials[index];
    return material;
}
static __forceinline s_global_material_definition *physics_model_definitions_material(long name)
{
    return physics_model_definitions_material_get(physics_model_definitions_material_index(name));
}
// @address 0x1c0a08 ?physics_model_compute_best_material_rigid_body@@YAJJJPAVc_global_material_type@@@Z
long physics_model_compute_best_material_rigid_body(long model_index, long body_index, c_global_material_type *global_material)
{
    s_physics_model_definition *model = TAG_GET('phmo', s_physics_model_definition, model_index);
    phys_shape *shape = model->rigid_bodies[body_index].shape;
    long material_name = 0;
    if (shape->m_type == 27)
        shape = (phys_shape *)((phys_bvh_tree_shape *)shape)->get_compound_shape();
    s_character_physics_shape_properties *primitive;
    if (physics_model_definitions_is_collection(shape)) {
        const phys_shape_container *container = (phys_compound_shape *)shape;
        __declspec(align(16)) char buffer[512];
        primitive = physics_model_definitions_primitive((phys_shape *)((phys_compound_shape *)shape)->get_child_shape(
            ((phys_compound_shape *)shape)->get_first_key(), buffer));
    } else {
        primitive = physics_model_definitions_primitive(shape);
    }
    if (primitive->material_index != -1) {
        physics_model_material *material = &model->materials[primitive->material_index];
        material_name = material->name;
        if (!physics_model_definitions_material(material_name))
            material_name = material->fallback_name;
    }
    global_material->m_index = primitive->runtime_material_type;
    return material_name;
}

static __forceinline short shape_type_from_pointer(const phys_shape *shape)
{
    s_shape_reference reference(shape);
    return reference.type;
}
static __forceinline short shape_index_from_pointer(const phys_shape *shape)
{
    s_shape_reference reference(shape);
    return reference.index;
}
static __forceinline phys_shape *physics_model_fixup_shape_pointer(const s_physics_model_definition *physics_model, const phys_shape *shape)
{
    short shape_type = shape_type_from_pointer(shape);
    short shape_index = shape_index_from_pointer(shape);
    return physics_model_get_shape_from_type_and_index(physics_model, shape_type, shape_index);
}
#ifndef HALO3_64BIT
static __forceinline phys_bvh_bytecode *mopp_code_data_get_mopp_code(s_tag_data *data, long offset)
{
    physics_model_serialized_mopp_code *serialized = (physics_model_serialized_mopp_code *)((char *)data->address + offset);
    serialized->code.m_data.m_data = serialized->bytecode;
    return &serialized->code;
}
static __forceinline long mopp_code_get_tag_data_size(const phys_bvh_bytecode *code)
{
    return (code->m_data.m_size + 63) & ~15;
}
#else
static void physics_model_unpack_shapes_64(s_physics_model_definition *physics_model)
{
    // 1. spheres: 0x70 -> sizeof(s_character_physics_dead_sphere) (0xA0)
    {
        s_tag_block *block = &physics_model->spheres;
        if (block->count > 0 && block->address && !tag_block_is_unpacked_64(block))
        {
            const unsigned char *src_base = (const unsigned char *)block->address;
            s_character_physics_dead_sphere *dst_base = (s_character_physics_dead_sphere *)havok_unpack_alloc_64(
                (size_t)block->count * sizeof(s_character_physics_dead_sphere));
            for (long i = 0; i < block->count; i++)
            {
                const unsigned char *src = src_base + (size_t)i * 0x70;
                s_character_physics_dead_sphere *dst = &dst_base[i];
                memcpy(dst, src, 0x20);
                dst->shape.set_shape_radius(*(const float *)(src + 0x30));
                dst->transform.set_shape_radius(*(const float *)(src + 0x50));
                memcpy(&dst->transform.get_translation(), src + 0x60, 0x10);
            }
            block->address = dst_base;
            tag_block_mark_unpacked_64(block);
        }
    }
    // 2. multi_spheres: 0xC0 -> sizeof(physics_fixup_multi_sphere) (0xD0)
    {
        s_tag_block *block = &physics_model->multi_spheres;
        if (block->count > 0 && block->address && !tag_block_is_unpacked_64(block))
        {
            const unsigned char *src_base = (const unsigned char *)block->address;
            physics_fixup_multi_sphere *dst_base = (physics_fixup_multi_sphere *)havok_unpack_alloc_64(
                (size_t)block->count * sizeof(physics_fixup_multi_sphere));
            for (long i = 0; i < block->count; i++)
            {
                const unsigned char *src = src_base + (size_t)i * 0xC0;
                physics_fixup_multi_sphere *dst = &dst_base[i];
                memcpy(&dst->properties, src, 0x20);
                dst->shape.m_sphere_count = *(const int *)(src + 0x30);
                memcpy(dst->shape.m_sphere_array, src + 0x40, 8 * sizeof(phys_vec4));
            }
            block->address = dst_base;
            tag_block_mark_unpacked_64(block);
        }
    }
    // 3. capsules: 0x60 -> sizeof(s_character_physics_capsule) (0x70)
    {
        s_tag_block *block = &physics_model->capsules;
        if (block->count > 0 && block->address && !tag_block_is_unpacked_64(block))
        {
            const unsigned char *src_base = (const unsigned char *)block->address;
            s_character_physics_capsule *dst_base = (s_character_physics_capsule *)havok_unpack_alloc_64(
                (size_t)block->count * sizeof(s_character_physics_capsule));
            for (long i = 0; i < block->count; i++)
            {
                const unsigned char *src = src_base + (size_t)i * 0x60;
                s_character_physics_capsule *dst = &dst_base[i];
                memcpy(&dst->properties, src, 0x20);
                dst->shape.set_shape_radius(*(const float *)(src + 0x30));
                memcpy(const_cast<phys_vec4 *>(dst->shape.get_vertices()), src + 0x40, 0x20);
            }
            block->address = dst_base;
            tag_block_mark_unpacked_64(block);
        }
    }
    // 4. boxes: 0xB0 -> sizeof(physics_fixup_box) (0xE0)
    {
        s_tag_block *block = &physics_model->boxes;
        if (block->count > 0 && block->address && !tag_block_is_unpacked_64(block))
        {
            const unsigned char *src_base = (const unsigned char *)block->address;
            physics_fixup_box *dst_base = (physics_fixup_box *)havok_unpack_alloc_64(
                (size_t)block->count * sizeof(physics_fixup_box));
            for (long i = 0; i < block->count; i++)
            {
                const unsigned char *src = src_base + (size_t)i * 0xB0;
                physics_fixup_box *dst = &dst_base[i];
                memcpy(&dst->properties, src, 0x20);
                dst->shape.set_shape_radius(*(const float *)(src + 0x30));
                memcpy(const_cast<phys_vec4 *>(&dst->shape.get_half_extents()), src + 0x40, 0x10);
                dst->transform.set_shape_radius(*(const float *)(src + 0x60));
                memcpy(&dst->transform.get_transform(), src + 0x70, 0x40);
            }
            block->address = dst_base;
            tag_block_mark_unpacked_64(block);
        }
    }
    // 5. triangles: 0x80 -> sizeof(physics_fixup_triangle) (0x90)
    {
        s_tag_block *block = &physics_model->triangles;
        if (block->count > 0 && block->address && !tag_block_is_unpacked_64(block))
        {
            const unsigned char *src_base = (const unsigned char *)block->address;
            physics_fixup_triangle *dst_base = (physics_fixup_triangle *)havok_unpack_alloc_64(
                (size_t)block->count * sizeof(physics_fixup_triangle));
            for (long i = 0; i < block->count; i++)
            {
                const unsigned char *src = src_base + (size_t)i * 0x80;
                physics_fixup_triangle *dst = &dst_base[i];
                memcpy(&dst->properties, src, 0x20);
                dst->shape.set_shape_radius(*(const float *)(src + 0x30));
                memcpy(&dst->shape.m_edge_weld_info, src + 0x34, 4);
                memcpy(&dst->shape.m_vertex_a, src + 0x40, 0x40);
            }
            block->address = dst_base;
            tag_block_mark_unpacked_64(block);
        }
    }
    // 6. convex_vertices: 0x80 -> sizeof(physics_fixup_convex) (0xA0)
    {
        s_tag_block *block = &physics_model->convex_vertices;
        if (block->count > 0 && block->address && !tag_block_is_unpacked_64(block))
        {
            const unsigned char *src_base = (const unsigned char *)block->address;
            physics_fixup_convex *dst_base = (physics_fixup_convex *)havok_unpack_alloc_64(
                (size_t)block->count * sizeof(physics_fixup_convex));
            for (long i = 0; i < block->count; i++)
            {
                const unsigned char *src = src_base + (size_t)i * 0x80;
                physics_fixup_convex *dst = &dst_base[i];
                memcpy(&dst->properties, src, 0x20);
                dst->shape.set_shape_radius(*(const float *)(src + 0x30));
                memcpy(&dst->shape.m_aabb_half_extents, src + 0x40, 0x10);
                memcpy(&dst->shape.m_hull_aabb_center, src + 0x50, 0x10);
                dst->shape.m_rotated_vertices.m_size = *(const int *)(src + 0x64);
                dst->shape.m_rotated_vertices.m_capacity_and_flags = *(const int *)(src + 0x68);
                dst->shape.m_hull_vertex_count = *(const int *)(src + 0x6C);
                dst->shape.m_plane_equations.m_size = *(const int *)(src + 0x74);
                dst->shape.m_plane_equations.m_capacity_and_flags = *(const int *)(src + 0x78);
            }
            block->address = dst_base;
            tag_block_mark_unpacked_64(block);
        }
    }
    // 7. lists: 0x50 -> sizeof(c_list_shape) (0x60)
    {
        s_tag_block *block = &physics_model->lists;
        if (block->count > 0 && block->address && !tag_block_is_unpacked_64(block))
        {
            const unsigned char *src_base = (const unsigned char *)block->address;
            c_list_shape *dst_base = (c_list_shape *)havok_unpack_alloc_64(
                (size_t)block->count * sizeof(c_list_shape));
            for (long i = 0; i < block->count; i++)
            {
                const unsigned char *src = src_base + (size_t)i * 0x50;
                c_list_shape *dst = &dst_base[i];
                memcpy(&dst->m_edge_weld_disabled, src + 0x14, 1);
                dst->m_child_info.m_size = *(const int *)(src + 0x1C);
                dst->m_child_info.m_capacity_and_flags = *(const int *)(src + 0x20);
                memcpy(&dst->m_aabb_half_extents, src + 0x30, 0x10);
                memcpy(&dst->m_aabb_center, src + 0x40, 0x10);
            }
            block->address = dst_base;
            tag_block_mark_unpacked_64(block);
        }
    }
    // 8. list_children: 0x10 -> sizeof(phys_list_shape::child_shape_entry) (0x20)
    {
        s_tag_block *block = &physics_model->list_children;
        if (block->count > 0 && block->address && !tag_block_is_unpacked_64(block))
        {
            const unsigned char *src_base = (const unsigned char *)block->address;
            phys_list_shape::child_shape_entry *dst_base = (phys_list_shape::child_shape_entry *)havok_unpack_alloc_64(
                (size_t)block->count * sizeof(phys_list_shape::child_shape_entry));
            for (long i = 0; i < block->count; i++)
            {
                const unsigned char *src = src_base + (size_t)i * 0x10;
                phys_list_shape::child_shape_entry *dst = &dst_base[i];
                unsigned int raw_ref = *(const unsigned int *)(src + 0x00);
                dst->m_proxy_shape = (const phys_shape *)(uintptr_t)raw_ref;
                dst->m_collision_filter_info = *(const unsigned int *)(src + 0x04);
                dst->m_shape_size = *(const int *)(src + 0x08);
                dst->m_child_shape_count = *(const int *)(src + 0x0C);
            }
            block->address = dst_base;
            tag_block_mark_unpacked_64(block);
        }
    }
    // 9. mopps: 0x20 -> sizeof(c_mopp_bv_tree_shape) (0x40)
    {
        s_tag_block *block = &physics_model->mopps;
        if (block->count > 0 && block->address && !tag_block_is_unpacked_64(block))
        {
            const unsigned char *src_base = (const unsigned char *)block->address;
            c_mopp_bv_tree_shape *dst_base = (c_mopp_bv_tree_shape *)havok_unpack_alloc_64(
                (size_t)block->count * sizeof(c_mopp_bv_tree_shape));
            for (long i = 0; i < block->count; i++)
            {
                const unsigned char *src = src_base + (size_t)i * 0x20;
                c_mopp_bv_tree_shape *dst = &dst_base[i];
                unsigned int raw_child_ref = *(const unsigned int *)(src + 0x14);
                new (&dst->m_child_shape) phys_single_shape_container(
                    phys_fixup_tag(), (const phys_shape *)(uintptr_t)raw_child_ref);
                dst->m_scale = *(const float *)(src + 0x1C);
            }
            block->address = dst_base;
            tag_block_mark_unpacked_64(block);
        }
    }
    // 10. phantom_shapes: 0x2C -> sizeof(physics_fixup_phantom) (0x58)
    {
        s_tag_block *block = &physics_model->phantom_shapes;
        if (block->count > 0 && block->address && !tag_block_is_unpacked_64(block))
        {
            const unsigned char *src_base = (const unsigned char *)block->address;
            physics_fixup_phantom *dst_base = (physics_fixup_phantom *)havok_unpack_alloc_64(
                (size_t)block->count * sizeof(physics_fixup_phantom));
            for (long i = 0; i < block->count; i++)
            {
                const unsigned char *src = src_base + (size_t)i * 0x2C;
                physics_fixup_phantom *dst = &dst_base[i];
                unsigned int raw_bounding_ref = *(const unsigned int *)(src + 0x10);
                dst->bounding.m_bounding_shape = (const phys_shape *)(uintptr_t)raw_bounding_ref;
                unsigned int raw_material_index = *(const unsigned int *)(src + 0x24);
                dst->phantom.set_user_data((phys_ulong)(long)raw_material_index);
            }
            block->address = dst_base;
            tag_block_mark_unpacked_64(block);
        }
    }
}
#endif
// @address 0x1c0238 ?physics_model_fixup@@YAXJ_N@Z
void physics_model_fixup(long physics_model_index, bool vtables_only)
{
    vtables_only = false;
    s_physics_model_definition *physics_model = (s_physics_model_definition *)TAG_GET('phmo', s_physics_model_definition, physics_model_index);
#ifdef HALO3_64BIT
    physics_model_unpack_shapes_64(physics_model);
#endif
    if (!vtables_only)
    {
        // match: 0x1c0238: identity rotations shift temp numbering; keeps this loop's element-address add register pairing.
        for (long rigid_body_index = 0; rigid_body_index < (long)_rotl(_rotl(physics_model->rigid_bodies.s_tag_block::count, 0), 0); rigid_body_index++)
        {
            s_physics_model_rigid_body *rigid_body = TAG_BLOCK_GET_ELEMENT(&physics_model->rigid_bodies, rigid_body_index, s_physics_model_rigid_body);
            rigid_body->shape = physics_model_fixup_shape_pointer((s_physics_model_definition *)physics_model, rigid_body->shape);
        }
    }
    for (long sphere_index = 0; sphere_index < physics_model->spheres.s_tag_block::count; sphere_index++)
    {
        s_character_physics_dead_sphere *sphere = TAG_BLOCK_GET_ELEMENT(&physics_model->spheres, sphere_index, s_character_physics_dead_sphere);
        new (&sphere->shape) c_sphere_shape(phys_fixup_tag(), (phys_ulong)sphere);
        new (&sphere->transform) c_transform_shape(phys_fixup_tag(), 0, (phys_shape *)&sphere->shape);
    }
    // match: 0x1c0238: identity rotations shift temp numbering; keeps this loop's element-address add register pairing.
    for (long multi_sphere_index = 0; multi_sphere_index < (long)_rotl(physics_model->multi_spheres.s_tag_block::count, 0); multi_sphere_index++)
    {
        physics_fixup_multi_sphere *multi_sphere = TAG_BLOCK_GET_ELEMENT(&physics_model->multi_spheres, multi_sphere_index, physics_fixup_multi_sphere);
        new (&multi_sphere->shape) c_multi_sphere_shape(phys_fixup_tag(), (phys_ulong)multi_sphere);
    }
    // match: 0x1c0238: identity rotations shift temp numbering; keeps this loop's element-address add register pairing.
    for (long capsule_index = 0; capsule_index < (long)_rotl(_rotl(physics_model->capsules.s_tag_block::count, 0), 0); capsule_index++)
    {
        s_character_physics_capsule *capsule = TAG_BLOCK_GET_ELEMENT(&physics_model->capsules, capsule_index, s_character_physics_capsule);
        new (&capsule->shape) c_capsule_shape(phys_fixup_tag(), (phys_ulong)capsule);
    }
    // match: 0x1c0238: identity rotations shift temp numbering; keeps this loop's element-address add register pairing.
    for (long box_index = 0; box_index < (long)_rotl(_rotl(physics_model->boxes.s_tag_block::count, 0), 0); box_index++)
    {
        physics_fixup_box *box = TAG_BLOCK_GET_ELEMENT(&physics_model->boxes, box_index, physics_fixup_box);
        new (&box->shape) c_box_shape(phys_fixup_tag(), (phys_ulong)box);
        new (&box->transform) c_convex_transform_shape(phys_fixup_tag(), 0, (phys_shape *)&box->shape);
        if (physics_model->version < 1)
            box->shape.set_shape_radius(phys_convex_shape_default_radius * 0.328f);
    }
    for (long triangle_index = 0; triangle_index < physics_model->triangles.s_tag_block::count; triangle_index++)
    {
        physics_fixup_triangle *triangle = TAG_BLOCK_GET_ELEMENT(&physics_model->triangles, triangle_index, physics_fixup_triangle);
        new (&triangle->shape) c_triangle_shape(phys_fixup_tag(), (phys_ulong)triangle);
        if (physics_model->version < 1)
            triangle->shape.set_shape_radius(phys_convex_shape_default_radius * 0.328f);
    }
    long vertices_index = 0;
    long planes_index = 0;
    // match: 0x1c0238: identity rotations shift temp numbering; keeps this loop's element-address add register pairing.
    for (long convex_index = 0; convex_index < (long)_rotl(physics_model->convex_vertices.s_tag_block::count, 0); convex_index++)
    {
        phys_convex_hull_shape::four_vecs *vertices = TAG_BLOCK_GET_ELEMENT(&physics_model->polyhedron_vertices,
            vertices_index, phys_convex_hull_shape::four_vecs);
        physics_fixup_convex *convex_vertices = TAG_BLOCK_GET_ELEMENT(&physics_model->convex_vertices, convex_index, physics_fixup_convex);
        c_convex_vertices_shape *shape = &convex_vertices->shape;
        phys_vec4 *planes = TAG_BLOCK_GET_ELEMENT(&physics_model->polyhedron_planes, planes_index, phys_vec4);
        new (shape) c_convex_vertices_shape(phys_fixup_tag(), (phys_ulong)convex_vertices, vertices, planes);
        vertices_index += shape->m_rotated_vertices.getSize();
        planes_index += shape->m_plane_equations.getSize();
        if (physics_model->version < 1)
            shape->set_shape_radius(phys_convex_shape_default_radius * 0.328f);
    }
    long shapes_index = 0;
    for (long list_index = 0; list_index < physics_model->lists.s_tag_block::count; list_index++)
    {
        c_list_shape *list = TAG_BLOCK_GET_ELEMENT(&physics_model->lists, list_index, c_list_shape);
        phys_list_shape::child_shape_entry *child_info = TAG_BLOCK_GET_ELEMENT(&physics_model->list_children, shapes_index, phys_list_shape::child_shape_entry);
        new (list) c_list_shape(phys_fixup_tag(), 0, child_info);
        for (long child_index = 0; child_index < list->get_child_shape_count(); child_index++)
        {
            child_info[child_index].m_proxy_shape = physics_model_fixup_shape_pointer((s_physics_model_definition *)physics_model,
                child_info[child_index].m_proxy_shape);
            child_info[child_index].m_collision_filter_info = 0;
        }
        shapes_index += list->m_child_info.getSize();
    }
    long mopp_index;
    long mopp_code_offset = 0;
#ifdef HALO3_64BIT
    phys_bvh_bytecode *mopp_codes_64 = 0;
    if (physics_model->mopps.s_tag_block::count > 0 && !(physics_model->flags & 1))
        mopp_codes_64 = (phys_bvh_bytecode *)havok_unpack_alloc_64((size_t)physics_model->mopps.s_tag_block::count * sizeof(phys_bvh_bytecode));
#endif
    for (mopp_index = 0; mopp_index < physics_model->mopps.s_tag_block::count; mopp_index++)
    {
        c_mopp_bv_tree_shape *mopp = TAG_BLOCK_GET_ELEMENT(&physics_model->mopps, mopp_index, c_mopp_bv_tree_shape);
        phys_bvh_bytecode *mopp_code;
        c_list_shape *list = TAG_BLOCK_GET_ELEMENT(&physics_model->lists, shape_index_from_pointer(mopp->get_compound_shape()), c_list_shape);
        if (physics_model->flags & 1)
            mopp_code = 0;
        else
        {
#ifdef HALO3_64BIT
            const unsigned char *src_entry = (const unsigned char *)physics_model->mopp_codes.address + mopp_code_offset;
            phys_bvh_bytecode::bytecode_info src_info;
            memcpy(&src_info, src_entry + 0x10, sizeof(src_info));
            int src_size = *(const int *)(src_entry + 0x24);
            mopp_code = new (&mopp_codes_64[mopp_index]) phys_bvh_bytecode(src_info, src_entry + 0x30, src_size);
            mopp_code->m_build_type = static_cast<phys_bvh_bytecode::bvh_build_kind>(*(const signed char *)(src_entry + 0x2C));
            mopp_code_offset += (src_size + 0x3f) & ~0xf;
#else
            mopp_code = mopp_code_data_get_mopp_code(&physics_model->mopp_codes, mopp_code_offset);
            mopp_code_offset += mopp_code_get_tag_data_size(mopp_code);
#endif
        }
        new (mopp) c_mopp_bv_tree_shape(phys_fixup_tag(), 0, list, mopp_code);
    }
    for (long phantom_index = 0; phantom_index < physics_model->phantom_shapes.s_tag_block::count; phantom_index++)
    {
        physics_fixup_phantom *phantom = TAG_BLOCK_GET_ELEMENT(&physics_model->phantom_shapes, phantom_index, physics_fixup_phantom);
        c_bv_shape *bv_shape = &phantom->bounding;
        c_phantom_shape *phantom_shape = &phantom->phantom;
        long material_index = phantom_shape->get_phantom_material_index();
        phys_shape *bounding_shape = physics_model_fixup_shape_pointer((s_physics_model_definition *)physics_model, bv_shape->get_bounding_volume_shape());
        new (bv_shape) c_bv_shape(phys_fixup_tag(), 0, bounding_shape, (const phys_shape *)phantom_shape);
        new (phantom_shape) c_phantom_shape(phys_fixup_tag(), material_index);
    }
    for (long list_index = 0; list_index < physics_model->lists.s_tag_block::count; list_index++)
    {
        c_list_shape *list = TAG_BLOCK_GET_ELEMENT(&physics_model->lists, list_index, c_list_shape);
        ((phys_list_shape *)list)->recompute_bounds_extents();
    }
}

