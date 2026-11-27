// objectslibxbox/object_early_movers.obj: from the records 0x9ff18 0xda3d8 0xf3788 0x10df38 0x10ede8 0x1125f8 0x32ebc0 0x32ec30 0x32ec78 0x32ec90 0x32ee88 0x32efc8 0x32f228 0x32f3c0 0x32f468 0x32f590 0x32f940 0x32fa38 0x32fbb8 0x32fd70 0x32fe10
// @flags /TP /O2
#include "memory/tls_globals.h"
#include "objects/object_datum.h"
#include "items/projectile_types.h"
#include "math/real_math.h"
#include "math/matrix_math.h"
#include "physics/havok_component.h"
#include "math/real_math_types.h"
#include "math/matrix_math_types.h"
#include "units/motor_enums.h"
#include "units/motor_request.h"
#include <string.h>
#include "cseries/cseries.h"
#include "memory/data_types.h"
#include "memory/data.h"
#include "tag_files/tag_groups.h"
#include "phys/phys_base.h"
#include "objects/objects.h"
#include "simulation/game_interface/simulation_game_action.h"
#include "objects/object_early_movers.h"
#include "objects/object_scripting.h"
#include "physics/bsp3d.h"
#include "physics/collision_models.h"
#include "cache/restricted_memory_regions.h"
#include "motor/motor_system.h"
#include "physics/collision_model_bsp_iterator_types.h"
#include "objects/object_early_movers_tls.h"
#include "objects/object_definitions.h"
#include "models/model_types.h"
#include "models/model_variant_types.h"
#include "objects/object_header.h"
#include "physics/collision_model_types.h"
#include "physics/havok_component_class.h"
#include "physics/havok_contact_point_iterator.h"
#include "physics/physics_model_types.h"
#include "physics/physics_model_definition_types.h"
#include "cache/restricted_memory_layout.h"
#include "cache/restricted_memory_regions_inline.h"
#include "cache/restricted_memory_registration.h"
#include "multithreading/thread_tls.h"
#include "phys/phys_group_filter.h"
#include "physics/havok.h"
#include "units/biped_types.h"
#include "physics/collision_model_definition_types.h"

struct collision_model_instance;
struct s_render_list;
struct s_marker_entity;
struct s_model;
struct s_model_definition;
struct collision_model_definition;
struct collision_model_bsp_iterator;
class c_restricted_memory;
struct bsp3d;
typedef char early_mover_layout_1[(sizeof(s_mover) == 0x130) ? 1 : -1];
typedef char early_mover_layout_2[(offsetof(s_mover, previous_transform) == 0x6c) ? 1 : -1];
typedef char early_mover_layout_3[(offsetof(s_mover, bsp_count) == 0x10c) ? 1 : -1];
typedef s_physics_model_rigid_body s_early_mover_physics_body;
typedef s_physics_model_definition s_early_mover_physics_model;
typedef biped_datum s_early_mover_biped;
typedef s_collision_model_bsp early_mover_bsp_block;
typedef s_collision_model_permutation early_mover_permutation;
typedef s_collision_model_region early_mover_region;
typedef collision_model_definition early_mover_collision_definition;
typedef t_restricted_allocation_manager<3, static_cast<void (*)(void *)>(0), static_cast<void (*)(void *)>(0),
    &__tls_set_g_object_early_movers_globals_allocator> early_movers_allocation_manager;
struct s_object_early_mover_obb { real_rectangle3d bounds; real_matrix4x3 transform; };
struct s_early_mover_obb_entry {
    long node_name;
    real_rectangle3d bounds;
    real_euler_angles3d angles;
};
class c_early_mover_filtered_iterator {
public:
    object_header_datum *current;
    long flags_offset;
    unsigned char mask, match;
    s_data_iterator iterator;
    __forceinline void begin(s_data_array *data) {
        iterator.data = data;
        iterator.absolute_index = -1;
        iterator.index = -1;
        flags_offset = 2;
        mask = 17;
        match = 1;
    }
    __forceinline object_header_datum *next() {
        current = (object_header_datum *)data_iterator_next_with_flags(&iterator, flags_offset, mask, match);
        return current;
    }
};


// @address 0x11df708 g_object_early_movers_globals
extern "C" {
__declspec(thread) s_owner_table *g_object_early_movers_globals;
}

static __forceinline s_owner_table *object_early_movers_globals()
{
    return (s_owner_table *)g_object_early_movers_globals;
}


// the slot of the owner of the object's outermost parent, when that object has flag 1 or 2 set
long object_early_mover_local_space_index(long object_index)
{
    int root_index = -1;
    while (object_index != -1) {
        root_index = object_index;
        object_index = object_header_get(object_index)->datum->parent_object_index;
    }
    object_datum *root = object_header_get(root_index)->datum;
    int result = -1;
    bool first = TEST_BIT(root->physics_flags, 1);
    bool second = TEST_BIT(root->physics_flags, 2);
    if (first || second) {
        int owner = root->local_physics_object_index;
        s_owner_table *table = object_early_movers_globals();
        for (int i = 0; i < table->owner_count; i++) {
            if (table->owners[i] == owner) {
                result = i;
                break;
            }
        }
    }
    return result;
}

