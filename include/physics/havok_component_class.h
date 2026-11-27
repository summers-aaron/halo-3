#ifndef PHYSICS_HAVOK_COMPONENT_CLASS_H
#define PHYSICS_HAVOK_COMPONENT_CLASS_H

#include "physics/havok_contact_point_reference.h"
#include "phys/phys_dynamics.h"
#include "math/matrix_math_types.h"
#include "memory/data_types.h"
#include "memory/static_arrays.h"
#include "physics/impact.h"
#include "physics/havok_utilities.h"
#include <stddef.h>
#include <new>

enum e_havok_group;
enum e_rigid_body_size;
enum e_physics_model_constraint_type;
enum e_object_type;
struct phys_contact_process_event;
struct phys_contact_removed_event;
struct s_havok_component_rigid_body_state;
struct s_character_physics_definition;
struct s_physics_model_instance;
struct s_physics_model_constraint_bodies;
struct s_physics_model_node_constraint_edge_constraint;
struct s_location;
class c_character_physics_component;
class c_runtime_node_to_havok_component_rigid_body_index_mapping;
class c_runtime_node_to_physics_model_rigid_body_index_mapping;
class c_havok_contact_point;

class __declspec(align(16)) c_havok_component {
public:
    struct s_powered_chain_link_reference { short constraint_index; short link_index; };
    /* one pending rigid body of the build loop (76 bytes): handed to add_rigid_body */
    struct s_rigid_body_construction_template {
        phys_rigid_body *rigid_body;
        char physics_model_rigid_body_indices[64];
        long physics_model_rigid_body_index_count;
        bool flag_a, flag_b, flag_c;                        /* add_rigid_body's three bools */
        char pad4b;
    };
    struct s_body_in_phantom {                              /* 12 bytes */
        signed char material_index;
        signed char rigid_body_index;
        signed char physics_model_material_index;
        char pad3;
        long object_index;
        long reference_count;
    };

    class c_rigid_body {                 /* 72 bytes */
    public:
        // Raw storage: March 82b802d8 returns the buffer address as a contact point.
        struct s_contact_point_buffer { unsigned char data[68]; };
        phys_array<s_contact_point_buffer> m_contact_points;
        phys_inline_array<char, 4> m_physics_model_rigid_body_indices;
        real_point3d m_pre_simulation_position;
        real_vector3d m_pre_simulation_linear_velocity;
        real_vector3d m_pre_simulation_angular_velocity;
        void set_pre_simulation_state(const real_point3d *position, const real_vector3d *linear_velocity, const real_vector3d *angular_velocity)
        {
            m_pre_simulation_position = *position;
            m_pre_simulation_linear_velocity = *linear_velocity;
            m_pre_simulation_angular_velocity = *angular_velocity;
        }
        phys_rigid_body *m_rigid_body;
        unsigned char m_flags;                              /* bit 1: infinite inertia tensor */
        // Inplace elements start without a body.
        c_rigid_body() : m_rigid_body(0) {}
        c_rigid_body(phys_rigid_body *rigid_body, const char *physics_model_rigid_body_indices, long physics_model_rigid_body_index_count,
            bool plays_impact_effects, bool infinite_inertia_tensor, bool does_not_collide_with_environment);
        c_havok_contact_point *get_contact_point(long index) { return (c_havok_contact_point *)&m_contact_points[index]; }
        long get_contact_point_count() const { return m_contact_points.getSize(); }
        phys_rigid_body *get_rigid_body() { return m_rigid_body; }
        phys_rigid_body *get_rigid_body() const { return m_rigid_body; }
        long get_physics_model_rigid_body_index_count() const { return m_physics_model_rigid_body_indices.getSize(); }
        long get_physics_model_rigid_body_index(long index) const { return m_physics_model_rigid_body_indices[index]; }
        const real_point3d *get_pre_simulation_position() const { return &m_pre_simulation_position; }
        const real_vector3d *get_pre_simulation_linear_velocity() const { return &m_pre_simulation_linear_velocity; }
        const real_vector3d *get_pre_simulation_angular_velocity() const { return &m_pre_simulation_angular_velocity; }
        bool has_infinite_inertia_tensor() const { return (m_flags & 2) != 0; }
        bool get_does_not_collide_with_environment() const { return (m_flags & 4) != 0; }
        bool is_fixed() const { return m_rigid_body->body_is_fixed(); }
        bool is_keyframed() const { return m_rigid_body->body_is_keyframed(); }
        long add_contact_point(unsigned short contact_point_id, bool can_replace, long bsp_index, unsigned int environment_shape_key,
            long surface_index, long other_object_index,
            unsigned char flags, phys_rigid_body *rigid_body, phys_rigid_body *other_rigid_body, float friction, float restitution,
            c_global_material_type material_a, c_global_material_type material_b);
        void process_contact_point(phys_contact_process_event *event, long bsp_index, long object_index, long other_havok_component_index,
            long rigid_body_index, long other_rigid_body_index);
        bool plays_impact_effects() const { return (m_flags & 1) != 0; }
        void remove_contact_point(unsigned short contact_id, long structure_bsp_index, long other_object_index, long rigid_body_index,
            long other_rigid_body_index);
        void remove_contact_point(long contact_point_index);
        ~c_rigid_body();
    };

