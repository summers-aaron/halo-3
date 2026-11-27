#ifndef INCLUDE_PHYS_DYNAMICS_H
#define INCLUDE_PHYS_DYNAMICS_H

#include <stddef.h>
#include "phys/phys_collide.h"
#include "phys/phys_step_info.h"
#include "physics/property_set_types.h"

class phys_world;
class phys_type_desc;
class phys_entity;
class phys_rigid_body;
class phys_volume;
class phys_action;
class phys_simulation;
class phys_island;
class phys_constraint;
class phys_constraint_data;
class phys_entity_deactivator;
class phys_world_maintenance_manager;
class phys_world_memory_watchdog;
class phys_broadphase;
class phys_typed_broadphase_dispatcher;
class phys_volume_broadphase_listener;
class phys_entity_pair_broadphase_listener;
class phys_broadphase_border_listener;
class phys_broadphase_border;
struct phys_process_collision_input;
class phys_collision_filter;
class phys_dispatcher;
class phys_convex_list_filter;
class phys_world_op_queue;
struct phys_pending_op_queue_debug_info;
class phys_action_listener;
class phys_entity_listener;
class phys_entity_activation_listener;
class phys_volume_listener;
class phys_constraint_listener;
class phys_world_deletion_listener;
class phys_island_activation_listener;
class phys_post_simulation_listener;
class phys_post_integrate_listener;
class phys_post_collide_listener;
class phys_island_post_integrate_listener;
class phys_island_post_collide_listener;
class phys_collision_listener;
class phys_collision_callback_util;
class phys_physics_system;
class phys_dynamics_contact_manager;
class phys_critical_section;
enum phys_collider_accept;

class phys_multi_thread_lock {
public:
    enum access_mode { PHYS_ACCESS_IGNORE, PHYS_ACCESS_RO, PHYS_ACCESS_RW };
    enum read_mode { READ_SELF_ONLY, READ_MODE_RECURSIVE };
    void check_access(access_mode type) const;
    static void check_access_with_parent(const phys_multi_thread_lock *parent, access_mode lock_parent_access,
        const phys_multi_thread_lock &child, access_mode child_access_type);
    void disable_checks();
    void enable_checks();
    bool is_checking_enabled() const;
    static void static_init();
    static void static_quit();
    static phys_critical_section *m_display_lock;
    __forceinline phys_multi_thread_lock() : m_lock_thread_id(0xffffffd1), m_lock_count(0) {}
    __forceinline void init() { m_lock_thread_id = 0xffffffd1; m_lock_count = 0; }
    __forceinline void mark_for_read(read_mode mode = READ_MODE_RECURSIVE) {}
    __forceinline void mark_for_write() {}
    __forceinline void unmark_for_read() {}
    __forceinline void unmark_for_write() {}
protected:
    void mark_for_read_impl(read_mode mode);
    void mark_for_write_impl();
    bool is_marked_for_write_impl();
    void unmark_for_read_impl();
    void unmark_for_write_impl();
    enum { MARK_UNLOCKED = -15, MT_LOCK_READ_ONLY = -31, MT_MARKING_OFF = -47, MT_LOCK_READ_SELF_ONLY = -63 };
public:
    unsigned int m_lock_thread_id;
    short m_lock_count;
    unsigned short m_lock_bit_stack;
};

#include "phys/phys_critical_section.h"
class phys_world_memory_watchdog;
class phys_job_queue;
struct phys_multithread_config;
namespace phys_world_op {
    enum Result {
        OP_POSTPONED = 0,
        STEP_RESULT_DONE = 1
    };
    struct base_op;
    class user_callback;
}

class phys_world_object : public phys_ref_object {
public:
    enum broadphase_kind {
        BROADPHASE_KIND_INVALID = 0,
        BROADPHASE_KIND_ENTITY = 1,
        BROADPHASE_KIND_VOLUME = 2,
        BROADPHASE_KIND_BORDER = 3,
        BROADPHASE_KIND_COUNT = 4
    };

    void add_property(unsigned int key, phys_property_value value);
    phys_property_value remove_object_property(unsigned int key);
    phys_property_value edit_property(unsigned int key, phys_property_value value);
    void copy_object_props(const phys_world_object *other);
    void check_read_only() const;
    void check_read_write();
    void mark_for_write_impl();
    void lock_property(unsigned int key);
    void unlock_property(unsigned int key);

    __forceinline phys_property_value get_world_object_property(unsigned int key) const {
        for (int i = 0; i < m_properties.getSize(); ++i) {
            if (m_properties[i].m_key == key) {
                return m_properties[i].m_value;
            }
        }
        return phys_property_value(0ull);
    }
    __forceinline phys_bool has_property(unsigned int key) const {
        for (int i = 0; i < m_properties.getSize(); ++i) {
            if (m_properties[i].m_key == key) {
                return phys_bool(true);
            }
        }
        return phys_bool(false);
    }

    __forceinline phys_collider *get_collider_rw() { return &m_collider; }
    __forceinline const phys_collider *get_collider() const { return &m_collider; }
    __forceinline phys_linked_collider *get_linked_collider() { return &m_collider; }
    __forceinline const phys_linked_collider *get_linked_collider() const { return &m_collider; }
    __forceinline phys_bool is_in_world() const { return phys_bool(m_world != 0); }
    __forceinline phys_world *get_action_world() const { return m_world; }
    __forceinline unsigned long get_user_data() const { return m_user_data; }
    __forceinline void set_user_data(unsigned long data) { m_user_data = data; }
    __forceinline const char *get_action_name() const { return m_name; }
    __forceinline void set_action_name(const char *name) { m_name = name; }

    void add_object_reference();
    void object_remove_ref();
    void add_reference_critical();
    void remove_reference_as_critical_op();

    virtual void gather_memory_stats(phys_statistics_collector *collector) const;
    virtual phys_world_op::Result set_proxy_shape(const phys_shape *shape);

protected:
    virtual phys_motion_state *get_motion_state() = 0;
    phys_world_object(const phys_shape *shape, broadphase_kind type);
    virtual ~phys_world_object();

public:
    phys_world_object(phys_fixup_tag flag);

    phys_world *m_world;
    unsigned long m_user_data;
    phys_linked_collider m_collider;
    phys_multi_thread_lock m_mt_lock;
    const char *m_name;
    phys_array<phys_property> m_properties;
};

__forceinline phys_world_object *phys_get_world_object(const phys_collider *collidable) {
    return reinterpret_cast<phys_world_object *>(collidable->get_collider_owner());
}

class phys_material {
public:
    enum response_kind {
        COLLISION_RESPONSE_INVALID = 0,
        COLLISION_RESPONSE_SIMPLE_CONTACT = 1,
        COLLISION_RESPONSE_REPORTING = 2,
        COLLISION_RESPONSE_NONE = 3,
        COLLISION_RESPONSE_ID_MAX = 4
    };

    __forceinline phys_material() : m_contact_friction(0.5f), m_contact_restitution(0.4f) {}
    __forceinline phys_material(phys_fixup_tag) {}

    __forceinline float get_friction() const { return m_contact_friction; }
    __forceinline response_kind get_response_type() const { return m_response_type; }
    __forceinline void set_friction_coeff(float f) { m_contact_friction = f; }
    __forceinline float get_restitution() const { return m_contact_restitution; }
    __forceinline void set_restitution(float r) { m_contact_restitution = r; }
    static __forceinline float combine_friction(float a, float b) { return phys_math::sqrt(a * b); }
    static __forceinline float combine_restitution(float a, float b) { return phys_math::sqrt(a * b); }

    phys_enum<response_kind, signed char> m_response_type;
    float m_contact_friction;
    float m_contact_restitution;
};

class phys_motion : public phys_ref_object {
public:
    PHYS_DECLARE_ALLOCATOR(PHYS_MEM_MOTION);