static __forceinline bool flag32_test(const unsigned long *flags,long bit) { unsigned char result=(*flags>>bit)&1; return result; }
static __forceinline bool flag16_test(const unsigned short *flags,long bit) { unsigned char result=(*flags>>bit)&1; return result; }
static __forceinline real_vector3d *object_early_mover_add_vectors3d(const real_vector3d *a, const real_vector3d *b, real_vector3d *result)
{
    result->i = b->i + a->i;
    result->j = b->j + a->j;
    result->k = a->k + b->k;
    return result;
}
static __forceinline bool attachment_owner_flag(const s_owner_table *globals, long index)
{
    bool flag = (TAG_GET(0, s_object_definition, object_get(globals->owners[index])->definition_index)->flags >> 7) & 1;
    return flag;
}
static __forceinline void flag_set(unsigned long *flags, int bit, bool value) { if (value) *flags |= 1 << bit; else *flags &= ~(1 << bit); }
// @address 0x9ff18 ?object_get_early_mover_local_space_velocity@@YA_NJPBTreal_point3d@@PATreal_vector3d@@1_N2@Z
bool object_get_early_mover_local_space_velocity(long object_index, const real_point3d *point, real_vector3d *offset,
                  // match: 0x9ff18: pointer __restrict qualifier prevents alias reload
                  real_vector3d *__restrict axis_out, bool check_owner, bool primary)
{
  bool result = false;
  bool attached = (object_get(object_index)->physics_flags >> 3) & 1;
  if (attached) {
    long index = object_early_mover_local_space_index(object_index);
    if (index != -1) {
      s_owner_table *globals = object_early_movers_globals();
      s_mover *attachment = &globals->movers[index];
      if ((!check_owner || attachment_owner_flag(globals, index)) && attachment->valid1) {
        const real_vector3d *axis = primary ? &attachment->angular_velocity : &attachment->angular_velocity2;
        const real_vector3d *origin = primary ? &attachment->velocity : &attachment->previous_velocity;
        const real_point3d *center = primary ? &attachment->center : &attachment->older_center;
        real_vector3d delta;
        vector_from_points3d(center, point, &delta);
        real_vector3d cross;
        cross.k = axis->i * delta.j - axis->j * delta.i;
        cross.j = axis->k * delta.i - axis->i * delta.k;
        cross.i = axis->j * delta.k - axis->k * delta.j;
        offset->i = cross.i;
        offset->j = cross.j;
        offset->k = cross.k;
        offset->i = origin->i + offset->i;
        offset->j = origin->j + offset->j;
        offset->k = origin->k + offset->k;
        *axis_out = *axis;
        result = true;
      }
    }
  }
  return result;
}

// @address 0x32fd70 ?reserve_memory@?$t_restricted_allocation_manager@$02$0A@$0A@$1?__tls_set_g_object_early_movers_globals_allocator@@YAXPAX@Z@@QAAPAXPBD0IJ@Z
template<>
void *t_restricted_allocation_manager<3, static_cast<void (*)(void *)>(0), static_cast<void (*)(void *)>(0),
    &__tls_set_g_object_early_movers_globals_allocator>::reserve_memory(
    const char *name, const char *type, unsigned int size, long alignment_bits)
{
  // match: 0x32fd70: value range assumption guides branch lowering
  __assume(alignment_bits == 0);
  int member_index = restricted_memory_register_member(&g_restricted_regions[3], name, type, size, alignment_bits,
      __tls_set_g_object_scripting_state_allocator, 0, 0);
  unsigned int offset = restricted_region_get_member_offset(3, member_index);
  void *address = restricted_region_get_base_address(3) + offset;
  int owner = get_current_thread_index();
  m_thread_id = owner;
  m_member_index = member_index;
  g_object_early_movers_globals = (s_owner_table *)address;
  return restricted_region_get_base_address(3) + restricted_region_get_member_offset(3, member_index);
}

template<>
void *t_restricted_allocation_manager<3, static_cast<void (*)(void *)>(0), static_cast<void (*)(void *)>(0),
    &__tls_set_g_object_early_movers_globals_allocator>::allocate(unsigned int size, const char *name)
{
    return reserve_memory(name, "", size, 0);
}

void object_early_movers_initialize(void)
{
    g_object_early_movers_globals = (s_owner_table *)g_object_early_movers_globals_allocator.reserve_memory("object early movers", 0, sizeof(s_owner_table), 0);
    memset(object_early_movers_globals(), 0, sizeof(s_owner_table));
}

// @address 0x32ec30 ?object_early_movers_initialize_for_new_map@@YAXXZ
void object_early_movers_initialize_for_new_map(void)
{
    s_owner_table *globals = object_early_movers_globals();
    memset(globals, 0, sizeof(s_owner_table));
    globals->map_initialized = true;
}

// @address 0x32ec78 ?object_early_movers_dispose_from_old_map@@YAXXZ
void object_early_movers_dispose_from_old_map(void)
{
    object_early_movers_globals()->map_initialized = false;
}


