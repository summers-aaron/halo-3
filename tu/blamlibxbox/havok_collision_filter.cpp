// blamlibxbox/havok_collision_filter.obj: from the records 0x105330 0x105678 0x105700 0x105b40 0x105ca8 0x25eeb8 0x262370 0x262488 0x262560 0x262590 0x2626a0 0x262790 0x262880 0x262d08 0x262d70 0x262d78 0x262d80 0x262d88
// @flags /TP /O2 /Oc
#include "objects/object_datum.h"
#include <stddef.h>
#include "memory/datum_index.h"
#include "physics/physics_model_primitive.h"
#include "phys/phys_dynamics.h"
#include "phys/phys_group_filter.h"
#include "phys/phys_transform_shape.h"
#include "phys/phys_bounding_shape.h"
#include "physics/havok_component_class.h"
#include "physics/havok_component_globals.h"
#include "physics/havok_environment_surface_shape.h"
#include "physics/havok_environment_mopp_bv_tree_shape.h"
#include "memory/data_types.h"
#include "memory/tls_globals.h"
#include "physics/havok_monitor_storage.h"
// @address 0x74b910 g_havok_monitor_globals
// @address 0x105ca8 havok_collision_filter_test_phantom_collision
// @address 0x262880 ?havok_initialize_collision_filter@@YAPAVphys_group_filter@@XZ
// @address 0x25eeb8 ?havok_group_soft_surface_sink_class_get@@YA?AW4e_soft_surface_sink_class@@W4e_havok_group@@@Z
// @address 0x262488 ?allows_collision@c_group_filter@@UBA?AVphys_bool@@ABUphys_ray_input@@ABVphys_shape@@ABVphys_shape_container@@I@Z
// @address 0x262590 havok_collision_filter_get_primitive
// @address 0x2626a0 group_filter_is_phantom_shape_a_versus_phantom_b_collision
// @address 0x105678 group_filter_is_phantom_shape_versus_phantom_collision
// @address 0x105700 group_filter_is_phantom_shape_versus_environment_collision
// @address 0x262370 ?allows_collision@c_group_filter@@UBA?AVphys_bool@@ABVphys_collider@@0@Z
// @address 0x262790 group_filter_is_no_collision_with_environment_rigid_body_vs_environment
// @address 0x105b40 havok_collision_filter_try_and_get_primitive_collision
// @address 0x625c50 ??0phys_group_filter@@QAA@XZ
// @address 0x625b60 ?disable_layer_bits@phys_group_filter@@QAAXII@Z
// @address 0x625a98 ?enable_collisions_by_mask@phys_group_filter@@QAAXII@Z
// @address 0x625af0 ?enable_collisions_between@phys_group_filter@@QAAXHH@Z
// @address 0x625b28 ?disable_layer_pair@phys_group_filter@@QAAXHH@Z
// @address 0x60a298 ?allocate_chunk@phys_thread_memory@@QAAPAXHW4phys_memory_class@@@Z
// @address 0x625a48 ?allows_collision@phys_group_filter@@UBA?AVphys_bool@@ABUphys_ray_input@@ABVphys_shape@@ABVphys_shape_container@@I@Z
// @address 0x625d40 ?allows_collision@phys_group_filter@@UBA?AVphys_bool@@ABVphys_collider@@0@Z
// @address 0x625d50 ?allows_collision@phys_group_filter@@UBA?AVphys_bool@@ABUphys_world_ray_input@@ABVphys_collider@@@Z
#include "physics/havok_collision_filter.h"
#include "physics/havok_component.h"
#include "physics/havok_runtime_pool.h"

#include "physics/havok_collision_surface_shape.h"
#include "physics/havok_shape_key.h"



#include "phys/phys_base_types.h"

