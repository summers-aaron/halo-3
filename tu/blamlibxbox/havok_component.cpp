#include "math/real_constant_pool.h"
// blamlibxbox/havok_component.obj: from the records 0x9ce30 0x9fde0 0xdf3f0 0xe2ec0 0xe8f68 0xe9ad8 0xeb010 0xeb078 0xec098 0xefac0 0xefba8 0xefbe8 0xf08c0 0xf1b10 0xfe330 0x104e40 0x104e90 0x104f30 0x1050d0 0x1051b8 0x105f00 0x106a50 0x106b50 0x106ef8 0x106f78 0x107060 0x107118 0x107888 0x108278 0x108328 0x109290 0x10c1d0 0x10f3f8 0x10f668 0x113e10 0x113f00 0x1ba760 0x1c1260 0x250438 0x2504e8 0x250618 0x250708 0x250980 0x250a20 0x250ac0 0x250c48 0x250d40 0x250ed0 0x250fd0 0x2510d0 0x251170 0x251388 0x251458 0x251568 0x2515d8 0x251648 0x251820 0x251958 0x251be0 0x251f00 0x252220 0x2522f0 0x2523e8 0x252550 0x252758 0x2528b8 0x2529d8 0x252c58 0x253a30 0x253c90 0x253d00 0x253e18 0x253e90 0x253fc8 0x254858 0x254928 0x254d30 0x254d48 0x255000 0x255318 0x2554f8 0x255b00 0x255d70 0x255e60 0x256388 0x2565d0 0x256920 0x256c90 0x257268 0x2575a8 0x257690 0x257ad0 0x257bd0 0x257cf8 0x257e80 0x258270 0x2583e8 0x258740 0x258890 0x258de0 0x259190 0x259678 0x259970 0x259b00 0x259b80 0x259ca8 0x259d38 0x36cb40 0x36d118 0x5f8090 0x5f8118 0x5f8258
#include "math/random_math.h"
#include "math/random_math_range.h"
// @address 0x74b910 g_havok_monitor_globals
#include "objects/object_reference_types.h"
#include "math/real_math_types.h"
#include "phys/phys_collision_listener.h"
#include "game/game_engine_default.h"
#include "objects/objects.h"
#include "objects/object_markers_inline.h"
#include "objects/damage.h"
#include "memory/data_array_functions.h"
#include "render/render_cameras.h"
#include "phys/phys_array_util.h"
#include "physics/havok_rigid_body_state.h"
#include "physics/havok_shape_key.h"
#include "physics/havok_contact_point_reference.h"
#include "units/vehicle_type_component_internals.h"
#include <xtl.h>
#include "physics/havok_monitor_storage.h"
#include "phys/phys_dynamics.h"
#include "phys/phys_spatial_body_deactivator.h"
#include "phys/phys_broadphase.h"
#include "phys/phys_powered_chain_data.h"
#include "phys/phys_contact_point_props.h"

// @flags /TP /O2 /Oc /DPHYS_RIGID_BODY_OUT_OF_LINE /DXM_NO_MISALIGNED_VECTOR_ACCESS
#include "physics/physics_constants_tls.h"
// @address 0x1918 k_havok_phantom_byte_rounding_bias
#include "math/matrix_math_types.h"
#include "models/model_types.h"
#include "units/biped_definitions.h"
#include "units/vehicle_definitions.h"
#include "scenario/scenario_structure_bsp_reference.h"
#include "structures/structure_bsp_tag.h"
#include "math/matrix_math_functions.h"
#include "math/real_math_functions.h"
#include "math/real_math_globals.h"
#include "physics/havok_component_globals.h"
#include "physics/havok_group_types.h"
#include "physics/physics_model_constraint_types.h"
#include "physics/physics_model_definition_types.h"
#include <stddef.h>
#include <math.h>
#include "game/game_globals_storage.h"
#include "memory/static_arrays.h"
#include "memory/data_types.h"
#include "memory/datum_index.h"
#include "tag_files/tag_types.h"
#include "phys/phys_base_types.h"
#include "objects/object_datum.h"
#include "objects/object_placement.h"
#include "units/vehicle_types.h"
#include "physics/character_physics_component.h"
#include "physics/character_physics_definition_types.h"
#include "cseries/cseries_wrapped.h"
#include "tag_files/tag_groups.h"
#include "game/game_engine_util.h"
#include "motor/motor_system.h"
#include "units/motor_program_state.h"
#include "physics/havok.h"
#include "physics/havok_component.h"
#include "physics/physics_constants_globals.h"
#include "physics/havok_halo_external.h"
#include "physics/havok_utilities.h"
#include "physics/impacts.h"
#include "physics/impact.h"
#include "physics/physics_models.h"
#include "units/bipeds.h"
#include "units/biped_types.h"
#include "creatures/creature_datum.h"
#include "units/vehicle_datum.h"
#include "units/vehicles.h"
#include "physics/havok_vehicle_physics_definitions.h"
#include "phys/phys_collision_proxy.h"
#include "phys/phys_entity_activation_listener.h"
#include "phys/phys_memory_class.h"
#include "math/function_definitions.h"
#include "scenario/scenario_tag.h"
#include "physics/collision_bsp_types.h"

#include "phys/phys_collide.h"
#include "phys/phys_motor.h"
#include "physics/physics_model_constraint_bodies_iterator.h"
#include "phys/phys_transform_shape.h"
#include "physics/havok_collision_geometry_shape.h"
#include "physics/character_physics_shape_types.h"
#include "physics/character_physics_mode_dead.h"
#include "physics/impacts_globals_types.h"

#include "physics/havok_memory_globals.h"
#include "physics/havok_memory_class.h"
#include "phys/phys_inertia_util.h"

class phys_transform;
class phys_vec4;
class phys_rotation;
class phys_rigid_body_deactivator;

class phys_mat3;
class phys_world_object;
class phys_ref_object;
// @address 0x106f78 ?calculate_global_material_type_b@c_havok_contact_point@@QBA?AVc_global_material_type@@XZ
// @address 0xeb6aa0 ?g_havok_memory@@3Vc_havok_memory@@A
// @address 0x9ce30 ?get_rigid_body_linear_velocity@c_havok_component@@QAAXJPATreal_vector3d@@@Z
// @address 0x9fde0 ?get_rigid_body_center_of_mass@c_havok_component@@QAAXJPATreal_point3d@@@Z
// @address 0xe8f68 ?set_rigid_body_linear_velocity@c_havok_component@@QAAXJPBTreal_vector3d@@@Z
// @address 0xe9ad8 ?update_rigid_body_pre_simulation_state@c_havok_component@@AAAXJ@Z
// @address 0xeb010 ?get_rigid_body_angular_velocity@c_havok_component@@QAAXJPATreal_vector3d@@@Z
// @address 0xeb078 ?set_rigid_body_angular_velocity@c_havok_component@@QAAXJPBTreal_vector3d@@@Z
// @address 0xefac0 ?calculate_local_space_physics_bounding_box@c_havok_component@@QAA_NJPATreal_rectangle3d@@@Z
// @address 0x108328 ?calculate_object_transform@c_havok_component@@QAAXPAUreal_matrix4x3@@PBU2@@Z
// @address 0x10f668 ?calculate_physics_bounding_sphere@c_havok_component@@QAA_NPATreal_point3d@@PAM@Z
// @address 0xefba8 ?get_rigid_body_is_dynamic@c_havok_component@@QBA_NJ@Z
// @address 0xefbe8 ?get_rigid_body_transform@c_havok_component@@QAAXJPAUreal_matrix4x3@@@Z
// @address 0xf1b10 ?get_rigid_body_mass@c_havok_component@@QAAMJ@Z
// @address 0xfe330 ?havok_entity_get_havok_component_index@@YAJPBVphys_world_object@@@Z
// @address 0x104e40 ?havok_entity_get_havok_component_rigid_body_index@@YAJPBVphys_world_object@@@Z
// @address 0x104e90 ?havok_entity_get_structure_bsp_index@@YAJPBVphys_world_object@@@Z
// @address 0x107060 impact_initialize_contact_description
// @address 0x10c1d0 ?get_surface_normal@c_havok_contact_point@@QBAXPATreal_vector3d@@@Z
// @address 0x113f00 ?get_rigid_body_linear_velocity_at_point@c_havok_component@@QAAXJPATreal_vector3d@@PBTreal_point3d@@@Z
// @address 0x250438 ?compute_bounds@phys_aabb_util@@SAXABVphys_transform@@ABVphys_vec4@@1MAAVphys_aabb@@@Z
// @address 0x250618 ?havok_component_new@@YAJJ@Z
// @address 0x250d40 ?calculate_rigid_body_index_from_node_index@c_havok_component@@QAAJJ@Z
// @address 0x254d30 ?get_body_in_phantom_count@c_havok_component@@QBAJXZ
// @address 0x259b00 ??1c_rigid_body@c_havok_component@@QAA@XZ
// @address 0x259b80 entity_belongs_in_phantom
// @address 0xe2ec0 havok_component_activate_rigid_bodies
class phys_entity;
#include "items/projectile_definitions.h"
#include "objects/crates.h"
#include "creatures/creature_definitions.h"
#include "units/giant_datum.h"
#include "physics/physics_model_instance.h"
#include "physics/havok_math_types.h"
#include "phys/phys_math_value_types.h"
#include "phys/phys_contact_point.h"
#include "physics/collision_bsp_cache_types.h"
#include "objects/object_definitions.h"
#include "structures/structure_instanced_geometry.h"
#include "math/real_math.h"
#include "math/point_math.h"
#include "memory/tls_globals.h"
#include "physics/havok_vehicle_physics.h"
#include "game/materials.h"
#include "physics/collision_materials.h"
#include "cache/cache_file_globals.h"
#include "physics/havok_contact_points.h"
#include "physics/havok_memory.h"
#include "physics/impact_array_types.h"
#include "physics/memory_allocator_types.h"
#include "items/items.h"
#include "scenario/scenario.h"
#include "objects/widgets/muffins.h"
#include "objects/widgets/muffin_definitions.h"
#include "physics/physics_model_definitions.h"
#include "cseries/crt_xdk.h"
#include <xboxmath.h>
#include <string.h>
#include <vectorintrinsics.h>
#include <ppcintrinsics.h>
#include <stdlib.h>
#include <float.h>
// Consolidated headers (moved above the first function).
#include "phys/phys_constraint_data.h"
#include "phys/phys_constraint.h"
#include "phys/phys_ragdoll_data.h"
#include "phys/phys_limited_hinge_data.h"
#include "phys/phys_hinge_data.h"
#include "phys/phys_ball_socket_data.h"
#include "phys/phys_prismatic_data.h"
#include "phys/phys_stiff_spring_data.h"
#include "physics/havok_world_constraint.h"
#include "physics/havok_math.h"
#include "objects/object_header.h"
#include "objects/object_type_access.h"
#include "physics/havok_contact_point.h"
#include "phys/phys_aabb_types.h"
#include "phys/phys_aabb_util.h"
#include "models/render_model_definitions.h"
#include <new>
#include "physics/runtime_node_to_havok_component_rigid_body_index_mapping.h"
#include "physics/havok_component_class.h"
#include "physics/havok_component_datum_access.h"
#include "phys/phys_dynamics_contact_manager.h"
#include "structures/structure_instanced_geometry_definition.h"
#include "physics/havok_constants_types.h"
#include "physics/havok_contact_point_iterator_types.h"
#include "physics/havok_contact_point_iterator_inlines.h"
#include "physics/havok_penetration_collector.h"
#include "physics/havok_component_retail_phantom.h"
#include "phys/phys_world_op_util.h"
#include "objects/object_node_matrices_inline.h"
#include "physics/havok_entity_listener.h"
#include "game/game_time_types.h"
#include "physics/physics_model_shape_internal.h"
#include "physics/collision_bsp_physics_definitions.h"
#include "physics/collision_model_definition_types.h"
#include "physics/character_physics_definitions.h"
#include "physics/character_physics_mode_sentinel.h"
#include "physics/runtime_node_rigid_body_indices_types.h"
#include "physics/runtime_node_to_physics_model_rigid_body_index_mapping.h"
#include "physics/havok_component_owner_helpers.h"
#include "phys/phys_scalar_types.h"
// File-local types (moved above the first function).
class c_object_rigid_body_deactivator : public phys_spatial_body_deactivator {
public:
    c_object_rigid_body_deactivator(phys_rigid_body *rigid_body, long object_index) :
        phys_spatial_body_deactivator(),
        m_object_index(object_index)
    {
        m_rigid_body_index = havok_entity_get_havok_component_rigid_body_index(rigid_body);
    }

    long m_object_index;
    long m_rigid_body_index;
    
    virtual phys_bool should_sleep_high_freq(const phys_entity *entity);
    virtual phys_bool should_sleep_low_freq(const phys_entity *entity);
    void update_in_local_space(const real_vector3d *linear_velocity, const real_vector3d *angular_velocity);
private:
    bool should_override_deactivaion();
};
enum
{
    k_motion_type_keyframed = 6,
    k_motion_type_fixed = 7,
};
enum e_rigid_body_size { _rigid_body_size_default = 0 };
struct hc_spring_definition { long name; float max_force; float minimum_force; float spring, damper, power; };
struct hc_position_definition { long name; float max_force; float minimum_force; float tau, damping, proportional, constant, power; };
namespace {
class c_havok_group_filter {
public:
    static __forceinline unsigned int calcFilterInfo(int layer, int system_group, int subsystem, int dont_collide)
    {
        return (subsystem << 5) | (dont_collide << 10) | (system_group << 16) | layer;
    }
};
}

// Same 12-byte prefix as s_body_in_phantom: update_phantom consumes these
// bodies through that type (see the call below).
struct s_phantom_body {
    signed char material_index;
    signed char rigid_body_index;
    signed char physics_model_material_index;
    long object_index;
    long reference_count;
    long body_object_index;
};
class c_wrapped_shape : public phys_shape {
public:
    unsigned long code;
    const phys_shape *child_shape;
};
struct s_chain_simd { __vector4 value; };
struct s_havok_selected_rigid_shape {
    char index;
    bool alternate_used;
    bool has_alternate;
};
struct s_havok_phantom_activity {
    bool preserve_inactive;
    bool was_active;
};
static __forceinline void *havok_component_allocate_chunk_const_size(int size, phys_memory_class memory_class)
{
    phys_thread_memory *memory = &phys_thread_memory::get_singleton_instance();
    if (g_havok_memory_globals.active_allocator == _memory_allocator_havok)
    {
        phys_memory *allocator = &g_havok_memory;
        return allocator->allocate_chunk(size, memory_class);
    }
    int row = phys_thread_memory::const_size_to_row(size);
    phys_thread_memory::free_block_list::free_elem *element = memory->m_block_free_list[row].m_head;
    if (element)
    {
        --memory->m_block_free_list[row].m_elem_count;
        memory->m_block_free_list[row].m_head = element->m_next;
        return element;
    }
    return memory->refill_free_list(row);
}

extern "C" {
    // global_zero_vector3d

}


   /* 32 bytes */
extern "C" {
 /* tag instances */
}
/* ---- callees (dev-build names in comments; VA = 0x82000000 + rva) ---- */
class phys_thread_memory;
class phys_rotation;
class phys_quat;
class phys_rigid_body;

extern "C" {
                                 /* TlsGetValue */
}
extern "C" {
                                      /* hkThreadMemory TLS slot */
}
extern "C" {
        /* hkThreadMemory::allocateChunk */
}
extern "C" {
/* hkThreadMemory::deallocateChunk */
}
extern "C" {
                      /* hkArrayUtil::_reserveMore */
}
extern "C" {
       /* hkMatrix3_from_matrix3x3 */
}
extern "C" {
                /* hkQuaternion::set(const hkRotation &) */
}
/* ---- Havok ---- */

struct c_havok_memory_pool;
// @address 0xa430e0 g_havok_memory_globals



class phys_vec4;
class phys_constraint_owner;
struct phys_modifier_atom;
/* hkReferencedObject: explicit vtable pointer (the judge cannot map compiler vtables). */
  /* halo_managed_memory_address */
/* halo's removeReference (0x251f00 shape): managed memory keeps one reference, else virtual delete */
class phys_broadphase;
class phys_volume;
class phys_simulation;
class phys_island;

class phys_volume;
class phys_rigid_body_deactivator;
// @address 0x40458 ??_7c_object_rigid_body_deactivator@@6B@
// @address 0x2518f8 ??_Gc_object_rigid_body_deactivator@@UAAPAXI@Z
static __forceinline void havok_world_add_phantom(phys_volume *phantom) { g_havok_world->add_volume(phantom); }
/* hkVector4 / hkSimdReal: VMX through xboxmath (XMVectorSet = float stores + lvx/stvx self copy) */
class phys_transform;
class phys_transform;
static __forceinline void havok_component_copy_transform(phys_transform *destination, const phys_transform &source)
{
    destination->m_rotation.get_column(0).m_simd_quad = source.m_rotation.get_column(0).m_simd_quad;
    destination->m_rotation.get_column(1).m_simd_quad = source.m_rotation.get_column(1).m_simd_quad;
    destination->m_rotation.get_column(2).m_simd_quad = source.m_rotation.get_column(2).m_simd_quad;
    destination->m_translation.m_simd_quad = source.m_translation.m_simd_quad;
}

/* hkutilities hkMassProperties / hkMassElement (hkInertiaTensorComputer.h); the
   out-of-line default ctor COMDAT was emitted from this module (0x2504e8) */
class phys_shape;
/* hkMotion: real virtual class (never constructed here, so no vtable is emitted);
   getInertiaLocal is slot 4 (+16), applyPointImpulse slot 18 (+72). A real virtual call on
   rigid_body->m_motion passes &m_motion as this for virtual calls. */

extern "C" {
             /* hkMotion::getMass */
}
 /* hkRigidBody::setTransform */

class phys_rigid_body_desc;

/* c_entity_listener: the global listener (hkEntityActivationListener at 0, hkCollisionListener at 4) */
class c_entity_listener;

// @address 0x8c425c g_havok_component_data
s_data_array *g_havok_component_data;
extern "C" {
 // @address 0x8c4260 g_global_entity_listener
 // match: typed listener view; havok_memory.cpp initializes this through void *.

}
                  /* the global entity listener */

class phys_entity;

/* c_object_rigid_body_deactivator: 128 bytes, hkSpatialRigidBodyDeactivator base */


/* ---- halo data arrays and tags ---- */
template <typename T> static __forceinline T *tag_block_get_element(const s_tag_block *block, long index) { return (T *)block->address + index; }
  /* object header data (TLS) */
/* Look up an object header and keep it only when its type is in the mask. */
static __forceinline s_object_header *object_header_try_and_get(const s_data_array *array, long index)
{
    long absolute_index = datum_index_to_absolute_index(index);
    if (absolute_index < array->first_unallocated) {
        s_object_header *header = (s_object_header *)((char *)array->data + array->size * absolute_index);
        if (header->identifier != 0 && header->identifier == (index >> 16))
            return header;
    }
    return 0;
}
static __forceinline void *object_try_and_get_and_verify_type(long object_index, unsigned long type_mask)
{
    s_object_header *header = object_header_try_and_get(object_header_data, object_index);
    if (header && ((1 << header->object_type) & type_mask) != 0)
        return header->datum;
    return 0;
}

            /* 'obje': model tag index at 0x40 */

      /* 'hlmt': physics model tag index at 0x3c */

   /* nodes: short node indices */
    /* g_impact_data */
static __forceinline c_impact *impact_get(long impact_index)
{
    return (c_impact *)(tls_impact_data()->data + (unsigned short)impact_index * sizeof(c_impact));
}
/* c_static_flags<N>: bit vector of N bits in longs */



   /* 36 bytes; flags bit 0 rigid */






  /* 28 bytes */
  /* Physics model node edges. */
struct s_physics_model_definition;

extern "C" {
  /* 0x48 */
}
  /* object_constraint_is_loosened */

/* ---- havok_component.obj classes ---- */
/* non-POD (user copy-assignment, as in the dev build): returned through a hidden pointer, passed in a register */
class phys_dynamics_contact_manager;
// hkSolverResults followed by hkContactPointMaterial, then the solver's internal data.


// @address 0xefba8 ?get_rigid_body_is_dynamic@c_havok_component@@QBA_NJ@Z
// c_havok_component::get_rigid_body_is_dynamic (havok_component.cpp)


static __forceinline void set_vector(real_vector3d *v, const float *f)
{
  v->i = f[0];
  v->j = f[1];
  v->k = f[2];
}

// match: 0x254928: the scalar lookup keeps the f19 stack load before the f10 multiply after get_rigid_body_transform.
static inline unsigned __int64 find_property_value(const phys_world_object *world_object, unsigned int key)
{
  for (int i = 0; i < world_object->m_properties.m_size; i++) {
    if (world_object->m_properties.m_data[i].m_key == key) {
      return world_object->m_properties.m_data[i].m_value.m_data;
    }
  }
  return 0;
}

      /* 'hlmt': physics model tag index at 0x3c */
  /* 96 bytes */
                    /* Render model nodes. */

                  /* 'hlmt': render model tag index at 0x0c */

  /* (unsigned short): data loaded before the offset (0x108328) */

   /* nodes: short node indices */




/* hkArray<T>: m_data, m_size, m_capacityAndFlags (12 bytes).
   releaseMemory reads m_capacityAndFlags twice (test, then capacity): 0x250570 needed that.
   ~hkArray, releaseMemory and ~hkInplaceArray are HK_FORCE_INLINE as in Havok: ~c_rigid_body 0x259b00 inlines
   both member arrays' releases are inlined. */

/* hkInplaceArray<T,N>: hkArray whose data starts in the trailing storage (capacity N, DONT_DEALLOCATE) */



   /* nodes: short node indices */

class phys_world_object;
class phys_dynamics_contact_manager;
class c_runtime_node_to_physics_model_rigid_body_index_mapping;




inline c_havok_component::c_havok_component(long object_index) :
        m_flags(0),
        m_object_index(object_index),
        m_object_postion_controlling_rigid_body_index(-1),
        m_object_postion_controlling_node_index(-1),
        m_maximum_rigid_body_size(0),
        m_impact_array_index(-1),
        m_bodies_in_phantom(0),
        m_simple_shape_phantom(0),
        m_aabb_phantom(0)
    {
        new (&m_rigid_bodies) phys_inline_array<c_rigid_body, 1>;
    }


// This owner routes the phantom array through the game's explicit allocator.
template <> __forceinline void *phys_array<c_havok_component::s_body_in_phantom>::operator new(size_t size)
{
    return havok_component_allocate_chunk_const_size(size, PHYS_MEM_ARRAY);
}

             /* g_havok_component_data (144-byte components) */



// havok_component_new: a datum in g_havok_component_data, constructed in place; returns its index.
long havok_component_new(long object_index)
{
    s_data_array *data = ((s_data_array *)g_havok_component_data);
    long havok_component_index = datum_new((s_data_array *)(data));
    c_havok_component *component = havok_component_get(havok_component_index);
    if (component)
        new (component) c_havok_component(object_index);
    return havok_component_index;
}

long c_havok_component::calculate_rigid_body_index_from_node_index(long node_index)
{
    object_datum *object = (object_datum *)object_get_and_verify_type(get_object_index(), 0xffffffff);
    s_object_definition *definition = (s_object_definition *)tag_get('obje', object->definition_index);
    s_model_definition *model = (s_model_definition *)tag_get('hlmt', definition->model.index);
    s_physics_model_definition *physics_model = (s_physics_model_definition *)tag_get('phmo', model->physics_model_index);
    long physics_model_rigid_body_index = -1;
    if (node_index < physics_model->nodes.count) {
        long current_node_index = node_index;
        do {
            s_physics_model_node *node = tag_block_get_element<s_physics_model_node>(&physics_model->nodes, current_node_index);
            for (long index = 0; index < physics_model->rigid_bodies.count(); index++) {
                if (tag_block_get_element<s_physics_model_rigid_body>(&physics_model->rigid_bodies, index)->node == current_node_index) {
                    physics_model_rigid_body_index = index;
                    break;
                }
            }
            current_node_index = node->parent_node_index;
        } while (current_node_index != -1 && physics_model_rigid_body_index == -1);
    }
    if (physics_model_rigid_body_index == -1) {
        for (long index = 0; index < physics_model->rigid_bodies.count(); index++) {
            if (tag_block_get_element<s_physics_model_rigid_body>(&physics_model->rigid_bodies, index)->node == -1) {
                physics_model_rigid_body_index = index;
                break;
            }
        }
    }
    return find_rigid_body_index_from_physics_model_rigid_body_index(physics_model_rigid_body_index);
}

void c_havok_component::get_rigid_body_linear_velocity(long index, real_vector3d *result)
{
    phys_rigid_body *body = m_rigid_bodies[index].m_rigid_body;
    if (body->get_motion_type() != k_motion_type_fixed) {
        const phys_vec4 &velocity = body->get_linear_velocity();
        set_real_vector3d(result, velocity(0), velocity(1), velocity(2));
    } else {
        *result = *global_zero_vector3d;
    }
}

void c_havok_component::get_rigid_body_center_of_mass(long marker_index, union real_point3d * position)
{
    bool has_nodes = (m_flags >> 8) & 1;
    if (has_nodes) {
        c_rigid_body *marker = &m_rigid_bodies[marker_index];
        int node_index;
        if (marker->m_rigid_body->get_motion_type() == 7 && marker->m_physics_model_rigid_body_indices.getSize() > 0
            && (node_index = marker->m_physics_model_rigid_body_indices[0]) != -1) {
            object_datum *object = object_get(m_object_index);
            s_object_definition *definition = (s_object_definition *)tag_get('obje', object->definition_index);
            s_model_definition *model = (s_model_definition *)tag_get('hlmt', definition->model.index);
            s_physics_model_definition *physics_model = (s_physics_model_definition *)tag_get('phmo', model->physics_model_index);
            phys_vec4 result;
            result.vec_set_transformed_pos(marker->m_rigid_body->get_transform(),
                    ((s_physics_model_rigid_body *)physics_model->rigid_bodies.address)[node_index].center_of_mass);
            position->x = result(0);
            position->y = result(1);
            position->z = result(2);
            return;
        }
    }
    const phys_vec4 &center = m_rigid_bodies[marker_index].m_rigid_body->get_center_of_mass_in_world();
    float z = center(2);
    float y = center(1);
    float x = center(0);
    position->x = x;
    position->y = y;
    position->z = z;
}

/* c_havok_component::set_rigid_body_linear_velocity */
void c_havok_component::set_rigid_body_linear_velocity(long index, const real_vector3d *velocity)
{
    phys_rigid_body *body = m_rigid_bodies[index].m_rigid_body;
    if (body->get_motion_type() != 7) {
        phys_vec4 v;
        v.m_simd_quad = XMVectorSet(velocity->n[0], velocity->n[1], velocity->n[2], 0.0f);
        body->set_linear_velocity(v);
    }
}

/* c_havok_component::update_rigid_body_pre_simulation_state */
void c_havok_component::update_rigid_body_pre_simulation_state(long rigid_body_index)
{
    c_havok_component::c_rigid_body *rigid_body = &m_rigid_bodies[rigid_body_index];
    real_point3d position;
    real_vector3d linear_velocity;
    real_vector3d angular_velocity;
    get_rigid_body_position(rigid_body_index, &position);
    get_rigid_body_linear_velocity( rigid_body_index, &linear_velocity);
    get_rigid_body_angular_velocity(rigid_body_index, &angular_velocity);
    rigid_body->set_pre_simulation_state(&position, &linear_velocity, &angular_velocity);
}

// The angular counterpart of get_rigid_body_linear_velocity.
void c_havok_component::get_rigid_body_angular_velocity(long index, real_vector3d *result)
{
    phys_rigid_body *body = m_rigid_bodies[index].m_rigid_body;
    if (body->get_motion_type() != k_motion_type_fixed) {
        const phys_vec4 &velocity = body->get_angular_velocity();
        set_real_vector3d(result, velocity(0), velocity(1), velocity(2));
    } else {
        *result = *global_zero_vector3d;
    }
}

/* c_havok_component::set_rigid_body_angular_velocity; the entry flags byte sits at +0x44 */
void c_havok_component::set_rigid_body_angular_velocity(long index, const real_vector3d *velocity)
{
    const float *p = velocity->n;
    if (!(m_rigid_bodies[index].m_flags & 2) && m_rigid_bodies[index].m_rigid_body->get_motion_type() != 7) {
        phys_vec4 v;
        v.set(p[0], p[1], p[2], 0.0f);
        m_rigid_bodies[index].m_rigid_body->set_angular_velocity(v);
    }
}

bool c_havok_component::get_rigid_body_is_dynamic(long rigid_body_index) const
{
    return !get_rigid_body_is_fixed(rigid_body_index) && !get_rigid_body_is_keyframed(rigid_body_index);
}

void c_havok_component::get_rigid_body_transform(long node_index, real_matrix4x3 *result)
{
  const phys_transform &source = m_rigid_bodies[node_index].m_rigid_body->get_transform();
  union { __vector4 rows[4]; float f[4][4]; } m;
  m.rows[0] = source.m_rotation.get_column(0).m_simd_quad; m.rows[1] = source.m_rotation.get_column(1).m_simd_quad;
  m.rows[2] = source.m_rotation.get_column(2).m_simd_quad; m.rows[3] = source.m_translation.m_simd_quad;
  result->scale = k_real_one;
  real_matrix3x3 *matrix = &result->rotation;
  set_vector(&matrix->forward, m.f[0]);
  set_vector(&matrix->left, m.f[1]);
  set_vector(&matrix->up, m.f[2]);
  result->position.x = m.f[3][0];
  result->position.y = m.f[3][1];
  result->position.z = m.f[3][2];
}

float c_havok_component::get_rigid_body_mass(long rigid_body_index)
{
    phys_motion *motion = &m_rigid_bodies[rigid_body_index].m_rigid_body->m_motion;
    if (motion->get_mass() > 1.0f)
        return motion->get_mass();
    return 1.0f;
}

long havok_entity_get_havok_component_index(const phys_world_object *world_object)
{
  int value = world_object->get_world_object_property(0x2001).getInt();
  if (value == 0) {
    return -1;
  }
  return value;
}

long havok_entity_get_havok_component_rigid_body_index(const phys_world_object *world_object)
{
  return world_object->get_world_object_property(0x2002).getInt();
}

long havok_entity_get_structure_bsp_index(const phys_world_object *world_object)
{
  int result = -1;
  if (world_object->has_property(0x2004))
    result = world_object->get_world_object_property(0x2004).getInt();
  return result;
}

// @address 0xa9548 ?vehicle_get_type@@YA?AW4e_vehicle_type@@J@Z
namespace {

}
// Virtual order: hkReferencedObject, hkContactMgr, hkDynamicsContactMgr.
class phys_statistics_collector;
class phys_constraint_owner;
class phys_contact_point_material;
struct phys_process_collision_input;
struct phys_process_collision_output;
struct phys_simplex_cache;
enum phys_result;
static __forceinline real_vector3d *havok_component_contact_scale_vector3d(const real_vector3d *vector, float scale, real_vector3d *result)
{
    result->i = vector->i * scale;
    result->j = vector->j * scale;
    result->k = vector->k * scale;
    return result;
}
// @address 0x1050d0 ?update@c_havok_contact_point@@QAAX_NPBVphys_vec4@@1PAVphys_dynamics_contact_manager@@PBVphys_shape@@0JJ@Z
void c_havok_contact_point::update(bool unused, const phys_vec4 *position, const phys_vec4 *normal, phys_dynamics_contact_manager *contact_manager,
    const phys_shape *shape, bool reversed, long component_index, long rigid_body_index)
{
    phys_contact_point *contact_point = contact_manager->get_process_contact_point(m_contact_point_id);
    phys_contact_point_props *properties = contact_manager->get_contact_point_props(m_contact_point_id);
    m_distance = contact_point->get_distance();
    set_real_point3d(&m_position, (*position)(0), (*position)(1), (*position)(2));
    set_real_vector3d(&m_normal, (*normal)(0), (*normal)(1), (*normal)(2));
    if (reversed)
        havok_component_contact_scale_vector3d(&m_normal, -1.0f, &m_normal);
    if (!reversed)
        update_contact_point_properties(properties, component_index);
}
// @address 0x1051b8 ?update_contact_point_properties@c_havok_contact_point@@QAAXPAVphys_contact_point_props@@J@Z
void c_havok_contact_point::update_contact_point_properties(phys_contact_point_props *properties, long component_index)
{
    long object_index = havok_component_get(component_index)->get_object_index();
    bool override_friction = false;
    float friction;
    s_object_header *header = &((s_object_header *)object_header_data->data)[(unsigned short)object_index];
    if (((1 << header->object_type) & 2) != 0) {
        vehicle_datum *vehicle = (vehicle_datum *)header->datum;
        vehicle_definition *definition = (vehicle_definition *)tag_get('vehi', vehicle->object.definition_index);
        if ((bool)(definition->flags & 1)) {
            override_friction = vehicle->unit.driver_object_index != -1 || (bool)((vehicle->unit.unit_flags >> 1) & 1);
            friction = 0.0f;
        }
        if (vehicle_get_type(object_index) == 6) {
            override_friction = false;
            friction = 1.0f;
        }
    }
    if ((m_flags & 64) != 0) {
        override_friction = true;
        friction = 0.0f;
    }
    properties->m_contact_friction = (unsigned short)(int)((override_friction ? friction : m_contact_friction) * 256.0f);
    properties->m_contact_restitution = (unsigned char)(int)(m_contact_restitution * 128.0f);
}

extern "C" void impact_initialize_contact_description(c_impact::s_contact_description *description, long type, bool use_marker,
    long object_index_a, long node_index_a,
    c_global_material_type material_a, long object_index_b, long node_index_b, c_global_material_type material_b,
    const real_point3d *position, const real_vector3d *normal, long vehicle_friction_point_index,
    const s_physics_model_constraint_reference *constraint_reference)
{
  description->use_marker = use_marker;
  description->type = (unsigned char)type;
  description->padding_after_type = 0;
  description->object_a = object_index_a;
  description->node_index_a = node_index_a;
  description->material_a = material_a;
  description->object_b = object_index_b;
  description->node_index_b = node_index_b;
  description->material_b = material_b;
  description->position = *position;
  description->normal = *normal;
  description->vehicle_friction_point_index = vehicle_friction_point_index;
  description->disconnected_from_physics = 0;
  description->padding_after_disconnected_byte = 0;
  description->padding_after_disconnected_word = 0;
  if (constraint_reference == 0) {
    description->constraint_reference.type = -1;
    description->constraint_reference.index = -1;
  } else {
    description->constraint_reference = *constraint_reference;
  }
}