// the early mover's first-frame state: its component's velocities and the nodes of its collision bsps
// @address 0x32ee88 object_early_mover_update_state
extern "C" void object_early_mover_update_state(long object_index, long early_mover_index)
{
    s_mover *mover = &object_early_movers_globals()->movers[early_mover_index];
    object_datum *object = object_get(object_index);
    mover->bsp_count = 0;
    if (object->havok_component_index != -1) {
        c_havok_component *component = datum_get<c_havok_component>(g_havok_component_data, object->havok_component_index);
        long node_index, rigid_body_index;
        if (object_early_mover_get_node_and_rigid_body_index(object_index, &node_index, &rigid_body_index)) {
            component->get_rigid_body_linear_velocity( rigid_body_index, &mover->velocity);
            component->get_rigid_body_angular_velocity(rigid_body_index, &mover->angular_velocity);
            component
                ->get_rigid_body_center_of_mass(rigid_body_index, &mover->center);
            mover->valid2 = true;
            union { collision_model_instance instance; collision_model_bsp_iterator iterator; } scratch;
            if (::collision_model_instance_new(&scratch.instance, object_index)) {
                collision_model_bsp_iterator_new(
                    &scratch.iterator, scratch.instance.model, scratch.instance.collision_model, scratch.instance.region_permutations);
                while (collision_model_bsp_iterator_next(&scratch.iterator)) {
                    if ((unsigned char)scratch.iterator.index == node_index) {
                        if (mover->bsp_count < 8) {
                            long slot = mover->bsp_count++;
                            mover->bsps[slot] = scratch.iterator.index;
                        }
                    }
                }
            }
        }
    }
}

static __forceinline float object_early_movers_3_dot_product3d(const real_vector3d *a, const real_vector3d *b) { return a->i*b->i+(a->k*b->k+a->j*b->j); }
static __forceinline real_vector3d *object_early_movers_3_scale_vector3d(const real_vector3d *a, float scale, real_vector3d *v)
{
    v->i = scale * a->i;
    v->j = a->j * scale;
    v->k = a->k * scale;
    return v;
}
static __forceinline real_vector3d *object_early_movers_3_cross_product3d(const real_vector3d *a, const real_vector3d *b, real_vector3d *v)
{
    float i = a->j * b->k - a->k * b->j;
    float k = a->i * b->j - a->j * b->i;
    float j = a->k * b->i - a->i * b->k;
    v->i = i;
    v->j = j;
    v->k = k;
    return v;
}
// @address 0x32efc8 object_early_mover_move
extern "C" void object_early_mover_move(long object_index, long early_mover_index)
{
    object_datum *object = object_get(object_index);
    bool moved = false;
    s_mover *m = (s_mover *)&g_object_early_movers_globals->movers[early_mover_index];
    if (object->havok_component_index != -1) {
        c_havok_component *component = datum_get<c_havok_component>(g_havok_component_data, object->havok_component_index);
        long node, body;
        if (object_early_mover_get_node_and_rigid_body_index(object_index, &node, &body)) {
            if (m->valid1) {
                m->previous_transform = m->transform;
                m->older_velocity = m->previous_velocity;
                m->previous_angular_velocity = m->angular_velocity2;
                m->previous_center = m->older_center;
                m->valid0 = true;
            }
            component->get_rigid_body_transform(body, &m->transform);
            component->get_rigid_body_linear_velocity( body, &m->previous_velocity);
            component->get_rigid_body_angular_velocity(body, &m->angular_velocity2);
            component->get_rigid_body_center_of_mass(body, &m->older_center);
            m->valid1 = true;
            moved = true;
        }
    }
    if (!moved) {
        m->valid1 = false;
        m->valid0 = false;
    } else if (m->valid1 && m->valid0) {
        real_matrix4x3 inverse;
        matrix4x3_inverse(&m->previous_transform, &inverse);
        matrix4x3_compute_product(&m->transform, &inverse, &m->delta);
        normalize3d(&m->delta.rotation.forward);
        normalize3d(&m->delta.rotation.left);
        normalize3d(&m->delta.rotation.up);
        float dot = object_early_movers_3_dot_product3d(&m->delta.rotation.up, &m->delta.rotation.forward);
        real_vector3d scaled;
        object_early_movers_3_scale_vector3d(&m->delta.rotation.forward, dot, &scaled);
        subtract_vectors3d(&m->delta.rotation.up, &scaled, &m->delta.rotation.up);
        normalize3d(&m->delta.rotation.up);
        object_early_movers_3_cross_product3d(&m->delta.rotation.up, &m->delta.rotation.forward, &m->delta.rotation.left);
        normalize3d(&m->delta.rotation.left);
    }
}
// @address 0x32f228 object_get_early_mover_local_space_acceleration
extern "C" bool object_get_early_mover_local_space_acceleration(long object_index, const real_point3d *point, real_vector3d *acceleration,
    // match: 0x32f228: pointer __restrict qualifier prevents alias reload
    real_vector3d *__restrict angular, bool check_owner, bool primary)
{
  
 object_datum *o=object_get(object_index);
 bool result=false;
 if(flag32_test(&o->physics_flags,3)) {
  long index=object_early_mover_local_space_index(object_index);
  if(index!=-1) {
   long owner_index=object_early_movers_globals()->owners[index];
   s_mover *m=&object_early_movers_globals()->movers[index];
   object_datum *owner=object_get(owner_index);
   s_object_definition *d=TAG_GET(0, s_object_definition, owner->definition_index);
   if(flag16_test(&d->flags,7) && m->valid1 && m->valid2 && m->valid0) {
    real_vector3d angular_delta,velocity_delta,offset;
    if(true) {
     subtract_vectors3d(&m->angular_velocity,&m->angular_velocity2,&angular_delta);
     subtract_vectors3d(&m->velocity,&m->previous_velocity,&velocity_delta);
     vector_from_points3d(&m->center,point,&offset);
    } else {
     subtract_vectors3d(&m->angular_velocity2,&m->previous_angular_velocity,&angular_delta);
     subtract_vectors3d(&m->previous_velocity,&m->older_velocity,&velocity_delta);
     vector_from_points3d(&m->older_center,point,&offset);
    }
    cross_product3d(&angular_delta,&offset,acceleration);
    add_vectors3d(acceleration,&velocity_delta,acceleration);
    *angular=angular_delta;
    result=true;
   }
  }
 }
 return result;
}