    // match: 0x250618: defer inplace element construction until the owner's placement-new, preserving the null guard and store batches.
    class c_rigid_body_inplace_array : public phys_array<c_rigid_body> {
    public:
        char m_storage[sizeof(c_rigid_body)];
        c_rigid_body_inplace_array() : phys_array<c_rigid_body>(phys_fixup_tag()) {}
    };

    class c_constraint {                 /* 12 bytes */
    public:
        struct s_header {                                   /* copied as one word */
            unsigned char flags;
            signed char constraint_type;                    /* e_physics_model_constraint_type */
            signed char constraint_type_index;
            char pad3;
        } m_header;
        long m_impact_datum_index;                          /* NONE when built */
        phys_constraint *m_contact_constraint;
        // March 82b7f350: flags/type/index/padding, then impact index and constraint.
        c_constraint(phys_constraint *constraint, e_physics_model_constraint_type constraint_type, long constraint_type_index, bool flag)
        {
            m_header.flags = flag != 0;
            m_header.constraint_type = (signed char)constraint_type;
            m_header.constraint_type_index = (signed char)constraint_type_index;
            m_header.pad3 = 0;
            m_impact_datum_index = -1;
            m_contact_constraint = constraint;
        }
        ~c_constraint() {}
        // March 82ba48b8: checked whole-object copy.
        c_constraint &operator=(const c_constraint &other)
        {
            if (&other != this) {
                memcpy(this, &other, sizeof(*this));
            }
            return *this;
        }
        phys_constraint *get_constraint() { return m_contact_constraint; }
        e_physics_model_constraint_type get_constraint_type() const { return (e_physics_model_constraint_type)m_header.constraint_type; }
        long get_constraint_type_index() const { return m_header.constraint_type_index; }
        bool is_powered() const { return (m_header.flags & 1) != 0; }
        long get_constraint_impact_datum_index() { return m_impact_datum_index; }
        void set_constraint_impact_datum_index(long index) { m_impact_datum_index = index; }
        void get_connected_rigid_bodies(phys_rigid_body **body_a, phys_rigid_body **body_b);
        void get_pivots(real_point3d *pivot_a, real_point3d *pivot_b);
    };

    // Written by datum_new as the element salt; the constructor leaves these bytes alone.
    s_datum_header m_header;
    unsigned long m_flags;                                  /* bit 3: connected to world */
    long m_object_index;
    signed char m_object_postion_controlling_rigid_body_index;
    signed char m_object_postion_controlling_node_index;
    signed char m_maximum_rigid_body_size;                  /* March assert 82b833d4 */
    char pad0f;
    long m_impact_array_index;
    /* True padding to 0x20: March and September constructors leave 0x14..0x20 untouched. */
    long pad14, pad18, pad1c;
    c_rigid_body_inplace_array m_rigid_bodies;
    phys_array<c_constraint> m_component_constraints;
    phys_array<s_body_in_phantom> *m_bodies_in_phantom;
    phys_simple_shape_volume *m_simple_shape_phantom;
    phys_box_volume *m_aabb_phantom;

