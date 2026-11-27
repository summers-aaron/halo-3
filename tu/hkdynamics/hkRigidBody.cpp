// hkdynamics/hkRigidBody.obj: from the records 0x648fe8 0x649038 0x649088 0x6490d0 0x6490e8 0x649118 0x649168 0x649178 0x649180 0x649188 0x6491a0 0x6491b8 0x6491d0 0x6491e8 0x6492f0 0x649530 0x6495a8 0x6497a0 0x6497f0 0x649920 0x649938 0x649960 0x649988 0x649ab8 0x649b00 0x649b48 0x649b90 0x649bd8 0x649c78 0x649d18 0x649db8 0x649e58 0x64a0a0 0x64a1e0 0x64a290 0x64a340 0x64a410 0x64a578 0x64a5e8
// @address 0x0164f4 phys_max_size_motion_vtable
// @flags /TP /O2 /Oc
// @address 0x648fe8 ?apply_lin_impulse@phys_rigid_body@@QAAXABVphys_vec4@@@Z
// @address 0x649038 ?apply_ang_impulse@phys_rigid_body@@QAAXABVphys_vec4@@@Z
// @address 0x649088 ?fixup_loaded_rigid_body@@YAXPAX@Z
// @address 0x6490d0 ?destroy_loaded_rigid_body@@YAXPAX@Z
// @address 0x6490e8 ?get_rigid_body_vtable@@YAPBXXZ
// @address 0x649168 ??1phys_rigid_body@@UAA@XZ
// @address 0x649178 ?get_motion_state@phys_rigid_body@@MAAPAVphys_motion_state@@XZ
// @address 0x649180 ?set_deactivator@phys_rigid_body@@QAAXPAVphys_rigid_body_deactivator@@@Z
// @address 0x649188 ?set_body_mass@phys_rigid_body@@QAAXM@Z
// @address 0x6491a0 ?set_inv_mass@phys_rigid_body@@QAAXM@Z
// @address 0x6491b8 ?set_local_inertia@phys_rigid_body@@QAAXABVphys_mat3@@@Z
// @address 0x6491d0 ?set_local_inv_inertia@phys_rigid_body@@QAAXABVphys_mat3@@@Z
// @address 0x6497a0 ?set_center_of_mass_local@phys_rigid_body@@QAAXABVphys_vec4@@@Z
// @address 0x6497f0 ?set_deactivator@phys_rigid_body@@QAAXW4sleep_detector_kind@phys_rigid_body_deactivator@@@Z
// @address 0x649920 ?get_deactivator_type@phys_rigid_body@@QAA?AW4sleep_detector_kind@phys_rigid_body_deactivator@@XZ
// @address 0x649938 ?get_stored_dynamic_motion@phys_rigid_body@@QAAPAVphys_motion@@XZ
// @address 0x649960 ?get_stored_dynamic_motion@phys_rigid_body@@QBAPBVphys_motion@@XZ
// @address 0x649ab8 ?contact_set_position@phys_rigid_body@@QAAXABVphys_vec4@@@Z
// @address 0x649b00 ?set_rotation@phys_rigid_body@@QAAXABVphys_quat@@@Z
// @address 0x649b48 ?set_position_and_rotation@phys_rigid_body@@QAAXABVphys_vec4@@ABVphys_quat@@@Z
// @address 0x649b90 ?set_transform@phys_rigid_body@@QAAXABVphys_transform@@@Z
// @address 0x649bd8 ?set_linear_velocity_critical@phys_rigid_body@@QAAXABVphys_vec4@@@Z
// @address 0x649c78 ?set_angular_velocity_as_critical_op@phys_rigid_body@@QAAXABVphys_vec4@@@Z
// @address 0x649d18 ?apply_lin_impulse_critical@phys_rigid_body@@QAAXABVphys_vec4@@@Z
// @address 0x649db8 ?apply_ang_impulse_critical@phys_rigid_body@@QAAXABVphys_vec4@@@Z
// @address 0x64a0a0 ?get_desc@phys_rigid_body@@QBAXAAVphys_rigid_body_desc@@@Z
// @address 0x64a1e0 ?set_position_and_rotation_critical@phys_rigid_body@@QAAXABVphys_vec4@@ABVphys_quat@@@Z
// @address 0x64a290 ?apply_point_impulse_critical@phys_rigid_body@@QAAXABVphys_vec4@@0@Z
// @address 0x64a340 ?check_performance@phys_rigid_body@@QBAXXZ
// @address 0x64a410 ?clone@phys_rigid_body@@UBAPAV1@XZ
// @address 0x64a5e8 ?copy_object_props@phys_world_object@@QAAXPBV1@@Z
#include "phys/phys_base_types.h"
#include "phys/phys_world_op_queue.h"
// @address 0x649118 ?estimate_allowed_penetration@@YAXPAVphys_collider@@MABVphys_vec4@@@Z
// @address 0x6491e8 ?update_cached_shape_info@phys_rigid_body@@QAAXPBVphys_shape@@AAVphys_vec4@@@Z
// @address 0x6492f0 ?create_dynamic_motion@phys_rigid_body@@SAXW4motion_kind@phys_motion@@ABVphys_vec4@@ABVphys_quat@@MABVphys_mat3@@1MMPAVphys_max_size_motion@@@Z
// @address 0x649988 ?refresh_warped_body@phys_rigid_body@@SAXPAVphys_entity@@@Z
// @address 0x64a578 ??_Gphys_rigid_body@@UAAPAXI@Z
// @address 0x6495a8 ?set_proxy_shape@phys_rigid_body@@UAA?AW4Result@phys_world_op@@PBVphys_shape@@@Z
#include <ppcintrinsics.h>
#include <vectorintrinsics.h>
#include <math.h>
#include <stdlib.h>
#include "physics/havok_halo_external.h"
#include "phys/phys_math.h"
#include "phys/phys_collide.h"
#include "phys/phys_simulation.h"
#include "phys/phys_rigid_body.h"
#include "phys/phys_dynamics.h"
#include "phys/phys_world_op_util.h"
#include "phys/phys_entity_callback_util.h"
#include "phys/phys_spatial_body_deactivator.h"
#include "phys/phys_fake_rigid_body_deactivator.h"
#include "phys/phys_type_desc.h"