    enum motion_kind {
        BODY_MOTION_INVALID = 0,
        BODY_MOTION_DYNAMIC = 1,
        BODY_MOTION_SPHERE_INERTIA = 2,
        BODY_MOTION_STABLE_SPHERE_INERTIA = 3,
        BODY_MOTION_BOX_INERTIA = 4,
        BODY_MOTION_STABLE_BOX_INERTIA = 5,
        MOTION_KEYFRAMED = 6,
        MOTION_FIXED = 7,
        BODY_MOTION_THIN_BOX_INERTIA = 8,
        BODY_MOTION_CHARACTER = 9,
        BODY_MOTION_ID_MAX = 10
    };

    phys_motion(const phys_vec4 &position, const phys_quat &rotation, bool want_deactivation = false);
    __forceinline phys_motion(phys_fixup_tag) {}

    __forceinline motion_kind get_type_id() const { return m_type; }
    float get_mass() const;
    __forceinline phys_simd_scalar get_inv_mass() const { return m_inv_inertia_mass.get_simd_at(3); }
    __forceinline phys_motion_state *get_motion_state() { return &m_motion_state; }
    __forceinline const phys_motion_state *get_motion_state() const { return &m_motion_state; }
    __forceinline const phys_vec4 &get_center_of_mass_local() const { return m_motion_state.m_swept_transform.m_center_of_mass_local; }
    __forceinline const phys_vec4 &get_center_of_mass_in_world() const { return m_motion_state.m_swept_transform.m_center_of_mass1; }
    __forceinline const phys_vec4 &get_contact_position() const { return m_motion_state.m_body_transform.m_translation; }
    __forceinline const phys_quat &get_rotation() const { return m_motion_state.m_swept_transform.m_rotation_1; }
    __forceinline const phys_transform &get_transform() const { return m_motion_state.m_body_transform; }
    __forceinline const phys_vec4 &get_linear_velocity() const { return m_linear_velocity; }
    __forceinline const phys_vec4 &get_angular_velocity() const { return m_ang_velocity; }

    __forceinline void get_point_velocity(const phys_vec4 &p, phys_vec4 &point_vel_out) const {
        phys_vec4 rel_pos;
        rel_pos.set_sub4(p, get_center_of_mass_in_world());
        point_vel_out.set_cross(m_ang_velocity, rel_pos);
        point_vel_out.add4(m_linear_velocity);
    }

    __forceinline int get_inactive_frame_count(int select) { return m_inactive_frame_counts[select] & 0x7f; }
    __forceinline void bump_inactive_frames_mt(int select, int world_flag) {
        int dc = m_inactive_frame_counts[select];
        dc = dc & 0x7f;
        int new_deact_counter = dc + 1 - (dc >> 6);
        dc = new_deact_counter | (dc << 7) | (world_flag << 14);
        m_inactive_frame_counts[select] = (unsigned short)dc;
    }
    __forceinline void zero_inactive_frames_mt(int select, int world_flag) {
        int dc = m_inactive_frame_counts[select];
        dc = dc & 0x7f;
        dc = (dc << 7) | (world_flag << 14);
        m_inactive_frame_counts[select] = (unsigned short)dc;
    }
    void approx_body_transform_at(float time, phys_transform &xform_out);
    __forceinline void enable_sleeping(bool enable, int random_number) {
        if (enable) m_integrate_sleep_counter = random_number & 15;
        else {
            m_inactive_frame_counts[0] = 0;
            m_integrate_sleep_counter = 255;
            m_inactive_frame_counts[1] = 0;
        }
    }
    void set_deactivation_class(int body_sleep_class);
    void set_world_select_flags_neg(int select_flag0, int select_flag1, int world_sleep_counter);

    virtual void set_body_mass(float m);
    virtual void set_inv_mass(float inv_mass_arg);
    virtual void get_inertia_local(phys_mat3 &out_inertia) const = 0;
    virtual void get_inertia_world(phys_mat3 &out_inertia) const = 0;
    virtual void set_local_inertia(const phys_mat3 &inertia) = 0;
    virtual void set_local_inv_inertia(const phys_mat3 &inv_inertia) = 0;
    virtual void get_inv_inertia_local(phys_mat3 &out_inv_inertia) const = 0;
    virtual void get_inv_inertia_world(phys_mat3 &out_inv_inertia) const = 0;
    virtual void set_center_of_mass_in_local(const phys_vec4 &body_center_of_mass);
    virtual void contact_set_position(const phys_vec4 &position);
    virtual void set_rotation(const phys_quat &rotation);
    virtual void set_position_and_rotation(const phys_vec4 &position, const phys_quat &rotation);
    virtual void set_transform(const phys_transform &transform);
    virtual void set_linear_velocity(const phys_vec4 &body_new_lin_vel);
    virtual void set_angular_velocity(const phys_vec4 &body_new_lin_vel);
    virtual void apply_lin_impulse(const phys_vec4 &body_linear_impulse);
    virtual void apply_point_impulse(const phys_vec4 &body_linear_impulse, const phys_vec4 &p) = 0;
    virtual void apply_ang_impulse(const phys_vec4 &body_linear_impulse) = 0;
    virtual void apply_force(const float body_delta_time, const phys_vec4 &force) = 0;
    virtual void apply_force(const float body_delta_time, const phys_vec4 &force, const phys_vec4 &p) = 0;
    virtual void apply_torque(const float body_delta_time, const phys_vec4 &torque) = 0;
    virtual void get_motion_snapshot(phys_motion *motion_out);

    phys_enum<motion_kind, unsigned char> m_type;
    unsigned char m_integrate_sleep_counter;
    unsigned short m_inactive_frame_counts[2];
    phys_motion_state m_motion_state;
    phys_vec4 m_inv_inertia_mass;
    phys_vec4 m_linear_velocity;
    phys_vec4 m_ang_velocity;
    phys_vec4 m_sleep_ref_position[2];
};

class phys_max_size_motion;

class phys_keyframed_motion : public phys_motion {
public:
    phys_keyframed_motion(const phys_vec4 &position, const phys_quat &rotation);
    __forceinline phys_keyframed_motion(phys_fixup_tag flag) : phys_motion(flag) {}
    virtual ~phys_keyframed_motion();

    virtual void set_body_mass(float m);
    virtual void set_inv_mass(float inv_mass_arg);
    virtual void get_inertia_local(phys_mat3 &inertia) const;
    virtual void set_local_inertia(const phys_mat3 &inertia);
    virtual void get_inertia_world(phys_mat3 &out_inertia) const;
    virtual void set_local_inv_inertia(const phys_mat3 &inv_inertia);
    virtual void get_inv_inertia_local(phys_mat3 &inv_inertia) const;
    virtual void get_inv_inertia_world(phys_mat3 &out_inv_inertia) const;
    virtual void apply_lin_impulse(const phys_vec4 &body_linear_impulse);
    virtual void apply_point_impulse(const phys_vec4 &body_linear_impulse, const phys_vec4 &p);
    virtual void apply_ang_impulse(const phys_vec4 &body_linear_impulse);
    virtual void apply_force(const float body_delta_time, const phys_vec4 &force);
    virtual void apply_force(const float body_delta_time, const phys_vec4 &force, const phys_vec4 &p);
    virtual void apply_torque(const float body_delta_time, const phys_vec4 &torque);
    virtual void set_step_position(float position, float timestep);
    virtual void set_stored_motion(phys_max_size_motion *saved_motion);

protected:
    __forceinline phys_keyframed_motion() : phys_motion(phys_fixup_tag()), m_saved_motion(0) {}

public:
    phys_max_size_motion *m_saved_motion;
    int m_saved_quality_idx;
};

class phys_max_size_motion : public phys_keyframed_motion {
public:
    __forceinline phys_max_size_motion() : phys_keyframed_motion() {}
    __forceinline phys_max_size_motion(phys_fixup_tag flag) : phys_keyframed_motion(flag) {}
};

// Rigid motion types (Physics/Dynamics/Motion/Rigid). Their vtables are emitted by hkEntity.obj, whose
// finish-loading constructor re-creates the motion in place by type.
class phys_sphere_motion : public phys_motion {
public:
    __forceinline phys_sphere_motion(const phys_vec4 &position, const phys_quat &rotation) : phys_motion(position,
        rotation) { m_type = BODY_MOTION_SPHERE_INERTIA; }
    __forceinline phys_sphere_motion(phys_fixup_tag flag) : phys_motion(flag) {}