// whether the object owns an early mover slot (and, when asked, its definition has flag 7)
// @address 0x32f3c0 object_early_mover_is_owner
extern "C" bool object_early_mover_is_owner(long object_index, bool check_definition)
{
    bool result = false;
    if (object_index != -1) {
        s_owner_table *table = object_early_movers_globals();
        for (long i = 0; i < table->owner_count; i++) {
            if (table->owners[i] == object_index) {
                if (check_definition) {
                    bool flagged = flag16_test(&TAG_GET(0, s_object_definition, object_get(object_index)->definition_index)->flags, 7);
                    if (!flagged)
                        break;
                }
                result = true;
                break;
            }
        }
    }
    return result;
}

// @address 0x32f468 ?object_in_early_mover_join@@YAXJJ@Z
void object_in_early_mover_join(long source_index, long object_index)
{
    object_datum *object = object_get(object_index);
    int attachment = object_early_mover_local_space_index(source_index);
    object->local_physics_object_index = -1;
    flag_set(&object->physics_flags, 1, false);
    flag_set(&object->physics_flags, 2, false);
    flag_set(&object->physics_flags, 3, false);
    if (attachment != -1 && object_early_movers_globals()->owners[attachment] != -1) {
        object_datum *source = object_get(source_index);
        object->local_physics_object_index = source->local_physics_object_index;
        flag_set(&object->physics_flags, 1, ((source->physics_flags >> 1) & 1) != 0);
        flag_set(&object->physics_flags, 2, ((source->physics_flags >> 2) & 1) != 0);
        flag_set(&object->physics_flags, 3, ((source->physics_flags >> 3) & 1) != 0);
    }
}


// @address 0x32f940 ?object_in_early_mover_detach@@YAXJ@Z
void object_in_early_mover_detach(long object_index)
{
    object_datum *object = object_get(object_index);
    s_owner_table *globals = object_early_movers_globals();
    long attachment = -1;
    for (long i = 0; i < globals->owner_count; i++) {
        if (globals->owners[i] == object->parent_object_index) {
            attachment = globals->owners[i];
            break;
        }
    }
    if (attachment != -1) {
        object_detach(object_index);
        object->local_physics_object_index = attachment;
        flag_set(&object->physics_flags, 1, true);
        flag_set(&object->physics_flags, 2, true);
        flag_set(&object->physics_flags, 3, true);
        real_point3d center;
        real_vector3d angular_velocity;
        real_vector3d linear_velocity;
        object_get_center_of_mass(object_index, &center);
        object_get_early_mover_local_space_velocity(object_index, &center, &linear_velocity, &angular_velocity, true, true);
        object_set_velocities(object_index, &linear_velocity, &angular_velocity);
    }
}

// 0xda3d8: an early mover pulls its rigid bodies toward the parent's frame and tests its contacts
// @address 0xda3d8 ?object_in_early_mover_physics_update@@YAXJ@Z
void object_in_early_mover_physics_update(long object_index)
{
    object_datum *object = object_get(object_index);
    if (flag32_test(&object->physics_flags, 7) && flag32_test(&object->physics_flags, 2)
            && object->parent_object_index == -1
            && object_early_mover_local_space_index(object_index) != -1
            && object->havok_component_index != -1) {
        c_havok_component *component = datum_get<c_havok_component>(g_havok_component_data, object->havok_component_index);
        real_point3d center;
        real_vector3d acceleration, angular_acceleration;
        real_vector3d linear_velocity, angular_velocity;
        for (long i = 0; i < component->get_rigid_body_count(); i++) {
            component->get_rigid_body_center_of_mass(i, &center);
            if (object_get_early_mover_local_space_acceleration(
                    object_index, &center, &acceleration, &angular_acceleration, true, true))
            {
                component->get_rigid_body_linear_velocity( i, &linear_velocity);
                component->get_rigid_body_angular_velocity(i, &angular_velocity);
                object_early_mover_add_vectors3d(&acceleration, &linear_velocity, &linear_velocity);
                object_early_mover_add_vectors3d(&angular_acceleration, &angular_velocity, &angular_velocity);
                component->set_rigid_body_linear_velocity(i, &linear_velocity);
                component->set_rigid_body_angular_velocity(i, &angular_velocity);
            }
        }
        long contact_count = 0;
        long matching_count = 0;
        for (c_havok_contact_point_iterator iterator(object_index, false); iterator.valid(); iterator.next()) {
            c_havok_contact_point *contact_point = iterator.get_contact_point();
            long contact_object_index = contact_point->get_contact_point_object_index();
            if (contact_object_index == object->local_physics_object_index) {
                matching_count++;
            } else if (contact_object_index != -1) {
                object_datum *contact_object = object_get(contact_object_index);
                if (object->local_physics_object_index != contact_object->local_physics_object_index)
                    matching_count++;
            }
            contact_count++;
        }
        if (contact_count != 0 && matching_count == contact_count)
            flag_set(&object->physics_flags, 3, true);
    }
}