enum phys_memory_class;
class phys_entity_deactivator;
struct motion_kind_op : phys_world_op::base_op {
    __forceinline motion_kind_op() : base_op(phys_world_op::RIGID_BODY_SET_MOTION_TYPE) {}
    phys_rigid_body *owner;
    unsigned char a, b, c;
};
class phys_entity;

class phys_world;


#include "phys/phys_world_callback_util.h"

#include "phys/phys_aabb_types.h"

class phys_shape;

class phys_statistics_collector;

#include "phys/phys_thread_memory.h"
#include "phys/phys_fixup_tag.h"
class phys_fixup_tag;

class phys_rigid_body_desc;

class phys_ref_object;

// 0x6491e8

// 0x6495a8


class phys_ref_object;


namespace {
static __forceinline void release_deactivator_ref(phys_rigid_body_deactivator *deactivator) {
    if (deactivator->m_mem_size_and_flags != 0) {
        if (--deactivator->m_object_ref_count == 0) {
            if (halo_managed_memory_address((const phys_ref_object *)deactivator)) deactivator->m_object_ref_count = 1;
            else delete deactivator;
        }
    }
}
static __forceinline phys_vec4 *alloc_vec_chunk(int count, int mem_class) {
    phys_thread_memory *memory = (&phys_thread_memory::get_singleton_instance());
    return (phys_vec4 *)((phys_thread_memory *)memory)->allocate_chunk(count * sizeof(phys_vec4), (phys_memory_class)mem_class);
}
}


// 0x648fe8
void phys_rigid_body::apply_lin_impulse(const phys_vec4 &body_linear_impulse)
{
    activate();
#if !defined(_XBOX) || defined(__clang__)
    get_rigid_motion()->apply_lin_impulse(body_linear_impulse);
#else
    m_motion.apply_lin_impulse(body_linear_impulse);
#endif
}
// 0x649038
void phys_rigid_body::apply_ang_impulse(const phys_vec4 &body_linear_impulse)
{
    activate();
#if !defined(_XBOX) || defined(__clang__)
    get_rigid_motion()->apply_ang_impulse(body_linear_impulse);
#else
    m_motion.apply_ang_impulse(body_linear_impulse);
#endif
}