    virtual void get_inertia_local(phys_mat3 &out_inertia) const;
    virtual void get_inertia_world(phys_mat3 &out_inertia) const;
    virtual void set_local_inertia(const phys_mat3 &inertia);
    virtual void set_local_inv_inertia(const phys_mat3 &inv_inertia);
    virtual void get_inv_inertia_local(phys_mat3 &out_inv_inertia) const;
    virtual void get_inv_inertia_world(phys_mat3 &out_inv_inertia) const;
    virtual void apply_point_impulse(const phys_vec4 &body_linear_impulse, const phys_vec4 &p);
    virtual void apply_ang_impulse(const phys_vec4 &body_linear_impulse);
    virtual void apply_force(const float body_delta_time, const phys_vec4 &force);
    virtual void apply_force(const float body_delta_time, const phys_vec4 &force, const phys_vec4 &p);
    virtual void apply_torque(const float body_delta_time, const phys_vec4 &torque);
};

class phys_stabilized_sphere_motion : public phys_sphere_motion {
public:
    __forceinline phys_stabilized_sphere_motion(const phys_vec4 &position, const phys_quat &rotation)
        : phys_sphere_motion(position, rotation) { m_type = BODY_MOTION_STABLE_SPHERE_INERTIA; }
    __forceinline phys_stabilized_sphere_motion(phys_fixup_tag flag) : phys_sphere_motion(flag) {}
};

class phys_box_motion : public phys_motion {
public:
    phys_box_motion(const phys_vec4 &position, const phys_quat &rotation);
    __forceinline phys_box_motion(phys_fixup_tag flag) : phys_motion(flag) {}

    virtual void set_body_mass(float m);
    virtual void get_inertia_local(phys_mat3 &out_inertia) const;
    virtual void get_inertia_world(phys_mat3 &out_inertia) const;
    virtual void set_local_inertia(const phys_mat3 &inertia);
    virtual void set_local_inv_inertia(const phys_mat3 &inv_inertia);
    virtual void get_inv_inertia_local(phys_mat3 &out_inv_inertia) const;
    virtual void get_inv_inertia_world(phys_mat3 &out_inv_inertia) const;
    virtual void apply_point_impulse(const phys_vec4 &body_linear_impulse, const phys_vec4 &p);
    virtual void apply_ang_impulse(const phys_vec4 &body_linear_impulse);
    virtual void apply_force(const float body_delta_time, const phys_vec4 &force);
    virtual void apply_force(const float body_delta_time, const phys_vec4 &force, const phys_vec4 &p);
    virtual void apply_torque(const float body_delta_time, const phys_vec4 &torque);
};

class phys_stabilized_box_motion : public phys_box_motion {
public:
    phys_stabilized_box_motion(const phys_vec4 &position, const phys_quat &rotation);
    __forceinline phys_stabilized_box_motion(phys_fixup_tag flag) : phys_box_motion(flag) {}
};

class phys_thin_box_motion : public phys_box_motion {
public:
    phys_thin_box_motion(const phys_vec4 &position, const phys_quat &rotation);
    __forceinline phys_thin_box_motion(phys_fixup_tag flag) : phys_box_motion(flag) {}
};

class phys_fixed_motion : public phys_keyframed_motion {
public:
    phys_fixed_motion(const phys_vec4 &position, const phys_quat &rotation);
    __forceinline phys_fixed_motion(phys_fixup_tag flag) : phys_keyframed_motion(flag) {}

    virtual void set_step_position(float position, float timestep);
    virtual void get_position_and_velocities(phys_motion *motion_out);
};

struct phys_constraint_internal;

class phys_entity : public phys_world_object {
    friend class phys_world;
public:
    PHYS_DECLARE_ALLOCATOR(PHYS_MEM_ENTITY);

    struct collision_callback_slot {
        phys_collision_callback_util *m_callback_util;
        unsigned short m_small_capacity;
        unsigned char m_event_filter;
        unsigned char m_user_filter;
    };

    phys_entity(phys_fixup_tag flag);
    virtual ~phys_entity();

    __forceinline phys_material &get_body_material() { return m_body_material; }
    __forceinline const phys_material &get_body_material() const { return m_body_material; }
    __forceinline phys_motion *get_motion() { return &m_motion; }
    __forceinline const phys_motion *get_motion() const { return &m_motion; }
    __forceinline phys_island *get_island() const { return m_owner_island; }
    __forceinline unsigned int get_uid() const { return m_entity_uid; }

    void activate();
    void deactivate();
    void activate_as_critical_op();
    void deactivate_critical();
    phys_bool is_body_active() const;
    __forceinline phys_bool body_is_fixed() const { return m_motion.get_type_id() == phys_motion::MOTION_FIXED; }
    __forceinline phys_bool is_fixed_or_keyframed() const {
        return m_motion.m_type == phys_motion::MOTION_FIXED || m_motion.m_type == phys_motion::MOTION_KEYFRAMED;
    }

    void add_entity_listener(phys_entity_listener *el);
    void remove_entity_listener(phys_entity_listener *el);
    void add_entity_activation_listener(phys_entity_activation_listener *el);
    void remove_activation_listener(phys_entity_activation_listener *el);
    void add_collision_listener(phys_collision_listener *cl);
    void remove_collision_listener(phys_collision_listener *cl);
    phys_dynamics_contact_manager *find_contact_manager_to(const phys_entity *entity);
    // March 0x82ba8e38 / 0x834ce0c0: direct field access, no locals.
    __forceinline phys_entity_deactivator *get_deactivator() { return m_body_deactivator; }
    __forceinline const phys_entity_deactivator *get_deactivator() const { return m_body_deactivator; }

    int get_chain_constraint_count() const;
    __forceinline const phys_small_array<phys_constraint_internal> &get_constraint_masters() const { return m_master_constraints; }
    phys_constraint *get_entity_constraint(int i);
    const phys_constraint *get_entity_constraint(int i) const;

    virtual void gather_memory_stats(phys_statistics_collector *collector) const;
    virtual void free_internal_arrays();
    virtual phys_motion_state *get_motion_state() { return 0; }

protected:
    phys_entity(const phys_shape *shape);
    const phys_small_array<phys_constraint_internal> &get_constraint_masters_impl() const;
    phys_small_array<phys_constraint_internal> &get_constraint_masters_rw_impl();
    const phys_array<phys_constraint *> &get_constraint_slaves_impl() const;
    void set_deactivator(phys_entity_deactivator *deactivator);

public:
    phys_material m_body_material;
    unsigned int m_solver_data;
    unsigned short m_storage_idx;
    unsigned short m_contact_callback_delay;
    phys_small_array<phys_constraint_internal> m_master_constraints;
    phys_array<phys_constraint *> m_slave_constraints;
    phys_array<unsigned char> m_constraint_runtime;
    phys_entity_deactivator *m_body_deactivator;
    phys_island *m_owner_island;
    signed char m_auto_remove_level;
    unsigned int m_entity_uid;
    phys_max_size_motion m_motion;
    phys_small_array<phys_collision_listener *> m_collision_listeners;
    mutable phys_small_array<phys_entity_activation_listener *> m_activation_listeners;
    mutable phys_small_array<phys_entity_listener *> m_entity_listeners;
    phys_small_array<phys_action *> m_actions;
    collision_callback_slot m_collision_callback;
};

// The reflection record defined beside the class in hkEntity.cpp.
extern const phys_type_desc phys_entity_type_desc;

__forceinline phys_entity *phys_get_entity(const phys_collider *collidable) {
    if (collidable->get_type_id() == phys_world_object::BROADPHASE_KIND_ENTITY) {
        return reinterpret_cast<phys_entity *>(phys_get_world_object(collidable));
    }
    return 0;
}

enum phys_solver_sleep {
    SOLVER_SLEEP_INVALID = 0,
    SOLVER_SLEEP_OFF = 1,
    SOLVER_SLEEP_LOW = 2,
    SOLVER_DEACTIVATION_MEDIUM = 3,
    SOLVER_SLEEP_HIGH = 4,
    SOLVER_DEACTIVATION_MAX = 5
};