static __forceinline long havok_component_impact_array_impact_get(long array_index, long index)
{
    return ((s_impact_array *)tls_impact_array_data()->data)[datum_index_to_absolute_index(array_index)].impact_indices[index];
}
static __forceinline c_impact *havok_component_impact_datum_get(long impact_index)
{
    return &((c_impact *)tls_impact_data()->data)[datum_index_to_absolute_index(impact_index)];
}
// @address 0x107118 ?find_impact_which_includes_contact@c_havok_component@@QAAJPBUs_contact_description@c_impact@@_N@Z
long c_havok_component::find_impact_which_includes_contact(const c_impact::s_contact_description *contact_description, bool test_space)
{
    long impact_datum_index = -1;
    for (long index = 0; index < get_impact_count(); index++) {
        long impact_index = havok_component_impact_array_impact_get(m_impact_array_index, index);
        c_impact *impact = havok_component_impact_datum_get(impact_index);
        if (impact->includes_point(contact_description, test_space)) {
            impact_datum_index = impact_index;
            break;
        }
    }
    return impact_datum_index;
}


// 0x10c1d0 (no record)











// BEGIN-REALNAMES
// END-REALNAMES
static __forceinline real_plane3d *havok_component_bsp3d_get_plane_from_designator(const collision_bsp *bsp, long plane_designator, real_plane3d *plane)
{
  const real_plane3d *bsp_plane = &((const real_plane3d *)bsp->planes.address)[plane_designator & 0x7fff];
  if (plane_designator & 0x8000) {
    plane->normal.i = -bsp_plane->normal.i;
    plane->normal.j = -bsp_plane->normal.j;
    plane->normal.k = -bsp_plane->normal.k;
    plane->distance = -bsp_plane->distance;
  } else {
    *plane = *bsp_plane;
  }
  return plane;
}
static __forceinline void havok_component_get_instanced_surface_normal(
    long structure_bsp_index, long instance_index, long surface_index, real_plane3d *plane, real_vector3d *surface_normal)
{
  const structure_bsp *bsp_definition = (const structure_bsp *)tag_get('sbsp',
      ((const scenario_structure_bsp_reference *)global_scenario->structure_bsps.address)[structure_bsp_index].structure_bsp.index);
  const structure_instanced_geometry_instance *instance =
      &((const structure_instanced_geometry_instance *)bsp_definition->instanced_geometry_instances.address)[instance_index];
  const structure_instanced_geometry_definition *definition =
      &((const structure_instanced_geometry_definition *)g_collision_bsp_cache.users[g_registered_thread_index]
          .entries[structure_bsp_index]
          .data)[instance->definition_index];
  const collision_surface *surface = tag_block_get_element<collision_surface>(&definition->bsp.surfaces, surface_index);
  havok_component_bsp3d_get_plane_from_designator(&definition->bsp, surface->plane, plane);
  matrix3x3_transform_vector(&instance->transform.rotation, &plane->normal, &plane->normal);
  *surface_normal = plane->normal;
}
// match: 0x10c1d0: __restrict issues the m_normal j/k loads before the stores and keeps the shape-key load/cmpwi 0xffff ahead of them
void c_havok_contact_point::get_surface_normal(real_vector3d *__restrict surface_normal) const
{
  *surface_normal = m_normal;
  if (m_environment_shape_key != 0xffffffff) {
    long structure_bsp_index = m_structure_bsp_index;
    long index;
    long surface_index;
    long type;
    real_plane3d plane;
    values_from_shape_key(&index, &surface_index, &type, m_environment_shape_key);
    switch (type) {
    case 1: {
#ifdef HALO3_64BIT
      const collision_bsp *bsp = g_collision_bsp_cache.thread_users[g_registered_thread_index].collision_bsps[structure_bsp_index];
#else
      const collision_bsp *bsp = ((const collision_bsp **)&g_collision_bsp_cache)[g_registered_thread_index * 64 + structure_bsp_index];
#endif
      real_plane3d bsp_plane;
      const collision_surface *surface = tag_block_get_element<collision_surface>(&bsp->surfaces, index);
      havok_component_bsp3d_get_plane_from_designator(bsp, surface->plane, &bsp_plane);
      *surface_normal = bsp_plane.normal;
      break;
    }
    case 2:
      havok_component_get_instanced_surface_normal(structure_bsp_index, index, m_surface_index, &plane, surface_normal);
      break;
    case 3:
      havok_component_get_instanced_surface_normal(structure_bsp_index, index, surface_index, &plane, surface_normal);
      break;
    }
  }
}


static __forceinline c_global_material_type havok_component_structure_bsp_global_material_type(
    long structure_bsp_index, const real_point3d *position, long surface_index)
{
#ifdef HALO3_64BIT
  const collision_bsp *bsp = g_collision_bsp_cache.thread_users[g_registered_thread_index].collision_bsps[structure_bsp_index];
#else
  const collision_bsp *bsp = ((const collision_bsp **)&g_collision_bsp_cache)[g_registered_thread_index * 64 + structure_bsp_index];
#endif
  return collision_get_surface_material(structure_bsp_index, bsp, position, surface_index, -1);
}
// 0x106f78
c_global_material_type c_havok_contact_point::calculate_global_material_type_b() const
{
  c_global_material_type material = m_global_material_type_b;
  if (m_environment_shape_key != 0xffffffff) {
    long index;
    long surface_index;
    long type;
    values_from_shape_key(&index, &surface_index, &type, m_environment_shape_key);
    switch (type) {
    case 1:
      material = havok_component_structure_bsp_global_material_type(m_structure_bsp_index, &m_position, index);
      break;
    case 3:
      material = collision_get_bsp_surface_material(m_structure_bsp_index, &m_position, surface_index, index);
      break;
    }
  }
  material = global_material_resolve_underwater_material_proxy(material, &m_position);
  return material;
}

void c_havok_component::get_rigid_body_linear_velocity_at_point(long index, union real_vector3d * result, union real_point3d const * point)
{
  phys_rigid_body *body = m_rigid_bodies[index].m_rigid_body;
  if (body->get_motion_type() != 7) {
    phys_motion *state = &body->m_motion;
    __vector4 p;
    p.x = point->x;
    p.y = point->y;
    p.z = point->z;
    p.w = k_real_zero_storage;
    const __vector4 *w = &state->get_angular_velocity().m_simd_quad;
    __vector4 d = __vsubfp(p, state->get_center_of_mass_in_world().m_simd_quad);
    __vector4 a_zxy = __vpermwi(*w, 0x87);
    __vector4 a_yzx = __vpermwi(*w, 0x63);
    __vector4 d_yzx = __vpermwi(d, 0x63);
    __vector4 d_zxy = __vpermwi(d, 0x87);
    __vector4 r = __vsubfp(__vmulfp(a_yzx, d_zxy), __vmulfp(a_zxy, d_yzx));
    r = __vaddfp(r, state->m_linear_velocity.m_simd_quad);
    float *rv = r.v;
    result->i = rv[0];
    result->j = rv[1];
    result->k = rv[2];
  } else {
    *result = *global_zero_vector3d;
  }
}

// @address 0x1c1260 ?set@phys_simd_scalar@@QAAXABM@Z




#ifdef HALO3_64BIT
__attribute__((used))
#endif
inline void phys_aabb_util::compute_bounds(const phys_transform &local_to_world, const phys_vec4 &box_half_extents, const phys_vec4 &center,
    float extra_radius, phys_aabb &aabb_out)
{
    phys_vec4 hx, hy, hz;
    hx.set_broadcast(box_half_extents, 0);
    hy.set_broadcast(box_half_extents, 1);
    hz.set_broadcast(box_half_extents, 2);
    phys_vec4 ex;
    ex.m_simd_quad = __vmulfp(hx.m_simd_quad, local_to_world.m_rotation.get_column(0).m_simd_quad);
    phys_vec4 ey;
    ey.m_simd_quad = __vmulfp(hy.m_simd_quad, local_to_world.m_rotation.get_column(1).m_simd_quad);
    phys_vec4 ez;
    ez.m_simd_quad = __vmulfp(hz.m_simd_quad, local_to_world.m_rotation.get_column(2).m_simd_quad);
    ex.set_abs4(ex);
    ey.set_abs4(ey);
    ez.set_abs4(ez);
    phys_vec4 margin;
    margin.set_all3(extra_radius);
    ez.add4(margin);
    phys_vec4 extent;
    extent.set_add4(ex, ey);
    extent.add4(ez);
    phys_vec4 negative;
    negative.set_neg4(extent);
    phys_vec4 transformed;
    transformed.set_transformed_pos(local_to_world, center);
    extent.add4(transformed);
    negative.add4(transformed);
    aabb_out.m_max_limit = extent;
    aabb_out.m_min_limit = negative;
}

long c_havok_component::get_body_in_phantom_count() const
{
    if (m_bodies_in_phantom == 0)
        return 0;
    return m_bodies_in_phantom->getSize();
}

c_havok_component::c_rigid_body::~c_rigid_body()
{
    m_rigid_body = 0;
}

/* entity_belongs_in_phantom */
bool entity_belongs_in_phantom(long havok_component_index, long body_havok_component_index, long body_rigid_body_index, long phantom_index)
{
    bool result = false;
    if (body_havok_component_index != -1) {
        c_havok_component *component = havok_component_get(havok_component_index);
        object_datum *object = (object_datum *)object_get_and_verify_type(component->get_object_index(), 0xffffffff);
        s_object_definition *definition = (s_object_definition *)tag_get('obje', object->definition_index);
        s_model_definition *model = (s_model_definition *)tag_get('hlmt', definition->model.index);
        s_physics_model_definition *physics_model = (s_physics_model_definition *)tag_get('phmo', model->physics_model_index);
        s_physics_model_phantom *phantom = tag_block_get_element<s_physics_model_phantom>(&physics_model->phantoms, phantom_index);
        s_physics_model_phantom_type *phantom_type = tag_block_get_element<s_physics_model_phantom_type>(
            &physics_model->phantom_types, phantom->phantom_type_index);
        c_havok_component *body_component = havok_component_get(body_havok_component_index);
        s_object_header *header = object_header_get(body_component->get_object_index());
        long size = body_component->get_maximum_rigid_body_size();
        if ((size == 0 || (size >= phantom_type->minimum_rigid_body_size && size <= phantom_type->maximum_rigid_body_size)) &&
            !(phantom_type->flags & (1 << (header->object_type + 5))))
            result = true;
    }
    return result;
}

static __forceinline void append_constraint(phys_array<c_havok_component::c_constraint> *constraints, const c_havok_component::c_constraint &value)
{
    if (constraints->m_size == constraints->get_sector_capacity())
        phys_array_util::grow_array_storage((void *)(constraints), sizeof(c_havok_component::c_constraint));
    c_havok_component::c_constraint *destination = &constraints->m_data[constraints->m_size++];
    if (&value != destination)
        memcpy(destination, &value, sizeof(c_havok_component::c_constraint));
}
// @address 0x2510d0 ?add_constraint@c_havok_component@@QAAXPAVphys_constraint@@JJ_N@Z
void c_havok_component::add_constraint(phys_constraint *instance, long type, long index, bool flag)
{
    c_constraint value(instance, (e_physics_model_constraint_type)type, index, flag);
    append_constraint(&m_component_constraints, value);
    value.m_contact_constraint = 0;
}

/* ---- records 0x2523e8, 0x253c90, 0x253e90 (their own structs; the globals keep the types the unit declares) ---- */
static __forceinline void *havok_component_tag_block_element(const s_tag_block *block, long index, long size) { return (char *)block->address + index * size; }
static __forceinline bool havok_component_phantom_test(const s_physics_model_phantom_type *phantom, long bit) { return (phantom->flags & (1UL << bit)) != 0; }
static __forceinline bool havok_component_object_damaged(const object_datum *object, long bit) { return (object->damage_flags & (1UL << bit)) != 0; }



// @address 0x251458 ?rigid_body_apply_acceleration_at_point@c_havok_component@@QAAXJPBTreal_point3d@@PBTreal_vector3d@@@Z
void c_havok_component::rigid_body_apply_acceleration_at_point(long rigid_body_index, const real_point3d *point, const real_vector3d *acceleration)
{
    if (get_rigid_body_has_infinite_inertia_tensor(rigid_body_index)) {
        rigid_body_apply_acceleration(rigid_body_index, acceleration);
    } else {
        phys_rigid_body *rigid_body = havok_component_get_rigid_body(rigid_body_index);
        phys_vec4 havok_point;
        phys_vec4 havok_acceleration;
        phys_vec4_from_real_point3d(&havok_point, point);
        phys_vec4_from_real_vector3d(&havok_acceleration, acceleration);
        havok_acceleration.mul4(phys_simd_scalar(rigid_body->get_mass()));
        rigid_body->get_rigid_motion()->apply_point_impulse(havok_acceleration, havok_point);
    }
}

// 0x251820
static __forceinline const s_havok_constants *get_havok_constants() { return &g_havok_constants; }
static __forceinline bool vehicle_should_override_deactivation(long object_index)
{
    vehicle_datum *vehicle = (vehicle_datum *)object_get_and_verify_type(object_index, 2);
    return ((c_vehicle_type_component *)vehicle->vehicle.type_state)->should_override_deactivation(object_index);
}
static __forceinline bool biped_should_override_deactivation(long object_index)
{
    biped_datum *biped = (biped_datum *)object_get_and_verify_type(object_index, 1);
    bool should_override = false;
    if (biped->biped.physics.m_mode == 1)
        should_override = motor_system_in_airborne_state(object_index);
    return should_override;
}
// @address 0x251568 ?should_sleep_high_freq@c_object_rigid_body_deactivator@@UAA?AVphys_bool@@PBVphys_entity@@@Z
phys_bool c_object_rigid_body_deactivator::should_sleep_high_freq(const phys_entity *entity)
{
    return should_override_deactivaion() ? phys_bool(false) : phys_spatial_body_deactivator::should_sleep_high_freq(entity);
}

// @address 0x2515d8 ?should_sleep_low_freq@c_object_rigid_body_deactivator@@UAA?AVphys_bool@@PBVphys_entity@@@Z
phys_bool c_object_rigid_body_deactivator::should_sleep_low_freq(const phys_entity *entity)
{
    return should_override_deactivaion() ? phys_bool(false) : phys_spatial_body_deactivator::should_sleep_low_freq(entity);
}

// @address 0x251820 ?should_override_deactivaion@c_object_rigid_body_deactivator@@AAA_NXZ
bool c_object_rigid_body_deactivator::should_override_deactivaion()
{
    bool should_override = false;
    switch (object_header_get(m_object_index)->object_type) {
    case 0:
        if (motor_system_is_relaxed(m_object_index) != 1)
            should_override = biped_should_override_deactivation(m_object_index);
        break;
    case 1:
        should_override = vehicle_should_override_deactivation(m_object_index);
        break;
    default:
        should_override = false;
        break;
    }
    return get_havok_constants()->disable_deactivation || should_override;
}

// @address 0x2523e8 entity_should_update_in_phantom
extern "C" bool entity_should_update_in_phantom(long component_index, long entity_index, long shape_index)
{
    bool result = false;
    c_havok_component *source = havok_component_get(component_index);
    object_datum *source_object = object_get(source->m_object_index);
    s_object_definition *definition = (s_object_definition *)tag_get('obje', source_object->definition_index);
    s_model_definition *model = (s_model_definition *)tag_get('hlmt', definition->model.index);
    s_physics_model_definition *physics = (s_physics_model_definition *)tag_get('phmo', model->physics_model_index);
    s_physics_model_phantom *s = (s_physics_model_phantom *)havok_component_tag_block_element(&physics->phantoms, shape_index, sizeof(s_physics_model_phantom));
    s_physics_model_phantom_type *p = (s_physics_model_phantom_type *)havok_component_tag_block_element(&physics->phantom_types,
        s->phantom_type_index, sizeof(s_physics_model_phantom_type));
    c_havok_component *entity = havok_component_get(entity_index);
    s_object_header *h = object_header_get(entity->m_object_index);
    object_datum *o = h->datum;
    bool attached;
    if (((1 << h->object_type) & 4099) != 0
        && ((unit_datum *)object_get(entity->m_object_index))->unit.player_index != -1)
        attached = true;
    else
        attached = false;
    bool dead;
    if (((1 << h->object_type) & 1) != 0 && havok_component_object_damaged(o, 2))
        dead = true;
    else
        dead = false;
    if ((havok_component_phantom_test(p, 3) && attached) || (havok_component_phantom_test(p, 4) && !attached) || (havok_component_phantom_test(p, 27) && dead))
        result = false;
    else
        result = true;
    return result;
}





typedef char havok_component_spring_motor_size[(sizeof(void *) != 4 || sizeof(phys_spring_damper_motor) == 28) ? 1 : -1];
typedef char havok_component_position_motor_size[(sizeof(void *) != 4 || sizeof(phys_position_motor) == 36) ? 1 : -1];

// @address 0x253c90 scale_constraint_motor
extern "C" void scale_constraint_motor(const s_physics_model_definition *physics_model,
    s_physics_model_motor_reference motor_reference, phys_limited_force_motor *motor, float scale)
{
    float maximum_force;
    float minimum_force;
    if (motor_reference.motor_type != 1) {
        if (motor_reference.motor_type != 2)
            return;
        const hc_position_definition *definition = tag_block_get_element<hc_position_definition>(
            &physics_model->position_motors, motor_reference.motor_index);
        maximum_force = definition->max_force;
        minimum_force = definition->minimum_force;
    } else {
        const hc_spring_definition *definition = tag_block_get_element<hc_spring_definition>(
            &physics_model->spring_motors, motor_reference.motor_index);
        maximum_force = definition->max_force;
        minimum_force = definition->minimum_force;
    }
    float force = (maximum_force - minimum_force) * scale + minimum_force;
    motor->m_motor_min_force = -force;
    motor->m_motor_max_force = force;
}

static __forceinline void *havok_component_motor_allocate(unsigned int size)
{
    void *thread = TlsGetValue(phys_thread_memory::s_threadMemoryInstance.m_slot);
    void *memory = ((phys_thread_memory *)thread)->allocate_chunk(size, PHYS_MEM_CONSTRAINT);
    ((phys_motor *)memory)->m_mem_size_and_flags = (unsigned short)size;
    return memory;
}


// @address 0x253e90 ?build_motor_from_motor_reference@@YAPAVphys_motor@@PBUs_physics_model_instance@@PBUs_physics_model_motor_reference@@PAM@Z
phys_motor *build_motor_from_motor_reference(
    // match: 0x253e90: pointer __restrict qualifier prevents alias reload
    const s_physics_model_instance *instance, const s_physics_model_motor_reference *reference, float *__restrict power)
{
    phys_motor *result = 0;
    const float zero = k_real_zero_storage;
    *power = zero;
    if (reference->motor_index != -1) {
        switch (reference->motor_type) {
        case 0:
            result = 0;
            break;
        case 1: {
            const hc_spring_definition *definition = (const hc_spring_definition *)havok_component_tag_block_element(
                &instance->physics_model->spring_motors, reference->motor_index,
                sizeof(hc_spring_definition));
            phys_spring_damper_motor *spring = new (havok_component_motor_allocate(sizeof(phys_spring_damper_motor)))
                phys_spring_damper_motor(definition->spring, definition->damper, definition->max_force);
            result = spring;
            *power = definition->power;
            break;
        }
        case 2: {
            const hc_position_definition *definition = (const hc_position_definition *)havok_component_tag_block_element(
                &instance->physics_model->position_motors, reference->motor_index,
                sizeof(hc_position_definition));
            phys_position_motor *position = new (havok_component_motor_allocate(sizeof(phys_position_motor))) phys_position_motor(zero);
            position->m_motor_max_force = definition->max_force;
            position->m_tau_scale = definition->tau;
            position->m_motor_damping = definition->damping;
            position->m_proportional_recovery_vel = definition->proportional;
            position->m_const_recovery_vel = definition->constant;
            result = position;
            *power = definition->power;
            break;
        }
        default:
            // match: 0x253e90: unreachable hint omits dead branch check
            __assume(0);
            break;
        }
    }
    return result;
}







// @address 0x623088 ??0phys_sphere_shape@@QAA@M@Z
// @address 0x64aca0 ??0phys_simple_shape_volume@@QAA@PBVphys_shape@@ABVphys_transform@@I@Z
namespace {

static __forceinline bool havok_component_projectile_effected_by_phantom_volumes(long definition_index)
{
    projectile_definition *definition = (projectile_definition *)tag_get('proj', definition_index);
    return ((definition->targeting_flags >> 10) & 1) != 0;
}
}


// @address 0x250980 ?havok_entity_set_havok_component_index@@YAXPAVphys_world_object@@J@Z
void havok_entity_set_havok_component_index(phys_world_object *world_object, long havok_component_index)
{
    phys_property_value value = phys_property_value((int)havok_component_index);
    if (world_object->has_property(0x2001)) {
        phys_property_value removed = world_object->remove_object_property(0x2001);
    }
    world_object->add_property(0x2001, value);
}

// @address 0x250a20 ?havok_entity_set_structure_bsp_index@@YAXPAVphys_world_object@@J@Z
void havok_entity_set_structure_bsp_index(phys_world_object *world_object, long structure_bsp_index)
{
    phys_property_value value = phys_property_value((int)structure_bsp_index);
    if (world_object->has_property(0x2004)) {
        phys_property_value removed = world_object->remove_object_property(0x2004);
    }
    world_object->add_property(0x2004, value);
}

// @address 0x250ac0 ?havok_entity_set_havok_component_rigid_body_index@@YAXPAVphys_world_object@@J@Z
void havok_entity_set_havok_component_rigid_body_index(phys_world_object *world_object, long rigid_body_index)
{
    phys_property_value value = phys_property_value((int)rigid_body_index);
    if (world_object->has_property(0x2002)) {
        phys_property_value removed = world_object->remove_object_property(0x2002);
    }
    world_object->add_property(0x2002, value);
}

// @address 0x250fd0 ?add_rigid_body@c_havok_component@@AAAXPAVphys_rigid_body@@PBDJ_N22@Z
void c_havok_component::add_rigid_body(
    phys_rigid_body *havok_rigid_body, const char *name, long physics_model_rigid_body_index, bool plays_impact_effects,
    bool infinite_inertia_tensor, bool does_not_collide_with_environment)
{
    object_datum *object = (object_datum *)object_get_and_verify_type(get_object_index(), 0xffffffff);
    long rigid_body_index = m_rigid_bodies.getSize();
    new (&m_rigid_bodies.expand_one()) c_rigid_body(havok_rigid_body, name, physics_model_rigid_body_index,
        plays_impact_effects, infinite_inertia_tensor, does_not_collide_with_environment);
    update_rigid_body_pre_simulation_state(rigid_body_index);
    havok_entity_set_havok_component_index((phys_world_object *)havok_rigid_body, object->havok_component_index);
    havok_entity_set_havok_component_rigid_body_index((phys_world_object *)havok_rigid_body, rigid_body_index);
    if (havok_rigid_body->get_motion_type() != k_motion_type_fixed && havok_rigid_body->get_motion_type() != k_motion_type_keyframed)
        m_flags |= 1024;
}


static __forceinline void havok_component_set_cross(phys_vec4 *result, const phys_vec4 &a, const phys_vec4 &b)
{
    result->m_simd_quad = __vsubfp(__vmulfp(__vpermwi(a.m_simd_quad, 99), __vpermwi(b.m_simd_quad, 135)),
        __vmulfp(__vpermwi(a.m_simd_quad, 135), __vpermwi(b.m_simd_quad, 99)));
}
static __forceinline float havok_component_dot(const phys_vec4 &a, const phys_vec4 &b)
{
    float result;
    __stvewx(__vmsum3fp(a.m_simd_quad, b.m_simd_quad), &result, 0);
    return result;
}
static __forceinline float havok_component_inverse_mass(const phys_rigid_body *body)
{
    float result;
    __stvewx(__vspltw(body->m_motion.m_inv_inertia_mass.m_simd_quad, 3), &result, 0);
    return result;
}

static __forceinline phys_rigid_body *get_havok_world_fixed_rigid_body() { return g_havok_world->get_fixed_rigid_body(); }

/* the averaged contact normal falls back to up when the contacts cancel out */
static __forceinline void havok_component_normalize_contact_normal(real_vector3d *normal) { normalize3d_with_default(normal, global_up3d); }
// @rdata 0x82033b90
static const float k_havok_component_maximum_angular_velocity = 50.0f;
// @rdata 0x82033b94
static const float k_impulse_denominator_epsilon = 0.00001f;

// @address 0x33b98 ?k_invalid_powered_chain_link_reference@@3Us_powered_chain_link_reference@c_havok_component@@B
// @rdata 0x82033b98..0x82033b9c
const c_havok_component::s_powered_chain_link_reference k_invalid_powered_chain_link_reference = { -1, -1 };
// @rdata-end

// @address 0x109290 ?calculate_impulse@c_havok_component@@QAA_NJJJPAMPATreal_point3d@@PATreal_vector3d@@@Z
bool c_havok_component::calculate_impulse(long rigid_body_index, long other_object_index, long other_rigid_body_index, float *impulse,
    real_point3d *position, real_vector3d *normal)
{
    c_havok_component *other_component;
    if (other_object_index != -1) {
        object_datum *other_object = (object_datum *)object_get_and_verify_type(other_object_index, 0xffffffff);
        if (other_object && other_object->havok_component_index != -1)
            other_component = havok_component_get(other_object->havok_component_index);
        else
            other_component = 0;
    } else {
        other_component = 0;
    }
    long contact_point_count = 0;
    *position = *global_origin3d;
    *normal = *global_zero_vector3d;
    for (c_havok_contact_point_iterator iterator(get_object_index(), false); iterator.valid(); iterator.next()) {
        if (iterator.get_contact_point_through_reference()->get_contact_point_object_index() == other_object_index) {
            add_points3d(position, iterator.get_contact_point_through_reference()->get_position(), position);
            add_vectors3d(normal, iterator.get_contact_point_through_reference()->get_normal(), normal);
            contact_point_count++;
        }
    }
    if (contact_point_count != 0) {
        float scale = 1.0f / (float)contact_point_count;
        position->x *= scale;
        position->y *= scale;
        position->z *= scale;
        havok_component_normalize_contact_normal(normal);
        phys_vec4 hk_position;
        phys_vec4_from_real_point3d(&hk_position, position);
        phys_vec4 hk_normal;
        phys_vec4_from_real_vector3d(&hk_normal, normal);
        phys_rigid_body *rigid_body = havok_component_get_rigid_body(rigid_body_index);
        phys_rigid_body *other_rigid_body = other_component ? other_component->havok_component_get_rigid_body(other_rigid_body_index)
                                                        : get_havok_world_fixed_rigid_body();
        phys_vec4 linear_velocity;
        phys_vec4_from_real_vector3d(&linear_velocity, &m_rigid_bodies[rigid_body_index].m_pre_simulation_linear_velocity);
        phys_vec4 angular_velocity;
        phys_vec4_from_real_vector3d(&angular_velocity, &m_rigid_bodies[rigid_body_index].m_pre_simulation_angular_velocity);
        phys_vec4 other_linear_velocity;
        phys_vec4 other_angular_velocity;
        if (other_component) {
            phys_vec4_from_real_vector3d(&other_linear_velocity, &other_component->m_rigid_bodies[other_rigid_body_index].m_pre_simulation_linear_velocity);
            phys_vec4_from_real_vector3d(&other_angular_velocity, &other_component->m_rigid_bodies[other_rigid_body_index].m_pre_simulation_angular_velocity);
        } else {
            phys_vec4_from_real_vector3d(&other_linear_velocity, global_zero_vector3d);
            phys_vec4_from_real_vector3d(&other_angular_velocity, global_zero_vector3d);
        }

        phys_vec4 offset;
        offset.set_sub4(hk_position, rigid_body->get_center_of_mass_in_world());
        phys_vec4 other_offset;
        other_offset.set_sub4(hk_position, other_rigid_body->get_center_of_mass_in_world());
        phys_vec4 cross;
        cross.set_cross(rigid_body->get_angular_velocity(), offset);
        phys_vec4 velocity;
        velocity.set_add4(linear_velocity, cross);
        cross.set_cross(other_rigid_body->get_angular_velocity(), other_offset);
        phys_vec4 other_velocity;
        other_velocity.set_add4(other_linear_velocity, cross);
        phys_vec4 relative_velocity;
        relative_velocity.set_sub4(velocity, other_velocity);
        float normal_velocity = hk_normal.dot3(relative_velocity).operator float();
        float restitution = 0.0f;
        float other_restitution = 0.0f;
        float restitution_product = restitution * other_restitution;
        float coefficient = 1.0f + __fsqrts(restitution_product);
        float numerator = -coefficient * normal_velocity;

        float mass_inv = 0.0f;
        phys_vec4 angular;
        angular.set_zero4();
        if (rigid_body->get_motion_type() != k_motion_type_fixed) {
            phys_mat3 inertia_inv;
            rigid_body->get_inv_inertia_world(inertia_inv);
            phys_vec4 a;
            a.set_cross(offset, hk_normal);
            phys_vec4 b;
            b.set_mul3(inertia_inv, a);
            b.set_cross(b, offset);
            angular.m_simd_quad = b.m_simd_quad;
            mass_inv = rigid_body->get_inv_mass();
        }
        float other_mass_inv = 0.0f;
        phys_vec4 other_angular;
        other_angular.set_zero4();
        if (other_rigid_body->get_motion_type() != k_motion_type_fixed && other_rigid_body->get_motion_type() != k_motion_type_keyframed) {
            phys_mat3 inertia_inv;
            other_rigid_body->get_inv_inertia_world(inertia_inv);
            phys_vec4 a;
            a.set_cross(other_offset, hk_normal);
            phys_vec4 b;
            b.set_mul3(inertia_inv, a);
            b.set_cross(b, other_offset);
            other_angular.m_simd_quad = b.m_simd_quad;
            other_mass_inv = other_rigid_body->get_inv_mass();
        }
        float denominator;
        if (other_mass_inv == 0.0f) {
            denominator = mass_inv;
        } else {
            phys_vec4 angular_sum;
            angular_sum.set_add4(angular, other_angular);
            denominator = other_mass_inv + hk_normal.dot3(angular_sum).operator float() + mass_inv;
        }
        float impulse_magnitude = fabsf(denominator) >= k_impulse_denominator_epsilon ? numerator / denominator : 0.0f;
        *impulse = (impulse_magnitude < 0.0f ? -impulse_magnitude : impulse_magnitude) * rigid_body->get_inv_mass();
        return true;
    }
    return false;
}

// @address 0x255d70 ?disconnect_from_impacts@c_havok_component@@QAAXXZ
void c_havok_component::disconnect_from_impacts()
{
    for (c_havok_contact_point_iterator iterator(get_object_index(), false); iterator.valid(); iterator.next()) {
        c_havok_contact_point *contact_point = iterator.get_contact_point();
        contact_point->set_impact_datum_index(-1);
    }
    if (m_impact_array_index != -1)
        m_impact_array_index = -1;
}

// @address 0x258270 ?build_projectile_component@c_havok_component@@AAAXXZ
void c_havok_component::build_projectile_component()
{
    object_datum *projectile = (object_datum *)object_get_and_verify_type(get_object_index(), 32);
    if (havok_component_projectile_effected_by_phantom_volumes(projectile->definition_index)) {
        phys_transform transform;
        real_matrix4x3 matrix;
        object_get_world_matrix(get_object_index(), &matrix);
        real_point3d center;
        float radius;
        object_get_bounding_sphere(get_object_index(), &center, &radius);
        phys_shape *shape = new phys_sphere_shape(radius);
        phys_transform_from_matrix4x3(&transform, &matrix);
        m_simple_shape_phantom = new phys_simple_shape_volume(shape, transform,
            c_havok_group_filter::calcFilterInfo(22, get_object_index() + 1, 0, 0));
        havok_entity_set_havok_component_index(m_simple_shape_phantom, projectile->havok_component_index);
        havok_entity_set_havok_component_rigid_body_index(m_simple_shape_phantom, -1);
        shape->object_remove_ref();
    }
}




// @address 0x259ca8 ?sqrt@phys_math@@YAMM@Z


namespace phys_math {
// @address 0x259ca8 ?sqrt@phys_math@@YAMM@Z

}

// @address 0x2504e8 ??0phys_mass_element@@QAA@XZ

// @address 0x259d38 ??0phys_quat@@QAA@ABVphys_vec4@@M@Z



namespace {
static __forceinline void havok_component_real_rectangle3d_from_hkAabb(real_rectangle3d *rectangle, const phys_aabb *aabb)
{
    rectangle->x0 = aabb->m_min_limit.m_simd_quad.v[0];
    rectangle->x1 = aabb->m_max_limit.m_simd_quad.v[0];
    rectangle->y0 = aabb->m_min_limit.m_simd_quad.v[1];
    rectangle->y1 = aabb->m_max_limit.m_simd_quad.v[1];
    rectangle->z0 = aabb->m_min_limit.m_simd_quad.v[2];
    rectangle->z1 = aabb->m_max_limit.m_simd_quad.v[2];
}
}