// 0x649088
void fixup_loaded_rigid_body(void *memory)
{
    if (memory) {
        phys_fixup_tag flag;
        flag.m_fixup_finishing = 1;
        new (memory) phys_rigid_body(flag);
    }
}
// 0x6490d0
void destroy_loaded_rigid_body(void *object)
{
    ((phys_rigid_body *)object)->~phys_rigid_body();
}
// 0x6490e8
const void *get_rigid_body_vtable()
{
    return phys_type_desc::get_vtable<phys_rigid_body>();
}
// 0x649168
phys_rigid_body::~phys_rigid_body()
{
}
// 0x649178
phys_motion_state *phys_rigid_body::get_motion_state()
{
    return get_rigid_motion()->get_motion_state();
}
// 0x649180
void phys_rigid_body::set_deactivator(phys_rigid_body_deactivator *deactivator)
{
    phys_entity::set_deactivator((phys_entity_deactivator *)deactivator);
}
// 0x649188
void phys_rigid_body::set_body_mass(float mass)
{
    static_cast<phys_motion &>(m_motion).set_body_mass(mass);
}
// 0x6491a0
void phys_rigid_body::set_inv_mass(float inv_mass)
{
    static_cast<phys_motion &>(m_motion).set_inv_mass(inv_mass);
}
// 0x6491b8
void phys_rigid_body::set_local_inertia(const phys_mat3 &inertia)
{
    static_cast<phys_motion &>(m_motion).set_local_inertia(inertia);
}
// 0x6491d0
void phys_rigid_body::set_local_inv_inertia(const phys_mat3 &inv_inertia)
{
    static_cast<phys_motion &>(m_motion).set_local_inv_inertia(inv_inertia);
}
void estimate_allowed_penetration(phys_collider *collidable, float allowed_penetration_depth, const phys_vec4 &extent)
{
    float min_object_diameter = phys_math::min_of_two(phys_math::min_of_two(extent(0), extent(1)), extent(2));
    if (min_object_diameter < 0.5f)
        collidable->m_allowed_penetration_depth = min_object_diameter * 0.2f;
    else
        collidable->m_allowed_penetration_depth = 0.1f;
}

void phys_rigid_body::update_cached_shape_info(const phys_shape *shape, phys_vec4 &extent)
{
    phys_aabb aabb;
    shape->get_handle_aabb(phys_transform::get_identity(), 0.0f, aabb);
    extent.set_sub4(aabb.m_max_limit, aabb.m_min_limit);
    float object_radius = 0.5f * (float)extent.length_3d();
    phys_vec4 center;
    center.set_add4(aabb.m_min_limit, aabb.m_max_limit);
    center.mul4(phys_simd_scalar(0.5f));
    phys_vec4 difference;
    difference.set_sub4(center, get_center_of_mass_local());
    object_radius += (float)difference.length_3d();
    phys_motion_state *motion_state = get_rigid_motion()->get_motion_state();
    motion_state->m_object_radius = object_radius;
}