// Deactivators: their virtual interface, in slot order (the high- and low-frequency tests, then the type).
class phys_entity_deactivator : public phys_ref_object {
public:
    virtual phys_bool should_sleep_high_freq(const phys_entity *entity) = 0;
    virtual phys_bool should_sleep_low_freq(const phys_entity *entity) = 0;
};

class phys_rigid_body_deactivator : public phys_entity_deactivator {
public:
    enum sleep_detector_kind {
        sleep_policy_invalid = 0,
        sleep_policy_never = 1,
        sleep_policy_spatial = 2,
        k_deactivation_count = 3
    };

    phys_rigid_body_deactivator() {}
    phys_rigid_body_deactivator(phys_fixup_tag flag) {}
    virtual sleep_detector_kind get_body_deactivator_type() const = 0;
};

class phys_rigid_body_desc {
public:
    PHYS_DECLARE_NONVIRTUAL_ALLOCATOR(PHYS_MEM_ENTITY, phys_rigid_body_desc);

    phys_rigid_body_desc();

    unsigned int m_collision_filter_info;
    const phys_shape *m_proxy_shape;
    phys_enum<phys_material::response_kind, signed char> m_collision_response;
    unsigned short m_contact_callback_delay;
    phys_vec4 m_position;
    phys_quat m_rotation;
    phys_vec4 m_linear_velocity;
    phys_vec4 m_ang_velocity;
    phys_mat3 m_inertia_tensor;
    phys_vec4 m_center_of_mass;
    float m_body_mass;
    float m_linear_damping;
    float m_ang_damping;
    float m_contact_friction;
    float m_contact_restitution;
    float m_max_lin_vel;
    float m_max_ang_vel;
    float m_allowed_penetration_depth;
    phys_enum<phys_motion::motion_kind, signed char> m_motion_type;
    phys_enum<phys_rigid_body_deactivator::sleep_detector_kind, signed char> m_deactivator_kind;
    phys_enum<phys_solver_sleep, signed char> m_solver_sleep;
    phys_enum<phys_collider_quality, signed char> m_collider_quality;
    signed char m_auto_remove_level;
    signed char m_contact_user_data_count;
};

enum phys_entity_activation {
    PHYS_ENTITY_ACTIVATION_DO_NOT_ACTIVATE = 0,
    PHYS_ENTITY_ACTIVATION_DO_ACTIVATE = 1
};

enum phys_entity_filter_update_mode {
    PHYS_UPDATE_FILTER_ON_ENTITY_FULL_CHECK = 0,
    PHYS_UPDATE_FILTER_ON_ENTITY_DISABLE_ENTITY_ENTITY_COLLISIONS_ONLY = 1
};

class phys_rigid_body : public phys_entity {
public:
    phys_rigid_body(const phys_rigid_body_desc &info);
    __forceinline phys_rigid_body(phys_fixup_tag flag) : phys_entity(flag) {}
    virtual ~phys_rigid_body();

    void get_desc(phys_rigid_body_desc &info) const;
    void set_deactivator(phys_rigid_body_deactivator *deactivator);
    __forceinline void set_deactivator_inline(phys_rigid_body_deactivator *deactivator) { phys_entity::set_deactivator(deactivator); }
    void set_deactivator(phys_rigid_body_deactivator::sleep_detector_kind type);
    phys_rigid_body_deactivator::sleep_detector_kind get_deactivator_type();
    phys_motion *get_stored_dynamic_motion();
    const phys_motion *get_stored_dynamic_motion() const;
    void check_performance() const;

    __forceinline float get_mass() const { return m_motion.get_mass(); }
    __forceinline float get_inv_mass() const { return m_motion.get_inv_mass(); }
    void set_body_mass(float m);
    void set_inv_mass(float inv_mass_arg);

    // The concrete motion is constructed in place. Call through phys_motion so Clang uses its vtable.
#if !defined(_XBOX) || defined(__clang__)
    __forceinline void get_inertia_local(phys_mat3 &out_inertia) const {
        get_rigid_motion()->get_inertia_local(out_inertia);
    }
    __forceinline void get_inertia_world(phys_mat3 &out_inertia) const {
        get_rigid_motion()->get_inertia_world(out_inertia);
    }
    __forceinline void get_inv_inertia_local(phys_mat3 &inv_inertia) const {
        get_rigid_motion()->get_inv_inertia_local(inv_inertia);
    }
    __forceinline void get_inv_inertia_world(phys_mat3 &out_inv_inertia) const {
        get_rigid_motion()->get_inv_inertia_world(out_inv_inertia);
    }
#else
    __forceinline void get_inertia_local(phys_mat3 &out_inertia) const { m_motion.get_inertia_local(out_inertia); }
    __forceinline void get_inertia_world(phys_mat3 &out_inertia) const { m_motion.get_inertia_world(out_inertia); }
    __forceinline void get_inv_inertia_local(phys_mat3 &inv_inertia) const { m_motion.get_inv_inertia_local(inv_inertia); }
    __forceinline void get_inv_inertia_world(phys_mat3 &out_inv_inertia) const { m_motion.get_inv_inertia_world(out_inv_inertia); }
#endif
    void set_local_inertia(const phys_mat3 &inertia);
    void set_local_inv_inertia(const phys_mat3 &inv_inertia);

    void set_center_of_mass_local(const phys_vec4 &body_center_of_mass);
    __forceinline const phys_vec4 &get_center_of_mass_local() const { return m_motion.get_center_of_mass_local(); }
    __forceinline const phys_vec4 &get_center_of_mass_in_world() const { return m_motion.get_center_of_mass_in_world(); }

    __forceinline const phys_vec4 &get_contact_position() const { return m_motion.get_contact_position(); }
    void contact_set_position(const phys_vec4 &position);
    __forceinline const phys_quat &get_rotation() const { return m_motion.get_rotation(); }
    void set_rotation(const phys_quat &rotation);
    void set_position_and_rotation(const phys_vec4 &position, const phys_quat &rotation);
    void set_position_and_rotation_critical(const phys_vec4 &position, const phys_quat &rotation);
    __forceinline const phys_transform &get_transform() const { return m_motion.get_transform(); }
    void set_transform(const phys_transform &transform);

    __forceinline const phys_vec4 &get_linear_velocity() const { return m_motion.get_linear_velocity(); }
    void set_linear_velocity(const phys_vec4 &body_new_lin_vel);
    void set_linear_velocity_critical(const phys_vec4 &body_new_lin_vel);
    __forceinline const phys_vec4 &get_angular_velocity() const { return m_motion.get_angular_velocity(); }
    void set_angular_velocity(const phys_vec4 &body_new_lin_vel);
    void set_angular_velocity_as_critical_op(const phys_vec4 &body_new_lin_vel);
    __forceinline void get_point_velocity(const phys_vec4 &p, phys_vec4 &point_vel_out) const { m_motion.get_point_velocity(p, point_vel_out); }

    void apply_lin_impulse(const phys_vec4 &body_linear_impulse);
    void apply_lin_impulse_critical(const phys_vec4 &body_linear_impulse);
    __forceinline void apply_point_impulse(const phys_vec4 &body_linear_impulse, const phys_vec4 &p) {
        activate();
#if !defined(_XBOX) || defined(__clang__)
        get_rigid_motion()->apply_point_impulse(body_linear_impulse, p);
#else
        m_motion.apply_point_impulse(body_linear_impulse, p);
#endif
    }
    void apply_point_impulse_critical(const phys_vec4 &body_linear_impulse, const phys_vec4 &p);
    void apply_ang_impulse(const phys_vec4 &body_linear_impulse);
    void apply_ang_impulse_critical(const phys_vec4 &body_linear_impulse);
    __forceinline void apply_force(const float body_delta_time, const phys_vec4 &force) {
        activate();
#if !defined(_XBOX) || defined(__clang__)
        get_rigid_motion()->apply_force(body_delta_time, force);
#else
        m_motion.apply_force(body_delta_time, force);
#endif
    }
    __forceinline void apply_force(const float body_delta_time, const phys_vec4 &force, const phys_vec4 &p) {
        activate();
#if !defined(_XBOX) || defined(__clang__)
        get_rigid_motion()->apply_force(body_delta_time, force, p);
#else
        m_motion.apply_force(body_delta_time, force, p);
#endif
    }
    __forceinline void apply_torque(const float body_delta_time, const phys_vec4 &torque) {
        activate();
#if !defined(_XBOX) || defined(__clang__)
        get_rigid_motion()->apply_torque(body_delta_time, torque);
#else
        m_motion.apply_torque(body_delta_time, torque);
#endif
    }