#include "phys/phys_thread_memory.h"
// Compiler-generated: the vtables of c_group_filter, its deleting destructor and the this-adjusting thunks of its
// secondary vtables.
// @address 0x233c ??_7c_group_filter@@6Bphys_ref_object@@@
// @address 0x40690 ??_7c_group_filter@@6Bphys_collider_pair_filter@@@
// @address 0xc98 ??_7c_group_filter@@6Bphys_compound_shape_filter@@@
// @address 0x40698 ??_7c_group_filter@@6Bphys_ray_compound_shape_filter@@@
// @address 0x406a0 ??_7c_group_filter@@6Bphys_ray_collider_filter@@@
// @address 0x262d08 ??_Gc_group_filter@@UAAPAXI@Z
// @address 0x262d80 ??_Ec_group_filter@@W7AAPAXI@Z
// @address 0x262d88 ??_Ec_group_filter@@WM@AAPAXI@Z
// @address 0x262d70 ??_Ec_group_filter@@WBA@AAPAXI@Z
// @address 0x262d78 ??_Ec_group_filter@@WBE@AAPAXI@Z
struct phys_ray_input;
struct phys_world_ray_input;
struct phys_collision_input;
class phys_shape;
class phys_shape_container;
class phys_collision_proxy;
class c_group_filter : public phys_group_filter {
public:
    c_group_filter() {}
    virtual phys_bool allows_collision(const phys_collider &a, const phys_collider &b) const;
    virtual phys_bool allows_collision(const phys_collision_input &input,
        const phys_collision_proxy &body_a,
        const phys_collision_proxy &body_b,
        const phys_shape_container &container_b,
        unsigned int key_b) const;
    virtual phys_bool allows_collision(const phys_ray_input &input,
        const phys_shape &shape,
        const phys_shape_container &container,
        unsigned int key) const;
    virtual phys_bool allows_collision(const phys_world_ray_input &input,
        const phys_collider &collidable) const;
private:
    __forceinline bool is_environment_mopp_versus_mopp_collision(
        const phys_collision_proxy &body_a,
        const phys_shape_container &container_b,
        unsigned int key_b) const;
    __forceinline bool is_disable_gravity_lift_rigid_body_collision(
        unsigned long filter_info_a,
        unsigned long filter_info_b,
        const phys_collision_proxy &body_a,
        const phys_shape_container &container_b,
        unsigned int key_b) const;
    __forceinline bool is_dead_environment_shape_key(
        long group_a,
        const phys_shape &shape_b,
        unsigned int key_b) const;
    __forceinline bool is_primitive_collision(
        const phys_collision_proxy &body_a,
        const phys_collision_proxy &body_b,
        const phys_shape_container &container_b,
        unsigned int key_b,
        bool *is_collision_enabled) const;
    char m_storage[256 - sizeof(phys_group_filter)];
};


// @address 0x262560 ?allows_collision@c_group_filter@@UBA?AVphys_bool@@ABUphys_world_ray_input@@ABVphys_collider@@@Z

namespace {
static __forceinline bool havok_collision_filter_is_convex_shape(const phys_shape *shape)
{
    bool convex = false;
    switch (shape->get_type_id()) {
    case PHYS_SHAPE_CONVEX:
    case PHYS_SHAPE_SPHERE:
    case PHYS_SHAPE_TRIANGLE:
    case PHYS_SHAPE_BOX:
    case PHYS_SHAPE_CAPSULE:
    case PHYS_SHAPE_CONVEX_HULL:
    case PHYS_SHAPE_CONVEX_OFFSET:
    case PHYS_SHAPE_CONVEX_XFORM:
    case PHYS_SHAPE_USER0:
        convex = true;
        break;
    }
    return convex;
}
}


typedef char phys_shape_buffer[512];

namespace {
static __forceinline bool havok_collision_filter_is_environment_rigid_body(const phys_rigid_body *body)
{
    phys_rigid_body *const *environment_bodies = g_havok_monitor_globals.rigid_bodies;
    bool result = false;
    for (long i = 0; i < 16; i++) {
        if (environment_bodies[i] == body) {
            result = true;
            break;
        }
    }
    return result;
}
}



__forceinline bool c_group_filter::is_environment_mopp_versus_mopp_collision(
    const phys_collision_proxy &body_a,
    const phys_shape_container &container_b,
    unsigned int key_b) const
{
    bool result = false;
    if (body_a.get_shape()->get_type_id() == PHYS_SHAPE_BVH_TREE)
    {
        const phys_rigid_body *rigid_body_a = phys_get_rigid_body(body_a.get_root_collider());
        if (rigid_body_a != 0 && ::havok_entity_get_havok_component_index(rigid_body_a) == -1)
        {
            __declspec(align(16)) phys_shape_buffer buffer;
            const phys_shape *shape_b = container_b.get_child_shape(key_b, buffer);
            result = shape_b != 0 && shape_b->get_type_id() == PHYS_SHAPE_BVH_TREE;
        }
    }
    return result;
}