// @address 0x255000 ?calculate_physics_bounding_box@c_havok_component@@QAA_NPATreal_rectangle3d@@@Z
bool c_havok_component::calculate_physics_bounding_box(real_rectangle3d *rectangle)
{
    bool result = false;
    if (is_connected_to_world() && is_physics_model_havok_component()) {
        object_datum *object = (object_datum *)object_get_and_verify_type(get_object_index(), 0xffffffff);
        s_object_definition *definition = (s_object_definition *)tag_get('obje', object->definition_index);
        s_model_definition *model = (s_model_definition *)tag_get('hlmt', definition->model.index);
        s_physics_model_definition *physics_model = (s_physics_model_definition *)tag_get('phmo', model->physics_model_index);
        phys_aabb aabb;
        for (long rigid_body_index = 0; rigid_body_index < get_rigid_body_count(); rigid_body_index++) {
            const phys_collider *collidable = havok_component_get_rigid_body(rigid_body_index)->get_collider();
            float margin = 0.0f;
            phys_aabb rigid_body_aabb;
            for (long index = 0; index < get_physics_model_rigid_body_index_count(rigid_body_index); index++) {
                s_physics_model_rigid_body *physics_rigid_body = tag_block_get_element<s_physics_model_rigid_body>(
                    &physics_model->rigid_bodies, get_physics_model_rigid_body_index(rigid_body_index, index));
                margin = margin > physics_rigid_body->margin ? margin : physics_rigid_body->margin;
            }
            g_havok_world->get_broadphase()->get_handle_aabb(collidable->get_broadphase_handle(), rigid_body_aabb);
            if (margin != 0.0f) {
                for (long index = 0; index < 3; index++) {
                    rigid_body_aabb.m_min_limit(index) -= margin;
                    rigid_body_aabb.m_max_limit(index) += margin;
                }
            }
            if (rigid_body_index == 0) {
                aabb = rigid_body_aabb;
            } else {
                aabb.m_min_limit.set_min4(aabb.m_min_limit, rigid_body_aabb.m_min_limit);
                aabb.m_max_limit.set_max4(aabb.m_max_limit, rigid_body_aabb.m_max_limit);
            }
            result = true;
        }
        if (result)
            havok_component_real_rectangle3d_from_hkAabb(rectangle, &aabb);
    }
    return result;
}

// @address 0xefac0 ?calculate_local_space_physics_bounding_box@c_havok_component@@QAA_NJPATreal_rectangle3d@@@Z
// @address 0x10f668 ?calculate_physics_bounding_sphere@c_havok_component@@QAA_NPATreal_point3d@@PAM@Z
bool c_havok_component::calculate_local_space_physics_bounding_box(long rigid_body_index, real_rectangle3d *rectangle)
{
    const phys_shape *shape = havok_component_get_rigid_body(rigid_body_index)->get_collider()->get_shape();
    phys_aabb aabb;
    phys_transform transform;
    transform.set_to_identity();
    shape->get_handle_aabb(transform, 0.0f, aabb);
    havok_component_real_rectangle3d_from_hkAabb(rectangle, &aabb);
    return true;
}

static __forceinline void havok_component_hkAabb_from_real_rectangle3d(phys_aabb *aabb, const real_rectangle3d *rectangle)
{
    aabb->m_min_limit.set(rectangle->x0, rectangle->y0, rectangle->z0, 0.0f);
    aabb->m_max_limit.set(rectangle->x1, rectangle->y1, rectangle->z1, 0.0f);
}
static __forceinline float havok_component_length3(const phys_vec4 &v)
{
    phys_vec4 dot;
    dot.m_simd_quad = __vmsum3fp(v.m_simd_quad, v.m_simd_quad);
    phys_simd_scalar length(sqrtf(dot(0)));
    float result;
    __stvewx(length.m_simd_scalar, &result, 0);
    return result;
}

// @address 0x10f668 ?calculate_physics_bounding_sphere@c_havok_component@@QAA_NPATreal_point3d@@PAM@Z
bool c_havok_component::calculate_physics_bounding_sphere(real_point3d *center, float *radius)
{
    object_datum *object = (object_datum *)object_get_and_verify_type(get_object_index(), 0xffffffff);
    const s_object_definition *definition = (const s_object_definition *)tag_get('obje', object->definition_index);
    bool result = false;
    real_rectangle3d bounds;
    if (!((definition->runtime_flags >> 1) & 1) && get_rigid_body_count() > 1 && calculate_physics_bounding_box(&bounds)) {
        phys_aabb aabb;
        havok_component_hkAabb_from_real_rectangle3d(&aabb, &bounds);
        phys_vec4 sphere_center;
        sphere_center.m_simd_quad = __vaddfp(aabb.m_min_limit.m_simd_quad, aabb.m_max_limit.m_simd_quad);
        sphere_center.mul4(phys_simd_scalar(0.5f));
        phys_vec4 extents;
        extents.m_simd_quad = __vsubfp(aabb.m_max_limit.m_simd_quad, aabb.m_min_limit.m_simd_quad);
        extents.mul4(phys_simd_scalar(0.5f));
        center->x = sphere_center(0);
        center->y = sphere_center(1);
        center->z = sphere_center(2);
        *radius = havok_component_length3(extents) + 0.001f;
        result = true;
    }
    return result;
}


namespace {
static __forceinline void havok_component_real_point3d_from_hkVector4(real_point3d *point, const phys_vec4 &vector)
{
    float z = vector(2);
    set_real_point3d(point, vector(0), vector(1), z);
}
}


// 0x259678 (no record)
// Havok shape classes: only their layout and the container's virtual call are needed; none is ever constructed.



static __forceinline void extract_havok_vector(real_vector3d *vector, const phys_vec4 *v)
{
    float z = v->m_simd_quad.v[2];
    float y = v->m_simd_quad.v[1];
    float x = v->m_simd_quad.v[0];
    vector->i = x;
    vector->j = y;
    vector->k = z;
}
static __forceinline void havok_component_matrix3x3_from_hkMatrix3(real_matrix3x3 *matrix, const phys_mat3 *rotation)
{
    extract_havok_vector(&matrix->forward, &rotation->get_column(0));
    extract_havok_vector(&matrix->left, &rotation->get_column(1));
    extract_havok_vector(&matrix->up, &rotation->get_column(2));
}
static __forceinline void havok_component_real_point3d_from_hkVector4(real_point3d *point, const phys_vec4 *v)
{
    float z = v->m_simd_quad.v[2];
    float y = v->m_simd_quad.v[1];
    float x = v->m_simd_quad.v[0];
    point->x = x;
    point->y = y;
    point->z = z;
}
static __forceinline void extract_havok_transform(real_matrix4x3 *matrix, const phys_transform *transform)
{
    matrix->scale = 1.0f;
    havok_component_matrix3x3_from_hkMatrix3(&matrix->rotation, &transform->m_rotation);
    havok_component_real_point3d_from_hkVector4(&matrix->position, &transform->m_translation);
}
// 0x2554f8



// @address 0x1ba760 ??_Gc_penetration_collector@@UAAPAXI@Z

// Phantom dispatch uses the interface in the companion header.

static __forceinline void havok_component_hkRotation_from_matrix3x3(phys_vec4 *rotation, const real_matrix3x3 *matrix)
{
    phys_vec4_from_real_vector3d(&rotation[0], &matrix->forward);
    phys_vec4_from_real_vector3d(&rotation[1], &matrix->left);
    phys_vec4_from_real_vector3d(&rotation[2], &matrix->up);
}
static __forceinline void havok_component_hkTransform_from_matrix4x3(phys_transform *transform, const real_matrix4x3 *matrix)
{
    havok_component_hkRotation_from_matrix3x3(&transform->m_rotation.get_column(0), &matrix->rotation);
    phys_vec4_from_real_point3d(&transform->m_translation, &matrix->position);
}
// @address 0x2554f8 ?rigid_body_calculate_fixed_position_along_z@c_havok_component@@QAA_NJW4e_havok_group@@PBVphys_shape@@MJPAJ@Z
bool c_havok_component::rigid_body_calculate_fixed_position_along_z(
    long rigid_body_index, e_havok_group havok_group, const phys_shape *shape, float step, long iterations, long *ending_iteration_index)
{
    // match: 0x2554f8: value range assumption guides branch lowering
    __assume(havok_group == 11);
    phys_transform transform;
    bool result = false;
    havok_component_copy_transform(&transform, havok_component_get_rigid_body(rigid_body_index)->get_transform());
    transform.m_translation(2) += step * (float)iterations;
    __declspec(align(16)) char shape_buffer[512];
    if (shape->m_type == 7) {
        const phys_capsule_shape *capsule = (const phys_capsule_shape *)shape;
        float radius = capsule->get_convex_radius() - 4.0f * get_havok_constants()->collision_tolerance;
        radius = radius > get_havok_constants()->minimum_radius ? radius : get_havok_constants()->minimum_radius;
        shape = new (shape_buffer) phys_capsule_shape(capsule->get_vertex(0), capsule->get_vertex(1), radius);
    }
    phys_volume *phantom = new phys_simple_shape_volume(shape, transform,
        c_havok_group_filter::calcFilterInfo(havok_group, get_object_index() + 1, 0, 0));
    havok_world_add_phantom(phantom);
    c_penetration_collector collector;
    bool positive = iterations > 0;
    long iteration = iterations - (positive ? 1 : -1);
    while (positive ? iteration >= 0 : iteration <= 0) {
        collector.reset();
        ((phys_shape_volume *)phantom)->set_transform(transform);
        ((havok_component_retail::phys_shape_volume *)phantom)->get_penetrations(collector);
        if (!collector.get_early_out()) {
            *ending_iteration_index = iterations - iteration;
            real_matrix4x3 matrix;
            extract_havok_transform(&matrix, &transform);
            phys_transform fixed_transform;
            havok_component_hkTransform_from_matrix4x3(&fixed_transform, &matrix);
            ((c_havok_component *)(this))->set_rigid_body_transform_internal(rigid_body_index, (phys_transform const *)(&fixed_transform));
            result = true;
            break;
        }
        transform.m_translation(2) += step * (positive ? -1.0f : 1.0f);
        if (positive)
            iteration--;
        else
            iteration++;
    }
    g_havok_world->remove_volume(phantom);
    phantom->object_remove_ref();
    return result;
}

static __forceinline void havok_component_real_vector3d_in_order_from_hkVector4(real_vector3d *vector, const phys_vec4 *v)
{
    vector->i = v->m_simd_quad.v[0];
    vector->j = v->m_simd_quad.v[1];
    vector->k = v->m_simd_quad.v[2];
}
// @address 0x25d210 ?calculate_shape_fixed_position@@YA_NPBVphys_shape@@JW4e_havok_group@@PBUreal_matrix4x3@@PBTreal_vector3d@@PATreal_point3d@@PAUs_location@@MM_NPBT5@J@Z

// @address 0x255318 ?rigid_body_calculate_fixed_position@c_havok_component@@QAA_NJW4e_havok_group@@PBTreal_point3d@@PBTreal_vector3d@@PAT3@PAUs_location@@MM_N1J@Z
bool c_havok_component::rigid_body_calculate_fixed_position(long rigid_body_index, e_havok_group havok_group, const real_point3d *position,
    const real_vector3d *velocity, real_point3d *fixed_position, s_location *location, float minimum_search_radius,
    float maximum_search_radius, bool search_everywhere, const real_point3d *raycast_origin, long raycast_object_index)
{
    phys_rigid_body *rigid_body = havok_component_get_rigid_body(rigid_body_index);
    const phys_shape *shape = rigid_body->get_collider()->get_shape();
    __declspec(align(16)) char shape_buffer[512];
    if (((1 << object_header_get(get_object_index())->object_type) & 1) != 0 && shape->m_type == 7) {
        const phys_capsule_shape *capsule = (const phys_capsule_shape *)shape;
        float radius = (capsule->get_convex_radius() - 2.0f * get_havok_constants()->minimum_radius) >
                get_havok_constants()->minimum_radius
            ? (capsule->get_convex_radius() - 2.0f * get_havok_constants()->minimum_radius)
            : get_havok_constants()->minimum_radius;
        shape = new (shape_buffer) phys_capsule_shape(capsule->get_vertex(0), capsule->get_vertex(1), radius);
    }
    real_matrix4x3 matrix;
    matrix.scale = 1.0f;
    const phys_mat3 *rotation = &rigid_body->get_transform().m_rotation;
    havok_component_real_vector3d_in_order_from_hkVector4(&matrix.rotation.forward, &rotation->get_column(0));
    havok_component_real_vector3d_in_order_from_hkVector4(&matrix.rotation.left, &rotation->get_column(1));
    matrix.position = *position;
    havok_component_real_vector3d_in_order_from_hkVector4(&matrix.rotation.up, &rotation->get_column(2));
    bool result = calculate_shape_fixed_position(shape, get_object_index(), havok_group, &matrix, velocity, fixed_position, location, minimum_search_radius,
        maximum_search_radius,
        search_everywhere, raycast_origin, raycast_object_index);
    if (result) {
        matrix.position = *fixed_position;
        real_matrix4x3 object_transform;
        calculate_object_transform(&object_transform, &matrix);
        *fixed_position = object_transform.position;
    }
    return result;
}

// @address 0x259678 ?calculate_transform_between_nodes_on_rigid_body@c_havok_component@@AAAXJJJPAUreal_matrix4x3@@@Z
void c_havok_component::calculate_transform_between_nodes_on_rigid_body(long rigid_body_index, long node_index_a, long node_index_b, real_matrix4x3 *matrix)
{
    object_datum *object = (object_datum *)object_get_and_verify_type(get_object_index(), 0xffffffff);
    s_object_definition *definition = (s_object_definition *)tag_get('obje', object->definition_index);
    s_model_definition *model = (s_model_definition *)tag_get('hlmt', definition->model.index);
    s_physics_model_definition *physics_model = (s_physics_model_definition *)tag_get('phmo', model->physics_model_index);
    long index_a = -1;
    long index_b = -1;
    for (long index = 0; index < get_physics_model_rigid_body_index_count(rigid_body_index) && (index_a == -1 || index_b == -1); index++) {
        long physics_model_rigid_body_index = get_physics_model_rigid_body_index(rigid_body_index, index);
        s_physics_model_rigid_body *physics_model_rigid_body = tag_block_get_element<s_physics_model_rigid_body>(
            &physics_model->rigid_bodies, physics_model_rigid_body_index);
        if (physics_model_rigid_body->node == node_index_a && index_a == -1)
            index_a = index;
        if (physics_model_rigid_body->node == node_index_b && index_b == -1)
            index_b = index;
    }
    const phys_list_shape *shape = (const phys_list_shape *)havok_component_get_rigid_body(rigid_body_index)->get_collider()->get_shape();
    if (index_b == 0) {
        __declspec(align(16)) char buffer[512];
        const phys_transform_shape *child_shape = (const phys_transform_shape *)shape->get_child_shape(index_a, buffer);
        extract_havok_transform(matrix, &child_shape->get_transform());
    } else if (index_a == 0) {
        __declspec(align(16)) char buffer[512];
        const phys_transform_shape *child_shape = (const phys_transform_shape *)shape->get_child_shape(index_b, buffer);
        extract_havok_transform(matrix, &child_shape->get_transform());
    } else {
        phys_transform inverse;
        phys_transform product;
        __declspec(align(16)) char buffer_a[512];
        __declspec(align(16)) char buffer_b[512];
        const phys_transform_shape *child_shape_a = (const phys_transform_shape *)shape->get_child_shape(index_a, buffer_a);
        const phys_transform_shape *child_shape_b = (const phys_transform_shape *)shape->get_child_shape(index_b, buffer_b);
        ((phys_transform *)(&inverse))->set_to_inverse((*(phys_transform const *)(&child_shape_a->get_transform())));
        ((phys_transform *)(&product))->set_mul((*(phys_transform const *)(&child_shape_b->get_transform())), (*(phys_transform const *)(&inverse)));
        extract_havok_transform(matrix, &product);
    }
}

/* ---- 0xe2ec0 group restart (natural source for the widget group loop) ---- */
class phys_island;

// @address 0xe2ec0 havok_component_activate_rigid_bodies
extern "C" void havok_component_activate_rigid_bodies(c_havok_component *component, unsigned char restart)
{
    unsigned char active = (component->m_flags >> 3) & 1;
    if (active == 0)
        return;
    for (long i = 0; i < component->get_rigid_body_count(); i++) {
        phys_rigid_body *rigid_body = component->havok_component_get_rigid_body(i);
        rigid_body->activate();
        if (restart == 0)
            continue;
        phys_world *owner = g_havok_world;
        if (owner == 0)
            continue;
        phys_rigid_body *body = rigid_body;
        if (body->m_motion.m_type == 7)
            continue;
        body->m_motion.m_inactive_frame_counts[0] = 0;
        body->m_motion.m_inactive_frame_counts[1] = 0;
        phys_world_op_util::mark_island_active(owner, body->m_owner_island);
        (&body->m_motion)
            ->set_world_select_flags_neg(g_havok_world->m_dynamics_step_info.m_solver_info.m_inactive_frame_select[0],
                g_havok_world->m_dynamics_step_info.m_solver_info.m_inactive_frame_select[1],
                g_havok_world->m_dynamics_step_info.m_solver_info.m_integrate_sleep_counter);
    }
}
static __forceinline bool havok_component_contact_point_removal_deferred() { return g_havok_monitor_globals.update_in_progress; }
// @address 0x105f00 ?remove_contact_point@c_rigid_body@c_havok_component@@QAAXGJJJJ@Z
void c_havok_component::c_rigid_body::remove_contact_point(
    unsigned short contact_id, long structure_bsp_index, long other_object_index, long rigid_body_index, long other_rigid_body_index)
{
    for (long contact_point_index = 0; contact_point_index < get_contact_point_count(); contact_point_index++) {
        c_havok_contact_point *contact_point = get_contact_point(contact_point_index);
        if (contact_point->matches_contact_description(contact_id, structure_bsp_index, other_object_index, rigid_body_index, other_rigid_body_index)) {
            if (!havok_component_contact_point_removal_deferred()) {
                remove_contact_point(contact_point_index);
                break;
            } else if (!contact_point->get_life_cycle_flag_is_deleted()) {
                contact_point->notify_is_deleted();
                break;
            }
        }
    }
}

// @address 0x106ef8 ?remove_contact_point@c_rigid_body@c_havok_component@@QAAXJ@Z
void c_havok_component::c_rigid_body::remove_contact_point(long contact_point_index)
{
    c_havok_contact_point *contact_point = get_contact_point(contact_point_index);
    if (contact_point->get_impact_datum_index() != -1) {
        long impact_index = contact_point->get_impact_datum_index();
        c_impact *impact = (c_impact *)(tls_impact_data()->data + (unsigned short)impact_index * sizeof(c_impact));
        impact->remove_reference(impact_index, false);
        contact_point->set_impact_datum_index(-1);
    }
    m_contact_points.remove_at(contact_point_index);
}
// @address 0x253d00 ?find_powered_chain_reference_connecting_nodes@c_havok_component@@QAA?AUs_powered_chain_link_reference@1@JJ@Z
c_havok_component::s_powered_chain_link_reference c_havok_component::find_powered_chain_reference_connecting_nodes(long node_a, long node_b)
{
    object_datum *object = (object_datum *)object_get_and_verify_type(get_object_index(), 0xffffffff);
    s_object_definition *definition = (s_object_definition *)tag_get('obje', object->definition_index);
    s_model_definition *model = (s_model_definition *)tag_get('hlmt', definition->model.index);
    s_physics_model_definition *physics_model = (s_physics_model_definition *)tag_get('phmo', model->physics_model_index);
    s_powered_chain_link_reference result;
    result.constraint_index = -1;
    result.link_index = -1;
    for (long constraint_index = 0; constraint_index < get_constraint_count(); constraint_index++) {
        c_constraint &constraint = m_component_constraints[constraint_index];
        if (constraint.get_constraint_type() == 6) {
            s_physics_model_powered_chain *chain = tag_block_get_element<s_physics_model_powered_chain>(
                &physics_model->powered_chains, constraint.get_constraint_type_index());
            for (long link_index = 0; link_index < chain->nodes.count - 1; link_index++) {
                if (*tag_block_get_element<short>(&chain->nodes, link_index) == node_a &&
                    *tag_block_get_element<short>(&chain->nodes, link_index + 1) == node_b) {
                    result.constraint_index = (short)constraint_index;
                    result.link_index = (short)link_index;
                    break;
                }
            }
        }
    }
    return result;
}
// @address 0x253e18 ?keyframe_powered_chain_link@c_havok_component@@QAAXUs_powered_chain_link_reference@1@PBUreal_matrix3x3@@@Z
void c_havok_component::keyframe_powered_chain_link(s_powered_chain_link_reference reference, const real_matrix3x3 *matrix)
{
    phys_constraint *constraint = m_component_constraints[reference.constraint_index].get_constraint();
    phys_powered_chain_data *data = (phys_powered_chain_data *)constraint->get_chain_data();
    phys_rotation rotation;
    phys_mat3_from_matrix3x3((phys_mat3 *)(&rotation), (real_matrix3x3 const *)(matrix));
    ((phys_quat *)(&data->m_chain_infos[reference.link_index].m_a_to_c))->set((*(phys_rotation const *)(&rotation)));
}

// @address 0x104f30 ?process_contact_point@c_rigid_body@c_havok_component@@QAAXPAUphys_contact_process_event@@JJJJJ@Z
void c_havok_component::c_rigid_body::process_contact_point(phys_contact_process_event *event, long bsp_index, long object_index,
    long other_havok_component_index, long rigid_body_index, long other_rigid_body_index)
{
    for (long contact_point_index = 0; contact_point_index < event->m_collision_data->get_process_contact_count(); contact_point_index++) {
        phys_process_hit_point *point = &event->m_collision_data->m_process_contact_points[contact_point_index];
        for (long index = 0; index < get_contact_point_count(); index++) {
            c_havok_contact_point *contact_point = get_contact_point(index);
            if (contact_point->matches_contact_description((unsigned short)point->m_process_contact_id, bsp_index, other_havok_component_index,
                    rigid_body_index, other_rigid_body_index))
            {
                phys_rigid_body *rigid_body = phys_get_rigid_body(event->m_collider_a);
                if (rigid_body) {
                    bool flip = (phys_entity *)rigid_body != event->m_callback_fired_from;
                    const phys_shape *shape;
                    if (flip)
                        shape = event->m_collider_a->get_shape();
                    else
                        shape = event->m_collider_b->get_shape();
                    get_contact_point(index)->update(false, &point->m_hit_contact.get_contact_position(), &point->m_hit_contact.get_separating_normal(),
                        event->m_internal_contact_manager, shape, flip, object_index, rigid_body_index);
                }
            }
        }
    }
}

__forceinline c_havok_contact_point::c_havok_contact_point(unsigned short contact_point_id, long structure_bsp_index, unsigned int environment_shape_key,
    long surface_index, long other_object_index,
    unsigned char flags, phys_rigid_body *rigid_body, phys_rigid_body *other_rigid_body, float friction, float restitution,
    c_global_material_type material_a, c_global_material_type material_b) :
    m_environment_shape_key(environment_shape_key),
    m_impact_datum_index(-1),
    m_surface_index(surface_index),
    m_global_material_type_a(material_a),
    m_global_material_type_b(material_b),
    m_other_havok_component_index(havok_entity_get_havok_component_index((phys_world_object const *)((const phys_world_object *)other_rigid_body))),
    m_other_object_index(other_object_index),
    m_contact_friction(friction),
    m_contact_restitution(restitution),
    m_contact_point_id(contact_point_id),
    m_life_cycle_flags(1),
    m_flags(flags),
    m_rigid_body_index((signed char)havok_entity_get_havok_component_rigid_body_index((phys_world_object const *)((const phys_world_object *)rigid_body))),
    m_other_rigid_body_index((signed char)havok_entity_get_havok_component_rigid_body_index(
        (phys_world_object const *)((const phys_world_object *)other_rigid_body))),
    m_structure_bsp_index((signed char)structure_bsp_index)
{
}
// @address 0x106b50 ?add_contact_point@c_rigid_body@c_havok_component@@QAAJG_NJIJJEPAVphys_rigid_body@@1MMVc_global_material_type@@2@Z
long c_havok_component::c_rigid_body::add_contact_point(unsigned short contact_point_id, bool can_replace, long bsp_index, unsigned int environment_shape_key,
    long surface_index, long other_object_index,
    unsigned char flags, phys_rigid_body *rigid_body, phys_rigid_body *other_rigid_body, float friction, float restitution,
    c_global_material_type material_a, c_global_material_type material_b)
{
    bool has_room = m_contact_points.getSize() < 60;
    long contact_point_index = -1;
    if (!has_room && can_replace) {
        long best_index = -1;
        float best = FLT_MAX;
        for (long index = 0; index < m_contact_points.getSize(); index++) {
            c_havok_contact_point *contact_point = get_contact_point(index);
            if (contact_point->get_normal()->k < best && contact_point->get_impact_datum_index() == -1) {
                best_index = index;
                best = contact_point->get_normal()->k;
            }
        }
        if (best_index != -1) {
            remove_contact_point(best_index);
            has_room = true;
        }
    }
    if (has_room) {
        contact_point_index = m_contact_points.getSize();
        c_havok_contact_point *contact_point = (c_havok_contact_point *)m_contact_points.expand_by(1);
        new (contact_point) c_havok_contact_point(contact_point_id, bsp_index, environment_shape_key, surface_index, other_object_index, flags,
            rigid_body, other_rigid_body, friction, restitution, material_a, material_b);
    }
    return contact_point_index;
}

static __forceinline void havok_contact_point_reference_set(s_havok_contact_point_reference *reference, long rigid_body_index, long contact_point_index)
{
    reference->rigid_body_index = (short)rigid_body_index;
    reference->contact_point_index = (short)contact_point_index;
}
// @address 0x106a50 ?notify_contact_point_added@c_havok_component@@QAA?AUs_havok_contact_point_reference@@GJIJJEPAVphys_rigid_body@@0MMVc_global_material_type@@1@Z
s_havok_contact_point_reference c_havok_component::notify_contact_point_added(unsigned short contact_point_id, long bsp_index,
    unsigned int environment_shape_key, long surface_index, long other_object_index,
    unsigned char flags, phys_rigid_body *rigid_body, phys_rigid_body *other_rigid_body, float friction, float restitution,
    c_global_material_type material_a, c_global_material_type material_b)
{
    s_havok_contact_point_reference reference = k_invalid_contact_point_reference;
    if (records_contact_points()) {
        long rigid_body_index = havok_entity_get_havok_component_rigid_body_index((phys_world_object const *)((const phys_world_object *)rigid_body));
        bool can_replace = (((1 << object_get_type((unsigned short)get_object_index())) & 1) != 0 && !is_physics_model_havok_component()) ? true : false;
        long contact_point_index = m_rigid_bodies[rigid_body_index].add_contact_point(contact_point_id, can_replace, bsp_index,
            environment_shape_key, surface_index, other_object_index, flags, rigid_body, other_rigid_body, friction, restitution, material_a,
            material_b);
        if (contact_point_index != -1)
            havok_contact_point_reference_set(&reference, rigid_body_index, contact_point_index);
    }
    return reference;
}

// match: 0x108278: retain the owner's long ternary before narrowing; the shared out-of-line accessor has a different copy web.
// match: 0x254858: the same validated-index helper preserves the inlined narrowing in constraint alignment.
static __forceinline short havok_component_controlling_rigid_body_index(const c_havok_component *component)
{
    long index = component->m_object_postion_controlling_rigid_body_index;
    return (short)((index >= 0 && index < component->m_rigid_bodies.getSize()) ? index : -1);
}

// @address 0x108278 ?calculate_object_transform@c_havok_component@@QAAXPAUreal_matrix4x3@@@Z
void c_havok_component::calculate_object_transform(real_matrix4x3 *transform)
{
    real_matrix4x3 matrix;
    if (havok_component_controlling_rigid_body_index(this) != -1)
        get_rigid_body_transform(havok_component_controlling_rigid_body_index(this), &matrix);
    else
        object_get_world_matrix(m_object_index, &matrix);
    calculate_object_transform(transform, &matrix);
}

// 0x108328
// @address 0x108328 ?calculate_object_transform@c_havok_component@@QAAXPAUreal_matrix4x3@@PBU2@@Z
void c_havok_component::calculate_object_transform(real_matrix4x3 *transform, const real_matrix4x3 *matrix)
{
    if (is_one_body_object_space_component()) {
        memcpy(transform, matrix, sizeof(real_matrix4x3));
        long object_index = get_object_index();
        float z_offset;
        if ((1 << object_get_type((unsigned short)object_index) & 1) != 0 && biped_calculate_invisible_crouched_uber_melee_z_offset(object_index, &z_offset)) {
            real_matrix4x3 *object_transform = transform;
            real_point3d *position = &object_transform->position;
            float *z = &position->z;
            *z += z_offset;
        } else {
            object_type_fix_transform_from_physics(object_index, transform);
        }
    } else if (get_object_postion_controlling_rigid_body_index() != -1) {
        memcpy(transform, matrix, sizeof(real_matrix4x3));
        if ((short)get_object_postion_controlling_node_index() != -1) {
            object_datum *object = (object_datum *)object_get_and_verify_type(get_object_index(), 0xffffffff);
            s_object_definition *definition = (s_object_definition *)tag_get('obje', object->definition_index);
            s_model_definition *model = (s_model_definition *)tag_get('hlmt', definition->model.index);
            render_model_definition *render_model = (render_model_definition *)tag_get('mode', model->render_model_index);
            render_model_node *node = tag_block_get_element<render_model_node>(&render_model->nodes, (short)get_object_postion_controlling_node_index());
            matrix4x3_compute_product(transform, &node->default_inverse_matrix, transform);
        }
    } else {
        memcpy(transform, matrix, sizeof(real_matrix4x3));
    }
}



// @address 0x10f3f8 ?body_slam_unsimulated_nodes_recursive@c_havok_component@@QAAXJPAV?$c_static_flags@$0PP@@@PBV2@PAUreal_orientation@@J@Z
void c_havok_component::body_slam_unsimulated_nodes_recursive(long node_index, c_static_flags<255> *simulation_controlled_nodes,
    const c_static_flags<255> *desired_simulation_controlled_nodes, real_orientation *node_orientations, long node_orientations_count)
{
    object_datum *object = (object_datum *)object_get_and_verify_type(get_object_index(), 0xffffffff);
    s_object_definition *definition = (s_object_definition *)tag_get('obje', object->definition_index);
    s_model_definition *model = (s_model_definition *)tag_get('hlmt', definition->model.index);
    s_physics_model_definition *physics_model = (s_physics_model_definition *)tag_get('phmo', model->physics_model_index);
    s_physics_model_node *node = tag_block_get_element<s_physics_model_node>(&physics_model->nodes, node_index);
    render_model_definition *render_model = (render_model_definition *)tag_get('mode', ((s_model_definition *)model)->render_model_index);
    short parent_node_index = node->parent_node_index;
    if (parent_node_index != -1) {
        if (simulation_controlled_nodes->test(node_index)) {
            if (node_orientations) {
                real_matrix4x3 inverse;
                real_matrix4x3 relative;
                matrix4x3_inverse(object_get_node_matrix(get_object_index(), parent_node_index), &inverse);
                matrix4x3_compute_product(&inverse, object_get_node_matrix(get_object_index(), node_index), &relative);
                matrix4x3_to_orientation(&relative, &node_orientations[node_index]);
            }
        } else if (desired_simulation_controlled_nodes->test(node_index)) {
            render_model_node *render_node = tag_block_get_element<render_model_node>(&render_model->nodes, node_index);
            real_matrix4x3 *parent_matrix = object_get_node_matrix(get_object_index(), parent_node_index);
            real_matrix4x3 *node_matrix = object_get_node_matrix(get_object_index(), node_index);
            real_matrix4x3 matrix;
            matrix3x3_rotation_from_quaternion(&matrix.rotation, &render_node->default_rotation);
            matrix.scale = 1.0f;
            matrix.position = render_node->default_translation;
            matrix4x3_compute_product(parent_matrix, &matrix, node_matrix);
            simulation_controlled_nodes->set(node_index, true);
            if (node_orientations) {
                node_orientations[node_index].rotation = render_node->default_rotation;
                node_orientations[node_index].translation = render_node->default_translation;
                node_orientations[node_index].scale = 1.0f;
            }
        }
    }
    long child_node_index = node->child_node_index;
    while (child_node_index != -1) {
        s_physics_model_node *child_node = tag_block_get_element<s_physics_model_node>(&physics_model->nodes, child_node_index);
        body_slam_unsimulated_nodes_recursive(
            child_node_index, simulation_controlled_nodes, desired_simulation_controlled_nodes, node_orientations, node_orientations_count);
        child_node_index = child_node->sibling_node_index;
    }
}


// @address 0x64bc58 ??0phys_box_volume@@QAA@ABVphys_aabb@@I@Z
// @address 0x64b588 ?set_aabb@phys_box_volume@@QAAXABVphys_aabb@@@Z

// @address 0x250c48 ?move_aabb_phantom@c_havok_component@@QAAXPBUreal_matrix4x3@@PBTreal_rectangle3d@@@Z
void c_havok_component::move_aabb_phantom(const real_matrix4x3 *matrix, const real_rectangle3d *bounds)
{
    if (m_aabb_phantom) {
        phys_vec4 center;
        center.set((bounds->x0 + bounds->x1) * 0.5f, (bounds->y0 + bounds->y1) * 0.5f, (bounds->z0 + bounds->z1) * 0.5f, 0.0f);
        phys_vec4 half_extents;
        half_extents.set((bounds->x1 - bounds->x0) * 0.5f, (bounds->y1 - bounds->y0) * 0.5f, (bounds->z1 - bounds->z0) * 0.5f, 0.0f);
        phys_transform transform;
        phys_aabb aabb;
        phys_transform_from_matrix4x3(&transform, matrix);
        phys_aabb_util::compute_bounds(transform, half_extents, center, 0.0f, aabb);
        ((phys_box_volume *)m_aabb_phantom)->set_aabb(aabb);
    }
}

static __forceinline void havok_component_release_world_object(phys_world_object *world_object)
{
    havok_entity_set_havok_component_index(world_object, -1);
    havok_entity_set_havok_component_rigid_body_index(world_object, -1);
    world_object->object_remove_ref();
}

// @address 0x251170 ?clear_rigid_bodies@c_havok_component@@QAAXXZ
void c_havok_component::clear_rigid_bodies()
{
    for (long constraint_index = 0; constraint_index < get_constraint_count(); constraint_index++) {
        c_constraint *constraint = &m_component_constraints[constraint_index];
        ((phys_ref_object *)constraint->get_constraint())->object_remove_ref();
        constraint->m_contact_constraint = 0;
        constraint->~c_constraint();
    }
    for (long rigid_body_index = 0; rigid_body_index < get_rigid_body_count(); rigid_body_index++) {
        c_rigid_body *body = &m_rigid_bodies[rigid_body_index];
        phys_rigid_body *rigid_body = body->m_rigid_body;
        havok_component_release_world_object((phys_world_object *)rigid_body);
        body->~c_rigid_body();
    }
    m_flags = 0;
    m_maximum_rigid_body_size = 0;
    m_object_postion_controlling_rigid_body_index = -1;
    m_object_postion_controlling_node_index = -1;
    m_rigid_bodies.clear_and_deallocate();
    m_component_constraints.clear_and_deallocate();
    if (m_simple_shape_phantom) {
        havok_entity_set_havok_component_index(m_simple_shape_phantom, -1);
        havok_entity_set_havok_component_rigid_body_index(m_simple_shape_phantom, -1);
        m_simple_shape_phantom->object_remove_ref();
        m_simple_shape_phantom = 0;
    }
    if (m_aabb_phantom) {
        havok_entity_set_havok_component_index(m_aabb_phantom, -1);
        havok_entity_set_havok_component_rigid_body_index(m_aabb_phantom, -1);
        m_aabb_phantom->object_remove_ref();
        m_aabb_phantom = 0;
    }
}