    __forceinline phys_motion *get_rigid_motion() const { return const_cast<phys_max_size_motion *>(&m_motion); }
    __forceinline phys_motion::motion_kind get_motion_type() const { return m_motion.get_type_id(); }
    void set_motion_type(phys_motion::motion_kind new_motion_kind, phys_entity_activation body_activation_state = PHYS_ENTITY_ACTIVATION_DO_NOT_ACTIVATE,
        phys_entity_filter_update_mode filter_update_mode = PHYS_UPDATE_FILTER_ON_ENTITY_FULL_CHECK);

    __forceinline float get_friction() const { return m_body_material.get_friction(); }
    __forceinline float get_restitution() const { return m_body_material.get_restitution(); }
    void set_friction_coeff(float new_friction);
    void set_restitution(float new_restitution);

    __forceinline phys_collider_quality get_quality_type() const { return get_collider()->get_quality_type(); }
    __forceinline void set_quality_type(phys_collider_quality type) { get_collider_rw()->set_quality_type(type); }
    __forceinline bool body_is_fixed() const { return m_motion.get_type_id() == phys_motion::MOTION_FIXED; }
    __forceinline bool body_is_keyframed() const { return m_motion.get_type_id() == phys_motion::MOTION_KEYFRAMED; }

    void update_cached_shape_info(const phys_shape *shape, phys_vec4 &extent_out);
    static void refresh_warped_body(phys_entity *entity);
    static void create_dynamic_motion(phys_motion::motion_kind body_motion_type, const phys_vec4 &position, const phys_quat &rotation, float mass,
        const phys_mat3 &local_inertia, const phys_vec4 &local_centre_of_mass, float max_lin_vel, float max_ang_vel, phys_max_size_motion *motion_out);

    virtual phys_world_op::Result set_proxy_shape(const phys_shape *shape);
    virtual phys_rigid_body *clone() const;

protected:
    // Defined by hkRigidBody.cpp; March 0x834cbaf8 returns the rigid motion's state.
    virtual phys_motion_state *get_motion_state();
};

__forceinline phys_rigid_body *phys_get_rigid_body(const phys_collider *collidable) {
    return static_cast<phys_rigid_body *>(phys_get_entity(collidable));
}

enum phys_volume_type {
    PHYS_VOLUME_AABB = 0,
    PHYS_VOLUME_SIMPLE_SHAPE = 1,
    PHYS_VOLUME_CACHING_SHAPE = 2,
    PHYS_VOLUME_USER0 = 3,
    PHYS_VOLUME_MAX_ID = 13
};

class phys_volume : public phys_world_object {
    friend class phys_world;
public:
    PHYS_DECLARE_ALLOCATOR(PHYS_MEM_VOLUME);
    inline phys_volume(const phys_shape *shape) : phys_world_object(shape, BROADPHASE_KIND_VOLUME) { m_collider.set_collider_owner(this); }
    inline phys_volume(phys_fixup_tag flag) : phys_world_object(flag) {}
    virtual ~phys_volume();

    virtual phys_volume_type get_type_id() const = 0;
    virtual void compute_bounds(phys_aabb &aabb) = 0;
    virtual void insert_overlapping_collider(phys_collider *collidable) = 0;
    virtual phys_bool has_overlapping_collider(phys_collider *collidable) = 0;
    virtual void erase_overlapping_collider(phys_collider *collidable) = 0;
    virtual phys_volume *clone() const = 0;
    virtual void update_compound_filter();
    virtual void free_internal_arrays();
    virtual void gather_memory_stats(phys_statistics_collector *collector) const;

    void update_broadphase(const phys_aabb &aabb);
    void set_bounding_volume_data(const phys_aabb &aabb);
    void add_volume_overlap_listener(class phys_volume_overlap_listener *el);
    void remove_volume_overlap_listener(class phys_volume_overlap_listener *el);
    void add_volume_listener(phys_volume_listener *el);
    void remove_volume_listener(phys_volume_listener *el);

    void notify_volume_added();
    void notify_volume_removed();
    void notify_volume_shape_set();

protected:
    void notify_volume_deleted();
    enum phys_collider_accept notify_collider_added(const phys_collider *collidable);
    void notify_collider_removed(const phys_collider *collidable, phys_bool collider_was_added);
    virtual phys_motion_state *get_motion_state() { return 0; }

    phys_array<class phys_volume_overlap_listener *> m_overlap_listeners;
    phys_array<phys_volume_listener *> m_volume_listeners;
};

__forceinline phys_volume *phys_get_volume(const phys_collider *collidable) {
    if (collidable->get_type_id() == phys_world_object::BROADPHASE_KIND_VOLUME) {
        return reinterpret_cast<phys_volume *>(phys_get_world_object(collidable));
    }
    return 0;
}

class phys_box_volume : public phys_volume {
public:
    phys_box_volume(const phys_aabb &aabb, unsigned int collision_filter_info = 0);
    inline phys_box_volume(phys_fixup_tag flag) : phys_volume(flag) {}
    virtual ~phys_box_volume();

    __forceinline phys_array<phys_collider *> &get_overlapping_colliders() { return m_overlapping_colliders; }
    __forceinline const phys_aabb &get_handle_aabb() const { return m_volume_aabb; }
    void set_aabb(const phys_aabb &new_aabb);
    void trace_ray(const phys_world_ray_input &input, phys_ray_hit_collector &collector) const;
    void trace_ray(const phys_world_ray_input &input, phys_world_ray_output &output) const;
    void sweep_cast(const phys_collider * const to_be_cast, const phys_linear_cast_input &input,
        phys_hit_collector &cast_hit_collector, phys_hit_collector &start_collector) const;

    virtual phys_volume_type get_type_id() const;
    virtual void compute_bounds(phys_aabb &aabb);
    virtual void insert_overlapping_collider(phys_collider *collidable);
    virtual phys_bool has_overlapping_collider(phys_collider *collidable);
    virtual void erase_overlapping_collider(phys_collider *collidable);
    virtual phys_volume *clone() const;
    virtual void gather_memory_stats(phys_statistics_collector *collector) const;
    virtual void free_internal_arrays();

protected:
    phys_aabb m_volume_aabb;
    phys_array<phys_collider *> m_overlapping_colliders;
    phys_bool m_order_dirty;
};

// The reflection record defined beside the class in hkAabbPhantom.cpp.
extern const phys_type_desc phys_box_volume_type_desc;

class phys_shape_volume : public phys_volume {
public:
    phys_shape_volume(const phys_shape *shape, const phys_transform &transform);
    inline phys_shape_volume(phys_fixup_tag flag) : phys_volume(flag) {}

    void contact_set_position(const phys_vec4 &position, float extra_tolerance = 0.0f);
    virtual void set_position_and_sweep_cast(const phys_vec4 &position, const phys_linear_cast_input &input,
        phys_hit_collector &cast_hit_collector, phys_hit_collector *start_point_collector) = 0;
    void set_transform_and_sweep_cast(const phys_transform &transform, const phys_linear_cast_input &input,
        phys_hit_collector &cast_hit_collector, phys_hit_collector *start_point_collector);
    __forceinline const phys_transform &get_transform() const { return m_motion_state.get_transform(); }
    void set_transform(const phys_transform &transform);