    c_havok_component(long object_index);
    ~c_havok_component();

    s_havok_contact_point_reference notify_contact_point_added(unsigned short contact_point_id, long bsp_index,
        unsigned int environment_shape_key, long surface_index, long other_object_index,
        unsigned char flags, phys_rigid_body *rigid_body, phys_rigid_body *other_rigid_body, float friction, float restitution,
        c_global_material_type material_a, c_global_material_type material_b);
    void keyframe_powered_chain_link(s_powered_chain_link_reference reference, const real_matrix3x3 *matrix);
    const long get_object_index() const { return m_object_index; }
    long get_rigid_body_count() const { return m_rigid_bodies.getSize(); }
    long get_constraint_count() const { return m_component_constraints.getSize(); }
    long get_body_in_phantom_count() const;
    const bool records_contact_points() const { return (m_flags & 1) != 0; }
    const bool is_physics_model_havok_component() const { return ((m_flags >> 8) & 1) != 0; }
    // match: 0x109290: the owner's direct body accessor preserves calculate_impulse's vector-register colouring.
    phys_rigid_body *havok_component_get_rigid_body(long index) { return m_rigid_bodies[index].m_rigid_body; }
    phys_rigid_body *get_rigid_body(long index) { return m_rigid_bodies[index].get_rigid_body(); }
    c_havok_contact_point *get_contact_point(long rigid_body_index, long contact_point_index)
    {
        return m_rigid_bodies[rigid_body_index].get_contact_point(contact_point_index);
    }
    c_havok_contact_point *get_contact_point(const s_havok_contact_point_reference *reference)
    {
        return get_contact_point(reference->rigid_body_index, reference->contact_point_index);
    }
    void connect_to_havok_world();
    void disconnect_from_havok_world();
    void save_rigid_body_state(s_havok_component_rigid_body_state *state);
    e_rigid_body_size get_maximum_rigid_body_size() const { return (e_rigid_body_size)m_maximum_rigid_body_size; }
    friend void havok_component_construct(long havok_component_index);
private:
    void build_creature_component();
    void build_vehicle_component();
    void build_biped_component();
    e_rigid_body_size build_physics_model_component(bool a, bool b);
    void postprocess_physics_model_havok_component(s_physics_model_instance *instance);
    bool build_physics_model_rigid_body_for_node(const s_physics_model_instance *instance, long node_index,
        e_rigid_body_size *rigid_body_size, c_static_flags<255> *nodes_done,
        const char *rigid_body_motion_types, const c_runtime_node_to_physics_model_rigid_body_index_mapping *mapping,
        s_rigid_body_construction_template *construction_template);
    const phys_shape *build_physics_model_rigid_body_shape(const s_physics_model_instance *instance,
        const char *rigid_body_motion_types, char *rigid_body_indexes,
        long *rigid_body_indexes_count, const c_runtime_node_to_physics_model_rigid_body_index_mapping *mapping, long node_index,
        c_static_flags<255> *nodes_currently_in_graph_bit_vector, float *mass, phys_vec4 *center_of_mass, phys_mat3 *inertia_tensor, bool *constrained_to_world,
        e_rigid_body_size *maximum_rigid_body_size, bool *plays_impact_effects, bool *has_phantom_in_shape, bool *does_not_interact_with_environment);
    void build_powered_chain_constraints(s_physics_model_instance *instance, const c_runtime_node_to_havok_component_rigid_body_index_mapping &mapping);
    void build_constraints(s_physics_model_instance *instance, const c_runtime_node_to_havok_component_rigid_body_index_mapping &mapping);
    void build_character_component(const s_character_physics_definition *definition, c_character_physics_component *physics,
        e_havok_group group_a, e_havok_group group_b, bool a, bool b, bool c);
    void build_projectile_component();
    void add_rigid_body(phys_rigid_body *havok_rigid_body, const char *name, long physics_model_rigid_body_index, bool plays_impact_effects,
        bool infinite_inertia_tensor, bool does_not_collide_with_environment);
    void generate_rigidly_constrained_shape_list_recursive(const s_physics_model_instance *instance, const char *motion_types,
        const c_static_flags<4096> *rigid_edges,
        const c_runtime_node_to_physics_model_rigid_body_index_mapping *mapping, long first_node, long previous_node, long node_index, char *rigid_body_indices,
        long *rigid_body_count, long maximum_count, c_static_flags<255> *visited_nodes, bool *constrained_to_world);
public:
    void generate_rigid_edges_bit_vector(const s_physics_model_instance *instance, c_static_flags<4096> *rigid_physics_model_edges_bit_vector);
    void disconnect_from_impacts();
    void deactivator_update_in_local_space(long rigid_body_index, const real_vector3d *linear_velocity,
        const real_vector3d *angular_velocity, long *high_result, long *low_result);
    void get_constraint_pivots(long constraint_index, real_point3d *pivot_in_a, real_point3d *pivot_in_b);
    bool calculate_physics_bounding_box(real_rectangle3d *rectangle);
    bool calculate_local_space_physics_bounding_box(long rigid_body_index, real_rectangle3d *rectangle);
    bool calculate_physics_bounding_sphere(real_point3d *center, float *radius);
    bool is_connected_to_world() const { return ((m_flags >> 3) & 1) != 0; }
    const bool is_constrained_to_world_bit() const { return ((m_flags >> 7) & 1) != 0; }
    bool is_constraint_powered(long constraint_index) const { return m_component_constraints[constraint_index].is_powered(); }
    void scale_constraint_motor_power(long constraint_index, float scale);
    void clear_rigid_bodies();
    long calculate_rigid_body_index_from_node_index(long node_index);
    long find_rigid_body_index_from_physics_model_rigid_body_index(long physics_model_rigid_body_index)
    {
        long rigid_body_index = -1;
        for (long index = 0; index < get_rigid_body_count(); index++) {
            for (long j = 0; j < get_physics_model_rigid_body_index_count(index); j++) {
                if (get_physics_model_rigid_body_index(index, j) == physics_model_rigid_body_index) {
                    rigid_body_index = index;
                    break;
                }
            }
        }
        return rigid_body_index;
    }
private:
    void update_rigid_body_pre_simulation_state(long rigid_body_index);
public:
    void pre_simulation_update();
    bool calculate_impulse(long rigid_body_index, long other_object_index, long other_rigid_body_index, float *impulse,
        real_point3d *position, real_vector3d *normal);
    void body_slam_object_node_matrices(c_static_flags<255> *simulation_controlled_nodes, float z_offset);
    void body_slam_unsimulated_nodes_recursive(long node_index, c_static_flags<255> *simulation_controlled_nodes,
        const c_static_flags<255> *desired_simulation_controlled_nodes, real_orientation *node_orientations, long node_orientations_count);
    bool make_space_for_new_impact(float score, long rigid_body_index);
    long get_impact_count() const;
    long find_impact_which_includes_contact(const c_impact::s_contact_description *contact_description, bool test_space);
    void get_rigid_body_position(long rigid_body_index, real_point3d *position)
    {
        const float *p = (const float *)&get_rigid_body(rigid_body_index)->get_contact_position();
        position->x = p[0];
        position->y = p[1];
        position->z = p[2];
    }
    void move_aabb_phantom(const real_matrix4x3 *matrix, const real_rectangle3d *bounds);
    void notify_entity_entered_phantom_material(long havok_component_index, long physics_model_material_index, const phys_world_object *phantom,
        const phys_world_object *entity, long material_index);
    void update_phantoms_which_intersect_shape_phantom();
    static bool update_phantom(long, bool, const s_body_in_phantom &, bool, float &, bool &, bool &, bool &);
    void notify_entity_left_phantom_material(long havok_component_index, long physics_model_material_index, const phys_world_object *phantom,
        const phys_world_object *entity, long material_index);
    phys_rigid_body *construct_proxy_rigid_body(long rigid_body_index);
    void add_constraint(phys_constraint *constraint, long constraint_type, long constraint_type_index, bool flag);
    void create_constraint(const s_physics_model_instance *physics_model_instance,
        const c_runtime_node_to_havok_component_rigid_body_index_mapping *runtime_node_to_havok_component_rigid_body_index_mapping,
        const struct s_physics_model_node_constraint_edge_constraint *physics_model_node_constraint_edge_constraint, const real_matrix4x3 *matrix, bool force);
    void align_constraint_positions();
private:
    void align_constraint_positions_recursive(long rigid_body_index, c_static_flags<64> *aligned_rigid_bodies);
public:
    void set_rigid_body_transform_internal(long rigid_body_index, const phys_transform *transform);
    void restore_rigid_body_state(const s_havok_component_rigid_body_state *state);
    long get_physics_model_rigid_body_index_count(long rigid_body_index) const
    {
        return m_rigid_bodies[rigid_body_index].get_physics_model_rigid_body_index_count();
    }
    long get_physics_model_rigid_body_index(long rigid_body_index, long index) const
    {
        return m_rigid_bodies[rigid_body_index].get_physics_model_rigid_body_index(index);
    }
    bool get_rigid_body_is_dynamic(long rigid_body_index) const;
    // match: 0x578140: compare the SDK motion value directly, preserving the owner inline body in vehicle physics.
    bool get_rigid_body_is_fixed(long rigid_body_index) const
    {
        return m_rigid_bodies[rigid_body_index].m_rigid_body->get_motion_type() == phys_motion::MOTION_FIXED;
    }
    // match: 0x25a5e8: compare the SDK motion value directly; hkEnum equality introduces an hkBool temporary and moves utility address operands.
    bool get_rigid_body_is_keyframed(long rigid_body_index) const
    {
        return m_rigid_bodies[rigid_body_index].m_rigid_body->get_motion_type() == phys_motion::MOTION_KEYFRAMED;
    }
    void get_rigid_body_linear_velocity(long rigid_body_index, real_vector3d *velocity);
    void set_rigid_body_linear_velocity(long rigid_body_index, const real_vector3d *velocity);
    void get_rigid_body_angular_velocity(long rigid_body_index, real_vector3d *velocity);
    void set_rigid_body_angular_velocity(long rigid_body_index, const real_vector3d *velocity);
    void rigid_body_apply_acceleration(long rigid_body_index, const real_vector3d *acceleration);
    void rigid_body_apply_acceleration_at_point(long rigid_body_index, const real_point3d *point, const real_vector3d *acceleration);
    // match: 0xf1308: retain the owner inline flag read; the nested reference accessor moves vehicle array address operands.
    bool get_rigid_body_has_infinite_inertia_tensor(long rigid_body_index) const { return (m_rigid_bodies[rigid_body_index].m_flags & 2) != 0; }
    float get_rigid_body_mass(long rigid_body_index);
    s_powered_chain_link_reference find_powered_chain_reference_connecting_nodes(long node_a, long node_b);
    const short get_object_postion_controlling_rigid_body_index() const;
    const short get_object_postion_controlling_node_index() const { return m_object_postion_controlling_node_index; }
    const bool is_one_body_object_space_component() const { return ((m_flags >> 6) & 1) != 0; }
    void calculate_object_transform(real_matrix4x3 *transform);
    bool rigid_body_calculate_fixed_position(long rigid_body_index, e_havok_group havok_group, const real_point3d *position,
        const real_vector3d *velocity, real_point3d *fixed_position,
        struct s_location *location, float minimum_search_radius, float maximum_search_radius, bool search_everywhere,
        const real_point3d *raycast_origin, long raycast_object_index);
    bool rigid_body_calculate_fixed_position_along_z(long rigid_body_index, e_havok_group havok_group, const phys_shape *shape, float step,
        long iterations, long *ending_iteration_index);
    void calculate_object_transform(real_matrix4x3 *transform, const real_matrix4x3 *matrix);
    void get_rigid_body_transform(long rigid_body_index, real_matrix4x3 *transform);
private:
    void calculate_transform_between_nodes_on_rigid_body(long rigid_body_index, long node_index_a, long node_index_b, real_matrix4x3 *matrix);
    void calculate_constraint_transforms(const s_physics_model_instance *physics_model_instance,
        const c_runtime_node_to_havok_component_rigid_body_index_mapping *runtime_node_to_havok_component_rigid_body_index_mapping,
        phys_transform *hk_transform, phys_transform *hk_transform2, phys_rigid_body **rigid_body_a, phys_rigid_body **rigid_body_b,
        const s_physics_model_constraint_bodies *physics_model_constraint_bodies);
public:
    void get_rigid_body_center_of_mass(long rigid_body_index, real_point3d *center_of_mass);
    void get_rigid_body_linear_velocity_at_point(long rigid_body_index, real_vector3d *velocity, const real_point3d *point);
public:
    c_constraint *get_constraint(long index) { return &m_component_constraints[index]; }
    s_physics_model_constraint_reference get_physics_model_constraint_reference(long index) {
        c_constraint *constraint = &m_component_constraints[index];
        s_physics_model_constraint_reference result;
        result.type = (short)constraint->get_constraint_type();
        result.index = (short)constraint->get_constraint_type_index();
        return result;
    }
    long get_constraint_impact_datum_index(long index) { return m_component_constraints[index].get_constraint_impact_datum_index(); }
    void set_constraint_impact_datum_index(long index, long value) { m_component_constraints[index].set_constraint_impact_datum_index(value); }
    const real_vector3d *get_rigid_body_pre_simulation_linear_velocity(long index) {
        c_rigid_body &rigid_body = m_rigid_bodies[index];
        return rigid_body.get_pre_simulation_linear_velocity();
    }
    const real_vector3d *get_rigid_body_pre_simulation_angular_velocity(long index) {
        c_rigid_body &rigid_body = m_rigid_bodies[index];
        return rigid_body.get_pre_simulation_angular_velocity();
    }
    const real_point3d *get_rigid_body_pre_simulation_position(long index) {
        c_rigid_body &rigid_body = m_rigid_bodies[index];
        return rigid_body.get_pre_simulation_position();
    }
    long get_contact_point_count(long index) const { return m_rigid_bodies[index].get_contact_point_count(); }
    bool rigid_body_plays_impact_effects(long index) const {
        const c_rigid_body &rigid_body = m_rigid_bodies[index];
        return rigid_body.plays_impact_effects();
    }
    bool get_rigid_body_does_not_collide_with_environment(long index) const { return m_rigid_bodies[index].get_does_not_collide_with_environment(); }
    void DEBUG_validate(bool);
    void move_simple_shape_phantom(const real_matrix4x3 *);
    void render_debug(bool, bool, bool, bool, bool, bool);
    void get_rigid_body_orientation(long, real_orientation *);
    const phys_collider *get_rigid_body_collidable(long);
    // March 82b834c8: one hkTransform local, converted before the internal setter.
    void set_rigid_body_transform(long index, const real_matrix4x3 *matrix) {
        phys_transform transform;
        phys_transform_from_matrix4x3(&transform, matrix);
        set_rigid_body_transform_internal(index, &transform);
    }
    void rigid_body_apply_hard_keyframe(long, const real_matrix4x3 *);
    void rigid_body_apply_angular_acceleration(long, const real_vector3d *);
    float rigid_body_calculate_radius(long);
    bool is_active();
    bool is_active(long);
    void force_activate();
    bool has_dynamic_rigid_bodies();
    void update_phantom_physics_version();
    void update_phantoms(bool);
    void scale_constraint_limit_tau(long, float);
    void keyframe_ragdoll_constraint(long, const real_matrix3x3 *, bool);
    void keyframe_limited_hinge_constraint(long, float);
    void get_bodies_in_phantom_info(long, long *, long *, long *);
    long calculate_heap_memory_usage(long *, long *, long *, long *);
    void align_rigid_bodies_to_object_tranform(long, bool);
    bool get_rigid_body_exceeded_broadbase(long) const;
    bool has_dynamic_object_position_controlling_rigid_body();
    bool has_rigid_body_with_infinite_inertia_tensor() const;
    bool rigid_body_is_shape_current_shape(long, const phys_shape *);
    c_global_material_type get_rigid_body_material_type(long);
    const bool has_phantom_shape_in_at_least_one_rigid_body() const;
    const bool ignores_gravity() const;
    const bool is_sentinal() const;
    const bool using_rigid_body_no_phantom_power_version() const;
    const bool was_constructed_active() const;
    phys_box_volume *get_aabb_phantom();
    phys_rigid_body *DEBUG_get_rigid_body(long);
    phys_simple_shape_volume *get_simple_shape_phantom();
    long find_constraint(s_physics_model_constraint_reference);
    long find_constraint_connecting_nodes(long, long, long);
    long get_bodies_in_phantoms_count();
    long get_impact_datum_index(long) const;
    unsigned char *get_stretched_counter(long);
    void add_impact(long);
    void destroy_constraint(long);
    void get_rigid_body_inertia_tensor(long, real_matrix3x3 *);
    void get_rigid_body_inverse_inertia_tensor(long, real_matrix3x3 *);
    void get_rigid_body_transform_direct(long, real_matrix4x3 *);
    void notify_matrices_clean();
    void remove_impact(long);
    void set_rigid_body_exceeded_broadbase(long, bool);
    void wake_all_bodies_in_phantoms();
    void get_constraint_rigid_bodies(long, long *, long *);
    void notify_contact_point_removed(phys_contact_removed_event *, phys_rigid_body *, phys_rigid_body *);
    void notify_contact_point_process(phys_contact_process_event *, long, long, long, long, long);
private:
    void build_crate_component();
    void build_terminal_component();
    e_havok_group calculate_physics_model_rigid_body_havok_component_group(const s_physics_model_instance *, e_rigid_body_size, phys_motion::motion_kind, long);
    void build_giant_component();
    void build_machine_component();
    void build_item_component();
    phys_rigid_body *build_rigid_body(const phys_shape *, bool, bool, const phys_vec4 *, float, float, const phys_mat3 *, float, long,
        const phys_transform *, long, bool, bool, float, float);
    void calculate_quality_type_and_maximum_penetration_depth(phys_collider_quality *, float *, phys_motion::motion_kind, float, e_object_type);
    long rigid_body_motion_type_to_havok_motion_type(long);
};

extern const c_havok_component::s_powered_chain_link_reference k_invalid_powered_chain_link_reference;

typedef char havok_component_rigid_body_size[(sizeof(void *) != 4 || ((sizeof(c_havok_component::c_rigid_body) == 72) )) ? 1 : -1];
typedef char havok_component_constraint_size[(sizeof(void *) != 4 || ((sizeof(c_havok_component::c_constraint) == 12) )) ? 1 : -1];
typedef char havok_component_size[(sizeof(void *) != 4 || ((sizeof(c_havok_component) == 144) )) ? 1 : -1];
typedef char havok_component_bodies_offset[(sizeof(void *) != 4 || offsetof(c_havok_component, m_rigid_bodies) == 0x20) ? 1 : -1];
typedef char havok_component_constraints_offset[(sizeof(void *) != 4 || offsetof(c_havok_component, m_component_constraints) == 0x74) ? 1 : -1];
typedef char havok_component_phantoms_offset[(sizeof(void *) != 4 || offsetof(c_havok_component, m_bodies_in_phantom) == 0x80) ? 1 : -1];

#endif