extern "C" {
                                              /* havok_world_remove_entity */
}
static __forceinline void havok_component_world_remove_constraint(phys_constraint *constraint)
{
    if (!g_havok_monitor_globals.inside_simulation_step)
        g_havok_world->remove_constraint(constraint);
}
static __forceinline void havok_component_clear_contact_points(phys_array<c_havok_component::c_rigid_body::s_contact_point_buffer> &points)
{
    if ((points.m_capacity_and_flags & PHYS_ARRAY_DONT_DEALLOCATE_FLAG) == 0)
        phys_thread_memory::get_singleton_instance().free_chunk(
            points.m_data, points.get_sector_capacity() * sizeof(c_havok_component::c_rigid_body::s_contact_point_buffer), PHYS_MEM_ARRAY);
    points.m_data = 0;
    points.m_size = 0;
    points.m_capacity_and_flags = PHYS_ARRAY_DONT_DEALLOCATE_FLAG |
        (points.m_capacity_and_flags & PHYS_ARRAY_LOCKED_FLAG);
}
// @address 0x251958 ?deactivator_update_in_local_space@c_havok_component@@QAAXJPBTreal_vector3d@@0PAJ1@Z
void c_havok_component::deactivator_update_in_local_space(
    long rigid_body_index, const real_vector3d *linear_velocity, const real_vector3d *angular_velocity, long *high_result, long *low_result)
{
    c_rigid_body *body = &m_rigid_bodies[rigid_body_index];
    *high_result = 0;
    *low_result = 0;
    phys_rigid_body *rigid_body = body->m_rigid_body;
    if (rigid_body->get_action_world() != 0) {
        c_object_rigid_body_deactivator *deactivator = (c_object_rigid_body_deactivator *)rigid_body->m_body_deactivator;
        float saved_54 = deactivator->m_min_high_freq_translation;
        float saved_58 = deactivator->m_min_high_freq_rotation;
        float saved_5c = deactivator->m_min_low_freq_pos;
        float saved_60 = deactivator->m_min_low_freq_rot;
        long high_ticks = (long)(rigid_body->get_action_world()->m_hf_sleep_period * (float)(long)game_time_globals->ticks_per_second + 0.5f);
        long low_ticks = (long)(rigid_body->get_action_world()->m_low_freq_sleep_period * (float)(long)game_time_globals->ticks_per_second + 0.5f);
        float high_scale = 3.0f;
        float low_scale = high_scale;
        if (is_physics_model_havok_component()) {
            object_datum *object = (object_datum *)object_get_and_verify_type(get_object_index(), 0xffffffff);
            s_object_definition *definition = (s_object_definition *)tag_get('obje', object->definition_index);
            s_model_definition *model = (s_model_definition *)tag_get('hlmt', definition->model.index);
            s_physics_model_definition *physics_model = (s_physics_model_definition *)tag_get('phmo', model->physics_model_index);
            if (physics_model->deactivator_scale_b > 0.001f && physics_model->deactivator_scale_a > 0.001f) {
                low_scale = high_scale / physics_model->deactivator_scale_a;
                high_scale = high_scale / physics_model->deactivator_scale_b;
            }
        }
        deactivator->m_min_high_freq_translation *= low_scale;
        deactivator->m_min_low_freq_pos *= high_scale;
        deactivator->m_min_high_freq_rotation *= low_scale;
        deactivator->m_min_low_freq_rot *= high_scale;
        deactivator->update_in_local_space(linear_velocity, angular_velocity);
        if (game_time_globals->time % high_ticks == 0)
            *high_result = deactivator->should_sleep_high_freq((const phys_entity *)rigid_body) ? 1 : -1;
        if (game_time_globals->time % low_ticks == 0)
            *low_result = deactivator->should_sleep_low_freq((const phys_entity *)rigid_body) ? 1 : -1;
        c_havok_contact_point_iterator iterator(get_object_index(), false);
        if (!iterator.valid()) {
            *high_result = -1;
            *low_result = -1;
        }
        deactivator->m_min_high_freq_translation = saved_54;
        deactivator->m_min_low_freq_pos = saved_5c;
        deactivator->m_min_high_freq_rotation = saved_58;
        deactivator->m_min_low_freq_rot = saved_60;
    }
}
// @address 0x251be0 ?disconnect_from_havok_world@c_havok_component@@QAAXXZ
void c_havok_component::disconnect_from_havok_world()
{
    for (long constraint_index = 0; constraint_index < get_constraint_count(); constraint_index++)
        havok_component_world_remove_constraint(m_component_constraints[constraint_index].get_constraint());
    for (long rigid_body_index = 0; rigid_body_index < get_rigid_body_count(); rigid_body_index++) {
        c_rigid_body_inplace_array *rigid_bodies = &m_rigid_bodies;
        phys_rigid_body *rigid_body = (*rigid_bodies)[rigid_body_index].m_rigid_body;
        bool fixed = rigid_body->get_motion_type() == k_motion_type_fixed;
        if (!fixed)
            ((phys_entity *)(rigid_body))->remove_activation_listener((phys_entity_activation_listener *)(g_global_entity_listener));
        if (records_contact_points() && !fixed && rigid_body->get_motion_type() != k_motion_type_keyframed)
            ((phys_entity *)(rigid_body))->remove_collision_listener((phys_collision_listener *)(g_global_entity_listener));
        havok_world_remove_entity((phys_entity *)rigid_body);
    }
    if (m_bodies_in_phantom != 0) {
        for (long index = m_bodies_in_phantom->getSize() - 1; index >= 0; index--) {
            s_body_in_phantom *body = &(*m_bodies_in_phantom)[index];
            if (body->object_index != -1) {
                if (object_try_and_get_and_verify_type(body->object_index, 0xffffffff) != 0)
                    object_set_at_rest(body->object_index, false);
            }
        }
        m_bodies_in_phantom->clear();
    }
    while (get_impact_count() != 0) {
        long impact_index = havok_component_impact_array_impact_get(m_impact_array_index, 0);
        c_impact *impact = havok_component_impact_datum_get(impact_index);
        if (impact->get_type() == 3 && impact->get_reference_count() == 1)
            impact->remove_reference(impact_index, false);
        impact_delete(impact_index);
    }
    if (records_contact_points()) {
        for (long rigid_body_index = 0; rigid_body_index < get_rigid_body_count(); rigid_body_index++)
            havok_component_clear_contact_points(m_rigid_bodies[rigid_body_index].m_contact_points);
    }
    if (m_simple_shape_phantom != 0)
        g_havok_world->remove_volume(m_simple_shape_phantom);
    if (m_aabb_phantom != 0)
        g_havok_world->remove_volume(m_aabb_phantom);
    m_flags &= ~8;
}


// @address 0x251f00 ?connect_to_havok_world@c_havok_component@@QAAXXZ
void c_havok_component::connect_to_havok_world()
{
    if (m_aabb_phantom != 0)
        havok_world_add_phantom(m_aabb_phantom);
    if (m_simple_shape_phantom != 0)
        havok_world_add_phantom(m_simple_shape_phantom);

    long rigid_body_index = 0;
    object_datum *object = (object_datum *)object_get_and_verify_type(m_object_index, 0xffffffff);
    if (get_rigid_body_count() > 0) {
      (void)global_tag_instances;
      object_datum **object_ptr = &object;
            float one = 1.0f;
      do {
        phys_rigid_body *rigid_body = havok_component_get_rigid_body(rigid_body_index);
        long motion_type = rigid_body->get_motion_type();
        bool fixed = motion_type == 7;
        bool keyframed = motion_type == 6;
        if (!fixed)
            ((phys_entity *)(rigid_body))->add_entity_activation_listener(
                (phys_entity_activation_listener *)((phys_entity_activation_listener *)g_global_entity_listener));
        if (records_contact_points() && !fixed && !keyframed) {
            phys_collision_listener *listener = g_global_entity_listener ? static_cast<phys_collision_listener *>(g_global_entity_listener) : 0;
            ((phys_entity *)(rigid_body))->add_collision_listener((phys_collision_listener *)(listener));
            rigid_body->m_contact_callback_delay = 3;
        }
        set_rigid_body_linear_velocity(rigid_body_index, &object->linear_velocity);
        set_rigid_body_angular_velocity(rigid_body_index, &object->angular_velocity);
        havok_world_add_entity((phys_entity *)(rigid_body), (object->physics_flags & (1 << 9)) != 0);

        if (!fixed && !keyframed) {
            c_object_rigid_body_deactivator *deactivator = new c_object_rigid_body_deactivator(rigid_body, m_object_index);
            if (is_physics_model_havok_component()) {
                s_object_definition *definition = (s_object_definition *)tag_get('obje', (*object_ptr)->definition_index);
                s_model_definition *model = (s_model_definition *)tag_get('hlmt', definition->model.index);
                s_physics_model_definition *physics_model = (s_physics_model_definition *)tag_get('phmo', model->physics_model_index);
                if (physics_model->deactivator_scale_a != one || physics_model->deactivator_scale_b != one) {
                    deactivator->m_min_high_freq_translation *= physics_model->deactivator_scale_a;
                    deactivator->m_min_high_freq_rotation *= physics_model->deactivator_scale_a;
                    deactivator->m_min_low_freq_pos *= physics_model->deactivator_scale_b;
                    deactivator->m_min_low_freq_rot *= physics_model->deactivator_scale_b;
                }
            }
            rigid_body->set_deactivator((phys_rigid_body_deactivator *)deactivator);
            ((phys_ref_object *)deactivator)->object_remove_ref();
        }
        rigid_body_index++;
      } while (rigid_body_index < get_rigid_body_count());
    }

    for (long constraint_index = 0; constraint_index < get_constraint_count(); constraint_index++)
        havok_world_add_constraint(m_component_constraints[constraint_index].get_constraint());
    m_flags |= 8;
}

// @address 0x251388 ?set_rigid_body_transform_internal@c_havok_component@@QAAXJPBVphys_transform@@@Z

void c_havok_component::set_rigid_body_transform_internal(long rigid_body_index, const phys_transform *transform)
{
    phys_transform test_transform;
    havok_component_copy_transform(&test_transform, *transform);
    if (havok_component_get_rigid_body(rigid_body_index)->m_motion.m_type == k_motion_type_fixed && (m_flags & (1 << 3))) {
        disconnect_from_havok_world();
        havok_component_get_rigid_body(rigid_body_index)->set_transform(test_transform);
        connect_to_havok_world();
    } else {
        havok_component_get_rigid_body(rigid_body_index)->set_transform(test_transform);
    }
}

// @address 0x252220 ?save_rigid_body_state@c_havok_component@@QAAXPAUs_havok_component_rigid_body_state@@@Z
void c_havok_component::save_rigid_body_state(s_havok_component_rigid_body_state *state)
{
    state->valid_rigid_bodies.clear();
    for (long rigid_body_index = 0; rigid_body_index < get_rigid_body_count(); rigid_body_index++) {
        if (get_physics_model_rigid_body_index_count(rigid_body_index) > 0) {
            long physics_model_rigid_body_index = get_physics_model_rigid_body_index(rigid_body_index, 0);
            if (physics_model_rigid_body_index >= 0 && physics_model_rigid_body_index < 64) {
                state->valid_rigid_bodies.set(physics_model_rigid_body_index, true);
                get_rigid_body_transform(rigid_body_index, &state->transforms[physics_model_rigid_body_index]);
                get_rigid_body_linear_velocity(rigid_body_index, &state->linear_velocities[physics_model_rigid_body_index]);
                get_rigid_body_angular_velocity(rigid_body_index, &state->angular_velocities[physics_model_rigid_body_index]);
            }
        }
    }
}

// @address 0x2522f0 ?restore_rigid_body_state@c_havok_component@@QAAXPBUs_havok_component_rigid_body_state@@@Z
void c_havok_component::restore_rigid_body_state(const s_havok_component_rigid_body_state *state)
{
    for (long rigid_body_index = 0; rigid_body_index < get_rigid_body_count(); rigid_body_index++) {
        if (get_physics_model_rigid_body_index_count(rigid_body_index) > 0) {
            long physics_model_rigid_body_index = get_physics_model_rigid_body_index(rigid_body_index, 0);
            if (physics_model_rigid_body_index >= 0 && physics_model_rigid_body_index < 64 && state->valid_rigid_bodies.test(physics_model_rigid_body_index)) {
                phys_transform transform;
                phys_transform_from_matrix4x3(&transform, &state->transforms[physics_model_rigid_body_index]);
                set_rigid_body_transform_internal(rigid_body_index, &transform);
                set_rigid_body_linear_velocity(rigid_body_index, &state->linear_velocities[physics_model_rigid_body_index]);
                set_rigid_body_angular_velocity(rigid_body_index, &state->angular_velocities[physics_model_rigid_body_index]);
            }
        }
    }
}

// @address 0x252550 ?notify_entity_entered_phantom_material@c_havok_component@@QAAXJJPBVphys_world_object@@0J@Z
void c_havok_component::notify_entity_entered_phantom_material(long havok_component_index, long physics_model_material_index,
    const phys_world_object *phantom, const phys_world_object *entity, long material_index)
{
    long body_havok_component_index = havok_entity_get_havok_component_index((phys_world_object const *)(entity));
    long body_rigid_body_index = havok_entity_get_havok_component_rigid_body_index((phys_world_object const *)(entity));
    if (body_havok_component_index != -1 && body_havok_component_index != havok_component_index &&
        entity_belongs_in_phantom(havok_component_index, body_havok_component_index, body_rigid_body_index, material_index)) {
        long body_object_index = havok_component_get(body_havok_component_index)->get_object_index();
        if (!m_bodies_in_phantom)
            m_bodies_in_phantom = new phys_array<s_body_in_phantom>();
        phys_array<s_body_in_phantom> *bodies = m_bodies_in_phantom;
        s_body_in_phantom *matching_body = 0;
        for (long index = 0; index < bodies->getSize(); index++) {
            s_body_in_phantom *body = &(*bodies)[index];
            if (body->object_index == body_object_index && body->physics_model_material_index == physics_model_material_index &&
                body->rigid_body_index == body_rigid_body_index && body->material_index == material_index) {
                matching_body = body;
                break;
            }
        }
        if (!matching_body) {
            matching_body = &bodies->expand_one();
            matching_body->physics_model_material_index = (signed char)physics_model_material_index;
            matching_body->material_index = (signed char)material_index;
            matching_body->rigid_body_index = (signed char)body_rigid_body_index;
            matching_body->reference_count = 0;
            matching_body->object_index = body_object_index;
        }
        matching_body->reference_count++;
    }
}


class phys_motor;
static __forceinline float havok_component_stiff_spring_length(const phys_vec4 &separation)
{
    phys_vec4 length_squared;
    length_squared.m_simd_quad = __vmsum3fp(separation.m_simd_quad, separation.m_simd_quad);
    float length = phys_math::sqrt(length_squared(0));
    phys_simd_scalar simd_length = phys_simd_scalar::create(length);
    float result;
    __stvewx(simd_length.m_simd_scalar, &result, 0);
    return result;
}
static __forceinline void havok_component_stiff_spring_set_in_world_space(phys_stiff_spring_data *data,
    const phys_transform &transform_a, const phys_transform &transform_b, const phys_vec4 &pivot_a, const phys_vec4 &pivot_b)
{
    data->m_data_atoms.m_pivots_atom.m_translation_a.vec_set_transformed_inverse_pos(transform_a, pivot_a);
    data->m_data_atoms.m_pivots_atom.m_translation_b.vec_set_transformed_inverse_pos(transform_b, pivot_b);
    phys_vec4 separation;
    separation.m_simd_quad = __vsubfp(pivot_a.m_simd_quad, pivot_b.m_simd_quad);
    data->m_data_atoms.m_spring_atom.m_length = havok_component_stiff_spring_length(separation);
}

static __forceinline phys_motor *havok_component_limited_hinge_motor(const phys_limited_hinge_data *data)
{
    return data->m_data_atoms.m_ang_motor.m_schema_motor;
}

// @address 0x253a30 ?scale_constraint_motor_power@c_havok_component@@QAAXJM@Z
void c_havok_component::scale_constraint_motor_power(long constraint_index, float scale)
{
    if (is_physics_model_havok_component() && is_constraint_powered(constraint_index)) {
        c_constraint &constraint = m_component_constraints[constraint_index];
        object_datum *object = (object_datum *)object_get_and_verify_type(get_object_index(), 0xffffffff);
        s_object_definition *definition = (s_object_definition *)tag_get('obje', object->definition_index);
        s_model_definition *model = (s_model_definition *)tag_get('hlmt', definition->model.index);
        s_physics_model_definition *physics_model = (s_physics_model_definition *)tag_get('phmo', model->physics_model_index);
        s_physics_model_node_constraint_edge_constraint *constraint_reference = 0;
        for (long edge_index = 0; !constraint_reference && edge_index < physics_model->node_edges.count; edge_index++) {
            s_physics_model_node_constraint_edge *edge = tag_block_get_element<s_physics_model_node_constraint_edge>(&physics_model->node_edges, edge_index);
            for (long index = 0; !constraint_reference && index < edge->constraints.count; index++) {
                s_physics_model_node_constraint_edge_constraint *reference =
                    tag_block_get_element<s_physics_model_node_constraint_edge_constraint>(&edge->constraints, index);
                if (reference->type == constraint.get_constraint_type() && reference->index == constraint.get_constraint_type_index())
                    constraint_reference = reference;
            }
        }
        if (constraint_reference) {
            switch (constraint.get_constraint_type()) {
            case 1:
                if (constraint_reference->limited_hinge_motors.count) {
                    s_physics_model_motor_reference *motor = tag_block_get_element<s_physics_model_motor_reference>(
                        &constraint_reference->limited_hinge_motors, 0);
                    phys_limited_hinge_data *data = (phys_limited_hinge_data *)constraint.get_constraint()->get_chain_data();
                    if (motor->motor_index != -1 && havok_component_limited_hinge_motor(data))
                        scale_constraint_motor(physics_model, *motor,
                            static_cast<phys_limited_force_motor *>(havok_component_limited_hinge_motor(data)), scale);
                }
                break;
            case 2:
                if (constraint_reference->ragdoll_motors.count) {
                    s_physics_model_ragdoll_motors *motors = tag_block_get_element<s_physics_model_ragdoll_motors>(&constraint_reference->ragdoll_motors, 0);
                    phys_ragdoll_data *data = (phys_ragdoll_data *)constraint.get_constraint()->get_chain_data();
                    if (motors->twist_motor.motor_index != -1 && data->get_twist_motor())
                        scale_constraint_motor(physics_model, motors->twist_motor, static_cast<phys_limited_force_motor *>(data->get_twist_motor()), scale);
                    if (motors->cone_motor.motor_index != -1 && data->get_cone_motor())
                        scale_constraint_motor(physics_model, motors->cone_motor, static_cast<phys_limited_force_motor *>(data->get_cone_motor()), scale);
                    if (motors->plane_motor.motor_index != -1 && data->get_plane_motor())
                        scale_constraint_motor(physics_model, motors->plane_motor, static_cast<phys_limited_force_motor *>(data->get_plane_motor()), scale);
                }
                break;
            }
        }
    }
}

// @address 0x254858 ?align_constraint_positions@c_havok_component@@QAAXXZ
void c_havok_component::align_constraint_positions()
{
    c_static_flags<64> aligned_rigid_bodies;
    aligned_rigid_bodies.clear();
    if (havok_component_controlling_rigid_body_index(this) != -1)
        align_constraint_positions_recursive(havok_component_controlling_rigid_body_index(this), &aligned_rigid_bodies);
    for (long rigid_body_index = 0; rigid_body_index < get_rigid_body_count(); rigid_body_index++) {
        if (!aligned_rigid_bodies.test(rigid_body_index))
            align_constraint_positions_recursive(rigid_body_index, &aligned_rigid_bodies);
    }
}

// @address 0x257bd0 ?postprocess_physics_model_havok_component@c_havok_component@@AAAXPAUs_physics_model_instance@@@Z
void c_havok_component::postprocess_physics_model_havok_component(s_physics_model_instance *instance)
{
    m_object_postion_controlling_rigid_body_index = -1;
    m_object_postion_controlling_node_index = -1;
    long best_node_index = 0x7fffffff;
    for (long rigid_body_index = 0; rigid_body_index < get_rigid_body_count(); rigid_body_index++) {
        if (get_physics_model_rigid_body_index_count(rigid_body_index) > 0) {
            long physics_model_rigid_body_index = get_physics_model_rigid_body_index(rigid_body_index, 0);
            s_physics_model_rigid_body *physics_model_rigid_body = tag_block_get_element<s_physics_model_rigid_body>(
                &instance->physics_model->rigid_bodies, physics_model_rigid_body_index);
            if (physics_model_rigid_body->node == -1) {
                m_object_postion_controlling_rigid_body_index = (signed char)rigid_body_index;
                m_object_postion_controlling_node_index = (signed char)physics_model_rigid_body->node;
                break;
            }
            if (physics_model_rigid_body->node < best_node_index) {
                m_object_postion_controlling_rigid_body_index = (signed char)rigid_body_index;
                best_node_index = physics_model_rigid_body->node;
                m_object_postion_controlling_node_index = (signed char)best_node_index;
            }
        }
    }
    if (get_rigid_body_count() == 1 && (unsigned char)m_object_postion_controlling_node_index == 255)
        m_flags |= 64;
    else
        m_flags &= ~64;
    c_runtime_node_to_havok_component_rigid_body_index_mapping mapping(get_object_index());
    build_powered_chain_constraints(instance, mapping);
    build_constraints(instance, mapping);
}

// @address 0x259970 ??0c_rigid_body@c_havok_component@@QAA@PAVphys_rigid_body@@PBDJ_N22@Z
c_havok_component::c_rigid_body::c_rigid_body(phys_rigid_body *rigid_body, const char *physics_model_rigid_body_indices,
    long physics_model_rigid_body_index_count, bool plays_impact_effects, bool infinite_inertia_tensor, bool does_not_collide_with_environment)
{
    m_rigid_body = rigid_body;
    m_flags = 0;
    m_pre_simulation_linear_velocity = *global_zero_vector3d;
    m_pre_simulation_angular_velocity = *global_zero_vector3d;
    m_pre_simulation_position = *global_origin3d;
    m_physics_model_rigid_body_indices.set_array_size(physics_model_rigid_body_index_count);
    if (plays_impact_effects)
        m_flags |= 1;
    else
        m_flags &= ~1;
    if (infinite_inertia_tensor)
        m_flags |= 2;
    else
        m_flags &= ~2;
    if (does_not_collide_with_environment)
        m_flags |= 4;
    else
        m_flags &= ~4;
    for (long index = 0; index < physics_model_rigid_body_index_count; index++)
        m_physics_model_rigid_body_indices[index] = physics_model_rigid_body_indices[index];
}

// @address 0x2575a8 ?generate_rigid_edges_bit_vector@c_havok_component@@QAAXPBUs_physics_model_instance@@PAV?$c_static_flags@$0BAAA@@@@Z
void c_havok_component::generate_rigid_edges_bit_vector(const s_physics_model_instance *instance, c_static_flags<4096> *rigid_physics_model_edges_bit_vector)
{
    for (long edge_index = 0; edge_index < instance->physics_model->node_edges.count; edge_index++) {
        s_physics_model_node_constraint_edge *edge = tag_block_get_element<s_physics_model_node_constraint_edge>(
            &instance->physics_model->node_edges, edge_index);
        rigid_physics_model_edges_bit_vector->set(edge_index, false);
        if (edge->constraints.count == 0) {
            rigid_physics_model_edges_bit_vector->set(edge_index, true);
        } else {
            for (long index = 0; index < edge->constraints.count; index++) {
                s_physics_model_node_constraint_edge_constraint *constraint_reference = tag_block_get_element<s_physics_model_node_constraint_edge_constraint>(
                    &edge->constraints, index);
                if (constraint_reference->flags & 1) {
                    if (!object_constraint_is_loosened(
                            get_object_index(), (e_physics_model_constraint_type)constraint_reference->type, constraint_reference->index))
                    {
                        rigid_physics_model_edges_bit_vector->set(edge_index, true);
                        break;
                    }
                }
            }
        }
    }
}

// 0x255e60: creatures without a physics model get one rigid body per visible collision permutation
class phys_bvh_bytecode;
extern "C" {
// @address 0x139c k_c_mopp_bv_tree_shape_vtable
}
class phys_compound_shape;




static __forceinline void get_object_region_permutations(long object_index, long *region_count, char **permutations)
{
    object_datum *object = (object_datum *)object_get_and_verify_type(object_index, 0xffffffff);
    *region_count = object->region_information.size;
    const char *base = (const char *)object + object->region_information.offset;
    bool shifted = (bool)((object->flags >> 11) & 1);
    if (shifted)
        *region_count -= 16;
    *region_count = *region_count / 10;
    *permutations = (char *)(base + *region_count * 8);
}
static __forceinline long havok_component_creature_model_index(const s_object_definition *definition)
{
    long index = definition->model.index;
    return index;
}
static __forceinline phys_shape *havok_component_creature_scaled_shape(s_collision_bsp_physics_definition *bsp_physics,
    s_object_definition *definition, float scale)
{
    phys_vec4 aabb_center;
    aabb_center.m_simd_quad = ((c_collision_geometry_shape *)bsp_physics->m_geometry_shape)->m_aabb_center.m_simd_quad;
    aabb_center.mul4(phys_simd_scalar(scale));
    phys_vec4 aabb_half_extents;
    aabb_half_extents.m_simd_quad = ((c_collision_geometry_shape *)bsp_physics->m_geometry_shape)->m_aabb_half_extents.m_simd_quad;
    aabb_half_extents.mul4(phys_simd_scalar(scale));
    c_collision_geometry_shape *collection = new c_collision_geometry_shape(1, ((c_collision_geometry_shape *)bsp_physics->m_geometry_shape)->m_bsp, -1, -1,
        havok_component_creature_model_index(definition), scale, 0, &aabb_center, &aabb_half_extents);
    phys_shape *shape = new c_mopp_bv_tree_shape(static_cast<const phys_compound_shape *>(collection),
        ((c_mopp_bv_tree_shape *)bsp_physics->m_mopp_shape)->m_bvh_code, scale);
    collection->object_remove_ref();
    return shape;
}
template<typename T>
static __forceinline void havok_component_reserve_exactly(phys_array<T> *array, int capacity)
{
    if (array->get_sector_capacity() < capacity) {
        // match: 0x255e60: see required_capacity below
        const int &element_size = (int)_rotl(sizeof(T), 0);
        // match: 0x255e60: both const-ref rotate temporaries keep object_header_data in r14 in build_creature_component; plain ints give r14 a constant 1
        const int &required_capacity = (int)_rotl((unsigned long)capacity, 0);
        phys_array_util::reserve_array_storage(array, required_capacity, element_size);
    }
}
// @address 0x255e60 ?build_creature_component@c_havok_component@@AAAXXZ
void c_havok_component::build_creature_component()
{
    bool built = false;
    object_datum *creature = (object_datum *)object_get_and_verify_type(get_object_index(), 0x40);
    s_object_definition *definition = (s_object_definition *)tag_get('crea', creature->definition_index);
    float scale = creature->scale;
    if (definition->model.index != -1) {
        s_model_definition *model = (s_model_definition *)tag_get('hlmt', definition->model.index);
        // match: 0x255e60: zero-bit rotate preserves 32-bit GPR value
        const unsigned long &physics_model_tag = _rotl((unsigned long)model->physics_model_index, 0);
        // match: 0x255e60: zero-bit rotate preserves 32-bit GPR value
        const unsigned long &physics_model_index = _rotl(physics_model_tag, 0);
        // match: 0x255e60: const reference binding preserves temporary lifetime
        const long &physics_model = (long)_rotl(physics_model_index, 0);
        if (physics_model == -1 && model->collision_model_index != -1) {
            long rigid_body_count = 0;
            phys_rigid_body *rigid_bodies[16];
            long region_count;
            char *permutations;
            get_object_region_permutations(get_object_index(), &region_count, &permutations);
            for (long region_index = 0; region_index < region_count; region_index++) {
                short permutation_index = permutations[region_index];
                if (permutation_index == -1)
                    continue;
                s_model_region *region = tag_block_get_element<s_model_region>(&model->regions, region_index);
                const long &permutation_offset = permutation_index * (long)sizeof(s_model_region_permutation);
                s_model_region_permutation *permutation = (s_model_region_permutation *)((char *)region->permutations.address + permutation_offset);
                if (region->collision_region_index == -1 || permutation->collision_permutation_index == -1)
                    continue;
                // match: 0x255e60: binding the rotated tag index keeps model in r25 and this in r24 across the region loop
                const long &collision_model_index = (long)_rotl((unsigned long)model->collision_model_index, 0);
                collision_model_definition *collision_model = (collision_model_definition *)tag_get(
                    'coll', collision_model_index);
                s_collision_model_region *collision_region = tag_block_get_element<s_collision_model_region>(
                    &collision_model->regions, region->collision_region_index);
                s_collision_model_permutation *collision_permutation = tag_block_get_element<s_collision_model_permutation>(
                    &collision_region->permutations, permutation->collision_permutation_index);
                if (collision_permutation->bsps.count == 0 || collision_permutation->physics.count == 0)
                    continue;
                s_collision_model_bsp *bsp = tag_block_get_element<s_collision_model_bsp>(&collision_permutation->bsps, 0);
                s_collision_bsp_physics_definition *bsp_physics = tag_block_get_element<s_collision_bsp_physics_definition>(
                    &collision_permutation->physics, 0);
                phys_vec4 zero;
                zero.set(0.0f, 0.0f, 0.0f, 0.0f);
                phys_shape *shape;
                if (scale == 1.0f) {
                    shape = (c_mopp_bv_tree_shape *)bsp_physics->m_mopp_shape;
                    shape->add_object_reference();
                } else {
                    shape = havok_component_creature_scaled_shape(bsp_physics, definition, scale);
                }
                phys_mat3 inertia_tensor;
                inertia_tensor.get_column(0).m_simd_quad = zero.m_simd_quad;
                inertia_tensor.get_column(1).m_simd_quad = zero.m_simd_quad;
                inertia_tensor.get_column(2).m_simd_quad = zero.m_simd_quad;
                phys_transform transform;
                struct __declspec(align(16)) havok_component_creature_matrix { real_matrix4x3 m; } world_matrix;
                const real_matrix4x3 *matrix;
                if (bsp->node_index == -1) {
                    object_get_world_matrix(get_object_index(), &world_matrix.m);
                    matrix = &world_matrix.m;
                } else {
                    matrix = object_get_node_matrix(get_object_index(), bsp->node_index);
                }
                phys_transform_from_matrix4x3(&transform, matrix);
                phys_vec4 center_of_mass;
                center_of_mass.set(0.0f, 0.0f, 0.0f, 0.0f);
                rigid_bodies[rigid_body_count++] = havok_component_create_rigid_body(
                    this, shape, 1.0f, true, &center_of_mass, 1.0f, 1.0f, &inertia_tensor, 1.0f, 7, &transform, 1, true, false, 0.0f, 0.0f);
                shape->object_remove_ref();
            }
            if (rigid_body_count > 0) {
                char name = -1;
                havok_component_reserve_exactly(&m_rigid_bodies, rigid_body_count);
                for (long index = 0; index < rigid_body_count; index++)
                    add_rigid_body(rigid_bodies[index], &name, 1, true, true, false);
                m_object_postion_controlling_rigid_body_index = 0;
            }
            built = true;
        }
    }
    if (!built)
        m_maximum_rigid_body_size = (signed char)build_physics_model_component(true, false);
}

static __forceinline const phys_shape *havok_vehicle_physics_get_phantom_shape(const s_havok_vehicle_physics_definition *physics)
{
    if (physics->shapes.count != 0) {
        s_havok_vehicle_physics_shapes *block = ((s_havok_vehicle_physics_shapes *)physics->shapes.address);
        if (block->shape_count > 0) {
            if (block->list_shape.get_child_shape_count() <= 1)
                return &block->spheres[0];
            return &block->list_shape;
        }
    }
    return 0;
}
static __forceinline bool havok_vehicle_physics_has_suspension(const s_havok_vehicle_physics_definition *physics)
{
    bool result = false;
    if (physics->shapes.count != 0)
        result = (static_cast<unsigned long>(static_cast<s_havok_vehicle_physics_shapes *>(physics->shapes.address)->flags) >> 1) & 1;
    return result;
}
// @address 0x256388 ?build_vehicle_component@c_havok_component@@AAAXXZ
void c_havok_component::build_vehicle_component()
{
    vehicle_datum *vehicle = (vehicle_datum *)object_get_and_verify_type(get_object_index(), 2);
    vehicle_definition *definition = (vehicle_definition *)tag_get('vehi', vehicle->object.definition_index);
    bool flag = ((vehicle->vehicle.flags >> 2) & 1) != 0;
    m_maximum_rigid_body_size = (signed char)build_physics_model_component(flag, false);
    if (get_object_postion_controlling_rigid_body_index() != -1) {
        const s_havok_vehicle_physics_definition *physics = &definition->havok_vehicle_physics;
        const phys_shape *phantom_shape = havok_vehicle_physics_get_phantom_shape(physics);
        if (phantom_shape) {
            real_matrix4x3 matrix;
            phys_transform transform;
            calculate_object_transform(&matrix);
            phys_transform_from_matrix4x3(&transform, &matrix);
            m_simple_shape_phantom = new phys_simple_shape_volume(phantom_shape, transform,
                c_havok_group_filter::calcFilterInfo(15, get_object_index() + 1, 0, 0));
            havok_entity_set_havok_component_index(m_simple_shape_phantom, vehicle->object.havok_component_index);
            havok_entity_set_havok_component_rigid_body_index(m_simple_shape_phantom, -1);
        }
        if (havok_vehicle_physics_has_suspension(physics) || !vehicle_physics_disabled(get_object_index())) {
            real_rectangle3d bounds;
            if (havok_vehicle_physics_get_phantom_aabb(physics, &bounds)) {
                phys_aabb aabb;
                aabb.m_min_limit.set(bounds.x0, bounds.y0, bounds.z0, k_real_zero_storage);
                aabb.m_max_limit.set(bounds.x1, bounds.y1, bounds.z1, k_real_zero_storage);
                m_aabb_phantom = new phys_box_volume(aabb, c_havok_group_filter::calcFilterInfo(15, get_object_index() + 1, 0, 0));
                havok_entity_set_havok_component_index(m_aabb_phantom, vehicle->object.havok_component_index);
                havok_entity_set_havok_component_rigid_body_index(m_aabb_phantom, -1);
            }
        }
    }
}