__forceinline bool c_group_filter::is_disable_gravity_lift_rigid_body_collision(
    unsigned long filter_info_a,
    unsigned long filter_info_b,
    const phys_collision_proxy &body_a,
    const phys_shape_container &container_b,
    unsigned int key_b) const
{
    long group_a = filter_info_a & 0x1f;
    long group_b = filter_info_b & 0x1f;
    bool result = false;
    if (group_b == 8 && key_b != (unsigned int)-1)
    {
        __declspec(align(16)) phys_shape_buffer buffer;
        const phys_shape *shape_b = container_b.get_child_shape(key_b, buffer);
        if (shape_b->get_type_id() != PHYS_SHAPE_BOUNDING)
        {
            unsigned long mask = 0x10622;
            result = ((1 << group_a) & mask) == 0;
        }
    }
    return result;
}

__forceinline bool c_group_filter::is_dead_environment_shape_key(
    long group_a,
    const phys_shape &shape_b,
    unsigned int key_b) const
{
    bool result = false;
    unsigned long user_data = shape_b.get_user_data();
    const c_environment_surface_shape *surface_shape = 0;
    switch (user_data)
    {
    case k_environment_surface_shape_user_data:
        surface_shape = (const c_environment_surface_shape *)&shape_b;
        break;
    case k_environement_body_user_data:
        surface_shape = ((const c_environment_mopp_bv_tree_shape *)&shape_b)->get_environment_surface_shape();
        break;
    }
    if (surface_shape != 0)
    {
        long surface_index;
        long sink_class;
        long type;
        values_from_shape_key(&surface_index, &sink_class, &type, key_b);
        if (type == 1 && sink_class != 5 && havok_group_soft_surface_sink_class_get((e_havok_group)group_a) != sink_class)
        {
            result = true;
        }
        else
        {
            result = !surface_shape->is_key_alive(key_b);
        }
    }
    return result;
}

__forceinline bool c_group_filter::is_primitive_collision(
    const phys_collision_proxy &body_a,
    const phys_collision_proxy &body_b,
    const phys_shape_container &container_b,
    unsigned int key_b,
    bool *is_collision_enabled) const
{
    __declspec(align(16)) phys_shape_buffer buffer;
    const phys_shape *shape_b;
    if (key_b != (unsigned int)-1)
    {
        shape_b = container_b.get_child_shape(key_b, buffer);
    }
    else
    {
        shape_b = body_b.get_shape();
    }
    return havok_collision_filter_try_and_get_primitive_collision(
        this,
        &body_a,
        &body_b,
        shape_b,
        is_collision_enabled) != 0;
}

// @address 0x105330 ?allows_collision@c_group_filter@@UBA?AVphys_bool@@ABUphys_collision_input@@ABVphys_collision_proxy@@1ABVphys_shape_container@@I@Z
phys_bool c_group_filter::allows_collision(
    const phys_collision_input &input,
    const phys_collision_proxy &body_a,
    const phys_collision_proxy &body_b,
    const phys_shape_container &container_b,
    unsigned int key_b) const
{
    const phys_collider *collidable_a = body_a.get_root_collider();
    const phys_collider *collidable_b = body_b.get_root_collider();
    bool is_collision_enabled = false;
    if (is_environment_mopp_versus_mopp_collision(body_a, container_b, key_b))
    {
        is_collision_enabled = false;
    }
    else if (is_disable_gravity_lift_rigid_body_collision(
                 collidable_a->get_collision_filter_info(),
                 collidable_b->get_collision_filter_info(),
                 body_a,
                 container_b,
                 key_b))
    {
        is_collision_enabled = false;
    }
    else if (is_dead_environment_shape_key(
                 collidable_a->get_collision_filter_info() & 0x1f,
                 *body_b.get_shape(),
                 key_b))
    {
        is_collision_enabled = false;
    }
    else if (!group_filter_is_phantom_shape_versus_phantom_collision(this, collidable_a, collidable_b, &is_collision_enabled)
          && !group_filter_is_phantom_shape_versus_environment_collision(
                  &body_a,
                  &body_b,
                  &container_b,
                  key_b)
          && !is_primitive_collision(body_a, body_b, container_b, key_b, &is_collision_enabled))
    {
        if (havok_collision_filter_test_phantom_collision(collidable_a, collidable_b, &body_a, &body_b, &container_b, key_b))
        {
            is_collision_enabled = false;
        }
        else
        {
            is_collision_enabled = phys_group_filter::allows_collision(
                collidable_a->get_collision_filter_info(),
                collidable_b->get_collision_filter_info());
        }
    }
    return phys_bool(is_collision_enabled);
}