static __forceinline real_matrix4x3 *early_mover_node_matrix(long object_index, short node_index)
{
    object_datum *object = object_get(object_index);
    real_matrix4x3 *matrices = (real_matrix4x3 *)((char *)object + object->node_matrices.offset);
    return &matrices[node_index];
}
static __forceinline void early_mover_rotation_from_quaternion(real_matrix4x3 *matrix, const real_quaternion *quaternion)
{
    matrix3x3_rotation_from_quaternion(&matrix->rotation, quaternion);
    matrix->position.x = 0.0f;
    matrix->position.y = 0.0f;
    matrix->position.z = 0.0f;
    matrix->scale = 1.0f;
}

bool object_early_mover_get_obb(long early_mover_array_index, s_object_early_mover_obb *obb)
{
    long object_index = object_early_movers_globals()->owners[early_mover_array_index];
    object_datum *object = object_get(object_index);
    s_object_definition *definition = (s_object_definition *)TAG_GET(0, s_object_definition, object->definition_index);
    bool result = false;
    if (definition->early_mover_obb.count != 0) {
        s_model_definition *model = (s_model_definition *)tag_get(0, definition->model.index);
        s_early_mover_obb_entry *entry = (s_early_mover_obb_entry *)definition->early_mover_obb.address;
        real_matrix4x3 transform;
        object_get_world_matrix(object_index, &transform);
        for (long node_index = 0; node_index < model->nodes.count; node_index++) {
            s_model_node *node = &((s_model_node *)model->nodes.address)[node_index];
            if (node->name == entry->node_name) {
                transform = *early_mover_node_matrix(object_index, (short)node_index);
                break;
            }
        }
        real_quaternion rotation;
        quaternion_from_euler_angles(&rotation, &entry->angles);
        real_matrix4x3 local_rotation;
        early_mover_rotation_from_quaternion(&local_rotation, &rotation);
        matrix4x3_compute_product(&transform, &local_rotation, &transform);
        obb->bounds = entry->bounds;
        obb->transform = transform;
        result = true;
    }
    return result;
}


// @address 0x32fbb8 object_early_mover_get_node_and_rigid_body_index
// match: 0x32fbb8: pointer __restrict qualifier prevents alias reload
extern "C" bool object_early_mover_get_node_and_rigid_body_index(long index, long *__restrict node, long *__restrict rigid) {
    object_datum *o = object_get(index);
    s_object_definition *d = (s_object_definition *)tag_get(0, o->definition_index);
    bool result = false;
    *node = -1; *rigid = -1;
    if (o->havok_component_index != -1) {
        c_havok_component *c = datum_get<c_havok_component>(g_havok_component_data, o->havok_component_index);
        if (d->model.index != -1) {
            s_model_definition *m = (s_model_definition *)tag_get(0, d->model.index);
            if (m->physics_model_index != -1) {
                s_early_mover_physics_model *p = (s_early_mover_physics_model *)tag_get(0, m->physics_model_index);
                for (long i = 0; i < c->get_rigid_body_count() && !result; i++) {
                    for (long j = 0; j < c->get_physics_model_rigid_body_index_count(i) && !result; j++) {
                        long physics_index = c->get_physics_model_rigid_body_index(i, j);
                        s_early_mover_physics_body *body = (s_early_mover_physics_body *)tag_block_get_element_with_size(&p->rigid_bodies,
                            physics_index, sizeof(s_physics_model_rigid_body));
                        if (body->flags & 16) { *rigid = i; *node = body->node; result = true; break; }
                    }
                }
                if (!result) {
                    long body_index = c->get_object_postion_controlling_rigid_body_index();
                    if (body_index != -1 && c->get_physics_model_rigid_body_index_count(body_index) > 0) {
                        long physics_index = c->get_physics_model_rigid_body_index(body_index, 0);
                        s_early_mover_physics_body *body = (s_early_mover_physics_body *)tag_block_get_element_with_size(&p->rigid_bodies,
                            physics_index, sizeof(s_physics_model_rigid_body));
                        *rigid = body_index; *node = body->node; result = true;
                    }
                }
            }
        }
    }
    return result;
}