static __forceinline bool biped_is_running_invisible_crouched_uber_melee(const biped_datum *biped)
{
    bool result = false;
    if (biped->biped.physics.m_mode == 6 &&
        biped->biped.physics.m_melee.is_running_invisible_crouched_uber_melee())
        result = true;
    return result;
}
static __forceinline bool biped_is_very_heavy(long object_index)
{
    biped_datum *biped = (biped_datum *)object_get_and_verify_type(object_index, 1);
    return (unsigned char)(_rotl((unsigned short)((const c_motor_program_state *)(const void *)((const char *)biped
        + biped->unit.motor_program.offset))->state, 30) & 1);
}
// @address 0x257cf8 ?build_biped_component@c_havok_component@@AAAXXZ
void c_havok_component::build_biped_component()
{
    biped_datum *biped = (biped_datum *)object_get_and_verify_type(get_object_index(), 1);
    biped_definition *definition = (biped_definition *)tag_get('bipd', biped->object.definition_index);
    s_model_definition *model = (s_model_definition *)tag_get('hlmt', definition->unit.model.index);
    bool invisible_crouched_uber_melee;
    if ((bool)((biped->unit.unit_flags >> 22) & 1) || biped_is_running_invisible_crouched_uber_melee(biped))
        invisible_crouched_uber_melee = true;
    else
        invisible_crouched_uber_melee = false;
    bool dead = biped->biped.physics.m_mode == 3;
    if (dead && model->physics_model_index != -1 && (motor_system_is_relaxed(get_object_index()) == 1 || (bool)((definition->flags >> 11) & 1))) {
        m_maximum_rigid_body_size = (signed char)build_physics_model_component(false, false);
        if (get_rigid_body_count() == 0) {
            clear_rigid_bodies();
            build_character_component(&definition->physics, &biped->biped.physics, _havok_group_biped, _havok_group_dead_biped,
                invisible_crouched_uber_melee, dead, false);
        }
    } else {
        bool very_heavy = biped_is_very_heavy(get_object_index());
        e_havok_group group = motor_system_is_relaxed(get_object_index()) == 2 ? _havok_group_ragdoll : _havok_group_dead_biped;
        build_character_component(&definition->physics, &biped->biped.physics, _havok_group_biped, group, invisible_crouched_uber_melee, dead, very_heavy);
    }
}




// @address 0x257e80 ?build_character_component@c_havok_component@@AAAXPBUs_character_physics_definition@@PAVc_character_physics_component@@W4e_havok_group@@2_N33@Z
void c_havok_component::build_character_component(const s_character_physics_definition *definition, c_character_physics_component *physics,
    e_havok_group group_a, e_havok_group group_b, bool crouched, bool dead, bool very_heavy)
{
    if (character_physics_definition_test_flag(definition, s_character_physics_definition::_force_group)) {
        group_a = _havok_group_dead_biped;
        group_b = _havok_group_dead_biped;
    }
    if (character_physics_definition_test_flag(definition, s_character_physics_definition::_no_rigid_body)) {
        m_object_postion_controlling_rigid_body_index = -1;
        return;
    }
    const s_object_header *object_header = &((s_object_header *)object_header_data->data)[(unsigned short)get_object_index()];
    long motion_type = (object_header->flags & 1) ? 5 : 7;
    char name = -1;
    real_matrix4x3 matrix = k_real_math_identity_matrix_storage;
    phys_transform transform;
    phys_vec4 zero;
    zero.set(0.0f, 0.0f, 0.0f, 0.0f);
    phys_mat3 inertia;
    const phys_shape *shape;
    if (dead) {
        shape = character_physics_definition_test_flag(definition, s_character_physics_definition::_use_spheres)
            ? static_cast<const phys_shape *>(&TAG_BLOCK_GET_ELEMENT(&definition->dead_spheres,
                  physics->get_mode_dead()->get_dead_size(),
                  s_character_physics_dead_sphere)
                      ->shape)
            : static_cast<const phys_shape *>(&TAG_BLOCK_GET_ELEMENT(&definition->dead_spheres,
                physics->get_mode_dead()->get_dead_size(),
                s_character_physics_dead_sphere)->transform);
    } else {
        shape = character_physics_definition_get_shape(definition, (s_character_physics_definition::e_shape_type)(crouched != false));
    }
    inertia.get_column(0).m_simd_quad = zero.m_simd_quad;
    inertia.get_column(1).m_simd_quad = zero.m_simd_quad;
    inertia.get_column(2).m_simd_quad = zero.m_simd_quad;
    if (!dead) m_flags |= 2; else m_flags &= ~2;
    m_flags |= 0x40;
    if (physics->is_sentinel_mode()) m_flags |= 0x20; else m_flags &= ~0x20;
    if (physics->is_sentinel_mode()) {
        matrix.position = *physics->get_mode_sentinel()->get_sentinel_physics_position();
    } else {
        object_get_origin(get_object_index(), &matrix.position);
        float z_offset;
        if (((1 << object_get_type((unsigned short)get_object_index())) & 1) != 0
            && biped_calculate_invisible_crouched_uber_melee_z_offset(get_object_index(), &z_offset))
            matrix.position.z -= z_offset;
        else
            object_type_fix_transform_to_physics(get_object_index(), &matrix);
    }
    float maximum_mass = 8000.0f;
    phys_transform_from_matrix4x3(&transform, &matrix);
    phys_vec4 center_of_mass;
    float mass = very_heavy ? maximum_mass : definition->mass;
    if (shape->m_type == 7) {
        const phys_capsule_shape *capsule = static_cast<const phys_capsule_shape *>(shape);
        center_of_mass.set_add4(capsule->get_vertex(0), capsule->get_vertex(1));
        center_of_mass.mul4(phys_simd_scalar(0.5f));
    } else if (shape->m_type == 12) {
        const phys_convex_offset_shape *transform_shape = static_cast<const phys_convex_offset_shape *>(shape);
        center_of_mass.m_simd_quad = transform_shape->get_translation().m_simd_quad;
    } else {
        center_of_mass.set(0.0f, 0.0f, 0.0f, 0.0f);
    }
    phys_rigid_body *rigid_body = havok_component_create_rigid_body(this, shape, 1.0f, false, &center_of_mass, dead ? 1.0f : 0.0f, 0.0f,
        &transform.m_rotation, mass, motion_type, &transform, dead ? group_b : group_a, true, false, 0.0f, 0.0f);
    if (!rigid_body->is_fixed_or_keyframed())
        rigid_body->set_local_inv_inertia(inertia);
    add_rigid_body(rigid_body, &name, 1, true, true, false);
    m_object_postion_controlling_rigid_body_index = 0;
}


/* c_runtime_node_to_physics_model_rigid_body_index_mapping (17476 bytes) */
static __forceinline long object_get_ultimate_parent(long object_index)
{
    long result = -1;
    while (object_index != -1) {
        result = object_index;
        object_index = object_get(object_index)->parent_object_index;
    }
    return result;
}
/* Map the tag motion type to the Havok motion type. */
static __forceinline long rigid_body_motion_type_to_havok_motion_type(short motion_type)
{
    long result;
    switch (motion_type) {
    case 0: result = 2; break;
    case 1: result = 3; break;
    case 2: result = 4; break;
    case 3: result = 5; break;
    case 4: result = 6; break;
    case 5: result = 7; break;
    // match: 0x2565d0: __assume(0) drops the motion_type > 5 bounds check (cmplwi/bgt) and the stack result slot in build_physics_model_component
    default: __assume(0);
    }
    return result;
}
/* a nonzero penetration depth scale, else 1.0f (zero and NaN): two branch-free fsel selects, x < 0 then -x < 0 */
static __forceinline float havok_component_penetration_depth_scale(float scale)
{
    float result = scale < 0.0f ? scale : 1.0f;
    return -scale < 0.0f ? scale : result;
}
// @address 0x259fd0 ??0c_runtime_node_to_physics_model_rigid_body_index_mapping@@QAA@J_N@Z
// @address 0x2de920 ?physics_model_instance_new@@YA_NPAUs_physics_model_instance@@J@Z
// @address 0x256920 ?build_physics_model_rigid_body_for_node@c_havok_component@@AAA_NPBUs_physics_model_instance@@JPAW4e_rigid_body_size@@PAV?$c_static_flags@$0PP@@@PBDPBVc_runtime_node_to_physics_model_rigid_body_index_mapping@@PAUs_rigid_body_construction_template@1@@Z
bool c_havok_component::build_physics_model_rigid_body_for_node(const s_physics_model_instance *instance, long node_index,
    e_rigid_body_size *rigid_body_size, c_static_flags<255> *nodes_currently_in_graph_bit_vector,
    const char *rigid_body_motion_types, const c_runtime_node_to_physics_model_rigid_body_index_mapping *mapping,
    s_rigid_body_construction_template *rigid_body_construction_template)
{
    bool result = false;
    long rigid_body_indexes_count = 0;
    bool constrained_to_world = false;
    bool plays_impact_effects = true;
    bool has_phantom_in_shape = false;
    bool does_not_interact_with_environment = false;
    float mass;
    phys_vec4 center_of_mass;
    phys_mat3 inertia_tensor;
    char rigid_body_indexes[64];
    const phys_shape *shape = build_physics_model_rigid_body_shape(instance, rigid_body_motion_types, rigid_body_indexes, &rigid_body_indexes_count, mapping,
        node_index,
        nodes_currently_in_graph_bit_vector, &mass, &center_of_mass, &inertia_tensor, &constrained_to_world, rigid_body_size,
        &plays_impact_effects, &has_phantom_in_shape, &does_not_interact_with_environment);
    if (shape) {
        long first_rigid_body_index = rigid_body_indexes[0];
        const s_physics_model_rigid_body *first_rigid_body = tag_block_get_element<s_physics_model_rigid_body>(
            &instance->physics_model->rigid_bodies, first_rigid_body_index);
        bool infinite_inertia_tensor = false;
        long motion_type = constrained_to_world ? 7 : rigid_body_motion_types[first_rigid_body_index];
        long havok_group = havok_component_get_collision_group(this, instance, *rigid_body_size, motion_type, first_rigid_body_index);
        phys_transform transform;
        if (motion_type >= 1 && motion_type <= 5)
            infinite_inertia_tensor = TEST_BIT(first_rigid_body->flags, 6);
        phys_transform_from_matrix4x3(&transform, instance->get_node_matrix(first_rigid_body->node));
        phys_rigid_body *rigid_body = havok_component_create_rigid_body(this, shape,
            havok_component_penetration_depth_scale(instance->physics_model->maximum_penetration_depth_scale),
            motion_type == 7 || motion_type == 6, &center_of_mass, 1.0f, 0.0f, &inertia_tensor, mass, motion_type, &transform, havok_group,
            TEST_BIT(first_rigid_body->flags, 0) || TEST_BIT(1 << object_get_type((unsigned short)get_object_index()), 1), TEST_BIT(first_rigid_body->flags, 1),
            first_rigid_body->linear_damping, first_rigid_body->angular_damping);
        ((phys_ref_object *)shape)->object_remove_ref();
        rigid_body_construction_template->rigid_body = rigid_body;
        memcpy(rigid_body_construction_template->physics_model_rigid_body_indices, rigid_body_indexes, rigid_body_indexes_count);
        rigid_body_construction_template->physics_model_rigid_body_index_count = rigid_body_indexes_count;
        rigid_body_construction_template->flag_a = plays_impact_effects;
        rigid_body_construction_template->flag_b = infinite_inertia_tensor;
        rigid_body_construction_template->flag_c = does_not_interact_with_environment;
        if (infinite_inertia_tensor && !rigid_body->is_fixed_or_keyframed()) {
            phys_vec4 zero;
            zero.set(0.0f, 0.0f, 0.0f, 0.0f);
            phys_mat3 inverse_inertia;
            inverse_inertia.set_cols(zero, zero, zero);
            rigid_body->set_local_inv_inertia(inverse_inertia);
        }
        result = true;
        if (has_phantom_in_shape)
            m_flags |= 0x200;
    }
    return result;
}
// @address 0x2565d0 ?build_physics_model_component@c_havok_component@@AAA?AW4e_rigid_body_size@@_N0@Z
e_rigid_body_size c_havok_component::build_physics_model_component(bool fixed, bool keyframe_unflagged_nodes)
{
    long rigid_body_size_max = 0;
    s_object_header *object_header = object_header_get(get_object_index());
    s_object_header *parent_header = object_header_get(object_get_ultimate_parent(get_object_index()));
    if ((parent_header->flags & 1) == 0 && ((1 << object_header->object_type) & 0x80) == 0)
        fixed = true;
    s_physics_model_instance instance;
    if (physics_model_instance_new(&instance, get_object_index())) {
        c_runtime_node_to_physics_model_rigid_body_index_mapping mapping(get_object_index(), false);
        c_static_flags<255> nodes_done;
        nodes_done.clear();
        char rigid_body_motion_types[64];
        s_rigid_body_construction_template construction_templates[64];
        for (long rigid_body_index = 0; rigid_body_index < instance.physics_model->rigid_bodies.count(); rigid_body_index++) {
            s_physics_model_rigid_body *rigid_body = tag_block_get_element<s_physics_model_rigid_body>(&instance.physics_model->rigid_bodies, rigid_body_index);
            long motion_type = ::rigid_body_motion_type_to_havok_motion_type(rigid_body->motion_type);
            long adjusted = fixed ? 7 : motion_type;
            if (keyframe_unflagged_nodes && adjusted != 7) {
                if (rigid_body->node == -1 ||
                    (tag_block_get_element<s_physics_model_node>(&instance.physics_model->nodes, rigid_body->node)->flags & 1) == 0)
                    adjusted = 6;
            }
            rigid_body_motion_types[rigid_body_index] = (char)adjusted;
        }
        long construction_templates_count = 0;
        long rigid_body_size = 0;
        if (build_physics_model_rigid_body_for_node(&instance, -1, (e_rigid_body_size *)&rigid_body_size, &nodes_done,
                rigid_body_motion_types, &mapping, &construction_templates[construction_templates_count]))
        {
            construction_templates_count++;
            rigid_body_size_max = MAX(rigid_body_size_max, rigid_body_size);
        } else {
            for (long node_index = 0; node_index < instance.physics_model->nodes.count; node_index++) {
                if (!nodes_done.test(node_index)) {
                    rigid_body_size = 0;
                    if (build_physics_model_rigid_body_for_node(&instance, node_index, (e_rigid_body_size *)&rigid_body_size, &nodes_done,
                            rigid_body_motion_types, &mapping, &construction_templates[construction_templates_count]))
                    {
                        construction_templates_count++;
                        rigid_body_size_max = MAX(rigid_body_size_max, rigid_body_size);
                    }
                }
            }
        }
        if (construction_templates_count > 0) {
            m_rigid_bodies.reserve_exactly(construction_templates_count);
            for (long template_index = 0; template_index < construction_templates_count; template_index++)
                add_rigid_body(construction_templates[template_index].rigid_body,
                    construction_templates[template_index].physics_model_rigid_body_indices,
                    construction_templates[template_index].physics_model_rigid_body_index_count,
                    construction_templates[template_index].flag_a, construction_templates[template_index].flag_b,
                    construction_templates[template_index].flag_c);
        }
        postprocess_physics_model_havok_component(&instance);
        m_flags |= 0x100;
    }
    return (e_rigid_body_size)rigid_body_size_max;
}

static __forceinline void crate_set_flag(crate_datum *crate, long bit, bool value)
{
    if (value)
        crate->flags |= 1 << bit;
    else
        crate->flags &= ~(1 << bit);
}
// @address 0xe2c88 ?havok_object_set_velocities@@YAXJPBTreal_vector3d@@0_N@Z
// @address 0x250708 ?havok_component_construct@@YAXJ@Z
void havok_component_construct(long havok_component_index)
{
    c_havok_component *component = havok_component_get(havok_component_index);
    s_object_header *header = object_header_get(component->get_object_index());
    object_datum *object = header->datum;
    unsigned char header_flag = (unsigned char)(header->flags & 1);
    long type_mask = 1 << object->object_type;
    if ((type_mask & 0x400) != 0) {
        crate_datum *crate = (crate_datum *)object_get_and_verify_type(component->get_object_index(), 0x400);
        component->m_maximum_rigid_body_size = (signed char)component->build_physics_model_component(false, false);
        crate_set_flag(crate, 0, component->get_maximum_rigid_body_size() <= 1);
    } else if ((type_mask & 0x40) != 0) {
        component->build_creature_component();
    } else if ((type_mask & 0x1) != 0) {
        component->build_biped_component();
    } else if ((type_mask & 0x800) != 0) {
        creature_datum *creature = (creature_datum *)object_get_and_verify_type(component->get_object_index(), 0x800);
        creature_definition *definition = (creature_definition *)tag_get('gint', creature->object.definition_index);
        component->build_character_component(&definition->physics, &creature->creature.physics, _havok_group_giant, _havok_group_giant, false,
            creature->creature.physics.m_mode == 3, false);
    } else if ((type_mask & 0x80) != 0) {
        component->m_flags |= 16;
        component->m_maximum_rigid_body_size = (signed char)component->build_physics_model_component(false, true);
    } else if ((type_mask & 0x2) != 0) {
        component->build_vehicle_component();
    } else if ((type_mask & 0xc) != 0) {
        component->m_maximum_rigid_body_size = (signed char)component->build_physics_model_component(false, false);
    } else if ((type_mask & 0x20) != 0) {
        component->build_projectile_component();
    } else if ((type_mask & 0x1000) != 0) {
        s_giant_datum *datum = (s_giant_datum *)object_get_and_verify_type(
            component->get_object_index(), 0x1000);
        component->m_flags |= 16;
        component->m_maximum_rigid_body_size = (signed char)component->build_physics_model_component((unsigned char)((datum->giant_flags >> 7) & 1), true);
    } else {
        component->m_maximum_rigid_body_size = (signed char)component->build_physics_model_component(false, false);
    }
    havok_object_set_velocities(component->get_object_index(), &object->linear_velocity, &object->angular_velocity, false);
    if (header_flag)
        component->m_flags |= 0x1000;
    else
        component->m_flags &= ~0x1000;
    for (long rigid_body_index = 0; rigid_body_index < component->get_rigid_body_count(); rigid_body_index++) {
        if (!component->get_rigid_body_is_fixed(rigid_body_index)) {
            component->m_flags |= 1;
            break;
        }
    }
}

static __forceinline long havok_component_impacts_get_highest_score_datum_index()
{
    long result = -1;
    if (g_impacts_globals->highest_score_index != -1) {
        long datum_index = g_impacts_globals->sorted_indexes[g_impacts_globals->highest_score_index];
        while (g_impacts_globals->highest_score_index >= 0) {
            if (g_impacts_globals->sorted_indexes[g_impacts_globals->highest_score_index] != -1)
                break;
            g_impacts_globals->highest_score_index = g_impacts_globals->highest_score_index - 1;
        }
        result = datum_index;
    }
    return result;
}
static __forceinline void havok_component_impacts_pop_highest_score_datum_index()
{
    g_impacts_globals->highest_score_index = g_impacts_globals->highest_score_index > 0 ? g_impacts_globals->highest_score_index - 1 : -1;
}
// @address 0x255b00 ?make_space_for_new_impact@c_havok_component@@QAA_NMJ@Z
bool c_havok_component::make_space_for_new_impact(float score, long rigid_body_index)
{
    long lowest_score_index = -1;
    bool result = false;
    if (rigid_body_index == -1 || m_rigid_bodies[rigid_body_index].plays_impact_effects()) {
        if (get_impact_count() == 32) {
            for (long index = 0; index < get_impact_count(); index++) {
                c_impact *impact = impact_get(havok_component_impact_array_impact_get(m_impact_array_index, index));
                if (impact->score > score)
                    lowest_score_index = index;
            }
        }
        if (get_impact_count() < 32 || lowest_score_index != -1) {
            if (lowest_score_index != -1) {
                impact_delete(havok_component_impact_array_impact_get(m_impact_array_index, lowest_score_index));
            } else if (tls_impact_data()->actual_count == 32) {
                long highest_score_datum_index = havok_component_impacts_get_highest_score_datum_index();
                if (highest_score_datum_index != -1) {
                    if (havok_component_impact_datum_get(highest_score_datum_index)->score > score) {
                        impact_delete(highest_score_datum_index);
                        havok_component_impacts_pop_highest_score_datum_index();
                    }
                }
            }
            result = tls_impact_data()->actual_count < 32 && get_impact_count() < 32;
        }
    }
    return result;
}
// match: 0x5f8090: size optimization keeps the three s3tc functions on savegprlr_26 frames; at the default level they save from r23 and reschedule
#pragma optimize("s", on)
// @address 0x5f8090 s3tc_compute_dxt5a_palette
extern "C" void s3tc_compute_dxt5a_palette(unsigned short *colors, unsigned char color_a, unsigned char color_b)
{
  colors[0] = color_a * 0x101;
  colors[1] = color_b * 0x101;
  colors[2] = color_a * 216 + color_b * 40;
  colors[3] = color_b * 72 + color_a * 184;
  colors[4] = color_b * 112 + color_a * 144;
  colors[5] = color_b * 144 + color_a * 112;
  colors[6] = color_b * 184 + color_a * 72;
  colors[7] = color_b * 216 + color_a * 40;
}
// @address 0x5f8118 s3tc_find_closest_palette_entry
extern "C" inline long s3tc_find_closest_palette_entry(const unsigned short *colors, float value)
{
    long best_index = 0;
    float delta = value - colors[0] * (1.0f / 65535.0f);
    float best_distance = delta * delta;
    for (long index = 1; index < 8; index++) {
        float d = value - colors[index] * (1.0f / 65535.0f);
        float distance = d * d;
        if (distance < best_distance) {
            best_distance = distance;
            best_index = index;
        }
    }
    return best_index;
}
// @address 0x5f8258 s3tc_encode_dxt5a_block
extern "C" void s3tc_encode_dxt5a_block(unsigned char *block, float *values, long width, long height)
{
    unsigned short colors[8];
    unsigned char indices[16];
    float maximum = values[0];
    float minimum = maximum;
    if (height > 0) {
        float *row = values;
        do {
            if (width > 0) {
                float *value = row;
                long x = width;
                do {
                    if (*value < k_havok_pack_minimum)
                        *value = k_havok_pack_minimum;
                    else if (*value > k_havok_pack_maximum)
                        *value = k_havok_pack_maximum;
                    if (*value < minimum)
                        minimum = *value;
                    if (*value > maximum)
                        maximum = *value;
                    value++;
                    x--;
                } while (x != 0);
            }
            height--;
            row += 4;
        } while (height != 0);
    }
    float minimum255 = minimum * 255.0f;
    float maximum255 = maximum * 255.0f;
    long low = (long)(minimum * 255.0f + k_havok_phantom_byte_rounding_bias);
    long high = (long)(maximum * 255.0f + k_havok_phantom_byte_rounding_bias);
    if (low == high) {
        low = (long)minimum255;
        high = (long)(float)ceil(maximum255);
        if (high > 255)
            high = 255;
    }
    if (high < low) {
        long swap = low;
        low = high;
        high = swap;
    } else if (high == low) {
        if (high < 255)
            high++;
        else
            low--;
    }
    s3tc_compute_dxt5a_palette(colors, (unsigned char)high, (unsigned char)low);
    for (long y = 0; y < 4; y++) {
        for (long x = 0; x < 4; x++)
            indices[y * 4 + x] = (unsigned char)s3tc_find_closest_palette_entry(colors, values[x]);
        values += 4;
    }
    block[1] = colors[0] >> 8;
    block[0] = colors[1] >> 8;
    block[3] = ((indices[2] << 3 | indices[1]) << 3) | indices[0];
    block[2] = (unsigned char)((((indices[5] << 3 | indices[4]) << 3) | indices[3]) << 1) | (indices[2] >> 2);
    block[5] = (unsigned char)((indices[7] << 3 | indices[6]) << 2) | (indices[5] >> 1);
    block[4] = ((indices[10] << 3 | indices[9]) << 3) | indices[8];
    block[7] = (unsigned char)((((indices[13] << 3 | indices[12]) << 3) | indices[11]) << 1) | (indices[10] >> 2);
    block[6] = (unsigned char)((indices[15] << 3 | indices[14]) << 2) | (indices[13] >> 1);
}
// match: 0x5f8258: ends the size-optimized s3tc block; the units after it compile at the default optimization
#pragma optimize("", on)

// @address 0xec098 ?rigid_body_apply_acceleration@c_havok_component@@QAAXJPBTreal_vector3d@@@Z
void c_havok_component::rigid_body_apply_acceleration(long rigid_body_index, const real_vector3d *acceleration)
{
    real_vector3d velocity;
    get_rigid_body_linear_velocity( rigid_body_index, &velocity);
    velocity.i = velocity.i + acceleration->i;
    velocity.j = velocity.j + acceleration->j;
    velocity.k = velocity.k + acceleration->k;
    set_rigid_body_linear_velocity(rigid_body_index, &velocity);
}



// @address 0x250b60 havok_component_delete_phantom_bodies
// Other compilers only: on Xbox the compiler emits the destructor with its scalar deleting destructor
// (havok_component_scalar_delete) and hkArray<s_body_in_phantom>'s scalar deleting destructor
// (havok_component_delete_phantom_bodies), which the callers reach under these names.
#if !defined(_XBOX) || defined(__clang__)
c_havok_component::~c_havok_component()
{
    if (is_connected_to_world())
        disconnect_from_havok_world();
    clear_rigid_bodies();
    if (m_bodies_in_phantom) {
        havok_component_delete_phantom_bodies(m_bodies_in_phantom, 1);
        m_bodies_in_phantom = 0;
    }
}

// Destroys without freeing: the component data array owns the storage.
extern "C" void havok_component_scalar_delete(c_havok_component *component, long flag)
{
    component->~c_havok_component();
}

extern "C" void *havok_component_delete_phantom_bodies(phys_array<c_havok_component::s_body_in_phantom> *array, unsigned int flags)
{
    // Always frees the array: every caller passes flags 1.
    delete array;
    return array;
}
#endif

// @address 0x252758 ?notify_entity_left_phantom_material@c_havok_component@@QAAXJJPBVphys_world_object@@0J@Z
void c_havok_component::notify_entity_left_phantom_material(long havok_component_index, long physics_model_material_index,
    const phys_world_object *phantom, const phys_world_object *entity, long material_index)
{
    long body_havok_component_index = havok_entity_get_havok_component_index((phys_world_object const *)(entity));
    long body_rigid_body_index = havok_entity_get_havok_component_rigid_body_index((phys_world_object const *)(entity));
    if (body_havok_component_index != -1 && body_havok_component_index != havok_component_index && m_bodies_in_phantom) {
        long body_object_index = havok_component_get(body_havok_component_index)->get_object_index();
        phys_array<c_havok_component::s_body_in_phantom> *bodies = m_bodies_in_phantom;
        for (long index = 0; index < bodies->getSize(); index++) {
            c_havok_component::s_body_in_phantom &body = (*bodies)[index];
            if (body.object_index == body_object_index && body.physics_model_material_index == physics_model_material_index &&
                body.rigid_body_index == body_rigid_body_index && body.material_index == material_index) {
                if (--body.reference_count == 0) {
                    bodies->m_size--;
                    bodies->m_data[index] = bodies->m_data[bodies->m_size];
                }
                break;
            }
        }
        if (bodies->getSize() == 0) {
            if (m_bodies_in_phantom)
                havok_component_delete_phantom_bodies(m_bodies_in_phantom, 1);
            m_bodies_in_phantom = 0;
        }
    }
}

static __forceinline const phys_collider *havok_component_get_root_collidable(const phys_collision_proxy *body)
{
    while (body->m_parent)
        body = body->m_parent;
    return (const phys_collider *)body;
}
static __forceinline phys_world_object *havok_component_get_world_object(const phys_collider *collidable)
{
    return (phys_world_object *)collidable->get_collider_owner();
}
extern "C" {
// @address 0x8c0 k_maximum_collector_distance
}
// @address 0x2528b8 ?update_phantoms_which_intersect_shape_phantom@c_havok_component@@QAAXXZ
void c_havok_component::update_phantoms_which_intersect_shape_phantom()
{
    {
    class c_point_collector : public phys_hit_collector {
    public:
        c_point_collector(s_phantom_body *body_storage, long capacity, long *body_count, long owner_object_index) :
            bodies(body_storage), object_index(owner_object_index), maximum_count(capacity), count(body_count)
        {
            *count = 0;
        }
        virtual void add_hit(const phys_hit_point &point) {}
        // @address 0x2529d8 ?add_volume_callback_shape@c_point_collector@?2??update_phantoms_which_intersect_shape_phantom@c_havok_component@@QAAXXZ@UAAXABVphys_collision_proxy@@@Z
        virtual void add_volume_callback_shape(const phys_collision_proxy &cd_body_ref)
        {
            const phys_collision_proxy *cd_body = &cd_body_ref;
    phys_world_object *object = havok_component_get_world_object(havok_component_get_root_collidable(cd_body));
    const phys_shape *shape = cd_body->m_proxy_shape;
    long havok_component_index = havok_entity_get_havok_component_index((phys_world_object const *)(object));
    if (havok_component_index != -1) {
        c_havok_component *component = havok_component_get(havok_component_index);
        long rigid_body_index = havok_entity_get_havok_component_rigid_body_index((phys_world_object const *)(object));
        if (*count < maximum_count) {
            object_datum *phantom_object = (object_datum *)object_get_and_verify_type(object_index, 0xffffffff);
            if (entity_belongs_in_phantom(havok_component_index, phantom_object->havok_component_index, -1, shape->m_user_data)) {
                bool found = false;
                for (long index = 0; index < *count; index++) {
                    s_phantom_body *body = &bodies[index];
                    if (body->material_index == (long)shape->m_user_data && body->rigid_body_index == -1 &&
                        body->physics_model_material_index == rigid_body_index && body->object_index == object_index &&
                        body->body_object_index == component->get_object_index()) {
                        found = true;
                        break;
                    }
                }
                if (!found) {
                    s_phantom_body *body = &bodies[(*count)++];
                    body->material_index = (signed char)shape->m_user_data;
                    body->rigid_body_index = -1;
                    body->physics_model_material_index = (signed char)rigid_body_index;
                    body->object_index = object_index;
                    body->body_object_index = component->get_object_index();
                    body->reference_count = 1;
                }
            }
        }
    }
        }
        s_phantom_body *bodies;
        long object_index;
        long maximum_count;
        long *count;
    };
    phys_simple_shape_volume *phantom = m_simple_shape_phantom;
    long object_index = m_object_index;
    bool accelerated;
    long count;
    count = 0;
    bool updated;
    bool was_active;
    float maximum_scale;
    s_phantom_body bodies[16];
    accelerated = false;
    maximum_scale = k_real_zero_storage;
    c_point_collector collector(bodies, 16, &count, object_index);
    phantom->get_closest_points(collector);
    for (long index = 0; index < count; index++) {
        s_phantom_body *body = &bodies[index];
        object_datum *object = (object_datum *)object_get_and_verify_type(body->body_object_index, 0xffffffff);
        bool enabled = ((object->physics_flags >> 17) & 1) != 0;
        if (enabled != 0) {
            update_phantom(havok_component_get(object->havok_component_index)->get_object_index(), enabled,
                (const s_body_in_phantom &)*body, false, maximum_scale, accelerated, updated, was_active);
        }
    }
    }
}

/* ---- 0x254d48: realign the component's rigid bodies to the object's transform ---- */
// @address 0x254d48 havok_component_update_body_transforms
extern "C" void havok_component_update_body_transforms(c_havok_component *component, long havok_component_index, bool unused, bool keep_rotation)
{
    phys_transform transform;
    real_matrix4x3 matrix;
    bool constrained_to_world = component->is_constrained_to_world_bit();
    if (constrained_to_world) {
        bool connected = component->is_connected_to_world();
        if (connected)
            component->disconnect_from_havok_world();
        component->clear_rigid_bodies();
        object_compute_node_matrices(component->get_object_index());
        havok_component_construct(havok_component_index);
        if (connected)
            component->connect_to_havok_world();
    } else {
        bool one_body = component->is_one_body_object_space_component();
        if (one_body) {
            object_get_world_matrix(component->get_object_index(), &matrix);
            long object_index = component->get_object_index();
            float z_offset;
            if (((1 << object_get_type((unsigned short)object_index)) & 1) != 0 &&
                biped_calculate_invisible_crouched_uber_melee_z_offset(object_index, &z_offset))
                matrix.position.z -= z_offset;
            else
                object_type_fix_transform_to_physics(object_index, (real_matrix4x3 *)(&matrix));
            if (component->get_rigid_body_has_infinite_inertia_tensor(0) && !keep_rotation)
                matrix.rotation = k_real_math_identity_matrix_storage.rotation;
            phys_transform_from_matrix4x3(&transform, &matrix);
            component->set_rigid_body_transform_internal(0, &transform);
        } else {
            real_matrix4x3 delta;
            struct __declspec(align(16)) aligned_matrix4x3 { real_matrix4x3 m; } object_transform;
            component->calculate_object_transform(&object_transform.m);
            real_matrix4x3 matrix;
            object_get_world_matrix(component->get_object_index(), &matrix);
            real_matrix4x3 result;
            matrix4x3_inverse(&object_transform.m, &result);
            matrix4x3_compute_product(&matrix, &result, &delta);
            for (long rigid_body_index = 0; rigid_body_index < component->get_rigid_body_count(); rigid_body_index++) {
                component->get_rigid_body_transform(rigid_body_index, &matrix);
                matrix4x3_compute_product(&delta, &matrix, &result);
                matrix3x3_normalize(&result.rotation);
                havok_component_hkTransform_from_matrix4x3(&transform, &result);
                component->set_rigid_body_transform_internal(rigid_body_index, &transform);
            }
            component->m_flags |= 0x800;
        }
    }
}

