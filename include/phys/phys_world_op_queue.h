#ifndef INCLUDE_PHYS_WORLD_OP_QUEUE_H
#define INCLUDE_PHYS_WORLD_OP_QUEUE_H

#include <stddef.h>
#include "phys/phys_base_types.h"
#include "phys/phys_object.h"

class phys_world_object;
class phys_shape;
class phys_entity;
class phys_rigid_body;
class phys_volume;
class phys_action;
class phys_constraint;
class phys_aabb;
class phys_vec4;
enum phys_entity_activation;
enum phys_compound_filter_update_mode;
enum phys_entity_filter_update_mode;
enum phys_world_filter_update_mode;

namespace phys_world_op {
enum Type {
    WORLD_OP_ID_INVALID = 0,
    OP_ENTITY_ATTACH,
    OP_ENTITY_DETACH,
    OP_ENTITY_REFRESH_BROADPHASE,
    RIGID_BODY_SET_MOTION_TYPE,
    OP_WORLD_OBJECT_SET_SHAPE,
    OP_ENTITY_BATCH_ATTACH,
    OP_ENTITY_BATCH_DETACH,
    OP_CONSTRAINT_ATTACH,
    OP_CONSTRAINT_DETACH,
    OP_ACTION_ATTACH,
    OP_ACTION_DETACH,
    OP_ISLAND_MERGE,
    VOLUME_ATTACH,
    VOLUME_DETACH,
    VOLUME_BATCH_ATTACH,
    VOLUME_BATCH_DETACH,
    VOLUME_UPDATE_BROADPHASE,
    OP_REFILTER_ENTITY,
    OP_REFILTER_VOLUME,
    OP_REFILTER_WORLD,
    OP_REFRESH_MOVED_BODY_INFO,
    OP_ENTITY_BATCH_RESTEP,
    RIGID_BODY_SET_POS_AND_ROT,
    RIGID_BODY_SET_LIN_VEL,
    RIGID_BODY_SET_ANG_VEL,
    RIGID_BODY_APPLY_LIN_IMPULSE,
    RIGID_BODY_APPLY_POINT_IMPULSE,
    RIGID_BODY_APPLY_ANG_IMPULSE,
    OP_WORLD_OBJECT_ADD_REF,
    OP_WORLD_OBJECT_REMOVE_REF,
    OP_REGION_WAKE,
    OP_ENTITY_WAKE,
    OP_ENTITY_SLEEP,
    OP_USER_CALLBACK
};
struct base_op {
    base_op(Type type) : m_type(type) {}
    phys_enum<Type, unsigned char> m_type;
};
struct add_reference_op : public base_op {
    add_reference_op() : base_op(OP_WORLD_OBJECT_ADD_REF) {}
    phys_world_object *m_world_object;
};
struct remove_reference_op : public base_op {
    remove_reference_op() : base_op(OP_WORLD_OBJECT_REMOVE_REF) {}
    phys_world_object *m_world_object;
};
struct set_object_shape_op : public base_op {
    set_object_shape_op() : base_op(OP_WORLD_OBJECT_SET_SHAPE) {}
    phys_world_object *m_world_object;
    const phys_shape *m_proxy_shape;
};
struct activate_entity_op : public base_op {
    activate_entity_op() : base_op(OP_ENTITY_WAKE) {}
    phys_entity *m_op_entity;
};
struct deactivate_entity_op : public base_op {
    deactivate_entity_op() : base_op(OP_ENTITY_SLEEP) {}
    phys_entity *m_op_entity;
};
struct add_entity_op : base_op {
    add_entity_op() : base_op(OP_ENTITY_ATTACH) {}
    phys_entity *m_op_entity;
    phys_entity_activation m_add_activation;
};
struct remove_entity_op : base_op {
    remove_entity_op() : base_op(OP_ENTITY_DETACH) {}
    phys_entity *m_op_entity;
};
struct add_entity_batch_op : base_op {
    add_entity_batch_op() : base_op(OP_ENTITY_BATCH_ATTACH) {}
    phys_entity **m_island_entities;
    unsigned short m_entity_count;
    phys_enum<phys_entity_activation, unsigned char> m_add_activation;
};
struct remove_entity_batch_op : base_op {
    remove_entity_batch_op() : base_op(OP_ENTITY_BATCH_DETACH) {}
    phys_entity **m_island_entities;
    unsigned short m_entity_count;
};
struct add_constraint_op : base_op {
    add_constraint_op() : base_op(OP_CONSTRAINT_ATTACH) {}
    phys_constraint *m_contact_constraint;
};
struct remove_constraint_op : base_op {
    remove_constraint_op() : base_op(OP_CONSTRAINT_DETACH) {}
    phys_constraint *m_contact_constraint;
};
struct add_action_op : base_op {
    add_action_op() : base_op(OP_ACTION_ATTACH) {}
    phys_action *m_chain_action;
};
struct remove_action_op : base_op {
    remove_action_op() : base_op(OP_ACTION_DETACH) {}
    phys_action *m_chain_action;
};
struct merge_islands_op : base_op {
    typedef phys_entity *entity_pair[2];
    merge_islands_op() : base_op(OP_ISLAND_MERGE) {}
    entity_pair m_island_entities;
};
struct add_volume_op : base_op {
    add_volume_op() : base_op(VOLUME_ATTACH) {}
    phys_volume *m_overlap_volume;
};
struct remove_volume_op : base_op {
    remove_volume_op() : base_op(VOLUME_DETACH) {}
    phys_volume *m_overlap_volume;
};
struct add_volume_batch_op : base_op {
    add_volume_batch_op() : base_op(VOLUME_BATCH_ATTACH) {}
    phys_volume **m_border_volumes;
    unsigned short m_volume_count;
};
struct remove_volume_batch_op : base_op {
    remove_volume_batch_op() : base_op(VOLUME_BATCH_DETACH) {}
    phys_volume **m_border_volumes;
    unsigned short m_volume_count;
};
struct update_entity_broadphase_op : base_op {
    update_entity_broadphase_op() : base_op(OP_ENTITY_REFRESH_BROADPHASE) {}
    phys_entity *m_op_entity;
};
struct update_volume_broadphase_op : base_op {
    update_volume_broadphase_op() : base_op(VOLUME_UPDATE_BROADPHASE) {}
    phys_volume *m_overlap_volume;
    phys_aabb *m_volume_aabb;
};
struct update_entity_filter_op : base_op {
    update_entity_filter_op() : base_op(OP_REFILTER_ENTITY) {}
    phys_entity *m_op_entity;
    phys_enum<phys_entity_filter_update_mode, unsigned char> m_filter_update_mode;
    phys_enum<phys_compound_filter_update_mode, unsigned char> m_update_compound_shapes;
};
struct update_volume_filter_op : base_op {
    update_volume_filter_op() : base_op(OP_REFILTER_VOLUME) {}
    phys_volume *m_overlap_volume;
    phys_enum<phys_compound_filter_update_mode, unsigned char> m_update_compound_shapes;
};
struct update_world_filter_op : base_op {
    update_world_filter_op() : base_op(OP_REFILTER_WORLD) {}
    phys_enum<phys_world_filter_update_mode, unsigned char> m_filter_update_mode;
    phys_enum<phys_compound_filter_update_mode, unsigned char> m_update_compound_shapes;
};
struct update_moved_body_op : base_op {
    update_moved_body_op() : base_op(OP_REFRESH_MOVED_BODY_INFO) {}
    phys_entity *m_op_entity;
};
struct reintegrate_recollide_batch_op : base_op {
    reintegrate_recollide_batch_op() : base_op(OP_ENTITY_BATCH_RESTEP) {}
    phys_entity **m_island_entities;
    unsigned short m_entity_count;
    phys_enum<phys_entity_activation, unsigned char> m_add_activation;
};
struct set_pos_rot_op : base_op {
    set_pos_rot_op() : base_op(RIGID_BODY_SET_POS_AND_ROT) {}
    phys_rigid_body *m_op_rigid_body;
    phys_vec4 *m_pos_and_rot;
};
struct set_lin_vel_op : base_op {
    set_lin_vel_op() : base_op(RIGID_BODY_SET_LIN_VEL) {}
    phys_rigid_body *m_op_rigid_body;
    float m_linear_velocity[3];
};
struct set_ang_vel_op : base_op {
    set_ang_vel_op() : base_op(RIGID_BODY_SET_ANG_VEL) {}
    phys_rigid_body *m_op_rigid_body;
    float m_ang_velocity[3];
};
struct apply_lin_impulse_op : base_op {
    apply_lin_impulse_op() : base_op(RIGID_BODY_APPLY_LIN_IMPULSE) {}
    phys_rigid_body *m_op_rigid_body;
    float m_linear_impulse[3];
};
struct apply_point_impulse_op : base_op {
    apply_point_impulse_op() : base_op(RIGID_BODY_APPLY_POINT_IMPULSE) {}
    phys_rigid_body *m_op_rigid_body;
    phys_vec4 *m_point_impulse;
};
struct apply_ang_impulse_op : base_op {
    apply_ang_impulse_op() : base_op(RIGID_BODY_APPLY_ANG_IMPULSE) {}
    phys_rigid_body *m_op_rigid_body;
    float m_ang_impulse[3];
};
struct activate_region_op : base_op {
    activate_region_op() : base_op(OP_REGION_WAKE) {}
    phys_aabb *m_volume_aabb;
};
class user_callback : public phys_ref_object {
public:
    virtual void world_op_user_callback(void *action_user_data) = 0;
};
struct user_callback_op : base_op {
    user_callback_op() : base_op(OP_USER_CALLBACK) {}
    user_callback *m_callback;
    void *m_user_data;
};
struct biggest_op : base_op {
    biggest_op() : base_op(WORLD_OP_ID_INVALID) {}
    union {
        size_t m_operation_words[4];
        merge_islands_op::entity_pair m_island_entities;
    };
};
}