    virtual void get_closest_points(phys_hit_collector &collector) = 0;
    virtual void get_penetrations(phys_proxy_pair_collector &collector) = 0;
    virtual phys_world_op::Result set_proxy_shape(const phys_shape *shape);
    virtual void compute_bounds(phys_aabb &aabb);
    virtual void free_internal_arrays();

protected:
    virtual phys_motion_state *get_motion_state();

public:

    phys_motion_state m_motion_state;
};

class phys_simple_shape_volume : public phys_shape_volume {
public:
    struct collision_detail {
        phys_collider *m_collider;
    };

    phys_simple_shape_volume(const phys_shape *shape, const phys_transform &transform, unsigned int m_collision_filter_info = 0);
    inline phys_simple_shape_volume(phys_fixup_tag flag) : phys_shape_volume(flag) {}
    virtual ~phys_simple_shape_volume();

    __forceinline phys_array<collision_detail> &get_collision_details() { return m_collision_details; }

    virtual phys_volume_type get_type_id() const;
    virtual void set_position_and_sweep_cast(const phys_vec4 &position, const phys_linear_cast_input &input,
        phys_hit_collector &cast_hit_collector, phys_hit_collector *start_point_collector);
    void get_closest_points(phys_hit_collector &collector, const struct phys_collision_input *input);
    void get_penetrations(phys_proxy_pair_collector &collector, const struct phys_collision_input *input);
    virtual void get_closest_points(phys_hit_collector &collector);
    virtual void get_penetrations(phys_proxy_pair_collector &collector);
    virtual phys_volume *clone() const;
    virtual void insert_overlapping_collider(phys_collider *collidable);
    virtual phys_bool has_overlapping_collider(phys_collider *collidable);
    virtual void erase_overlapping_collider(phys_collider *collidable);
    virtual void gather_memory_stats(phys_statistics_collector *collector) const;
    virtual void free_internal_arrays();

protected:
    phys_array<collision_detail> m_collision_details;
    phys_bool m_order_dirty;
};

// The reflection record defined beside the class in hkSimpleShapePhantom.cpp.
extern const phys_type_desc phys_simple_shape_volume_type_desc;

class phys_world_desc : public phys_ref_object {
public:
    enum solver_kind {
        SOLVER_PRESET_INVALID = 0,
        SOLVER_PRESET_2ITER_SOFT = 1,
        SOLVER_PRESET_2ITER_MEDIUM = 2,
        SOLVER_PRESET_2ITER_HARD = 3,
        SOLVER_PRESET_4ITER_SOFT = 4,
        SOLVER_PRESET_4ITER_MEDIUM = 5,
        SOLVER_PRESET_4ITER_HARD = 6,
        SOLVER_PRESET_8ITER_SOFT = 7,
        SOLVER_PRESET_8ITER_MEDIUM = 8,
        SOLVER_PRESET_8ITER_HARD = 9,
        SOLVER_PRESET_ID_MAX = 10
    };
    enum simulation_kind {
        SIM_KIND_INVALID = 0,
        SIM_KIND_DISCRETE = 1,
        SIMULATION_TYPE_CONTINUOUS = 2,
        SIM_KIND_MULTITHREADED = 3
    };
    enum contact_point_mode {
        CONTACT_ACCEPT_ALL = 0,
        CONTACT_REJECT_DUBIOUS = 1,
        CONTACT_POINT_REJECT_MANY = 2
    };
    enum broadphase_border_mode {
        BROADPHASE_BORDER_ASSERT = 0,
        BORDER_CLAMP_ENTITY = 1,
        BORDER_EVICT_ENTITY = 2,
        BROADPHASE_BORDER_DO_NOTHING = 3
    };


    phys_world_desc();
    phys_world_desc(phys_fixup_tag flag);
    virtual __forceinline ~phys_world_desc() {}
    void setup_solver_info(solver_kind st);
    void set_broadphase_world_size(float size);

    phys_vec4 m_gravity;
    int m_broadphase_query_size;
    float m_contact_resting_vel;
    phys_enum<broadphase_border_mode, signed char> m_border_behaviour;
    phys_aabb m_broadphase_bounds;
    float m_collision_tolerance;
    phys_collision_filter *m_collision_filter;
    phys_convex_list_filter *m_convex_list_filter;
    float m_expected_max_lin_vel;
    int m_toi_event_queue_size;
    float m_expected_min_psi_dt;
    phys_world_memory_watchdog *m_memory_watchdog;
    int m_broadphase_marker_count;
    phys_enum<contact_point_mode, signed char> m_contact_point_gen;
    float m_solver_tau;
    float m_solver_damp;
    int m_solver_iterations;
    int m_solver_microsteps;
    phys_bool m_coherent_solver_order;
    float m_convex_edge_snap_threshold;
    float m_concave_edge_snap_threshold;
    phys_bool m_toi_weld_reject;
    phys_bool m_legacy_edge_weld;
    float m_iter_cast_early_out_dist;
    int m_iter_cast_max_iters;
    float m_hf_sleep_period;
    float m_low_freq_sleep_period;
    unsigned char m_inactive_frame_select0;
    unsigned char m_inactive_frame_select1;
    unsigned char m_integrate_sleep_counter;
    phys_bool m_wake_on_body_move;
    phys_bool m_want_legacy_sleep;
    float m_sleep_ref_distance;
    float m_toi_response_rotate_normal;
    phys_bool m_sleep_enabled;
    phys_enum<simulation_kind, signed char> m_simulation_type;
    phys_bool m_islands_enabled;
    unsigned int m_min_island_size;
    phys_bool m_single_thread_actions;
    float m_frame_marker_psi_snap;
};


struct phys_solver_info {
    phys_solver_info &operator=(const phys_solver_info &other);
    inline void set_tau_and_damping(float tau, float damping)
    {
        m_query_tau = tau;
        m_friction_tau = 0.5f * tau;
        m_solver_damping = damping;
        m_damp_div_tau = damping / tau;
        m_damp_div_friction_tau = damping / m_friction_tau;
        m_tau_div_damp = tau / damping;
        m_friction_tau_div_damp = m_friction_tau / damping;
        m_integrate_vel_factor.set_all3(m_tau_div_damp);
        m_inv_integrate_vel_factor.set_all3(m_damp_div_tau);
    }
    void bump_sleep_flags();
    enum sleep_class {
        SLEEP_CLASS_INVALID = 0,
        SLEEP_CLASS_OFF = 1,
        SLEEP_CLASS_LOW = 2,
        SLEEP_CLASS_MEDIUM = 3,
        SLEEP_CLASS_HIGH = 4,
        SLEEP_CLASS_AGGRESSIVE = 5,
        SLEEP_CLASS_COUNT = 6
    };
    struct sleep_info {
        float m_inv_lin_vel_threshold;
        float m_ang_vel_threshold_inv;
        float m_slow_object_vel_mul;
        float m_rel_sleep_vel_threshold;
        unsigned short m_steps_to_sleep;
        float m_max_dist_sq[2];
        phys_half m_max_rot_sq[2];
    };

    __declspec(align(16)) float m_one_const;
    float m_query_tau;
    float m_solver_damping;
    float m_friction_tau;
    phys_vec4 m_gravity_per_substep;
    phys_vec4 m_gravity_per_step;
    phys_vec4 m_integrate_vel_factor;
    phys_vec4 m_inv_integrate_vel_factor;
    float m_damp_div_tau;
    float m_tau_div_damp;
    float m_damp_div_friction_tau;
    float m_friction_tau_div_damp;
    float m_contact_resting_vel;
    sleep_info m_deactivation_info[6];
    float m_delta_time;
    float m_inv_dt;
    int m_step_count;
    int m_micro_step_count;
    float m_inv_micro_steps;
    float m_inv_steps;
    phys_bool m_coherent_solver_order;
    unsigned char m_inactive_frame_select[2];
    unsigned char m_integrate_sleep_counter;
};

struct phys_world_dynamics_step_info {
    phys_step_info m_step_info;
    phys_solver_info m_solver_info;
};

typedef char phys_solver_info_sleep_size[(sizeof(phys_solver_info::sleep_info) == 0x20) ? 1 : -1];
typedef char phys_solver_info_rotation_threshold_offset[(offsetof(phys_solver_info::sleep_info, m_max_rot_sq) == 0x1c) ? 1 : -1];
typedef char phys_solver_info_size[(sizeof(phys_solver_info) == 0x140) ? 1 : -1];