// The rigid body that maps to a node index; node -1 is the mapping's own unmapped body (0x25a160's inline).
static __forceinline long havok_component_rigid_body_index(const c_runtime_node_to_havok_component_rigid_body_index_mapping *mapping, long node_index)
{
    if (node_index == -1)
        return mapping->m_unmapped_rigid_body;
    return mapping->m_node_to_rigid_body_mapping[node_index];
}

// @address 0x259190 ?calculate_constraint_transforms@c_havok_component@@AAAXPBUs_physics_model_instance@@PBVc_runtime_node_to_havok_component_rigid_body_index_mapping@@PAVphys_transform@@2PAPAVphys_rigid_body@@3PBUs_physics_model_constraint_bodies@@@Z
void c_havok_component::calculate_constraint_transforms(const s_physics_model_instance *physics_model_instance,
    const c_runtime_node_to_havok_component_rigid_body_index_mapping *runtime_node_to_havok_component_rigid_body_index_mapping,
    // match: 0x259190: pointer __restrict qualifier prevents alias reload
    phys_transform *hk_transform, phys_transform *hk_transform2, phys_rigid_body ** __restrict rigid_body_a, phys_rigid_body ** __restrict rigid_body_b,
    const s_physics_model_constraint_bodies *physics_model_constraint_bodies)
{
    long node_index_a = physics_model_constraint_bodies->node_a;
    long rigid_body_index_a = node_index_a == -1
        ? runtime_node_to_havok_component_rigid_body_index_mapping->m_unmapped_rigid_body
        : runtime_node_to_havok_component_rigid_body_index_mapping->m_node_to_rigid_body_mapping[node_index_a];
    long physics_model_rigid_body_index_count_a = get_physics_model_rigid_body_index_count(rigid_body_index_a);
    s_physics_model_rigid_body *physics_model_rigid_body_a = tag_block_get_element<s_physics_model_rigid_body>(
        &physics_model_instance->physics_model->rigid_bodies, get_physics_model_rigid_body_index(rigid_body_index_a, 0));
    for (long index = 0; index < physics_model_rigid_body_index_count_a; index++) {
        if (tag_block_get_element<s_physics_model_rigid_body>(&physics_model_instance->physics_model->rigid_bodies,
                get_physics_model_rigid_body_index(rigid_body_index_a, index))->node == node_index_a)
            break;
    }
    if (physics_model_rigid_body_a->node != node_index_a) {
        real_matrix4x3 transform_matrix;
        calculate_transform_between_nodes_on_rigid_body(rigid_body_index_a, physics_model_rigid_body_a->node, node_index_a, &transform_matrix);
        real_matrix4x3 matrix;
        matrix4x3_compute_product(&transform_matrix, &physics_model_constraint_bodies->transform_a, &matrix);
        havok_component_hkTransform_from_matrix4x3(hk_transform, &matrix);
    } else {
        havok_component_hkTransform_from_matrix4x3(hk_transform, &physics_model_constraint_bodies->transform_a);
    }
    long node_index_b = physics_model_constraint_bodies->node_b;
    c_rigid_body *rigid_bodies = m_rigid_bodies.m_data;
    *rigid_body_a = m_rigid_bodies[rigid_body_index_a].m_rigid_body;
    if (node_index_b != -1) {
        int rigid_body_index_b = node_index_b == -1
            ? runtime_node_to_havok_component_rigid_body_index_mapping->m_unmapped_rigid_body
            : runtime_node_to_havok_component_rigid_body_index_mapping->m_node_to_rigid_body_mapping[node_index_b];
        if (rigid_body_index_b != -1) {
            int physics_model_rigid_body_index_count_b = get_physics_model_rigid_body_index_count(rigid_body_index_b);
            s_physics_model_rigid_body *physics_model_rigid_body_b = tag_block_get_element<s_physics_model_rigid_body>(
                &physics_model_instance->physics_model->rigid_bodies, get_physics_model_rigid_body_index(rigid_body_index_b, 0));
            for (long index = 0; index < physics_model_rigid_body_index_count_b; index++) {
                if (tag_block_get_element<s_physics_model_rigid_body>(&physics_model_instance->physics_model->rigid_bodies,
                        rigid_bodies[rigid_body_index_b].m_physics_model_rigid_body_indices[index])->node == node_index_b)
                    break;
            }
            if (physics_model_rigid_body_b->node != node_index_b) {
                real_matrix4x3 transform_matrix;
                calculate_transform_between_nodes_on_rigid_body(rigid_body_index_b, physics_model_rigid_body_b->node, node_index_b, &transform_matrix);
                real_matrix4x3 matrix;
                matrix4x3_compute_product(&physics_model_constraint_bodies->transform_b, &transform_matrix, &matrix);
                havok_component_hkTransform_from_matrix4x3(hk_transform2, &matrix);
            } else {
                havok_component_hkTransform_from_matrix4x3(hk_transform2, &physics_model_constraint_bodies->transform_b);
            }
            *rigid_body_b = m_rigid_bodies[rigid_body_index_b].m_rigid_body;
            return;
        }
    }
    ((phys_transform *)(hk_transform2))->set_mul((*(phys_transform const *)(&(*rigid_body_a)->get_transform())), (*(phys_transform const *)(hk_transform)));
    *rigid_body_b = g_havok_world->m_fixed_body;
}

extern "C" {
struct vehicle_type_component_component;

}
// @address 0xdf3f0 ?pre_simulation_update@c_havok_component@@QAAXXZ
void c_havok_component::pre_simulation_update()
{
    if (is_connected_to_world() && havok_component_has_active_entity(this) != 0) {
        real_vector3d gravity;
        float gravity_acceleration = global_physics_constants_get()->gravity;
        float seconds_per_tick = game_time_globals->tick_length;
        gravity.i = k_real_zero_storage;
        gravity.j = k_real_zero_storage;
        float scale = 0.0f;
        gravity.k = -(seconds_per_tick * gravity_acceleration);
        object_set_requires_motion(m_object_index);
        if ((unsigned char)((m_flags >> 1) & 1) == 0) {
            long object_type_mask = 1 << object_get_type((unsigned short)m_object_index);
            if ((object_type_mask & 2) != 0) {
                vehicle_datum *vehicle = (vehicle_datum *)object_get_and_verify_type(
                    m_object_index, 0xffffffff);
                if (vehicle->unit.throttle_scale != k_real_zero_storage) {
                    scale = ((vehicle_definition *)tag_get('vehi', vehicle->object.definition_index))
                                ->havok_vehicle_physics.gravity_scale;
                    gravity.i = scale * gravity.i;
                    gravity.j = scale * gravity.j;
                    gravity.k = gravity.k * scale;
                    ((c_vehicle_type_component *)vehicle->vehicle.type_state)->adjust_gravity(m_object_index, &gravity);
                }
            }
            if ((object_type_mask & 12) != 0)
                item_adjust_gravity(m_object_index, &gravity);
            if (current_game_engine() != 0) {
                const c_game_engine *engine = current_game_engine();
                engine->adjust_object_gravity(m_object_index, &gravity);
            }
        }
        for (long rigid_body_index = 0; rigid_body_index < get_rigid_body_count(); rigid_body_index++) {
            if (m_rigid_bodies[rigid_body_index].m_rigid_body->m_motion.m_type != 6) {
                real_vector3d linear_velocity;
                real_vector3d angular_velocity;
                get_rigid_body_linear_velocity( rigid_body_index, &linear_velocity);
                get_rigid_body_angular_velocity(rigid_body_index, &angular_velocity);
#if defined(_XBOX) && !defined(__clang__)
                // match: 0xdf3f0: 100.0f is read off the k_real_zero_storage base already held for the gravity stores;
                // a plain literal loads its own base and the function grows by 12 bytes
                if (limit3d(&linear_velocity, (&k_real_zero_storage)[418]) != 0)
#else
                if (limit3d(&linear_velocity, 100.0f) != 0)
#endif
                    set_rigid_body_linear_velocity(rigid_body_index, &linear_velocity);
                if (limit3d(&angular_velocity, k_havok_component_maximum_angular_velocity) != 0)
                    set_rigid_body_angular_velocity(rigid_body_index, &angular_velocity);
            }
            update_rigid_body_pre_simulation_state(rigid_body_index);
            if (((phys_entity const *)(m_rigid_bodies[rigid_body_index].m_rigid_body))->is_body_active() && (unsigned char)((m_flags >> 1) & 1) == 0) {
                unsigned char motion_type = m_rigid_bodies[rigid_body_index].m_rigid_body->m_motion.m_type;
                if (motion_type != 6 && motion_type != 7)
                    rigid_body_apply_acceleration(rigid_body_index, &gravity);
            }
        }
    }
}

// @address 0x113e10 havok_component_get_point_velocity
static __forceinline c_havok_component::c_rigid_body *havok_component_rigid_body_at(c_havok_component *component, long index)
{
    return &component->m_rigid_bodies[index];
}
/* Reload the array data pointer after the type test. */
static __forceinline c_havok_component::c_rigid_body *havok_component_rigid_body_at_reload(c_havok_component *component, long index)
{
    c_havok_component::c_rigid_body *data = ((volatile phys_array<c_havok_component::c_rigid_body> *)&component->m_rigid_bodies)->m_data;
    return &data[index];
}
extern "C" void havok_component_get_point_velocity(c_havok_component *component, long rigid_body_index, real_vector3d *velocity, const real_point3d *point)
{
    long body_index = rigid_body_index;
    c_havok_component::c_rigid_body *body = havok_component_rigid_body_at(component, body_index);
    if (body->m_rigid_body->m_motion.m_type != k_motion_type_fixed) {
        real_point3d center_of_mass;
        c_havok_component::c_rigid_body *rigid_body = havok_component_rigid_body_at_reload(component, body_index);
        component->get_rigid_body_center_of_mass(rigid_body_index, &center_of_mass);

        real_vector3d delta;
        delta.i = point->x - center_of_mass.x;
        delta.j = point->y - center_of_mass.y;
        delta.k = point->z - center_of_mass.z;

        real_vector3d cross;
        cross.k = (rigid_body->m_pre_simulation_angular_velocity).i * delta.j - (rigid_body->m_pre_simulation_angular_velocity).j * delta.i;
        cross.j = (rigid_body->m_pre_simulation_angular_velocity).k * delta.i - (rigid_body->m_pre_simulation_angular_velocity).i * delta.k;
        cross.i = (rigid_body->m_pre_simulation_angular_velocity).j * delta.k - (rigid_body->m_pre_simulation_angular_velocity).k * delta.j;

        velocity->k = cross.k;
        velocity->i = cross.i;
        velocity->j = cross.j;

        const real_vector3d *linear_velocity = &rigid_body->m_pre_simulation_linear_velocity;
        velocity->i = linear_velocity->i + cross.i;
        velocity->j = linear_velocity->j + cross.j;
        velocity->k = linear_velocity->k + cross.k;
    } else {
        *velocity = *global_zero_vector3d;
    }
}

/* ---- 0x257268: build the (possibly transformed) child shape for one rigid body of a constraint group ---- */

static __forceinline void havok_component_transform_set_identity(phys_transform *transform)
{
    phys_vec4 zero;
#if defined(_XBOX) && !defined(__clang__)
    zero.m_simd_quad = __vxor(zero.m_simd_quad, zero.m_simd_quad);
#else
    zero.m_simd_quad = __vzero();
#endif
    transform->m_rotation.get_column(0).m_simd_quad = zero.m_simd_quad;
    transform->m_rotation.get_column(1).m_simd_quad = zero.m_simd_quad;
    transform->m_rotation.get_column(2).m_simd_quad = zero.m_simd_quad;
    transform->m_rotation.get_column(0)(0) = 1.0f;
    transform->m_rotation.get_column(1)(1) = 1.0f;
    transform->m_rotation.get_column(2)(2) = 1.0f;
#if defined(_XBOX) && !defined(__clang__)
    transform->m_translation.m_simd_quad = __vxor(transform->m_translation.m_simd_quad, transform->m_translation.m_simd_quad);
#else
    transform->m_translation.m_simd_quad = __vzero();
#endif
}


// @address 0x620c88 ??0phys_convex_xform_shape@@QAA@PBVphys_convex_shape@@ABVphys_transform@@@Z
// @address 0x257268 havok_component_build_group_shape
extern "C" void havok_component_build_group_shape(long group_index, bool unused, const s_physics_model_rigid_body *node,
    render_model_definition *nodes, phys_transform *transform, void **shapes, long *shape_count)
{
    unsigned long flags = node->flags;
    const phys_shape *wrapper = node->shape;
    if (!unused && wrapper->m_type == 27 && !(flags & 128))
        wrapper = ((const phys_transform_shape *)wrapper)->get_child_shape();
    if (node->node == group_index) {
        if (wrapper->m_type != 27 && wrapper->m_type == 10) {
            long i = 0;
            if (((const phys_list_shape *)wrapper)->m_child_info.m_size > 0) {
                long count = *shape_count;
#ifdef HALO3_64BIT
                do {
                    phys_ref_object *child = (phys_ref_object *)((const phys_list_shape *)wrapper)->m_child_info[i].m_proxy_shape;
                    shapes[count++] = child;
                    if (child->m_mem_size_and_flags)
                        child->m_object_ref_count++;
                    i++;
                } while (i < ((const phys_list_shape *)wrapper)->m_child_info.m_size);
#else
                long offset = 0;
                do {
                    long slot_offset = count << 2;
                    count++;
                    phys_ref_object *child = *(phys_ref_object **)((char *)((const phys_list_shape *)wrapper)->m_child_info.m_data + offset);
                    *(void **)((char *)shapes + slot_offset) = child;
                    if (child->m_mem_size_and_flags)
                        child->m_object_ref_count++;
                    i++;
                    offset += 16;
                } while (i < ((const phys_list_shape *)wrapper)->m_child_info.m_size);
#endif
                *shape_count = count;
            }
            havok_component_transform_set_identity(transform);
            return;
        }
    }

    if (node->node != group_index) {
        real_matrix4x3 inverse, relative;
        const real_matrix4x3 *a = &tag_block_get_element<render_model_node>(&nodes->nodes, node->node)->default_inverse_matrix;
        const real_matrix4x3 *b = group_index != -1
            ? &tag_block_get_element<render_model_node>(&nodes->nodes, group_index)->default_inverse_matrix
            : &k_real_math_identity_matrix_storage;
        matrix4x3_inverse(a, &inverse);
        matrix4x3_compute_product(b, &inverse, &relative);
        phys_transform_from_matrix4x3(transform, &relative);
        phys_transform local;
        havok_component_copy_transform(&local, *transform);
        void *memory;
        const phys_shape *inner;
        switch (wrapper->m_type) {
        case PHYS_SHAPE_CONVEX_OFFSET: {
            local.m_translation.m_simd_quad = __vaddfp(local.m_translation.m_simd_quad,
                ((const phys_convex_offset_shape *)wrapper)->get_translation().m_simd_quad);
            inner = ((const phys_convex_offset_shape *)wrapper)->get_child_shape();
            wrapper = new phys_convex_offset_shape((const phys_convex_shape *)inner, local.m_translation);
            break;
        }
        case PHYS_SHAPE_CONVEX_XFORM:
            local.set_mul_eq(((const phys_convex_xform_shape *)wrapper)->get_transform());
            inner = ((const phys_convex_xform_shape *)wrapper)->get_child_shape();
            wrapper = new phys_convex_xform_shape((const phys_convex_shape *)inner, local);
            break;
        case PHYS_SHAPE_TRANSFORM:
            local.set_mul_eq(((const phys_transform_shape *)wrapper)->get_transform());
            inner = ((const phys_transform_shape *)wrapper)->get_child_shape();
            memory = phys_thread_memory::get_singleton_instance().allocate_chunk(sizeof(phys_transform_shape), PHYS_MEM_COLLISION_INFO);
            ((phys_ref_object *)memory)->m_mem_size_and_flags = sizeof(phys_transform_shape);
            wrapper = new (memory) phys_transform_shape((const phys_shape *)inner, local);
            break;
        default:
            memory = phys_thread_memory::get_singleton_instance().allocate_chunk(sizeof(phys_transform_shape), PHYS_MEM_COLLISION_INFO);
            ((phys_ref_object *)memory)->m_mem_size_and_flags = sizeof(phys_transform_shape);
            wrapper = new (memory) phys_transform_shape((const phys_shape *)wrapper, local);
            break;
        }
    } else {
        havok_component_transform_set_identity(transform);
    }
    long count = *shape_count;
    *shape_count = count + 1;
    shapes[count] = (phys_shape *)wrapper;
}



static __forceinline void havok_component_multiply_rotation(float *rotation, const phys_quat &delta)
{
    phys_vec4 *orientation = (phys_vec4 *)rotation;
    const phys_vec4 *change = (const phys_vec4 *)&delta;
    __vector4 original = orientation->m_simd_quad;
    __vector4 increment = change->m_simd_quad;
    float scalar = rotation[3];
    float dot;
    __stvewx(__vmsum3fp(original, increment), &dot, 0);
    float delta_scalar = delta.m_quat_vec(3);
    __vector4 cross = __vsubfp(__vmulfp(__vpermwi(original, 99), __vpermwi(increment, 135)),
        __vmulfp(__vpermwi(original, 135), __vpermwi(increment, 99)));
    orientation->m_simd_quad = __vmaddfp(original, __vspltw(increment, 3),
        __vmaddfp(increment, __vspltw(original, 3), cross));
    rotation[3] = delta_scalar * scalar - dot;
}


static __forceinline float havok_component_deactivator_tick_length()
{
    return game_time_globals->tick_length;
}

// @address 0x251648 ?update_in_local_space@c_object_rigid_body_deactivator@@QAAXPBTreal_vector3d@@0@Z
void c_object_rigid_body_deactivator::update_in_local_space(const real_vector3d *linear_velocity, const real_vector3d *angular_velocity)
{
    real_vector3d axis = *linear_velocity;
    phys_vec4 translation;
    phys_vec4_from_real_vector3d(&translation, linear_velocity);
    translation.mul4(phys_simd_scalar(havok_component_deactivator_tick_length()));
    float angular_speed = normalize3d(&axis);
    if (angular_speed != 0.0f) {
        phys_vec4 hk_axis;
        phys_vec4_from_real_vector3d(&hk_axis, &axis);
        const float *tick_length = &game_time_globals->tick_length;
        phys_quat delta(hk_axis, *tick_length * angular_speed);
        m_hf_sample.m_ref_rot.mul(delta);
        m_low_freq_sample.m_ref_rot.mul(delta);
    }
    phys_vec4 *high = &m_hf_sample.m_ref_pos;
    phys_vec4 *low = &m_low_freq_sample.m_ref_pos;
    high->m_simd_quad = __vaddfp(high->m_simd_quad, translation.m_simd_quad);
    low->m_simd_quad = __vaddfp(low->m_simd_quad, translation.m_simd_quad);
}

typedef char havok_component_spring_source_offset[(offsetof(s_muffin_piece, render_matrices) == 0x5c) ? 1 : -1];
typedef char havok_component_spring_blend_offset[(offsetof(s_muffin_definition_piece, blend) == 0x40) ? 1 : -1];


static __forceinline real_point3d *havok_component_spring_add_vector3d(const real_point3d *point, const real_vector3d *vector, real_point3d *result)
{
    result->x = point->x + vector->i;
    result->y = point->y + vector->j;
    result->z = point->z + vector->k;
    return result;
}

static __forceinline void havok_component_spring_add_scaled_point3d(real_vector3d *vector, const real_point3d *point, float scale)
{
    vector->i += scale * point->x;
    vector->j += scale * point->y;
    vector->k += scale * point->z;
}

// @address 0x36d118 havok_component_update_transform_spring
extern "C" void havok_component_update_transform_spring(s_muffin_piece *spring, const s_muffin_definition_piece *settings)
{
    if (settings->blend > 0.0f) {
        const real_vector3d *up = &spring->render_matrices[0].rotation.up;
        real_point3d target;
        real_vector3d delta;
        point_from_line3d(&spring->render_matrices[0].position, &spring->render_matrices[0].rotation.up, settings->distance, &target);
        vector_from_points3d(&spring->position, &target, &delta);
        point_from_line3d(&spring->velocity, &delta, settings->acceleration, &spring->velocity);
        scale_vector3d(&spring->velocity, settings->damping, &spring->velocity);
        limit3d(&spring->velocity, settings->maximum_velocity);
        havok_component_spring_add_vector3d(&spring->position, &spring->velocity, &spring->position);
        real_vector3d offset;
        memcpy(offset.n, spring->position.n, sizeof(offset.n));
        float blend = settings->blend;
        if (blend < 1.0f) {
            offset.i *= blend;
            offset.j *= blend;
            offset.k *= blend;
            havok_component_spring_add_scaled_point3d(&offset, &target, 1.0f - blend);
        }
        subtract_vectors3d(&offset, &spring->render_matrices[0].position, &offset);
        float maximum = settings->distance;
        if (maximum <= 0.0001f)
            maximum = 0.0001f;
        limit3d(&offset, maximum * 2.0f);
        havok_component_spring_add_vector3d(&spring->render_matrices[0].position, &offset, &spring->position);
        normalize3d(&offset);
        if (!valid_real_normal3d(&offset))
            offset = *up;
        real_matrix4x3 rotation;
        matrix4x3_rotation_between_vectors(&rotation, &offset, up);
        matrix4x3_compute_product(&spring->render_matrices[0], &rotation, &spring->render_matrices[1]);
    } else {
        spring->render_matrices[1] = spring->render_matrices[0];
    }
}

// @address 0x107888 ?body_slam_object_node_matrices@c_havok_component@@QAAXPAV?$c_static_flags@$0PP@@@M@Z
void c_havok_component::body_slam_object_node_matrices(c_static_flags<255> *simulation_controlled_nodes, float z_offset)
{
    if (!is_one_body_object_space_component() && is_physics_model_havok_component() && get_rigid_body_count() > 0) {
        object_datum *object = (object_datum *)object_get_and_verify_type(get_object_index(), 0xffffffff);
        s_object_definition *definition = (s_object_definition *)tag_get('obje', object->definition_index);
        s_model_definition *model = (s_model_definition *)tag_get('hlmt', definition->model.index);
        s_physics_model_definition *physics_model = (s_physics_model_definition *)tag_get('phmo', model->physics_model_index);
        render_model_definition *render_model = (render_model_definition *)tag_get('mode', ((s_model_definition *)model)->render_model_index);
        c_static_flags<255> desired_simulation_controlled_nodes;
        desired_simulation_controlled_nodes.clear();
        long node_orientations_count = 0;
        real_orientation *node_orientations = 0;
        bool changed = false;
        if (((object_datum *)object_get_and_verify_type(get_object_index(), 0xffffffff))->node_orientations.offset != -1) {
            node_orientations_count = (unsigned long)object->node_orientations.size / sizeof(real_orientation);
            node_orientations = (real_orientation *)((char *)object + (short)object->node_orientations.offset);
        }
        for (long body_index = 0; body_index < get_rigid_body_count(); body_index++) {
            long first_index = get_physics_model_rigid_body_index(body_index, 0);
            const s_physics_model_rigid_body *first_body = tag_block_get_element<s_physics_model_rigid_body>(&physics_model->rigid_bodies, first_index);
            if (get_physics_model_rigid_body_index_count(body_index) > 0) {
                for (long index = 0; index < get_physics_model_rigid_body_index_count(body_index); index++) {
                    const s_physics_model_rigid_body *body = tag_block_get_element<s_physics_model_rigid_body>(
                        &physics_model->rigid_bodies, get_physics_model_rigid_body_index(body_index, index));
                    long node_index = body->node;
                    if (node_index != -1 && get_rigid_body_is_dynamic(body_index) && !simulation_controlled_nodes->test(node_index)) {
                        long parent_node_index = node_index;
                        bool descendant = false;
                        while (parent_node_index != -1) {
                            if (parent_node_index == first_body->node) {
                                descendant = true;
                                break;
                            }
                            parent_node_index = tag_block_get_element<s_physics_model_node>(&physics_model->nodes, parent_node_index)->parent_node_index;
                        }
                        if (!descendant || body->node == first_body->node) {
                            const s_physics_model_node *node = tag_block_get_element<s_physics_model_node>(&physics_model->nodes, body->node);
                            long parent = node->parent_node_index;
                            real_matrix4x3 *matrix = object_get_node_matrix(get_object_index(), body->node);
                            get_rigid_body_transform(body_index, matrix);
                            matrix->position.z += z_offset;
                            if (body->node != first_body->node) {
                                __declspec(align(16)) char shape_buffer[512];
                                const phys_list_shape *list = (const phys_list_shape *)havok_component_get_rigid_body(body_index)->get_collider()->get_shape();
                                const phys_transform_shape *shape = (const phys_transform_shape *)list->get_child_shape(index, shape_buffer);
                                real_matrix4x3 local_matrix;
                                extract_havok_transform(&local_matrix, &shape->get_transform());
                                matrix4x3_compute_product(matrix, &local_matrix, matrix);
                            }
                            changed = true;
                            simulation_controlled_nodes->set(body->node, true);
                            desired_simulation_controlled_nodes.set(body->node, true);
                            if (!(node->flags & 1)) {
                                while (parent != -1) {
                                    const s_physics_model_node *parent_node = tag_block_get_element<s_physics_model_node>(&physics_model->nodes, parent);
                                    desired_simulation_controlled_nodes.set(parent, true);
                                    parent = parent_node->parent_node_index;
                                }
                            }
                        }
                    }
                }
            }
        }
        if (changed && render_model->nodes.count > 0)
            body_slam_unsimulated_nodes_recursive(
                0, simulation_controlled_nodes, &desired_simulation_controlled_nodes, node_orientations, node_orientations_count);
        if (changed && object->scale != 1.0f) {
                object_datum *current_object = (object_datum *)object_get_and_verify_type(get_object_index(), 0xffffffff);
                long count = (unsigned long)current_object->node_matrices_size / sizeof(real_matrix4x3);
                real_matrix4x3 *matrices = (real_matrix4x3 *)((char *)current_object + current_object->node_matrices_offset);
                for (long index = 0; index < count; index++)
                    matrices[index].scale = object->scale;
        }
    }
}

// @address 0x258740 havok_component_compute_deactivation_params
extern "C" void havok_component_compute_deactivation_params(
    // match: 0x258740: pointer __restrict qualifier prevents alias reload
    long object_index, long *quality_type, float *__restrict velocity_threshold, long motion_type, long havok_group, float size)
{
    s_object_header *header = object_header_get(object_index);
    const object_datum *object = (const object_datum *)header->datum;
    long object_type = header->object_type;
    if (motion_type == 7 || motion_type == 6) {
        *quality_type = PHYS_COLLIDER_QUALITY_INVALID;
        *velocity_threshold = size * 0.4f;
    } else {
        game_globals_storage *game = game_globals;
        *quality_type = PHYS_COLLIDER_QUALITY_CRITICAL;
        float threshold = size * 0.4f;
        *velocity_threshold = threshold;
        if (game->options.game_mode == 1) {
            if (!(threshold > 0.1f))
                threshold = 0.1f;
            *velocity_threshold = threshold;
        }
        long type_mask = 1 << object_type;
        if (type_mask & 1) {
            if (!(unsigned char)((object->damage_flags >> 2) & 1)) {
                *quality_type = PHYS_COLLIDER_QUALITY_CHARACTER;
                *velocity_threshold = size * 0.2f;
            }
        } else if (type_mask & 12) {
            *quality_type = PHYS_COLLIDER_QUALITY_CRITICAL;
        } else if (type_mask & 2) {
            long definition_index = object->definition_index;
            *quality_type = PHYS_COLLIDER_QUALITY_CRITICAL;
            const vehicle_definition *definition = (const vehicle_definition *)tag_get('vehi', definition_index);
            if ((unsigned char)((definition->flags >> 11) & 1))
                *velocity_threshold = 0.025f;
        } else if (havok_group == 17 || havok_group == 18) {
            *quality_type = PHYS_COLLIDER_QUALITY_DEBRIS;
        }
    }
}





static __forceinline void havok_component_body_copy_inertia(phys_mat3 *destination, const phys_mat3 &source)
{
    destination->get_column(0).m_simd_quad = source.get_column(0).m_simd_quad;
    destination->get_column(1).m_simd_quad = source.get_column(1).m_simd_quad;
    destination->get_column(2).m_simd_quad = source.get_column(2).m_simd_quad;
}
static __forceinline void havok_component_body_set_quality(phys_rigid_body_desc *info, long value)
{
    info->m_collider_quality = (phys_collider_quality)value;
}

static __forceinline void havok_component_body_copy_rotation(phys_quat *destination, const phys_quat &source)
{
    destination->m_quat_vec.m_simd_quad = source.m_quat_vec.m_simd_quad;
}

static __forceinline void havok_component_normalize_body_rotation(phys_quat *rotation)
{
    phys_simd_vec_union length;
    length.q = __vmsum4fp(rotation->m_quat_vec.m_simd_quad, rotation->m_quat_vec.m_simd_quad);
    length.r[0] = k_real_constant_pool[0] / sqrtf(length.r[0]);
    rotation->m_quat_vec.m_simd_quad = __vmulfp(rotation->m_quat_vec.m_simd_quad, __vspltw(length.q, 0));
}

static __forceinline bool havok_component_body_test_flag(unsigned long flags, long bit)
{
    return ((flags >> bit) & 1) != 0;
}

static __forceinline void havok_component_body_unwrap_shape(phys_rigid_body_desc *info, const bool &keep)
{
    if (!keep && info->m_proxy_shape->m_type == 27)
        info->m_proxy_shape = static_cast<const c_wrapped_shape *>(info->m_proxy_shape)->child_shape;
}

// @address 0x2583e8 havok_component_create_rigid_body
extern "C" phys_rigid_body *havok_component_create_rigid_body(c_havok_component *component, const phys_shape *shape,
    float deactivation_scale, bool keep_wrapped_shape, const phys_vec4 *center_of_mass,
    float friction, float restitution, const phys_mat3 *inertia_tensor,
    float mass, long motion_type, const phys_transform *transform, long havok_group,
    bool use_physics_model, bool unused_proxy_flag, float linear_damping, float angular_damping)
{
    if (!(mass > 0.001f))
        mass = 0.001f;
    __int64 system_group = datum_index_to_absolute_index(component->get_object_index()) + 1;
    phys_rigid_body_desc info;
    phys_quat rotation;
    rotation.set(transform->m_rotation);
    s_object_header *header = object_header_get(component->get_object_index());
    const object_datum *object = (const object_datum *)header->datum;
    if (havok_group == 12)
        system_group = 0x802;
    havok_component_normalize_body_rotation(&rotation);
    float minimum_dimension;
    float largest_dimension;
    long convex_count;
    long phantom_count;
    calculate_havok_shape_bounding_sphere_radius(shape, &minimum_dimension, &largest_dimension, &convex_count, &phantom_count);
    unsigned long flags = object->physics_flags;
    if (havok_component_body_test_flag(flags, 20) && motion_type == 7) {
        keep_wrapped_shape = false;
        motion_type = 4;
        havok_group = ((1 << header->object_type) & (1 << 6)) ? 24 : 3;
    }
    if (havok_component_body_test_flag(flags, 6) || (convex_count == 0 && motion_type != 6))
        motion_type = 7;
    long default_quality_type;
    float velocity_threshold;
    havok_component_compute_deactivation_params(
        component->get_object_index(), &default_quality_type, &velocity_threshold, motion_type, havok_group, largest_dimension);
    float allowed_penetration = velocity_threshold * deactivation_scale;
    long quality_type;
    if (havok_component_body_test_flag(flags, 23) && motion_type != 7 && motion_type != 6)
        quality_type = PHYS_COLLIDER_QUALITY_DEBRIS;
    else
        quality_type = default_quality_type;
    info.m_proxy_shape = shape;
    info.m_center_of_mass.m_simd_quad = center_of_mass->m_simd_quad;
    info.m_contact_friction = friction;
    info.m_contact_restitution = restitution;
    havok_component_body_copy_inertia(&info.m_inertia_tensor, *inertia_tensor);
    info.m_body_mass = mass;
    info.m_motion_type = (phys_motion::motion_kind)motion_type;
    info.m_position.m_simd_quad = transform->m_translation.m_simd_quad;
    havok_component_body_copy_rotation(&info.m_rotation, rotation);
    info.m_collision_filter_info = (unsigned long)((system_group << 16) | havok_group);
    info.m_linear_damping = linear_damping;
    info.m_ang_damping = angular_damping;
    info.m_allowed_penetration_depth = allowed_penetration > get_havok_constants()->minimum_velocity_threshold
        ? allowed_penetration : get_havok_constants()->minimum_velocity_threshold;
    info.m_collider_quality = (phys_collider_quality)quality_type;
    if (object->scripted_timeout_ticks)
#if defined(_XBOX) && !defined(__clang__)
        // match: 0x2583e8: 0.05f is read at 0x63c off the k_real_constant_pool base held in r18 for the entry load;
        // a plain literal loads its own base, frees r16 (savegprlr_17) and shrinks the frame by 16 bytes
        info.m_inertia_tensor.mul(phys_simd_scalar((float)object->scripted_timeout_ticks * k_real_constant_pool[399]));
#else
        info.m_inertia_tensor.mul(phys_simd_scalar((float)object->scripted_timeout_ticks * 0.05f));
#endif
    header = object_header_get(component->get_object_index());
    object = (const object_datum *)header->datum;
    if (((1 << header->object_type) & 1) && havok_component_body_test_flag(object->damage_flags, 2))
        info.m_solver_sleep = SOLVER_DEACTIVATION_MAX;
    else
        info.m_solver_sleep = SOLVER_DEACTIVATION_MEDIUM;
    havok_component_body_unwrap_shape(&info, keep_wrapped_shape);
    phys_thread_memory *memory = (phys_thread_memory *)TlsGetValue(phys_thread_memory::s_threadMemoryInstance.m_slot);
    void *storage = memory->allocate_chunk(sizeof(phys_rigid_body), PHYS_MEM_ENTITY);
    static_cast<phys_ref_object *>(storage)->m_mem_size_and_flags = sizeof(phys_rigid_body);
    return new (storage) phys_rigid_body(info);
}