void phys_rigid_body::create_dynamic_motion(phys_motion::motion_kind body_motion_type, const phys_vec4 &position, const phys_quat &rotation,
    float mass, const phys_mat3 &local_inertia, const phys_vec4 &local_centre_of_mass, float max_lin_vel, float max_ang_vel,
    phys_max_size_motion *motion_buf_out)
{
    phys_motion *motion;
    switch (body_motion_type)
    {
    case phys_motion::BODY_MOTION_SPHERE_INERTIA:
        if (motion_buf_out) {
            motion = new (motion_buf_out) phys_sphere_motion(position, rotation);
        } else {
            motion = 0;
        }
        break;
    case phys_motion::BODY_MOTION_BOX_INERTIA:
        if (motion_buf_out) {
            motion = new (motion_buf_out) phys_box_motion(position, rotation);
        } else {
            motion = 0;
        }
        break;
    case phys_motion::BODY_MOTION_THIN_BOX_INERTIA:
        if (motion_buf_out) {
            motion = new (motion_buf_out) phys_thin_box_motion(position, rotation);
        } else {
            motion = 0;
        }
        break;
    case phys_motion::BODY_MOTION_STABLE_SPHERE_INERTIA:
        if (motion_buf_out) {
            motion = new (motion_buf_out) phys_stabilized_sphere_motion(position, rotation);
        } else {
            motion = 0;
        }
        break;
    case phys_motion::BODY_MOTION_STABLE_BOX_INERTIA:
        if (motion_buf_out) {
            motion = new (motion_buf_out) phys_stabilized_box_motion(position, rotation);
        } else {
            motion = 0;
        }
        break;
    case phys_motion::BODY_MOTION_DYNAMIC:
        {
            float inertia_diag_x = local_inertia.m_column0(0);
            float inertia_diag_y = local_inertia.m_column1(1);
            float inertia_diag_z = local_inertia.m_column2(2);
            float inertia_diff_yz = inertia_diag_y - inertia_diag_z;
            float hi = (float)__fsel(inertia_diff_yz, inertia_diag_y, inertia_diag_z);
            float lo = (float)__fsel(inertia_diff_yz, inertia_diag_z, inertia_diag_y);
            float ma = (float)__fsel(inertia_diag_x - hi, inertia_diag_x, hi);
            float mi = (float)__fsel(inertia_diag_x - lo, lo, inertia_diag_x);
            if (mi > ma * 0.8f) {
                if (motion_buf_out) {
                    motion = new (motion_buf_out) phys_sphere_motion(position, rotation);
                } else {
                    motion = 0;
                }
            } else {
                if (motion_buf_out) {
                    motion = new (motion_buf_out) phys_box_motion(position, rotation);
                } else {
                    motion = 0;
                }
            }
        }
        break;
    case phys_motion::MOTION_KEYFRAMED:
        max_lin_vel = 1e6f;
        max_ang_vel = max_lin_vel;
        if (motion_buf_out) {
            motion = new (motion_buf_out) phys_keyframed_motion(position, rotation);
        } else {
            motion = 0;
        }
        break;
    default:
        if (motion_buf_out) {
            motion = new (motion_buf_out) phys_fixed_motion(position, rotation);
        } else {
            motion = 0;
        }
        break;
    }
    if (body_motion_type != phys_motion::MOTION_KEYFRAMED) {
        motion->set_local_inertia(local_inertia);
        motion->set_center_of_mass_in_local(local_centre_of_mass);
        motion->set_body_mass(mass);
    }
    motion->m_motion_state.m_max_lin_vel = max_lin_vel;
    motion->m_motion_state.m_max_ang_vel = max_ang_vel;
    motion->m_motion_state.m_deactivation_counter = 20;
}

// @address 0x649530 ?set_motion_type@phys_rigid_body@@QAAXW4motion_kind@phys_motion@@W4phys_entity_activation@@W4phys_entity_filter_update_mode@@@Z
void phys_rigid_body::set_motion_type(phys_motion::motion_kind motion_type, phys_entity_activation activation, phys_entity_filter_update_mode filter_mode)
{
    if (get_action_world() != 0 && get_action_world()->m_critical_ops_lock_count != 0) {
        motion_kind_op operation;
        operation.owner = this; operation.a = motion_type; operation.b = activation; operation.c = filter_mode;
        get_action_world()->queue_world_op(operation);
        return;
    }
    phys_world_op_util::set_rigid_body_motion_type(this, motion_type, activation, filter_mode);
}


phys_world_op::Result phys_rigid_body::set_proxy_shape(const phys_shape *shape)
{
    if (m_world && m_world->m_critical_ops_lock_count != 0) {
        phys_world_op::set_object_shape_op op;
        op.m_world_object = this;
        op.m_proxy_shape = shape;
        m_world->queue_world_op(op);
        return phys_world_op::OP_POSTPONED;
    }
    if (m_world) {
        m_world->lock_critical_ops();
        phys_world_op_util::remove_entity_from_broadphase(m_world, this);
    }
    {
        const phys_shape *previous_shape = m_collider.m_proxy_shape;
        m_collider.m_proxy_shape = shape;
        shape->add_object_reference();
        if (previous_shape) {
            previous_shape->object_remove_ref();
        }
        phys_vec4 extent;
        update_cached_shape_info(shape, extent);
        if (previous_shape && m_collider.m_allowed_penetration_depth != 3.40282e+38f) {
            m_collider.m_allowed_penetration_depth = -1.0f;
        }
        if (m_collider.m_allowed_penetration_depth <= 0.0f) {
            estimate_allowed_penetration(this->get_collider_rw(), m_collider.m_allowed_penetration_depth, extent);
        }
    }
    if (m_world) {
        phys_world_callback_util::notify_entity_shape_set(m_world, this);
    }
    phys_entity_callback_util::notify_entity_shape_set(this);
    if (m_world) {
        phys_world_op_util::add_entity_broadphase(m_world, this);
        m_world->unlock_and_flush_ops();
    }
    return phys_world_op::STEP_RESULT_DONE;
}