enum phys_world_filter_update_mode {
    PHYS_UPDATE_FILTER_ON_WORLD_FULL_CHECK = 0,
    PHYS_UPDATE_FILTER_ON_WORLD_DISABLE_ENTITY_ENTITY_COLLISIONS_ONLY = 1
};

enum phys_compound_filter_update_mode {
    PHYS_UPDATE_FILTER_IGNORE_COMPOUND_SHAPES = 0,
    PHYS_UPDATE_FILTER_PROCESS_COMPOUND_SHAPES = 1
};

enum phys_step_result;
enum phys_thread_token;

class phys_world : public phys_ref_object {
public:
    static __forceinline void *operator new(size_t size) throw();
    struct property_lock {
        unsigned int m_key;
        mutable phys_multi_thread_lock m_mt_lock;
    };

  void get_penetrations(const phys_collider *collidable, const phys_collision_input &input, phys_proxy_pair_collector &collector) const;
  void find_initial_contacts(phys_entity **entities, int solver_entity_count);
  void find_all_initial_contacts();
  void add_action_listener(phys_action_listener *listener);
  void add_constraint_listener(phys_constraint_listener *listener);
  void add_entity_listener(phys_entity_listener *listener);
  void add_volume_listener(phys_volume_listener *listener);
  void add_island_activation_listener(phys_island_activation_listener *listener);
  void add_post_collide_listener(phys_post_collide_listener *listener);
  void add_post_simulation_listener(phys_post_simulation_listener *listener);
  void add_post_integrate_listener(phys_post_integrate_listener *listener);
  void add_island_post_collide_listener(phys_island_post_collide_listener *listener);
  void add_island_post_integrate_listener(phys_island_post_integrate_listener *listener);
  void add_collision_listener(phys_collision_listener *listener);
  void remove_collision_listener(phys_collision_listener *listener);
  void remove_island_post_integrate_listener(phys_island_post_integrate_listener *listener);
  void remove_island_post_collide_listener(phys_island_post_collide_listener *listener);
  void remove_world_post_integrate_listener(phys_post_integrate_listener *listener);
  void remove_world_post_simulation_listener(phys_post_simulation_listener *listener);
  void remove_world_post_collide_listener(phys_post_collide_listener *listener);
  void remove_island_activation_listener(phys_island_activation_listener *listener);
  void remove_volume_listener(phys_volume_listener *listener);
  void remove_entity_listener(phys_entity_listener *listener);
  void remove_constraint_listener(phys_constraint_listener *listener);
  void remove_action_listener(phys_action_listener *listener);
  void set_high_freq_sleep_period(float period);
  void set_low_freq_sleep_period(float period);
  phys_step_result integrate_world(float body_delta_time);
  phys_step_result collide();
  void get_multithread_config(phys_multithread_config &config);
  void set_multithread_config(const phys_multithread_config &config);
  void check_access_active_islands() const;
  phys_step_result step_delta_time(float body_delta_time);
  void set_memory_watchdog(phys_world_memory_watchdog *memory_watchdog);
  void set_broadphase_border(phys_broadphase_border *border);
  void add_physics_system(const phys_physics_system *system);
  void remove_physics_system(const phys_physics_system *system);
  void attach_action_to_entity(phys_action *action, phys_entity *entity);
  void get_world_as_systems(phys_array<phys_physics_system *> &systems) const;
  void detach_entity_action(phys_action *action, phys_entity *entity);
  static __forceinline void operator delete(void *p) {
      phys_ref_object *b = static_cast<phys_ref_object *>(p);
      phys_thread_memory::get_singleton_instance().free_chunk(p, b->m_mem_size_and_flags, PHYS_MEM_WORLD);
  }
    __forceinline void allow_critical_ops(phys_bool allow) { m_critical_ops_allowed = allow; }
    __forceinline void block_pending_ops(phys_bool block) { m_block_pending_ops = block; }
    phys_physics_system *get_world_as_one_system() const;
    phys_job_queue *get_job_queue();
    phys_world_memory_watchdog *get_memory_watchdog() const;
    phys_broadphase_border *get_broadphase_border() const;
    phys_world_op::user_callback *queue_callback(phys_world_op::user_callback *callback, void *action_user_data);
    phys_constraint *create_and_add_constraint(phys_rigid_body *constraint_first_body, phys_rigid_body *second_body, phys_constraint_data *data);

    phys_world(const phys_world_desc &info, unsigned int sdk_version = 0x04510000);
    virtual ~phys_world();

    void get_desc(phys_world_desc &info) const;
    void activate_region(const phys_aabb &aabb);
    phys_step_result advance_time();
    void set_frame_time_marker(float frame_delta_time);
    bool sim_at_marker() const;
    bool sim_at_psi() const;
    void step_begin_st(float physics_delta_time);
    void step_process_mt(const phys_thread_token &token);
    void step_end_st();
    phys_thread_token get_thread_token();
    void reset_thread_tokens();
    void check_determinism();
    void add_world_delete_listener(phys_world_deletion_listener *listener);
    void remove_world_deletion_listener(phys_world_deletion_listener *listener);
    __forceinline float get_high_freq_sleep_period() const { return m_hf_sleep_period; }
    __forceinline float get_low_freq_sleep_period() const { return m_low_freq_sleep_period; }

    phys_entity *add_chain_entity(phys_entity *entity, phys_entity_activation initial_activation = PHYS_ENTITY_ACTIVATION_DO_ACTIVATE);
    phys_bool remove_entity(phys_entity *entity);
    void add_entity_batch(phys_entity *const *entity_batch, int solver_entity_count,
        phys_entity_activation initial_activation = PHYS_ENTITY_ACTIVATION_DO_ACTIVATE);
    void remove_entity_batch(phys_entity *const *entity_batch, int solver_entity_count);

    phys_action *add_world_action(phys_action *action);
    void remove_action(phys_action *action);
    void remove_action_now(phys_action *action);

    phys_constraint *add_world_constraint(phys_constraint *constraint);
    phys_bool remove_constraint(phys_constraint *constraint);

    phys_volume *add_volume(phys_volume *phantom);
    void add_volume_batch(phys_volume *const *volume_batch, int world_volume_count);
    void remove_volume(phys_volume *phantom);
    void remove_volume_batch(phys_volume *const *volume_batch, int world_volume_count);

    void refilter_entity(phys_entity *entity, phys_entity_filter_update_mode entity_filter_update_mode,
        phys_compound_filter_update_mode update_compound_filter);
    void reintegrate_recollide(phys_entity **entity_batch, int solver_entity_count, unsigned char reintegrate_mode = 3);
    void refilter_volume(phys_volume *phantom, phys_compound_filter_update_mode update_compound_filter);
    void refilter_world(phys_world_filter_update_mode entity_filter_update_mode, phys_compound_filter_update_mode update_compound_filter);

    void set_collision_filter(phys_collision_filter *filter, phys_bool update_filter_on_world = true,
        phys_world_filter_update_mode broadphase_check_mode = PHYS_UPDATE_FILTER_ON_WORLD_FULL_CHECK,
        phys_compound_filter_update_mode update_compound_filter = PHYS_UPDATE_FILTER_PROCESS_COMPOUND_SHAPES);
    __forceinline const phys_collision_filter *get_collision_filter() const { return m_collision_filter; }
    __forceinline phys_dispatcher *get_dispatcher() const { return m_collision_dispatcher; }
    __forceinline const phys_process_collision_input *get_collision_input() const { return m_collision_input; }
    __forceinline phys_process_collision_input *get_collision_input() { return m_collision_input; }
    __forceinline phys_process_collision_input *get_collision_input_rw() { return m_collision_input; }