extern "C" bool group_filter_is_phantom_shape_versus_environment_collision(const phys_collision_proxy *a, const phys_collision_proxy *b,
    const phys_shape_container *container, unsigned long key)
{
    const phys_collider *root_a = a->get_root_collider();
    const phys_collider *root_b = b->get_root_collider();
    bool result = false;
    if (havok_collision_filter_is_environment_rigid_body(phys_get_rigid_body(root_b)) && a->m_proxy_shape->get_type_id() == PHYS_SHAPE_BOUNDING) result = true;
    else if (havok_collision_filter_is_environment_rigid_body(phys_get_rigid_body(root_a))) {
        __declspec(align(16)) phys_shape_buffer buffer;
        const phys_shape *s = container->get_child_shape(key, buffer);
        result = s != 0 && s->get_type_id() == PHYS_SHAPE_BOUNDING;
    }
    return result;
}

extern "C" bool havok_collision_filter_test_phantom_collision(const phys_collider *a, const phys_collider *b,
    const phys_collision_proxy *body_a, const phys_collision_proxy *body_b,
    const phys_shape_container *container, unsigned long key)
{
    const phys_rigid_body *rigid_b = phys_get_rigid_body(b);
    bool disabled = false;
    if (rigid_b && !havok_collision_filter_is_environment_rigid_body(rigid_b)) {
        __declspec(align(16)) phys_shape_buffer buffer;
        const phys_shape *s = container->get_child_shape(key, buffer);
        if (s->get_type_id() == PHYS_SHAPE_BOUNDING) {
            long component_b = ::havok_entity_get_havok_component_index(rigid_b);
            if (component_b != -1) {
                const phys_rigid_body *rigid_a = phys_get_rigid_body(a);
                if (rigid_a->m_motion.m_type == 7 || rigid_a->m_motion.m_type == 6) {
                    disabled = true;
                } else {
                    long component_a = ::havok_entity_get_havok_component_index(rigid_a);
                    long rigid_index = ::havok_entity_get_havok_component_rigid_body_index(rigid_a);
                    if (component_a != -1 && component_a != component_b) {
                        long material = (long)((const phys_bounding_shape *)s)->get_child_shape()->get_user_data();
                        if (!entity_belongs_in_phantom(component_b, component_a, rigid_index, material)) {
                            disabled = true;
                        }
                    }
                }
            }
        }
    }
    return disabled;
}

// @address 0x105678 group_filter_is_phantom_shape_versus_phantom_collision
extern "C" bool group_filter_is_phantom_shape_versus_phantom_collision(const c_group_filter *filter, const phys_collider *a,
    const phys_collider *b, bool *enabled)
{
    if (b->get_type_id() != 2 && a->get_type_id() != 2) {
        return false;
    }
    return group_filter_is_phantom_shape_a_versus_phantom_b_collision(a, b, enabled)
        || group_filter_is_phantom_shape_a_versus_phantom_b_collision(b, a, enabled);
}


e_soft_surface_sink_class havok_group_soft_surface_sink_class_get(e_havok_group type)
{
  switch (type) {
  case _havok_group_biped:
    return (e_soft_surface_sink_class)1;
  case 3:
  case 5:
  case _havok_group_vehicle_physics:
  case 16:
  case 20:
    return (e_soft_surface_sink_class)3;
  case _havok_group_dead_biped:
  case _havok_group_recently_dead_biped:
  case _havok_group_ragdoll_biped:
    return (e_soft_surface_sink_class)2;
  case 4:
  case _havok_group_vehicle:
  case 7:
  case 8:
  case _havok_group_giant:
    return (e_soft_surface_sink_class)4;
  default:
    return _soft_surface_sink_class_default;
  }
}

phys_bool c_group_filter::allows_collision(const phys_ray_input &input,
    const phys_shape &shape,
    const phys_shape_container &container,
    unsigned int key) const
{
    int shape_type = shape.m_type;
    unsigned int mismatch = 0;
    if (shape_type == PHYS_SHAPE_USER0 && (static_cast<const c_collision_surface_shape &>(shape).m_flags & 1) != 0) {
        int key_group = (key >> 16) & 0x1fff;
        if ((key & 0xe0000000u) == 0x20000000u && key_group != 5) {
            e_soft_surface_sink_class shape_group = havok_group_soft_surface_sink_class_get((e_havok_group)(input.m_filter_info & 0x1f));
            if (shape_group != key_group) {
                mismatch = 1;
            }
        }
    }

    bool enabled;
    if ((mismatch & 0xff) != 0) {
        enabled = false;
    } else if (shape_type == PHYS_SHAPE_USER0 && (static_cast<const c_collision_surface_shape &>(shape).m_flags & 2) != 0) {
        enabled = false;
    } else {
        enabled = phys_group_filter::allows_collision(input, shape, container, key);
    }
    return phys_bool(enabled);
}