// 0x6497a0
void phys_rigid_body::set_center_of_mass_local(const phys_vec4 &shape_centre)
{
    static_cast<phys_motion &>(m_motion).set_center_of_mass_in_local(shape_centre);
    phys_vec4 extent;
    update_cached_shape_info(m_collider.m_proxy_shape, extent);
}

// 0x6497f0
void phys_rigid_body::set_deactivator(phys_rigid_body_deactivator::sleep_detector_kind type)
{
    if (get_deactivator() != 0) {
        phys_rigid_body_deactivator *deactivator = static_cast<phys_rigid_body_deactivator *>(m_body_deactivator);
        if (deactivator->get_body_deactivator_type() == type) return;
    }
    switch (type) {
    case phys_rigid_body_deactivator::sleep_policy_never:
        set_deactivator_inline(phys_fake_rigid_body_deactivator::get_fake_body_deactivator());
        m_motion.enable_sleeping(false, 0);
        break;
    case phys_rigid_body_deactivator::sleep_policy_spatial:
        {
            phys_spatial_body_deactivator *deactivator = new phys_spatial_body_deactivator;
            set_deactivator_inline(deactivator);
            m_motion.enable_sleeping(true, m_entity_uid);
            release_deactivator_ref(deactivator);
        }
        break;
    default:
        break;
    }
}

// @address 0x649e58 ??0phys_rigid_body@@QAA@ABVphys_rigid_body_desc@@@Z
phys_rigid_body::phys_rigid_body(const phys_rigid_body_desc &info)
    : phys_entity(info.m_proxy_shape)
{
    m_body_material.m_response_type = info.m_collision_response;
    m_contact_callback_delay = info.m_contact_callback_delay;
    m_collider.m_broadphase_handle.m_collision_filter_info = info.m_collision_filter_info;
    if (info.m_motion_type == phys_motion::MOTION_FIXED) {
        phys_fixed_motion *p = new (&m_motion) phys_fixed_motion(info.m_position, info.m_rotation);
        p->m_motion_state.m_max_lin_vel = info.m_max_lin_vel;
        p->m_motion_state.m_max_ang_vel = info.m_max_ang_vel;
        p->set_deactivation_class(1);
        phys_motion_state *ms = get_rigid_motion()->get_motion_state();
        this->get_collider_rw()->m_motion = ms;
        if (info.m_allowed_penetration_depth <= 0.0f) {
            m_collider.m_allowed_penetration_depth = 3.40282e+38f;
        } else {
            m_collider.m_allowed_penetration_depth = info.m_allowed_penetration_depth;
        }
    } else {
        phys_rigid_body::create_dynamic_motion((phys_motion::motion_kind)info.m_motion_type, info.m_position, info.m_rotation, info.m_body_mass,
            info.m_inertia_tensor, info.m_center_of_mass, info.m_max_lin_vel, info.m_max_ang_vel, (phys_max_size_motion *)&m_motion);
        phys_motion *rigid_motion = &m_motion;
        rigid_motion->set_deactivation_class(info.m_solver_sleep);
        set_linear_velocity(info.m_linear_velocity);
        set_angular_velocity(info.m_ang_velocity);
        this->get_collider_rw()->m_motion = get_rigid_motion()->get_motion_state();
        set_deactivator((phys_rigid_body_deactivator::sleep_detector_kind)info.m_deactivator_kind);
        this->get_collider_rw()->m_allowed_penetration_depth = info.m_allowed_penetration_depth;
    }
    m_motion.m_motion_state.m_linear_damping = info.m_linear_damping;
    m_motion.m_motion_state.m_ang_damping = info.m_ang_damping;
    if (m_collider.get_shape() != 0) {
        phys_vec4 extent;
        update_cached_shape_info(m_collider.m_proxy_shape, extent);
        if (m_collider.m_allowed_penetration_depth <= 0.0f) {
            estimate_allowed_penetration(this->get_collider_rw(), m_collider.m_allowed_penetration_depth, extent);
        }
    }
    if (info.m_collider_quality != 0) {
        m_collider.m_broadphase_handle.m_quality_type = (unsigned short)info.m_collider_quality;
    } else {
        if (body_is_fixed()) {
            m_collider.m_broadphase_handle.m_quality_type = 1;
        } else if (info.m_motion_type == phys_motion::MOTION_KEYFRAMED) {
            m_collider.m_broadphase_handle.m_quality_type = 2;
        } else {
            m_collider.m_broadphase_handle.m_quality_type = 3;
        }
    }
    m_auto_remove_level = info.m_auto_remove_level;
    if (info.m_contact_user_data_count) {
        m_collider.m_force_collide_ppu |= 1;
    }
    m_body_material.m_contact_friction = info.m_contact_friction;
    m_body_material.m_contact_restitution = info.m_contact_restitution;
}