// @address 0x250ed0 ?construct_proxy_rigid_body@c_havok_component@@QAAPAVphys_rigid_body@@J@Z
phys_rigid_body *c_havok_component::construct_proxy_rigid_body(long rigid_body_index)
{
    phys_rigid_body *body = m_rigid_bodies[rigid_body_index].m_rigid_body;
    phys_mat3 inertia;
    const phys_shape *shape = body->get_collider()->get_shape();
    body->get_inertia_local(inertia);
    float angular_damping = body->m_motion.m_motion_state.m_ang_damping;
    float linear_damping = body->m_motion.m_motion_state.m_linear_damping;
    long motion_type = body->get_motion_type();
    float mass = body->get_mass();
    phys_rigid_body *proxy = havok_component_create_rigid_body(this, shape, 1.0f, false,
        &body->get_center_of_mass_local(), 0.0f, 0.0f,
        &inertia, mass, motion_type, &body->get_transform(), 19, true, false,
        linear_damping, angular_damping);
    havok_entity_set_havok_component_index((phys_world_object *)proxy, -1);
    havok_entity_set_structure_bsp_index((phys_world_object *)proxy, -1);
    havok_entity_set_havok_component_rigid_body_index((phys_world_object *)proxy, -1);
    return proxy;
}

class phys_chain_data;

class phys_motor;
static __forceinline void havok_component_chain_remove_reference(phys_ref_object *object)
{
    if (object->m_mem_size_and_flags != 0 && --object->m_object_ref_count == 0)
    {
        long size = cache_files_get_total_tags_size();
        const char *base = (const char *)cache_file_globals_get_tag_cache_base_address();
        bool managed = (const char *)object >= base && (const char *)object <= base + size - 1;
        if (managed)
            object->m_object_ref_count = 1;
        else
            delete object;
    }
}
static __forceinline void *havok_component_chain_allocate(unsigned int size)
{
    phys_thread_memory *memory = (phys_thread_memory *)TlsGetValue(phys_thread_memory::s_threadMemoryInstance.m_slot);
    void *address = memory->allocate_chunk(size, PHYS_MEM_CONSTRAINT);
    ((phys_ref_object *)address)->m_mem_size_and_flags = (unsigned short)size;
    return address;
}
static __forceinline const phys_quat &havok_component_chain_rotation(const phys_rigid_body *body)
{
    return *(const phys_quat *)&body->m_motion.m_motion_state.m_swept_transform.m_rotation_1;
}
static __forceinline void havok_component_chain_inverse(phys_quat *result, const phys_quat &rotation)
{
    float real = rotation.m_quat_vec(3);
    result->m_quat_vec.set_neg4(rotation.m_quat_vec);
    result->m_quat_vec(3) = real;
}
static __forceinline const phys_vec4 &havok_component_chain_imaginary(const phys_quat &q)
{
    return *(const phys_vec4 *)&q;
}
static __forceinline void havok_component_chain_cross(phys_vec4 *result, const phys_vec4 &a, const phys_vec4 &b)
{
    __vector4 first = __vpermwi(a.m_simd_quad, 99);
    __vector4 second = __vpermwi(b.m_simd_quad, 135);
    __vector4 positive = __vmulfp(first, second);
    first = __vpermwi(a.m_simd_quad, 135);
    second = __vpermwi(b.m_simd_quad, 99);
    result->m_simd_quad = __vsubfp(positive, __vmulfp(first, second));
}
static __forceinline s_chain_simd havok_component_chain_splat(const phys_quat &q)
{ s_chain_simd result; result.value = __vspltw(q.m_quat_vec.m_simd_quad, 3); return result; }
static __forceinline s_chain_simd havok_component_chain_dot(const phys_quat &a, const phys_quat &b)
{ s_chain_simd result; result.value = __vmsum3fp(a.m_quat_vec.m_simd_quad, b.m_quat_vec.m_simd_quad); return result; }
static __forceinline float havok_component_chain_scalar(const s_chain_simd &s)
{ float result; __stvewx(s.value, &result, 0); return result; }
static __forceinline void havok_component_chain_addmul(phys_vec4 *result, const s_chain_simd &s, const phys_vec4 &v)
{ result->m_simd_quad = __vmaddfp(v.m_simd_quad, s.value, result->m_simd_quad); }
static __forceinline void havok_component_chain_multiply(phys_quat *result,
    phys_quat a, const phys_quat &b)
{
    phys_vec4 imaginary;
    havok_component_chain_cross(&imaginary, havok_component_chain_imaginary(a), havok_component_chain_imaginary(b));
    havok_component_chain_addmul(&imaginary, havok_component_chain_splat(a), havok_component_chain_imaginary(b));
    havok_component_chain_addmul(&imaginary, havok_component_chain_splat(b), havok_component_chain_imaginary(a));
    float real = a.get_real() * b.get_real() - havok_component_chain_scalar(havok_component_chain_dot(a, b));
    result->m_quat_vec.m_simd_quad = imaginary.m_simd_quad;
    result->m_quat_vec(3) = real;

}
static __forceinline void havok_component_chain_add_to_world(phys_constraint *constraint)
{
    if (!g_havok_monitor_globals.inside_simulation_step)
        g_havok_world->add_world_constraint(constraint);
}
static __forceinline void *havok_component_chain_tag_element(const s_tag_block *block, long index, long size)
{
    return (char *)block->address + index * size;
}
// @address 0x256c90 ?build_physics_model_rigid_body_shape@c_havok_component@@AAAPBVphys_shape@@PBUs_physics_model_instance@@PBDPADPAJPBVc_runtime_node_to_physics_model_rigid_body_index_mapping@@JPAV?$c_static_flags@$0PP@@@PAMPAVphys_vec4@@PAVphys_mat3@@PA_NPAW4e_rigid_body_size@@999@Z
const phys_shape *c_havok_component::build_physics_model_rigid_body_shape(const s_physics_model_instance *instance, const char *rigid_body_motion_types,
    char *rigid_body_indexes,
    long *rigid_body_indexes_count, const c_runtime_node_to_physics_model_rigid_body_index_mapping *mapping, long node_index,
    // match: 0x256c90: pointer __restrict qualifier prevents alias reload
    c_static_flags<255> *nodes_currently_in_graph_bit_vector, float *__restrict mass, phys_vec4 *center_of_mass, phys_mat3 *inertia_tensor,
        bool *constrained_to_world,
    // match: 0x256c90: pointer __restrict qualifier prevents alias reload
    e_rigid_body_size *__restrict maximum_rigid_body_size, bool *__restrict plays_impact_effects,
    // match: 0x256c90: pointer __restrict qualifier prevents alias reload
    bool *__restrict has_phantom_in_shape, bool *__restrict does_not_interact_with_environment)
{
    c_static_flags<4096> rigid_physics_model_edges;
    const phys_shape *shape = 0;
    *plays_impact_effects = false;
    *rigid_body_indexes_count = 0;
    *constrained_to_world = false;
    generate_rigid_edges_bit_vector(instance, &rigid_physics_model_edges);
    generate_rigidly_constrained_shape_list_recursive(instance, rigid_body_motion_types, &rigid_physics_model_edges, mapping, node_index, -1, node_index,
        rigid_body_indexes, rigid_body_indexes_count, 64, nodes_currently_in_graph_bit_vector, constrained_to_world);
    if (*rigid_body_indexes_count > 0) {
        long first_rigid_body_index = rigid_body_indexes[0];
        bool keyframed;
        if (rigid_body_motion_types[first_rigid_body_index] == 7 || rigid_body_motion_types[first_rigid_body_index] == 6)
            keyframed = true;
        else
            keyframed = false;
        long effects_disabled_count = 0;
        if (*rigid_body_indexes_count == 1) {
            const s_physics_model_rigid_body *rigid_body = tag_block_get_element<s_physics_model_rigid_body>(
                &instance->physics_model->rigid_bodies, first_rigid_body_index);
            *mass = rigid_body->mass;
            center_of_mass->m_simd_quad = rigid_body->center_of_mass.m_simd_quad;
            havok_component_body_copy_inertia(inertia_tensor, rigid_body->inertia_tensor);
            effects_disabled_count += TEST_BIT(rigid_body->flags, 2);
            shape = rigid_body->shape;
            *maximum_rigid_body_size = (e_rigid_body_size)rigid_body->size;
            shape->add_object_reference();
            *has_phantom_in_shape |= TEST_BIT(rigid_body->runtime_flags, 0);
            *does_not_interact_with_environment |= TEST_BIT(rigid_body->flags, 3);
        } else {
            phys_shape *shapes[1024];
            long shape_count = 0;
            const s_physics_model_rigid_body *first_rigid_body = tag_block_get_element<s_physics_model_rigid_body>(
                &instance->physics_model->rigid_bodies, first_rigid_body_index);
            render_model_definition *render_model = (render_model_definition *)tag_get('mode', ((s_model_definition *)instance->model)->render_model_index);
            phys_inline_array<phys_mass_element, 64> mass_elements(*rigid_body_indexes_count);
            phys_mass_props mass_properties;
            *maximum_rigid_body_size = _rigid_body_size_default;
            *does_not_interact_with_environment |= TEST_BIT(first_rigid_body->flags, 3);
            for (long index = 0; index < *rigid_body_indexes_count; index++) {
                const s_physics_model_rigid_body *rigid_body = tag_block_get_element<s_physics_model_rigid_body>(
                    &instance->physics_model->rigid_bodies, rigid_body_indexes[index]);
                phys_transform transform;
                havok_component_build_group_shape(first_rigid_body->node, keyframed, rigid_body, render_model,
                    &transform, (void **)shapes, &shape_count);
                *has_phantom_in_shape |= TEST_BIT(rigid_body->runtime_flags, 0);
                havok_component_copy_transform(&mass_elements[index].m_body_transform, transform);
                mass_elements[index].m_properties.m_body_volume = 0.0f;
                mass_elements[index].m_properties.m_body_mass = rigid_body->mass;
                mass_elements[index].m_properties.m_center_of_mass.m_simd_quad = rigid_body->center_of_mass.m_simd_quad;
                havok_component_body_copy_inertia(&mass_elements[index].m_properties.m_inertia_tensor, rigid_body->inertia_tensor);
                *maximum_rigid_body_size = (e_rigid_body_size)MAX((long)*maximum_rigid_body_size, (long)rigid_body->size);
                effects_disabled_count += TEST_BIT(rigid_body->flags, 2);
            }
            phys_inertia_util::combine_mass_props(mass_elements, mass_properties);
            *mass = mass_properties.m_body_mass;
            center_of_mass->m_simd_quad = mass_properties.m_center_of_mass.m_simd_quad;
            havok_component_body_copy_inertia(inertia_tensor, mass_properties.m_inertia_tensor);
            shape = new phys_list_shape(shapes, shape_count);
            for (long shape_index = 0; shape_index < shape_count; shape_index++)
                havok_component_chain_remove_reference(shapes[shape_index]);
        }
        *plays_impact_effects = *rigid_body_indexes_count - effects_disabled_count != 0;
    }
    return shape;
}
// @address 0x258890 ?build_powered_chain_constraints@c_havok_component@@AAAXPAUs_physics_model_instance@@ABVc_runtime_node_to_havok_component_rigid_body_index_mapping@@@Z
void c_havok_component::build_powered_chain_constraints(s_physics_model_instance *instance,
    const c_runtime_node_to_havok_component_rigid_body_index_mapping &mapping)
{
    for (long chain_index = 0; chain_index < instance->physics_model->powered_chains.count; ++chain_index)
    {
        s_physics_model_powered_chain *chain = tag_block_get_element<s_physics_model_powered_chain>(
            &instance->physics_model->powered_chains, chain_index);
        if (chain->nodes.count - 1 == chain->links.count && chain->links.count != 0)
        {
            bool valid = true;
            for (long node = 0; valid && node < chain->nodes.count; ++node)
            {
                long node_index = *(const short *)havok_component_chain_tag_element(&chain->nodes, node, sizeof(short));
                if (node_index == -1 || havok_component_rigid_body_index(&mapping, node_index) == -1)
                {
                    valid = false;
                    break;
                }
                if (node < chain->nodes.count - 1)
                {
                    s_physics_model_powered_chain_link *link = (s_physics_model_powered_chain_link *)havok_component_chain_tag_element(
                        &chain->links, node, sizeof(s_physics_model_powered_chain_link));
                    if (link->constraint.type == -1 || link->constraint.index == -1 ||
                        !(const s_physics_model_constraint_bodies *)physics_model_try_and_get_constraint(instance->physics_model,
                        &link->constraint))
                    {
                        valid = false;
                        break;
                    }
                    for (long dimension = 0; dimension < 3; ++dimension)
                    {
                        const s_physics_model_motor_reference &motor = link->motors[dimension];
                        if ((motor.motor_type != 1 && motor.motor_type != 2) || motor.motor_index == -1)
                        {
                            valid = false;
                            break;
                        }
                    }
                }
            }
            if (valid)
            {
                phys_powered_chain_data *data = new (havok_component_chain_allocate(sizeof(phys_powered_chain_data))) phys_powered_chain_data;
                phys_constraint_chain *constraint = new (havok_component_chain_allocate(sizeof(phys_constraint_chain)))
                    phys_constraint_chain((phys_chain_data *)data);
                havok_component_chain_remove_reference((phys_ref_object *)data);
                for (long node = 0; node < chain->nodes.count; ++node)
                {
                    long node_index = *(const short *)havok_component_chain_tag_element(&chain->nodes, node, sizeof(short));
                    long rigid_body_index = node_index == -1 ? mapping.m_unmapped_rigid_body : mapping.m_node_to_rigid_body_mapping[node_index];
                    constraint->add_chain_entity((phys_entity *)havok_component_get_rigid_body(rigid_body_index));
                    if (node < chain->nodes.count - 1)
                    {
                        s_physics_model_powered_chain_link *link =
                            (s_physics_model_powered_chain_link *)havok_component_chain_tag_element(
                                &chain->links, node, sizeof(s_physics_model_powered_chain_link));
                        const s_physics_model_constraint_bodies *bodies = (const s_physics_model_constraint_bodies *)
                            physics_model_try_and_get_constraint(instance->physics_model, &link->constraint);
                        phys_transform transform_a;
                        phys_transform transform_b;
                        phys_quat inverse;
                        phys_quat relative;
                        phys_rigid_body *body_a;
                        phys_rigid_body *body_b;
                        phys_motor *motors[3];
                        float power;
                        for (long dimension = 0; dimension < 3; ++dimension)
                            motors[dimension] = build_motor_from_motor_reference(instance, &link->motors[dimension], &power);
                        phys_rigid_body **body_a_out = &body_a;
                        phys_rigid_body **body_b_out = &body_b;
                        calculate_constraint_transforms(instance, &mapping, &transform_a, &transform_b, body_a_out, body_b_out, bodies);
                        const phys_quat &rotation_b = havok_component_chain_rotation(body_b);
                        havok_component_chain_inverse(&inverse, havok_component_chain_rotation(body_a));
                        havok_component_chain_multiply(&relative, inverse, rotation_b);
                        data->add_info_in_body_space(transform_a.m_translation, transform_b.m_translation,
                            relative, motors[0], motors[1], motors[2]);
                        for (long dimension = 0; dimension < 3; ++dimension)
                            havok_component_chain_remove_reference((phys_ref_object *)motors[dimension]);
                    }
                }
                add_constraint(constraint, 6, chain_index, true);
                if (m_flags & 8)
                    havok_component_chain_add_to_world(constraint);
            }
        }
    }
}

const float k_havok_component_degrees_to_radians = 0.017453292f;

/* Callers pass no matrix and false for force. */
// @address 0x253fc8 ?create_constraint@c_havok_component@@QAAXPBUs_physics_model_instance@@PBVc_runtime_node_to_havok_component_rigid_body_index_mapping@@PBUs_physics_model_node_constraint_edge_constraint@@PBUreal_matrix4x3@@_N@Z
void c_havok_component::create_constraint(const s_physics_model_instance *physics_model_instance,
    const c_runtime_node_to_havok_component_rigid_body_index_mapping *runtime_node_to_havok_component_rigid_body_index_mapping,
    const s_physics_model_node_constraint_edge_constraint *physics_model_node_constraint_edge_constraint, const real_matrix4x3 *matrix, bool force)
{
    if ((physics_model_node_constraint_edge_constraint->flags & 4) == 0)
    {
        object_datum *object = (object_datum *)object_get_and_verify_type(get_object_index(), 0xffffffff);
        s_object_definition *definition = (s_object_definition *)tag_get('obje', object->definition_index);
        s_model_definition *model = (s_model_definition *)tag_get('hlmt', definition->model.index);
        s_physics_model_definition *physics_model = (s_physics_model_definition *)tag_get('phmo', model->physics_model_index);
        const s_physics_model_constraint_bodies *constraint_bodies = (const s_physics_model_constraint_bodies *)
            physics_model_try_and_get_constraint(physics_model, (const s_physics_model_constraint_reference *)physics_model_node_constraint_edge_constraint);
        phys_transform transform_a;
        phys_transform transform_b;
        phys_rigid_body *rigid_body_a;
        phys_rigid_body *rigid_body_b;
        phys_constraint_data *constraint_data = 0;
        float max_friction_torque = physics_model_node_constraint_edge_constraint->max_friction_torque;
        bool motors_active = false;
        calculate_constraint_transforms(physics_model_instance, runtime_node_to_havok_component_rigid_body_index_mapping,
            &transform_a, &transform_b, &rigid_body_a, &rigid_body_b, constraint_bodies);
        switch (physics_model_node_constraint_edge_constraint->type)
        {
        case 0:
        {
            phys_hinge_data *hinge = new (havok_component_chain_allocate(sizeof(phys_hinge_data))) phys_hinge_data;
            hinge->set_in_body_space(transform_a.m_translation, transform_b.m_translation, transform_a.m_rotation.get_column(2),
                transform_b.m_rotation.get_column(2));
            constraint_data = hinge;
            break;
        }
        case 1:
        {
            phys_motor *motor = 0;
            float motor_target_angle;
            if (physics_model_node_constraint_edge_constraint->limited_hinge_motors.count)
            {
                motor = build_motor_from_motor_reference(physics_model_instance,
                    tag_block_get_element<s_physics_model_motor_reference>(&physics_model_node_constraint_edge_constraint->limited_hinge_motors, 0),
                    &motor_target_angle);
                motors_active = motor != 0;
            }
            const s_physics_model_limited_hinge_constraint *limited_hinge_definition = tag_block_get_element<s_physics_model_limited_hinge_constraint>(
                &physics_model->limited_hinge_constraints, physics_model_node_constraint_edge_constraint->index);
            phys_limited_hinge_data *limited_hinge = new (havok_component_chain_allocate(sizeof(phys_limited_hinge_data)))
                phys_limited_hinge_data;
            limited_hinge->set_in_body_space(transform_a.m_translation, transform_b.m_translation, transform_a.m_rotation.get_column(2),
                transform_b.m_rotation.get_column(2),
                transform_a.m_rotation.get_column(1), transform_b.m_rotation.get_column(1));
            limited_hinge->m_data_atoms.m_ang_friction.m_max_friction_torque =
                MAX(max_friction_torque != 0.0f ? max_friction_torque : limited_hinge_definition->max_friction_torque, 0.01f);
            limited_hinge->m_data_atoms.m_ang_limit.m_min_angle = 
                ((-limited_hinge_definition->min_angle) <= (-limited_hinge_definition->max_angle)
                    ? (-limited_hinge_definition->min_angle) : (-limited_hinge_definition->max_angle))
                    * k_havok_component_degrees_to_radians;
            limited_hinge->m_data_atoms.m_ang_limit.m_atom_max_angle = 
                MAX(-limited_hinge_definition->min_angle, -limited_hinge_definition->max_angle) * k_havok_component_degrees_to_radians;
            if (motor)
            {
                limited_hinge->m_data_atoms.m_ang_motor.m_target_angle = motor_target_angle;
                limited_hinge->set_hinge_motor(motor);
                ((phys_ref_object *)motor)->object_remove_ref();
            }
            if (motors_active)
                limited_hinge->set_motor_active(0, true);
            constraint_data = limited_hinge;
            break;
        }
        case 2:
        {
            const s_physics_model_ragdoll_constraint *ragdoll_definition = tag_block_get_element<s_physics_model_ragdoll_constraint>(
                &physics_model_instance->physics_model->ragdoll_constraints, physics_model_node_constraint_edge_constraint->index);
            phys_ragdoll_data *ragdoll = new (havok_component_chain_allocate(sizeof(phys_ragdoll_data))) phys_ragdoll_data;
            phys_motor *twist_motor = 0;
            phys_motor *cone_motor = 0;
            phys_motor *plane_motor = 0;
            if (physics_model_node_constraint_edge_constraint->ragdoll_motors.count)
            {
                const s_physics_model_ragdoll_motors *motors = tag_block_get_element<s_physics_model_ragdoll_motors>(
                    &physics_model_node_constraint_edge_constraint->ragdoll_motors, 0);
                float unused_power;
                twist_motor = build_motor_from_motor_reference(physics_model_instance, &motors->twist_motor, &unused_power);
                cone_motor = build_motor_from_motor_reference(physics_model_instance, &motors->cone_motor, &unused_power);
                plane_motor = build_motor_from_motor_reference(physics_model_instance, &motors->plane_motor, &unused_power);
                motors_active = twist_motor || cone_motor || plane_motor;
            }
            ragdoll->set_in_body_space(transform_a.m_translation, transform_b.m_translation, transform_a.m_rotation.get_column(1),
                transform_b.m_rotation.get_column(1),
                transform_a.m_rotation.get_column(0), transform_b.m_rotation.get_column(0));
            ragdoll->m_data_atoms.m_twist_limit.m_min_angle = ((ragdoll_definition->min_twist) <= (ragdoll_definition->max_twist)
                ? (ragdoll_definition->min_twist) : (ragdoll_definition->max_twist)) * k_havok_component_degrees_to_radians;
            ragdoll->m_data_atoms.m_twist_limit.m_atom_max_angle = MAX(ragdoll_definition->min_twist,
                ragdoll_definition->max_twist) * k_havok_component_degrees_to_radians;
            ragdoll->m_data_atoms.m_planes_limit.m_min_angle = ((-ragdoll_definition->min_plane) <= (-ragdoll_definition->max_plane)
                ? (-ragdoll_definition->min_plane) : (-ragdoll_definition->max_plane))
                * k_havok_component_degrees_to_radians;
            ragdoll->m_data_atoms.m_planes_limit.m_atom_max_angle = MAX(-ragdoll_definition->min_plane, -ragdoll_definition->max_plane)
                * k_havok_component_degrees_to_radians;
            ragdoll->m_data_atoms.m_cone_limit.m_atom_max_angle = MAX(ragdoll_definition->min_cone,
                ragdoll_definition->max_cone) * k_havok_component_degrees_to_radians;
            ragdoll->set_max_friction_torque(MAX(max_friction_torque != 0.0f ? max_friction_torque : ragdoll_definition->max_friction_torque, 0.01f));
            if (twist_motor)
            {
                ragdoll->set_twist_motor(twist_motor);
                ((phys_ref_object *)twist_motor)->object_remove_ref();
            }
            if (cone_motor)
            {
                ragdoll->set_cone_motor(cone_motor);
                ((phys_ref_object *)cone_motor)->object_remove_ref();
            }
            if (plane_motor)
            {
                ragdoll->set_plane_motor(plane_motor);
                ((phys_ref_object *)plane_motor)->object_remove_ref();
            }
            if (motors_active)
                ragdoll->set_motors_active(0, true);
            constraint_data = ragdoll;
            break;
        }
        case 3:
        {
            const s_physics_model_stiff_spring_constraint *stiff_spring_definition = tag_block_get_element<s_physics_model_stiff_spring_constraint>(
                &physics_model_instance->physics_model->stiff_spring_constraints, physics_model_node_constraint_edge_constraint->index);
            phys_stiff_spring_data *stiff_spring = new (havok_component_chain_allocate(sizeof(phys_stiff_spring_data))) phys_stiff_spring_data;
            havok_component_stiff_spring_set_in_world_space(stiff_spring, transform_a, transform_b, transform_a.m_rotation.get_column(2),
                transform_b.m_rotation.get_column(2));
            stiff_spring->m_data_atoms.m_spring_atom.m_length = stiff_spring_definition->spring_length;
            constraint_data = stiff_spring;
            break;
        }
        case 4:
        {
            phys_ball_socket_data *ball_and_socket = new (havok_component_chain_allocate(sizeof(phys_ball_socket_data)))
                phys_ball_socket_data;
            ball_and_socket->set_in_body_space(transform_a.m_translation, transform_b.m_translation);
            constraint_data = ball_and_socket;
            break;
        }
        case 5:
        {
            const s_physics_model_prismatic_constraint *prismatic_definition = tag_block_get_element<s_physics_model_prismatic_constraint>(
                &physics_model_instance->physics_model->prismatic_constraints, physics_model_node_constraint_edge_constraint->index);
            phys_prismatic_data *prismatic = new (havok_component_chain_allocate(sizeof(phys_prismatic_data))) phys_prismatic_data;
            prismatic->set_in_body_space(transform_a.m_translation, transform_b.m_translation, transform_a.m_rotation.get_column(2),
                transform_b.m_rotation.get_column(2),
                transform_a.m_rotation.get_column(1), transform_b.m_rotation.get_column(1));
            prismatic->m_data_atoms.m_lin_limit.m_max_limit = prismatic_definition->max_limit;
            prismatic->m_data_atoms.m_lin_limit.m_min_limit = prismatic_definition->min_limit;
            prismatic->m_data_atoms.m_contact_friction.m_max_friction_force = prismatic_definition->max_friction_force;
            constraint_data = prismatic;
            break;
        }
        default:
            // match: 0x253fc8: unreachable hint omits dead branch check
            __assume(0);
        }
        phys_constraint *constraint = new (havok_component_chain_allocate(sizeof(phys_constraint))) phys_constraint(
            (phys_entity *)rigid_body_a, (phys_entity *)rigid_body_b, constraint_data, phys_constraint::PRIORITY_PSI);
        add_constraint(constraint, physics_model_node_constraint_edge_constraint->type, physics_model_node_constraint_edge_constraint->index, motors_active);
        ((phys_ref_object *)constraint_data)->object_remove_ref();
        if (m_flags & 8)
            havok_component_chain_add_to_world(constraint);
    }
}



// @address 0x258de0 ?build_constraints@c_havok_component@@AAAXPAUs_physics_model_instance@@ABVc_runtime_node_to_havok_component_rigid_body_index_mapping@@@Z
void c_havok_component::build_constraints(s_physics_model_instance *instance, const c_runtime_node_to_havok_component_rigid_body_index_mapping &mapping)
{
    c_physics_model_constraint_bodies_iterator iterator((s_physics_model_definition *)tag_get('phmo', instance->model->physics_model_index));
    for (const s_physics_model_constraint_bodies *bodies = iterator.get(); bodies; bodies = iterator.get())
    {
        if (!object_constraint_is_broken(get_object_index(), iterator.get_constraint_type(), iterator.get_constraint_type_index()) &&
            bodies->node_a != -1)
        {
            long rigid_body_index_a = mapping.get_rigid_body_index(bodies->node_a);
            long rigid_body_index_b = mapping.get_rigid_body_index(bodies->node_b);
            if (rigid_body_index_a != rigid_body_index_b && rigid_body_index_a != -1 && (rigid_body_index_b == -1) == (bodies->node_b == -1) &&
                (!havok_component_get_rigid_body(rigid_body_index_a)->is_fixed_or_keyframed() ||
                    rigid_body_index_b != -1 && !havok_component_get_rigid_body(rigid_body_index_b)->is_fixed_or_keyframed()))
            {
                const s_physics_model_node_constraint_edge *edge = tag_block_get_element<s_physics_model_node_constraint_edge>(
                    &instance->physics_model->node_edges, bodies->node_edge_index);
                bool in_powered_chain = false;
                for (long constraint_index = 0; !in_powered_chain && constraint_index < get_constraint_count(); constraint_index++)
                {
                    c_constraint &constraint = m_component_constraints[constraint_index];
                    if (constraint.get_constraint_type() == 6)
                    {
                        const s_physics_model_powered_chain *chain = tag_block_get_element<s_physics_model_powered_chain>(
                            &instance->physics_model->powered_chains, constraint.get_constraint_type_index());
                        for (long node = 0; node < chain->nodes.count - 1; node++)
                        {
                            long node_a = *tag_block_get_element<short>(&chain->nodes, node);
                            long node_b = *tag_block_get_element<short>(&chain->nodes, node + 1);
                            if (node_a == edge->node_a && node_b == edge->node_b || node_b == edge->node_a && node_a == edge->node_b)
                            {
                                in_powered_chain = true;
                                break;
                            }
                        }
                    }
                }
                if (!in_powered_chain)
                {
                    for (long index = 0; index < edge->constraints.count; index++)
                    {
                        const s_physics_model_node_constraint_edge_constraint *edge_constraint =
                            tag_block_get_element<s_physics_model_node_constraint_edge_constraint>(&edge->constraints, index);
                        if (edge_constraint->type == iterator.get_constraint_type() && edge_constraint->index == iterator.get_constraint_type_index())
                        {
                            create_constraint(instance, &mapping, edge_constraint, 0, false);
                            break;
                        }
                    }
                }
            }
        }
        iterator.next();
    }
    m_flags &= ~0x80;
    for (long constraint_index = 0; constraint_index < get_constraint_count(); constraint_index++)
    {
        if (m_component_constraints[constraint_index].get_constraint_type() != 6 &&
            m_component_constraints[constraint_index].get_constraint()->get_entity_b() == (phys_entity *)g_havok_world->get_fixed_rigid_body())
        {
            if (m_object_postion_controlling_rigid_body_index != -1 &&
                havok_component_get_rigid_body(m_object_postion_controlling_rigid_body_index)
                == (phys_rigid_body *)m_component_constraints[constraint_index].get_constraint()->get_entity_a())
            {
                m_object_postion_controlling_rigid_body_index = -1;
                m_object_postion_controlling_node_index = -1;
            }
            m_flags |= 0x80;
            return;
        }
    }
}

// @address 0xf08c0 havok_component_update_phantom_state
extern "C" void havok_component_update_phantom_state(c_havok_component *component, bool force)
{
    if ((component->m_flags & 0x200) == 0 && (component->m_flags & 0x4000) == 0)
        return;
    object_datum *object = object_get(component->m_object_index);
    bool brighten = false;
    float max_value = 0.0f;
    unsigned char any_flag = (unsigned char)((~object->physics_flags >> 9) & 1);
    if (component->m_bodies_in_phantom != 0) {
        if (force) {
            for (long body_index = 0; body_index < component->m_bodies_in_phantom->m_size; body_index++) {
                float contribution = 0.0f;
                bool remove_body;
                bool active;
                bool succeeded = c_havok_component::update_phantom(component->m_object_index, force,
                    component->m_bodies_in_phantom->m_data[body_index], false, contribution, brighten, remove_body, active);
                any_flag |= active;
                if (max_value <= contribution)
                    max_value = contribution;
                if (!succeeded && remove_body) {
                    phys_array<c_havok_component::s_body_in_phantom> *bodies = component->m_bodies_in_phantom;
                    bodies->m_size--;
                    bodies->m_data[body_index] = bodies->m_data[bodies->m_size];
                    body_index--;
                }
            }
        }
        phys_array<c_havok_component::s_body_in_phantom> *bodies = component->m_bodies_in_phantom;
        if (bodies != 0 && bodies->m_size == 0) {
            havok_component_delete_phantom_bodies(bodies, 1);
            component->m_bodies_in_phantom = 0;
        }
    }
    float half_tick = (float)game_time_globals->ticks_per_second * 0.5f;
    long step_power = (long)(half_tick + 0.5f);
    /* recomputed inline so the two conversions keep separate fadds */
    long step_speed = (long)(((float)game_time_globals->ticks_per_second * 0.5f) + 0.5f);
    long current_power = object->phantom_power;
    long current_speed = object->phantom_speed;
    long new_power;
    if (brighten) {
        float scale;
        if (max_value > 0.0f && max_value < 1.0f)
            scale = max_value;
        else if ((max_value > 0.0f) == false)
            scale = 0.0f;
        else
            scale = 1.0f;
        long target = (long)(scale * 255.0f);
        if (target != current_speed) {
            if (target > current_speed)
                current_speed += step_speed;
            else
                current_speed -= step_speed;
            if (current_speed >= target)
                current_speed = target;
        }
        new_power = current_power + step_power;
    } else {
        new_power = current_power - step_power;
        current_speed -= step_speed;
    }
    if (any_flag != 0 || component->m_bodies_in_phantom == 0 || component->m_bodies_in_phantom->m_size == 0) {
        object->phantom_power = (unsigned char)PIN(new_power, 0, 255);
        object->phantom_speed = (unsigned char)PIN(current_speed, 0, 255);
    }
    if (force)
        object->physics_flags |= 0x20000;
    else
        object->physics_flags &= ~0x20000;
}


static __forceinline bool havok_component_group_flag(unsigned long flags, long bit)
{
    return (flags & (1UL << bit)) != 0;
}
static __forceinline int havok_component_size_group(long size, bool special)
{
    if (size <= 1)
        return special ? 18 : 4;
    if (size >= 5)
        return special ? 17 : 5;
    return special ? 17 : 3;
}
// @address 0x257ad0 havok_component_get_collision_group
extern "C" int havok_component_get_collision_group(
    c_havok_component *component, const s_physics_model_instance *instance, long size, long motion_type, long rigid_body_index)
{
    long object_index = component->get_object_index();
    s_object_header *header = object_header_get(object_index);
    object_datum *object = (object_datum *)header->datum;
    if (motion_type == 7)
    {
        switch (header->object_type)
        {
        case 1:
        {
            const vehicle_definition *definition =
                (const vehicle_definition *)tag_get('vehi', object->definition_index);
            return havok_component_group_flag(definition->flags, 11) ? 16 : 15;
        }
        case 6:
            return havok_component_group_flag(object->physics_flags, 20) ? 24 : 1;
        case _object_type_crate:
            if ((instance->physics_model->flags & 2) != 0)
                return havok_component_size_group(size, (instance->physics_model->flags & 2) != 0);
            break;
        }
        return 1;
    }
    return havok_get_object_collision_group(object_index, size);
}