    __forceinline phys_broadphase *get_broadphase() { return m_broadphase; }
    __forceinline const phys_broadphase *get_broadphase() const { return m_broadphase; }
    __forceinline phys_rigid_body *get_fixed_rigid_body() { return m_fixed_body; }
    __forceinline const phys_rigid_body *get_fixed_rigid_body() const { return m_fixed_body; }
    __forceinline const phys_island *get_fixed_island() const { return m_fixed_island; }
    __forceinline const phys_array<phys_island *> &get_active_islands() const { return m_active_islands; }
    __forceinline const phys_array<phys_island *> &get_inactive_islands() const { return m_inactive_islands; }
    __forceinline const phys_array<phys_volume *> &get_volumes() const { return m_border_volumes; }
    __forceinline const phys_vec4 &get_world_gravity() const { return m_gravity; }
    void set_world_gravity(const phys_vec4 &gravity);
    int get_integration_mem_usage();

    void trace_ray(const phys_world_ray_input &input, phys_world_ray_output &output) const;
    void trace_ray(const phys_world_ray_input &input, phys_ray_hit_collector &collector) const;
    void sweep_cast(const phys_collider *collider_a, const phys_linear_cast_input &input, phys_hit_collector &cast_hit_collector,
        phys_hit_collector *start_collector = 0) const;
    void get_closest_points(const phys_collider *collider_a, const struct phys_collision_input &input, phys_hit_collector &collector) const;

    void lock();
    void lock_read_only();
    void unlock();
    void unlock_read_only();
    bool is_locked();

    __forceinline void lock_critical_ops() { ++m_critical_ops_lock_count; }
    __forceinline void unlock_critical_ops() { --m_critical_ops_lock_count; }
    __forceinline void try_execute_pending_ops() {
        if (m_critical_ops_lock_count == 0 && m_pending_op_count && !m_block_pending_ops) {
            execute_pending_ops();
        }
    }
    __forceinline void unlock_and_flush_ops() {
        unlock_critical_ops();
        try_execute_pending_ops();
    }
    __forceinline int critical_ops_locked() const { return m_critical_ops_lock_count; }
    __forceinline int critical_ops_locked_unchecked() const { return m_critical_ops_lock_count; }
    __forceinline int critical_ops_locked_volumes() const { return m_critical_ops_lock_count + m_critical_ops_volume_locks; }
    void queue_world_op(const phys_world_op::base_op &operation);
    void lock_island_for_constraints(phys_island *island);
    void unlock_island_constraints(phys_island *island);

    virtual void gather_memory_stats(phys_statistics_collector *collector) const;

private:
    void execute_pending_ops();

public:

    phys_simulation *m_world_simulation;
    phys_vec4 m_gravity;
    phys_island *m_fixed_island;
    phys_rigid_body *m_fixed_body;
    phys_array<phys_island *> m_active_islands;
    phys_array<phys_island *> m_inactive_islands;
    phys_array<phys_island *> m_dirty_islands;
    phys_world_maintenance_manager *m_maintenance_manager;
    phys_world_memory_watchdog *m_memory_watchdog;
    phys_broadphase *m_broadphase;
    phys_typed_broadphase_dispatcher *m_broadphase_dispatcher;
    phys_volume_broadphase_listener *m_volume_broadphase_listener;
    phys_entity_pair_broadphase_listener *m_entity_pair_listener;
    phys_broadphase_border_listener *m_broadphase_border_listener;
    phys_process_collision_input *m_collision_input;
    phys_collision_filter *m_collision_filter;
    phys_dispatcher *m_collision_dispatcher;
    phys_convex_list_filter *m_convex_list_filter;
    phys_world_op_queue *m_pending_ops;
    int m_pending_op_count;
    int m_critical_ops_lock_count;
    int m_critical_ops_volume_locks;
    phys_bool m_block_pending_ops;
    phys_bool m_critical_ops_allowed;
    phys_pending_op_queue_debug_info *m_pending_op_queues;
    int m_pending_op_queue_count;
    mutable phys_multi_thread_lock m_mt_lock;
    phys_bool m_single_thread_actions;
    int m_min_island_size;
    phys_critical_section *m_constraint_edit_lock;
    phys_critical_section *m_world_lock;
    phys_critical_section *m_island_dirty_lock;
    phys_critical_section *m_property_master_lock;
    phys_array<property_lock> m_property_locks;
    phys_bool m_want_islands;
    float m_convex_edge_snap_threshold;
    float m_concave_edge_snap_threshold;
    phys_bool m_toi_weld_reject;
    phys_bool m_want_sleep;
    phys_bool m_want_legacy_sleep;
    phys_bool m_wake_on_body_move;
    float m_hf_sleep_period;
    float m_low_freq_sleep_period;
    float m_sleep_ref_distance;
    float m_toi_response_rotate_normal;
    phys_enum<phys_world_desc::simulation_kind, int> m_simulation_type;

protected:
    unsigned int m_last_entity_uid;

public:
    phys_array<phys_volume *> m_border_volumes;
    phys_array<phys_action_listener *> m_action_listeners;
    phys_array<phys_entity_listener *> m_entity_listeners;
    phys_array<phys_volume_listener *> m_volume_listeners;
    phys_array<phys_constraint_listener *> m_constraint_listeners;
    phys_array<phys_world_deletion_listener *> m_world_delete_listeners;
    phys_array<phys_island_activation_listener *> m_island_activation_listeners;
    phys_array<phys_post_simulation_listener *> m_post_sim_listeners;
    phys_array<phys_post_integrate_listener *> m_post_integrate_listeners;
    phys_array<phys_post_collide_listener *> m_post_collide_listeners;
    phys_array<phys_island_post_integrate_listener *> m_island_post_integ_listeners;
    phys_array<phys_island_post_collide_listener *> m_island_post_collide_listeners;
    phys_array<phys_collision_listener *> m_collision_listeners;
    phys_broadphase_border *m_broadphase_border;
    phys_world_dynamics_step_info m_dynamics_step_info;
    phys_vec4 m_broadphase_extents[2];
    int m_broadphase_marker_count;
    int m_toi_event_queue_size;
    int m_broadphase_query_size;
    int m_broadphase_update_size;
    phys_enum<phys_world_desc::contact_point_mode, signed char> m_contact_point_gen;
};

#include "phys/phys_motor.h"

struct phys_surface_info {
    enum supported_state {
        SURFACE_UNSUPPORTED = 0,
        CONTACT_SLIDING = 1,
        SURFACE_SUPPORTED = 2
    };
    supported_state m_supported_state;
    phys_vec4 m_surface_normal;
    phys_vec4 m_surface_vel;
    float m_surface_dist_excess;
    bool m_surface_is_dynamic;
};

typedef char phys_dynamics_world_watchdog_offset[(sizeof(void *) != 4 || offsetof(phys_world, m_memory_watchdog) == 0x50) ? 1 : -1];

typedef char phys_dynamics_size_prop_val[(sizeof(phys_property_value) == 8) ? 1 : -1];
typedef char phys_dynamics_size_prop[(sizeof(phys_property) == 0x10) ? 1 : -1];
typedef char phys_dynamics_size_world_object[(sizeof(void *) != 4 || ((sizeof(phys_world_object) == 0x58) )) ? 1 : -1];
typedef char phys_dynamics_size_motion[(sizeof(void *) != 4 || ((sizeof(phys_motion) == 0x120) )) ? 1 : -1];
typedef char phys_dynamics_size_entity[(sizeof(void *) != 4 || ((sizeof(phys_entity) == 0x200) )) ? 1 : -1];
typedef char phys_dynamics_size_rigid_body[(sizeof(void *) != 4 || ((sizeof(phys_rigid_body) == 0x200) )) ? 1 : -1];

// Some native SDK units call the emitted rigid-body helpers out of line.
#ifndef PHYS_RIGID_BODY_OUT_OF_LINE
__forceinline void phys_rigid_body::set_linear_velocity(const phys_vec4 &body_new_lin_vel) {
    activate(); static_cast<phys_motion &>(m_motion).set_linear_velocity(body_new_lin_vel); }

__forceinline void phys_rigid_body::set_angular_velocity(const phys_vec4 &body_new_lin_vel) {
    activate(); static_cast<phys_motion &>(m_motion).set_angular_velocity(body_new_lin_vel); }
#endif

#endif