// 0x649920
phys_rigid_body_deactivator::sleep_detector_kind phys_rigid_body::get_deactivator_type()
{
    return static_cast<phys_rigid_body_deactivator *>(m_body_deactivator)->get_body_deactivator_type();
}
// 0x649938
phys_motion *phys_rigid_body::get_stored_dynamic_motion()
{
    if (m_motion.get_type_id() != 6 && m_motion.get_type_id() != 7) return 0;
    else return m_motion.m_saved_motion;
}
// 0x649960
const phys_motion *phys_rigid_body::get_stored_dynamic_motion() const
{
    if (m_motion.get_type_id() != 6 && m_motion.get_type_id() != 7) return 0;
    else return m_motion.m_saved_motion;
}
// 0x649988
void phys_rigid_body::refresh_warped_body(phys_entity *body)
{
    phys_rigid_body *owner_rigid_body = (phys_rigid_body *)body;
    phys_world *world = owner_rigid_body->m_world;
    if (world != 0) {
        if (world->m_critical_ops_lock_count != 0) {
            phys_world_op::update_moved_body_op operation;
            operation.m_op_entity = body;
            world->queue_world_op(operation);
            return;
        }
        world->m_critical_ops_lock_count = 1;
        phys_entity *entity = body;
        world->m_world_simulation->reset_collision_info_for_entities(&entity, 1, world, false);
        if (owner_rigid_body->m_collider.get_shape() != 0)
            phys_simulation::collide_entities_broadphase_discrete(&entity, 1, world);
        if (!body->is_body_active()) {
            if (world->m_wake_on_body_move) {
                if (owner_rigid_body->m_motion.m_type != 7)
                    body->activate();
            }
            phys_world_callback_util::notify_inactive_entity_moved(world, body);
        }
        world->unlock_and_flush_ops();
    }
}
// 0x649ab8
void phys_rigid_body::contact_set_position(const phys_vec4 &position)
{
    static_cast<phys_motion &>(m_motion).contact_set_position(position);
    refresh_warped_body(this);
}
// 0x649b00
void phys_rigid_body::set_rotation(const phys_quat &rotation)
{
    static_cast<phys_motion &>(m_motion).set_rotation(rotation);
    refresh_warped_body(this);
}
// 0x649b48
void phys_rigid_body::set_position_and_rotation(const phys_vec4 &position, const phys_quat &rotation)
{
    static_cast<phys_motion &>(m_motion).set_position_and_rotation(position, rotation);
    refresh_warped_body(this);
}
// 0x649b90
void phys_rigid_body::set_transform(const phys_transform &transform)
{
    static_cast<phys_motion &>(m_motion).set_transform(transform);
    refresh_warped_body(this);
}
// 0x649bd8
void phys_rigid_body::set_linear_velocity_critical(const phys_vec4 &vector)
{
    if (get_action_world() != 0 && m_world->m_critical_ops_lock_count) {
        phys_world_op::set_lin_vel_op operation;
        operation.m_op_rigid_body = this;
        operation.m_linear_velocity[0] = vector(0);
        operation.m_linear_velocity[1] = vector(1);
        operation.m_linear_velocity[2] = vector(2);
        m_world->queue_world_op(operation);
    } else {
        set_linear_velocity(vector);
    }
}
// 0x649c78
void phys_rigid_body::set_angular_velocity_as_critical_op(const phys_vec4 &vector)
{
    if (get_action_world() != 0 && m_world->m_critical_ops_lock_count) {
        phys_world_op::set_ang_vel_op operation;
        operation.m_op_rigid_body = this;
        operation.m_ang_velocity[0] = vector(0);
        operation.m_ang_velocity[1] = vector(1);
        operation.m_ang_velocity[2] = vector(2);
        m_world->queue_world_op(operation);
    } else {
        set_angular_velocity(vector);
    }
}
// 0x649d18
void phys_rigid_body::apply_lin_impulse_critical(const phys_vec4 &vector)
{
    if (get_action_world() != 0 && m_world->m_critical_ops_lock_count) {
        phys_world_op::apply_lin_impulse_op operation;
        operation.m_op_rigid_body = this;
        operation.m_linear_impulse[0] = vector(0);
        operation.m_linear_impulse[1] = vector(1);
        operation.m_linear_impulse[2] = vector(2);
        m_world->queue_world_op(operation);
    } else {
        apply_lin_impulse(vector);
    }
}
// 0x649db8
void phys_rigid_body::apply_ang_impulse_critical(const phys_vec4 &vector)
{
    if (get_action_world() != 0 && m_world->m_critical_ops_lock_count) {
        phys_world_op::apply_ang_impulse_op operation;
        operation.m_op_rigid_body = this;
        operation.m_ang_impulse[0] = vector(0);
        operation.m_ang_impulse[1] = vector(1);
        operation.m_ang_impulse[2] = vector(2);
        m_world->queue_world_op(operation);
    } else {
        apply_ang_impulse(vector);
    }
}