// @address 0x32f590 ?object_in_early_mover_physics_move@@YAXJP6AXJ@Z@Z
void object_in_early_mover_physics_move(long object_index, void (*move)(long))
{
    object_datum *object = object_get(object_index);
    long local_space_index = object_early_mover_local_space_index(object_index);
    bool updated = false;
    if (flag32_test(&object->physics_flags, 7) && flag32_test(&object->physics_flags, 3)
            && object->parent_object_index == -1
            && object->havok_component_index != -1
            && local_space_index != -1) {
        long early_mover_index = object_early_movers_globals()->owners[local_space_index];
        if (early_mover_index != -1) {
            long high_frequency_count = 0;
            long low_frequency_count = 0;
            long contact_count = 0;
            phys_group_filter *filter = (phys_group_filter *)g_havok_world->get_collision_filter();
            c_havok_component *component = datum_get<c_havok_component>(g_havok_component_data, object->havok_component_index);
            bool collisions_disabled = true;
            for (long i = 0; i < component->get_rigid_body_count(); i++) {
                if (filter->allows_collision(component->get_rigid_body(i)->get_collider()->get_collision_filter_info(), 11))
                    collisions_disabled = false;
            }
            if (collisions_disabled) {
                c_havok_contact_point_iterator iterator(object_index, false);
                while (iterator.valid()) {
                    if (iterator.get_contact_point()->get_contact_point_object_index() == early_mover_index)
                        contact_count++;
                    iterator.next();
                }
                if (contact_count != 0) {
                    for (long i = 0; i < component->get_rigid_body_count(); i++) {
                        real_point3d center;
                        real_vector3d linear_velocity, angular_velocity;
                        component->get_rigid_body_center_of_mass(i, &center);
                        if (object_get_early_mover_local_space_velocity(object_index, &center, &linear_velocity, &angular_velocity, true, false)) {
                            long high_result, low_result;
                            component->deactivator_update_in_local_space(i, &linear_velocity, &angular_velocity, &high_result, &low_result);
                            if (high_result < 0)
                                object->high_frequency_rest_ticks = 0;
                            else if (high_result > 0)
                                high_frequency_count++;
                            if (low_result < 0)
                                object->high_frequency_rest_ticks = 0;
                            else if (low_result > 0)
                                low_frequency_count++;
                            updated = true;
                        }
                    }
                }
                if (updated && high_frequency_count == component->get_rigid_body_count())
                    object->high_frequency_rest_ticks = MIN(object->high_frequency_rest_ticks + 1, 5);
                if (updated && low_frequency_count == component->get_rigid_body_count())
                    object->low_frequency_rest_ticks = MIN(object->low_frequency_rest_ticks + 1, 5);
                if (object->high_frequency_rest_ticks == 5 || object->low_frequency_rest_ticks == 5) {
                    long parent_index = object_early_movers_globals()->owners[local_space_index];
                    long node_index, rigid_body_index;
                    if (parent_index != -1 && object_early_mover_get_node_and_rigid_body_index(parent_index, &node_index, &rigid_body_index)) {
                        if (move)
                            move(object_index);
                        object_attach_to_node_with_type(parent_index, object_index, (short)node_index);
                        object->physics_flags |= 1 << 4;
                        simulation_action_object_update(object_index, 0x800);
                    }
                }
            }
        }
    }
    if (!updated) {
        object->high_frequency_rest_ticks = 0;
        object->low_frequency_rest_ticks = 0;
    }
}

// 0x10ede8: an early mover's physics move waits on flag 20 of its object
// @address 0x10ede8 ?object_early_mover_physics_move_if_flagged@@YAXJ@Z
void object_early_mover_physics_move_if_flagged(long object_index)
{
    if (!flag32_test(&object_get(object_index)->physics_flags, 20))
        return;
    object_in_early_mover_physics_move(object_index, 0);
}


// @address 0xa0d80 ?objects_in_sphere@@YAFKKPBUs_location@@PBTreal_point3d@@MPAJF@Z
// @address 0xe2c20 ?object_wake@@YAXJ@Z
// @address 0xe3070 ?object_set_requires_motion@@YAXJ@Z
// @address 0x9e400 ?point_in_rectangle3d@@YA_NPBTreal_point3d@@PBTreal_rectangle3d@@@Z
// @address 0x9fb10 ?matrix4x3_inverse_transform_point@@YAPATreal_point3d@@PBUreal_matrix4x3@@PBT1@PAT1@@Z

static __forceinline bool notify_physics_flag(const object_datum *object, int bit)
{
    return (object->physics_flags >> bit) & 1;
}
static __forceinline void notify_flag_set(unsigned long *flags, int bit, bool value)
{
    if (value) *flags |= 1 << bit;
    else *flags &= ~(1 << bit);
}
static __forceinline void notify_flag_or(unsigned long *flags, int bit)
{
    *flags |= 1 << bit;
}

// @address 0x32ec90 ?object_early_mover_notify_local_objects@@YAXJ@Z
void object_early_mover_notify_local_objects(long early_mover_object_index)
{
    object_datum *early_mover = object_header_get(early_mover_object_index)->datum;
    bool obb_valid = false;
    long object_indices[1024];
    s_object_early_mover_obb obb;
    long object_count = objects_in_sphere(0, 5231, &early_mover->location, &early_mover->bounding_center,
                                          early_mover->bounding_radius, object_indices, 1024);
    s_owner_table *globals = object_early_movers_globals();
    for (long i = 0; i < globals->owner_count; i++) {
        if (globals->owners[i] == early_mover_object_index) {
            obb_valid = object_early_mover_get_obb(i, &obb);
            break;
        }
    }
    for (long i = 0; i < object_count; i++) {
        long object_index = object_indices[i];
        object_header_datum *header = object_header_get(object_index);
        if ((header->flags & 1) && !(header->flags & 16) && !(header->flags & 8)
            && !notify_physics_flag(header->datum, 2)) {
            object_datum *object = object_get(object_index);
            real_point3d center = object->bounding_center;
            real_point3d local_center;
            if (!obb_valid || (matrix4x3_inverse_transform_point(&obb.transform, &center, &local_center),
                              point_in_rectangle3d(&local_center, &obb.bounds))) {
                bool already_local = notify_physics_flag(object, 3) &&
                                     early_mover_object_index == object->local_physics_object_index;
                notify_flag_set(&object->physics_flags, 3, already_local);
                notify_flag_or(&object->physics_flags, 2);
                object->local_physics_object_index = early_mover_object_index;
                object_wake(object_index);
                if ((1 << header->object_type) & 32) {
                    object_set_requires_motion(object_index);
                    header->flags |= 8;
                }
            }
        }
    }
}