// @address 0x254928 ?align_constraint_positions_recursive@c_havok_component@@AAAXJPAV?$c_static_flags@$0EA@@@@Z

static __forceinline void havok_component_constraint_rigid_bodies(const phys_constraint *instance, long *body_a, long *body_b)
{
    const phys_world_object *entity_a = instance->get_entity_a();
    const phys_world_object *entity_b = instance->get_entity_b();
    *body_a = (long)find_property_value(entity_a, 0x2002);
    long component_b = (long)find_property_value(entity_b, 0x2001);
    *body_b = component_b != 0 && component_b != -1 ? (long)find_property_value(entity_b, 0x2002) : -1;
}
static __forceinline real_point3d *havok_component_transform_point_with_forward(
    const real_matrix4x3 *matrix, const real_point3d *point, real_point3d *result, float fwd_i)
{
    float x = point->x, y = point->y, z = point->z;
    x *= matrix->scale; y *= matrix->scale; z *= matrix->scale;
    result->x = fwd_i * x + matrix->rotation.left.i * y + matrix->rotation.up.i * z + matrix->position.x;
    result->y = matrix->rotation.forward.j * x + matrix->rotation.left.j * y + matrix->rotation.up.j * z + matrix->position.y;
    result->z = matrix->rotation.forward.k * x + matrix->rotation.left.k * y + matrix->rotation.up.k * z + matrix->position.z;
    return result;
}
static __forceinline real_point3d *havok_component_transform_point(
    const real_matrix4x3 *matrix, const real_point3d *point, real_point3d *result)
{
    float x = point->x, y = point->y, z = point->z;
    x *= matrix->scale; y *= matrix->scale; z *= matrix->scale;
    result->x = matrix->rotation.forward.i * x + matrix->rotation.left.i * y + matrix->rotation.up.i * z + matrix->position.x;
    result->y = matrix->rotation.forward.j * x + matrix->rotation.left.j * y + matrix->rotation.up.j * z + matrix->position.y;
    result->z = matrix->rotation.forward.k * x + matrix->rotation.left.k * y + matrix->rotation.up.k * z + matrix->position.z;
    return result;
}
static __forceinline real_vector3d *havok_component_vector_from_points(const real_point3d *a, const real_point3d *b, real_vector3d *v)
{
    v->i = b->x - a->x;
    v->j = b->y - a->y;
    v->k = b->z - a->z;
    return v;
}
static __forceinline float havok_component_magnitude_squared(const real_vector3d *v) { return v->i * v->i + v->j * v->j + v->k * v->k; }
void c_havok_component::align_constraint_positions_recursive(long rigid_body_index, c_static_flags<64> *aligned)
{
    aligned->set(rigid_body_index, true);
    for (long constraint_index = 0; constraint_index < m_component_constraints.getSize(); constraint_index++) {
        long other = -1;
        phys_constraint *instance = (phys_constraint *)m_component_constraints[constraint_index].get_constraint();
        long body_a, body_b;
        havok_component_constraint_rigid_bodies(instance, &body_a, &body_b);
        real_point3d pivot_a, pivot_b;
        if (body_a == rigid_body_index && body_b != -1 && !aligned->test(body_b)) {
            other = body_b;
            get_constraint_pivots(constraint_index, &pivot_a, &pivot_b);
        } else if (body_b == rigid_body_index && body_a != -1 && !aligned->test(body_a)) {
            other = body_a;
            get_constraint_pivots(constraint_index, &pivot_b, &pivot_a);
        }
        if (other != -1) {
        real_matrix4x3 current, adjacent;
        get_rigid_body_transform(rigid_body_index, &current);
        get_rigid_body_transform(other, &adjacent);
        real_point3d current_pivot;
        havok_component_transform_point_with_forward(&current, &pivot_a, &current_pivot, current.rotation.forward.i);
        real_point3d adjacent_pivot;
        havok_component_transform_point(&adjacent, &pivot_b, &adjacent_pivot);
        real_vector3d delta;
        havok_component_vector_from_points(&current_pivot, &adjacent_pivot, &delta);
        if (!(havok_component_magnitude_squared(&delta) <= 0.0000010000001f)) {
            adjacent.position.x -= delta.i;
            adjacent.position.y -= delta.j;
            adjacent.position.z -= delta.k;
            phys_transform transform;
            havok_component_hkTransform_from_matrix4x3(&transform, &adjacent);
            set_rigid_body_transform_internal(other, &transform);
        }
        align_constraint_positions_recursive(other, aligned);
        }
    }
}
static __forceinline long havok_component_mapping_get_rigid_body_count(const c_runtime_node_to_physics_model_rigid_body_index_mapping *mapping, long node_index)
{
    if (node_index == -1)
        return mapping->m_unmapped_rigid_bodies.count;
    return mapping->m_node_to_rigid_body_mapping[node_index].count;
}

static __forceinline long havok_component_mapping_get_rigid_body_index(
    const c_runtime_node_to_physics_model_rigid_body_index_mapping *mapping, long node_index, long index)
{
    if (node_index == -1)
        return mapping->m_unmapped_rigid_bodies.indices[index];
    return mapping->m_node_to_rigid_body_mapping[node_index].indices[index];
}


static __forceinline s_havok_selected_rigid_shape havok_component_select_rigid_shape(const s_physics_model_instance *instance,
    bool use_alternate, long rigid_body_index)
{
    s_havok_selected_rigid_shape result;
    result.has_alternate = false;
    result.alternate_used = false;
    const s_physics_model_rigid_body *body = tag_block_get_element<s_physics_model_rigid_body>(&instance->physics_model->rigid_bodies, rigid_body_index);
    if (body->flags & (1 << 5)) {
        long alternate = body->alternate_rigid_body;
        if (alternate >= 0 && alternate < instance->physics_model->rigid_bodies.count()) {
            result.has_alternate = true;
            if (use_alternate) {
                result.alternate_used = true;
                rigid_body_index = alternate;
            }
        }
    }
    result.index = (char)rigid_body_index;
    return result;
}

// @address 0x257690 ?generate_rigidly_constrained_shape_list_recursive@c_havok_component@@AAAXPBUs_physics_model_instance@@PBDPBV?$c_static_flags@$0BAAA@@@PBVc_runtime_node_to_physics_model_rigid_body_index_mapping@@JJJPADPAJJPAV?$c_static_flags@$0PP@@@PA_N@Z
void c_havok_component::generate_rigidly_constrained_shape_list_recursive(const s_physics_model_instance *instance,
    const char *motion_types, const c_static_flags<4096> *rigid_edges,
    const c_runtime_node_to_physics_model_rigid_body_index_mapping *mapping,
    long first_node, long previous_node, long node_index, char *rigid_body_indices, long *rigid_body_count,
    long maximum_count, c_static_flags<255> *visited_nodes, bool *constrained_to_world)
{
    const object_datum *object = (const object_datum *)object_get_and_verify_type(get_object_index(), -1);
    bool use_alternate = (object->physics_flags & (1 << 17)) == 0;
    if (mapping->get_rigid_body_count(node_index) > 0) {
        for (long index = 0; index < mapping->get_rigid_body_count(node_index); ++index) {
            long rigid_body_index = havok_component_mapping_get_rigid_body_index(mapping, node_index, index);
            s_havok_selected_rigid_shape shape = havok_component_select_rigid_shape(instance, use_alternate, rigid_body_index);
            rigid_body_indices[*rigid_body_count] = shape.index;
            ++*rigid_body_count;
            if (shape.alternate_used)
                m_flags |= 0x4000;
            if (shape.has_alternate)
                m_flags |= 0x2000;
        }
        if (node_index != -1)
            visited_nodes->set(node_index, true);
        for (long edge_index = 0; edge_index < instance->physics_model->node_edges.count; ++edge_index) {
            const s_physics_model_node_constraint_edge *edge = tag_block_get_element<s_physics_model_node_constraint_edge>(
                &instance->physics_model->node_edges, edge_index);
            if (rigid_edges->test(edge_index)) {
                if (edge->node_a == node_index || edge->node_b == node_index) {
                    long next_node = edge->node_a == node_index ? edge->node_b : edge->node_a;
                    long next_body;
                    if (next_node != -1 && havok_component_mapping_get_rigid_body_count(mapping, next_node) > 0)
                        next_body = havok_component_mapping_get_rigid_body_index(mapping, next_node, 0);
                    else
                        next_body = -1;
                    if (next_body == -1 || (motion_types[next_body] != 6 && !visited_nodes->test(next_node))) {
                        bool connected = true;
                        if (next_node == -1) {
                            connected = false;
                            for (long constraint_index = 0; constraint_index < edge->constraints.count; ++constraint_index) {
                                const s_physics_model_node_constraint_edge_constraint *constraint =
                                    tag_block_get_element<s_physics_model_node_constraint_edge_constraint>(&edge->constraints, constraint_index);
                                if (!object_constraint_is_loosened(get_object_index(), (e_physics_model_constraint_type)constraint->type, constraint->index)
                                    && !object_constraint_is_broken(get_object_index(), (e_physics_model_constraint_type)constraint->type, constraint->index)) {
                                    connected = true;
                                    break;
                                }
                            }
                        } else if (havok_component_mapping_get_rigid_body_count(mapping, next_node) > 0) {
                            if (edge->constraints.count == 0) {
                                if (motion_types[mapping->get_rigid_body_index(node_index, 0)] == 7
                                    || motion_types[havok_component_mapping_get_rigid_body_index(mapping, next_node, 0)] == 7)
                                    connected = false;
                            } else {
                                connected = false;
                                for (long constraint_index = 0; constraint_index < edge->constraints.count; ++constraint_index) {
                                    const s_physics_model_node_constraint_edge_constraint *constraint =
                                        tag_block_get_element<s_physics_model_node_constraint_edge_constraint>(&edge->constraints, constraint_index);
                                    if (!object_constraint_is_loosened(get_object_index(), (e_physics_model_constraint_type)constraint->type, constraint->index)
                                        && !object_constraint_is_broken(get_object_index(), (e_physics_model_constraint_type)constraint->type,
                                        constraint->index))
                                        {
                                        connected = true;
                                        break;
                                    }
                                }
                            }
                        }
                        if (connected) {
                            if (next_node == -1)
                                *constrained_to_world = true;
                            else if (next_body != -1)
                                generate_rigidly_constrained_shape_list_recursive(instance, motion_types, rigid_edges, mapping,
                                    first_node, node_index, next_node, rigid_body_indices, rigid_body_count, maximum_count,
                                    visited_nodes, constrained_to_world);
                        }
                    }
                }
            }
        }
    }
}


static __forceinline real_vector3d *havok_component_phantom_subtract(const real_point3d *a, const real_point3d *b, real_vector3d *v)
{ v->i=b->x-a->x; v->j=b->y-a->y; v->k=b->z-a->z; return v; }
static __forceinline float havok_component_phantom_dot(const real_vector3d *a, const real_vector3d *b)
{ return a->i*b->i + a->j*b->j + a->k*b->k; }
static __forceinline float havok_component_phantom_direction_dot(const real_vector3d *a, const real_vector3d *b)
{
    return a->k*b->k + ((float)(a->i*b->i) + a->j*b->j);
}
static __forceinline real_vector3d *havok_component_phantom_scale(const real_vector3d *a, float k, real_vector3d *v)
{ v->i=a->i*k; v->j=a->j*k; v->k=a->k*k; return v; }
static __forceinline real_vector3d *havok_component_phantom_damping_scale(float k, const real_vector3d *a, real_vector3d *v)
{ v->i=a->i*k; v->j=a->j*k; v->k=a->k*k; return v; }
static __forceinline real_vector3d *havok_component_phantom_difference(const real_vector3d *a, const real_vector3d *b, real_vector3d *v)
{ v->i=a->i-b->i; v->j=a->j-b->j; v->k=a->k-b->k; return v; }
static __forceinline real_vector3d *havok_component_phantom_add(const real_vector3d *a, const real_vector3d *b, real_vector3d *result)
{ result->i=a->i+b->i; result->j=a->j+b->j; result->k=a->k+b->k; return result; }
static __forceinline float havok_component_phantom_magnitude_squared(const real_vector3d *v)
{ return v->j*v->j + ((float)(v->i*v->i) + v->k*v->k); }
static __forceinline float havok_component_phantom_magnitude(const real_vector3d *v)
{ return sqrtf(havok_component_phantom_magnitude_squared(v)); }
static __forceinline bool havok_component_phantom_test(unsigned long flags, long bit) { return (flags & (1UL << bit)) != 0; }
static __forceinline s_havok_phantom_activity havok_component_phantom_activity(const s_physics_model_phantom_type *type, unsigned long affected_flags)
{
    s_havok_phantom_activity result = {
        havok_component_phantom_test(type->flags, 30),
        !havok_component_phantom_test(affected_flags, 9)
    };
    return result;
}
static __forceinline float havok_component_phantom_abs(float x)
{ return x >= 0.0f ? x : -x; }



static __forceinline bool havok_component_phantom_dynamic(const c_havok_component *component, long index)
{ return !component->get_rigid_body_is_fixed(index) && !component->get_rigid_body_is_keyframed(index); }
// @address 0x252c58 ?update_phantom@c_havok_component@@SA_NJ_NABUs_body_in_phantom@1@0AAMAA_N33@Z
bool c_havok_component::update_phantom(long object_index, bool force, const s_body_in_phantom &body,
bool reverse, float &maximum_scale, bool &accelerated, bool &updated, bool &was_active)
{

    s_object_header *headers = ((s_object_header *)object_header_data->data);
    object_datum *object = (object_datum *)headers[datum_index_to_absolute_index(object_index)].datum;
    long component_index = object->havok_component_index;
    c_havok_component *component = havok_component_get(component_index);
    bool force_update;
    if (!force || havok_component_phantom_test(object->physics_flags, 17)) force_update = false;
    else force_update = true;
    long other_object_index = body.object_index;
    bool result = false;
    updated = false;
    was_active = false;
    if (other_object_index != -1) {
        object_datum *other = (object_datum *)headers[datum_index_to_absolute_index(other_object_index)].datum;
        if (other->havok_component_index != -1 && entity_should_update_in_phantom(component_index, other->havok_component_index, body.material_index)) {
            s_object_definition *definition = (s_object_definition *)tag_get('obje', object->definition_index);
            s_model_definition *model = (s_model_definition *)tag_get('hlmt', definition->model.index);
            s_physics_model_definition *physics = (s_physics_model_definition *)tag_get('phmo', model->physics_model_index);
            s_physics_model_phantom *phantom = tag_block_get_element<s_physics_model_phantom>(&physics->phantoms, body.material_index);
            s_physics_model_phantom_type *type = tag_block_get_element<s_physics_model_phantom_type>(&physics->phantom_types, phantom->phantom_type_index);
            long source_index = reverse ? other_object_index : object_index;
            long affected_index = reverse ? object_index : other_object_index;
            long source_body = reverse ? body.rigid_body_index : body.physics_model_material_index;
            long affected_body = reverse ? body.physics_model_material_index : body.rigid_body_index;
            object_datum *source = (object_datum *)headers[datum_index_to_absolute_index(source_index)].datum;
            object_datum *affected = (object_datum *)headers[datum_index_to_absolute_index(affected_index)].datum;
            c_havok_component *source_component = havok_component_get(source->havok_component_index);
            c_havok_component *affected_component = havok_component_get(affected->havok_component_index);
            if (havok_component_phantom_test(type->flags, 28) && !reverse && body.physics_model_material_index != -1 && body.rigid_body_index != -1) {
                bool reverse_updated, reverse_active;
                result = update_phantom(object_index, force, body, true, maximum_scale, accelerated, reverse_updated, reverse_active);
                updated |= reverse_updated;
            }
            unsigned long affected_flags = affected->physics_flags;
            s_havok_phantom_activity activity = havok_component_phantom_activity(type, affected_flags);
            was_active = activity.was_active;
            if (!activity.preserve_inactive || havok_component_phantom_test(affected_flags, 16) || havok_component_phantom_test(affected_flags, 15)) {
                updated = true; affected->physics_flags |= 1 << 16;
            }
            if ((havok_component_phantom_test(affected->physics_flags, 16) || havok_component_phantom_test(affected->physics_flags, 15)) &&
            (affected_body == -1 || affected_body >= 0 && affected_body < affected_component->get_rigid_body_count()) &&
            (reverse || !havok_component_phantom_test(type->flags, 29))) {
                bool scale_by_mass = (type->flags & 2) != 0;
                bool ignore_gravity;
                if (reverse || !havok_component_phantom_test(type->flags, 2)) ignore_gravity = false;
                else ignore_gravity = true;
                bool source_active = (havok_component_phantom_dynamic(component, body.physics_model_material_index) &&
                ((phys_entity *)component->havok_component_get_rigid_body(body.physics_model_material_index))->is_body_active()) || force_update;
                bool should_update;
                if (havok_component_phantom_test(affected->physics_flags, 9) && !source_active) should_update = false;
                else should_update = true;
                if (should_update) {
                    real_vector3d acceleration = *global_zero_vector3d;
                    long object_type = object_header_get(affected_index)->object_type;
                    bool instant_linear = havok_component_phantom_test(type->flags, 25);
                    bool instant_angular = havok_component_phantom_test(type->flags, 26);
                    object_marker marker;
                    if (!object_get_markers_by_string_id(object_index, type->marker_name, &marker, 1))
                    object_get_world_matrix(object_index, &marker.matrix);
                    long type_mask = 1 << object_type;
                    if (type_mask & 0x1003)
                    ((unit_datum *)object_get_and_verify_type(affected_index, -1))->unit.last_in_phantom_volume_time = game_time_globals->time;
                    if (type_mask & 1)
                    ((biped_datum *)object_get_and_verify_type(affected_index, -1))->biped.flags |= 4;
                    else if (type_mask & 0x800)
                    ((creature_datum *)object_get_and_verify_type(affected_index, -1))->creature.flags |= 0x40;
                    real_vector3d velocity, angular_velocity;
                    real_point3d affected_center, source_center;
                    if (affected_body == -1) {
                        object_get_velocities(affected_index, &velocity, &angular_velocity);
                        object_get_center_of_mass(affected_index, &affected_center);
                    } else {
                        affected_component->get_rigid_body_center_of_mass(affected_body, &affected_center);
                        affected_component->get_rigid_body_angular_velocity(affected_body, &angular_velocity);
                        affected_component->get_rigid_body_linear_velocity(affected_body, &velocity);
                    }
                    if (source_body == -1) object_get_center_of_mass(source_index, &source_center);
                    else source_component->get_rigid_body_center_of_mass(source_body, &source_center);
                    const real_point3d *from = reverse ? &marker.matrix.position : &affected_center;
                    const real_point3d *to = reverse ? &source_center : &marker.matrix.position;
                    if (type->radial_acceleration != 0.0f && type->radial_maximum_velocity != 0.0f) {
                        real_vector3d difference; havok_component_phantom_subtract(from, to, &difference);
                        real_vector3d direction = difference;
                        float distance = normalize3d(&direction);
                        if (distance == 0.0f) direction = *global_up3d;
                        float speed = havok_component_phantom_direction_dot(&direction, &velocity);
                        float scale;
                        float mass = affected_body == -1 ? 1.0f : affected_component->get_rigid_body_mass(affected_body);
                        float magnitude = calculate_acceleration_magnitude(true, scale_by_mass, mass,
                        distance, type->radial_acceleration, speed, type->radial_maximum_velocity, type->linear_dead_zone, type->linear_exponent,
                            instant_linear, &scale);
                        real_vector3d delta; havok_component_phantom_scale(&direction, magnitude, &delta);
                        float limit = type->radial_acceleration * game_time_globals->tick_length;
                        if (!ignore_gravity && magnitude < limit) {
                            float gravity = g_physics_constants->gravity * game_time_globals->tick_length;
                            delta.k += gravity < limit - magnitude ? gravity : limit - magnitude;
                        }
                        if (distance - type->linear_dead_zone >= 0.0f) {
                            real_vector3d parallel; havok_component_phantom_scale(&direction, speed, &parallel);
                            real_vector3d transverse; havok_component_phantom_difference(&velocity, &parallel, &transverse);
                            float transverse_speed = havok_component_phantom_magnitude(&transverse);
                            if (havok_component_phantom_abs(transverse_speed) > 0.001f) {
                                if (transverse_speed > limit) {
                                    real_vector3d scaled;
                                    havok_component_phantom_scale(&transverse,
                                        1.0f - (transverse_speed - havok_component_phantom_abs(limit)) / transverse_speed, &scaled);
                                    havok_component_phantom_difference(&delta, &scaled, &delta);
                                } else {
                                    havok_component_phantom_difference(&delta, &transverse, &delta);
                                }
                            }
                            maximum_scale = maximum_scale > scale ? maximum_scale : scale;
                            accelerated = true;
                        }
                        havok_component_phantom_add(&delta, &acceleration, &acceleration);
                    }
                    const real_vector3d *axis = &marker.matrix.rotation.forward;
                    if (type->centripetal_acceleration != 0.0f && type->centripetal_maximum_velocity != 0.0f) {
                        real_vector3d difference; havok_component_phantom_subtract(from, to, &difference);
                        real_vector3d axial; havok_component_phantom_scale(axis, havok_component_phantom_dot(axis, &difference), &axial);
                        real_vector3d direction; havok_component_phantom_difference(&difference, &axial, &direction);
                        float distance = normalize3d(&direction);
                        if (distance != 0.0f) {
                            float speed = havok_component_phantom_direction_dot(&direction, &velocity);
                            float scale;
                            float mass = affected_body == -1 ? 1.0f : affected_component->get_rigid_body_mass(affected_body);
                            float magnitude = calculate_acceleration_magnitude(true, scale_by_mass, mass,
                            distance, type->centripetal_acceleration, speed, type->centripetal_maximum_velocity, type->linear_dead_zone,
                                type->linear_exponent, instant_linear, &scale);
                            real_vector3d delta; havok_component_phantom_scale(&direction, magnitude, &delta);
                            if (distance - type->linear_dead_zone >= 0.0f) {
                                real_vector3d axial_velocity;
                                havok_component_phantom_scale(
                                    axis, havok_component_phantom_direction_dot(axis, &velocity), &axial_velocity);
                                real_vector3d planar_velocity; havok_component_phantom_difference(&velocity, &axial_velocity, &planar_velocity);
                                real_vector3d parallel; havok_component_phantom_scale(&direction, speed, &parallel);
                                real_vector3d transverse; havok_component_phantom_difference(&planar_velocity, &parallel, &transverse);
                                float transverse_speed = havok_component_phantom_magnitude(&transverse);
                                if (havok_component_phantom_abs(transverse_speed) > 0.001f) {
                                    if (transverse_speed > type->centripetal_acceleration * game_time_globals->tick_length) {
                                        real_vector3d scaled;
                                        havok_component_phantom_damping_scale(1.0f -
                                                (transverse_speed -
                                                    (type->centripetal_acceleration >= 0.0f ? type->centripetal_acceleration
                                                                                            : -type->centripetal_acceleration) *
                                                        game_time_globals->tick_length) /
                                                    transverse_speed,
                                            &transverse, &scaled);
                                        havok_component_phantom_difference(&delta, &scaled, &delta);
                                    } else {
                                        havok_component_phantom_difference(&delta, &transverse, &delta);
                                    }
                                }
                                maximum_scale = maximum_scale > scale ? maximum_scale : scale;
                                accelerated = true;
                            }
                            havok_component_phantom_add(&delta, &acceleration, &acceleration);
                        }
                    }
                    if (type->axial_acceleration != 0.0f && type->axial_maximum_velocity != 0.0f) {
                        bool reverse_acceleration = havok_component_phantom_test(type->flags, 31);
                        float magnitude = type->axial_acceleration;
                        float speed = havok_component_phantom_direction_dot(axis, &velocity);
                        if (reverse_acceleration) {
                            real_vector3d difference; havok_component_phantom_subtract(from, to, &difference);
                            if (havok_component_phantom_dot(&difference, axis) > 0.0f) magnitude *= -1.0f;
                        }
                        float scale;
                        float mass = affected_body == -1 ? 1.0f : affected_component->get_rigid_body_mass(affected_body);
                        magnitude = calculate_acceleration_magnitude(true, scale_by_mass, mass,
                        10000.0f, magnitude, speed, type->axial_maximum_velocity, type->linear_dead_zone, type->linear_exponent, instant_linear, &scale);
                        real_vector3d delta; havok_component_phantom_scale(axis, magnitude, &delta);
                        havok_component_phantom_add(&delta, &acceleration, &acceleration);
                        maximum_scale = maximum_scale > scale ? maximum_scale : scale;
                        accelerated = true;
                    }
                    if (ignore_gravity) acceleration.k += game_time_globals->tick_length * g_physics_constants->gravity;
                    if (affected_body == -1) {
                        object_apply_acceleration(affected_index, -1, 0, &acceleration, 0);
                    } else {
                        affected_component->rigid_body_apply_acceleration(affected_body, &acceleration);
                        if (!affected_component->get_rigid_body_has_infinite_inertia_tensor(affected_body)) {
                            if (type->alignment_marker_name != 1) object_get_markers_by_string_id(object_index, type->alignment_marker_name, &marker, 1);
                            real_vector3d angular_acceleration = *global_zero_vector3d;
                            if (type->angular_acceleration != 0.0f && type->angular_maximum_velocity != 0.0f) {
                                real_matrix4x3 transform;
                                real_matrix3x3 rotation;
                                real_quaternion quaternion;
                                real_vector3d direction;
                                float angle;
                                affected_component->get_rigid_body_transform(affected_body, &transform);
                                matrix3x3_transpose(&transform.rotation, &transform.rotation);
                                matrix3x3_multiply(&marker.matrix.rotation, &transform.rotation, &rotation);
                                matrix3x3_rotation_to_quaternion(&rotation, &quaternion);
                                quaternion_to_angle_and_vector(&quaternion, &angle, &direction);
                                if (normalize3d(&direction) == 0.0f) { direction = angular_velocity; normalize3d(&direction); }
                                float speed = havok_component_phantom_dot(&angular_velocity, &direction);
                                float mass = affected_body == -1 ? 1.0f : affected_component->get_rigid_body_mass(affected_body);
                                float scale;
                                float magnitude = calculate_acceleration_magnitude(true, scale_by_mass, mass, angle,
                                type->angular_acceleration, speed, type->angular_maximum_velocity, 0.0f, type->angular_exponent, instant_angular, &scale);
                                havok_component_phantom_scale(&direction, magnitude, &angular_acceleration);
                                real_vector3d parallel; havok_component_phantom_scale(&direction, speed, &parallel);
                                real_vector3d transverse; havok_component_phantom_difference(&angular_velocity, &parallel, &transverse);
                                limit3d(&transverse, game_time_globals->tick_length * type->angular_acceleration);
                                real_vector3d negative; havok_component_phantom_scale(&transverse, -1.0f, &negative);
                                havok_component_phantom_add(&angular_acceleration, &negative, &angular_acceleration);
                                maximum_scale = maximum_scale > scale ? maximum_scale : scale;
                                accelerated = true;
                            }
                            havok_component_phantom_add(&angular_velocity, &angular_acceleration, &angular_velocity);
                            affected_component->set_rigid_body_angular_velocity(affected_body, &angular_velocity);
                        }
                    }
                }
                result = true;
            }
        }
    }
    return result;
}

// ---- muffin-piece update ----
static __forceinline short havok_component_muffin_random_range(short minimum, short maximum)
{
    unsigned long *seed = get_local_random_seed_address();
    random_seed_next(seed);
    unsigned long range = (unsigned long)(maximum - minimum) * (unsigned short)(*seed >> 16);
    return (short)((range >> 16) + minimum);
}
static __forceinline float havok_component_muffin_random_ticks(const s_muffin_definition_piece *definition)
{
    short lifetime = havok_component_muffin_random_range((short)definition->minimum_lifetime, (short)(definition->maximum_lifetime + 1.0f));
    return (float)lifetime * (float)game_time_globals->ticks_per_second;
}
static __forceinline float havok_component_muffin_curve(const c_function_definition *curve, float input)
{
    float result = 0.0f;
    if ((const void *)curve->m_data.address != 0 && curve->m_data.size > 0)
        result = curve->map_to_output_range_legacy(curve->evaluate_legacy(input, 0.5f));
    return result;
}

// @address 0x36cb40 havok_muffin_update
extern "C" void havok_muffin_update(long muffin_index, float time)
{
    s_muffin_datum *muffin = &((s_muffin_datum *)tls_muffin_data()->data)[(unsigned short)muffin_index];
    const s_muffin_definition *definition = (const s_muffin_definition *)tag_get('mufn', muffin->definition_index);
    for (long index = 0; index < muffin->piece_count; ++index) {
        s_muffin_piece *piece = &muffin->pieces[index];
        bool destroy = false;
        if (piece->countdown != 0) {
            if (--piece->countdown == 0) {
                if (piece->flags & 2)
                    destroy = true;
                else if (!(piece->flags & 1))
                    piece->flags = (piece->flags & ~1) | 1;
            } else if (!(piece->flags & 2))
                continue;
        }
        if (piece->flags & 4)
            continue;
        {
            const s_muffin_definition_piece *piece_definition = &definition->pieces[piece->node_index];
            matrix4x3_compute_product(object_get_node_matrix(muffin->object_index, piece->object_node_index), &piece->matrix, &piece->render_matrices[0]);
            piece->flags |= 16;
            if (piece->flags & 1) {
                point_from_line3d(&piece->render_matrices[0].position, &piece->render_matrices[0].rotation.up,
                    piece_definition->distance, &piece->position);
                set_real_vector3d(&piece->velocity, 0.0f, 0.0f, 0.0f);
                piece->flags = (piece->flags & ~3) | 2;
                if (piece->flags & 8)
                    piece->countdown = 0;
                else
                    piece->countdown = (unsigned short)MIN(65535.0f, havok_component_muffin_random_ticks(piece_definition));
                piece->start_time = game_time_globals->time;
            }
            if (piece->flags & 2) {
                object_marker marker;
                if (internal_object_get_markers_by_string_id(muffin->object_index, piece->marker_name, &marker, 1, false) <= 0)
                    destroy = true;
                havok_component_update_transform_spring(
                    piece, piece_definition);
                float elapsed = (float)(game_time_globals->time - piece->start_time) * game_time_globals->tick_length;
                float fraction = elapsed / piece_definition->curve_duration;
                fraction = PIN(fraction, 0.0f, 1.0f);
                float lifetime_scale = havok_component_muffin_curve(&piece_definition->lifetime_curve, fraction);
                float scale = havok_component_muffin_curve(&piece_definition->scale_curve, elapsed);
                float result_scale = (float)piece->scale * scale * lifetime_scale * 0.00390625f;
                piece->render_matrices[0].scale = result_scale;
                piece->render_matrices[1].scale = result_scale;
            }
            matrix3x3_normalize(&piece->render_matrices[0].rotation);
            matrix3x3_normalize(&piece->render_matrices[1].rotation);
            if (destroy)
                muffins_destroy_piece_with_effect(piece, muffin, piece_definition);
        }
    }
}

typedef char havok_component_motion_transform_offset[(sizeof(void *) != 4 || ((offsetof(phys_motion, m_motion_state) == 0x10))) ? 1 : -1];
typedef char havok_component_motion_inertia_offset[(sizeof(void *) != 4 || ((offsetof(phys_motion, m_inv_inertia_mass) == 0xd0))) ? 1 : -1];
typedef char havok_component_collidable_size[(sizeof(void *) != 4 || ((sizeof(phys_collider) == 0x24))) ? 1 : -1];
typedef char havok_component_rigid_body_motion_offset[(sizeof(void *) != 4 || ((offsetof(phys_rigid_body, m_motion) == 0xa0))) ? 1 : -1];
typedef char havok_component_world_fixed_body_offset[(sizeof(void *) != 4 || ((offsetof(phys_world, m_fixed_body) == 0x24))) ? 1 : -1];
// Natural member alignment supplies the removed explicit gaps and tails.
typedef char havok_component_contact_process_size[(sizeof(phys_process_hit_point) == 0x30 && offsetof(phys_process_collision_data,
    m_process_contact_points) == 0x10) ? 1 : -1];
typedef char havok_component_construction_template_size[
    (sizeof(void *) != 4 || ((sizeof(c_havok_component::s_rigid_body_construction_template) == 0x4c))) ? 1 : -1];
typedef char havok_component_phantom_body_size[(sizeof(c_havok_component::s_body_in_phantom) == 0xc) ? 1 : -1];
typedef char havok_component_rigid_body_entry_size[(sizeof(void *) != 4 || ((sizeof(c_havok_component::c_rigid_body) == 0x48))) ? 1 : -1];
typedef char havok_component_flags_impact_offset[(offsetof(c_havok_component, m_impact_array_index) == 0x10) ? 1 : -1];
typedef char havok_component_iterator_size[(sizeof(c_havok_contact_point_iterator) == 0xc) ? 1 : -1];
typedef char havok_component_collision_aabb_offset[(sizeof(void *) != 4 || ((sizeof(c_collision_geometry_shape) == 0x50))) ? 1 : -1];
typedef char havok_component_stiff_spring_size[(sizeof(void *) != 4 || ((sizeof(phys_stiff_spring_data) == 0x50))) ? 1 : -1];
typedef char s_character_physics_dead_sphere_size[(sizeof(void *) != 4 || ((sizeof(s_character_physics_dead_sphere) == 0x70))) ? 1 : -1];
typedef char havok_component_contact_properties_size[
    (sizeof(void *) != 4 || ((sizeof(phys_contact_point_props) == 0x14
    && offsetof(phys_contact_point_props, m_contact_friction) == 0xc))) ? 1 : -1];
typedef char havok_component_property_size[(sizeof(phys_property) == 0x10 && offsetof(phys_property, m_value) == 8) ? 1 : -1];
typedef char havok_component_phantom_query_body_size[(sizeof(s_phantom_body) == 0x10 && offsetof(s_phantom_body, object_index) == 4) ? 1 : -1];

// @address 0xaf8 k_havok_pack_minimum
// @address 0xafc k_havok_pack_maximum