// 0x64a0a0
void phys_rigid_body::get_desc(phys_rigid_body_desc &info) const
{
    info.m_contact_user_data_count = (m_collider.m_force_collide_ppu & 1) != 0;
    info.m_auto_remove_level = m_auto_remove_level;
    info.m_contact_callback_delay = m_contact_callback_delay;
    phys_rigid_body_deactivator::sleep_detector_kind deactivator_kind;
    if (get_deactivator() != 0) {
        const phys_rigid_body_deactivator *deactivator = static_cast<const phys_rigid_body_deactivator *>(m_body_deactivator);
        deactivator_kind = deactivator->get_body_deactivator_type();
    }
    else deactivator_kind = phys_rigid_body_deactivator::sleep_policy_never;
    info.m_deactivator_kind = deactivator_kind;
    info.m_contact_friction = m_body_material.m_contact_friction;
    info.m_collision_response = m_body_material.m_response_type;
    info.m_contact_restitution = m_body_material.m_contact_restitution;
    info.m_linear_damping = m_motion.m_motion_state.m_linear_damping;
    info.m_ang_damping = m_motion.m_motion_state.m_ang_damping;
    info.m_linear_velocity = m_motion.get_linear_velocity();
    info.m_ang_velocity = m_motion.get_angular_velocity();
    info.m_body_mass = m_motion.get_mass();
    static_cast<const phys_motion &>(m_motion).get_inertia_local(info.m_inertia_tensor);
    info.m_motion_type = m_motion.get_type_id();
    info.m_solver_sleep = (phys_solver_sleep)m_motion.m_motion_state.m_deactivation_class;
    info.m_max_lin_vel = m_motion.m_motion_state.m_max_lin_vel;
    info.m_max_ang_vel = m_motion.m_motion_state.m_max_ang_vel;
    info.m_position = m_motion.m_motion_state.m_body_transform.m_translation;
    info.m_rotation = m_motion.get_rotation();
    info.m_center_of_mass = m_motion.get_center_of_mass_local();
    info.m_proxy_shape = m_collider.m_proxy_shape;
    info.m_collision_filter_info = m_collider.m_broadphase_handle.m_collision_filter_info;
    info.m_allowed_penetration_depth = m_collider.m_allowed_penetration_depth;
    info.m_collider_quality = (phys_collider_quality)m_collider.m_broadphase_handle.m_quality_type;
}
// 0x64a1e0
void phys_rigid_body::set_position_and_rotation_critical(const phys_vec4 &position, const phys_quat &rotation)
{
    if (get_action_world() != 0 && m_world->m_critical_ops_lock_count) {
        phys_world_op::set_pos_rot_op operation;
        operation.m_op_rigid_body = this;
        operation.m_pos_and_rot = alloc_vec_chunk(2, 4);
        operation.m_pos_and_rot[0] = position;
        operation.m_pos_and_rot[1] = rotation.m_quat_vec;
        m_world->queue_world_op(operation);
    } else {
        set_position_and_rotation(position, rotation);
    }
}
// 0x64a290
void phys_rigid_body::apply_point_impulse_critical(const phys_vec4 &impulse, const phys_vec4 &position)
{
    if (get_action_world() != 0 && m_world->m_critical_ops_lock_count) {
        phys_world_op::apply_point_impulse_op operation;
        operation.m_op_rigid_body = this;
        operation.m_point_impulse = alloc_vec_chunk(2, 4);
        operation.m_point_impulse[0] = position;
        operation.m_point_impulse[1] = impulse;
        m_world->queue_world_op(operation);
    } else {
        apply_point_impulse(impulse, position);
    }
}