phys_bool c_group_filter::allows_collision(const phys_world_ray_input &input,
    const phys_collider &collidable) const
{
    return phys_group_filter::allows_collision(input, collidable);
}

extern "C" inline const s_physics_model_primitive *havok_collision_filter_get_primitive(const phys_shape *shape)
{
    const s_physics_model_primitive *primitive = 0;
    if (shape->get_type_id() == PHYS_SHAPE_CONVEX_OFFSET || shape->get_type_id() == PHYS_SHAPE_CONVEX_XFORM) {
        shape = ((const phys_convex_xform_shape *)shape)->get_child_shape();
    } else if (shape->get_type_id() == PHYS_SHAPE_TRANSFORM) {
        shape = ((const phys_transform_shape *)shape)->get_child_shape();
    }
    if (shape->get_type_id() != PHYS_SHAPE_USER0 && havok_collision_filter_is_convex_shape(shape)) {
        uintptr_t user_data = shape->get_user_data();
        if (user_data != 0 && (user_data < 1 || user_data > 16)) {
            primitive = (const s_physics_model_primitive *)user_data;
        }
    }
    return primitive;
}


inline bool group_filter_is_phantom_shape_a_versus_phantom_b_collision(const phys_collider *a,const phys_collider *b,bool *enabled)
{
 bool result=false;
 if(b->get_type_id()==2) {
  long b_index=::havok_entity_get_havok_component_index(phys_get_world_object(b));
  if(b_index!=-1) {
   int type=((s_object_header *)tls_object_header_data()->data)[datum_index_to_absolute_index(
       ((c_havok_component *)g_havok_component_data->data
           + datum_index_to_absolute_index(b_index))->get_object_index())].object_type;
   if(((1<<type)&7391)==0) {
    long a_index=::havok_entity_get_havok_component_index(phys_get_world_object(a));
    if(a_index!=-1) {
     c_havok_component *havok_component=(c_havok_component *)g_havok_component_data->data + (datum_index_to_absolute_index(a_index));
     *enabled=(havok_component->m_flags & 512) != 0;
     result=true;
    }
   }
  }
 }
 return result;
}


// @address 0x105b40 havok_collision_filter_try_and_get_primitive_collision
extern "C" unsigned char havok_collision_filter_try_and_get_primitive_collision(const phys_group_filter *filter,
    const phys_collision_proxy *a, const phys_collision_proxy *b, const phys_shape *shape_b, bool *collision)
{
    const s_physics_model_primitive *primitive_a = a->m_proxy_shape ? havok_collision_filter_get_primitive(a->m_proxy_shape) : 0;
    const s_physics_model_primitive *primitive_b = shape_b ? havok_collision_filter_get_primitive(shape_b) : 0;
    const signed char *group_a = &primitive_a->proxy_collision_group;
    const signed char *group_b = &primitive_b->proxy_collision_group;
    unsigned char has_a = primitive_a != 0 && (unsigned char)*group_a != 255;
    unsigned char has_b = primitive_b != 0 && (unsigned char)*group_b != 255;
    bool result = false;
    if (has_a || has_b) {
    const phys_collider *root_a = a->get_root_collider();
    int filter_a = root_a->get_collision_filter_info();
    const phys_collider *root_b = b->get_root_collider();
    int filter_b = root_b->get_collision_filter_info();
    if (has_a) {
        filter_a = (filter_a & 0xffff0000) | *group_a;
    }
    if (has_b) {
        filter_b = (filter_b & 0xffff0000) | *group_b;
    }
    *collision = filter->allows_collision((unsigned int)filter_b, (unsigned int)filter_a);
        result = true;
    }
    return result;
}

extern "C" bool group_filter_is_no_collision_with_environment_rigid_body_vs_environment(const phys_collider *a,const phys_collider *b) {
 const phys_rigid_body *body_a=phys_get_rigid_body(a);
 const phys_rigid_body *body_b=phys_get_rigid_body(b);
 bool result=false;
 if(body_a && body_b && (body_b->m_motion.m_type==7 || body_b->m_motion.m_type==6)) {
  long component_index=::havok_entity_get_havok_component_index(body_a);
  if(component_index!=-1) {
   long body_index=::havok_entity_get_havok_component_rigid_body_index(body_a);
   c_havok_component *component =
       (c_havok_component *)g_havok_component_data->data + (datum_index_to_absolute_index(component_index));
   result=component->get_rigid_body_does_not_collide_with_environment(body_index);
  }
 }
 return result;
}