static __forceinline long object_early_mover_local_physics_object_get(long object_index)
{
    object_datum *object = object_get(object_index);
    long result = -1;
    if (flag32_test(&object->physics_flags, 1) || flag32_test(&object->physics_flags, 2))
        result = object->local_physics_object_index;
    return result;
}
static __forceinline void object_early_mover_biped_notify_deleted(long object_index)
{
    s_early_mover_biped *biped = (s_early_mover_biped *)object_get(object_index);
    if (biped->biped.physics.m_mode == 3) {
        c_motor_request request;
        request.setup_force_airborne(_action_none);
        motor_system_submit(object_index, &request);
    }
}
// match: 0x1125f8: pragma keeps the out-of-line memset library call; without it the EXTRN memset:PROC declaration disappears
#pragma function(memset)
// @address 0x1125f8 ?object_early_mover_delete@@YAXJ@Z
void object_early_mover_delete(long object_index)
{
    object_datum *object = object_get(object_index);
    s_object_definition *definition = TAG_GET(0, s_object_definition, object->definition_index);
    if (flag16_test(&definition->flags, 6)) {
        bool found = false;
        s_owner_table *globals = object_early_movers_globals();
        long object_count = globals->owner_count;
        for (long i = 0; i < object_count; i++) {
            if (globals->owners[i] == object_index) {
                flag_set(&object->physics_flags, 0, false);
                globals->owners[i] = globals->owners[globals->owner_count - 1];
                memset(&globals->movers[i], 0, sizeof(s_mover));
                globals->owner_count--;
                found = true;
                break;
            }
        }
        if (found) {
            c_early_mover_filtered_iterator iterator;
            iterator.begin(object_header_data);
            object_header_datum *header;
            while ((header = iterator.next()) != 0) {
                long local_object_index = iterator.iterator.index;
                object_datum *local_object = object_get(local_object_index);
                if (object_early_mover_local_physics_object_get(local_object_index) == object_index) {
                    do {
                        if (!((1 << header->object_type) & 12)) {
                            if (!((1 << header->object_type) & 1))
                                break;
                            object_early_mover_biped_notify_deleted(local_object_index);
                        }
                        ::object_set_at_rest(local_object_index, false);
                        object_wake(local_object_index);
                    } while (false);
                    flag_set(&local_object->physics_flags, 1, false);
                    flag_set(&local_object->physics_flags, 2, false);
                    flag_set(&local_object->physics_flags, 3, false);
                    local_object->local_physics_object_index = -1;
                }
            }
        }
    }
}
// @address 0x32ebb0 ?__tls_set_g_object_scripting_state_allocator@@YAXPAX@Z
// @address 0x9d0d0 ?matrix3x3_rotation_from_quaternion@@YAPAUreal_matrix3x3@@PAU1@PBUreal_quaternion@@@Z
// @address 0x9f8f0 ?collision_model_bsp_iterator_new@@YAXPAUcollision_model_bsp_iterator@@PBUs_model_definition@@PBUcollision_model_definition@@PBD@Z
// @address 0x9f9d0 ?collision_model_bsp_iterator_next@@YA_NPAUcollision_model_bsp_iterator@@@Z
// @address 0x33e620 ?motor_system_submit@@YA?AW4e_motor_result@@JPBVc_motor_request@@@Z
// @address 0xe2c20 ?object_wake@@YAXJ@Z
// @address 0xe7880 ?next@c_havok_contact_point_iterator@@QAAXXZ
// @address 0xe7990 ?valid@c_havok_contact_point_iterator@@QAA_NXZ
// @address 0x19e328 ?quaternion_from_euler_angles@@YAXPAUreal_quaternion@@PBTreal_euler_angles3d@@@Z
// @address 0x110a98 ?object_detach@@YAXJ@Z
// @address 0x9fcb8 ?object_get_center_of_mass@@YAPATreal_point3d@@JPAT1@@Z
// @address 0xe2b70 ?object_set_velocities@@YAXJPBTreal_vector3d@@0@Z
static __forceinline real_vector3d *object_early_movers_arm(const real_point3d *center, const real_point3d *position, real_vector3d *arm)
{
    arm->i=position->x-center->x; arm->j=position->y-center->y; arm->k=position->z-center->z; return arm;
}
static __forceinline real_vector3d *object_early_movers_cross(const real_vector3d *a, const real_vector3d *b, real_vector3d *v)
{
    float i=a->j*b->k-a->k*b->j;
    float j=a->k*b->i-a->i*b->k;
    float k=a->i*b->j-a->j*b->i;
    v->i=i; v->j=j; v->k=k; return v;
}
static __forceinline real_point3d *object_early_movers_point_from_line(
    const real_point3d *point, const real_vector3d *vector, float t, real_point3d *result)
{
    result->x = point->x + vector->i * t;
    result->y = point->y + vector->j * t;
    result->z = point->z + vector->k * t;
    return result;
}
static __forceinline real_vector3d *object_early_movers_add_velocity(const real_vector3d *a, const real_vector3d *b, real_vector3d *result)
{
    float i = a->i;
    result->i = i + b->i;
    result->j = a->j + b->j;
    result->k = a->k + b->k;
    return result;
}
// @address 0xf3788 ?object_move_in_early_mover_local_space@@YA_NJPATreal_vector3d@@0_N@Z
// match: 0xf3788: pointer __restrict qualifier prevents alias reload
bool object_move_in_early_mover_local_space(long index, real_vector3d *mover_velocity, real_vector3d *__restrict object_velocity, bool use_physics)
{
    projectile_datum *object = (projectile_datum *)object_get(index);
    bool moved_object = false;
    if (flag32_test(&object->object.physics_flags, 2)) {
        long owner_index = object->object.local_physics_object_index;
        for (long mover_index=0; mover_index<g_object_early_movers_globals->owner_count; ++mover_index) {
            if (g_object_early_movers_globals->owners[mover_index] == owner_index) {
                s_mover *mover = (s_mover *)&g_object_early_movers_globals->movers[mover_index];
                if (mover->valid0 && mover->valid1 && object->object.parent_object_index == -1) {
                    object_datum *owner = object_get(owner_index);
                    s_object_definition *definition = TAG_GET(0, s_object_definition, owner->definition_index);
                    if (owner->havok_component_index != -1) {
                        c_havok_component *component = datum_get<c_havok_component>(g_havok_component_data, owner->havok_component_index);
                        long rigid_body_index = component->get_object_postion_controlling_rigid_body_index();
                        if (rigid_body_index != -1) {
                            real_matrix4x3 world;
                            object_get_world_matrix(index, &world);
                            if (use_physics && flag16_test(&definition->flags, 7)) {
                                real_matrix4x3 moved;
                                matrix4x3_compute_product(&mover->delta, &world, &moved);
                                real_vector3d arm, velocity;
                                object_early_movers_arm(&mover->previous_center, &world.position, &arm);
                                object_early_movers_cross(&mover->previous_angular_velocity, &arm, &velocity);
                                add_vectors3d(&velocity, &mover->older_velocity, &velocity);
                                subtract_vectors3d(&object->object.linear_velocity, &velocity, &object->object.linear_velocity);
                                matrix4x3_transform_vector(&mover->delta, &object->object.linear_velocity, &object->object.linear_velocity);
                                real_vector3d current_arm;
                                object_early_movers_arm(&mover->older_center, &moved.position, &current_arm);
                                cross_product3d(&mover->angular_velocity2, &current_arm, mover_velocity);
                                add_vectors3d(mover_velocity, &mover->previous_velocity, mover_velocity);
                                s_mover *raw = (s_mover *)mover;
                                if (raw->bsp_count
                                    && ((1 << ((s_object_header *)object_header_data->data)[DATUM_INDEX_TO_ABSOLUTE_INDEX(index)].object_type) & 32)) {
                                    projectile_datum *collision_object = (projectile_datum *)object_get(index);
                                    collision_model_instance instance;
                                    if (::collision_model_instance_new(&instance, owner_index)) {
                                        for (long i=0; i<g_object_early_movers_globals->movers[mover_index].bsp_count; ++i) {
                                            unsigned long key=raw->bsps[i];
                                            early_mover_collision_definition *collision=(early_mover_collision_definition *)instance.collision_model;
                                            early_mover_region *regions=(early_mover_region *)collision->regions.address;
                                            early_mover_permutation *permutations=(early_mover_permutation *)regions[(key>>24)&255].permutations.address;
                                            early_mover_bsp_block *bsps=(early_mover_bsp_block *)permutations[(key>>16)&255].bsps.address;
                                            real_point3d local;
                                            early_mover_bsp_block *bsp=&bsps[(key>>8)&255];
                                            if (bsp3d_test_point((const bsp3d *)&bsp->bsp, 0,
                                                    matrix4x3_inverse_transform_point(
                                                        &((real_matrix4x3 *)instance.node_matrices)[key & 255], &moved.position, &local)) ==
                                                -1)
                                            {
                                                object_early_movers_point_from_line(&moved.position, &collision_object->projectile.hit_normal,
                                                    0.003f, &moved.position);
                                                break;
                                            }
                                        }
                                    }
                                }
                                object_set_position_direct(index, &moved.position, &moved.rotation.forward, &moved.rotation.up, 0, false);
                                *object_velocity = object->object.linear_velocity;
                            } else {
                                real_point3d position;
                                matrix4x3_transform_point(&mover->delta, &world.position, &position);
                                real_vector3d displacement;
                                object_early_movers_arm(&position, &world.position, &displacement);
                                float rate=(float)TLS_GAME_TIME_GLOBALS->ticks_per_second;
                                object_early_movers_3_scale_vector3d(&displacement, rate, &displacement);
                                object_set_position_direct(index, &position, 0, 0, 0, false);
                                *object_velocity = object->object.linear_velocity;
                                object_early_movers_add_velocity(&object->object.linear_velocity, &displacement, &object->object.linear_velocity);
                                object_early_movers_3_scale_vector3d(&displacement, -1.0f, mover_velocity);
                            }
                            moved_object=true;
                        }
                    }
                }
                break;
            }
        }
    }
    return moved_object;
}

early_movers_allocation_manager g_object_early_movers_globals_allocator;