// 0x64a340
void phys_rigid_body::check_performance() const
{
    const phys_shape *shape = m_collider.m_proxy_shape;
    if (shape == 0) {
        return;
    } else {
        int type = shape->get_type_id();
        if (type == 10)
            static_cast<const phys_compound_shape *>(shape)->get_child_shape_count();
        if (m_motion.m_type != 7 && (type == 27 || type == 19)) {
            const phys_shape_container *container = shape->get_shape_container();
            container->get_child_shape_count();
        }
        if (type == 2) {
            static_cast<const phys_compound_shape *>(m_collider.m_proxy_shape)->get_child_shape_count();
        }
        if (m_motion.m_type != 7 && type == 11)
            static_cast<const phys_compound_shape *>(shape)->get_child_shape_count();
    }
}

// 0x64a410
phys_rigid_body *phys_rigid_body::clone() const
{
    phys_rigid_body_desc cur_body_desc;
    {
        get_desc(cur_body_desc);
    }
    phys_rigid_body *body_clone = new phys_rigid_body(cur_body_desc);
    {
        phys_string::mem_copy16_nonempty(&body_clone->m_motion, &m_motion, (unsigned int)(sizeof(phys_max_size_motion) >> 4));
        if (m_motion.m_saved_motion) {
            phys_thread_memory *memory = (&phys_thread_memory::get_singleton_instance());
            phys_max_size_motion *saved_motion = (phys_max_size_motion *)memory->allocate_chunk(sizeof(phys_max_size_motion), PHYS_MEM_MOTION);
            saved_motion->m_mem_size_and_flags = sizeof(phys_max_size_motion);
            *(const void **)saved_motion = phys_max_size_motion_vtable;
            saved_motion->m_object_ref_count = 1;
            saved_motion->m_saved_motion = 0;
            body_clone->m_motion.m_saved_motion = saved_motion;
            phys_string::mem_copy16_nonempty(body_clone->m_motion.m_saved_motion, m_motion.m_saved_motion, (unsigned int)(sizeof(phys_max_size_motion) >> 4));
        }
        body_clone->get_collider_rw()->m_motion = body_clone->get_motion_state();
    }
    body_clone->copy_object_props(this);
    {
        body_clone->m_name = m_name;
        body_clone->m_user_data = m_user_data;
    }
    return body_clone;
}

// 0x64a5e8
void phys_world_object::copy_object_props(const phys_world_object *other)
{
    m_properties = other->m_properties;
}

const phys_type_desc phys_rigid_body_type_desc = phys_type_desc("hkRigidBody", fixup_loaded_rigid_body, destroy_loaded_rigid_body, get_rigid_body_vtable());