typedef char phys_world_op_add_ref_size[(sizeof(void *) != 4 || ((sizeof(phys_world_op::add_reference_op) == 8) )) ? 1 : -1];
typedef char phys_world_op_add_ref_offset[(sizeof(void *) != 4 || ((offsetof(phys_world_op::add_reference_op, m_world_object) == 4) )) ? 1 : -1];

typedef char phys_world_op_biggest_size[(sizeof(phys_world_op::biggest_op) == 5 * sizeof(void *)) ? 1 : -1];
typedef char phys_world_op_merge_offset[(offsetof(phys_world_op::biggest_op, m_island_entities) ==
    offsetof(phys_world_op::merge_islands_op, m_island_entities)) ? 1 : -1];
typedef char phys_world_op_add_entity_activation[(sizeof(void *) != 4 || ((offsetof(phys_world_op::add_entity_op, m_add_activation) == 8) )) ? 1 : -1];
typedef char phys_world_op_batch_count[(sizeof(void *) != 4
    || ((offsetof(phys_world_op::reintegrate_recollide_batch_op, m_entity_count) == 8) )) ? 1 : -1];
typedef char phys_world_op_batch_activation[(sizeof(void *) != 4
    || ((offsetof(phys_world_op::reintegrate_recollide_batch_op, m_add_activation) == 10) )) ? 1 : -1];
typedef char phys_world_op_volume_aabb[(sizeof(void *) != 4 || ((offsetof(phys_world_op::update_volume_broadphase_op, m_volume_aabb) == 8) )) ? 1 : -1];
#endif