phys_bool c_group_filter::allows_collision(const phys_collider &a, const phys_collider &b) const
{
    bool collision = false;
    bool both_lists = a.get_shape() != 0 &&
        a.m_proxy_shape->get_type_id() == PHYS_SHAPE_BVH_TREE &&
        b.get_shape() != 0 &&
        b.m_proxy_shape->get_type_id() == PHYS_SHAPE_BVH_TREE;
    if (!both_lists) {
        if (!group_filter_is_phantom_shape_versus_phantom_collision(this, &a, &b, &collision)
            && !group_filter_is_no_collision_with_environment_rigid_body_vs_environment(&a, &b)) {
            const phys_shape *shape_b = b.m_proxy_shape;
            if (!havok_collision_filter_try_and_get_primitive_collision(this, &a, &b, shape_b, &collision))
            {
                collision = phys_group_filter::allows_collision(a, b);
            }
        }
    }
    return phys_bool(collision);
}


phys_group_filter *havok_initialize_collision_filter()
{
    phys_group_filter *filter = new c_group_filter();
    filter->enable_collisions_by_mask(~0u, ~0u);
    filter->disable_layer_pair(4, 4);
    filter->disable_layer_pair(4, 21);
    filter->disable_layer_pair(4, 11);
    filter->disable_layer_pair(4, 15);
    filter->disable_layer_pair(4, 20);
    filter->disable_layer_pair(4, 6);
    filter->disable_layer_pair(21, 21);
    filter->disable_layer_pair(21, 11);
    filter->disable_layer_pair(21, 6);
    filter->disable_layer_bits(1u << 12, ~1u);
    filter->enable_collisions_between(12, 1);
    filter->enable_collisions_between(12, 5);
    filter->enable_collisions_between(12, 10);
    filter->enable_collisions_between(12, 9);
    filter->enable_collisions_between(12, 16);
    filter->disable_layer_bits(1u << 14, ~1u);
    filter->enable_collisions_between(14, 1);
    filter->enable_collisions_between(14, 10);
    filter->enable_collisions_between(14, 16);
    filter->disable_layer_bits(1u << 13, ~1u);
    filter->enable_collisions_between(13, 1);
    filter->enable_collisions_between(13, 9);
    filter->enable_collisions_between(13, 10);
    filter->enable_collisions_between(13, 16);
    filter->disable_layer_pair(9, 9);
    filter->disable_layer_pair(9, 10);
    filter->disable_layer_pair(9, 1);
    filter->disable_layer_pair(10, 10);
    filter->disable_layer_pair(10, 1);
    filter->disable_layer_bits(1u << 2, ~1u);
    filter->enable_collisions_between(2, 1);
    filter->disable_layer_pair(20, 11);
    filter->disable_layer_pair(20, 21);
    filter->disable_layer_pair(20, 6);
    filter->disable_layer_pair(20, 7);
    filter->disable_layer_pair(20, 4);
    filter->disable_layer_pair(20, 8);
    filter->disable_layer_pair(6, 6);
    filter->disable_layer_pair(6, 11);
    filter->disable_layer_pair(6, 15);
    filter->disable_layer_bits(1u << 22, ~1u);
    filter->enable_collisions_between(22, 3);
    filter->enable_collisions_between(22, 5);
    filter->enable_collisions_between(22, 9);
    filter->enable_collisions_between(22, 10);
    filter->enable_collisions_between(22, 6);
    filter->enable_collisions_between(22, 7);
    filter->enable_collisions_between(22, 8);
    filter->enable_collisions_between(22, 15);
    filter->enable_collisions_between(22, 16);
    filter->enable_collisions_between(8, 5);
    filter->enable_collisions_between(8, 16);
    filter->enable_collisions_between(8, 10);
    filter->enable_collisions_between(8, 9);
    filter->disable_layer_bits(1u << 19, ~1u);
    filter->disable_layer_bits(1u << 17, ~1u);
    filter->enable_collisions_between(17, 19);
    filter->disable_layer_bits(1u << 18, ~1u);
    filter->disable_layer_bits(1u << 23, ~1u);
    filter->enable_collisions_between(23, 1);
    filter->enable_collisions_between(23, 23);
    filter->enable_collisions_between(23, 16);
    return filter;
}
